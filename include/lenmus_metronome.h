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

#ifndef __LENMUS_METRONOME_H__        //to avoid nested includes
#define __LENMUS_METRONOME_H__

//lomse
#include "lomse_metronome.h"
using namespace lomse;

//lenmus
#include "lenmus_injectors.h"
#include "lenmus_midi_server.h"
using namespace lenmus;

//wxWidgets
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif


namespace lenmus
{

//forward declarations
class GlobalMetronome;


//---------------------------------------------------------------------------------------
class MetronomeTimer : public wxTimer
{
private:
    GlobalMetronome* m_pOwner;

public:
    MetronomeTimer(GlobalMetronome* pOwner) : wxTimer(), m_pOwner(pOwner) {}

    void Notify();

};

//---------------------------------------------------------------------------------------
class GlobalMetronome : public Metronome
{
private:
    int m_nSound;               //last click on sound
    MetronomeTimer* m_pTimer;   //timer associated to this metronome
    MidiServer* m_pMidiServer;

public:
    GlobalMetronome(ApplicationScope& appScope, long nMM = 60);
    ~GlobalMetronome();

    // mandatory overrides
    void start();
    void stop();

    // timer events handler
    void OnTimerEvent();

    // commands
    void DoClick(bool fFirstBeatOfBar=true);


private:
    void ClickOn(bool fFirstBeatOfBar);
    void ClickOff();

};


}   // namespace lenmus

#endif    // __LENMUS_METRONOME_H__

