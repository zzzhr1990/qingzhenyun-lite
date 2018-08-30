//
// Created by zzzhr on 2018/8/30.
//

#ifndef FUCK_REMOTE_USER_FILE_PANEL_H
#define FUCK_REMOTE_USER_FILE_PANEL_H

#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/notebook.h"
#include "wx/dataview.h"

#endif


class RemoteUserFilePanel: public wxPanel {
public:
    explicit RemoteUserFilePanel(wxWindow *parent, wxWindowID winid = wxID_ANY,
    const wxPoint& pos = wxDefaultPosition,
    const wxSize& size = wxDefaultSize);
    void UpdateUserRemoteFiles();

private:
    wxTextCtrl * currentPageInput= nullptr;
    wxButton * prevBtn = nullptr;
    wxButton * nextBtn = nullptr;
    wxTextCtrl * pathInput = nullptr;
    wxButton * parentBtn = nullptr;
    wxDataViewListCtrl* userRemoteFilePage;

    void OnSizeChanged(wxSizeEvent& event);
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
};


#endif //FUCK_REMOTE_USER_FILE_PANEL_H
