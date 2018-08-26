#ifndef FUCK_ResponseEntity_H
#define FUCK_ResponseEntity_H
#include <cpprest/http_client.h>

struct ResponseEntity
{
	/*
	ResponseEntity(const bool &success, const web::json::value & result, const int &code) {
	this->success = success;
	this->result = result;
	this->code = code;
	};
	*/

	bool success;
	web::json::value  result;
	int code;
	utility::string_t message;
};

#endif