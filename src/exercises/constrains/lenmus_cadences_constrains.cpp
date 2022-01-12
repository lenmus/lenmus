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
#include "lenmus_cadences_constrains.h"

#include "lenmus_constrains.h"
#include "lenmus_generators.h"

//lomse
#include <lomse_logger.h>
using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>


namespace lenmus
{

//---------------------------------------------------------------------------------------
CadencesConstrains::CadencesConstrains(wxString sSection, ApplicationScope& appScope)
    : ExerciseOptions(sSection, appScope)
{
    //
    // default settings
    //

    // allowed cadences. Default: all allowed
    for (int i=0; i < k_cadence_max; i++)
        m_fValidCadences[i] = true;

    // key signatures. Default use C major
    for (int i=k_min_key; i <= k_max_key; i++)
    {
        bool fValid = ((EKeySignature)i == k_key_C);
        m_oValidKeys.SetValid((EKeySignature)i, fValid);
    }

    // answer buttons. Default: transient / terminal
    for (int i=0; i < lm_eCadMaxButton; i++)
        m_fValidButtons[i] = (i < 2);

    // how to display key. Default: play tonic chord
    m_nKeyDisplayMode = 1;
}

//---------------------------------------------------------------------------------------
void CadencesConstrains::save_settings()
{
    if (m_appScope.get_exercises_level() != 100)
        return;

    wxConfigBase* pPrefs = m_appScope.get_preferences();

    // allowed cadences
    for (int i=0; i < k_cadence_max; i++)
    {
        wxString sKey = wxString::Format("/Constrains/IdfyCadence/%s/Cadence%dAllowed",
                                         GetSection(), i );
        pPrefs->Write(sKey, m_fValidCadences[i]);
    }

    // key signatures
    bool fValid;
    for (int i=k_min_key; i <= k_max_key; i++)
    {
        wxString sKey = wxString::Format("/Constrains/IdfyCadence/%s/KeySignature%d",
                                         GetSection(), i );
        fValid = m_oValidKeys.IsValid((EKeySignature)i);
        pPrefs->Write(sKey, fValid);
    }

    // answer buttons
    for (int i=0; i < lm_eCadMaxButton; i++)
    {
        wxString sKey = wxString::Format("/Constrains/IdfyCadence/%s/Button%dAllowed",
            GetSection(), i );
        pPrefs->Write(sKey, m_fValidButtons[i]);
    }

    // how to display key
    wxString sKey =  wxString::Format("/Constrains/IdfyCadence/%s/DisplayKeyMode",
                                      GetSection());
    pPrefs->Write(sKey, m_nKeyDisplayMode);

}

//---------------------------------------------------------------------------------------
void CadencesConstrains::load_settings()
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
void CadencesConstrains::load_settings_for_customized_level()
{
    wxConfigBase* pPrefs = m_appScope.get_preferences();

    // allowed cadences. Default: all allowed
    for (int i=0; i < k_cadence_max; i++)
    {
        wxString sKey = wxString::Format("/Constrains/IdfyCadence/%s/Cadence%dAllowed",
                                         GetSection(), i );
        pPrefs->Read(sKey, &m_fValidCadences[i], true );
    }

    // key signatures. Default use C major
    bool fValid;
    for (int i=k_min_key; i <= k_max_key; i++)
    {
        wxString sKey = wxString::Format("/Constrains/IdfyCadence/%s/KeySignature%d",
                                         GetSection(), i );
        pPrefs->Read(sKey, &fValid, (bool)((EKeySignature)i == k_key_C) );
        m_oValidKeys.SetValid((EKeySignature)i, fValid);
    }

    // answer buttons. Default: transient / terminal
    for (int i=0; i < lm_eCadMaxButton; i++)
    {
        wxString sKey = wxString::Format("/Constrains/IdfyCadence/%s/Button%dAllowed",
                                         GetSection(), i );
        pPrefs->Read(sKey, &m_fValidButtons[i], (bool)(i < 2) );
    }

    // how to display key. Default: play tonic chord
    wxString sKey = wxString::Format("/Constrains/IdfyCadence/%s/DisplayKeyMode",
                                     GetSection());
    pPrefs->Read(sKey, &m_nKeyDisplayMode, 1);
}

//---------------------------------------------------------------------------------------
void CadencesConstrains::load_settings_for_level_1()
{
//    if (GetSection() == "EarIdfyCadence")
//    {
        // allowed cadences
        for (int i=0; i < k_cadence_max; i++)
            m_fValidCadences[i] = false;

        m_fValidCadences[k_cadence_perfect_V_I] = true;
        m_fValidCadences[k_cadence_perfect_V7_I] = true;
        m_fValidCadences[k_cadence_plagal_IV_I] = true;
        m_fValidCadences[k_cadence_plagal_IIc6_I] = true;
//        m_fValidCadences[k_cadence_plagal_IImc6_I] = true;

        //key signatures
        for (int i=k_min_key; i <= k_max_key; i++)
            m_oValidKeys.SetValid((EKeySignature)i, false);

        m_oValidKeys.SetValid(k_key_C, true);
        m_oValidKeys.SetValid(k_key_a, true);
        m_oValidKeys.SetValid(k_key_G, true);
        m_oValidKeys.SetValid(k_key_e, true);
        m_oValidKeys.SetValid(k_key_F, true);
        m_oValidKeys.SetValid(k_key_d, true);

        // answer buttons
        for (int i=0; i < lm_eCadMaxButton; i++)
            m_fValidButtons[i] = false;

        m_fValidButtons[lm_eCadButtonPerfect] = true;
        m_fValidButtons[lm_eCadButtonPlagal] = true;

        //how to display the key
        m_nKeyDisplayMode = 1;      //Not used. 0-play A4 note. 1-play tonic chord

        SetSettingsLink(false);

//    }
//    else if (GetSection() == "TheoIdfyCadence")
//    {
//    }
//    else
//        load_settings_for_customized_level();
}

//---------------------------------------------------------------------------------------
void CadencesConstrains::load_settings_for_level_2()
{
//    if (GetSection() == "EarIdfyCadence")
//    {
        // allowed cadences
        for (int i=0; i < k_cadence_max; i++)
            m_fValidCadences[i] = false;

        m_fValidCadences[k_cadence_perfect_V_I] = true;
        m_fValidCadences[k_cadence_perfect_V7_I] = true;
        m_fValidCadences[k_cadence_plagal_IV_I] = true;
        m_fValidCadences[k_cadence_plagal_IIc6_I] = true;
        m_fValidCadences[k_cadence_half_I_V] = true;
        m_fValidCadences[k_cadence_deceptive_V_VI] = true;
        m_fValidCadences[k_cadence_deceptive_V_IV] = true;

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

        // answer buttons
        for (int i=0; i < lm_eCadMaxButton; i++)
            m_fValidButtons[i] = false;

        m_fValidButtons[lm_eCadButtonPerfect] = true;
        m_fValidButtons[lm_eCadButtonPlagal] = true;
        m_fValidButtons[lm_eCadButtonDeceptive] = true;
        m_fValidButtons[lm_eCadButtonHalf] = true;

        //how to display the key
        m_nKeyDisplayMode = 1;      //Not used. 0-play A4 note. 1-play tonic chord

        SetSettingsLink(false);
//    }
//    else if (GetSection() == "TheoIdfyCadence")
//    {
//    }
//    else
//        load_settings_for_customized_level();
}

//---------------------------------------------------------------------------------------
ECadenceType CadencesConstrains::GetRandomCadence()
{
    int nWatchDog = 0;
    int nType = RandomGenerator::random_number(0, k_cadence_max-1);
    while (!IsCadenceValid((ECadenceType)nType))
    {
        nType = RandomGenerator::random_number(0, k_cadence_max-1);
        if (nWatchDog++ == 1000)
        {
            LOMSE_LOG_ERROR("Program error: Loop detected");
            return (ECadenceType)0;
        }
    }
    return (ECadenceType)nType;
}


}   // namespace lenmus
