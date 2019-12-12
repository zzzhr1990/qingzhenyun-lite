//
// Created by herui on 2019/12/12.
//

#include <qingzhen/transfer/single_file_task.h>

using namespace qingzhen::transfer;

single_file_task::single_file_task(const std::shared_ptr<qingzhen::api::file> &remote_source_file,
                                   const std::shared_ptr<std::filesystem::path> &local_parent,
                                   transfer_direction tr_direction) : remote_file(remote_source_file),
                                                                      local_parent_path(local_parent),
                                                                      progress_bytes_completed(0),
                                                                      download_bytes_completed(0),
                                                                      _status(transfer_status::add),
                                                                      direction(tr_direction) {

}

transfer_status single_file_task::status() {
    return _status;
}

void single_file_task::set_selected(bool selected) {
    this->_selected = selected;
}

std::shared_ptr<single_file_task> single_file_task::get_ptr() {
    return shared_from_this();
}

std::shared_ptr<single_file_task>
single_file_task::create(const std::shared_ptr<qingzhen::api::file> &remote_source_file,
                         const std::shared_ptr<std::filesystem::path> &local_parent, transfer_direction tr_direction) {
    std::cout << _XPLATSTR("Create task: ") << local_parent->c_str() << _XPLATSTR("/")
              << remote_source_file->name().c_str() << std::endl;
    return std::shared_ptr<single_file_task>(new single_file_task(remote_source_file, local_parent, tr_direction));
}
