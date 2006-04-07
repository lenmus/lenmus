// RCS-ID: $Id: MainFrame.cpp,v 1.16 2006/02/25 15:15:58 cecilios Exp $
//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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
#include "MidiWizard.h"                    //Use lmMidiWizard
#include "wx/helpbase.h"		//for wxHELP constants


#include "../../wxMidi/include/wxMidi.h"    //MIDI support throgh Portmidi lib
#include "../sound/MidiManager.h"                //access to Midi configuration
#include "Preferences.h"                //access to user preferences

//access to error's logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;

// to test DlgPatternEditor dialog
#include "DlgPatternEditor.h"



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
extern bool g_fReleaseVersion;            // in TheApp.cpp
extern bool g_fReleaseBehaviour;        // in TheApp.cpp
extern bool g_fShowDebugLinks;            // in TheApp.cpp


// IDs for menus and controls
enum
{
#ifdef _DEBUG           //to disable New/Open items in Release version
    // Menu File
    MENU_File_New = wxID_NEW,
    MENU_File_Open = wxID_OPEN,
    MENU_File_Import = 1005,
#else
    // Menu File
    MENU_File_New = 1000,
    MENU_File_Open,
    MENU_File_Import = 1005,
#endif

     // Menu View
    MENU_View_Tools,
    MENU_View_Rulers,
    MENU_View_ToolBar,
    MENU_View_StatusBar,

    // Menu Debug
    MENU_Debug_ForceReleaseBehaviour,
    MENU_Debug_ShowDebugLinks,
    MENU_Debug_recSelec,
    MENU_Debug_DumpStaffObjs,
    MENU_Debug_SeeSource,
    MENU_Debug_SeeXML,
    MENU_Debug_SeeMIDIEvents,
    MENU_Debug_SetTraceLevel,
    MENU_Debug_PatternEditor,

    // Menu Zoom
    MENU_Zoom_75,
    MENU_Zoom_100,
    MENU_Zoom_150,
    MENU_Zoom_200,

    //Menu Sound
    MENU_Sound_MidiWizard,
    MENU_Sound_test,
    MENU_Sound_AllSoundsOff,

    //Menu Play
    MENU_Play_Start,
    MENU_Play_Stop,
    MENU_Play_Pause,

    //Menu Options
    MENU_Options,

    // Menu Help
    MENU_Help_About,
    MENU_OpenHelp,
    MENU_OpenBook,

    // Menu Print
    MENU_Print,
    MENU_Print_Preview,
    MENU_Page_Setup,

    // Menu metronome
    MENU_Metronome,

  // controls IDs
    ID_TOOLBAR,
    ID_COMBO_ZOOM,
    ID_SPIN_METRONOME,

  // other IDs
    ID_TIMER_MTR

};

enum
{
    //wxID_HTML_HELPFRAME = wxID_HIGHEST + 1,
    wxID_HTML_PANEL = wxID_HIGHEST + 2,
    wxID_HTML_BACK,
    wxID_HTML_FORWARD,
    wxID_HTML_UPNODE,
    wxID_HTML_UP,
    wxID_HTML_DOWN,
    wxID_HTML_PRINT,
    wxID_HTML_OPENFILE,
    wxID_HTML_OPTIONS,
    wxID_HTML_BOOKMARKSLIST,
    wxID_HTML_BOOKMARKSADD,
    wxID_HTML_BOOKMARKSREMOVE,
    wxID_HTML_TREECTRL,
    wxID_HTML_INDEXPAGE,
    wxID_HTML_INDEXLIST,
    wxID_HTML_INDEXTEXT,
    wxID_HTML_INDEXBUTTON,
    wxID_HTML_INDEXBUTTONALL,
    wxID_HTML_NOTEBOOK,
    wxID_HTML_SEARCHPAGE,
    wxID_HTML_SEARCHTEXT,
    wxID_HTML_SEARCHLIST,
    wxID_HTML_SEARCHBUTTON,
    wxID_HTML_SEARCHCHOICE,
    wxID_HTML_COUNTINFO
};

/*
 lmMainFrame is the top-level window of the application.
*/
 
