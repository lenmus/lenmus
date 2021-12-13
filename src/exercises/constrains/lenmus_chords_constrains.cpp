//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2021 LenMus project
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
#include "lenmus_chords_constrains.h"

#include "lenmus_generators.h"

//lomse
#include <lomse_logger.h>
using namespace lomse;


namespace lenmus
{

//---------------------------------------------------------------------------------------
ChordConstrains::ChordConstrains(wxString sSection, ApplicationScope& appScope)
    : ExerciseOptions(sSection, appScope)
{
    load_settings();
}

//---------------------------------------------------------------------------------------
bool ChordConstrains::IsValidGroup(EChordGroup nGroup)
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

//---------------------------------------------------------------------------------------
void ChordConstrains::save_settings()
{
    if (m_appScope.get_exercises_level() != 100)
        return;

    wxConfigBase* pPrefs = m_appScope.get_preferences();

    // allowed chords
    for (int i=0; i < ect_MaxInExercises; i++)
    {
        wxString sKey = wxString::Format("/Constrains/IdfyChord/%s/Chord%dAllowed",
            GetSection(), i );
        pPrefs->Write(sKey, m_fValidChords[i]);
    }

    // play modes
    for (int i=0; i < 3; i++)
    {
        wxString sKey = wxString::Format("/Constrains/IdfyChord/%s/PlayMode%d",
            GetSection(), i );
        pPrefs->Write(sKey, m_fAllowedModes[i]);
    }

    // key signatures
    bool fValid;
    for (int i=k_min_key; i <= k_max_key; i++) {
        wxString sKey = wxString::Format("/Constrains/IdfyChord/%s/KeySignature%d",
            GetSection(), i );
        fValid = m_oValidKeys.IsValid((EKeySignature)i);
        pPrefs->Write(sKey, fValid);
    }

    // other settings
    wxString sKey = wxString::Format("/Constrains/IdfyChord/%s/AllowInversions",
                        GetSection());
    pPrefs->Write(sKey, m_fAllowInversions);
    sKey = wxString::Format("/Constrains/IdfyChord/%s/DisplayKey", GetSection());
    pPrefs->Write(sKey, m_fDisplayKey);
}

//---------------------------------------------------------------------------------------
void ChordConstrains::load_settings()
{
    switch (m_appScope.get_exercises_level())
    {
        case 1: load_settings_for_level_1();        break;
        case 2: load_settings_for_level_2();        break;
        default:
            load_settings_for_customized_level();
    }
}

//---------------------------------------------------------------------------------------
void ChordConstrains::load_settings_for_customized_level()
{
    wxConfigBase* pPrefs = m_appScope.get_preferences();

    // allowed chords. Default: four main triads
    for (int i=0; i < ect_MaxInExercises; i++)
    {
        wxString sKey = wxString::Format("/Constrains/IdfyChord/%s/Chord%dAllowed",
            GetSection(), i );
        pPrefs->Read(sKey, &m_fValidChords[i], (bool)(i < 4) );
    }

    // play modes. Default: only harmonic
    for (int i=0; i < 3; i++)
    {
        wxString sKey = wxString::Format("/Constrains/IdfyChord/%s/PlayMode%d",
            GetSection(), i );
        pPrefs->Read(sKey, &m_fAllowedModes[i], (i == 0));
    }

    // key signatures. Default use C major
    bool fValid;
    for (int i=k_min_key; i <= k_max_key; i++)
    {
        wxString sKey = wxString::Format("/Constrains/IdfyChord/%s/KeySignature%d",
            GetSection(), i );
        pPrefs->Read(sKey, &fValid, (bool)((EKeySignature)i == k_key_C) );
        m_oValidKeys.SetValid((EKeySignature)i, fValid);
    }

    // other settings:
    //      Inversions - default: not allowed
    //      Display key - default: not allowed
    wxString sKey = wxString::Format("/Constrains/IdfyChord/%s/AllowInversions",
                        GetSection());
    pPrefs->Read(sKey, &m_fAllowInversions, false);
    sKey = wxString::Format("/Constrains/IdfyChord/%s/DisplayKey",
                        GetSection());
    pPrefs->Read(sKey, &m_fDisplayKey, false);
}

//---------------------------------------------------------------------------------------
void ChordConstrains::load_settings_for_level_1()
{
//    if (GetSection() == "EarIdfyChord")
//    {
        // allowed chords
        for (int i=0; i < ect_MaxInExercises; i++)
            m_fValidChords[i] = false;

        m_fValidChords[ect_MajorTriad] = true;
        m_fValidChords[ect_MinorTriad] = true;
        m_fValidChords[ect_DominantSeventh] = true;

        // play modes
        m_fAllowedModes[0] = true;      // 0-harmonic
        m_fAllowedModes[1] = true;      // 1-melodic ascending
        m_fAllowedModes[2] = false;     // 2-melodic descending

        //key signatures
        for (int i=k_min_key; i <= k_max_key; i++)
            m_oValidKeys.SetValid((EKeySignature)i, false);

        m_oValidKeys.SetValid(k_key_C, true);
        m_oValidKeys.SetValid(k_key_a, true);
        m_oValidKeys.SetValid(k_key_G, true);
        m_oValidKeys.SetValid(k_key_e, true);
        m_oValidKeys.SetValid(k_key_F, true);
        m_oValidKeys.SetValid(k_key_d, true);

        //other settings:
        m_fAllowInversions = false;     //inversions allowed?
        m_fDisplayKey = true;           //display key signature?

        SetSettingsLink(false);
//    }
//    else if (GetSection() == "TheoIdfyChord")
//    {
//    }
//    else
//        load_settings_for_customized_level();
}

//---------------------------------------------------------------------------------------
void ChordConstrains::load_settings_for_level_2()
{
//    if (GetSection() == "EarIdfyChord")
//    {
        // allowed chords
        for (int i=0; i < ect_MaxInExercises; i++)
            m_fValidChords[i] = false;

        m_fValidChords[ect_MajorTriad] = true;
        m_fValidChords[ect_MinorTriad] = true;
        m_fValidChords[ect_DominantSeventh] = true;
        m_fValidChords[ect_AugTriad] = true;
        m_fValidChords[ect_DimTriad] = true;
        m_fValidChords[ect_MajorSeventh] = true;
        m_fValidChords[ect_MinorSeventh] = true;
        m_fValidChords[ect_DimSeventh] = true;

        // play modes
        m_fAllowedModes[0] = true;      // 0-harmonic
        m_fAllowedModes[1] = true;      // 1-melodic ascending
        m_fAllowedModes[2] = false;     // 2-melodic descending

        //key signatures
        for (int i=k_min_key; i <= k_max_key; i++)
            m_oValidKeys.SetValid((EKeySignature)i, false);

        m_oValidKeys.SetValid(k_key_C, true);
        m_oValidKeys.SetValid(k_key_a, true);
        m_oValidKeys.SetValid(k_key_G, true);
        m_oValidKeys.SetValid(k_key_e, true);
        m_oValidKeys.SetValid(k_key_F, true);
        m_oValidKeys.SetValid(k_key_d, true);
        m_oValidKeys.SetValid(k_key_D, true);
        m_oValidKeys.SetValid(k_key_b, true);
        m_oValidKeys.SetValid(k_key_Bf, true);
        m_oValidKeys.SetValid(k_key_g, true);

        //other settings:
        m_fAllowInversions = false;     //inversions allowed?
        m_fDisplayKey = true;           //display key signature?

        SetSettingsLink(false);
//    }
//    else if (GetSection() == "TheoIdfyChord")
//    {
//    }
//    else
//        load_settings_for_customized_level();
}

//---------------------------------------------------------------------------------------
EChordType ChordConstrains::GetRandomChordType()
{
    RandomGenerator oGenerator;
    int nWatchDog = 0;
    int nType = oGenerator.random_number(0, ect_MaxInExercises-1);
    while (!IsChordValid((EChordType)nType))
    {
        nType = oGenerator.random_number(0, ect_MaxInExercises-1);
        if (nWatchDog++ == 1000)
        {
            LOMSE_LOG_ERROR("Program error: Loop detected");
            return (EChordType)0;
        }
    }
    return (EChordType)nType;
}

//---------------------------------------------------------------------------------------
int ChordConstrains::GetRandomMode()
{
    RandomGenerator oGenerator;
    int nWatchDog = 0;
    int nMode = oGenerator.random_number(0, 2);
    while (!IsModeAllowed(nMode))
    {
        nMode = oGenerator.random_number(0, 2);
        if (nWatchDog++ == 1000) {
            return 0;   //harmonic
        }
    }
    return nMode;
}


}  //namespace lenmus
