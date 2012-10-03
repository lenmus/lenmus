//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2012 LenMus project
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

#include <wx/wxprec.h>
#include <wx/wx.h>

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
    #if (LENMUS_DEBUG_BUILD == 1)
        m_root.AssignDir(_T(LENMUS_DBG_ROOT_PATH));
    #else
        m_root.AssignDir(sBinPath);
        m_root.RemoveLastDir();
    #endif
    m_root.Normalize();

    // ------------------------------------------------------------------------------
    //      Linux                       Windows
    //    Default <prefix> = /usr/local
    //
    // 0. The lenmus program
    // ------------------------------------------------------------------------------
    //      <prefix>                    lenmus
    //          + /bin                      + \bin
    //
    // 1. Shared non-modificable files (INSTALL_HOME):
    // ------------------------------------------------------------------------------
    //      <prefix>/share/lenmus       lenmus
    //          + /xrc                      + \xrc
    //          + /res                      + \res
    //          + /locale                   + \locale
    //          + /books                    + \books
    //          + /templates                + \templates
    //          + /test-scores              + \test-scores
    //
    // 2. Logs & temporal files (ROOT_G2)
    // ------------------------------------------------------------------------------
    //      ~/.config/lenmus/           lenmus
    //          + /logs                      + \logs
    //          + /temp                      + \temp
    //
    // 3. Configuration files (user dependent): (ROOT_G3)
    // ------------------------------------------------------------------------------
    //      ~/.config/lenmus/5.0/       lenmus\bin
    //
    // 4. User data: scores, samples, etc. (ROOT_G4)
    // ------------------------------------------------------------------------------
    //      ~/lenmus                    lenmus
    //          + /scores                   + \scores
    //          + /5.0/samples              + \5.0\samples
	//



	wxFileName path;
    wxString sVersion = m_appScope.get_version_string();

#if (LENMUS_PLATFORM_WIN32 == 1 || LENMUS_DEBUG_BUILD == 1 || LENMUS_IS_TEST_INSTALL == 1)
    wxFileName oInstallHome = m_root;
    wxFileName oLogsHome = m_root;
    wxFileName oConfigHome = m_root;
    wxFileName oDataHome = m_root;
    #if (LENMUS_PLATFORM_WIN32 == 1)
        #if (LENMUS_DEBUG_BUILD == 1)
            oConfigHome.AssignDir(sBinPath);
            oLogsHome.AssignDir(sBinPath);
        #else
            oConfigHome.AppendDir(_T("bin"));
        #endif
    #endif

#elif (LENMUS_PLATFORM_UNIX == 1)
    //get user home folder
    char* homedir = getenv("HOME");
    if (homedir == NULL)
    {
        struct passwd* pw = getpwuid(getuid());
        homedir = pw->pw_dir;
    }
    string sHomedir(homedir);
    wxString sHome = to_wx_string(sHomedir);

    //1. Shared non-modificable files: LENMUS_INSTALL_HOME (<prefix>/share/lenmus)
    wxFileName oInstallHome;
    oInstallHome.AssignDir( _T(LENMUS_INSTALL_HOME) );

    //2. Logs & temporal files: ~/.config/lenmus/
    wxFileName oLogsHome;
    oLogsHome.AssignDir( sHome );
    oLogsHome.AppendDir(_T(".config"));
    if (!::wxDirExists( oLogsHome.GetFullPath() ))
	{
		oLogsHome.Mkdir(0777);
        if (!::wxDirExists( oLogsHome.GetFullPath() ))
            wxLogMessage(_T("[Paths::Paths] Failed to create '%s'."),
                         oLogsHome.GetFullPath().c_str() );
    }
    oLogsHome.AppendDir(_T("lenmus"));
    if (!::wxDirExists( oLogsHome.GetFullPath() ))
	{
		oLogsHome.Mkdir(0777);
        if (!::wxDirExists( oLogsHome.GetFullPath() ))
            wxLogMessage(_T("[Paths::Paths] Failed to create '%s'."),
                         oLogsHome.GetFullPath().c_str() );
    }

    //3. Configuration files: ~/.config/lenmus/5.0/
    wxFileName oConfigHome;
    oConfigHome.AssignDir( sHome );
    oConfigHome.AppendDir(_T(".config"));
    oConfigHome.AppendDir(_T("lenmus"));
    oConfigHome.AppendDir(sVersion);
    if (!::wxDirExists( oConfigHome.GetFullPath() ))
	{
		oConfigHome.Mkdir(0777);
        if (!::wxDirExists( oConfigHome.GetFullPath() ))
            wxLogMessage(_T("[Paths::Paths] Failed to create '%s'."),
                         oConfigHome.GetFullPath().c_str() );
    }

    //4. User data: ~/lenmus/
    wxFileName oDataHome;
    oDataHome.AssignDir( sHome );
    oDataHome.AppendDir(_T("lenmus"));
    if (!::wxDirExists( oDataHome.GetFullPath() ))
	{
		oDataHome.Mkdir(0777);
        if (!::wxDirExists( oDataHome.GetFullPath() ))
            wxLogMessage(_T("[Paths::Paths] Failed to create '%s'."),
                         oDataHome.GetFullPath().c_str() );
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
            wxLogMessage(_T("[Paths::Paths] Failed to create '%s'."),
                         path.GetFullPath().c_str() );
    }

    path = oLogsHome;
    path.AppendDir(_T("logs"));
    m_sLogs = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    if (!::wxDirExists( path.GetFullPath() ))
	{
		path.Mkdir(0777);
        if (!::wxDirExists( path.GetFullPath() ))
            wxLogMessage(_T("[Paths::Paths] Failed to create '%s'."),
                         path.GetFullPath().c_str() );
    }


    // Group 3. Configuration files, user dependent

    path = oConfigHome;
    m_sConfig = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);


    // Group 4. User scores and samples

    path = oDataHome;
    path.AppendDir(_T("scores"));
    m_sScores = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    if (!::wxDirExists( path.GetFullPath() ))
	{
		path.Mkdir(0777);
        if (!::wxDirExists( path.GetFullPath() ))
            wxLogMessage(_T("[Paths::Paths] Failed to create '%s'."),
                         path.GetFullPath().c_str() );
    }
    path.AppendDir(sVersion);
    if (!::wxDirExists( path.GetFullPath() ))
	{
		path.Mkdir(0777);
        if (!::wxDirExists( path.GetFullPath() ))
            wxLogMessage(_T("[Paths::Paths] Failed to create '%s'."),
                         path.GetFullPath().c_str() );
    }
    path.AppendDir(_T("samples"));
    m_sSamples = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    if (!::wxDirExists( path.GetFullPath() ))
	{
		path.Mkdir(0777);
        if (!::wxDirExists( path.GetFullPath() ))
            wxLogMessage(_T("[Paths::Paths] Failed to create '%s'."),
                         path.GetFullPath().c_str() );
    }
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
            wxLogMessage(_T("[Paths::create_folders] Failed to create '%s'."),
                         oFN.GetFullPath().c_str() );
    }