IMPLEMENT_CLASS(lmMainFrame, wxDocMDIParentFrame)
BEGIN_EVENT_TABLE(lmMainFrame, wxDocMDIParentFrame)
    EVT_MENU (MENU_File_Import, lmMainFrame::OnImportFile)
    EVT_MENU      (MENU_Print_Preview, lmMainFrame::OnPrintPreview)
    EVT_UPDATE_UI (MENU_Print_Preview, lmMainFrame::OnPrintPreviewUI)
    EVT_MENU      (wxID_PRINT_SETUP, lmMainFrame::OnPrintSetup)
    EVT_MENU      (MENU_Print, lmMainFrame::OnPrint)
    EVT_UPDATE_UI (MENU_Print, lmMainFrame::OnPrintUI)

    EVT_MENU      (MENU_View_Tools, lmMainFrame::OnViewTools)
    EVT_MENU      (MENU_View_Rulers, lmMainFrame::OnViewRulers)
    EVT_MENU      (MENU_View_ToolBar, lmMainFrame::OnViewToolBar)
    EVT_UPDATE_UI (MENU_View_ToolBar, lmMainFrame::OnToolbarsUI)
    EVT_MENU      (MENU_View_StatusBar, lmMainFrame::OnViewStatusBar)
    EVT_UPDATE_UI (MENU_View_StatusBar, lmMainFrame::OnStatusbarUI)

    EVT_MENU (MENU_Zoom_75, lmMainFrame::OnZoom75)
    EVT_MENU (MENU_Zoom_100, lmMainFrame::OnZoom100)
    EVT_MENU (MENU_Zoom_150, lmMainFrame::OnZoom150)
    EVT_MENU (MENU_Zoom_200, lmMainFrame::OnZoom200)

    EVT_MENU (MENU_Sound_MidiWizard, lmMainFrame::OnRunMidiWizard)
    EVT_MENU (MENU_Sound_test, lmMainFrame::OnSoundTest)
    EVT_MENU (MENU_Sound_AllSoundsOff, lmMainFrame::OnAllSoundsOff)

    EVT_MENU (MENU_Play_Start, lmMainFrame::OnPlayStart)
    EVT_MENU (MENU_Play_Stop, lmMainFrame::OnPlayStop)
    EVT_MENU (MENU_Play_Pause, lmMainFrame::OnPlayPause)

    EVT_MENU (MENU_Options, lmMainFrame::OnOptions)

    EVT_MENU (MENU_Help_About, lmMainFrame::OnAbout)
    EVT_MENU      (MENU_OpenHelp, lmMainFrame::OnOpenHelp)
    EVT_UPDATE_UI (MENU_OpenHelp, lmMainFrame::OnOpenHelpUI)
    
    EVT_MENU      (MENU_OpenBook, lmMainFrame::OnOpenBook)
    EVT_UPDATE_UI (MENU_OpenBook, lmMainFrame::OnOpenBookUI)
    EVT_MENU      (MENU_Metronome, lmMainFrame::OnMetronomeOnOff)

    EVT_MENU (MENU_Debug_ForceReleaseBehaviour, lmMainFrame::OnDebugForceReleaseBehaviour)
    EVT_MENU (MENU_Debug_ShowDebugLinks, lmMainFrame::OnDebugShowDebugLinks)
    EVT_MENU (MENU_Debug_recSelec, lmMainFrame::OnDebugRecSelec)
    EVT_MENU (MENU_Debug_SetTraceLevel, lmMainFrame::OnDebugSetTraceLevel)
    EVT_MENU (MENU_Debug_PatternEditor, lmMainFrame::OnDebugPatternEditor)
    EVT_MENU      (MENU_Debug_DumpStaffObjs, lmMainFrame::OnDebugDumpStaffObjs)
    EVT_UPDATE_UI (MENU_Debug_DumpStaffObjs, lmMainFrame::OnDebugDumpStaffObjsUI)
    EVT_MENU      (MENU_Debug_SeeSource, lmMainFrame::OnDebugSeeSource)
    EVT_UPDATE_UI (MENU_Debug_SeeSource, lmMainFrame::OnDebugSeeSourceUI)
    EVT_MENU      (MENU_Debug_SeeXML, lmMainFrame::OnDebugSeeXML)
    EVT_UPDATE_UI (MENU_Debug_SeeXML, lmMainFrame::OnDebugSeeXMLUI)
    EVT_MENU      (MENU_Debug_SeeMIDIEvents, lmMainFrame::OnDebugSeeMidiEvents)
    EVT_UPDATE_UI (MENU_Debug_SeeMIDIEvents, lmMainFrame::OnDebugSeeMidiEventsUI)

    EVT_COMBOBOX (ID_COMBO_ZOOM, lmMainFrame::OnComboZoom)

    //metronome
    EVT_SPINCTRL    (ID_SPIN_METRONOME, lmMainFrame::OnMetronomeUpdate) 
    EVT_TEXT        (ID_SPIN_METRONOME,    lmMainFrame::OnMetronomeUpdateText)
    EVT_TIMER       (ID_TIMER_MTR,        lmMainFrame::OnMetronomeTimer)

END_EVENT_TABLE()

lmMainFrame::lmMainFrame(wxDocManager *manager, wxFrame *frame, const wxString& title,
    const wxPoint& pos, const wxSize& size, long type, wxLocale& l)
