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

// lmTheApp class represents the application itself. Takes care of
// implementing the main event loop and to handle events not handled by other objects in
// the application. The two main visible responsibilities are:
//    - To set and get application-wide properties.
//    - To initiate application processing via wxApp::OnInit()


#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "TheApp.h"
#endif

// For compilers that support precompilation
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <wx/wx.h>
#include <wx/frame.h>
#include <wx/image.h>
#include <wx/xrc/xmlres.h>      // use the xrc resource system
#include <wx/splash.h>
#include <wx/datetime.h>
#include <wx/txtstrm.h>
#include <wx/cmdline.h>
#include <wx/stdpaths.h>		//to get executable path
#include <wx/memory.h>			//to trace memory leaks

#include <wx/arrstr.h>          //AWARE: Required by wxsqlite3. In Linux GCC complains about wxArrayString not defined in wxsqlite3.h
#include <wx/wxsqlite3.h>       //to initialize wxSQLite3 DB

// verify wxWidgets setup
#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error "You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!"
#endif

#if !wxUSE_MDI_ARCHITECTURE
#error "You must set wxUSE_MDI_ARCHITECTURE to 1 in setup.h!"
#endif

#if !wxUSE_MENUS
#error "You must set wxUSE_MENUS to 1 in setup.h!"
#endif


#if defined( __WXMSW__ ) && defined( _DEBUG )
// for debugging: Detecting and isolating memory leaks with Visual C++
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
//#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
//#else
//#define DEBUG_NEW new
#endif

// CONFIG: Under Windows, change this to 1 to use wxGenericDragImage
#define wxUSE_GENERIC_DRAGIMAGE 1


#include "TheApp.h"
#include "MainFrame.h"
#include "ScoreDoc.h"
#include "ScoreView.h"
#include "AboutDialog.h"
#include "DlgUploadLog.h"               //upload forensic log
#include "LangChoiceDlg.h"
#include "ArtProvider.h"
#include "SplashFrame.h"
#include "toolbox/ToolsBox.h"
#include "../mdi/DocViewMDI.h"         //lmDocManager
#include "../widgets/MsgBox.h"

#ifdef __WXDEBUG__
	#include "../tests/TestRunner.h"            //to run tests
#endif

// to save config information into a file
#include "wx/confbase.h"
#include "wx/fileconf.h"
#include "wx/filename.h"

#include "wx/fs_zip.h"                  //to use the zip file system

#include "Preferences.h"                //access to user preferences
#include "Processor.h"                  //to delete the only instance
#include "../sound/MidiManager.h"       //access to Midi configuration
#include "../sound/WaveManager.h"       //access to Wave sound manager
#include "Logger.h"                     //access to error's logger
#include "../options/Languages.h"       //to check config_ini.txt stored language
#include "../graphic/AggDrawer.h"       //to delete lmMusicFontManager singleton
#include "../auxmusic/Chord.h"          //to delete lmChordsDB singleton

//access to global objects
#include "../globals/Paths.h"
#include "../globals/Colors.h"


//-------------------------------------------------------------------------------------------
// global variables
//-------------------------------------------------------------------------------------------
lmMainFrame *g_pMainFrame = (lmMainFrame*) NULL;

#ifdef __WXDEBUG__
bool g_fReleaseVersion = false;     // to enable/disable debug features
#else
bool g_fReleaseVersion = true;      // to enable/disable debug features
#endif

bool g_fReleaseBehaviour = false;   // This flag is only used to force release behaviour when
                                    // in debug mode, and only for some functions (the ones using this flag)

bool g_fShowDebugLinks = false;     // force to add aditional debug ctrols in exercises.
                                    // only operative in debug mode.

bool g_fShowDirtyObjects = false;   // for testing purposes. Forces to render dirty objects in red

bool g_fBorderOnScores = false;     //to facilitate the height adjustement of
                                    //scores included on eMusicBooks.


// Global print data, to remember settings during the session
wxPrintData* g_pPrintData = (wxPrintData*) NULL;

// Global page setup data
wxPageSetupData* g_pPaperSetupData = (wxPageSetupData*) NULL;

// Paths names
lmPaths* g_pPaths = (lmPaths*) NULL;

// Colors
lmColors* g_pColors = (lmColors*)NULL;

// Error's logger
lmLogger* g_pLogger = (lmLogger*)NULL;

// Conversion factors
double g_rScreenDPI = 96.0;
double g_rPixelsPerLU = 1.0;

//the database
wxSQLite3Database* g_pDB;


// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. lmTheApp and
// not wxApp)
IMPLEMENT_APP(lmTheApp)

// ============================================================================
// implementation
// ============================================================================

DEFINE_EVENT_TYPE(lmEVT_CHANGE_LANGUAGE)


// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(lmTheApp, wxApp)
    EVT_COMMAND(wxID_ANY, lmEVT_CHANGE_LANGUAGE, lmTheApp::OnChangeLanguage)
