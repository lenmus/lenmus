//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2014 LenMus project
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

#ifndef __LENMUS_PATHS_H__
#define __LENMUS_PATHS_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_injectors.h"

//wxWidgets
#include <wx/config.h>
#include <wx/filename.h>

namespace lenmus
{

class Paths
{
private:
    ApplicationScope& m_appScope;

    //path names

    //BIN_DIR (relative). The lenmus program: <prefix>/bin
    wxString    m_sBin;         //path for binaries

    //PREFIX_DIR. The root of all relative paths: <prefix>
    wxString    m_sPrefix;      //the <prefix> path

    //SHARED_DIR (relative). Shared non-modificable files
    wxString    m_sLocaleRoot;  //root path for locale folders. <shared>/locale
    wxString    m_sSamples;     //path for sample scores. <shared>/samples
    wxString    m_sCursors;     //path for resource cursors. <shared>/res/cursors
    wxString    m_sFonts;       //path for resource fonts. <shared>/res/fonts
    wxString    m_sImages;      //path for resource images. <shared>/res/icons
    wxString    m_sSounds;      //path for wave sounds. <shared>/res/sounds
    wxString    m_sTemplates;   //path for templates. <shared>/templates
    wxString    m_sXrc;         //path for .xrc and .xrs resource files. <shared>/xrc

    //SHARED_DIR. Paths depending on language
    wxString    m_sLangCode;    //language code (2 or 5 chars: i.e. "en", "en_US")
    wxString    m_sLocale;      //path containing program translation files ( .mo catalogs)
    wxString    m_sBooks;       //path for books
    wxString    m_sHelp;        //path for help files

    //CONFIG_DIR (absolute). User dependent configuration files
    wxString    m_sConfig;      //path for user configuration file

    //DATA_DIR (absolute). User data: scores
    wxString    m_sScores;      //path for user scores

    //LOGS_DIR (absolute). Logs & temporal files, per user
    wxString    m_sTemp;        //path for temporary files
    wxString    m_sLogs;        //path for logs and dumps

    //SOUND_FONT (absolute / relative)
    wxString    m_sSoundFonts;  //path for default sound font FluidR3_GM.sf2

    //Paths for test and debug. Only valid in local debug builds
    wxString    m_sSourceRoot;  //path to source tree. <SOURCE_ROOT>
    wxString    m_sTestScores;  //path for scores for unit tests. <SOURCE_ROOT>/test-scores

public:
    Paths(wxString sBinPath, ApplicationScope& appScope);
    ~Paths();

    //actions
    void log_paths();
    string dump_paths();

    //Access to paths
    wxString GetSrcRootPath() { return m_sSourceRoot; } //Only valid in local debug builds
    wxString GetBinPath() { return m_sBin; }
    wxString GetXrcPath() { return m_sXrc; }
    wxString GetTemporaryPath() { return m_sTemp; }
    wxString GetImagePath() { return m_sImages; }
    wxString GetCursorsPath() { return m_sCursors; }
    wxString GetSoundsPath() { return m_sSounds; }
    wxString GetSoundFontsPath() { return m_sSoundFonts; }
    wxString GetLocaleRootPath() { return m_sLocaleRoot; }
    wxString GetScoresPath() { return m_sScores; }
    wxString GetTestScoresPath() { return m_sTestScores; }
    wxString GetSamplesPath() { return m_sSamples; }
    wxString GetTemplatesPath() { return m_sTemplates; }
    wxString GetConfigPath() { return m_sConfig; }
    wxString GetLogPath() { return m_sLogs; }
    wxString GetFontsPath() { return m_sFonts; }

    // paths that depend on the language name
    wxString GetLocalePath() { return m_sLocale; }
    wxString GetHelpPath() { return m_sHelp; }
    wxString GetBooksPath() { return m_sBooks; }

    void SetLanguageCode(wxString sLangCode);

private:
	void ClearTempFiles();
	void initialize();
	wxString get_user_home_folder();
    void determine_prefix();

};


}   //namespace lenmus

#endif    // __LENMUS_PATHS_H__
