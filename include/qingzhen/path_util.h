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

        static bool ensure_and_alloc_file(const std::filesystem::path &path, int64_t file_size);
    };
}

#endif //QINGZHENYUN_LITE_PATH_UTIL_H
