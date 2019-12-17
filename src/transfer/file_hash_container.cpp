//
// Created by herui on 2019/12/17.
//
#include <qingzhen/transfer/file_hash_container.h>
#include <utility>
#include <qingzhen/string_util.h>
#include <cpprest/asyncrt_utils.h>
#include <openssl/md5.h>
#include <openssl/sha.h>

using namespace qingzhen::transfer;

qingzhen::transfer::file_hash_container::file_hash_container(std::filesystem::path path) : file_path(std::move(path)) {
    // this->md5_ctx = new MD5_CTX;
    // MD5_Init(this->md5_ctx);
    // MD5_U
}

std::unique_ptr<transfer_result>
file_hash_container::hash_file(const pplx::cancellation_token &cancellation_token, bool calc_md5_and_sha1) {
    auto result = std::move(transfer_result::create());
    try {
        this->buf.open(this->file_path, std::ios_base::in | std::ios_base::binary);
        if (!this->buf.is_open()) {
            result->hash_failed();
            return result;
        }
    } catch (const std::exception &ex) {
        result->hash_failed();
        return result;
    }


    // now open
    // auto current = std::chrono::system_clock::now();
    uint64_t t_size = 0;
    auto last_size = this->buf.sgetn(this->file_read_buffer, BUFFER_SIZE);
    t_size += last_size;
    auto last_block_write = last_size;
    SHA_CTX wcs_single_block_ctx;
    SHA_CTX common_sha_ctx;
    MD5_CTX common_md5_ctx;
    if (SHA1_Init(&wcs_single_block_ctx) == 0) {
        result->hash_failed();
        return result;
    }
    if (calc_md5_and_sha1) {
        if (SHA1_Init(&common_sha_ctx) == 0) {
            result->hash_failed();
            return result;
        }
        SHA1_Update(&common_sha_ctx, this->file_read_buffer, last_size); // consider zero size
        if (MD5_Init(&common_md5_ctx) == 0) {
            result->hash_failed();
            return result;
        }
        MD5_Update(&common_md5_ctx, this->file_read_buffer, last_size); // consider zero size
    }
    SHA1_Update(&wcs_single_block_ctx, this->file_read_buffer, last_size); // consider zero size
    bool flag = true;
    unsigned char temp_digest[SHA_DIGEST_LENGTH] = {0};

    std::vector<unsigned char> ss;
    while (last_size >= BUFFER_SIZE) {
        flag = false;
        if (cancellation_token.is_canceled()) {
            result->cancel();
            return result;
        }
        last_size = this->buf.sgetn(this->file_read_buffer, BUFFER_SIZE);
        t_size += last_size;
        if (calc_md5_and_sha1) {

            SHA1_Update(&common_sha_ctx, this->file_read_buffer, last_size); // consider zero size

            MD5_Update(&common_md5_ctx, this->file_read_buffer, last_size); // consider zero size
        }
        SHA1_Update(&wcs_single_block_ctx, this->file_read_buffer, last_size);
        last_block_write += last_size;
        if (last_size > 0 && t_size % WCS_BLOCK_SIZE == 0) {
            SHA1_Final(temp_digest, &wcs_single_block_ctx);
            //
            for (unsigned char p: temp_digest) {
                ss.push_back(p);
            }
            if (SHA1_Init(&wcs_single_block_ctx) == 0) {
                result->hash_failed();
                return result;
            }
            last_block_write = 0;
        }
    }
    if (cancellation_token.is_canceled()) {
        result->cancel();
        return result;
    }
    if (flag || last_block_write > 0) {
        SHA1_Final(temp_digest, &wcs_single_block_ctx);
        for (unsigned char p: temp_digest) {
            ss.push_back(p);
        }
    }


    unsigned char final_digest[SHA_DIGEST_LENGTH + 1];
    if (t_size <= WCS_BLOCK_SIZE) {
        final_digest[0] = 0x16;
    } else {
        final_digest[0] = 0x96;
        unsigned char *cdata = ss.data();
        SHA1_Init(&wcs_single_block_ctx);
        SHA1_Update(&wcs_single_block_ctx, cdata, ss.size());
        SHA1_Final(temp_digest, &wcs_single_block_ctx);
    }
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {//SHA1 20
        final_digest[i + 1] = temp_digest[i];
    }
    std::vector<unsigned char> v(std::begin(final_digest), std::end(final_digest));
    auto base64 = utility::conversions::to_base64(v);
    for (auto &i : base64) {
        if (i == _XPLATSTR('+')) {
            i = _XPLATSTR('-');
        }

        if (i == _XPLATSTR('/')) {
            i = _XPLATSTR('_');
        }
    }

    result->success = true;
    result->hash = base64;

    if (calc_md5_and_sha1) {
        if (cancellation_token.is_canceled()) {
            result->hash_failed();
            return result;
        }
        unsigned char common_md5_digest[MD5_DIGEST_LENGTH] = {0};
        MD5_Final(common_md5_digest, &common_md5_ctx);
        utility::ostringstream_t oss;
        for (auto c : common_md5_digest) {
            oss << std::hex << std::setw(2) << std::setfill(_XPLATSTR('0')) << static_cast<int>(c);
        }
        result->md5 = oss.str();
        oss.str(utility::string_t());
        SHA1_Final(temp_digest, &common_sha_ctx);
        for (auto c : temp_digest) {
            oss << std::hex << std::setw(2) << std::setfill(_XPLATSTR('0')) << static_cast<int>(c);
        }
        result->sha1 = oss.str();
    }
    // Hash old
    //
    return result;
}

file_hash_container::~file_hash_container() {
    if (this->buf.is_open()) {
        try {
            this->buf.close();
        } catch (std::exception &ex) {}
    }
    delete[] this->file_read_buffer;
    this->file_read_buffer = nullptr;

}

