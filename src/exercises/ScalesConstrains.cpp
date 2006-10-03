//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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
/*! @file ScalesConstrains.cpp
    @brief Implementation file for lmScalesConstrains class
    @ingroup generators
*/
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "ScalesConstrains.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "ScalesConstrains.h"
#include "Generators.h"

// the config object
extern wxConfigBase *g_pPrefs;


lmScalesConstrains::lmScalesConstrains(wxString sSection)
    : lmEarConstrains(sSection)
{
}

bool lmScalesConstrains::IsValidGroup(EScaleGroup nGroup)
{
    if (nGroup == esg_Major)
    {
        return (IsScaleValid( est_MajorNatural ) ||
                IsScaleValid( est_MajorTypeII ) ||
                IsScaleValid( est_MajorTypeIII ) ||
                IsScaleValid( est_MajorTypeIV ) );
    }
    else if(nGroup == esg_Minor)
    {
        return (IsScaleValid( est_MinorNatural ) ||
                IsScaleValid( est_MinorDorian ) ||
                IsScaleValid( est_MinorHarmonic ) ||
                IsScaleValid( est_MinorMelodic ) );
    }
    else if(nGroup == esg_Greek)
    {
        return (IsScaleValid( est_GreekDorian ) ||
                IsScaleValid( est_GreekPhrygian ) ||
                IsScaleValid( est_GreekLydian ) );
    }
    else if(nGroup == esg_Other)
    {
        return (IsScaleValid( est_GreekDorian ) ||
                IsScaleValid( est_GreekPhrygian ) ||
                IsScaleValid( est_GreekLydian ) );
    }
    else {
        wxASSERT(false);    //impossible
        return false;
    }

}

void lmScalesConstrains::SaveSettings()
{
    //
    // save settings in user configuration data file
    //

    // allowed chords
    int i;
    wxString sKey;
    for (i=0; i < est_Max; i++) {
        sKey = wxString::Format(_T("/Constrains/IdfyScale/%s/Scale%dAllowed"), 
            m_sSection, i );
        g_pPrefs->Write(sKey, m_fValidScales[i]);
    }

    // play modes
    g_pPrefs->Write(_T("/Constrains/IdfyScale/%s/PlayMode"), m_nPlayMode);

    // key signatures
    bool fValid;
    for (i=lmMIN_KEY; i <= lmMAX_KEY; i++) {
        sKey = wxString::Format(_T("/Constrains/IdfyScale/%s/KeySignature%d"), 
            m_sSection, i );
        fValid = m_oValidKeys.IsValid((EKeySignatures)i);
        g_pPrefs->Write(sKey, fValid);
    }

    // other settings
    sKey = wxString::Format(_T("/Constrains/IdfyScale/%s/AllowInversions"), m_sSection);
    g_pPrefs->Write(sKey, m_fAllowInversions);
    sKey = wxString::Format(_T("/Constrains/IdfyScale/%s/DisplayKey"), m_sSection);
    g_pPrefs->Write(sKey, m_fDisplayKey);

}

void lmScalesConstrains::LoadSettings()
{
    //
    // load settings form user configuration data or default values
    //

    // allowed chords. Default: major and minor scales
    int i;
    wxString sKey;
    for (i=0; i < est_Max; i++) {
        sKey = wxString::Format(_T("/Constrains/IdfyScale/%s/Scale%dAllowed"), 
            m_sSection, i );
        g_pPrefs->Read(sKey, &m_fValidScales[i], (bool)(i < 8) );
    }

    // play modes. Default: ascending
    g_pPrefs->Read(_T("/Constrains/IdfyScale/%s/PlayMode"), &m_nPlayMode, 0);

    // key signatures. Default use C major
    bool fValid;
    for (i=lmMIN_KEY; i <= lmMAX_KEY; i++) {
        sKey = wxString::Format(_T("/Constrains/IdfyScale/%s/KeySignature%d"), 
            m_sSection, i );
        g_pPrefs->Read(sKey, &fValid, (bool)((EKeySignatures)i == earmDo) );
        m_oValidKeys.SetValid((EKeySignatures)i, fValid);
    }

    // other settings:
    //      Inversions - default: not allowed
    //      Display key - default: not allowed
    sKey = wxString::Format(_T("/Constrains/IdfyScale/%s/AllowInversions"), m_sSection);
    g_pPrefs->Read(sKey, &m_fAllowInversions, false);
    sKey = wxString::Format(_T("/Constrains/IdfyScale/%s/DisplayKey"), m_sSection);
    g_pPrefs->Read(sKey, &m_fDisplayKey, false);

}

EScaleType lmScalesConstrains::GetRandomScaleType()
{
    lmRandomGenerator oGenerator;
    int nWatchDog = 0;
    int nType = oGenerator.RandomNumber(0, est_Max-1);
    while (!IsScaleValid((EScaleType)nType)) {
        nType = oGenerator.RandomNumber(0, est_Max-1);
        if (nWatchDog++ == 1000) {
            wxMessageBox(_("Program error: Loop detected in lmScalesConstrains::GetRandomChordType."));
            return (EScaleType)0;
        }
    }
    return (EScaleType)nType;

}

bool lmScalesConstrains::GetRandomPlayMode()
{
    //return 'true' for ascending and 'false' for descending

    if (m_nPlayMode == 0) //ascending
        return true;
    else if (m_nPlayMode == 1) //descending
        return false;
    else {  // both modes allowed. Choose one at random
        lmRandomGenerator oGenerator;
        return oGenerator.FlipCoin();
   }
}

