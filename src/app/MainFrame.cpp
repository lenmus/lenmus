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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "MainFrame.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/image.h"
#include "wx/print.h"
#include "wx/printdlg.h"

// the next three includes are for saving config information into a file
#include "wx/confbase.h"
#include "wx/fileconf.h"
#include "wx/filename.h"

#include "wx/dir.h"             // to search directories
#include "ArtProvider.h"        // to use ArtProvider for managing icons


#include "TheApp.h"
#include "MainFrame.h"
#include "EditFrame.h"
#include "ScoreDoc.h"
#include "ScoreView.h"
#include "AboutDialog.h"
#include "StatusBar.h"
#include "DlgDebug.h"
#include "DlgDebugTrace.h"
#include "Printout.h"
#include "MidiWizard.h"             //Use lmMidiWizard
#include "ScoreWizard.h"            //Use lmScoreWizard
#include "WelcomeWnd.h"
#include "Preferences.h"            //access to user preferences

#include "wx/helpbase.h"		    //for wxHELP constants
#include "../options/OptionsDlg.h"
#include "toolbox/ToolsBox.h"
#include "toolbox/ToolPage.h"
#include "../../wxMidi/include/wxMidi.h"    //MIDI support throgh Portmidi lib
#include "../sound/MidiManager.h"           //access to Midi configuration
#include "../updater/Updater.h"
#include "../graphic/BoxScore.h"

#include "Processor.h"      //Debug: Harmony processor


//access to error's logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;

#include "DlgPatternEditor.h"               // to test DlgPatternEditor dialog
#include "../auxmusic/Chord.h"       //for Unit Tests

// to use html help controller
#include "wx/html/helpctrl.h"
#include "../html/TextBookController.h"
#include "../html/HelpController.h"

#include "../sound/Metronome.h"


#define USE_WX_DOC_MANAGER      0

//========================================================================================
//AWARE
//            Platform configuration defines. Change values as appropiate
//========================================================================================

// define this to 1 to use wxToolBarSimple instead of the native one
#define USE_GENERIC_TBAR 0

//-- Verify coherence --------------------------------------------------------------------
// define this to use XPMs everywhere (by default, BMPs are used under Win)
// BMPs use less space, but aren't compiled into the executable on other platforms
#ifdef __WXMSW__
    #define USE_XPM_BITMAPS 0
    #define wxUSE_GENERIC_DRAGIMAGE 1
#else
    #define USE_XPM_BITMAPS 1
    #define wxUSE_GENERIC_DRAGIMAGE 0
#endif

#if USE_GENERIC_TBAR
    #if !wxUSE_TOOLBAR_SIMPLE
        #error wxToolBarSimple is not compiled in, set wxUSE_TOOLBAR_SIMPLE \
               to 1 in setup.h and recompile the library.
    #else
        #include <wx/tbarsmpl.h>
    #endif
#endif // USE_GENERIC_TBAR

#if USE_XPM_BITMAPS && defined(__WXMSW__) && !wxUSE_XPM_IN_MSW
    #error You need to enable XPM support to use XPM bitmaps with toolbar!
#endif // USE_XPM_BITMAPS

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

//#if !wxUSE_UNICODE
//#error "You must set wxUSE_UNICODE to 1 in setup.h!"
//#endif
//
//#if !wxUSE_UNICODE_MSLU
//#error "You must set wxUSE_UNICODE_MSLU to 1 in setup.h!"
//#endif
//-----------------------------------------------------------------------------------------




//---------------------------------------------------------------------------------------
// Global variables
//---------------------------------------------------------------------------------------

// global data structures for printing. Defined in TheApp.cpp
#include "wx/cmndata.h"
extern wxPrintData* g_pPrintData;
extern wxPageSetupData* g_pPaperSetupData;

// access to paths
#include "../globals/Paths.h"
extern lmPaths* g_pPaths;

// access to global external variables
extern bool g_fReleaseVersion;          // in TheApp.cpp
extern bool g_fReleaseBehaviour;        // in TheApp.cpp
extern bool g_fShowDebugLinks;          // in TheApp.cpp
extern bool g_fShowDirtyObjects;         // in TheApp.cpp
extern bool g_fBorderOnScores;          // in TheApp.cpp

// IDs for menus and controls
// Appart of these, there are more definitions in MainFrame.h
enum
{
    // Menu File
    MENU_File_New = lmMENU_Last_Public_ID,
    MENU_File_Open,     //wxID_OPEN
    MENU_File_Save,     //wxID_SAVE
    MENU_File_SaveAs,   //wxID_SAVEAS
    MENU_File_Close,    //wxID_CLOSE

    MENU_File_Import,
    MENU_File_Export,
    MENU_File_Export_MusicXML,
    MENU_File_Export_bmp,
    MENU_File_Export_jpg,
    MENU_OpenBook,

     // Menu Edit
    MENU_Edit_Copy,
    MENU_Edit_Cut,
    MENU_Edit_Paste,

     // Menu View
    MENU_View_Tools,
    MENU_View_Rulers,
    MENU_View_ToolBar,
    MENU_View_StatusBar,
    MENU_View_Page_Margins,
    MENU_View_Welcome_Page,

	// Menu Score
	MENU_Score_Titles,

	// Menu Instrument
    MENU_Instr_Properties,

    // Menu Debug
    MENU_Debug_ForceReleaseBehaviour,
    MENU_Debug_ShowDebugLinks,
    MENU_Debug_ShowBorderOnScores,
    MENU_Debug_recSelec,
    MENU_Debug_CheckHarmony,
    MENU_Debug_DrawAnchors,
    MENU_Debug_DrawBounds,
    MENU_Debug_DrawBounds_BoxSystem,
    MENU_Debug_DrawBounds_BoxSlice,
    MENU_Debug_DrawBounds_BoxSliceInstr,
    MENU_Debug_DrawBounds_BoundsShapes,
    MENU_Debug_DumpStaffObjs,
	MENU_Debug_DumpGMObjects,
    MENU_Debug_SeeSource,
    MENU_Debug_SeeSourceUndo,
    MENU_Debug_SeeXML,
    MENU_Debug_SeeMIDIEvents,
    MENU_Debug_SetTraceLevel,
    MENU_Debug_PatternEditor,
    MENU_Debug_DumpBitmaps,
    MENU_Debug_UnitTests,
    MENU_Debug_ShowDirtyObjects,
    MENU_Debug_TestProcessor,

    // Menu Zoom
    MENU_Zoom_100,
    MENU_Zoom_Other,
    MENU_Zoom_Fit_Full,
    MENU_Zoom_Fit_Width,
    MENU_Zoom_Decrease,
    MENU_Zoom_Increase,

    //Menu Sound
    MENU_Sound_MidiWizard,
    MENU_Sound_test,
    MENU_Sound_AllSoundsOff,

    //Menu Play
    MENU_Play_Start,
	MENU_Play_Cursor_Start,
    MENU_Play_Stop,
    MENU_Play_Pause,

    //Menu Options
    MENU_Preferences,

    //Menu Window
    MENU_WindowClose,
    MENU_WindowCloseAll,
    MENU_WindowNext,
    MENU_WindowPrev,

    // Menu Help
    MENU_Help_Open,
    MENU_Help_QuickGuide,
    MENU_VisitWebsite,

    // Menu Print
    MENU_Print,
    MENU_Print_Preview,
    MENU_Page_Setup,

    // Menu metronome
    MENU_Metronome,

  // controls IDs
    lmID_COMBO_ZOOM,
    lmID_SPIN_METRONOME,

  // other IDs
    lmID_TIMER_MTR,
	lmID_F1_KEY,

  // special IDs

    //wxID_ABOUT and wxID_EXIT are predefined by wxWidgets and have a special meaning
    //since entries using these IDs will be taken out of the normal menus under MacOS X
    //and will be inserted into the system menu (following the appropriate MacOS X
    //interface guideline). On PalmOS wxID_EXIT is disabled according to Palm OS
    //Companion guidelines.
    MENU_Help_About = wxID_ABOUT,


};


 //lmMainFrame is the top-level window of the application.

const wxString lmRECENT_FILES = _T("Recent Files/file");

IMPLEMENT_CLASS(lmMainFrame, lmDocTDIParentFrame)
BEGIN_EVENT_TABLE(lmMainFrame, lmDocTDIParentFrame)
	EVT_CHAR(lmMainFrame::OnKeyPress)
	EVT_MENU(lmID_F1_KEY, lmMainFrame::OnKeyF1)


    //File menu/toolbar
    EVT_MENU      (MENU_File_Open, lmMainFrame::OnFileOpen)
    EVT_UPDATE_UI (MENU_File_Open, lmMainFrame::OnFileUpdateUI)
    EVT_MENU      (MENU_File_Close, lmMainFrame::OnFileClose)
    EVT_UPDATE_UI (MENU_File_Close, lmMainFrame::OnFileUpdateUI)
    EVT_MENU      (MENU_File_Save, lmMainFrame::OnFileSave)
    EVT_UPDATE_UI (MENU_File_Save, lmMainFrame::OnFileUpdateUI)
    EVT_MENU      (MENU_File_SaveAs, lmMainFrame::OnFileSaveAs)
    EVT_UPDATE_UI (MENU_File_SaveAs, lmMainFrame::OnFileUpdateUI)

    EVT_MENU      (MENU_File_New, lmMainFrame::OnScoreWizard)
    EVT_MENU      (MENU_File_Import, lmMainFrame::OnFileImport)
    EVT_UPDATE_UI (MENU_File_Import, lmMainFrame::OnFileUpdateUI)
    EVT_UPDATE_UI (MENU_File_Export, lmMainFrame::OnFileUpdateUI)
    EVT_MENU      (MENU_File_Export_MusicXML, lmMainFrame::OnExportMusicXML)
    EVT_UPDATE_UI (MENU_File_Export_MusicXML, lmMainFrame::OnFileUpdateUI)
    EVT_MENU      (MENU_File_Export_bmp, lmMainFrame::OnExportBMP)
    EVT_UPDATE_UI (MENU_File_Export_bmp, lmMainFrame::OnFileUpdateUI)
    EVT_MENU      (MENU_File_Export_jpg, lmMainFrame::OnExportJPG)
    EVT_UPDATE_UI (MENU_File_Export_jpg, lmMainFrame::OnFileUpdateUI)
    EVT_MENU      (MENU_Print_Preview, lmMainFrame::OnPrintPreview)
    EVT_UPDATE_UI (MENU_Print_Preview, lmMainFrame::OnFileUpdateUI)
    EVT_MENU      (wxID_PRINT_SETUP, lmMainFrame::OnPrintSetup)
    EVT_UPDATE_UI (wxID_PRINT_SETUP, lmMainFrame::OnFileUpdateUI)
    EVT_MENU      (MENU_Print, lmMainFrame::OnPrint)
    EVT_UPDATE_UI (MENU_Print, lmMainFrame::OnFileUpdateUI)
    EVT_UPDATE_UI (wxID_SAVE, lmMainFrame::OnFileUpdateUI)
    EVT_UPDATE_UI (wxID_SAVEAS, lmMainFrame::OnFileUpdateUI)
    EVT_UPDATE_UI (MENU_File_New, lmMainFrame::OnFileUpdateUI)
    EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, lmMainFrame::OnOpenRecentFile)

    //Edit menu/toolbar
    EVT_MENU      (MENU_Edit_Copy, lmMainFrame::OnEditCopy)
    EVT_UPDATE_UI (MENU_Edit_Copy, lmMainFrame::OnEditUpdateUI)
    EVT_MENU      (MENU_Edit_Cut, lmMainFrame::OnEditCut)
    EVT_UPDATE_UI (MENU_Edit_Cut, lmMainFrame::OnEditUpdateUI)
    EVT_MENU      (MENU_Edit_Paste, lmMainFrame::OnEditPaste)
    EVT_UPDATE_UI (MENU_Edit_Paste, lmMainFrame::OnEditUpdateUI)
    EVT_UPDATE_UI (wxID_UNDO, lmMainFrame::OnEditUpdateUI)
    EVT_UPDATE_UI (wxID_REDO, lmMainFrame::OnEditUpdateUI)

    //View menu/toolbar
    EVT_MENU      (MENU_View_Tools, lmMainFrame::OnViewTools)
    EVT_UPDATE_UI (MENU_View_Tools, lmMainFrame::OnEditUpdateUI)
    EVT_MENU      (MENU_View_Rulers, lmMainFrame::OnViewRulers)
    EVT_UPDATE_UI (MENU_View_Rulers, lmMainFrame::OnViewRulersUI)
    EVT_MENU      (MENU_View_ToolBar, lmMainFrame::OnViewToolBar)
    EVT_UPDATE_UI (MENU_View_ToolBar, lmMainFrame::OnToolbarsUI)
    EVT_MENU      (MENU_View_StatusBar, lmMainFrame::OnViewStatusBar)
    EVT_UPDATE_UI (MENU_View_StatusBar, lmMainFrame::OnStatusbarUI)
    EVT_MENU      (MENU_View_Page_Margins, lmMainFrame::OnViewPageMargins)
    EVT_UPDATE_UI (MENU_View_Page_Margins, lmMainFrame::OnEditUpdateUI)
    EVT_MENU      (MENU_View_Welcome_Page, lmMainFrame::OnViewWelcomePage)
    EVT_UPDATE_UI (MENU_View_Welcome_Page, lmMainFrame::OnViewWelcomePageUI)

    //Score menu/toolbar
    EVT_MENU      (MENU_Score_Titles, lmMainFrame::OnScoreTitles)
    EVT_UPDATE_UI (MENU_Score_Titles, lmMainFrame::OnEditUpdateUI)

	// Instrument menu
	EVT_MENU      (MENU_Instr_Properties, lmMainFrame::OnInstrumentProperties)
    EVT_UPDATE_UI (MENU_Instr_Properties, lmMainFrame::OnEditUpdateUI)

	//Zoom menu/toolbar
    EVT_MENU (MENU_Zoom_100, lmMainFrame::OnZoom100)
    EVT_MENU (MENU_Zoom_Other, lmMainFrame::OnZoomOther)
    EVT_MENU (MENU_Zoom_Fit_Full, lmMainFrame::OnZoomFitFull)
    EVT_MENU (MENU_Zoom_Fit_Width, lmMainFrame::OnZoomFitWidth)
    EVT_MENU (MENU_Zoom_Decrease, lmMainFrame::OnZoomDecrease)
    EVT_MENU (MENU_Zoom_Increase, lmMainFrame::OnZoomIncrease)
    EVT_UPDATE_UI_RANGE (MENU_Zoom_100, MENU_Zoom_Increase, lmMainFrame::OnZoomUpdateUI)
    EVT_COMBOBOX  (lmID_COMBO_ZOOM, lmMainFrame::OnComboZoom )
    EVT_TEXT_ENTER(lmID_COMBO_ZOOM, lmMainFrame::OnComboZoom )
    EVT_UPDATE_UI (lmID_COMBO_ZOOM, lmMainFrame::OnZoomUpdateUI)

    //Sound menu/toolbar
    EVT_MENU      (MENU_Sound_MidiWizard, lmMainFrame::OnRunMidiWizard)
    EVT_MENU      (MENU_Sound_test, lmMainFrame::OnSoundTest)
    EVT_MENU      (MENU_Sound_AllSoundsOff, lmMainFrame::OnAllSoundsOff)
    EVT_MENU      (MENU_Play_Start, lmMainFrame::OnPlayStart)
    EVT_UPDATE_UI (MENU_Play_Start, lmMainFrame::OnSoundUpdateUI)
    EVT_MENU      (MENU_Play_Cursor_Start, lmMainFrame::OnPlayCursorStart)
    EVT_UPDATE_UI (MENU_Play_Cursor_Start, lmMainFrame::OnSoundUpdateUI)
    EVT_MENU      (MENU_Play_Stop, lmMainFrame::OnPlayStop)
    EVT_UPDATE_UI (MENU_Play_Stop, lmMainFrame::OnSoundUpdateUI)
    EVT_MENU      (MENU_Play_Pause, lmMainFrame::OnPlayPause)
    EVT_UPDATE_UI (MENU_Play_Pause, lmMainFrame::OnSoundUpdateUI)

    EVT_MENU (MENU_Preferences, lmMainFrame::OnOptions)

    EVT_MENU      (MENU_OpenBook, lmMainFrame::OnOpenBook)
    EVT_UPDATE_UI (MENU_OpenBook, lmMainFrame::OnOpenBookUI)
    EVT_MENU      (MENU_Metronome, lmMainFrame::OnMetronomeOnOff)

    // Window menu
    EVT_MENU (MENU_WindowClose, lmMainFrame::OnWindowClose)
    EVT_MENU (MENU_WindowCloseAll, lmMainFrame::OnWindowCloseAll)
    EVT_MENU (MENU_WindowNext, lmMainFrame::OnWindowNext)
    EVT_MENU (MENU_WindowPrev, lmMainFrame::OnWindowPrev)

    // Help menu
    EVT_MENU      (MENU_Help_About, lmMainFrame::OnAbout)
    EVT_MENU      (MENU_Help_Open, lmMainFrame::OnHelpOpen)
    EVT_MENU      (MENU_Help_QuickGuide, lmMainFrame::OnHelpQuickGuide)
    EVT_MENU      (lmMENU_CheckForUpdates, lmMainFrame::OnCheckForUpdates)
    EVT_MENU      (MENU_VisitWebsite, lmMainFrame::OnVisitWebsite)

        //general debug options. Always enabled
