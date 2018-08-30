cmake_minimum_required(VERSION 3.12)
project(fuck)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(OPENSSL_INCLUDE_DIR "/usr/local/opt/openssl/include")
set(OPENSSL_LIB_DIR "/usr/local/opt/openssl/lib")
SET(OPENSSL_INCLUDE_DIR "/usr/local/opt/openssl/include")
SET (BOOST_ROOT "/usr/local/opt/boost")
SET (BOOST_INCLUDEDIR "/usr/local/opt/boost/include")
SET (BOOST_LIBRARYDIR "/usr/local/opt/boost/lib")
SET (BOOST_MIN_VERSION "1.67")
SET (Boost_NO_BOOST_CMAKE ON)


include_directories(
        /usr/local/opt/openssl/include
)

link_directories(
        /usr/local/opt/openssl/lib
)

add_executable(fuck main.cpp frame/main_frame.cpp frame/main_frame.h common_id.h model/user_model.cpp model/user_model.h util/common_api.cpp util/common_api.h frame/log_frame.cpp frame/log_frame.h util/simple_timer.cpp util/simple_timer.h frame/login_frame.cpp frame/login_frame.h frame/notebook/note_index.cpp frame/notebook/note_index.h model/remote_file_model.cpp model/remote_file_model.h common/common_event_ids.h util/common_util.h frame/notebook/remote_download_task.cpp frame/notebook/remote_download_task.h model/remote_download_task_model.cpp model/remote_download_task_model.h frame/notebook/remote_user_file_panel.cpp frame/notebook/remote_user_file_panel.h)
find_package(wxWidgets)
find_package(cpprestsdk REQUIRED cpprestsdk cpprest web pplx)
FIND_PACKAGE(Boost ${BOOST_MIN_VERSION} REQUIRED filesystem system chrono thread)
FIND_PACKAGE(OpenSSL 1.0.1 REQUIRED)
include(${wxWidgets_USE_FILE} ${cpprestsdk_INCLUDE_FILE} ${Boost_USE_FILE} ${OPENSSL_USE_FILE})
target_link_libraries(fuck ${wxWidgets_LIBRARIES} cpprestsdk::cpprest ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})

#FOREACH(LETTER ${OPENSSL_LIBRARIES})
#    MESSAGE(WARNING "<<${LETTER}>>")
#ENDFOREACH()
#MESSAGE(WARNING "OK -")
#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()