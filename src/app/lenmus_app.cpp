//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2013 LenMus project
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

//wxWidgets
#include <wx/filesys.h>
#include <wx/fs_zip.h>          //to use the zip file system
#include <wx/xrc/xmlres.h>      //to use XRC resorces system
#include <wx/wfstream.h>        //to read config.ini in setting language
#include <wx/txtstrm.h>         //idem
//#include <wx/memory.h>			//to trace memory leaks

//lomse
#include <lomse_logger.h>
using namespace lomse;

//other
#include <iostream>
using namespace std;


// This macro will allow wxWindows to create the application object during program
// execution (it's better than using a static object for many reasons) and also
// declares the accessor function wxGetApp() which will return the reference of
// the right type (i.e. TheApp and not wxApp)
IMPLEMENT_APP(lenmus::TheApp)



namespace lenmus
{

DEFINE_EVENT_TYPE(LM_EVT_CHANGE_LANGUAGE)

#if LOMSE_IS_USING_STD_SHARED_PTRS == 1
    #pragma message("Using std shared pointers")
#else
    #pragma message("Using boost shared pointers")
    #if defined(BOOST_MSVC6_MEMBER_TEMPLATES)
        #pragma message("BOOST_MSVC6_MEMBER_TEMPLATES is defined")
    #endif
#endif

//=======================================================================================
// TheApp implementation
//=======================================================================================

BEGIN_EVENT_TABLE(TheApp, wxApp)
    EVT_COMMAND(wxID_ANY, LM_EVT_CHANGE_LANGUAGE, TheApp::on_change_language)
END_EVENT_TABLE()

//---------------------------------------------------------------------------------------
TheApp::TheApp()
    : wxApp()
//    , m_fUseGui(true)
    , m_pInstanceChecker((wxSingleInstanceChecker*)NULL)
    , m_pLocale(NULL)
    , m_pSplash(NULL)
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

//    //wxApp::OnInit() will invoke OnInitCmdLine() and OnCmdLineParsed()
//    //Therefore, at this point command line is parsed, and all options set up
//    if (!m_fUseGui)
//    {
//        do_application_cleanup();
//        return false;   //stop
//    }

    create_main_frame();
    ::wxBeginBusyCursor();
    wait_and_destroy_splash();
    show_welcome_window();
//    RecoverScoreIfPreviousCrash();
    ::wxEndBusyCursor();

    configure_midi();

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
    wxString name = sAppName + _T("-") + m_appScope.get_version_string()
                    + _T("-") + wxGetUserId() + _T(".lock");
    m_pInstanceChecker = LENMUS_NEW wxSingleInstanceChecker(name);
    if ( m_pInstanceChecker->IsAnotherRunning() )
    {
        wxString msg =  wxString::Format(_("Another instance of %s is already running."),
                                         m_appScope.get_app_name().c_str() );
        //TODO: Linux: Advise user that if there is no another instance running, a lock
        // file is left 'abandoned' in Home dir. The file is named
        //  "Lenmus Phonascus <version>-<username>.lock"
        // and its size is few bytes (around five). Delete this file to fix this problem.
        wxMessageBox(msg, sAppName, wxOK | wxICON_EXCLAMATION );
        return false;
    }

    create_paths_object();
    create_needed_folders_if_dont_exist();
	//All paths, except user configurable ones, are valid from this point

    m_appScope.create_preferences_object();
    //Now preferences object and root path are set up. We can proceed to initialize
    //global variables that depend on user preferences.

    // Load user preferences or default values if first run
    load_user_preferences();

	// AWARE: All paths, even user configurable ones, are valid from this point
	// *************************************************************************


    m_appScope.create_logger();
    set_up_current_language();

    //in Windows, Lomse library is statically linked and default fonts are in LenMus tree
    #if (LENMUS_PLATFORM_WIN32 == 1)
    inform_lomse_about_fonts_path();
    #endif

//    //UploadForensicLogIfExists();
//    //Upload forensic log, if exists
//
//    // open forensic log file
//    sLogFile = g_pPaths->GetLogPath() + sUserId + _T("_forensic_log.txt");
//    wxString sLogScore = g_pPaths->GetLogPath() + sUserId + _T("_score.lmb");
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
//	wxLogMessage(_T("[TheApp::OnInit] Config file: ") + oCfgFile.GetFullPath() );
//
    m_appScope.get_paths()->log_paths();
    m_appScope.open_database();

