//
// Created by herui on 2019/11/28.
//

#ifndef QINGZHEN_USER_H
#define QINGZHEN_USER_H

#include "qingzhen/api/models.hpp"
#include "cpprest/details/basic_types.h"

namespace qingzhen::api {
    class user : public standard_json_response {
    public:
        user();

        qingzhen::property<int64_t> identity;
        qingzhen::property<utility::string_t> name;
        qingzhen::property<utility::string_t> icon;
        qingzhen::property<int64_t> space_used;
        qingzhen::property<int64_t> space_capacity;

        bool parse(const web::json::value &v) override;

        [[nodiscard]] web::json::value to_json() const override;
    };
}
#endif //QINGZHEN_USER_H
