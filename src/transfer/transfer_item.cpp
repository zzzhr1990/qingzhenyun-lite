//
// Created by herui on 2019/12/11.
//

#include <qingzhen/transfer/transfer_item.h>
#include <qingzhen/api/user_file_client.hpp>

using namespace qingzhen::transfer;

const int MAX_RETRY_TIME = 5;

std::shared_ptr<transfer_item> transfer_item::get_ptr() {
    return shared_from_this();
}

transfer_item::transfer_item(const std::shared_ptr<qingzhen::api::file> &file,
                             const std::shared_ptr<std::filesystem::path> &dest,
                             qingzhen::transfer::transfer_direction dir) : remote_file(file),
                                                                           local_parent_path(dest), _select(true),
                                                                           _parsed(false), direction(dir),
                                                                           _status(transfer_status::add),
                                                                           _failed_count(0) {

}

std::shared_ptr<transfer_item>
transfer_item::create(const std::shared_ptr<qingzhen::api::file> &file, const std::filesystem::path &dest,
                      qingzhen::transfer::transfer_direction dir) {
    return std::shared_ptr<transfer_item>(new transfer_item(file, std::make_shared<std::filesystem::path>(dest), dir));
}

bool transfer_item::start(pplx::cancellation_token token) {
    // return can continue
    std::lock_guard<std::mutex> _start(this->mutex);
    // selected?
    if (!this->_select.load()) {
        return false; // need to continue next
    }
    if (this->_status == transfer_status::complete || this->_status == transfer_status::pause ||
        this->_status == transfer_status::fatal_error) {
        return false; // need to continue next
    }
    if (this->_status == transfer_status::pre_checking || this->_status == transfer_status::transfer ||
        this->_status == transfer_status::after_checking) {
        return true; // doing, not transfer
    }

    if (!this->cancellation_token_source.get_token().is_canceled()) {
        this->cancellation_token_source.cancel();
    }

    // start transfer...;
    // auto token_s = this->cancellation_token_source.get_token();
    //
    this->cancellation_token_source = pplx::cancellation_token_source::create_linked_source(token);
    this->_status = transfer_status::pre_checking;
    this->_failed_count.store(0);
    auto ptr = get_ptr();
    auto cancellation_token = this->cancellation_token_source.get_token();
    pplx::create_task([ptr, cancellation_token]() { ptr->parse_task_sync(cancellation_token); });
    return true;
}

void transfer_item::parse_task_sync(const pplx::cancellation_token &cancellation_token) {
    this->_tasks.clear();
    // status should be pre checking
    if (this->direction() == transfer_direction::download) {
        this->parse_download_task_sync(cancellation_token);
    }
}

bool transfer_item::parse_remote_directory_sync(const std::shared_ptr<qingzhen::api::file> &file,
                                                const std::shared_ptr<std::filesystem::path> &parent_path,
                                                const pplx::cancellation_token &cancellation_token) {
    // 1. get list
    if (cancellation_token.is_canceled()) {
        return false;
    }
    qingzhen::api::file_list_request fr(file);
    auto result = qingzhen::api::user_file_client::list_directory(fr, cancellation_token).get();
    if (result.cancel()) {
        return false;
    }
    if (!result.success()) {
        this->on_error(result.error_ref(), result.error_message());
        return false;
    }
    for (auto &single_file : result.data()->list()) {
        // add self first
        this->_tasks.emplace_back(single_file_task::create(single_file, parent_path, this->direction()));
        if (single_file->directory()) {
            //

            std::filesystem::path cur_path = std::filesystem::path(*parent_path);
            try {
                cur_path.append(single_file->name());
            } catch (const std::exception &) {
                this->on_error(_XPLATSTR("FILENAME_ILLEGAL_ON_YOUR_SYSTEM"),
                               _XPLATSTR("File name illegal on your system"));
                return false;
            }
            auto suc = this->parse_remote_directory_sync(single_file, std::make_shared<std::filesystem::path>(cur_path),
                                                         cancellation_token);
            if (!suc) {
                return false;
            }
        }
    }
    return true;
}

void transfer_item::on_error(const utility::string_t &error_referer, const utility::string_t &error_message) {
    std::cout << "on error: " << error_referer.c_str() << std::endl;
    std::lock_guard<std::mutex> _error(this->mutex);
    this->_failed_count.fetch_add(1);
    if (this->_failed_count > MAX_RETRY_TIME) {
        this->_status = transfer_status::fatal_error;
    } else {
        this->_status = transfer_status::error;
    }
}

void transfer_item::parse_download_task_sync(const pplx::cancellation_token &cancellation_token) {
    if (!this->_parsed) {
        if (this->remote_file()->directory()) {
            // Dir, need to parse
            this->_tasks.emplace_back(
                    single_file_task::create(this->remote_file(), this->local_parent_path(), this->direction()));
            // loop
            _parsed = this->parse_remote_directory_sync(this->remote_file(), this->local_parent_path(),
                                                        cancellation_token);
        } else {

            // Add file,
            this->_tasks.emplace_back(
                    single_file_task::create(this->remote_file(), this->local_parent_path(), this->direction()));
            _parsed = true;
        }
    }
    if (!_parsed) {
        return;
    }

    if (cancellation_token.is_canceled()) {
        return;
    }

    std::cout << _XPLATSTR("Starting task") << this->_tasks.size() << std::endl;
    this->mutex.lock();
    this->_status = transfer_status::transfer;
    this->mutex.unlock();
}

