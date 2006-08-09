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
/*! @file ChordConstrains.cpp
    @brief Implementation file for lmChordConstrains class
    @ingroup generators
*/
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "ChordConstrains.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "ChordConstrains.h"
#include "Generators.h"

// the config object
extern wxConfigBase *g_pPrefs;


lmChordConstrains::lmChordConstrains(wxString sSection)
{
    m_sSection = sSection;
    LoadSettings();

    //default values for constrains
    m_fAllowInversions = false;
    m_sLowerRoot = _T("a3");    //valid range for root notes
    m_sUpperRoot = _T("a4");

    //default values for valid chords
        // Triads
    SetValid( ect_MajorTriad, true);
    SetValid( ect_MinorTriad, true);
    SetValid( ect_AugTriad, false);
    SetValid( ect_DimTriad, false);
    SetValid( ect_Suspended_4th, true);
    SetValid( ect_Suspended_2nd, true);

        // Seventh chords
    SetValid( ect_MajorSeventh, true);
    SetValid( ect_DominantSeventh, true);
    SetValid( ect_MinorSeventh, true);
    SetValid( ect_DimSeventh, true);
    SetValid( ect_HalfDimSeventh, true);
    SetValid( ect_AugMajorSeventh, false);
    SetValid( ect_AugSeventh, false);
    SetValid( ect_MinorMajorSeventh, false);

        //// Sixth chords
    SetValid( ect_MajorSixth, true);
    SetValid( ect_MinorSixth, true);
    SetValid( ect_AugSixth, true);

    //default values for allowed modes
    SetModeAllowed(0, true);       // 0-harmonic
    SetModeAllowed(1, false);       // 1-melodic ascending
    SetModeAllowed(2, false);       // 2-melodic descending

    //Other options' default values
    m_fDisplayKey = false;          //do not display key signatures
    m_fTheoryMode = true;           //theory configuration

}

bool lmChordConstrains::IsValidGroup(EChordGroup nGroup)
{
    if (nGroup == ecg_Triads)
    {
        return (IsValid( ect_MajorTriad ) ||
                IsValid( ect_MinorTriad ) ||
                IsValid( ect_AugTriad ) ||
                IsValid( ect_DimTriad ) ||
                IsValid( ect_Suspended_4th ) ||
                IsValid( ect_Suspended_2nd ) );
    }
    else if(nGroup == ecg_Sevenths)
    {
        return (IsValid( ect_MajorSeventh ) ||
                IsValid( ect_DominantSeventh ) ||
                IsValid( ect_MinorSeventh ) ||
                IsValid( ect_DimSeventh ) ||
                IsValid( ect_HalfDimSeventh ) ||
                IsValid( ect_AugMajorSeventh ) ||
                IsValid( ect_AugSeventh ) ||
                IsValid( ect_MinorMajorSeventh ) );
    }
    else if(nGroup == ecg_Sixths)
    {
        return (IsValid( ect_MajorSixth ) ||
                IsValid( ect_MinorSixth ) ||
                IsValid( ect_AugSixth ) );
    }
    else {
        wxASSERT(false);    //impossible
        return false;
    }

}

void lmChordConstrains::SaveSettings()
{
    /*
    save settings in user configuration data file
    */

    /*
    // allowed intervals
    int i;
    wxString sKey;
    for (i=0; i < lmNUM_INTVALS; i++) {
        sKey = wxString::Format(_T("/Constrains/EarIntval/%s/Interval%dAllowed"), 
            m_sSection, i );
        g_pPrefs->Write(sKey, m_fIntervalAllowed[i]);
    }

    // notes range
    sKey = wxString::Format(_T("/Constrains/EarIntval/%s/MinPitch"), m_sSection);
    g_pPrefs->Write(sKey, (long)m_nMinPitch);
    sKey = wxString::Format(_T("/Constrains/EarIntval/%s/MaxPitch"), m_sSection);
    g_pPrefs->Write(sKey, (long)m_nMaxPitch);

    // intervals types
    for (i=0; i < 3; i++) {
        sKey = wxString::Format(_T("/Constrains/EarIntval/%s/IntervalType%d"), 
            m_sSection, i );
        g_pPrefs->Write(sKey, m_fTypeAllowed[i]);
    }

    // accidentals and key signatures
    sKey = wxString::Format(_T("/Constrains/EarIntval/%s/OnlyNatural"), m_sSection);
    g_pPrefs->Write(sKey, m_fOnlyNatural);
    bool fValid;
    for (i=lmMIN_KEY; i <= lmMAX_KEY; i++) {
        sKey = wxString::Format(_T("/Constrains/EarIntval/%s/KeySignature%d"), 
            m_sSection, i );
        fValid = m_oValidKeys.IsValid((EKeySignatures)i);
        g_pPrefs->Write(sKey, fValid);
    }

    // for interval comparison exercises
    sKey = wxString::Format(_T("/Constrains/EarIntval/%s/FirstEqual"), m_sSection);
    g_pPrefs->Write(sKey, m_fFirstEqual);
    */

}

