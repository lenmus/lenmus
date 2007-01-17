//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the 
//    terms of the GNU General Public License as published by the Free Software Foundation;
//    either version 2 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this 
//    program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, 
//    Fifth Floor, Boston, MA  02110-1301, USA.
//
//    For any comment, suggestion or feature request, please contact the manager of 
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "ColorsOptPanel.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif


#include "ColorsOptPanel.h"

BEGIN_EVENT_TABLE(lmColorsOptPanel, wxPanel)
    //EVT_BUTTON(wxID_OK, lmColorsOptPanel::OnOk)
END_EVENT_TABLE()

lmColorsOptPanel::lmColorsOptPanel(wxWindow* parent) :
    lmOptionsPanel(parent)
{
    //! @todo  GetLanguages(m_cLangCodes, m_cLangNames);
    wxString* m_cLangCodes = new wxString[2];
    wxString* m_cLangNames = new wxString[2];
    m_cLangCodes[0] = _T("en");        m_cLangNames[0] = _T("Green");
    m_cLangCodes[1] = _T("sp");        m_cLangNames[1] =  _T("Blue");
    int nNumLangs = 2;    //! @todo  m_cLangNames.GetCount();

    wxString sysLang = _T("en");    //! @todo GetSystemLanguageCode();

    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *hSizer;

    hSizer = new wxBoxSizer(wxHORIZONTAL);
    hSizer->Add(new wxStaticText(this, -1,
                                    _("Choose colors to use:")),
                0, wxALIGN_CENTRE | wxALIGN_CENTER_VERTICAL | wxALL, 8);

    wxString *langArray = new wxString[nNumLangs];
    int i;
    for(i=0; i < nNumLangs; i++)
        langArray[i] = m_cLangNames[i];
    m_pChoice = new wxChoice(this, -1, wxDefaultPosition, wxDefaultSize,
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

    //! @todo Delete this debug code
    delete [] m_cLangNames;
    delete [] m_cLangCodes;


}

lmColorsOptPanel::~lmColorsOptPanel()
{
}

bool lmColorsOptPanel::Verify()
{
    return false;
}

void lmColorsOptPanel::Apply()
{
}