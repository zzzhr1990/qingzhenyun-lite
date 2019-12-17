//
// Created by herui on 2019/12/13.
//

#include <qingzhen/path_util.h>

#ifdef _WIN32
#include <Windows.h>
#else

#include <fcntl.h>
#include <unistd.h>

#endif // _WIN32


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
        // Windows
#ifdef _WIN32
        HANDLE handle = CreateFile(path.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
        if (handle == INVALID_HANDLE_VALUE) {
            return false;
        }
        /* old Windows
         * LARGE_INTEGER size;
            size.HighPart = (uint32_t)((length & 0xFFFFFFFF00000000LL) >> 32);
            size.LowPart = (uint32_t)(length & 0xFFFFFFFFLL);
         */
        LARGE_INTEGER size;
        size.QuadPart = file_size;
        if(!::SetFilePointerEx(handle, size, 0, FILE_BEGIN)){
            CloseHandle(handle);
            return false;
        }
        if(!::SetEndOfFile(handle)){
            CloseHandle(handle);
            return false;
        }
        // ::SetFilePointer(handle, 0, 0, FILE_BEGIN);
        CloseHandle(handle);
        return true;
#else // Other standard system
        FILE *fp = fopen(path.c_str(), "w+");
        if (!fp) {
            return false;
        }
        int fd = fileno(fp);
#ifdef __APPLE__
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
        int ret = fallocate(fd, FALLOC_FL_ZERO_RANGE, 0, file_size);
        fclose(fp);
        return ret == 0;
#endif // __APPLE__
#endif // _WIN32
    } catch (const std::exception &) {
        return false;
    }

}

void path_util::clean_path(const std::filesystem::path &path) {
    try {
        std::filesystem::remove_all(path);
    } catch (...) {}
    // return false;
}

bool path_util::move_file(const std::filesystem::path &source, const std::filesystem::path &dest) {
    try {
        std::filesystem::remove_all(dest);
    } catch (...) {
    }

    try {
        std::filesystem::rename(source, dest);
    } catch (...) {
        return false;
    }
    return true;
    // return false;
}
