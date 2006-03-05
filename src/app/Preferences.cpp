// RCS-ID: $Id: Preferences.cpp,v 1.3 2006/02/23 19:17:12 cecilios Exp $
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
//--------------------------------------------------------------------------------
/*! @class lmPreferences
    @ingroup app_gui
    @brief management of global preferences

    All global variables related to user preferences that will be saved for next
    time, are defined in this module, as well as initialization and updating
    functions.

    LenMus uses the wxConfig class to handle preferences.
    On all platforms, preferences are stored in a file (lenmus.ini) in
    the /lenmus directory.

  lmPreferences:
      /
    [Version]
        Version                    - LenMus version that created these prefs

    [MainFrame] = Main window startup size and position
        Width                    
        Height
        Left
        Top
        Maximized                - 0 | 1
        
    [MIDI] = MIDI configuration
        IsSet                    - 0 | 1 (the user has set Midi preferences)
        InDevice                - device to use for input (-1 if none)
        OutDevice                - device to use for output (-1 if none)
        VoiceChannel
        VoiceInstr
        MtrChannel
        MtrInstr
        MtrTone1
        MtrTone2

    //[Locale]
    //    Language                - two-letter language code for translations

    In addition to these keys, certain wxWidgets classes will store values


*/
//------------------------------------------------------------------------------------------
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



void InitPreferences()
{
    g_pPrefs = wxConfigBase::Get();

}


//   wxString vendorName = "LenMus";
//   wxString appName = "LenMus";
//
//   wxTheApp->SetVendorName(vendorName);
//   wxTheApp->SetAppName(appName);
//
//   g_pPrefs = new wxConfig(appName);
//   wxConfigBase::Set(g_pPrefs);
//
//#ifdef __WXMAC__
//#ifndef __UNIX__
//   // This fixes changes in Mac filenames under wxWindows between versions
//   // 0.95 and 0.96 of LenMus.
//   wxString path;
//   bool fix = false;   
//   path = g_pPrefs->Read("/DefaultOpenPath", "");
//   if (path.Length() > 0 && path.Left(1)=="/")
//      fix = true;
//   path = g_pPrefs->Read("/DefaultExportPath", "");
//   if (path.Length() > 0 && path.Left(1)=="/")
//      fix = true;
//   path = g_pPrefs->Read("/Directories/TempDir", "");
//   if (path.Length() > 0 && path.Left(1)=="/")
//      fix = true;
//   if (fix) {
//      g_pPrefs->Write("/DefaultOpenPath", FROMFILENAME(::wxGetCwd()));
//      g_pPrefs->Write("/DefaultExportPath", FROMFILENAME(::wxGetCwd()));
//      g_pPrefs->Write("/Directories/TempDir", "");
//      wxMessageBox(_("Some of your preferences were from an earlier version "
//                     "of LenMus and have been reset."));
//   }
//#endif
//#endif
//
//   g_pPrefs->Write("/Version", (wxString)AUDACITY_VERSION_STRING);
//
//   // BG: Make sure the users prefs are up to date
//   // BG: Otherwise reset some of them to their defaults
//   wxString prefsversion;
//   prefsversion = g_pPrefs->Read("/PrefsVersion", "");
//
//   if(prefsversion.CmpNoCase((wxString)AUDACITY_PREFS_VERSION_STRING))
//   {
//      // BG: Reset the prefs by removing them
//      if(g_pPrefs->Exists("/Keyboard"))
//         g_pPrefs->DeleteGroup("/Keyboard");
//      if(g_pPrefs->Exists("/Locale"))
//         g_pPrefs->DeleteGroup("/Locale");
//      g_pPrefs->Write("/PrefsVersion", (wxString)AUDACITY_PREFS_VERSION_STRING);
//   }
//}
//
//void FinishPreferences()
//{
//   if (g_pPrefs) {
//      wxConfigBase::Set(NULL);
//      delete g_pPrefs;
//      g_pPrefs = NULL;
//   }
//}
//
//int ReadExportFormatPref()
//{
//   return g_pPrefs->Read("/FileFormats/ExportFormat_SF1",
//                       (long int)(SF_FORMAT_WAV | SF_FORMAT_PCM_16));
//}
//
//void WriteExportFormatPref(int format)
//{
//   g_pPrefs->Write("/FileFormats/ExportFormat_SF1", (long int)format);
//}
