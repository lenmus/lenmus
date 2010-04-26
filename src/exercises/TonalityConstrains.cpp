//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-1010 LenMus project
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
#pragma implementation "TonalityConstrains.h"
#endif

// For compilers that support precompilation, includes <wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "TonalityConstrains.h"
#include "Generators.h"

// the config object
extern wxConfigBase *g_pPrefs;


lmTonalityConstrains::lmTonalityConstrains(wxString sSection)
    : lmExerciseOptions(sSection)
{
    //
    // default settings
    //

    //answer buttons: only major/minor buttons
    m_fUseMajorMinorButtons = true;

    // key signatures and . Default: all
    for (int i=lmMIN_KEY; i <= lmMAX_KEY; i++)
    {
        lmEKeySignatures nKey = static_cast<lmEKeySignatures>(i);
        m_oValidKeys.SetValid(nKey, true);
    }

    //always ear training
    SetTheoryMode(false);
}

void lmTonalityConstrains::SaveSettings()
{
    // save settings in user configuration data file

    // allowed key signatures
    int i;
    wxString sKey;
    bool fValid;
    for (i=lmMIN_KEY; i <= lmMAX_KEY; i++)
    {
        sKey = wxString::Format(_T("/Constrains/IdfyTonality/%s/KeySignature%d"),
            m_sSection.c_str(), i );
        fValid = m_oValidKeys.IsValid((lmEKeySignatures)i);
        g_pPrefs->Write(sKey, fValid);
    }

    //answer buttons
    sKey = _T("/Constrains/IdfyTonality/UseMajorMinorButtons");
    g_pPrefs->Write(sKey, m_fUseMajorMinorButtons);
}

void lmTonalityConstrains::LoadSettings()
{
    // load settings form user configuration data or default values

    // allowed key signatures. Default: all allowed
    int i;
    wxString sKey;
    bool fValid;
    for (i=lmMIN_KEY; i <= lmMAX_KEY; i++)
    {
        sKey = wxString::Format(_T("/Constrains/IdfyTonality/%s/KeySignature%d"),
            m_sSection.c_str(), i );
        g_pPrefs->Read(sKey, &fValid, true);
        m_oValidKeys.SetValid((lmEKeySignatures)i, fValid);
    }

    //answer buttons. Default: use major/minor buttons
    sKey = _T("/Constrains/IdfyTonality/UseMajorMinorButtons");
    g_pPrefs->Read(sKey, &m_fUseMajorMinorButtons, true);
}

