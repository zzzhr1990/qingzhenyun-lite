//
// Created by herui on 2019/12/10.
//

#ifndef QINGZHEN_FILE_REQUESTS_H
#define QINGZHEN_FILE_REQUESTS_H

#include <qingzhen/api/file.h>
#include <qingzhen/api/common_list_response.hpp>
#include <utility>
#include <map>

namespace qingzhen::api {
    class file_list_request {
    public:
        explicit file_list_request(file parent_file_data);

        property <int64_t> start;
        property <int64_t> limit;
        property<int> directory;
        std::map<utility::string_t, utility::string_t> sort_map;

        std::shared_ptr<file> parent() {
            return parent_entity;
        }

        [[nodiscard]] web::json::value to_json() const;

    private:
        std::shared_ptr<file> parent_entity;
    };
}


#endif //QINGZHEN_FILE_REQUESTS_H
