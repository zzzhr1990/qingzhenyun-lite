//
// Created by zzzhr on 2018/8/26.
//

#include "note_index.h"
#include "../../model/remote_file_model.h"
#include <chrono>
#include <iomanip> // put_time


NoteIndex::NoteIndex(MainFrame *mainFrame, wxWindowID id) : wxNotebook(mainFrame, id) {
    this->main_frame = mainFrame;

    auto firstPanel = new wxPanel(this, wxID_ANY);
    const wxSizerFlags border = wxSizerFlags().DoubleBorder();

    auto *topSizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText * st1 = new wxStaticText(firstPanel, wxID_ANY, _T("Current Path"));
    topSizer->Add(st1, 0, wxRIGHT, 8);
    pathInput = new wxTextCtrl(firstPanel, wxID_ANY);
    pathInput->SetEditable(false);
    topSizer->Add(pathInput, 1);
    parentBtn  = new wxButton(firstPanel, wxID_ANY, _T("Parent Directory"));
    parentBtn->Bind(wxEVT_BUTTON,&NoteIndex::ParentBtnClicked,this);
    auto addDirectoryBtn  = new wxButton(firstPanel, wxID_ANY, _T("New Directory"));
    topSizer->Add(parentBtn);
    topSizer->Add(addDirectoryBtn);

    // end top sizer
    auto *button_sizer = new wxBoxSizer(wxHORIZONTAL);

    button_sizer->Add(new wxButton(firstPanel, wxID_ANY, "Delete selected"), border);
    button_sizer->Add(new wxButton(firstPanel, wxID_ANY, "Move selected"), border);
    button_sizer->Add(new wxButton(firstPanel, wxID_ANY, "Share selected"), border);
    wxStaticText * st2 = new wxStaticText(firstPanel, wxID_ANY, _T("|"),wxDefaultPosition,wxDefaultSize,wxALIGN_CENTER_HORIZONTAL);
    button_sizer->Add(st2, 1,wxCENTER,border.GetBorderInPixels());
    prevBtn  = new wxButton(firstPanel, wxID_ANY, _T("Prev"));
    currentPageInput = new wxTextCtrl(firstPanel, wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxALIGN_CENTER_HORIZONTAL);
    currentPageInput->SetEditable(false);
    currentPageInput->Bind(wxEVT_LEFT_DCLICK,&NoteIndex::OnPageInputDClick,this);

    nextBtn  = new wxButton(firstPanel, wxID_ANY, _T("Next"));
    prevBtn->Bind(wxEVT_BUTTON,&NoteIndex::PrevBtnClicked,this);
    nextBtn->Bind(wxEVT_BUTTON,&NoteIndex::NextBtnClicked,this);
    button_sizer->Add(prevBtn, border);
    button_sizer->Add(currentPageInput, border);
    button_sizer->Add(nextBtn, border);
    // button_sizer->Add(new wxButton(firstPanel, ID_EXPAND, "Expand"), border);
    /*
    auto sizerCurrent = new wxBoxSizer(wxHORIZONTAL);
    sizerCurrent->Add(new wxButton(firstPanel, ID_SHOW_CURRENT,
                                   "&Show current"), border);
    sizerCurrent->Add(new wxButton(firstPanel, ID_SET_NINTH_CURRENT,
                                   "Make &ninth symphony current"), border);
                                   */
    wxSizer *firstPanelSz = new wxBoxSizer(wxVERTICAL);
    userRemoteFilePage = new wxDataViewListCtrl(firstPanel, ID_ATTR_CTRL, wxDefaultPosition,
                                     wxDefaultSize);
    // userRemoteFilePage->ContextM
    auto model = &RemoteFileModel::instance();
    // EVT_DATAVIEW_ITEM_CONTEXT_MENU()
    // model->AppendItem()
    userRemoteFilePage->AssociateModel(model);
    userRemoteFilePage->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU,&NoteIndex::OnUserRemoteFileContextMenu,this,userRemoteFilePage->GetId());
    //
    // wxEVT_DATAVIEW_ITEM_ACTIVATED
    userRemoteFilePage->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED,&NoteIndex::OnUserRemoteFileActivated,this,userRemoteFilePage->GetId());


    userRemoteFilePage->AppendTextColumn(_T(" "),wxDATAVIEW_CELL_INERT,40,wxALIGN_LEFT,wxDATAVIEW_COL_RESIZABLE);
    userRemoteFilePage->AppendTextColumn(_T("File name"),wxDATAVIEW_CELL_INERT,wxCOL_WIDTH_DEFAULT,wxALIGN_LEFT,wxDATAVIEW_COL_RESIZABLE);
    userRemoteFilePage->AppendTextColumn(_T("File Size"),wxDATAVIEW_CELL_INERT,100,wxALIGN_CENTRE,wxDATAVIEW_COL_RESIZABLE);
    userRemoteFilePage->AppendTextColumn(_T("File Mime"),wxDATAVIEW_CELL_INERT,160,wxALIGN_CENTRE,wxDATAVIEW_COL_RESIZABLE);
    userRemoteFilePage->AppendTextColumn(_T("Create Time"),wxDATAVIEW_CELL_INERT,120,wxALIGN_CENTRE,wxDATAVIEW_COL_RESIZABLE);
    userRemoteFilePage->SetMinSize(wxSize(-1, 200));
    userRemoteFilePage->SetFocus();
    //firstPanelSz->Add(topSizer);
    firstPanelSz->Add(topSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
    firstPanelSz->Add(-1, 10);
    firstPanelSz->Add(userRemoteFilePage, 1, wxGROW | wxALL, 5);
    /*
    firstPanelSz->Add(
            new wxStaticText(firstPanel, wxID_ANY, "Most of the cells above are editable!"),
            0, wxGROW | wxALL, 5);

     */

    firstPanelSz->Add(button_sizer,0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP);
    // firstPanelSz->Add(sizerCurrent);
    firstPanel->SetSizerAndFit(firstPanelSz);
    this->Connect(wxEVT_THREAD, wxThreadEventHandler(NoteIndex::OnThreadEvent));

    /*
    auto size = userRemoteFilePage->GetSize();
    std::cout << size.GetWidth() << "x" << size.GetHeight();
    userRemoteFilePage->GetColumn(0)->SetResizeable(false);
    userRemoteFilePage->GetColumn(0)->SetWidth(size.GetWidth() / 3);
    userRemoteFilePage->GetColumn(1)->SetWidth(size.GetWidth() / 3);
    userRemoteFilePage->GetColumn(2)->SetWidth(size.GetWidth() / 3);
     */
    this->AddPage(firstPanel, "My remote files");
    userRemoteFilePage->Bind(wxEVT_SIZE,&NoteIndex::OnSizeChanged,this);

}