:
  wxDocMDIParentFrame(manager, frame, -1, title, pos, size, type, _T("myFrame")),
  m_locale(l)
{
    m_pToolsDlg = (lmToolsDlg *) NULL;
    m_pHelp = (lmHelpController*) NULL;
    m_pBookController = (lmTextBookController*) NULL;
	m_pNavigationToolbar = (wxToolBar*) NULL;

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
    bool fToolBar = true;
    g_pPrefs->Read(_T("/MainFrame/ViewToolBar"), &fToolBar);
    if (!m_pToolbar && fToolBar) {
        CreateMyToolBar();
    }

    // initialize status bar
    m_pStatusbar = (wxStatusBar*) NULL;
    bool fStatusBar = true;
    g_pPrefs->Read(_T("/MainFrame/ViewStatusBar"), &fStatusBar);
    if (!m_pStatusbar && fStatusBar) {
        // create a status bar (by default with 1 pane only)
        CreateMyStatusBar();
        SetStatusText(_("Welcome to LenMus!"));
    }
	//CreateNavigationToolBar();

    // initialize flags for toggle buttons status
    m_fBookOpened = false;
    m_fHelpOpened = false;


    //! @todo metronome LED
    // Set picMetronomoOn = LoadResPicture("METRONOMO_ON", vbResBitmap)
    //Set picMetronomoOff = LoadResPicture("METRONOMO_OFF", vbResBitmap)
    //configurar controles en el frame Metrónomo
    //picMtrLEDOff.Visible = True
    //Me.picMtrLEDRojoOn.Visible = False
    //picMtrLEDRojoOn.Top = Me.picMtrLEDOff.Top
    //picMtrLEDRojoOn.Left = Me.picMtrLEDOff.Left

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
    if (m_fBookOpened) {
        //The book controller is open. Update the toolbar
        wxASSERT(m_pBookController);
        m_pBookController->UpdateToolbarsLayout();
    }


}

void lmMainFrame::CreateMyToolBar()
{
    // return if exists
    if (m_pToolbar) return;

    long style = wxTB_FLAT | wxTB_DOCKABLE | wxTB_HORIZONTAL;
    long nLabelsIndex = g_pPrefs->Read(_T("/Toolbars/Labels"), 1L);
    if (nLabelsIndex == 1) 
        style |= wxTB_TEXT;
    else if (nLabelsIndex == 2) 
        style |= wxTB_HORZ_TEXT;

    m_pToolbar = CreateToolBar(style, ID_TOOLBAR);

    // set the icons size
    long nIconSize = g_pPrefs->Read(_T("/Toolbars/IconSize"), 16);
    wxSize nSize(nIconSize, nIconSize);
    m_pToolbar->SetToolBitmapSize(nSize);

    //Load the tools
    m_pToolbar->AddTool(MENU_File_New, _("New"), wxArtProvider::GetIcon(_T("tool_new"), wxART_TOOLBAR, nSize), _("New score"));
    m_pToolbar->AddTool(MENU_File_Open, _("Open"), wxArtProvider::GetIcon(_T("tool_open"), wxART_TOOLBAR, nSize), _("Open score"));

//    //! @todo How to do it in these platforms?
//    // the generic toolbar doesn't really support this
//#if (wxUSE_TOOLBAR_NATIVE && !USE_GENERIC_TBAR) && !defined(__WXX11__) || defined(__WXUNIVERSAL__)
//        wxComboBox *combo = new wxComboBox(m_pToolbar, ID_COMBO_ZOOM, _T(""), wxDefaultPosition, wxSize(120, -1) );
//        combo->Append(_T("100%"));
//        combo->Append(_T("200%"));
//        combo->Append(_T("combobox"));
//        combo->Append(_T("Print size"));
//        combo->Append(_T("Adjust to page"));
//        m_pToolbar->AddControl(combo);
//
//#endif // toolbars which don't support controls

    m_pToolbar->AddTool(wxID_SAVE, _("Save"), wxArtProvider::GetIcon(_T("tool_save"), wxART_TOOLBAR, nSize), _("Save current score to disk"));
    m_pToolbar->AddTool(wxID_COPY, _("Copy"), wxArtProvider::GetIcon(_T("tool_copy"), wxART_TOOLBAR, nSize), _("Copy"));
    m_pToolbar->AddTool(wxID_CUT, _("Cut"), wxArtProvider::GetIcon(_T("tool_cut"), wxART_TOOLBAR, nSize), _("Cut"));
    m_pToolbar->AddTool(wxID_PASTE, _("Paste"), wxArtProvider::GetIcon(_T("tool_paste"), wxART_TOOLBAR, nSize), _("Paste"));
    m_pToolbar->AddTool(MENU_Print, _("Print"), wxArtProvider::GetIcon(_T("tool_print"), wxART_TOOLBAR, nSize), _("Print document"));
    m_pToolbar->AddSeparator();
    m_pToolbar->AddTool(MENU_Options, _("Options"), wxArtProvider::GetIcon(_T("tool_options"), wxART_TOOLBAR, nSize), _("Options"));
    m_pToolbar->AddTool(MENU_OpenHelp, _("Help"), wxArtProvider::GetIcon(_T("tool_help"), wxART_TOOLBAR, nSize), _("Help button"), wxITEM_CHECK);
    m_pToolbar->AddTool(MENU_OpenBook, _("Books"), wxArtProvider::GetIcon(_T("tool_open_ebook"), wxART_TOOLBAR, nSize), _("Show the music books"), wxITEM_CHECK);

    //metronome section
    m_pToolbar->AddSeparator();
    m_pToolbar->AddTool(MENU_Metronome, _("Metronome"), wxArtProvider::GetIcon(_T("tool_metronome"), wxART_TOOLBAR, nSize), _("Turn metronome on/off"), wxITEM_CHECK);
    m_pSpinMetronome = new wxSpinCtrl(m_pToolbar, ID_SPIN_METRONOME, _T(""), wxDefaultPosition, 
        wxSize(60, -1), wxSP_ARROW_KEYS | wxSP_WRAP);    // , 20, 300, 60);
    m_pSpinMetronome->SetRange(20,300);
    m_pSpinMetronome->SetValue( m_pMtr->GetMM() );
    m_pToolbar->AddControl(m_pSpinMetronome);

    // after adding the buttons to the toolbar, must call Realize() to reflect
    // the changes
    m_pToolbar->Realize();

    m_pToolbar->SetRows(1);
}

