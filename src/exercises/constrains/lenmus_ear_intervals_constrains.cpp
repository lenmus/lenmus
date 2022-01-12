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
#include "lenmus_constrains.h"
#include "lenmus_ear_intervals_constrains.h"

//wxWidgets
//#include <wx/wxprec.h>


namespace lenmus
{

//---------------------------------------------------------------------------------------
EarIntervalsConstrains::EarIntervalsConstrains(wxString sSection,
                                               ApplicationScope& appScope)
    : ExerciseOptions(sSection, appScope)
{
    load_settings();
}

//---------------------------------------------------------------------------------------
void EarIntervalsConstrains::save_settings()
{
    //save settings in user configuration data file

    if (m_appScope.get_exercises_level() != 100)
        return;

    wxConfigBase* pPrefs = m_appScope.get_preferences();

    // allowed intervals
    int i;
    wxString sKey;
    for (i=0; i < lmNUM_INTVALS; i++) {
        sKey = wxString::Format("/Constrains/EarIntval/%s/Interval%dAllowed",
            GetSection(), i );
        pPrefs->Write(sKey, m_fIntervalAllowed[i]);
    }

    // notes range
    sKey = wxString::Format("/Constrains/EarIntval/%s/MinPitch", GetSection());
    pPrefs->Write(sKey, (long)m_nMinPitch);
    sKey = wxString::Format("/Constrains/EarIntval/%s/MaxPitch", GetSection());
    pPrefs->Write(sKey, (long)m_nMaxPitch);

    // intervals types
    for (i=0; i < 3; i++) {
        sKey = wxString::Format("/Constrains/EarIntval/%s/IntervalType%d",
            GetSection(), i );
        pPrefs->Write(sKey, m_fTypeAllowed[i]);
    }

    // accidentals and key signatures
    sKey = wxString::Format("/Constrains/EarIntval/%s/OnlyNatural", GetSection());
    pPrefs->Write(sKey, m_fOnlyNatural);
    bool fValid;
    for (i=k_min_key; i <= k_max_key; i++) {
        sKey = wxString::Format("/Constrains/EarIntval/%s/KeySignature%d",
            GetSection(), i );
        fValid = m_oValidKeys.IsValid((EKeySignature)i);
        pPrefs->Write(sKey, fValid);
    }

    // for interval comparison exercises
    sKey = wxString::Format("/Constrains/EarIntval/%s/FirstEqual", GetSection());
    pPrefs->Write(sKey, m_fFirstEqual);

}

//---------------------------------------------------------------------------------------
void EarIntervalsConstrains::load_settings()
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
void EarIntervalsConstrains::load_settings_for_customized_level()
{
    wxConfigBase* pPrefs = m_appScope.get_preferences();

    // allowed intervals. Default: all in one octave range
    int i;
    wxString sKey;
    for (i=0; i < lmNUM_INTVALS; i++) {
        sKey = wxString::Format("/Constrains/EarIntval/%s/Interval%dAllowed",
            GetSection(), i );
        pPrefs->Read(sKey, &m_fIntervalAllowed[i], (bool)(i < 13) );
    }

    // notes range. Default A3 to A5
    sKey = wxString::Format("/Constrains/EarIntval/%s/MinPitch", GetSection());
    m_nMinPitch = (int) pPrefs->Read(sKey, 27L);      // 27 = A3
    sKey = wxString::Format("/Constrains/EarIntval/%s/MaxPitch", GetSection());
    m_nMaxPitch = (int) pPrefs->Read(sKey, 41L);      // 41 = A5

    // intervals types. Default: all types allowed
    for (i=0; i < 3; i++) {
        sKey = wxString::Format("/Constrains/EarIntval/%s/IntervalType%d",
            GetSection(), i );
        pPrefs->Read(sKey, &m_fTypeAllowed[i], true);
    }

    // accidentals and key signatures. Default use only natual intervals from C major scale
    sKey = wxString::Format("/Constrains/EarIntval/%s/OnlyNatural", GetSection());
    pPrefs->Read(sKey, &m_fOnlyNatural, true);    //use only natural intervals
    bool fValid;
    for (i=k_min_key; i <= k_max_key; i++) {
        sKey = wxString::Format("/Constrains/EarIntval/%s/KeySignature%d",
            GetSection(), i );
        pPrefs->Read(sKey, &fValid, (bool)((EKeySignature)i == k_key_C) );
        m_oValidKeys.SetValid((EKeySignature)i, fValid);
    }

    // for interval comparison exercises
    sKey = wxString::Format("/Constrains/EarIntval/%s/FirstEqual", GetSection());
    pPrefs->Read(sKey, &m_fFirstEqual, true);    // first note equal in both intervals

}

//---------------------------------------------------------------------------------------
void EarIntervalsConstrains::load_settings_for_level_1()
{
    if (GetSection() == "EarCompare")
    {
        // allowed intervals
        for (int i=0; i < lmNUM_INTVALS; i++)
            m_fIntervalAllowed[i] = false;

        m_fIntervalAllowed[ein_2min] = true;
        m_fIntervalAllowed[ein_2maj] = true;
        m_fIntervalAllowed[ein_3min] = true;
        m_fIntervalAllowed[ein_3maj] = true;
        m_fIntervalAllowed[ein_4] = true;
        m_fIntervalAllowed[ein_5] = true;

        // notes range: A3 to A5
        m_nMinPitch = 27;       // 27 = A3
        m_nMaxPitch = 41;       // 41 = A5

        // intervals types
        m_fTypeAllowed[0] = false;   // 0-harmonic
        m_fTypeAllowed[1] = true;   // 1-melodic ascending
        m_fTypeAllowed[2] = true;   // 2-melodic descending

        //key signatures
        for (int i=k_min_key; i <= k_max_key; i++)
            m_oValidKeys.SetValid((EKeySignature)i, false);

        m_oValidKeys.SetValid(k_key_C, true);
        m_oValidKeys.SetValid(k_key_a, true);
        m_oValidKeys.SetValid(k_key_G, true);
        m_oValidKeys.SetValid(k_key_e, true);
        m_oValidKeys.SetValid(k_key_F, true);
        m_oValidKeys.SetValid(k_key_d, true);

        //other
        m_fOnlyNatural = true;    // use only natural intervals of the scale
        m_fFirstEqual = true;     // first note equal in both intervals

        SetSettingsLink(false);
    }
    else if (GetSection() == "EarIntervals")
    {
        // allowed intervals
        for (int i=0; i < lmNUM_INTVALS; i++)
            m_fIntervalAllowed[i] = false;

        m_fIntervalAllowed[ein_4] = true;
        m_fIntervalAllowed[ein_5] = true;
        m_fIntervalAllowed[ein_8] = true;

        // notes range: A3 to A5
        m_nMinPitch = 27;       // 27 = A3
        m_nMaxPitch = 41;       // 41 = A5

        // intervals types
        m_fTypeAllowed[0] = false;   // 0-harmonic
        m_fTypeAllowed[1] = true;    // 1-melodic ascending
        m_fTypeAllowed[2] = false;   // 2-melodic descending

        //key signatures
        for (int i=k_min_key; i <= k_max_key; i++)
            m_oValidKeys.SetValid((EKeySignature)i, false);

        m_oValidKeys.SetValid(k_key_C, true);
        m_oValidKeys.SetValid(k_key_a, true);
        m_oValidKeys.SetValid(k_key_G, true);
        m_oValidKeys.SetValid(k_key_e, true);
        m_oValidKeys.SetValid(k_key_F, true);
        m_oValidKeys.SetValid(k_key_d, true);

        //other
        m_fOnlyNatural = true;    // use only natural intervals of the scale
        m_fFirstEqual = true;     //Not used. first note equal in both intervals

        SetSettingsLink(false);
    }
    else
        load_settings_for_customized_level();
}

//---------------------------------------------------------------------------------------
void EarIntervalsConstrains::load_settings_for_level_2()
{
    if (GetSection() == "EarCompare")
    {
        // allowed intervals
        for (int i=0; i < lmNUM_INTVALS; i++)
            m_fIntervalAllowed[i] = false;

        m_fIntervalAllowed[ein_2min] = true;
        m_fIntervalAllowed[ein_2maj] = true;
        m_fIntervalAllowed[ein_3min] = true;
        m_fIntervalAllowed[ein_3maj] = true;
        m_fIntervalAllowed[ein_4] = true;
        m_fIntervalAllowed[ein_5] = true;
        m_fIntervalAllowed[ein_6min] = true;
        m_fIntervalAllowed[ein_6maj] = true;
        m_fIntervalAllowed[ein_7min] = true;
        m_fIntervalAllowed[ein_7maj] = true;
        m_fIntervalAllowed[ein_8] = true;
        m_fIntervalAllowed[ein_9min] = true;
        m_fIntervalAllowed[ein_9maj] = true;

        // notes range: A3 to A5
        m_nMinPitch = 27;       // 27 = A3
        m_nMaxPitch = 41;       // 41 = A5

        // intervals types
        m_fTypeAllowed[0] = true;   // 0-harmonic
        m_fTypeAllowed[1] = true;   // 1-melodic ascending
        m_fTypeAllowed[2] = true;   // 2-melodic descending

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
        m_oValidKeys.SetValid(k_key_A, true);
        m_oValidKeys.SetValid(k_key_fs, true);
        m_oValidKeys.SetValid(k_key_E, true);
        m_oValidKeys.SetValid(k_key_cs, true);

        //other
        m_fOnlyNatural = true;    // use only natural intervals of the scale
        m_fFirstEqual = true;     // first note equal in both intervals

        SetSettingsLink(false);
    }
    else if (GetSection() == "EarIntervals")
    {
        // allowed intervals
        for (int i=0; i < lmNUM_INTVALS; i++)
            m_fIntervalAllowed[i] = false;

        m_fIntervalAllowed[ein_2min] = true;
        m_fIntervalAllowed[ein_2maj] = true;
        m_fIntervalAllowed[ein_3min] = true;
        m_fIntervalAllowed[ein_3maj] = true;
        m_fIntervalAllowed[ein_4] = true;
        m_fIntervalAllowed[ein_5] = true;
        m_fIntervalAllowed[ein_6min] = true;
        m_fIntervalAllowed[ein_6maj] = true;
        m_fIntervalAllowed[ein_7min] = true;
        m_fIntervalAllowed[ein_7maj] = true;
        m_fIntervalAllowed[ein_8] = true;

        // notes range: A3 to A5
        m_nMinPitch = 27;       // 27 = A3
        m_nMaxPitch = 41;       // 41 = A5

        // intervals types
        m_fTypeAllowed[0] = false;   // 0-harmonic
        m_fTypeAllowed[1] = true;    // 1-melodic ascending
        m_fTypeAllowed[2] = true;   // 2-melodic descending

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
        m_oValidKeys.SetValid(k_key_A, true);
        m_oValidKeys.SetValid(k_key_fs, true);
        m_oValidKeys.SetValid(k_key_E, true);
        m_oValidKeys.SetValid(k_key_cs, true);

        //other
        m_fOnlyNatural = true;    // use only natural intervals of the scale
        m_fFirstEqual = true;     //Not used. first note equal in both intervals

        SetSettingsLink(false);
    }
    else
        load_settings_for_customized_level();
}


}   // namespace lenmus
