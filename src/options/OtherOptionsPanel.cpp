//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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
//-------------------------------------------------------------------------------------/

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "OtherOptionsPanel.h"
#endif

// For compilers that support precompilation, includes <wx/wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/artprov.h>

#include "OtherOptionsPanel.h"

//access to preferences object
#include <wx/config.h>
extern wxConfigBase* g_pPrefs;

//access to global flag
#include "../app/Preferences.h"
extern bool g_fAnswerSoundsEnabled;
extern bool g_fAutoNewProblem;



lmOtherOptionsPanel::lmOtherOptionsPanel(wxWindow* parent)
    : lmOptionsPanel(parent)
{
    // create the panel
    CreateControls();

    //load icon
    m_pBmpIconTitle->SetBitmap( wxArtProvider::GetIcon(_T("opt_other"), wxART_TOOLBAR, wxSize(24,24)) );

   //Select current settings

    //Exercises options
    m_pChkAnswerSounds->SetValue(g_fAnswerSoundsEnabled);
    m_pChkAutoNewProblem->SetValue(g_fAutoNewProblem);

}

void lmOtherOptionsPanel::CreateControls()
{
	this->SetExtraStyle( wxWS_EX_BLOCK_EVENTS );
	
	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_pHeaderPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER|wxTAB_TRAVERSAL );
	wxBoxSizer* pHeaderSizer;
	pHeaderSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_pTxtTitle = new wxStaticText( m_pHeaderPanel, wxID_ANY, _("Other options"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	m_pTxtTitle->Wrap( -1 );
	m_pTxtTitle->SetFont( wxFont( 8, 74, 90, 92, false, wxT("Tahoma") ) );
	
	pHeaderSizer->Add( m_pTxtTitle, 0, wxALIGN_TOP|wxALL|wxADJUST_MINSIZE, 5 );
	
	
	pHeaderSizer->Add( 5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_pBmpIconTitle = new wxStaticBitmap( m_pHeaderPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	pHeaderSizer->Add( m_pBmpIconTitle, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	m_pHeaderPanel->SetSizer( pHeaderSizer );
	m_pHeaderPanel->Layout();
	pHeaderSizer->Fit( m_pHeaderPanel );
	pMainSizer->Add( m_pHeaderPanel, 0, wxEXPAND|wxBOTTOM, 5 );
	
	wxBoxSizer* pOptionsSizer;
	pOptionsSizer = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* pChecksSizer;
	pChecksSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Exercises") ), wxVERTICAL );
	
	m_pChkAnswerSounds = new wxCheckBox( this, wxID_ANY, _("Generate right/wrong sounds when clicking an answer button"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
	
	pChecksSizer->Add( m_pChkAnswerSounds, 0, wxEXPAND|wxALL, 5 );
	
	m_pChkAutoNewProblem = new wxCheckBox( this, wxID_ANY, _("Do not show solution and generate new problem if answer is correct"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
	
	pChecksSizer->Add( m_pChkAutoNewProblem, 0, wxEXPAND|wxALL, 5 );
	
	pOptionsSizer->Add( pChecksSizer, 0, wxEXPAND|wxALL, 5 );
	
	pMainSizer->Add( pOptionsSizer, 0, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( pMainSizer );
	this->Layout();
}

lmOtherOptionsPanel::~lmOtherOptionsPanel()
{
}

bool lmOtherOptionsPanel::Verify()
{
    return false;
}

void lmOtherOptionsPanel::Apply()
{
    // Exercises options
    g_fAnswerSoundsEnabled = m_pChkAnswerSounds->GetValue();
    g_pPrefs->Write(_T("/Options/EnableAnswerSounds"), g_fAnswerSoundsEnabled);

    g_fAutoNewProblem = m_pChkAutoNewProblem->GetValue();
    g_pPrefs->Write(_T("/Options/AutoNewProblem"), g_fAutoNewProblem);

}
