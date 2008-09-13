//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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
//-------------------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "MusicSymbolCbo.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../app/ArtProvider.h"
#include "MusicSymbolCbo.h"


//------------------------------------------------------------------------------------
// lmMusicSymbolCboBase implementation
//------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(lmMusicSymbolCboBase, wxDialog)
    EVT_COMMAND_RANGE (lmID_BUTTON, lmID_BUTTON+lmMAX_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmMusicSymbolCboBase::OnRespButton)
END_EVENT_TABLE()



lmMusicSymbolCboBase::lmMusicSymbolCboBase(const wxString& sMessage, const wxString& sTitle)
	: wxDialog((wxWindow*)NULL, wxID_ANY, sTitle, wxDefaultPosition,
               wxSize(-1, -1), wxDEFAULT_DIALOG_STYLE, _T("MusicSymbolCbo")),
      m_sMessage(sMessage)
{
    m_nNumButtons = 0;
}

lmMusicSymbolCboBase::~lmMusicSymbolCboBase()
{
}

void lmMusicSymbolCboBase::CreateControls()
{
	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxVERTICAL );
	
	pMainSizer->Add( 0, 10, 0, wxALL, 5 );
	
	wxBoxSizer* pMsgSizer;
	pMsgSizer = new wxBoxSizer( wxHORIZONTAL );
	
	pMsgSizer->Add( 20, 0, 0, wxALL, 5 );
	
	m_pBitmap = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	pMsgSizer->Add( m_pBitmap, 0, wxALL, 5 );
	
	m_pMessage = new wxStaticText( this, wxID_ANY, m_sMessage, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	pMsgSizer->Add( m_pMessage, 0, wxALL, 5 );
	
	pMsgSizer->Add( 0, 0, 1, wxALL, 5 );
	
	pMainSizer->Add( pMsgSizer, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* pTableSizer;
	pTableSizer = new wxBoxSizer( wxHORIZONTAL );
	
	pTableSizer->Add( 20, 0, 0, wxALL, 5 );

	m_pButtonsSizer = new wxBoxSizer( wxVERTICAL );
	pTableSizer->Add( m_pButtonsSizer, 0, 0, 5 );
	
	pTableSizer->Add( 20, 0, 0, wxALL, 5 );

	pMainSizer->Add( pTableSizer, 1, wxEXPAND|wxTOP, 20 );
	
	this->SetSizer( pMainSizer );
}

void lmMusicSymbolCboBase::AddButton(const wxString& sLabel, const wxString& sDescr)
{
    if (m_nNumButtons >= lmMAX_BUTTONS)
    {
        wxMessageBox(_T("lmMusicSymbolCboBase: Max. number of buttons exceeded"));
        return;
    }

    //size for the button and its explanation text
    wxBoxSizer* pBtSizer = new wxBoxSizer( wxHORIZONTAL );
    m_pButtonsSizer->Add( pBtSizer, 0, wxALIGN_CENTER_VERTICAL, 5 );

    //the button
    wxButton* pButton = new wxButton( this, lmID_BUTTON + m_nNumButtons, sLabel,
        wxDefaultPosition, wxDefaultSize);
    pBtSizer->Add(pButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

    //the explanation
    wxStaticText* pText = new wxStaticText( this, wxID_ANY, sDescr, 
        wxDefaultPosition, wxDefaultSize, 0 );
    pBtSizer->Add(pText, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
    m_nNumButtons++;
}



void lmMusicSymbolCboBase::OnRespButton(wxCommandEvent& event)
{
    int nIndex = event.GetId() - lmID_BUTTON;
    EndModal( nIndex );
}




//------------------------------------------------------------------------------------
// lmQuestionBox implementation
//------------------------------------------------------------------------------------


lmQuestionBox::lmQuestionBox(const wxString& sMessage, int nNumButtons, ...)
    : lmMusicSymbolCboBase(sMessage, _("Question"))
{
    //Additional parameters are button strings, in pairs, button label+button details.
    //Notice that va_arg does not determine either whether the retrieved argument is 
    //the last argument passed to the function. Therefore, parameter nNumButtons is
    //used to know the amount of additional arguments it is going to get.

    va_list pArg;
    va_start(pArg, nNumButtons);      //points pArg to first arg after 'nNumButtons'
    CreateControls();
    for (int i=0;i < nNumButtons; i++)
    {
        wxString sLabel = wxString::Format(_T("%s"), va_arg(pArg, wxString*));
        wxString sDescr = wxString::Format(_T("%s"), va_arg(pArg, wxString*));
        AddButton(sLabel, sDescr);
    }
    va_end(pArg);

    m_pBitmap->SetBitmap( wxArtProvider::GetBitmap(_T("msg_idea"), wxART_OTHER, wxSize(48, 48)) );
	this->Layout();
}

//------------------------------------------------------------------------------------
// lmErrorBox implementation
//------------------------------------------------------------------------------------


lmErrorBox::lmErrorBox(const wxString& sMessage, const wxString& sButtonText)
    : lmMusicSymbolCboBase(sMessage, _("Error"))
{
    CreateControls();

    wxString sLabel = _T("Accept");
    AddButton(sLabel, sButtonText);

    m_pBitmap->SetBitmap( wxArtProvider::GetBitmap(_T("msg_error"), wxART_OTHER, wxSize(32, 32)) );
	this->Layout();
}

