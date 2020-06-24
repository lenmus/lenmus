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

//lenmus
#include "lenmus_app.h"
#include "lenmus_standard_header.h"

#include "lenmus_main_frame.h"
#include "lenmus_art_provider.h"
#include "lenmus_paths.h"
#include "lenmus_midi_server.h"
#include "lenmus_languages.h"
#include "lenmus_dlg_choose_lang.h"
#include "lenmus_splash_frame.h"
#include "lenmus_chord.h"
#include "lenmus_string.h"

#if (LENMUS_ENABLE_UNIT_TESTS == 1)
	#include "lenmus_test_runner.h"            //to run tests
#endif

//wxWidgets
#include <wx/filesys.h>
#include <wx/fs_zip.h>          //to use the zip file system
#include <wx/xrc/xmlres.h>      //to use XRC resorces system
#include <wx/wfstream.h>        //to read config.ini in setting language
#include <wx/txtstrm.h>         //idem
//#include <wx/memory.h>			//to trace memory leaks
#include <wx/cmdline.h>         //for parsing command line

//lomse
#include <lomse_logger.h>
using namespace lomse;

//other
#include <iostream>
using namespace std;

#if (LENMUS_PLATFORM_UNIX == 1 || LENMUS_PLATFORM_MAC == 1)     //for determine_exec_path()
    #include <limits.h>
    #include <libgen.h>
    #include <unistd.h>
    #ifndef PATH_MAX
        #define PATH_MAX   4096
    #endif
#endif

#if (LENMUS_PLATFORM_WIN32 == 1)     //for determine_exec_path()
    #include <io.h>
    #include <string.h>
    #define strtok_r     strtok_s
    //https://social.msdn.microsoft.com/Forums/vstudio/en-US/963aac07-da1a-4612-be4a-faac3f1d65ca/strtok-threadsafety?forum=vcgeneral
    #define access   _access_s
    #define F_OK     0
    #ifndef _MAX_PATH
        #define _MAX_PATH   4096
    #endif
    #ifndef PATH_MAX
        #define PATH_MAX _MAX_PATH
    #endif
    #define realpath(N,R) _fullpath((R),(N),_MAX_PATH)
#endif

//For finding executable path on MacOS:
#if (LENMUS_PLATFORM_MAC == 1)
#include <mach-o/dyld.h>
#endif


// This macro will allow wxWindows to create the application object during program
// execution (it's better than using a static object for many reasons) and also
// declares the accessor function wxGetApp() which will return the reference of
// the right type (i.e. TheApp and not wxApp)
wxIMPLEMENT_APP(lenmus::TheApp);



