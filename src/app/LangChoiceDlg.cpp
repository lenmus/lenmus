//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "LangChoiceDlg.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/defs.h>
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/dialog.h>
#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

#include "wx/xrc/xmlres.h"


#include <wx/hashmap.h>

WX_DECLARE_STRING_HASH_MAP(wxString, LangHash);


#include "LangChoiceDlg.h"
#include "../options/Languages.h"

BEGIN_EVENT_TABLE(lmLangChoiceDlg, wxDialog)
    EVT_BUTTON(wxID_OK, lmLangChoiceDlg::OnOk)
END_EVENT_TABLE()


lmLangChoiceDlg::lmLangChoiceDlg(wxWindow* parent, wxWindowID id, const wxString& title):
   wxDialog(parent, id, title)
{
    GetLanguages(m_cLangCodes, m_cLangNames);
    m_nNumLangs = m_cLangNames.GetCount();

    wxString sysLang = GetSystemLanguageCode();

    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *hSizer;

    hSizer = new wxBoxSizer(wxHORIZONTAL);
    hSizer->Add(new wxStaticText(this, -1,
                                    _("Choose language to use:")),
                0, wxALIGN_CENTRE | wxALIGN_CENTER_VERTICAL | wxALL, 8);

    wxString *langArray = new wxString[m_nNumLangs];
    int i;
    for(i=0; i < m_nNumLangs; i++)
        langArray[i] = m_cLangNames[i];
    m_pChoice = new wxChoice(this, -1, wxDefaultPosition, wxDefaultSize,
                            m_nNumLangs, langArray);
    m_pChoice->SetSelection(0); // in case nothing else matches
    delete[] langArray;

    for(i=0; i < m_nNumLangs; i++) {
        if (m_cLangCodes[i] == sysLang)
            m_pChoice->SetSelection(i);
    }
    hSizer->Add(m_pChoice,
                0, wxALIGN_CENTRE | wxALIGN_CENTER_VERTICAL | wxALL, 8);

    mainSizer->Add(hSizer,
                    0, wxALL, 8);

    wxButton *ok = new wxButton(this, wxID_OK, _("OK"));
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

void lmLangChoiceDlg::OnOk(wxCommandEvent & event)
{
    m_sLang = m_cLangCodes[m_pChoice->GetSelection()];
    EndModal(true);
}