void lmMainFrame::DeleteToolbar()
{
    if (!m_pToolbar) return;

    // delete toolbar
    delete m_pToolbar;
    SetToolBar(NULL);
    m_pToolbar = (wxToolBar*)NULL;
}

void lmMainFrame::CreateNavigationToolBar()
{
    long style = wxSIMPLE_BORDER | wxTB_FLAT | wxTB_DOCKABLE | wxTB_TEXT | wxTB_HORIZONTAL;
	wxPoint nPos = m_pToolbar->GetPosition();
	wxSize nToolbarSize = m_pToolbar->GetSize();
	nPos.y += nToolbarSize.GetHeight();
    m_pNavigationToolbar = new wxToolBar(this, wxID_ANY, nPos,
										wxDefaultSize, style);


    // set the icons size
    long nIconSize = g_pPrefs->Read(_T("/Toolbars/IconSize"), 16);
    wxSize nSize(nIconSize, nIconSize);
    m_pNavigationToolbar->SetToolBitmapSize(nSize);

    m_pNavigationToolbar->SetMargins( 2, 2 );

	// add tool buttons
    wxBitmap wpanelBitmap =
        wxArtProvider::GetBitmap(wxART_HELP_SIDE_PANEL, wxART_TOOLBAR, nSize);
    wxBitmap wbackBitmap =
        wxArtProvider::GetBitmap(wxART_GO_BACK, wxART_TOOLBAR, nSize);
    wxBitmap wforwardBitmap =
        wxArtProvider::GetBitmap(wxART_GO_FORWARD, wxART_TOOLBAR, nSize);
    wxBitmap wupnodeBitmap =
        wxArtProvider::GetBitmap(wxART_GO_TO_PARENT, wxART_TOOLBAR, nSize);
    wxBitmap wupBitmap =
        wxArtProvider::GetBitmap(wxART_GO_UP, wxART_TOOLBAR, nSize);
    wxBitmap wdownBitmap =
        wxArtProvider::GetBitmap(wxART_GO_DOWN, wxART_TOOLBAR, nSize);
    wxBitmap wopenBitmap =
        wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_TOOLBAR, nSize);
    wxBitmap wprintBitmap =
        wxArtProvider::GetBitmap(wxART_PRINT, wxART_TOOLBAR, nSize);
    wxBitmap woptionsBitmap =
        wxArtProvider::GetBitmap(wxART_HELP_SETTINGS, wxART_TOOLBAR, nSize);

    wxASSERT_MSG( (wpanelBitmap.Ok() && wbackBitmap.Ok() &&
                   wforwardBitmap.Ok() && wupnodeBitmap.Ok() &&
                   wupBitmap.Ok() && wdownBitmap.Ok() &&
                   wopenBitmap.Ok() && wprintBitmap.Ok() &&
                   woptionsBitmap.Ok()),
                  wxT("One or more HTML help frame toolbar bitmap could not be loaded.")) ;

    //CSG Modif----------------------------------------
    // test to try to customize layout and behaviour
    m_pNavigationToolbar->AddTool(wxID_HTML_PANEL, _("Index"), wpanelBitmap, 
            _("Show/hide navigation panel"), wxITEM_CHECK );
    m_pNavigationToolbar->ToggleTool(wxID_HTML_PANEL, false);


    //m_pNavigationToolbar->AddTool(wxID_HTML_PANEL, wpanelBitmap, wxNullBitmap,
    //                   false, wxDefaultCoord, wxDefaultCoord, (wxObject *) NULL,
    //                   _("Show/hide navigation panel"));
    //End CSG modif -------------------------------------

    m_pNavigationToolbar->AddSeparator();
    m_pNavigationToolbar->AddTool(wxID_HTML_BACK, wbackBitmap, wxNullBitmap,
                       false, wxDefaultCoord, wxDefaultCoord, (wxObject *) NULL,
                       _("Go back"));
    m_pNavigationToolbar->AddTool(wxID_HTML_FORWARD, wforwardBitmap, wxNullBitmap,
                       false, wxDefaultCoord, wxDefaultCoord, (wxObject *) NULL,
                       _("Go forward"));
    m_pNavigationToolbar->AddSeparator();

    m_pNavigationToolbar->AddTool(wxID_HTML_UPNODE, wupnodeBitmap, wxNullBitmap,
                       false, wxDefaultCoord, wxDefaultCoord, (wxObject *) NULL,
                       _("Go one level up in document hierarchy"));
    m_pNavigationToolbar->AddTool(wxID_HTML_UP, wupBitmap, wxNullBitmap,
                       false, wxDefaultCoord, wxDefaultCoord, (wxObject *) NULL,
                       _("Previous page"));
    m_pNavigationToolbar->AddTool(wxID_HTML_DOWN, wdownBitmap, wxNullBitmap,
                       false, wxDefaultCoord, wxDefaultCoord, (wxObject *) NULL,
                       _("Next page"));

    if ((style & wxHF_PRINT) || (style & wxHF_OPEN_FILES))
        m_pNavigationToolbar->AddSeparator();

    if (style & wxHF_OPEN_FILES)
        m_pNavigationToolbar->AddTool(wxID_HTML_OPENFILE, wopenBitmap, wxNullBitmap,
                           false, wxDefaultCoord, wxDefaultCoord, (wxObject *) NULL,
                           _("Open HTML document"));

