//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation;
//    either version 2 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street,
//    Fifth Floor, Boston, MA  02110-1301, USA.
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

#include "wx/wx.h"
#include "wx/frame.h"
#include "wx/image.h"
#include "wx/xrc/xmlres.h"          // use the xrc resource system
#include "wx/splash.h"
#include <wx/datetime.h>
#include "wx/txtstrm.h"
#include "wx/stdpaths.h"		// to get executable path
#include "wx/memory.h"			// to trace memory leaks

//#ifdef __WXMSW__
////Support for alpha channel on toolbar bitmaps
//#include "wx/sysopt.h"      // to access wxSystemOptions.
//#endif


#if defined(__WXMSW__) && !defined(__WXDEBUG__)
//For release versions (ANSI and Unicode) there is a compilation/linking error somewhere
//either in wxWidgets or in wxMidi as these two errors are generated:
//  wxmidiu.lib(wxMidi.obj) : error LNK2019: símbolo externo "void __cdecl wxOnAssert(unsigned short const *,int,char const *,unsigned short const *,unsigned short const *)" (?wxOnAssert@@YAXPBGHPBD00@Z) sin resolver al que se hace referencia en la función "void __cdecl wxPostEvent(class wxEvtHandler *,class wxEvent &)" (?wxPostEvent@@YAXPAVwxEvtHandler@@AAVwxEvent@@@Z)
//  wxmidiu.lib(wxMidiDatabase.obj) : error LNK2001: símbolo externo "void __cdecl wxOnAssert(unsigned short const *,int,char const *,unsigned short const *,unsigned short const *)" (?wxOnAssert@@YAXPBGHPBD00@Z) sin resolver
//As I can not avoid the error, these next definitions are a bypass:
    #if defined(_UNICODE)
        extern void __cdecl wxOnAssert(unsigned short const *n1,int n2,char const *n3,unsigned short const *n4,unsigned short const *n5);
        void __cdecl wxOnAssert(unsigned short const *n1,int n2,char const *n3,unsigned short const *n4,unsigned short const *n5) {}
    #else
        extern void __cdecl wxOnAssert(char const * n1,int n2,char const * m3,char const * n4,char const * n5);
        void __cdecl wxOnAssert(char const * n1,int n2,char const * m3,char const * n4,char const * n5) {}
    #endif
#endif

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
#include "LangChoiceDlg.h"
#include "ArtProvider.h"
#include "toolbox/ToolsBox.h"

// to save config information into a file
#include "wx/confbase.h"
#include "wx/fileconf.h"
#include "wx/filename.h"

#include "wx/fs_zip.h"                  //to use the zip file system
#include "Preferences.h"                //access to user preferences
#include "../sound/MidiManager.h"       //access to Midi configuration
#include "../sound/WaveManager.h"       //access to Wave sound manager
#include "Logger.h"                     //access to error's logger
#include "../ldp_parser/LDPTags.h"      //to delete the LDP tags table
#include "../options/Languages.h"       //to check config_ini.txt stored language

//access to global objects
#include "../globals/Paths.h"
#include "../globals/Colors.h"

//-------------------------------------------------------------------------------------------
// global variables
//-------------------------------------------------------------------------------------------
lmMainFrame *g_pMainFrame = (lmMainFrame*) NULL;
lmTheApp* g_pTheApp = (lmTheApp*) NULL;

#ifdef __WXDEBUG__
bool g_fReleaseVersion = false;       // to enable/disable debug features
#else
bool g_fReleaseVersion = true;        // to enable/disable debug features
#endif

bool g_fReleaseBehaviour = false;   // This flag is only used to force release behaviour when
                                    // in debug mode, and only for some functions (the ones using this flag)

bool g_fShowDebugLinks = false;     // force to add aditional debug ctrols in exercises.
                                    // only operative in debug mode.

bool g_fUseAntiAliasing = true;     // for testing and comparison purposes. Changing the
                                    // value of this flags forces to use standar aliased
                                    // renderization in screen

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


// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. lmTheApp and
// not wxApp)
IMPLEMENT_APP(lmTheApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

bool lmTheApp::OnInit(void)
{
   //lmAboutDialog dlg(NULL);
   //dlg.ShowModal();

    g_pTheApp = this;
    m_pInstanceChecker = (wxSingleInstanceChecker*) NULL;
    m_pDocManager = (wxDocManager *) NULL;
    m_pLocale = (wxLocale*) NULL;

//#ifdef __WXMSW__
////Support for alpha channel on toolbar bitmaps
//// call wxSystemOptions::SetOption(wxT("msw.remap"), 0) to switch off the remapping
//// that wxWidgets does to make the tool colours match the current system colours.
//// This remapping is only relevant for 16-colour tools, and messes up high-colour tools.
////You'll need to include wx/sysopt.h to access wxSystemOptions.
//    wxLogMessage(_T("[App::OnInit] msw.remap=%s"), wxSystemOptions::GetOption(wxT("msw.remap")));
//    if (wxTheApp->GetComCtl32Version() >= 600 && ::wxDisplayDepth() >= 32)
//        wxSystemOptions::SetOption(wxT("msw.remap"), 2);
//    else
//        wxSystemOptions::SetOption(wxT("msw.remap"), 0);
//
//  Convert your image to grayscale, using your favorite image editor. Submit the grayscale
//  image as second (disabled) bitmap in wxToolBar::AddTool().

//  Keep in mind that setting the system option msw.remap equal to "0" on Windows
//  results in disabled icons drawing the same way they do on Windows platforms with
//  ComCtl version less than 600 or a display color depth less than 32-bit which
//  means that images with alpha (not talking about image masks) are not going to
//  be supported even on versions of Windows that meet those requirements and do
//  have support for icons with alpha transparency

//#endif

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

        //
        // Get program directory and set up global paths object
        //

    #if defined(__WXGTK__) || defined(__WXMSW__) || defined(__MACOSX__)
    // On Linux, Windows and Mac OS X the path to the LenMus program is in argv[0]
    wxString sHomeDir = wxPathOnly(argv[0]);
    #elif defined(__MACOS9__)
    // On Mac OS 9, the initial working directory is the one that
    // contains the program.
    wxString sHomeDir = wxGetCwd();
    #else
    #error "Unknown operating system!"
    #endif
    g_pPaths = new lmPaths(sHomeDir);


	// AWARE: All paths, exect user configurable ones, are valid from this point
	// *************************************************************************


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

#ifdef __WXDEBUG__
    //define trace masks to be known by trace system
	g_pLogger->DefineTraceMask(_T("lmCadence"));
	g_pLogger->DefineTraceMask(_T("lmChordManager"));
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
    g_pLogger->DefineTraceMask(_T("lmScoreAuxCtrol"));
	g_pLogger->DefineTraceMask(_T("lmScoreCtrolParams"));
    g_pLogger->DefineTraceMask(_T("lmScoreView::OnMouseEvent"));
	g_pLogger->DefineTraceMask(_T("lmTheoKeySignCtrol"));
    g_pLogger->DefineTraceMask(_T("lmUpdater"));
#endif


	// AWARE: Log/debug methods are available from this point
	// *******************************************************

	wxLogMessage(_T("[lmTheApp::OnInit] Config file: ") + oCfgFile.GetFullPath() );

        //
        // Set up current language
        //

    wxString lang = g_pPrefs->Read(_T("/Locale/Language"), _T(""));
    if (lang == _T("")) {
        // The language is not set
        // This will only happen the first time the program is run or if
        // lenmus.ini file is deleted

        // try to get installer choosen language and use it if found
        lang = GetInstallerLanguage();

        if (lang == _T("")) {
            // Not found. Pop up a dialog to choose language.
            lang = ChooseLanguage(NULL);
        }
        g_pPrefs->Write(_T("/Locale/Language"), lang);
    }
    // Now that language code is know we can finish lmPaths initialization
    // and load locale catalogs
    SetUpLocale(lang);

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

        //
        // XRC resource system
        //

    // Initialize all the XRC handlers.
    wxXmlResource::Get()->InitAllHandlers();

    // get path for xrc resources
    wxString sPath = g_pPaths->GetXrcPath();

        //
        // Load all of the XRC files that will be used. You can put everything
        // into one giant XRC file if you wanted, but then they become more
        // difficult to manage, and harder to reuse in later projects.
        //

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

    // Configuration options: other options panel
    oXrcFile = wxFileName(sPath, _T("OtherOptionsPanel"), _T("xrc"), wxPATH_NATIVE);
    wxXmlResource::Get()->Load( oXrcFile.GetFullPath() );

    // Ear Interval exercises: configuration dialog
    oXrcFile = wxFileName(sPath, _T("DlgCfgEarIntervals"), _T("xrc"), wxPATH_NATIVE);
    wxXmlResource::Get()->Load( oXrcFile.GetFullPath() );

    // Theo Interval exercises: configuration dialog
    oXrcFile = wxFileName(sPath, _T("DlgCfgTheoIntervals"), _T("xrc"), wxPATH_NATIVE);
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

    // Error dialog
    oXrcFile = wxFileName(sPath, _T("ErrorDlg"), _T("xrc"), wxPATH_NATIVE);
    wxXmlResource::Get()->Load( oXrcFile.GetFullPath() );

#ifdef __WXDEBUG__
    // Debug: masks to trace dialog
    oXrcFile = wxFileName(sPath, _T("DlgDebugTrace"), _T("xrc"), wxPATH_NATIVE);
    wxXmlResource::Get()->Load( oXrcFile.GetFullPath() );
#endif

        //
        // Create document manager and templates
        //

    // Create a document manager
    m_pDocManager = new wxDocManager;

    // Sets the directory to be displayed to the user when opening a score.
    m_pDocManager->SetLastDirectory(g_pPaths->GetScoresPath());

    // Create a template relating score documents to their views
    (void) new wxDocTemplate(m_pDocManager, _T("LenMus score"), _T("*.lms"), _T(""), _T("lms"), _T("LenMus score"), _T("lmScore View"),
          CLASSINFO(lmScoreDocument), CLASSINFO(lmScoreView));
    (void) new wxDocTemplate(m_pDocManager, _T("MusicXML score"), _T("*.xml;*.*"), _T(""), _T("xml"), _T("MusicXML score"), _T("lmScore View"),
          CLASSINFO(lmScoreDocument), CLASSINFO(lmScoreView), wxTEMPLATE_INVISIBLE );


        //
        // Create the main frame window
        //

    int nMilliseconds = 10000;   // at least visible for 3 seconds
	long nSplashTime = (long) time( NULL );
    lmSplashFrame* pSplash = RecreateGUI(nMilliseconds);
    ::wxBeginBusyCursor();

        //
        //Main frame created and visible. Proceed with initializations
        //

    // create global data structures for printer settings
    g_pPrintData = new wxPrintData;
    g_pPaperSetupData = new wxPageSetupDialogData;

    //Seed the random-number generator with current time so that
    //the numbers will be different every time we run.
    srand( (unsigned)time( NULL ) );

        //
        //Set up MIDI
        //

    g_pMidi = lmMidiManager::GetInstance();

    //if MIDI not set, force to run the MIDI wizard
    if (!g_pMidi->IsConfigured()) {
	    // first of all, remove splash; otherwise, splahs hides the wizard!
        if (pSplash) {
            pSplash->TerminateSplash();
            pSplash = (lmSplashFrame*) NULL;
        }
        //now we can run the wizard
        ::wxEndBusyCursor();
        g_pMainFrame->DoRunMidiWizard();
        ::wxBeginBusyCursor();
    }

    //Set up MIDI devices
    g_pMidi->SetUpCurrentConfig();

    //program sound for metronome
    if (g_pMidiOut)
        g_pMidiOut->ProgramChange(g_pMidi->MtrChannel(), g_pMidi->MtrInstr());

        // all initialization finished.

	// check if the splash window display time is ellapsed and wait if not
    if (pSplash) {
	    nMilliseconds -= ((long)time( NULL ) - nSplashTime);
	    if (nMilliseconds > 0) ::wxMilliSleep( nMilliseconds );
        pSplash->AllowDestroy();    // allow to destroy the splash
    }

    //force to show book frame
    #if !defined(__WXDEBUG__)       //in debug version, start with nothing displayed
        wxCommandEvent event;       //it is not used, so not need to initialize it
        g_pMainFrame->OnOpenBook(event);
    #endif

    //cursor normal
    ::wxEndBusyCursor();

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
            wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, MENU_CheckForUpdates);
            wxPostEvent(g_pMainFrame, event);
        }
    }

    return true;
}

