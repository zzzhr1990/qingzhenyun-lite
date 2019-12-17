//
// Created by herui on 2019/12/11.
//

#include <qingzhen/transfer/transfer_item.h>
#include <qingzhen/api/user_file_client.hpp>
#include <boost/log/trivial.hpp>
#include <qingzhen/string_util.h>

using namespace qingzhen::transfer;

const int MAX_RETRY_TIME = 5;


transfer_item::transfer_item(const std::shared_ptr<qingzhen::api::file> &file,
                             const std::shared_ptr<std::filesystem::path> &dest,
                             qingzhen::transfer::transfer_direction dir, int64_t size) : remote_file(file),
                                                                                         local_parent_path(dest),
                                                                                         _parsed(false),
                                                                                         base_file_task<transfer_item>(
                                                                                                 dir, size) {

}

std::shared_ptr<transfer_item>
transfer_item::create(const std::shared_ptr<qingzhen::api::file> &file, const std::filesystem::path &dest,
                      qingzhen::transfer::transfer_direction dir, int64_t size) {
    return std::shared_ptr<transfer_item>(
            new transfer_item(file, std::make_shared<std::filesystem::path>(dest), dir, size));
}



void transfer_item::parse_task_sync(const pplx::cancellation_token &cancellation_token) {
    this->_tasks.clear();
    // status should be pre checking
    if (this->direction() == transfer_direction::download) {
        this->parse_download_task_sync(cancellation_token);
        this->update_status_with_lock(transfer_status::transfer);
    } else {
        std::cout << _XPLATSTR("Not support") << std::endl;
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
        this->on_error_lock(result.error_ref(), result.error_message(), MAX_RETRY_TIME);
        return false;
    }
    for (auto &single_file : result.data()->list()) {
        // add self first
        this->_tasks.emplace_back(
                single_file_task::create(single_file, parent_path, this->direction(), single_file->size()));
        if (single_file->directory()) {
            //

            std::filesystem::path cur_path = std::filesystem::path(*parent_path);
            try {
                cur_path.append(single_file->name());
            } catch (const std::exception &) {
                this->on_error_lock(_XPLATSTR("FILENAME_ILLEGAL_ON_YOUR_SYSTEM"),
                                    _XPLATSTR("File name illegal on your system"), MAX_RETRY_TIME);
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


void transfer_item::parse_download_task_sync(const pplx::cancellation_token &cancellation_token) {
    if (!this->_parsed) {
        if (this->remote_file()->directory()) {
            // Dir, need to parse
            this->_tasks.emplace_back(
                    single_file_task::create(this->remote_file(), this->local_parent_path(), this->direction(),
                                             this->remote_file()->size()));
            // loop
            std::filesystem::path cur_path = std::filesystem::path(*this->local_parent_path());
            try {
                cur_path.append(this->remote_file()->name());
            } catch (const std::exception &) {
                this->on_error_lock(_XPLATSTR("FILENAME_ILLEGAL_ON_YOUR_SYSTEM"),
                                    _XPLATSTR("File name illegal on your system"), MAX_RETRY_TIME);
                return;
            }
            _parsed = this->parse_remote_directory_sync(this->remote_file(),
                                                        std::make_shared<std::filesystem::path>(cur_path),
                                                        cancellation_token);
        } else {

            // Add file,
            this->_tasks.emplace_back(
                    single_file_task::create(this->remote_file(), this->local_parent_path(), this->direction(),
                                             this->remote_file()->size()));
            _parsed = true;
        }
    }
    if (!_parsed) {
        return;
    }

    if (cancellation_token.is_canceled()) {
        return;
    }



}

void transfer_item::_tick() {
    const int MAX_CONCURRENT_FILE = 1;
    size_t success = 0;
    // bool all_success = true;
    size_t paused_complete_state = 0;
    size_t error_state = 0;
    if (this->_status != transfer_status::transfer) {
        return;
    }
    auto tk = this->cancellation_token_source.get_token();
    size_t count = 0;
    for (auto &sig_task : this->_tasks) {
        if (tk.is_canceled()) {
            return;
        }
        if (success < MAX_CONCURRENT_FILE) {
            auto cont = sig_task->start(tk);
            if (cont.first) {
                success++;
                // dbo = false;
            }
        }


        // if (true) {
        sig_task->refresh_progress();
        auto task_status = sig_task->get_status_with_lock();
        if (task_status == transfer_status::complete || task_status == transfer_status::pause) {
            paused_complete_state++;
        } else if (task_status == transfer_status::fatal_error) {
            error_state++;
        }
        if (task_status == transfer_status::transfer) {
            sig_task->refresh_byte_counter();
            sig_task->progress->dump_to_file();
            int64_t bytes_down = 0;
            int64_t bytes_total = 0;
            int64_t progress = 0;
            std::tie(bytes_down, bytes_total, progress) = sig_task->get_current_progress();
            std::cout << "File: " << sig_task->remote_file()->name() << " progress: " << progress << " speed: "
                      << qingzhen::string_util::pretty_bytes(sig_task->get_last_speed()) << std::endl;
        }
        // }
        count++;
    }

    // if(success == 0)

    if (paused_complete_state >= count) {
        this->_status = transfer_status::complete;
        BOOST_LOG_TRIVIAL(info) << _XPLATSTR("task complete: ") << this->remote_file()->name().c_str();
        return;
    } else if (paused_complete_state + error_state >= count) {
        BOOST_LOG_TRIVIAL(info) << _XPLATSTR("task error: ") << this->remote_file()->name().c_str();
        this->_status = transfer_status::fatal_error;
    }
}

void transfer_item::refresh_progress() {

}


