//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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
/*! @class lmTheApp
    @ingroup app_gui
    @brief lmTheApp class represents the application itself

    Derived from wxApp class, lmTheApp class represents the application itself. Takes care of
    implementing the main event loop and to handle events not handled by other objects in
    the application. The two main visible responsibilities are:

    - To set and get application-wide properties;
    - To initiate application processing via wxApp::OnInit;

*/
#ifdef __GNUG__
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

//#ifdef __WXMSW__
////Support for alpha channel on toolbar bitmaps
//#include "wx/sysopt.h"      // to access wxSystemOptions.
//#endif


#ifndef _DEBUG
//For release versions (ANSI and Unicode) there is a compilation/linking error somewhere
//either in wxWidgets or in wxMidi as these two errors are generated:
//  wxmidiu.lib(wxMidi.obj) : error LNK2019: símbolo externo "void __cdecl wxOnAssert(unsigned short const *,int,char const *,unsigned short const *,unsigned short const *)" (?wxOnAssert@@YAXPBGHPBD00@Z) sin resolver al que se hace referencia en la función "void __cdecl wxPostEvent(class wxEvtHandler *,class wxEvent &)" (?wxPostEvent@@YAXPAVwxEvtHandler@@AAVwxEvent@@@Z)
//  wxmidiu.lib(wxMidiDatabase.obj) : error LNK2001: símbolo externo "void __cdecl wxOnAssert(unsigned short const *,int,char const *,unsigned short const *,unsigned short const *)" (?wxOnAssert@@YAXPBGHPBD00@Z) sin resolver
//As I can not avoid the error, these next definitions are a bypass:
    #if _UNICODE
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

#ifdef __WXMSW__
// Detecting and isolating memory leaks with Visual C++
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

// CONFIG: Under Windows, change this to 1 to use wxGenericDragImage
#define wxUSE_GENERIC_DRAGIMAGE 1


//#ifdef _DEBUG
//    #if !wxUSE_UNICODE
//        #error "You must set wxUSE_UNICODE to 1 in setup.h!"
//    #endif
//    #if !wxUSE_UNICODE_MSLU
//        #error "You must set wxUSE_UNICODE_MSLU to 1 in setup.h!"
//    #endif
//#endif


#include "TheApp.h"
#include "MainFrame.h"
#include "ScoreDoc.h"
#include "scoreView.h"
#include "AboutDialog.h"
#include "LangChoiceDlg.h"
#include "ArtProvider.h"

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

#ifdef _DEBUG
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

lmTheApp::lmTheApp(void)
{
    g_pTheApp = this;
}

