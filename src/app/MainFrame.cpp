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
/*! @file MainFrame.cpp
    @brief Implementation file for class lmMainFrame
    @ingroup app_gui
*/
#ifdef __GNUG__
// #pragma implementation "MainFrame.h"
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
#include "ScoreDoc.h"
#include "scoreView.h"
#include "AboutDialog.h"
#include "../options/OptionsDlg.h"
#include "ToolsDlg.h"
#include "DlgDebug.h"
#include "Printout.h"
#include "MidiWizard.h"             //Use lmMidiWizard
#include "wx/helpbase.h"		    //for wxHELP constants

#include "../../wxMidi/include/wxMidi.h"    //MIDI support throgh Portmidi lib
#include "../sound/MidiManager.h"           //access to Midi configuration
#include "Preferences.h"                    //access to user preferences
#include "../updater/Updater.h" 

//access to error's logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;

#include "DlgPatternEditor.h"               // to test DlgPatternEditor dialog
#include "../auxmusic/ChordManager.h"       //for Unit Tests

// to use html help controller
#include "wx/html/helpctrl.h"
#include "../html/TextBookController.h"
#include "../html/HelpController.h"

#include "../sound/Metronome.h"


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
extern bool g_fUseAntiAliasing;         // in TheApp.cpp 
extern bool g_fBorderOnScores;          // in TheApp.cpp


// IDs for menus and controls
// Appart of these, there are more definitions in MainFrame.h
enum
{
#ifdef _DEBUG           //to disable New/Open items in Release version
    // Menu File
    MENU_File_New = wxID_NEW,
#else
    // Menu File
    MENU_File_New = 1000,
#endif
    MENU_File_Import = MENU_Last_Public_ID,
    MENU_File_Export,
    MENU_File_Export_bmp,
    MENU_File_Export_jpg,
    MENU_OpenBook,

     // Menu View
    MENU_View_Tools,
    MENU_View_Rulers,
    MENU_View_ToolBar,
    MENU_View_StatusBar,

    // Menu Debug
    MENU_Debug_ForceReleaseBehaviour,
    MENU_Debug_ShowDebugLinks,
    MENU_Debug_ShowBorderOnScores,
    MENU_Debug_recSelec,
    MENU_Debug_DumpStaffObjs,
    MENU_Debug_SeeSource,
    MENU_Debug_SeeXML,
    MENU_Debug_SeeMIDIEvents,
    MENU_Debug_SetTraceLevel,
    MENU_Debug_PatternEditor,
    MENU_Debug_DumpBitmaps,
    MENU_Debug_UnitTests,
    MENU_Debug_UseAntiAliasing,

    // Menu Zoom
    MENU_Zoom_100,
    MENU_Zoom_Other,
    MENU_Zoom_Fit_Full,
    MENU_Zoom_Fit_Width,


    //Menu Sound
    MENU_Sound_MidiWizard,
    MENU_Sound_test,
    MENU_Sound_AllSoundsOff,

    //Menu Play
    MENU_Play_Start,
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
    MENU_OpenHelp,
    MENU_VisitWebsite,

    // Menu Print
    MENU_Print,
    MENU_Print_Preview,
    MENU_Page_Setup,

    // Menu metronome
    MENU_Metronome,

  // controls IDs
    ID_COMBO_ZOOM,
    ID_SPIN_METRONOME,

  // other IDs
    ID_TIMER_MTR,

  // special IDs

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    MENU_Help_About = wxID_ABOUT,


};


/*
 lmMainFrame is the top-level window of the application.
*/
 
IMPLEMENT_CLASS(lmMainFrame, lmDocMDIParentFrame)
BEGIN_EVENT_TABLE(lmMainFrame, lmDocMDIParentFrame)

    //File menu/toolbar
    EVT_MENU      (MENU_File_Import, lmMainFrame::OnImportFile)
    EVT_UPDATE_UI (MENU_File_Import, lmMainFrame::OnFileUpdateUI)
    EVT_MENU      (MENU_File_Export_bmp, lmMainFrame::OnExportBMP)
    EVT_MENU      (MENU_File_Export_jpg, lmMainFrame::OnExportJPG)
    EVT_UPDATE_UI (MENU_File_Export, lmMainFrame::OnFileUpdateUI)
    EVT_MENU      (MENU_Print_Preview, lmMainFrame::OnPrintPreview)
    EVT_UPDATE_UI (MENU_Print_Preview, lmMainFrame::OnFileUpdateUI)
    EVT_MENU      (wxID_PRINT_SETUP, lmMainFrame::OnPrintSetup)
    EVT_UPDATE_UI (wxID_PRINT_SETUP, lmMainFrame::OnFileUpdateUI)
    EVT_MENU      (MENU_Print, lmMainFrame::OnPrint)
    EVT_UPDATE_UI (MENU_Print, lmMainFrame::OnFileUpdateUI)
    EVT_UPDATE_UI (wxID_SAVE, lmMainFrame::OnFileUpdateUI)
    EVT_UPDATE_UI (wxID_SAVEAS, lmMainFrame::OnFileUpdateUI)
    EVT_UPDATE_UI (MENU_File_New, lmMainFrame::OnFileUpdateUI)

    //Edit menu/toolbar
    EVT_UPDATE_UI (wxID_COPY, lmMainFrame::OnEditUpdateUI)
    EVT_UPDATE_UI (wxID_PASTE, lmMainFrame::OnEditUpdateUI)
    EVT_UPDATE_UI (wxID_CUT, lmMainFrame::OnEditUpdateUI)

    //View menu/toolbar
    EVT_MENU      (MENU_View_Tools, lmMainFrame::OnViewTools)
    EVT_MENU      (MENU_View_Rulers, lmMainFrame::OnViewRulers)
    EVT_UPDATE_UI (MENU_View_Rulers, lmMainFrame::OnViewRulersUI)
    EVT_MENU      (MENU_View_ToolBar, lmMainFrame::OnViewToolBar)
    EVT_UPDATE_UI (MENU_View_ToolBar, lmMainFrame::OnToolbarsUI)
    EVT_MENU      (MENU_View_StatusBar, lmMainFrame::OnViewStatusBar)
    EVT_UPDATE_UI (MENU_View_StatusBar, lmMainFrame::OnStatusbarUI)

    //Zoom menu/toolbar
    EVT_MENU (MENU_Zoom_100, lmMainFrame::OnZoom100)
    EVT_MENU (MENU_Zoom_Other, lmMainFrame::OnZoomOther)
    EVT_MENU (MENU_Zoom_Fit_Full, lmMainFrame::OnZoomFitFull)
    EVT_MENU (MENU_Zoom_Fit_Width, lmMainFrame::OnZoomFitWidth)
    EVT_UPDATE_UI_RANGE (MENU_Zoom_100, MENU_Zoom_Fit_Width, lmMainFrame::OnZoomUpdateUI)
    EVT_COMBOBOX  (ID_COMBO_ZOOM, lmMainFrame::OnComboZoom )
    EVT_TEXT_ENTER(ID_COMBO_ZOOM, lmMainFrame::OnComboZoom )
    EVT_UPDATE_UI (ID_COMBO_ZOOM, lmMainFrame::OnZoomUpdateUI)


    //Sound menu/toolbar
    EVT_MENU      (MENU_Sound_MidiWizard, lmMainFrame::OnRunMidiWizard)
    EVT_MENU      (MENU_Sound_test, lmMainFrame::OnSoundTest)
    EVT_MENU      (MENU_Sound_AllSoundsOff, lmMainFrame::OnAllSoundsOff)
    EVT_MENU      (MENU_Play_Start, lmMainFrame::OnPlayStart)
    EVT_UPDATE_UI (MENU_Play_Start, lmMainFrame::OnSoundUpdateUI)
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
    EVT_MENU (MENU_Help_About, lmMainFrame::OnAbout)
    EVT_MENU      (MENU_OpenHelp, lmMainFrame::OnOpenHelp)
    EVT_UPDATE_UI (MENU_OpenHelp, lmMainFrame::OnOpenHelpUI)
    EVT_MENU      (MENU_CheckForUpdates, lmMainFrame::OnCheckForUpdates)
    EVT_MENU      (MENU_VisitWebsite, lmMainFrame::OnVisitWebsite)
 
        //general debug options. Always enabled
    EVT_MENU (MENU_Debug_ForceReleaseBehaviour, lmMainFrame::OnDebugForceReleaseBehaviour)
    EVT_MENU (MENU_Debug_ShowDebugLinks, lmMainFrame::OnDebugShowDebugLinks)
    EVT_MENU (MENU_Debug_ShowBorderOnScores, lmMainFrame::OnDebugShowBorderOnScores)
    EVT_MENU (MENU_Debug_SetTraceLevel, lmMainFrame::OnDebugSetTraceLevel)
    EVT_MENU (MENU_Debug_PatternEditor, lmMainFrame::OnDebugPatternEditor)
    EVT_MENU (MENU_Debug_recSelec, lmMainFrame::OnDebugRecSelec)
    EVT_MENU (MENU_Debug_UnitTests, lmMainFrame::OnDebugUnitTests)
    EVT_MENU (MENU_Debug_UseAntiAliasing, lmMainFrame::OnDebugUseAntiAliasing)
        //debug events requiring a score to be enabled
    EVT_MENU      (MENU_Debug_DumpStaffObjs, lmMainFrame::OnDebugDumpStaffObjs)
    EVT_UPDATE_UI (MENU_Debug_DumpStaffObjs, lmMainFrame::OnDebugScoreUI)
    EVT_MENU      (MENU_Debug_SeeSource, lmMainFrame::OnDebugSeeSource)
    EVT_UPDATE_UI (MENU_Debug_SeeSource, lmMainFrame::OnDebugScoreUI)
    EVT_MENU      (MENU_Debug_SeeXML, lmMainFrame::OnDebugSeeXML)
    EVT_UPDATE_UI (MENU_Debug_SeeXML, lmMainFrame::OnDebugScoreUI)
    EVT_MENU      (MENU_Debug_SeeMIDIEvents, lmMainFrame::OnDebugSeeMidiEvents)
    EVT_UPDATE_UI (MENU_Debug_SeeMIDIEvents, lmMainFrame::OnDebugScoreUI)
    EVT_MENU      (MENU_Debug_DumpBitmaps, lmMainFrame::OnDebugDumpBitmaps)
    EVT_UPDATE_UI (MENU_Debug_DumpBitmaps, lmMainFrame::OnDebugScoreUI)
    


    //metronome
    EVT_SPINCTRL    (ID_SPIN_METRONOME, lmMainFrame::OnMetronomeUpdate) 
    EVT_TEXT        (ID_SPIN_METRONOME,    lmMainFrame::OnMetronomeUpdateText)
    EVT_TIMER       (ID_TIMER_MTR,        lmMainFrame::OnMetronomeTimer)

    //TextBookFrame
    EVT_TOOL_RANGE(MENU_eBookPanel, MENU_eBook_IncreaseFont, lmMainFrame::OnBookFrame)
    EVT_UPDATE_UI_RANGE (MENU_eBookPanel, MENU_eBook_IncreaseFont, lmMainFrame::OnBookFrameUpdateUI)