namespace lenmus
{

DEFINE_EVENT_TYPE(LM_EVT_CHANGE_LANGUAGE)
DEFINE_EVENT_TYPE(LM_EVT_RESTART_APP)


//=======================================================================================
// TheApp implementation
//=======================================================================================

wxBEGIN_EVENT_TABLE(TheApp, wxApp)
    EVT_COMMAND(wxID_ANY, LM_EVT_CHANGE_LANGUAGE, TheApp::on_change_language)
    EVT_COMMAND(wxID_ANY, LM_EVT_RESTART_APP, TheApp::on_restart)
wxEND_EVENT_TABLE()

//---------------------------------------------------------------------------------------
TheApp::TheApp()
    : wxApp()
    , m_fUseGui(true)
    , m_pInstanceChecker((wxSingleInstanceChecker*)nullptr)
    , m_pLocale(nullptr)
    , m_pSplash(nullptr)
    , m_appScope(cout)
{
    #if (LENMUS_DEBUG_BUILD == 0)
        //in release version we will deal with crashes.
        //tell base class to call our OnFatalException()
        wxHandleFatalExceptions();
    #endif
}

//---------------------------------------------------------------------------------------
bool TheApp::OnInit()
{
    //A wxWidgets application does not have a main procedure; the equivalent is
    //this: the OnInit member
    //The method builds the objects necessary to run the application (it is like
    //the 'constructor'. It must return true to continue, and in this case, control
    //is transferred to OnRun(). Otherwise the application is terminated witout further
    //processing

    if (!do_application_setup())
        return false;

    if (!wxApp::OnInit())
    {
        do_application_cleanup();
        return false;
    }

    //wxApp::OnInit() will invoke OnInitCmdLine() and OnCmdLineParsed()
    //Therefore, at this point command line is parsed, and all options set up
    if (!m_fUseGui)
    {
        do_application_cleanup();
        return false;   //stop
    }

    create_main_frame();
    ::wxBeginBusyCursor();
    wait_and_destroy_splash();
    show_welcome_window();
//    RecoverScoreIfPreviousCrash();
    ::wxEndBusyCursor();

    check_for_updates();
//    #if (LENMUS_DEBUG_BUILD == 1)
//        m_frame->RunUnitTests();
//    #endif

    return true;
}

//---------------------------------------------------------------------------------------
bool TheApp::do_application_setup()
{
    // set information about this application
    wxString& sAppName = m_appScope.get_app_name();
    SetVendorName( m_appScope.get_vendor_name() );
    SetAppName(sAppName);

    // verify that this is the only instance running
    wxString name = sAppName + "-" + m_appScope.get_version_string()
                    + "-" + wxGetUserId() + ".lock";
    m_pInstanceChecker = LENMUS_NEW wxSingleInstanceChecker(name);
    if ( m_pInstanceChecker->IsAnotherRunning() )
    {
        wxString msg =  wxString::Format(_("Another instance of %s is already running."),
                                         m_appScope.get_app_name().wx_str() );
        //TODO: Linux: Advise user that if there is no another instance running, a lock
        // file is left 'abandoned' in Home dir. The file is named
        //  "Lenmus Phonascus <version>-<username>.lock"
        // and its size is few bytes (around five). Delete this file to fix this problem.
        wxMessageBox(msg, sAppName, wxOK | wxICON_EXCLAMATION );
        return false;
    }

    //this determines the executable path and creates the path objects and the user
    //folders if they do not exist.
    create_paths_object();
	//all paths, except user configurable ones, are valid from this point

    m_appScope.create_preferences_object();
    //preferences object and paths are set. We can proceed to initialize
    //global variables that depend on user preferences.

    load_user_preferences();
	//all paths, even user configurable ones, are valid from this point

    m_appScope.create_logger();
    //from this point macros LOMSE_LOG_XXX can be used

    set_up_current_language();
    m_appScope.inform_lomse_about_fonts_path();

//    //UploadForensicLogIfExists();
//    //Upload forensic log, if exists
//
//    // open forensic log file
//    sLogFile = g_pPaths->GetLogPath() + sUserId + "_forensic_log.txt";
//    wxString sLogScore = g_pPaths->GetLogPath() + sUserId + "_score.lmb";
//    if (g_pLogger->IsValidForensicTarget(sLogFile))
//    {
//        //previous program run terminated with a crash and forensic log was not
//        //uploaded (probably we were in debug mode and program execution was
//        //cancelled. Inform user and request permision to submit file for bug
//        //analysis
//        SendForensicLog(sLogFile, false);       //false: not handling a crash
//    }
//    g_pLogger->SetForensicTarget(sLogFile, sLogScore);
//
//    DefineTraceMasks();
//	// AWARE: Log/debug methods are available from this point
//
//	wxLogMessage("[TheApp::OnInit] Config file: " + oCfgFile.GetFullPath() );
//
    m_appScope.get_paths()->log_paths();
    m_appScope.open_database();

    // Define handlers for the image types managed by the application
    // BMP handler is by default always defined
    wxImage::AddHandler( LENMUS_NEW wxPNGHandler );
    wxImage::AddHandler( LENMUS_NEW wxJPEGHandler );

    // Set the art provider and get current user selected background bitmap
    wxArtProvider::Push(LENMUS_NEW ArtProvider(m_appScope));
    //m_background = wxArtProvider::GetBitmap("backgrnd");

    //Include support for zip files
    wxFileSystem::AddHandler(LENMUS_NEW wxZipFSHandler);

    initialize_xrc_resources();
//    CreateDocumentManager();
//    CreateDocumentTemplates();


//#if (LENMUS_DEBUG_BUILD == 1) && (LENMUS_PLATFORM_UNIX == 1)
//    //For Linux in Debug build, use a window to show wxLog messages. This is
//    //the only way I've found to see wxLog messages with Code::Blocks
//    wxLogWindow* pMyLog = LENMUS_NEW wxLogWindow(m_frame, "Debug window: wxLogMessages");
//    wxLog::SetActiveTarget(pMyLog);
//    pMyLog->Flush();
//#endif

    //Seed the random-number generator with current time so that
    //the numbers will be different every time we run.
    srand( (unsigned)time( nullptr ) );

    return true;
}

//---------------------------------------------------------------------------------------
void TheApp::create_paths_object()
{
    m_appScope.set_bin_folder( determine_exec_path() );
}

//---------------------------------------------------------------------------------------
wxString TheApp::determine_exec_path()
{
    // Base on code from:
    // https://stackoverflow.com/questions/1023306/finding-current-executables-path-without-proc-self-exe
    // Copyright 2015 by Mark Whitis.  License=MIT

    #ifndef PATH_MAX
        #define PATH_MAX   4096
    #endif

    char execPathName[PATH_MAX];     // PATH_MAX incudes the \0 so +1 is not required

#if (LENMUS_PLATFORM_WIN32 == 1)
    // On Windows the path to the LenMus program is in argv[0]
    // Use this old code (nobody rported failures) instead of new code that I cannot test
    wxString sBinPath = wxPathOnly(argv[0]);
    //wxLogMessage("[TheApp::determine_exe_path] sBinPath='%s'", sBinPath.wx_str());
    //but in console mode fails!
    if (sBinPath.IsEmpty())
        sBinPath = wxGetCwd();
    return sBinPath;

#elif (LENMUS_PLATFORM_UNIX == 1 || LENMUS_PLATFORM_MAC == 1)

    //For Linux try first the /proc/self/exe path
    #if __linux__  //&& !__ANDROID__
        #if defined(__sun)
            #define PROC_SELF_EXE "/proc/self/path/a.out"
        #else
            #define PROC_SELF_EXE "/proc/self/exe"
        #endif

        if (realpath(PROC_SELF_EXE, execPathName))
            return wxPathOnly(wxString(execPathName));

    #endif // __linux__ && !__ANDROID__
    
    //A method for MacOS:
#elif (LENMUS_PLATFORM_MAC == 1)
         char macPathName[PATH_MAX];
         uint32_t execSize = sizeof(macPathName);
         if(_NSGetExecutablePath(macPathName, &execSize ) == 0){
            if(realpath(macPathName, execPathName)){
                return wxPathOnly(wxString(execPathName));
            }
         }

#endif

    //for other platforms or when the /proc/self/exe method fails in Linux, try to
    //deduce it from argv[0]

    //get some needed values
    char saved_pwd[PATH_MAX];
    char saved_argv0[PATH_MAX];
    char saved_path[PATH_MAX];
    char path_separator='/';
    char path_separator_as_string[2]="/";
    char path_list_separator[8]=":";  // could be ":; "

    char* res = getcwd(saved_pwd, sizeof(saved_pwd));

    strncpy(saved_argv0, to_std_string(wxTheApp->argv[0]).c_str(), sizeof(saved_argv0));
    saved_argv0[sizeof(saved_argv0)-1]=0;

    strncpy(saved_path, getenv("PATH"), sizeof(saved_path));
    saved_path[sizeof(saved_path)-1]=0;

    //now, let's proceed
    char newpath[PATH_MAX+256];
    char newpath2[PATH_MAX+256];

    res = realpath(saved_argv0, execPathName);
    size_t size_of_result = sizeof(execPathName);
    execPathName[0]=0;
    if (res == nullptr || size_of_result == 0)
    {
        //Error. argv[0] is empty. Return current path to try to avoid crashes
        return wxGetCwd();
    }


    if(saved_argv0[0]==path_separator)
    {
        //If argv[0] is an absolute path, use that as a starting point. An absolute
        //path probably starts with "/" but on some non-Unix systems it might start
        //with "\" or a drive letter or name prefix followed by a colon.

        res = realpath(saved_argv0, newpath);
        if(res && !access(newpath, F_OK))
        {
            strncpy(execPathName, newpath, size_of_result);
            execPathName[size_of_result-1]=0;
            return wxPathOnly(wxString(execPathName));
        }
        else
        {
            //Error. return current path to try to avoid crashes
            return wxGetCwd();
        }
    }

    else if( strchr(saved_argv0, path_separator ))
    {
        //if argv[0] is a relative path (contains "/" or "\" but doesn't start with it,
        //such as "../../bin/foo", then combine pwd+"/"+argv[0] (use present working
        //directory from when program started, not current).

        strncpy(newpath2, saved_pwd, sizeof(newpath2));
        newpath2[sizeof(newpath2)-1]=0;

        strncat(newpath2, path_separator_as_string, sizeof(newpath2));
        newpath2[sizeof(newpath2)-1]=0;

        strncat(newpath2, saved_argv0, sizeof(newpath2));
        newpath2[sizeof(newpath2)-1]=0;

        res = realpath(newpath2, newpath);
        if(res && !access(newpath, F_OK))
        {
            strncpy(execPathName, newpath, size_of_result);
            execPathName[size_of_result-1]=0;
            return wxPathOnly(wxString(execPathName));
        }
        else
        {
            //Error. return current path to try to avoid crashes
            return wxGetCwd();
        }
    }
    else
    {
        //if argv[0] is a plain basename (no slashes), then combine it with each entry
        //in PATH environment variable in turn and try those and use the first one which
        //succeeds.

        char *saveptr;
        char *pathitem;
        for(pathitem=strtok_r(saved_path, path_list_separator,  &saveptr);
            pathitem;
            pathitem=strtok_r(nullptr, path_list_separator, &saveptr) )
        {

            strncpy(newpath2, pathitem, sizeof(newpath2));
            newpath2[sizeof(newpath2)-1]=0;

            strncat(newpath2, path_separator_as_string, sizeof(newpath2));
            newpath2[sizeof(newpath2)-1]=0;

            strncat(newpath2, saved_argv0, sizeof(newpath2));
            newpath2[sizeof(newpath2)-1]=0;

            res = realpath(newpath2, newpath);
            if(res && !access(newpath, F_OK))
            {
                strncpy(execPathName, newpath, size_of_result);
                execPathName[size_of_result-1]=0;
                return wxPathOnly(wxString(execPathName));
            }
        }
        //Error. return current path to try to avoid crashes
        return wxGetCwd();

    } // end else

    //if we get here, we have tried all three methods on argv[0] and still haven't
    //succeeded. Include fallback methods here.
    //You could try the very platform specific /proc/self/exe, /proc/curproc/file (BSD),
    //and (char *)getauxval(AT_EXECFN), and dlgetname(...) if present. You might even
    //try these before argv[0]-based methods, if they are available and you don't
    //encounter permission issues. In the somewhat unlikely event (when you consider
    //all versions of all systems) that they are present and don't fail, they might
    //be more authoritative.

    //Error. return current path to try to avoid crashes
    return wxGetCwd();
}

//---------------------------------------------------------------------------------------
void TheApp::load_user_preferences()
{
    wxConfigBase* pPrefs = m_appScope.get_preferences();

    bool value;
    pPrefs->Read("/Options/EnableAnswerSounds", &value, true);
    m_appScope.enable_answer_sounds(value);

    pPrefs->Read("/Options/AutoNewProblem", &value, true);
    m_appScope.enable_auto_new_problem(value);

    pPrefs->Read("/Options/ExperimentalFeatures", &value, false);
    m_appScope.enable_experimental_features(value);

    //pPrefs->Read("/Options/AutoBeam", &g_fAutoBeam, true);
}

////---------------------------------------------------------------------------------------
//void TheApp::DefineTraceMasks()
//{
//#if (LENMUS_DEBUG_BUILD == 1)
//    //define trace masks to be known by trace system
//	g_pLogger->DefineTraceMask("Cadence");
//	g_pLogger->DefineTraceMask("Chord");
//	g_pLogger->DefineTraceMask("lmColStaffObjs::Delete");
//	g_pLogger->DefineTraceMask("lmColStaffObjs::Insert");
//    g_pLogger->DefineTraceMask("lmComposer6");
//    g_pLogger->DefineTraceMask("lmComposer6::AssignNonChordNotes");
//    g_pLogger->DefineTraceMask("lmComposer6::FunctionToChordNotes");
//    g_pLogger->DefineTraceMask("lmComposer6::GenerateContour");
//    g_pLogger->DefineTraceMask("lmComposer6::InstantiateNotes");
//    g_pLogger->DefineTraceMask("lmComposer6::NearestNoteOnChord");
//    g_pLogger->DefineTraceMask("Formater4");
//    g_pLogger->DefineTraceMask("Formatter4.Step1");
//    g_pLogger->DefineTraceMask("FragmentsTable::GetFirstSegmentDuracion");
//    g_pLogger->DefineTraceMask("IdfyScalesCtrol");
//    g_pLogger->DefineTraceMask("IdfyCadencesCtrol");
//    g_pLogger->DefineTraceMask("Interval");
//    g_pLogger->DefineTraceMask("lmKeySignature");
//    g_pLogger->DefineTraceMask("lmLDPParser");
//    g_pLogger->DefineTraceMask("LDPParser_beams");
//    g_pLogger->DefineTraceMask("lmMusicXMLParser");
//    g_pLogger->DefineTraceMask("OnMouseEvent");
//    g_pLogger->DefineTraceMask("lmScoreAuxCtrol");
//    g_pLogger->DefineTraceMask("Timing: Score renderization");
//	g_pLogger->DefineTraceMask("TheoKeySignCtrol");
//    g_pLogger->DefineTraceMask("Updater");
//#endif
//}

//---------------------------------------------------------------------------------------
void TheApp::initialize_xrc_resources()
{
    // Initialize all the XRC handlers.
    wxXmlResource::Get()->InitAllHandlers();

    // get path for xrc resources
    Paths* pPaths = m_appScope.get_paths();
    wxString sPath = pPaths->GetXrcPath();

    // Load all of the XRC files that will be used. You can put everything
    // into one giant XRC file if you wanted, but then they become more
    // difficult to manage, and harder to reuse in later projects.

    // The score generation settings dialog
    wxFileName oXrcFile(sPath, "DlgCfgScoreReading", "xrc", wxPATH_NATIVE);
    wxXmlResource::Get()->Load( oXrcFile.GetFullPath() );

    // Ear Interval exercises: configuration dialog
    oXrcFile = wxFileName(sPath, "DlgCfgEarIntervals", "xrc", wxPATH_NATIVE);
    wxXmlResource::Get()->Load( oXrcFile.GetFullPath() );

    // Chord identification exercises: configuration dialog
    oXrcFile = wxFileName(sPath, "DlgCfgIdfyChord", "xrc", wxPATH_NATIVE);
    wxXmlResource::Get()->Load( oXrcFile.GetFullPath() );

    // Scales identification exercises: configuration dialog
    oXrcFile = wxFileName(sPath, "DlgCfgIdfyScale", "xrc", wxPATH_NATIVE);
    wxXmlResource::Get()->Load( oXrcFile.GetFullPath() );

    // Cedences identification exercises: configuration dialog
    oXrcFile = wxFileName(sPath, "DlgCfgIdfyCadence", "xrc", wxPATH_NATIVE);
    wxXmlResource::Get()->Load( oXrcFile.GetFullPath() );

    // Pattern Editor dialog
    oXrcFile = wxFileName(sPath, "DlgPatternEditor", "xrc", wxPATH_NATIVE);
    wxXmlResource::Get()->Load( oXrcFile.GetFullPath() );

    // Updater dialog: start
    oXrcFile = wxFileName(sPath, "UpdaterDlgStart", "xrc", wxPATH_NATIVE);
    wxXmlResource::Get()->Load( oXrcFile.GetFullPath() );

    // Updater dialog: info
    oXrcFile = wxFileName(sPath, "UpdaterDlgInfo", "xrc", wxPATH_NATIVE);
    wxXmlResource::Get()->Load( oXrcFile.GetFullPath() );

    #if (LENMUS_DEBUG_BUILD == 1)
        // Debug: masks to trace dialog
        oXrcFile = wxFileName(sPath, "DlgDebugTrace", "xrc", wxPATH_NATIVE);
        wxXmlResource::Get()->Load( oXrcFile.GetFullPath() );
    #endif
}

//---------------------------------------------------------------------------------------
void TheApp::create_main_frame()
{
    m_nSplashVisibleMilliseconds = 3000L;   // at least visible for 3 seconds
	m_nSplashStartTime = long( time(nullptr) );
#if 1       //1 = no splash. useful for what? only for delaying user
    m_pSplash = create_GUI(0L, true);
#else
    m_pSplash = create_GUI(m_nSplashVisibleMilliseconds, true /*first time*/);
#endif
}

//---------------------------------------------------------------------------------------
void TheApp::wait_and_destroy_splash()
{
	// check if the splash window display time is elapsed and wait if not

    if (m_pSplash)
    {
	    m_nSplashVisibleMilliseconds -= ((long)time( nullptr ) - m_nSplashStartTime);
	    if (m_nSplashVisibleMilliseconds > 0) ::wxMilliSleep( m_nSplashVisibleMilliseconds );
        m_pSplash->AllowDestroy();    // allow to destroy the splash
    }
}

//---------------------------------------------------------------------------------------
void TheApp::show_welcome_window()
{
    m_frame->show_welcome_window();
}

////---------------------------------------------------------------------------------------
//void TheApp::RecoverScoreIfPreviousCrash()
//{
//    //open any existing score being edited before a crash
//    wxString sUserId = ::wxGetUserId();
//    wxString sLogScore = g_pPaths->GetLogPath() + sUserId + "_score.lmb";
//    if (::wxFileExists(sLogScore))
//    {
//        wxString sQuestion =
//            _("An score being edited before a program crash has been detected!");
//        sQuestion += "\n\n";
//        sQuestion += _("Should the program attempt to recover it?");
//        lmQuestionBox oQB(sQuestion, 2,     //msge, num buttons,
//            //labels (2 per button: button text + explanation)
//            _("Yes"), _("Yes, try to recover the score"),
//            _("No"), _("No, forget about that score")
//        );
//        int nAnswer = oQB.ShowModal();
//
//		if (nAnswer == 0)       //'Yes' button
//            m_frame->OpenScore(sLogScore, true);    //true: as LENMUS_NEW file
//    }
//}

//---------------------------------------------------------------------------------------
void TheApp::check_for_updates()
{
    //check for updates if this option is set up. Default: do check
    wxConfigBase* pPrefs = m_appScope.get_preferences();
    wxString sCheckFreq = pPrefs->Read("/Options/CheckForUpdates/Frequency", "Weekly" );
    if (sCheckFreq != "Never")
    {
        //get date of last sucessful check
        bool fDoCheck = false;
        wxString sLastCheckDate =
            pPrefs->Read("/Options/CheckForUpdates/LastCheck", "");
        if (sLastCheckDate == "")
        {
            fDoCheck = true;
        }
        else
        {
            wxDateTime dtLastCheck, dtNextCheck;
            wxDateSpan dsSpan;
            const wxChar *p = dtLastCheck.ParseDate(sLastCheckDate);
            if ( !p )
            {
                LOMSE_LOG_ERROR("Error parsing the last check for updates date '%s'.",
                                to_std_string(sLastCheckDate).c_str());
                fDoCheck = true;
            }
            else
            {
                //verify elapsed time
                if (sCheckFreq == "Daily")
                    dsSpan = wxDateSpan::Days(1);
                else if (sCheckFreq == "Weekly")
                    dsSpan = wxDateSpan::Weeks(1);
                else
                    dsSpan = wxDateSpan::Months(1);

                dtNextCheck = dtLastCheck;
                dtNextCheck.Add(dsSpan);
                fDoCheck = (dtNextCheck <= wxDateTime::Now());
            }

            wxString sDoCheck = fDoCheck ? "True" : "False";
            LOMSE_LOG_INFO("[TheApp::OnInit] CheckForUpdates: dtLastCheck='%s', "
                           "sCheckFreq=%s (%d), dtNextCheck='%s', fDoCheck=%s"
                , to_std_string(dtLastCheck.Format("%x")).c_str()
                , to_std_string(sCheckFreq).c_str()
                , dsSpan.GetTotalDays()
                , to_std_string(dtNextCheck.Format("%x")).c_str()
                , to_std_string(sDoCheck).c_str() );
        }

        // if time for another check, do it
        if (fDoCheck)
        {
            wxCommandEvent event(LM_EVT_CHECK_FOR_UPDATES, k_id_check_for_updates);
            //wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, k_id_silently_check_for_updates);
            wxPostEvent(m_frame, event);
        }
    }
}

//---------------------------------------------------------------------------------------
void TheApp::do_application_cleanup()
{
//    // the wave sound manager object
//    lmWaveManager::Destroy();
//

    //the single instance checker
    delete m_pInstanceChecker;

//    //other objects
//    lmPgmOptions::DeleteInstance();             //the program options object
    delete m_pLocale;                           //locale object
//    lmProcessorMngr::DeleteInstance();          //Processor manager
    ChordsDB::DeleteInstance();               //Chords Database
}

//---------------------------------------------------------------------------------------
void TheApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    //AWARE: This method is automatically invoked from OnInit()

