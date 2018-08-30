//
// Created by zzzhr on 2018/8/29.
//

#ifndef FUCK_REMOTE_DOWNLOAD_TASK_MODEL_H
#define FUCK_REMOTE_DOWNLOAD_TASK_MODEL_H
#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/dataview.h"
#endif

class RemoteDownloadTaskModel : public wxDataViewListStore {
public:
    static RemoteDownloadTaskModel& instance();

    int GetCurrentPage(){
        return current_page;
    }
    int GetTotalPage(){
        return total_page;
    }
    int GetCurrentPageSize(){
        return current_page_size;
    }

    void GetPage(wxWindow* handler,const int &page = -1, const int &pageSize = -1);
    void UpdateCurrent( const int &page, const int &pageSize, const int &totalPage);

private:
    int current_page = 1;
    int current_page_size = 20;
    int total_page = 1;
};



#endif //FUCK_REMOTE_DOWNLOAD_TASK_MODEL_H
