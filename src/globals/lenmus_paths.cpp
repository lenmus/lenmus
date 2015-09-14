//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2015 LenMus project
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
//=======================================================================================
Paths::Paths(wxString sBinPath, ApplicationScope& appScope)
    : m_appScope(appScope)
{
    //Receives the full path to the LenMus executable folder (/bin) and
    //extracts the root path
    m_sBin = sBinPath;
    #if (LENMUS_DEBUG_BUILD == 1 || LENMUS_RELEASE_INSTALL == 0)
        m_root.AssignDir(_T(LENMUS_SOURCE_ROOT));

        #if (LENMUS_PLATFORM_WIN32 == 1)
            //Ignore drive letter in Windows
            wxFileName drive;
            drive.GetCwd();
            m_root.SetVolume( drive.GetVolume() );
        #endif

    #else
        m_root.AssignDir(sBinPath);
        m_root.RemoveLastDir();
    #endif
    m_root.Normalize();

    // ------------------------------------------------------------------------------
    //      Linux                       Windows                 Windows (Debug)
    //    Default <prefix> = /usr/local
    //
    // 0. The lenmus program
    // ------------------------------------------------------------------------------
    //      <prefix>                    lenmus                  lm\temp\lenmus
    //          + /bin                      + \bin                  + \z_bin
    //
    // 1. Shared non-modificable files (INSTALL_ROOT):
    // ------------------------------------------------------------------------------
    //      <prefix>/share/lenmus       lenmus                  lm\projects\lenmus\trunk
    //          + /xrc                      + \xrc
    //          + /res                      + \res
    //          + /locale                   + \locale
    //          + /books                    + \books
    //          + /templates                + \templates
    //          + /test-scores              + \test-scores
    //          + /samples
    //
    // 2. Logs & temporal files (ROOT_G2)
    // ------------------------------------------------------------------------------
    //      ~/.config/lenmus/           lenmus                  lm\temp\lenmus
    //          + /logs                      + \logs
    //          + /temp                      + \temp
    //
    // 3. Configuration files (user dependent):
    // ------------------------------------------------------------------------------
    //      ~/.config/lenmus/5.0/       lenmus\bin              lm\temp\lenmus
    //
    // 4. User data: scores, samples, etc.
    // ------------------------------------------------------------------------------
    //      ~/lenmus                    lenmus                  lm\projects\lenmus\trunk
    //          + /scores                   + \scores           (INSTALL_ROOT)
    //          + /5.0/samples              + \5.0\samples
	//



	wxFileName path;
    wxString sVersion = m_appScope.get_version_string();

#if (LENMUS_DEBUG_BUILD == 1 || LENMUS_RELEASE_INSTALL == 0)
    //Debug version or Release version for tests.
    //Use source tree
    wxFileName oInstallHome = m_root;
    wxFileName oLogsHome = m_root;
    oLogsHome.AssignDir(sBinPath);
    wxFileName oConfigHome = m_root;
    oConfigHome.AssignDir(sBinPath);
    wxFileName oDataHome = m_root;

#elif (LENMUS_PLATFORM_WIN32 == 1)
    //Windows Release version, to install
    //Use install root. Binaries in /bin folder
    //Configuration files in /bin, All others in install root

    wxFileName oInstallHome = m_root;
    wxFileName oLogsHome = m_root;
    wxFileName oConfigHome = m_root;
    oConfigHome.AppendDir(_T("bin"));
    wxFileName oDataHome = m_root;


#elif (LENMUS_PLATFORM_UNIX == 1)
    //Linux Release version, to install
    //Use install root. Binaries in /bin folder
    //Configuration and user dependent files in /home

    //get user home folder
    char* homedir = getenv("HOME");
    if (homedir == NULL)
    {
        struct passwd* pw = getpwuid(getuid());
        homedir = pw->pw_dir;
    }
    string sHomedir(homedir);
    wxString sHome = to_wx_string(sHomedir);

    //1. Shared non-modificable files: LENMUS_INSTALL_ROOT (<prefix>/share/lenmus)
    wxFileName oInstallHome;
    oInstallHome.AssignDir( _T(LENMUS_INSTALL_ROOT) );

    //2. Logs & temporal files: ~/.config/lenmus/
    wxFileName oLogsHome;
    oLogsHome.AssignDir( sHome );
    oLogsHome.AppendDir(_T(".config"));
    if (!::wxDirExists( oLogsHome.GetFullPath() ))
	{
		oLogsHome.Mkdir(0777);
        if (!::wxDirExists( oLogsHome.GetFullPath() ))
            LOMSE_LOG_ERROR(to_std_string( wxString::Format(_T("Failed to create '%s'.")
                            , oLogsHome.GetFullPath().c_str() )));
    }
    oLogsHome.AppendDir(_T("lenmus"));
    if (!::wxDirExists( oLogsHome.GetFullPath() ))
	{
		oLogsHome.Mkdir(0777);
        if (!::wxDirExists( oLogsHome.GetFullPath() ))
            LOMSE_LOG_ERROR(to_std_string( wxString::Format(_T("Failed to create '%s'.")
                            , oLogsHome.GetFullPath().c_str() )));
    }

    //3. Configuration files: ~/.config/lenmus/5.x/
    wxFileName oConfigHome;
    oConfigHome.AssignDir( sHome );
    oConfigHome.AppendDir(_T(".config"));
    oConfigHome.AppendDir(_T("lenmus"));
    oConfigHome.AppendDir(sVersion);
    if (!::wxDirExists( oConfigHome.GetFullPath() ))
	{
		oConfigHome.Mkdir(0777);
        if (!::wxDirExists( oConfigHome.GetFullPath() ))
            LOMSE_LOG_ERROR(to_std_string( wxString::Format(_T("Failed to create '%s'.")
                            , oConfigHome.GetFullPath().c_str() )));
    }

    //4. User data: ~/lenmus/
    wxFileName oDataHome;
    oDataHome.AssignDir( sHome );
    oDataHome.AppendDir(_T("lenmus"));
    if (!::wxDirExists( oDataHome.GetFullPath() ))
	{
		oDataHome.Mkdir(0777);
        if (!::wxDirExists( oDataHome.GetFullPath() ))
            LOMSE_LOG_ERROR(to_std_string( wxString::Format(_T("Failed to create '%s'.")
                            , oDataHome.GetFullPath().c_str() )));
    }
#endif

    // Group 1. Software and essentials

    path = oInstallHome;
    path.AppendDir(_T("xrc"));
    m_sXrc = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = oInstallHome;
    path.AppendDir(_T("res"));
    path.AppendDir(_T("icons"));
    m_sImages = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = oInstallHome;
    path.AppendDir(_T("res"));
    path.AppendDir(_T("cursors"));
    m_sCursors = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = oInstallHome;
    path.AppendDir(_T("res"));
    path.AppendDir(_T("sounds"));
    m_sSounds = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = oInstallHome;
    path.AppendDir(_T("locale"));
    m_sLocaleRoot = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = oInstallHome;
    path.AppendDir(_T("templates"));
    m_sTemplates = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = oInstallHome;
    path.AppendDir(_T("test-scores"));
    m_sTestScores = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = oInstallHome;
    path.AppendDir(_T("res"));
    path.AppendDir(_T("fonts"));
    m_sFonts = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);


    // Group 2. Logs and temporal files

    path = oLogsHome;
    path.AppendDir(_T("temp"));
    m_sTemp = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    if (!::wxDirExists( path.GetFullPath() ))
	{
		path.Mkdir(0777);
        if (!::wxDirExists( path.GetFullPath() ))
            LOMSE_LOG_ERROR(to_std_string( wxString::Format(_T("Failed to create '%s'.")
                            , path.GetFullPath().c_str() )));
    }

    path = oLogsHome;
    path.AppendDir(_T("logs"));
    m_sLogs = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    if (!::wxDirExists( path.GetFullPath() ))
	{
		path.Mkdir(0777);
        if (!::wxDirExists( path.GetFullPath() ))
            LOMSE_LOG_ERROR(to_std_string( wxString::Format(_T("Failed to create '%s'.")
                            , path.GetFullPath().c_str() )));
    }


    // Group 3. Configuration files, user dependent

    path = oConfigHome;
    m_sConfig = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);


    // Group 4. User scores and samples

    path = oDataHome;
