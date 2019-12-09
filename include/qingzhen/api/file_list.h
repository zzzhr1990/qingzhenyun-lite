//
// Created by herui on 2019/12/10.
//

#ifndef QINGZHENYUN_LITE_FILE_LIST_H
#define QINGZHENYUN_LITE_FILE_LIST_H

#include <qingzhen/api/file.h>
#include <qingzhen/api/common_list_response.hpp>


namespace qingzhen::api {
    class file_list : public common_list_response<file> {
    public:
        bool parse(const web::json::value &v) override {
            if (v.has_field(_XPLATSTR("parent"))) {
                // auto fp = file();
                if (parent_entity->parse(v.at(_XPLATSTR("parent")))) {
                    // this->parent_entity = fp;
                }
            }
            this->parse_list(v);
            return true;
        }

        std::shared_ptr<file> parent() {
            return parent_entity;
        }

    private:
        std::shared_ptr<file> parent_entity = std::make_shared<file>();
    };
}
#endif //QINGZHENYUN_LITE_FILE_LIST_H