#ifdef __WXDEBUG__
    EVT_MENU (MENU_Debug_ForceReleaseBehaviour, lmMainFrame::OnDebugForceReleaseBehaviour)
    EVT_MENU (MENU_Debug_ShowDebugLinks, lmMainFrame::OnDebugShowDebugLinks)
    EVT_MENU (MENU_Debug_ShowBorderOnScores, lmMainFrame::OnDebugShowBorderOnScores)
    EVT_MENU (MENU_Debug_SetTraceLevel, lmMainFrame::OnDebugSetTraceLevel)
    EVT_MENU (MENU_Debug_PatternEditor, lmMainFrame::OnDebugPatternEditor)
    EVT_MENU (MENU_Debug_recSelec, lmMainFrame::OnDebugRecSelec)
    EVT_MENU (MENU_Debug_DrawBounds, lmMainFrame::OnDebugDrawBounds)
    EVT_MENU (MENU_Debug_DrawBounds_BoxSystem, lmMainFrame::OnDebugDrawBounds)
    EVT_MENU (MENU_Debug_DrawBounds_BoxSlice, lmMainFrame::OnDebugDrawBounds)
    EVT_MENU (MENU_Debug_DrawBounds_BoxSliceInstr, lmMainFrame::OnDebugDrawBounds)
    EVT_MENU (MENU_Debug_DrawBounds_BoundsShapes, lmMainFrame::OnDebugDrawBounds)
    EVT_MENU (MENU_Debug_DrawAnchors, lmMainFrame::OnDebugDrawAnchors)
    EVT_MENU (MENU_Debug_UnitTests, lmMainFrame::OnDebugUnitTests)
    EVT_MENU (MENU_Debug_ShowDirtyObjects, lmMainFrame::OnDebugShowDirtyObjects)
        //debug events requiring a score to be enabled
    EVT_MENU      (MENU_Debug_DumpStaffObjs, lmMainFrame::OnDebugDumpStaffObjs)
    EVT_UPDATE_UI (MENU_Debug_DumpStaffObjs, lmMainFrame::OnDebugScoreUI)
    EVT_MENU      (MENU_Debug_DumpGMObjects, lmMainFrame::OnDebugDumpGMObjects)
    EVT_UPDATE_UI (MENU_Debug_DumpGMObjects, lmMainFrame::OnDebugScoreUI)
    EVT_MENU      (MENU_Debug_SeeSource, lmMainFrame::OnDebugSeeSource)
    EVT_UPDATE_UI (MENU_Debug_SeeSource, lmMainFrame::OnDebugScoreUI)
    EVT_MENU      (MENU_Debug_SeeSourceUndo, lmMainFrame::OnDebugSeeSourceForUndo)
    EVT_UPDATE_UI (MENU_Debug_SeeSourceUndo, lmMainFrame::OnDebugScoreUI)
    EVT_MENU      (MENU_Debug_SeeXML, lmMainFrame::OnDebugSeeXML)
    EVT_UPDATE_UI (MENU_Debug_SeeXML, lmMainFrame::OnDebugScoreUI)
    EVT_MENU      (MENU_Debug_SeeMIDIEvents, lmMainFrame::OnDebugSeeMidiEvents)
    EVT_UPDATE_UI (MENU_Debug_SeeMIDIEvents, lmMainFrame::OnDebugScoreUI)
    EVT_MENU      (MENU_Debug_DumpBitmaps, lmMainFrame::OnDebugDumpBitmaps)
    EVT_UPDATE_UI (MENU_Debug_DumpBitmaps, lmMainFrame::OnDebugScoreUI)
    EVT_MENU      (MENU_Debug_CheckHarmony, lmMainFrame::OnDebugCheckHarmony)
    EVT_UPDATE_UI (MENU_Debug_CheckHarmony, lmMainFrame::OnDebugScoreUI)
    EVT_MENU      (MENU_Debug_TestProcessor, lmMainFrame::OnDebugTestProcessor)
    EVT_UPDATE_UI (MENU_Debug_TestProcessor, lmMainFrame::OnDebugScoreUI)
#endif



    //metronome
    EVT_SPINCTRL    (lmID_SPIN_METRONOME, lmMainFrame::OnMetronomeUpdate)
    EVT_TEXT        (lmID_SPIN_METRONOME,    lmMainFrame::OnMetronomeUpdateText)
    EVT_TIMER       (lmID_TIMER_MTR,        lmMainFrame::OnMetronomeTimer)

    //TextBookFrame
    EVT_TOOL_RANGE(lmMENU_eBookPanel, lmMENU_eBook_OpenFile, lmMainFrame::OnBookFrame)
    EVT_UPDATE_UI_RANGE (lmMENU_eBookPanel, lmMENU_eBook_OpenFile, lmMainFrame::OnBookFrameUpdateUI)

    //other events
    EVT_CLOSE(lmMainFrame::OnCloseWindow)

END_EVENT_TABLE()

lmMainFrame::lmMainFrame(lmDocManager* pDocManager, wxFrame* pFrame, const wxString& sTitle,
                         const wxPoint& pos, const wxSize& size, long style)
    : lmDocTDIParentFrame(pDocManager, pFrame, -1, sTitle, pos, size, style, _T("myFrame"))
    , m_fClosingAll(false)
    , m_pToolBox((lmToolBox*) NULL)
    , m_pWelcomeWnd((lmWelcomeWnd*)NULL)
    , m_pHelp((lmHelpController*) NULL)
    , m_pBookController((lmTextBookController*) NULL)
	, m_pTbTextBooks((wxToolBar*) NULL)
    , m_pHtmlWin((lmHtmlWindow*) NULL)

{
    // set the app icon
	// All non-MSW platforms use a bitmap. MSW uses an .ico file
    #if defined(__WXMSW__)
        //macro wxICON creates an icon using an icon resource on Windows.
        SetIcon(wxICON(app_icon));
	#else
		SetIcon(wxArtProvider::GetIcon(_T("app_icon"), wxART_OTHER));
	#endif

	//acceleration keys table
    wxAcceleratorEntry entries[1];
    entries[0].Set(wxACCEL_CTRL, WXK_F1, wxID_ABOUT);
    wxAcceleratorTable accel(1, entries);
    SetAcceleratorTable(accel);

    //load recent files
    m_pRecentFiles = new wxFileHistory();
    LoadRecentFiles();

	// create main metronome and associate it to frame metronome controls
    //metronome speed. Default MM=60
    long nMM = g_pPrefs->Read(_T("/Metronome/MM"), 60);
    m_pMainMtr = new lmMetronome(nMM);
    m_pMtr = m_pMainMtr;

    // create main menu
	m_pMenuEdit = (wxMenu*)NULL;
    wxMenuBar* menu_bar = CreateMenuBar(NULL, NULL);
    SetMenuBar(menu_bar);

    // initialize tool bars
    m_pToolbar = (wxToolBar*) NULL;
    m_pTbPlay = (wxToolBar*)NULL;
    m_pTbMtr = (wxToolBar*)NULL;
    m_pTbFile = (wxToolBar*)NULL;
    m_pTbEdit = (wxToolBar*)NULL;
    m_pTbZoom = (wxToolBar*)NULL;
    m_pTbTextBooks = (wxToolBar*)NULL;

    // initialize status bar
    m_pStatusBar = (lmStatusBar*)NULL;
    bool fStatusBar = false;
    g_pPrefs->Read(_T("/MainFrame/ViewStatusBar"), &fStatusBar);
    if (!m_pStatusBar && fStatusBar) {
        // create a status bar (by default with 1 pane only)
        CreateTheStatusBar();
        m_pStatusBar->SetMsgText(_("Welcome to LenMus!"));
    }

    // initialize flags for toggle buttons status
    m_fHelpOpened = false;

    // other initializations
    m_fSilentCheck = false;     //default: visible 'check for updates' process

    //TODO metronome LED
    // Set picMetronomoOn = LoadResPicture("METRONOMO_ON", vbResBitmap)
    //Set picMetronomoOff = LoadResPicture("METRONOMO_OFF", vbResBitmap)
    //configurar controles en el frame Metrónomo
    //picMtrLEDOff.Visible = True
    //Me.picMtrLEDRojoOn.Visible = False
    //picMtrLEDRojoOn.Top = Me.picMtrLEDOff.Top
    //picMtrLEDRojoOn.Left = Me.picMtrLEDOff.Left

}

void lmMainFrame::LoadRecentFiles()
{
    //m_pRecentFiles->Load( *wxConfigBase::Get() );
    //wxFileHistory::Load() does not use any key to look for the files and this causes problems.
    //So lets do it here
    for (int nFile = 1; nFile <= 9; nFile++)
    {
        wxString sKey = wxString::Format(_T("/RecentFiles/file%d"), nFile);
        wxString sFile = g_pPrefs->Read(sKey, _T(""));
        if (sFile.empty())
            break;
        m_pRecentFiles->AddFileToHistory(sFile);
    }

    //if no recent files, load some samples
    if (m_pRecentFiles->GetCount() == 0)
    {
        wxString sPath = g_pPaths->GetSamplesPath();
        wxFileName oFile1(sPath, _T("greensleeves_v15.lms"));
        wxFileName oFile2(sPath, _T("chopin_prelude20_v15.lms"));
        wxFileName oFile3(sPath, _T("beethoven_moonlight_sonata_v15.lms"));
        wxLogMessage(_T("[lmMainFrame::LoadRecentFiles] sPath='%s', sFile1='%s'"),
                     sPath.c_str(), oFile1.GetFullPath().c_str() );
        m_pRecentFiles->AddFileToHistory(oFile1.GetFullPath());
        m_pRecentFiles->AddFileToHistory(oFile2.GetFullPath());
        m_pRecentFiles->AddFileToHistory(oFile3.GetFullPath());
    }
}

void lmMainFrame::SaveRecentFiles()
{
    //wxFileHistory is not using a key to save the files, and this causes problems. Therefore
    //I will implement my own function

    if (!m_pRecentFiles) return;

    int nNumFiles = m_pRecentFiles->GetCount();
    for (int i = 1; i <= 9; i++)
    {
        wxString buf;
        buf.Printf(_T("/RecentFiles/file%d"), i);
        if (i <= nNumFiles)
            g_pPrefs->Write(buf, m_pRecentFiles->GetHistoryFile(i-1));
        else
            g_pPrefs->Write(buf, wxEmptyString);
    }
}

