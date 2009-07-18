//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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

#ifndef __LM_MIDIMANAGER_H__
#define __LM_MIDIMANAGER_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "MidiManager.cpp"
#endif

#include <wx/config.h>

//to use wxMidi
#include "../../wxMidi/include/wxMidi.h"


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

    //access to methods in midi system
    int CountDevices();

	//default instrument
    int DefaultVoiceChannel() { return m_nDefaultVoiceChannel; }
    int DefaultVoiceInstr() { return m_nDefaultVoiceInstr; }

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
    int		m_nInDevId;
    int		m_nOutDevId;

    int		m_nVoiceChannel;
    int		m_nVoiceInstr;
	int		m_nDefaultVoiceChannel;
	int		m_nDefaultVoiceInstr;


    int		m_nMtrChannel;
    int		m_nMtrInstr;
    int		m_nMtrTone1;
    int		m_nMtrTone2;

    bool    m_fMidiSet;
    bool    m_fMidiOK;

};

// global MIDI related variables
// direct access to MidiOutDevice and MidiInDevices to avoid delays
extern wxMidiInDevice* g_pMidiIn;       //in device object
extern wxMidiOutDevice* g_pMidiOut;     //out device object
extern lmMidiManager* g_pMidi;          //current MIDI configuration


#endif    // __LM_MIDIMANAGER_H__

