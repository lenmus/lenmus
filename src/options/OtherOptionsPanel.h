//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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
//-------------------------------------------------------------------------------------

#ifndef __LM_OTHEROPTIONSPANEL_H__
#define __LM_OTHEROPTIONSPANEL_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "OtherOptionsPanel.cpp"
#endif

#include "OptionsPanel.h"


class lmOtherOptionsPanel: public lmOptionsPanel
{
public:
    lmOtherOptionsPanel(wxWindow* parent);
    ~lmOtherOptionsPanel();
    bool Verify();
    void Apply();

private:

    // controls
        // Exercises
    wxCheckBox*     m_pChkAnswerSounds;       // Enable right/wrong answer sounds
    wxCheckBox*     m_pChkTeamCounters;       // Show two counters (for teams competition)
    wxCheckBox*     m_pChkAutoNewProblem;     // Show two counters (for teams competition)

};

#endif    // __LM_OTHEROPTIONSPANEL_H__
