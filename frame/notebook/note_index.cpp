//
// Created by zzzhr on 2018/8/26.
//

#include "note_index.h"

NoteIndex::NoteIndex(MainFrame *mainFrame, wxWindowID id) : wxNotebook(mainFrame, id) {
    this->main_frame = mainFrame;

    auto firstPanel = new wxPanel(this, wxID_ANY);
    const wxSizerFlags border = wxSizerFlags().DoubleBorder();
    auto *button_sizer = new wxBoxSizer(wxHORIZONTAL);
    button_sizer->Add(new wxButton(firstPanel, ID_ADD_MOZART, "Add Mozart"), border);
    button_sizer->Add(new wxButton(firstPanel, ID_DELETE_SEL, "Delete selected"), border);
    button_sizer->Add(new wxButton(firstPanel, ID_DELETE_YEAR, "Delete \"Year\" column"), border);
    button_sizer->Add(new wxButton(firstPanel, ID_SELECT_NINTH, "Select ninth symphony"), border);
    button_sizer->Add(new wxButton(firstPanel, ID_COLLAPSE, "Collapse"), border);
    // button_sizer->Add(new wxButton(firstPanel, ID_EXPAND, "Expand"), border);
    auto *sizerCurrent = new wxBoxSizer(wxHORIZONTAL);
    sizerCurrent->Add(new wxButton(firstPanel, ID_SHOW_CURRENT,
                                   "&Show current"), border);
    sizerCurrent->Add(new wxButton(firstPanel, ID_SET_NINTH_CURRENT,
                                   "Make &ninth symphony current"), border);
    wxSizer *firstPanelSz = new wxBoxSizer(wxVERTICAL);
    auto m_ctrl = new wxDataViewCtrl(firstPanel, ID_ATTR_CTRL, wxDefaultPosition,
                                     wxDefaultSize);
    m_ctrl->AppendTextColumn("editable string",
                                0,
                                wxDATAVIEW_CELL_EDITABLE,
                                wxCOL_WIDTH_AUTOSIZE,wxALIGN_CENTRE_HORIZONTAL,wxDATAVIEW_COL_RESIZABLE);
    m_ctrl->SetMinSize(wxSize(-1, 200));
    firstPanelSz->Add(m_ctrl, 1, wxGROW | wxALL, 5);
    firstPanelSz->Add(
            new wxStaticText(firstPanel, wxID_ANY, "Most of the cells above are editable!"),
            0, wxGROW | wxALL, 5);
    firstPanelSz->Add(button_sizer);
    firstPanelSz->Add(sizerCurrent);
    firstPanel->SetSizerAndFit(firstPanelSz);
    this->AddPage(firstPanel, "My remote files");

}