//TODO: Else code fails: no permision to create folders if they do not exist
#if 1
    m_sScores = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    m_sSamples = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
#else
    path.AppendDir(_T("scores"));
    m_sScores = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    if (!::wxDirExists( path.GetFullPath() ))
	{
		path.Mkdir(0777);
        if (!::wxDirExists( path.GetFullPath() ))
            LOMSE_LOG_ERROR(to_std_string( wxString::Format(_T("Failed to create '%s'.")
                            , path.GetFullPath().c_str() )));
    }
    path.AppendDir(sVersion);
    if (!::wxDirExists( path.GetFullPath() ))
	{
		path.Mkdir(0777);
        if (!::wxDirExists( path.GetFullPath() ))
            LOMSE_LOG_ERROR(to_std_string( wxString::Format(_T("Failed to create '%s'.")
                            , path.GetFullPath().c_str() )));
    }
    path.AppendDir(_T("samples"));
    m_sSamples = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    if (!::wxDirExists( path.GetFullPath() ))
	{
		path.Mkdir(0777);
        if (!::wxDirExists( path.GetFullPath() ))
            LOMSE_LOG_ERROR(to_std_string( wxString::Format(_T("Failed to create '%s'.")
                            , path.GetFullPath().c_str() )));
    }
#endif
}

