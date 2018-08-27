//
// Created by zzzhr on 2018/8/26.
//

#ifndef FUCK_NOTE_INDEX_H
#define FUCK_NOTE_INDEX_H
#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/notebook.h"

#endif

#include "../main_frame.h"
#include "../../common_id.h"
class NoteIndex : public wxNotebook {
public:
    NoteIndex(MainFrame * mainFrame, wxWindowID id);

private:
    MainFrame * main_frame;
};


#endif //FUCK_NOTE_INDEX_H