#if wxUSE_PRINTING_ARCHITECTURE
    if (style & wxHF_PRINT)
        m_pNavigationToolbar->AddTool(wxID_HTML_PRINT, wprintBitmap, wxNullBitmap,
                           false, wxDefaultCoord, wxDefaultCoord, (wxObject *) NULL,
                           _("Print this page"));
#endif

    m_pNavigationToolbar->AddSeparator();
    m_pNavigationToolbar->AddTool(wxID_HTML_OPTIONS, woptionsBitmap, wxNullBitmap,
                       false, wxDefaultCoord, wxDefaultCoord, (wxObject *) NULL,
                       _("Display options dialog"));

    m_pNavigationToolbar->Realize();

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


wxMenuBar* lmMainFrame::CreateMenuBar(wxDocument* doc, wxView* view, 
                                bool fEdit, bool fDebug)
{
    /*
    Centralized code to create the menu bar. It will be customized according to the
    received flags:
    fEdit - Include score edit commands
    fDebug - Include debug commands
    */

    // file menu
    wxMenu *file_menu = new wxMenu;

#if defined(__WXMSW__) || defined(__WXGTK__)
    //bitmaps on menus are supported only on Windoows and GTK+ 
    wxMenuItem* pItem;
    wxSize nIconSize(16, 16);

    pItem = new wxMenuItem(file_menu, MENU_File_New, _("&New\tCtrl+N"), _("Open new blank score"), wxITEM_NORMAL); 
    pItem->SetBitmap( wxArtProvider::GetBitmap(_T("tool_new"), wxART_TOOLBAR, nIconSize) ); 
    file_menu->Append(pItem); 

    pItem = new wxMenuItem(file_menu, MENU_File_Open, _("&Open ...\tCtrl+O"), _("Open a score"), wxITEM_NORMAL );
    pItem->SetBitmap( wxArtProvider::GetBitmap(_T("tool_open"), wxART_TOOLBAR, nIconSize) ); 
    file_menu->Append(pItem); 

    pItem = new wxMenuItem(file_menu, MENU_OpenBook, _("Open &books"), _("Hide/show eMusicBooks"), wxITEM_CHECK);
    pItem->SetBitmaps( wxArtProvider::GetBitmap(_T("tool_open_ebook"), wxART_TOOLBAR, nIconSize),
                       wxArtProvider::GetBitmap(_T("tool_open_ebook"), wxART_TOOLBAR, nIconSize) ); 
    file_menu->Append(pItem); 

    file_menu->Append(MENU_File_Import, _("&Import..."));

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
    file_menu->Append(MENU_File_Open, _("&Open ...\tCtrl+O"), _("Open a score"), wxITEM_NORMAL );
    file_menu->Append(MENU_OpenBook, _("Open &books"), _("Hide/show eMusicBooks"), wxITEM_CHECK);
    file_menu->Append(MENU_File_Import, _("&Import..."));
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
        debug_menu->Append(MENU_Debug_recSelec, _T("&Draw recSelec"), 
            _T("Force to draw selection rectangles around staff objects"), wxITEM_CHECK);
        debug_menu->Append(MENU_Debug_SetTraceLevel, _T("Set trace level ...") );
        debug_menu->Append(MENU_Debug_PatternEditor, _T("Test Pattern Editor") );
        debug_menu->Append(MENU_Debug_DumpStaffObjs, _T("&Dump of score") ); 
        debug_menu->Append(MENU_Debug_SeeSource, _T("See &LDP source") ); 
        debug_menu->Append(MENU_Debug_SeeXML, _T("See &XML") );
        debug_menu->Append(MENU_Debug_SeeMIDIEvents, _T("See &MIDI events") );
    }


    // Zoom menu
    wxMenu *zoom_menu = new wxMenu;
    zoom_menu->Append(MENU_Zoom_75, _T("75%"));
    zoom_menu->Append(MENU_Zoom_100, _T("100%"));
    zoom_menu->Append(MENU_Zoom_150, _T("150%"));
    zoom_menu->Append(MENU_Zoom_200, _T("200%"));

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
    options_menu->Append(MENU_Options, _("&Preferences"));

    // help menu
    wxMenu *help_menu = new wxMenu;
    help_menu->Append(MENU_Help_About, _("&About\tF1"));
    help_menu->AppendSeparator();
#if defined(__WXMSW__) || defined(__WXGTK__)
    pItem = new wxMenuItem(help_menu, MENU_OpenHelp,  _("&Content\tCtrl+Alt+F1"),
                            _T("Open help book"), wxITEM_CHECK);
    pItem->SetBitmaps( wxArtProvider::GetBitmap(_T("tool_help"), wxART_TOOLBAR, nIconSize),
                       wxArtProvider::GetBitmap(_T("tool_help"), wxART_TOOLBAR, nIconSize) ); 
    help_menu->Append(pItem); 
