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

////lenmus
//#include "lenmus_constrains.h"
//
//#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
//#pragma implementation "HarmonyConstrains.h"
//#endif
//
//// For compilers that support precompilation, includes "wx.h".
//#include "wx/wxprec.h"
//
//#ifdef __BORLANDC__
//#pragma hdrstop
//#endif
//
//#include "HarmonyConstrains.h"
//#include "lenmus_generators.h"
//
//// the config object
//extern wxConfigBase *pPrefs;
//
//
//
//namespace lenmus
//{
//
//lmHarmonyConstrains::lmHarmonyConstrains(wxString sSection,
//                                                 ApplicationScope& appScope)
//    : ExerciseOptions(sSection, appScope)
//{
//    //
//    // default settings
//    //
//
//    // allowed cadences. Default: all allowed
//    for (int i=0; i < k_cadence_max; i++) {
//        m_fValidCadences[i] = true;
//    }
//
//    // key signatures. Default use C major
//    for (int i=k_min_key; i <= k_max_key; i++) {
//        bool fValid = ((EKeySignature)i == k_key_C);
//        m_oValidKeys.SetValid((EKeySignature)i, fValid);
//    }
//
//    // answer buttons. Default: transient / terminal
//    for (int i=0; i < lm_eCadMaxButton; i++) {
//        m_fValidButtons[i] = (i < 2);
//    }
//
//    // how to display key. Default: play tonic chord
//    m_nKeyDisplayMode = 1;
//
//}
//
//void lmHarmonyConstrains::save_settings()
//{
//    //save settings in user configuration data file
//
//    wxConfigBase* pPrefs = m_appScope.get_preferences();
//
//    // allowed cadences
//    int i;
//    wxString sKey;
//    for (i=0; i < k_cadence_max; i++) {
//        sKey = wxString::Format("/Constrains/IdfyCadence/%s/Cadence%dAllowed",
//            m_sSection.wx_str(), i );
//        pPrefs->Write(sKey, m_fValidCadences[i]);
//    }
//
//    // key signatures
//    bool fValid;
//    for (i=k_min_key; i <= k_max_key; i++) {
//        sKey = wxString::Format("/Constrains/IdfyCadence/%s/KeySignature%d",
//            m_sSection.wx_str(), i );
//        fValid = m_oValidKeys.IsValid((EKeySignature)i);
//        pPrefs->Write(sKey, fValid);
//    }
//
//    // answer buttons
//    for (i=0; i < lm_eCadMaxButton; i++) {
//        sKey = wxString::Format("/Constrains/IdfyCadence/%s/Button%dAllowed",
//            m_sSection.wx_str(), i );
//        pPrefs->Write(sKey, m_fValidButtons[i]);
//    }
//
//    // how to display key
//    sKey = wxString::Format("/Constrains/IdfyCadence/%s/DisplayKeyMode", m_sSection.wx_str());
//    pPrefs->Write(sKey, m_nKeyDisplayMode);
//
//}
//
//void lmHarmonyConstrains::load_settings()
//{
//    // load settings form user configuration data or default values
//
//    wxConfigBase* pPrefs = m_appScope.get_preferences();
//
//    // allowed cadences. Default: all allowed
//    int i;
//    wxString sKey;
//    for (i=0; i < k_cadence_max; i++) {
//        sKey = wxString::Format("/Constrains/IdfyCadence/%s/Cadence%dAllowed",
//            m_sSection.wx_str(), i );
//        pPrefs->Read(sKey, &m_fValidCadences[i], true );
//    }
//
//    // key signatures. Default use C major
//    bool fValid;
//    for (i=k_min_key; i <= k_max_key; i++) {
//        sKey = wxString::Format("/Constrains/IdfyCadence/%s/KeySignature%d",
//            m_sSection.wx_str(), i );
//        pPrefs->Read(sKey, &fValid, (bool)((EKeySignature)i == k_key_C) );
//        m_oValidKeys.SetValid((EKeySignature)i, fValid);
//    }
//
//    // answer buttons. Default: transient / terminal
//    for (i=0; i < lm_eCadMaxButton; i++) {
//        sKey = wxString::Format("/Constrains/IdfyCadence/%s/Button%dAllowed",
//            m_sSection.wx_str(), i );
//        pPrefs->Read(sKey, &m_fValidButtons[i], (bool)(i < 2) );
//    }
//
//    // how to display key. Default: play tonic chord
//    sKey = wxString::Format("/Constrains/IdfyCadence/%s/DisplayKeyMode", m_sSection.wx_str());
//    pPrefs->Read(sKey, &m_nKeyDisplayMode, 1);
//
//}
//
//ECadenceType lmHarmonyConstrains::GetRandomCadence()
//{
//    RandomGenerator oGenerator;
//    int nWatchDog = 0;
//    int nType = oGenerator.random_number(0, k_cadence_max-1);
//    while (!IsCadenceValid((ECadenceType)nType)) {
//        nType = oGenerator.random_number(0, k_cadence_max-1);
//        if (nWatchDog++ == 1000) {
//            wxLogMessage("Program error: Loop detected in lmHarmonyConstrains::GetRandomCadence.");
//            return (ECadenceType)0;
//        }
//    }
//    return (ECadenceType)nType;
//
//}
//
//
//}   // namespace lenmus
