//
// Created by zzzhr on 2018/8/23.
//

#include "common_api.h"

using namespace pplx;

CommonApi &CommonApi::instance() {
    static CommonApi c;
    return c;
}


task<web::json::value> CommonApi::post_data(const std::string& uri,const web::json::value & data,const bool &return_result) {
    //return web::json::object();
    using namespace web::http;
    http_request request(methods::POST);
    uri_builder login_uri(utility::conversions::to_string_t(uri));
    request.set_request_uri(login_uri.to_string());
    request.headers().add(header_names::accept, U("application/json"));
    request.headers().add(header_names::content_type, U("application/json"));
    request.set_body(data);
    auto resp = raw_client.request(request).then([](http_response response){
        return response.extract_json();
    }).then([return_result](web::json::value v){
        if(!return_result){
            return create_task([v](){ return v;});
        }

        auto success = v[U("success")].as_bool();
        if(!success){
            return create_task([](){
                return web::json::value();
            });
        }
        auto res = v[U("result")];
        return create_task([res](){ return res;});
    },task_continuation_context::use_default());
    return resp;
}

/*
CommonApi::CommonApi() {
    auto const API_PREFIX = _XPLATSTR("https://api.6pan.cn");
    web::http::client::http_client_config _cfg;
    _cfg.set_timeout(std::chrono::seconds(10));
    // web::http::client::http_client _raw_client(API_PREFIX, _cfg);
    this->raw_client = new web::http::client::http_client(API_PREFIX, _cfg);
    // delete raw_client;
    // this->raw_client = new web::http::client::http_client(API_PREFIX, _cfg);
}

 */