      //Possible commands for future:
      //  export scores/documents as pdf
      //        lenmus export -f <document> -o <document.pdf>
      //  check cadence (two chords)
      //        lenmus check -c1 "a2,c3,e3,a3" -c2 "a2,d3,f3,a3", -k a

    static const wxCmdLineEntryDesc cmdLineDesc[] =
    {
        { wxCMD_LINE_SWITCH, "h", "help", "Show this help message",
            wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
        { wxCMD_LINE_SWITCH, "t", "test",  "Run unit tests",
            wxCMD_LINE_VAL_NONE, 0 },

        //entry for end of table
        { wxCMD_LINE_NONE, "", "", "", wxCMD_LINE_VAL_NONE, 0 }
    };

    parser.SetDesc(cmdLineDesc);
    parser.SetSwitchChars("-");        //use '-' as parameter starter
}

//---------------------------------------------------------------------------------------
bool TheApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    m_fUseGui = !parser.Found("t");

    if ( parser.Found("t") )
    {
        //AWARE: In MS Windows, commands cannot write to the same command-line window
        // that started the program. Therefore, in MS Windows the results will be
        // written to file "unit-tests-results.txt"
        //
        //See:  http://forums.wxwidgets.org/viewtopic.php?t=22211
        //
        //According to the following article (see the second Q&A)
        //
        //    http://msdn.microsoft.com/en-ca/magazine/cc164023.aspx
        //
        //writting the output in the same command-line window as the one that
        //started the program is not possible under Windows, because when executing a
        //GUI program, the command prompt does not wait for the program to finish
        //executing, so the command prompt will be screwed up if you try to write
        //in the same console. The fault is with Windows, not with wxWidgets.

        #if (LENMUS_ENABLE_UNIT_TESTS == 1)
            bool fUseCout = false;
            #if (LENMUS_PLATFORM_UNIX == 1 || LENMUS_PLATFORM_MAC == 1)
                fUseCout = true;
            #endif
            MyTestRunner oTR(nullptr, fUseCout);
            oTR.RunTests();
        #else
            printf("Lenmus unit tests are not included in the build\n");
        #endif
	}

