//
// Created by herui on 2019/12/13.
//

#ifndef QINGZHENYUN_LITE_TRANSFER_PROGRESS_H
#define QINGZHENYUN_LITE_TRANSFER_PROGRESS_H

#include <qingzhen/properity.hpp>
#include <memory>
#include <filesystem>
#include <qingzhen/transfer/transfer_entity.h>
#include <qingzhen/transfer/part_progress_info.h>
#include <cpprest/json.h>


namespace qingzhen::transfer {
    class transfer_progress {
    public:
        static std::unique_ptr<transfer_progress> create(transfer_direction direction);

        void load_from_file(const std::filesystem::path &file_path);

        void dump_to_file(const std::filesystem::path &file_path);

        void add_part_progress(std::unique_ptr<part_progress_info> info);

        bool parsed();

        void set_parsed(bool new_parsed);

        bool reset();

        std::shared_ptr<std::vector<std::unique_ptr<part_progress_info>>> list();

        void set_list(const std::shared_ptr<std::vector<std::unique_ptr<part_progress_info>>> &new_list);

        qingzhen::property<utility::string_t> hash;
        qingzhen::property<int64_t> size;
        // qingzhen::property<bool > upload;
    private:
        explicit transfer_progress(transfer_direction direction);

        std::atomic_bool _parsed;
        transfer_direction _direction;
        std::shared_ptr<std::vector<std::unique_ptr<part_progress_info>>> _list;
        std::mutex mutex;

        bool _from_json(const web::json::value &value);
    };
}
#endif //QINGZHENYUN_LITE_TRANSFER_PROGRESS_H
