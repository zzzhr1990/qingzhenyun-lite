//
// Created by herui on 2019/12/8.
//

#include <iostream>

#ifdef _WIN32
#define QZOUTPUT std::wcout
#else
#define QZOUTPUT std::cout
#endif
#define QZEND std::endl

#include <qingzhen/string_util.h>
#include <qingzhen/path_util.h>
#include <qingzhen/api/user_api_client.hpp>
#include <qingzhen/api/user_file_client.hpp>
#include <qingzhen/transfer/transfer_manager.h>


#include <chrono>
#include <thread>


int main() {

    pplx::cancellation_token_source token_source;
    utility::string_t test_download_directory = _XPLATSTR("/");
    utility::string_t user_token = _XPLATSTR(
            "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpZGVudGl0eSI6NSwibmFtZSI6Inp6emhyIiwidmVyc2lvbiI6Miwic3NpZCI6ImM4OGU1ZjIxLTA0OTctNDlkYy1hNzJmLWVjZmM1NDdhZTYzZCIsImRldmljZSI6IlBvc3RtYW5SdW50aW1lLzcuMjAuMSIsInBlcm1pc3Npb24iOjMsInNpZ25UaW1lIjoxNTc2MTc0MDM0MjQxLCJsb2dpblRpbWUiOjE1NzYxNzQwMzQwMjYsImxvZ2luQWRkciI6IjE3MS40My4xNDYuMTQ1IiwiaWF0IjoxNTc2MTc0MDM0LCJleHAiOjE1Nzg3NjYwMzR9.zEWAVfIPy1_dThNV-a1Q5FKIvo3mkSkeV5X7_Qtfwcg");
    std::cout << "Starting..." << std::endl;
    //

    auto user_result = qingzhen::api::user_api_client::login(user_token, token_source.get_token()).get();

    if (!user_result.success()) {
        QZOUTPUT << _XPLATSTR("Login failed: ") << user_result.error_message().c_str() << QZEND;
        return 1;
    }

    QZOUTPUT << _XPLATSTR("Login in: ") << user_result.data()->name().data() << ", space used: "
             << qingzhen::string_util::pretty_bytes(user_result.data()->space_used()).data() << " / "
             << qingzhen::string_util::pretty_bytes(user_result.data()->space_capacity()).data() << QZEND;

    qingzhen::api::file file_req;
    file_req.path = _XPLATSTR("/1495");
    qingzhen::api::file_list_request dir_req(file_req);
    std::cout << "Listing root directory..." << std::endl;
    auto start = std::chrono::system_clock::now();
    auto user_file_result = qingzhen::api::user_file_client::list_directory(dir_req, token_source.get_token()).get();
    if (!user_file_result.success()) {
        QZOUTPUT << _XPLATSTR("List root directory failed: ") << user_file_result.error_message().c_str() << QZEND;
        return 1;
    }
    auto dur = (std::chrono::system_clock::now() - start) / std::chrono::milliseconds(1);
    QZOUTPUT << _XPLATSTR("Root dir dir: ") << user_file_result.data()->list().size() << _XPLATSTR(" in ") << dur
             << _XPLATSTR(" milliseconds file size: ")
             << qingzhen::string_util::pretty_bytes(user_file_result.data()->parent()->size()) << QZEND;

    // get first dir
    // if (user_file_result.data()->list().empty()) {
    //    QZOUTPUT << _XPLATSTR("Empty root dir exit...") << QZEND;
    //    return 0;
    // }
	/*
    auto list = user_file_result.data()->list();
    int count = 0;
    for (const auto &dt : list) {
        QZOUTPUT << _XPLATSTR("File: ") << dt->path() << _XPLATSTR(" Size: ")
                 << qingzhen::string_util::pretty_bytes(dt->size()).c_str() << QZEND;
        count++;
        if (count > 100) {
            break;
        }
    }
	return 0;
	*/
#ifdef _WIN32
	auto location = std::filesystem::path(_XPLATSTR("D:\\TEST")).append(_XPLATSTR("Downloads"));
#else
	auto location = std::filesystem::current_path().append(_XPLATSTR("Downloads"));
#endif // _WIN32

    
    QZOUTPUT << _XPLATSTR("Download to current path: ") << user_file_result.data()->parent()->path->c_str()
             << _XPLATSTR(" => ") << location << QZEND;

    qingzhen::transfer::transfer_manager::instance().add_download(user_file_result.data()->parent(), location);

    QZOUTPUT << _XPLATSTR("Press Enter to exit...") << QZEND;
    qingzhen::transfer::transfer_manager::instance().start();
    std::cin.get();
    std::cout << "Cancel task..." << std::endl;
    qingzhen::transfer::transfer_manager::instance().shutdown();
    token_source.cancel();
    std::cout << "Cancel finished..." << std::endl;
    return 0;
}