    // Define handlers for the image types managed by the application
    // BMP handler is by default always defined
    wxImage::AddHandler( LENMUS_NEW wxPNGHandler );
    wxImage::AddHandler( LENMUS_NEW wxJPEGHandler );

    // Set the art provider and get current user selected background bitmap
    wxArtProvider::Push(LENMUS_NEW ArtProvider(m_appScope));
    //m_background = wxArtProvider::GetBitmap(_T("backgrnd"));

    //Include support for zip files
    wxFileSystem::AddHandler(LENMUS_NEW wxZipFSHandler);

    initialize_xrc_resources();
//    CreateDocumentManager();
//    CreateDocumentTemplates();


//#if (LENMUS_DEBUG_BUILD == 1) && (LENMUS_PLATFORM_UNIX == 1)
//    //For Linux in Debug build, use a window to show wxLog messages. This is
//    //the only way I've found to see wxLog messages with Code::Blocks
//    wxLogWindow* pMyLog = LENMUS_NEW wxLogWindow(m_frame, _T("Debug window: wxLogMessages"));
//    wxLog::SetActiveTarget(pMyLog);
//    pMyLog->Flush();
//#endif

    //Seed the random-number generator with current time so that
    //the numbers will be different every time we run.
    srand( (unsigned)time( NULL ) );

    return true;
}

//---------------------------------------------------------------------------------------
void TheApp::create_paths_object()
{
    m_appScope.set_bin_folder( determine_exe_path() );
}

//---------------------------------------------------------------------------------------
wxString TheApp::determine_exe_path()
{
    #if (LENMUS_PLATFORM_WIN32 == 1 || LENMUS_PLATFORM_UNIX == 1)
    {
        // On Linux, Windows and Mac OS X the path to the LenMus program is in argv[0]
        wxString sBinPath = wxPathOnly(argv[0]);
        //wxLogMessage(_T("[TheApp::determine_exe_path] sBinPath='%s'"), sBinPath.c_str());
        //but in console mode fails!
        if (sBinPath.IsEmpty())
            sBinPath = wxGetCwd();
        return sBinPath;
    }
    //#elif defined(__MACOS9__)
    //    // On Mac OS 9, the initial working directory is the one that
    //    // contains the program.
    //    wxString sBinPath = wxGetCwd();
    #else
        #error "Unknown operating system!"
    #endif
}

//---------------------------------------------------------------------------------------
void TheApp::create_needed_folders_if_dont_exist()
{
    //force to create the Paths object. This will create any needed folder
    m_appScope.get_paths();
}

//---------------------------------------------------------------------------------------
void TheApp::inform_lomse_about_fonts_path()
{
    m_appScope.inform_lomse_about_fonts_path();
}

//---------------------------------------------------------------------------------------
void TheApp::load_user_preferences()
{
    wxConfigBase* pPrefs = m_appScope.get_preferences();

    bool value;
    pPrefs->Read(_T("/Options/EnableAnswerSounds"), &value, true);
    m_appScope.enable_answer_sounds(value);

    pPrefs->Read(_T("/Options/AutoNewProblem"), &value, true);
    m_appScope.enable_auto_new_problem(value);

    //pPrefs->Read(_T("/Options/AutoBeam"), &g_fAutoBeam, true);
}

