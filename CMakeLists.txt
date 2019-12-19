cmake_minimum_required(VERSION 3.15)

project(qingzhenyun_lite)
set(CMAKE_CXX_STANDARD 17)

if (POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW) # use MACOSX_RPATH
endif ()
if (UNIX)
    project(qingzhenyun_lite C CXX)
else ()
    project(qingzhenyun_lite CXX)
endif ()

set(QINGZHEN_VERSION_MAJOR 2)
set(QINGZHEN_VERSION_MINOR 10)
set(QINGZHEN_VERSION_REVISION 14)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries")

if (WIN32)
    set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "Default filename postfix for libraries under configuration DEBUG")
else ()
    set(CMAKE_DEBUG_POSTFIX "" CACHE STRING "Default filename postfix for libraries under configuration DEBUG")
endif ()

if (WIN32)
    set(TOOLSET)
    if (CMAKE_VS_PLATFORM_TOOLSET)
        string(REGEX REPLACE "^v" "" TOOLSET "${CMAKE_VS_PLATFORM_TOOLSET}")
    endif ()
    set(QINGZHEN_ABI_TAG "${TOOLSET}_${QINGZHEN_VERSION_MAJOR}_${QINGZHEN_VERSION_MINOR}" CACHE STRING "Postfix tag for the cpprest abi")
else ()
    set(QINGZHEN_ABI_TAG "" CACHE STRING "Postfix tag for the cpprest abi")
endif ()

if (ANDROID)
    set(Boost_USE_STATIC_LIBS ON CACHE BOOL "Link against boost statically.")
else ()
    set(Boost_USE_STATIC_LIBS OFF CACHE BOOL "Link against boost statically.")
endif ()

include(cmake/find_lib.cmake)


include(CheckIncludeFiles)
include(GNUInstallDirs)

find_package(Threads REQUIRED)
if (THREADS_HAVE_PTHREAD_ARG)
    add_compile_options(-pthread)
endif ()
if (CMAKE_THREAD_LIBS_INIT)
    link_libraries(${CMAKE_THREAD_LIBS_INIT})
endif ()

if (NOT WIN32)
    CHECK_INCLUDE_FILES(xlocale.h HAVE_XLOCALE_H)
endif ()

if (UNIX) # This includes OSX
elseif (WIN32)
    add_definitions(-DUNICODE -D_UNICODE -DWIN32 -D_SCL_SECURE_NO_WARNINGS)
    if (CMAKE_SYSTEM_NAME STREQUAL "WindowsStore")
        add_definitions(-D_WIN32_WINNT=0x0A00)
    else ()
        add_definitions(-D_WIN32_WINNT=0x0600)
    endif ()

    if (NOT BUILD_SHARED_LIBS)
        # This causes cmake to not link the test libraries separately, but instead hold onto their object files.
        set(TEST_LIBRARY_TARGET_TYPE OBJECT)
    endif ()

    add_definitions(-D_WINSOCK_DEPRECATED_NO_WARNINGS)
else ()
    message(FATAL_ERROR "-- Unsupported Build Platform.")
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    message("-- Setting msvc options")
    set(WARNINGS)
    set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} /ignore:4264")
    add_compile_options(/bigobj)
    add_compile_options("/utf-8")
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MP")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /MP")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MP")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MP")

    set(CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL "${CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL} /profile /OPT:REF /OPT:ICF")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /profile /OPT:REF /OPT:ICF")

    if (WINDOWS_STORE OR WINDOWS_PHONE)
        add_compile_options(/ZW)
    else ()
        if (NOT (MSVC_VERSION LESS 1920))
            add_compile_options(/permissive-)
        endif ()
    endif ()
endif ()

# Reconfigure final output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/Binaries)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/Binaries)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/Binaries)

add_subdirectory(src)
add_subdirectory(cli)