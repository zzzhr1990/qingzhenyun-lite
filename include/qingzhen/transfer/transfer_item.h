//
// Created by herui on 2019/12/10.
//

#ifndef QINGZHENYUN_LITE_TRANSFER_ITEM_H
#define QINGZHENYUN_LITE_TRANSFER_ITEM_H

#include <qingzhen/properity.hpp>
#include <qingzhen/api/file.h>
#include <qingzhen/transfer/single_file_task.h>
#include <qingzhen/transfer/transfer_entity.h>


namespace qingzhen::transfer {
    // typedef std::tuple<int, double, int, double> item_start_result;
    class transfer_item : public base_file_task<transfer_item> {
    public:
        qingzhen::property<std::shared_ptr<qingzhen::api::file>> remote_file;
        qingzhen::property<std::shared_ptr<std::filesystem::path>> local_parent_path;


        void parse_task_sync(const pplx::cancellation_token &cancellation_token) override;

        bool parse_remote_directory_sync(const std::shared_ptr<qingzhen::api::file> &file,
                                         const std::shared_ptr<std::filesystem::path> &parent_path,
                                         const pplx::cancellation_token &cancellation_token);

        static std::shared_ptr<transfer_item>
        create(const std::shared_ptr<qingzhen::api::file> &file, const std::filesystem::path &dest,
               qingzhen::transfer::transfer_direction dir);

    private:
        transfer_item(const std::shared_ptr<qingzhen::api::file> &file,
                      const std::shared_ptr<std::filesystem::path> &dest, qingzhen::transfer::transfer_direction dir);

        void _tick() override;

        std::atomic_bool _parsed = false;
        //
        std::vector<std::shared_ptr<single_file_task>> _tasks;

        void parse_download_task_sync(const pplx::cancellation_token &cancellation_token);

        // std::

    };


}
#endif //QINGZHENYUN_LITE_TRANSFER_ITEM_H
