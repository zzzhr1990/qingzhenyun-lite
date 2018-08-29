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

#include "../main_frame.h"
#include "../../common_id.h"
class NoteIndex : public wxNotebook {
public:
    NoteIndex(MainFrame * mainFrame, wxWindowID id);
    void RefreshUserCurrentData();
private:
    MainFrame * main_frame = nullptr;
    void OnSizeChanged(wxSizeEvent& event);
    wxDataViewListCtrl* userRemoteFilePage;
    void UpdateUserRemoteFiles();
    // bool CheckUserOnline(MainFrame * mainFrame);
    void OnUserRemoteFileContextMenu(wxDataViewEvent &event);
    void OnThreadEvent(wxThreadEvent& event);
    void OnUserRemoteFileActivated(wxDataViewEvent &event);
    void OnPageInputDClick(wxMouseEvent &event);
    void OnPageInputKillFocus(wxFocusEvent &event);
    void ResetCurrentPathDisplay();
    void ParentBtnClicked(wxCommandEvent & event);
    void PrevBtnClicked(wxCommandEvent & event);
    void NextBtnClicked(wxCommandEvent & event);
    wxTextCtrl * currentPageInput= nullptr;
    wxButton * prevBtn = nullptr;
    wxButton * nextBtn = nullptr;
    wxTextCtrl * pathInput = nullptr;
    wxButton * parentBtn = nullptr;
};


#endif //FUCK_NOTE_INDEX_H
