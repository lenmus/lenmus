//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the 
//    terms of the GNU General Public License as published by the Free Software Foundation;
//    either version 2 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this 
//    program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, 
//    Fifth Floor, Boston, MA  02110-1301, USA.
//
//    For any comment, suggestion or feature request, please contact the manager of 
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

#ifndef __METRONOME_H__        //to avoid nested includes
#define __METRONOME_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

class lmMetronome;        //forward declaration

class lmMetronomeTimer : public wxTimer
{
public:
    lmMetronomeTimer(lmMetronome* pOwner) : wxTimer() { m_pOwner = pOwner; }

    virtual void Notify();

private:
    lmMetronome*    m_pOwner;

};

class lmMetronome
{
public:
    lmMetronome(long nMM = 60);
    ~lmMetronome();

    // settings
    void SetMM(long nMM);
    void SetInterval(long milliseconds);

    // accessors
    long GetMM() { return m_nMM; }
    long GetInterval() { return m_nInterval; }
    bool IsEnabled() { return m_fEnabled; }
    bool IsRunning() { return m_fRunning; }

    // timer events handler
    void OnTimerEvent();

    // commands
    void Start();
    void Stop();
    void Enable(bool fValue) { m_fEnabled = fValue; }


private:
    long        m_nMM;            //metronome frequency: beats per minute
    long        m_nInterval;    //metronome period: milliseconds between beats
    bool        m_fEnabled;        //metronome is enabled
    bool        m_fRunning;        //true if Start() invoked
    lmMetronomeTimer*    m_pTimer;        //timer associated to this metronome

};

#endif    // __METRONOME_H__

