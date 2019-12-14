//
// Created by herui on 2019/12/13.
//

#ifndef QINGZHENYUN_LITE_SIMPLE_HTTP_DOWNLOADER_H
#define QINGZHENYUN_LITE_SIMPLE_HTTP_DOWNLOADER_H

#include <memory>
#include <qingzhen/transfer/single_file_task.h>

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

    private:
        simple_http_downloader() = default;

    };
}
#endif //QINGZHENYUN_LITE_SIMPLE_HTTP_DOWNLOADER_H