END_EVENT_TABLE()


lmTheApp::lmTheApp()
    : wxApp()
    , m_fUseGui(true)
    , m_pInstanceChecker((wxSingleInstanceChecker*)NULL)
    , m_pDocManager((lmDocManager*)NULL)
    , m_pLocale((wxLocale*) NULL)
    , m_pSplash((lmSplashFrame*)NULL)
{
    #ifndef __WXDEBUG__
        //in release version we will deal with crashes.
        //tell base class to call our OnFatalException()
        wxHandleFatalExceptions();
    #endif
}

bool lmTheApp::OnInit(void)
{
    //A wxWidgets application does not have a main procedure; the equivalent is 
    //this: the OnInit member
    //The method builds the objects necessary to run the application (it is like 
    //the 'constructor'. It must return true to continue, and in this case, control
    //is transferred to OnRun(). Otherwise the application is terminated witout further
    //processing

    DoApplicationSetUp();

    if (!wxApp::OnInit())
    {
        DoApplicationCleanUp();
        return false;
    }

    //wxApp::OnInit() will invoke OnInitCmdLine() and OnCmdLineParsed()
    //Therefore, at this point command line is parsed, and all options set up
    if (!m_fUseGui)
    {
        DoApplicationCleanUp();
        return false;   //stop
    }

    CreateMainFrame();
    ::wxBeginBusyCursor();
    WaitAndDestroySplash();
    OpenWelcomeWindow();
    RecoverScoreIfPreviousCrash();
    ::wxEndBusyCursor();
    CheckForUpdates();
    #ifdef __WXDEBUG__
	    g_pMainFrame->RunUnitTests();
    #endif

    return true;
}

int lmTheApp::OnRun()
{
    //Events processing main loop. It terminates when the main frame is destroyed.
    //Return code is set, with the one returned here, and control is passed to
    //OnExit() method (equivalent to a 'destructor')

    int exitcode = wxApp::OnRun();
    //wxTheClipboard->Flush();
    return exitcode;
}

bool lmTheApp::DoApplicationSetUp()
{
    //Create the necessary objects and set up to run the application

    // set information about this application
    const wxString sAppName = _T("LenMus");
    SetVendorName(_T("LenMus"));
    SetAppName(sAppName);

    // verify that this is the only instance running
    wxString name = sAppName + _T("-") + GetVersionNumber() + _T("-") + wxGetUserId();
    m_pInstanceChecker = new wxSingleInstanceChecker(name);
    if ( m_pInstanceChecker->IsAnotherRunning() )
    {
        wxMessageBox(_("Another instance of LenMus is already running."),
                     sAppName, wxOK | wxICON_EXCLAMATION );
        return false;
    }

    CreatePathsObject();
	// AWARE: All paths, except user configurable ones, are valid from this point


        //
        // Prepare preferences object
        //

    // set path and name for config file
    wxFileName oCfgFile(g_pPaths->GetConfigPath(), _T("lenmus"), _T("ini") );
    wxFileConfig *pConfig = new wxFileConfig(_T("lenmus"), _T("LenMus"), oCfgFile.GetFullPath(),
            _T("lenmus"), wxCONFIG_USE_LOCAL_FILE );
    wxConfigBase::Set(pConfig);

    // force writing back the default values just in case
    // they're not present in the config file
    pConfig->SetRecordDefaults();

        //
        // Now preferences object and root path are set up. We can proceed to initialize
        // global variables that depend on user preferences.
        //

    // Load user preferences or default values if first run
    InitPreferences();
    g_pPaths->LoadUserPreferences();


	// AWARE: All paths, even user configurable ones, are valid from this point
	// *************************************************************************


    // colors
    g_pColors = new lmColors();

        //
        // Error reporting and trace/dump logs
        //

    g_pLogger = new lmLogger();

	// For debugging: send log messages to a file
    wxString sUserId = ::wxGetUserId();
    wxString sLogFile = g_pPaths->GetLogPath() + sUserId + _T("_Debug_log.txt");
	wxLog *logger = new wxLogStderr( wxFopen(sLogFile.c_str(), _T("w")) );
    new wxLogChain(logger);

    // open data log file and re-direct all loging there
    sLogFile = g_pPaths->GetLogPath() + sUserId + _T("_DataError_log.txt");
    g_pLogger->SetDataErrorTarget(sLogFile);

    SetUpCurrentLanguage();

    //UploadForensicLogIfExists();
    //Upload forensic log, if exists

    // open forensic log file
    sLogFile = g_pPaths->GetLogPath() + sUserId + _T("_forensic_log.txt");
    wxString sLogScore = g_pPaths->GetLogPath() + sUserId + _T("_score.lmb");
    if (g_pLogger->IsValidForensicTarget(sLogFile))
    {
        //previous program run terminated with a crash and forensic log was not
        //uploaded (probably we were in debug mode and program execution was
        //cancelled. Inform user and request permision to submit file for bug
        //analysis
        SendForensicLog(sLogFile, false);       //false: not handling a crash
    }
    g_pLogger->SetForensicTarget(sLogFile, sLogScore);

    DefineTraceMasks();
	// AWARE: Log/debug methods are available from this point

	wxLogMessage(_T("[lmTheApp::OnInit] Config file: ") + oCfgFile.GetFullPath() );

    OpenDataBase();

    // Compute some screen conversion factors
	FindOutScreenDPI();

    // Define handlers for the image types managed by the application
    // BMP handler is by default always defined
    wxImage::AddHandler( new wxPNGHandler );
    wxImage::AddHandler( new wxJPEGHandler );

    // Set the art provider and get current user selected background bitmap
    wxArtProvider::Push(new lmArtProvider());
    //m_background = wxArtProvider::GetBitmap(_T("backgrnd"));

    //Include support for zip files
    wxFileSystem::AddHandler(new wxZipFSHandler);

    InitializeXrcResources();
    CreateDocumentManager();
    CreateDocumentTemplates();


#if defined(__WXDEBUG__) && defined(__WXGTK__)
    //For Linux in Debug build, use a window to show wxLog messages. This is
    //the only way I found to see wxLog messages with Code::Blocks
    wxLogWindow* pMyLog = new wxLogWindow(g_pMainFrame, _T("Debug window: wxLogMessages"));
    wxLog::SetActiveTarget(pMyLog);
	wxLogMessage(_T("[lmTheApp::OnInit] Config file: ") + oCfgFile.GetFullPath() );
    pMyLog->Flush();
#endif

    // create global data structures for printer settings
    g_pPrintData = new wxPrintData;
    g_pPaperSetupData = new wxPageSetupDialogData;

    //Seed the random-number generator with current time so that
    //the numbers will be different every time we run.
    srand( (unsigned)time( NULL ) );

    SetUpMidi();

    return true;
}

