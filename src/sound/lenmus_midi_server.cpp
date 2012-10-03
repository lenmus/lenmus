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

#include "lenmus_midi_server.h"

#include <wx/wxprec.h>
#include <wx/wx.h>


namespace lenmus
{

//=======================================================================================
// MidiServer implementation
//=======================================================================================
MidiServer::MidiServer(ApplicationScope& appScope)
    : m_appScope(appScope)
    , m_pMidiSystem(NULL)
    , m_pMidiIn(NULL)
    , m_pMidiOut(NULL)
{
    m_pMidiSystem = wxMidiSystem::GetInstance();
    LoadUserPreferences();
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
}

//---------------------------------------------------------------------------------------
void MidiServer::LoadUserPreferences()
{
    //load settings form user congiguration data or default values

    wxConfigBase* pPrefs = m_appScope.get_preferences();

    pPrefs->Read(_T("/Midi/IsSet"), &m_fMidiSet, false );

    m_nOutDevId = (int)pPrefs->Read(_T("/Midi/OutDevice"), (long)0);            // 0 based. So this is device 1
    m_nVoiceChannel = (int)pPrefs->Read(_T("/Midi/VoiceChannel"), (long)0);    // 0 based. So this is channel 1
    m_nVoiceInstr = (int)pPrefs->Read(_T("/Midi/VoiceInstr"), (long)0);        // 0 based. So this is instrument 1 (grand piano)

    m_nMtrChannel = (int)pPrefs->Read(_T("/Midi/MtrChannel"), 9);        // 0 based. So this is channel 10
    m_nMtrInstr = (int)pPrefs->Read(_T("/Midi/MtrInstr"), (long)0);    // 0 based. So this is instrument 1 (grand piano)
    m_nMtrTone1 = (int)pPrefs->Read(_T("/Midi/MtrTone1"), 60L);        // 60-High bongo
    m_nMtrTone2 = (int)pPrefs->Read(_T("/Midi/MtrTone2"), 61L);        // 61-low bongo

    m_nInDevId = (int)pPrefs->Read(_T("/Midi/InDevice"), (long)0);    // 0 based. So this is device 1

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

    pPrefs->Write(_T("/Midi/IsSet"), m_fMidiSet );
    pPrefs->Write(_T("/Midi/InDevice"), (long)m_nInDevId );
    pPrefs->Write(_T("/Midi/OutDevice"), (long)m_nOutDevId );
    pPrefs->Write(_T("/Midi/VoiceChannel"), (long)m_nVoiceChannel );
    pPrefs->Write(_T("/Midi/VoiceInstr"), (long)m_nVoiceInstr );
    pPrefs->Write(_T("/Midi/MtrChannel"), (long)m_nMtrChannel );
    pPrefs->Write(_T("/Midi/MtrInstr"), (long)m_nMtrInstr );
    pPrefs->Write(_T("/Midi/MtrTone1"), (long)m_nMtrTone1 );
    pPrefs->Write(_T("/Midi/MtrTone2"), (long)m_nMtrTone2 );

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
            m_pMidiOut = NULL;
            //TODO better error reporting
            if (nErr)
            {
                wxMessageBox( wxString::Format(
                    _T("Error %d while closing Midi device: %s \n")
                    , nErr, m_pMidiSystem->GetErrorText(nErr).c_str() ));
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
                nErr = m_pMidiOut->Open(0, NULL);        // 0 latency, no driver user info
            }
            catch(...)      //handle all exceptions
            {
				wxLogMessage(_T("[MidiServer::SetOutDevice] Crash opening Midi device"));
				return;
            }
            //TODO better error reporting
            if (nErr)
            {
				wxLogMessage(_T("[MidiServer::SetOutDevice] Error %d opening Midi device"), nErr);
                //wxMessageBox( wxString::Format(
                //    _T("Error %d while opening Midi device: %s \n"),
                //    nErr, m_pMidiSystem->GetErrorText(nErr).c_str() ));
                m_fMidiOK = false;
                return;
            }
            else
				wxLogMessage(_T("[MidiServer::SetOutDevice] Midi out device correctly set. OutDevId=%d"),
                             m_nOutDevId);
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
            m_pMidiIn = NULL;
            //TODO better error reporting
            if (nErr)
            {
                wxMessageBox( wxString::Format(
                    _T("Error %d in Open: %s \n"),
                    nErr, m_pMidiSystem->GetErrorText(nErr).c_str() ));
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
            nErr = m_pMidiIn->Open(NULL);        // 0 latency, no driver user info
            //TODO better error reporting
            if (nErr)
            {
                wxMessageBox( wxString::Format(
                    _T("Error %d in Open: %s \n")
                    , nErr, m_pMidiSystem->GetErrorText(nErr).c_str() ));
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

    //program new voices
    if (m_pMidiOut)
    {
        wxMidiError nErr = m_pMidiOut->ProgramChange(m_nVoiceChannel, m_nVoiceInstr);
        //TODO error reporting eLocalError
        if (nErr)
        {
            wxMessageBox( wxString::Format(
				_T("Error %d in ProgramChange:\n%s")
                , nErr, m_pMidiSystem->GetErrorText(nErr).c_str() ));
        }
    }

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

