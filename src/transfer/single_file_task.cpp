//
// Created by herui on 2019/12/12.
//

#include <qingzhen/transfer/single_file_task.h>
#include <qingzhen/transfer/simple_http_downloader.h>

using namespace qingzhen::transfer;

single_file_task::single_file_task(const std::shared_ptr<qingzhen::api::file> &remote_source_file,
                                   const std::shared_ptr<std::filesystem::path> &local_parent,
                                   transfer_direction tr_direction, int64_t size) : remote_file(remote_source_file),
                                                                                    local_parent_path(local_parent),
                                                                                    progress(std::move(
                                                                                            transfer_progress::create(
                                                                                                    tr_direction))),
                                                                                    base_file_task<single_file_task>(
                                                                                            tr_direction, size) {

}


std::shared_ptr<single_file_task>
single_file_task::create(const std::shared_ptr<qingzhen::api::file> &remote_source_file,
                         const std::shared_ptr<std::filesystem::path> &local_parent, transfer_direction tr_direction,
                         int64_t size) {
    return std::shared_ptr<single_file_task>(
            new single_file_task(remote_source_file, local_parent, tr_direction, size));
}

void single_file_task::parse_task_sync(const pplx::cancellation_token &cancellation_token) {
    // start task
    // this->update_status_with_lock(transfer_status::fatal_error);
    if (this->direction() == transfer_direction::download) {
        this->update_status_with_lock(transfer_status::pre_checking);
        simple_http_downloader::instance()->start(this->get_ptr(), cancellation_token);
    } else {
        this->on_error_lock(_XPLATSTR("UN_SUPPORT_DIRECTION"), _XPLATSTR("Un support direction"), 1);
    }
}

void single_file_task::refresh_progress() {
    // How?
    if (this->_status != transfer_status::transfer) {
        return;
    }
    int64_t proc = 0;
    for (auto &cs : *this->progress->list()) {
        // cs->processed_index
        auto fz = cs->processed_index->load() - cs->start_index();
        proc += fz;
    }
    this->_processed_size.store(proc);
}

