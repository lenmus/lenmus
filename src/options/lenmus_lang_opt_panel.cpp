//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2018 LenMus project
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

#include "lenmus_lang_opt_panel.h"
#include "lenmus_standard_header.h"
#include "lenmus_languages.h"
#include "lenmus_app.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/xrc/xmlres.h>
#include <wx/config.h>


namespace lenmus
{

//---------------------------------------------------------------------------------------
LangOptionsPanel::LangOptionsPanel(wxWindow* parent, ApplicationScope& appScope)
    : OptionsPanel(parent, appScope)
{
    create_controls();

    //load icon
    m_pTitleIcon->SetBitmap( wxArtProvider::GetIcon("opt_language", wxART_TOOLBAR, wxSize(24,24)) );

    //Get current selected language
    wxConfigBase* pPrefs = m_appScope.get_preferences();
    m_sCurLang = pPrefs->Read("/Locale/Language", "en");

    // Get array of available languages
    GetLanguages(m_cLangCodes, m_cLangNames);
    int nNumLangs = m_cLangNames.GetCount();
    m_pChoice->Append(m_cLangNames);

    //select current language
    m_pChoice->SetSelection(0);         //default selection
    int i;
    for(i=0; i < nNumLangs; i++) {
        if (m_cLangCodes[i] == m_sCurLang)
            m_pChoice->SetSelection(i);
    }

}

//---------------------------------------------------------------------------------------
LangOptionsPanel::~LangOptionsPanel()
{
}

//---------------------------------------------------------------------------------------
void LangOptionsPanel::create_controls()
{
    //content generated with wxFormBuilder

	this->SetExtraStyle( wxWS_EX_BLOCK_EVENTS );

	wxBoxSizer* pMainSizer;
	pMainSizer = LENMUS_NEW wxBoxSizer( wxVERTICAL );

	m_pTitlePannel = LENMUS_NEW wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER|wxTAB_TRAVERSAL );
	wxBoxSizer* pTitleSizer;
	pTitleSizer = LENMUS_NEW wxBoxSizer( wxHORIZONTAL );

	m_pTitle = LENMUS_NEW wxStaticText( m_pTitlePannel, wxID_ANY, _("Language preferences"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	m_pTitle->Wrap( -1 );
	m_pTitle->SetFont( wxFont(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Tahoma" ) );

	pTitleSizer->Add( m_pTitle, 0, wxALIGN_TOP|wxALL, 5 );


	pTitleSizer->Add( 5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_pTitleIcon = LENMUS_NEW wxStaticBitmap( m_pTitlePannel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	pTitleSizer->Add( m_pTitleIcon, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_pTitlePannel->SetSizer( pTitleSizer );
	m_pTitlePannel->Layout();
	pTitleSizer->Fit( m_pTitlePannel );
	pMainSizer->Add( m_pTitlePannel, 0, wxEXPAND|wxBOTTOM, 5 );

	wxBoxSizer* pLangSizer;
	pLangSizer = LENMUS_NEW wxBoxSizer( wxHORIZONTAL );

	m_pLang = LENMUS_NEW wxStaticText( this, wxID_ANY, _("Choose language to use:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLang->Wrap( -1 );
	pLangSizer->Add( m_pLang, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	wxArrayString m_pChoiceChoices;
	m_pChoice = LENMUS_NEW wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_pChoiceChoices, 0 );
	m_pChoice->SetSelection( 0 );
	pLangSizer->Add( m_pChoice, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	pMainSizer->Add( pLangSizer, 0, wxEXPAND|wxALL, 5 );

	this->SetSizer( pMainSizer );
	this->Layout();
}

//---------------------------------------------------------------------------------------
bool LangOptionsPanel::Verify()
{
    return false;
}

//---------------------------------------------------------------------------------------
void LangOptionsPanel::Apply()
{
    wxString sLangName = m_cLangNames[m_pChoice->GetSelection()];
    wxString sLang = m_cLangCodes[m_pChoice->GetSelection()];
    wxConfigBase* pPrefs = m_appScope.get_preferences();
    pPrefs->Write("/Locale/Language", sLang);
    if (sLang != m_sCurLang)
    {
        // When changing language a flag must be stored so that at next run the program must
        // clean the temp folder. Otherwise, as books have the same names in English and
        // in Spanish, the LENMUS_NEW language .hcc and hhk files will not be properly loaded.
        bool f = true;
        pPrefs->Write("/Locale/LanguageChanged", f);

        wxCommandEvent event(LM_EVT_CHANGE_LANGUAGE, lenmus::k_id_change_language);
        wxGetApp().AddPendingEvent(event);
    }
}


}   //namespace lenmus
