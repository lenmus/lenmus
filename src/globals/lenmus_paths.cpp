//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2011 LenMus project
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

#include "lenmus_config.h"
#include "lenmus_injectors.h"

#include <wx/wxprec.h>
#include <wx/wx.h>


namespace lenmus
{


//=======================================================================================
// Paths implementation
//=======================================================================================
Paths::Paths(wxString sBinPath)
{
    //Receives the full path to the LenMus executable folder (/bin) and
    //extracts the root path
    m_sBin = sBinPath;
    #if (LENMUS_DEBUG == 1)
        m_root.Assign(_T(LENMUS_DBG_ROOT_PATH), _T(""), wxPATH_NATIVE);
    #else
        //wxLogMessage(_T("[Paths::Paths] sBinPath='%s'"), sBinPath.c_str());
        m_root.Assign(sBinPath, _T(""), wxPATH_NATIVE);
        m_root.RemoveLastDir();
    #endif
    m_root.Normalize();
    //wxString dbg = m_root.GetFullName();

    // Folders are organized into four groups
    //		1. Software and essentials
    //		2. Logs and temporal files
    //		3. Configuration files, user dependent
    //		4. User scores and samples
	//
	// Only files in the four group can be configured by the user
	//
	// For Windows all folders follow the working copy structure, but for Linux
	// there is a root path for each group. See next table:
    //
    // ------------------------------------------------------------------------------
    //      Linux                       Windows
    //    Default <prefix> = /usr/local
    //
    // 0. The lenmus program
    // ------------------------------------------------------------------------------
    //      <prefix>                    lenmus
    //          + /bin                      + \bin
    //
    // 1. Software and essentials (RootG1):
    // ------------------------------------------------------------------------------
    //      <prefix>/share/lenmus       lenmus
    //          + /xrc                      + \xrc
    //          + /res                      + \res
    //          + /locale                   + \locale
    //          + /books                    + \books
    //          + /templates                + \templates
    //          + /test-scores              + \test-scores
    //
    // 2. Logs and temporal files (RootG2):
    // ------------------------------------------------------------------------------
    //                                  lenmus
    // logs:    ~/.lenmus/logs              + \logs
    // temp:    /tmp/lenmus                 + \temp
    //
    // 3. Configuration files, user dependent (RootG3):
    // ------------------------------------------------------------------------------
    //      ~/.lenmus                    lenmus\bin
    //
    // 4. User scores and samples (RootG4):
    // ------------------------------------------------------------------------------
    //      ~/lenmus/scores              lenmus\scores
	//


	wxFileName path;

#if (LENMUS_PLATFORM_WIN32 == 1 || LENMUS_DEBUG == 1)
    wxFileName oRootG1 = m_root;
    wxFileName oRootG2 = m_root;
    wxFileName oRootG3 = m_root;
    wxFileName oRootG4 = m_root;
    #if (LENMUS_PLATFORM_WIN32 == 1)
        #if (LENMUS_DEBUG == 1)
            oRootG3.AppendDir(_T("z_bin"));
        #else
            oRootG3.AppendDir(_T("bin"));
        #endif
    #endif

#elif (LENMUS_PLATFORM_UNIX == 1)
	#if defined PACKAGE_PREFIX
		wxFileName oRootG1( _T(PACKAGE_PREFIX) );        //<prefix>
	#else
    	wxFileName oRootG1 = m_root;        //<prefix>
	#endif
    oRootG1.AppendDir(_T("share"));
    oRootG1.AppendDir(_T("lenmus"));
    wxFileName oRootG3(wxFileName::GetHomeDir() + _T("/") + _T(".lenmus/"));
    wxFileName oRootG4(wxFileName::GetHomeDir() + _T("/lenmus/"));

#endif

    // Group 1. Software and essentials

    path = oRootG1;
    path.AppendDir(_T("xrc"));
    m_sXrc = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = oRootG1;
    path.AppendDir(_T("res"));
    path.AppendDir(_T("icons"));
    m_sImages = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = oRootG1;
    path.AppendDir(_T("res"));
    path.AppendDir(_T("cursors"));
    m_sCursors = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = oRootG1;
    path.AppendDir(_T("res"));
    path.AppendDir(_T("sounds"));
    m_sSounds = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = oRootG1;
    path.AppendDir(_T("locale"));
    m_sLocaleRoot = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = oRootG1;
    path.AppendDir(_T("templates"));
    m_sTemplates = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = oRootG1;
    path.AppendDir(_T("test-scores"));
    m_sTestScores = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = oRootG1;
    path.AppendDir(_T("res"));
    path.AppendDir(_T("fonts"));
    m_sFonts = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);


    // Group 2. Logs and temporal files

#if (LENMUS_PLATFORM_WIN32 == 1 || LENMUS_DEBUG == 1)
    path = oRootG2;
    path.AppendDir(_T("temp"));
    m_sTemp = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = oRootG2;
    path.AppendDir(_T("logs"));
    m_sLogs = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

#else   //Linux, release version
    m_sTemp = _T("/tmp/lenmus/");
    path = oRootG3;
    path.AppendDir(_T("logs"));
    m_sLogs = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

#endif

