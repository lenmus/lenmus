//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2011 LenMus project
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

#include "lenmus_other_opt_panel.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/artprov.h>
#include <wx/config.h>


namespace lenmus
{

//---------------------------------------------------------------------------------------
OtherOptionsPanel::OtherOptionsPanel(wxWindow* parent, ApplicationScope& appScope)
    : OptionsPanel(parent, appScope)
{
    // create the panel
    CreateControls();

    //load icon
    m_pBmpIconTitle->SetBitmap( wxArtProvider::GetIcon(_T("opt_other"), wxART_TOOLBAR, wxSize(24,24)) );

   //Select current settings

    //Exercises options
    m_pChkAnswerSounds->SetValue( m_appScope.are_answer_sounds_enabled() );
    m_pChkAutoNewProblem->SetValue( m_appScope.is_auto_new_problem_enabled() );
}

//---------------------------------------------------------------------------------------
void OtherOptionsPanel::CreateControls()
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

//---------------------------------------------------------------------------------------
OtherOptionsPanel::~OtherOptionsPanel()
{
}

//---------------------------------------------------------------------------------------
bool OtherOptionsPanel::Verify()
{
    return false;
}

//---------------------------------------------------------------------------------------
void OtherOptionsPanel::Apply()
{
    wxConfigBase* pPrefs = m_appScope.get_preferences();

    bool enabled = m_pChkAnswerSounds->GetValue();
    m_appScope.enable_answer_sounds(enabled);
    pPrefs->Write(_T("/Options/EnableAnswerSounds"), enabled);

    enabled = m_pChkAutoNewProblem->GetValue();
    m_appScope.enable_auto_new_problem(enabled);
    pPrefs->Write(_T("/Options/AutoNewProblem"), enabled);
}


}   //namespace lenmus
