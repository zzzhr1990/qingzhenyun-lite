//
// Created by herui on 2019/12/13.
//

#ifndef QINGZHENYUN_LITE_PATH_UTIL_H
#define QINGZHENYUN_LITE_PATH_UTIL_H

#include <filesystem>

namespace qingzhen {
    class path_util {
    public:
        static bool ensure_directory(const std::filesystem::path &path);
    };
}

#endif //QINGZHENYUN_LITE_PATH_UTIL_H