    return true;
}

//---------------------------------------------------------------------------------------
void TheApp::set_up_current_language()
{
    wxConfigBase* pPrefs = m_appScope.get_preferences();
    wxString lang = pPrefs->Read("/Locale/Language", "");
    if (lang.IsEmpty())
    {
        //The language is not set. This will only happen the first time
        //the program is run or if lenmus.ini file is deleted

        // try to get installer choosen language and use it if found
        lang = get_installer_language();

        if (lang.IsEmpty())
        {
            // Not found. Pop up a dialog to choose language.
            lang = choose_language(nullptr);
        }
        pPrefs->Write("/Locale/Language", lang);
    }

    // Now that language code is known we can finish Paths initialization
    // and load locale catalogs
    m_appScope.on_language_changed(lang);
    set_up_locale(lang);
}

//---------------------------------------------------------------------------------------
void TheApp::on_change_language(wxCommandEvent& WXUNUSED(event))
{
    set_up_current_language();
    restart();
}

//---------------------------------------------------------------------------------------
void TheApp::on_restart(wxCommandEvent& WXUNUSED(event))
{
    restart();
}

//---------------------------------------------------------------------------------------
void TheApp::restart()
{
    create_GUI(0, false);   //0 = No splash, false=not first time
    show_welcome_window();
}

