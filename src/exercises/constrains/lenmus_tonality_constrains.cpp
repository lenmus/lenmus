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
#include "lenmus_tonality_constrains.h"

////wxWidgets
//#include <wx/wxprec.h>

namespace lenmus
{

//---------------------------------------------------------------------------------------
TonalityConstrains::TonalityConstrains(wxString sSection, ApplicationScope& appScope)
    : ExerciseOptions(sSection, appScope)
{
    //
    // default settings
    //

    //answer buttons: only major/minor buttons
    m_fUseMajorMinorButtons = true;

    // key signatures and . Default: all
    for (int i=k_min_key; i <= k_max_key; i++)
    {
        EKeySignature nKey = static_cast<EKeySignature>(i);
        m_oValidKeys.SetValid(nKey, true);
    }

    //always ear training
    set_theory_mode(false);
}

//---------------------------------------------------------------------------------------
void TonalityConstrains::save_settings()
{
    //save settings in user configuration data file

    wxConfigBase* pPrefs = m_appScope.get_preferences();

    // allowed key signatures
    int i;
    wxString sKey;
    bool fValid;
    for (i=k_min_key; i <= k_max_key; i++)
    {
        sKey = wxString::Format("/Constrains/IdfyTonality/%s/KeySignature%d",
            m_sSection.wx_str(), i );
        fValid = m_oValidKeys.IsValid((EKeySignature)i);
        pPrefs->Write(sKey, fValid);
    }

    //answer buttons
    sKey = "/Constrains/IdfyTonality/UseMajorMinorButtons";
    pPrefs->Write(sKey, m_fUseMajorMinorButtons);
}

//---------------------------------------------------------------------------------------
void TonalityConstrains::load_settings()
{
    // load settings form user configuration data or default values

    wxConfigBase* pPrefs = m_appScope.get_preferences();

    // allowed key signatures. Default: all allowed
    int i;
    wxString sKey;
    bool fValid;
    for (i=k_min_key; i <= k_max_key; i++)
    {
        sKey = wxString::Format("/Constrains/IdfyTonality/%s/KeySignature%d",
            m_sSection.wx_str(), i );
        pPrefs->Read(sKey, &fValid, true);
        m_oValidKeys.SetValid((EKeySignature)i, fValid);
    }

    //answer buttons. Default: use major/minor buttons
    sKey = "/Constrains/IdfyTonality/UseMajorMinorButtons";
    pPrefs->Read(sKey, &m_fUseMajorMinorButtons, true);
}


}   // namespace lenmus
