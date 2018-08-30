//
// Created by zzzhr on 2018/8/27.
//

#ifndef FUCK_REMOTE_FILE_MODEL_H
#define FUCK_REMOTE_FILE_MODEL_H


#include <pplx/pplxtasks.h>
#include "../util/common_api.h"
#include "cpprest/http_client.h"
#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/dataview.h"
#include "../common/common_event_ids.h"
#endif


class RemoteFileModel : public wxDataViewListStore {
public:
    static RemoteFileModel& instance();
    void GetPage(wxWindow* handler, const utility::string_t &path = U(""), const int &page = -1, const int &pageSize = -1, const int &type = -1);
    void GetPageById(wxWindow* handler, const utility::string_t &uuid, const int &page = -1, const int &pageSize = -1, const int &type = -1);
    /*
    static void SendThreadMessage(wxWindow * handler, ResponseEntity v){

    }
     */
    int GetCurrentPage(){
        return current_page;
    }
    int GetTotalPage(){
        return total_page;
    }
    int GetCurrentPageSize(){
        return current_page_size;
    }

    utility::string_t GetParent(){
        return current_parent;
    }

    void UpdateCurrentLocation(const utility::string_t &path, const utility::string_t &fileId, const int &page,
                               const int &pageSize, const int &totalPage, const utility::string_t &parent);
    utility::string_t GetCurrentPath();

private:
    utility::string_t current_path = U("/");
    utility::string_t current_file_id = U("");
    int current_page = 1;
    int current_page_size = 20;
    int total_page = 1;
    utility::string_t current_parent = U("");
};


#endif //FUCK_REMOTE_FILE_MODEL_H