void lmMainFrame::CreateControls()
{
    // notify wxAUI which frame to use
    m_mgrAUI.SetManagedWindow(this);

    //menu bars and other aui panes creation
    bool fToolBar = true;
    g_pPrefs->Read(_T("/MainFrame/ViewToolBar"), &fToolBar);
    if (!m_pToolbar && fToolBar) {
        CreateMyToolBar();
    }

    //create the MDI client window
    long style = wxAUI_NB_CLOSE_ON_ACTIVE_TAB |
                 wxAUI_NB_WINDOWLIST_BUTTON |
                 wxAUI_NB_SCROLL_BUTTONS |
                 wxAUI_NB_TAB_MOVE ;

    m_pClientWindow = new lmTDIClientWindow(this, style);

    m_mgrAUI.AddPane(m_pClientWindow, wxAuiPaneInfo().Name(wxT("notebook")).
                  CenterPane().PaneBorder(false));

	//AUI options
    unsigned int flags = wxAUI_MGR_ALLOW_FLOATING |
                        wxAUI_MGR_TRANSPARENT_HINT |
                        wxAUI_MGR_HINT_FADE |
						wxAUI_MGR_ALLOW_ACTIVE_PANE |
                        wxAUI_MGR_NO_VENETIAN_BLINDS_FADE;

	#if !defined(__WXMSW__) && !defined(__WXMAC__) && !defined(__WXGTK__)
	//This option is only available on wxGTK, wxMSW and wxMac
	flags |= wxAUI_MGR_TRANSPARENT_DRAG;
	#endif

	m_mgrAUI.SetFlags(flags);

	m_mgrAUI.Update();

}

void lmMainFrame::OnMetronomeOnOff(wxCommandEvent& WXUNUSED(event))
{
    if (m_pMtr->IsRunning()) {
        m_pMtr->Stop();
        //TODO switch off metronome LED
    }
    else {
        m_pMtr->Start();
    }

}

// Recreate toolbars if visible. User has changed visualization options
void lmMainFrame::UpdateToolbarsLayout()
{
	if (m_pToolbar) {
		DeleteToolbar();
		CreateMyToolBar();
	}
}

