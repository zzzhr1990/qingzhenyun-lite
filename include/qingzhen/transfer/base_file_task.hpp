//
// Created by herui on 2019/12/13.
//

#ifndef QINGZHENYUN_LITE_BASE_FILE_TASK_H
#define QINGZHENYUN_LITE_BASE_FILE_TASK_H

#include <memory>
#include <qingzhen/properity.hpp>
#include <qingzhen/transfer/transfer_entity.h>
#include <pplx/pplxtasks.h>
#include <qingzhen/string_util.h>

namespace qingzhen::transfer {
    template<class _Tp>
    class base_file_task : public std::enable_shared_from_this<_Tp> {
    public:
        std::shared_ptr<_Tp> get_ptr() {
            return this->shared_from_this();
        }

        qingzhen::property<transfer_direction> direction;

        virtual std::pair<bool, transfer_status> start(const pplx::cancellation_token &token) { return _start(token); };

        virtual void parse_task_sync(const pplx::cancellation_token &cancellation_token) = 0;

        virtual transfer_status get_status_with_lock() {
            std::lock_guard<std::mutex> _ulc(this->mutex);
            return this->_status;
        }

        virtual void update_status_with_lock(transfer_status new_status) {
            std::lock_guard<std::mutex> _ulc(this->mutex);
            this->_status = new_status;
        }

        virtual int64_t processed() {
            return this->_processed_size;
        }

        virtual std::tuple<int64_t, int64_t, int64_t> get_current_progress() {
            int64_t progress = 0;
            if (_size > 1) {
                progress = this->_processed_size * 100 / _size;
                if (progress > 100) {
                    progress = 100;
                }
            }
            return std::make_tuple(this->_processed_size.load(), this->_size, progress);
        }

        virtual void refresh_progress() = 0;

        virtual void success_with_lock() {
            _bytes_per_second = 0;
            this->reset_counter();
            this->update_status_with_lock(transfer_status::complete);
        }

        virtual void complete_with_lock() {
            this->success_with_lock();
        }

        virtual void on_error_lock(const utility::string_t &error_referer, const utility::string_t &error_message,
                                   int max_error_count) {
            std::cout << "______________on error: " << qingzhen::string_util::string_t_to_ansi(error_referer.c_str()) << std::endl;
            std::lock_guard<std::mutex> _error(this->mutex);
            this->_error_count.fetch_add(1);
            if (this->_error_count > max_error_count) {
                this->_status = transfer_status::fatal_error;
            } else {
                this->_status = transfer_status::error;
            }
        }

        virtual void reset_counter() {
            _last_refresh_time = std::chrono::system_clock::now();
            _bytes_transfer = 0;
            _last_bytes_transfer = 0;
            _bytes_per_second = 0;
        }

        virtual void incr_counter(int64_t bytes) {
            _bytes_transfer.fetch_add(bytes);
        }

        virtual int64_t get_last_speed() {
            return _bytes_per_second;
        }

        virtual void refresh_byte_counter() {
            auto current = std::chrono::system_clock::now();
            auto time_diff_in_sec = (current - _last_refresh_time) / std::chrono::seconds(1);
            if (time_diff_in_sec < 1) {
                return;
            }
            auto last_transfer = this->_bytes_transfer.load();
            auto bytes_diff = last_transfer - this->_last_bytes_transfer;
            if (bytes_diff < 0) {
                bytes_diff = 0;
            }
            this->_last_bytes_transfer.store(last_transfer);
            this->_last_refresh_time = current;
            _bytes_per_second = bytes_diff / time_diff_in_sec;
        }

    protected:
        // can start,  next task
        explicit base_file_task<_Tp>(transfer_direction dir, int64_t size) : direction(dir), _size(size),
                                                                             _processed_size(0) {}

        virtual void _tick() {};

        virtual std::pair<bool, transfer_status> _start(pplx::cancellation_token token) {
            // return can continue
            std::lock_guard<std::mutex> _start(this->mutex);
            // selected?
            auto[ct, success] = this->_can_start(); // param1: can start, param2: task is running
            if (success) {
                this->_tick();
            }
            if (!ct) {
                return std::make_pair(success, _status);
            }


            if (!this->cancellation_token_source.get_token().is_canceled()) {
                this->cancellation_token_source.cancel();
            }

            // start transfer...;
            // auto token_s = this->cancellation_token_source.get_token();
            //
            this->cancellation_token_source = pplx::cancellation_token_source::create_linked_source(token);
			this->_status = transfer_status::pre_checking;
            // this->_error_count.store(0);
            auto ptr = get_ptr();
            auto cancellation_token = this->cancellation_token_source.get_token();
            auto __ = pplx::create_task([ptr, cancellation_token]() { ptr->parse_task_sync(cancellation_token); });
            return std::make_pair(true, _status);
        }

        virtual std::pair<bool, bool> _can_start() {
            if (!this->_selected.load()) {
                return std::make_pair(false, false); // need to continue next
            }
            if (this->_status == transfer_status::complete || this->_status == transfer_status::pause ||
                this->_status == transfer_status::fatal_error) {
                return std::make_pair(false, false); // need to continue next
            }
            /*
            if(this->_status == transfer_status::transfer) {
                // refresh task
                this->refresh_progress();
            }
             */
            return std::make_pair(
                    !(this->_status == transfer_status::pre_checking || this->_status == transfer_status::transfer ||
                      this->_status == transfer_status::after_checking), true);
        }


        std::atomic_int _error_count = 0;
        pplx::cancellation_token_source cancellation_token_source;
        std::mutex mutex;
        transfer_status _status = transfer_status::add;
        std::atomic_bool _selected = true;
        std::int64_t _size;
        std::atomic_int64_t _processed_size;
        std::chrono::system_clock::time_point _last_refresh_time = std::chrono::system_clock::now();
        std::atomic_int64_t _bytes_transfer = 0;
        std::atomic_int64_t _last_bytes_transfer = 0;
        std::atomic_int64_t _bytes_per_second = 0;
    };
}

#endif //QINGZHENYUN_LITE_BASE_FILE_TASK_H
