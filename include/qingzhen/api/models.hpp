//
// Created by herui on 2019/11/28.
//

#ifndef QINGZHEN_MODELS_H
#define QINGZHEN_MODELS_H

#include <cpprest/json.h>
#include <qingzhen/properity.hpp>
#include <memory>

namespace qingzhen::api {
    class standard_json_response {
    public:
        virtual bool parse(const web::json::value &v) = 0;

        [[nodiscard]] virtual web::json::value to_json() const = 0;
    };

    template<typename EntityType, typename = std::enable_if<std::is_base_of<standard_json_response, standard_json_response>::value>>
    class standard_api_response {
    public:
        standard_api_response() : success(false), cancel(false), error_message(utility::string_t()),
                                  error_ref(utility::string_t()), data(std::make_shared<EntityType>()) {};
        qingzhen::property<bool> success;
        qingzhen::property<bool> cancel;
        qingzhen::property<utility::string_t> error_message;
        qingzhen::property<utility::string_t> error_ref;
        qingzhen::property<std::shared_ptr<EntityType>> data;
    };


}
#endif //QINGZHEN_MODELS_H
