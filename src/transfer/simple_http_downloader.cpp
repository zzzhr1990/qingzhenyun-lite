//
// Created by herui on 2019/12/13.
//

#include <qingzhen/transfer/simple_http_downloader.h>
#include <qingzhen/path_util.h>
#include <boost/log/trivial.hpp>
#include <qingzhen/api/user_file_client.hpp>

using namespace qingzhen::transfer;

const int MAX_THREAD = 5;

const int MAX_ERROR_COUNT = 5;

std::shared_ptr<simple_http_downloader> &simple_http_downloader::instance() {
    static std::shared_ptr<simple_http_downloader> _ins = std::shared_ptr<simple_http_downloader>(
            new simple_http_downloader());
    return _ins;
}

std::shared_ptr<simple_http_downloader> simple_http_downloader::get_ptr() {
    return this->shared_from_this();
}

void simple_http_downloader::start(const std::shared_ptr<single_file_task> &task,
                                   const pplx::cancellation_token &cancellation_token) {
    // pre checking
    auto &file = task->remote_file();
    if (file == nullptr) {
        task->on_error_lock(_XPLATSTR("TASK_INFO_EMPTY"), _XPLATSTR("Task info cannot be empty"), MAX_ERROR_COUNT);
        return;
    }
    // parse
    auto file_path = std::filesystem::path(*task->local_parent_path());
    try {
        file_path.append(task->remote_file()->name());
    } catch (std::exception &) {
        task->on_error_lock(_XPLATSTR("FILENAME_ILLEGAL_ON_YOUR_SYSTEM"),
                            _XPLATSTR("File name illegal on your system"), MAX_ERROR_COUNT);
        return;
    }
    if (file->directory()) {


        auto create = path_util::ensure_directory(file_path);
        if (create) {
            task->complete_with_lock();
            return;
        } else {
            task->on_error_lock(_XPLATSTR("CANNOT_CREATE_DIRECTORY"), _XPLATSTR("Cannot create directory"),
                                MAX_ERROR_COUNT);
            return;
        }
    }
    if (!path_util::ensure_directory(*task->local_parent_path())) {
        task->on_error_lock(_XPLATSTR("CANNOT_CREATE_DIRECTORY"), _XPLATSTR("Cannot create directory"),
                            MAX_ERROR_COUNT);
        return;
    }
    auto recover_file_path = std::filesystem::path(*task->local_parent_path());
    try {
        recover_file_path.append(task->remote_file()->name() + _XPLATSTR(".qz.d"));
    } catch (std::exception &) {
        task->on_error_lock(_XPLATSTR("FILENAME_ILLEGAL_ON_YOUR_SYSTEM"),
                            _XPLATSTR("File name illegal on your system"), MAX_ERROR_COUNT);
        return;
    }
    // we got file
	BOOST_LOG_TRIVIAL(info) << _XPLATSTR("Download file: ") << task->remote_file()->path().c_str() << _XPLATSTR(" to: =>  ") << file_path.c_str();

    // split file
    if (cancellation_token.is_canceled()) {
        // just cancelled
        return;
    }

    // check if exists
    // if(task->remote_file)
    try {
        // check file exists
        if (std::filesystem::is_regular_file(file_path)) {
            task->success_with_lock();

            return;
        }
    } catch (...) {
        return;
    }

    // need download, load new file
    task->progress->load_from_file(recover_file_path);
    if (cancellation_token.is_canceled()) {
        return;
    }
    // split file
    auto max_thread = MAX_THREAD;
    if (max_thread < 1) {
        max_thread = 1;
    }
    if (max_thread > 10) {
        max_thread = 10;
    }

    int64_t file_size = task->remote_file()->size();
    const int64_t MIN_BLOCK_SIZE = 10485760;
    int64_t test_thread = file_size / MIN_BLOCK_SIZE + (file_size % MIN_BLOCK_SIZE >= MIN_BLOCK_SIZE / 4 ? 1 : 0);
    if (test_thread >= max_thread) {
        test_thread = max_thread; // limit to max thread, calc
    }

    if (test_thread < 1) {
        test_thread = 1;
    }

    int64_t block_size = file_size / test_thread;

    // split files
    std::shared_ptr<std::vector<std::unique_ptr<part_progress_info>>> test_queue = std::make_shared<std::vector<std::unique_ptr<part_progress_info>>>();

    if (test_thread < 2) {
        auto single_task = part_progress_info::create(0, file_size - 1, 0, 0);
        test_queue->push_back(std::move(single_task));
    } else {
        size_t thread_id = 0;
        int64_t start_index = 0;

        while (true) {
            bool need_break = false;
            int64_t test_end_index = start_index + block_size;
            if (test_end_index >= file_size || thread_id == test_thread - 1) {
                test_end_index = file_size;
                need_break = true;
            }
            auto single_task = part_progress_info::create(start_index, test_end_index - 1, thread_id, start_index);
            test_queue->push_back(std::move(single_task));
            if (need_break) {
                break;
            }
            start_index = test_end_index;
            thread_id++;
        }
    }

    // comp

    if (!task->progress->parsed() || task->progress->size() != file_size ||
        task->progress->list()->size() != test_queue->size()) {
        task->progress->set_list(test_queue);
        task->progress->size = file_size;
        task->progress->hash = task->remote_file()->hash();
        task->progress->set_parsed(true);
        task->progress->dump_to_file(recover_file_path);
    } else {
        bool confirm = true;
        size_t current_idx = 0;
        // for and compare...
        for (auto &s : *task->progress->list()) {
            auto &tst = test_queue->at(current_idx);
            if (!confirm || s->start_index() != tst->start_index() || tst->end_index() != s->end_index()) {
                confirm = false;
            }
            if (!confirm) {
                s->start_index = tst->start_index();
                s->end_index = tst->end_index();
                s->processed_index->store(tst->start_index());
            } else {
                if (s->processed_index->load() > s->end_index() + 1) {
                    s->processed_index->store(s->start_index());
                }
                confirm = false;
            }
        }

        if (!confirm) {
            task->progress->dump_to_file(recover_file_path);
        }
    }
    if (cancellation_token.is_canceled()) {
        return;
    }

    utility::stringstream_t st;
    st << _XPLATSTR("File: ") << task->remote_file()->name() << _XPLATSTR(" size: ") << file_size
       << _XPLATSTR("\r\n");
    for (auto &s : *task->progress->list()) {
        st << _XPLATSTR("Part: ") << s->part_id(

        ) << _XPLATSTR(" range: ") << s->start_index() << _XPLATSTR(" => ")
           << s->end_index();
    }
    BOOST_LOG_TRIVIAL(info) << st.str().c_str();
    // Okay. >>>>====>>
    /*
    BOOST_LOG_TRIVIAL(trace) << "A trace severity message";
    BOOST_LOG_TRIVIAL(debug) << "A debug severity message";
    BOOST_LOG_TRIVIAL(info) << "An informational severity message";
    BOOST_LOG_TRIVIAL(warning) << "A warning severity message";
    BOOST_LOG_TRIVIAL(error) << "An error severity message";
    BOOST_LOG_TRIVIAL(fatal) << "A fatal severity message";
     */

    // start download...
    qingzhen::api::file file_request;
    file_request.identity = task->remote_file()->identity();
    auto download_info = qingzhen::api::user_file_client::get_download_info(file_request, cancellation_token).get();
    if (download_info.cancel()) {
        return;
    }
    if (!download_info.success()) {
        task->on_error_lock(download_info.error_ref(), download_info.error_message(), MAX_ERROR_COUNT);
        return;
    }
    //

}

