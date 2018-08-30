//
// Created by zzzhr on 2018/8/29.
//

//#include <iomanip>
#include <iomanip>
#include "remote_download_task.h"
#include "../../model/remote_download_task_model.h"
#include "cpprest/http_client.h"
#include "../../common/common_event_ids.h"
#include "../../util/common_util.h"

RemoteDownloadTaskPanel::RemoteDownloadTaskPanel(wxWindow *parent,
                                       wxWindowID winid ,
                                       const wxPoint& pos,
                                       const wxSize& size):wxPanel(parent, winid,pos,size){
    // first, we create panel
    const wxSizerFlags border = wxSizerFlags().DoubleBorder();
    wxSizer *mainPanelSizer = new wxBoxSizer(wxVERTICAL);
    // main grid
    userRemoteTaskPage = new wxDataViewListCtrl(this, wxID_ANY, wxDefaultPosition,
                                                wxDefaultSize);
    userRemoteTaskPage->AssociateModel(&RemoteDownloadTaskModel::instance());
    userRemoteTaskPage->AppendTextColumn(_T("Task name"),wxDATAVIEW_CELL_INERT,wxCOL_WIDTH_DEFAULT,wxALIGN_LEFT,wxDATAVIEW_COL_RESIZABLE);
    userRemoteTaskPage->AppendTextColumn(_T("Task Size"),wxDATAVIEW_CELL_INERT,100,wxALIGN_CENTRE,wxDATAVIEW_COL_RESIZABLE);
    userRemoteTaskPage->AppendProgressColumn(_T("Task Progress"),wxDATAVIEW_CELL_INERT,140,wxALIGN_CENTRE,wxDATAVIEW_COL_RESIZABLE);
    userRemoteTaskPage->AppendTextColumn(_T("Task Status"),wxDATAVIEW_CELL_INERT,120,wxALIGN_CENTRE,wxDATAVIEW_COL_RESIZABLE);
    userRemoteTaskPage->AppendTextColumn(_T("Create Time"),wxDATAVIEW_CELL_INERT,140,wxALIGN_CENTRE,wxDATAVIEW_COL_RESIZABLE);
    // userRemoteTaskPage->SetMinSize(wxSize(-1, 400));
    userRemoteTaskPage->SetFocus();
    //grid end
    // btns
    auto *button_sizer = new wxBoxSizer(wxHORIZONTAL);
    button_sizer->Add(new wxButton(this, wxID_ANY, "Delete selected"), border);
    auto refreshBtn = new wxButton(this, wxID_ANY, "Refresh");
    refreshBtn->Bind(wxEVT_BUTTON, &RemoteDownloadTaskPanel::RefreshBtnClicked,this);
    button_sizer->Add(refreshBtn, border);
    wxStaticText * st2 = new wxStaticText(this, wxID_ANY, _T("-"),wxDefaultPosition,wxDefaultSize,wxALIGN_CENTER_HORIZONTAL);
    button_sizer->Add(st2, wxSizerFlags(1).DoubleBorder().Center());
    prevBtn  = new wxButton(this, wxID_ANY, _T("Prev"));
    currentPageInput = new wxTextCtrl(this, wxID_ANY,wxEmptyString,wxDefaultPosition,wxDefaultSize,wxALIGN_CENTER_HORIZONTAL);
    currentPageInput->SetEditable(false);
    currentPageInput->Bind(wxEVT_LEFT_DCLICK,&RemoteDownloadTaskPanel::OnPageInputDClick,this);
    nextBtn  = new wxButton(this, wxID_ANY, _T("Next"));
    prevBtn->Bind(wxEVT_BUTTON,&RemoteDownloadTaskPanel::PrevBtnClicked,this);
    nextBtn->Bind(wxEVT_BUTTON,&RemoteDownloadTaskPanel::NextBtnClicked,this);
    button_sizer->Add(prevBtn, border);
    button_sizer->Add(currentPageInput, border);
    button_sizer->Add(nextBtn, border);
    // btns end
    mainPanelSizer->Add(userRemoteTaskPage, wxSizerFlags(1).DoubleBorder().Expand());
    this->Bind(wxEVT_THREAD,&RemoteDownloadTaskPanel::OnThreadEvent,this);
    userRemoteTaskPage->Bind(wxEVT_SIZE,&RemoteDownloadTaskPanel::OnSizeChanged,this);
    mainPanelSizer->Add(button_sizer,wxSizerFlags().Expand().Left().Right().Top());
    this->SetSizerAndFit(mainPanelSizer);

}


void RemoteDownloadTaskPanel::OnPageInputKillFocus(wxFocusEvent &event) {

    currentPageInput->Unbind(wxEVT_KILL_FOCUS,&RemoteDownloadTaskPanel::OnPageInputKillFocus, this);
    currentPageInput->SetEditable(false);
    auto & downloadTaskModel = RemoteDownloadTaskModel::instance();
    wxString input = currentPageInput->GetValue();
    int c = wxAtoi(input);
    if(c > 0 && c != downloadTaskModel.GetCurrentPage() && c <= downloadTaskModel.GetTotalPage()&& c != downloadTaskModel.GetCurrentPage()){
        downloadTaskModel.GetPage(this,c);
    }else{
        ResetCurrentDisplay();
    }

}



void RemoteDownloadTaskPanel::OnPageInputDClick(wxMouseEvent &event) {
    //event.Get
    auto & downloadTaskModel = RemoteDownloadTaskModel::instance();
    currentPageInput->SetValue(wxString::Format(_T("%d"), downloadTaskModel.GetCurrentPage()));
    currentPageInput->SetEditable(true);
    currentPageInput->Bind(wxEVT_KILL_FOCUS,&RemoteDownloadTaskPanel::OnPageInputKillFocus, this);
}


