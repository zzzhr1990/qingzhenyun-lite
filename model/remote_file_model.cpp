//
// Created by zzzhr on 2018/8/27.
//

#include "remote_file_model.h"
RemoteFileModel& RemoteFileModel::instance() {
    static RemoteFileModel c;
    return c;
}

pplx::task<ResponseEntity> RemoteFileModel::page(const utility::string_t &path,const int &page, const int &pageSize, const int &type) {
    web::json::value request;
    request[U("path")] = web::json::value::string(path);
    request[U("page")] = web::json::value::number(page);
    request[U("pageSize")] = web::json::value::number(pageSize);
    request[U("type")] = web::json::value::number(pageSize);
    return CommonApi::instance().post_data(U("/v1/files/page"),request);
}
