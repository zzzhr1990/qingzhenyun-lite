//
// Created by herui on 2019/11/30.
//

#ifndef QINGZHEN_USER_API_CLIENT_HPP
#define QINGZHEN_USER_API_CLIENT_HPP

#include "qingzhen/api/user.h"
#include "qingzhen/api/common_api_client.hpp"

namespace qingzhen::api {
    class user_api_client {
    public:
        user_api_client &operator=(const user_api_client &rhs) = delete;

        user_api_client(const user_api_client &) = delete;

        user_api_client(const user_api_client &&) = delete;

        [[nodiscard]] static pplx::task<standard_api_response<user>>
        login(const utility::string_t &new_user_token, const pplx::cancellation_token &cancellation_token) {
            return pplx::create_task([new_user_token, cancellation_token] {
                auto &client = common_api_client::instance();
                client.set_current_user_token(new_user_token);
                auto se = client.sync_send_data<user>(web::http::methods::GET, _XPLATSTR("/v3/user/"),
                                                      web::json::value(), cancellation_token);
                return se;
            });
        }

    private:
        user_api_client() = default;
    };
}
#endif //QINGZHEN_USER_API_CLIENT_HPP