void lmTheApp::CreatePathsObject()
{
    // Get program directory
    #if defined(__WXGTK__) || defined(__WXMSW__) || defined(__MACOSX__)
        // On Linux, Windows and Mac OS X the path to the LenMus program is in argv[0]
        wxString sHomeDir = wxPathOnly(argv[0]);
        //but in console mode fails!
        if (sHomeDir.IsEmpty())
            sHomeDir = wxGetCwd();
    #elif defined(__MACOS9__)
        // On Mac OS 9, the initial working directory is the one that
        // contains the program.
        wxString sHomeDir = wxGetCwd();
    #else
        #error "Unknown operating system!"
    #endif

    g_pPaths = new lmPaths(sHomeDir);
}

void lmTheApp::OpenDataBase()
{
    //initialize DataBase support and open database
    try
    {
        //wxSQLite3Database::InitializeSQLite();
        g_pDB = new wxSQLite3Database();
        wxFileName oDBFile(g_pPaths->GetConfigPath(), _T("lenmus"), _T("db") );
        wxLogMessage(_T("[lmTheApp::OnInit] SQLite3 Version: %s. DB file: '%s'"),
                     g_pDB->GetVersion().c_str(), oDBFile.GetFullPath().c_str() );
        g_pDB->Open(oDBFile.GetFullPath());
    }
    catch (wxSQLite3Exception& e)
    {
        wxLogMessage(_T("Error code: %d, Message: '%s'"),
                    e.GetErrorCode(), e.GetMessage().c_str() );
    }
}

void lmTheApp::DefineTraceMasks()
{
#ifdef __WXDEBUG__
    //define trace masks to be known by trace system
	g_pLogger->DefineTraceMask(_T("lmCadence"));
	g_pLogger->DefineTraceMask(_T("lmChord"));
	g_pLogger->DefineTraceMask(_T("lmColStaffObjs::Delete"));
	g_pLogger->DefineTraceMask(_T("lmColStaffObjs::Insert"));
    g_pLogger->DefineTraceMask(_T("lmComposer6"));
    g_pLogger->DefineTraceMask(_T("lmComposer6::AssignNonChordNotes"));
    g_pLogger->DefineTraceMask(_T("lmComposer6::FunctionToChordNotes"));
    g_pLogger->DefineTraceMask(_T("lmComposer6::GenerateContour"));
    g_pLogger->DefineTraceMask(_T("lmComposer6::InstantiateNotes"));
    g_pLogger->DefineTraceMask(_T("lmComposer6::NearestNoteOnChord"));
    g_pLogger->DefineTraceMask(_T("Formater4"));
    g_pLogger->DefineTraceMask(_T("Formatter4.Step1"));
    g_pLogger->DefineTraceMask(_T("lmFragmentsTable::GetFirstSegmentDuracion"));
    g_pLogger->DefineTraceMask(_T("lmIdfyScalesCtrol"));
    g_pLogger->DefineTraceMask(_T("lmIdfyCadencesCtrol"));
    g_pLogger->DefineTraceMask(_T("lmInterval"));
    g_pLogger->DefineTraceMask(_T("lmKeySignature"));
    g_pLogger->DefineTraceMask(_T("lmLDPParser"));
    g_pLogger->DefineTraceMask(_T("LDPParser_beams"));
    g_pLogger->DefineTraceMask(_T("lmMusicXMLParser"));
    g_pLogger->DefineTraceMask(_T("OnMouseEvent"));
    g_pLogger->DefineTraceMask(_T("lmScoreAuxCtrol"));
	g_pLogger->DefineTraceMask(_T("lmScoreCtrolParams"));
    g_pLogger->DefineTraceMask(_T("Timing: Score renderization"));
	g_pLogger->DefineTraceMask(_T("lmTheoKeySignCtrol"));
    g_pLogger->DefineTraceMask(_T("lmUpdater"));
#endif
}

