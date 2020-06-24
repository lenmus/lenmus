//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2020 LenMus project
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

#if (LENMUS_PLATFORM_UNIX == 1 || LENMUS_PLATFORM_MAC == 1)     //for getenv(), getpwuid() and getuid()
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
//To install files LenMus uses several prefixes:
//    1. BIN_ROOT: LenMus program, other binaries, and related files.
//    2. SHARED_DIR: Non-modificable data, shared among all users on the computer.
//    3. CONFIG_DIR: Configuration files, user & version dependent.
//    4. DATA_DIR: User scores, user dependent.
//    5. LOGS_DIR: Logs and temporal files, user dependent.
//    6. SOUND_FONT: Default soundfont.
//
//    Paths for programming and debugging
//    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//    When the program is run from the IDE the source tree is used. The root of the
//    source tree is provided by configuration macro LENMUS_SOURCE_ROOT and all other
//    roots are derived from it.
//
//    Macro LENMUS_SOUNDFONT_PATH is ignored and, instead, the value of
//    ${LENMUS_SOURCE_ROOT}/res/sounds/fluidR3_GM.sf2  is used.
//
//    LENMUS_TEST_SCORES_PATH is used for accesing the unit test scores.
//
//
//    Paths for installation
//    ~~~~~~~~~~~~~~~~~~~~~~~~~
//    There is a root path for each group of files, as described in following table:
//
//    ------------------------------------------------------------------------------
//       Linux                       		  Windows
//       <prefix> = /usr/local                NSIS automatically provides variables:
//                                                $INSTDIR = C:\Program Files
//                                                $PROFILE = C:\Users\<user>
//                                                $DOCUMENTS = $PROFILE\MyDocuments
//                                                $LOCALAPPDATA = C:\Users\<user>\AppData\Local
//                                            with the drive letter correctly set.
//                                            Therefore it is not necessary to do anything
//                                            in LenMus program for fixing the drive letter.
//
//    1. The lenmus program (BIN_DIR):
//    ------------------------------------------------------------------------------
//        Linux:      <prefix>/bin
//        Windows:    $INSTDIR\lenmus-x.x.x\bin
//
//    2. Shared non-modificable files (SHARED_DIR):
//    ------------------------------------------------------------------------------
//        Linux:      <prefix>/share/lenmus
//        Windows:    $INSTDIR\lenmus-x.x.x
//                    (e.g.: C:\Program Files\lenmus-x.x.x)
//
//        <SHARED_DIR>
//            + /xrc
//            + /res
//            + /locale
//            + /books
//            + /templates
//            + /test-scores
//            + /samples
//            + AUTHORS
//            + LICENSE
//            + ...
//
//    3. User dependent configuration files (CONFIG_DIR):
//    ------------------------------------------------------------------------------
//        Linux:      ~/.config/lenmus/x.x.x/
//        Windows:    $PROFILE\AppData\Local\lenmus\x.x.x
//                    (e.g.: C:\Users\<user>\\AppData\Local\lenmus\x.x.x\)
//
//        <CONFIG_DIR>
//            + lenmus.db
//            + lenmus.dini
//
//    4. User data: scores, samples, etc.(DATA_DIR)
//    ------------------------------------------------------------------------------
//        Linux:      ~/lenmus
//        Windows:    $PROFILE\lenmus
//                    (e.g.: C:\Users\<user>\lenmus)
//
//        <DATA_DIR>
//            + /scores
//
//    5. Logs & temporal files, per user (LOGS_DIR)
//    ------------------------------------------------------------------------------
//        Linux:      ~/lenmus/temp
//        Windows:    $PROFILE\lenmus\temp
//                    (e.g.: C:\Users\<user>\lenmus\temp)
//
//        <LOGS_DIR>
//
//    6. Default soundfont (SOUND_FONT)
//    ------------------------------------------------------------------------------
//        Linux:      LENMUS_SOUNDFONT_PATH when != "", or
//                    <SHARED_DIR>/res/sounds, when LENMUS_SOUNDFONT_PATH == ""
//
//        Windows:    <SHARED_DIR>/res/sounds
//                    (e.g.: C:\Program Files\lenmus-x.x.x\res\sounds)
//
//    7. Other resources
//    ------------------------------------------------------------------------------
//    Music font Bravura.otf
//        Linux:
//            The program uses FontConfig. If not found, fallbacks to LENMUS_SOUNDFONT_PATH.
//            The value of this macro is:
//            - For builds from IDE:  <SHARED_DIR>/res/fonts
//            - For builds using cmake:
//                cmake -DLENMUS_INSTALL_BRAVURA_FONT:BOOL=OFF
//                    path to system Btavura.otf font
//                cmake -DLENMUS_INSTALL_BRAVURA_FONT:BOOL=ON
//                    <SHARED_DIR>/res/fonts
//        Windows:
//            <SHARED_DIR>/res/fonts
//            (e.g.: C:\Program Files\lenmus-x.x.x\res\fonts)
//
//
//=======================================================================================
Paths::Paths(wxString sBinPath, ApplicationScope& appScope)
    : m_appScope(appScope)
{
    m_sBin = sBinPath;      //save the full path to the LenMus executable
    initialize();
}

