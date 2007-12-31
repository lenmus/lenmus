//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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
#pragma implementation "Preferences.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Preferences.h"

// to save config information into a file
#include "wx/confbase.h"
#include "wx/fileconf.h"
#include "wx/filename.h"



wxConfigBase* g_pPrefs = (wxConfigBase*) NULL;

// global values
bool g_fAnswerSoundsEnabled;    // Feedback right/wrong answer sounds are enabled
bool g_fTeamCounters;           // Exercises will have two counters to play in teams
bool g_fAutoNewProblem;         // If right answer, don't show solution and generate a new problem


// proxy settings
lmProxySettings m_rProxySettings;



void InitPreferences()
{
    g_pPrefs = wxConfigBase::Get();

    g_pPrefs->Read(_T("/Options/EnableAnswerSounds"), &g_fAnswerSoundsEnabled, true);
    g_pPrefs->Read(_T("/Options/TeamCounters"), &g_fTeamCounters, false);
    g_pPrefs->Read(_T("/Options/AutoNewProblem"), &g_fAutoNewProblem, true);

}

lmProxySettings* GetProxySettings()
{
    bool fUseProxy;
    g_pPrefs->Read(_T("/Internet/UseProxy"), &fUseProxy, false);
    m_rProxySettings.fUseProxy = fUseProxy;

    m_rProxySettings.sProxyHostname = g_pPrefs->Read(_T("/Internet/Hostname"), _T(""));
    long nPort = 0;
    wxString sPort = g_pPrefs->Read(_T("/Internet/PortNumber"), _T(""));
    if (sPort.IsNumber())
        sPort.ToLong(&nPort);
    m_rProxySettings.nProxyPort = nPort;

    bool fAuthentication;
    g_pPrefs->Read(_T("/Internet/ProxyAuthentication"), &fAuthentication, false);
    m_rProxySettings.fRequiresAuth = fAuthentication;
    m_rProxySettings.sProxyUsername = g_pPrefs->Read(_T("/Internet/Username"), _T(""));
    m_rProxySettings.sProxyPassword = g_pPrefs->Read(_T("/Internet/Password"), _T(""));

    return &m_rProxySettings;
}