END_EVENT_TABLE()

lmMainFrame::lmMainFrame(wxDocManager *manager, wxFrame *frame, const wxString& title,
    const wxPoint& pos, const wxSize& size, long style)
:
  lmDocMDIParentFrame(manager, frame, -1, title, pos, size, style, _T("myFrame"))
{
    m_pToolsDlg = (lmToolsDlg *) NULL;
    m_pHelp = (lmHelpController*) NULL;
    m_pBookController = (lmTextBookController*) NULL;
	m_pTbTextBooks = (wxToolBar*) NULL;


    // set the app icon
	// All non-MSW platforms use a bitmap. MSW uses an .ico file
	#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__)
		SetIcon(wxArtProvider::GetIcon(_T("app_icon"), wxART_OTHER));
	#else
        //macro wxICON creates an icon using an icon resource on Windows.
        SetIcon(wxICON(app_icon));
	#endif

	// create main metronome and associate it to frame metronome controls
    //metronome speed. Default MM=60
    long nMM = g_pPrefs->Read(_T("/Metronome/MM"), 60);
    m_pMainMtr = new lmMetronome(nMM);
    m_pMtr = m_pMainMtr;

    // create main menu
    wxMenuBar* menu_bar = CreateMenuBar(NULL, NULL, false, !g_fReleaseVersion);        //fEdit, fDebug
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
    m_pStatusbar = (wxStatusBar*) NULL;
    bool fStatusBar = true;
    g_pPrefs->Read(_T("/MainFrame/ViewStatusBar"), &fStatusBar);
    if (!m_pStatusbar && fStatusBar) {
        // create a status bar (by default with 1 pane only)
        CreateMyStatusBar();
        SetStatusText(_("Welcome to LenMus!"));
    }

    // initialize flags for toggle buttons status
    m_fBookOpened = false;
    m_fHelpOpened = false;

    m_fSilentCheck = false;     //default: visible 'check for updates' process


    //! @todo metronome LED
    // Set picMetronomoOn = LoadResPicture("METRONOMO_ON", vbResBitmap)
    //Set picMetronomoOff = LoadResPicture("METRONOMO_OFF", vbResBitmap)
    //configurar controles en el frame Metrónomo
    //picMtrLEDOff.Visible = True
    //Me.picMtrLEDRojoOn.Visible = False
    //picMtrLEDRojoOn.Top = Me.picMtrLEDOff.Top
    //picMtrLEDRojoOn.Left = Me.picMtrLEDOff.Left

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

    m_pClientWindow = new lmMDIClientWindow(this, style);

    m_mgrAUI.AddPane(m_pClientWindow, wxAuiPaneInfo().Name(wxT("notebook")).
                  CenterPane().PaneBorder(false));
    m_mgrAUI.Update();

}

