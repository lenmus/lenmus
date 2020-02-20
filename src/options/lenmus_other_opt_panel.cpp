//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2014 LenMus project
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
#include "lenmus_standard_header.h"
#include "lenmus_app.h"

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
    m_pBmpIconTitle->SetBitmap( wxArtProvider::GetIcon("opt_other", wxART_TOOLBAR, wxSize(24,24)) );

   //Select current settings

    //Exercises options
    m_pChkAnswerSounds->SetValue( m_appScope.are_answer_sounds_enabled() );
    m_pChkAutoNewProblem->SetValue( m_appScope.is_auto_new_problem_enabled() );
    m_fExperimentalEnabled = m_appScope.are_experimental_features_enabled();
    if (m_pChkExperimental)
	    m_pChkExperimental->SetValue(m_fExperimentalEnabled);
}

//---------------------------------------------------------------------------------------
void OtherOptionsPanel::CreateControls()
{
	this->SetExtraStyle( wxWS_EX_BLOCK_EVENTS );

	wxBoxSizer* pMainSizer;
	pMainSizer = LENMUS_NEW wxBoxSizer( wxVERTICAL );

	m_pHeaderPanel = LENMUS_NEW wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER|wxTAB_TRAVERSAL );
	wxBoxSizer* pHeaderSizer;
	pHeaderSizer = LENMUS_NEW wxBoxSizer( wxHORIZONTAL );

	m_pTxtTitle = LENMUS_NEW wxStaticText( m_pHeaderPanel, wxID_ANY, _("Other options"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	m_pTxtTitle->Wrap( -1 );
	m_pTxtTitle->SetFont( wxFont(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Tahoma" ) );

	pHeaderSizer->Add( m_pTxtTitle, 0, wxALIGN_TOP|wxALL, 5 );


	pHeaderSizer->Add( 5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_pBmpIconTitle = LENMUS_NEW wxStaticBitmap( m_pHeaderPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	pHeaderSizer->Add( m_pBmpIconTitle, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_pHeaderPanel->SetSizer( pHeaderSizer );
	m_pHeaderPanel->Layout();
	pHeaderSizer->Fit( m_pHeaderPanel );
	pMainSizer->Add( m_pHeaderPanel, 0, wxEXPAND|wxBOTTOM, 5 );

	wxBoxSizer* pOptionsSizer;
	pOptionsSizer = LENMUS_NEW wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* pChecksSizer;
	pChecksSizer = LENMUS_NEW wxStaticBoxSizer( LENMUS_NEW wxStaticBox( this, wxID_ANY, _("Exercises") ), wxVERTICAL );

	m_pChkAnswerSounds = LENMUS_NEW wxCheckBox( this, wxID_ANY,
            _("Generate right/wrong sounds when clicking an answer button"),
            wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	pChecksSizer->Add( m_pChkAnswerSounds, 0, wxEXPAND|wxALL, 5 );

	m_pChkAutoNewProblem = LENMUS_NEW wxCheckBox( this, wxID_ANY,
            _("Do not show solution and generate new problem if answer is correct"),
            wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

	pChecksSizer->Add( m_pChkAutoNewProblem, 0, wxEXPAND|wxALL, 5 );

	pOptionsSizer->Add( pChecksSizer, 0, wxEXPAND|wxALL, 5 );

	pMainSizer->Add( pOptionsSizer, 0, wxEXPAND|wxALL, 5 );


    m_pChkExperimental = nullptr;
#if (LENMUS_DEBUG_BUILD == 1 || LENMUS_RELEASE_INSTALL == 0)
    if (!m_appScope.is_release_behaviour())
    {
        m_pChkExperimental = LENMUS_NEW wxCheckBox( this, wxID_ANY,
                _("Enable experimental (unstable / in development) features"),
                wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );

        pMainSizer->Add( m_pChkExperimental, 0, wxEXPAND|wxALL, 5 );
    }
#endif

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
    pPrefs->Write("/Options/EnableAnswerSounds", enabled);

    enabled = m_pChkAutoNewProblem->GetValue();
    m_appScope.enable_auto_new_problem(enabled);
    pPrefs->Write("/Options/AutoNewProblem", enabled);

#if (LENMUS_DEBUG_BUILD == 1 || LENMUS_RELEASE_INSTALL == 0)
    m_appScope.enable_experimental_features(false);
    if (!m_appScope.is_release_behaviour())
    {
        enabled = m_pChkExperimental->GetValue();
        m_appScope.enable_experimental_features(enabled);
        pPrefs->Write("/Options/ExperimentalFeatures", enabled);

        if ((enabled && !m_fExperimentalEnabled) || (!enabled && m_fExperimentalEnabled))
        {
            //force to rebuild main frame and re-initialize all
            wxCommandEvent event(LM_EVT_RESTART_APP, lenmus::k_id_restart_app);
            wxGetApp().AddPendingEvent(event);
        }
    }
#endif

}


}   //namespace lenmus
