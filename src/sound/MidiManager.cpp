// RCS-ID: $Id: MidiManager.cpp,v 1.4 2006/02/23 19:25:44 cecilios Exp $
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
/*! @file MidiManager.cpp
    @brief Implementation file for class lmMidiManager
    @ingroup sound_management
*/
/*! @class lmMidiManager
    @ingroup sound_management
    @brief Stores current Midi configuration and interfaces with the MIDI API
*/
#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "MidiManager.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif


#include "MidiManager.h"

// the config object
extern wxConfigBase *g_pPrefs;

// global MIDI configuration variables
wxMidiSystem*        g_pMidiSystem = (wxMidiSystem*)NULL;    //MIDI system
wxMidiInDevice*        g_pMidiIn = (wxMidiInDevice*) NULL;        //in device object
wxMidiOutDevice*    g_pMidiOut = (wxMidiOutDevice*) NULL;    //out device object
lmMidiManager*        g_pMidi = (lmMidiManager*)NULL;            //current MIDI configuration

//-------------------------------------------------------------------------------------------
// lmMidiManager implementation
//-------------------------------------------------------------------------------------------

lmMidiManager* lmMidiManager::m_pInstance = (lmMidiManager*)NULL;

lmMidiManager::lmMidiManager()
{
}

lmMidiManager::~lmMidiManager()
{
    //close all devices
    if (g_pMidiIn) {
        g_pMidiIn->Close();
        delete     g_pMidiIn;
        g_pMidiIn = (wxMidiInDevice*)NULL;
    }

    if (g_pMidiOut) {
        g_pMidiOut->Close();
        delete     g_pMidiOut;
        g_pMidiOut = (wxMidiOutDevice*)NULL;
    }

    //delete midi system
    if (g_pMidiSystem) {
        delete g_pMidiSystem;
        g_pMidiSystem = (wxMidiSystem*)NULL;
    }

    g_pMidi = (lmMidiManager*)NULL;

}

lmMidiManager* lmMidiManager::GetInstance()
{
    if (!m_pInstance) {
        m_pInstance = new lmMidiManager();
        g_pMidiSystem = wxMidiSystem::GetInstance();
        m_pInstance->LoadUserPreferences();
    }
    return m_pInstance;
}

void lmMidiManager::LoadUserPreferences()
{
    //load settings form user congiguration data or default values
    g_pPrefs->Read(_T("/Midi/IsSet"), &m_fMidiSet, false );

    m_nOutDevId = (int)g_pPrefs->Read(_T("/Midi/OutDevice"), (long)0);            // 0 based. So this is device 1
    m_nVoiceChannel = (int)g_pPrefs->Read(_T("/Midi/VoiceChannel"), (long)0);    // 0 based. So this is channel 1
    m_nVoiceInstr = (int)g_pPrefs->Read(_T("/Midi/VoiceInstr"), (long)0);        // 0 based. So this is instrument 1 (grand piano)

    m_nMtrChannel = (int)g_pPrefs->Read(_T("/Midi/MtrChannel"), 9);        // 0 based. So this is channel 10
    m_nMtrInstr = (int)g_pPrefs->Read(_T("/Midi/MtrInstr"), (long)0);    // 0 based. So this is instrument 1 (grand piano)
    m_nMtrTone1 = (int)g_pPrefs->Read(_T("/Midi/MtrTone1"), 60L);        // 60-High bongo
    m_nMtrTone2 = (int)g_pPrefs->Read(_T("/Midi/MtrTone2"), 61L);        // 61-low bongo

    m_nInDevId = (int)g_pPrefs->Read(_T("/Midi/InDevice"), (long)0);    // 0 based. So this is device 1

}

void lmMidiManager::SaveUserPreferences()
{
    //save settings in user congiguration data
    g_pPrefs->Write(_T("/Midi/IsSet"), m_fMidiSet );   
    g_pPrefs->Write(_T("/Midi/InDevice"), (long)m_nInDevId );
    g_pPrefs->Write(_T("/Midi/OutDevice"), (long)m_nOutDevId );
    g_pPrefs->Write(_T("/Midi/VoiceChannel"), (long)m_nVoiceChannel );
    g_pPrefs->Write(_T("/Midi/VoiceInstr"), (long)m_nVoiceInstr );
    g_pPrefs->Write(_T("/Midi/MtrChannel"), (long)m_nMtrChannel );
    g_pPrefs->Write(_T("/Midi/MtrInstr"), (long)m_nMtrInstr );
    g_pPrefs->Write(_T("/Midi/MtrTone1"), (long)m_nMtrTone1 );
    g_pPrefs->Write(_T("/Midi/MtrTone2"), (long)m_nMtrTone2 );

}

