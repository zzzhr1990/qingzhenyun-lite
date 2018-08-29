//
// Created by zzzhr on 2018/8/24.
//

#ifndef FUCK_LOG_FRAME_H
#define FUCK_LOG_FRAME_H

#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

class LogFrame : public  wxFrame{
public:
    LogFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size );
    ~LogFrame()= default;
    void OnClose(wxCloseEvent& event);
protected:
    wxTextCtrl* log_text_ctrl = nullptr;
    wxLog * wxLogger = nullptr;
};


#endif //FUCK_LOG_FRAME_H