void lmTheApp::CreateDocumentManager()
{
    m_pDocManager = new lmDocManager();
    m_pDocManager->LoadRecentFiles(g_pPrefs, _T("/RecentFiles/"));

    //if no recent files, load some samples
    if (m_pDocManager->NumFilesInHistory() == 0)
    {
        wxString sPath = g_pPaths->GetSamplesPath();
        wxFileName oFile1(sPath, _T("greensleeves_v15.lms"));
        wxFileName oFile2(sPath, _T("chopin_prelude20_v15.lms"));
        wxFileName oFile3(sPath, _T("beethoven_moonlight_sonata_v15.lms"));
        //wxLogMessage(_T("[lmMainFrame::LoadRecentFiles] sPath='%s', sFile1='%s'"),
        //             sPath.c_str(), oFile1.GetFullPath().c_str() );
        m_pDocManager->AddToHistory(oFile1.GetFullPath());
        m_pDocManager->AddToHistory(oFile2.GetFullPath());
        m_pDocManager->AddToHistory(oFile3.GetFullPath());
    }

    // Sets the directory to be displayed to the user when opening a score.
    m_pDocManager->SetLastDirectory(g_pPaths->GetScoresPath());
}

void lmTheApp::CreateDocumentTemplates()
{
    // Create a template relating score documents to their views
    (void) new wxDocTemplate(m_pDocManager, _T("LenMus score"), _T("*.lms"), _T(""), _T("lms"), _T("LenMus score"), _T("lmScore View"),
          CLASSINFO(lmDocument), CLASSINFO(lmScoreView));
    (void) new wxDocTemplate(m_pDocManager, _T("MusicXML score"), _T("*.xml;*.*"), _T(""), _T("xml"), _T("MusicXML score"), _T("lmScore View"),
          CLASSINFO(lmDocument), CLASSINFO(lmScoreView), wxTEMPLATE_INVISIBLE );
    //#ifdef __WXMAC__
    //    wxFileName::MacRegisterDefaultTypeAndCreator( wxT("lms") , 'WXMB' , 'WXMA' ) ;
    //#endif
}

void lmTheApp::InitializeXrcResources()
{
    // Initialize all the XRC handlers.
    wxXmlResource::Get()->InitAllHandlers();

    // get path for xrc resources
    wxString sPath = g_pPaths->GetXrcPath();

    // Load all of the XRC files that will be used. You can put everything
    // into one giant XRC file if you wanted, but then they become more
    // difficult to manage, and harder to reuse in later projects.

    // The score generation settings dialog
    wxFileName oXrcFile(sPath, _T("DlgCfgScoreReading"), _T("xrc"), wxPATH_NATIVE);
    wxXmlResource::Get()->Load( oXrcFile.GetFullPath() );

    // Configuration options: toolbars panel
    oXrcFile = wxFileName(sPath, _T("ToolbarsOptPanel"), _T("xrc"), wxPATH_NATIVE);
    wxXmlResource::Get()->Load( oXrcFile.GetFullPath() );

    // Configuration options: languages panel
    oXrcFile = wxFileName(sPath, _T("LangOptionsPanel"), _T("xrc"), wxPATH_NATIVE);
    wxXmlResource::Get()->Load( oXrcFile.GetFullPath() );

    // Configuration options: Internet options panel
    oXrcFile = wxFileName(sPath, _T("InternetOptPanel"), _T("xrc"), wxPATH_NATIVE);
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

    #ifdef __WXDEBUG__
        // Debug: masks to trace dialog
        oXrcFile = wxFileName(sPath, _T("DlgDebugTrace"), _T("xrc"), wxPATH_NATIVE);
        wxXmlResource::Get()->Load( oXrcFile.GetFullPath() );
    #endif
}

void lmTheApp::CreateMainFrame()
{
    // Create the GUI: the main frame window

    m_nSplashVisibleMilliseconds = 3000L;   // at least visible for 3 seconds
	m_nSplashStartTime = (long) time( NULL );
    m_pSplash = RecreateGUI(m_nSplashVisibleMilliseconds);
}