void lmMidiManager::SetUpCurrentConfig()
{
    int nInDev = m_nInDevId;    //save current Id
    m_nInDevId = -1;            //mark as 'not set yet'

    int nOutDev = m_nOutDevId;    //save current Id
    m_nOutDevId = -1;            //mark as 'not set yet'
    SetUpMidiOut(nOutDev, m_nVoiceChannel, m_nVoiceInstr);

}

void lmMidiManager::SetOutDevice(int nOutDevId)
{
    wxMidiError nErr;

    //if out device Id has changed close current device and open the new one
    if (!g_pMidiOut || (m_nOutDevId != nOutDevId))
    {
        //close current device
         if (g_pMidiOut) {
            nErr = g_pMidiOut->Close();
            delete g_pMidiOut;
            g_pMidiOut = (wxMidiOutDevice*) NULL;
            //! @todo better error reporting
            if (nErr) {
                wxMessageBox( wxString::Format(
                    _T("Error %d in Open: %s \n"),
                    nErr, g_pMidiSystem->GetErrorText(nErr) ));
                m_fMidiOK = false;
                return;
            }
        }

        //open new one
        m_nOutDevId = nOutDevId;
        if (m_nOutDevId != -1) {
            g_pMidiOut = new wxMidiOutDevice(m_nOutDevId);
            // open output device
            nErr = g_pMidiOut->Open(0, NULL);        // 0 latency, no driver user info
            //! @todo better error reporting
            if (nErr) {
                wxMessageBox( wxString::Format(
                    _T("Error %d in Open: %s \n"),
                    nErr, g_pMidiSystem->GetErrorText(nErr) ));
                m_fMidiOK = false;
                return;
            }
        }
    }

}

void lmMidiManager::SetInDevice(int nInDevId)
{
    wxMidiError nErr;

    //if in device Id has changed close current device and open the new one
    if (!g_pMidiIn || (m_nInDevId != nInDevId))
    {
        //close current device
         if (g_pMidiIn) {
            nErr = g_pMidiIn->Close();
            delete g_pMidiIn;
            g_pMidiIn = (wxMidiInDevice*) NULL;
            //! @todo better error reporting
            if (nErr) {
                wxMessageBox( wxString::Format(
                    _T("Error %d in Open: %s \n"),
                    nErr, g_pMidiSystem->GetErrorText(nErr) ));
                m_fMidiOK = false;
                return;
            }
        }

        //open new one
        m_nInDevId = nInDevId;
        if (m_nInDevId != -1) {
            g_pMidiIn = new wxMidiInDevice(m_nInDevId);
            // open input device
            nErr = g_pMidiIn->Open(NULL);        // 0 latency, no driver user info
            //! @todo better error reporting
            if (nErr) {
                wxMessageBox( wxString::Format(
                    _T("Error %d in Open: %s \n"),
                    nErr, g_pMidiSystem->GetErrorText(nErr) ));
                m_fMidiOK = false;
                return;
            }
        }
    }

}

void lmMidiManager::SetUpMidiOut(int nOutDevId, int nChannel, int nInstrument)
{
    //open the new out device
    SetOutDevice(nOutDevId);

    //program new voices
    VoiceChange(nChannel, nInstrument);

}

void lmMidiManager::VoiceChange(int nChannel, int nInstrument)
{
    //save new channel and instrument
    m_nVoiceChannel = nChannel;
    m_nVoiceInstr = nInstrument;

    //program new voices
    if (g_pMidiOut) {
        wxMidiError nErr = g_pMidiOut->ProgramChange(m_nVoiceChannel, m_nVoiceInstr);
        //! @todo error reporting eLocalError
        if (nErr) {
            wxMessageBox( wxString::Format(
                _T("Error %d in ProgramChange\n"), nErr) );
        }
    }

    m_fMidiOK = true;

}

void lmMidiManager::SetMetronomeTones(int nTone1, int nTone2)
{
    m_nMtrTone1 = nTone1;
    m_nMtrTone2 = nTone2;
}

void lmMidiManager::TestOut()
{
    if (!g_pMidiOut) return;

    //Play a scale 
    int scale[] = { 60, 62, 64, 65, 67, 69, 71, 72 };
    #define SCALE_SIZE 8

    wxMidiTimestamp now = g_pMidiSystem->GetTime();
    for (int i = 0; i < SCALE_SIZE; i++) {
        g_pMidiOut->NoteOn(m_nVoiceChannel, scale[i], 100);
        ::wxMilliSleep(200);    // wait 200ms
        g_pMidiOut->NoteOff(m_nVoiceChannel, scale[i], 100);
    }

}
