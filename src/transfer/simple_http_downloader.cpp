//
// Created by herui on 2019/12/13.
//

#include <qingzhen/transfer/simple_http_downloader.h>
#include <qingzhen/transfer/file_hash_container.h>
#include <qingzhen/path_util.h>
#include <qingzhen/string_util.h>
#include <boost/log/trivial.hpp>
#include <qingzhen/api/user_file_client.hpp>
#include <qingzhen/transfer/basic_file_buf.h>
#ifdef _WIN32
#include <Windows.h>
#include <winhttp.h>
#endif // _WIN32


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
    auto file_temp_path = std::filesystem::path(*task->local_parent_path());
    try {
        file_temp_path.append(task->remote_file()->name() + _XPLATSTR(".qz.dt"));
    } catch (std::exception &) {
        task->on_error_lock(_XPLATSTR("FILENAME_ILLEGAL_ON_YOUR_SYSTEM"),
                            _XPLATSTR("File name illegal on your system"), MAX_ERROR_COUNT);
        return;
    }
    // we got file

	BOOST_LOG_TRIVIAL(info) << ("Download file: ") << qingzhen::string_util::string_t_to_ansi(task->remote_file()->path())
			<< " to: =>  " << qingzhen::string_util::string_t_to_ansi(file_path);

   

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
            qingzhen::path_util::clean_path(file_temp_path);
            qingzhen::path_util::clean_path(recover_file_path);
            return;
        }
		if (std::filesystem::exists(file_temp_path) ){
			auto fsize = std::filesystem::file_size(file_temp_path);
			if (fsize > task->remote_file()->size()) {
				qingzhen::path_util::clean_path(file_temp_path);
				qingzhen::path_util::clean_path(recover_file_path);
			} else if(!std::filesystem::exists(recover_file_path)){
				file_hash_container fckc(file_temp_path);
				auto check_res = fckc.hash_file(cancellation_token, false);
				if (cancellation_token.is_canceled()) {
					return;
				}
				if (check_res->hash() == task->remote_file()->hash()) {
					if (!qingzhen::path_util::move_file(file_temp_path, file_path)) {
						task->on_error_lock(_XPLATSTR("CANNOT_MOVE_DEST_FILE"), _XPLATSTR("Cannot move file to dest"), MAX_ERROR_COUNT);
						return;
					}
					qingzhen::path_util::clean_path(recover_file_path);
					task->success_with_lock();
					return;
				}
			}
		}
            // std::filesystem::file_size(file_temp_path) > ) {
            
       
    } catch (...) {
        task->on_error_lock(_XPLATSTR("CANNOT_CHECK_FILE"), _XPLATSTR("Cannot check file"), MAX_ERROR_COUNT);
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
        task->progress->dump_to_file();
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
                    confirm = false;
                }
            }
            current_idx++;
        }

        if (!confirm) {
            task->progress->dump_to_file();
        }
    }
    if (cancellation_token.is_canceled()) {
        return;
    }


    //
    // alloc file
   
    if(!qingzhen::path_util::ensure_and_alloc_file(file_temp_path, task->remote_file()->size())){
        task->on_error_lock(_XPLATSTR("CANNOT_CREATE_TEMP_FILE"), _XPLATSTR("Cannot create temp file"), MAX_ERROR_COUNT);
        return;
    }


    if (task->remote_file()->size() < 1) {
		task->progress->clean();
		// move file
		if (!qingzhen::path_util::move_file(file_temp_path, file_path)) {
			task->on_error_lock(_XPLATSTR("CANNOT_MOVE_DEST_FILE"), _XPLATSTR("Cannot move file to dest"), MAX_ERROR_COUNT);
			return;
		}
		task->success_with_lock();
        return;
    }
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

    if (download_info.data()->download_address->empty()) {
        task->on_error_lock(_XPLATSTR("CANNOT_GET_DOWNLOAD_ADDRESS"), _XPLATSTR("Cannot get download address"),
                            MAX_ERROR_COUNT);
        return;
    }
    // LOL, start true download, create tasks
    bool success = false;
    bool can_range = false;
    utility::string_t real_url = utility::string_t();
    std::tie(success, can_range, real_url) = this->check_support_ranges(download_info.data()->download_address(),
                                                                        cancellation_token, 0);
    if (cancellation_token.is_canceled()) {
        return;
    }
    if (!success) {
        task->on_error_lock(_XPLATSTR("CANNOT_PRE_FLIGHT_HEADERS"), _XPLATSTR("Cannot check download info"),
                            MAX_ERROR_COUNT);
        return;
    }
    // if cannot range, delete others req
    if (!can_range) {
        task->progress->set_not_support_range(task->remote_file()->size());
    }
    std::vector<pplx::task<bool >> v;
    task->reset_counter();
    for (auto &s : *task->progress->list()) {
        auto ch = this->get_ptr();
        auto pid = s->part_id();
        auto task_ptr = task->get_ptr();
        v.emplace_back(pplx::create_task([ch, real_url, task_ptr, pid, file_temp_path, cancellation_token]() {
            try {
                return ch->start_download_single_part(real_url, task_ptr, pid, file_temp_path, cancellation_token);
            } catch (const std::exception &ex) {
                if (cancellation_token.is_canceled()) {
                    return false;
                }
                BOOST_LOG_TRIVIAL(error) << "Download part failed: " << ex.what();
            }
            return false;
        }));
    }
    task->update_status_with_lock(transfer_status::transfer);
	bool all_success = true;
    try {
        auto ge = pplx::when_all(v.begin(), v.end()).get();
		for (bool s : ge) {
			if (!s) {
				all_success = false;
			}
		}
		if (!all_success) {
			if (cancellation_token.is_canceled()) {
				return;
			}
			task->on_error_lock(_XPLATSTR("DOWNLOAD_FILE_FAILED"), _XPLATSTR("Download file failed"), MAX_ERROR_COUNT);
			return;
		}
    } catch (const std::exception &ex) {
        if (cancellation_token.is_canceled()) {
            return;
        }
        task->on_error_lock(_XPLATSTR("DOWNLOAD_FILE_FAILED"), _XPLATSTR("Download file failed"), MAX_ERROR_COUNT);
        return;
    }
    if (cancellation_token.is_canceled()) {
        return;
    }

	
    //
    qingzhen::transfer::file_hash_container hash_container(file_temp_path);
    //
    auto hash_result = hash_container.hash_file(cancellation_token, true);
    if (cancellation_token.is_canceled()) {
        return;
    }
    if (hash_result->hash() != task->remote_file()->hash()) {
        task->on_error_lock(_XPLATSTR("HASH_NOT_MATCH"), _XPLATSTR("Hash not match"), MAX_ERROR_COUNT);
        // clean all
        BOOST_LOG_TRIVIAL(error) << "Hash not match: local:" << qingzhen::string_util::string_t_to_ansi(hash_result->hash()) << " remote: "
                                 << qingzhen::string_util::string_t_to_ansi(task->remote_file()->hash()) << " ->" <<
			task->remote_file()->size();
        task->progress->clean();
        qingzhen::path_util::clean_path(file_temp_path);
		qingzhen::path_util::clean_path(recover_file_path);
        return;
    }
    // clean temp file

    task->progress->clean();
    // move file
    if (!qingzhen::path_util::move_file(file_temp_path, file_path)) {
        task->on_error_lock(_XPLATSTR("CANNOT_MOVE_DEST_FILE"), _XPLATSTR("Cannot move file to dest"), MAX_ERROR_COUNT);
		return;
    }
    task->success_with_lock();
    BOOST_LOG_TRIVIAL(info) << "Download file complete: " << qingzhen::string_util::string_t_to_ansi(real_url);
}

