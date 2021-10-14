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
#include "lenmus_theo_intervals_constrains.h"

//lomse
#include <lomse_internal_model.h>


namespace lenmus
{

//=======================================================================================
// TheoIntervalsConstrains implementation
//=======================================================================================
TheoIntervalsConstrains::TheoIntervalsConstrains(const wxString& sSection,
                                                 ApplicationScope& appScope)
    : ExerciseOptions(sSection, appScope)
{
    load_settings();
}

//---------------------------------------------------------------------------------------
void TheoIntervalsConstrains::save_settings()
{
    // save settings in user configuration data file

    if (m_appScope.get_exercises_level() != 100)
        return;

    wxConfigBase* pPrefs = m_appScope.get_preferences();

    // allowed clefs
    int i;
    wxString sKey;
    for (i = k_min_clef_in_exercises; i <= k_max_clef_in_exercises; i++) {
        sKey = wxString::Format("/Constrains/TheoIntval/%s/Clef%d",
            GetSection(), i );
        pPrefs->Write(sKey, IsValidClef((EClef)i) );
    }

    // allowed key signatures
    bool fValid;
    for (i=k_min_key; i <= k_max_key; i++) {
        sKey = wxString::Format("/Constrains/TheoIntval/%s/KeySignature%d",
            GetSection(), i );
        fValid = m_oValidKeys.IsValid((EKeySignature)i);
        pPrefs->Write(sKey, fValid);
    }

    // allowed intervals types
    for (i=0; i < 3; i++) {
        sKey = wxString::Format("/Constrains/TheoIntval/%s/IntervalType%d",
            GetSection(), i );
        pPrefs->Write(sKey, m_fTypeAllowed[i]);
    }

    //ledger lines
    sKey = wxString::Format("/Constrains/TheoIntval/%s/LedgerAbove",
                            GetSection());
    pPrefs->Write(sKey, (long)m_nLedgerAbove);
    sKey = wxString::Format("/Constrains/TheoIntval/%s/LedgerBelow",
                            GetSection());
    pPrefs->Write(sKey, (long)m_nLedgerBelow);

    //problem level
    sKey = wxString::Format("/Constrains/TheoIntval/%s/ProblemLevel",
                            GetSection());
    pPrefs->Write(sKey, (long)m_nProblemLevel);

    // allowed accidentals
    sKey = wxString::Format("/Constrains/TheoIntval/%s/Accidentals", GetSection() );
    pPrefs->Write(sKey, m_fAccidentals);
    sKey = wxString::Format("/Constrains/TheoIntval/%s/DoubleAccidentals", GetSection() );
    pPrefs->Write(sKey, m_fDoubleAccidentals);

    // problem type
    sKey = wxString::Format("/Constrains/TheoIntval/%s/ProblemType", GetSection() );
    pPrefs->Write(sKey, (long) m_nProblemType );

}

//---------------------------------------------------------------------------------------
void TheoIntervalsConstrains::load_settings()
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
void TheoIntervalsConstrains::load_settings_for_customized_level()
{
    wxConfigBase* pPrefs = m_appScope.get_preferences();
    if (!pPrefs)
        return;

    // allowed clefs. Default G clef
    int i;
    wxString sKey;
    bool fValid;
    for (i = k_min_clef_in_exercises; i <= k_max_clef_in_exercises; i++) {
        sKey = wxString::Format("/Constrains/TheoIntval/%s/Clef%d", GetSection(), i );
        pPrefs->Read(sKey, &fValid, (i == k_clef_G2) );
        SetClef((EClef)i, fValid);
    }

    // allowed key signatures. Default: C major key signature
    for (i=k_min_key; i <= k_max_key; i++) {
        sKey = wxString::Format("/Constrains/TheoIntval/%s/KeySignature%d",
            GetSection(), i );
        pPrefs->Read(sKey, &fValid, (bool)((EKeySignature)i == k_key_C) );
        m_oValidKeys.SetValid((EKeySignature)i, fValid);
    }

    // intervals types. Default: melodic
    for (i=0; i < 3; i++) {
        sKey = wxString::Format("/Constrains/TheoIntval/%s/IntervalType%d",
            GetSection(), i );
        pPrefs->Read(sKey, &m_fTypeAllowed[i], (i != 0));
    }

    //ledger lines. Default: 1
    sKey = wxString::Format("/Constrains/TheoIntval/%s/LedgerAbove", GetSection());
    m_nLedgerAbove = (int)pPrefs->Read(sKey, 1L);
    sKey = wxString::Format("/Constrains/TheoIntval/%s/LedgerBelow", GetSection());
    m_nLedgerBelow = (int)pPrefs->Read(sKey, 1L);

    //problem level. Default: 2 - Also augmented and diminished
    sKey = wxString::Format("/Constrains/TheoIntval/%s/ProblemLevel", GetSection());
    m_nProblemLevel = (int)pPrefs->Read(sKey, 2L);

    // allowed accidentals. Defaul: none
    sKey = wxString::Format("/Constrains/TheoIntval/%s/Accidentals", GetSection() );
    pPrefs->Read(sKey, &m_fAccidentals, false);
    sKey = wxString::Format("/Constrains/TheoIntval/%s/DoubleAccidentals", GetSection() );
    pPrefs->Read(sKey, &m_fDoubleAccidentals, false);

    // problem type
    sKey = wxString::Format("/Constrains/TheoIntval/%s/ProblemType", GetSection() );
    m_nProblemType = pPrefs->Read(sKey, (long) TheoIntervalsConstrains::k_both );
}

//---------------------------------------------------------------------------------------
void TheoIntervalsConstrains::load_settings_for_level_1()
{
    if (GetSection() == "BuildIntervals")
    {
        //clefs: G clef
        for (int i = k_min_clef_in_exercises; i <= k_max_clef_in_exercises; i++)
            SetClef((EClef)i, false);

        SetClef(k_clef_G2, true);

        //key signatures
        for (int i=k_min_key; i <= k_max_key; i++)
            m_oValidKeys.SetValid((EKeySignature)i, false);

        m_oValidKeys.SetValid(k_key_C, true);
        m_oValidKeys.SetValid(k_key_a, true);
        m_oValidKeys.SetValid(k_key_G, true);
        m_oValidKeys.SetValid(k_key_e, true);
        m_oValidKeys.SetValid(k_key_F, true);
        m_oValidKeys.SetValid(k_key_d, true);

        // intervals types
        m_fTypeAllowed[0] = false;   // 0-harmonic
        m_fTypeAllowed[1] = true;    // 1-melodic ascending
        m_fTypeAllowed[2] = false;   // 2-melodic descending

        //problem level
        m_nProblemLevel = 1;      //1-Perfect, major and minor intervals

        //ledger lines
        m_nLedgerAbove = 1;
        m_nLedgerBelow = 2;

        //allowed accidentals
        m_fAccidentals = false;         //allow accidentals
        m_fDoubleAccidentals = false;   //allow double accidentals

        // problem type (da igual, no se usa)
        m_nProblemType = TheoIntervalsConstrains::k_both;
    }
    else if (GetSection() == "IdfyIntervals")
    {
        //clefs: G clef
        for (int i = k_min_clef_in_exercises; i <= k_max_clef_in_exercises; i++)
            SetClef((EClef)i, false);

        SetClef(k_clef_G2, true);

        //key signatures
        for (int i=k_min_key; i <= k_max_key; i++)
            m_oValidKeys.SetValid((EKeySignature)i, false);

        m_oValidKeys.SetValid(k_key_C, true);
        m_oValidKeys.SetValid(k_key_a, true);
        m_oValidKeys.SetValid(k_key_G, true);
        m_oValidKeys.SetValid(k_key_e, true);
        m_oValidKeys.SetValid(k_key_F, true);
        m_oValidKeys.SetValid(k_key_d, true);

        // intervals types
        m_fTypeAllowed[0] = false;   // 0-harmonic
        m_fTypeAllowed[1] = true;   // 1-melodic ascending
        m_fTypeAllowed[2] = true;   // 2-melodic descending

        //problem level
        m_nProblemLevel = 0;      //  0-Just name the interval number

        //ledger lines
        m_nLedgerAbove = 1;
        m_nLedgerBelow = 2;

        //allowed accidentals
        m_fAccidentals = false;         //allow accidentals
        m_fDoubleAccidentals = false;   //allow double accidentals

        // problem type (da igual, no se usa)
        m_nProblemType = TheoIntervalsConstrains::k_both;
    }
    else
        load_settings_for_customized_level();
}

//---------------------------------------------------------------------------------------
void TheoIntervalsConstrains::load_settings_for_level_2()
{
    if (GetSection() == "BuildIntervals")
    {
        //clefs: G clef
        for (int i = k_min_clef_in_exercises; i <= k_max_clef_in_exercises; i++)
            SetClef((EClef)i, false);

        SetClef(k_clef_G2, true);

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
        m_oValidKeys.SetValid(k_key_A, true);
        m_oValidKeys.SetValid(k_key_fs, true);
        m_oValidKeys.SetValid(k_key_E, true);
        m_oValidKeys.SetValid(k_key_cs, true);

        // intervals types
        m_fTypeAllowed[0] = false;   // 0-harmonic
        m_fTypeAllowed[1] = true;   // 1-melodic ascending
        m_fTypeAllowed[2] = true;   // 2-melodic descending

        //problem level
        m_nProblemLevel = 2;      //  2-Also augmented and diminished

        //ledger lines
        m_nLedgerAbove = 1;
        m_nLedgerBelow = 2;

        //allowed accidentals
        m_fAccidentals = false;         //allow accidentals
        m_fDoubleAccidentals = false;   //allow double accidentals

        // problem type (da igual, no se usa)
        m_nProblemType = TheoIntervalsConstrains::k_both;
    }
    else if (GetSection() == "IdfyIntervals")
    {
        //clefs: G clef
        for (int i = k_min_clef_in_exercises; i <= k_max_clef_in_exercises; i++)
            SetClef((EClef)i, false);

        SetClef(k_clef_G2, true);
        SetClef(k_clef_F4, true);

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
        m_oValidKeys.SetValid(k_key_A, true);
        m_oValidKeys.SetValid(k_key_fs, true);
        m_oValidKeys.SetValid(k_key_E, true);
        m_oValidKeys.SetValid(k_key_cs, true);

        // intervals types
        m_fTypeAllowed[0] = false;   // 0-harmonic
        m_fTypeAllowed[1] = true;   // 1-melodic ascending
        m_fTypeAllowed[2] = true;   // 2-melodic descending

        //problem level
        m_nProblemLevel = 2;      //  2-Also augmented and diminished

        //ledger lines
        m_nLedgerAbove = 1;
        m_nLedgerBelow = 2;

        //allowed accidentals
        m_fAccidentals = false;         //allow accidentals
        m_fDoubleAccidentals = false;   //allow double accidentals

        // problem type (da igual, no se usa)
        m_nProblemType = TheoIntervalsConstrains::k_both;
    }
    else
        load_settings_for_customized_level();
}


}   // namespace lenmus