void lmTheApp::ChangeLanguage(wxString lang)
{
    SetUpLocale(lang);

    //Re-create main frame
    RecreateGUI(0);   //recreate all. No splash

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
        //
        //Save any other user preferences and values, not saved yet
        //

    // Save the last selected directories
    g_pPaths->SetScoresPath( m_pDocManager->GetLastDirectory() );
    g_pPaths->SaveUserPreferences();


        //
        // delete all objects used by lmTheApp
        //

    // the Midi configuration and related objects
    if (g_pMidi) delete g_pMidi;

    // the wave sound manager object
    lmWaveManager::Destroy();

    // the docManager
    delete m_pDocManager;

    // the printer setup data
    delete g_pPrintData;
    delete g_pPaperSetupData;

    // path names
    delete g_pPaths;

    // colors object
    delete g_pColors;

    // the wxConfig object
    delete wxConfigBase::Set((wxConfigBase *) NULL);

    // the error's logger
    delete g_pLogger;

    // the LDP tags table
    lmLdpTagsTable::DeleteInstance();

    //the program options object
    lmPgmOptions::DeleteInstance();

    //locale object
    delete m_pLocale;

    // single instance checker
    if (m_pInstanceChecker) delete m_pInstanceChecker;

	return 0;
}

// Set up the default size and position of the main window
void lmTheApp::GetDefaultMainWindowRect(wxRect *defRect)
{
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

//Pop up a language asking the user to choose a language for the user interface.
//Generally only popped up once, the first time the program is run.
wxString lmTheApp::ChooseLanguage(wxWindow *parent)
{
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

// Update all views of document associated to currentView
void lmTheApp::UpdateCurrentDocViews(void)
{
    lmScoreDocument *doc = (lmScoreDocument *)m_pDocManager->GetCurrentDocument();
    doc->UpdateAllViews();

}

wxString lmTheApp::GetVersionNumber()
{
    // Increment this every time you release a new version
    wxString sVersion = _T("4.0a0");
    return sVersion;
}

lmSplashFrame* lmTheApp::RecreateGUI(int nMilliseconds)
{
    bool fRestarting = false;
	wxWindow* pTopwindow = GetTopWindow();
	if(pTopwindow) {
		SetTopWindow(NULL);
		pTopwindow->Destroy();
        fRestarting = true;
	}

    bool fMaximized;
    wxRect wndRect;
    GetMainWindowPlacement(&wndRect, &fMaximized);

    g_pMainFrame = new lmMainFrame((wxDocManager *) m_pDocManager, (wxFrame *) NULL,
                      _T("LenMus"),                             // title
                      wxPoint(wndRect.x, wndRect.y),            // origin
                      wxSize(wndRect.width, wndRect.height),    // size
                      wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE);

    g_pMainFrame->CreateControls();

    if (fMaximized)  g_pMainFrame->Maximize(true);

        //
        // Create and show the splash window. The splash can have a non-rectangular
        // shape. The color specified as second parameter of lmSplashFrame creation will
        // be used as the mask color to set the shape
        //

    lmSplashFrame* pSplash = (lmSplashFrame*) NULL;
    if (!fRestarting)
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

    //force to show book frame
    if (fRestarting) {
        wxCommandEvent event;       //it is not used, so not need to initialize it
        g_pMainFrame->OnOpenBook(event);
    }

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
				g_pMainFrame->GetActiveToolBox()->ProcessEvent(event);
				return true;	//true: the event had been already processed
			}

			//g_pMainFrame->OnHelpF1( (wxKeyEvent&)event );
			//return true;	//true: the event had been already processed
			//				//false: the event is not going to be processed at all
		}
	}

	return -1;		//process the event normally
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