void lmMainFrame::OnMetronomeOnOff(wxCommandEvent& WXUNUSED(event))
{
    bool fIsMetronomeOn = m_pMtr->IsRunning();
    if (m_pMtr->IsRunning()) {
        m_pMtr->Stop();
        //! @todo switch off metronome LED
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
    m_pToolbar->AddTool(MENU_Preferences, _("Preferences"), wxArtProvider::GetBitmap(_T("tool_options"), wxART_TOOLBAR, nSize), _("Set user preferences"));
    m_pToolbar->AddTool(MENU_OpenHelp, _("Help"), wxArtProvider::GetBitmap(_T("tool_help"), wxART_TOOLBAR, nSize), _("Help button"), wxITEM_CHECK);
    m_pToolbar->AddTool(MENU_OpenBook, _("Books"), wxArtProvider::GetBitmap(_T("tool_open_ebook"), wxART_TOOLBAR, nSize), _("Show the music books"), wxITEM_CHECK);
    m_pToolbar->Realize();

    //File toolbar
    m_pTbFile = new wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize, style);
    m_pTbFile->SetToolBitmapSize(nSize);
    m_pTbFile->AddTool(MENU_File_New, _("New"), 
            wxArtProvider::GetBitmap(_T("tool_new"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_new_dis"), wxART_TOOLBAR, nSize), 
            wxITEM_NORMAL, _("New score"));
    m_pTbFile->AddTool(wxID_OPEN, _("Open"), wxArtProvider::GetBitmap(_T("tool_open"), wxART_TOOLBAR, nSize), _("Open score"));
    m_pTbFile->AddTool(wxID_SAVE, _("Save"), 
            wxArtProvider::GetBitmap(_T("tool_save"), wxART_TOOLBAR, nSize), 
            wxArtProvider::GetBitmap(_T("tool_save_dis"), wxART_TOOLBAR, nSize), 
            wxITEM_NORMAL, _("Save current score to disk"));
    m_pTbFile->AddTool(MENU_Print, _("Print"), 
            wxArtProvider::GetBitmap(_T("tool_print"), wxART_TOOLBAR, nSize), 
            wxArtProvider::GetBitmap(_T("tool_print_dis"), wxART_TOOLBAR, nSize), 
            wxITEM_NORMAL, _("Print document"));
    m_pTbFile->Realize();

    //Edit toolbar
    m_pTbEdit = new wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize, style);
    m_pTbEdit->SetToolBitmapSize(nSize);
    m_pTbEdit->AddTool(wxID_COPY, _("Copy"), 
            wxArtProvider::GetBitmap(_T("tool_copy"), wxART_TOOLBAR, nSize), 
            wxArtProvider::GetBitmap(_T("tool_copy_dis"), wxART_TOOLBAR, nSize),     
            wxITEM_NORMAL, _("Copy"));
    m_pTbEdit->AddTool(wxID_CUT, _("Cut"), 
            wxArtProvider::GetBitmap(_T("tool_cut"), wxART_TOOLBAR, nSize),     
            wxArtProvider::GetBitmap(_T("tool_cut_dis"), wxART_TOOLBAR, nSize),     
            wxITEM_NORMAL, _("Cut"));
    m_pTbEdit->AddTool(wxID_PASTE, _("Paste"), 
            wxArtProvider::GetBitmap(_T("tool_paste"), wxART_TOOLBAR, nSize), 
            wxArtProvider::GetBitmap(_T("tool_paste_dis"), wxART_TOOLBAR, nSize), 
            wxITEM_NORMAL, _("Paste"));
    m_pTbEdit->Realize();

    //Zoom toolbar
    m_pTbZoom = new wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize, style);
    m_pTbZoom->SetToolBitmapSize(nSize);
    m_pTbZoom->AddTool(MENU_Zoom_Fit_Full, _("Fit full"), 
            wxArtProvider::GetBitmap(_T("tool_zoom_fit_full"), wxART_TOOLBAR, nSize), 
            wxArtProvider::GetBitmap(_T("tool_zoom_fit_full_dis"), wxART_TOOLBAR, nSize), 
            wxITEM_NORMAL, _("Zoom so that the full page is displayed"));
    m_pTbZoom->AddTool(MENU_Zoom_Fit_Width, _("Fit width"), 
            wxArtProvider::GetBitmap(_T("tool_zoom_fit_width"), wxART_TOOLBAR, nSize), 
            wxArtProvider::GetBitmap(_T("tool_zoom_fit_width_dis"), wxART_TOOLBAR, nSize), 
            wxITEM_NORMAL, _("Zoom so that page width equals window width"));
    m_pComboZoom = new wxComboBox(m_pTbZoom, ID_COMBO_ZOOM, _T(""),
                                  wxDefaultPosition, wxSize(70, -1) );
    m_pComboZoom->Append(_T("25%"));
    m_pComboZoom->Append(_T("50%"));
    m_pComboZoom->Append(_T("75%"));
    m_pComboZoom->Append(_T("100%"));
    m_pComboZoom->Append(_T("150%"));
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
    m_pTbPlay->AddTool(MENU_Play_Start, _("Play"), 
            wxArtProvider::GetBitmap(_T("tool_play"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_play_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Start/resume play back of the score"));
    m_pTbPlay->AddTool(MENU_Play_Stop, _("Stop"), 
            wxArtProvider::GetBitmap(_T("tool_stop"), wxART_TOOLBAR, nSize), 
            wxArtProvider::GetBitmap(_T("tool_stop_dis"), wxART_TOOLBAR, nSize), 
            wxITEM_NORMAL, _("Stop playing back"));
    m_pTbPlay->AddTool(MENU_Play_Pause, _("Pause"), 
            wxArtProvider::GetBitmap(_T("tool_pause"), wxART_TOOLBAR, nSize), 
            wxArtProvider::GetBitmap(_T("tool_pause_dis"), wxART_TOOLBAR, nSize), 
            wxITEM_NORMAL, _("Pause playing back"));
    m_pTbPlay->Realize();

    //Metronome toolbar
    m_pTbMtr = new wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize, style);
    m_pTbMtr->SetToolBitmapSize(nSize);
    m_pTbMtr->AddTool(MENU_Metronome, _("Metronome"), wxArtProvider::GetBitmap(_T("tool_metronome"), wxART_TOOLBAR, nSize), _("Turn metronome on/off"), wxITEM_CHECK);
    m_pSpinMetronome = new wxSpinCtrl(m_pTbMtr, ID_SPIN_METRONOME, _T(""), wxDefaultPosition, 
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
    wxSize sizeZoomTb(2 * (sizeButton.GetWidth() + m_pTbZoom->GetToolSeparation()) +
                      sizeCombo.GetWidth() + 
                      m_pTbZoom->GetToolSeparation() + 10,
                      wxMax(sizeCombo.GetHeight(), sizeButton.GetHeight()));

    // add the toolbars to the manager
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
                ToolbarPane().Top().Row(1).
                LeftDockable(false).RightDockable(false));
    m_mgrAUI.AddPane(m_pTbMtr, wxAuiPaneInfo().
                Name(wxT("Metronome")).Caption(_("Metronome tools")).
                ToolbarPane().Top().Row(1).BestSize( sizeBest ).
                LeftDockable(false).RightDockable(false));

    CreateTextBooksToolBar(style, nSize);

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

void lmMainFrame::CreateTextBooksToolBar(long style, wxSize nIconSize)
{
    m_pTbTextBooks = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
    m_pTbTextBooks->SetToolBitmapSize(nIconSize);

    //add tools
    m_pTbTextBooks->AddTool(MENU_eBookPanel, _T("Index"), 
            wxArtProvider::GetBitmap(_T("tool_index_panel"), wxART_TOOLBAR, nIconSize),
            wxArtProvider::GetBitmap(_T("tool_index_panel_dis"), wxART_TOOLBAR, nIconSize),
            wxITEM_CHECK, _("Show/hide navigation panel") );
    m_pTbTextBooks->ToggleTool(MENU_eBookPanel, false);

    m_pTbTextBooks->AddSeparator();
    m_pTbTextBooks->AddTool(MENU_eBook_PagePrev, _T("Back page"), 
            wxArtProvider::GetBitmap(_T("tool_page_previous"), wxART_TOOLBAR, nIconSize),
            wxArtProvider::GetBitmap(_T("tool_page_previous_dis"), wxART_TOOLBAR, nIconSize),
            wxITEM_NORMAL, _("Previous page of current eMusicBook") );
    m_pTbTextBooks->AddTool(MENU_eBook_PageNext, _T("Next page"), 
            wxArtProvider::GetBitmap(_T("tool_page_next"), wxART_TOOLBAR, nIconSize),
            wxArtProvider::GetBitmap(_T("tool_page_next_dis"), wxART_TOOLBAR, nIconSize),
            wxITEM_NORMAL, _("Next page of current eMusicBook") );

    m_pTbTextBooks->AddSeparator();
    m_pTbTextBooks->AddTool(MENU_eBook_GoBack, _T("Go back"), 
            wxArtProvider::GetBitmap(_T("tool_previous"), wxART_TOOLBAR, nIconSize),
            _("Go to previous visited page"), wxITEM_NORMAL );
    m_pTbTextBooks->AddTool(MENU_eBook_GoForward, _T("Go forward"), 
            wxArtProvider::GetBitmap(_T("tool_next"), wxART_TOOLBAR, nIconSize),
            _("Go to next visited page"), wxITEM_NORMAL );

    m_pTbTextBooks->AddSeparator();
    m_pTbTextBooks->AddTool(MENU_eBook_IncreaseFont, _T("Increase font"), 
            wxArtProvider::GetBitmap(_T("tool_font_increase"), wxART_TOOLBAR, nIconSize),
            _("Increase font size"), wxITEM_NORMAL );
    m_pTbTextBooks->AddTool(MENU_eBook_DecreaseFont, _T("Decrease font"),  
            wxArtProvider::GetBitmap(_T("tool_font_decrease"), wxART_TOOLBAR, nIconSize),
            _("Decrease font size"), wxITEM_NORMAL );

    m_pTbTextBooks->Realize();

    m_mgrAUI.AddPane(m_pTbTextBooks, wxAuiPaneInfo().
                Name(_T("Navigation")).Caption(_("eBooks navigation tools")).
                ToolbarPane().Top().Row(1).
                LeftDockable(false).RightDockable(false));

}

void lmMainFrame::CreateMyStatusBar()
{
    if (m_pStatusbar) return;

    int ch = GetCharWidth();
    const int widths[] = {-1, 20*ch};
    m_pStatusbar = CreateStatusBar (WXSIZEOF(widths), wxST_SIZEGRIP, MENU_View_StatusBar);
    m_pStatusbar->SetStatusWidths (WXSIZEOF(widths), widths);
    SendSizeEvent();
}

void lmMainFrame::DeleteStatusBar()
{
    if (!m_pStatusbar) return;

    //delete status bar
    SetStatusBar (NULL);
    delete m_pStatusbar;
    m_pStatusbar = (wxStatusBar*)NULL;
    SendSizeEvent();
}


wxMenuBar* lmMainFrame::CreateMenuBar(wxDocument* doc, wxView* pView, 
                                bool fEdit, bool fDebug)
{
    //Centralized code to create the menu bar. It will be customized according to the
    //received flags:
    //fEdit - Include score edit commands
    //fDebug - Include debug commands

    // file menu
    wxMenu* file_menu = new wxMenu;
    wxMenu* pExportMenu = new wxMenu; 

#if defined(__WXMSW__) || defined(__WXGTK__)
    //bitmaps on menus are supported only on Windoows and GTK+ 
    wxMenuItem* pItem;
    wxSize nIconSize(16, 16);

    pItem = new wxMenuItem(file_menu, MENU_File_New, _("&New\tCtrl+N"), _("Open new blank score"), wxITEM_NORMAL); 
    pItem->SetBitmap( wxArtProvider::GetBitmap(_T("tool_new"), wxART_TOOLBAR, nIconSize) ); 
    file_menu->Append(pItem); 

    pItem = new wxMenuItem(file_menu, wxID_OPEN, _("&Open ...\tCtrl+O"), _("Open a score"), wxITEM_NORMAL );
    pItem->SetBitmap( wxArtProvider::GetBitmap(_T("tool_open"), wxART_TOOLBAR, nIconSize) ); 
    file_menu->Append(pItem); 

    pItem = new wxMenuItem(file_menu, MENU_OpenBook, _("Open &books"), _("Hide/show eMusicBooks"), wxITEM_CHECK);
    pItem->SetBitmaps( wxArtProvider::GetBitmap(_T("tool_open_ebook"), wxART_TOOLBAR, nIconSize),
                       wxArtProvider::GetBitmap(_T("tool_open_ebook"), wxART_TOOLBAR, nIconSize) ); 
    file_menu->Append(pItem); 

    file_menu->Append(MENU_File_Import, _("&Import..."));

    //export submenu -----------------------------------------------
    pItem = new wxMenuItem(pExportMenu, MENU_File_Export_bmp, _("As &bmp image"), _("Save score as BMP images"));
    pItem->SetBitmap( wxArtProvider::GetBitmap(_T("tool_save_as_bmp"), wxART_TOOLBAR, nIconSize) );
    pExportMenu->Append(pItem);

    pItem = new wxMenuItem(pExportMenu, MENU_File_Export_jpg, _("As &jpg image"), _("Save score as JPG images"));
    pItem->SetBitmap( wxArtProvider::GetBitmap(_T("tool_save_as_jpg"), wxART_TOOLBAR, nIconSize) );
    pExportMenu->Append(pItem);

    //end of export submenu ----------------------------------------


    file_menu->Append(MENU_File_Export, _("&Export ..."), pExportMenu,
                           _("Save score in other formats") );

    pItem = new wxMenuItem(file_menu, wxID_SAVE, _("&Save\tCtrl+S"));
    pItem->SetBitmap( wxArtProvider::GetBitmap(_T("tool_save"), wxART_TOOLBAR, nIconSize) );
    file_menu->Append(pItem); 

    file_menu->Append(wxID_SAVEAS, wxString::Format(_T("%s\tCtrl+Shift+S"), _("Save &as ...")) );

    file_menu->Append(wxID_CLOSE, _("&Close\tCtrl+W"));
    file_menu->AppendSeparator();

    pItem = new wxMenuItem(file_menu, MENU_Print, _("&Print ...\tCtrl+P"));
    pItem->SetBitmap( wxArtProvider::GetBitmap(_T("tool_print"), wxART_TOOLBAR, nIconSize) );
    file_menu->Append(pItem); 

    file_menu->Append(wxID_PRINT_SETUP, _("Print &Setup..."));
    file_menu->Append(MENU_Print_Preview, wxString::Format(_T("%s\tCtrl+Shift+P"), _("Print Pre&view")) );
    file_menu->AppendSeparator();
    file_menu->Append(wxID_EXIT, _("&Quit\tCtrl+Q"));

#else
    file_menu->Append(MENU_File_New, _("&New\tCtrl+N"), _("Open new blank score"), wxITEM_NORMAL);
    file_menu->Append(wxID_OPEN, _("&Open ...\tCtrl+O"), _("Open a score"), wxITEM_NORMAL );
    file_menu->Append(MENU_OpenBook, _("Open &books"), _("Hide/show eMusicBooks"), wxITEM_CHECK);
    file_menu->Append(MENU_File_Import, _("&Import..."));
    //export submenu -----------------------------------------------
    pExportMenu->Append(MENU_File_Export_bmp, _("As &bmp image"), _("Save score as BMP images"));
    pExportMenu->Append(MENU_File_Export_jpg, _("As &jpg image"), _("Save score as JPG images"));
    pExportMenu->Append(pItem);
    //end of export submenu ----------------------------------------
    file_menu->Append(MENU_File_Export, _("&Export ..."), pExportMenu,
                           _("Save score in other formats") );

    file_menu->Append(wxID_SAVE, _("&Save\tCtrl+S"));
    file_menu->Append(wxID_SAVEAS, _("Save &as ...\tCtrl+Shift+S"));
    file_menu->Append(wxID_CLOSE, _("&Close\tCtrl+W"));
    file_menu->AppendSeparator();
    file_menu->Append(MENU_Print, _("&Print ...\tCtrl+P"));
    file_menu->Append(wxID_PRINT_SETUP, _("Print &Setup..."));
    file_menu->Append(MENU_Print_Preview, _("Print Pre&view\tCtrl+Shift+P"));
    file_menu->AppendSeparator();
    file_menu->Append(wxID_EXIT, _("&Quit\tCtrl+Q"));

#endif

    // history of files visited.
    //m_pDocManager->FileHistoryUseMenu(file_menu);

    // edit menu
    wxMenu* edit_menu;
    if (fEdit) {
        edit_menu = (wxMenu *) NULL;
        edit_menu = new wxMenu;
        edit_menu->Append(wxID_UNDO, _("&Undo"));
        edit_menu->Append(wxID_REDO, _("&Redo"));
        //edit_menu->AppendSeparator();
        doc->GetCommandProcessor()->SetEditMenu(edit_menu);
    }

    // View menu
    wxMenu *view_menu = new wxMenu;
    view_menu->Append(MENU_View_ToolBar, _("Tool &bar"), _("Hide/show the tools bar"), wxITEM_CHECK);
    view_menu->Append(MENU_View_StatusBar, _("&Status bar"), _("Hide/show the status bar"), wxITEM_CHECK);
    file_menu->AppendSeparator();
    //view_menu->Append(MENU_View_Tools, _("&Tools box"), _("&Tools box"), wxITEM_CHECK);
    view_menu->Append(MENU_View_Rulers, _("&Rulers"), _("Hide/show rulers"), wxITEM_CHECK);

    // debug menu
    // Debug strings will not be translatable. It is mandatory that all development is 
    // in English
    wxMenu* debug_menu;
    if (fDebug) {
        debug_menu = new wxMenu;
        debug_menu->Append(MENU_Debug_ForceReleaseBehaviour, _T("&Release Behaviour"), 
            _T("Force release behaviour for certain functions"), wxITEM_CHECK);
        debug_menu->Append(MENU_Debug_ShowDebugLinks, _T("&Include debug links"), 
            _T("Include debug controls in exercises"), wxITEM_CHECK);
        debug_menu->Append(MENU_Debug_ShowBorderOnScores, _T("&Border on ScoreAuxCtrol"), 
            _T("Show border on ScoreAuxCtrol"), wxITEM_CHECK);
        debug_menu->Append(MENU_Debug_recSelec, _T("&Draw recSelec"), 
            _T("Force to draw selection rectangles around staff objects"), wxITEM_CHECK);
        debug_menu->Append(MENU_Debug_UseAntiAliasing, _T("&Use anti-aliasing"), 
            _T("Use anti-aliasing for screen renderization"), wxITEM_CHECK);
        debug_menu->Append(MENU_Debug_SetTraceLevel, _T("Set trace level ...") );
        debug_menu->Append(MENU_Debug_PatternEditor, _T("Test Pattern Editor") );
        debug_menu->Append(MENU_Debug_DumpStaffObjs, _T("&Dump of score") ); 
        debug_menu->Append(MENU_Debug_SeeSource, _T("See &LDP source") ); 
        debug_menu->Append(MENU_Debug_SeeXML, _T("See &XML") );
        debug_menu->Append(MENU_Debug_SeeMIDIEvents, _T("See &MIDI events") );
        debug_menu->Append(MENU_Debug_DumpBitmaps, _T("Save offscreen bitmaps") );
        debug_menu->Append(MENU_Debug_UnitTests, _T("Unit Tests") );
    }


    // Zoom menu
    wxMenu *zoom_menu = new wxMenu;
    zoom_menu->Append(MENU_Zoom_100, _("Actual size"));
    zoom_menu->Append(MENU_Zoom_Fit_Full, _("Fit page full"));
    zoom_menu->Append(MENU_Zoom_Fit_Width, _("Fit page width"));
    zoom_menu->Append(MENU_Zoom_Other, _("Zoom to ..."));

    //Sound menu
    wxMenu *sound_menu = new wxMenu;
#if defined(__WXMSW__) || defined(__WXGTK__)

    pItem = new wxMenuItem(sound_menu, MENU_Play_Start, _("&Play"));
    pItem->SetBitmap( wxArtProvider::GetBitmap(_T("tool_play"), wxART_TOOLBAR, nIconSize) ); 
    sound_menu->Append(pItem); 

    pItem = new wxMenuItem(sound_menu, MENU_Play_Stop, _("S&top"));
    pItem->SetBitmap( wxArtProvider::GetBitmap(_T("tool_stop"), wxART_TOOLBAR, nIconSize) ); 
    sound_menu->Append(pItem); 

    pItem = new wxMenuItem(sound_menu, MENU_Play_Pause, _("P&ause"));
    pItem->SetBitmap( wxArtProvider::GetBitmap(_T("tool_pause"), wxART_TOOLBAR, nIconSize) ); 
    sound_menu->Append(pItem); 


#else
    sound_menu->Append(MENU_Play_Start, _("&Play"));
    sound_menu->Append(MENU_Play_Stop, _("S&top"));
    sound_menu->Append(MENU_Play_Pause, _("P&ause"));

#endif

    sound_menu->AppendSeparator();
    sound_menu->Append(MENU_Sound_MidiWizard, _("&Run Midi wizard"),
                        _("Midi configuration wizard"));
    sound_menu->AppendSeparator();
    sound_menu->Append(MENU_Sound_test, _("&Test sound"), _("Play an scale to test sound"));
    sound_menu->Append(MENU_Sound_AllSoundsOff, _("&All sounds off"),
                        _("Stop inmediatly all sounds"));


    // Options menu
    wxMenu* options_menu = new wxMenu;
#if defined(__WXMSW__) || defined(__WXGTK__)
    pItem = new wxMenuItem(options_menu, MENU_Preferences,  _("&Preferences"),
                            _("Open help book"), wxITEM_CHECK);
    pItem->SetBitmaps( wxArtProvider::GetBitmap(_T("tool_options"), wxART_TOOLBAR, nIconSize),
                       wxArtProvider::GetBitmap(_T("tool_options"), wxART_TOOLBAR, nIconSize) ); 
    options_menu->Append(pItem); 
#else
    options_menu->Append(MENU_Preferences, _("&Preferences"));
#endif


#if lmUSE_NOTEBOOK_MDI
    // Window menu
    wxMenu* pWindowMenu = new wxMenu;
    pWindowMenu->Append(MENU_WindowClose,    _("Cl&ose"));
    pWindowMenu->Append(MENU_WindowCloseAll, _("Close All"));
    pWindowMenu->AppendSeparator();
    pWindowMenu->Append(MENU_WindowNext,     _("&Next"));
    pWindowMenu->Append(MENU_WindowPrev,     _("&Previous"));
#endif  // lmUSE_NOTEBOOK_MDI


    // help menu
    wxMenu *help_menu = new wxMenu;
    help_menu->Append(MENU_Help_About, _("&About\tF1"),
                _("Display information about program version and credits") );
    help_menu->AppendSeparator();
#if defined(__WXMSW__) || defined(__WXGTK__)
    pItem = new wxMenuItem(help_menu, MENU_OpenHelp,  _("&Content\tCtrl+Alt+F1"),
                            _("Open help book"), wxITEM_CHECK);
    pItem->SetBitmaps( wxArtProvider::GetBitmap(_T("tool_help"), wxART_TOOLBAR, nIconSize),
                       wxArtProvider::GetBitmap(_T("tool_help"), wxART_TOOLBAR, nIconSize) ); 
    help_menu->Append(pItem); 
#else
    help_menu->Append(MENU_OpenHelp, _("&Content\tCtrl+Alt+F1"),
        _("Open help book"), wxITEM_CHECK);
#endif
    help_menu->AppendSeparator();
    help_menu->Append(MENU_CheckForUpdates, _("Check now for &updates"), 
        _("Connect to Internet and check for program updates") );

#if defined(__WXMSW__) || defined(__WXGTK__)
    pItem = new wxMenuItem(help_menu, MENU_VisitWebsite,  _("&Visit LenMus website"),
                            _("Open the Internet browser and go to LenMus website") );
    pItem->SetBitmaps( wxArtProvider::GetBitmap(_T("tool_website"), wxART_TOOLBAR, nIconSize),
                       wxArtProvider::GetBitmap(_T("tool_website"), wxART_TOOLBAR, nIconSize) ); 
    help_menu->Append(pItem); 
#else
    help_menu->Append(MENU_VisitWebsite, _("&Visit LenMus website"), 
        _("Open the Internet browser and go to LenMus website") );
#endif

    // set up the menubar.
    // AWARE: As lmMainFrame is derived from lmMDIParentFrame, in MSWindows build the menu 
    // bar automatically inherits a "Window" menu inserted in the second last position.
    // To suppress it (under MSWindows) it is necessary to add style wxFRAME_NO_WINDOW_MENU  
    // in frame creation.
    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, _("&File"));
    if (fEdit) menu_bar->Append(edit_menu, _("&Edit"));
    menu_bar->Append(view_menu, _("&View"));
    menu_bar->Append(sound_menu, _("&Sound"));
    if (fDebug) menu_bar->Append(debug_menu, _T("&Debug"));     //DO NOT TRANSLATE
    menu_bar->Append(zoom_menu, _("&Zoom"));
    menu_bar->Append(options_menu, _("&Options"));
#if lmUSE_NOTEBOOK_MDI
    menu_bar->Append(pWindowMenu, _("&Window"));
#endif  // lmUSE_NOTEBOOK_MDI
    menu_bar->Append(help_menu, _("&Help"));

        //
        // items initially checked
        //

    gfDrawSelRec = false;    //true;

    //debug toolbar
    if (fDebug) {
        menu_bar->Check(MENU_Debug_ForceReleaseBehaviour, g_fReleaseBehaviour);
        menu_bar->Check(MENU_Debug_ShowDebugLinks, g_fShowDebugLinks);
        menu_bar->Check(MENU_Debug_recSelec, gfDrawSelRec);
        menu_bar->Check(MENU_Debug_UseAntiAliasing, g_fUseAntiAliasing);
    }

    // view toolbar
    bool fToolBar = true;
    g_pPrefs->Read(_T("/MainFrame/ViewToolBar"), &fToolBar);
    menu_bar->Check(MENU_View_ToolBar, fToolBar);

    // view status bar
    bool fStatusBar = true;
    g_pPrefs->Read(_T("/MainFrame/ViewStatusBar"), &fStatusBar);
    menu_bar->Check(MENU_View_StatusBar, fStatusBar);

    return menu_bar;

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
        wxMessageBox(wxString::Format(_("Failed adding book %s"), oFilename.GetFullPath() ));

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

//Scan the received folder for books and load all books found
void lmMainFrame::ScanForBooks(wxString sPath, wxString sPattern)
{
    //wxLogMessage(_T("[lmMainFrame::ScanForBooks] Scanning path <%s>"), sPath);
    wxDir dir(sPath);
    if ( !dir.IsOpened() ) {
        // TODO: deal with the error here - wxDir would already log an error message
        // explaining the exact reason of the failure
        wxMessageBox(wxString::Format(_("Error when trying to move to folder %s"),
            sPath ));
        return;
    }

    //wxLogMessage(wxString::Format(
    //    _T("Enumerating .hhp files in directory: %s"), sPath));
    wxString sFilename;
    bool fFound = dir.GetFirst(&sFilename, sPattern, wxDIR_FILES);
    while (fFound) {
        //wxLogMessage(_T("[lmMainFrame::ScanForBooks] Encontrado %s"), sFilename);
        wxFileName oFilename(sPath, sFilename, wxPATH_NATIVE);
        if (oFilename.GetName() != _T("help")) {
            if (!m_pBookController->AddBook(oFilename)) {
                //! @todo better error handling
                wxMessageBox(wxString::Format(_("Failed adding book %s"), oFilename.GetFullPath() ));
            }
        }
        fFound = dir.GetNext(&sFilename);
    }

}

void lmMainFrame::SilentlyCheckForUpdates(bool fSilent)
{
    m_fSilentCheck = fSilent;
}


// ----------------------------------------------------------------------------
// menu callbacks
// ----------------------------------------------------------------------------
void lmMainFrame::OnBookFrame(wxCommandEvent& event)
{
    lmTextBookFrame* pBookFrame = m_pBookController->GetFrame();
    pBookFrame->OnToolbar(event);
    event.Skip(false);
}

void lmMainFrame::OnBookFrameUpdateUI(wxUpdateUIEvent& event)
{
    //enable only if current active view is TextBookFrame class
    lmMDIChildFrame* pChild = GetActiveChild();
    bool fEnabled = pChild && pChild->IsKindOf(CLASSINFO(lmTextBookFrame)) &&
                    m_pBookController;

    if (fEnabled) {
        // TextBookFrame is visible. Enable/disable buttons
        lmTextBookFrame* pBookFrame = m_pBookController->GetFrame();
        if (pBookFrame)
            pBookFrame->UpdateUIEvent(event, m_pTbTextBooks);
    }
    else
        event.Enable(false);
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
    wxString sFilename = ::wxFileSelector(_("Name for the exported file"),
                                        _T(""),    //default path
                                        _T(""),    //default filename
                                        sExt,
                                        sFilter,
                                        wxOPEN,        //flags
                                        this);
    if ( !sFilename.IsEmpty() )
    {
        //remove extension including dot 
        wxString sName = sFilename.Left( sFilename.Length() - sExt.Length() - 1 );
        lmScoreView* pView = g_pTheApp->GetActiveView();
        pView->SaveAsImage(sName, sExt, nImgType);
    }

}

void lmMainFrame::OnOpenHelp(wxCommandEvent& event)
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

void lmMainFrame::OnOpenHelpUI(wxUpdateUIEvent &event)
{
    event.Check (m_fHelpOpened);
}

void lmMainFrame::SetOpenHelpButton(bool fPressed)
{
    m_fHelpOpened = fPressed;
}

void lmMainFrame::OnOpenBook(wxCommandEvent& event)
{
    if (m_fBookOpened) {
        //The book is open. Close it.
        wxASSERT(m_pBookController);
        m_pBookController->Quit();
        delete m_pBookController;
        m_pBookController = (lmTextBookController*)NULL;
        m_fBookOpened = false;
    }
    else {
        // open book
        m_fBookOpened = true;

        // in case the previous window was closed directly, the controller still
        // exists. So delete the old controller
        if (m_pBookController) {
            delete m_pBookController;
            m_pBookController = (lmTextBookController*)NULL;
        }

        // create the new controller
        InitializeBooks();
        wxASSERT(m_pBookController);

        // open it and display book "intro"
        m_pBookController->Display(_T("intro_thm0.htm"));     //By page name
    }

}

void lmMainFrame::OnOpenBookUI(wxUpdateUIEvent &event)
{
    event.Check (m_fBookOpened);
}

void lmMainFrame::SetOpenBookButton(bool fPressed)
{
    m_fBookOpened = fPressed;
}

void lmMainFrame::OnWindowClose(wxCommandEvent& WXUNUSED(event))
{
    lmMDIChildFrame* pChild = GetActiveChild();
    if (pChild && pChild->IsKindOf(CLASSINFO(lmTextBookFrame)) && m_pBookController)
    {
        // is the eBook manager. Close it
        wxCommandEvent event(MENU_OpenBook);
        OnOpenBook(event);
    }
    else
    {
        // it is a score. Close is managed by doc/view manager
        wxCommandEvent event(wxID_CLOSE);
        g_pTheApp->GetDocManager()->OnFileClose(event);
    }
}

void lmMainFrame::OnWindowCloseAll(wxCommandEvent& WXUNUSED(event))
{
#if lmUSE_NOTEBOOK_MDI
    CloseAll();
#endif  // lmUSE_NOTEBOOK_MDI
}

void lmMainFrame::OnWindowNext(wxCommandEvent& WXUNUSED(event))
{
    ActivateNext();
}

void lmMainFrame::OnWindowPrev(wxCommandEvent& WXUNUSED(event))
{
    ActivatePrevious();
}


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

void lmMainFrame::OnDebugUseAntiAliasing(wxCommandEvent& event)
{
    g_fUseAntiAliasing = event.IsChecked();
}

void lmMainFrame::OnDebugRecSelec(wxCommandEvent& event)
{
 //   wxString msg = wxString::Format(wxT("Menu command %d"), event.GetId());
 //   msg += wxString::Format(wxT(" (the item is currently %schecked)"),
 //                           event.IsChecked() ? "" : "not ");
    //wxLogMessage(msg);
    gfDrawSelRec = event.IsChecked();
    g_pTheApp->UpdateCurrentDocViews();

}

void lmMainFrame::OnDebugPatternEditor(wxCommandEvent& WXUNUSED(event))
{
    lmDlgPatternEditor dlg(this);
    dlg.ShowModal();

}

void lmMainFrame::OnDebugDumpBitmaps(wxCommandEvent& event)
{
    // get the view
    lmScoreView* pView = g_pTheApp->GetActiveView();
    pView->DumpBitmaps();
}

void lmMainFrame::OnDebugDumpStaffObjs(wxCommandEvent& event)
{
    // get the score
    lmScoreView* pView = g_pTheApp->GetActiveView();
    lmScoreDocument* pDoc = (lmScoreDocument*) pView->GetDocument();
    lmScore* pScore = pDoc->GetScore();

    lmDlgDebug dlg(this, _T("lmStaff objects dump"), pScore->Dump());
    dlg.ShowModal();

}

void lmMainFrame::OnDebugScoreUI(wxUpdateUIEvent& event)
{
    lmScoreView* pView = g_pTheApp->GetActiveView();
    event.Enable( (pView != (lmScoreView*)NULL) );
}

void lmMainFrame::OnDebugSeeSource(wxCommandEvent& event)
{
    // get the score
    lmScoreView* pView = g_pTheApp->GetActiveView();
    lmScoreDocument* pDoc = (lmScoreDocument*) pView->GetDocument();
    lmScore* pScore = pDoc->GetScore();

    lmDlgDebug dlg(this, _T("Generated source code"), pScore->SourceLDP());
    dlg.ShowModal();

}

void lmMainFrame::OnDebugSeeXML(wxCommandEvent& event)
{
    // get the score
    lmScoreView* pView = g_pTheApp->GetActiveView();
    lmScoreDocument* pDoc = (lmScoreDocument*) pView->GetDocument();
    lmScore* pScore = pDoc->GetScore();

    lmDlgDebug dlg(this, _T("Generated MusicXML code"), pScore->SourceXML());
    dlg.ShowModal();

}

void lmMainFrame::OnDebugUnitTests(wxCommandEvent& event)
{
#ifdef _DEBUG
    lmChordManager oChord(_T("c4"), ect_MajorTriad);
    oChord.UnitTests();
#endif
}

void lmMainFrame::OnDebugSeeMidiEvents(wxCommandEvent& WXUNUSED(event))
{
    // get the score
    lmScoreView* pView = g_pTheApp->GetActiveView();
    lmScoreDocument* pDoc = (lmScoreDocument*) pView->GetDocument();
    lmScore* pScore = pDoc->GetScore();

    lmDlgDebug dlg(this, _T("MIDI events table"), pScore->DumpMidiEvents() );
    dlg.ShowModal();

}

void lmMainFrame::OnDebugSetTraceLevel(wxCommandEvent& WXUNUSED(event))
{
    wxString sData = ::wxGetTextFromUser(_("Mask to add"));
    if (!sData.IsEmpty()) g_pLogger->AddTraceMask(sData);
}

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

void lmMainFrame::OnZoom(wxCommandEvent& event, int nZoom)
{
    lmScoreView* pView = g_pTheApp->GetActiveView();
    pView->SetScale((double)nZoom / 100.0 );
    m_pComboZoom->SetValue(wxString::Format(_T("%d%%"), nZoom));

}

void lmMainFrame::OnZoomUpdateUI(wxUpdateUIEvent &event)
{
    lmMDIChildFrame* pChild = GetActiveChild();
    event.Enable( pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame)) );

}

void lmMainFrame::OnZoomOther(wxCommandEvent& event)
{
    lmScoreView* pView = g_pTheApp->GetActiveView();
    double rScale = pView->GetScale() * 100;
    int nZoom = (int) ::wxGetNumberFromUser(_T(""),
        _("Zooming? (10 to 800)"), _T(""), (int)rScale, 10, 800);
    if (nZoom != -1)    // -1 means invalid input or user canceled
        OnZoom(event, nZoom);
}

void lmMainFrame::OnZoomFitWidth(wxCommandEvent& event)
{
    lmScoreView* pView = g_pTheApp->GetActiveView();
    pView->SetScaleFitWidth();
    double rScale = pView->GetScale() * 100;
    m_pComboZoom->SetValue(wxString::Format(_T("%.2f%%"), rScale));
}

void lmMainFrame::OnZoomFitFull(wxCommandEvent& event)
{
    lmScoreView* pView = g_pTheApp->GetActiveView();
    pView->SetScaleFitFull();
    double rScale = pView->GetScale() * 100;
    m_pComboZoom->SetValue(wxString::Format(_T("%.2f%%"), rScale));
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
        OnZoom(event, 100);
    }
    else {
        //sValue.Replace(_T(","), _T("."));
        sValue.Replace(_T("%"), _T(""));
        sValue.Trim();
        double rZoom;
        if (!sValue.ToDouble(&rZoom)) {
            wxMessageBox(wxString::Format(_("Invalid zooming factor '%s'"), sValue),
                         _("Error message"), wxOK || wxICON_HAND );
            return;
        }
        if (rZoom < 9.9 || rZoom > 801.0) {
            wxMessageBox(_("Zooming factor must be greater that 10% and lower than 800%"),
                         _("Error message"), wxOK || wxICON_HAND );
            return;
        }
        OnZoom(event, (int)rZoom);
    }

}