bool lmTheApp::OnInit(void)
{
    m_pDocManager = (wxDocManager *) NULL;
    m_pLocale = (wxLocale*) NULL;

    // Error reporting and trace
    g_pLogger = new lmLogger();

#ifdef _DEBUG
	// For debugging: send log messages to a file
    FILE* pFile;
    #ifdef _UNICODE
        pFile = _wfopen(_T("LenMus_error_log.txt"), _T("w"));
    #else
        pFile = fopen(_T("LenMus_error_log.txt"), _T("w"));
    #endif
	wxLog *logger = new wxLogStderr(pFile);
    wxLogChain *logChain = new wxLogChain(logger);
	//delete wxLog::SetActiveTarget(logger);
#endif

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


    //define trace mask to be known by trace system
    g_pLogger->DefineTraceMask(_T("lmKeySignature"));
    g_pLogger->DefineTraceMask(_T("lmTheoKeySignCtrol"));
    g_pLogger->DefineTraceMask(_T("lmComposer5"));
    g_pLogger->DefineTraceMask(_T("lmMusicXMLParser"));
    g_pLogger->DefineTraceMask(_T("lmUpdater"));
    g_pLogger->DefineTraceMask(_T("lmInterval"));


    // set information about this application
    SetVendorName(_T("LenMus"));
    SetAppName(_T("LenMus"));

        //
        // Get program directory and set up global paths object
        //

    #ifdef __WXMSW__
    // On Windows, the path to the LenMus program is in argv[0]
    wxString sHomeDir = wxPathOnly(argv[0]);
    #endif
    #ifdef __MACOSX__
    // On Mac OS X, the path to the LenMus program is in argv[0]
    wxString sHomeDir = wxPathOnly(argv[0]);
    #endif
    #ifdef __MACOS9__
    // On Mac OS 9, the initial working directory is the one that
    // contains the program.
    wxString sHomeDir = wxGetCwd();
    #endif
    //! @todo Set path on UNIX systems
    g_pPaths = new lmPaths(sHomeDir);


        //
        // Prepare preferences object
        //

    // set path and name for config file
    wxFileName sCfgFile(sHomeDir, _T("lenmus"), _T("ini") );
    wxFileConfig *pConfig = new wxFileConfig(_T("lenmus"), _T("LenMus"), sCfgFile.GetFullPath(),
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

    // colors
    g_pColors = new lmColors();

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

    // open log file and re-direct all loging there
    wxFileName oFilename(g_pPaths->GetTempPath(), _T("DataError"), _T("log"), wxPATH_NATIVE);
    g_pLogger->SetDataErrorTarget(oFilename.GetFullPath());


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

    // About box
    oXrcFile = wxFileName(sPath, _T("AboutDialog"), _T("xrc"), wxPATH_NATIVE);
    wxXmlResource::Get()->Load( oXrcFile.GetFullPath() );

        //
        // Create document manager and templates
        //

    // Create a document manager
    m_pDocManager = new wxDocManager;

    // Sets the directory to be displayed to the user when opening a score.
    m_pDocManager->SetLastDirectory(g_pPaths->GetScoresPath());

    // Create a template relating score documents to their views
    (void) new wxDocTemplate(m_pDocManager, _T("LenMus score"), _T("*.lms"), _T(""), _T("lms"), _T("Music lmScore"), _T("lmScore View"),
          CLASSINFO(lmScoreDocument), CLASSINFO(lmScoreView));

        //
        // Create the main frame window
        //

    int nMilliseconds = 3000;   // at least visible for 3 seconds
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
    g_pMidiOut->ProgramChange(g_pMidi->MtrChannel(), g_pMidi->MtrInstr());

        // all initialization finished.

	// check if the splash window display time is ellapsed and wait if not
    if (pSplash) {
	    nMilliseconds -= ((long)time( NULL ) - nSplashTime);
	    if (nMilliseconds > 0) ::wxMilliSleep( nMilliseconds );
        pSplash->AllowDestroy();    // allow to destroy the splash
    }

//remove this in debug version to start with nothing displayed
#if !_DEBUG
    //force to show book frame
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
                wxLogMessage(_T("[TheApp::OnInit] Error parsing the last check for updates date '%s'.\n"), sLastCheckDate);
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

            wxLogMessage(_T("[TheApp::OnInit] CheckForUpdates: dtLastCheck='%s', sCheckFreq=%s (%d), dtNextCheck='%s', fDoCheck=%s"),
                    dtLastCheck.Format(_T("%x")).c_str(),
                    sCheckFreq, dsSpan.GetTotalDays(),
                    dtNextCheck.Format(_T("%x")).c_str(),
                    (fDoCheck ? _T("True") : _T("False")) );

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
        wxLogMessage(_T("[lmTheApp::SetUpLocale] Language '%s' not found. Update lmApp.cpp?"), lang);
    }


    // locale object re-initialization
    if (m_pLocale) delete m_pLocale;
    m_pLocale = new wxLocale();
    if (!m_pLocale->Init(_T(""), lang, _T(""), false, true)) {
    //if (!m_pLocale->Init( nLang, wxLOCALE_CONV_ENCODING )) {
        wxMessageBox( wxString::Format(_T("Language %s can not be set. ")
            _T("Please, verify that any required language codepages are installed in your system."),
            sLangName));
    }
    else {
        wxString sPath = g_pPaths->GetLocaleRootPath();
        m_pLocale->AddCatalogLookupPathPrefix( sPath );
        wxString sCtlg;
        wxString sNil = _T("");
        sCtlg = sNil + _T("lenmus_") + lang;    //m_pLocale->GetName();
        if (!m_pLocale->AddCatalog(sCtlg))
            wxLogMessage(_T("[lmTheApp::SetUpLocale] Failure to load catalog '%s'. Path='%s'"), sCtlg, sPath);
        sCtlg = sNil + _T("wxwidgets_") + lang;
        if (!m_pLocale->AddCatalog(sCtlg))
            wxLogMessage(_T("[lmTheApp::SetUpLocale] Failure to load catalog '%s'. Path='%s'"), sCtlg, sPath);
        sCtlg = sNil + _T("wxmidi_") + lang;
        if (!m_pLocale->AddCatalog(sCtlg))
            wxLogMessage(_T("[lmTheApp::SetUpLocale] Failure to load catalog '%s'. Path='%s'"), sCtlg, sPath);
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
    delete g_pMidi;

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

    //locale object
    delete m_pLocale;

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

   defRect->width = screenRect.width * 0.95;
   defRect->height = screenRect.height * 0.95;

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

// Update all views of document associated to currentView
void lmTheApp::UpdateCurrentDocViews(void)
{
    lmScoreDocument *doc = (lmScoreDocument *)m_pDocManager->GetCurrentDocument();
    doc->UpdateAllViews();

}

//
// Centralised code for creating a document frame.
// Called from scoreview.cpp, when a view is created.
/*! @todo this is no longer needed here as CreateMenuBar is no longer invoked. So move
    this to EditFrame.cpp
    Chage to create frame maximized?
    set icon ?
*/
lmEditFrame* lmTheApp::CreateProjectFrame(wxDocument* doc, wxView* view)
{
    //Get the size of the main frame client area
    int nWidth, nHeight;
    GetMainFrame()->GetClientSize(&nWidth, &nHeight);

    // Create a child frame
    int nToolbarHeight = 100;       //! @todo get toolbar height to discount it
    wxSize size(nWidth-20, nHeight-nToolbarHeight-20);
    wxPoint pos(10, 10);
    lmEditFrame* pEditFrame = new lmEditFrame(doc, view, GetMainFrame(), pos, size);

    //set icon
    //! @todo Find a better icon
    pEditFrame->SetIcon( wxArtProvider::GetIcon(_T("tool_new"), wxART_TOOLBAR, wxSize(16,16)) );

    return pEditFrame;
}

wxString lmTheApp::GetVersionNumber()
{
    // Increment this every time you release a new version
    wxString sVersion = _T("3.4");
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
                wxSIMPLE_BORDER|wxSTAY_ON_TOP);
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

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
// Global functions
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------


lmMainFrame* GetMainFrame(void)
{
    return g_pMainFrame;
}

lmLUnits lmToLogicalUnits(int nValue, lmEUnits nUnits)
{
    return lmToLogicalUnits((double)nValue, nUnits);
}

lmLUnits lmToLogicalUnits(double rValue, lmEUnits nUnits)
{
    // first convert to tenths of millimeter (mode MM_LOMETRIC), then divide by SCALE factor
    switch(nUnits) {
        case lmMICRONS:         return (int)((rValue / 100.) / lmSCALE);      break;
        case lmMILLIMETERS:     return (int)((rValue * 10.) / lmSCALE);       break;
        case lmCENTIMETERS:     return (int)((rValue * 100.) / lmSCALE);      break;
        case lmINCHES:          return (int)((rValue * 254.) / lmSCALE);      break;
        default:
            wxASSERT(false);
            return 10;
    }

}

