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
    : lmExerciseOptions(sSection)
{
    m_sSection = sSection;
    m_fSettingsLink = false;
    LoadSettings();
}

bool lmChordConstrains::IsValidGroup(EChordGroup nGroup)
{
    if (nGroup == ecg_Triads)
    {
        return (IsChordValid( ect_MajorTriad ) ||
                IsChordValid( ect_MinorTriad ) ||
                IsChordValid( ect_AugTriad ) ||
                IsChordValid( ect_DimTriad ) ||
                IsChordValid( ect_Suspended_4th ) ||
                IsChordValid( ect_Suspended_2nd ) );
    }
    else if(nGroup == ecg_Sevenths)
    {
        return (IsChordValid( ect_MajorSeventh ) ||
                IsChordValid( ect_DominantSeventh ) ||
                IsChordValid( ect_MinorSeventh ) ||
                IsChordValid( ect_DimSeventh ) ||
                IsChordValid( ect_HalfDimSeventh ) ||
                IsChordValid( ect_AugMajorSeventh ) ||
                IsChordValid( ect_AugSeventh ) ||
                IsChordValid( ect_MinorMajorSeventh ) );
    }
    else if(nGroup == ecg_Sixths)
    {
        return (IsChordValid( ect_MajorSixth ) ||
                IsChordValid( ect_MinorSixth ) ||
                IsChordValid( ect_AugSixth ) );
    }
    else {
        wxASSERT(false);    //impossible
        return false;
    }

}

void lmChordConstrains::SaveSettings()
{
    //
    // save settings in user configuration data file
    //

    // allowed chords
    int i;
    wxString sKey;
    for (i=0; i < ect_Max; i++) {
        sKey = wxString::Format(_T("/Constrains/IdfyChord/%s/Chord%dAllowed"),
            m_sSection.c_str(), i );
        g_pPrefs->Write(sKey, m_fValidChords[i]);
    }

    // play modes
    for (i=0; i < 3; i++) {
        sKey = wxString::Format(_T("/Constrains/IdfyChord/%s/PlayMode%d"),
            m_sSection.c_str(), i );
        g_pPrefs->Write(sKey, m_fAllowedModes[i]);
    }

    // key signatures
    bool fValid;
    for (i=lmMIN_KEY; i <= lmMAX_KEY; i++) {
        sKey = wxString::Format(_T("/Constrains/IdfyChord/%s/KeySignature%d"),
            m_sSection.c_str(), i );
        fValid = m_oValidKeys.IsValid((EKeySignatures)i);
        g_pPrefs->Write(sKey, fValid);
    }

    // other settings
    sKey = wxString::Format(_T("/Constrains/IdfyChord/%s/AllowInversions"),
                        m_sSection.c_str());
    g_pPrefs->Write(sKey, m_fAllowInversions);
    sKey = wxString::Format(_T("/Constrains/IdfyChord/%s/DisplayKey"),
                        m_sSection.c_str());
    g_pPrefs->Write(sKey, m_fDisplayKey);

}

void lmChordConstrains::LoadSettings()
{
    //
    // load settings form user configuration data or default values
    //

    // allowed chords. Default: four main triads
    int i;
    wxString sKey;
    for (i=0; i < ect_Max; i++) {
        sKey = wxString::Format(_T("/Constrains/IdfyChord/%s/Chord%dAllowed"),
            m_sSection.c_str(), i );
        g_pPrefs->Read(sKey, &m_fValidChords[i], (bool)(i < 4) );
    }

    // play modes. Default: only harmonic, but is set in IdfyChrdCtrolParms when the
    // control is created. This is necesary to simplify param settings
    for (i=0; i < 3; i++) {
        sKey = wxString::Format(_T("/Constrains/IdfyChord/%s/PlayMode%d"),
            m_sSection.c_str(), i );
        g_pPrefs->Read(sKey, &m_fAllowedModes[i], false);
    }

    // key signatures. Default use C major
    bool fValid;
    for (i=lmMIN_KEY; i <= lmMAX_KEY; i++) {
        sKey = wxString::Format(_T("/Constrains/IdfyChord/%s/KeySignature%d"),
            m_sSection.c_str(), i );
        g_pPrefs->Read(sKey, &fValid, (bool)((EKeySignatures)i == earmDo) );
        m_oValidKeys.SetValid((EKeySignatures)i, fValid);
    }

    // other settings:
    //      Inversions - default: not allowed
    //      Display key - default: not allowed
    sKey = wxString::Format(_T("/Constrains/IdfyChord/%s/AllowInversions"),
                        m_sSection.c_str());
    g_pPrefs->Read(sKey, &m_fAllowInversions, false);
    sKey = wxString::Format(_T("/Constrains/IdfyChord/%s/DisplayKey"),
                        m_sSection.c_str());
    g_pPrefs->Read(sKey, &m_fDisplayKey, false);

}

EChordType lmChordConstrains::GetRandomChordType()
{
    lmRandomGenerator oGenerator;
    int nWatchDog = 0;
    int nType = oGenerator.RandomNumber(0, ect_Max-1);
    while (!IsChordValid((EChordType)nType)) {
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

