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
//-------------------------------------------------------------------------------------/

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "OtherOptionsPanel.h"
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

#include "OtherOptionsPanel.h"

//access to preferences object
#include "wx/config.h"
extern wxConfigBase* g_pPrefs;

//access to global flag
#include "../app/Preferences.h"
extern bool g_fAnswerSoundsEnabled;
extern bool g_fTeamCounters;
extern bool g_fAutoNewProblem;



lmOtherOptionsPanel::lmOtherOptionsPanel(wxWindow* parent)
{
    // create the panel
    wxXmlResource::Get()->LoadPanel(this, parent, _T("OtherOptionsPanel"));

    //load icon
    wxStaticBitmap* pBmpIcon = XRCCTRL(*this, "bmpIconTitle", wxStaticBitmap);
    pBmpIcon->SetBitmap( wxArtProvider::GetIcon(_T("opt_other"), wxART_TOOLBAR, wxSize(24,24)) );

    //store pointers to controls
    m_pChkAnswerSounds = XRCCTRL(*this, "chkAnswerSounds", wxCheckBox);
    m_pChkTeamCounters = XRCCTRL(*this, "chkTeamCounters", wxCheckBox);
    m_pChkAutoNewProblem = XRCCTRL(*this, "chkAutoNewProblem", wxCheckBox);

        //Select current settings

    // Exercises options
    m_pChkAnswerSounds->SetValue(g_fAnswerSoundsEnabled);
    m_pChkTeamCounters->SetValue(g_fTeamCounters);
    m_pChkAutoNewProblem->SetValue(g_fAutoNewProblem);

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

    g_fTeamCounters = m_pChkTeamCounters->GetValue();
    g_pPrefs->Write(_T("/Options/TeamCounters"), g_fTeamCounters);

    g_fAutoNewProblem = m_pChkAutoNewProblem->GetValue();
    g_pPrefs->Write(_T("/Options/AutoNewProblem"), g_fAutoNewProblem);

}
