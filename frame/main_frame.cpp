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

	auto userMenu = new wxMenu();
	//auto userMenuLoginId = wxID_ANY;
	userMenu->Append(ID_USER_LOGIN_MENU_ITEM, _T("&Login\tAlt-U"), _T("Change user to login"));

    // the "About" item should be in the help menu
    helpMenu = new wxMenu();

    helpMenu->Append(wxID_ABOUT, "&About\tF1", _T("Show about dialog"));

    fileMenu->Append(wxID_EXIT, "&Exit\tAlt-X", "Quit this program");

    fileMenu->Append(wxID_FILE1, "&Log\tAlt-L", "Show log window");


    // now append the freshly created menu to the menu bar...
    auto *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
	menuBar->Append(userMenu, "&User");
    menuBar->Append(helpMenu, "&Help");
	

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
	// File UI
	const wxSizerFlags border = wxSizerFlags().DoubleBorder();
	auto m_notebook = new wxNotebook( this, wxID_ANY );
	auto firstPanel = new wxPanel(m_notebook, wxID_ANY);
	wxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *button_sizer = new wxBoxSizer(wxHORIZONTAL);
	button_sizer->Add(new wxButton(firstPanel, ID_ADD_MOZART, "Add Mozart"), border);
	button_sizer->Add(new wxButton(firstPanel, ID_DELETE_SEL, "Delete selected"), border);
	button_sizer->Add(new wxButton(firstPanel, ID_DELETE_YEAR, "Delete \"Year\" column"), border);
	button_sizer->Add(new wxButton(firstPanel, ID_SELECT_NINTH, "Select ninth symphony"), border);
	button_sizer->Add(new wxButton(firstPanel, ID_COLLAPSE, "Collapse"), border);
	button_sizer->Add(new wxButton(firstPanel, ID_EXPAND, "Expand"), border);
	wxBoxSizer *sizerCurrent = new wxBoxSizer(wxHORIZONTAL);
	sizerCurrent->Add(new wxButton(firstPanel, ID_SHOW_CURRENT,
		"&Show current"), border);
	sizerCurrent->Add(new wxButton(firstPanel, ID_SET_NINTH_CURRENT,
		"Make &ninth symphony current"), border);
	wxSizer *firstPanelSz = new wxBoxSizer(wxVERTICAL);
	auto m_ctrl = new wxDataViewCtrl(firstPanel, ID_ATTR_CTRL, wxDefaultPosition,
		wxDefaultSize);
	m_ctrl->SetMinSize(wxSize(-1, 200));
	firstPanelSz->Add(m_ctrl, 1, wxGROW | wxALL, 5);
	firstPanelSz->Add(
		new wxStaticText(firstPanel, wxID_ANY, "Most of the cells above are editable!"),
		0, wxGROW | wxALL, 5);
	firstPanelSz->Add(button_sizer);
	firstPanelSz->Add(sizerCurrent);
	firstPanel->SetSizerAndFit(firstPanelSz);
	m_notebook->AddPage(firstPanel, "My remote files");
	mainSizer->Add(m_notebook, 1, wxGROW);
	SetSizerAndFit(mainSizer);
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText("Checking User Login...");
    auto id = this->GetId();
    this->Connect(id, wxEVT_CLOSE_WINDOW, wxCloseEventHandler(MainFrame::OnClose));
	// this->Connect(id, wxEVT_MENU, wxCloseEventHandler(MainFrame::OnClose));
    //this->user_model = UserModel;
    // user_model->add_event_frame(this);

    this->Connect(wxEVT_THREAD, wxThreadEventHandler(MainFrame::OnThreadEvent));
    //EVT WINDOW CREATE(ShapedFrame::OnWindowCreate)
    this->Connect(wxEVT_IDLE, wxIdleEventHandler(MainFrame::OnWindowCreate));
    // this->Connect(wxEVT_TIMER, wxTimerEventHandler(MainFrame::OnUserCheckingTimerEvent));
    // wxCloseEventHandler(LogFrame::OnClose)
    UserModel::instance().start_user_check_loop(this, ID_USER_CHECK_TIMER);

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
	if (!UserModel::instance().IsUserLogin()) {
		showLoginFrame();
	}
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


void MainFrame::OnLoginMenuClick(wxCommandEvent &WXUNUSED(event)) {
	showLoginFrame();
}

