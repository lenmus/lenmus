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
/*! @file Preferences.cpp
    @brief Implementation file for class lmPreferences
    @ingroup app_gui
*/
#ifdef __GNUG__
#pragma implementation "TheApp.h"
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



// proxy settings
wxProxySettings m_oProxySettings;



void InitPreferences()
{
    g_pPrefs = wxConfigBase::Get();

    g_pPrefs->Read(_T("/Options/EnableAnswerSounds"), &g_fAnswerSoundsEnabled, true);


}

wxProxySettings* GetProxySettings()
{
    bool fUseProxy;
    g_pPrefs->Read(_T("/Internet/UseProxy"), &fUseProxy, false);
    m_oProxySettings.m_bUseProxy = fUseProxy;

    m_oProxySettings.m_strProxyHostname = g_pPrefs->Read(_T("/Internet/Hostname"), _T(""));
    long nPort = 0;
    wxString sPort = g_pPrefs->Read(_T("/Internet/PortNumber"), _T(""));
    if (sPort.IsNumber())
        sPort.ToLong(&nPort);
    m_oProxySettings.m_nProxyPort = nPort;

    bool fAuthentication;
    g_pPrefs->Read(_T("/Internet/ProxyAuthentication"), &fAuthentication, false);
    m_oProxySettings.m_bRequiresAuth = fAuthentication;
    m_oProxySettings.m_strProxyUsername = g_pPrefs->Read(_T("/Internet/Username"), _T(""));
    m_oProxySettings.m_strProxyPassword = g_pPrefs->Read(_T("/Internet/Password"), _T(""));

    return &m_oProxySettings;
}


