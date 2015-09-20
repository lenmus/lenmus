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

#include "lenmus_colors_opt_panel.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>


namespace lenmus
{

//---------------------------------------------------------------------------------------
ColorsOptPanel::ColorsOptPanel(wxWindow* parent, ApplicationScope& appScope)
    : OptionsPanel(parent, appScope)
{
    //TODO  GetLanguages(m_cLangCodes, m_cLangNames);
    wxString* m_cLangCodes = LENMUS_NEW wxString[2];
    wxString* m_cLangNames = LENMUS_NEW wxString[2];
    m_cLangCodes[0] = "en";        m_cLangNames[0] = "Green";
    m_cLangCodes[1] = "sp";        m_cLangNames[1] =  "Blue";
    int nNumLangs = 2;    //TODO  m_cLangNames.GetCount();

    wxString sysLang = "en";    //TODO GetSystemLanguageCode();

    wxBoxSizer *mainSizer = LENMUS_NEW wxBoxSizer(wxVERTICAL);
    wxBoxSizer *hSizer;

    hSizer = LENMUS_NEW wxBoxSizer(wxHORIZONTAL);
    hSizer->Add(LENMUS_NEW wxStaticText(this, -1,
                                    _("Choose colors to use:")),
                0, wxALIGN_CENTRE | wxALIGN_CENTER_VERTICAL | wxALL, 8);

    wxString *langArray = LENMUS_NEW wxString[nNumLangs];
    int i;
    for(i=0; i < nNumLangs; i++)
        langArray[i] = m_cLangNames[i];
    m_pChoice = LENMUS_NEW wxChoice(this, -1, wxDefaultPosition, wxDefaultSize,
                            nNumLangs, langArray);
    m_pChoice->SetSelection(0); // in case nothing else matches
    delete[] langArray;

    for(i=0; i < nNumLangs; i++) {
        if (m_cLangCodes[i] == sysLang)
            m_pChoice->SetSelection(i);
    }
    hSizer->Add(m_pChoice,
                0, wxALIGN_CENTRE | wxALIGN_CENTER_VERTICAL | wxALL, 8);

    mainSizer->Add(hSizer,
                    0, wxALL, 8);

    SetAutoLayout(true);
    SetSizer(mainSizer);
    mainSizer->Fit(this);
    mainSizer->SetSizeHints(this);

    //TODO Delete this debug code
    delete [] m_cLangNames;
    delete [] m_cLangCodes;


}

//---------------------------------------------------------------------------------------
ColorsOptPanel::~ColorsOptPanel()
{
}

//---------------------------------------------------------------------------------------
bool ColorsOptPanel::Verify()
{
    return false;
}

//---------------------------------------------------------------------------------------
void ColorsOptPanel::Apply()
{
}


}   //namespace lenmus
