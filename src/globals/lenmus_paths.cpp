//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2018 LenMus project
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

#include "lenmus_paths.h"
#include "lenmus_standard_header.h"

#include "lenmus_string.h"
#include "lenmus_injectors.h"

//lomse
#include <lomse_logger.h>
using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/stdpaths.h>

//other
#include <sstream>
using namespace std;

#if (LENMUS_PLATFORM_UNIX == 1)     //for getenv(), getpwuid() and getuid()
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <cstdlib>
#endif


namespace lenmus
{


//=======================================================================================
// Paths implementation
//
// Files to install are divided in groups:
//  1. LenMus program, other binaries, and related files (INSTALL_DIR)
//  2. Non-modificable data, shared among all users on the computer (SHARED_DIR)
//  3. Configuration files, user & version dependent (CONFIG_DIR)
//  4. User scores and samples, user dependent (DATA_DIR)
//  5. Logs and temporal files, user dependent (LOGS_DIR)

//=======================================================================================
Paths::Paths(wxString sBinPath, ApplicationScope& appScope)
    : m_appScope(appScope)
{
    //Save the full path to the LenMus executable
    m_sBin = sBinPath;

    //determine user home folder
#if (LENMUS_PLATFORM_UNIX == 1)
    char* homedir = getenv("HOME");
#elif (LENMUS_PLATFORM_WIN32 == 1)
    char* homedir = getenv("USERPROFILE");
#elif (LENMUS_PLATFORM_MAC == 1)
    char* homedir = getenv("HOME");      //TODO
#endif
    if (homedir == NULL)
    {
        #if (LENMUS_PLATFORM_UNIX == 1 || LENMUS_PLATFORM_MAC == 1)
            struct passwd* pw = getpwuid(getuid());
            homedir = pw->pw_dir;
        #elif (LENMUS_PLATFORM_WIN32 == 1)
            //TODO
        #endif
    }
    string sHomedir(homedir);
    wxString sHome = to_wx_string(sHomedir);


	//determine version string
    wxString sVersion = m_appScope.get_version_string();


    //determine all paths, according to the following specifications:
    //
    // - when the program is a debug build (LENMUS_DEBUG_BUILD == 1) or is not a build
    //   for installation (LENMUS_RELEASE_INSTALL == 0) the root of the source tree
    //   is used. The root of the source tree is provided by configuration
    //   macro LENMUS_SOURCE_ROOT.
    //
    // - Otherwise, there is a root path for each group of files, as described in the
    //   following table:
    //
    // ------------------------------------------------------------------------------
    //    Linux                       			Windows
    //    <prefix> = /usr/local					$INSTDIR = C:\Program Files
    //											$PROFILE = C:\Users\<user>
    //											$DOCUMENTS = C:\Users\<user>
    //
    // 1. The lenmus program (INSTALL_DIR):
    // ------------------------------------------------------------------------------
    //      <prefix>/bin                		$INSTDIR\lenmus-x.x.x\bin
    //          + lenmus                            + lenmus.exe
    //                                              + config-init.txt (NSIS)
    //                                              + dll (libpng16.dll, zlib.dll)
    //
    // 2. Shared non-modificable files (SHARED_ROOT):
    // ------------------------------------------------------------------------------
    //      <prefix>/share/lenmus       		$INSTDIR\lenmus-x.x.x
    //          + /xrc                      		+ \xrc
    //          + /res                      		+ \res
    //          + /locale                   		+ \locale
    //          + /books                    		+ \books
    //          + /templates                		+ \templates
    //          + /test-scores              		+ \test-scores
    //          + /samples							+ \samples
	//          + AUTHORS							+ AUTHORS
	//          + LICENSE							+ LICENSE
	//		    + ...								+ ...
    //
    // 3. User dependent configuration files (CONFIG_DIR):
    // ------------------------------------------------------------------------------
    //      ~/.config/lenmus/x.x.x			$PROFILE\AppData\Local\lenmus\x.x.x
	//			+ lenmus.db						+ lenmus.db
	//			+ lenmus.ini					+ lenmus.ini
    //
    // 4. User data: scores (DATA_DIR)
    // ------------------------------------------------------------------------------
    //      ~/lenmus/scores                 $DOCUMENTS\lenmus\scores
	//
    // 5. Logs & temporal files, per user (LOGS_DIR)
    // ------------------------------------------------------------------------------
    //      ~/lenmus/temp      				$PROFILE\AppData\Local\lenmus\temp
    //                                          + <user>_lenmus_log.txt
	//

#if (LENMUS_DEBUG_BUILD == 1 || LENMUS_RELEASE_INSTALL == 0)
    //Debug version or Release version for tests.
    //Use source tree
    m_installRoot.AssignDir(LENMUS_SOURCE_ROOT);

    #if (LENMUS_PLATFORM_WIN32 == 1)
        //Ignore drive letter in Windows
        wxFileName drive;
        drive.GetCwd();
        m_installRoot.SetVolume( drive.GetVolume() );
    #endif
    m_installRoot.Normalize();

    wxFileName oInstallHome(m_installRoot);
    wxFileName oSharedHome(m_installRoot);
    wxFileName oLogsHome;
    oLogsHome.AssignDir(sBinPath);
    wxFileName oConfigHome;
    oConfigHome.AssignDir(sBinPath);
    wxFileName oDataHome(m_installRoot);

#elif (LENMUS_PLATFORM_UNIX == 1)
    //Linux Release version

    //1. LenMus program, other binaries, and related files (INSTALL_DIR)
    //      /usr/local/bin
    m_installRoot.AssignDir( LENMUS_INSTALL_ROOT );
    m_installRoot.Normalize();
    wxFileName oInstallHome(m_installRoot);

    //2. Non-modificable data, shared among all users on the computer (SHARED_DIR)
    //      /usr/local/share/lenmus/x.x.x/
    wxFileName oSharedHome(m_installRoot);
    oSharedHome.AppendDir("share");
    oSharedHome.AppendDir("lenmus");
    oSharedHome.AppendDir(sVersion);

    //3. Configuration files, user & version dependent (CONFIG_DIR)
    //      ~/.config/lenmus/x.x.x/
    wxFileName oConfigHome;
    oConfigHome.AssignDir( sHome );
    oConfigHome.AppendDir(".config");
    if (!::wxDirExists( oConfigHome.GetFullPath() ))
	{
		oConfigHome.Mkdir(0777);
        if (!::wxDirExists( oConfigHome.GetFullPath() ))
            LOMSE_LOG_ERROR("Failed to create '%s'."
                            , oConfigHome.GetFullPath().ToStdString().c_str() );
    }
    oConfigHome.AppendDir("lenmus");
    if (!::wxDirExists( oConfigHome.GetFullPath() ))
	{
		oConfigHome.Mkdir(0777);
        if (!::wxDirExists( oConfigHome.GetFullPath() ))
            LOMSE_LOG_ERROR("Failed to create '%s'."
                            , oConfigHome.GetFullPath().ToStdString().c_str() );
    }
    oConfigHome.AppendDir(sVersion);
    if (!::wxDirExists( oConfigHome.GetFullPath() ))
	{
		oConfigHome.Mkdir(0777);
        if (!::wxDirExists( oConfigHome.GetFullPath() ))
            LOMSE_LOG_ERROR("Failed to create '%s'."
                            , oConfigHome.GetFullPath().ToStdString().c_str() );
    }

    //4. User scores, user dependent (DATA_DIR)
    //      ~/lenmus/scores/
    wxFileName oDataHome;
    oDataHome.AssignDir( sHome );
    oDataHome.AppendDir("lenmus");
    if (!::wxDirExists( oDataHome.GetFullPath() ))
	{
		oDataHome.Mkdir(0777);
        if (!::wxDirExists( oDataHome.GetFullPath() ))
            LOMSE_LOG_ERROR("Failed to create '%s'."
                            , oDataHome.GetFullPath().ToStdString().c_str() );
    }
    oDataHome.AppendDir("scores");
    if (!::wxDirExists( oDataHome.GetFullPath() ))
	{
		oDataHome.Mkdir(0777);
        if (!::wxDirExists( oDataHome.GetFullPath() ))
            LOMSE_LOG_ERROR("Failed to create '%s'."
                            , oDataHome.GetFullPath().ToStdString().c_str() );
    }

    //5. Logs and temporal files, user dependent (LOGS_DIR)
    //      ~/lenmus/temp/
    wxFileName oLogsHome;
    oLogsHome.AssignDir( sHome );
    oLogsHome.AppendDir("lenmus");
    oLogsHome.AppendDir("temp");
    if (!::wxDirExists( oLogsHome.GetFullPath() ))
	{
		oLogsHome.Mkdir(0777);
        if (!::wxDirExists( oLogsHome.GetFullPath() ))
            LOMSE_LOG_ERROR("Failed to create '%s'."
                            , oLogsHome.GetFullPath().ToStdString().c_str() );
    }

#elif (LENMUS_PLATFORM_WIN32 == 1)
    //Windows Release version
    wxStandardPaths& stdPaths = wxStandardPaths::Get();

    //1. LenMus program, other binaries, and related files (INSTALL_DIR)
    //      "C:/Program Files/lenmus-x.x.x/"
    //TODO: Fix this. Using sBinPath is working only when the user runs lenmus by
    //      mouse click on main menu or in symlinks, becuase tehy will point to the
    //      path in which the lenmus.exe is placed, and then sBinPath will point to
    //      that path. This should be fixed by using GetModuleFileName(). See:
    //      https://stackoverflow.com/questions/1528298/get-path-of-executable
    m_installRoot.AssignDir(sBinPath);
    m_installRoot.RemoveLastDir();
    m_installRoot.Normalize();
    wxFileName oInstallHome(m_installRoot);

    //2. Non-modificable data, shared among all users on the computer (SHARED_DIR)
    //      "C:/Program Files/lenmus-x.x.x/"
    wxFileName oSharedHome(m_installRoot);

    //3. Configuration files, user & version dependent (CONFIG_DIR)
    //      "C:\Users\<user>\AppData\Local\lenmus\x.x.x\"
    wxFileName oConfigHome;
    oConfigHome.AssignDir( stdPaths.GetDocumentsDir() );  // C:/Users/<user>/Documents/
    oConfigHome.RemoveLastDir();                          // C:/Users/<user>/
    oConfigHome.AppendDir("AppData");                     // C:/Users/<user>/AppData/
    oConfigHome.AppendDir("Local");                       // C:/Users/<user>/AppData/Local/
    oConfigHome.AppendDir("lenmus");                      // C:/Users/<user>/AppData/Local/lenmus/
    if (!::wxDirExists( oConfigHome.GetFullPath() ))
	{
		oConfigHome.Mkdir(0777);
        if (!::wxDirExists( oConfigHome.GetFullPath() ))
            LOMSE_LOG_ERROR("Failed to create '%s'."
                            , oConfigHome.GetFullPath().ToStdString().c_str() );
    }
    oConfigHome.AppendDir(sVersion);                                // C:/Users/<user>/AppData/Local/lenmus/x.x.x/
    if (!::wxDirExists( oConfigHome.GetFullPath() ))
	{
		oConfigHome.Mkdir(0777);
        if (!::wxDirExists( oConfigHome.GetFullPath() ))
            LOMSE_LOG_ERROR("Failed to create '%s'."
                            , oConfigHome.GetFullPath().ToStdString().c_str() );
    }

    //4. User scores, user dependent (DATA_DIR)
    //      "C:\Users\<user>\lenmus\scores"
    wxFileName oDataHome;
    oDataHome.AssignDir( stdPaths.GetDocumentsDir() );  // C:/Users/<user>/Documents/
    oDataHome.RemoveLastDir();                          // C:/Users/<user>/
    oDataHome.AppendDir("lenmus");                      // C:/Users/<user>/lenmus/
    if (!::wxDirExists( oDataHome.GetFullPath() ))
	{
		oDataHome.Mkdir(0777);
        if (!::wxDirExists( oDataHome.GetFullPath() ))
            LOMSE_LOG_ERROR("Failed to create '%s'."
                            , oDataHome.GetFullPath().ToStdString().c_str() );
    }
    wxFileName oLogsHome(oDataHome);
    oDataHome.AppendDir("scores");          // C:/Users/<user>/lenmus/scores/
    if (!::wxDirExists( oDataHome.GetFullPath() ))
	{
		oDataHome.Mkdir(0777);
        if (!::wxDirExists( oDataHome.GetFullPath() ))
            LOMSE_LOG_ERROR("Failed to create '%s'."
                            , oDataHome.GetFullPath().ToStdString().c_str() );
    }

    //5. Logs and temporal files, user dependent (LOGS_DIR)
    //      "C:\Users\<user>\lenmus\"
    oLogsHome.AppendDir("temp");            // C:/Users/<user>/lenmus/temp/
    if (!::wxDirExists( oLogsHome.GetFullPath() ))
	{
		oLogsHome.Mkdir(0777);
        if (!::wxDirExists( oLogsHome.GetFullPath() ))
            LOMSE_LOG_ERROR("Failed to create '%s'."
                            , oLogsHome.GetFullPath().ToStdString().c_str() );
    }

#endif
    LOMSE_LOG_INFO("Install root = %s", oInstallHome.GetFullPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("Shared root = %s", oSharedHome.GetFullPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("Config root = %s", oConfigHome.GetFullPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("sHome = %s", sHome.ToStdString().c_str() );
    LOMSE_LOG_INFO("Data root = %s", oDataHome.GetFullPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("Logs root = %s\n", oLogsHome.GetFullPath().ToStdString().c_str() );

    //determine paths for subfolders
    wxFileName path;

    // Group 2 subfolders. Shared non-modificable files

    path = oSharedHome;
    path.AppendDir("xrc");
    m_sXrc = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = oSharedHome;
    path.AppendDir("res");
    path.AppendDir("icons");
    m_sImages = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = oSharedHome;
    path.AppendDir("res");
    path.AppendDir("cursors");
    m_sCursors = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = oSharedHome;
    path.AppendDir("res");
    path.AppendDir("sounds");
    m_sSounds = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = oSharedHome;
    path.AppendDir("locale");
    m_sLocaleRoot = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = oSharedHome;
    path.AppendDir("templates");
    m_sTemplates = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = oSharedHome;
    path.AppendDir("test-scores");
    m_sTestScores = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = oSharedHome;
    path.AppendDir("res");
    path.AppendDir("fonts");
    m_sFonts = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = oSharedHome;
    path.AppendDir("samples");
    m_sSamples = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);


    // Group 3 subfolders. Configuration files, user & version dependent

    path = oConfigHome;
    m_sConfig = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);


    // Group 4 subfolders. User data: scores

    path = oDataHome;
    m_sScores = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);


    // Group 5 subfolders. Logs and temporal files

    path = oLogsHome;
    m_sTemp = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    m_sLogs = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

}

//---------------------------------------------------------------------------------------
Paths::~Paths()
{
}

//---------------------------------------------------------------------------------------
void Paths::SetLanguageCode(wxString sLangCode)
{
    //
    // Lang code has changed. It is necessary to rebuild paths depending on language
    //
    // IMPORTANT: When this method is invoked wxLocale object is not
    //            yet initialized. DO NOT USE LANGUAGE DEPENDENT STRINGS HERE
    //
    m_sLangCode = sLangCode;
    wxFileName oLocalePath(m_sLocaleRoot, "", wxPATH_NATIVE);
    oLocalePath.AppendDir(m_sLangCode);
    m_sLocale = oLocalePath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    wxFileName oBooksPath = oLocalePath;
    oBooksPath.AppendDir("books");
    m_sBooks = oBooksPath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    wxFileName oHelpPath = oLocalePath;
    oHelpPath.AppendDir("help");
    m_sHelp = oHelpPath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    // When changing language a flag was stored so that at next run the program must
    // clean the temp folder. Check this.
	ClearTempFiles();

}

//---------------------------------------------------------------------------------------
void Paths::log_paths()
{
    LOMSE_LOG_INFO("LENMUS_INSTALL_ROOT = [%s]", LENMUS_INSTALL_ROOT);

    LOMSE_LOG_INFO("SrcRoot = %s", GetSrcRootPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("Bin = %s", GetBinPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("Xrc = %s", GetXrcPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("Temp = %s", GetTemporaryPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("Img = %s", GetImagePath().ToStdString().c_str() );
    LOMSE_LOG_INFO("Cursors = %s", GetCursorsPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("Sounds = %s", GetSoundsPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("Locale = %s", GetLocaleRootPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("Scores = %s", GetScoresPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("TestScores = %s", GetTestScoresPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("Samples = %s", GetSamplesPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("Templates = %s", GetTemplatesPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("Config = %s", GetConfigPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("Log = %s", GetLogPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("Fonts = %s\n", GetFontsPath().ToStdString().c_str() );

    wxLogMessage("Install root = %s", GetBinPath());
    wxLogMessage("Shared root = %s", GetFontsPath());
    wxLogMessage("Config root = %s", GetConfigPath());
    wxLogMessage("Data root = %s", GetScoresPath());
    wxLogMessage("Logs root = %s", GetLogPath());
}

//---------------------------------------------------------------------------------------
string Paths::dump_paths()
{
    stringstream s;
    s << "SrcRoot = " << to_std_string(GetSrcRootPath()) << endl;
    s << "Bin = " << to_std_string(GetBinPath()) << endl;
    s << "Xrc = " << to_std_string(GetXrcPath()) << endl;
    s << "Temp = " << to_std_string(GetTemporaryPath()) << endl;
    s << "Img = " << to_std_string(GetImagePath()) << endl;
    s << "Cursors = " << to_std_string(GetCursorsPath()) << endl;
    s << "Sounds = " << to_std_string(GetSoundsPath()) << endl;
    s << "Locale = " << to_std_string(GetLocaleRootPath()) << endl;
    s << "Scores = " << to_std_string(GetScoresPath()) << endl;
    s << "TestScores = " << to_std_string(GetTestScoresPath()) << endl;
    s << "Samples = " << to_std_string(GetSamplesPath()) << endl;
    s << "Templates = " << to_std_string(GetTemplatesPath()) << endl;
    s << "Config = " << to_std_string(GetConfigPath()) << endl;
    s << "Log = " << to_std_string(GetLogPath()) << endl;
    s << "Fonts = " << to_std_string(GetFontsPath()) << endl;
    return s.str();
}

//---------------------------------------------------------------------------------------
void Paths::ClearTempFiles()
{
    // When changing language a flag was stored so that at next run the program must
    // clean the temp folder. Otherwise, as books have the same names in all languages,
    // in Spanish, the new language .hcc and hhk files will not be properly loaded.
    // Here I test this flag and if true, remove all files in temp folder

    bool fClearTemp;
    wxConfigBase* pPrefs = m_appScope.get_preferences();
    pPrefs->Read("/Locale/LanguageChanged", &fClearTemp, false );
    if (fClearTemp)
    {
        wxString sFile = wxFindFirstFile(m_sTemp);
        while ( !sFile.empty() )
        {
            if (!::wxRemoveFile(sFile))
            {
                wxLogMessage("[Paths::LoadUserPreferences] Error deleting %s",
                    sFile.wx_str() );
            }
            sFile = wxFindNextFile();
        }
        //reset flag
        fClearTemp = false;
        pPrefs->Write("/Locale/LanguageChanged", fClearTemp);
    }

}


}   //namespace lenmus