void lmMainFrame::CreateMyToolBar()
{
    // return if exists
    if (m_pToolbar) return;

    //prepare style
    long style = wxTB_FLAT | wxTB_NODIVIDER;
    long nLabelsIndex = g_pPrefs->Read(_T("/Toolbars/Labels"), 0L);
    if (nLabelsIndex == 1)
        style |= wxTB_TEXT;
    else if (nLabelsIndex == 2)
        style |= wxTB_HORZ_TEXT;

    //prepare icons size
    long nIconSize = g_pPrefs->Read(_T("/Toolbars/IconSize"), 16);
    wxSize nSize(nIconSize, nIconSize);


    //create main tool bar
    m_pToolbar = new wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize, style);
    m_pToolbar->SetToolBitmapSize(nSize);
    m_pToolbar->AddTool(MENU_Preferences, _T("Preferences"), wxArtProvider::GetBitmap(_T("tool_options"), wxART_TOOLBAR, nSize), _("Set user preferences"));
    m_pToolbar->AddTool(MENU_Help_Open, _T("Help"), wxArtProvider::GetBitmap(_T("tool_help"), wxART_TOOLBAR, nSize), _("Help button"));
    m_pToolbar->Realize();

    //File toolbar
    m_pTbFile = new wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize, style);
    m_pTbFile->SetToolBitmapSize(nSize);
    m_pTbFile->AddTool(MENU_File_New, _T("New"),
            wxArtProvider::GetBitmap(_T("tool_new"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_new_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("New score"));
    m_pTbFile->AddTool(MENU_File_Open, _T("Open"), wxArtProvider::GetBitmap(_T("tool_open"),
            wxART_TOOLBAR, nSize), _("Open a score"));
    m_pTbFile->AddTool(MENU_OpenBook, _T("Books"),
            wxArtProvider::GetBitmap(_T("tool_open_ebook"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_open_ebook_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Open the music books"));
    m_pTbFile->AddTool(wxID_SAVE, _T("Save"),
            wxArtProvider::GetBitmap(_T("tool_save"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_save_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Save current score to disk"));
    m_pTbFile->AddTool(MENU_Print, _T("Print"),
            wxArtProvider::GetBitmap(_T("tool_print"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_print_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Print document"));
    m_pTbFile->Realize();

    //Edit toolbar
    m_pTbEdit = new wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize, style);
    m_pTbEdit->SetToolBitmapSize(nSize);
    m_pTbEdit->AddTool(MENU_Edit_Copy, _T("Copy"),
            wxArtProvider::GetBitmap(_T("tool_copy"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_copy_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Copy"));
    m_pTbEdit->AddTool(MENU_Edit_Cut, _T("Cut"),
            wxArtProvider::GetBitmap(_T("tool_cut"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_cut_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Cut"));
    m_pTbEdit->AddTool(MENU_Edit_Paste, _T("Paste"),
            wxArtProvider::GetBitmap(_T("tool_paste"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_paste_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Paste"));
    m_pTbEdit->AddTool(wxID_UNDO, _T("Undo"),
            wxArtProvider::GetBitmap(_T("tool_undo"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_undo_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Undo"));
    m_pTbEdit->AddTool(wxID_REDO, _T("Redo"),
            wxArtProvider::GetBitmap(_T("tool_redo"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_redo_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Redo"));
    m_pTbEdit->AddSeparator();
    m_pTbEdit->AddTool(MENU_View_Page_Margins, _T("Page margins"),
            wxArtProvider::GetBitmap(_T("tool_page_margins"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_page_margins"), wxART_TOOLBAR, nSize),
            wxITEM_CHECK, _("Show/hide page margins and spacers"));
    m_pTbEdit->Realize();

    //Zoom toolbar
    m_pTbZoom = new wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize, style);
    m_pTbZoom->SetToolBitmapSize(nSize);
    m_pTbZoom->AddTool(MENU_Zoom_Fit_Full, _T("Fit full"),
            wxArtProvider::GetBitmap(_T("tool_zoom_fit_full"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_zoom_fit_full_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Zoom so that the full page is displayed"));
    m_pTbZoom->AddTool(MENU_Zoom_Fit_Width, _T("Fit width"),
            wxArtProvider::GetBitmap(_T("tool_zoom_fit_width"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_zoom_fit_width_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Zoom so that page width equals window width"));
    m_pTbZoom->AddTool(MENU_Zoom_Increase, _T("Zoom in"),
            wxArtProvider::GetBitmap(_T("tool_zoom_in"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_zoom_in_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Enlarge image size"));
    m_pTbZoom->AddTool(MENU_Zoom_Decrease, _T("Zoom out"),
            wxArtProvider::GetBitmap(_T("tool_zoom_out"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_zoom_out_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Reduce image size"));

    m_pComboZoom = new wxComboBox(m_pTbZoom, lmID_COMBO_ZOOM, _T(""),
                                  wxDefaultPosition, wxSize(70, -1) );
    m_pComboZoom->Append(_T("25%"));
    m_pComboZoom->Append(_T("50%"));
    m_pComboZoom->Append(_T("75%"));
    m_pComboZoom->Append(_T("90%"));
    m_pComboZoom->Append(_T("100%"));
    m_pComboZoom->Append(_T("110%"));
    m_pComboZoom->Append(_T("120%"));
    m_pComboZoom->Append(_T("133%"));
    m_pComboZoom->Append(_T("150%"));
    m_pComboZoom->Append(_T("175%"));
    m_pComboZoom->Append(_T("200%"));
    m_pComboZoom->Append(_T("300%"));
    m_pComboZoom->Append(_T("400%"));
    m_pComboZoom->Append(_T("800%"));
    m_pComboZoom->Append(_("Actual size"));         // tamaño real
    m_pComboZoom->Append(_("Fit page full"));       // toda la página
    m_pComboZoom->Append(_("Fit page width"));      // ancho de página
    m_pComboZoom->SetSelection(3);
    m_pTbZoom->AddControl(m_pComboZoom);
    m_pTbZoom->Realize();

    //Play toolbar
    m_pTbPlay = new wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize, style);
    m_pTbPlay->SetToolBitmapSize(nSize);
    m_pTbPlay->AddTool(MENU_Play_Start, _T("Play"),
            wxArtProvider::GetBitmap(_T("tool_play"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_play_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Start/resume play back. From selection of full score"));
    m_pTbPlay->AddTool(MENU_Play_Cursor_Start, _T("Play from cursor"),
            wxArtProvider::GetBitmap(_T("tool_play_cursor"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_play_cursor_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Start/resume play back. From cursor measure"));
    m_pTbPlay->AddTool(MENU_Play_Stop, _T("Stop"),
            wxArtProvider::GetBitmap(_T("tool_stop"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_stop_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Stop playing back"));
    m_pTbPlay->AddTool(MENU_Play_Pause, _T("Pause"),
            wxArtProvider::GetBitmap(_T("tool_pause"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_pause_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Pause playing back"));
    m_pTbPlay->Realize();

    //Metronome toolbar
    m_pTbMtr = new wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize, style);
    m_pTbMtr->SetToolBitmapSize(nSize);
    m_pTbMtr->AddTool(MENU_Metronome, _T("Metronome"),
        wxArtProvider::GetBitmap(_T("tool_metronome"),
        wxART_TOOLBAR, nSize), _("Turn metronome on/off"),
        wxITEM_CHECK);
    m_pSpinMetronome = new wxSpinCtrl(m_pTbMtr, lmID_SPIN_METRONOME, _T(""), wxDefaultPosition,
        wxSize(60, -1), wxSP_ARROW_KEYS | wxSP_WRAP, 20, 300);
    m_pSpinMetronome->SetValue( m_pMtr->GetMM() );
    m_pTbMtr->AddControl(m_pSpinMetronome);
    m_pTbMtr->Realize();

    //compute best size for metronome toolbar
    wxSize sizeSpin = m_pSpinMetronome->GetSize();
    wxSize sizeButton = m_pTbMtr->GetToolSize();
    wxSize sizeBest(sizeButton.GetWidth() + sizeSpin.GetWidth() +
                        m_pTbMtr->GetToolSeparation() + 10,
                    wxMax(sizeSpin.GetHeight(), sizeButton.GetHeight()));

    //compute best size for zoom toolbar
    wxSize sizeCombo = m_pComboZoom->GetSize();
    sizeButton = m_pTbZoom->GetToolSize();
    wxSize sizeZoomTb(4 * (sizeButton.GetWidth() + m_pTbZoom->GetToolSeparation()) +
                      sizeCombo.GetWidth() +
                      m_pTbZoom->GetToolSeparation() + 10,
                      wxMax(sizeCombo.GetHeight(), sizeButton.GetHeight()));

    // add the toolbars to the manager
	const int ROW_1 = 0;
	//const int ROW_2 = 1;
#if defined(__WXGTK__)
    //In gtk reverse creation order
        // row 1
    CreateTextBooksToolBar(style, nSize, ROW_1);

    m_mgrAUI.AddPane(m_pTbMtr, wxAuiPaneInfo().
                Name(wxT("Metronome")).Caption(_("Metronome tools")).
                ToolbarPane().Top().Row(ROW_1).BestSize( sizeBest ).
                LeftDockable(false).RightDockable(false));
    m_mgrAUI.AddPane(m_pTbPlay, wxAuiPaneInfo().
                Name(wxT("Play")).Caption(_("Play tools")).
                ToolbarPane().Top().Row(ROW_1).
                LeftDockable(false).RightDockable(false));
    m_mgrAUI.AddPane(m_pToolbar, wxAuiPaneInfo().
                Name(wxT("toolbar")).Caption(_("Main tools")).
                ToolbarPane().Top().
                LeftDockable(false).RightDockable(false));
    m_mgrAUI.AddPane(m_pTbZoom, wxAuiPaneInfo().
                Name(wxT("Zooming tools")).Caption(_("Zooming tools")).
                ToolbarPane().Top().BestSize( sizeZoomTb ).
                LeftDockable(false).RightDockable(false));
    m_mgrAUI.AddPane(m_pTbEdit, wxAuiPaneInfo().
                Name(wxT("Edit tools")).Caption(_("Edit tools")).
                ToolbarPane().Top().
                LeftDockable(false).RightDockable(false));
    m_mgrAUI.AddPane(m_pTbFile, wxAuiPaneInfo().
                Name(wxT("File tools")).Caption(_("File tools")).
                ToolbarPane().Top().
                LeftDockable(false).RightDockable(false));

#else
    // row 1
    m_mgrAUI.AddPane(m_pTbFile, wxAuiPaneInfo().
                Name(wxT("File tools")).Caption(_("File tools")).
                ToolbarPane().Top().
                LeftDockable(false).RightDockable(false));
    m_mgrAUI.AddPane(m_pTbEdit, wxAuiPaneInfo().
                Name(wxT("Edit tools")).Caption(_("Edit tools")).
                ToolbarPane().Top().
                LeftDockable(false).RightDockable(false));
    m_mgrAUI.AddPane(m_pTbZoom, wxAuiPaneInfo().
                Name(wxT("Zooming tools")).Caption(_("Zooming tools")).
                ToolbarPane().Top().BestSize( sizeZoomTb ).
                LeftDockable(false).RightDockable(false));
    m_mgrAUI.AddPane(m_pToolbar, wxAuiPaneInfo().
                Name(wxT("toolbar")).Caption(_("Main tools")).
                ToolbarPane().Top().
                LeftDockable(false).RightDockable(false));
    m_mgrAUI.AddPane(m_pTbPlay, wxAuiPaneInfo().
                Name(wxT("Play")).Caption(_("Play tools")).
                ToolbarPane().Top().Row(ROW_1).
                LeftDockable(false).RightDockable(false));
    m_mgrAUI.AddPane(m_pTbMtr, wxAuiPaneInfo().
                Name(wxT("Metronome")).Caption(_("Metronome tools")).
                ToolbarPane().Top().Row(ROW_1).BestSize( sizeBest ).
                LeftDockable(false).RightDockable(false));

    CreateTextBooksToolBar(style, nSize, ROW_1);

#endif

    // tell the manager to "commit" all the changes just made
    m_mgrAUI.Update();
}

void lmMainFrame::DeleteToolbar()
{
    // main toolbar
    if (m_pToolbar) {
        m_mgrAUI.DetachPane(m_pToolbar);
        delete m_pToolbar;
        m_pToolbar = (wxToolBar*)NULL;
    }

    // file toolbar
    if (m_pTbFile) {
        m_mgrAUI.DetachPane(m_pTbFile);
        delete m_pTbFile;
        m_pTbFile = (wxToolBar*)NULL;
    }

    // edit toolbar
    if (m_pTbEdit) {
        m_mgrAUI.DetachPane(m_pTbEdit);
        delete m_pTbEdit;
        m_pTbEdit = (wxToolBar*)NULL;
    }

    // play toolbar
    if (m_pTbPlay) {
        m_mgrAUI.DetachPane(m_pTbPlay);
        delete m_pTbPlay;
        m_pTbPlay = (wxToolBar*)NULL;
    }

    // metronome toolbar
    if (m_pTbMtr) {
        m_mgrAUI.DetachPane(m_pTbMtr);
        delete m_pTbMtr;
        m_pTbMtr = (wxToolBar*)NULL;
    }

    // zoom toolbar
    if (m_pTbZoom) {
        m_mgrAUI.DetachPane(m_pTbZoom);
        delete m_pTbZoom;
        m_pTbZoom = (wxToolBar*)NULL;
    }

    // Text books navigation toolbar
    if (m_pTbTextBooks) {
        m_mgrAUI.DetachPane(m_pTbTextBooks);
        delete m_pTbTextBooks;
        m_pTbTextBooks = (wxToolBar*)NULL;
    }

    // tell the manager to "commit" all the changes just made
    m_mgrAUI.Update();
}

void lmMainFrame::CreateTextBooksToolBar(long style, wxSize nIconSize, int nRow)
{
    m_pTbTextBooks = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
    m_pTbTextBooks->SetToolBitmapSize(nIconSize);

    //add tools
    m_pTbTextBooks->AddTool(lmMENU_eBookPanel, _T("Index"),
            wxArtProvider::GetBitmap(_T("tool_index_panel"), wxART_TOOLBAR, nIconSize),
            wxArtProvider::GetBitmap(_T("tool_index_panel_dis"), wxART_TOOLBAR, nIconSize),
            wxITEM_CHECK, _("Show/hide navigation panel") );
    m_pTbTextBooks->ToggleTool(lmMENU_eBookPanel, false);

    m_pTbTextBooks->AddSeparator();
    m_pTbTextBooks->AddTool(lmMENU_eBook_PagePrev, _T("Back page"),
            wxArtProvider::GetBitmap(_T("tool_page_previous"), wxART_TOOLBAR, nIconSize),
            wxArtProvider::GetBitmap(_T("tool_page_previous_dis"), wxART_TOOLBAR, nIconSize),
            wxITEM_NORMAL, _("Previous page of current eMusicBook") );
    m_pTbTextBooks->AddTool(lmMENU_eBook_PageNext, _T("Next page"),
            wxArtProvider::GetBitmap(_T("tool_page_next"), wxART_TOOLBAR, nIconSize),
            wxArtProvider::GetBitmap(_T("tool_page_next_dis"), wxART_TOOLBAR, nIconSize),
            wxITEM_NORMAL, _("Next page of current eMusicBook") );

    m_pTbTextBooks->AddSeparator();
    m_pTbTextBooks->AddTool(lmMENU_eBook_GoBack, _T("Go back"),
            wxArtProvider::GetBitmap(_T("tool_previous"), wxART_TOOLBAR, nIconSize),
            _("Go to previous visited page"), wxITEM_NORMAL );
    m_pTbTextBooks->AddTool(lmMENU_eBook_GoForward, _T("Go forward"),
            wxArtProvider::GetBitmap(_T("tool_next"), wxART_TOOLBAR, nIconSize),
            _("Go to next visited page"), wxITEM_NORMAL );

    m_pTbTextBooks->Realize();

    m_mgrAUI.AddPane(m_pTbTextBooks, wxAuiPaneInfo().
                Name(_T("Navigation")).Caption(_("eBooks navigation tools")).
                ToolbarPane().Top().Row(nRow).
                LeftDockable(false).RightDockable(false));

}

void lmMainFrame::CreateTheStatusBar(int nType)
{
    //if the status bar exists and it is of same type, nothing to do
    if (m_pStatusBar && m_pStatusBar->GetType() == nType) return;

    //create the status bar
    if (m_pStatusBar)
        delete m_pStatusBar;

    m_pStatusBar = new lmStatusBar(this, (lmEStatusBarLayout)nType, MENU_View_StatusBar);
    SetStatusBar(m_pStatusBar);

    //the status bar pane is used to display menu and toolbar help.
    //Using -1 disables help display.
    SetStatusBarPane(-1);

    SendSizeEvent();
}

void lmMainFrame::DeleteTheStatusBar()
{
    if (!m_pStatusBar) return;

    //delete status bar
    SetStatusBar(NULL);
    delete m_pStatusBar;
    m_pStatusBar = (lmStatusBar*)NULL;
    SendSizeEvent();
}

void lmMainFrame::AddMenuItem(wxMenu* pMenu, int nId, const wxString& sItemName,
                              const wxString& sToolTip, wxItemKind nKind,
                              const wxString& sIconName)
{
    //Create a menu item and add it to the received menu

    wxMenuItem* pItem = new wxMenuItem(pMenu, nId, sItemName, sToolTip, nKind);


    //icons are supported only in Windows and Linux, and only in wxITEM_NORMAL items
    #if defined(__WXMSW__) || defined(__WXGTK__)
    if (nKind == wxITEM_NORMAL)
        pItem->SetBitmap( wxArtProvider::GetBitmap(sIconName, wxART_TOOLBAR, wxSize(16, 16)) );
    #endif

    pMenu->Append(pItem);
}

wxMenuBar* lmMainFrame::CreateMenuBar(wxDocument* doc, wxView* pView)
{
    //Centralized code to create the menu bar
    //bitmaps on menus are supported only on Windows and GTK+

	bool fDebug = !g_fReleaseVersion;
    wxMenuItem* pItem;
    wxSize nIconSize(16, 16);

    // file menu --------------------------------------------------------------------------
    wxMenu* pMenuFile = new wxMenu;
    wxMenu* pSubmenuExport = new wxMenu;

    AddMenuItem(pMenuFile, MENU_File_New, _("&New\tCtrl+N"),
                _("Open new blank score"), wxITEM_NORMAL, _T("tool_new"));
    AddMenuItem(pMenuFile, MENU_File_Open, _("&Open ...\tCtrl+O"),
                _("Open a score"), wxITEM_NORMAL, _T("tool_open"));
    AddMenuItem(pMenuFile, MENU_OpenBook, _("Open &books"),
                _("Hide/show eMusicBooks"), wxITEM_NORMAL, _T("tool_open_ebook"));
    AddMenuItem(pMenuFile, MENU_File_Import, _("&Import..."),
                _("Open a MusicXML score"), wxITEM_NORMAL);

    //-- export submenu --
    AddMenuItem(pSubmenuExport, MENU_File_Export_MusicXML, _("MusicXML format"),
                _("Save score as a MusicXML file"), wxITEM_NORMAL);
    AddMenuItem(pSubmenuExport, MENU_File_Export_bmp, _("As &bmp image"),
                _("Save score as BMP images"), wxITEM_NORMAL, _T("tool_save_as_bmp"));
    AddMenuItem(pSubmenuExport, MENU_File_Export_jpg, _("As &jpg image"),
                _("Save score as JPG images"), wxITEM_NORMAL, _T("tool_save_as_jpg"));

    pItem = new wxMenuItem(pMenuFile, MENU_File_Export, _("&Export ..."),
                          _("Save score in other formats"), wxITEM_NORMAL, pSubmenuExport);
    pItem->SetBitmap( wxArtProvider::GetBitmap(_T("empty"), wxART_TOOLBAR, nIconSize) );
    pMenuFile->Append(pItem);

    //-- end of export submenu --

    AddMenuItem(pMenuFile, wxID_SAVE, _("&Save\tCtrl+S"),
                _T(""), wxITEM_NORMAL, _T("tool_save"));
    AddMenuItem(pMenuFile, wxID_SAVEAS, _("Save &as ..."),
                _T(""), wxITEM_NORMAL);
    AddMenuItem(pMenuFile, wxID_CLOSE, _("&Close\tCtrl+W"),
                _("Close a score"), wxITEM_NORMAL);
    pMenuFile->AppendSeparator();

    AddMenuItem(pMenuFile, MENU_Print, _("&Print ...\tCtrl+P"),
                _T(""), wxITEM_NORMAL, _T("tool_print"));
    AddMenuItem(pMenuFile, wxID_PRINT_SETUP, _("Print &Setup..."),
                _("Configure printer options"), wxITEM_NORMAL );
    AddMenuItem(pMenuFile, MENU_Print_Preview, _("Print Pre&view"),
                _T(""), wxITEM_NORMAL);
    pMenuFile->AppendSeparator();

    AddMenuItem(pMenuFile, wxID_EXIT, _("&Quit\tCtrl+Q"),
                _("Exit program"), wxITEM_NORMAL, _T("tool_exit"));


    // history of files visited.
    m_pRecentFiles->UseMenu(pMenuFile);
    m_pRecentFiles->AddFilesToMenu(pMenuFile);


    // edit menu -------------------------------------------------------------------

    m_pMenuEdit = new wxMenu;
    AddMenuItem(m_pMenuEdit, wxID_UNDO, _("&Undo"),
                _("Undo"), wxITEM_NORMAL, _T("tool_undo"));
    AddMenuItem(m_pMenuEdit, wxID_REDO, _("&Redo"),
                _("Redo"), wxITEM_NORMAL, _T("tool_redo"));


    // View menu -------------------------------------------------------------------

    wxMenu* pMenuView = new wxMenu;
    AddMenuItem(pMenuView, MENU_View_ToolBar, _("Tool &bar"),
                _("Hide/show the tools bar"), wxITEM_CHECK);
    AddMenuItem(pMenuView, MENU_View_StatusBar, _("&Status bar"),
                _("Hide/show the status bar"), wxITEM_CHECK);
    pMenuView->AppendSeparator();
    AddMenuItem(pMenuView, MENU_View_Tools, _("&Tool box"),
                _("Hide/show edition tool box window"), wxITEM_CHECK);
    AddMenuItem(pMenuView, MENU_View_Rulers, _("&Rulers"),
                _("Hide/show rulers"), wxITEM_CHECK);
    AddMenuItem(pMenuView, MENU_View_Welcome_Page, _("&Welcome page"),
                _("Hide/show welcome page"));


    // score menu ------------------------------------------------------------------

    wxMenu* pMenuScore = new wxMenu;
    AddMenuItem(pMenuScore, MENU_Score_Titles, _("Add title"),
				_("Add a title to the score"), wxITEM_NORMAL, _T("tool_add_text"));
    AddMenuItem(pMenuScore, MENU_View_Page_Margins, _("Margins and spacers"),
				_("Show/hide page margins and spacers"), wxITEM_CHECK, _T("tool_page_margins"));


    // instrument menu ------------------------------------------------------------------

    wxMenu* pMenuInstr = new wxMenu;
    AddMenuItem(pMenuInstr, MENU_Instr_Properties, _("Properties"),
				_("Edit name, abbreviation, MIDI settings and other properties"), wxITEM_NORMAL);


    // debug menu --------------------------------------------------------------------

    // Debug strings will not be translatable. It is mandatory that all development is
    // in English
    wxMenu* pMenuDebug;
    if (fDebug)
	{
        pMenuDebug = new wxMenu;

        AddMenuItem(pMenuDebug, MENU_Debug_ForceReleaseBehaviour, _T("&Release Behaviour"),
            _T("Force release behaviour for certain functions"), wxITEM_CHECK);
        AddMenuItem(pMenuDebug, MENU_Debug_ShowDebugLinks, _T("&Include debug links"),
            _T("Include debug controls in exercises"), wxITEM_CHECK);
        AddMenuItem(pMenuDebug, MENU_Debug_ShowBorderOnScores, _T("&Border on ScoreAuxCtrol"),
            _T("Show border on ScoreAuxCtrol"), wxITEM_CHECK);
        AddMenuItem(pMenuDebug, MENU_Debug_recSelec, _T("&Draw recSelec"),
            _T("Force to draw selection rectangles around staff objects"), wxITEM_CHECK);

        //-- Draw bounds submenu --
        wxMenu* pSubmenuDrawBounds = new wxMenu;

        AddMenuItem(pSubmenuDrawBounds, MENU_Debug_DrawBounds_BoxSystem, _T("BoxSystem bounds"),
            _T("Force to draw bound rectangles around BoxSystem objects"), wxITEM_CHECK);
        AddMenuItem(pSubmenuDrawBounds, MENU_Debug_DrawBounds_BoxSlice, _T("BoxSlice bounds"),
            _T("Force to draw bound rectangles around BoxSlice objects"), wxITEM_CHECK);
        AddMenuItem(pSubmenuDrawBounds, MENU_Debug_DrawBounds_BoxSliceInstr, _T("BoxSliceInstr bounds"),
            _T("Force to draw bound rectangles around BoxSliceInstr objects"), wxITEM_CHECK);
        AddMenuItem(pSubmenuDrawBounds, MENU_Debug_DrawBounds_BoundsShapes, _("Non-boxes: shapes bounds"),
            _T("Force to draw bound rectangles around shapes"), wxITEM_CHECK);

        pItem = new wxMenuItem(pMenuDebug, MENU_Debug_DrawBounds, _("Draw bounds ..."),
                            _("Force to draw bound rectangles"), wxITEM_NORMAL, pSubmenuDrawBounds);
        pMenuDebug->Append(pItem);


        //AddMenuItem(pMenuDebug, MENU_Debug_DrawBounds, _T("&Draw bounds"),
        //    _T("Force to draw bound rectangles around staff objects"), wxITEM_CHECK);
        AddMenuItem(pMenuDebug, MENU_Debug_DrawAnchors, _T("Draw anchors"),
            _T("Draw a red line to show anchor objects"), wxITEM_CHECK);
        AddMenuItem(pMenuDebug, MENU_Debug_ShowDirtyObjects, _T("&Show dirty objects"),
            _T("Render 'dirty' objects in red colour"), wxITEM_CHECK);
        AddMenuItem(pMenuDebug, MENU_Debug_SetTraceLevel, _T("Set trace level ...") );
        AddMenuItem(pMenuDebug, MENU_Debug_PatternEditor, _T("Test Pattern Editor") );
        AddMenuItem(pMenuDebug, MENU_Debug_DumpStaffObjs, _T("&Dump of score") );
		AddMenuItem(pMenuDebug, MENU_Debug_DumpGMObjects, _T("&Dump of graphical model") );
        AddMenuItem(pMenuDebug, MENU_Debug_SeeSource, _T("See &LDP source") );
        AddMenuItem(pMenuDebug, MENU_Debug_SeeSourceUndo, _T("See LDP source for &Undo/Redo") );
        AddMenuItem(pMenuDebug, MENU_Debug_SeeXML, _T("See &XML") );
        AddMenuItem(pMenuDebug, MENU_Debug_SeeMIDIEvents, _T("See &MIDI events") );
        AddMenuItem(pMenuDebug, MENU_Debug_DumpBitmaps, _T("Save offscreen bitmaps") );
        AddMenuItem(pMenuDebug, MENU_Debug_UnitTests, _T("Unit Tests") );
        AddMenuItem(pMenuDebug, MENU_Debug_CheckHarmony, _T("Check harmony") );
        AddMenuItem(pMenuDebug, MENU_Debug_TestProcessor, _T("Run test processor") );
    }


    // Zoom menu -----------------------------------------------------------------------

    wxMenu* pMenuZoom = new wxMenu;
    AddMenuItem(pMenuZoom, MENU_Zoom_100, _("Actual size"),
                _T("Zoom to real print size"), wxITEM_NORMAL, _T("tool_zoom_actual"));
    AddMenuItem(pMenuZoom, MENU_Zoom_Fit_Full, _("Fit page full"),
                _("Zoom so that the full page is displayed"), wxITEM_NORMAL,
                _T("tool_zoom_fit_full"));
    AddMenuItem(pMenuZoom, MENU_Zoom_Fit_Width, _("Fit page width"),
                _("Zoom so that page width equals window width"), wxITEM_NORMAL,
                _T("tool_zoom_fit_width"));
    AddMenuItem(pMenuZoom, MENU_Zoom_Increase, _T("Zoom in"),
                _("Enlarge image size"), wxITEM_NORMAL, _T("tool_zoom_in"));
    AddMenuItem(pMenuZoom, MENU_Zoom_Decrease, _T("Zoom out"),
                _("Reduce image size"), wxITEM_NORMAL, _T("tool_zoom_out"));
    AddMenuItem(pMenuZoom, MENU_Zoom_Other, _("Zoom to ..."));


    //Sound menu -------------------------------------------------------------------------

    wxMenu* pMenuSound = new wxMenu;

    AddMenuItem(pMenuSound, MENU_Play_Start, _("&Play"),
                _("Start/resume play back. From selection of full score"), wxITEM_NORMAL,
                _T("tool_play"));
    AddMenuItem(pMenuSound, MENU_Play_Cursor_Start, _("Play from cursor"),
                _("Start/resume play back. From cursor measure"), wxITEM_NORMAL,
                _T("tool_play_cursor"));
    AddMenuItem(pMenuSound, MENU_Play_Stop, _("S&top"),
                _("Stop playing back"), wxITEM_NORMAL, _T("tool_stop"));
    AddMenuItem(pMenuSound, MENU_Play_Pause, _("P&ause"),
                _("Pause playing back"), wxITEM_NORMAL, _T("tool_pause"));
    pMenuSound->AppendSeparator();

    AddMenuItem(pMenuSound, MENU_Sound_MidiWizard, _("&Run Midi wizard"),
                _("MIDI configuration wizard"), wxITEM_NORMAL, _T("tool_midi_wizard"));
	pMenuSound->AppendSeparator();

    AddMenuItem(pMenuSound, MENU_Sound_test, _("&Test sound"),
                _("Play an scale to test sound"), wxITEM_NORMAL, _T("tool_test_sound"));
    AddMenuItem(pMenuSound, MENU_Sound_AllSoundsOff, _("&All sounds off"),
                _("Stop inmediatly all sounds"), wxITEM_NORMAL, _T("tool_stop_sounds"));


    // Options menu ---------------------------------------------------------------------

    wxMenu* pMenuOptions = new wxMenu;
    AddMenuItem(pMenuOptions, MENU_Preferences,  _("&Preferences"),
                _("Open help book"), wxITEM_NORMAL, _T("tool_options"));


    // Window menu -----------------------------------------------------------------------
    wxMenu* pMenuWindow = new wxMenu;
    pMenuWindow->Append(MENU_WindowClose,    _("Cl&ose"));
    pMenuWindow->Append(MENU_WindowCloseAll, _("Close All"));
    pMenuWindow->AppendSeparator();

    pMenuWindow->Append(MENU_WindowNext,     _("&Next"));
    pMenuWindow->Append(MENU_WindowPrev,     _("&Previous"));


    // Help menu -------------------------------------------------------------------------

    wxMenu* pMenuHelp = new wxMenu;

    AddMenuItem(pMenuHelp, MENU_Help_About, _("&About"),
				_("Display information about program version and credits"), wxITEM_NORMAL,
                _T("tool_about"));
    pMenuHelp->AppendSeparator();

    AddMenuItem(pMenuHelp, MENU_Help_QuickGuide,  _("Editor quick guide"),
                _("Show editor reference card"), wxITEM_NORMAL, _T("tool_quick_guide"));
    AddMenuItem(pMenuHelp, MENU_Help_Open,  _("&Content"),
                _("Open help book"), wxITEM_NORMAL, _T("tool_help"));
    pMenuHelp->AppendSeparator();

	AddMenuItem(pMenuHelp, lmMENU_CheckForUpdates, _("Check now for &updates"),
				_("Connect to the Internet and check for program updates"), wxITEM_NORMAL,
                _T("tool_web_update"));
    AddMenuItem(pMenuHelp, MENU_VisitWebsite,  _("&Visit LenMus website"),
                _("Open the Internet browser and go to LenMus website"), wxITEM_NORMAL,
                _T("tool_website"));


    // set up the menubar ---------------------------------------------------------------

    // AWARE: As lmMainFrame is derived from lmTDIParentFrame, in MSWindows build the menu
    // bar automatically inherits a "Window" menu inserted in the second last position.
    // To suppress it (under MSWindows) it is necessary to add style wxFRAME_NO_WINDOW_MENU
    // in frame creation.
    wxMenuBar* pMenuBar = new wxMenuBar;
    pMenuBar->Append(pMenuFile, _("&File"));
    pMenuBar->Append(m_pMenuEdit, _("&Edit"));
    pMenuBar->Append(pMenuView, _("&View"));
	pMenuBar->Append(pMenuScore, _("S&core"));
	pMenuBar->Append(pMenuInstr, _("&Instrument"));
    pMenuBar->Append(pMenuSound, _("&Sound"));
    if (fDebug) pMenuBar->Append(pMenuDebug, _T("&Debug"));     //DO NOT TRANSLATE
    pMenuBar->Append(pMenuZoom, _("&Zoom"));
    pMenuBar->Append(pMenuOptions, _("&Options"));
    pMenuBar->Append(pMenuWindow, _("&Window"));
    pMenuBar->Append(pMenuHelp, _("&Help"));

        //
        // items initially checked
        //

    g_fDrawSelRect = false;    //true;

    //debug toolbar
    if (fDebug) {
        pMenuBar->Check(MENU_Debug_ForceReleaseBehaviour, g_fReleaseBehaviour);
        pMenuBar->Check(MENU_Debug_ShowDebugLinks, g_fShowDebugLinks);
        pMenuBar->Check(MENU_Debug_recSelec, g_fDrawSelRect);
        pMenuBar->Check(MENU_Debug_DrawAnchors, g_fDrawAnchors);
    }

    // view toolbar
    bool fToolBar = true;
    g_pPrefs->Read(_T("/MainFrame/ViewToolBar"), &fToolBar);
    pMenuBar->Check(MENU_View_ToolBar, fToolBar);

    // view status bar
    bool fStatusBar = true;
    g_pPrefs->Read(_T("/MainFrame/ViewStatusBar"), &fStatusBar);
    pMenuBar->Check(MENU_View_StatusBar, fStatusBar);

    return pMenuBar;
}

lmMainFrame::~lmMainFrame()
{
    // deinitialize the frame manager
    m_mgrAUI.UnInit();


    if (m_pHelp) delete m_pHelp;
    if (m_pBookController) delete m_pBookController;

    // save user configuration data
    if (g_pPrefs) {

        // save the frame size and position
        wxSize wndSize = GetSize();
        wxPoint wndPos = GetPosition();
        bool fMaximized = IsMaximized();
        g_pPrefs->Write(_T("/MainFrame/Width"), wndSize.GetWidth());
        g_pPrefs->Write(_T("/MainFrame/Height"), wndSize.GetHeight());
        g_pPrefs->Write(_T("/MainFrame/Left"), wndPos.x );
        g_pPrefs->Write(_T("/MainFrame/Top"), wndPos.y );
        g_pPrefs->Write(_T("/MainFrame/Maximized"), fMaximized);
    }

    //save metronome settings and delete main metronome
    if (m_pMainMtr) {
        g_pPrefs->Write(_T("/Metronome/MM"), m_pMainMtr->GetMM() );
        delete m_pMainMtr;
    }

    //save and delete other objects
    SaveRecentFiles();
    delete m_pRecentFiles;
}

void lmMainFrame::OnCloseBookFrame()
{
    //the TexBookFrame has been closed. Clean up
    delete m_pBookController;
    m_pBookController = (lmTextBookController*)NULL;
}

void lmMainFrame::InitializeHelp()
{
    // create the help window
    //m_pHelp = new lmHelpController(wxHF_DEFAULT_STYLE | wxHF_FLAT_TOOLBAR );
    // previous sentence commented out and replaced by next one to remove
    // index panel.
    m_pHelp = new lmHelpController(wxHF_TOOLBAR | wxHF_FLAT_TOOLBAR | wxHF_CONTENTS |
                        wxHF_SEARCH | wxHF_BOOKMARKS | wxHF_PRINT);
    // set the config object
    m_pHelp->UseConfig(wxConfig::Get(), _T("HelpController"));

    //set directory for cache files. TODO: allow user to set up any other directory
    m_pHelp->SetTempDir( g_pPaths->GetTempPath() );

    //In release versions helpfile will be a single .htb file, precompiled in cache format,
    //located in locale folder
    //In test versions, documentation will be in native hhp format, located in /help folder.
    //For testing purposes,
    //it must be possible to switch to "release mode" (use of cached htb files).

    wxString sPath;
    wxString sExt;
    if (g_fReleaseVersion || g_fReleaseBehaviour) {
        //Release behaviour. Use precompiled cached .htb files and don't show title
        sPath = g_pPaths->GetLocalePath();
        m_pHelp->SetTitleFormat(_("LenMus help"));
        sExt = _T("htb");
    }
    else {
        //Test behaviour. Use native .hhp, .hhc, .hhk and .htm files
        sPath = g_pPaths->GetHelpPath();
        m_pHelp->SetTitleFormat(_("Test mode: using .hhp help file"));
        sExt = _T("hhp");
    }
    // set base path for help documentation and load content
    wxFileName oFilename(sPath, _T("help"), sExt, wxPATH_NATIVE);
    bool fOK = m_pHelp->AddBook(oFilename);
    if (! fOK)
        wxMessageBox(wxString::Format(_("Failed adding book %s"),
            oFilename.GetFullPath().c_str() ));

}

void lmMainFrame::InitializeBooks()
{
    // create the books window
    m_pBookController = new lmTextBookController();

    // set the config object
    m_pBookController->UseConfig(wxConfig::Get(), _T("TextBooksController"));

    //set directory for cache files.
    m_pBookController->SetTempDir( g_pPaths->GetTempPath() );
    m_pBookController->SetTitleFormat(_("Available books"));

    // eMusicBooks are a single .lmb (LenMus Book) file
    // All eMusicBooks are located in 'book' folder
    wxString sPath = g_pPaths->GetBooksPath();
    wxString sPattern = _T("*.lmb");
    ScanForBooks(sPath, sPattern);

}

void lmMainFrame::ShowWelcomeWindow()
{
    //show/hide welcome window

    if (!m_pWelcomeWnd)
    {
        //show welcome window
        m_pWelcomeWnd =  new lmWelcomeWnd(this, wxID_ANY);
	    m_pWelcomeWnd->SetFocus();
    }
    else
    {
        //hide welcome window
        m_pWelcomeWnd = (lmWelcomeWnd*)NULL;
    }

}

void lmMainFrame::AddFileToHistory(const wxString& filename)
{
    if (m_pRecentFiles)
        m_pRecentFiles->AddFileToHistory(filename);
}

void lmMainFrame::OnOpenRecentFile(wxCommandEvent &event)
{
    if (m_pRecentFiles)
    {
        wxString sFile(m_pRecentFiles->GetHistoryFile(event.GetId() - wxID_FILE1));
        OpenRecentFile(sFile);
    }
}

void lmMainFrame::OpenRecentFile(wxString sFile)
{
    if (!sFile.empty())
    {
        ShowToolBox(true);      //force to display ToolBox
        m_pDocManager->OpenFile(sFile);
    }
}

void lmMainFrame::OnCloseWelcomeWnd()
{
    //the welcome window has been closed. Clean up
    m_pWelcomeWnd = (lmWelcomeWnd*)NULL;
}

//Scan the received folder for books and load all books found
void lmMainFrame::ScanForBooks(wxString sPath, wxString sPattern)
{
    //wxLogMessage(_T("[lmMainFrame::ScanForBooks] Scanning path <%s>"), sPath);
    wxDir dir(sPath);
    if ( !dir.IsOpened() ) {
        // TODO: deal with the error here - wxDir would already log an error message
        // explaining the exact reason of the failure
        wxMessageBox(wxString::Format(_("Error when trying to move to folder %s"),
            sPath.c_str() ));
        return;
    }

    // Add firts the 'intro' eBook
    wxFileName oFileIntro(sPath, _T("intro"), _T("lmb"), wxPATH_NATIVE);
    if (!m_pBookController->AddBook(oFileIntro)) {
        //TODO better error handling
        wxMessageBox(wxString::Format(_("Failed adding book %s"),
            oFileIntro.GetFullPath().c_str() ));
    }

    // Second, the 'General Exercises' eBook
    wxFileName oFileExercises(sPath, _T("GeneralExercises"), _T("lmb"), wxPATH_NATIVE);
    if (!m_pBookController->AddBook(oFileExercises)) {
        //TODO better error handling
        wxMessageBox(wxString::Format(_("Failed adding book %s"),
            oFileExercises.GetFullPath().c_str() ));
    }

    // Add now any other eBook found on this folder

    //wxLogMessage(wxString::Format(
    //    _T("Enumerating .hhp files in directory: %s"), sPath));
    wxString sFilename;
    bool fFound = dir.GetFirst(&sFilename, sPattern, wxDIR_FILES);
    while (fFound) {
        //wxLogMessage(_T("[lmMainFrame::ScanForBooks] Encontrado %s"), sFilename);
        wxFileName oFilename(sPath, sFilename, wxPATH_NATIVE);
        if (oFilename.GetName() != _T("help") && oFilename.GetName() != _T("intro") &&
			oFilename.GetName() != _T("GeneralExercises")) {
            if (!m_pBookController->AddBook(oFilename)) {
                //TODO better error handling
                wxMessageBox(wxString::Format(_("Failed adding book %s"),
                    oFilename.GetFullPath().c_str() ));
            }
        }
        fFound = dir.GetNext(&sFilename);
    }

}

void lmMainFrame::SilentlyCheckForUpdates(bool fSilent)
{
    m_fSilentCheck = fSilent;
}

lmController* lmMainFrame::GetActiveController()
{
	//returns the controller associated to the active view

    lmTDIChildFrame* pChild = GetActiveChild();
	if (pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame)) )
    {
        lmScoreView* pView = ((lmEditFrame*)pChild)->GetView();
        if (pView)
            return pView->GetController();
    }
    return (lmController*)NULL;
}

// ----------------------------------------------------------------------------
// menu callbacks
// ----------------------------------------------------------------------------
void lmMainFrame::OnBookFrame(wxCommandEvent& event)
{
    lmTextBookFrame* pBookFrame = m_pBookController->GetFrame();
    pBookFrame->OnToolbar(event);
    event.Skip(false);      //no further processing
}

void lmMainFrame::OnBookFrameUpdateUI(wxUpdateUIEvent& event)
{
    //enable only if current active view is TextBookFrame class
    lmTDIChildFrame* pChild = GetActiveChild();
    bool fEnabled = pChild && pChild->IsKindOf(CLASSINFO(lmTextBookFrame)) &&
                    m_pBookController;

    if (fEnabled) {
        // TextBookFrame is visible. Enable/disable buttons
        lmTextBookFrame* pBookFrame = m_pBookController->GetFrame();
        if (pBookFrame)
            pBookFrame->UpdateUIEvent(event, m_pTbTextBooks);
    }
    else {
        // Bug in wxWidgets: if the button is a check button, disabled image is not
        // set unless the button is unchecked. To bypass this bug, first uncheck button
        // then disable it and finally, restore check state
        bool fChecked = event.GetChecked();
        event.Check(false);
        event.Enable(false);
        event.Check(fChecked);
    }
}

void lmMainFrame::OnVisitWebsite(wxCommandEvent& WXUNUSED(event))
{
    LaunchDefaultBrowser( _T("www.lenmus.org") );
}

void lmMainFrame::OnCheckForUpdates(wxCommandEvent& WXUNUSED(event))
{
    lmUpdater oUpdater;
    oUpdater.CheckForUpdates(this, m_fSilentCheck);

    //force a visible 'check for updates' process unless previously reset flag
    SilentlyCheckForUpdates(false);
}

void lmMainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
   lmAboutDialog dlg(this);
   dlg.ShowModal();
}

void lmMainFrame::OnHelpQuickGuide(wxCommandEvent& WXUNUSED(event))
{
    wxString sPath = g_pPaths->GetLocalePath();
    wxFileName oFile(sPath, _T("editor_quick_guide.htm"), wxPATH_NATIVE);
	if (!oFile.FileExists())
	{
		//use english version
		sPath = g_pPaths->GetLocaleRootPath();
		oFile.AssignDir(sPath);
		oFile.AppendDir(_T("en"));
		oFile.SetFullName(_T("editor_quick_guide.htm"));
	}
    ::wxLaunchDefaultBrowser( oFile.GetFullPath() );
}

void lmMainFrame::OnExportMusicXML(wxCommandEvent& WXUNUSED(event))
{
	//TODO
}

void lmMainFrame::OnExportBMP(wxCommandEvent& WXUNUSED(event))
{
    ExportAsImage(wxBITMAP_TYPE_BMP);
}

void lmMainFrame::OnExportJPG(wxCommandEvent& WXUNUSED(event))
{
    ExportAsImage(wxBITMAP_TYPE_JPEG);
}

void lmMainFrame::ExportAsImage(int nImgType)
{
    wxString sExt;
    wxString sFilter = _T("*.");

    if (nImgType == wxBITMAP_TYPE_BMP) {
        sExt = _T("bmp");
    }
    else if (nImgType == wxBITMAP_TYPE_JPEG) {
        sExt = _T("jpg");
    }
    else if (nImgType == wxBITMAP_TYPE_PNG) {
        sExt = _T("png");
    }
    else if (nImgType == wxBITMAP_TYPE_PCX) {
        sExt = _T("pcx");
    }
    else if (nImgType == wxBITMAP_TYPE_PNM) {
        sExt = _T("pnm");
    }
    else
        wxASSERT(false);

    sFilter += sExt;

    // ask for the name to give to the exported file
    wxFileDialog dlg(this,
                     _("Name for the exported file"),
                     _T(""),    //default path
                     _T(""),    //default filename
                     sFilter,
                     wxFD_SAVE | wxFD_OVERWRITE_PROMPT);        //flags

    if (dlg.ShowModal() == wxID_CANCEL)
        return;

    wxString sFilename = dlg.GetFilename();
    if ( !sFilename.IsEmpty() )
    {
        //remove extension including dot
        wxString sName = sFilename.Left( sFilename.length() - sExt.length() - 1 );
        lmScoreView* pView = GetActiveScoreView();
        pView->SaveAsImage(sName, sExt, nImgType);
    }

}

void lmMainFrame::OnHelpOpen(wxCommandEvent& event)
{
    if (m_fHelpOpened) {
        //The help is open. Close it.
        wxASSERT(m_pHelp);
        m_pHelp->Quit();
        delete m_pHelp;
        m_pHelp = (lmHelpController*)NULL;
        m_fHelpOpened = false;
    }
    else {
        // open help

        // in case the previous window was closed directly, the controller still
        // exists. So delete the old controller
        if (m_pHelp) {
            delete m_pHelp;
            m_pHelp = (lmHelpController*)NULL;
        }

        // create the new controller
        InitializeHelp();
        wxASSERT(m_pHelp);

        // open it
        m_pHelp->Display(_T("index.htm"));
        m_fHelpOpened = true;
    }

}

void lmMainFrame::SetOpenHelpButton(bool fPressed)
{
    m_fHelpOpened = fPressed;
}

void lmMainFrame::OnOpenBook(wxCommandEvent& event)
{
    if (!m_pBookController)
    {
        // create book controller and load books
        InitializeBooks();
        wxASSERT(m_pBookController);

        // display book "intro"
        m_pBookController->Display(_T("intro_thm0.htm"));       //By page name
        m_pBookController->GetFrame()->NotifyPageChanged();     // needed in Linux. I don't know why !
		OnActiveChildChanged(m_pBookController->GetFrame());
    }
    else
    {
        m_pBookController->GetFrame()->SetFocus();
    }
}

void lmMainFrame::OnOpenBookUI(wxUpdateUIEvent &event)
{
    event.Enable(m_pBookController == (lmTextBookController*)NULL);
}

void lmMainFrame::OnCloseWindow(wxCloseEvent& event)
{
    // Invoked when the application is going to close the main window
    // Override default method in lmDocTDIParentFrame, as it will only close
    // the lmDocTDIChild windows (the scores) but no other windows (Welcome, eBooks)

    m_fClosingAll = true;
    if (CloseAll())     //force to close all windows
        event.Skip();   //allow event to continue normal processing if all closed
    m_fClosingAll = false;
}

void lmMainFrame::OnWindowClose(wxCommandEvent& WXUNUSED(event))
{
    // Invoked from menu: Window > Close

    CloseActive();
}

void lmMainFrame::OnWindowCloseAll(wxCommandEvent& WXUNUSED(event))
{
    // Invoked from menu: Window > Close all

    m_fClosingAll = true;
    CloseAll();
    m_fClosingAll = false;
}

void lmMainFrame::OnWindowNext(wxCommandEvent& WXUNUSED(event))
{
    ActivateNext();
}

void lmMainFrame::OnWindowPrev(wxCommandEvent& WXUNUSED(event))
{
    ActivatePrevious();
}

lmScoreView* lmMainFrame::GetActiveScoreView()
{
    // get the view
    lmTDIChildFrame* pChild = GetActiveChild();
	wxASSERT(pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame)));
    return ((lmEditFrame*)pChild)->GetView();
}

lmScore* lmMainFrame::GetActiveScore()
{
    // get the score
    lmTDIChildFrame* pChild = GetActiveChild();
	wxASSERT(pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame)));
    lmDocument* pDoc = (lmDocument*)((lmEditFrame*)pChild)->GetDocument();
    return pDoc->GetScore();
}

lmDocument* lmMainFrame::GetActiveDoc()
{
    lmTDIChildFrame* pChild = GetActiveChild();
	if (pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame)))
        return (lmDocument*)((lmEditFrame*)pChild)->GetDocument();
    else
        return (lmDocument*)NULL;
}

//------------------------------------------------------------------------------------
// Methods only for the debug version
//------------------------------------------------------------------------------------

#ifdef __WXDEBUG__

void lmMainFrame::OnDebugForceReleaseBehaviour(wxCommandEvent& event)
{
    g_fReleaseBehaviour = event.IsChecked();
}

void lmMainFrame::OnDebugShowDebugLinks(wxCommandEvent& event)
{
    g_fShowDebugLinks = event.IsChecked();
}

void lmMainFrame::OnDebugShowBorderOnScores(wxCommandEvent& event)
{
    g_fBorderOnScores = event.IsChecked();
}

void lmMainFrame::OnDebugShowDirtyObjects(wxCommandEvent& event)
{
    g_fShowDirtyObjects = event.IsChecked();
}

void lmMainFrame::OnDebugRecSelec(wxCommandEvent& event)
{
    g_fDrawSelRect = event.IsChecked();
    if (GetActiveDoc())
    {
	    GetActiveDoc()->Modify(true);
        GetActiveDoc()->UpdateAllViews((wxView*)NULL, new lmUpdateHint() );
    }
}

void lmMainFrame::OnDebugDrawAnchors(wxCommandEvent& event)
{
    g_fDrawAnchors = event.IsChecked();
    if (GetActiveDoc())
    {
	    GetActiveDoc()->Modify(true);
        GetActiveDoc()->UpdateAllViews((wxView*)NULL, new lmUpdateHint() );
    }
}

void lmMainFrame::OnDebugDrawBounds(wxCommandEvent& event)
{
    if (event.GetId() == MENU_Debug_DrawBounds_BoxSystem)
        g_fDrawBoundsBoxSystem = event.IsChecked();
    else if (event.GetId() == MENU_Debug_DrawBounds_BoxSlice)
        g_fDrawBoundsBoxSlice = event.IsChecked();
    else if (event.GetId() == MENU_Debug_DrawBounds_BoxSliceInstr)
        g_fDrawBoundsBoxSliceInstr = event.IsChecked();
    else if (event.GetId() == MENU_Debug_DrawBounds_BoundsShapes)
        g_fDrawBoundsShapes = event.IsChecked();

    g_fDrawBounds = g_fDrawBoundsBoxSystem | g_fDrawBoundsBoxSlice | g_fDrawBoundsBoxSliceInstr
                    | g_fDrawBoundsShapes;

    if (GetActiveDoc())
    {
	    GetActiveDoc()->Modify(true);
        GetActiveDoc()->UpdateAllViews((wxView*)NULL, new lmUpdateHint() );
    }
}

void lmMainFrame::OnDebugPatternEditor(wxCommandEvent& WXUNUSED(event))
{
    lmDlgPatternEditor dlg(this);
    dlg.ShowModal();

}

void lmMainFrame::OnDebugDumpBitmaps(wxCommandEvent& event)
{
    // get the view
    lmTDIChildFrame* pChild = GetActiveChild();
	wxASSERT(pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame)));
    lmScoreView* pView = ((lmEditFrame*)pChild)->GetView();

    pView->DumpBitmaps();
}

void lmMainFrame::OnDebugDumpStaffObjs(wxCommandEvent& event)
{
    lmScore* pScore = GetActiveScore();
    wxASSERT(pScore);

    lmDlgDebug dlg(this, _T("lmStaff objects dump"), pScore->Dump());
    dlg.ShowModal();

}

void lmMainFrame::OnDebugDumpGMObjects(wxCommandEvent& event)
{
    // get the BoxScore
    lmScoreView* pView = GetActiveScoreView();
	lmBoxScore* pBox = pView->GetBoxScore();
	if (!pBox) return;

    lmDlgDebug dlg(this, _T("lmBoxScore dump"), pBox->Dump(0));
    dlg.ShowModal();

}

void lmMainFrame::OnDebugScoreUI(wxUpdateUIEvent& event)
{
    lmTDIChildFrame* pChild = GetActiveChild();
	bool fEnable = (pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame)));
    event.Enable(fEnable);
}

void lmMainFrame::OnDebugCheckHarmony(wxCommandEvent& WXUNUSED(event))
{
    lmProcessorMngr* pMngr = lmProcessorMngr::GetInstance();
    lmHarmonyProcessor* pProc = 
        (lmHarmonyProcessor*)pMngr->CreateScoreProcessor( CLASSINFO(lmHarmonyProcessor) );
    pProc->DoProcess();
}

void lmMainFrame::OnDebugTestProcessor(wxCommandEvent& WXUNUSED(event))
{
    lmProcessorMngr* pMngr = lmProcessorMngr::GetInstance();
    lmTestProcessor* pProc = 
        (lmTestProcessor*)pMngr->CreateScoreProcessor( CLASSINFO(lmTestProcessor) );
    pProc->DoProcess();
}

void lmMainFrame::OnDebugSeeSource(wxCommandEvent& event)
{
    lmScore* pScore = GetActiveScore();
    wxASSERT(pScore);

    lmDlgDebug dlg(this, _T("Generated source code"), pScore->SourceLDP(false));    //false: do not include undo/redo data
    dlg.ShowModal();
}

void lmMainFrame::OnDebugSeeSourceForUndo(wxCommandEvent& event)
{
    lmScore* pScore = GetActiveScore();
    wxASSERT(pScore);

    lmDlgDebug dlg(this, _T("Generated source code"), pScore->SourceLDP(true));     //true: include undo/redo data
    dlg.ShowModal();
}

void lmMainFrame::OnDebugSeeXML(wxCommandEvent& event)
{
    lmScore* pScore = GetActiveScore();
    wxASSERT(pScore);

    lmDlgDebug dlg(this, _T("Generated MusicXML code"), pScore->SourceXML());
    dlg.ShowModal();

}

void lmMainFrame::OnDebugUnitTests(wxCommandEvent& event)
{
#ifdef __WXDEBUG__
    //lmChord oChord(_T("c4"), ect_MajorTriad);
    //oChord.UnitTests();
    wxString sResult = _T("");

    //start tests
    lmChordUnitTests();

    if (!lmFiguredBassUnitTests())
        sResult += _T("Test failure in lmFiguredBassUnitTests\n");

    //present results
    if (sResult == _T(""))
        sResult = _T("Unit test success");

    wxMessageBox(sResult);
#endif
}

void lmMainFrame::OnDebugSeeMidiEvents(wxCommandEvent& WXUNUSED(event))
{
    lmScore* pScore = GetActiveScore();
    wxASSERT(pScore);

    lmDlgDebug dlg(this, _T("MIDI events table"), pScore->DumpMidiEvents() );
    dlg.ShowModal();

}

void lmMainFrame::OnDebugSetTraceLevel(wxCommandEvent& WXUNUSED(event))
{
    lmDlgDebugTrace dlg(this);
    dlg.ShowModal();
}
#endif

// END OF DEBUG METHODS ------------------------------------------------------------
//----------------------------------------------------------------------------------

void lmMainFrame::OnAllSoundsOff(wxCommandEvent& WXUNUSED(event))
{
    if (!g_pMidiOut) return;
    g_pMidiOut->AllSoundsOff();
}

void lmMainFrame::OnSoundTest(wxCommandEvent& WXUNUSED(event))
{
    if (!g_pMidi) return;
    g_pMidi->TestOut();

}

void lmMainFrame::OnActiveChildChanged(lmTDIChildFrame* pFrame)
{
	// The active child frame has changed. Update things

    //do nothing if closing all windows
    if (m_fClosingAll) return;

	// update zoom combo box
	double rScale = pFrame->GetActiveViewScale();
    UpdateZoomControls(rScale);
    SetFocusOnActiveView();

    //wxLogMessage(_T("[lmMainFrame::OnActiveChildChanged] Is kind of lmDocTDIChildFrame: %s"),
    //    pFrame->IsKindOf(CLASSINFO(lmDocTDIChildFrame)) ? _T("yes") : _T("No") );
}

void lmMainFrame::UpdateZoomControls(double rScale)
{
    //invoked from the view at score creation to inform about the scale used.
    //Also invoked internally to centralize code to update zoom controls

    if (m_pComboZoom)
        m_pComboZoom->SetValue(wxString::Format(_T("%.2f%%"), rScale * 100.0));
}

void lmMainFrame::OnZoom(wxCommandEvent& event, double rScale)
{
    lmTDIChildFrame* pChild = GetActiveChild();
	if (pChild)
	{
		if (pChild->SetActiveViewScale(rScale) )
			UpdateZoomControls(rScale);
	}
}

void lmMainFrame::OnZoomIncrease(wxCommandEvent& event)
{
    lmTDIChildFrame* pChild = GetActiveChild();
	if (pChild)
	{
		double rScale = pChild->GetActiveViewScale() * 1.1;
		if (pChild->SetActiveViewScale(rScale) )
			UpdateZoomControls(rScale);
	}

}

void lmMainFrame::OnZoomDecrease(wxCommandEvent& event)
{
    lmTDIChildFrame* pChild = GetActiveChild();
	if (pChild)
	{
		double rScale = pChild->GetActiveViewScale() / 1.1;
		if ( pChild->SetActiveViewScale(rScale) )
			UpdateZoomControls(rScale);
	}

}

void lmMainFrame::OnZoomUpdateUI(wxUpdateUIEvent &event)
{
	int nId = event.GetId();
    lmTDIChildFrame* pChild = GetActiveChild();
	if (pChild)
	{
		if ( pChild->IsKindOf(CLASSINFO(lmEditFrame)) )
			event.Enable(true);
		else
			event.Enable(nId == MENU_Zoom_Decrease ||
						 nId == lmID_COMBO_ZOOM ||
						 nId == MENU_Zoom_Increase );
	}
	else
		event.Enable(false);
}

void lmMainFrame::OnZoomOther(wxCommandEvent& event)
{
    lmScoreView* pView = GetActiveScoreView();
    double rScale = pView->GetScale() * 100;
    int nZoom = (int) ::wxGetNumberFromUser(_T(""),
        _("Zooming? (10 to 800)"), _T(""), (int)rScale, 10, 800);
    if (nZoom != -1)    // -1 means invalid input or user canceled
        OnZoom(event, (double)nZoom / 100.0);
}

void lmMainFrame::OnZoomFitWidth(wxCommandEvent& event)
{
    lmScoreView* pView = GetActiveScoreView();
    pView->SetScaleFitWidth();
    UpdateZoomControls(pView->GetScale());
}

void lmMainFrame::OnZoomFitFull(wxCommandEvent& event)
{
    lmScoreView* pView = GetActiveScoreView();
    pView->SetScaleFitFull();
    UpdateZoomControls(pView->GetScale());
}

void lmMainFrame::OnComboZoom(wxCommandEvent& event)
{
    wxString sValue = event.GetString();
    if (sValue == _("Fit page full")) {
        OnZoomFitFull(event);
    }
    else if (sValue == _("Fit page width")) {
        OnZoomFitWidth(event);
    }
    else if (sValue == _("Actual size")) {
        OnZoom(event, 1.0);
    }
    else {
        //sValue.Replace(_T(","), _T("."));
        sValue.Replace(_T("%"), _T(""));
        sValue.Trim();
        double rZoom;
        if (!sValue.ToDouble(&rZoom)) {
            wxMessageBox(wxString::Format(_("Invalid zooming factor '%s'"), sValue.c_str()),
                         _("Error message"), wxOK || wxICON_HAND );
            return;
        }
        if (rZoom < 9.9 || rZoom > 801.0) {
            wxMessageBox(_("Zooming factor must be greater that 10% and lower than 800%"),
                         _("Error message"), wxOK || wxICON_HAND );
            return;
        }
        OnZoom(event, rZoom/100.0);
    }
	event.Skip();      //continue processing the  event

}

void lmMainFrame::OnComboVoice(wxCommandEvent& event)
{
    //int nVoice = event.GetSelection();
    //wxMessageBox(wxString::Format(_T("Voice %d selected"), nVoice));
    SetFocusOnActiveView();
}

// View menu event handlers

bool lmMainFrame::IsToolBoxVisible()
{
	return (m_pToolBox && m_mgrAUI.GetPane(_T("ToolBox")).IsShown());
}


void lmMainFrame::OnViewTools(wxCommandEvent& event)
{
    ShowToolBox(event.IsChecked());
}

void lmMainFrame::ShowToolBox(bool fShow)
{
    //create the ToolBox
    if (!m_pToolBox)
    {
        m_pToolBox =  new lmToolBox(this, wxID_ANY);
        m_pToolBox->Hide();
    }

    if (fShow)
    {
        //if not added to AUI manager do it now
        wxAuiPaneInfo panel = m_mgrAUI.GetPane(_T("ToolBox"));
        if (!panel.IsOk())
            m_mgrAUI.AddPane(m_pToolBox, wxAuiPaneInfo(). Name(_T("ToolBox")).
                                Caption(_("Edit tool box")).Left().
							    Floatable(true).
							    Resizable(false).
							    TopDockable(true).
							    BottomDockable(false).
							    MaxSize(wxSize(m_pToolBox->GetWidth(), -1)).
							    MinSize(wxSize(m_pToolBox->GetWidth(), -1)) );

        //show ToolBox
        m_mgrAUI.GetPane(_T("ToolBox")).Show(true);
        m_mgrAUI.Update();
        m_pToolBox->SetFocus();
    }
    else
    {
        //if added to AUI manager hide ToolBox
        wxAuiPaneInfo panel = m_mgrAUI.GetPane(_T("ToolBox"));
        if (panel.IsOk())
        {
            m_mgrAUI.GetPane(_T("ToolBox")).Show(false);
            m_mgrAUI.Update();
        }
    }
}

void lmMainFrame::OnViewRulers(wxCommandEvent& event)
{
    lmScoreView* pView = GetActiveScoreView();
    pView->SetRulersVisible(event.IsChecked());
}

void lmMainFrame::OnViewRulersUI(wxUpdateUIEvent &event)
{
    //For now, always disabled in release versions
    if (g_fReleaseVersion || g_fReleaseBehaviour) {
        event.Enable(false);
    }
    else {
        lmTDIChildFrame* pChild = GetActiveChild();
        event.Enable( pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame)) );
    }
}

bool lmMainFrame::ShowRulers()
{
    return GetMenuBar()->IsChecked(MENU_View_Rulers);
}

void lmMainFrame::OnViewToolBar(wxCommandEvent& WXUNUSED(event))
{
    bool fToolBar;
    if (!m_pToolbar) {
        CreateMyToolBar ();
        fToolBar = true;
    } else{
        DeleteToolbar ();
        fToolBar = false;
    }
    g_pPrefs->Write(_T("/MainFrame/ViewToolBar"), fToolBar);

}

void lmMainFrame::OnToolbarsUI (wxUpdateUIEvent &event) {
    event.Check (m_pToolbar != NULL);
}


void lmMainFrame::OnViewStatusBar(wxCommandEvent& WXUNUSED(event))
{
    bool fStatusBar;
    if (!m_pStatusBar) {
        CreateTheStatusBar ();
        fStatusBar = true;
    }else{
        DeleteTheStatusBar ();
        fStatusBar = false;
    }
    g_pPrefs->Write(_T("/MainFrame/ViewStatusBar"), fStatusBar);

}

void lmMainFrame::OnStatusbarUI (wxUpdateUIEvent &event) {
    event.Check (m_pStatusBar != NULL);
}

void lmMainFrame::NewScoreWindow(lmEditorMode* pMode, lmScore* pScore)
{
    //Open a new score editor window in mode pMode

    wxASSERT(pScore);
    ShowToolBox(true);      //force to display ToolBox
    m_pDocManager->OpenDocument(pMode, pScore);
}

//-----------------------------------------------------------------------------------------------
// Print/preview
//-----------------------------------------------------------------------------------------------

void lmMainFrame::OnPrintPreview(wxCommandEvent& WXUNUSED(event))
{
    lmTDIChildFrame* pChild = GetActiveChild();
    bool fEditFrame = pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame));
    bool fTextBookFrame = pChild && pChild->IsKindOf(CLASSINFO(lmTextBookFrame));

    if (fEditFrame) {
        // Get the active view
        lmScoreView* pView = GetActiveScoreView();

        // Pass two printout objects: for preview, and possible printing.
        wxPrintDialogData printDialogData(*g_pPrintData);
        wxPrintPreview *preview = new wxPrintPreview(new lmPrintout(pView), new lmPrintout(pView), &printDialogData);
        if (!preview->Ok()) {
            delete preview;
            wxMessageBox(_("There is a problem previewing.\nPerhaps your current printer is not set correctly?"), _("Previewing"), wxOK);
            return;
        }

        wxPreviewFrame *frame = new wxPreviewFrame(preview, this, _("Preview"), wxPoint(100, 100), wxSize(600, 650));
        frame->Centre(wxBOTH);
        frame->Initialize();
        frame->Show(true);
    }
    else if (fTextBookFrame) {
    }
}

//void lmMainFrame::OnPageSetup(wxCommandEvent& WXUNUSED(event))
//{
//    (*g_pPaperSetupData) = *g_pPrintData;
//
//    wxPageSetupDialog pageSetupDialog(this, g_pPaperSetupData);
//    pageSetupDialog.ShowModal();
//
//    (*g_pPrintData) = pageSetupDialog.GetPageSetupData().GetPrintData();
//    (*g_pPaperSetupData) = pageSetupDialog.GetPageSetupData();
//
//}

void lmMainFrame::OnPrintSetup(wxCommandEvent& WXUNUSED(event))
{
    //wxPrintDialogData printDialogData(* g_pPrintData);
    //wxPrintDialog printerDialog(this, &printDialogData);
    //
    //printerDialog.GetPrintDialogData().SetSetupDialog(TRUE);
    //printerDialog.ShowModal();

    //(*g_pPrintData) = printerDialog.GetPrintDialogData().GetPrintData();

}

void lmMainFrame::OnPrint(wxCommandEvent& event)
{
    lmTDIChildFrame* pChild = GetActiveChild();
    bool fEditFrame = pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame));
    bool fTextBookFrame = pChild && pChild->IsKindOf(CLASSINFO(lmTextBookFrame));

    if (fEditFrame) {
        wxPrintDialogData printDialogData(* g_pPrintData);
        wxPrinter printer(& printDialogData);

        // Get the active view and create the printout object
        lmScoreView* pView = GetActiveScoreView();
        lmPrintout printout(pView);

        if (!printer.Print(this, &printout, true)) {
            if (wxPrinter::GetLastError() == wxPRINTER_ERROR)
                wxMessageBox(_("There is a problem for printing.\nPerhaps your current printer is not set correctly?"), _("Printing"), wxOK);
            else
                wxMessageBox(_("Printing cancelled"), _("Printing"), wxOK);

        } else {
            (*g_pPrintData) = printer.GetPrintDialogData().GetPrintData();
        }
    }
    else if (fTextBookFrame) {
        event.SetId(lmMENU_eBook_Print);
        lmTextBookFrame* pBookFrame = m_pBookController->GetFrame();
        pBookFrame->OnToolbar(event);
        event.Skip(false);      //no further processing
    }

}

void lmMainFrame::OnEditCut(wxCommandEvent& event)
{
    //When invoked, current active child frame must be an lmEditFrame

    lmTDIChildFrame* pChild = GetActiveChild();
	if (pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame)))
    {
        ((lmEditFrame*)pChild)->GetView()->GetController()->DeleteSelection();
    }
}

void lmMainFrame::OnEditCopy(wxCommandEvent& event)
{
    //When invoked, current active child frame must be an lmEditFrame

    lmTDIChildFrame* pChild = GetActiveChild();
	if (pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame)))
    {
        lmTODO(_T("[lmMainFrame::OnEditCopy] All code in this method"));
    }
}

void lmMainFrame::OnEditPaste(wxCommandEvent& event)
{
    //When invoked, current active child frame must be an lmEditFrame

    lmTDIChildFrame* pChild = GetActiveChild();
	if (pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame)))
    {
        lmTODO(_T("[lmMainFrame::OnEditPaste] All code in this method"));
    }
}

void lmMainFrame::OnEditUpdateUI(wxUpdateUIEvent &event)
{
    lmTDIChildFrame* pChild = GetActiveChild();
	bool fEnable = (pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame)));
    if (!fEnable)
    {
        event.Enable(false);
    }
    else
    {
        switch (event.GetId())
        {
            case MENU_Edit_Copy:
            case MENU_Edit_Cut:
                // Copy & cut: enable only if something selected
                event.Enable( ((lmEditFrame*)pChild)->GetView()->SomethingSelected() );
                break;
            case MENU_Edit_Paste:
                //Enable only if something saved in clipboard
                event.Enable(false);    //TODO
                break;

			case MENU_View_Page_Margins:
				event.Enable(true);
				event.Check(g_fShowMargins);	//synchronize check status
				break;


            // Other commnads: always enabled
            default:
                event.Enable(true);


        }
    }
}

void lmMainFrame::OnFileOpen(wxCommandEvent& event)
{
    // ask for the file to open
    wxString sFilter = wxT("*.lms");
    wxString sFilename = ::wxFileSelector(_("Choose the file to open"),
                                        wxT(""),        //default path
                                        wxT(""),        //default filename
                                        wxT("lms"),     //default_extension
                                        sFilter,
                                        wxFD_OPEN,      //flags
                                        this);
    if ( !sFilename.IsEmpty() )
    {
        ShowToolBox(true);      //force to display ToolBox
        m_pDocManager->OpenFile(sFilename);
    }
}

void lmMainFrame::OnFileImport(wxCommandEvent& WXUNUSED(event))
{
    // ask for the file to import
    wxString sFilter = wxT("*.*");
    wxString sFilename = ::wxFileSelector(_("Choose the file to import"),
                                        wxT(""),        //default path
                                        wxT(""),        //default filename
                                        wxT("xml"),     //default_extension
                                        sFilter,
                                        wxFD_OPEN,      //flags
                                        this);
    if ( !sFilename.IsEmpty() )
    {
        //wxString sPath = _T("\\<<IMPORT>>//");
        //sPath += sFilename;
        //sPath += _T(".txt");
        ShowToolBox(true);      //force to display ToolBox
        m_pDocManager->ImportFile(sFilename);   //sPath);
    }
}

void lmMainFrame::OnFileClose(wxCommandEvent& event)
{
    m_pDocManager->OnFileClose(event);
}

void lmMainFrame::OnFileSave(wxCommandEvent& event)
{
    m_pDocManager->OnFileSave(event);
}

void lmMainFrame::OnFileSaveAs(wxCommandEvent& event)
{
    m_pDocManager->OnFileSaveAs(event);
}

void lmMainFrame::OnFileUpdateUI(wxUpdateUIEvent &event)
{
    lmTDIChildFrame* pChild = GetActiveChild();
    bool fEditFrame = pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame));
    bool fTextBookFrame = pChild && pChild->IsKindOf(CLASSINFO(lmTextBookFrame));
    bool fEnableImport = !(g_fReleaseVersion || g_fReleaseBehaviour);

    switch (event.GetId())
    {
        // Print related commands: enabled if EditFrame or TextBookFrame
        case MENU_Print_Preview:
            event.Enable(fEditFrame);
            //TODO Add print preview capabilities to TextBookFrame
            break;
        case wxID_PRINT_SETUP:
            //TODO: disabled in 3.3. Incompatibilities with wx2.7.1
            event.Enable(false);    //fEditFrame || fTextBookFrame);
            break;
        case MENU_Print:
            event.Enable(fEditFrame || fTextBookFrame);
            break;

        // Save related commands: enabled if EditFrame
        case wxID_SAVE:
            event.Enable(fEditFrame);
            break;
        case wxID_SAVEAS:
            event.Enable(fEditFrame);
            break;
        case MENU_File_Export:
            event.Enable(fEditFrame);
            break;
		case MENU_File_Export_MusicXML:
			event.Enable(fEditFrame);
			break;
        case MENU_File_Export_bmp:
            event.Enable(fEditFrame);
            break;
        case MENU_File_Export_jpg:
            event.Enable(fEditFrame);
            break;
        case MENU_File_Import:
            event.Enable(fEnableImport);
            break;

        // Other commnads: always enabled
        default:
            event.Enable(true);
    }

    if (g_fReleaseVersion || g_fReleaseBehaviour) {
        switch (event.GetId())
        {
            case MENU_File_Export_MusicXML:
                event.Enable(false);
                break;
        }
    }

}

void lmMainFrame::OnSoundUpdateUI(wxUpdateUIEvent &event)
{
    lmTDIChildFrame* pChild = GetActiveChild();
    event.Enable( pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame)) );

}

void lmMainFrame::RedirectKeyPressEvent(wxKeyEvent& event)
{
	//Redirects a Key Press event to the active child
    lmTDIChildFrame* pChild = GetActiveChild();
    if(pChild)
		pChild->ProcessEvent(event);
	else
		event.Skip();
}

void lmMainFrame::SetFocusOnActiveView()
{
	//Move the focus to the active child

    lmTDIChildFrame* pChild = GetActiveChild();
    if(pChild)
		pChild->SetFocus();
}

void lmMainFrame::OnRunMidiWizard(wxCommandEvent& WXUNUSED(event))
{
    DoRunMidiWizard();
}

void lmMainFrame::DoRunMidiWizard()
{
    lmMidiWizard oWizard(this);
    oWizard.Run();
}

void lmMainFrame::OnScoreWizard(wxCommandEvent& WXUNUSED(event))
{
    lmScore* pScore = (lmScore*)NULL;
    lmScoreWizard oWizard(this, &pScore);
    oWizard.Run();

    if (pScore)
    {
        //Wizard finished successfully. A score has been defined.
        //Create a new score editor window and display it
        NewScoreWindow((lmEditorMode*)NULL, pScore);
    }
}

void lmMainFrame::OnOptions(wxCommandEvent& WXUNUSED(event))
{
    lmOptionsDlg dlg(this, -1);
    dlg.CentreOnParent();
    dlg.ShowModal();
}

void lmMainFrame::OnPlayStart(wxCommandEvent& WXUNUSED(event))
{
    lmScoreView* pView = GetActiveScoreView();
    pView->GetController()->PlayScore(false);	//false: full score or from selection
}

void lmMainFrame::OnPlayCursorStart(wxCommandEvent& WXUNUSED(event))
{
    lmScoreView* pView = GetActiveScoreView();
    pView->GetController()->PlayScore(true);	//true: from cursor
}

void lmMainFrame::OnPlayStop(wxCommandEvent& WXUNUSED(event))
{
    lmScoreView* pView = GetActiveScoreView();
    pView->GetController()->StopPlaying();
}

void lmMainFrame::OnPlayPause(wxCommandEvent& WXUNUSED(event))
{
    lmScoreView* pView = GetActiveScoreView();
    pView->GetController()->PausePlaying();
}

void lmMainFrame::OnScoreTitles(wxCommandEvent& WXUNUSED(event))
{
    lmScoreView* pView = GetActiveScoreView();
    pView->GetController()->AddTitle();
}

void lmMainFrame::OnInstrumentProperties(wxCommandEvent& WXUNUSED(event))
{
    lmController* pController = GetActiveScoreView()->GetController();
    GetActiveScore()->OnInstrProperties(-1, pController);    //-1 = select instrument
}

void lmMainFrame::OnMetronomeTimer(wxTimerEvent& event)
{
    //A metronome click has been produced, and this event is generated so that we
    //can flash the metronome LED or do any other desired visual efect.
    //Do not generate sounds as they are done by the lmMetronome object

    //TODO flash metronome LED
  //  Me.picMtrLEDOff.Visible = false;
  //  Me.picMtrLEDRojoOn.Visible = true;
//    ::wxMilliSleep(100);
  //  Me.picMtrLEDOff.Visible = true;
  //  Me.picMtrLEDRojoOn.Visible = false;

}

void lmMainFrame::OnMetronomeUpdate(wxSpinEvent& WXUNUSED(event))
{
    int nMM = m_pSpinMetronome->GetValue();
    if (m_pMtr) m_pMtr->SetMM(nMM);
}

void lmMainFrame::OnMetronomeUpdateText(wxCommandEvent& WXUNUSED(event))
{
    int nMM = m_pSpinMetronome->GetValue();
    if (m_pMtr) m_pMtr->SetMM(nMM);
}

void lmMainFrame::DumpScore(lmScore* pScore)
{
    if (!pScore) return;
    lmDlgDebug dlg(this, _T("lmStaff objects dump"), pScore->Dump());
    dlg.ShowModal();

}

void lmMainFrame::OnViewPageMargins(wxCommandEvent& event)
{
    g_fShowMargins = event.IsChecked();
    if (GetActiveDoc())
    {
	    GetActiveDoc()->Modify(true);
        GetActiveDoc()->UpdateAllViews((wxView*)NULL, new lmUpdateHint() );
    }
}


void lmMainFrame::OnViewWelcomePage(wxCommandEvent& event)
{
    WXUNUSED(event)
    ShowWelcomeWindow();
}

void lmMainFrame::OnViewWelcomePageUI(wxUpdateUIEvent &event)
{
	bool fEnable = (m_pWelcomeWnd == (lmWelcomeWnd*)NULL);
    event.Enable(fEnable);
}

void lmMainFrame::OnPaneClose(wxAuiManagerEvent& event)
{
    event.Skip();      //continue processing the  event
}

//-----------------------------------------------------------------------------------
// status bar
//-----------------------------------------------------------------------------------

void lmMainFrame::SetStatusBarMsg(const wxString& sText)
{
    if (m_pStatusBar)
        m_pStatusBar->SetMsgText(sText);
}

void lmMainFrame::SetStatusBarMousePos(float x, float y)
{
    if (m_pStatusBar)
        m_pStatusBar->SetMousePos(x, y);
}

void lmMainFrame::SetStatusBarCursorRelPos(float rTime, int nMeasure)
{
    if (m_pStatusBar)
        m_pStatusBar->SetCursorRelPos(rTime, nMeasure);
}

void lmMainFrame::SetStatusBarNumPage(int nPage)
{
    if (m_pStatusBar)
        m_pStatusBar->SetNumPage(nPage);
}

void lmMainFrame::OnKeyPress(wxKeyEvent& event)
{
	//if (event.GetEventType()==wxEVT_KEY_DOWN)
        //if (event.GetKeyCode()==WXK_F1 && IsToolBoxVisible())
	{
		RedirectKeyPressEvent(event);
	}
}

void lmMainFrame::OnKeyF1(wxCommandEvent& event)
{
//		int i = 1;
}

/*
//------------------------------------------------------------------------------------
// Tips at application start
//------------------------------------------------------------------------------------

void lmMainFrame::ShowTips(bool fForceShow)
{
    bool fShowTips = false;
    g_pPrefs->Read(_T("/MainFrame/ShowTips"), &fShowTips);
    if (fForceShow || fShowTips)
    {
        //read data from last run
        wxLogNull null; // disable error message if tips file does not exist
        //wxString sTipsFile = g_pPrefs->Read(_T("/data_path")) + _T("/tips.txt");
        //sTipsFile = g_pPaths->GetHelpPath();
        //m_pHelp->SetTitleFormat(_("Test mode: using .hhp help file"));
        long nTipsIndex = g_pPrefs->Read(_T("/MainFrame/NextTip"), 0L);

        //show next tip
        wxTipProvider oTipDlg = wxCreateFileTipProvider(sTipsFile, nTipsIndex);
        fShowTips = wxShowTip(this, &oTipDlg, fShowTips);

        //save data for next run
        g_pPrefs->Write(_T("/MainFrame/ShowTips"), fShowTips);
        g_pPrefs->Write(_T("/MainFrame/NextTip"), (long)oTipDlg.GetCurrentTip());
    }
}

*/