//---------------------------------------------------------------------------------------
void TheApp::set_up_locale(wxString lang)
{
    Paths* pPaths = m_appScope.get_paths();
    pPaths->SetLanguageCode(lang);

    //get wxLanguage name
    const wxLanguageInfo* pInfo = wxLocale::FindLanguageInfo(lang);
    wxString sLangName;
    if (pInfo)
        sLangName = pInfo->Description;
    else
    {
        sLangName = "English";
        LOMSE_LOG_INFO("Language '%s' not found. Update TheApp.cpp?",
                       to_std_string(lang).c_str());
    }


    // locale object re-initialization
    delete m_pLocale;
    m_pLocale = LENMUS_NEW wxLocale();
    if (!m_pLocale->Init("", lang, "", false))
    {
        wxMessageBox( wxString::Format("Language %s can not be set. "
            "Please, verify that any required language codepages are installed in your system.",
            sLangName.wx_str()));
    }
    else if (lang != "en")
    {
        wxString sPath = pPaths->GetLocaleRootPath();
        m_pLocale->AddCatalogLookupPathPrefix( sPath );
        wxString sCtlg;
        wxString sNil = "";
        sCtlg = sNil + "lenmus_" + lang;    //m_pLocale->GetName();
        if (!m_pLocale->AddCatalog(sCtlg))
            LOMSE_LOG_INFO("Failure to load catalog '%s'. Path='%s'",
                           to_std_string(sCtlg).c_str(),
                           to_std_string(sPath).c_str() );
        sCtlg = sNil + "wxwidgets_" + lang;
        if (!m_pLocale->AddCatalog(sCtlg))
            LOMSE_LOG_INFO("Failure to load catalog '%s'. Path='%s'",
                           to_std_string(sCtlg).c_str(),
                           to_std_string(sPath).c_str() );
        sCtlg = sNil + "wxmidi_" + lang;
        if (!m_pLocale->AddCatalog(sCtlg))
            LOMSE_LOG_INFO("Failure to load catalog '%s'. Path='%s'",
                           to_std_string(sCtlg).c_str(),
                           to_std_string(sPath).c_str() );
    }
}