////---------------------------------------------------------------------------------------
//void TheApp::DefineTraceMasks()
//{
//#if (LENMUS_DEBUG_BUILD == 1)
//    //define trace masks to be known by trace system
//	g_pLogger->DefineTraceMask(_T("Cadence"));
//	g_pLogger->DefineTraceMask(_T("Chord"));
//	g_pLogger->DefineTraceMask(_T("lmColStaffObjs::Delete"));
//	g_pLogger->DefineTraceMask(_T("lmColStaffObjs::Insert"));
//    g_pLogger->DefineTraceMask(_T("lmComposer6"));
//    g_pLogger->DefineTraceMask(_T("lmComposer6::AssignNonChordNotes"));
//    g_pLogger->DefineTraceMask(_T("lmComposer6::FunctionToChordNotes"));
//    g_pLogger->DefineTraceMask(_T("lmComposer6::GenerateContour"));
//    g_pLogger->DefineTraceMask(_T("lmComposer6::InstantiateNotes"));
//    g_pLogger->DefineTraceMask(_T("lmComposer6::NearestNoteOnChord"));
//    g_pLogger->DefineTraceMask(_T("Formater4"));
//    g_pLogger->DefineTraceMask(_T("Formatter4.Step1"));
//    g_pLogger->DefineTraceMask(_T("FragmentsTable::GetFirstSegmentDuracion"));
//    g_pLogger->DefineTraceMask(_T("IdfyScalesCtrol"));
//    g_pLogger->DefineTraceMask(_T("IdfyCadencesCtrol"));
//    g_pLogger->DefineTraceMask(_T("Interval"));
//    g_pLogger->DefineTraceMask(_T("lmKeySignature"));
//    g_pLogger->DefineTraceMask(_T("lmLDPParser"));
//    g_pLogger->DefineTraceMask(_T("LDPParser_beams"));
//    g_pLogger->DefineTraceMask(_T("lmMusicXMLParser"));
//    g_pLogger->DefineTraceMask(_T("OnMouseEvent"));
//    g_pLogger->DefineTraceMask(_T("lmScoreAuxCtrol"));
//    g_pLogger->DefineTraceMask(_T("Timing: Score renderization"));
//	g_pLogger->DefineTraceMask(_T("TheoKeySignCtrol"));
//    g_pLogger->DefineTraceMask(_T("Updater"));
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
    wxFileName oXrcFile(sPath, _T("DlgCfgScoreReading"), _T("xrc"), wxPATH_NATIVE);
    wxXmlResource::Get()->Load( oXrcFile.GetFullPath() );

    // Ear Interval exercises: configuration dialog
    oXrcFile = wxFileName(sPath, _T("DlgCfgEarIntervals"), _T("xrc"), wxPATH_NATIVE);
    wxXmlResource::Get()->Load( oXrcFile.GetFullPath() );

    // Chord identification exercises: configuration dialog
    oXrcFile = wxFileName(sPath, _T("DlgCfgIdfyChord"), _T("xrc"), wxPATH_NATIVE);
    wxXmlResource::Get()->Load( oXrcFile.GetFullPath() );

    // Scales identification exercises: configuration dialog
    oXrcFile = wxFileName(sPath, _T("DlgCfgIdfyScale"), _T("xrc"), wxPATH_NATIVE);
    wxXmlResource::Get()->Load( oXrcFile.GetFullPath() );

    // Cedences identification exercises: configuration dialog
    oXrcFile = wxFileName(sPath, _T("DlgCfgIdfyCadence"), _T("xrc"), wxPATH_NATIVE);
    wxXmlResource::Get()->Load( oXrcFile.GetFullPath() );

    // Pattern Editor dialog
    oXrcFile = wxFileName(sPath, _T("DlgPatternEditor"), _T("xrc"), wxPATH_NATIVE);
    wxXmlResource::Get()->Load( oXrcFile.GetFullPath() );

    // Updater dialog: start
    oXrcFile = wxFileName(sPath, _T("UpdaterDlgStart"), _T("xrc"), wxPATH_NATIVE);
    wxXmlResource::Get()->Load( oXrcFile.GetFullPath() );

    // Updater dialog: info
    oXrcFile = wxFileName(sPath, _T("UpdaterDlgInfo"), _T("xrc"), wxPATH_NATIVE);
    wxXmlResource::Get()->Load( oXrcFile.GetFullPath() );

    #if (LENMUS_DEBUG_BUILD == 1)
        // Debug: masks to trace dialog
        oXrcFile = wxFileName(sPath, _T("DlgDebugTrace"), _T("xrc"), wxPATH_NATIVE);
        wxXmlResource::Get()->Load( oXrcFile.GetFullPath() );
    #endif
}

//---------------------------------------------------------------------------------------
void TheApp::create_main_frame()
{
    m_nSplashVisibleMilliseconds = 3000L;   // at least visible for 3 seconds
	m_nSplashStartTime = long( time(NULL) );
    m_pSplash = create_GUI(0L, true);   //m_nSplashVisibleMilliseconds, true /*first time*/);
}