void lmChordConstrains::LoadSettings()
{
    /*
    load settings form user configuration data or default values
    */

    /*
    // allowed intervals. Default: all in one octave range
    int i;
    wxString sKey;
    for (i=0; i < lmNUM_INTVALS; i++) {
        sKey = wxString::Format(_T("/Constrains/EarIntval/%s/Interval%dAllowed"), 
            m_sSection, i );
        g_pPrefs->Read(sKey, &m_fIntervalAllowed[i], (bool)(i < 13) );
    }

    // notes range. Default A3 to A5
    sKey = wxString::Format(_T("/Constrains/EarIntval/%s/MinPitch"), m_sSection);
    m_nMinPitch = (int) g_pPrefs->Read(sKey, 27L);      // 27 = A3
    sKey = wxString::Format(_T("/Constrains/EarIntval/%s/MaxPitch"), m_sSection);
    m_nMaxPitch = (int) g_pPrefs->Read(sKey, 41L);      // 41 = A5

    // intervals types. Default: all types allowed
    for (i=0; i < 3; i++) {
        sKey = wxString::Format(_T("/Constrains/EarIntval/%s/IntervalType%d"), 
            m_sSection, i );
        g_pPrefs->Read(sKey, &m_fTypeAllowed[i], true);
    }

    // accidentals and key signatures. Default use only natual intervals from C major scale
    sKey = wxString::Format(_T("/Constrains/EarIntval/%s/OnlyNatural"), m_sSection);
    g_pPrefs->Read(sKey, &m_fOnlyNatural, true);    //use only natural intervals
    bool fValid;
    for (i=lmMIN_KEY; i <= lmMAX_KEY; i++) {
        sKey = wxString::Format(_T("/Constrains/EarIntval/%s/KeySignature%d"), 
            m_sSection, i );
        g_pPrefs->Read(sKey, &fValid, (bool)((EKeySignatures)i == earmDo) );
        m_oValidKeys.SetValid((EKeySignatures)i, fValid);
    }

    // for interval comparison exercises
    sKey = wxString::Format(_T("/Constrains/EarIntval/%s/FirstEqual"), m_sSection);
    g_pPrefs->Read(sKey, &m_fFirstEqual, true);    // first note equal in both intervals
    */

}

EChordType lmChordConstrains::GetRandomChordType()
{
    lmRandomGenerator oGenerator;
    int nWatchDog = 0;
    int nType = oGenerator.RandomNumber(0, ect_Max-1);
    while (!IsValid((EChordType)nType)) {
        nType = oGenerator.RandomNumber(0, ect_Max-1);
        if (nWatchDog++ == 1000) {
            wxMessageBox(_("Program error: Loop detected in lmChordConstrains::GetRandomChordType."));
            return (EChordType)0;
        }
    }
    return (EChordType)nType;

}

int lmChordConstrains::GetRandomMode()
{
    lmRandomGenerator oGenerator;
    int nWatchDog = 0;
    int nMode = oGenerator.RandomNumber(0, 2);
    while (!IsModeAllowed(nMode)) {
        nMode = oGenerator.RandomNumber(0, 2);
        if (nWatchDog++ == 1000) {
            return 0;   //harmonic
        }
    }
    return nMode;

}

