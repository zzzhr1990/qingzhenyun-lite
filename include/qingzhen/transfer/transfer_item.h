//
// Created by herui on 2019/12/10.
//

#ifndef QINGZHENYUN_LITE_TRANSFER_ITEM_H
#define QINGZHENYUN_LITE_TRANSFER_ITEM_H

#include <qingzhen/properity.hpp>
#include <qingzhen/api/file.h>
#include <qingzhen/transfer/single_file_task.h>

#include "transfer_entity.h"

namespace qingzhen::transfer {
    // typedef std::tuple<int, double, int, double> item_start_result;
    class transfer_item : public std::enable_shared_from_this<transfer_item> {
    public:
        qingzhen::property<std::shared_ptr<qingzhen::api::file>> remote_file;
        qingzhen::property<std::shared_ptr<std::filesystem::path>> local_parent_path;
        qingzhen::property<qingzhen::transfer::transfer_direction> direction;

        bool start(pplx::cancellation_token token);

        void parse_task_sync(const pplx::cancellation_token &cancellation_token);

        bool parse_remote_directory_sync(const std::shared_ptr<qingzhen::api::file> &file,
                                         const std::shared_ptr<std::filesystem::path> &parent_path,
                                         const pplx::cancellation_token &cancellation_token);

        std::shared_ptr<transfer_item> get_ptr();

        static std::shared_ptr<transfer_item>
        create(const std::shared_ptr<qingzhen::api::file> &file, const std::filesystem::path &dest,
               qingzhen::transfer::transfer_direction dir);

    private:
        transfer_item(const std::shared_ptr<qingzhen::api::file> &file,
                      const std::shared_ptr<std::filesystem::path> &dest, qingzhen::transfer::transfer_direction dir);

        pplx::cancellation_token_source cancellation_token_source;
        std::mutex mutex;
        std::atomic_bool _select = true;
        std::atomic_bool _parsed = false;
        transfer_status _status;
        std::atomic_int _failed_count;
        std::vector<std::shared_ptr<single_file_task>> _tasks;

        void parse_download_task_sync(const pplx::cancellation_token &cancellation_token);

        // std::
        void on_error(const utility::string_t &error_referer, const utility::string_t &error_message);
    };


}
#endif //QINGZHENYUN_LITE_TRANSFER_ITEM_H
