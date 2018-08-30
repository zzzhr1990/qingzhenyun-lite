//
// Created by zzzhr on 2018/8/30.
//

#include "remote_user_file_panel.h"
#include "../../model/remote_file_model.h"
#include "../../util/common_util.h"
#include <iomanip>


RemoteUserFilePanel::RemoteUserFilePanel(wxWindow *parent, wxWindowID winid, const wxPoint &pos, const wxSize &size
            ):wxPanel(parent, winid,pos,size){
    const wxSizerFlags border = wxSizerFlags().DoubleBorder();
    auto *topSizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText * st1 = new wxStaticText(this, wxID_ANY, _T("Current Path"));
    topSizer->Add(st1, 0, wxRIGHT, 8);
    pathInput = new wxTextCtrl(this, wxID_ANY);
    pathInput->SetEditable(false);
    topSizer->Add(pathInput, 1);
    parentBtn  = new wxButton(this, wxID_ANY, _T("Parent Directory"));
    parentBtn->Bind(wxEVT_BUTTON,&RemoteUserFilePanel::ParentBtnClicked,this);
    auto addDirectoryBtn  = new wxButton(this, wxID_ANY, _T("New Directory"));
    topSizer->Add(parentBtn);
    topSizer->Add(addDirectoryBtn);
    auto *button_sizer = new wxBoxSizer(wxHORIZONTAL);

    button_sizer->Add(new wxButton(this, wxID_ANY, "Delete selected"), border);
    button_sizer->Add(new wxButton(this, wxID_ANY, "Move selected"), border);
    button_sizer->Add(new wxButton(this, wxID_ANY, "Share selected"), border);
    wxStaticText * st2 = new wxStaticText(this, wxID_ANY, _T("|"),wxDefaultPosition,wxDefaultSize,wxALIGN_CENTER_HORIZONTAL);
    button_sizer->Add(st2, 1,wxCENTER,border.GetBorderInPixels());
    prevBtn  = new wxButton(this, wxID_ANY, _T("Prev"));
    currentPageInput = new wxTextCtrl(this, wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxALIGN_CENTER_HORIZONTAL);
    currentPageInput->SetEditable(false);
    currentPageInput->Bind(wxEVT_LEFT_DCLICK,&RemoteUserFilePanel::OnPageInputDClick,this);

    nextBtn  = new wxButton(this, wxID_ANY, _T("Next"));
    prevBtn->Bind(wxEVT_BUTTON,&RemoteUserFilePanel::PrevBtnClicked,this);
    nextBtn->Bind(wxEVT_BUTTON,&RemoteUserFilePanel::NextBtnClicked,this);
    button_sizer->Add(prevBtn, border);
    button_sizer->Add(currentPageInput, border);
    button_sizer->Add(nextBtn, border);
    wxSizer *firstPanelSz = new wxBoxSizer(wxVERTICAL);
    userRemoteFilePage = new wxDataViewListCtrl(this, wxID_ANY, wxDefaultPosition,
                                                wxDefaultSize);
    auto model = &RemoteFileModel::instance();
    userRemoteFilePage->AssociateModel(model);
    userRemoteFilePage->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU,&RemoteUserFilePanel::OnUserRemoteFileContextMenu,this);
    userRemoteFilePage->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED,&RemoteUserFilePanel::OnUserRemoteFileActivated,this);


    userRemoteFilePage->AppendTextColumn(_T(" "),wxDATAVIEW_CELL_INERT,40,wxALIGN_LEFT,wxDATAVIEW_COL_RESIZABLE);
    userRemoteFilePage->AppendTextColumn(_T("File name"),wxDATAVIEW_CELL_INERT,wxCOL_WIDTH_DEFAULT,wxALIGN_LEFT,wxDATAVIEW_COL_RESIZABLE);
    userRemoteFilePage->AppendTextColumn(_T("File Size"),wxDATAVIEW_CELL_INERT,100,wxALIGN_CENTRE,wxDATAVIEW_COL_RESIZABLE);
    userRemoteFilePage->AppendTextColumn(_T("File Mime"),wxDATAVIEW_CELL_INERT,160,wxALIGN_CENTRE,wxDATAVIEW_COL_RESIZABLE);
    userRemoteFilePage->AppendTextColumn(_T("Create Time"),wxDATAVIEW_CELL_INERT,120,wxALIGN_CENTRE,wxDATAVIEW_COL_RESIZABLE);
    userRemoteFilePage->SetMinSize(wxSize(-1, 200));
    userRemoteFilePage->SetFocus();
    firstPanelSz->Add(topSizer, wxSizerFlags(0).DoubleBorder().Expand().Left().Right().Top());
    firstPanelSz->Add(-1, 10);
    firstPanelSz->Add(userRemoteFilePage, wxSizerFlags(1).DoubleBorder().Expand());
    firstPanelSz->Add(button_sizer,wxSizerFlags(0).Expand().Left().Right().Top());
    this->SetSizerAndFit(firstPanelSz);
    this->Connect(wxEVT_THREAD, wxThreadEventHandler(RemoteUserFilePanel::OnThreadEvent));
    userRemoteFilePage->Bind(wxEVT_SIZE,&RemoteUserFilePanel::OnSizeChanged,this);
}

void RemoteUserFilePanel::OnUserRemoteFileContextMenu(wxDataViewEvent &event)
{
    wxMenu menu;
    menu.Append(1, "Detail");
    menu.Append(2, "Rename");
    menu.Append(3, "Move");
    menu.Append(4, "Delete");
    PopupMenu(&menu);
}

