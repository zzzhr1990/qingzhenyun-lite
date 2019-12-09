//
// Created by herui on 2019/11/28.
//

#include <qingzhen/api/user.h>

qingzhen::api::user::user() :
        identity(int64_t(0)),
        name(utility::string_t()),
        icon(utility::string_t()),
        space_used(int64_t(0)),
        space_capacity(0) {}

bool qingzhen::api::user::parse(const web::json::value &v) {

    if (!v.has_number_field(_XPLATSTR("identity"))) {
        return false;
    }
    this->identity = v.at(_XPLATSTR("identity")).as_number().to_int64();
    if (!v.has_string_field(_XPLATSTR("name"))) {
        return false;
    }
    this->name = v.at(_XPLATSTR("name")).as_string();
    if (v.has_string_field(_XPLATSTR("icon"))) {
        this->icon = v.at(_XPLATSTR("icon")).as_string();
    }
    if (v.has_number_field(_XPLATSTR("spaceUsed"))) {
        this->space_used = v.at(_XPLATSTR("spaceUsed")).as_number().to_int64();
    }
    if (v.has_number_field(_XPLATSTR("spaceCapacity"))) {
        this->space_capacity = v.at(_XPLATSTR("spaceCapacity")).as_number().to_int64();
    }
    return true;
}

web::json::value qingzhen::api::user::to_json() const {
    //auto res = web::json::value();
    // res.
    web::json::value res;
    res[_XPLATSTR("name")] = web::json::value(this->name());
    return res;
}

