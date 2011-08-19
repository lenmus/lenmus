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

#include "lenmus_lang_opt_panel.h"

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
    m_pTitleIcon->SetBitmap( wxArtProvider::GetIcon(_T("opt_language"), wxART_TOOLBAR, wxSize(24,24)) );

    //Get current selected language
    wxConfigBase* pPrefs = m_appScope.get_preferences();
    m_sCurLang = pPrefs->Read(_T("/Locale/Language"), _T("en"));

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
	pMainSizer = new wxBoxSizer( wxVERTICAL );

	m_pTitlePannel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER|wxTAB_TRAVERSAL );
	wxBoxSizer* pTitleSizer;
	pTitleSizer = new wxBoxSizer( wxHORIZONTAL );

	m_pTitle = new wxStaticText( m_pTitlePannel, wxID_ANY, _("Language preferences"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	m_pTitle->Wrap( -1 );
	m_pTitle->SetFont( wxFont( 8, 74, 90, 92, false, wxT("Tahoma") ) );

	pTitleSizer->Add( m_pTitle, 0, wxALIGN_TOP|wxALL|wxADJUST_MINSIZE, 5 );


	pTitleSizer->Add( 5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_pTitleIcon = new wxStaticBitmap( m_pTitlePannel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	pTitleSizer->Add( m_pTitleIcon, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_pTitlePannel->SetSizer( pTitleSizer );
	m_pTitlePannel->Layout();
	pTitleSizer->Fit( m_pTitlePannel );
	pMainSizer->Add( m_pTitlePannel, 0, wxEXPAND|wxBOTTOM, 5 );

	wxBoxSizer* pLangSizer;
	pLangSizer = new wxBoxSizer( wxHORIZONTAL );

	m_pLang = new wxStaticText( this, wxID_ANY, _("Choose language to use:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pLang->Wrap( -1 );
	pLangSizer->Add( m_pLang, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5 );

	wxArrayString m_pChoiceChoices;
	m_pChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_pChoiceChoices, 0 );
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
    pPrefs->Write(_T("/Locale/Language"), sLang);
    if (sLang != m_sCurLang)
    {
        // When changing language a flag must be stored so that at next run the program must
        // clean the temp folder. Otherwise, as books have the same names in English and
        // in Spanish, the new language .hcc and hhk files will not be properly loaded.
        bool f = true;
        pPrefs->Write(_T("/Locale/LanguageChanged"), f);

#if 0
        //inform user
        wxMessageBox(wxString::Format(
            _("Language '%s' will be used the next time you run LenMus."),
            sLangName.c_str() ));
#else
        wxCommandEvent event(lmEVT_CHANGE_LANGUAGE, ID_CHANGE_LANGUAGE);
        wxGetApp().AddPendingEvent(event);
#endif
    }
}


}   //namespace lenmus