// View menu event handlers

void lmMainFrame::OnViewTools(wxCommandEvent& WXUNUSED(event))
{
    if (m_pToolsDlg) {
        m_pToolsDlg->Show(false);
        delete m_pToolsDlg;
        m_pToolsDlg = (lmToolsDlg *) NULL;
    } else {
        m_pToolsDlg = new lmToolsDlg(this, _T("Tools box"), 50, 50);
        m_pToolsDlg->Show(true);
    }
}

void lmMainFrame::OnViewRulers(wxCommandEvent& event)
{
    lmScoreView* pView = g_pTheApp->GetActiveView();
    pView->SetRulersVisible(event.IsChecked());

}

void lmMainFrame::OnViewRulersUI(wxUpdateUIEvent &event)
{
    //For now, always disabled in release versions
    if (g_fReleaseVersion || g_fReleaseBehaviour) {
        event.Enable(false);
    }
    else {
        lmMDIChildFrame* pChild = GetActiveChild();
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
    if (!m_pStatusbar) {
        CreateMyStatusBar ();
        fStatusBar = true;
    }else{
        DeleteStatusBar ();
        fStatusBar = false;
    }
    g_pPrefs->Write(_T("/MainFrame/ViewStatusBar"), fStatusBar);

}

void lmMainFrame::OnStatusbarUI (wxUpdateUIEvent &event) {
    event.Check (m_pStatusbar != NULL);
}



void lmMainFrame::OnImportFile(wxCommandEvent& WXUNUSED(event))
{
    // ask for the file to import
    wxString sFilter = wxT("*.*");
    wxString sFilename = ::wxFileSelector(_T("Choose the file to open"),
                                        wxT(""),    //default path
                                        wxT(""),    //default filename
                                        wxT("txt"),    //default_extension
                                        sFilter,
                                        wxOPEN,        //flags
                                        this);
    if ( !sFilename.IsEmpty() )
    {
        wxString sPath = _T("\\<<IMPORT>>//");
        sPath += sFilename;
        sPath += _T(".txt");            //for DocumentManager
        // get the document manager 
        wxDocManager* pDocManager = g_pTheApp->GetDocManager();
        
        //WXDEP: The following code is a modified copy of wxDocManager::OnFileOpen taken
        // form docview.cpp source file. Review each time a new version of wxWidgets is used.
        if ( !pDocManager->CreateDocument( sPath, wxDOC_SILENT) )
        {
            pDocManager->OnOpenFileFailure();
        }
    }
    
}

//-----------------------------------------------------------------------------------------------
// Print/preview
//-----------------------------------------------------------------------------------------------

void lmMainFrame::OnPrintPreview(wxCommandEvent& WXUNUSED(event))
{
    lmMDIChildFrame* pChild = GetActiveChild();
    bool fEditFrame = pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame));
    bool fTextBookFrame = pChild && pChild->IsKindOf(CLASSINFO(lmTextBookFrame));

    if (fEditFrame) {
        // Get the active view
        lmScoreView* pView = g_pTheApp->GetActiveView();

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
    lmMDIChildFrame* pChild = GetActiveChild();
    bool fEditFrame = pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame));
    bool fTextBookFrame = pChild && pChild->IsKindOf(CLASSINFO(lmTextBookFrame));

    if (fEditFrame) {
        wxPrintDialogData printDialogData(* g_pPrintData);
        wxPrinter printer(& printDialogData);

        // Get the active view and create the printout object
        lmScoreView* pView = g_pTheApp->GetActiveView();
        lmPrintout printout(pView);

        if (!printer.Print(this, &printout, true)) {
            if (wxPrinter::GetLastError() == wxPRINTER_ERROR)
                wxMessageBox(_("There is a problem for printing.\nPerhaps your current printer is not set correctly?"), _T("Printing"), wxOK);
            else
                wxMessageBox(_("Printing canceled"), _T("Printing"), wxOK);

        } else {
            (*g_pPrintData) = printer.GetPrintDialogData().GetPrintData();
        }
    }
    else if (fTextBookFrame) {
        event.SetId(MENU_eBook_Print);
        lmTextBookFrame* pBookFrame = m_pBookController->GetFrame();
        pBookFrame->OnToolbar(event);
        event.Skip(false);
    }

}

