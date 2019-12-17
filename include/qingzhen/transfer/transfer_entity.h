//
// Created by herui on 2019/12/2.
//

#ifndef QINGZHEN_TRANSFER_ENTITY_H
#define QINGZHEN_TRANSFER_ENTITY_H

#include <cpprest/details/basic_types.h>
#include <qingzhen/properity.hpp>
namespace qingzhen::transfer {
    enum class transfer_direction {
        download = 0, upload = 1, sync = 2
    };

    enum class transfer_status {
        add = 0,
        pause = 1,
        error = 2,
        pre_checking = 3,
        transfer = 4,
        after_checking = 5,
        fatal_error = 9,
        complete = 10
    };

    class transfer_result {
    public:
        static std::unique_ptr<transfer_result> create();

        void cancel();

        void error(const utility::string_t &err_ref, const utility::string_t &err_message);

        void hash_failed();

        property<bool> success;
        property<bool> cancelled;
        property<utility::string_t> error_message;
        property<utility::string_t> error_reference;
        property<utility::string_t> md5;
        property<utility::string_t> sha1;
        property<utility::string_t> hash;
    private:
        transfer_result();
    };

}
#endif //QINGZHEN_TRANSFER_ENTITY_H