//---------------------------------------------------------------------------------------
int TheApp::OnExit()
{
    do_application_cleanup();
	return 0;
}

//---------------------------------------------------------------------------------------
void TheApp::get_default_placement(wxRect *defRect)
{
    //Get screen size
    wxRect screenRect;
    wxClientDisplayRect(&screenRect.x, &screenRect.y,
                        &screenRect.width, &screenRect.height);

   //default size/pos: full screen
   defRect->x = 0;
   defRect->y = 0;
   defRect->width = screenRect.width;
   defRect->height = screenRect.height;
}

//---------------------------------------------------------------------------------------
void TheApp::get_main_window_placement(wxRect* frameRect, bool* fMaximized)
{
    // set the default window size
    wxRect defWndRect;
    get_default_placement(&defWndRect);
    //wxLogMessage( wxString::Format("[TheApp::get_main_window_placement] default: x=%d, y=%d, w=%d, h=%d",
    //                defWndRect.x, defWndRect.y, defWndRect.width, defWndRect.height));

    //Read the values from the config file, or use the defaults
    wxConfigBase* pConfig = m_appScope.get_preferences();
    frameRect->SetWidth(pConfig->Read("/MainFrame/Width", defWndRect.GetWidth()));
    frameRect->SetHeight(pConfig->Read("/MainFrame/Height", defWndRect.GetHeight()));
    frameRect->SetLeft(pConfig->Read("/MainFrame/Left", defWndRect.GetLeft() ));
    frameRect->SetTop(pConfig->Read("/MainFrame/Top", defWndRect.GetTop() ));

    pConfig->Read("/MainFrame/Maximized", fMaximized);

        //--- Make sure that the Window will be completely visible -------------

    //Get the size of the screen
    wxRect screenRect;
    wxClientDisplayRect(&screenRect.x, &screenRect.y,
                        &screenRect.width, &screenRect.height);
    //wxLogMessage( wxString::Format("[TheApp::get_main_window_placement] screen: x=%d, y=%d, w=%d, h=%d",
    //                screenRect.x, screenRect.y, screenRect.width, screenRect.height));

    //If we have hit the bottom of the screen restore default position on the screen
    if (frameRect->y + frameRect->height > screenRect.y + screenRect.height)
    {
        frameRect->x = defWndRect.x;
        frameRect->y = defWndRect.y;
    }

    //if we have hit the right side of the screen restore default position
    if (frameRect->x + frameRect->width > screenRect.x + screenRect.width)
    {
          frameRect->x = defWndRect.x;
          frameRect->y = defWndRect.y;
    }

    //Next check if the screen is too small for the default width and height
    if ((frameRect->x + frameRect->width > screenRect.x + screenRect.width) ||
        (frameRect->y + frameRect->height > screenRect.y + screenRect.height) )
    {
        //Resize the main window to fit in the screen
        frameRect->width = screenRect.width - frameRect->x;
        frameRect->height = screenRect.height - frameRect->y;
    }
    //wxLogMessage( wxString::Format("[TheApp::get_main_window_placement] proposed: x=%d, y=%d, w=%d, h=%d",
    //                frameRect->x, frameRect->y, frameRect->width, frameRect->height));
}

