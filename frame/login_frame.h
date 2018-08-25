//
// Created by zzzhr on 2018/8/24.
//

#ifndef FUCK_LOGIN_FRAME_H
#define FUCK_LOGIN_FRAME_H

#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

class LoginFrame : public wxFrame{
public:
    LoginFrame(wxWindow* parent, wxWindowID id);
    ~LoginFrame(){
        delete passwordInput;
        delete okBtn;
        delete userInput;
    }

private:
    void OnOkBtnClicked(wxMouseEvent &event);
    void OnThreadEvent(wxThreadEvent& event);
    void LockInterface();
    void UnlockInterface();

    wxTextCtrl * passwordInput = nullptr;
    wxButton * okBtn = nullptr;
    wxTextCtrl * userInput = nullptr;
};


#endif //FUCK_LOGIN_FRAME_H
