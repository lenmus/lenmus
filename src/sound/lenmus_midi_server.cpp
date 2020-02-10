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

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>


namespace lenmus
{

//=======================================================================================
// FluidSynthesizer implementation
//=======================================================================================
FluidSynthesizer::FluidSynthesizer(ApplicationScope& appScope, MidiServer* parent)
    : m_appScope(appScope)
    , m_pMidiServer(parent)
    , m_pSettings(nullptr)
    , m_pSynth(nullptr)
    , m_pDriver(nullptr)
{
    //create and configure the FluidSynth settings
    m_pSettings = new_fluid_settings();
    if (m_pSettings == nullptr)
    {
        LOMSE_LOG_ERROR("Failed to create the FluidSynth settings");
    }
 	fluid_settings_setstr(m_pSettings, "audio.driver", "alsa");

    //create the FluidSynth synthesizer
    m_pSynth = new_fluid_synth(m_pSettings);
    if (m_pSynth == nullptr)
    {
        LOMSE_LOG_ERROR("Failed to create the FluidSynth synthesizer");
    }

    //create the FluidSynth audio driver
    m_pDriver = new_fluid_audio_driver(m_pSettings, m_pSynth);
    if (m_pDriver == nullptr)
    {
        LOMSE_LOG_ERROR("Failed to create the FluidSynth audio driver");
    }

    //load the SoundFont
    m_sfontId = fluid_synth_sfload(m_pSynth, "/usr/share/sounds/sf2/FluidR3_GM.sf2", 1);
    if(m_sfontId == FLUID_FAILED)
    {
        LOMSE_LOG_ERROR("Loading the SoundFont failed!");
    }
}

//---------------------------------------------------------------------------------------
FluidSynthesizer::~FluidSynthesizer()
{
    if (m_pDriver)
        delete_fluid_audio_driver(m_pDriver);
    if (m_pSynth)
        delete_fluid_synth(m_pSynth);
    if (m_pSettings)
        delete_fluid_settings(m_pSettings);
}

//---------------------------------------------------------------------------------------
void FluidSynthesizer::program_change(int channel, int instr)
{
    //m_pMidiOut->ProgramChange(channel, instr);
    fluid_synth_program_change (m_pSynth, channel, instr);
}

//---------------------------------------------------------------------------------------
void FluidSynthesizer::voice_change(int channel, int instr)
{
    m_pMidiServer->VoiceChange(channel, instr);
}

//---------------------------------------------------------------------------------------
void FluidSynthesizer::note_on(int channel, int pitch, int volume)
{
    LOMSE_LOG_TRACE(Logger::k_score_player,
                    "Note On: channel %d, pitch %d, volume %d",
                    channel, pitch, volume);
    //m_pMidiOut->NoteOn(channel, pitch, volume);
    fluid_synth_noteon(m_pSynth, channel, pitch, volume);
}

//---------------------------------------------------------------------------------------
void FluidSynthesizer::note_off(int channel, int pitch, int WXUNUSED(volume))
{
    //m_pMidiOut->NoteOff(channel, pitch, volume);
    fluid_synth_noteoff(m_pSynth, channel, pitch);
}

//---------------------------------------------------------------------------------------
void FluidSynthesizer::all_sounds_off()
{
    LOMSE_LOG_TRACE(Logger::k_score_player, "All sounds off");
    //m_pMidiOut->AllSoundsOff();
    for (int channel=0; channel < 16; ++channel)
        fluid_synth_all_sounds_off(m_pSynth, channel);
}


//=======================================================================================
// MidiServer implementation
//=======================================================================================
MidiServer::MidiServer(ApplicationScope& appScope)
    : m_appScope(appScope)
    , m_pMidiSystem(nullptr)
    , m_pMidiIn(nullptr)
    , m_pMidiOut(nullptr)
    , m_pFluidSynth(nullptr)
{
    m_pMidiSystem = wxMidiSystem::GetInstance();
    LoadUserPreferences();

    m_pFluidSynth = LENMUS_NEW FluidSynthesizer(m_appScope, this);

}

//---------------------------------------------------------------------------------------
MidiServer::~MidiServer()
{
    if (m_pMidiIn)
        m_pMidiIn->Close();
    if (m_pMidiOut)
        m_pMidiOut->Close();

    delete m_pMidiIn;
    delete m_pMidiOut;
    delete m_pMidiSystem;
    delete m_pFluidSynth;
}

//---------------------------------------------------------------------------------------
void MidiServer::LoadUserPreferences()
{
    //load settings form user congiguration data or default values

    wxConfigBase* pPrefs = m_appScope.get_preferences();

    pPrefs->Read("/Midi/IsSet", &m_fMidiSet, false );

    m_nOutDevId = (int)pPrefs->Read("/Midi/OutDevice", (long)0);            // 0 based. So this is device 1
    m_nVoiceChannel = (int)pPrefs->Read("/Midi/VoiceChannel", (long)0);    // 0 based. So this is channel 1
    m_nVoiceInstr = (int)pPrefs->Read("/Midi/VoiceInstr", (long)0);        // 0 based. So this is instrument 1 (grand piano)

    m_nMtrChannel = (int)pPrefs->Read("/Midi/MtrChannel", 9);        // 0 based. So this is channel 10
    m_nMtrInstr = (int)pPrefs->Read("/Midi/MtrInstr", (long)0);    // 0 based. So this is instrument 1 (grand piano)
    m_nMtrTone1 = (int)pPrefs->Read("/Midi/MtrTone1", 76L);        // 76-High Wood Block
    m_nMtrTone2 = (int)pPrefs->Read("/Midi/MtrTone2", 77L);        // 77-Low Wood Block

    m_nInDevId = (int)pPrefs->Read("/Midi/InDevice", (long)0);    // 0 based. So this is device 1

	m_nDefaultVoiceChannel = m_nVoiceChannel;
	m_nDefaultVoiceInstr = m_nVoiceInstr;

    //sanity checks in case configuration file is corrupted
    if (m_nVoiceChannel < 0 || m_nVoiceChannel > 15) m_nVoiceChannel = 0;
    if (m_nVoiceInstr < 0 || m_nVoiceInstr > 255) m_nVoiceInstr = 0;
    if (m_nMtrChannel < 0 || m_nMtrChannel > 15) m_nMtrChannel = 9;
    if (m_nMtrInstr < 0 || m_nMtrInstr > 255) m_nMtrInstr = 0;
    if (m_nMtrTone1 < 0 || m_nMtrTone1 > 255) m_nMtrTone1 = 60;
    if (m_nMtrTone2 < 0 || m_nMtrTone2 > 255) m_nMtrTone2 = 61;
}

//---------------------------------------------------------------------------------------
void MidiServer::SaveUserPreferences()
{
    //save settings in user congiguration data

    wxConfigBase* pPrefs = m_appScope.get_preferences();

    pPrefs->Write("/Midi/IsSet", m_fMidiSet );
    pPrefs->Write("/Midi/InDevice", (long)m_nInDevId );
    pPrefs->Write("/Midi/OutDevice", (long)m_nOutDevId );
    pPrefs->Write("/Midi/VoiceChannel", (long)m_nVoiceChannel );
    pPrefs->Write("/Midi/VoiceInstr", (long)m_nVoiceInstr );
    pPrefs->Write("/Midi/MtrChannel", (long)m_nMtrChannel );
    pPrefs->Write("/Midi/MtrInstr", (long)m_nMtrInstr );
    pPrefs->Write("/Midi/MtrTone1", (long)m_nMtrTone1 );
    pPrefs->Write("/Midi/MtrTone2", (long)m_nMtrTone2 );

	m_nDefaultVoiceChannel = m_nVoiceChannel;
	m_nDefaultVoiceInstr = m_nVoiceInstr;

	pPrefs->Flush();
}

//---------------------------------------------------------------------------------------
void MidiServer::SetUpCurrentConfig()
{
    //int nInDev = m_nInDevId;    //save current Id
    m_nInDevId = -1;            //mark as 'not set yet'

    int nOutDev = m_nOutDevId;    //save current Id
    m_nOutDevId = -1;            //mark as 'not set yet'
    SetUpMidiOut(nOutDev, m_nVoiceChannel, m_nVoiceInstr);
}

//---------------------------------------------------------------------------------------
void MidiServer::SetOutDevice(int nOutDevId)
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
void MidiServer::SetInDevice(int nInDevId)
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
void MidiServer::SetUpMidiOut(int nOutDevId, int nChannel, int nInstrument)
{
    //open the new out device
    SetOutDevice(nOutDevId);

    //program new voices
    VoiceChange(nChannel, nInstrument);
}

//---------------------------------------------------------------------------------------
void MidiServer::VoiceChange(int nChannel, int nInstrument)
{
    //save new channel and instrument
    m_nVoiceChannel = nChannel;
    m_nVoiceInstr = nInstrument;

    if (m_pFluidSynth)
        m_pFluidSynth->program_change(m_nVoiceChannel, m_nVoiceInstr);

//    //program new voices
//    if (m_pMidiOut)
//    {
//        wxMidiError nErr = m_pMidiOut->ProgramChange(m_nVoiceChannel, m_nVoiceInstr);
//        //TODO error reporting eLocalError
//        if (nErr)
//        {
//            wxMessageBox( wxString::Format(
//				"Error %d in ProgramChange:\n%s"
//                , nErr, m_pMidiSystem->GetErrorText(nErr).wx_str() ));
//        }
//    }

    m_fMidiOK = true;
}

//---------------------------------------------------------------------------------------
void MidiServer::SetMetronomeTones(int nTone1, int nTone2)
{
    m_nMtrTone1 = nTone1;
    m_nMtrTone2 = nTone2;
}

//---------------------------------------------------------------------------------------
void MidiServer::TestOut()
{
    if (!m_pMidiOut) return;

    //Play a scale
    int scale[] = { 60, 62, 64, 65, 67, 69, 71, 72 };
    #define SCALE_SIZE 8

    for (int i = 0; i < SCALE_SIZE; i++)
    {
        m_pMidiOut->NoteOn(m_nVoiceChannel, scale[i], 100);
        ::wxMilliSleep(200);    // wait 200ms
        m_pMidiOut->NoteOff(m_nVoiceChannel, scale[i], 100);
    }
}

//---------------------------------------------------------------------------------------
int MidiServer::CountDevices()
{
    return m_pMidiSystem->CountDevices();
}


}   //namespace lenmus