std::tuple<bool, bool, utility::string_t> simple_http_downloader::check_support_ranges(utility::string_t url_ori,
                                                                                       pplx::cancellation_token cancellation_token,
                                                                                       int redirect_count) {
    // 1. confirm http status is HTTP 200/206
    web::http::client::http_client_config client_config;
#ifdef _WIN32
    client_config.set_nativehandle_options([&](web::http::client::native_handle nativeHandle) {
        // Disable auto redirects
        DWORD dwOptionValue = WINHTTP_DISABLE_REDIRECTS;
		
        BOOL sts = WinHttpSetOption(nativeHandle, WINHTTP_OPTION_DISABLE_FEATURE, &dwOptionValue, sizeof(dwOptionValue));
        if (!sts) {
            BOOST_LOG_TRIVIAL(error) << "WINHTTP_DISABLE_REDIRECTS failed.\n";
        }
    });
#endif
    if (cancellation_token.is_canceled()) {
        return std::make_tuple(false, false, url_ori);
    }
    const utility::string_t url = std::move(url_ori);
    bool confirmed = false;
    bool can_range = false;
    auto cst = pplx::cancellation_token_source::create_linked_source(cancellation_token);
    try {
        web::http::client::http_client client(url, client_config);
        const web::http::method mtd = web::http::methods::GET;
        web::http::http_request req(mtd);
        auto &headers = req.headers();
        headers.add(web::http::header_names::range, _XPLATSTR("bytes=0-0"));
        headers.add(web::http::header_names::user_agent,
                    _XPLATSTR("Mozilla/5.0 (Windows NT 10.0; Win64; x64) QingzhenyunClient/0.01"));
        auto response = client.request(req, cst.get_token()).get();
        auto code = response.status_code();
        if (code == web::http::status_codes::PartialContent) {
            confirmed = true;
            can_range = true;
            cst.cancel();
            return std::make_tuple(confirmed, can_range, url);
        }
        if (code == web::http::status_codes::OK) {
            confirmed = true;
            cst.cancel();
            return std::make_tuple(confirmed, can_range, url);
        }
        if (code == web::http::status_codes::Found
            || code == web::http::status_codes::PermanentRedirect
            || code == web::http::status_codes::TemporaryRedirect
            || code == web::http::status_codes::SeeOther
            || code == web::http::status_codes::MovedPermanently
                ) {
            redirect_count++;
            if (redirect_count > 99) {
                BOOST_LOG_TRIVIAL(error) << "Redirect too much";
                return std::make_tuple(false, false, url);
            }
            // Location
            return this->check_support_ranges(response.headers()[web::http::header_names::location], cancellation_token,
                                              redirect_count);
        }
        return std::make_tuple(confirmed, can_range, url);
    } catch (const std::exception &ex) {
        BOOST_LOG_TRIVIAL(error) << ex.what();
        return std::make_tuple(confirmed, can_range, url);
    }
}

