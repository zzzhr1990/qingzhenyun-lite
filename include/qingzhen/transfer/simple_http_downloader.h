//
// Created by herui on 2019/12/13.
//

#ifndef QINGZHENYUN_LITE_SIMPLE_HTTP_DOWNLOADER_H
#define QINGZHENYUN_LITE_SIMPLE_HTTP_DOWNLOADER_H

#include <memory>
#include <qingzhen/transfer/single_file_task.h>
#include <atomic>

namespace qingzhen::transfer {
    class simple_http_downloader : public std::enable_shared_from_this<simple_http_downloader> {
    public:
        static std::shared_ptr<simple_http_downloader> &instance();

        simple_http_downloader(simple_http_downloader const &) = delete;             // Copy construct
        simple_http_downloader(simple_http_downloader &&) = delete;                  // Move construct
        simple_http_downloader &operator=(simple_http_downloader const &) = delete;  // Copy assign
        simple_http_downloader &operator=(simple_http_downloader &&) = delete;      // Move assign
        std::shared_ptr<simple_http_downloader> get_ptr();

        void start(const std::shared_ptr<single_file_task> &task, const pplx::cancellation_token &cancellation_token);

        std::tuple<bool, bool, utility::string_t>
        check_support_ranges(utility::string_t url, pplx::cancellation_token cancellation_token, int redirect_count);

        bool start_download_single_part(const utility::string_t &uri, const std::shared_ptr<single_file_task> &task,
                                        size_t index, const std::filesystem::path &path,
                                        const pplx::cancellation_token &cancellation_token);

        void incr_global_counter(int64_t count);

    private:
        simple_http_downloader() = default;

        std::atomic_int64_t _global_bytes_transfer = 0;
    };
}
#endif //QINGZHENYUN_LITE_SIMPLE_HTTP_DOWNLOADER_H
