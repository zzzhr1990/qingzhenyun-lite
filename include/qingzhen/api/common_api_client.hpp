//
// Created by herui on 2019/11/28.
//

#ifndef QINGZHEN_COMMON_API_CLIENT_HPP
#define QINGZHEN_COMMON_API_CLIENT_HPP

#include <cpprest/http_client.h>
#include "qingzhen/api/models.hpp"
#include <mutex>


namespace qingzhen::api {
    class common_api_client {
    public:
        static common_api_client &instance() {
            static common_api_client instance;
            return instance;
        }

        common_api_client &operator=(const common_api_client &rhs) = delete;

        common_api_client(const common_api_client &) = delete;

        common_api_client(const common_api_client &&) = delete;

        void set_current_user_token(const utility::string_t &v) {
            std::lock_guard<std::mutex> _(this->mutex);
            this->user_token = v;
        }

        /* Sync send */
        template<typename K_STD_VAL, typename = std::enable_if<std::is_base_of<qingzhen::api::standard_json_response, qingzhen::api::standard_json_response>::value>>
        standard_api_response <K_STD_VAL>
        sync_send_data(const web::http::method &mtd, const utility::string_t &path, const web::json::value &data,
                       const pplx::cancellation_token &cancellation_token) noexcept {
            standard_api_response<K_STD_VAL> cst;
            try {
                web::http::http_request request(mtd);
                web::http::uri_builder uri(path);
                request.set_request_uri(uri.to_string());
                auto &headers = request.headers();
                headers.add(web::http::header_names::accept, _XPLATSTR("application/json"));
                headers.add(web::http::header_names::content_type, _XPLATSTR("application/json"));
                headers.add(web::http::header_names::accept_encoding, _XPLATSTR("gzip"));
                const auto &token = this->get_current_user_token();

                if (!token.empty()) {
                    headers.add(_XPLATSTR("Authorization"), _XPLATSTR("Bearer ") + token);
                }
                if (mtd != web::http::methods::GET && mtd != web::http::methods::HEAD) {
                    request.set_body(data);
                }
                auto resp = this->raw_client.request(request, cancellation_token).get();
                const auto &code = resp.status_code();
                if (code < web::http::status_codes::OK || code >= web::http::status_codes::BadRequest) {
                    // error
                    cst.success = false;
                    cst.cancel = false;
                    cst.error_message = _XPLATSTR("INTERNAL_ERROR");
                    cst.error_ref = _XPLATSTR("INTERNAL_ERROR");

                    try {
                        const auto &dataErr = resp.extract_json().get();
                        if (dataErr.has_string_field(_XPLATSTR("reference"))) {
                            cst.error_ref = dataErr.at(_XPLATSTR("reference")).as_string();
                        }
                        // const auto& dataErr = resp.extract_json().get();
                        if (dataErr.has_string_field(_XPLATSTR("message"))) {
                            cst.error_message = dataErr.at(_XPLATSTR("message")).as_string();
                        }
                    }
                    catch (const std::exception &) {
                        // JSON error
                    }

                    return cst;
                }

                // std::errc::
                const auto &json = resp.extract_json().get();
                // parse
                bool succ = cst.data()->parse(json);
                if (!succ) {
                    cst.success = false;
                    cst.cancel = false;
                    cst.error_message = _XPLATSTR("PARSE_RESULT_ERROR");
                    cst.error_message = _XPLATSTR("PARSE_RESULT_ERROR");
                    return cst;
                }
                cst.success = true;
                cst.cancel = false;

            } catch (const pplx::task_canceled &) {
                cst.success = false;
                cst.cancel = true;
            } catch (const web::http::http_exception &hex) {
                auto e_code = hex.error_code().value();
                if (e_code == ECANCELED) {
                    cst.success = false;
                    cst.cancel = true;
                } else {
                    cst.success = false;
                    cst.cancel = false;
                    cst.error_ref = _XPLATSTR("NETWORK_ERROR");
                    utility::conversions::to_string_t(utility::conversions::utf8_to_utf16(hex.what()));

                }

            } catch (const std::exception &une) {
                cst.success = false;
                cst.cancel = false;
                if (cancellation_token.is_canceled()) {
                    cst.cancel = true;
                }
                cst.error_ref = _XPLATSTR("LOCAL_EXCEPTION");
                cst.error_message = utility::conversions::to_string_t(utility::conversions::utf8_to_utf16(une.what()));
            }
            return cst;
        }

    private:
        common_api_client() : raw_client(_XPLATSTR("https://api.6pan.cn")) {
            web::http::client::http_client_config cfg;
            cfg.set_timeout(std::chrono::seconds(30));
            cfg.set_request_compressed_response(true);
            cfg.set_validate_certificates(false);
            this->raw_client = web::http::client::http_client(_XPLATSTR("https://api.6pan.cn"), cfg);
        }

        web::http::client::http_client raw_client;
        std::mutex mutex;

        utility::string_t get_current_user_token() {
            std::lock_guard<std::mutex> _(this->mutex);
            return this->user_token;
        }

        utility::string_t user_token;
    };
}

#endif //QINGZHEN_COMMON_API_CLIENT_HPP
