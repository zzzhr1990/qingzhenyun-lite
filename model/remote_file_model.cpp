//
// Created by zzzhr on 2018/8/27.
//

#include "remote_file_model.h"
RemoteFileModel& RemoteFileModel::instance() {
    static RemoteFileModel c;
    return c;
}

void RemoteFileModel::GetPage(wxWindow* handler, const utility::string_t &path,const int &page, const int &pageSize, const int &type) {
    web::json::value request;
    request[U("path")] = web::json::value::string(path.empty() ? current_path : path);
    request[U("page")] = web::json::value::number(page > 0 ? page : current_page);
    request[U("pageSize")] = web::json::value::number(pageSize > 0 ? pageSize : current_page_size);
    if(type > -1){
        request[U("type")] = web::json::value::number(type);
    }

    CommonApi::instance().post_data(U("/v1/files/page"),request).then([handler](ResponseEntity v){
        SendCommonThreadEvent(handler,USER_REMOTE_FILE_PAGE_DATA,v, true);
    });

}

void RemoteFileModel::GetPageById(wxWindow* handler, const utility::string_t &uuid,const int &page, const int &pageSize, const int &type) {
    web::json::value request;
    request[U("uuid")] = web::json::value::string(uuid);
    request[U("page")] = web::json::value::number(page > 0 ? page : current_page);
    request[U("pageSize")] = web::json::value::number(pageSize > 0 ? pageSize : current_page_size);
    if(type > -1){
        request[U("type")] = web::json::value::number(type);
    }

    CommonApi::instance().post_data(U("/v1/files/page"),request).then([handler](ResponseEntity v){
        SendCommonThreadEvent(handler,USER_REMOTE_FILE_PAGE_DATA,v, true);
    });

}

utility::string_t RemoteFileModel::GetCurrentPath() {
    return current_path;
}

void
RemoteFileModel::UpdateCurrentLocation(const utility::string_t &path, const utility::string_t &fileId, const int &page,
                                       const int &pageSize, const int &totalPage, const utility::string_t &parent) {
    this->current_path = path;
    this->current_file_id = fileId;
    this->current_page = page;
    this->current_page_size = pageSize;
    this->total_page = totalPage;
    this->current_parent = parent;
}


