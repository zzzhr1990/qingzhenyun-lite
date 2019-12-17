//
// Created by herui on 2019/12/17.
//

#ifndef QINGZHENYUN_LITE_FILE_HASH_CONTAINER_H
#define QINGZHENYUN_LITE_FILE_HASH_CONTAINER_H

#include <fstream>
#include <filesystem>
#include <qingzhen/transfer/transfer_entity.h>
#include <pplx/pplxtasks.h>


namespace qingzhen::transfer {
    class file_hash_container {
    public:
        explicit file_hash_container(std::filesystem::path path);

        ~file_hash_container();

        file_hash_container(const file_hash_container &) = delete;

        file_hash_container(const file_hash_container &&) = delete;

        std::unique_ptr<transfer_result>
        hash_file(const pplx::cancellation_token &cancellation_token, bool calc_md5_and_sha1);

        // void on_read_update(bool switch_);

    private:
        std::filebuf buf;
        std::filesystem::path file_path;
        const int BUFFER_SIZE = 1048576;
        const int WCS_BLOCK_SIZE = 1048576 * 4;
        char *file_read_buffer = new char[BUFFER_SIZE]; // 1MB
        // MD5_CTX *md5_ctx = nullptr;
    };

}
#endif //QINGZHENYUN_LITE_FILE_HASH_CONTAINER_H