void NoteIndex::OnUserRemoteFileContextMenu(wxDataViewEvent &event)
{
    wxMenu menu;
    menu.Append(1, "Detail");
    menu.Append(2, "Rename");
    menu.Append(3, "Move");
    menu.Append(4, "Delete");
    PopupMenu(&menu);
}

void NoteIndex::OnUserRemoteFileActivated(wxDataViewEvent &event)
{
    //std::cout<< "Dclicked???" << std::endl;
    // item.GetID();
    auto item = event.GetItem();
    auto & fileModel = RemoteFileModel::instance();
    if(item == nullptr){
        fileModel.GetPageById(this,fileModel.GetParent());
        return;
    }
    auto row = fileModel.GetRow(item);
    wxDataViewListStoreLine *line = fileModel.m_data[row];

    auto path =  line->m_values.at(5).GetString();
    auto type =  line->m_values.at(6).GetInteger();
    if(type == 1){
        fileModel.GetPage(this,path, 1);
    }else if(type == -1){
        wxMessageBox( wxT("This file is in invalid.\nThere's something wrong with our server."), _T("ErrorFile"), wxICON_INFORMATION);
        //ShowModal())msg->Show();
    }
    //
    //std::cout << data->at(0).GetString() << std::endl;
}

void NoteIndex::OnThreadEvent(wxThreadEvent& event)
{
    // true is to force the frame to close
    // Close(true);
    int eventType = event.GetInt();
    /*
    switch (eventType){
        case USER_REMOTE_FILE_PAGE_DATA:
            std::cout<< "Refresh User Remote file" << std::endl;
            break;
        default:
            //SetStatusText(wxString::Format(_T("Unknown event type...%d"), eventType));
            break;

    }*/
    if(eventType == USER_REMOTE_FILE_PAGE_DATA){
        // create ts
        auto payload = event.GetPayload<ResponseEntity>();
        if(payload.code == U("FILE_NOT_FOUND")){
            wxMessageBox( wxT("Destination invalid.\nThere parent directory not found.\nAuto goto root dir."), _T("Cannot go to directory"), wxICON_INFORMATION);
            RemoteFileModel::instance().GetPage(this,U("/"),1);
            return;
        }
        web::json::array list =  payload.result.at(U("list")).as_array();
        // model->AppendItem( data ,1);
        auto model = &RemoteFileModel::instance();
        model->DeleteAllItems();
        // model->AppendItem( data ,1);
        auto dirInfo = payload.result.at(U("info"));
        auto currentPath = dirInfo.at(U("path")).as_string();
        auto currentId = dirInfo.at(U("uuid")).as_string();
        auto parent = dirInfo.at(U("parent")).as_string();
        auto currentPage = payload.result.at(U("page")).as_integer();
        auto currentPageSize = payload.result.at(U("pageSize")).as_integer();
        auto totalPage = payload.result.at(U("totalPage")).as_integer();
        //const utility::string_t &patent

        model->UpateCurrentLocation(currentPath,currentId, currentPage,currentPageSize,totalPage,parent);
        for(const auto& i : list){
            i.as_object();
            //std::cout << i.at(U("name")) << std::endl;

            wxVector<wxVariant> data = wxVector<wxVariant>();
            if(i.at(U("type")).as_integer() == 1){
                data.emplace_back(_T("+"));
            }else{
                data.emplace_back(_T("-"));
            }
            data.emplace_back(i.at(U("name")).as_string());
            data.emplace_back(wxString::Format(_T("%ld"),i.at(U("size")).as_number().to_int64()));
            data.emplace_back(i.at(U("mime")).as_string());
            time_t time = i.at(U("atime")).as_number().to_int64() / 1000;
            std::stringstream ss;
            ss << std::put_time(std::localtime(&time), "%Y-%m-%d");
            //current.
            data.emplace_back(wxString::Format(_T("%s"),ss.str()));
            data.emplace_back(i.at(U("path")).as_string());
            data.emplace_back(i.at(U("type")).as_integer());
            //data.emplace_back(wxString::Format(_T("row %d======================得得得得得得得得的 ========================="), i) );
            model->AppendItem(data);

        }
        ResetCurrentPathDisplay();

    }
    // long d = event.GetTimestamp();
    // SetStatusText(wxString::Format(_T("User info update at...%ld"), d));

}

