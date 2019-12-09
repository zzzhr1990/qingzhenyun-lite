//
// Created by herui on 2019/11/30.
//

#ifndef QINGZHEN_FILE_H
#define QINGZHEN_FILE_H

#include "qingzhen/api/models.hpp"
#include "cpprest/details/basic_types.h"

namespace qingzhen::api {
    class file : public standard_json_response {
    public:
        file();

        bool parse(const web::json::value &v) override;

        qingzhen::property<utility::string_t> identity;
        qingzhen::property<utility::string_t> hash;
        qingzhen::property<int64_t> user_identity;
        qingzhen::property<utility::string_t> path;
        qingzhen::property<utility::string_t> name;
        qingzhen::property<utility::string_t> ext;
        qingzhen::property<int64_t> size;
        qingzhen::property<utility::string_t> human_read_size;
        qingzhen::property<utility::string_t> mime;
        qingzhen::property<bool> deleted;
        qingzhen::property<bool> root;
        qingzhen::property<utility::string_t> parent;
        qingzhen::property<int> type;
        qingzhen::property<bool> directory;
        qingzhen::property<int64_t> access_time;
        qingzhen::property<int64_t> create_time;
        qingzhen::property<int64_t> modify_time;
        // Property <int64_t> SpaceCapacity = int64_t(0);
        qingzhen::property<int64_t> version;
        qingzhen::property<bool> locking;
        qingzhen::property<int> op;
        qingzhen::property<bool> preview;
        qingzhen::property<int> preview_type;
        qingzhen::property<int> flag;
        qingzhen::property<utility::string_t> unique_identity;
        qingzhen::property<bool> share;
        qingzhen::property<utility::string_t> download_address;

        [[nodiscard]] web::json::value to_json() const override;

    private:

    };
}
#endif //QINGZHEN_FILE_H
