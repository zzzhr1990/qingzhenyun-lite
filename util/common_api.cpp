//
// Created by zzzhr on 2018/8/23.
//

#include "common_api.h"
#include "../model/user_model.h"

using namespace pplx;

CommonApi &CommonApi::instance() {
    static CommonApi c;
    return c;
}

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

task<ResponseEntity> CommonApi::post_data(const utility::string_t& uri,const web::json::value & data) {
    using namespace web::http;
    http_request request(methods::POST);
    uri_builder login_uri(uri);
    request.set_request_uri(login_uri.to_string());
    auto & headers = request.headers();
    headers.add(header_names::accept, U("application/json"));
    headers.add(header_names::content_type, U("application/json"));
    auto &token = UserModel::instance().GetToken();
	if (!token.empty()) {
        headers.add(U("Token"),token);
    }
	request.set_body(data);

	//std::string abc = "abc";
	// std::string & bbc = abc;


	const task<ResponseEntity> resp = raw_client.request(request).then([](pplx::task<http_response> response_task){
		// if there is any task, cancel it.
		try
		{
			auto json_response = response_task.get();
			auto json = json_response.extract_json();
			auto v = json.get();
			if (v.has_field(U("token"))) {
                utility::string_t new_token = v.at(U("token")).as_string();
				UserModel::instance().UpdateToken(new_token);
			}
			auto success = v[U("success")].as_bool();
			ResponseEntity response;
			response.success = success;
			if (v.has_field(U("message"))) {
				response.message = v.at(U("message")).as_string();
			}
            if (v.has_field(U("code"))) {
                response.code = v.at(U("code")).as_string();
            }
			if (v.has_field(U("result"))) {
				response.result = v.at(U("result"));
			}
            if (v.has_field(U("status"))) {
                response.status = v.at(U("status")).as_integer();
            }
			return create_task([response]() {
				return response;
			});
		}
		catch (const std::exception& e)
		{
			ResponseEntity response;
			response.success = false;
			response.status = -1;
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