//---------------------------------------------------------------------------------------
wxString TheApp::choose_language(wxWindow *parent)
{
    //Pop up a dialog asking the user to choose the language for the GUI.
    //Only popped up once, the first time the program is run, in case no config.ini
    //file created by installer.

    DlgChooseLanguage dlg(parent, -1, _("LenMus First Run"));
    dlg.CentreOnParent();
    dlg.ShowModal();
    return dlg.get_language();
}

//---------------------------------------------------------------------------------------
SplashFrame* TheApp::create_GUI(int nMilliseconds, bool fFirstTime)
{
    bool fRestarting = false;
	if (m_frame && !fFirstTime)
    {
		SetTopWindow(nullptr);
        m_frame->quit();
        fRestarting = true;
	}

    bool fMaximized = false;
    wxRect wndRect;
    get_main_window_placement(&wndRect, &fMaximized);

    //log
    Paths* pPaths = m_appScope.get_paths();
    wxString path = pPaths->GetConfigPath();
    //wxLogMessage( wxString::Format("[TheApp::create_GUI] preferences: <%s>", path.wx_str()));
    //wxLogMessage( wxString::Format("[TheApp::create_GUI] x=%d, y=%d, w=%d, h=%d",
    //                               wndRect.x, wndRect.y, wndRect.width, wndRect.height));

    m_frame = LENMUS_NEW MainFrame(m_appScope
                            , wxPoint(wndRect.x, wndRect.y)             // origin
                            , wxSize(wndRect.width, wndRect.height) );  // size

    m_frame->create_controls();

    if (fMaximized)
        m_frame->Maximize(true);

        // Create and show the splash window. The splash can have a non-rectangular
        // shape. The color specified as second parameter of SplashFrame creation will
        // be used as the mask color to set the shape

    SplashFrame* pSplash = nullptr;
    if (nMilliseconds > 0 && !fRestarting)
    {
        wxBitmap bitmap = wxArtProvider::GetBitmap("app_splash", wxART_OTHER);
        if (bitmap.Ok() && bitmap.GetHeight() > 100)
	    {
		    //the bitmap exists and it is not the error bitmap (height > 100 pixels). Show it
            wxColour colorTransparent(255, 0, 255);   //cyan mask
            pSplash = LENMUS_NEW SplashFrame(bitmap, colorTransparent,
                lmSPLASH_CENTRE_ON_SCREEN | lmSPLASH_TIMEOUT,
                nMilliseconds, m_frame, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                wxBORDER_SIMPLE|wxSTAY_ON_TOP);
        }
        wxSafeYield();
    }

//#ifndef __WXMAC__
    m_frame->Show(true);
//#endif //ndef __WXMAC__
    SetTopWindow(m_frame);

    return pSplash;
}

