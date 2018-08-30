//
// Created by zzzhr on 2018/8/26.
//

#include "note_index.h"
#include "../../model/remote_file_model.h"
#include <chrono>
#include <iomanip> // put_time
#include "../../util/common_util.h"
#include "./remote_download_task.h"
#include "./remote_user_file_panel.h"


NoteIndex::NoteIndex(wxFrame *mainFrame, wxWindowID id) : wxNotebook(mainFrame, id) {
    //this->main_frame = mainFrame;

    // auto remoteFilePanel = new wxPanel(this, wxID_ANY);
    remoteDownloadTaskPanel = new RemoteDownloadTaskPanel(this);
    remoteUserFilePanel = new RemoteUserFilePanel(this);



    // end top sizer

    // button_sizer->Add(new wxButton(remoteFilePanel, ID_EXPAND, "Expand"), border);
    /*
    auto sizerCurrent = new wxBoxSizer(wxHORIZONTAL);
    sizerCurrent->Add(new wxButton(remoteFilePanel, ID_SHOW_CURRENT,
                                   "&Show current"), border);
    sizerCurrent->Add(new wxButton(remoteFilePanel, ID_SET_NINTH_CURRENT,
                                   "Make &ninth symphony current"), border);
                                   */

    // userRemoteFilePage->ContextM

    // EVT_DATAVIEW_ITEM_CONTEXT_MENU()
    // model->AppendItem()

    //
    // wxEVT_DATAVIEW_ITEM_ACTIVATED

    //firstPanelSz->Add(topSizer);

    // firstPanelSz->Add(userRemoteFilePage, 1, wxGROW | wxALL, 5);

    /*
    firstPanelSz->Add(
            new wxStaticText(remoteFilePanel, wxID_ANY, "Most of the cells above are editable!"),
            0, wxGROW | wxALL, 5);

     */


    // firstPanelSz->Add(sizerCurrent);


    /*
    auto size = userRemoteFilePage->GetSize();
    std::cout << size.GetWidth() << "x" << size.GetHeight();
    userRemoteFilePage->GetColumn(0)->SetResizeable(false);
    userRemoteFilePage->GetColumn(0)->SetWidth(size.GetWidth() / 3);
    userRemoteFilePage->GetColumn(1)->SetWidth(size.GetWidth() / 3);
    userRemoteFilePage->GetColumn(2)->SetWidth(size.GetWidth() / 3);
     */
    this->AddPage(remoteUserFilePanel, "My remote files");
    this->AddPage(remoteDownloadTaskPanel, "Remote download task");
    this->Bind(wxEVT_IDLE, &NoteIndex::OnWindowCreate,this);
    this->Bind(wxEVT_NOTEBOOK_PAGE_CHANGED,&NoteIndex::OnNoteBookChange,this);

}

void NoteIndex::RefreshUserCurrentData() {
    // get current page...
    // auto page = this->GetCurrentPage();
    // SetStatusText(_T("User"));
    auto nPanel = this->GetSelection();
    if(nPanel == 0){
        remoteUserFilePanel->UpdateUserRemoteFiles();
    }else if( nPanel == 1){
        remoteDownloadTaskPanel->LoadData(true);
    }
    // EVT_NOTEBOOK_PAGE_CHANGED
    //

}

void NoteIndex::OnNoteBookChange(wxBookCtrlEvent &event) {

    event.Skip();
    auto nPanel = event.GetSelection();
    if(nPanel == 0){
        remoteUserFilePanel->UpdateUserRemoteFiles();
    }else if( nPanel == 1){
        remoteDownloadTaskPanel->LoadData(false);
    }


}

void NoteIndex::OnWindowCreate(wxIdleEvent &event) {
    // this->Bind(wxEVT_NOTEBOOK_PAGE_CHANGED, &NoteIndex::OnNoteBookChange,this);
}





