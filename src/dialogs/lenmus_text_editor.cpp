//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2010-2015 LenMus project
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation,
//    either version 3 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program. If not, see <http://www.gnu.org/licenses/>.
//
//    For any comment, suggestion or feature request, please contact the manager of
//    the project at cecilios@users.sourceforge.net
//
//---------------------------------------------------------------------------------------

#include "lenmus_text_editor.h"

#include "lenmus_art_provider.h"

////lomse
//#include <lomse_internal_model.h>
//using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/intl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>


namespace lenmus
{

// IDs for controls
enum
{
	k_id_text_window = 3010,
	k_id_text_control,
	k_id_toolbar,

	//tools
	k_id_tool_cut,
	k_id_tool_copy,
	k_id_tool_paste,
	k_id_tool_alignleft,
};


//---------------------------------------------------------------------------------------
wxBEGIN_EVENT_TABLE(TextEditor, wxDialog)
   EVT_BUTTON       (wxID_OK, TextEditor::on_ok_button)
   EVT_TEXT_ENTER   (k_id_text_control, TextEditor::on_ok_button)   //on_enter_key)

wxEND_EVENT_TABLE()

IMPLEMENT_CLASS(TextEditor, wxDialog)

//---------------------------------------------------------------------------------------
TextEditor::TextEditor(wxWindow * parent, wxString sTitle, wxString sData)
    : wxDialog(parent, k_id_text_window, sTitle, wxDefaultPosition, wxSize(800, 430),
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER )
{
    Centre();

    wxBoxSizer* pMainSizer = LENMUS_NEW wxBoxSizer(wxVERTICAL);

    //add toobal
    pMainSizer->Add(create_tool_bar(),
                    0,            //no vertically stretchable
                    wxEXPAND |    //horizontally stretchable
                    wxALL,        //some space border all around
                    5 );          //set border width to 5 px

    //add text control. Use wxTE_RICH2 style to avoid 64kB limit under MSW
    m_pTxtData = LENMUS_NEW wxTextCtrl(this, k_id_text_control, sData,
                                wxPoint(0, 0), wxDefaultSize,
                                wxTE_MULTILINE | wxTE_NOHIDESEL | wxTE_RICH2
                                | wxTE_PROCESS_ENTER );

    pMainSizer->Add(m_pTxtData,
                    1,            //vertically stretchable
                    wxEXPAND |    //horizontally stretchable
                    wxALL,        //some space border all around
                    5 );          //set border width to 5 px

    //add buttons
    wxBoxSizer* pButtonsSizer = LENMUS_NEW wxBoxSizer(wxHORIZONTAL);

    wxButton *cmdOK = LENMUS_NEW wxButton(this, wxID_OK, _("OK"));
    pButtonsSizer->Add(cmdOK, 0, 0, 1);

    wxButton *cmdCancel = LENMUS_NEW wxButton(this, wxID_CANCEL, _("Cancel"));
    pButtonsSizer->Add(cmdCancel, 0, 0, 1);

    pMainSizer->Add(pButtonsSizer, 0, wxALIGN_CENTER | wxALL, 5);

    // set autolayout based on sizers
    SetAutoLayout(true);
    SetSizer(pMainSizer);

    cmdOK->SetDefault();
    m_pTxtData->SetFocus();
}

//---------------------------------------------------------------------------------------
TextEditor::~TextEditor()
{
}

//---------------------------------------------------------------------------------------
wxToolBar* TextEditor::create_tool_bar()
{
    wxSize nSize(24, 24);
    wxToolBar* pToolBar = LENMUS_NEW wxToolBar(this, k_id_toolbar, wxDefaultPosition,
                            wxDefaultSize, wxTB_HORIZONTAL | wxNO_BORDER);
    pToolBar->AddTool(k_id_tool_cut, "Cut",
                      wxArtProvider::GetBitmap("tool_cut", wxART_TOOLBAR, nSize),
                      _("Cut"));
    pToolBar->AddTool(k_id_tool_copy, "Copy",
                      wxArtProvider::GetBitmap("tool_copy", wxART_TOOLBAR, nSize),
                      _("Copy"));
    pToolBar->AddTool(k_id_tool_paste, "Paste",
                      wxArtProvider::GetBitmap("tool_paste", wxART_TOOLBAR, nSize),
                      _("Paste"));
    pToolBar->AddTool(k_id_tool_alignleft, "Margins",
                      wxArtProvider::GetBitmap("tool_page_margins", wxART_TOOLBAR, nSize),
                      _("Change page margins"));
    pToolBar->Realize();
    return pToolBar;
}

//---------------------------------------------------------------------------------------
void TextEditor::on_ok_button(wxCommandEvent& WXUNUSED(event))
{
    EndModal(wxID_OK);
}

//---------------------------------------------------------------------------------------
void TextEditor::on_cancel_button(wxCommandEvent& WXUNUSED(event))
{
    EndModal(wxID_CANCEL);
}

//---------------------------------------------------------------------------------------
void TextEditor::load_text(const wxString& sText)
{
    m_pTxtData->AppendText(sText);
}

//---------------------------------------------------------------------------------------
wxString TextEditor::get_text()
{
    return m_pTxtData->GetValue();
}

//---------------------------------------------------------------------------------------
void TextEditor::load_paragraph(Document* WXUNUSED(pDoc), ImoParagraph* WXUNUSED(pImo))
{
    //TODO
    load_text("TODO: Se supone que esto es el contenido del párrafo seleccionado");
}


}   // namespace lenmus