//---------------------------------------------------------------------------------------
void Paths::create_folders()
{
	//create temp folder if it does not exist. Otherwise the program will
    //fail when the user tries to open an eMusicBook
    if (!::wxDirExists(m_sTemp))
	{
		//bypass for bug in unicode build (GTK) for wxMkdir
        //::wxMkDir(m_sTemp.c_str());
		wxFileName oFN(m_sTemp);
		oFN.Mkdir(0777);
        if (!::wxDirExists(m_sTemp))
            LOMSE_LOG_ERROR(to_std_string( wxString::Format(_T("Failed to create '%s'.")
                            , oFN.GetFullPath().c_str() )));
    }

#if (LENMUS_PLATFORM_UNIX == 1)
    //create folders if they don't exist
    if (!::wxDirExists(m_sLogs))
	{
		wxFileName oFN(m_sLogs);
		oFN.Mkdir(0777);
        if (!::wxDirExists(m_sLogs))
            LOMSE_LOG_ERROR(to_std_string( wxString::Format(_T("Failed to create '%s'.")
                            , oFN.GetFullPath().c_str() )));
    }
    if (!::wxDirExists(m_sConfig))
	{
		wxFileName oFN(m_sConfig);
		oFN.Mkdir(0777);
        if (!::wxDirExists(m_sConfig))
            LOMSE_LOG_ERROR(to_std_string( wxString::Format(_T("Failed to create '%s'.")
                            , oFN.GetFullPath().c_str() )));
    }
#endif

}

//---------------------------------------------------------------------------------------
Paths::~Paths()
{
    //SaveUserPreferences();
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
    wxFileName oLocalePath(m_sLocaleRoot, _T(""), wxPATH_NATIVE);
    oLocalePath.AppendDir(m_sLangCode);
    m_sLocale = oLocalePath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    wxFileName oBooksPath = oLocalePath;
    oBooksPath.AppendDir(_T("books"));
    m_sBooks = oBooksPath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    wxFileName oHelpPath = oLocalePath;
    oHelpPath.AppendDir(_T("help"));
    m_sHelp = oHelpPath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    // When changing language a flag was stored so that at next run the program must
    // clean the temp folder. Check this.
	ClearTempFiles();

}

