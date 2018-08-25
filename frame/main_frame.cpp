//
// Created by zzzhr on 2018/8/23.
//

#include "main_frame.h"
#include "../common_id.h"

MainFrame::MainFrame(const wxString& title, int w, int h)
        : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition,  wxSize(w, h))
{
    // set the frame icon
    // SetIcon(wxICON(sample));


    // create a menu bar
    fileMenu = new wxMenu();



    // the "About" item should be in the help menu
    helpMenu = new wxMenu();

    helpMenu->Append(wxID_ABOUT, "&About\tF1", "Show about dialog");

    fileMenu->Append(wxID_EXIT, "&Exit\tAlt-X", "Quit this program");

    fileMenu->Append(wxID_FILE1, "&Log\tAlt-L", "Show log window");


    // now append the freshly created menu to the menu bar...
    auto *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText("Checking User Login...");
    auto id = this->GetId();
    this->Connect(id, wxEVT_CLOSE_WINDOW, wxCloseEventHandler(MainFrame::OnClose));
    auto timer_id = wxID_ANY;
    //this->user_model = UserModel;
    // user_model->add_event_frame(this);

    this->Connect(timer_id, wxEVT_THREAD, wxThreadEventHandler(MainFrame::OnThreadEvent));
    //EVT WINDOW CREATE(ShapedFrame::OnWindowCreate)
    this->Connect(wxEVT_IDLE, wxIdleEventHandler(MainFrame::OnWindowCreate));
    // this->Connect(wxEVT_TIMER, wxTimerEventHandler(MainFrame::OnUserCheckingTimerEvent));
    // wxCloseEventHandler(LogFrame::OnClose)
    UserModel::instance().start_user_check_loop(this);

    std::cout << "Create window id " << id << std::endl;

}


// event handlers

void MainFrame::OnClose(wxCloseEvent& event){
    event.Skip();
    UserModel::instance().terminate();
}

void MainFrame::OnWindowCreate(wxIdleEvent& WXUNUSED(event)){
    // event.Skip();
    // user_model->terminate();
    this->Disconnect(wxEVT_IDLE, wxIdleEventHandler(MainFrame::OnWindowCreate));
	showLoginFrame();
}

void MainFrame::showLoginFrame(){
	if (this->login_frame == nullptr) {
		login_frame = new LoginFrame(this, wxID_ANY);
		login_frame->Iconize(false);
	}
	
	// login_frame->Show(true);
	login_frame->Raise();  // doesn't seem to work
	login_frame->SetFocus();  // does nothing
	login_frame->ShowModal(); // this by itself doesn't work
	login_frame->RequestUserAttention();
}

void MainFrame::OnThreadEvent(wxThreadEvent& event)
{
    // true is to force the frame to close
    // Close(true);
    long d = event.GetTimestamp();
    SetStatusText(wxString::Format("Checking User Login...%ld", d));

}


void MainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxString::Format
                         (
                                 "Welcome to %s!\n"
                                 "\n"
                                 "Access https://www.qingzhenyun.com\n"
                                 "for more information.\n\n"
                                 "running under %s.",
                                 "Halal Cloud Disk",
                                 wxGetOsDescription()
                         ),
                 "Halal Cloud Disk",
                 wxOK | wxICON_INFORMATION,
                 this);
}

void MainFrame::ShowLogWindow(wxCommandEvent& WXUNUSED(event)){
    if(logFrame == nullptr){
        logFrame = new LogFrame(this, wxID_ANY, "Log window", wxDefaultPosition, wxSize(300, 200));
        logFrame->Iconize(false);
        //logFrame->OnCloseWindow();
    }
    logFrame->Raise();  // doesn't seem to work
    logFrame->SetFocus();  // does nothing
    logFrame->Show(true); // this by itself doesn't work
    logFrame->RequestUserAttention();
}

