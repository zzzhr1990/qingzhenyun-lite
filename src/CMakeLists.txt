cmake_policy(SET CMP0022 NEW)

file(GLOB HEADERS_QINGZHEN "../include/qingzhen/*.h" "../include/qingzhen/*.hpp" "../include/qingzhen/*.dat")
file(GLOB HEADERS_QINGZHEN_API "../include/qingzhen/api/*.h" "../include/qingzhen/api/*.hpp" "../include/qingzhen/api/*.dat")
file(GLOB HEADERS_QINGZHEN_TRANSFER "../include/qingzhen/transfer/*.h" "../include/qingzhen/transfer/*.hpp" "../include/qingzhen/transfer/*.dat")

source_group("Header Files\\qingzhen" FILES ${HEADERS_QINGZHEN})
source_group("Header Files\\qingzhen\\transfer" FILES ${HEADERS_QINGZHEN_TRANSFER})
source_group("Header Files\\api" FILES ${HEADERS_QINGZHEN_API})

set(SOURCES
        ${HEADERS_QINGZHEN}
        ${HEADERS_QINGZHEN_API}
        ${HEADERS_QINGZHEN_TRANSFER}
        test.cpp
        api/user.cpp
        api/file.cpp
        api/file_requests.cpp
        string_util.cpp
        transfer/transfer_entity.cpp transfer/transfer_manager.cpp transfer/transfer_item.cpp transfer/single_file_task.cpp transfer/simple_http_downloader.cpp path_util.cpp transfer/transfer_progress.cpp transfer/file_hash_container.cpp)

add_library(qingzhen ${SOURCES})
target_include_directories(qingzhen
        PUBLIC
        $<INSTALL_INTERFACE:include> $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        PRIVATE
        )


# we need boosts
# find_package(OpenSSL REQUIRED)
# find_package(Boost REQUIRED chrono thread system)
# find_package(boost-asio REQUIRED)
target_include_directories(qingzhen PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries(qingzhen PUBLIC OpenSSL::SSL OpenSSL::Crypto ${Boost_LIBRARIES} cpprestsdk::cpprest cpprestsdk::cpprestsdk_zlib_internal cpprestsdk::cpprestsdk_boost_internal cpprestsdk::cpprestsdk_openssl_internal)

#         pch
# configure_pch(qingzhen stdafx.h pch/stdafx.cpp /Zm120)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    if (WERROR)
        target_compile_options(qingzhen PRIVATE -Werror)
    endif ()
    target_compile_options(qingzhen PRIVATE -pedantic ${WARNINGS})
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    if (WERROR)
        target_compile_options(qingzhen PRIVATE /WX ${WARNINGS})
    endif ()
else ()
    message(FATAL_ERROR "Unknown compiler")
endif ()

if (WIN32)
    if (BUILD_SHARED_LIBS)
        target_compile_definitions(qingzhen PRIVATE -D_QINGZHEN_EXPORT)
    else ()
        target_compile_definitions(qingzhen PUBLIC -D_NO_QINGZHENIMP)
    endif ()
elseif (ANDROID)
    target_link_libraries(qingzhen PRIVATE ${ANDROID_STL_FLAGS})
endif ()

if (WIN32 AND NOT WINDOWS_STORE AND NOT WINDOWS_PHONE)
    target_link_libraries(qingzhen PRIVATE
            bcrypt.lib
            crypt32.lib
            )
elseif (WINDOWS_STORE)
    if (NOT CMAKE_GENERATOR MATCHES "Visual Studio .*")
        target_compile_definitions(qingzhen PRIVATE -DWINAPI_FAMILY=WINAPI_FAMILY_PC_APP)
        get_target_property(LINK_FLAGS qingzhen LINK_FLAGS)
        if (NOT LINK_FLAGS)
            set(LINK_FLAGS "")
        endif ()
        set(LINK_FLAGS "${LINK_FLAGS} /APPCONTAINER")
        set_target_properties(qingzhen PROPERTIES LINK_FLAGS "${LINK_FLAGS}")
    endif ()
endif ()

set_target_properties(qingzhen PROPERTIES OUTPUT_NAME "qingzhen${QINGZHEN_ABI_TAG}")
if (WIN32)
elseif (ANDROID)
    # Do not use SOVERSION on android. It is completely unsupported (and causes problems).
    # Perhaps revisit in the future? (NDK r9d, 8/7/14)
else ()
    set_target_properties(qingzhen PROPERTIES
            SOVERSION ${QINGZHEN_VERSION_MAJOR}.${QINGZHEN_VERSION_MINOR})
endif ()