bool simple_http_downloader::start_download_single_part(const utility::string_t &uri,
                                                        const std::shared_ptr<single_file_task> &task, size_t index,
                                                        const std::filesystem::path &path,
                                                        const pplx::cancellation_token &cancellation_token) {
    auto &single_task = task->progress->list()->at(index);
    auto file_buf = new qingzhen::transfer::basic_file_buf<char>();
    if (!file_buf->open_file(path, single_task->processed_index->load(),
		std::ios::in | std::ios::out | std::ios::binary | std::ios::ate)) {
        BOOST_LOG_TRIVIAL(error) << "Cannot open dest file: " << qingzhen::string_util::string_t_to_ansi(path.c_str());
        return true;
    }

    std::shared_ptr<concurrency::streams::details::basic_streambuf<char>> ptr = std::shared_ptr<concurrency::streams::details::basic_streambuf<char>>(
            file_buf);
    concurrency::streams::streambuf<char> sb(ptr);
    concurrency::streams::basic_ostream<uint8_t> oss(sb);
    // std::ios::out | std::ios::binary
    const web::http::method mtd = web::http::methods::GET;
    web::http::client::http_client_config config;
#ifdef _WIN32
	config.set_nativehandle_options([&](web::http::client::native_handle nativeHandle) {
		// Disable auto redirects
		DWORD dwOptionValue = WINHTTP_DISABLE_REDIRECTS;

		BOOL sts = WinHttpSetOption(nativeHandle, WINHTTP_OPTION_DISABLE_FEATURE, &dwOptionValue, sizeof(dwOptionValue));
		if (!sts) {
			BOOST_LOG_TRIVIAL(error) << "WINHTTP_DISABLE_REDIRECTS failed.\n";
		}
		});
#endif
    config.set_timeout(std::chrono::seconds(30));
    // config.set_chunksize(1024 * 1024);
    web::http::client::http_client client(uri, config);
    web::http::http_request req(mtd);
    auto &headers = req.headers();
    headers.add(web::http::header_names::user_agent,
                _XPLATSTR("Mozilla/5.0 (Windows NT 10.0; Win64; x64) QingzhenyunClient/0.01"));
    if (single_task->support_part()) {
        auto st = _XPLATSTR("bytes=") + utility::conversions::to_string_t(std::to_string(single_task->processed_index->load()))
                  + _XPLATSTR("-"
                              + utility::conversions::to_string_t(
                std::to_string(single_task->end_index())));
        headers.add(web::http::header_names::range, st);
        if (single_task->processed_index->load() > single_task->end_index()) {
            return true;
        }
    }
    auto last_bytes = file_buf->getpos(std::ios::out);
    auto tsk_p = task.get();
    auto downloader = this->get_ptr();
    req.set_progress_handler(
            [file_buf, tsk_p, downloader, &single_task, &last_bytes](web::http::message_direction::direction _,
                                                                     utility::size64_t so_far) {
                // img
                // auto offset = so_far - download_count;
                // download_count = so_far;
                // global_transfer_counter::add_download_count(offset);
                // auto fp = qingzhen::string_util::pretty_bytes(single_task->end_index() + 1 - file_buf->getpos(std::ios::out));
                auto off = file_buf->getpos(std::ios::out);
                single_task->processed_index->store(off);
                auto diff = off - last_bytes;
                if (diff < 0) {
                    diff = 0;
                }
                last_bytes = off;
                tsk_p->incr_counter(diff);
                downloader->incr_global_counter(diff);

            });
    req.set_response_stream(oss);
    auto cr = client.request(req, cancellation_token).then(
            [file_buf, &single_task](const web::http::http_response &response) -> pplx::task<web::http::http_response> {
                if (response.status_code() != web::http::status_codes::OK &&
                    response.status_code() != web::http::status_codes::PartialContent) {

                    throw web::http::http_exception(9001);
                } else {
                    auto headers = response.headers();
                    file_buf->set_file_can_write(true);
                }
                return response.content_ready();
            }).then([file_buf](const web::http::http_response &response) -> pplx::task<void> {
        return file_buf->close(std::ios_base::out);
    });
	try
	{
		cr.get();
	}
	catch (const std::exception& ex) {
		if (!cancellation_token.is_canceled()) {
			BOOST_LOG_TRIVIAL(error) << "Http error: " << ex.what();
		}
		return false;
	}
	if (cancellation_token.is_canceled()) {
		return false;
	}
    auto fin_off = file_buf->getpos(std::ios::out);
    single_task->processed_index->store(fin_off);
	if (single_task->support_part()) {
		auto ed_index = single_task->end_index();
		if (single_task->end_index() > 0) {
			if (fin_off <= ed_index) {
				return false;
			}
		}
	}
    return true;
}

void simple_http_downloader::incr_global_counter(int64_t count) {
    this->_global_bytes_transfer.fetch_add(count);
}

void qingzhen::transfer::simple_http_downloader::refresh_counter()
{
	auto current = std::chrono::system_clock::now();
	auto time_diff_in_sec = (current - _last_refresh_time) / std::chrono::seconds(1);
	if (time_diff_in_sec < 1) {
		return;
	}
	auto last_transfer = this->_global_bytes_transfer.load();
	auto bytes_diff = last_transfer - this->_last_bytes_transfer;
	if (bytes_diff < 0) {
		bytes_diff = 0;
	}
	this->_last_bytes_transfer.store(last_transfer);
	this->_last_refresh_time = current;
	_speed_bytes_sec = bytes_diff / time_diff_in_sec;
}

int64_t qingzhen::transfer::simple_http_downloader::get_downloading_speed()
{
	return this->_speed_bytes_sec;
}