void RemoteUserFilePanel::OnUserRemoteFileActivated(wxDataViewEvent &event)
{
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

void RemoteUserFilePanel::OnThreadEvent(wxThreadEvent& event)
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

        model->UpdateCurrentLocation(currentPath, currentId, currentPage, currentPageSize, totalPage, parent);
        for(const auto& i : list){//std::cout << i.at(U("name")) << std::endl;

            wxVector<wxVariant> data = wxVector<wxVariant>();
            if(i.at(U("type")).as_integer() == 1){
                data.emplace_back(_T("+"));
            }else{
                data.emplace_back(_T("-"));
            }
            data.emplace_back(i.at(U("name")).as_string());
            data.emplace_back(ConvertSizeToDisplay(i.at(U("size")).as_number().to_int64()));
            data.emplace_back(i.at(U("mime")).as_string());
            time_t time = i.at(U("atime")).as_number().to_int64() / 1000;
            //current.
            data.emplace_back(ConvertTimeToDisplay(time));
            data.emplace_back(i.at(U("path")).as_string());
            data.emplace_back(i.at(U("type")).as_integer());
            model->AppendItem(data);

        }
        ResetCurrentPathDisplay();

    }
    // long d = event.GetTimestamp();
    // SetStatusText(wxString::Format(_T("User info update at...%ld"), d));

}

void RemoteUserFilePanel::OnSizeChanged(wxSizeEvent &event) {
    // std::cout << event.GetSize().GetWidth() << "x" <<  event.GetSize().GetHeight()<< std::endl;
    // switch (event.)
    int panelWidth = event.GetSize().GetWidth();
    // int panelHeight = event.GetSize().GetHeight();
    auto plusCol = userRemoteFilePage->GetColumn(0);
    auto nameCol = userRemoteFilePage->GetColumn(1);
    auto sizeCol = userRemoteFilePage->GetColumn(2);
    auto mimeCol = userRemoteFilePage->GetColumn(3);
    auto timeCol = userRemoteFilePage->GetColumn(4);
    auto s2 = 7 * 2;
    // wxBorder broder = userRemoteFilePage->GetBorder();
    // broder.
    auto diff = panelWidth - plusCol->GetWidth() - sizeCol->GetWidth() - mimeCol->GetWidth() - timeCol->GetWidth() - s2;
    if(diff > 0){
        nameCol->SetWidth(diff);
    }
}

void RemoteUserFilePanel::UpdateUserRemoteFiles() {
    // request
    auto & fileModel = RemoteFileModel::instance();
    RemoteFileModel::instance().GetPage(this);
}

void RemoteUserFilePanel::OnPageInputDClick(wxMouseEvent &event) {
    //event.Get
    auto & fileModel = RemoteFileModel::instance();
    currentPageInput->SetValue(wxString::Format(_T("%d"), fileModel.GetCurrentPage()));
    currentPageInput->SetEditable(true);
    currentPageInput->Bind(wxEVT_KILL_FOCUS,&RemoteUserFilePanel::OnPageInputKillFocus, this);
}

void RemoteUserFilePanel::OnPageInputKillFocus(wxFocusEvent &event) {

    currentPageInput->Unbind(wxEVT_KILL_FOCUS,&RemoteUserFilePanel::OnPageInputKillFocus, this);
    currentPageInput->SetEditable(false);
    auto & fileModel = RemoteFileModel::instance();
    wxString input = currentPageInput->GetValue();
    int c = wxAtoi(input);
    if(c > 0 && c != fileModel.GetCurrentPage() && c <= fileModel.GetTotalPage() && c != fileModel.GetCurrentPage()){
        fileModel.GetPage(this,fileModel.GetCurrentPath(),c);
    }else{
        ResetCurrentPathDisplay();
    }

}

void RemoteUserFilePanel::ResetCurrentPathDisplay() {
    auto & fileModel = RemoteFileModel::instance();
    auto const &currentPage = fileModel.GetCurrentPage();
    auto const &totalPage = fileModel.GetTotalPage();
    currentPageInput->SetValue(wxString::Format(_T("%d / %d"), currentPage, totalPage));
    prevBtn->Enable(currentPage > 1);
    nextBtn->Enable(currentPage < totalPage);
    pathInput->SetValue(fileModel.GetCurrentPath());
    parentBtn->Enable(!fileModel.GetParent().empty());
}

void RemoteUserFilePanel::ParentBtnClicked(wxCommandEvent &event) {
    auto & fileModel = RemoteFileModel::instance();
    fileModel.GetPageById(this,fileModel.GetParent());
    //std::cout << "EEE" << std::endl;
}

void RemoteUserFilePanel::PrevBtnClicked(wxCommandEvent &event) {
    auto & fileModel = RemoteFileModel::instance();
    auto page = fileModel.GetCurrentPage();
    if(page > 1){
        fileModel.GetPage(this,fileModel.GetCurrentPath(),page - 1);
    }
}

void RemoteUserFilePanel::NextBtnClicked(wxCommandEvent &event) {
    auto & fileModel = RemoteFileModel::instance();
    auto page = fileModel.GetCurrentPage();
    if(page < fileModel.GetTotalPage()){
        fileModel.GetPage(this,fileModel.GetCurrentPath(),page + 1);
    }
}

