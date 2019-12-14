//
// Created by herui on 2019/12/13.
//

#ifndef QINGZHENYUN_LITE_PART_PROGRESS_INFO_H
#define QINGZHENYUN_LITE_PART_PROGRESS_INFO_H

#include <qingzhen/properity.hpp>

namespace qingzhen::transfer {
    class part_progress_info {
    public:
        qingzhen::property<int64_t> start_index;
        qingzhen::property<int64_t> end_index;
        qingzhen::property<size_t> part_id;
        std::unique_ptr<std::atomic_int64_t> processed_index;

        static std::unique_ptr<part_progress_info>
        create(int64_t start_index, int64_t end_index, size_t part_id, int64_t processed_index) {
            return std::unique_ptr<part_progress_info>(
                    new part_progress_info(start_index, end_index, part_id, processed_index));
        }

    private:
        part_progress_info(int64_t start_index, int64_t end_index, size_t part_id, int64_t processed_index) :
                start_index(start_index),
                end_index(end_index),
                part_id(part_id),
                processed_index(std::make_unique<std::atomic_int64_t>(processed_index)) {
            //this->processed_index->store(processed_index);
        };
    };


}
#endif //QINGZHENYUN_LITE_PART_PROGRESS_INFO_H