    // Group 3. Configuration files, user dependent

    path = oRootG3;
    m_sConfig = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    //wxLogMessage(_T("[Paths] m_sConfig = '%s'"), m_sConfig.c_str());


    // Group 4. User scores and samples

    path = oRootG4;
    path.AppendDir(_T("scores"));
    m_sScores = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    path.AppendDir(_T("samples"));
    m_sSamples = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

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
    }

#if (LENMUS_PLATFORM_UNIX == 1)
    //create folders if they don't exist
    if (!::wxDirExists(m_sLogs))
	{
		wxFileName oFN(m_sLogs);
		oFN.Mkdir(0777);
    }
    if (!::wxDirExists(m_sConfig))
	{
		wxFileName oFN(m_sConfig);
		oFN.Mkdir(0777);
    }
    if (!::wxDirExists(m_sTemp))
	{
		wxFileName oFN(m_sTemp);
		oFN.Mkdir(0777);
    }
#endif

}

//---------------------------------------------------------------------------------------
Paths::~Paths()
{
    //SaveUserPreferences();
}

////---------------------------------------------------------------------------------------
//void Paths::SetLanguageCode(wxString sLangCode)
//{
//    //
//    // Lang code has changed. It is necessary to rebuild paths depending on language
//    //
//    // IMPORTANT: When this method is invoked wxLocale object is not
//    //            yet initialized. DO NOT USE LANGUAGE DEPENDENT STRINGS HERE
//    //
//    m_sLangCode = sLangCode;
//    wxFileName oLocalePath(m_sLocaleRoot, _T(""), wxPATH_NATIVE);
//    oLocalePath.AppendDir(m_sLangCode);
//    m_sLocale = oLocalePath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
//
//#if 0
//    wxFileName oBooksPath = m_root;
//    oBooksPath.AppendDir(_T("books"));
//    oBooksPath.AppendDir(m_sLangCode);
//    m_sBooks = oBooksPath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
//#else
//    wxFileName oBooksPath = oLocalePath;
//    oBooksPath.AppendDir(_T("books"));
//    m_sBooks = oBooksPath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
//#endif
//
//    wxFileName oHelpPath = oLocalePath;
//    oHelpPath.AppendDir(_T("help"));
//    m_sHelp = oHelpPath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
//
//    // When changing language a flag was stored so that at next run the program must
//    // clean the temp folder. Check this.
//	ClearTempFiles();
//
//}
//
////---------------------------------------------------------------------------------------
//void Paths::LoadUserPreferences()
//{
//    //
//    // load settings form user congiguration data or default values
//    //
//
//	// Only the path for group 4 files can be selected by the user
//    g_pPrefs->Read(_T("/Paths/Scores"), &m_sScores);
//
//}
//
////! save path settings in user configuration data
//void Paths::SaveUserPreferences()
//{
//    //g_pPrefs->Write(_T("/Paths/Locale"), m_sLocaleRoot);
//    g_pPrefs->Write(_T("/Paths/Scores"), m_sScores);
//    //g_pPrefs->Write(_T("/Paths/Temp"), m_sTemp);
//    //g_pPrefs->Write(_T("/Paths/Xrc"), m_sXrc);
//    //g_pPrefs->Write(_T("/Paths/Images"), m_sImages);
//    //g_pPrefs->Write(_T("/Paths/Sounds"), m_sSounds);
//    //g_pPrefs->Write(_T("/Paths/Config"), m_sConfig);
//    //g_pPrefs->Write(_T("/Paths/Logs"), m_sLogs);
//
//    // bin path is not user configurable
//    //g_pPrefs->Write(_T("/Paths/Bin"), m_sBin);
//
//}
//
////---------------------------------------------------------------------------------------
//void Paths::ClearTempFiles()
//{
//    // When changing language a flag was stored so that at next run the program must
//    // clean the temp folder. Otherwise, as books have the same names in all languages,
//    // in Spanish, the new language .hcc and hhk files will not be properly loaded.
//    // Here I test this flag and if true, remove all files in temp folder
//    bool fClearTemp;
//    g_pPrefs->Read(_T("/Locale/LanguageChanged"), &fClearTemp, false );
//    if (fClearTemp) {
//        wxString sFile = wxFindFirstFile(m_sTemp);
//        while ( !sFile.empty() ) {
//            if (!::wxRemoveFile(sFile)) {
//                wxLogMessage(_T("[Paths::LoadUserPreferences] Error deleting %s"),
//                    sFile.c_str() );
//            }
//            sFile = wxFindNextFile();
//        }
//        //reset flag
//        fClearTemp = false;
//        g_pPrefs->Write(_T("/Locale/LanguageChanged"), fClearTemp);
//    }
//
//}


}   //namespace lenmus
