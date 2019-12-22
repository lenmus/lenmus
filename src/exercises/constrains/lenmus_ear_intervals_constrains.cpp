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

    wxConfigBase* pPrefs = m_appScope.get_preferences();

    // allowed intervals
    int i;
    wxString sKey;
    for (i=0; i < lmNUM_INTVALS; i++) {
        sKey = wxString::Format("/Constrains/EarIntval/%s/Interval%dAllowed",
            m_sSection.wx_str(), i );
        pPrefs->Write(sKey, m_fIntervalAllowed[i]);
    }

    // notes range
    sKey = wxString::Format("/Constrains/EarIntval/%s/MinPitch", m_sSection.wx_str());
    pPrefs->Write(sKey, (long)m_nMinPitch);
    sKey = wxString::Format("/Constrains/EarIntval/%s/MaxPitch", m_sSection.wx_str());
    pPrefs->Write(sKey, (long)m_nMaxPitch);

    // intervals types
    for (i=0; i < 3; i++) {
        sKey = wxString::Format("/Constrains/EarIntval/%s/IntervalType%d",
            m_sSection.wx_str(), i );
        pPrefs->Write(sKey, m_fTypeAllowed[i]);
    }

    // accidentals and key signatures
    sKey = wxString::Format("/Constrains/EarIntval/%s/OnlyNatural",
                        m_sSection.wx_str());
    pPrefs->Write(sKey, m_fOnlyNatural);
    bool fValid;
    for (i=k_min_key; i <= k_max_key; i++) {
        sKey = wxString::Format("/Constrains/EarIntval/%s/KeySignature%d",
            m_sSection.wx_str(), i );
        fValid = m_oValidKeys.IsValid((EKeySignature)i);
        pPrefs->Write(sKey, fValid);
    }

    // for interval comparison exercises
    sKey = wxString::Format("/Constrains/EarIntval/%s/FirstEqual", m_sSection.wx_str());
    pPrefs->Write(sKey, m_fFirstEqual);

}

//---------------------------------------------------------------------------------------
void EarIntervalsConstrains::load_settings()
{
    // load settings form user configuration data or default values

    wxConfigBase* pPrefs = m_appScope.get_preferences();

    // allowed intervals. Default: all in one octave range
    int i;
    wxString sKey;
    for (i=0; i < lmNUM_INTVALS; i++) {
        sKey = wxString::Format("/Constrains/EarIntval/%s/Interval%dAllowed",
            m_sSection.wx_str(), i );
        pPrefs->Read(sKey, &m_fIntervalAllowed[i], (bool)(i < 13) );
    }

    // notes range. Default A3 to A5
    sKey = wxString::Format("/Constrains/EarIntval/%s/MinPitch",
                m_sSection.wx_str());
    m_nMinPitch = (int) pPrefs->Read(sKey, 27L);      // 27 = A3
    sKey = wxString::Format("/Constrains/EarIntval/%s/MaxPitch",
                m_sSection.wx_str());
    m_nMaxPitch = (int) pPrefs->Read(sKey, 41L);      // 41 = A5

    // intervals types. Default: all types allowed
    for (i=0; i < 3; i++) {
        sKey = wxString::Format("/Constrains/EarIntval/%s/IntervalType%d",
            m_sSection.wx_str(), i );
        pPrefs->Read(sKey, &m_fTypeAllowed[i], true);
    }

    // accidentals and key signatures. Default use only natual intervals from C major scale
    sKey = wxString::Format("/Constrains/EarIntval/%s/OnlyNatural", m_sSection.wx_str());
    pPrefs->Read(sKey, &m_fOnlyNatural, true);    //use only natural intervals
    bool fValid;
    for (i=k_min_key; i <= k_max_key; i++) {
        sKey = wxString::Format("/Constrains/EarIntval/%s/KeySignature%d",
            m_sSection.wx_str(), i );
        pPrefs->Read(sKey, &fValid, (bool)((EKeySignature)i == k_key_C) );
        m_oValidKeys.SetValid((EKeySignature)i, fValid);
    }

    // for interval comparison exercises
    sKey = wxString::Format("/Constrains/EarIntval/%s/FirstEqual", m_sSection.wx_str());
    pPrefs->Read(sKey, &m_fFirstEqual, true);    // first note equal in both intervals

}


}   // namespace lenmus
