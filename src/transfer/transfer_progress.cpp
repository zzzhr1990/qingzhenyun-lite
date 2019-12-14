//
// Created by herui on 2019/12/13.
//

#include <qingzhen/transfer/transfer_progress.h>
#include <qingzhen/transfer/part_progress_info.h>

using namespace qingzhen::transfer;

bool transfer_progress::parsed() {
    return _parsed.load();
}

std::unique_ptr<transfer_progress> qingzhen::transfer::transfer_progress::create(transfer_direction direction) {
    return std::unique_ptr<transfer_progress>(new transfer_progress(direction));
}

transfer_progress::transfer_progress(transfer_direction direction) : _parsed(false),
                                                                     _direction(direction),
                                                                     hash(utility::string_t()),
                                                                     size(0),
        // upload(false),
                                                                     _list(std::make_shared<std::vector<std::unique_ptr<part_progress_info>>>()) {
}

void transfer_progress::add_part_progress(std::unique_ptr<part_progress_info> info) {
    this->_list->push_back(std::move(info));
}

bool transfer_progress::reset() {
    this->_list->clear();
    this->_parsed.store(false);
    return false;
}

std::shared_ptr<std::vector<std::unique_ptr<part_progress_info>>> transfer_progress::list() {
    return this->_list;
}

void transfer_progress::load_from_file(const std::filesystem::path &file_path) {
    std::lock_guard<std::mutex> _op(this->mutex);
    utility::ifstream_t ist;
    try {
        ist.open(file_path);
        if (ist.is_open()) {
            utility::stringstream_t buffer;
            buffer << ist.rdbuf();
            web::json::value v1 = web::json::value::parse(buffer);
            if (!this->_from_json(v1)) {
                this->reset();
            } else {
                this->_parsed = true;
            }
        } else {
            this->reset();
        }
    } catch (const std::exception &ex) {
        std::cout << "Read failed" << std::endl;
        this->reset();
    }

    if (ist.is_open()) {
        ist.close();
    }
}

bool transfer_progress::_from_json(const web::json::value &value) {
    // read
    if (value.has_string_field(_XPLATSTR("hash"))) {
        this->hash = value.at(_XPLATSTR("hash")).as_string();
    } else {
        return false;
    }
    if (value.has_number_field(_XPLATSTR("size"))) {
        this->size = value.at(_XPLATSTR("size")).as_number().to_int64();
        return false;
    }

    if (value.has_array_field(_XPLATSTR("list"))) {
        auto arr = value.at(_XPLATSTR("list")).as_array();
        this->_list->clear();
        // for to add
        size_t idx = 0;
        for (const auto &v : arr) {

            int64_t start_index = 0;
            int64_t end_index = 0;
            size_t part_id = 0;
            int64_t processed_index = 0;
            // auto p = std::shared_ptr<part_progress>( new part_progress());
            if (v.has_number_field(_XPLATSTR("start_index"))) {
                start_index = v.at(_XPLATSTR("start_index")).as_number().to_int64();
            } else {
                return false;
            }
            if (v.has_number_field(_XPLATSTR("end_index"))) {
                end_index = v.at(_XPLATSTR("end_index")).as_number().to_int64();
            } else {
                return false;
            }
            if (v.has_number_field(_XPLATSTR("part_id"))) {
                part_id = v.at(_XPLATSTR("part_id")).as_number().to_int64();
            } else {
                return false;
            }
            if (part_id != idx) {
                std::cout << part_id << " : " << idx << " dis con" << std::endl;
                return false;
            }
            if (v.has_number_field(_XPLATSTR("processed_index"))) {
                processed_index = v.at(_XPLATSTR("processed_index")).as_number().to_int64();
            } else {
                return false;
            }
            this->add_part_progress(part_progress_info::create(start_index, end_index, part_id, processed_index));
            idx++;
        }
    } else {
        return false;
    }

    // for list




    return true;
}

void transfer_progress::dump_to_file(const std::filesystem::path &file_path) {
    std::lock_guard<std::mutex> _op(this->mutex);
}

void transfer_progress::set_list(const std::shared_ptr<std::vector<std::unique_ptr<part_progress_info>>> &new_list) {
    this->_list = new_list;
}

void transfer_progress::set_parsed(bool new_parsed) {
    this->_parsed.store(new_parsed);
}



