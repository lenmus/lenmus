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
//
//    You should have received a copy of the GNU General Public License along with this
//    program. If not, see <http://www.gnu.org/licenses/>.
//
//    For any comment, suggestion or feature request, please contact the manager of
//    the project at cecilios@users.sourceforge.net
//
//---------------------------------------------------------------------------------------

#include "lenmus_midi_server.h"

//lenmus
#include "lenmus_config.h"
#include "lenmus_paths.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>


namespace lenmus
{

#define DEFAULT_TAG     "[Default] "

//=======================================================================================
// FluidSynthesizer implementation
//=======================================================================================
FluidSynthesizer::FluidSynthesizer(ApplicationScope& appScope, MidiServer* parent)
    : Synthesizer(appScope, parent)
    , m_fValid(false)
    , m_pSettings(nullptr)
    , m_pSynth(nullptr)
    , m_pDriver(nullptr)
{
}

//---------------------------------------------------------------------------------------
FluidSynthesizer::~FluidSynthesizer()
{
    delete_all();
}

//---------------------------------------------------------------------------------------
void FluidSynthesizer::delete_all()
{
    if (m_pDriver)
        delete_fluid_audio_driver(m_pDriver);
    if (m_pSynth)
        delete_fluid_synth(m_pSynth);
    if (m_pSettings)
        delete_fluid_settings(m_pSettings);

    m_pSettings = nullptr;
    m_pSynth = nullptr;
    m_pDriver = nullptr;

    m_fValid = false;
}

//---------------------------------------------------------------------------------------
void FluidSynthesizer::program_change(int channel, int instr)
{
    if (m_fValid)
        fluid_synth_program_change (m_pSynth, channel, instr);
}

//---------------------------------------------------------------------------------------
void FluidSynthesizer::voice_change(int channel, int instr)
{
    if (m_fValid)
        m_pMidiServer->VoiceChange(channel, instr);
}

//---------------------------------------------------------------------------------------
void FluidSynthesizer::note_on(int channel, int pitch, int volume)
{
    if (m_fValid)
        fluid_synth_noteon(m_pSynth, channel, pitch, volume);
}

//---------------------------------------------------------------------------------------
void FluidSynthesizer::note_off(int channel, int pitch, int WXUNUSED(volume))
{
    if (m_fValid)
        fluid_synth_noteoff(m_pSynth, channel, pitch);
}

//---------------------------------------------------------------------------------------
void FluidSynthesizer::all_sounds_off()
{
    if (m_fValid)
    {
        for (int channel=0; channel < 16; ++channel)
            fluid_synth_all_sounds_off(m_pSynth, channel);
    }
}

//---------------------------------------------------------------------------------------
bool FluidSynthesizer::load_soundfont(const string& path)
{
    //return true if error

    if (path.rfind(DEFAULT_TAG, 0) == 0)
        m_soundfont = get_default_soundfont();
    else
        m_soundfont = path;

    m_sfontId = fluid_synth_sfload(m_pSynth, path.c_str(), 1);
    if(m_sfontId == FLUID_FAILED)
    {

        LOMSE_LOG_ERROR("SoundFont load failed. path=%s", path.c_str());
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------------------
string FluidSynthesizer::get_soundfont()
{
    if (m_soundfont == get_default_soundfont())
        return DEFAULT_TAG + m_soundfont;
    else
        return m_soundfont;
}

//---------------------------------------------------------------------------------------
void FluidSynthesizer::configure()
{
    delete_all();
    m_fValid = true;        //assume there will be no errors

    //create and configure the FluidSynth settings
    m_pSettings = new_fluid_settings();
    if (m_pSettings == nullptr)
    {
        LOMSE_LOG_ERROR("Failed to create the FluidSynth settings");
        m_fValid = false;
        return;
    }
 	fluid_settings_setstr(m_pSettings, "audio.driver", LENMUS_AUDIO_DRIVER);
 	LOMSE_LOG_INFO("Using '%s' audio driver", LENMUS_AUDIO_DRIVER);

    if (strcmp(LENMUS_AUDIO_DRIVER, "alsa") == 0 && strcmp(LENMUS_ALSA_DEVICE, "default") != 0)
     	fluid_settings_setstr(m_pSettings, "audio.alsa.device", LENMUS_ALSA_DEVICE);

    else if (strcmp(LENMUS_AUDIO_DRIVER, "dsound") == 0 && strcmp(LENMUS_DSOUND_DEVICE, "default") != 0)
     	fluid_settings_setstr(m_pSettings, "audio.dsound.device", LENMUS_DSOUND_DEVICE);

    else if (strcmp(LENMUS_AUDIO_DRIVER, "coreaudio") == 0 && strcmp(LENMUS_COREAUDIO_DEVICE, "default") != 0)
     	fluid_settings_setstr(m_pSettings, "audio.coreaudio.device", LENMUS_COREAUDIO_DEVICE);


    //create the FluidSynth synthesizer
    m_pSynth = new_fluid_synth(m_pSettings);
    if (m_pSynth == nullptr)
    {
        LOMSE_LOG_ERROR("Failed to create the FluidSynth synthesizer");
        m_fValid = false;
        return;
    }

    //create the FluidSynth audio driver
    m_pDriver = new_fluid_audio_driver(m_pSettings, m_pSynth);
    if (m_pDriver == nullptr)
    {
        LOMSE_LOG_ERROR("Failed to create the FluidSynth audio driver");
        m_fValid = false;
        return;
    }

    //load the SoundFont
    m_fValid = !load_soundfont(m_soundfont);
}

//---------------------------------------------------------------------------------------
void FluidSynthesizer::load_user_preferences()
{
    wxConfigBase* pPrefs = m_appScope.get_preferences();
    wxString defaultSoundfont = to_wx_string( get_default_soundfont() );
    wxString soundfont(defaultSoundfont);
    wxString file;
    pPrefs->Read("/Midi/SoundFont", &file, soundfont);

    if (file.rfind(DEFAULT_TAG, 0) == 0)
        m_soundfont = to_std_string(defaultSoundfont);
    else
        m_soundfont = to_std_string(file);
}

//---------------------------------------------------------------------------------------
void FluidSynthesizer::save_user_preferences()
{
    wxConfigBase* pPrefs = m_appScope.get_preferences();

    if (get_default_soundfont() == m_soundfont)
    {
        pPrefs->Write("/Midi/SoundFont", DEFAULT_TAG + to_wx_string(m_soundfont) );
    }
    else
    {
        pPrefs->Write("/Midi/SoundFont", to_wx_string(m_soundfont) );
    }

    pPrefs->Flush();
}

//---------------------------------------------------------------------------------------
void FluidSynthesizer::reset_to_defaults()
{
    m_fValid = !load_soundfont( get_default_soundfont() );
}

//---------------------------------------------------------------------------------------
string FluidSynthesizer::get_default_soundfont()
{
    Paths* pPaths = m_appScope.get_paths();
    wxString soundsPath = pPaths->GetSoundFontsPath();

    string soundfont(soundsPath.ToStdString());
    soundfont += "FluidR3_GM.sf2";
    return soundfont;
}


//=======================================================================================
// ExternalSynthesizer implementation
//=======================================================================================
ExternalSynthesizer::ExternalSynthesizer(ApplicationScope& appScope, MidiServer* parent)
    : Synthesizer(appScope, parent)
    , m_pMidiSystem(nullptr)
    , m_pMidiIn(nullptr)
    , m_pMidiOut(nullptr)
{
    m_pMidiSystem = wxMidiSystem::GetInstance();
}

//---------------------------------------------------------------------------------------
ExternalSynthesizer::~ExternalSynthesizer()
{
    if (m_pMidiIn)
        m_pMidiIn->Close();
    if (m_pMidiOut)
        m_pMidiOut->Close();

    delete m_pMidiIn;
    delete m_pMidiOut;
    delete m_pMidiSystem;
}

//---------------------------------------------------------------------------------------
void ExternalSynthesizer::program_change(int channel, int instr)
{
    m_pMidiOut->ProgramChange(channel, instr);
}

//---------------------------------------------------------------------------------------
void ExternalSynthesizer::voice_change(int channel, int instr)
{
    m_pMidiServer->VoiceChange(channel, instr);
}

//---------------------------------------------------------------------------------------
void ExternalSynthesizer::note_on(int channel, int pitch, int volume)
{
    m_pMidiOut->NoteOn(channel, pitch, volume);
}

//---------------------------------------------------------------------------------------
void ExternalSynthesizer::note_off(int channel, int pitch, int volume)
{
    m_pMidiOut->NoteOff(channel, pitch, volume);
}

//---------------------------------------------------------------------------------------
void ExternalSynthesizer::all_sounds_off()
{
    m_pMidiOut->AllSoundsOff();
}

//---------------------------------------------------------------------------------------
void ExternalSynthesizer::configure()
{
    //int nInDev = m_nInDevId;    //save current Id
    m_nInDevId = -1;            //mark as 'not set yet'

    int nOutDev = m_nOutDevId;    //save current Id
    m_nOutDevId = -1;            //mark as 'not set yet'

    SetOutDevice(nOutDev);

//    //set sound for instrument
//    program_change(m_pMidiServer->get_voice_channel(), m_pMidiServer->get_voice_instr());
//
//    //set sound for metronome
//    program_change(pMidi->MtrChannel(), pMidi->MtrInstr());

    m_fMidiOK = true;
}

//---------------------------------------------------------------------------------------
void ExternalSynthesizer::SetOutDevice(int nOutDevId)
{
    wxMidiError nErr;

    //if out device Id has changed close current device and open the new one
    if (!m_pMidiOut || (m_nOutDevId != nOutDevId))
    {
        //close current device
         if (m_pMidiOut)
         {
            nErr = m_pMidiOut->Close();
            delete m_pMidiOut;
            m_pMidiOut = nullptr;
            //TODO better error reporting
            if (nErr)
            {
                wxMessageBox( wxString::Format(
                    "Error %d while closing Midi device: %s \n"
                    , nErr, m_pMidiSystem->GetErrorText(nErr).wx_str() ));
                m_fMidiOK = false;
                return;
            }
        }

        //open new one
        m_nOutDevId = nOutDevId;
        if (m_nOutDevId != -1)
        {
            try
            {
                m_pMidiOut = LENMUS_NEW wxMidiOutDevice(m_nOutDevId);
                nErr = m_pMidiOut->Open(0, nullptr);        // 0 latency, no driver user info
            }
            catch(...)      //handle all exceptions
            {
				LOMSE_LOG_ERROR("Crash opening Midi device");
				return;
            }
            if (nErr)
            {
				LOMSE_LOG_ERROR("Error %d opening Midi device", nErr);
                m_fMidiOK = false;
                return;
            }
            else
				LOMSE_LOG_INFO("Midi out device correctly set. OutDevId=%d", m_nOutDevId);
        }
    }
}

//---------------------------------------------------------------------------------------
void ExternalSynthesizer::SetInDevice(int nInDevId)
{
    wxMidiError nErr;

    //if in device Id has changed close current device and open the new one
    if (!m_pMidiIn || (m_nInDevId != nInDevId))
    {
        //close current device
         if (m_pMidiIn)
         {
            nErr = m_pMidiIn->Close();
            delete m_pMidiIn;
            m_pMidiIn = nullptr;
            //TODO better error reporting
            if (nErr)
            {
                wxMessageBox( wxString::Format(
                    "Error %d in Open: %s \n",
                    nErr, m_pMidiSystem->GetErrorText(nErr).wx_str() ));
                m_fMidiOK = false;
                return;
            }
        }

        //open new one
        m_nInDevId = nInDevId;
        if (m_nInDevId != -1)
        {
            m_pMidiIn = LENMUS_NEW wxMidiInDevice(m_nInDevId);
            // open input device
            nErr = m_pMidiIn->Open(nullptr);        // 0 latency, no driver user info
            //TODO better error reporting
            if (nErr)
            {
                wxMessageBox( wxString::Format(
                    "Error %d in Open: %s \n"
                    , nErr, m_pMidiSystem->GetErrorText(nErr).wx_str() ));
                m_fMidiOK = false;
                return;
            }
        }
    }
}

//---------------------------------------------------------------------------------------
int ExternalSynthesizer::CountDevices()
{
    return m_pMidiSystem->CountDevices();
}

//---------------------------------------------------------------------------------------
void ExternalSynthesizer::load_user_preferences()
{
    //load settings form user congiguration data or default values

    wxConfigBase* pPrefs = m_appScope.get_preferences();

    pPrefs->Read("/Midi/IsSet", &m_fMidiSet, false );

    m_nOutDevId = (int)pPrefs->Read("/Midi/OutDevice", (long)0);            // 0 based. So this is device 1
    m_nInDevId = (int)pPrefs->Read("/Midi/InDevice", (long)0);    // 0 based. So this is device 1

}

//---------------------------------------------------------------------------------------
void ExternalSynthesizer::save_user_preferences()
{
    //save settings in user congiguration data

    wxConfigBase* pPrefs = m_appScope.get_preferences();

    pPrefs->Write("/Midi/IsSet", m_fMidiSet );
    pPrefs->Write("/Midi/InDevice", (long)m_nInDevId );
    pPrefs->Write("/Midi/OutDevice", (long)m_nOutDevId );

	pPrefs->Flush();
}


//=======================================================================================
// MidiServer implementation
//=======================================================================================
MidiServer::MidiServer(ApplicationScope& appScope)
    : m_appScope(appScope)
    , m_pFluidSynth(nullptr)
    , m_pExtSynth(nullptr)
    , m_fMidiConfigured(false)
    , m_fUseInternalSynth(true)
    , m_nVoiceChannel(0)            //channel 1 for music
    , m_nVoiceInstr(0)              //instr 1 (grand piano)
	, m_nDefaultVoiceChannel(0)     //channel 1
	, m_nDefaultVoiceInstr(0)       //instr 1 (grand piano)
    , m_nMtrChannel(9)              //channel 10 for metronome;
    , m_nMtrInstr(0)                //instrument 1
    , m_nMtrTone1(76)              // 76-High Wood Block)
    , m_nMtrTone2(77)              // 77-Low Wood Block
{
    m_pFluidSynth = LENMUS_NEW FluidSynthesizer(m_appScope, this);
    m_pExtSynth = LENMUS_NEW ExternalSynthesizer(m_appScope, this);
    load_user_preferences();
    configure_synthesizers();
}

//---------------------------------------------------------------------------------------
MidiServer::~MidiServer()
{
    save_user_preferences();

    delete m_pFluidSynth;
    delete m_pExtSynth;
}

//---------------------------------------------------------------------------------------
void MidiServer::configure_synthesizers()
{
    if (m_pExtSynth)
        m_pExtSynth->configure();

    if (m_pFluidSynth)
        m_pFluidSynth->configure();

    m_fMidiConfigured = true;
}

//---------------------------------------------------------------------------------------
Synthesizer* MidiServer::get_current_synth()
{
    if (m_fUseInternalSynth)
        return m_pFluidSynth;
    else
        return m_pExtSynth;
}

//---------------------------------------------------------------------------------------
void MidiServer::set_metronome_tones(int nTone1, int nTone2)
{
    m_nMtrTone1 = nTone1;
    m_nMtrTone2 = nTone2;
}

//---------------------------------------------------------------------------------------
void MidiServer::set_metronome_program(int nChannel, int nInstrument)
{
    m_nMtrChannel = nChannel;
    m_nMtrInstr = nInstrument;
}

//---------------------------------------------------------------------------------------
void MidiServer::VoiceChange(int nChannel, int nInstrument)
{
    //save new channel and instrument
    m_nVoiceChannel = nChannel;
    m_nVoiceInstr = nInstrument;

    Synthesizer* pSynth = get_current_synth();
    if (pSynth)
        pSynth->program_change(m_nVoiceChannel, m_nVoiceInstr);
}

//---------------------------------------------------------------------------------------
void MidiServer::do_sound_test()
{
    Synthesizer* pSynth = get_current_synth();
    if (!pSynth)
        return;

    //Play a scale
    int scale[] = { 60, 62, 64, 65, 67, 69, 71, 72 };
    #define SCALE_SIZE 8

    pSynth->program_change(m_nVoiceChannel, m_nVoiceInstr);
    for (int i = 0; i < SCALE_SIZE; i++)
    {
        pSynth->note_on(m_nVoiceChannel, scale[i], 100);
        ::wxMilliSleep(200);    // wait 200ms
        pSynth->note_off(m_nVoiceChannel, scale[i], 100);
    }
}

//---------------------------------------------------------------------------------------
void MidiServer::load_user_preferences()
{
    //load settings form user configuration data or default values

    wxConfigBase* pPrefs = m_appScope.get_preferences();

    pPrefs->Read("/Midi/UseInternalSynth", &m_fUseInternalSynth, true);

    m_nVoiceInstr = (int)pPrefs->Read("/Midi/get_voice_instr", (long)0);        // 0 based. So this is instrument 1 (grand piano)

    m_nMtrChannel = (int)pPrefs->Read("/Midi/MtrChannel", 9);        // 0 based. So this is channel 10
    m_nMtrInstr = (int)pPrefs->Read("/Midi/MtrInstr", (long)0);    // 0 based. So this is instrument 1 (grand piano)
    m_nMtrTone1 = (int)pPrefs->Read("/Midi/MtrTone1", 76L);        // 76-High Wood Block
    m_nMtrTone2 = (int)pPrefs->Read("/Midi/MtrTone2", 77L);        // 77-Low Wood Block

	m_nDefaultVoiceChannel = m_nVoiceChannel;
	m_nDefaultVoiceInstr = m_nVoiceInstr;

    //sanity checks in case configuration file is corrupted
    if (m_nVoiceChannel < 0 || m_nVoiceChannel > 15) m_nVoiceChannel = 0;
    if (m_nVoiceInstr < 0 || m_nVoiceInstr > 255) m_nVoiceInstr = 0;
    if (m_nMtrChannel < 0 || m_nMtrChannel > 15) m_nMtrChannel = 9;
    if (m_nMtrInstr < 0 || m_nMtrInstr > 255) m_nMtrInstr = 0;
    if (m_nMtrTone1 < 0 || m_nMtrTone1 > 255) m_nMtrTone1 = 60;
    if (m_nMtrTone2 < 0 || m_nMtrTone2 > 255) m_nMtrTone2 = 61;


    //asks synthesizers to load user preferences
    if (m_pFluidSynth)
        m_pFluidSynth->load_user_preferences();
    if (m_pExtSynth)
        m_pExtSynth->load_user_preferences();
}

//---------------------------------------------------------------------------------------
void MidiServer::save_user_preferences()
{
    //save settings in user congiguration data

    wxConfigBase* pPrefs = m_appScope.get_preferences();

    pPrefs->Write("/Midi/UseInternalSynth", m_fUseInternalSynth);

    pPrefs->Write("/Midi/get_voice_channel", (long)m_nVoiceChannel );
    pPrefs->Write("/Midi/get_voice_instr", (long)m_nVoiceInstr );
    pPrefs->Write("/Midi/MtrChannel", (long)m_nMtrChannel );
    pPrefs->Write("/Midi/MtrInstr", (long)m_nMtrInstr );
    pPrefs->Write("/Midi/MtrTone1", (long)m_nMtrTone1 );
    pPrefs->Write("/Midi/MtrTone2", (long)m_nMtrTone2 );

	m_nDefaultVoiceChannel = m_nVoiceChannel;
	m_nDefaultVoiceInstr = m_nVoiceInstr;

	pPrefs->Flush();

    //asks synthesizers to save user preferences
    if (m_pFluidSynth)
        m_pFluidSynth->save_user_preferences();
    if (m_pExtSynth)
        m_pExtSynth->save_user_preferences();
}


}   //namespace lenmus

