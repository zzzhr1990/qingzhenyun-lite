//
// Created by herui on 2019/12/13.
//

#include <qingzhen/path_util.h>

#ifdef __APPLE__

#include <fcntl.h>
#include <zconf.h>

#endif
using namespace qingzhen;

bool path_util::ensure_directory(const std::filesystem::path &path) {
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

bool path_util::ensure_and_alloc_file(const std::filesystem::path &path, const int64_t file_size) {
    try {
        if (std::filesystem::exists(path)) {
            if (std::filesystem::is_directory(path)) {
                std::filesystem::remove_all(path);
            } else {
                if (std::filesystem::file_size(path) == file_size) {
                    return true;
                }
                std::filesystem::remove_all(path);
            }
        }
        // 1. windows
#ifdef _WIN32
        HANDLE handle = CreateFile(path.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
        if (handle == INVALID_HANDLE_VALUE) {
            return false;
        }
        /* OLD
         * LARGE_INTEGER size;
            size.HighPart = (uint32_t)((length & 0xFFFFFFFF00000000LL) >> 32);
            size.LowPart = (uint32_t)(length & 0xFFFFFFFFLL);
         */
        LARGE_INTEGER size;
        size.QuadPart = file_size;
        if(!::SetFilePointerEx(handle, size, 0, FILE_BEGIN)){
            CloseHandle(file);
            return false;
        }
        if(!::SetEndOfFile(handle)){
            CloseHandle(file);
            return false;
        }
        // ::SetFilePointer(handle, 0, 0, FILE_BEGIN);
        CloseHandle(file);
        return true;
#elif __APPLE__
        FILE *fp = fopen(path.c_str(), "w+");
        if (!fp) {
            return false;
        }
        int fd = fileno(fp);
        fstore_t store = {F_ALLOCATECONTIG, F_PEOFPOSMODE, 0, file_size, 0};
        int ret = fcntl(fd, F_PREALLOCATE, &store);
        if (-1 == ret) {
            store.fst_flags = F_ALLOCATEALL;
            ret = fcntl(fd, F_PREALLOCATE, &store);
            if (-1 == ret) {
                fclose(fp);
                return false;
            }
        }
        if (-1 == ftruncate(fd, file_size)) {
            fclose(fp);
            return false;
        }
        fclose(fp);
        return true;
#else
        FILE *fp = fopen(path.c_str(), "w+");
        if (!fp) {
            return false;
        }
        int fd = fileno(fp);
        int ret = fallocate(fd, FALLOC_FL_ZERO_RANGE, 0, file_size);
        fclose(fp);
        return ret == 0;

#endif // _WIN32
    } catch (const std::exception &) {
        return false;
    }

}
