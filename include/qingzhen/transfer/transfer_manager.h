//
// Created by herui on 2019/12/10.
//

#ifndef QINGZHENYUN_LITE_TRANSFER_MANAGER_H
#define QINGZHENYUN_LITE_TRANSFER_MANAGER_H

#include <qingzhen/api/file.h>
#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <qingzhen/transfer/transfer_entity.h>
#include <qingzhen/transfer/transfer_item.h>


namespace qingzhen::transfer {
    class transfer_manager {
    public:
        static transfer_manager &instance() {
            static transfer_manager instance;
            return instance;
        }

        transfer_manager &operator=(const transfer_manager &) = delete;

        transfer_manager(const transfer_manager &) = delete;

        transfer_manager(const transfer_manager &&) = delete;

        void start();

        bool add(const std::shared_ptr<qingzhen::api::file> &source, const std::filesystem::path &destination,
                 transfer_direction direction);

        bool add_download(const std::shared_ptr<qingzhen::api::file> &source, const std::filesystem::path &destination);

        void shutdown();

        void _timer_tick();

        void _start_io_loop();

        void _check();

        void set_concurrent_task(int tasks);

        std::atomic_bool wait_shutdown = false;
        std::atomic_bool shutdown_complete = true;
    private:
        transfer_manager() = default;

        bool
        _add_download(const std::shared_ptr<qingzhen::api::file> &source, const std::filesystem::path &destination);

        bool
        _add_upload(const std::shared_ptr<qingzhen::api::file> &source, const std::filesystem::path &destination);

        std::mutex mutex;
        pplx::cancellation_token_source cancellation_token_source;
        boost::asio::io_context io = boost::asio::io_context();
        boost::asio::steady_timer timer = boost::asio::steady_timer(io, boost::asio::chrono::seconds(1));
        boost::asio::io_context::work _ = boost::asio::io_context::work(io);
        std::atomic_bool shutdown_flag = false;
        std::atomic_bool init_flag = false;
        std::vector<std::shared_ptr<transfer_item>> _download_queue;
        std::vector<std::shared_ptr<transfer_item>> _upload_queue;
        std::atomic_int concurrent_task = 1;
    };
}
#endif //QINGZHENYUN_LITE_TRANSFER_MANAGER_H
