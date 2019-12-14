//
// Created by herui on 2019/12/13.
//

#ifndef QINGZHENYUN_LITE_BASE_FILE_TASK_H
#define QINGZHENYUN_LITE_BASE_FILE_TASK_H

#include <memory>
#include <qingzhen/properity.hpp>
#include <qingzhen/transfer/transfer_entity.h>
#include <pplx/pplxtasks.h>

namespace qingzhen::transfer {
    template<class _Tp>
    class base_file_task : public std::enable_shared_from_this<_Tp> {
    public:
        std::shared_ptr<_Tp> get_ptr() {
            return this->shared_from_this();
        }

        qingzhen::property<transfer_direction> direction;

        // virtual bool start(pplx::cancellation_token token) = 0;
        virtual bool start(const pplx::cancellation_token &token) { return _start(token); };

        virtual void parse_task_sync(const pplx::cancellation_token &cancellation_token) = 0;

        virtual transfer_status get_status_with_lock() {
            std::lock_guard<std::mutex> _ulc(this->mutex);
            return this->_status;
        }

        virtual void update_status_with_lock(transfer_status new_status) {
            std::lock_guard<std::mutex> _ulc(this->mutex);
            this->_status = new_status;
        }

        virtual void success_with_lock() {
            this->update_status_with_lock(transfer_status::complete);
        }

        virtual void complete_with_lock() {
            std::lock_guard<std::mutex> _ulc(this->mutex);
            this->_status = transfer_status::complete;
        }

        virtual void on_error_lock(const utility::string_t &error_referer, const utility::string_t &error_message,
                                   int max_error_count) {
            std::cout << "on error: " << error_referer.c_str() << std::endl;
            std::lock_guard<std::mutex> _error(this->mutex);
            this->_error_count.fetch_add(1);
            if (this->_error_count > max_error_count) {
                this->_status = transfer_status::fatal_error;
            } else {
                this->_status = transfer_status::error;
            }
        }

    protected:
        // can start,  next task
        explicit base_file_task<_Tp>(transfer_direction dir) : direction(dir) {}

        virtual void _tick() {};

        virtual bool _start(pplx::cancellation_token token) {
            // return can continue
            std::lock_guard<std::mutex> _start(this->mutex);
            // selected?
            auto[ct, success] = this->_can_start();
            if (success) {
                this->_tick();
            }
            if (!ct) {
                return success;
            }


            if (!this->cancellation_token_source.get_token().is_canceled()) {
                this->cancellation_token_source.cancel();
            }

            // start transfer...;
            // auto token_s = this->cancellation_token_source.get_token();
            //
            this->cancellation_token_source = pplx::cancellation_token_source::create_linked_source(token);
            this->_status = transfer_status::pre_checking;
            this->_error_count.store(0);
            auto ptr = get_ptr();
            auto cancellation_token = this->cancellation_token_source.get_token();
            pplx::create_task([ptr, cancellation_token]() { ptr->parse_task_sync(cancellation_token); });
            return true;
        }

        virtual std::pair<bool, bool> _can_start() {
            if (!this->_selected.load()) {
                return std::make_pair(false, false); // need to continue next
            }
            if (this->_status == transfer_status::complete || this->_status == transfer_status::pause ||
                this->_status == transfer_status::fatal_error) {
                return std::make_pair(false, false); // need to continue next
            }
            return std::make_pair(
                    !(this->_status == transfer_status::pre_checking || this->_status == transfer_status::transfer ||
                      this->_status == transfer_status::after_checking), true);
        }


        std::atomic_int _error_count = 0;
        pplx::cancellation_token_source cancellation_token_source;
        std::mutex mutex;
        transfer_status _status = transfer_status::add;
        std::atomic_bool _selected = true;
    };
}

#endif //QINGZHENYUN_LITE_BASE_FILE_TASK_H
