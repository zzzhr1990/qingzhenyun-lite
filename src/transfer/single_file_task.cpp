//
// Created by herui on 2019/12/12.
//

#include <qingzhen/transfer/single_file_task.h>
#include <qingzhen/transfer/simple_http_downloader.h>

using namespace qingzhen::transfer;

single_file_task::single_file_task(const std::shared_ptr<qingzhen::api::file> &remote_source_file,
                                   const std::shared_ptr<std::filesystem::path> &local_parent,
                                   transfer_direction tr_direction) : remote_file(remote_source_file),
                                                                      local_parent_path(local_parent),
                                                                      progress_bytes_completed(0),
                                                                      download_bytes_completed(0),
                                                                      progress(std::move(
                                                                              transfer_progress::create(tr_direction))),
                                                                      base_file_task<single_file_task>(tr_direction) {

}


std::shared_ptr<single_file_task>
single_file_task::create(const std::shared_ptr<qingzhen::api::file> &remote_source_file,
                         const std::shared_ptr<std::filesystem::path> &local_parent, transfer_direction tr_direction) {
    return std::shared_ptr<single_file_task>(new single_file_task(remote_source_file, local_parent, tr_direction));
}

void single_file_task::parse_task_sync(const pplx::cancellation_token &cancellation_token) {
    // start task
    // std::cout << "single file complete: " << this->remote_file()->path() << std::endl;
    // this->update_status_with_lock(transfer_status::fatal_error);
    if (this->direction() == transfer_direction::download) {
        simple_http_downloader::instance()->start(this->get_ptr(), cancellation_token);
    } else {
        this->on_error_lock(_XPLATSTR("UN_SUPPORT_DIRECTION"), _XPLATSTR("Un support direction"), 1);
    }
}

