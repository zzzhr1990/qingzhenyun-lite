//
// Created by herui on 2019/11/28.
//

#ifndef QINGZHEN_STRING_UTIL_H
#define QINGZHEN_STRING_UTIL_H

#include <cpprest/details/basic_types.h>
#include <cmath>

namespace qingzhen {
    class string_util {
    public:
        static utility::string_t pretty_bytes(const int64_t &bytes);
    };


}

#endif //QINGZHEN_STRING_UTIL_H