#else
    help_menu->Append(MENU_OpenHelp, _("&Content\tCtrl+Alt+F1"),
        _T("Open help book"), wxITEM_CHECK);
#endif

    // set up the menubar.
    // AWARE: As lmMainFrame is derived from wxMDIParentFrame, in MSWindows build the menu 
    // bar automatically inherits a "Window" menu inserted in the second last position.
    // To suppress it (under MSWindows) it is necessary to add style wxFRAME_NO_WINDOW_MENU  
    // in frame creation.
    wxMenuBar *menu_bar = new wxMenuBar;
    menu_bar->Append(file_menu, _("&File"));
    if (fEdit) menu_bar->Append(edit_menu, _("&Edit"));
    menu_bar->Append(view_menu, _("&View"));
    menu_bar->Append(sound_menu, _("&Sound"));
    if (fDebug) menu_bar->Append(debug_menu, _("&Debug"));
    menu_bar->Append(zoom_menu, _("&Zoom"));
    menu_bar->Append(options_menu, _("&Options"));
    menu_bar->Append(help_menu, _("&Help"));

        //
        // items initially checked
        //

    //if (fDebug) menu_bar->Check(MENU_Debug_recSelec, true);
    gfDrawSelRec = false;    //true;

    // view toolbar
    bool fToolBar = true;
    g_pPrefs->Read(_T("/MainFrame/ViewToolBar"), &fToolBar);
    menu_bar->Check(MENU_View_ToolBar, fToolBar);

    // view status bar
    bool fStatusBar = true;
    g_pPrefs->Read(_T("/MainFrame/ViewStatusBar"), &fStatusBar);
    menu_bar->Check(MENU_View_StatusBar, fStatusBar);

   if (view) {
        // view rulers
        menu_bar->Check(MENU_View_Rulers, true);
        ((lmScoreView*)view)->SetRulersVisible(true);

    }

    return menu_bar;

}

/*! Enable/disable menu items and toolbar buttons depending on active window.

    To always have synchronized the menu and toolbar items and its status (enable/disabled)
    with the allowed menu items /tools for a child frame, all MDI Child windows must implement
    an OnActivate() handler and, from there, invoke parent frame (the main frame) method
    UpdateMenuAndToolbar().
*/
void lmMainFrame::UpdateMenuAndToolbar()
{
    //default settings: everything disabled
    bool fPlay = false;         // menu Play
    bool fZoom = false;         // menu zoom, combo zoom
    bool fEdit = false;         // edit: copy, cut, paste
    bool fView = false;         // view rulers

    // check the type of active window and set flags in accordance
    wxMDIChildFrame* pChild = GetActiveChild();
    if (!pChild) {
        // no window displayed
        //wxLogMessage(_T("[lmMainFrame::UpdateMenuAndToolbar] No window active"));
    }
    else if (pChild->IsKindOf(CLASSINFO(lmTextBookFrame))) {
        //wxLogMessage(_T("[lmMainFrame::UpdateMenuAndToolbar] It is a book frame"));
    }
    else if (pChild->IsKindOf(CLASSINFO(lmEditFrame))) {
        //wxLogMessage(_T("[lmMainFrame::UpdateMenuAndToolbar] It is a score canvas"));
        fPlay = true;
        fZoom = true;
        fEdit = true;         // edit: copy, cut, paste
        fView = true;         // view rulers
   }
    else {
        //wxLogMessage(_T("[lmMainFrame::UpdateMenuAndToolbar] Unknown frame type"));
    }


    //
    // enable/disable menu items according to flag settings
    //

    wxToolBarBase* pToolBar = GetToolBar();
    wxMenuBar* pMenuBar = GetMenuBar();
    //wxLogMessage(_T("[lmMainFrame::UpdateMenuAndToolbar] Play=%s, Zoom=%s"),
    //                    (fPlay ? _T("yes") : _T("no")),
    //                    (fZoom ? _T("yes") : _T("no")) );

    // menu Sound
    //pToolBar->EnableTool(MENU_Play_Start, fPlay);
    //pToolBar->EnableTool(MENU_Play_Stop, fPlay);
    //pToolBar->EnableTool(MENU_Play_Pause, fPlay);
    pMenuBar->Enable(MENU_Play_Start, fPlay);
    pMenuBar->Enable(MENU_Play_Stop, fPlay);
    pMenuBar->Enable(MENU_Play_Pause, fPlay);

    // menu Zoom, combo Zoom
    int i = pMenuBar->FindMenu(_("&Zoom"));
    wxASSERT(i != wxNOT_FOUND);
    pMenuBar->EnableTop(i, fZoom);
    //pToolBar->ToggleTool(ID_COMBO_ZOOM, fZoom);

    // edit: copy, cut, paste
    //pToolBar->EnableTool(wxID_SAVE, fEdit);
    //pToolBar->EnableTool(wxID_COPY, fEdit);
    //pToolBar->EnableTool(wxID_CUT, fEdit);
    //pToolBar->EnableTool(wxID_PASTE, fEdit);
    //pToolBar->EnableTool(MENU_Print, fEdit);

    // view rulers
    pMenuBar->Enable(MENU_View_Rulers, fView);


    //in spite of program logic, here I force to disable any unfinished feature
    //if this is a release version
    //   ----------------------------------------------------
    //   VERY IMPORTANT: READ THIS BEFORE REMOVE THIS CODE
    //   ----------------------------------------------------
    //This code does not work to disable wxID_NEW and wxID_OPEN menu items
    //So have replaced identifiers:
    //          wxID_NEW -> MENU_File_New
    //          wxID_OPEN -> MENU_File_Open
    //The problem with this is that now this items don't work, as wxDOcManager
    //has no knowledge about them.
    //WHEN REMOVING THIS CODE RESTORE wxID_NEW and wxID_OPEN identifiers
    //
   if (g_fReleaseVersion || g_fReleaseBehaviour) {
        pMenuBar->Enable(MENU_File_New, false);
        pMenuBar->Enable(MENU_File_Open, false);
        pMenuBar->Enable(MENU_File_Import, false);
        pMenuBar->Enable(wxID_SAVE, false);
        pMenuBar->Enable(wxID_SAVEAS, false);
        pMenuBar->Enable(wxID_CLOSE, false);

        pToolBar->EnableTool(MENU_File_New, false);
        pToolBar->EnableTool(MENU_File_Open, false);
        pToolBar->EnableTool(wxID_COPY, false);
        pToolBar->EnableTool(wxID_CUT, false);
        pToolBar->EnableTool(wxID_PASTE, false);
        pToolBar->EnableTool(MENU_Print, false);

    }

}

