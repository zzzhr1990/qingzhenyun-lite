//
// Created by zzzhr on 2018/8/26.
//

#ifndef FUCK_NOTE_INDEX_H
#define FUCK_NOTE_INDEX_H
#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/notebook.h"
#include "wx/dataview.h"

#endif

#include "../../common_id.h"
#include "./remote_download_task.h"
#include "./remote_user_file_panel.h"

class NoteIndex : public wxNotebook {
public:
    NoteIndex(wxFrame * mainFrame, wxWindowID id);
    void RefreshUserCurrentData();
private:
    //MainFrame * main_frame = nullptr;
    RemoteDownloadTaskPanel * remoteDownloadTaskPanel = nullptr;
    RemoteUserFilePanel * remoteUserFilePanel = nullptr;
    void OnNoteBookChange(wxBookCtrlEvent &event);
    void OnWindowCreate(wxIdleEvent& event);
};


#endif //FUCK_NOTE_INDEX_H
