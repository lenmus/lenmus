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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "WaveManager.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "WaveManager.h"
#include "wx/filename.h"

// access to paths
#include "../globals/Paths.h"
extern lmPaths* g_pPaths;


//-------------------------------------------------------------------------------------------
// lmWaveManager implementation
//-------------------------------------------------------------------------------------------

lmWaveManager* lmWaveManager::m_pInstance = (lmWaveManager*)NULL;

lmWaveManager::lmWaveManager()
{
    //load sounds from files
    wxString sPath = g_pPaths->GetSoundsPath();
    wxFileName oFileWrong = wxFileName(sPath, _T("wrong"), _T("wav"), wxPATH_NATIVE);
    m_oWrong.Create(oFileWrong.GetFullPath());

    wxFileName oFileRight = wxFileName(sPath, _T("right"), _T("wav"), wxPATH_NATIVE);
    m_oRight.Create(oFileRight.GetFullPath());

}

lmWaveManager::~lmWaveManager()
{
}

lmWaveManager* lmWaveManager::GetInstance()
{
    if (!m_pInstance) {
        m_pInstance = new lmWaveManager();
        m_pInstance->LoadUserPreferences();
    }
    return m_pInstance;
}

void lmWaveManager::Destroy()
{
    if (m_pInstance) {
        delete m_pInstance;
        m_pInstance = (lmWaveManager*)NULL;
    }
}


void lmWaveManager::LoadUserPreferences()
{
    ////load settings form user congiguration data or default values
    //g_pPrefs->Read(_T("/Wave/IsSet"), &m_fMidiSet, false );

    //m_nOutDevId = (int)g_pPrefs->Read(_T("/Wave/Wrong"), (long)0);            // 0 based. So this is device 1
    //m_nVoiceChannel = (int)g_pPrefs->Read(_T("/Wave/Right"), (long)0);    // 0 based. So this is channel 1

}

void lmWaveManager::SaveUserPreferences()
{
    ////save settings in user congiguration data
    //g_pPrefs->Write(_T("/Wave/Wrong"), m_fMidiSet );   
    //g_pPrefs->Write(_T("/Wave/Right"), (long)m_nInDevId );

}

void lmWaveManager::WrongAnswerSound()
{
    if (m_oWrong.IsOk()) m_oWrong.Play();
}

void lmWaveManager::RightAnswerSound()
{
    if (m_oRight.IsOk()) m_oRight.Play();
}

