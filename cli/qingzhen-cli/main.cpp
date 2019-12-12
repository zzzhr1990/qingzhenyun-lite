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
#include <qingzhen/api/user_api_client.hpp>
#include <qingzhen/api/user_file_client.hpp>
#include <qingzhen/transfer/transfer_manager.h>


#include <chrono>
#include <thread>

int main() {

    pplx::cancellation_token_source token_source;
    utility::string_t test_download_directory = _XPLATSTR("/");
    utility::string_t user_token = _XPLATSTR(
            "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpZGVudGl0eSI6NSwibmFtZSI6Inp6emhyIiwidmVyc2lvbiI6Miwic3NpZCI6IjA0ODk0MTkxLWU3MGItNDc0OC04NTBlLTJjNTY4Mzk0MTcxMSIsImRldmljZSI6IlBvc3RtYW5SdW50aW1lLzcuMTkuMCIsInBlcm1pc3Npb24iOjMsInNpZ25UaW1lIjoxNTczNTc5NjIzODM4LCJsb2dpblRpbWUiOjE1NzM1Nzk2MjM4MTMsImxvZ2luQWRkciI6IjYwLjI1My4yMzAuMjE4IiwiaWF0IjoxNTczNTc5NjIzLCJleHAiOjE1NzYxNzE2MjN9.eiGA57eIOwYbMF1WxbdxusQme2iXw3wquX3wD6d9V_4");
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
    file_req.path = _XPLATSTR("/");
    qingzhen::api::file_list_request dir_req(file_req);
    std::cout << "Listing root directory..." << std::endl;
    auto start = std::chrono::system_clock::now();
    auto user_file_result = qingzhen::api::user_file_client::list_directory(dir_req, token_source.get_token()).get();
    if (!user_file_result.success()) {
        QZOUTPUT << _XPLATSTR("List root directory failed: ") << user_result.error_message().c_str() << QZEND;
        return 1;
    }
    auto dur = (std::chrono::system_clock::now() - start) / std::chrono::milliseconds(1);
    QZOUTPUT << _XPLATSTR("Root dir dir: ") << user_file_result.data()->list().size() << _XPLATSTR(" in ") << dur
             << _XPLATSTR(" milliseconds") << QZEND;

    // get first dir
    if (user_file_result.data()->list().empty()) {
        QZOUTPUT << _XPLATSTR("Empty root dir exit...") << QZEND;
        return 0;
    }

    auto list = user_file_result.data()->list();
    int count = 0;
    for (const auto &dt : list) {
        QZOUTPUT << _XPLATSTR("File: ") << dt->path() << _XPLATSTR(" Size: ")
                 << qingzhen::string_util::pretty_bytes(dt->size()).c_str() << QZEND;
        count++;
        if (count > 10) {
            break;
        }
    }
    auto location = std::filesystem::current_path().append(_XPLATSTR("Downloads"));
    QZOUTPUT << _XPLATSTR("Download to current path: ") << location << QZEND;

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