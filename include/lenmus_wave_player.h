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

#ifndef __LENMUS_WAVE_PLAYER_H__        //to avoid nested includes
#define __LENMUS_WAVE_PLAYER_H__

#include "lenmus_injectors.h"

// For compilers that support precompilation, includes <wx/wx.h>.
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/sound.h>

namespace lenmus
{


//---------------------------------------------------------------------------------------
class WavePlayer
{
private:
    ApplicationScope&   m_appScope;
    wxSound             m_oWrong;       //sound for wrong answers
    wxSound             m_oRight;       //sound for right answers

public:
    WavePlayer(ApplicationScope& appScope);
    ~WavePlayer() {}

    //produce predefined sounds
    void play_wrong_answer_sound();
    void play_right_answer_sound();

};


}   //namespace lenmus

#endif    // __LENMUS_WAVE_PLAYER_H__
