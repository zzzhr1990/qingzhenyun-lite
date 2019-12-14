//
// Created by herui on 2019/12/13.
//

#include <qingzhen/path_util.h>

bool qingzhen::path_util::ensure_directory(const std::filesystem::path &path) {
    try {
        if (std::filesystem::is_directory(path)) {
            return true;
        }
        if (std::filesystem::exists(path)) {
            return false;
        }
        if (std::filesystem::create_directories(path)) {
            return true;
        }
        if (std::filesystem::is_directory(path)) {
            return true;
        }
        // if
    } catch (std::exception &) {
        return false;
    }

    return true;
}