void lmMainFrame::OnEditUpdateUI(wxUpdateUIEvent &event)
{
    lmMDIChildFrame* pChild = GetActiveChild();
    event.Enable(false);    //pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame)));
    //always disabled in current version
}

void lmMainFrame::OnFileUpdateUI(wxUpdateUIEvent &event)
{
    lmMDIChildFrame* pChild = GetActiveChild();
    bool fEditFrame = pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame));
    bool fTextBookFrame = pChild && pChild->IsKindOf(CLASSINFO(lmTextBookFrame));

    switch (event.GetId())
    {
        // Print related commands: enabled if EditFrame or TextBookFrame
        case MENU_Print_Preview:
            event.Enable(fEditFrame);
            //! @todo Add print preview capabilities to TextBookFrame
            break;
        case wxID_PRINT_SETUP:
            //! @todo: disabled in 3.3. Incompatibilities with wx2.7.1
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

        //comands disbaled in current version
        case MENU_File_New:
            event.Enable(false);
            break;

        // Other commnads: always enabled
        default:
            event.Enable(true);
    }

    //in spite of program logic, here I force to disable any unfinished feature
    //if this is a release version
    //   ----------------------------------------------------
    //   VERY IMPORTANT: READ THIS BEFORE REMOVE THIS CODE
    //   ----------------------------------------------------
    //This code does not work to disable wxID_NEW menu item
    //So have replaced identifier:
    //          wxID_NEW -> MENU_File_New
    //The problem with this is that now this item doesn't work, as wxDocManager
    //has no knowledge about them.
    //WHEN REMOVING THIS CODE RESTORE wxID_NEW identifier
    //
    if (g_fReleaseVersion || g_fReleaseBehaviour) {
        switch (event.GetId())
        {
            case MENU_File_New:
                event.Enable(false);
                break;
            case MENU_File_Import:
                event.Enable(false);
                break;
            case wxID_SAVE:
                event.Enable(false);
                break;
            case wxID_SAVEAS:
                event.Enable(false);
                break;
        }
    }

}

