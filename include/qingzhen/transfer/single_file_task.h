//
// Created by herui on 2019/12/12.
//

#ifndef QINGZHENYUN_LITE_SINGLE_FILE_TASK_H
#define QINGZHENYUN_LITE_SINGLE_FILE_TASK_H

#include <qingzhen/properity.hpp>
#include <qingzhen/api/file.h>
#include <filesystem>
#include <qingzhen/transfer/transfer_entity.h>

namespace qingzhen::transfer {
    class single_file_task : public std::enable_shared_from_this<single_file_task> {
    public:
        qingzhen::property<std::shared_ptr<qingzhen::api::file>> remote_file;
        qingzhen::property<std::shared_ptr<std::filesystem::path>> local_parent_path;
        qingzhen::property<transfer_direction> direction;
        std::atomic_int64_t progress_bytes_completed;
        std::atomic_int64_t download_bytes_completed;

        transfer_status status();

        void set_selected(bool selected);

        std::shared_ptr<single_file_task> get_ptr();

        static std::shared_ptr<single_file_task> create(const std::shared_ptr<qingzhen::api::file> &remote_source_file,
                                                        const std::shared_ptr<std::filesystem::path> &local_parent,
                                                        transfer_direction tr_direction);

    private:
        single_file_task(const std::shared_ptr<qingzhen::api::file> &remote_source_file,
                         const std::shared_ptr<std::filesystem::path> &local_parent, transfer_direction tr_direction);

        transfer_status _status;
        std::atomic_bool _selected = true;
    };
}

#endif //QINGZHENYUN_LITE_SINGLE_FILE_TASK_H
