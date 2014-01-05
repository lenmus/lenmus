//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2014 LenMus project
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

#ifndef __LENMUS_CADENCESCONSTRAINS_H__        //to avoid nested includes
#define __LENMUS_CADENCESCONSTRAINS_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_generators.h"
#include "lenmus_constrains.h"
#include "lenmus_cadence.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>


namespace lenmus
{

enum ECadenceButtons
{
    lm_eCadButtonTerminal = 0,
    lm_eCadButtonTransient,
    lm_eCadButtonPerfect,
	lm_eCadButtonPlagal,
    lm_eCadButtonImperfect,
    lm_eCadButtonDeceptive,
    lm_eCadButtonHalf,

    lm_eCadMaxButton
};


class CadencesConstrains : public ExerciseOptions
{
public:
    CadencesConstrains(wxString sSection, ApplicationScope& appScope);
    ~CadencesConstrains() {}

    ECadenceType GetRandomCadence();

    bool IsCadenceValid(ECadenceType nType) { return m_fValidCadences[nType]; }
    void SetCadenceValid(ECadenceType nType, bool fValid) { m_fValidCadences[nType] = fValid; }
    bool* GetValidCadences() { return m_fValidCadences; }

    bool IsValidButton(ECadenceButtons nB) { return m_fValidButtons[nB]; }
    void SetValidButton(ECadenceButtons nB, bool fValue) { m_fValidButtons[nB] = fValue; }
    bool* GetValidButtons() { return m_fValidButtons; }

    int GetKeyDisplayMode() { return m_nKeyDisplayMode; }
    void SetKeyDisplayMode(int nKeyDisplayMode) { m_nKeyDisplayMode = nKeyDisplayMode; }

    void SetSection(wxString sSection) {
                m_sSection = sSection;
                load_settings();
            }

    void save_settings();

    KeyConstrains* GetKeyConstrains() { return &m_oValidKeys; }


private:
    void load_settings();

    bool                m_fValidButtons[lm_eCadMaxButton];
    bool                m_fValidCadences[k_cadence_max];
    KeyConstrains     m_oValidKeys;           //allowed key signatures

    //params only for ear training exercises
    int                 m_nKeyDisplayMode;      // 0-play A4 note
                                                // 1-play tonic chord

};


}   // namespace lenmus

#endif  // __LENMUS_CADENCESCONSTRAINS_H__
