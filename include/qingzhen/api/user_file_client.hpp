//
// Created by herui on 2019/11/30.
//

#ifndef QINGZHEN_USER_FILE_CLIENT_H

#include "qingzhen/api/file.h"
#include "qingzhen/api/file_list.h"
#include <qingzhen/api/file_requests.h>
#include "qingzhen/api/common_api_client.hpp"

namespace qingzhen::api {
    class user_file_client {
    public:
        user_file_client &operator=(const user_file_client &) = delete;

        user_file_client(const user_file_client &) = delete;

        user_file_client(const user_file_client &&) = delete;

        [[nodiscard]] static pplx::task<standard_api_response<file>>
        get_file_info(const file &file_request, const pplx::cancellation_token &cancellation_token) {
            // get file
            return pplx::create_task([file_request, cancellation_token] {
                auto &client = common_api_client::instance();
                return client.sync_send_data<file>(web::http::methods::POST, _XPLATSTR("/v3/file/get/"),
                                                   file_request.to_json(), cancellation_token);
            });
        }

        [[nodiscard]] static pplx::task<standard_api_response<file_list>>
        list_directory(const file_list_request &file_request, const pplx::cancellation_token &cancellation_token) {
            // get file
            return pplx::create_task([file_request, cancellation_token] {
                auto &client = common_api_client::instance();
                return client.sync_send_data<file_list>(web::http::methods::POST, _XPLATSTR("/v3/files/list/"),
                                                        file_request.to_json(), cancellation_token);
            });
        }

        [[nodiscard]] static pplx::task<standard_api_response<file>>
        get_download_info(const file &file_request, const pplx::cancellation_token &cancellation_token) {
            // get file
            return pplx::create_task([file_request, cancellation_token] {
                auto &client = common_api_client::instance();
                return client.sync_send_data<file>(web::http::methods::POST, _XPLATSTR("/v3/file/download"),
                                                   file_request.to_json(), cancellation_token);
            });
        }

    private:
        user_file_client() = default;
    };

}
#define QINGZHEN_USER_FILE_CLIENT_H

#endif //QINGZHEN_USER_FILE_CLIENT_H
