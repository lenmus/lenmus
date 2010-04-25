//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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
//-------------------------------------------------------------------------------------

#ifndef __LM_PATH_H__
#define __LM_PATH_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Paths.cpp"
#endif

#include "wx/config.h"
#include "wx/filename.h"
#include <config.h>

class lmPaths
{
public:
    lmPaths(wxString sBinPath);
    ~lmPaths();

    void LoadUserPreferences();
    void SaveUserPreferences();

    //Access to paths
    wxString GetRootPath() { return m_root.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR); }
    wxString GetBinPath() { return m_sBin; }
    wxString GetXrcPath() { return m_sXrc; }
    wxString GetTempPath() { return m_sTemp; }
    wxString GetImagePath() { return m_sImages; }
    wxString GetCursorsPath() { return m_sCursors; }
    wxString GetSoundsPath() { return m_sSounds; }
    wxString GetLocaleRootPath() { return m_sLocaleRoot; }
    wxString GetScoresPath() { return m_sScores; }
    wxString GetTestScoresPath() { return m_sTestScores; }
    wxString GetSamplesPath() { return m_sSamples; }
    wxString GetTemplatesPath() { return m_sTemplates; }
    wxString GetConfigPath() { return m_sConfig; }
    wxString GetLogPath() { return m_sLogs; }

    // paths that depend on the language name
    wxString GetLocalePath() { return m_sLocale; }
    wxString GetHelpPath() { return m_sHelp; }
    wxString GetBooksPath() { return m_sBooks; }

    void SetLanguageCode(wxString sLangCode);

    // Set paths selecteble by user
    void SetScoresPath(wxString sPath) { m_sScores = sPath; }

private:
	void ClearTempFiles();

        //
        // member variables
        //

    //root (instalation path) for LenMus
    wxFileName    m_root;

    //path names
    wxString    m_sLocaleRoot;  //root path for locale (it is m_sLocale without the language subfolder)
    wxString    m_sLocale;      //path containing program translation files ( .mo catalogs)
    wxString    m_sLangCode;    //language code (2 or 5 chars: i.e. "en", "en_US")
    wxString    m_sScores;      //path for scores
    wxString    m_sTestScores;  //path for scores for unit tests
    wxString    m_sSamples;     //path for sample scores
    wxString    m_sTemplates;   //path for templates
    wxString    m_sTemp;        //path for temporary files (help books preprocessed, ...)
    wxString    m_sBooks;       //path for books
    wxString    m_sHelp;        //path for help files
    wxString    m_sBin;         //path for binaries
    wxString    m_sXrc;         //path for .xrc and .xrs resource files
    wxString    m_sImages;      //path for resource images
    wxString    m_sCursors;     //path for resource cursors
    wxString    m_sSounds;      //path for wave sounds
    wxString    m_sConfig;      //path for user configuration file
    wxString    m_sLogs;        //path for logs and dumps

};

extern lmPaths* g_pPaths;

#endif    // __LM_PATH_H__