void RemoteDownloadTaskPanel::PrevBtnClicked(wxCommandEvent &event) {
    auto & downloadTaskModel = RemoteDownloadTaskModel::instance();
    auto page = downloadTaskModel.GetCurrentPage();
    if(page > 1){
        downloadTaskModel.GetPage(this,page - 1);
    }
}

void RemoteDownloadTaskPanel::NextBtnClicked(wxCommandEvent &event) {
    auto & downloadTaskModel = RemoteDownloadTaskModel::instance();
    auto page = downloadTaskModel.GetCurrentPage();
    if(page < downloadTaskModel.GetTotalPage()){
        downloadTaskModel.GetPage(this,page + 1);
    }
}




void RemoteDownloadTaskPanel::ResetCurrentDisplay() {
    auto & downloadTaskModel = RemoteDownloadTaskModel::instance();
    auto const &currentPage = downloadTaskModel.GetCurrentPage();
    auto const &totalPage = downloadTaskModel.GetTotalPage();
    currentPageInput->SetValue(wxString::Format(_T("%d / %d"), currentPage, totalPage));
    prevBtn->Enable(currentPage > 1);
    nextBtn->Enable(currentPage < totalPage);
}


void RemoteDownloadTaskPanel::OnThreadEvent(wxThreadEvent& event)
{
    // true is to force the frame to close
    // Close(true);
    int eventType = event.GetInt();

    if(eventType == USER_REMOTE_TASK_PAGE_DATA){
        // create ts
        auto payload = event.GetPayload<ResponseEntity>();
        auto model = &RemoteDownloadTaskModel::instance();
        if(!payload.success){
            model->GetPage(this,1);
            return;
        }
        web::json::array list =  payload.result.at(U("list")).as_array();
        // model->AppendItem( data ,1);

        model->DeleteAllItems();
        // model->AppendItem( data ,1);
        // auto dirInfo = payload.result.at(U("info"));
        // auto currentPath = dirInfo.at(U("path")).as_string();
        // auto currentId = dirInfo.at(U("uuid")).as_string();
        // auto parent = dirInfo.at(U("parent")).as_string();
        auto currentPage = payload.result.at(U("page")).as_integer();
        auto currentPageSize = payload.result.at(U("pageSize")).as_integer();
        auto totalPage = payload.result.at(U("totalPage")).as_integer();
        //const utility::string_t &patent
        model->UpdateCurrent(currentPage, currentPageSize, totalPage);


        for(const auto& i : list){
            wxVector<wxVariant> data = wxVector<wxVariant>();
            data.emplace_back(i.at(U("name")).as_string()); //0: name
            data.emplace_back(ConvertSizeToDisplay(i.at(U("size")).as_number().to_int64())); // 1:size
            int progress = i.at(U("progress")).as_integer();
            if(progress > 100){
                progress = 100;
            }
            if(progress < 0){
                progress = 0;
            }
            data.emplace_back(progress); // 2:progress

            int status = i.at(U("status")).as_integer();

            if(status < 0){
                data.emplace_back(wxString::Format(_T("Error %d"),i.at(U("errorCode")).as_integer()));
            }else{
                if(status == 90){
                    data.emplace_back(_T("Copying"));
                } else if(status == 100){
                    data.emplace_back(_T("Finish"));
                }else{
                    data.emplace_back(_T("Downloading"));
                }
            } // 3:status

            time_t time = i.at(U("createTime")).as_number().to_int64() / 1000;

            //current.
            data.emplace_back(ConvertTimeToDisplay(time)); // 4 time
            long userId = i.at(U("userId")).as_number().to_int64();
            data.emplace_back(userId); // 5 userId
            data.emplace_back(i.at(U("taskId")).as_string()); //6 task id
            model->AppendItem(data);

        }
        ResetCurrentDisplay();

    }
    // long d = event.GetTimestamp();
    // SetStatusText(wxString::Format(_T("User info update at...%ld"), d));

}

void RemoteDownloadTaskPanel::OnSizeChanged(wxSizeEvent &event) {
    // std::cout << event.GetSize().GetWidth() << "x" <<  event.GetSize().GetHeight()<< std::endl;
    // switch (event.)
    int panelWidth = event.GetSize().GetWidth();
    // int panelHeight = event.GetSize().GetHeight();
    auto nameCol = userRemoteTaskPage->GetColumn(0);
    auto sizeCol = userRemoteTaskPage->GetColumn(1);
    auto progressCol = userRemoteTaskPage->GetColumn(2);
    auto statusCol = userRemoteTaskPage->GetColumn(3);
    auto timeCol = userRemoteTaskPage->GetColumn(4);
    auto s2 = 7 * 2;
    auto diff = panelWidth - sizeCol->GetWidth() - progressCol->GetWidth() - statusCol->GetWidth() - timeCol->GetWidth() - s2;
    if(diff > 0){
        nameCol->SetWidth(diff);
    }
}

void RemoteDownloadTaskPanel::LoadData(const bool& force) {
    if(!force){
        if(loadData){
            return;
        }
    }
    RemoteDownloadTaskModel::instance().GetPage(this);
    loadData = true;
}

void RemoteDownloadTaskPanel::RefreshBtnClicked(wxCommandEvent &event) {
    RemoteDownloadTaskModel::instance().GetPage(this);
}

