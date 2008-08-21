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
#pragma implementation "LangOptionsPanel.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/xrc/xmlres.h"

#include "LangOptionsPanel.h"
#include "Languages.h"

//access to preferences object
#include "wx/config.h"
extern wxConfigBase* g_pPrefs;

#include "../app/TheApp.h"
extern lmTheApp* g_pTheApp;


lmLangOptionsPanel::lmLangOptionsPanel(wxWindow* parent)
{
    // create the panel
    wxXmlResource::Get()->LoadPanel(this, parent, _T("LangOptionsPanel"));

    //load icon
    wxStaticBitmap* pBmpIcon = XRCCTRL(*this, "bmpIconTitle", wxStaticBitmap);
    pBmpIcon->SetBitmap( wxArtProvider::GetIcon(_T("opt_language"), wxART_TOOLBAR, wxSize(24,24)) );

    //store pointer to wxChoice control
    m_pChoice = XRCCTRL(*this, "choiceLang", wxChoice);

    //Get current selected language
    m_sCurLang = g_pPrefs->Read(_T("/Locale/Language"), _T("en"));

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

lmLangOptionsPanel::~lmLangOptionsPanel()
{
}

bool lmLangOptionsPanel::Verify()
{
    return false;
}

void lmLangOptionsPanel::Apply()
{
    wxString sLangName = m_cLangNames[m_pChoice->GetSelection()];
    wxString sLang = m_cLangCodes[m_pChoice->GetSelection()];
    g_pPrefs->Write(_T("/Locale/Language"), sLang);
    if (sLang != m_sCurLang) {
        // When changing language a flag must be stored so that at next run the program must
        // clean the temp folder. Otherwise, as books have the same names in English and
        // in Spanish, the new language .hcc and hhk files will not be properly loaded.
        bool f = true;
        g_pPrefs->Write(_T("/Locale/LanguageChanged"), f);

#if 1   //TODO
        //changing the language would imply at least closing and saving all open documents.
        //So for now, just inform user

        //inform user
        wxMessageBox(wxString::Format(
            _("Language '%s' will be used the next time you run LenMus."),
            sLangName.c_str() ));
#else
        g_pTheApp->ChangeLanguage(sLang);
#endif
    }
}
