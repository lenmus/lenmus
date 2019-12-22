//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2015 LenMus project
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
    //
    // save settings in user configuration data file
    //

    wxConfigBase* pPrefs = m_appScope.get_preferences();

    // allowed chords
    for (int i=0; i < ect_MaxInExercises; i++)
    {
        wxString sKey = wxString::Format("/Constrains/IdfyChord/%s/Chord%dAllowed",
            m_sSection.wx_str(), i );
        pPrefs->Write(sKey, m_fValidChords[i]);
    }

    // play modes
    for (int i=0; i < 3; i++)
    {
        wxString sKey = wxString::Format("/Constrains/IdfyChord/%s/PlayMode%d",
            m_sSection.wx_str(), i );
        pPrefs->Write(sKey, m_fAllowedModes[i]);
    }

    // key signatures
    bool fValid;
    for (int i=k_min_key; i <= k_max_key; i++) {
        wxString sKey = wxString::Format("/Constrains/IdfyChord/%s/KeySignature%d",
            m_sSection.wx_str(), i );
        fValid = m_oValidKeys.IsValid((EKeySignature)i);
        pPrefs->Write(sKey, fValid);
    }

    // other settings
    wxString sKey = wxString::Format("/Constrains/IdfyChord/%s/AllowInversions",
                        m_sSection.wx_str());
    pPrefs->Write(sKey, m_fAllowInversions);
    sKey = wxString::Format("/Constrains/IdfyChord/%s/DisplayKey",
                        m_sSection.wx_str());
    pPrefs->Write(sKey, m_fDisplayKey);
}

//---------------------------------------------------------------------------------------
void ChordConstrains::load_settings()
{
    //
    // load settings form user configuration data or default values
    //

    wxConfigBase* pPrefs = m_appScope.get_preferences();

    // allowed chords. Default: four main triads
    for (int i=0; i < ect_MaxInExercises; i++)
    {
        wxString sKey = wxString::Format("/Constrains/IdfyChord/%s/Chord%dAllowed",
            m_sSection.wx_str(), i );
        pPrefs->Read(sKey, &m_fValidChords[i], (bool)(i < 4) );
    }

    // play modes. Default: only harmonic
    for (int i=0; i < 3; i++)
    {
        wxString sKey = wxString::Format("/Constrains/IdfyChord/%s/PlayMode%d",
            m_sSection.wx_str(), i );
        pPrefs->Read(sKey, &m_fAllowedModes[i], (i == 0));
    }

    // key signatures. Default use C major
    bool fValid;
    for (int i=k_min_key; i <= k_max_key; i++)
    {
        wxString sKey = wxString::Format("/Constrains/IdfyChord/%s/KeySignature%d",
            m_sSection.wx_str(), i );
        pPrefs->Read(sKey, &fValid, (bool)((EKeySignature)i == k_key_C) );
        m_oValidKeys.SetValid((EKeySignature)i, fValid);
    }

    // other settings:
    //      Inversions - default: not allowed
    //      Display key - default: not allowed
    wxString sKey = wxString::Format("/Constrains/IdfyChord/%s/AllowInversions",
                        m_sSection.wx_str());
    pPrefs->Read(sKey, &m_fAllowInversions, false);
    sKey = wxString::Format("/Constrains/IdfyChord/%s/DisplayKey",
                        m_sSection.wx_str());
    pPrefs->Read(sKey, &m_fDisplayKey, false);
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
