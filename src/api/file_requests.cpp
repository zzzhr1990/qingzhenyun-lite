//
// Created by herui on 2019/12/10.
//

#include <qingzhen/api/file_requests.h>

using namespace qingzhen::api;

web::json::value file_list_request::to_json() const {
    web::json::value v;
    v[_XPLATSTR("start")] = web::json::value(this->start());
    v[_XPLATSTR("limit")] = web::json::value(this->limit());
    v[_XPLATSTR("parentIdentity")] = web::json::value(this->parent_entity->identity());
    v[_XPLATSTR("parentPath")] = web::json::value(this->parent_entity->path());
    if (this->directory() != 0) {
        v[_XPLATSTR("directory")] = web::json::value(this->directory());
    }
    return v;
}


file_list_request::file_list_request(file parent_file_data) : parent_entity(std::make_shared<file>(parent_file_data)),
                                                              directory(0),
                                                              start(-1), limit(-1) {
}
