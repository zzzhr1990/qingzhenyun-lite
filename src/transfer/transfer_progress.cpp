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
                                                                     _part_support(true),
                                                                     _path(),
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
	this->reset();
    this->_path = std::filesystem::path(file_path);
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

web::json::value transfer_progress::_to_json() {
    web::json::value v;
    auto arr = web::json::value::array();
    for (auto &p : *this->_list) {
        web::json::value item;
        auto index = p->part_id();
        item[_XPLATSTR("start_index")] = web::json::value(p->start_index());
        item[_XPLATSTR("part_id")] = web::json::value(static_cast<int>(index));
        item[_XPLATSTR("processed_index")] = web::json::value(p->processed_index->load());
        item[_XPLATSTR("end_index")] = web::json::value(p->end_index());
        arr[index] = item;
    }
    v[_XPLATSTR("list")] = arr;
    v[_XPLATSTR("size")] = web::json::value(this->size());
    v[_XPLATSTR("hash")] = web::json::value(this->hash());
    return v;
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
    } else {
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

void transfer_progress::dump_to_file() {
    std::lock_guard<std::mutex> _op(this->mutex);
    if (this->_path.empty()) {
        return;
    }
    // fix
    utility::ofstream_t ost;
    try {
        ost.open(_path, std::ios_base::out | std::ios_base::trunc);
        if (ost.is_open()) {
            // web::json::value v1 ;
            ost << this->_to_json().serialize();
        }
    } catch (const std::exception &ex) {
        std::cout << "Read failed" << std::endl;
    }

    if (ost.is_open()) {
        ost.close();
    }
}

void transfer_progress::set_list(const std::shared_ptr<std::vector<std::unique_ptr<part_progress_info>>> &new_list) {
    this->_list = new_list;
}

void transfer_progress::set_parsed(bool new_parsed) {
    this->_parsed.store(new_parsed);
}

void transfer_progress::set_not_support_range(int64_t file_size) {
    std::lock_guard<std::mutex> _op(this->mutex);
    // copy on write
    auto new_list = std::make_shared<std::vector<std::unique_ptr<part_progress_info>>>();
    auto info = part_progress_info::create(0, file_size - 1, 0, 0);
    _part_support.store(false);
    info->support_part = false;
    new_list->emplace_back(std::move(info));
    this->set_list(new_list);
}

void transfer_progress::clean() {
    std::lock_guard<std::mutex> _op(this->mutex);
    if (this->_path.empty()) {
        return;
    }
    try {
        std::filesystem::remove_all(_path);
    } catch (...) {}

    _path = std::filesystem::path();
}



