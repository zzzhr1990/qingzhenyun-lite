//
// Created by herui on 2019/12/12.
//

#ifndef QINGZHENYUN_LITE_SINGLE_FILE_TASK_H
#define QINGZHENYUN_LITE_SINGLE_FILE_TASK_H

#include <qingzhen/properity.hpp>
#include <qingzhen/api/file.h>
#include <filesystem>
#include <qingzhen/transfer/transfer_entity.h>
#include <atomic>
#include <qingzhen/transfer/base_file_task.hpp>
#include <qingzhen/transfer/transfer_progress.h>


namespace qingzhen::transfer {
    class single_file_task : public base_file_task<single_file_task> {
    public:
        qingzhen::property<std::shared_ptr<qingzhen::api::file>> remote_file;
        qingzhen::property<std::shared_ptr<std::filesystem::path>> local_parent_path;
        // std::atomic_int64_t progress_bytes_completed;
        // std::atomic_int64_t download_bytes_completed;
        std::unique_ptr<qingzhen::transfer::transfer_progress> progress;


        static std::shared_ptr<single_file_task> create(const std::shared_ptr<qingzhen::api::file> &remote_source_file,
                                                        const std::shared_ptr<std::filesystem::path> &local_parent,
                                                        transfer_direction tr_direction, int64_t size);

        single_file_task(const single_file_task &) = delete;

        single_file_task(const single_file_task &&) = delete;

        void refresh_progress() override;

        void parse_task_sync(const pplx::cancellation_token &cancellation_token) override;

    private:
        single_file_task(const std::shared_ptr<qingzhen::api::file> &remote_source_file,
                         const std::shared_ptr<std::filesystem::path> &local_parent, transfer_direction tr_direction,
                         int64_t size);

    };
}

#endif //QINGZHENYUN_LITE_SINGLE_FILE_TASK_H
