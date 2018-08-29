//
// Created by zzzhr on 2018/8/24.
//

#include "log_frame.h"

LogFrame::LogFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size ) : wxFrame( parent, id, title, pos, size )
{
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    wxStaticBoxSizer* mainSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Log Output") ), wxVERTICAL );

    log_text_ctrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_MULTILINE | wxSUNKEN_BORDER );
    log_text_ctrl->SetMinSize(wxSize(600, 200));

    wxLogger = wxLog::SetActiveTarget(new wxLogTextCtrl(log_text_ctrl));

    // this->SetSizer( sbSizer2 );
    //error this-> SetSizerAndFit(&mainSizer);
    mainSizer->Add( log_text_ctrl, 0, wxGROW );
    this-> SetSizerAndFit(mainSizer);
    this->Layout();

    //this->Centre( wxBOTH );
    this->Connect(this->GetId(),wxEVT_CLOSE_WINDOW, wxCloseEventHandler(LogFrame::OnClose));
}

void LogFrame::OnClose(wxCloseEvent& event){
    event.Veto();
    this->Show(false);
}
