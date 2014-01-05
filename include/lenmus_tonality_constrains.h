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

#ifndef __LENMUS_TONALITY_CONSTRAINS_H__        //to avoid nested includes
#define __LENMUS_TONALITY_CONSTRAINS_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_generators.h"
#include "lenmus_constrains.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>


namespace lenmus
{

class TonalityConstrains : public ExerciseOptions
{
public:
    TonalityConstrains(wxString sSection, ApplicationScope& appScope);
    ~TonalityConstrains() {}

    inline KeyConstrains* GetKeyConstrains() { return &m_oValidKeys; }

    inline bool UseMajorMinorButtons() { return m_fUseMajorMinorButtons; }

    inline void UseMajorMinorButtons(bool fValue) { m_fUseMajorMinorButtons = fValue; }

    inline bool IsValidKey(EKeySignature nKey) {
        return m_oValidKeys.IsValid(nKey);
    }

    inline void SetSection(wxString sSection) {
        m_sSection = sSection;
        load_settings();
    }

    void save_settings();

private:
    void load_settings();

    bool              m_fUseMajorMinorButtons;
    KeyConstrains     m_oValidKeys;           //allowed key signatures
};


}   //namespace lenmus

#endif  // __LENMUS_TONALITY_CONSTRAINS_H__
