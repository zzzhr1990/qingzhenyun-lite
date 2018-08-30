//
// Created by zzzhr on 2018/8/29.
//

#ifndef FUCK_REMOTE_DOWNLOAD_TASK_H
#define FUCK_REMOTE_DOWNLOAD_TASK_H

#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/notebook.h"
#include "wx/dataview.h"

#endif

class RemoteDownloadTaskPanel : public wxPanel {
public:
    explicit RemoteDownloadTaskPanel(wxWindow *parent, wxWindowID winid = wxID_ANY,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize);
    // void ResetCurrentDisplay();
    void LoadData(const bool& force);

private:
    wxTextCtrl * currentPageInput= nullptr;
    wxButton * prevBtn = nullptr;
    wxButton * nextBtn = nullptr;

    void OnPageInputDClick(wxMouseEvent &event);
    void OnPageInputKillFocus(wxFocusEvent &event);
    void PrevBtnClicked(wxCommandEvent & event);
    void NextBtnClicked(wxCommandEvent & event);
    void RefreshBtnClicked(wxCommandEvent &event);
    void OnThreadEvent(wxThreadEvent& event);
    void RefreshClicked(wxMouseEvent & event);
    void OnSizeChanged(wxSizeEvent &event);
    bool loadData = false;
    wxDataViewListCtrl * userRemoteTaskPage = nullptr;
    void ResetCurrentDisplay();
};


#endif //FUCK_REMOTE_DOWNLOAD_TASK_H