//---------------------------------------------------------------------------------------
Paths::~Paths()
{
}

//---------------------------------------------------------------------------------------
void Paths::initialize()
{
    wxString sHome = get_user_home_folder();
    wxString sVersion = m_appScope.get_version_string();
    determine_prefix();

    //Step 1: Determine the base paths
    //----------------------------------------------------------------
    //  SHARED_DIR (oSharedHome). For shared non-modificable files
    //  CONFIG_DIR (oConfigHome). For user dependent configuration files
    //  DATA_DIR (oDataHome). For user data: e.g.: scores
    //  LOGS_DIR (oLogsHome). For logs & temporal files, per user
    //  SOUND_FONT (oSoundFonts). For FluidR3_GM.sf2 default soundfont

#if (LENMUS_DEBUG_BUILD == 1 || LENMUS_RELEASE_INSTALL == 0)
    //Base paths for local builds,  for programming and debugging
    //------------------------------------------------------------

    wxFileName oSharedHome(m_sPrefix);
    wxFileName oLogsHome(sHome);
    wxFileName oConfigHome(m_sBin);
    wxFileName oDataHome(m_sPrefix);
    wxFileName oSoundFonts(m_sPrefix);
    oSoundFonts.AppendDir("res");
    oSoundFonts.AppendDir("sounds");
    


#elif (LENMUS_PLATFORM_UNIX == 1 || LENMUS_PLATFORM_MAC == 1)
    //Base paths for Linux Release version
    //---------------------------------------

    //2. Non-modificable data, shared among all users on the computer (SHARED_DIR)
    //      /usr/local/share/lenmus/x.x.x/ (unless on a Mac).
#if (LENMUS_PLATFORM_MAC == 1)
    //On a Mac, if bundling LenMus in an app, SHARED_DIR should be
    // <app-location>/lenmus.app/Contents/Resources/
    //where <app-location> is the directory the app is installed in (usually "/Applications", but it
    // could be anywhere.)
    // Since the binary is installed in <app-location>/lenmus.app/Contents/MacOS, we can assume
    // that m_sPrefix contains "<app-location>/lenmus.app/Contents" at this point.
    wxFileName oSharedHome(m_sPrefix);
    oSharedHome.AppendDir("Resources");
#else
    // this is for Linux:
    wxFileName oSharedHome(m_sPrefix);
    oSharedHome.AppendDir("share");
    oSharedHome.AppendDir("lenmus");
    oSharedHome.AppendDir(sVersion);
#endif
    
    //Remaining configuration should be the same for Linux or Mac.

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

    //6. Other files from system
    wxFileName oSoundFonts;
    if (strlen(LENMUS_SOUNDFONT_PATH) != 0)
        oSoundFonts = wxFileName( LENMUS_SOUNDFONT_PATH , "");
    else
    {
        oSoundFonts = oSharedHome;
        oSoundFonts.AppendDir("res");
        oSoundFonts.AppendDir("sounds");
    }
    oSoundFonts.Normalize();


#elif (LENMUS_PLATFORM_WIN32 == 1)
    //Base paths for Windows Release version
    //-----------------------------------------

    wxStandardPaths& stdPaths = wxStandardPaths::Get();

    //2. Non-modificable data, shared among all users on the computer (SHARED_DIR)
    //      "C:/Program Files/lenmus-x.x.x/"
    wxFileName oSharedHome(m_sPrefix);

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
    wxFileName oLogsHome(oDataHome);    // C:/Users/<user>/lenmus

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

    //6. Other files from system
    wxFileName oSoundFonts = oSharedHome;
    oSoundFonts.AppendDir("res");
    oSoundFonts.AppendDir("sounds");
    oSoundFonts.Normalize();

#endif

    LOMSE_LOG_INFO("Prefix = %s", m_sPrefix.ToStdString().c_str() );
    LOMSE_LOG_INFO("Shared root = %s", oSharedHome.GetFullPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("Config root = %s", oConfigHome.GetFullPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("sHome = %s", sHome.ToStdString().c_str() );
    LOMSE_LOG_INFO("Data root = %s", oDataHome.GetFullPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("Logs root = %s", oLogsHome.GetFullPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("SoundFonts = %s\n", oSoundFonts.GetFullPath().ToStdString().c_str() );


    //Step 2: Derive paths from base roots
    //----------------------------------------------------------------

    wxFileName path;

    //SHARED_DIR subfolders. Shared non-modificable files

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

    path = oSoundFonts;
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
    path.AppendDir("res");
    path.AppendDir("fonts");
    m_sFonts = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = oSharedHome;
    path.AppendDir("samples");
    m_sSamples = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);


    //CONFIG_DIR subfolders. Configuration files, user & version dependent
    path = oConfigHome;
    m_sConfig = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);


    //DATA_DIR subfolders. User data: scores
    path = oDataHome;
    path.AppendDir("scores");
    m_sScores = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);


    //LOGS_DIR subfolders. Logs and temporal files
    path = oLogsHome;
    m_sTemp = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    m_sLogs = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);


    //SOUND_FONT. Default sountfont
    path = oSoundFonts;
    m_sSoundFonts = oSoundFonts.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);


    //Paths for tests and debug. Only valid in local debug builds
    path = wxFileName(LENMUS_SOURCE_ROOT);
    path.Normalize();
    #if (LENMUS_PLATFORM_WIN32 == 1)
        //for replacing drive letter in Windows
        wxFileName drive(m_sBin);
        path.SetVolume( drive.GetVolume() );
    #endif
    m_sSourceRoot = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = wxFileName(LENMUS_TEST_SCORES_PATH);
    path.Normalize();
    #if (LENMUS_PLATFORM_WIN32 == 1)
        path.SetVolume( drive.GetVolume() );
    #endif
    m_sTestScores = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

}