#if (LENMUS_PLATFORM_UNIX == 1)
    //create folders if they don't exist
    if (!::wxDirExists(m_sLogs))
	{
		wxFileName oFN(m_sLogs);
		oFN.Mkdir(0777);
        if (!::wxDirExists(m_sLogs))
            wxLogMessage(_T("[Paths::create_folders] Failed to create '%s'."),
                         oFN.GetFullPath().c_str() );
    }
    if (!::wxDirExists(m_sConfig))
	{
		wxFileName oFN(m_sConfig);
		oFN.Mkdir(0777);
        if (!::wxDirExists(m_sConfig))
            wxLogMessage(_T("[Paths::create_folders] Failed to create '%s'."),
                         oFN.GetFullPath().c_str() );
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
    wxLogMessage(_T("[Paths::log_paths] LENMUS_INSTALL_HOME = [%s]"), _T(LENMUS_INSTALL_HOME));

    wxLogMessage( _T("[Paths::log_paths] Root = %s"), GetRootPath().c_str() );
    wxLogMessage( _T("[Paths::log_paths] Bin = %s"), GetBinPath().c_str() );
    wxLogMessage( _T("[Paths::log_paths] Xrc = %s"), GetXrcPath().c_str() );
    wxLogMessage( _T("[Paths::log_paths] Temp = %s"), GetTemporaryPath().c_str() );
    wxLogMessage( _T("[Paths::log_paths] Img = %s"), GetImagePath().c_str() );
    wxLogMessage( _T("[Paths::log_paths] Cursors = %s"), GetCursorsPath().c_str() );
    wxLogMessage( _T("[Paths::log_paths] Sounds = %s"), GetSoundsPath().c_str() );
    wxLogMessage( _T("[Paths::log_paths] Locale = %s"), GetLocaleRootPath().c_str() );
    wxLogMessage( _T("[Paths::log_paths] Scores = %s"), GetScoresPath().c_str() );
    wxLogMessage( _T("[Paths::log_paths] TestScores = %s"), GetTestScoresPath().c_str() );
    wxLogMessage( _T("[Paths::log_paths] Samples = %s"), GetSamplesPath().c_str() );
    wxLogMessage( _T("[Paths::log_paths] Templates = %s"), GetTemplatesPath().c_str() );
    wxLogMessage( _T("[Paths::log_paths] Config = %s"), GetConfigPath().c_str() );
    wxLogMessage( _T("[Paths::log_paths] Log = %s"), GetLogPath().c_str() );
    wxLogMessage( _T("[Paths::log_paths] Fonts = %s\n"), GetFontsPath().c_str() );

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
    wxLogMessage(_T("[Paths::log_paths] homedir = %s"), sHome.c_str());

    wxFileName oLogsHome;
    oLogsHome.AssignDir( sHome );
    wxLogMessage(_T("[Paths::log_paths] initial oLogsHome = %s"), oLogsHome.GetFullPath().c_str());
    oLogsHome.AppendDir(_T(".config"));
    oLogsHome.AppendDir(_T("lenmus"));
    wxLogMessage(_T("[Paths::log_paths] final oLogsHome = %s"), oLogsHome.GetFullPath().c_str());
    //END_DBG -----------------------------------------------------------------------
#endif
}

//---------------------------------------------------------------------------------------
string Paths::dump_paths()
{
    stringstream s;
    s << "Root = " << to_std_string(GetRootPath()) << endl;
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
                    sFile.c_str() );
            }
            sFile = wxFindNextFile();
        }
        //reset flag
        fClearTemp = false;
        pPrefs->Write(_T("/Locale/LanguageChanged"), fClearTemp);
    }

}


}   //namespace lenmus