void lmMainFrame::OnSoundUpdateUI(wxUpdateUIEvent &event)
{
    lmMDIChildFrame* pChild = GetActiveChild();
    event.Enable( pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame)) );

}

//---------------------------------

void lmMainFrame::OnRunMidiWizard(wxCommandEvent& WXUNUSED(event))
{
    DoRunMidiWizard();
}

void lmMainFrame::DoRunMidiWizard()
{
    lmMidiWizard oWizard(this);
    oWizard.Run();
}

void lmMainFrame::OnOptions(wxCommandEvent& WXUNUSED(event))
{
    lmOptionsDlg dlg(this, -1);
    dlg.CentreOnParent();
    dlg.ShowModal();
}

void lmMainFrame::OnPlayStart(wxCommandEvent& WXUNUSED(event))
{
    lmScoreView* pView = g_pTheApp->GetActiveView();
    pView->PlayScore();
}

void lmMainFrame::OnPlayStop(wxCommandEvent& WXUNUSED(event))
{
    lmScoreView* pView = g_pTheApp->GetActiveView();
    pView->StopPlaying();
}

void lmMainFrame::OnPlayPause(wxCommandEvent& WXUNUSED(event))
{
    lmScoreView* pView = g_pTheApp->GetActiveView();
    pView->PausePlaying();
}

void lmMainFrame::OnMetronomeTimer(wxTimerEvent& event)
{
    /*
    A metronome click has been produced, and this event is generated so that we
    can flash the metronome LED or do any other    desired visual efect. 
    Do not generate sounds as they are done by the lmMetronome object
    */

    //! @todo flash metronome LED
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

