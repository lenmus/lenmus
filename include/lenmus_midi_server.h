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


namespace lenmus
{

/** -------------------------------------------------------------------------------------
    An abstract class from which all MidiServers must derive
*/
class Synthesizer : public lomse::MidiServerBase
{
protected:
    ApplicationScope&       m_appScope;
    MidiServer*             m_pMidiServer;

public:
    Synthesizer(ApplicationScope& appScope, MidiServer* parent)
        : m_appScope(appScope)
        , m_pMidiServer(parent)
    {
    }
    ~Synthesizer() {}

    //virtual methods
    virtual void save_user_preferences() {}
    virtual void load_user_preferences() {}
    virtual void configure() = 0;

};

/** -------------------------------------------------------------------------------------
    An internal Synthesizer using the FluidSynth library
*/
class FluidSynthesizer : public Synthesizer
{
protected:
    bool                    m_fValid;       //Synthesizer initialized
    fluid_settings_t*       m_pSettings;
    fluid_synth_t*          m_pSynth;
    fluid_audio_driver_t*   m_pDriver;
    int                     m_sfontId;
    string                  m_soundfont;

public:
    FluidSynthesizer(ApplicationScope& appScope, MidiServer* parent);
    ~FluidSynthesizer();

    void configure() override;
    bool load_soundfont(const string& path);
    string get_soundfont();

    //mandatory overrides from MidiServerBase
    void program_change(int channel, int instr) override;
    void voice_change(int channel, int instr) override;
    void note_on(int channel, int pitch, int volume) override;
    void note_off(int channel, int pitch, int volume) override;
    void all_sounds_off() override;

    //other
    void save_user_preferences() override;
    void load_user_preferences() override;
    void reset_to_defaults();
    string get_default_soundfont();

protected:
    void load_soundfont();
    void delete_all();

};

/** -------------------------------------------------------------------------------------
    An interface with any external Synthesizer. It uses wxMidi and the portmidi library
*/
class ExternalSynthesizer : public Synthesizer
{
protected:
    wxMidiSystem*       m_pMidiSystem;      //MIDI system
    wxMidiInDevice*     m_pMidiIn;          //in device object
    wxMidiOutDevice*    m_pMidiOut;         //out device object

    //MIDI in configuration
    int		m_nInDevId;

    //MIDI out configuration
    int		m_nOutDevId;

    bool    m_fMidiSet;     //the synthesizer has been configured
    bool    m_fMidiOK;


public:
    ExternalSynthesizer(ApplicationScope& appScope, MidiServer* parent);
    ~ExternalSynthesizer();


    inline wxMidiInDevice* get_in_device() { return m_pMidiIn; }
    inline wxMidiOutDevice* get_out_device() { return m_pMidiOut; }

    //setup
    void configure() override;

    void SetUpMidiOut(int nOutDevId, int nChannel, int nInstrument);
    void SetOutDevice(int nOutDevId);
    void SetInDevice(int nInDevId);
    inline void set_configured(bool value) { m_fMidiSet = value; }

    //Has user defined the MIDI configuration?
    inline bool is_configured() { return m_fMidiSet; }

    //Is MIDI operative?
    inline bool is_operative() { return m_fMidiOK; }

    //access to methods in midi system
    int CountDevices();

    //Access to MIDI configuration information
    inline int InDevId() { return m_nInDevId; }
    inline int OutDevId() { return m_nOutDevId; }

    //mandatory overrides from MidiServerBase
    void program_change(int channel, int instr) override;
    void voice_change(int channel, int instr) override;
    void note_on(int channel, int pitch, int volume) override;
    void note_off(int channel, int pitch, int volume) override;
    void all_sounds_off() override;

    //other
    void save_user_preferences() override;
    void load_user_preferences() override;

};

//---------------------------------------------------------------------------------------
//stores current MIDI configuration and manages the MIDI synthesizers
class MidiServer
{
protected:
    ApplicationScope& m_appScope;
    FluidSynthesizer*       m_pFluidSynth;     //synthesizer using FluidSynth
    ExternalSynthesizer*    m_pExtSynth;       //external synthesizer

    bool    m_fMidiConfigured;     //the selected synthesizer has been configured
    bool    m_fUseInternalSynth;

    //current channel and instrument for playing music exercises
    int		m_nVoiceChannel;
    int		m_nVoiceInstr;

    //default channel and instrument for playing music exercises
	int		m_nDefaultVoiceChannel;
	int		m_nDefaultVoiceInstr;

    //settings for metronome sounds
    int		m_nMtrChannel;
    int		m_nMtrInstr;
    int		m_nMtrTone1;
    int		m_nMtrTone2;

public:
    MidiServer(ApplicationScope& appScope);
    ~MidiServer();

    void configure_synthesizers();
    inline bool is_configured() { return m_fMidiConfigured; }

    //services
    Synthesizer* get_current_synth();
    inline ExternalSynthesizer* get_external_synth() { return m_pExtSynth; }
    inline FluidSynthesizer* get_internal_synth() { return m_pFluidSynth; }
    void save_user_preferences();
    void do_sound_test();
    inline bool is_using_internal_synth() { return m_fUseInternalSynth; }
    inline void use_internal_synth(bool value) { m_fUseInternalSynth = value; }

    //user configuration
    void set_metronome_program(int nChannel, int nInstrument);
    void set_metronome_tones(int nTone1, int nTone2);

    //change current channel and instr for music
    void VoiceChange(int nChannel, int nInstrument);

    //Access to user configuration information
    inline int get_voice_channel() { return m_nVoiceChannel; }
    inline int get_voice_instr() { return m_nVoiceInstr; }
    inline int MtrChannel() { return m_nMtrChannel; }
    inline int MtrInstr() { return m_nMtrInstr; }
    inline int MtrTone1() { return m_nMtrTone1; }
    inline int MtrTone2() { return m_nMtrTone2; }

	//default instrument for playing music exercises
    inline int get_default_voice_channel() { return m_nDefaultVoiceChannel; }
    inline int get_default_voice_instr() { return m_nDefaultVoiceInstr; }

protected:
    void load_user_preferences();

};


}   //namespace lenmus


#endif    // __LENMUS_MIDI_SERVER_H__