void lmTheApp::WaitAndDestroySplash()
{
	// check if the splash window display time is ellapsed and wait if not

    if (m_pSplash)
    {
	    m_nSplashVisibleMilliseconds -= ((long)time( NULL ) - m_nSplashStartTime);
	    if (m_nSplashVisibleMilliseconds > 0) ::wxMilliSleep( m_nSplashVisibleMilliseconds );
        m_pSplash->AllowDestroy();    // allow to destroy the splash
    }
}

void lmTheApp::OpenWelcomeWindow()
{
    //force to show welcome window

    #if !defined(__WXDEBUG__)       //in debug version, start with nothing displayed
        g_pMainFrame->ShowWelcomeWindow();
    #endif
}

void lmTheApp::RecoverScoreIfPreviousCrash()
{
    //open any existing score being edited before a crash
    wxString sUserId = ::wxGetUserId();
    wxString sLogScore = g_pPaths->GetLogPath() + sUserId + _T("_score.lmb");
    if (::wxFileExists(sLogScore))
    {
        wxString sQuestion =
            _("An score being edited before a program crash has been detected!");
        sQuestion += _T("\n\n");
        sQuestion += _("Should the program attempt to recover it?");
        lmQuestionBox oQB(sQuestion, 2,     //msge, num buttons,
            //labels (2 per button: button text + explanation)
            _("Yes"), _("Yes, try to recover the score"),
            _("No"), _("No, forget about that score")
        );
        int nAnswer = oQB.ShowModal();

		if (nAnswer == 0)       //'Yes' button
            g_pMainFrame->OpenScore(sLogScore, true);    //true: as new file
    }
}

void lmTheApp::CheckForUpdates()
{
    //check for updates if this option is set up. Default: do check
    wxString sCheckFreq = g_pPrefs->Read(_T("/Options/CheckForUpdates/Frequency"), _T("Weekly") );
    if (sCheckFreq != _T("Never")) {
        //get date of last sucessful check
        bool fDoCheck = false;
        wxString sLastCheckDate =
            g_pPrefs->Read(_T("/Options/CheckForUpdates/LastCheck"), _T(""));
        if (sLastCheckDate == _T("")) {
            fDoCheck = true;
        }
        else {
            wxDateTime dtLastCheck, dtNextCheck;
            wxDateSpan dsSpan;
            const wxChar *p = dtLastCheck.ParseDate(sLastCheckDate);
            if ( !p ) {
                wxLogMessage(_T("[TheApp::OnInit] Error parsing the last check for updates date '%s'.\n"), sLastCheckDate.c_str());
                fDoCheck = true;
            }
            else {
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
            wxLogMessage(_T("[TheApp::OnInit] CheckForUpdates: dtLastCheck='%s', sCheckFreq=%s (%d), dtNextCheck='%s', fDoCheck=%s"),
                    dtLastCheck.Format(_T("%x")).c_str(),
                    sCheckFreq.c_str(), dsSpan.GetTotalDays(),
                    dtNextCheck.Format(_T("%x")).c_str(),
                    sDoCheck.c_str() );

        }

        // if time for another check, do it
        if (fDoCheck) {
            g_pMainFrame->SilentlyCheckForUpdates(true);
            wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, lmMENU_CheckForUpdates);
            wxPostEvent(g_pMainFrame, event);
        }
    }
}

void lmTheApp::SetUpMidi()
{
    g_pMidi = lmMidiManager::GetInstance();

    //if MIDI not set, force to run the MIDI wizard
    if (!g_pMidi->IsConfigured())
        g_pMainFrame->DoRunMidiWizard();

    //Set up MIDI devices
    g_pMidi->SetUpCurrentConfig();

    //set sound for metronome
    if (g_pMidiOut)
        g_pMidiOut->ProgramChange(g_pMidi->MtrChannel(), g_pMidi->MtrInstr());
}

void lmTheApp::DoApplicationCleanUp()
{
    // delete all objects used by lmTheApp

    // the Midi configuration and related objects
    if (g_pMidi)
        delete g_pMidi;

    // the wave sound manager object
    lmWaveManager::Destroy();

    //delete the docManager
    delete m_pDocManager;

    //database
    g_pDB->Close();
    delete g_pDB;
    wxSQLite3Database::ShutdownSQLite();

    //remove forensic log and delete logger
    g_pLogger->DeleteForensicTarget();
    delete g_pLogger;

    // the printer setup data
    delete g_pPrintData;
    delete g_pPaperSetupData;

    //single instance checker
    if (m_pInstanceChecker)
        delete m_pInstanceChecker;

    //other objects
    delete g_pPaths;                            //path names
    delete g_pColors;                           //colors object
    delete wxConfigBase::Set((wxConfigBase *) NULL);    //the wxConfig object
    lmPgmOptions::DeleteInstance();             //the program options object
    delete m_pLocale;                           //locale object
    lmMusicFontManager::DeleteInstance();       //music font manager
    lmProcessorMngr::DeleteInstance();          //Processor manager
    lmChordsDB::DeleteInstance();               //Chords Database
}

