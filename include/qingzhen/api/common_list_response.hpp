//
// Created by herui on 2019/11/30.
//

#ifndef QINGZHEN_COMMON_LIST_RESPONSE_H
#define QINGZHEN_COMMON_LIST_RESPONSE_H

#include <type_traits>
#include "qingzhen/api/models.hpp"
#include <vector>

namespace qingzhen::api {
    template<typename _Response_Type, typename = std::enable_if<std::is_base_of<standard_json_response, standard_json_response>::value>>
    class common_list_response : public standard_json_response {
    public:
        bool parse(const web::json::value &v) override {
            return this->parse_list(v);
        }

        virtual bool parse_list(const web::json::value &v) {
            if (v.has_array_field(_XPLATSTR("dataList"))) {
                auto arr = v.at(_XPLATSTR("dataList")).as_array();
                for (auto sig: arr) {
                    std::shared_ptr<_Response_Type> ts = std::make_shared<_Response_Type>();
                    bool suc = ts->parse(sig);
                    if (suc) {
                        internal_list.push_back(ts);
                    }
                }
            }
            return true;
        }

        [[nodiscard]] web::json::value to_json() const override {
            return web::json::value();
        }

        std::vector<std::shared_ptr<_Response_Type>> list() {
            return this->internal_list;
        }

    private:
        std::vector<std::shared_ptr<_Response_Type>> internal_list;
    };

}
#endif //QINGZHEN_COMMON_LIST_RESPONSE_H
