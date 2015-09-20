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

//lenmus
#include "lenmus_dlg_choose_lang.h"
#include "lenmus_languages.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/defs.h>
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/dialog.h>
#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

#include <wx/xrc/xmlres.h>
#include <wx/hashmap.h>


namespace lenmus
{


//---------------------------------------------------------------------------------------
wxBEGIN_EVENT_TABLE(DlgChooseLanguage, wxDialog)
    EVT_BUTTON(wxID_OK, DlgChooseLanguage::on_ok)
wxEND_EVENT_TABLE()


//---------------------------------------------------------------------------------------
DlgChooseLanguage::DlgChooseLanguage(wxWindow* parent, wxWindowID id,
                                   const wxString& title)
    : wxDialog(parent, id, title)
{
    GetLanguages(m_cLangCodes, m_cLangNames);
    m_nNumLangs = m_cLangNames.GetCount();

    wxString sysLang = GetSystemLanguageCode();

    wxBoxSizer *mainSizer = LENMUS_NEW wxBoxSizer(wxVERTICAL);
    wxBoxSizer *hSizer;

    hSizer = LENMUS_NEW wxBoxSizer(wxHORIZONTAL);
    hSizer->Add(LENMUS_NEW wxStaticText(this, -1,
                                    _("Choose language to use:")),
                0, wxALIGN_CENTRE | wxALIGN_CENTER_VERTICAL | wxALL, 8);

    wxString *langArray = LENMUS_NEW wxString[m_nNumLangs];
    for(int i=0; i < m_nNumLangs; i++)
        langArray[i] = m_cLangNames[i];
    m_pChoice = LENMUS_NEW wxChoice(this, -1, wxDefaultPosition, wxDefaultSize,
                            m_nNumLangs, langArray);
    m_pChoice->SetSelection(0); // in case nothing else matches
    delete[] langArray;

    for(int i=0; i < m_nNumLangs; i++)
    {
        if (m_cLangCodes[i] == sysLang)
            m_pChoice->SetSelection(i);
    }
    hSizer->Add(m_pChoice,
                0, wxALIGN_CENTRE | wxALIGN_CENTER_VERTICAL | wxALL, 8);

    mainSizer->Add(hSizer,
                    0, wxALL, 8);

    wxButton *ok = LENMUS_NEW wxButton(this, wxID_OK, _("OK"));
    ok->SetDefault();
    mainSizer->Add(ok, 0, wxALIGN_CENTRE | wxALL, 8);

    SetAutoLayout(true);
    SetSizer(mainSizer);
    mainSizer->Fit(this);
    mainSizer->SetSizeHints(this);

    // set default language, just in case user closes the dialog by clicking on
    // the dialog title [X] button
    m_sLang = m_cLangCodes[m_pChoice->GetSelection()];
}

//---------------------------------------------------------------------------------------
void DlgChooseLanguage::on_ok(wxCommandEvent& WXUNUSED(event))
{
    m_sLang = m_cLangCodes[m_pChoice->GetSelection()];
    EndModal(true);
}


}  //namespace lenmus