void lmTheApp::ParseCommandLine()
{
    wxCmdLineParser parser;
    OnInitCmdLine(parser);
    OnCmdLineParsed(parser);
}

void lmTheApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    static const wxCmdLineEntryDesc cmdLineDesc[] =
    {
        { wxCMD_LINE_SWITCH, _T("h"), _T("help"), _("Show this help message"),
            wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
        { wxCMD_LINE_SWITCH, _T("t"), _T("test"),  _T("Run unit tests"),
            wxCMD_LINE_VAL_NONE },

        { wxCMD_LINE_NONE }
    };

    parser.SetDesc(cmdLineDesc);
    parser.SetSwitchChars(_T("-"));        //use '-' as parameter starter
}
 
bool lmTheApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    m_fUseGui = !parser.Found(_T("t"));
 
    if ( parser.Found(_T("t")) )
    {
		#ifdef __WXDEBUG__
			lmTestRunner oTR((wxWindow*)NULL);
			oTR.RunTests();
		#endif
	}

    //http://forums.wxwidgets.org/viewtopic.php?t=22211
    //According to this article (see the second Q&A)
    //
    //    http://msdn.microsoft.com/en-ca/magazine/cc164023.aspx
    //
    //getting the output to go into the same command-line window as the one that
    //started the program is not possible under Windows, because when executing a
    //GUI program, the command prompt does not wait for the program to finish
    //executing, so the command prompt will be screwed up if you try to write
    //in the same console. The fault is with Windows, not with wxWidgets. 

    return true;
}

void lmTheApp::SetUpCurrentLanguage()
{
    wxString lang = g_pPrefs->Read(_T("/Locale/Language"), _T(""));
    if (lang.IsEmpty())
    {
        //The language is not set. This will only happen the first time
        //the program is run or if lenmus.ini file is deleted

        // try to get installer choosen language and use it if found
        lang = GetInstallerLanguage();

        if (lang.IsEmpty())
        {
            // Not found. Pop up a dialog to choose language.
            lang = ChooseLanguage(NULL);
        }
        g_pPrefs->Write(_T("/Locale/Language"), lang);
    }

    // Now that language code is known we can finish lmPaths initialization
    // and load locale catalogs
    SetUpLocale(lang);
}

void lmTheApp::OnChangeLanguage(wxCommandEvent& WXUNUSED(event))
{
    SetUpCurrentLanguage();
    RecreateGUI(0);   //0 = No splash
    OpenWelcomeWindow();
}

void lmTheApp::SetUpLocale(wxString lang)
{
    // lmPaths re-initialization
    g_pPaths->SetLanguageCode(lang);

    //get wxLanguage code and name
    const wxLanguageInfo* pInfo = wxLocale::FindLanguageInfo(lang);
    int nLang;
    wxString sLangName;
    if (pInfo) {
        nLang = pInfo->Language;
        sLangName = pInfo->Description;
    }
    else {
        nLang = wxLANGUAGE_ENGLISH;
        sLangName = _T("English");
        wxLogMessage(_T("[lmTheApp::SetUpLocale] Language '%s' not found. Update lmApp.cpp?"), lang.c_str());
    }


    // locale object re-initialization
    if (m_pLocale) delete m_pLocale;
    m_pLocale = new wxLocale();
    if (!m_pLocale->Init(_T(""), lang, _T(""), false, true)) {
    //if (!m_pLocale->Init( nLang, wxLOCALE_CONV_ENCODING )) {
        wxMessageBox( wxString::Format(_T("Language %s can not be set. ")
            _T("Please, verify that any required language codepages are installed in your system."),
            sLangName.c_str()));
    }
    else {
        wxString sPath = g_pPaths->GetLocaleRootPath();
        m_pLocale->AddCatalogLookupPathPrefix( sPath );
        wxString sCtlg;
        wxString sNil = _T("");
        sCtlg = sNil + _T("lenmus_") + lang;    //m_pLocale->GetName();
        if (!m_pLocale->AddCatalog(sCtlg))
            wxLogMessage(_T("[lmTheApp::SetUpLocale] Failure to load catalog '%s'. Path='%s'"),
                sCtlg.c_str(), sPath.c_str());
        sCtlg = sNil + _T("wxwidgets_") + lang;
        if (!m_pLocale->AddCatalog(sCtlg))
            wxLogMessage(_T("[lmTheApp::SetUpLocale] Failure to load catalog '%s'. Path='%s'"),
                sCtlg.c_str(), sPath.c_str());
        sCtlg = sNil + _T("wxmidi_") + lang;
        if (!m_pLocale->AddCatalog(sCtlg))
            wxLogMessage(_T("[lmTheApp::SetUpLocale] Failure to load catalog '%s'. Path='%s'"),
                sCtlg.c_str(), sPath.c_str());
    }
}

