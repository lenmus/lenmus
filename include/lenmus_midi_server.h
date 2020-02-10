//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2020 LenMus project
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation,
//    either version 3 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//18
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
#include "lenmus_injectors.h"   //to use wxMidi
#include "lenmus_string.h"      //to use to_std_string
#include "wxMidi.h"    //to use wxMidi

//lomse headers
#include <lomse_score_player.h>
#include <lomse_logger.h>
using namespace lomse;

//other
#include <fluidsynth.h>


#define Synthesizer lomse::MidiServerBase


namespace lenmus
{

/** -------------------------------------------------------------------------------------
    An internal Synthesizer using the FluidSynth library
*/
class FluidSynthesizer : public Synthesizer
{
protected:
    ApplicationScope&       m_appScope;
    MidiServer*             m_pMidiServer;
    fluid_settings_t*       m_pSettings;
    fluid_synth_t*          m_pSynth;
    fluid_audio_driver_t*   m_pDriver;
    int                     m_sfontId;

public:
    FluidSynthesizer(ApplicationScope& appScope, MidiServer* parent);
    ~FluidSynthesizer();

    //mandatory overrides from MidiServerBase
    void program_change(int channel, int instr) override;
    void voice_change(int channel, int instr) override;
    void note_on(int channel, int pitch, int volume) override;
    void note_off(int channel, int pitch, int volume) override;
    void all_sounds_off() override;
};

//---------------------------------------------------------------------------------------
//stores current MIDI configuration and interfaces with the MIDI synthesizer
class MidiServer
{
protected:
    ApplicationScope& m_appScope;
    wxMidiSystem*  m_pMidiSystem;       //MIDI system
    wxMidiInDevice* m_pMidiIn;          //in device object
    wxMidiOutDevice*  m_pMidiOut;       //out device object

    FluidSynthesizer*    m_pFluidSynth;     //synthesizer using FluidSynth

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
    inline wxMidiInDevice* get_in_device() { return m_pMidiIn; }
    inline wxMidiOutDevice* get_out_device() { return m_pMidiOut; }
    inline Synthesizer* get_current_synth() { return m_pFluidSynth; }

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

protected:
    void LoadUserPreferences();
};


}   //namespace lenmus


#endif    // __LENMUS_MIDI_SERVER_H__

