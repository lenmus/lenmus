// RCS-ID: $Id: MidiManager.h,v 1.3 2006/02/23 19:25:44 cecilios Exp $
//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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
/*! @file MidiManager.h
    @brief Header file for class lmMidiManager
    @ingroup sound_management
*/
#ifndef __MIDICONFIG_H__
#define __MIDICONFIG_H__

#include <wx/config.h>

//to use wxMidi
#include "wxMidi.h"


//This class is a singleton
class lmMidiManager
{
public:
    ~lmMidiManager();
    static lmMidiManager* GetInstance();

    //set up configuration
    void SetUpMidiOut(int nOutDevId, int nChannel, int nInstrument);
    void SetOutDevice(int nOutDevId);
    void SetInDevice(int nInDevId);
    void SetUpCurrentConfig();
    void VoiceChange(int nChannel, int nInstrument);
    void SetMetronomeTones(int nTone1, int nTone2);

    //Has user defined the MIDI configuration?
    bool IsConfigured() { return m_fMidiSet; }

    //Is MIDI operative?
    bool IsOperative() { return m_fMidiOK; }

    //Access to MIDI configuration information
    int InDevId() { return m_nInDevId; }
    int OutDevId() { return m_nOutDevId; }
    int VoiceChannel() { return m_nVoiceChannel; }
    int VoiceInstr() { return m_nVoiceInstr; }
    int MtrChannel() { return m_nMtrChannel; }
    int MtrInstr() { return m_nMtrInstr; }
    int MtrTone1() { return m_nMtrTone1; }
    int MtrTone2() { return m_nMtrTone2; }

    //other
    void SetConfigured(bool value) { m_fMidiSet = value; }
    void SaveUserPreferences();
    void TestOut();

protected:
    lmMidiManager();

private:
    //default values and user preferences
    void LoadUserPreferences();

        //
        // member variables
        //

    static lmMidiManager*    m_pInstance;    //the only instance of this class


    //MIDI configuration information
    int        m_nInDevId;
    int        m_nOutDevId;

    int        m_nVoiceChannel;
    int        m_nVoiceInstr;

    int        m_nMtrChannel;
    int        m_nMtrInstr;
    int        m_nMtrTone1;
    int        m_nMtrTone2;

    bool    m_fMidiSet;
    bool    m_fMidiOK;

};

// global MIDI related variables
extern wxMidiSystem* g_pMidiSystem;        //MIDI system
extern wxMidiInDevice* g_pMidiIn;        //in device object
extern wxMidiOutDevice* g_pMidiOut;        //out device object
extern lmMidiManager* g_pMidi;        //current MIDI configuration


#endif    // __MIDICONFIG_H__