int lmTheApp::OnExit(void)
{
    g_pPaths->SaveUserPreferences();
    DoApplicationCleanUp();

	return 0;
}

void lmTheApp::GetDefaultMainWindowRect(wxRect *defRect)
{
    // Set up the default size and position for the main window

    //Get the size of the screen
    wxRect screenRect;
    wxClientDisplayRect(&screenRect.x, &screenRect.y, &screenRect.width, &screenRect.height);

   //the point that a new window should open at.
   defRect->x = 10;
   defRect->y = 10;

   defRect->width = (int)((double)screenRect.width * 0.95);
   defRect->height = (int)((double)screenRect.height * 0.95);

   //These conditional values assist in improving placement and size
   //of new windows on different platforms.
    #ifdef __WXMAC__
        defRect->y += 50;
    #endif

}

// Calculate where to place the main window
void lmTheApp::GetMainWindowPlacement(wxRect *frameRect, bool *fMaximized)
{
    *fMaximized = false;        // default: not maximized

    // set the default window size
    wxRect defWndRect;
    GetDefaultMainWindowRect(&defWndRect);

    //Read the values from the config file, or use the defaults
    wxConfigBase *pConfig = wxConfigBase::Get();
    frameRect->SetWidth(pConfig->Read(_T("/MainFrame/Width"), defWndRect.GetWidth()));
    frameRect->SetHeight(pConfig->Read(_T("/MainFrame/Height"), defWndRect.GetHeight()));
    frameRect->SetLeft(pConfig->Read(_T("/MainFrame/Left"), defWndRect.GetLeft() ));
    frameRect->SetTop(pConfig->Read(_T("/MainFrame/Top"), defWndRect.GetTop() ));

    pConfig->Read(_T("/MainFrame/Maximized"), fMaximized);

        //--- Make sure that the Window will be completely visible -------------

    //Get the size of the screen
    wxRect screenRect;
    wxClientDisplayRect(&screenRect.x, &screenRect.y, &screenRect.width, &screenRect.height);

    //If we have hit te bottom of the screen restore default position on the screen
    if( (frameRect->y + frameRect->height > screenRect.y + screenRect.height) ) {
      frameRect->x = defWndRect.x;
      frameRect->y = defWndRect.y;
   }

   //if we have hit the right side of the screen restore default position
    if( (frameRect->x+frameRect->width > screenRect.x+screenRect.width) )  {
      frameRect->x = defWndRect.x;
      frameRect->y = defWndRect.y;
   }

   //Next check if the screen is too small for the default width and height
   if( (frameRect->x+frameRect->width > screenRect.x+screenRect.width) ||
       (frameRect->y+frameRect->height > screenRect.y+screenRect.height) )
   {
      //Resize the main window to fit in the screen
      frameRect->width = screenRect.width-frameRect->x;
      frameRect->height = screenRect.height-frameRect->y;
   }

}

wxString lmTheApp::ChooseLanguage(wxWindow *parent)
{
    //Pop up a language asking the user to choose a language for the user interface.
    //Generally only popped up once, the first time the program is run.

    lmLangChoiceDlg dlog(parent, -1, _("LenMus First Run"));
    dlog.CentreOnParent();
    dlog.ShowModal();
    return dlog.GetLang();
}

void lmTheApp::FindOutScreenDPI()
{
	//We need to know the real screen resolution for texts (dots per inch) so that we
	//can set right the font size for scores.

	//GetPixelsPerLU()
    wxScreenDC dc;
    dc.SetMapMode(lmDC_MODE);
    dc.SetUserScale( 1.0 * lmSCALE, 1.0 * lmSCALE );

    double xPixelsPerLU = (double)dc.LogicalToDeviceXRel(100000) / 100000.0;
    double yPixelsPerLU = (double)dc.LogicalToDeviceYRel(100000) / 100000.0;
    // screen resolution (User settings, dots per inch)
    wxSize sizeDPI = dc.GetPPI();

    // In order to adjust staff lines to real size we will use only yPixelsPerLU.
	g_rPixelsPerLU = yPixelsPerLU;
	// To adjust font size to match staff size we need sizeDPI
	g_rScreenDPI = (double)sizeDPI.y;

	wxLogMessage(_T("[lmTheApp::FindOutScreenDPI] DisplayPixelsPerLU=(%f, %f), ")
				 _T("sizePPI=(%d, %d)"),
				 xPixelsPerLU, yPixelsPerLU, sizeDPI.x, sizeDPI.y );
}

const wxString lmTheApp::GetVersionNumber()
{
    // Increment this every time you release a new version
    wxString sVersion = _T("4.3b1");
    return sVersion;
}

const wxString lmTheApp::GetCurrentUser()
{
    wxString sUser = ::wxGetUserId();
    return sUser;
}