lmMainFrame::~lmMainFrame()
{
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

        //save main frame options
        wxMenuBar* pMenuBar = GetMenuBar();
        bool fToolBar = false;
        if (pMenuBar) fToolBar = pMenuBar->IsChecked(MENU_View_ToolBar);
        g_pPrefs->Write(_T("/MainFrame/ViewToolBar"), fToolBar);

        bool fStatusBar = false;
        if (pMenuBar) fStatusBar = pMenuBar->IsChecked(MENU_View_StatusBar);
        g_pPrefs->Write(_T("/MainFrame/ViewStatusBar"), fStatusBar);
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
    // create the help window 
    if (g_fReleaseVersion || g_fReleaseBehaviour) {
        m_pBookController = new
            lmTextBookController(wxHF_DEFAULT_STYLE | wxHF_FLAT_TOOLBAR );
    }
    else {
        m_pBookController = new
            lmTextBookController(wxHF_DEFAULT_STYLE | wxHF_OPEN_FILES | wxHF_FLAT_TOOLBAR );
    }

    // set the config object
    m_pBookController->UseConfig(wxConfig::Get(), _T("TextBooksController"));        

    //set directory for cache files.
    m_pBookController->SetTempDir( g_pPaths->GetTempPath() );

    //In release versions each books will be a single .htb file, precompiled in cache format,
    //all books located in locale folder
    //In test versions, each book will be in native hhp format, located in /books subfolders.
    //For testing purposes, it must be possible to switch to "release mode" (use of
    //cached htb files).

    bool fOK = true;

    wxString sPath;
    wxString sPattern;

    if (g_fReleaseVersion || g_fReleaseBehaviour) {
        //Release behaviour. Use precompiled cached .htb files and don't show title
        //! @todo books name. One or many books?
        sPath = g_pPaths->GetLocalePath();
        sPattern = _T("*.htb");
        m_pBookController->SetTitleFormat(_("Available books"));
        ScanForBooks(sPath, sPattern);
    }
    else {
        //Test behaviour. Use native .hhp, .hhc, .hhk and .htm files in subfolders
        sPath = g_pPaths->GetBooksPath();
        m_pBookController->SetTitleFormat(_T("Test mode: available books"));

        // loop to look for subdirectories
        wxDir dir(sPath);
        wxString sSubfolder;
        wxFileName oPath;
        bool fFound = dir.GetFirst(&sSubfolder, _T("*.*"), wxDIR_DIRS);
        while (fFound) {
            //wxLogMessage(wxString::Format(_T("Subdirectory found: %s"), sSubfolder));
            oPath.Assign(sPath, wxPATH_NATIVE);
            oPath.AppendDir(sSubfolder);
            ScanForBooks(oPath.GetPath(), _T("*.hhp"));
            fFound = dir.GetNext(&sSubfolder);
        }
    }

}

//Scan the received folder for books and load all books found
void lmMainFrame::ScanForBooks(wxString sPath, wxString sPattern)
{
    //wxLogMessage(_T("Scanning path <%s>"), sPath);
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
        //wxLogMessage(wxString::Format(_T("Encontrado %s"), sFilename));
        wxFileName oFilename(sPath, sFilename, wxPATH_NATIVE);
        //skip help file
        if (oFilename.GetName() != _T("help")) {
            if (!m_pBookController->AddBook(oFilename)) {
                //! @todo better error handling
                wxMessageBox(wxString::Format(_("Failed adding book %s"), oFilename.GetFullPath() ));
            }
        }
        fFound = dir.GetNext(&sFilename);
    }

}

