//
// Created by zzzhr on 2018/8/24.
//

#include "login_frame.h"
#include "../util/common_api.h"
#include "../model/user_model.h"

// #pragma clang diagnostic push
// #pragma ide diagnostic ignored "bugprone-suspicious-enum-usage"
LoginFrame::LoginFrame(wxWindow* parent, wxWindowID id) : wxFrame( parent, id, "User Login", wxDefaultPosition, wxSize(400, 200)) {
    wxPanel * panel = new wxPanel(this, wxID_ANY);

    /**< Main sizer */
    auto * vbox = new wxBoxSizer(wxVERTICAL);

    /**< hbox1 */

    // wxBoxSizer * hbox1 = new wxBoxSizer(wxHORIZONTAL);
    //(*
    /*
    wxStaticText * st1 = new wxStaticText(panel, wxID_ANY, _T("CLass Name"));
    hbox1->Add(st1, 0, wxRIGHT, 8);
    wxTextCtrl * tc = new wxTextCtrl(panel, wxID_ANY);
    hbox1->Add(tc, 1);
    vbox->Add(hbox1, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
    vbox->Add(-1, 10); // 10 pix space
     */
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

    /**< hbox3 */

    vbox->Add(-1, 10);
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
    wxButton * btn2 = new wxButton(panel, wxID_ANY, _T("Close"));
    hbox5->Add(btn2, 0, wxLEFT | wxBOTTOM, 15);
    vbox->Add(hbox5, 0, wxALIGN_RIGHT | wxRIGHT, 10);
    //*)
    panel->SetSizer(vbox);
    this->Connect(okBtn->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxMouseEventHandler(LoginFrame::OnOkBtnClicked));
    this->Connect(this->GetId(),wxEVT_THREAD, wxThreadEventHandler(LoginFrame::OnThreadEvent));
    this->CreateStatusBar();
}
//#pragma clang diagnostic pop

void LoginFrame::OnOkBtnClicked(wxMouseEvent & WXUNUSED(event)) {
    this->SetStatusText("Requesting remote server...");
    LockInterface();
    // CommonApi::instance()::post_data(U("/v1/user/login"),)
}

void LoginFrame::LockInterface() {
    std::string x = this->userInput->GetValue();
    std::string password = this->passwordInput->GetValue();
    passwordInput->Enable(false);
    okBtn->Enable(false);
    userInput->Enable(false);
    UserModel::instance().check_login(*this,x, password);
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
}

void LoginFrame::OnThreadEvent(wxThreadEvent &event) {
    auto success = event.GetInt() > 0;
    if(!success){
        UnlockInterface();
        this->SetStatusText("Login failed.");
    }else{
        UnlockInterface();
        passwordInput->SetValue(wxEmptyString);
        userInput->SetValue(wxEmptyString);
        this->SetStatusText("Login success.");
        auto payload = event.GetPayload<web::json::value>();
        UserModel::instance().on_user_login(payload);
        this->Close();
    }
}