//---------------------------------------------------------------------------------------
void Paths::log_paths()
{
    LOMSE_LOG_INFO(to_std_string(wxString::Format(_T("LENMUS_INSTALL_ROOT = [%s]"), _T(LENMUS_INSTALL_ROOT) )));

    LOMSE_LOG_INFO(to_std_string(wxString::Format(_T("SrcRoot = %s"), GetSrcRootPath().c_str() )));
    LOMSE_LOG_INFO(to_std_string(wxString::Format(_T("Bin = %s"), GetBinPath().c_str() )));
    LOMSE_LOG_INFO(to_std_string(wxString::Format(_T("Xrc = %s"), GetXrcPath().c_str() )));
    LOMSE_LOG_INFO(to_std_string(wxString::Format(_T("Temp = %s"), GetTemporaryPath().c_str() )));
    LOMSE_LOG_INFO(to_std_string(wxString::Format(_T("Img = %s"), GetImagePath().c_str() )));
    LOMSE_LOG_INFO(to_std_string(wxString::Format(_T("Cursors = %s"), GetCursorsPath().c_str() )));
    LOMSE_LOG_INFO(to_std_string(wxString::Format(_T("Sounds = %s"), GetSoundsPath().c_str() )));
    LOMSE_LOG_INFO(to_std_string(wxString::Format(_T("Locale = %s"), GetLocaleRootPath().c_str() )));
    LOMSE_LOG_INFO(to_std_string(wxString::Format(_T("Scores = %s"), GetScoresPath().c_str() )));
    LOMSE_LOG_INFO(to_std_string(wxString::Format(_T("TestScores = %s"), GetTestScoresPath().c_str() )));
    LOMSE_LOG_INFO(to_std_string(wxString::Format(_T("Samples = %s"), GetSamplesPath().c_str() )));
    LOMSE_LOG_INFO(to_std_string(wxString::Format(_T("Templates = %s"), GetTemplatesPath().c_str() )));
    LOMSE_LOG_INFO(to_std_string(wxString::Format(_T("Config = %s"), GetConfigPath().c_str() )));
    LOMSE_LOG_INFO(to_std_string(wxString::Format(_T("Log = %s"), GetLogPath().c_str() )));
    LOMSE_LOG_INFO(to_std_string(wxString::Format(_T("Fonts = %s\n"), GetFontsPath().c_str() )));

    //DBG: For debugging code for home folder assignment --------------------------
#if (LENMUS_PLATFORM_UNIX == 1)
    char* homedir = getenv("HOME");
    if (homedir == NULL)
    {
        struct passwd* pw = getpwuid(getuid());
        homedir = pw->pw_dir;
    }
    string sHomedir(homedir);
    wxString sHome = to_wx_string(sHomedir);
    LOMSE_LOG_INFO(to_std_string(wxString::Format(_T("homedir = %s"), sHome.wx_str() )));

    wxFileName oLogsHome;
    oLogsHome.AssignDir( sHome );
    LOMSE_LOG_INFO(to_std_string(wxString::Format(_T("initial oLogsHome = %s"),
                   oLogsHome.GetFullPath().c_str() )));
    oLogsHome.AppendDir(_T(".config"));
    oLogsHome.AppendDir(_T("lenmus"));
    LOMSE_LOG_INFO(to_std_string(wxString::Format(_T("final oLogsHome = %s"),
                   oLogsHome.GetFullPath().c_str() )));
    //END_DBG -----------------------------------------------------------------------
#endif
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

////---------------------------------------------------------------------------------------
//void Paths::LoadUserPreferences()
//{
//    // load settings form user congiguration data or default values
//
//	// Only the path for group 4 files can be selected by the user
//    pPrefs->Read(_T("/Paths/Scores"), &m_sScores);
//
//}
//
////! save path settings in user configuration data
//void Paths::SaveUserPreferences()
//{
//    //pPrefs->Write(_T("/Paths/Locale"), m_sLocaleRoot);
//    pPrefs->Write(_T("/Paths/Scores"), m_sScores);
//    //pPrefs->Write(_T("/Paths/Temp"), m_sTemp);
//    //pPrefs->Write(_T("/Paths/Xrc"), m_sXrc);
//    //pPrefs->Write(_T("/Paths/Images"), m_sImages);
//    //pPrefs->Write(_T("/Paths/Sounds"), m_sSounds);
//    //pPrefs->Write(_T("/Paths/Config"), m_sConfig);
//    //pPrefs->Write(_T("/Paths/Logs"), m_sLogs);
//
//    // bin path is not user configurable
//    //pPrefs->Write(_T("/Paths/Bin"), m_sBin);
//
//}

//---------------------------------------------------------------------------------------
void Paths::ClearTempFiles()
{
    // When changing language a flag was stored so that at next run the program must
    // clean the temp folder. Otherwise, as books have the same names in all languages,
    // in Spanish, the new language .hcc and hhk files will not be properly loaded.
    // Here I test this flag and if true, remove all files in temp folder

    bool fClearTemp;
    wxConfigBase* pPrefs = m_appScope.get_preferences();
    pPrefs->Read(_T("/Locale/LanguageChanged"), &fClearTemp, false );
    if (fClearTemp)
    {
        wxString sFile = wxFindFirstFile(m_sTemp);
        while ( !sFile.empty() )
        {
            if (!::wxRemoveFile(sFile))
            {
                wxLogMessage(_T("[Paths::LoadUserPreferences] Error deleting %s"),
                    sFile.wx_str() );
            }
            sFile = wxFindNextFile();
        }
        //reset flag
        fClearTemp = false;
        pPrefs->Write(_T("/Locale/LanguageChanged"), fClearTemp);
    }

}


}   //namespace lenmus
