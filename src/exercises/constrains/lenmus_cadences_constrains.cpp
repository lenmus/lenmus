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
    wxConfigBase* pPrefs = m_appScope.get_preferences();

    // allowed cadences
    for (int i=0; i < k_cadence_max; i++)
    {
        wxString sKey = wxString::Format("/Constrains/IdfyCadence/%s/Cadence%dAllowed",
                                         m_sSection.wx_str(), i );
        pPrefs->Write(sKey, m_fValidCadences[i]);
    }

    // key signatures
    bool fValid;
    for (int i=k_min_key; i <= k_max_key; i++)
    {
        wxString sKey = wxString::Format("/Constrains/IdfyCadence/%s/KeySignature%d",
                                         m_sSection.wx_str(), i );
        fValid = m_oValidKeys.IsValid((EKeySignature)i);
        pPrefs->Write(sKey, fValid);
    }

    // answer buttons
    for (int i=0; i < lm_eCadMaxButton; i++)
    {
        wxString sKey = wxString::Format("/Constrains/IdfyCadence/%s/Button%dAllowed",
            m_sSection.wx_str(), i );
        pPrefs->Write(sKey, m_fValidButtons[i]);
    }

    // how to display key
    wxString sKey =  wxString::Format("/Constrains/IdfyCadence/%s/DisplayKeyMode",
                                      m_sSection.wx_str());
    pPrefs->Write(sKey, m_nKeyDisplayMode);

}

//---------------------------------------------------------------------------------------
void CadencesConstrains::load_settings()
{
    wxConfigBase* pPrefs = m_appScope.get_preferences();

    // allowed cadences. Default: all allowed
    for (int i=0; i < k_cadence_max; i++)
    {
        wxString sKey = wxString::Format("/Constrains/IdfyCadence/%s/Cadence%dAllowed",
                                         m_sSection.wx_str(), i );
        pPrefs->Read(sKey, &m_fValidCadences[i], true );
    }

    // key signatures. Default use C major
    bool fValid;
    for (int i=k_min_key; i <= k_max_key; i++)
    {
        wxString sKey = wxString::Format("/Constrains/IdfyCadence/%s/KeySignature%d",
                                         m_sSection.wx_str(), i );
        pPrefs->Read(sKey, &fValid, (bool)((EKeySignature)i == k_key_C) );
        m_oValidKeys.SetValid((EKeySignature)i, fValid);
    }

    // answer buttons. Default: transient / terminal
    for (int i=0; i < lm_eCadMaxButton; i++)
    {
        wxString sKey = wxString::Format("/Constrains/IdfyCadence/%s/Button%dAllowed",
                                         m_sSection.wx_str(), i );
        pPrefs->Read(sKey, &m_fValidButtons[i], (bool)(i < 2) );
    }

    // how to display key. Default: play tonic chord
    wxString sKey = wxString::Format("/Constrains/IdfyCadence/%s/DisplayKeyMode", m_sSection.wx_str());
    pPrefs->Read(sKey, &m_nKeyDisplayMode, 1);

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
