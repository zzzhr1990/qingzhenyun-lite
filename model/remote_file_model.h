//
// Created by zzzhr on 2018/8/27.
//

#ifndef FUCK_REMOTE_FILE_MODEL_H
#define FUCK_REMOTE_FILE_MODEL_H


#include <pplx/pplxtasks.h>
#include "../util/common_api.h"
#include "cpprest/http_client.h"

class RemoteFileModel {
public:
    static RemoteFileModel& instance();
    pplx::task<ResponseEntity> page(const utility::string_t &path, const int &page = 1, const int &pageSize = 20, const int &type = -1);
};


#endif //FUCK_REMOTE_FILE_MODEL_H
