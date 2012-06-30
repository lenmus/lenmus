//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2012 LenMus project
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

#ifndef __LENMUS_MIDI_SERVER_H__
#define __LENMUS_MIDI_SERVER_H__

//lenmus headers
#include "lenmus_standard_header.h"
#include "lenmus_injectors.h"    //to use wxMidi
#include "wxMidi.h"    //to use wxMidi

//lomse headers
#include "lomse_score_player.h"
using namespace lomse;


namespace lenmus
{

//---------------------------------------------------------------------------------------
//stores current MIDI configuration and interfaces with the MIDI API
class MidiServer : public lomse::MidiServerBase
{
protected:
    ApplicationScope& m_appScope;
    wxMidiSystem*  m_pMidiSystem;       //MIDI system
    wxMidiInDevice* m_pMidiIn;          //in device object
    wxMidiOutDevice*  m_pMidiOut;       //out device object

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

public:
    MidiServer(ApplicationScope& appScope);
    ~MidiServer();

    //set up configuration
    void SetUpMidiOut(int nOutDevId, int nChannel, int nInstrument);
    void SetOutDevice(int nOutDevId);
    void SetInDevice(int nInDevId);
    void SetUpCurrentConfig();
    void VoiceChange(int nChannel, int nInstrument);
    void SetMetronomeTones(int nTone1, int nTone2);

    //services
    //wxMidiSystem*  m_pMidiSystem;       //MIDI system
    inline wxMidiInDevice* get_in_device() { return m_pMidiIn; }
    inline wxMidiOutDevice* get_out_device() { return m_pMidiOut; }

    //Has user defined the MIDI configuration?
    bool is_configured() { return m_fMidiSet; }

    //Is MIDI operative?
    bool is_operative() { return m_fMidiOK; }

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

    //mandatory overrides from MidiServerBase
    virtual void program_change(int channel, int instr) {
        m_pMidiOut->ProgramChange(channel, instr);
    }
    virtual void voice_change(int channel, int instr) {
        VoiceChange(channel, instr);
    }
    virtual void note_on(int channel, int pitch, int volume) {
        m_pMidiOut->NoteOn(channel, pitch, volume);
    }
    virtual void note_off(int channel, int pitch, int volume) {
        m_pMidiOut->NoteOff(channel, pitch, volume);
    }
    virtual void all_sounds_off() { m_pMidiOut->AllSoundsOff(); }



protected:
    void LoadUserPreferences();
};


}   //namespace lenmus


#endif    // __LENMUS_MIDI_SERVER_H__