//---------------------------------------------------------------------------------------
void TheApp::wait_and_destroy_splash()
{
	// check if the splash window display time is ellapsed and wait if not

    if (m_pSplash)
    {
	    m_nSplashVisibleMilliseconds -= ((long)time( NULL ) - m_nSplashStartTime);
	    if (m_nSplashVisibleMilliseconds > 0) ::wxMilliSleep( m_nSplashVisibleMilliseconds );
        m_pSplash->AllowDestroy();    // allow to destroy the splash
    }
}

//---------------------------------------------------------------------------------------
void TheApp::show_welcome_window()
{
//#if 0   //while in development, start with nothing displayed
    m_frame->show_welcome_window();
//#endif
}

////---------------------------------------------------------------------------------------
//void TheApp::RecoverScoreIfPreviousCrash()
//{
//    //open any existing score being edited before a crash
//    wxString sUserId = ::wxGetUserId();
//    wxString sLogScore = g_pPaths->GetLogPath() + sUserId + _T("_score.lmb");
//    if (::wxFileExists(sLogScore))
//    {
//        wxString sQuestion =
//            _("An score being edited before a program crash has been detected!");
//        sQuestion += _T("\n\n");
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
    wxString sCheckFreq = pPrefs->Read(_T("/Options/CheckForUpdates/Frequency"), _T("Weekly") );
    if (sCheckFreq != _T("Never"))
    {
        //get date of last sucessful check
        bool fDoCheck = false;
        wxString sLastCheckDate =
            pPrefs->Read(_T("/Options/CheckForUpdates/LastCheck"), _T(""));
        if (sLastCheckDate == _T(""))
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
                LOMSE_LOG_ERROR(str(boost::format(
                    "Error parsing the last check for updates date '%s'.")
                    % sLastCheckDate.c_str()) );
                fDoCheck = true;
            }
            else
            {
                //verify elapsed time
                if (sCheckFreq == _T("Daily"))
                    dsSpan = wxDateSpan::Days(1);
                else if (sCheckFreq == _T("Weekly"))
                    dsSpan = wxDateSpan::Weeks(1);
                else
                    dsSpan = wxDateSpan::Months(1);

                dtNextCheck = dtLastCheck;
                dtNextCheck.Add(dsSpan);
                fDoCheck = (dtNextCheck <= wxDateTime::Now());
            }

            wxString sDoCheck = fDoCheck ? _T("True") : _T("False");
            string msg = to_std_string( wxString::Format(
                _T("[TheApp::OnInit] CheckForUpdates: dtLastCheck='%s', sCheckFreq=%s (%d), dtNextCheck='%s', fDoCheck=%s")
                , dtLastCheck.Format(_T("%x")).c_str()
                , sCheckFreq.c_str(), dsSpan.GetTotalDays()
                , dtNextCheck.Format(_T("%x")).c_str()
                , sDoCheck.c_str() ));
            LOMSE_LOG_INFO(msg);
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
void TheApp::configure_midi()
{
    MidiServer* pMidi = m_appScope.get_midi_server();

    //if MIDI not set, force to run the MIDI wizard
    if (!pMidi->is_configured())
        m_frame->run_midi_wizard();

    //Set up MIDI devices
    pMidi->SetUpCurrentConfig();

    //set sound for metronome
    wxMidiOutDevice* pMidiOut = pMidi->get_out_device();
    if (pMidiOut)
        pMidiOut->ProgramChange(pMidi->MtrChannel(), pMidi->MtrInstr());
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

////---------------------------------------------------------------------------------------
//void TheApp::ParseCommandLine()
//{
//# #include <iostream>
//# using namespace std;
//#
//# int main( int, char **, char ** env)
//# {
//#     for ( ;  *env; env+=2)
//#         cout << "VAR: " << *env << "  Value: " << *(env+1) << endl;
//# }//    wxCmdLineParser parser;
//    OnInitCmdLine(parser);
//    OnCmdLineParsed(parser);
//}
//
////---------------------------------------------------------------------------------------
//void TheApp::OnInitCmdLine(wxCmdLineParser& parser)
//{
//    static const wxCmdLineEntryDesc cmdLineDesc[] =
//    {
//        { wxCMD_LINE_SWITCH, _T("h"), _T("help"), _("Show this help message"),
//            wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
//        { wxCMD_LINE_SWITCH, _T("t"), _T("test"),  _T("Run unit tests"),
//            wxCMD_LINE_VAL_NONE },
//
//        //end of table entry
//        { wxCMD_LINE_NONE, _T(""), _T(""), _T(""), wxCMD_LINE_VAL_NONE }
//    };
//
//    parser.SetDesc(cmdLineDesc);
//    parser.SetSwitchChars(_T("-"));        //use '-' as parameter starter
//}
//
////---------------------------------------------------------------------------------------
//bool TheApp::OnCmdLineParsed(wxCmdLineParser& parser)
//{
//    m_fUseGui = !parser.Found(_T("t"));
//
//    if ( parser.Found(_T("t")) )
//    {
//		#if (LENMUS_DEBUG_BUILD == 1)
//			lmTestRunner oTR((wxWindow*)NULL);
//			oTR.RunTests();
//		#endif
//	}
//
//    //http://forums.wxwidgets.org/viewtopic.php?t=22211
//    //According to this article (see the second Q&A)
//    //
//    //    http://msdn.microsoft.com/en-ca/magazine/cc164023.aspx
//    //
//    //getting the output to go into the same command-line window as the one that
//    //started the program is not possible under Windows, because when executing a
//    //GUI program, the command prompt does not wait for the program to finish
//    //executing, so the command prompt will be screwed up if you try to write
//    //in the same console. The fault is with Windows, not with wxWidgets.
//
//    return true;
//}

//---------------------------------------------------------------------------------------
void TheApp::set_up_current_language()
{
    wxConfigBase* pPrefs = m_appScope.get_preferences();
    wxString lang = pPrefs->Read(_T("/Locale/Language"), _T(""));
    if (lang.IsEmpty())
    {
        //The language is not set. This will only happen the first time
        //the program is run or if lenmus.ini file is deleted

        // try to get installer choosen language and use it if found
        lang = get_installer_language();

        if (lang.IsEmpty())
        {
            // Not found. Pop up a dialog to choose language.
            lang = choose_language(NULL);
        }
        pPrefs->Write(_T("/Locale/Language"), lang);
    }

    // Now that language code is known we can finish Paths initialization
    // and load locale catalogs
    set_up_locale(lang);
}

//---------------------------------------------------------------------------------------
void TheApp::on_change_language(wxCommandEvent& WXUNUSED(event))
{
    set_up_current_language();
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
        sLangName = _T("English");
        LOMSE_LOG_INFO(str(boost::format("Language '%s' not found. Update TheApp.cpp?")
                       % lang.c_str()) );
    }


    // locale object re-initialization
    delete m_pLocale;
    m_pLocale = LENMUS_NEW wxLocale();
    if (!m_pLocale->Init(_T(""), lang, _T(""), false, true))
    {
        wxMessageBox( wxString::Format(_T("Language %s can not be set. ")
            _T("Please, verify that any required language codepages are installed in your system."),
            sLangName.c_str()));
    }
    else
    {
        wxString sPath = pPaths->GetLocaleRootPath();
        m_pLocale->AddCatalogLookupPathPrefix( sPath );
        wxString sCtlg;
        wxString sNil = _T("");
        sCtlg = sNil + _T("lenmus_") + lang;    //m_pLocale->GetName();
        if (!m_pLocale->AddCatalog(sCtlg))
            LOMSE_LOG_INFO(str(boost::format("Failure to load catalog '%s'. Path='%s'")
                            % sCtlg.c_str() % sPath.c_str() ));
        sCtlg = sNil + _T("wxwidgets_") + lang;
        if (!m_pLocale->AddCatalog(sCtlg))
            LOMSE_LOG_INFO(str(boost::format("Failure to load catalog '%s'. Path='%s'")
                            % sCtlg.c_str() % sPath.c_str() ));
        sCtlg = sNil + _T("wxmidi_") + lang;
        if (!m_pLocale->AddCatalog(sCtlg))
            LOMSE_LOG_INFO(str(boost::format("Failure to load catalog '%s'. Path='%s'")
                            % sCtlg.c_str() % sPath.c_str() ));
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
    //wxLogMessage( wxString::Format(_T("[TheApp::get_main_window_placement] default: x=%d, y=%d, w=%d, h=%d"),
    //                defWndRect.x, defWndRect.y, defWndRect.width, defWndRect.height));

    //Read the values from the config file, or use the defaults
    wxConfigBase* pConfig = m_appScope.get_preferences();
    frameRect->SetWidth(pConfig->Read(_T("/MainFrame/Width"), defWndRect.GetWidth()));
    frameRect->SetHeight(pConfig->Read(_T("/MainFrame/Height"), defWndRect.GetHeight()));
    frameRect->SetLeft(pConfig->Read(_T("/MainFrame/Left"), defWndRect.GetLeft() ));
    frameRect->SetTop(pConfig->Read(_T("/MainFrame/Top"), defWndRect.GetTop() ));

    pConfig->Read(_T("/MainFrame/Maximized"), fMaximized);

        //--- Make sure that the Window will be completely visible -------------

    //Get the size of the screen
    wxRect screenRect;
    wxClientDisplayRect(&screenRect.x, &screenRect.y,
                        &screenRect.width, &screenRect.height);
    //wxLogMessage( wxString::Format(_T("[TheApp::get_main_window_placement] screen: x=%d, y=%d, w=%d, h=%d"),
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
    //wxLogMessage( wxString::Format(_T("[TheApp::get_main_window_placement] proposed: x=%d, y=%d, w=%d, h=%d"),
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
		SetTopWindow(NULL);
        m_frame->quit();
        fRestarting = true;
	}

    bool fMaximized = false;
    wxRect wndRect;
    get_main_window_placement(&wndRect, &fMaximized);

    //log
    Paths* pPaths = m_appScope.get_paths();
    wxString path = pPaths->GetConfigPath();
    //wxLogMessage( wxString::Format(_T("[TheApp::create_GUI] preferences: <%s>"), path.c_str()));
    //wxLogMessage( wxString::Format(_T("[TheApp::create_GUI] x=%d, y=%d, w=%d, h=%d"),
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

    SplashFrame* pSplash = NULL;
    if (nMilliseconds > 0 && !fRestarting)
    {
        wxBitmap bitmap = wxArtProvider::GetBitmap(_T("app_splash"), wxART_OTHER);
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

    m_frame->Show(true);
    SetTopWindow(m_frame);

    return pSplash;
}

//---------------------------------------------------------------------------------------
wxString TheApp::get_installer_language()
{
    wxString sLang = _T("");
    Paths* pPaths = m_appScope.get_paths();
    wxString sPath = pPaths->GetBinPath();
    wxFileName oFilename(sPath, _T("config_ini"), _T("txt"), wxPATH_NATIVE);
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
    sLang = _T("");
    return sLang;
}

////---------------------------------------------------------------------------------------
//int TheApp::FilterEvent(wxEvent& event)
//{
//	if (event.GetEventType()==wxEVT_KEY_DOWN)
//	{
//		if( ((wxKeyEvent&)event).GetKeyCode()==WXK_F1 && m_frame
//			&& m_frame->IsToolBoxVisible())
//		{
//			lmController* pController = m_frame->GetActiveController();
//			if (pController)
//			{
//                lmToolBox* pTB = m_frame->GetActiveToolBox();
//                if (pTB)
//                {
//				    pTB->OnKeyPress((wxKeyEvent&)event);
//				    return true;	        //true: the event had been already processed
//                }
//                else
//	                return -1;		//process the event normally
//			}
//
//			//m_frame->OnHelpF1( (wxKeyEvent&)event );
//			//return true;	//true: the event had been already processed
//			//				//false: the event is not going to be processed at all
//		}
//	}
//
//	return -1;		//process the event normally
//}

//---------------------------------------------------------------------------------------
void TheApp::OnFatalException()
{
    //called when a crash occurs in this application

//    // open forensic log file
//    wxString sUserId = ::wxGetUserId();
//    wxString sLogFile = g_pPaths->GetLogPath() + sUserId + _T("_forensic_log.txt");
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
//    //wxString sURL = _T("http://localhost/forensic.php/");
//    wxString sURL = _T("http://www.lenmus.org/forensic.php/");
//    wxString sCurlPath = g_pPaths->GetBinPath();
//    lmForensicLog oFLog(sLogFile, sCurlPath);
//    oFLog.UploadLog(sURL, _T("file"), _T(""), fHandlingCrash);
//    //AWARE: In Windows, after a crash program execution never returns to here because
//    //the main loop to handle events was stopped in previous sentence!
//}


}   //namespace lenmus
