//
// Created by zzzhr on 2018/8/23.
//

#ifndef FUCK_MAIN_FRAME_H
#define FUCK_MAIN_FRAME_H

#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "./log_frame.h"
#include "../model/user_model.h"
#include "./login_frame.h"

// Define a new frame type: this is going to be our main frame
class MainFrame : public wxFrame
{
public:
    // ctor(s)
    MainFrame(const wxString& title, int w, int h);
    ~MainFrame() final {
        //delete fileMenu;
        //delete helpMenu;
        // delete logFrame;
        // delete user_model;
    }

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void ShowLogWindow(wxCommandEvent& event);
    void OnThreadEvent(wxThreadEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnWindowCreate(wxIdleEvent& event);
	void showLoginFrame();
    // void OnUserCheckingTimerEvent(wxTimerEvent& event);
    // void CloseLogWindow(wxCloseEvent& event);

private:
    // any class wishing to process wxWidgets events must use this macro
    wxMenu *fileMenu = nullptr;
    // the "About" item should be in the help menu
    wxMenu *helpMenu = nullptr;

    LogFrame * logFrame = nullptr;

	LoginFrame *login_frame = nullptr;

    // UserModel * user_model = nullptr;




wxDECLARE_EVENT_TABLE();
};

#endif //FUCK_MAIN_FRAME_H