void NoteIndex::OnSizeChanged(wxSizeEvent &event) {
    // std::cout << event.GetSize().GetWidth() << "x" <<  event.GetSize().GetHeight()<< std::endl;
    // switch (event.)
    int panelWidth = event.GetSize().GetWidth();
    // int panelHeight = event.GetSize().GetHeight();
    auto plusCol = userRemoteFilePage->GetColumn(0);
    auto nameCol = userRemoteFilePage->GetColumn(1);
    auto sizeCol = userRemoteFilePage->GetColumn(2);
    auto mimeCol = userRemoteFilePage->GetColumn(3);
    auto timeCol = userRemoteFilePage->GetColumn(4);
    auto diff = panelWidth - plusCol->GetWidth() - sizeCol->GetWidth() - mimeCol->GetWidth() - timeCol->GetWidth();
    if(diff > 0){
        nameCol->SetWidth(diff);
    }
}

void NoteIndex::RefreshUserCurrentData() {
    // get current page...
    // auto page = this->GetCurrentPage();
    // SetStatusText(_T("User"));
    auto nPanel = this->GetSelection();
    if(nPanel == 0){
        UpdateUserRemoteFiles();
    }
}

void NoteIndex::UpdateUserRemoteFiles() {
    // request
    auto & fileModel = RemoteFileModel::instance();
    RemoteFileModel::instance().GetPage(this);
}



void NoteIndex::OnPageInputDClick(wxMouseEvent &event) {
    //event.Get
    auto & fileModel = RemoteFileModel::instance();
    currentPageInput->SetValue(wxString::Format(_T("%d"), fileModel.GetCurrentPage()));
    currentPageInput->SetEditable(true);
    currentPageInput->Bind(wxEVT_KILL_FOCUS,&NoteIndex::OnPageInputKillFocus, this);
}

void NoteIndex::OnPageInputKillFocus(wxFocusEvent &event) {

    currentPageInput->Unbind(wxEVT_KILL_FOCUS,&NoteIndex::OnPageInputKillFocus, this);
    currentPageInput->SetEditable(false);
    auto & fileModel = RemoteFileModel::instance();
    wxString input = currentPageInput->GetValue();
    int c = wxAtoi(input);
    if(c > 0 && c != fileModel.GetCurrentPage() && c <= fileModel.GetTotalPage()){
        fileModel.GetPage(this,fileModel.GetCurrentPath(),c);
    }else{
        ResetCurrentPathDisplay();
    }

}

void NoteIndex::ResetCurrentPathDisplay() {
    auto & fileModel = RemoteFileModel::instance();
    auto const &currentPage = fileModel.GetCurrentPage();
    auto const &totalPage = fileModel.GetTotalPage();
    currentPageInput->SetValue(wxString::Format(_T("%d / %d"), currentPage, totalPage));
    prevBtn->Enable(currentPage > 1);
    nextBtn->Enable(currentPage < totalPage);
    pathInput->SetValue(fileModel.GetCurrentPath());
    parentBtn->Enable(!fileModel.GetParent().empty());
}

void NoteIndex::ParentBtnClicked(wxCommandEvent &event) {
    auto & fileModel = RemoteFileModel::instance();
    fileModel.GetPageById(this,fileModel.GetParent());
    //std::cout << "EEE" << std::endl;
}

void NoteIndex::PrevBtnClicked(wxCommandEvent &event) {
    auto & fileModel = RemoteFileModel::instance();
    auto page = fileModel.GetCurrentPage();
    if(page > 1){
        fileModel.GetPage(this,fileModel.GetCurrentPath(),page - 1);
    }
}

void NoteIndex::NextBtnClicked(wxCommandEvent &event) {
    auto & fileModel = RemoteFileModel::instance();
    auto page = fileModel.GetCurrentPage();
    if(page < fileModel.GetTotalPage()){
        fileModel.GetPage(this,fileModel.GetCurrentPath(),page + 1);
    }
}