//---------------------------------------------------------------------------------------
wxString TheApp::get_installer_language()
{
    wxString sLang = "";
    Paths* pPaths = m_appScope.get_paths();
    wxString sPath = pPaths->GetBinPath();
    wxFileName oFilename(sPath, "config_ini", "txt", wxPATH_NATIVE);
    wxFileInputStream inFile( oFilename.GetFullPath() );
    if (!inFile.Ok())
    {
        //no config.ini file. Return empty string
        return sLang;
    }

    //Get installer language and verify that it is one of the supported languages
    wxTextInputStream inTextFile(inFile);
    sLang = inTextFile.ReadWord();
    wxArrayString cLangCodes;
    wxArrayString  cLangNames;
    GetLanguages(cLangCodes, cLangNames);
    int nNumLangs = cLangNames.GetCount();
    for(int i=0; i < nNumLangs; i++)
    {
        //if found, return it
        if(cLangCodes[i] == sLang) return sLang;
    }

    // not found. Return empty string
    sLang = "";
    return sLang;
}

//---------------------------------------------------------------------------------------
int TheApp::FilterEvent(wxEvent& WXUNUSED(event))
{
//#if (LENMUS_DEBUG_BUILD == 1)
//	if (event.GetEventType()==wxEVT_KEY_DOWN)
//	{
////		if( ((wxKeyEvent&)event).GetKeyCode()==WXK_F1 && m_frame
////			&& m_frame->IsToolBoxVisible())
//		if( ((wxKeyEvent&)event).GetKeyCode()==WXK_F1)
//		{
//            wxMessageBox("[TheApp::FilterEvent] Key pressed!");
////			lmController* pController = m_frame->GetActiveController();
////			if (pController)
////			{
////                lmToolBox* pTB = m_frame->GetActiveToolBox();
////                if (pTB)
////                {
////				    pTB->OnKeyPress((wxKeyEvent&)event);
////				    return true;	        //true: the event had been already processed
////                }
////                else
////	                return -1;		//process the event normally
////			}
////
////			//m_frame->OnHelpF1( (wxKeyEvent&)event );
//			return true;	//true: the event had been already processed
//							//false: the event is not going to be processed at all
//							//-1: process the event normally
//		}
//	}
//#endif

	return -1;		//process the event normally
}

//---------------------------------------------------------------------------------------
void TheApp::OnFatalException()
{
    //called when a crash occurs in this application

//    // open forensic log file
//    wxString sUserId = ::wxGetUserId();
//    wxString sLogFile = g_pPaths->GetLogPath() + sUserId + "_forensic_log.txt";
//    if (g_pLogger->IsValidForensicTarget(sLogFile))
//    {
//        //previous program run terminated with a crash.
//        //inform user and request permision to submit file for bug analysis
//        SendForensicLog(sLogFile, true);    //true: handling a crash
    //}
}

////---------------------------------------------------------------------------------------
//void TheApp::SendForensicLog(wxString& sLogFile, bool fHandlingCrash)
//{
//    //upload the report
//
//    //wxString sURL = "http://localhost/forensic.php/";
//    wxString sURL = "http://www.lenmus.org/forensic.php/";
//    wxString sCurlPath = g_pPaths->GetBinPath();
//    lmForensicLog oFLog(sLogFile, sCurlPath);
//    oFLog.UploadLog(sURL, "file", "", fHandlingCrash);
//    //AWARE: In Windows, after a crash program execution never returns to here because
//    //the main loop to handle events was stopped in previous sentence!
//}


}   //namespace lenmus
