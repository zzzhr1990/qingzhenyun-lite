//
// Created by zzzhr on 2018/8/23.
//

#include "common_api.h"

using namespace pplx;

CommonApi &CommonApi::instance() {
    static CommonApi c;
    return c;
}


task<ResponseEntity> CommonApi::post_data(const std::string& uri,const web::json::value & data) {
	
	/*
	pplx::cancellation_token_source cts;
	pplx::task<ResponseEntity> task = pplx::task<ResponseEntity>([cts,task]()
	{

	while (!cts.get_token().is_canceled())
	{
	std::cout << "task is running" << std::endl;
	pplx::wait(90);
	}

	std::cout << "task cancelled" << std::endl;

	task.get()
	});
	*/
	

    //return web::json::object();
    using namespace web::http;
    http_request request(methods::POST);
    uri_builder login_uri(utility::conversions::to_string_t(uri));
    request.set_request_uri(login_uri.to_string());
    request.headers().add(header_names::accept, U("application/json"));
    request.headers().add(header_names::content_type, U("application/json"));
    request.set_body(data);
    auto resp = raw_client.request(request).then([](pplx::task<http_response> response_task){
		// if there is any exc
		try
		{
			auto json_response = response_task.get();
			auto json = json_response.extract_json();
			auto v = json.get();
			if (v.has_field(U("token"))) {
				auto token = v.at(U("token")).as_string();
				UserModel::instance().UpdateToken(token);
			}
			auto success = v[U("success")].as_bool();
			
			// auto res = v[U("result")];
			// auto token = v.get(U("token"));
			ResponseEntity response;
			response.success = success;
			if (v.has_field(U("message"))) {
				response.message = v.at(U("message")).as_string();
			}
			if (v.has_field(U("result"))) {
				response.result = v.at(U("result"));
			}
			return create_task([response]() {
				return response;
			});
		}
		catch (const std::exception& e)
		{
			std::cout << "exception " << e.what() << std::endl;
			ResponseEntity response;
			response.success = false;
			response.result = web::json::value();
			response.message = utility::conversions::to_string_t(e.what());
			return create_task([response]() {
				return response;
			});
		}

	}, task_continuation_context::use_default());
        
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