//
// Created by zzzhr on 2018/8/24.
//

#include "login_frame.h"
#include "../util/common_api.h"
#include "../model/user_model.h"

// #pragma clang diagnostic push
// #pragma ide diagnostic ignored "bugprone-suspicious-enum-usage"
LoginFrame::LoginFrame(wxWindow* parent, wxWindowID id) : wxDialog( parent, id, "User Login", wxDefaultPosition, wxSize(400, 200), wxDEFAULT_DIALOG_STYLE | wxTAB_TRAVERSAL) {
    wxPanel * panel = new wxPanel(this, wxID_ANY);

    /**< Main sizer */
    auto * vbox = new wxBoxSizer(wxVERTICAL);

    /**< hbox1 */

    hbox1 = new wxBoxSizer(wxHORIZONTAL);
    //(*
    
    //wxStaticText * st1 = new wxStaticText(panel, wxID_ANY, _T("CLass Name"));
    //hbox1->Add(st1, 0, wxRIGHT, 8);
	infoText = new wxStaticText(panel, wxID_ANY, _T("Login"),wxDefaultPosition,wxDefaultSize,wxALIGN_CENTER_HORIZONTAL);
    //wxTextCtrl * tc = new wxTextCtrl(panel, wxID_ANY);
    hbox1->Add(infoText, 1);
    vbox->Add(hbox1, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
    //vbox->Add(-1, 10); // 10 pix space
     
    //*)


    /**< hbox2 */
    /*
    wxBoxSizer * hbox2 = new wxBoxSizer(wxHORIZONTAL);
    //(*
    wxStaticText * st2 = new wxStaticText(this, wxID_ANY, _T("User Login"));
    hbox2->Add(st2, 0);
    vbox->Add(hbox2, 0, wxLEFT | wxTOP, 10);
    vbox->Add(-1, 10);
     */
    //*)
	//wxBoxSizer * hbox2 = new wxBoxSizer(wxHORIZONTAL);
	//(*
	//wxStaticText * st2 = new wxStaticText(panel, wxID_ANY, _T("User Login"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL | wxALL | wxEXPAND);
	//hbox2->Add(st2, 0);
	//vbox->Add(hbox2, 0, wxLEFT | wxTOP, 10);

    /**< hbox3 */
	//wxStaticText * review = new wxStaticText(panel, wxID_ANY, _T("Review"));
    //vbox->Add(review);
    auto * hbox3 = new wxBoxSizer(wxHORIZONTAL);
    //(*
    /*
    wxTextCtrl * tc2 = new wxTextCtrl(this, wxID_ANY, _T(""),
                                      wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    hbox3->Add(tc2, 1, wxCENTER);
     */
    auto * fgs = new wxFlexGridSizer(2, 2, 9, 25);
    wxStaticText * thetitle = new wxStaticText(panel, wxID_ANY, _T("User/Phone"));
    wxStaticText * author   = new wxStaticText(panel, wxID_ANY, _T("Password"));
    //wxStaticText * review   = new wxStaticText(this, wxID_ANY, _T("Review"));
    userInput = new wxTextCtrl(panel, wxID_ANY);
    passwordInput = new wxTextCtrl(panel, wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxTE_PASSWORD);
    //wxTextCtrl * tc3 = new wxTextCtrl(this, wxID_ANY, _T(""),
    //                                  wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    //tc2->Enable(false);
    fgs->Add(thetitle);
    fgs->Add(userInput, 1, wxEXPAND);
    fgs->Add(author);
    fgs->Add(passwordInput, 1, wxEXPAND);
    //fgs->Add(review, 1, wxEXPAND);
    //fgs->Add(tc3, 1, wxEXPAND);

    fgs->AddGrowableRow(1, 1);
    fgs->AddGrowableCol(1, 1);
    hbox3->Add(fgs, 1, wxCENTER, 15);

    vbox->Add(hbox3, 1, wxLEFT | wxRIGHT | wxEXPAND, 10);
    //vbox->Add(-1, 10);
    //*)

    /**< hbox4 */
    // wxBoxSizer * hbox4 = new wxBoxSizer(wxHORIZONTAL);
    //(*
    /*
    wxCheckBox * cb1 = new wxCheckBox(panel, wxID_ANY, _T("Case Sensitive"));
    hbox4->Add(cb1);
    wxCheckBox * cb2 = new wxCheckBox(panel, wxID_ANY, _T("Nexted Classes"));
    hbox4->Add(cb2, 0, wxLEFT, 10);
    wxCheckBox * cb3 = new wxCheckBox(panel, wxID_ANY, _T("Non-Project Classes"));
    hbox4->Add(cb3, 0, wxLEFT, 10);
    vbox->Add(hbox4, 0, wxLEFT, 10);
    vbox->Add(-1, 25);
     */
    //*)

    /**< hbox5 */

    auto * hbox5 = new wxBoxSizer(wxHORIZONTAL);
    //(*
	
    okBtn = new wxButton(panel, wxID_ANY, _T("OK"));
    hbox5->Add(okBtn, 0);
	wxButton * closeBtn = new wxButton(panel, wxID_ANY, _T("Close"));
    hbox5->Add(closeBtn, 0, wxLEFT | wxBOTTOM, 15);
    vbox->Add(hbox5, 0, wxALIGN_RIGHT | wxRIGHT, 10);
    //*)
    panel->SetSizer(vbox);
	//this->Connect(this->GetId(), wxEVT_CLOSE_WINDOW, wxCloseEventHandler(LoginFrame::OnClose));
    this->Connect(okBtn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxMouseEventHandler(LoginFrame::OnOkBtnClicked));
	this->Connect(closeBtn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxMouseEventHandler(LoginFrame::OnCloseBtnClicked));
    this->Connect(this->GetId(),wxEVT_THREAD, wxThreadEventHandler(LoginFrame::OnThreadEvent));
    // this->CreateStatusBar();
	// infoText->SetLabelText(_T("Login failed, check your account."));
}
//#pragma clang diagnostic pop

void LoginFrame::OnOkBtnClicked(wxMouseEvent & WXUNUSED(event)) {
    // this->SetStatusText("Requesting remote server...");
    LockInterface();
    // CommonApi::instance()::post_data(U("/v1/user/login"),)
}

void LoginFrame::OnCloseBtnClicked(wxMouseEvent & WXUNUSED(event)) {
	// this->SetStatusText("Requesting remote server...");
	this->Close();
	// CommonApi::instance()::post_data(U("/v1/user/login"),)
}

void LoginFrame::LockInterface() {
    std::string x = this->userInput->GetValue().ToUTF8().data();
	std::string password = this->passwordInput->GetValue().ToUTF8().data();
    passwordInput->Enable(false);
    okBtn->Enable(false);
    userInput->Enable(false);
    UserModel::instance().check_login(this,x, password);
    /*
    task.then([&](web::json::value v){
        auto success = v[U("success")].as_bool();
        if(!success){
            // UnlockInterface();
            // this->SetStatusText("Login failed.");
            std::cout << "Login failed" << std::endl;
        }
    });
     */
}

void LoginFrame::UnlockInterface() {
    passwordInput->Enable(true);
    okBtn->Enable(true);
    userInput->Enable(true);
	okBtn->SetFocus();
}

void LoginFrame::OnThreadEvent(wxThreadEvent &event) {
    if(event.GetInt() == USER_LOGIN_RESPONSE){
        auto payload = event.GetPayload<ResponseEntity>();
        //auto success = ;
        if(!payload.success){
            UnlockInterface();
            // auto payload = event.GetPayload<ResponseEntity>();
            auto p = wxString::Format(_T("Login failed(%s), check your account."), payload.message);
            SetStatusText(p);

        }else{
            UnlockInterface();
            passwordInput->SetValue(wxEmptyString);
            userInput->SetValue(wxEmptyString);
            // this->SetStatusText("Login success.");

            UserModel::instance().on_user_login(payload);
            SetStatusText(_T("Login"));
            // infoText->Layout();
            this->Close();
        }
    }
}

void LoginFrame::SetStatusText(const wxString& text) {
	infoText->SetLabelText((text));
	hbox1->Layout();
}

/*
void LoginFrame::OnClose(wxCloseEvent& event) {
	UnlockInterface();
	event.Veto();
	infoText->SetLabelText(_T("Login"));
	this->Show(false);

}
*/