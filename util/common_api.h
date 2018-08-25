//
// Created by zzzhr on 2018/8/23.
//

#ifndef FUCK_COMMON_API_H
#define FUCK_COMMON_API_H

#include <cpprest/http_client.h>

class CommonApi {
public:
    static CommonApi& instance();
    /*
    ~CommonApi(){
        if(raw_client != nullptr){
            delete raw_client;
            raw_client = nullptr;
        }
    }
     */
    pplx::task<web::json::value> post_data(const std::string & uri,const web::json::value & data, const bool & return_result = false);
private:

    // CommonApi();
    web::http::client::http_client_config _cfg;
    CommonApi():
    raw_client(_XPLATSTR("https://api.6pan.cn"),_cfg)
    {
        _cfg.set_timeout(std::chrono::seconds(10));
    }

    web::http::client::http_client raw_client;
};

#endif //FUCK_COMMON_API_H
