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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "CadencesConstrains.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "CadencesConstrains.h"
#include "Generators.h"

// the config object
extern wxConfigBase *g_pPrefs;


lmCadencesConstrains::lmCadencesConstrains(wxString sSection)
    : lmIdfyConstrains(sSection)
{
    //
    // default settings
    //

    // allowed cadences. Default: all allowed
    for (int i=0; i < lm_eCadMaxCadence; i++) {
        m_fValidCadences[i] = true;
    }

    // key signatures. Default use C major
    for (int i=lmMIN_KEY; i <= lmMAX_KEY; i++) {
        bool fValid = ((EKeySignatures)i == earmDo);
        m_oValidKeys.SetValid((EKeySignatures)i, fValid);
    }

    // answer buttons. Default: transient / terminal
    for (int i=0; i < lm_eCadMaxButton; i++) {
        m_fValidButtons[i] = (i < 2);
    }

    // Use grand staff. Default: true
    m_fGrandStaff = true;

    // how to display key. Default: play tonic chord
    m_nKeyDisplayMode = 1;

}

void lmCadencesConstrains::SaveSettings()
{
    //
    // save settings in user configuration data file
    //

    // allowed cadences
    int i;
    wxString sKey;
    for (i=0; i < lm_eCadMaxCadence; i++) {
        sKey = wxString::Format(_T("/Constrains/IdfyCadence/%s/Cadence%dAllowed"),
            m_sSection.c_str(), i );
        g_pPrefs->Write(sKey, m_fValidCadences[i]);
    }

    // key signatures
    bool fValid;
    for (i=lmMIN_KEY; i <= lmMAX_KEY; i++) {
        sKey = wxString::Format(_T("/Constrains/IdfyCadence/%s/KeySignature%d"),
            m_sSection.c_str(), i );
        fValid = m_oValidKeys.IsValid((EKeySignatures)i);
        g_pPrefs->Write(sKey, fValid);
    }

    // answer buttons
    for (i=0; i < lm_eCadMaxButton; i++) {
        sKey = wxString::Format(_T("/Constrains/IdfyCadence/%s/Button%dAllowed"),
            m_sSection.c_str(), i );
        g_pPrefs->Write(sKey, m_fValidButtons[i]);
    }

    // some options flags
    sKey = _T("/Constrains/IdfyCadence/%s/UseGrandStaff");
    g_pPrefs->Write(sKey, m_fGrandStaff);

    // how to display key
    sKey = wxString::Format(_T("/Constrains/IdfyCadence/%s/DisplayKeyMode"), m_sSection.c_str());
    g_pPrefs->Write(sKey, m_nKeyDisplayMode);

}

void lmCadencesConstrains::LoadSettings()
{
    //
    // load settings form user configuration data or default values
    //

    // allowed cadences. Default: all allowed
    int i;
    wxString sKey;
    for (i=0; i < lm_eCadMaxCadence; i++) {
        sKey = wxString::Format(_T("/Constrains/IdfyCadence/%s/Cadence%dAllowed"),
            m_sSection.c_str(), i );
        g_pPrefs->Read(sKey, &m_fValidCadences[i], true );
    }

    // key signatures. Default use C major
    bool fValid;
    for (i=lmMIN_KEY; i <= lmMAX_KEY; i++) {
        sKey = wxString::Format(_T("/Constrains/IdfyCadence/%s/KeySignature%d"),
            m_sSection.c_str(), i );
        g_pPrefs->Read(sKey, &fValid, (bool)((EKeySignatures)i == earmDo) );
        m_oValidKeys.SetValid((EKeySignatures)i, fValid);
    }

    // answer buttons. Default: transient / terminal
    for (i=0; i < lm_eCadMaxButton; i++) {
        sKey = wxString::Format(_T("/Constrains/IdfyCadence/%s/Button%dAllowed"),
            m_sSection.c_str(), i );
        g_pPrefs->Read(sKey, &m_fValidButtons[i], (bool)(i < 2) );
    }

    // some options flags
    sKey = _T("/Constrains/IdfyCadence/%s/UseGrandStaff");
    g_pPrefs->Read(sKey, &m_fGrandStaff, true );

    // how to display key. Default: play tonic chord
    sKey = wxString::Format(_T("/Constrains/IdfyCadence/%s/DisplayKeyMode"), m_sSection.c_str());
    g_pPrefs->Read(sKey, &m_nKeyDisplayMode, 1);

}

lmECadenceType lmCadencesConstrains::GetRandomCadence()
{
    lmRandomGenerator oGenerator;
    int nWatchDog = 0;
    int nType = oGenerator.RandomNumber(0, lm_eCadMaxCadence-1);
    while (!IsCadenceValid((lmECadenceType)nType)) {
        nType = oGenerator.RandomNumber(0, lm_eCadMaxCadence-1);
        if (nWatchDog++ == 1000) {
            wxMessageBox(_("Program error: Loop detected in lmCadencesConstrains::GetRandomCadence."));
            return (lmECadenceType)0;
        }
    }
    return (lmECadenceType)nType;

}