lmSplashFrame* lmTheApp::RecreateGUI(int nMilliseconds)
{
    bool fRestarting = false;
	lmMainFrame* pMainFrame = (lmMainFrame*)GetTopWindow();
	if(pMainFrame)
    {
        pMainFrame->PrepareToBeDestroyed();
		SetTopWindow(NULL);
		pMainFrame->Destroy();
        fRestarting = true;
	}

    bool fMaximized;
    wxRect wndRect;
    GetMainWindowPlacement(&wndRect, &fMaximized);

    g_pMainFrame = new lmMainFrame(m_pDocManager, (wxFrame*)NULL,
                      _T("LenMus"),                             // title
                      wxPoint(wndRect.x, wndRect.y),            // origin
                      wxSize(wndRect.width, wndRect.height),    // size
                      wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE);

    g_pMainFrame->CreateControls();

    if (fMaximized)
        g_pMainFrame->Maximize(true);

        //
        // Create and show the splash window. The splash can have a non-rectangular
        // shape. The color specified as second parameter of lmSplashFrame creation will
        // be used as the mask color to set the shape
        //

    lmSplashFrame* pSplash = (lmSplashFrame*) NULL;
    if (nMilliseconds > 0 && !fRestarting)
    {
        wxBitmap bitmap = wxArtProvider::GetBitmap(_T("app_splash"), wxART_OTHER);
        if (bitmap.Ok() && bitmap.GetHeight() > 100)
	    {
		    //the bitmap exists and it is not the error bitmap (height > 100 pixels). Show it
            wxColour colorTransparent(255, 0, 255);   //cyan mask
            pSplash = new lmSplashFrame(bitmap, colorTransparent,
                lmSPLASH_CENTRE_ON_PARENT | lmSPLASH_TIMEOUT,
                nMilliseconds, g_pMainFrame, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                wxBORDER_SIMPLE|wxSTAY_ON_TOP);
        }
        wxSafeYield();
    }

    g_pMainFrame->Show(true);
    SetTopWindow(g_pMainFrame);

    return pSplash;
}

wxString lmTheApp::GetInstallerLanguage()
{
    wxString sLang = _T("");
    wxString sPath = g_pPaths->GetBinPath();
    wxFileName oFilename(sPath, _T("config_ini"), _T("txt"), wxPATH_NATIVE);
    wxFileInputStream inFile( oFilename.GetFullPath() );
    if (!inFile.Ok()) return sLang;
    wxTextInputStream inTextFile(inFile);
    sLang = inTextFile.ReadWord();

    //verify that the read string is one of the supported languages
    int              nNumLangs;
    wxArrayString    cLangCodes;
    wxArrayString    cLangNames;
    GetLanguages(cLangCodes, cLangNames);
    nNumLangs = cLangNames.GetCount();

    int i;
    for(i=0; i < nNumLangs; i++)
        if(cLangCodes[i] == sLang) return sLang;

    // not found. Return empty string
    sLang = _T("");
    return sLang;
}

int lmTheApp::FilterEvent(wxEvent& event)
{
	if (event.GetEventType()==wxEVT_KEY_DOWN)
	{
		if( ((wxKeyEvent&)event).GetKeyCode()==WXK_F1 && g_pMainFrame
			&& g_pMainFrame->IsToolBoxVisible())
		{
			lmController* pController = g_pMainFrame->GetActiveController();
			if (pController)
			{
                lmToolBox* pTB = g_pMainFrame->GetActiveToolBox();
                if (pTB)
                {
				    pTB->OnKeyPress((wxKeyEvent&)event);
				    return true;	        //true: the event had been already processed
                }
                else
	                return -1;		//process the event normally
			}

			//g_pMainFrame->OnHelpF1( (wxKeyEvent&)event );
			//return true;	//true: the event had been already processed
			//				//false: the event is not going to be processed at all
		}
	}

	return -1;		//process the event normally
}

void lmTheApp::OnFatalException()
{
    //called when a crash occurs in this application

    // open forensic log file
    wxString sUserId = ::wxGetUserId();
    wxString sLogFile = g_pPaths->GetLogPath() + sUserId + _T("_forensic_log.txt");
    if (g_pLogger->IsValidForensicTarget(sLogFile))
    {
        //previous program run terminated with a crash.
        //inform user and request permision to submit file for bug analysis
        SendForensicLog(sLogFile, true);    //true: handling a crash
    }
}

void lmTheApp::SendForensicLog(wxString& sLogFile, bool fHandlingCrash)
{
    //upload the report

    //wxString sURL = _T("http://localhost/forensic.php/");
    wxString sURL = _T("http://www.lenmus.org/forensic.php/");
    wxString sCurlPath = g_pPaths->GetBinPath();
    lmForensicLog oFLog(sLogFile, sCurlPath);
    oFLog.UploadLog(sURL, _T("file"), _T(""), fHandlingCrash);
    //AWARE: In Windows, after a crash program execution never returns to here because
    //the main loop to handle events was stopped in previous sentence!
}



//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
// Global functions
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------


lmMainFrame* GetMainFrame(void)
{
    return g_pMainFrame;
}

