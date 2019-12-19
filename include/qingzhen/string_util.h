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
		static utility::string_t ansi_to_string_t(const char* chars);
		static utility::string_t ansi_to_string_t(const std::string& string);
		static std::string string_t_to_ansi(const utility::string_t& str);
		static std::string string_t_to_ansi(const wchar_t* str);
		static std::string string_t_to_ansi(const char* str);
    };


}

#endif //QINGZHEN_STRING_UTIL_H
