//
// Created by herui on 2019/12/11.
//

#include <qingzhen/transfer/transfer_entity.h>

using namespace qingzhen::transfer;

qingzhen::transfer::transfer_result::transfer_result() : success(false), cancelled(false), hash(utility::string_t()),
                                                         error_message(utility::string_t()),
                                                         error_reference(utility::string_t()),
                                                         sha1(utility::string_t()), md5(utility::string_t()) {

}

std::unique_ptr<transfer_result> qingzhen::transfer::transfer_result::create() {
    return std::unique_ptr<transfer_result>(new transfer_result());
}

void transfer_result::cancel() {
    this->cancelled = true;
    this->success = false;
    this->error_message = _XPLATSTR("Cancelled");
    this->error_reference = _XPLATSTR("CANCELLED");
}

void transfer_result::error(const utility::string_t &err_ref, const utility::string_t &err_message) {
    this->cancelled = false;
    this->success = false;
    this->error_reference = err_ref;
    this->error_message = err_message;
}

void transfer_result::hash_failed() {
    this->error(_XPLATSTR("HASH_FAILED"), _XPLATSTR("Hash failed"));
}
