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
/*! @file Preferences.h
    @brief Header file for class lmPreferences
    @ingroup app_gui
*/
#ifndef __PREFERENCES_H__
#define __PREFERENCES_H__

#include <wx/config.h>


// the config object
extern wxConfigBase *g_pPrefs;

typedef struct lmProxySettingsStruct {
	wxString sProxyHostname;
	wxString sProxyUsername;
	wxString sProxyPassword;
	bool fUseProxy;
    bool fRequiresAuth;
	int nProxyPort;
} lmProxySettings;

//Global functions
extern void InitPreferences();
extern lmProxySettings* GetProxySettings();





#endif    // __PREFERENCES_H__