// ----------------------------------------------------------------------------
// menu callbacks
// ----------------------------------------------------------------------------

void lmMainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString localeInfo;
    wxString locale = m_locale.GetLocale();
    wxString sysname = m_locale.GetSysName();
    wxString canname = m_locale.GetCanonicalName();

    localeInfo.Printf( _("Language: %s\nSystem locale name: %s\nCanonical locale name: %s\n"),
        locale.c_str(), sysname.c_str(), canname.c_str() );

   lmAboutDialog dlg(this);
   dlg.ShowModal();
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
        //lmTextBookFrame* pBookFrame = m_pBookController->GetFrame();
        ////pBookFrame->NotifyPageChanged();
        ////pBookFrame->RefreshLists();
        ////pBookFrame->Display(_T("intro"));
        //m_pBookController->Display(_T("LenMus. Introduction"));     //By book name: Title parameter of .hhp file
        m_pBookController->Display(_T("Welcome"));     //By chapter name: from .hhc file
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

void lmMainFrame::OnDebugForceReleaseBehaviour(wxCommandEvent& event)
{
    g_fReleaseBehaviour = event.IsChecked();
}

void lmMainFrame::OnDebugShowDebugLinks(wxCommandEvent& event)
{
    g_fShowDebugLinks = event.IsChecked();
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

void lmMainFrame::OnDebugDumpStaffObjs(wxCommandEvent& event)
{
    // get the score
    lmScoreView* pView = g_pTheApp->GetActiveView();
    lmScoreDocument* pDoc = (lmScoreDocument*) pView->GetDocument();
    lmScore* pScore = pDoc->GetScore();

    lmDlgDebug dlg(this, _T("lmStaff objects dump"), pScore->Dump());
    dlg.ShowModal();

}

void lmMainFrame::OnDebugDumpStaffObjsUI(wxUpdateUIEvent& event)
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

void lmMainFrame::OnDebugSeeSourceUI(wxUpdateUIEvent& event)
{
    lmScoreView* pView = g_pTheApp->GetActiveView();
    event.Enable( (pView != (lmScoreView*)NULL) );
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

void lmMainFrame::OnDebugSeeXMLUI(wxUpdateUIEvent& event)
{
    lmScoreView* pView = g_pTheApp->GetActiveView();
    event.Enable( (pView != (lmScoreView*)NULL) );
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

void lmMainFrame::OnDebugSeeMidiEventsUI(wxUpdateUIEvent& event)
{
    lmScoreView* pView = g_pTheApp->GetActiveView();
    event.Enable( (pView != (lmScoreView*)NULL) );
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

void lmMainFrame::OnZoom(wxCommandEvent& event, wxInt32 nZoom)
{
 //   wxString msg = wxString::Format(wxT("Menu command %d"), event.GetId());
 //   msg += wxString::Format(wxT(" (Zoom = %d)"), nZoom);
    //wxLogMessage(msg);

    lmScoreView* pView = g_pTheApp->GetActiveView();
    pView->SetScale((double)nZoom / 100.0 );
    //g_pTheApp->UpdateCurrentDocViews();

}

void lmMainFrame::OnComboZoom(wxCommandEvent& event)
{
    wxString sValue = event.GetString();
    if (sValue.Cmp(_("Adjust to page")) == 0) {
        //wxLogStatus(_T("Ajustar a página"));
    } else if (sValue.Find(_T('%')) != -1) {
        //wxLogStatus(_T("Zoom '%s'"), sValue.c_str());
    } else {
        //wxLogStatus(_T("Combobox string '%s' selected"), event.GetString().c_str());
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

void lmMainFrame::OnViewToolBar(wxCommandEvent& WXUNUSED(event))
{
    if (!m_pToolbar) {
        CreateMyToolBar ();
    } else{
        DeleteToolbar ();
    }
}

void lmMainFrame::OnToolbarsUI (wxUpdateUIEvent &event) {
    event.Check (m_pToolbar != NULL);
}


void lmMainFrame::OnViewStatusBar(wxCommandEvent& WXUNUSED(event))
{
    if (!m_pStatusbar) {
        CreateMyStatusBar ();
    }else{
        DeleteStatusBar ();
    }
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

void lmMainFrame::OnPrintPreviewUI(wxUpdateUIEvent &event)
{
    lmScoreView* pView = g_pTheApp->GetActiveView();
    event.Enable( (pView != (lmScoreView*)NULL) );
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
    wxPrintDialogData printDialogData(* g_pPrintData);
    wxPrintDialog printerDialog(this, &printDialogData);
    
    printerDialog.GetPrintDialogData().SetSetupDialog(TRUE);
    printerDialog.ShowModal();

    (*g_pPrintData) = printerDialog.GetPrintDialogData().GetPrintData();

}

void lmMainFrame::OnPrint(wxCommandEvent& WXUNUSED(event))
{
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

void lmMainFrame::OnPrintUI(wxUpdateUIEvent &event)
{
    lmScoreView* pView = g_pTheApp->GetActiveView();
    event.Enable( (pView != (lmScoreView*)NULL) );
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

