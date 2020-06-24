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

//lenmus headers
#include "lenmus_msg_box.h"

#include "lenmus_art_provider.h"

//wxWidgets headers
#include <wx/stattext.h>
#include <wx/statbmp.h>


namespace lenmus
{


#define lmID_BUTTON 1000

//=======================================================================================
// MsgBoxBase implementation
//=======================================================================================

wxBEGIN_EVENT_TABLE(MsgBoxBase, wxDialog)
    EVT_COMMAND_RANGE (lmID_BUTTON, lmID_BUTTON+lmMAX_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, MsgBoxBase::OnRespButton)
wxEND_EVENT_TABLE()

//---------------------------------------------------------------------------------------
MsgBoxBase::MsgBoxBase(const wxString& sMessage, const wxString& sTitle)
	: wxDialog((wxWindow*)nullptr, wxID_ANY, sTitle, wxDefaultPosition,
               wxSize(600, -1), wxDEFAULT_DIALOG_STYLE, "MsgBox"),
      m_sMessage(sMessage)
{
    m_nNumButtons = 0;
    m_nMaxButtonWidth = 0;
}

//---------------------------------------------------------------------------------------
MsgBoxBase::~MsgBoxBase()
{
}

//---------------------------------------------------------------------------------------
void MsgBoxBase::CreateControls()
{
	this->SetSizeHints(wxSize(-1, -1), wxSize( -1,-1 ));
	m_pMainSizer = new wxBoxSizer( wxVERTICAL );

	m_pMainSizer->Add( 0, 10, 0, wxALL, 5 );

	wxBoxSizer* pMsgSizer;
	pMsgSizer = new wxBoxSizer( wxHORIZONTAL );

	m_pBitmap = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	pMsgSizer->Add( m_pBitmap, 0, wxALL, 5 );

	m_pMessage = new wxStaticText( this, wxID_ANY, m_sMessage, wxDefaultPosition, wxSize( -1,-1 ), wxALIGN_LEFT );
	m_pMessage->Wrap( 400 );
	pMsgSizer->Add( m_pMessage, 0, wxALL, 5 );

	m_pMainSizer->Add( pMsgSizer, 0, wxALIGN_CENTER_HORIZONTAL, 20 );

    m_pButtonsSizer = new wxBoxSizer( wxVERTICAL );
	m_pMainSizer->Add( m_pButtonsSizer, 1, wxEXPAND|wxALL, 20 );

    //a new sizer to center the buttons
	wxBoxSizer* pCenterButtonsSizer = new wxBoxSizer( wxVERTICAL );
	m_pButtonsSizer->Add( pCenterButtonsSizer, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

    //loop to add buttons
    int nTextWrap = 500 - 2*25 - 10 - m_nMaxButtonWidth;
    for (int i=0; i < m_nNumButtons; i++)
    {
        //sizer for the button and its explanation text
        wxBoxSizer* pBtSizer = new wxBoxSizer( wxHORIZONTAL );
        pCenterButtonsSizer->Add( pBtSizer, 0, wxALIGN_CENTER_VERTICAL, 5 );

        //the button
	    m_pButton[i]->SetMinSize( wxSize(m_nMaxButtonWidth, -1) );
        pBtSizer->Add(m_pButton[i], 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

        //the explanation
	    m_pText[i]->Wrap(nTextWrap);
        pBtSizer->Add(m_pText[i], 0, wxALL|wxEXPAND, 5 );
    }
}

//---------------------------------------------------------------------------------------
void MsgBoxBase::FinishLayout()
{
    this->SetSizer( m_pMainSizer );
    SetAutoLayout( true );
    m_pMainSizer->Fit( this );
	this->Layout();

    Centre( wxBOTH | wxCENTER_FRAME);
}

//---------------------------------------------------------------------------------------
void MsgBoxBase::AddButton(const wxString& sLabel, const wxString& sDescr)
{
    if (m_nNumButtons >= lmMAX_BUTTONS)
    {
        wxMessageBox("MsgBoxBase: Max. number of buttons exceeded");
        return;
    }

    //the button
    m_pButton[m_nNumButtons] = new wxButton( this, lmID_BUTTON + m_nNumButtons, sLabel,
        wxDefaultPosition, wxDefaultSize);
    m_nMaxButtonWidth = wxMax(m_nMaxButtonWidth, m_pButton[m_nNumButtons]->GetSize().x );

    //the explanation
    m_pText[m_nNumButtons] = new wxStaticText( this, wxID_ANY, sDescr,
            wxDefaultPosition, wxDefaultSize, 0 );

    m_nNumButtons++;
}

//---------------------------------------------------------------------------------------
void MsgBoxBase::OnRespButton(wxCommandEvent& event)
{
    int nIndex = event.GetId() - lmID_BUTTON;
    EndModal( nIndex );
}


//=======================================================================================
// QuestionBox implementation
//=======================================================================================
QuestionBox::QuestionBox(const wxString& sMessage, int nNumButtons, ...)
    : MsgBoxBase(sMessage, _("Question"))
{
    //Additional parameters are button strings, in pairs, button label+button details.
    //Notice that va_arg does not determine either whether the retrieved argument is
    //the last argument passed to the function. Therefore, parameter nNumButtons is
    //used to know the amount of additional arguments it is going to get.

    va_list pArg;
    va_start(pArg, nNumButtons);      //points pArg to first arg after 'nNumButtons'
    for (int i=0;i < nNumButtons; i++)
    {
        wxString sLabel = wxString::Format("%s", va_arg(pArg, wxChar*));
        wxString sDescr = wxString::Format("%s", va_arg(pArg, wxChar*));
        AddButton(sLabel, sDescr);
    }
    va_end(pArg);
    CreateControls();

    m_pBitmap->SetBitmap( wxArtProvider::GetBitmap("msg_idea", wxART_OTHER, wxSize(48, 48)) );
    FinishLayout();
}


//=======================================================================================
// ErrorBox implementation
//=======================================================================================
ErrorBox::ErrorBox(const wxString& sMessage, const wxString& sButtonText)
    : MsgBoxBase(sMessage, _("Error"))
{
    wxString sLabel = "Accept";
    AddButton(sLabel, sButtonText);
    CreateControls();

    m_pBitmap->SetBitmap( wxArtProvider::GetBitmap("msg_error", wxART_OTHER, wxSize(32, 32)) );
    FinishLayout();
}


}   //namespace lenmus
