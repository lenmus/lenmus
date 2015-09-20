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

    wxConfigBase* pPrefs = m_appScope.get_preferences();

    // allowed clefs
    int i;
    wxString sKey;
    for (i = k_min_clef_in_exercises; i <= k_max_clef_in_exercises; i++) {
        sKey = wxString::Format("/Constrains/TheoIntval/%s/Clef%d",
            m_sSection.wx_str(), i );
        pPrefs->Write(sKey, IsValidClef((EClef)i) );
    }

    // allowed key signatures
    bool fValid;
    for (i=k_min_key; i <= k_max_key; i++) {
        sKey = wxString::Format("/Constrains/TheoIntval/%s/KeySignature%d",
            m_sSection.wx_str(), i );
        fValid = m_oValidKeys.IsValid((EKeySignature)i);
        pPrefs->Write(sKey, fValid);
    }

    // allowed intervals types
    for (i=0; i < 3; i++) {
        sKey = wxString::Format("/Constrains/TheoIntval/%s/IntervalType%d",
            m_sSection.wx_str(), i );
        pPrefs->Write(sKey, m_fTypeAllowed[i]);
    }

    //ledger lines
    sKey = wxString::Format("/Constrains/TheoIntval/%s/LedgerAbove",
                            m_sSection.wx_str());
    pPrefs->Write(sKey, (long)m_nLedgerAbove);
    sKey = wxString::Format("/Constrains/TheoIntval/%s/LedgerBelow",
                            m_sSection.wx_str());
    pPrefs->Write(sKey, (long)m_nLedgerBelow);

    //problem level
    sKey = wxString::Format("/Constrains/TheoIntval/%s/ProblemLevel",
                            m_sSection.wx_str());
    pPrefs->Write(sKey, (long)m_nProblemLevel);

    // allowed accidentals
    sKey = wxString::Format("/Constrains/TheoIntval/%s/Accidentals", m_sSection.wx_str() );
    pPrefs->Write(sKey, m_fAccidentals);
    sKey = wxString::Format("/Constrains/TheoIntval/%s/DoubleAccidentals", m_sSection.wx_str() );
    pPrefs->Write(sKey, m_fDoubleAccidentals);

    // problem type
    sKey = wxString::Format("/Constrains/TheoIntval/%s/ProblemType", m_sSection.wx_str() );
    pPrefs->Write(sKey, (long) m_nProblemType );

}

//---------------------------------------------------------------------------------------
void TheoIntervalsConstrains::load_settings()
{
    //
    // load settings form user configuration data or default values
    //

    wxConfigBase* pPrefs = m_appScope.get_preferences();
    if (!pPrefs)
        return;

    // allowed clefs. Default G clef
    int i;
    wxString sKey;
    bool fValid;
    for (i = k_min_clef_in_exercises; i <= k_max_clef_in_exercises; i++) {
        sKey = wxString::Format("/Constrains/TheoIntval/%s/Clef%d",
            m_sSection.wx_str(), i );
        pPrefs->Read(sKey, &fValid, (i == k_clef_G2) );
        SetClef((EClef)i, fValid);
    }

    // allowed key signatures. Default: C major key signature
    for (i=k_min_key; i <= k_max_key; i++) {
        sKey = wxString::Format("/Constrains/TheoIntval/%s/KeySignature%d",
            m_sSection.wx_str(), i );
        pPrefs->Read(sKey, &fValid, (bool)((EKeySignature)i == k_key_C) );
        m_oValidKeys.SetValid((EKeySignature)i, fValid);
    }

    // intervals types. Default: melodic
    for (i=0; i < 3; i++) {
        sKey = wxString::Format("/Constrains/TheoIntval/%s/IntervalType%d",
            m_sSection.wx_str(), i );
        pPrefs->Read(sKey, &m_fTypeAllowed[i], (i != 0));
    }

    //ledger lines. Default: 1
    sKey = wxString::Format("/Constrains/TheoIntval/%s/LedgerAbove",
                            m_sSection.wx_str());
    m_nLedgerAbove = (int)pPrefs->Read(sKey, 1L);
    sKey = wxString::Format("/Constrains/TheoIntval/%s/LedgerBelow",
                            m_sSection.wx_str());
    m_nLedgerBelow = (int)pPrefs->Read(sKey, 1L);

    //problem level. Default: 2 - Also augmented and diminished
    sKey = wxString::Format("/Constrains/TheoIntval/%s/ProblemLevel",
                            m_sSection.wx_str());
    m_nProblemLevel = (int)pPrefs->Read(sKey, 2L);

    // allowed accidentals. Defaul: none
    sKey = wxString::Format("/Constrains/TheoIntval/%s/Accidentals", m_sSection.wx_str() );
    pPrefs->Read(sKey, &m_fAccidentals, false);
    sKey = wxString::Format("/Constrains/TheoIntval/%s/DoubleAccidentals", m_sSection.wx_str() );
    pPrefs->Read(sKey, &m_fDoubleAccidentals, false);

    // problem type
    sKey = wxString::Format("/Constrains/TheoIntval/%s/ProblemType", m_sSection.wx_str() );
    m_nProblemType = pPrefs->Read(sKey, (long) TheoIntervalsConstrains::k_both );


}


}   // namespace lenmus
