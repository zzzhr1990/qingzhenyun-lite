//
// Created by herui on 2019/12/11.
//

#include <qingzhen/transfer/transfer_manager.h>
#include <boost/log/trivial.hpp>
#include <qingzhen/transfer/simple_http_downloader.h>
#include <thread>
#include <qingzhen/string_util.h>

using namespace qingzhen::transfer;

void transfer_manager::start() {
    //
    std::lock_guard<std::mutex> _start_mutex(this->mutex);
    // setup io loop (boost)
    if (this->shutdown_flag.load()) {
        return;
    }
    if (this->init_flag.load()) {
        return;
    }
    this->init_flag.store(true);
    timer.async_wait([this](const boost::system::error_code &ec) {
        this->_timer_tick();
    });
    // Trick: prevent boost crash in boost::asio::detail::scheduler_operation
    // If the destructor called before the asio context shutdown completely
    // An EXC_BAD_ACCESS will be thrown
    std::thread t([this]() {
        this->shutdown_complete.store(false);
        this->_start_io_loop();
        this->shutdown_complete.store(true);
    });
    t.detach();
}

bool transfer_manager::add_download(const std::shared_ptr<qingzhen::api::file> &source,
                                    const std::filesystem::path &destination) {
    return this->add(source, destination, transfer_direction::download);
}

void transfer_manager::shutdown() {
    this->shutdown_flag.store(true);
    this->timer.cancel();
    this->io.stop();
    this->cancellation_token_source.cancel();
    while (!this->shutdown_complete.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void transfer_manager::_timer_tick() {
    std::lock_guard<std::mutex> _dml(this->mutex);
    if (!this->shutdown_flag.load()) {
        this->_check();
        this->timer.expires_from_now(boost::asio::chrono::seconds(1));
        this->timer.async_wait([this](const boost::system::error_code &ec) {
            this->_timer_tick();
        });
    }
}

void transfer_manager::_start_io_loop() {
    this->io.run();
}

void transfer_manager::_check() {
    // it must be single?
    if (this->shutdown_flag.load()) {
        return;
    }
    // 1. check download queue
    int count = 0;
    for (const auto &single_item : this->_download_queue) {
        auto cont = single_item->start(this->cancellation_token_source.get_token());
        if (cont.first) {
            count++;
        }
        if (count >= this->concurrent_task.load()) {
            break;
        }
    }
	auto& downloader = qingzhen::transfer::simple_http_downloader::instance();
	downloader->refresh_counter();
	BOOST_LOG_TRIVIAL(info) << "Running download tasks: " << count << " speed: " << qingzhen::string_util::string_t_to_ansi(qingzhen::string_util::pretty_bytes(downloader->get_downloading_speed())) << "/s" << std::endl;
}

void transfer_manager::set_concurrent_task(int tasks) {
    // std::lock_guard<std::mutex> _dml(this->mutex);
    this->concurrent_task.store(tasks);
}

bool transfer_manager::add(const std::shared_ptr<qingzhen::api::file> &source, const std::filesystem::path &destination,
                           qingzhen::transfer::transfer_direction direction) {

    // Add
    if (direction == transfer_direction::download) {
        return _add_download(source, destination);
    }
    return false;
}

bool transfer_manager::_add_download(const std::shared_ptr<qingzhen::api::file> &source,
                                     const std::filesystem::path &destination) {
    std::lock_guard<std::mutex> _dml(this->mutex);
    for (auto &single : this->_download_queue) {
        if (single->remote_file()->identity() == source->identity()) {
            return false;
        }
    }
    this->_download_queue.emplace_back(
            transfer_item::create(source, destination, transfer_direction::download, source->size()));
    return true;
}

bool transfer_manager::_add_upload(const std::shared_ptr<qingzhen::api::file> &remote_parent,
                                   const std::filesystem::path &source) {
    std::lock_guard<std::mutex> _dml(this->mutex);
    // calc
    for (auto &single : this->_upload_queue) {
        std::filesystem::path p = single->local_parent_path().operator*();
        if (p == source) {
            return false;
        }
    }
    // auto file_size = std::filesystem::file_size(source);
    this->_upload_queue.emplace_back(
            transfer_item::create(remote_parent, source, transfer_direction::upload, 0));
    return true;
}