//---------------------------------------------------------------------------------------
wxString Paths::get_user_home_folder()
{
#if (LENMUS_PLATFORM_UNIX == 1)
    char* homedir = getenv("HOME");
#elif (LENMUS_PLATFORM_WIN32 == 1)
    char* homedir = getenv("USERPROFILE");
#elif (LENMUS_PLATFORM_MAC == 1)
    char* homedir = getenv("HOME");
#endif
    if (homedir == nullptr)
    {
        #if (LENMUS_PLATFORM_UNIX == 1 || LENMUS_PLATFORM_MAC == 1)
            struct passwd* pw = getpwuid(getuid());
            homedir = pw->pw_dir;
        #elif (LENMUS_PLATFORM_WIN32 == 1)
            //TODO
        #endif
    }
    return to_wx_string(homedir);
}

//---------------------------------------------------------------------------------------
void Paths::determine_prefix()
{
    wxFileName oPrefix;
#if (LENMUS_DEBUG_BUILD == 1 || LENMUS_RELEASE_INSTALL == 0)
    //Debug version or Release version for tests. Use LENMUS_SOURCE_ROOT for <prefix>
    oPrefix.AssignDir(LENMUS_SOURCE_ROOT);

    #if (LENMUS_PLATFORM_WIN32 == 1)
        //Ignore drive letter in Windows
        wxFileName drive(m_sBin);
        oPrefix.SetVolume( drive.GetVolume() );
    #endif
#else
    oPrefix.AssignDir(m_sBin);
    oPrefix.RemoveLastDir();    // bin
#endif
    oPrefix.Normalize();
    m_sPrefix = oPrefix.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
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
    LOMSE_LOG_INFO("Bin = %s", GetBinPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("Xrc = %s", GetXrcPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("Temp = %s", GetTemporaryPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("Img = %s", GetImagePath().ToStdString().c_str() );
    LOMSE_LOG_INFO("Cursors = %s", GetCursorsPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("Sounds = %s", GetSoundsPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("Locale = %s", GetLocaleRootPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("Scores = %s", GetScoresPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("Samples = %s", GetSamplesPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("Templates = %s", GetTemplatesPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("Config = %s", GetConfigPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("Log = %s", GetLogPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("Fonts = %s", GetFontsPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("SoundFonts = %s", GetSoundFontsPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("SourceRoot = %s", GetSrcRootPath().ToStdString().c_str() );
    LOMSE_LOG_INFO("TestScores = %s\n", GetTestScoresPath().ToStdString().c_str() );

    wxLogMessage("Binary root = %s", GetBinPath());
    wxLogMessage("Shared root = %s", GetFontsPath());
    wxLogMessage("Config root = %s", GetConfigPath());
    wxLogMessage("Data root = %s", GetScoresPath());
    wxLogMessage("Logs root = %s", GetLogPath());
    wxLogMessage("Sound fonts = %s", GetSoundFontsPath());
    wxLogMessage("Source root = %s", GetSrcRootPath());
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
