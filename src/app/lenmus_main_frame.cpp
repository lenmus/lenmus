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

//lenmus headers
#include "lenmus_main_frame.h"
#include "lenmus_config.h"

#include "lenmus_string.h"
#include "lenmus_score_canvas.h"
#include "lenmus_canvas.h"
#include "lenmus_art_provider.h"
#include "lenmus_midi_wizard.h"
#include "lenmus_midi_server.h"
#include "lenmus_welcome_window.h"

//lomse headers
#include "lomse_score_player.h"


namespace lenmus
{

//=======================================================================================
// MainFrame implementation
//=======================================================================================

//// global data structures for printing. Defined in TheApp.cpp
//#include <wx/cmndata.h>
//extern wxPrintData* g_pPrintData;
//extern wxPageSetupData* g_pPaperSetupData;
//
//// access to global external variables
//extern bool g_fReleaseVersion;          // in TheApp.cpp
//extern bool g_fReleaseBehaviour;        // in TheApp.cpp
//extern bool g_fShowDebugLinks;          // in TheApp.cpp
//extern bool g_fShowDirtyObjects;         // in TheApp.cpp
//extern bool g_fBorderOnScores;          // in TheApp.cpp

//---------------------------------------------------------------------------------------
// constants for menu IDs
enum
{
    // special IDs

    //wxID_ABOUT and wxID_EXIT are predefined by wxWidgets and have a special meaning
    //since entries using these IDs will be taken out of the normal menus under MacOS X
    //and will be inserted into the system menu (following the appropriate MacOS X
    //interface guideline).
    k_menu_file_quit = wxID_EXIT,
    k_menu_help_about = wxID_ABOUT,


    // Menu File
    k_menu_file_new = 10000,  //lmMENU_Last_Public_ID,
    k_menu_file_open,       //wxID_OPEN
    k_menu_file_reload,
    k_menu_file_Save,       //wxID_SAVE
    k_menu_file_SaveAs,     //wxID_SAVEAS
    k_menu_file_Close,      //wxID_CLOSE

    k_menu_file_Import,
    k_menu_file_export,
    k_menu_file_export_MusicXML,
    k_menu_file_export_bmp,
    k_menu_file_export_jpg,
    k_menu_open_book,

     // Menu Edit
    k_menu_edit_copy,
    k_menu_edit_cut,
    k_menu_edit_paste,

     // Menu View
    k_menu_view_tools,
    k_menu_view_rulers,
    k_menu_view_toolBar,
    k_menu_view_statusBar,
    k_menu_view_page_margins,
    k_menu_view_welcome_page,

	// Menu Score
	k_menu_score_titles,

	// Menu Instrument
    k_menu_instr_properties,

    // Menu Debug
    k_menu_debug_do_tests,
    k_menu_debug_draw_box,
    k_menu_debug_draw_box_docpage,
    k_menu_debug_draw_box_score_page,
    k_menu_debug_draw_box_system,
    k_menu_debug_draw_box_slice,
    k_menu_debug_draw_box_slice_instr,
    k_menu_debug_remove_boxes,
    k_menu_debug_justify_systems,
    k_menu_debug_dump_column_tables,
    k_menu_debug_ForceReleaseBehaviour,
    k_menu_debug_ShowDebugLinks,
    k_menu_debug_ShowBorderOnScores,
    k_menu_debug_recSelec,
    k_menu_debug_CheckHarmony,
    k_menu_debug_DrawAnchors,
    k_menu_debug_DumpStaffObjs,
    k_menu_debug_DumpBitmaps,
	k_menu_debug_DumpGMObjects,
    k_menu_debug_SeeSource,
    k_menu_debug_SeeSourceUndo,
    k_menu_debug_SeeXML,
    k_menu_debug_SeeMIDIEvents,
    k_menu_debug_SetTraceLevel,
    k_menu_debug_PatternEditor,
    k_menu_debug_ShowDirtyObjects,
    k_menu_debug_TestProcessor,

    // Menu Zoom
    k_menu_zoom_in,
    k_menu_zoom_out,
    k_menu_zoom_100,
    k_menu_zoom_fit_full,
    k_menu_zoom_other,
    k_menu_zoom_fit_width,

    // Menu Sound
    k_menu_sound_midi_wizard,
    k_menu_sound_test,
    k_menu_sound_off,

    // Menu Play
    k_menu_play_start,
	k_menu_play_cursor_start,
    k_menu_play_stop,
    k_menu_play_pause,
    k_menu_play_countoff,

    // Menu Options
    k_menu_preferences,

    // Menu Window
    k_menu_windowClose,
    k_menu_windowCloseAll,
    k_menu_windowNext,
    k_menu_windowPrev,

    // Menu Help
    k_menu_help_open,
    k_menu_help_quick_guide,
    k_menu_help_visit_website,

    // Menu Print
    k_menu_print,
    k_menu_print_Preview,
    k_menu_page_Setup,

    // Menu metronome
    MENU_Metronome,

  // controls IDs
    k_id_combo_zoom,
    k_id_spin_metronome,

  // other IDs
    k_id_timer_metronome,
	k_id_key_F1,

    k_menu_max
};


//const wxString lmRECENT_FILES = _T("Recent Files/file");
//
//IMPLEMENT_CLASS(MainFrame, wxFrame)

//---------------------------------------------------------------------------------------
// events table
BEGIN_EVENT_TABLE(MainFrame, wxFrame)

    //File menu/toolbar
    EVT_MENU(k_menu_file_quit, MainFrame::on_quit)
    EVT_MENU(k_menu_file_open, MainFrame::on_file_open)
    EVT_MENU(k_menu_file_reload, MainFrame::on_file_reload)
//    EVT_MENU      (k_menu_file_open, MainFrame::OnFileOpen)
//    EVT_UPDATE_UI (k_menu_file_open, MainFrame::OnFileUpdateUI)
//    EVT_MENU      (k_menu_file_Close, MainFrame::OnFileClose)
//    EVT_UPDATE_UI (k_menu_file_Close, MainFrame::OnFileUpdateUI)
//    EVT_MENU      (k_menu_file_Save, MainFrame::OnFileSave)
//    EVT_UPDATE_UI (k_menu_file_Save, MainFrame::OnFileUpdateUI)
//    EVT_MENU      (k_menu_file_SaveAs, MainFrame::OnFileSaveAs)
//    EVT_UPDATE_UI (k_menu_file_SaveAs, MainFrame::OnFileUpdateUI)
//
//    EVT_MENU      (k_menu_file_new, MainFrame::OnScoreWizard)
//    EVT_MENU      (k_menu_file_Import, MainFrame::OnFileImport)
//    EVT_UPDATE_UI (k_menu_file_Import, MainFrame::OnFileUpdateUI)
//    EVT_UPDATE_UI (k_menu_file_export, MainFrame::OnFileUpdateUI)
//    EVT_MENU      (k_menu_file_export_MusicXML, MainFrame::OnExportMusicXML)
//    EVT_UPDATE_UI (k_menu_file_export_MusicXML, MainFrame::OnFileUpdateUI)
//    EVT_MENU      (k_menu_file_export_bmp, MainFrame::OnExportBMP)
//    EVT_UPDATE_UI (k_menu_file_export_bmp, MainFrame::OnFileUpdateUI)
//    EVT_MENU      (k_menu_file_export_jpg, MainFrame::OnExportJPG)
//    EVT_UPDATE_UI (k_menu_file_export_jpg, MainFrame::OnFileUpdateUI)
//    EVT_MENU      (k_menu_print_Preview, MainFrame::OnPrintPreview)
//    EVT_UPDATE_UI (k_menu_print_Preview, MainFrame::OnFileUpdateUI)
//    EVT_MENU      (wxID_PRINT_SETUP, MainFrame::OnPrintSetup)
//    EVT_UPDATE_UI (wxID_PRINT_SETUP, MainFrame::OnFileUpdateUI)
//    EVT_MENU      (k_menu_print, MainFrame::OnPrint)
//    EVT_UPDATE_UI (k_menu_print, MainFrame::OnFileUpdateUI)
//    EVT_UPDATE_UI (wxID_SAVE, MainFrame::OnFileUpdateUI)
//    EVT_UPDATE_UI (wxID_SAVEAS, MainFrame::OnFileUpdateUI)
//    EVT_UPDATE_UI (k_menu_file_new, MainFrame::OnFileUpdateUI)
//    EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, MainFrame::OnOpenRecentFile)
//
//    //eBooks menu/toolbar
//    EVT_TOOL_RANGE(lmMENU_eBookPanel, lmMENU_eBook_OpenFile, MainFrame::OnBookFrame)
//    EVT_UPDATE_UI_RANGE (lmMENU_eBookPanel, lmMENU_eBook_OpenFile, MainFrame::OnBookFrameUpdateUI)
//
//    //Edit menu/toolbar
//    EVT_MENU      (k_menu_edit_copy, MainFrame::OnEditCopy)
//    EVT_UPDATE_UI (k_menu_edit_copy, MainFrame::OnEditUpdateUI)
//    EVT_MENU      (k_menu_edit_cut, MainFrame::OnEditCut)
//    EVT_UPDATE_UI (k_menu_edit_cut, MainFrame::OnEditUpdateUI)
//    EVT_MENU      (k_menu_edit_paste, MainFrame::OnEditPaste)
//    EVT_UPDATE_UI (k_menu_edit_paste, MainFrame::OnEditUpdateUI)
//    EVT_UPDATE_UI (wxID_UNDO, MainFrame::OnEditUpdateUI)
//    EVT_UPDATE_UI (wxID_REDO, MainFrame::OnEditUpdateUI)
//
//    //View menu/toolbar
//    EVT_MENU      (k_menu_view_tools, MainFrame::OnViewTools)
//    //EVT_UPDATE_UI (k_menu_view_tools, MainFrame::OnEditUpdateUI)
//    EVT_MENU      (k_menu_view_rulers, MainFrame::OnViewRulers)
//    EVT_UPDATE_UI (k_menu_view_rulers, MainFrame::OnViewRulersUI)
//    EVT_MENU      (k_menu_view_toolBar, MainFrame::OnViewToolBar)
//    EVT_UPDATE_UI (k_menu_view_toolBar, MainFrame::OnToolbarsUI)
//    EVT_MENU      (k_menu_view_statusBar, MainFrame::OnViewStatusBar)
//    EVT_UPDATE_UI (k_menu_view_statusBar, MainFrame::OnStatusbarUI)
//    EVT_MENU      (k_menu_view_page_margins, MainFrame::OnViewPageMargins)
//    EVT_UPDATE_UI (k_menu_view_page_margins, MainFrame::OnEditUpdateUI)
//    EVT_MENU      (k_menu_view_welcome_page, MainFrame::OnViewWelcomePage)
//    EVT_UPDATE_UI (k_menu_view_welcome_page, MainFrame::OnViewWelcomePageUI)
//
//    //Score menu/toolbar
//    EVT_MENU      (k_menu_score_titles, MainFrame::OnScoreTitles)
//    EVT_UPDATE_UI (k_menu_score_titles, MainFrame::OnEditUpdateUI)
//
//	// Instrument menu
//	EVT_MENU      (k_menu_instr_properties, MainFrame::OnInstrumentProperties)
//    EVT_UPDATE_UI (k_menu_instr_properties, MainFrame::OnEditUpdateUI)
//
//	//Zoom menu/toolbar
    EVT_MENU(k_menu_zoom_in, MainFrame::on_zoom_in)
    EVT_MENU(k_menu_zoom_out, MainFrame::on_zoom_out)
//    EVT_MENU (k_menu_zoom_100, MainFrame::OnZoom100)
//    EVT_MENU (k_menu_zoom_other, MainFrame::OnZoomOther)
//    EVT_MENU (k_menu_zoom_fit_full, MainFrame::OnZoomFitFull)
//    EVT_MENU (k_menu_zoom_fit_width, MainFrame::OnZoomFitWidth)
//    EVT_UPDATE_UI_RANGE (k_menu_zoom_100, k_menu_zoom_in, MainFrame::OnZoomUpdateUI)
//    EVT_COMBOBOX  (k_id_combo_zoom, MainFrame::OnComboZoom )
//    EVT_TEXT_ENTER(k_id_combo_zoom, MainFrame::OnComboZoom )
//    EVT_UPDATE_UI (k_id_combo_zoom, MainFrame::OnZoomUpdateUI)

    //Sound menu/toolbar
    EVT_MENU      (k_menu_sound_midi_wizard, MainFrame::on_run_midi_wizard)
    EVT_MENU      (k_menu_sound_test, MainFrame::on_sound_test)
    EVT_MENU      (k_menu_sound_off, MainFrame::on_all_sounds_off)
    EVT_MENU      (k_menu_play_start, MainFrame::on_play_start)
//    EVT_UPDATE_UI (k_menu_play_start, MainFrame::OnSoundUpdateUI)
//    EVT_MENU      (k_menu_play_cursor_start, MainFrame::OnPlayCursorStart)
//    EVT_UPDATE_UI (k_menu_play_cursor_start, MainFrame::OnSoundUpdateUI)
//    EVT_MENU      (k_menu_play_stop, MainFrame::OnPlayStop)
//    EVT_UPDATE_UI (k_menu_play_stop, MainFrame::OnSoundUpdateUI)
//    EVT_MENU      (k_menu_play_pause, MainFrame::OnPlayPause)
//    EVT_UPDATE_UI (k_menu_play_pause, MainFrame::OnSoundUpdateUI)
//    EVT_MENU      (MENU_Metronome, MainFrame::OnMetronomeOnOff)
//    EVT_UPDATE_UI (MENU_Metronome, MainFrame::OnSoundUpdateUI)
//
//    EVT_MENU (k_menu_preferences, MainFrame::OnOptions)
//
//    EVT_MENU      (k_menu_open_book, MainFrame::OnOpenBook)
//    EVT_UPDATE_UI (k_menu_open_book, MainFrame::OnOpenBookUI)
//
//    // Window menu
//    EVT_MENU (k_menu_windowClose, MainFrame::OnWindowClose)
//    EVT_MENU (k_menu_windowCloseAll, MainFrame::OnWindowCloseAll)
//    EVT_MENU (k_menu_windowNext, MainFrame::OnWindowNext)
//    EVT_MENU (k_menu_windowPrev, MainFrame::OnWindowPrev)
//
//    // Help menu
    EVT_MENU(k_menu_help_about, MainFrame::on_about)
//    EVT_MENU      (k_menu_help_open, MainFrame::OnHelpOpen)
//    EVT_MENU      (k_menu_help_quick_guide, MainFrame::OnHelpQuickGuide)
//    EVT_MENU      (lmMENU_CheckForUpdates, MainFrame::OnCheckForUpdates)
//    EVT_MENU      (k_menu_help_visit_website, MainFrame::OnVisitWebsite)

        //debug menu. Only visible in Debug mode
#if (LENMUS_DEBUG == 1)
    EVT_MENU(k_menu_debug_do_tests, MainFrame::on_do_tests)
    EVT_MENU(k_menu_debug_draw_box_docpage, MainFrame::on_debug_draw_box)
    EVT_MENU(k_menu_debug_draw_box_score_page, MainFrame::on_debug_draw_box)
    EVT_MENU(k_menu_debug_draw_box_system, MainFrame::on_debug_draw_box)
    EVT_MENU(k_menu_debug_draw_box_slice, MainFrame::on_debug_draw_box)
    EVT_MENU(k_menu_debug_draw_box_slice_instr, MainFrame::on_debug_draw_box)
    EVT_MENU(k_menu_debug_remove_boxes, MainFrame::on_debug_draw_box)
    EVT_MENU(k_menu_debug_justify_systems, MainFrame::on_debug_justify_systems)
    EVT_MENU(k_menu_debug_dump_column_tables, MainFrame::on_debug_dump_column_tables)
//    EVT_MENU (k_menu_debug_ForceReleaseBehaviour, MainFrame::OnDebugForceReleaseBehaviour)
//    EVT_MENU (k_menu_debug_ShowDebugLinks, MainFrame::OnDebugShowDebugLinks)
//    EVT_MENU (k_menu_debug_ShowBorderOnScores, MainFrame::OnDebugShowBorderOnScores)
//    EVT_MENU (k_menu_debug_SetTraceLevel, MainFrame::OnDebugSetTraceLevel)
//    EVT_MENU (k_menu_debug_PatternEditor, MainFrame::OnDebugPatternEditor)
//    EVT_MENU (k_menu_debug_recSelec, MainFrame::OnDebugRecSelec)
//    EVT_MENU (k_menu_debug_DrawAnchors, MainFrame::OnDebugDrawAnchors)
//    EVT_MENU (k_menu_debug_do_tests, MainFrame::OnDebugUnitTests)
//    EVT_MENU (k_menu_debug_ShowDirtyObjects, MainFrame::OnDebugShowDirtyObjects)
//
//        //debug events requiring a score to be enabled
//    EVT_MENU      (k_menu_debug_DumpStaffObjs, MainFrame::OnDebugDumpStaffObjs)
//    EVT_UPDATE_UI (k_menu_debug_DumpStaffObjs, MainFrame::OnDebugScoreUI)
//    EVT_MENU      (k_menu_debug_DumpGMObjects, MainFrame::OnDebugDumpGMObjects)
//    EVT_UPDATE_UI (k_menu_debug_DumpGMObjects, MainFrame::OnDebugScoreUI)
//    EVT_MENU      (k_menu_debug_SeeSource, MainFrame::OnDebugSeeSource)
//    EVT_UPDATE_UI (k_menu_debug_SeeSource, MainFrame::OnDebugScoreUI)
//    EVT_MENU      (k_menu_debug_SeeSourceUndo, MainFrame::OnDebugSeeSourceForUndo)
//    EVT_UPDATE_UI (k_menu_debug_SeeSourceUndo, MainFrame::OnDebugScoreUI)
//    EVT_MENU      (k_menu_debug_SeeXML, MainFrame::OnDebugSeeXML)
//    EVT_UPDATE_UI (k_menu_debug_SeeXML, MainFrame::OnDebugScoreUI)
//    EVT_MENU      (k_menu_debug_SeeMIDIEvents, MainFrame::OnDebugSeeMidiEvents)
//    EVT_UPDATE_UI (k_menu_debug_SeeMIDIEvents, MainFrame::OnDebugScoreUI)
//    EVT_MENU      (k_menu_debug_DumpBitmaps, MainFrame::OnDebugDumpBitmaps)
//    EVT_UPDATE_UI (k_menu_debug_DumpBitmaps, MainFrame::OnDebugScoreUI)
//    EVT_MENU      (k_menu_debug_CheckHarmony, MainFrame::OnDebugCheckHarmony)
//    EVT_UPDATE_UI (k_menu_debug_CheckHarmony, MainFrame::OnDebugScoreUI)
//    EVT_MENU      (k_menu_debug_TestProcessor, MainFrame::OnDebugTestProcessor)
//    EVT_UPDATE_UI (k_menu_debug_TestProcessor, MainFrame::OnDebugScoreUI)
#endif

//    //metronome
//    EVT_SPINCTRL    (k_id_spin_metronome, MainFrame::OnMetronomeUpdate)
//    EVT_TEXT        (k_id_spin_metronome,    MainFrame::OnMetronomeUpdateText)
//    EVT_TIMER       (k_id_timer_metronome,        MainFrame::OnMetronomeTimer)
//
//    //other events
//    EVT_CLOSE   (MainFrame::OnCloseWindow)
    EVT_SIZE(MainFrame::on_size)
//	EVT_CHAR(MainFrame::OnKeyPress)
//	EVT_MENU(k_id_key_F1, MainFrame::OnKeyF1)

END_EVENT_TABLE()



//static variables
LomseDoorway MainFrame::m_lomse;            //the Lomse library doorway

//---------------------------------------------------------------------------------------
MainFrame::MainFrame(ApplicationScope& appScope, const wxPoint& pos,
                     const wxSize& size)
    : ContentFrame(NULL, wxID_ANY, appScope.get_app_full_name(), pos, size)
    , m_appScope(appScope)
    , m_lastOpenFile("")
    , m_pWelcomeWnd(NULL)
    , m_pToolbar(NULL)
    , m_pTbFile(NULL)
    , m_pTbEdit(NULL)
    , m_pTbZoom(NULL)
    , m_pTbPlay(NULL)
    , m_pTbMtr(NULL)
    , m_pTbTextBooks(NULL)
//    , m_fClosingAll(false)
//    , m_pToolBox(NULL)
//    , m_pHelp(NULL)
//    , m_pBookController(NULL)
//    , m_pHtmlWin(NULL)
{
    create_menu();
    create_status_bar();
    initialize_lomse();

//#if 0   //change to '1' to display a test score at start
//    //create a music score and a View. The view will display the score
//    //when the paint event is sent to lomse, once the main windows is
//    //shown and the event handling loop is started
//    m_pCanvas->open_test_document();
//#endif

#if 0   //chage to '1' to automatically, run tests on start
    wxCommandEvent event;
    on_do_tests(event);
#endif

//    // set the app icon
//	// All non-MSW platforms use a bitmap. MSW uses an .ico file
//    #if defined(_LM_WINDOWS_)
//        //macro wxICON creates an icon using an icon resource on Windows.
//        SetIcon(wxICON(app_icon));
//	#else
//		SetIcon(wxArtProvider::GetIcon(_T("app_icon"), wxART_OTHER));
//	#endif
//
//	//acceleration keys table
//    wxAcceleratorEntry entries[1];
//    entries[0].Set(wxACCEL_CTRL, WXK_F1, wxID_ABOUT);
//    wxAcceleratorTable accel(1, entries);
//    SetAcceleratorTable(accel);
//
//	// create main metronome and associate it to frame metronome controls
//    //metronome speed. Default MM=60
//    long nMM = g_pPrefs->Read(_T("/Metronome/MM"), 60);
//    m_pMainMtr = new lmMetronome(nMM);
//    m_pMtr = m_pMainMtr;

//    // initialize status bar
//    m_pStatusBar = (lmStatusBar*)NULL;
//    bool fStatusBar = false;
//    g_pPrefs->Read(_T("/MainFrame/ViewStatusBar"), &fStatusBar);
//    if (!m_pStatusBar && fStatusBar)
//        CreateTheStatusBar();
//
//    // initialize flags for toggle buttons status
//    m_fHelpOpened = false;
//
//    // other initializations
//    m_fSilentCheck = false;     //default: visible 'check for updates' process
}

//---------------------------------------------------------------------------------------
MainFrame::~MainFrame()
{
    // deinitialize the layout manager
    m_layoutManager.UnInit();

//    delete m_pHelp;
//    delete m_pBookController;

    // save user configuration data
    wxConfigBase* pPrefs = m_appScope.get_preferences();
    if (pPrefs)
    {
//        lmDocManager* pDocManager = this->GetDocumentManager();
//        //save the last selected directories
//        g_pPaths->SetScoresPath( pDocManager->GetLastDirectory() );
//        pDocManager->SaveRecentFiles();

        // save the frame size and position
        wxSize wndSize = GetSize();
        wxPoint wndPos = GetPosition();
        bool fMaximized = IsMaximized();
        pPrefs->Write(_T("/MainFrame/Width"), wndSize.GetWidth());
        pPrefs->Write(_T("/MainFrame/Height"), wndSize.GetHeight());
        pPrefs->Write(_T("/MainFrame/Left"), wndPos.x );
        pPrefs->Write(_T("/MainFrame/Top"), wndPos.y );
        pPrefs->Write(_T("/MainFrame/Maximized"), fMaximized);
    }

//    //save metronome settings and delete main metronome
//    if (m_pMainMtr) {
//        g_pPrefs->Write(_T("/Metronome/MM"), m_pMainMtr->GetMM() );
//        delete m_pMainMtr;
//    }
}

//---------------------------------------------------------------------------------------
void MainFrame::create_menu()
{
    //Centralized code to create the menu bar.
    //bitmaps on menus are supported only on Windows and GTK+

    m_booksMenu = NULL;

	bool fDebug = true; //TODO: !g_fReleaseVersion;
    wxMenuItem* pItem;
    wxSize nIconSize(16, 16);
    wxConfigBase* pPrefs = m_appScope.get_preferences();

    // file menu --------------------------------------------------------------------------

    wxMenu* pMenuFile = new wxMenu;
    wxMenu* pSubmenuExport = new wxMenu;

    create_menu_item(pMenuFile, k_menu_file_new, _("&New\tCtrl+N"),
                    _("Open new blank document"), wxITEM_NORMAL, _T("tool_new"));
    create_menu_item(pMenuFile, k_menu_file_open, _("&Open ...\tCtrl+O"),
                    _("Open a document"), wxITEM_NORMAL, _T("tool_open"));
    create_menu_item(pMenuFile, k_menu_file_reload, _T("&Reload"),
                    _("Reload document"), wxITEM_NORMAL, _T("tool_reload"));
    create_menu_item(pMenuFile, k_menu_open_book, _("Open &books"),
                    _("Hide/show eMusicBooks"), wxITEM_NORMAL, _T("tool_open_ebook"));
    create_menu_item(pMenuFile, k_menu_file_Import, _("&Import..."),
                    _("Open a MusicXML score"), wxITEM_NORMAL);

    //-- export submenu --
    create_menu_item(pSubmenuExport, k_menu_file_export_MusicXML, _("MusicXML format"),
                    _("Save score as a MusicXML file"), wxITEM_NORMAL);
    create_menu_item(pSubmenuExport, k_menu_file_export_bmp, _("As &bmp image"),
                    _("Save score as BMP images"), wxITEM_NORMAL, _T("tool_save_as_bmp"));
    create_menu_item(pSubmenuExport, k_menu_file_export_jpg, _("As &jpg image"),
                    _("Save score as JPG images"), wxITEM_NORMAL, _T("tool_save_as_jpg"));

    pItem = new wxMenuItem(pMenuFile, k_menu_file_export, _("&Export ..."),
                          _("Save score in other formats"), wxITEM_NORMAL, pSubmenuExport);
    pItem->SetBitmap( wxArtProvider::GetBitmap(_T("empty"), wxART_TOOLBAR, nIconSize) );
    pMenuFile->Append(pItem);

    //-- end of export submenu --

    create_menu_item(pMenuFile, wxID_SAVE, _("&Save\tCtrl+S"),
                    _T(""), wxITEM_NORMAL, _T("tool_save"));
    create_menu_item(pMenuFile, wxID_SAVEAS, _("Save &as ..."),
                    _T(""), wxITEM_NORMAL);
    create_menu_item(pMenuFile, wxID_CLOSE, _("&Close\tCtrl+W"),
                    _("Close a score"), wxITEM_NORMAL);
    pMenuFile->AppendSeparator();

    create_menu_item(pMenuFile, k_menu_print, _("&Print ...\tCtrl+P"),
                    _T(""), wxITEM_NORMAL, _T("tool_print"));
    create_menu_item(pMenuFile, wxID_PRINT_SETUP, _("Print &Setup..."),
                    _("Configure printer options"), wxITEM_NORMAL );
    create_menu_item(pMenuFile, k_menu_print_Preview, _("Print Pre&view"),
                    _T(""), wxITEM_NORMAL);
    pMenuFile->AppendSeparator();

    create_menu_item(pMenuFile, wxID_EXIT, _("&Quit\tCtrl+Q"),
                    _("Exit program"), wxITEM_NORMAL, _T("tool_exit"));


//    // history of files visited.
//    GetDocumentManager()->FileHistoryUsesMenu(pMenuFile);

//    // eBooks menu -------------------------------------------------------------------
//    m_booksMenu = new wxMenu;
//    create_menu_item(m_booksMenu, k_menu_open_book, _("Open &books"),
//                _("Hide/show eMusicBooks"), wxITEM_NORMAL, _T("tool_open_ebook"));
//    create_menu_item(m_booksMenu, lmMENU_eBookPanel, _("View index"),
//                _("Show/hide navigation panel"), wxITEM_CHECK, _T("tool_index_panel"));
//
//    m_booksMenu->AppendSeparator();
//    create_menu_item(m_booksMenu, lmMENU_eBook_PagePrev, _("Back page"),
//            _("Previous page of current eMusicBook"), wxITEM_NORMAL, _T("tool_page_previous") );
//    create_menu_item(m_booksMenu, lmMENU_eBook_PageNext, _("Next page"),
//            _("Next page of current eMusicBook"), wxITEM_NORMAL, _T("tool_page_next") );
//
//    m_booksMenu->AppendSeparator();
//    create_menu_item(m_booksMenu, lmMENU_eBook_GoBack, _("Go back"),
//            _("Go to previous visited page"), wxITEM_NORMAL, _T("tool_previous") );
//    create_menu_item(m_booksMenu, lmMENU_eBook_GoForward, _("Go forward"),
//            _("Go to next visited page"), wxITEM_NORMAL, _T("tool_next") );


    // edit menu -------------------------------------------------------------------

    m_editMenu = new wxMenu;
    create_menu_item(m_editMenu, wxID_UNDO, _("&Undo"),
                _("Undo"), wxITEM_NORMAL, _T("tool_undo"));
    create_menu_item(m_editMenu, wxID_REDO, _("&Redo"),
                _("Redo"), wxITEM_NORMAL, _T("tool_redo"));


    // View menu -------------------------------------------------------------------

    wxMenu* pMenuView = new wxMenu;
    create_menu_item(pMenuView, k_menu_view_toolBar, _("Tool &bar"),
                _("Hide/show the tools bar"), wxITEM_CHECK);
    create_menu_item(pMenuView, k_menu_view_statusBar, _("&Status bar"),
                _("Hide/show the status bar"), wxITEM_CHECK);
    pMenuView->AppendSeparator();
    create_menu_item(pMenuView, k_menu_view_tools, _("&Tool box"),
                _("Hide/show edition tool box window"), wxITEM_CHECK);
    create_menu_item(pMenuView, k_menu_view_rulers, _("&Rulers"),
                _("Hide/show rulers"), wxITEM_CHECK);
    create_menu_item(pMenuView, k_menu_view_welcome_page, _("&Welcome page"),
                _("Hide/show welcome page"));


    // score menu ------------------------------------------------------------------

    wxMenu* pMenuScore = new wxMenu;
    create_menu_item(pMenuScore, k_menu_score_titles, _("Add title"),
				_("Add a title to the score"), wxITEM_NORMAL, _T("tool_add_text"));
    create_menu_item(pMenuScore, k_menu_view_page_margins, _("Margins and spacers"),
				_("Show/hide page margins and spacers"), wxITEM_CHECK, _T("tool_page_margins"));


    // instrument menu ------------------------------------------------------------------

    wxMenu* pMenuInstr = new wxMenu;
    create_menu_item(pMenuInstr, k_menu_instr_properties, _("Properties"),
				_("Edit name, abbreviation, MIDI settings and other properties"), wxITEM_NORMAL);


    // debug menu --------------------------------------------------------------------

    // Debug strings will not be translatable. It is mandatory that all development is
    // in English
    m_dbgMenu = NULL;
    if (fDebug)
	{
        m_dbgMenu = new wxMenu;

        create_menu_item(m_dbgMenu, k_menu_debug_do_tests, _T("Run unit tests"));
        m_dbgMenu->AppendSeparator();
        create_menu_item(m_dbgMenu, k_menu_debug_justify_systems, _("Justify systems"),
                        _T(""), wxITEM_CHECK);
        create_menu_item(m_dbgMenu, k_menu_debug_dump_column_tables, _("Dump column tables"));
        create_menu_item(m_dbgMenu, k_menu_debug_ForceReleaseBehaviour, _T("&Release Behaviour"),
            _T("Force release behaviour for certain functions"), wxITEM_CHECK);
        create_menu_item(m_dbgMenu, k_menu_debug_ShowDebugLinks, _T("&Include debug links"),
            _T("Include debug controls in exercises"), wxITEM_CHECK);
        create_menu_item(m_dbgMenu, k_menu_debug_ShowBorderOnScores, _T("&Border on ScoreAuxCtrol"),
            _T("Show border on ScoreAuxCtrol"), wxITEM_CHECK);
        create_menu_item(m_dbgMenu, k_menu_debug_recSelec, _T("&Draw recSelec"),
            _T("Force to draw selection rectangles around staff objects"), wxITEM_CHECK);

        //-- Draw bounds submenu --
        wxMenu* pSubmenuDrawBox = new wxMenu;

        create_menu_item(pSubmenuDrawBox, k_menu_debug_draw_box_docpage, _("Draw document page box"));
        create_menu_item(pSubmenuDrawBox, k_menu_debug_draw_box_score_page, _("Draw score page box"));
        create_menu_item(pSubmenuDrawBox, k_menu_debug_draw_box_system, _("Draw system box"));
        create_menu_item(pSubmenuDrawBox, k_menu_debug_draw_box_slice, _("Draw slice box"));
        create_menu_item(pSubmenuDrawBox, k_menu_debug_draw_box_slice_instr, _("Draw slice intrs box"));
        create_menu_item(pSubmenuDrawBox, k_menu_debug_remove_boxes, _("Remove drawn boxes"));

        pItem = new wxMenuItem(m_dbgMenu, k_menu_debug_draw_box, _T("Draw box ..."),
                            _T("Force to draw box rectangles"), wxITEM_NORMAL, pSubmenuDrawBox);
        m_dbgMenu->Append(pItem);


        //create_menu_item(m_dbgMenu, k_menu_debug_DrawBounds, _T("&Draw bounds"),
        //    _T("Force to draw bound rectangles around staff objects"), wxITEM_CHECK);
        create_menu_item(m_dbgMenu, k_menu_debug_DrawAnchors, _T("Draw anchors"),
            _T("Draw a red line to show anchor objects"), wxITEM_CHECK);
        create_menu_item(m_dbgMenu, k_menu_debug_ShowDirtyObjects, _T("&Show dirty objects"),
            _T("Render 'dirty' objects in red colour"), wxITEM_CHECK);
        create_menu_item(m_dbgMenu, k_menu_debug_SetTraceLevel, _T("Set trace level ...") );
        create_menu_item(m_dbgMenu, k_menu_debug_PatternEditor, _T("Test Pattern Editor") );
        create_menu_item(m_dbgMenu, k_menu_debug_DumpStaffObjs, _T("&Dump of score") );
		create_menu_item(m_dbgMenu, k_menu_debug_DumpGMObjects, _T("&Dump of graphical model") );
        create_menu_item(m_dbgMenu, k_menu_debug_SeeSource, _T("See &LDP source") );
        create_menu_item(m_dbgMenu, k_menu_debug_SeeSourceUndo, _T("See LDP source for &Undo/Redo") );
        create_menu_item(m_dbgMenu, k_menu_debug_SeeXML, _T("See &XML") );
        create_menu_item(m_dbgMenu, k_menu_debug_SeeMIDIEvents, _T("See &MIDI events") );
        create_menu_item(m_dbgMenu, k_menu_debug_DumpBitmaps, _T("Save offscreen bitmaps") );
        create_menu_item(m_dbgMenu, k_menu_debug_CheckHarmony, _T("Check harmony") );
        create_menu_item(m_dbgMenu, k_menu_debug_TestProcessor, _T("Run test processor") );
    }


    // Zoom menu -----------------------------------------------------------------------

//    m_zoomMenu->Append(k_menu_zoom_out);
//
    wxMenu* pMenuZoom = new wxMenu;
    create_menu_item(pMenuZoom, k_menu_zoom_100, _("Actual size"),
                _T("Zoom to real print size"), wxITEM_NORMAL, _T("tool_zoom_actual"));
    create_menu_item(pMenuZoom, k_menu_zoom_fit_full, _("Fit page full"),
                _("Zoom so that the full page is displayed"), wxITEM_NORMAL,
                _T("tool_zoom_fit_full"));
    create_menu_item(pMenuZoom, k_menu_zoom_fit_width, _("Fit page width"),
                _("Zoom so that page width equals window width"), wxITEM_NORMAL,
                _T("tool_zoom_fit_width"));
    create_menu_item(pMenuZoom, k_menu_zoom_in, _T("Zoom in\tCtrl-+"),
                _("Enlarge image size"), wxITEM_NORMAL, _T("tool_zoom_in"));
    create_menu_item(pMenuZoom, k_menu_zoom_out, _T("Zoom out\tCtrl--"),
                _("Reduce image size"), wxITEM_NORMAL, _T("tool_zoom_out"));
    create_menu_item(pMenuZoom, k_menu_zoom_other, _("Zoom to ..."));


    //Sound menu -------------------------------------------------------------------------

    wxMenu* pMenuSound = new wxMenu;

    create_menu_item(pMenuSound, k_menu_play_start, _("&Play"),
                _("Start/resume play back. From selection of full score"), wxITEM_NORMAL,
                _T("tool_play"));
    create_menu_item(pMenuSound, k_menu_play_cursor_start, _("Play from cursor"),
                _("Start/resume play back. From cursor measure"), wxITEM_NORMAL,
                _T("tool_play_cursor"));
    create_menu_item(pMenuSound, k_menu_play_stop, _("S&top"),
                _("Stop playing back"), wxITEM_NORMAL, _T("tool_stop"));
    create_menu_item(pMenuSound, k_menu_play_pause, _("P&ause"),
                _("Pause playing back"), wxITEM_NORMAL, _T("tool_pause"));
    create_menu_item(pMenuSound, k_menu_play_countoff, _("Do count off"),
                    _("Do count off before starting the play back"), wxITEM_CHECK);
    pMenuSound->AppendSeparator();

    create_menu_item(pMenuSound, MENU_Metronome, _("Metronome on"),
                    _("Turn metronome on/off"), wxITEM_CHECK);
    pMenuSound->AppendSeparator();

    create_menu_item(pMenuSound, k_menu_sound_midi_wizard, _("&Run Midi wizard"),
                    _("MIDI configuration wizard"), wxITEM_NORMAL, _T("tool_midi_wizard"));
	pMenuSound->AppendSeparator();

    create_menu_item(pMenuSound, k_menu_sound_test, _("&Test sound"),
                    _("Play an scale to test sound"), wxITEM_NORMAL, _T("tool_test_sound"));
    create_menu_item(pMenuSound, k_menu_sound_off, _("&All sounds off"),
                    _("Stop inmediatly all sounds"), wxITEM_NORMAL, _T("tool_stop_sounds"));


    // Options menu ---------------------------------------------------------------------

    wxMenu* pMenuOptions = new wxMenu;
    create_menu_item(pMenuOptions, k_menu_preferences,  _("&Preferences"),
                    _("Open help book"), wxITEM_NORMAL, _T("tool_options"));


    // Window menu -----------------------------------------------------------------------
    wxMenu* pMenuWindow = new wxMenu;
    pMenuWindow->Append(k_menu_windowClose,    _("Cl&ose"));
    pMenuWindow->Append(k_menu_windowCloseAll, _("Close All"));
    pMenuWindow->AppendSeparator();

    pMenuWindow->Append(k_menu_windowNext,     _("&Next"));
    pMenuWindow->Append(k_menu_windowPrev,     _("&Previous"));


    // Help menu -------------------------------------------------------------------------

    wxMenu* pMenuHelp = new wxMenu;

    create_menu_item(pMenuHelp, k_menu_help_about, _("&About"),
				_("Display information about program version and credits"), wxITEM_NORMAL,
                _T("tool_about"));
    pMenuHelp->AppendSeparator();

    create_menu_item(pMenuHelp, k_menu_help_quick_guide,  _("Editor quick guide"),
                _("Show editor reference card"), wxITEM_NORMAL, _T("tool_quick_guide"));
    create_menu_item(pMenuHelp, k_menu_help_open,  _("&Content"),
                _("Open help book"), wxITEM_NORMAL, _T("tool_help"));
    pMenuHelp->AppendSeparator();

//	create_menu_item(pMenuHelp, lmMENU_CheckForUpdates, _("Check now for &updates"),
//				_("Connect to the Internet and check for program updates"), wxITEM_NORMAL,
//                _T("tool_web_update"));
    create_menu_item(pMenuHelp, k_menu_help_visit_website,  _("&Visit LenMus website"),
                _("Open the Internet browser and go to LenMus website"), wxITEM_NORMAL,
                _T("tool_website"));


    // set up the menubar ---------------------------------------------------------------

    // AWARE: As MainFrame is derived from wxFrame, in MSWindows the menu
    // bar automatically inherits a "Window" menu inserted in the second last position.
    // To suppress it (under MSWindows) it is necessary to add style
    // wxFRAME_NO_WINDOW_MENU in frame creation.
    wxMenuBar* pMenuBar = new wxMenuBar;
    pMenuBar->Append(pMenuFile, _("&File"));
    //pMenuBar->Append(m_booksMenu, _("e&Books"));
    pMenuBar->Append(m_editMenu, _("&Edit"));
    pMenuBar->Append(pMenuView, _("&View"));
	pMenuBar->Append(pMenuScore, _("S&core"));
	pMenuBar->Append(pMenuInstr, _("&Instrument"));
    pMenuBar->Append(pMenuSound, _("&Sound"));
    if (fDebug) pMenuBar->Append(m_dbgMenu, _T("&Debug"));     //DO NOT TRANSLATE
    pMenuBar->Append(pMenuZoom, _("&Zoom"));
    pMenuBar->Append(pMenuOptions, _("&Options"));
    pMenuBar->Append(pMenuWindow, _("&Window"));
    pMenuBar->Append(pMenuHelp, _("&Help"));

//        //
//        // items initially checked
//        //
//
//    g_fDrawSelRect = false;    //true;
//
//    //debug toolbar
//    if (fDebug) {
//        pMenuBar->Check(k_menu_debug_ForceReleaseBehaviour, g_fReleaseBehaviour);
//        pMenuBar->Check(k_menu_debug_ShowDebugLinks, g_fShowDebugLinks);
//        pMenuBar->Check(k_menu_debug_recSelec, g_fDrawSelRect);
//        pMenuBar->Check(k_menu_debug_DrawAnchors, g_fDrawAnchors);
//    }

    // view toolbar
    bool fToolBar = true;
    pPrefs->Read(_T("/MainFrame/ViewToolBar"), &fToolBar);
    pMenuBar->Check(k_menu_view_toolBar, fToolBar);

    // view status bar
    bool fStatusBar = true;
    pPrefs->Read(_T("/MainFrame/ViewStatusBar"), &fStatusBar);
    pMenuBar->Check(k_menu_view_statusBar, fStatusBar);

    // do count off
    pMenuBar->Check(k_menu_play_countoff, true);

    SetMenuBar(pMenuBar);
}

//---------------------------------------------------------------------------------------
void MainFrame::create_status_bar()
{
//    m_pStatusBar = new wxStatusBar();(this, (lmEStatusBarLayout)nType, k_menu_view_statusBar);
//    SetStatusBar(m_pStatusBar);
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar();
    SetStatusText(_("Welcome to Lenmus 5.0!"));
}

//---------------------------------------------------------------------------------------
void MainFrame::create_controls()
{
    m_layoutManager.SetManagedWindow(this);     //inform wxAUI which frame to use

    create_tool_bar();

    //create the ContentWindow (main pane, a notebook) for scores and other content
    long style = wxAUI_NB_CLOSE_ON_ACTIVE_TAB |
                 wxAUI_NB_WINDOWLIST_BUTTON |
                 wxAUI_NB_SCROLL_BUTTONS |
                 wxAUI_NB_TAB_MOVE ;

    m_pContentWindow = new ContentWindow(this, style);

    m_layoutManager.AddPane(m_pContentWindow, wxAuiPaneInfo().Name(wxT("MainPane")).
                    CenterPane().PaneBorder(false));

    unsigned int flags = wxAUI_MGR_ALLOW_FLOATING
                       | wxAUI_MGR_TRANSPARENT_HINT
                       | wxAUI_MGR_HINT_FADE
					   | wxAUI_MGR_ALLOW_ACTIVE_PANE
                       | wxAUI_MGR_NO_VENETIAN_BLINDS_FADE
	                   | wxAUI_MGR_TRANSPARENT_DRAG;

	m_layoutManager.SetFlags(flags);
	m_layoutManager.Update();
}

//---------------------------------------------------------------------------------------
void MainFrame::initialize_lomse()
{
    // Lomse knows nothing about windows. It renders everything on bitmaps and the
    // user application uses them. For instance, to display it on a wxWindos.
    // Lomse supports a lot of bitmap formats and pixel formats. Therefore, before
    // using the Lomse library you MUST specify which bitmap formap to use.
    //
    // For wxWidgets, I would suggets using a platform independent format. So
    // I will use a wxImage as the rendering  buffer. wxImage is platform independent
    // and its buffer is an array of characters in RGBRGBRGB... format,  in the
    // top-to-bottom, left-to-right order. That is, the first RGB triplet corresponds
    // to the first pixel of the first row; the second RGB triplet, to the second
    // pixel of the first row, and so on until the end of the first row,
    // with second row following after it and so on.
    // Therefore, the pixel format is RGB 24 bits.
    //
    // Let's define the requiered information:

        //the pixel format
        int pixel_format = k_pix_format_rgb24;  //RGB 24bits

        //the desired resolution. For Linux and Windows use 96 pixels per inch
        int resolution = 96;    //96 ppi

        //Normal y axis direction is 0 coordinate at top and increase downwards. You
        //must specify if you would like just the opposite behaviour. For Windows and
        //Linux the default behaviour is the right behaviour.
        bool reverse_y_axis = false;

    //redirect cout to my own stream, to intercept and display error msgs.
    m_cout_buffer = cout.rdbuf();
    cout.rdbuf (m_reporter.rdbuf());

    //Now, initialize the library with these values
    m_lomse.init_library(pixel_format,resolution, reverse_y_axis, m_reporter);

    //set required callbacks
    m_lomse.set_get_font_callback(get_font_filename);
    m_lomse.set_notify_callback(on_lomse_event);
}

//---------------------------------------------------------------------------------------
void MainFrame::on_quit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

//---------------------------------------------------------------------------------------
void MainFrame::on_about(wxCommandEvent& WXUNUSED(event))
{
    (void)wxMessageBox(_T("Lenmus Phonascus v5.0 alpha 0 for GNU/Linux"),
                       _T("About Lenmus Phonascus"),
                       wxICON_INFORMATION);
//   AboutDialog dlg(this);
//   dlg.ShowModal();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_file_open(wxCommandEvent& WXUNUSED(event))
{
    wxString defaultPath = _T(LENMUS_TEST_SCORES_PATH);

    wxString p = wxFileSelector(_("Open score"), defaultPath,
        wxEmptyString, wxEmptyString, wxT("LenMus files|*.lms;*.lmd"));

    if (!p.empty())
    {
        std::string filename( p.mb_str(wxConvUTF8) );
        load_file(filename);
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::on_file_reload(wxCommandEvent& WXUNUSED(event))
{
    load_file(m_lastOpenFile);
}

//---------------------------------------------------------------------------------------
void MainFrame::load_file(string& filename)
{
    m_reporter.str(std::string());      //remove any previous content

    ScoreCanvas* pCanvas = new ScoreCanvas(this, m_appScope, m_lomse);
    pCanvas->display_document(filename);
    m_lastOpenFile = filename;

    //show errors, if any
    if (!m_reporter.str().empty())
    {
        wxString msg = to_wx_string( m_reporter.str() );
        wxString title = _("Errors in file ");
        title += to_wx_string(filename);
        DlgDebug dlg(this, title, msg, true); //true = 'Save' button
        dlg.ShowModal();
    }
    m_reporter.str(std::string());      //remove any previous content
}

//---------------------------------------------------------------------------------------
void MainFrame::on_size(wxSizeEvent& WXUNUSED(event))
{
    //Reorganize toolbar items in rows, to fit frame size

    if (!m_pTbMtr) return;      //toolbars not yet created

    //AS I can not measure the grip witdh I will use a good approx.: half toolbar height
    int nGripWidth = m_pTbMtr->GetSize().GetHeight() / 2 + 1;

    int nRow = 0;
    int nPos = 0;
    wxSize size = this->GetClientSize();
    int nWidth;
    int nAvailable = size.x;

    //Pointers, in presentation order:
    wxToolBar* pTool[7];
    pTool[0] = m_pTbFile;       //File tools
    pTool[1] = m_pTbEdit;       //Edit tools
    pTool[2] = m_pTbZoom;       //Zooming tools
    pTool[3] = m_pToolbar;      //Main tools
    pTool[4] = m_pTbPlay;       //Play tools
    pTool[5] = m_pTbMtr;        //Metronome tools
    pTool[6] = m_pTbTextBooks;  //Textbooks

    //at least the longest toolbar
    nWidth = m_pTbEdit->GetSize().GetWidth() + nGripWidth;
    if (nAvailable < nWidth)
        return;

    for (int i=0; i < 7; i++)
    {
        if (pTool[i])
        {
            nWidth = pTool[i]->GetSize().GetWidth() + nGripWidth;
            if (nAvailable < nWidth)
            {
                ++nRow;
                nPos = 0;
                nAvailable = size.x;
            }
            nAvailable -= nWidth;
            m_layoutManager.GetPane(pTool[i]).Top().Row(nRow).Position(nPos);
        }
    }

    // tell the manager to "commit" all the changes just made
    m_layoutManager.Update();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_lomse_event(EventInfo& event)
{
    //wxMessageBox(_T("Global notification received from Lomse"));
    if (event.is_dynamic_content_event())
    {
        LibraryScope* pLibScope = m_lomse.get_library_scope();
        EventDynamic& ev = dynamic_cast<EventDynamic&>(event);
        ImoDocument* pDoc = ev.get_document();

        ImoObj* pImo = ev.get_object();
        delete pImo;
        ImoParagraph* pPara = new ImoParagraph();
        ImoObjFactory f(*pLibScope, cout);
        ImoTextItem* pText = dynamic_cast<ImoTextItem*>( f.create_object(
            "(txt \"This is dynamic content\")"));
        ImoStyle* pStyle = pDoc->get_style_info("Default style");
        pText->set_style(pStyle);
        pPara->set_style(pStyle);
        pPara->add_item(pText);
        ev.set_object(pPara);
    }
}

//---------------------------------------------------------------------------------------
string MainFrame::get_font_filename(const string& fontname, bool bold, bool italic)
{
    //This is just a trivial example. In real applications you should
    //use operating system services to find a suitable font

    //notes on parameters received:
    // - fontname can be either the face name (i.e. "Book Antiqua") or
    //   the familly name (i.e. "sans-serif")

#if (LENMUS_PLATFORM_UNIX == 1)

    string path = "/usr/share/fonts/truetype/";

    //if family name, choose a font name
    string name = fontname;
    if (name == "serif")
        name = "Times New Roman";
    else if (name == "sans-serif")
        name = "Tahoma";
    else if (name == "handwritten" || name == "cursive")
        name = "Monotype Corsiva";
    else if (name == "monospaced")
        name = "Courier New";

    //choose a suitable font file
    string fontfile;
    if (name == "Times New Roman")
    {
        if (italic && bold)
            fontfile = "freefont/FreeSerifBoldItalic.ttf";
        else if (italic)
            fontfile = "freefont/FreeSerifItalic.ttf";
        else if (bold)
            fontfile = "freefont/FreeSerifBold.ttf";
        else
            fontfile = "freefont/FreeSerif.ttf";
    }

    else if (name == "Tahoma")
    {
        if (bold)
            fontfile = "freefont/FreeSansOblique.ttf";
        else
            fontfile = "freefont/FreeSans.ttf";
    }

    else if (name == "Monotype Corsiva")
    {
        fontfile = "ttf-dejavu/DejaVuSans-Oblique.ttf";
    }

    else if (name == "Courier New")
    {
        if (italic && bold)
            fontfile = "freefont/FreeMonoBoldOblique.ttf";
        else if (italic)
            fontfile = "freefont/FreeMonoOblique.ttf";
        else if (bold)
            fontfile = "freefont/FreeMonoBold.ttf";
        else
            fontfile = "freefont/FreeMono.ttf";
    }

    else
        fontfile = "freefont/FreeSerif.ttf";


   return path + fontfile;
#elif (LENMUS_PLATFORM_WIN32 == 1)

    return "";

#else

    return "";

#endif
}

//---------------------------------------------------------------------------------------
void MainFrame::on_zoom_in(wxCommandEvent& WXUNUSED(event))
{
    ScoreCanvas* pCanvas = dynamic_cast<ScoreCanvas*>(get_active_canvas());
    if (pCanvas)
        pCanvas->zoom_in();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_zoom_out(wxCommandEvent& WXUNUSED(event))
{
    ScoreCanvas* pCanvas = dynamic_cast<ScoreCanvas*>(get_active_canvas());
    if (pCanvas)
        pCanvas->zoom_out();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_do_tests(wxCommandEvent& WXUNUSED(event))
{
    MyTestRunner oTR(this);
    oTR.RunTests();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_draw_box(wxCommandEvent& event)
{
    int key = '0';
    switch (event.GetId())
    {
        case k_menu_debug_draw_box_docpage:         key = '1'; break;
        case k_menu_debug_draw_box_score_page:      key = '2'; break;
        case k_menu_debug_draw_box_system:          key = '3'; break;
        case k_menu_debug_draw_box_slice:           key = '4'; break;
        case k_menu_debug_draw_box_slice_instr:     key = '5'; break;
        default:
            key = '0';
    }
    ScoreCanvas* pCanvas = dynamic_cast<ScoreCanvas*>(get_active_canvas());
    if (pCanvas)
        pCanvas->on_key(0, 0, key, 0);
}

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_justify_systems(wxCommandEvent& event)
{
    bool fChecked = m_dbgMenu->IsChecked(k_menu_debug_justify_systems);
    LibraryScope* pScope = m_lomse.get_library_scope();
    pScope->set_justify_systems(fChecked);

    ScoreCanvas* pCanvas = dynamic_cast<ScoreCanvas*>(get_active_canvas());
    if (pCanvas)
        pCanvas->on_document_updated();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_dump_column_tables(wxCommandEvent& event)
{
    LibraryScope* pScope = m_lomse.get_library_scope();
    pScope->set_dump_column_tables(true);
    ScoreCanvas* pCanvas = dynamic_cast<ScoreCanvas*>(get_active_canvas());
    if (pCanvas)
        pCanvas->on_document_updated();
    pScope->set_dump_column_tables(false);
}

////---------------------------------------------------------------------------------------
//void MainFrame::OnMetronomeOnOff(wxCommandEvent& WXUNUSED(event))
//{
//    if (m_pMtr->IsRunning()) {
//        m_pMtr->Stop();
//        //TODO switch off metronome LED
//    }
//    else {
//        m_pMtr->Start();
//    }
//
//}
//
//// Recreate toolbars if visible. User has changed visualization options
//void MainFrame::UpdateToolbarsLayout()
//{
//	if (m_pToolbar) {
//		DeleteToolbar();
//		create_tool_bar();
//	}
//}


//---------------------------------------------------------------------------------------
void MainFrame::create_tool_bar()
{
    if (m_pToolbar) return;

    bool fToolBar = true;
    wxConfigBase* pPrefs = m_appScope.get_preferences();
    pPrefs->Read(_T("/MainFrame/ViewToolBar"), &fToolBar);
    if (!fToolBar)
        return;


    //prepare style
    long style = wxTB_FLAT | wxTB_NODIVIDER;
    long nLabelsIndex = pPrefs->Read(_T("/Toolbars/Labels"), 0L);
    if (nLabelsIndex == 1)
        style |= wxTB_TEXT;
    else if (nLabelsIndex == 2)
        style |= wxTB_HORZ_TEXT;

    //prepare icons size
    long nIconSize = pPrefs->Read(_T("/Toolbars/IconSize"), 16);
    wxSize nSize(nIconSize, nIconSize);


    //create main tool bar
    m_pToolbar = new wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize, style);
    m_pToolbar->SetToolBitmapSize(nSize);
    m_pToolbar->AddTool(k_menu_preferences, _T("Preferences"), wxArtProvider::GetBitmap(_T("tool_options"), wxART_TOOLBAR, nSize), _("Set user preferences"));
    m_pToolbar->AddTool(k_menu_help_open, _T("Help"), wxArtProvider::GetBitmap(_T("tool_help"), wxART_TOOLBAR, nSize), _("Help button"));
    m_pToolbar->Realize();

    //File toolbar
    m_pTbFile = new wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize, style);
    m_pTbFile->SetToolBitmapSize(nSize);
    m_pTbFile->AddTool(k_menu_file_new, _T("New"),
            wxArtProvider::GetBitmap(_T("tool_new"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_new_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("New score"));
    m_pTbFile->AddTool(k_menu_file_open, _T("Open"), wxArtProvider::GetBitmap(_T("tool_open"),
            wxART_TOOLBAR, nSize), _("Open a score"));
    m_pTbFile->AddTool(k_menu_open_book, _T("Books"),
            wxArtProvider::GetBitmap(_T("tool_open_ebook"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_open_ebook_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Open the music books"));
    m_pTbFile->AddTool(wxID_SAVE, _T("Save"),
            wxArtProvider::GetBitmap(_T("tool_save"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_save_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Save current score to disk"));
    m_pTbFile->AddTool(k_menu_print, _T("Print"),
            wxArtProvider::GetBitmap(_T("tool_print"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_print_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Print document"));
    m_pTbFile->Realize();

    //Edit toolbar
    m_pTbEdit = new wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize, style);
    m_pTbEdit->SetToolBitmapSize(nSize);
    m_pTbEdit->AddTool(k_menu_edit_copy, _T("Copy"),
            wxArtProvider::GetBitmap(_T("tool_copy"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_copy_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Copy"));
    m_pTbEdit->AddTool(k_menu_edit_cut, _T("Cut"),
            wxArtProvider::GetBitmap(_T("tool_cut"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_cut_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Cut"));
    m_pTbEdit->AddTool(k_menu_edit_paste, _T("Paste"),
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
    m_pTbEdit->AddTool(k_menu_view_page_margins, _T("Page margins"),
            wxArtProvider::GetBitmap(_T("tool_page_margins"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_page_margins"), wxART_TOOLBAR, nSize),
            wxITEM_CHECK, _("Show/hide page margins and spacers"));
    m_pTbEdit->Realize();

    //Zoom toolbar
    m_pTbZoom = new wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize, style);
    m_pTbZoom->SetToolBitmapSize(nSize);
    m_pTbZoom->AddTool(k_menu_zoom_fit_full, _T("Fit full"),
            wxArtProvider::GetBitmap(_T("tool_zoom_fit_full"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_zoom_fit_full_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Zoom so that the full page is displayed"));
    m_pTbZoom->AddTool(k_menu_zoom_fit_width, _T("Fit width"),
            wxArtProvider::GetBitmap(_T("tool_zoom_fit_width"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_zoom_fit_width_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Zoom so that page width equals window width"));
    m_pTbZoom->AddTool(k_menu_zoom_in, _T("Zoom in"),
            wxArtProvider::GetBitmap(_T("tool_zoom_in"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_zoom_in_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Enlarge image size"));
    m_pTbZoom->AddTool(k_menu_zoom_out, _T("Zoom out"),
            wxArtProvider::GetBitmap(_T("tool_zoom_out"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_zoom_out_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Reduce image size"));

    m_pComboZoom = new wxComboBox(m_pTbZoom, k_id_combo_zoom, _T(""),
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
    m_pTbPlay->AddTool(k_menu_play_start, _T("Play"),
            wxArtProvider::GetBitmap(_T("tool_play"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_play_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Start/resume play back. From selection of full score"));
    m_pTbPlay->AddTool(k_menu_play_cursor_start, _T("Play from cursor"),
            wxArtProvider::GetBitmap(_T("tool_play_cursor"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_play_cursor_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Start/resume play back. From cursor measure"));
    m_pTbPlay->AddTool(k_menu_play_stop, _T("Stop"),
            wxArtProvider::GetBitmap(_T("tool_stop"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_stop_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Stop playing back"));
    m_pTbPlay->AddTool(k_menu_play_pause, _T("Pause"),
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
    m_pSpinMetronome = new wxSpinCtrl(m_pTbMtr, k_id_spin_metronome, _T(""), wxDefaultPosition,
        wxSize(60, -1), wxSP_ARROW_KEYS | wxSP_WRAP, 20, 300);
//    m_pSpinMetronome->SetValue( m_pMtr->GetMM() );
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
    const int ROW_2 = 1;
#if (LENMUS_PLATFORM_UNIX == 1)
    //In gtk reverse creation order
        // row 1
//    CreateTextBooksToolBar(style, nSize, ROW_1);

    m_layoutManager.AddPane(m_pTbMtr, wxAuiPaneInfo().
                Name(wxT("Metronome")).Caption(_("Metronome tools")).
                ToolbarPane().Top().Row(ROW_1).BestSize( sizeBest ).
                LeftDockable(false).RightDockable(false));
    m_layoutManager.AddPane(m_pTbPlay, wxAuiPaneInfo().
                Name(wxT("Play")).Caption(_("Play tools")).
                ToolbarPane().Top().Row(ROW_1).
                LeftDockable(false).RightDockable(false));
    m_layoutManager.AddPane(m_pToolbar, wxAuiPaneInfo().
                Name(wxT("toolbar")).Caption(_("Main tools")).
                ToolbarPane().Top().
                LeftDockable(false).RightDockable(false));
    m_layoutManager.AddPane(m_pTbZoom, wxAuiPaneInfo().
                Name(wxT("Zooming tools")).Caption(_("Zooming tools")).
                ToolbarPane().Top().BestSize( sizeZoomTb ).
                LeftDockable(false).RightDockable(false));
    m_layoutManager.AddPane(m_pTbEdit, wxAuiPaneInfo().
                Name(wxT("Edit tools")).Caption(_("Edit tools")).
                ToolbarPane().Top().
                LeftDockable(false).RightDockable(false));
    m_layoutManager.AddPane(m_pTbFile, wxAuiPaneInfo().
                Name(wxT("File tools")).Caption(_("File tools")).
                ToolbarPane().Top().
                LeftDockable(false).RightDockable(false));

#else
    // row 1
    m_layoutManager.AddPane(m_pTbFile, wxAuiPaneInfo().
                Name(wxT("File tools")).Caption(_("File tools")).
                ToolbarPane().Top().
                LeftDockable(false).RightDockable(false));
    m_layoutManager.AddPane(m_pTbEdit, wxAuiPaneInfo().
                Name(wxT("Edit tools")).Caption(_("Edit tools")).
                ToolbarPane().Top().
                LeftDockable(false).RightDockable(false));
    m_layoutManager.AddPane(m_pTbZoom, wxAuiPaneInfo().
                Name(wxT("Zooming tools")).Caption(_("Zooming tools")).
                ToolbarPane().Top().BestSize( sizeZoomTb ).
                LeftDockable(false).RightDockable(false));
    m_layoutManager.AddPane(m_pToolbar, wxAuiPaneInfo().
                Name(wxT("toolbar")).Caption(_("Main tools")).
                ToolbarPane().Top().
                LeftDockable(false).RightDockable(false));
    m_layoutManager.AddPane(m_pTbPlay, wxAuiPaneInfo().
                Name(wxT("Play")).Caption(_("Play tools")).
                ToolbarPane().Top().Row(ROW_1).
                LeftDockable(false).RightDockable(false));
    m_layoutManager.AddPane(m_pTbMtr, wxAuiPaneInfo().
                Name(wxT("Metronome")).Caption(_("Metronome tools")).
                ToolbarPane().Top().Row(ROW_1).BestSize( sizeBest ).
                LeftDockable(false).RightDockable(false));

//    CreateTextBooksToolBar(style, nSize, ROW_1);

#endif

    // tell the manager to "commit" all the changes just made
    m_layoutManager.Update();
}

//void MainFrame::DeleteToolbar()
//{
//    // main toolbar
//    if (m_pToolbar) {
//        m_layoutManager.DetachPane(m_pToolbar);
//        delete m_pToolbar;
//        m_pToolbar = (wxToolBar*)NULL;
//    }
//
//    // file toolbar
//    if (m_pTbFile) {
//        m_layoutManager.DetachPane(m_pTbFile);
//        delete m_pTbFile;
//        m_pTbFile = (wxToolBar*)NULL;
//    }
//
//    // edit toolbar
//    if (m_pTbEdit) {
//        m_layoutManager.DetachPane(m_pTbEdit);
//        delete m_pTbEdit;
//        m_pTbEdit = (wxToolBar*)NULL;
//    }
//
//    // play toolbar
//    if (m_pTbPlay) {
//        m_layoutManager.DetachPane(m_pTbPlay);
//        delete m_pTbPlay;
//        m_pTbPlay = (wxToolBar*)NULL;
//    }
//
//    // metronome toolbar
//    if (m_pTbMtr) {
//        m_layoutManager.DetachPane(m_pTbMtr);
//        delete m_pTbMtr;
//        m_pTbMtr = (wxToolBar*)NULL;
//    }
//
//    // zoom toolbar
//    if (m_pTbZoom) {
//        m_layoutManager.DetachPane(m_pTbZoom);
//        delete m_pTbZoom;
//        m_pTbZoom = (wxToolBar*)NULL;
//    }
//
//    // Text books navigation toolbar
//    if (m_pTbTextBooks) {
//        m_layoutManager.DetachPane(m_pTbTextBooks);
//        delete m_pTbTextBooks;
//        m_pTbTextBooks = (wxToolBar*)NULL;
//    }
//
//    // tell the manager to "commit" all the changes just made
//    m_layoutManager.Update();
//}
//
//void MainFrame::CreateTextBooksToolBar(long style, wxSize nIconSize, int nRow)
//{
//    m_pTbTextBooks = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
//    m_pTbTextBooks->SetToolBitmapSize(nIconSize);
//
//    //add tools
//    m_pTbTextBooks->AddTool(lmMENU_eBookPanel, _T("Index"),
//            wxArtProvider::GetBitmap(_T("tool_index_panel"), wxART_TOOLBAR, nIconSize),
//            wxArtProvider::GetBitmap(_T("tool_index_panel_dis"), wxART_TOOLBAR, nIconSize),
//            wxITEM_CHECK, _("Show/hide navigation panel") );
//    m_pTbTextBooks->ToggleTool(lmMENU_eBookPanel, false);
//
//    m_pTbTextBooks->AddSeparator();
//    m_pTbTextBooks->AddTool(lmMENU_eBook_PagePrev, _T("Back page"),
//            wxArtProvider::GetBitmap(_T("tool_page_previous"), wxART_TOOLBAR, nIconSize),
//            wxArtProvider::GetBitmap(_T("tool_page_previous_dis"), wxART_TOOLBAR, nIconSize),
//            wxITEM_NORMAL, _("Previous page of current eMusicBook") );
//    m_pTbTextBooks->AddTool(lmMENU_eBook_PageNext, _T("Next page"),
//            wxArtProvider::GetBitmap(_T("tool_page_next"), wxART_TOOLBAR, nIconSize),
//            wxArtProvider::GetBitmap(_T("tool_page_next_dis"), wxART_TOOLBAR, nIconSize),
//            wxITEM_NORMAL, _("Next page of current eMusicBook") );
//
//    m_pTbTextBooks->AddSeparator();
//    m_pTbTextBooks->AddTool(lmMENU_eBook_GoBack, _T("Go back"),
//            wxArtProvider::GetBitmap(_T("tool_previous"), wxART_TOOLBAR, nIconSize),
//            _("Go to previous visited page"), wxITEM_NORMAL );
//    m_pTbTextBooks->AddTool(lmMENU_eBook_GoForward, _T("Go forward"),
//            wxArtProvider::GetBitmap(_T("tool_next"), wxART_TOOLBAR, nIconSize),
//            _("Go to next visited page"), wxITEM_NORMAL );
//
//    m_pTbTextBooks->Realize();
//
//    m_layoutManager.AddPane(m_pTbTextBooks, wxAuiPaneInfo().
//                Name(_T("Navigation")).Caption(_("eBooks navigation tools")).
//                ToolbarPane().Top().Row(nRow).
//                LeftDockable(false).RightDockable(false));
//
//}
//
//void MainFrame::CreateTheStatusBar(int nType)
//{
//    //if the status bar exists and it is of same type, nothing to do
//    if (m_pStatusBar && m_pStatusBar->GetType() == nType) return;
//
//    //create the status bar
//    if (m_pStatusBar)
//        delete m_pStatusBar;
//
//    m_pStatusBar = new lmStatusBar(this, (lmEStatusBarLayout)nType, k_menu_view_statusBar);
//    SetStatusBar(m_pStatusBar);
//
//    //the status bar pane is used to display menu and toolbar help.
//    //Using -1 disables help display.
//    SetStatusBarPane(-1);
//
//    SendSizeEvent();
//}
//
//void MainFrame::DeleteTheStatusBar()
//{
//    if (!m_pStatusBar) return;
//
//    //delete status bar
//    SetStatusBar(NULL);
//    delete m_pStatusBar;
//    m_pStatusBar = (lmStatusBar*)NULL;
//    SendSizeEvent();
//}

//---------------------------------------------------------------------------------------
void MainFrame::create_menu_item(wxMenu* pMenu, int nId, const wxString& sItemName,
                                 const wxString& sToolTip, wxItemKind nKind,
                                 const wxString& sIconName)
{
    //Create a menu item and add it to the received menu

    wxMenuItem* pItem = new wxMenuItem(pMenu, nId, sItemName, sToolTip, nKind);


    //icons are supported only in Windows and Linux, and only in wxITEM_NORMAL items
    #if (LENMUS_PLATFORM_WIN32 == 1 || LENMUS_PLATFORM_UNIX == 1)
    if (nKind == wxITEM_NORMAL)
        pItem->SetBitmap( wxArtProvider::GetBitmap(sIconName, wxART_TOOLBAR, wxSize(16, 16)) );
    #endif

    pMenu->Append(pItem);
}

//void MainFrame::OnCloseBookFrame()
//{
//    //the TexBookFrame has been closed. Clean up
//    delete m_pBookController;
//    m_pBookController = (lmTextBookController*)NULL;
//}
//
//void MainFrame::InitializeHelp()
//{
//    // create the help window
//    //m_pHelp = new lmHelpController(wxHF_DEFAULT_STYLE | wxHF_FLAT_TOOLBAR );
//    // previous sentence commented out and replaced by next one to remove
//    // index panel.
//    m_pHelp = new lmHelpController(wxHF_TOOLBAR | wxHF_FLAT_TOOLBAR | wxHF_CONTENTS |
//                        wxHF_SEARCH | wxHF_BOOKMARKS | wxHF_PRINT);
//    // set the config object
//    m_pHelp->UseConfig(wxConfig::Get(), _T("HelpController"));
//
//    //set directory for cache files. TODO: allow user to set up any other directory
//    m_pHelp->SetTempDir( g_pPaths->GetTempPath() );
//
//    //In release versions helpfile will be a single .htb file, precompiled in cache format,
//    //located in locale folder
//    //In test versions, documentation will be in native hhp format, located in /help folder.
//    //For testing purposes,
//    //it must be possible to switch to "release mode" (use of cached htb files).
//
//    wxString sPath;
//    wxString sExt;
//    if (g_fReleaseVersion || g_fReleaseBehaviour) {
//        //Release behaviour. Use precompiled cached .htb files and don't show title
//        sPath = g_pPaths->GetLocalePath();
//        m_pHelp->SetTitleFormat(_("LenMus help"));
//        sExt = _T("htb");
//    }
//    else {
//        //Test behaviour. Use native .hhp, .hhc, .hhk and .htm files
//        sPath = g_pPaths->GetHelpPath();
//        m_pHelp->SetTitleFormat(_("Test mode: using .hhp help file"));
//        sExt = _T("hhp");
//    }
//    // set base path for help documentation and load content
//    wxFileName oFilename(sPath, _T("help"), sExt, wxPATH_NATIVE);
//    bool fOK = m_pHelp->AddBook(oFilename);
//    if (! fOK)
//        wxMessageBox(wxString::Format(_("Failed adding book %s"),
//            oFilename.GetFullPath().c_str() ));
//
//}
//
//void MainFrame::InitializeBooks()
//{
//    // create the books window
//    m_pBookController = new lmTextBookController();
//
//    // set the config object
//    m_pBookController->UseConfig(wxConfig::Get(), _T("TextBooksController"));
//
//    //set directory for cache files.
//    m_pBookController->SetTempDir( g_pPaths->GetTempPath() );
//    m_pBookController->SetTitleFormat(_("Available books"));
//
//    // eMusicBooks are a single .lmb (LenMus Book) file
//    // All eMusicBooks are located in 'book' folder
//    wxString sPath = g_pPaths->GetBooksPath();
//    wxString sPattern = _T("*.lmb");
//    ScanForBooks(sPath, sPattern);
//
//}

void MainFrame::show_welcome_window()
{
    //show/hide welcome window

    if (!m_pWelcomeWnd)
    {
        //show welcome window
        m_pWelcomeWnd =  new WelcomeWindow(this, m_appScope, wxID_ANY);
	    m_pWelcomeWnd->SetFocus();
    }
    else
    {
        //hide welcome window
        delete m_pWelcomeWnd;
        m_pWelcomeWnd = NULL;
    }

}

//void MainFrame::OnOpenRecentFile(wxCommandEvent &event)
//{
//    wxString sFile = GetDocumentManager()->GetFromHistory(event.GetId() - wxID_FILE1);
//    OpenScore(sFile, false);    //false: it is not a new file
//}
//
//void MainFrame::OnCloseWelcomeWnd()
//{
//    //the welcome window has been closed. Clean up
//    m_pWelcomeWnd = (WelcomeWindow*)NULL;
//}
//
//void MainFrame::ScanForBooks(wxString sPath, wxString sPattern)
//{
//    //Scan the received folder for books and load all books found
//
//    //wxLogMessage(_T("[MainFrame::ScanForBooks] Scanning path <%s>"), sPath);
//    wxDir dir(sPath);
//    if ( !dir.IsOpened() ) {
//        // TODO: deal with the error here - wxDir would already log an error message
//        // explaining the exact reason of the failure
//        wxMessageBox(wxString::Format(_("Error when trying to move to folder %s"),
//            sPath.c_str() ));
//        return;
//    }
//
//    // Add first the 'intro' page
//    wxFileName oFileIntro(sPath, _T("intro"), _T("lmb"), wxPATH_NATIVE);
//    if (!m_pBookController->AddBook(oFileIntro)) {
//        //TODO better error handling
//        wxMessageBox(wxString::Format(_("Failed adding book %s"),
//            oFileIntro.GetFullPath().c_str() ));
//    }
//
//#if 0
//    // Then the 'release_notes' page
//    wxFileName oFileRN(sPath, _T("release_notes"), _T("lmb"), wxPATH_NATIVE);
//    if (!m_pBookController->AddBook(oFileRN)) {
//        //TODO better error handling
//        wxMessageBox(wxString::Format(_("Failed adding book %s"),
//            oFileRN.GetFullPath().c_str() ));
//    }
//#endif
//
//    // Now, the 'General Exercises' eBook
//    wxFileName oFileExercises(sPath, _T("GeneralExercises"), _T("lmb"), wxPATH_NATIVE);
//    if (!m_pBookController->AddBook(oFileExercises)) {
//        //TODO better error handling
//        wxMessageBox(wxString::Format(_("Failed adding book %s"),
//            oFileExercises.GetFullPath().c_str() ));
//    }
//
//    // Add now any other eBook found on this folder
//
//    //wxLogMessage(wxString::Format(
//    //    _T("Enumerating .hhp files in directory: %s"), sPath));
//    wxString sFilename;
//    bool fFound = dir.GetFirst(&sFilename, sPattern, wxDIR_FILES);
//    while (fFound) {
//        //wxLogMessage(_T("[MainFrame::ScanForBooks] Encontrado %s"), sFilename);
//        wxFileName oFilename(sPath, sFilename, wxPATH_NATIVE);
//        if (oFilename.GetName() != _T("help")
//            && oFilename.GetName() != _T("intro")
//            && oFilename.GetName() != _T("release_notes")
//            && oFilename.GetName() != _T("GeneralExercises")) {
//            if (!m_pBookController->AddBook(oFilename)) {
//                //TODO better error handling
//                wxMessageBox(wxString::Format(_("Failed adding book %s"),
//                    oFilename.GetFullPath().c_str() ));
//            }
//        }
//        fFound = dir.GetNext(&sFilename);
//    }
//
//}
//
//void MainFrame::SilentlyCheckForUpdates(bool fSilent)
//{
//    m_fSilentCheck = fSilent;
//}
//
//lmController* MainFrame::GetActiveController()
//{
//	//returns the controller associated to the active view
//
//    lmTDIChildFrame* pChild = GetActiveChild();
//	if (pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame)) )
//    {
//        lmScoreView* pView = ((lmEditFrame*)pChild)->GetView();
//        if (pView)
//            return pView->GetController();
//    }
//    return (lmController*)NULL;
//}
//
//// ----------------------------------------------------------------------------
//// menu callbacks
//// ----------------------------------------------------------------------------
//void MainFrame::OnBookFrame(wxCommandEvent& event)
//{
//    lmTextBookFrame* pBookFrame = m_pBookController->GetFrame();
//    pBookFrame->OnToolbar(event);
//    event.Skip(false);      //no further processing
//}
//
//void MainFrame::OnBookFrameUpdateUI(wxUpdateUIEvent& event)
//{
//    //enable only if current active view is TextBookFrame class
//    lmTDIChildFrame* pChild = GetActiveChild();
//    bool fEnabled = pChild && pChild->IsKindOf(CLASSINFO(lmTextBookFrame)) &&
//                    m_pBookController;
//
//    if (event.GetEventObject()->IsKindOf(CLASSINFO(wxToolBar)) )
//    {
//        //Update toolbar items
//        if (fEnabled)
//        {
//            // TextBookFrame is visible. Enable/disable buttons
//            lmTextBookFrame* pBookFrame = m_pBookController->GetFrame();
//            if (pBookFrame)
//                pBookFrame->UpdateUIEvent(event, m_pTbTextBooks);
//        }
//        else
//        {
//            // Bug in wxWidgets: if the button is a check button, disabled image is not
//            // set unless the button is unchecked. To bypass this bug, first uncheck button
//            // then disable it and finally, restore check state
//            bool fChecked = event.GetChecked();
//            event.Check(false);
//            event.Enable(false);
//            event.Check(fChecked);
//        }
//    }
//    else
//    {
//        //enable/disable menu items
//        if (fEnabled)
//        {
//            // TextBookFrame is visible. Enable/disable menu items according toolbar state
//            lmTextBookFrame* pBookFrame = m_pBookController->GetFrame();
//            if (pBookFrame)
//            {
//                m_booksMenu->Enable(event.GetId(),
//                                     m_pTbTextBooks->GetToolEnabled(event.GetId()));
//                if (event.GetId() == lmMENU_eBookPanel)
//                {
//                    m_booksMenu->Check(lmMENU_eBookPanel,
//                                        pBookFrame->IsNavPanelVisible());
//                }
//            }
//        }
//        else
//            event.Enable(false);
//    }
//}
//
//void MainFrame::OnVisitWebsite(wxCommandEvent& WXUNUSED(event))
//{
//    LaunchDefaultBrowser( _T("www.lenmus.org") );
//}
//
//void MainFrame::OnCheckForUpdates(wxCommandEvent& WXUNUSED(event))
//{
//    lmUpdater oUpdater;
//    oUpdater.CheckForUpdates(this, m_fSilentCheck);
//
//    //force a visible 'check for updates' process unless previously reset flag
//    SilentlyCheckForUpdates(false);
//}
//
//void MainFrame::OnHelpQuickGuide(wxCommandEvent& WXUNUSED(event))
//{
//    wxString sPath = g_pPaths->GetLocalePath();
//    wxFileName oFile(sPath, _T("editor_quick_guide.htm"), wxPATH_NATIVE);
//	if (!oFile.FileExists())
//	{
//		//use english version
//		sPath = g_pPaths->GetLocaleRootPath();
//		oFile.AssignDir(sPath);
//		oFile.AppendDir(_T("en"));
//		oFile.SetFullName(_T("editor_quick_guide.htm"));
//	}
//    ::wxLaunchDefaultBrowser( oFile.GetFullPath() );
//}
//
//void MainFrame::OnExportMusicXML(wxCommandEvent& WXUNUSED(event))
//{
//	//TODO
//}
//
//void MainFrame::OnExportBMP(wxCommandEvent& WXUNUSED(event))
//{
//    ExportAsImage(wxBITMAP_TYPE_BMP);
//}
//
//void MainFrame::OnExportJPG(wxCommandEvent& WXUNUSED(event))
//{
//    ExportAsImage(wxBITMAP_TYPE_JPEG);
//}
//
//void MainFrame::ExportAsImage(int nImgType)
//{
//    wxString sExt;
//    wxString sFilter = _T("*.");
//
//    if (nImgType == wxBITMAP_TYPE_BMP) {
//        sExt = _T("bmp");
//    }
//    else if (nImgType == wxBITMAP_TYPE_JPEG) {
//        sExt = _T("jpg");
//    }
//    else if (nImgType == wxBITMAP_TYPE_PNG) {
//        sExt = _T("png");
//    }
//    else if (nImgType == wxBITMAP_TYPE_PCX) {
//        sExt = _T("pcx");
//    }
//    else if (nImgType == wxBITMAP_TYPE_PNM) {
//        sExt = _T("pnm");
//    }
//    else
//    {
//        wxASSERT(false);
//    }
//
//    sFilter += sExt;
//
//    // ask for the name to give to the exported file
//    wxFileDialog dlg(this,
//                     _("Name for the exported file"),
//                     _T(""),    //default path
//                     _T(""),    //default filename
//                     sFilter,
//                     wxFD_SAVE | wxFD_OVERWRITE_PROMPT);        //flags
//
//    if (dlg.ShowModal() == wxID_CANCEL)
//        return;
//
//    wxString sFilename = dlg.GetFilename();
//    if ( !sFilename.IsEmpty() )
//    {
//        //remove extension including dot
//        wxString sName = sFilename.Left( sFilename.length() - sExt.length() - 1 );
//        lmScoreView* pView = GetActiveScoreView();
//        pView->SaveAsImage(sName, sExt, nImgType);
//    }
//
//}
//
//void MainFrame::OnHelpOpen(wxCommandEvent& event)
//{
//    if (m_fHelpOpened) {
//        //The help is open. Close it.
//        wxASSERT(m_pHelp);
//        m_pHelp->Quit();
//        delete m_pHelp;
//        m_pHelp = (lmHelpController*)NULL;
//        m_fHelpOpened = false;
//    }
//    else {
//        // open help
//
//        // in case the previous window was closed directly, the controller still
//        // exists. So delete the old controller
//        if (m_pHelp) {
//            delete m_pHelp;
//            m_pHelp = (lmHelpController*)NULL;
//        }
//
//        // create the new controller
//        InitializeHelp();
//        wxASSERT(m_pHelp);
//
//        // open it
//        m_pHelp->Display(_T("index.htm"));
//        m_fHelpOpened = true;
//    }
//
//}
//
//void MainFrame::SetOpenHelpButton(bool fPressed)
//{
//    m_fHelpOpened = fPressed;
//}
//
//void MainFrame::OnOpenBook(wxCommandEvent& event)
//{
//    OpenBook(_T("intro_thm0.htm"));
//  //  if (!m_pBookController)
//  //  {
//  //      // create book controller and load books
//  //      InitializeBooks();
//  //      wxASSERT(m_pBookController);
//
//  //      // display book "intro"
//  //      m_pBookController->Display(_T("intro_thm0.htm"));       //By page name
//  //      m_pBookController->GetFrame()->NotifyPageChanged();     // needed in Linux. I don't know why !
//		//OnActiveChildChanged(m_pBookController->GetFrame());
//  //  }
//  //  else
//  //  {
//  //      m_pBookController->GetFrame()->SetFocus();
//  //  }
//}
//
//void MainFrame::OpenBook(const wxString& sPageName)
//{
//    if (!m_pBookController)
//    {
//        // create book controller and load books
//        InitializeBooks();
//        wxASSERT(m_pBookController);
//
//        // display requested book
//        m_pBookController->Display(sPageName);       //By page name
//        m_pBookController->GetFrame()->NotifyPageChanged();     // needed in Linux. I don't know why !
//		OnActiveChildChanged(m_pBookController->GetFrame());
//    }
//    else
//    {
//        m_pBookController->GetFrame()->SetFocus();
//    }
//}
//
//void MainFrame::OnOpenBookUI(wxUpdateUIEvent &event)
//{
//    event.Enable(m_pBookController == (lmTextBookController*)NULL);
//}
//
//void MainFrame::OnCloseWindow(wxCloseEvent& event)
//{
//    // Invoked when the application is going to close the main window
//    // Override default method in lmDocTDIParentFrame, as it will only close
//    // the lmDocTDIChild windows (the scores) but no other windows (Welcome, eBooks)
//
//    m_fClosingAll = true;
//    if (CloseAll())     //force to close all windows
//        event.Skip();   //allow event to continue normal processing if all closed
//    m_fClosingAll = false;
//}
//
//
//void MainFrame::OnWindowClose(wxCommandEvent& WXUNUSED(event))
//{
//    // Invoked from menu: Window > Close
//
//    CloseActive();
//}
//
//void MainFrame::OnWindowCloseAll(wxCommandEvent& WXUNUSED(event))
//{
//    // Invoked from menu: Window > Close all
//
//    CloseAllWindows();
//}
//
//void MainFrame::CloseAllWindows()
//{
//    m_fClosingAll = true;
//    lmDocTDIParentFrame::CloseAll();
//    m_fClosingAll = false;
//}
//
//void MainFrame::OnWindowNext(wxCommandEvent& WXUNUSED(event))
//{
//    ActivateNext();
//}
//
//void MainFrame::OnWindowPrev(wxCommandEvent& WXUNUSED(event))
//{
//    ActivatePrevious();
//}
//
//lmScoreView* MainFrame::GetActiveScoreView()
//{
//    // get the view
//    lmTDIChildFrame* pChild = GetActiveChild();
//	wxASSERT(pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame)));
//    return ((lmEditFrame*)pChild)->GetView();
//}
//
//lmScore* MainFrame::GetActiveScore()
//{
//    // get the score
//    lmTDIChildFrame* pChild = GetActiveChild();
//	wxASSERT(pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame)));
//    lmDocument* pDoc = (lmDocument*)((lmEditFrame*)pChild)->GetDocument();
//    return pDoc->GetScore();
//}
//
//lmDocument* MainFrame::GetActiveDoc()
//{
//    lmTDIChildFrame* pChild = GetActiveChild();
//	if (pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame)))
//        return (lmDocument*)((lmEditFrame*)pChild)->GetDocument();
//    else
//        return (lmDocument*)NULL;
//}
//
////------------------------------------------------------------------------------------
//// Methods only for the debug version
////------------------------------------------------------------------------------------
//
//#ifdef _LM_DEBUG_
//
//void MainFrame::OnDebugForceReleaseBehaviour(wxCommandEvent& event)
//{
//    g_fReleaseBehaviour = event.IsChecked();
//}
//
//void MainFrame::OnDebugShowDebugLinks(wxCommandEvent& event)
//{
//    g_fShowDebugLinks = event.IsChecked();
//}
//
//void MainFrame::OnDebugShowBorderOnScores(wxCommandEvent& event)
//{
//    g_fBorderOnScores = event.IsChecked();
//}
//
//void MainFrame::OnDebugShowDirtyObjects(wxCommandEvent& event)
//{
//    g_fShowDirtyObjects = event.IsChecked();
//}
//
//void MainFrame::OnDebugRecSelec(wxCommandEvent& event)
//{
//    g_fDrawSelRect = event.IsChecked();
//    if (GetActiveDoc())
//    {
//	    GetActiveDoc()->Modify(true);
//        GetActiveDoc()->UpdateAllViews((wxView*)NULL, new lmUpdateHint() );
//    }
//}
//
//void MainFrame::OnDebugDrawAnchors(wxCommandEvent& event)
//{
//    g_fDrawAnchors = event.IsChecked();
//    if (GetActiveDoc())
//    {
//	    GetActiveDoc()->Modify(true);
//        GetActiveDoc()->UpdateAllViews((wxView*)NULL, new lmUpdateHint() );
//    }
//}

//void MainFrame::OnDebugPatternEditor(wxCommandEvent& WXUNUSED(event))
//{
//    lmDlgPatternEditor dlg(this);
//    dlg.ShowModal();
//
//}
//
//void MainFrame::OnDebugDumpBitmaps(wxCommandEvent& event)
//{
//    // get the view
//    lmTDIChildFrame* pChild = GetActiveChild();
//	wxASSERT(pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame)));
//    lmScoreView* pView = ((lmEditFrame*)pChild)->GetView();
//
//    pView->DumpBitmaps();
//}
//
//void MainFrame::OnDebugDumpStaffObjs(wxCommandEvent& event)
//{
//    lmScore* pScore = GetActiveScore();
//    wxASSERT(pScore);
//
//    lmDlgDebug dlg(this, _T("lmStaff objects dump"), pScore->Dump());
//    dlg.ShowModal();
//
//}
//
//void MainFrame::OnDebugDumpGMObjects(wxCommandEvent& event)
//{
//    // get the BoxScore
//    lmScoreView* pView = GetActiveScoreView();
//	lmBoxScore* pBox = pView->GetBoxScore();
//	if (!pBox) return;
//
//    lmDlgDebug dlg(this, _T("lmBoxScore dump"), pBox->Dump(0));
//    dlg.ShowModal();
//
//}
//
//void MainFrame::OnDebugScoreUI(wxUpdateUIEvent& event)
//{
//    lmTDIChildFrame* pChild = GetActiveChild();
//	bool fEnable = (pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame)));
//    event.Enable(fEnable);
//}
//
//void MainFrame::OnDebugCheckHarmony(wxCommandEvent& WXUNUSED(event))
//{
//    lmProcessorMngr* pMngr = lmProcessorMngr::GetInstance();
//    lmHarmonyProcessor* pProc =
//        (lmHarmonyProcessor*)pMngr->CreateScoreProcessor( CLASSINFO(lmHarmonyProcessor) );
//    pProc->DoProcess();
//}
//
//void MainFrame::OnDebugTestProcessor(wxCommandEvent& WXUNUSED(event))
//{
//    lmProcessorMngr* pMngr = lmProcessorMngr::GetInstance();
//    lmTestProcessor* pProc =
//        (lmTestProcessor*)pMngr->CreateScoreProcessor( CLASSINFO(lmTestProcessor) );
//    pProc->DoProcess();
//}
//
//void MainFrame::OnDebugSeeSource(wxCommandEvent& event)
//{
//    lmScore* pScore = GetActiveScore();
//    wxASSERT(pScore);
//
//    lmDlgDebug dlg(this, _T("Generated source code"), pScore->SourceLDP(false));    //false: do not include undo/redo data
//    dlg.ShowModal();
//}
//
//void MainFrame::OnDebugSeeSourceForUndo(wxCommandEvent& event)
//{
//    lmScore* pScore = GetActiveScore();
//    wxASSERT(pScore);
//
//    lmDlgDebug dlg(this, _T("Generated source code"), pScore->SourceLDP(true));     //true: include undo/redo data
//    dlg.ShowModal();
//}
//
//void MainFrame::OnDebugSeeXML(wxCommandEvent& event)
//{
//    lmScore* pScore = GetActiveScore();
//    wxASSERT(pScore);
//
//    lmDlgDebug dlg(this, _T("Generated MusicXML code"), pScore->SourceXML());
//    dlg.ShowModal();
//
//}
//
//void MainFrame::OnDebugUnitTests(wxCommandEvent& event)
//{
//    RunUnitTests();
//}
//
//void MainFrame::RunUnitTests()
//{
//    lmTestRunner oTR(this);
//    oTR.RunTests();
//}
//
//void MainFrame::OnDebugSeeMidiEvents(wxCommandEvent& WXUNUSED(event))
//{
//    lmScore* pScore = GetActiveScore();
//    wxASSERT(pScore);
//
//    lmDlgDebug dlg(this, _T("MIDI events table"), pScore->DumpMidiEvents() );
//    dlg.ShowModal();
//
//}
//
//void MainFrame::OnDebugSetTraceLevel(wxCommandEvent& WXUNUSED(event))
//{
//    lmDlgDebugTrace dlg(this);
//    dlg.ShowModal();
//}
//#endif
//
//// END OF DEBUG METHODS ------------------------------------------------------------
////----------------------------------------------------------------------------------
//void MainFrame::OnActiveChildChanged(lmTDIChildFrame* pFrame)
//{
//	// The active child frame has changed. Update things
//
//    //do nothing if closing all windows
//    if (m_fClosingAll) return;
//
//	// update zoom combo box
//	double rScale = pFrame->GetActiveViewScale();
//    UpdateZoomControls(rScale);
//    SetFocusOnActiveView();
//
//    //wxLogMessage(_T("[MainFrame::OnActiveChildChanged] Is kind of lmDocTDIChildFrame: %s"),
//    //    pFrame->IsKindOf(CLASSINFO(lmDocTDIChildFrame)) ? _T("yes") : _T("No") );
//}
//
//void MainFrame::UpdateZoomControls(double rScale)
//{
//    //invoked from the view at score creation to inform about the scale used.
//    //Also invoked internally to centralize code to update zoom controls
//
//    if (m_pComboZoom)
//        m_pComboZoom->SetValue(wxString::Format(_T("%.2f%%"), rScale * 100.0));
//}
//
//void MainFrame::OnZoom(wxCommandEvent& event, double rScale)
//{
//    lmTDIChildFrame* pChild = GetActiveChild();
//	if (pChild)
//	{
//		if (pChild->SetActiveViewScale(rScale) )
//			UpdateZoomControls(rScale);
//	}
//}
//
//void MainFrame::OnZoomUpdateUI(wxUpdateUIEvent &event)
//{
//	int nId = event.GetId();
//    lmTDIChildFrame* pChild = GetActiveChild();
//	if (pChild)
//	{
//		if ( pChild->IsKindOf(CLASSINFO(lmEditFrame)) )
//			event.Enable(true);
//		else
//			event.Enable(nId == k_menu_zoom_out ||
//						 nId == k_id_combo_zoom ||
//						 nId == k_menu_zoom_in );
//	}
//	else
//		event.Enable(false);
//}
//
//void MainFrame::OnZoomOther(wxCommandEvent& event)
//{
//    lmScoreView* pView = GetActiveScoreView();
//    double rScale = pView->GetScale() * 100;
//    int nZoom = (int) ::wxGetNumberFromUser(_T(""),
//        _("Zooming? (10 to 800)"), _T(""), (int)rScale, 10, 800);
//    if (nZoom != -1)    // -1 means invalid input or user canceled
//        OnZoom(event, (double)nZoom / 100.0);
//}
//
//void MainFrame::OnZoomFitWidth(wxCommandEvent& event)
//{
//    lmScoreView* pView = GetActiveScoreView();
//    pView->SetScaleFitWidth();
//    UpdateZoomControls(pView->GetScale());
//}
//
//void MainFrame::OnZoomFitFull(wxCommandEvent& event)
//{
//    lmScoreView* pView = GetActiveScoreView();
//    pView->SetScaleFitFull();
//    UpdateZoomControls(pView->GetScale());
//}
//
//void MainFrame::OnComboZoom(wxCommandEvent& event)
//{
//    wxString sValue = event.GetString();
//    if (sValue == _("Fit page full")) {
//        OnZoomFitFull(event);
//    }
//    else if (sValue == _("Fit page width")) {
//        OnZoomFitWidth(event);
//    }
//    else if (sValue == _("Actual size")) {
//        OnZoom(event, 1.0);
//    }
//    else {
//        //sValue.Replace(_T(","), _T("."));
//        sValue.Replace(_T("%"), _T(""));
//        sValue.Trim();
//        double rZoom;
//        if (!sValue.ToDouble(&rZoom)) {
//            wxMessageBox(wxString::Format(_("Invalid zooming factor '%s'"), sValue.c_str()),
//                         _("Error message"), wxOK || wxICON_HAND );
//            return;
//        }
//        if (rZoom < 9.9 || rZoom > 801.0) {
//            wxMessageBox(_("Zooming factor must be greater that 10% and lower than 800%"),
//                         _("Error message"), wxOK || wxICON_HAND );
//            return;
//        }
//        OnZoom(event, rZoom/100.0);
//    }
//	event.Skip();      //continue processing the  event
//
//}
//
//void MainFrame::OnComboVoice(wxCommandEvent& event)
//{
//    //int nVoice = event.GetSelection();
//    //wxMessageBox(wxString::Format(_T("Voice %d selected"), nVoice));
//    SetFocusOnActiveView();
//}
//
//// View menu event handlers
//
//bool MainFrame::IsToolBoxVisible()
//{
//	return (m_pToolBox && m_layoutManager.GetPane(_T("ToolBox")).IsShown());
//}
//
//
//void MainFrame::OnViewTools(wxCommandEvent& event)
//{
//    ShowToolBox(event.IsChecked());
//}
//
//void MainFrame::ShowToolBox(bool fShow)
//{
//    //create the ToolBox
//    if (!m_pToolBox)
//    {
//        m_pToolBox =  new lmToolBox(this, wxID_ANY);
//        m_pToolBox->Hide();
//    }
//
//    if (fShow)
//    {
//        //if not added to AUI manager do it now
//        wxAuiPaneInfo panel = m_layoutManager.GetPane(_T("ToolBox"));
//        if (!panel.IsOk())
//            m_layoutManager.AddPane(m_pToolBox, wxAuiPaneInfo(). Name(_T("ToolBox")).
//                                Caption(_("Edit toolbox")).Left().
//							    Floatable(true).
//							    Resizable(false).
//							    TopDockable(true).
//							    BottomDockable(false).
//							    MaxSize(wxSize(m_pToolBox->GetWidth(), -1)).
//							    MinSize(wxSize(m_pToolBox->GetWidth(), -1)) );
//
//        //show ToolBox
//        m_layoutManager.GetPane(_T("ToolBox")).Show(true);
//        m_layoutManager.Update();
//        m_pToolBox->SetFocus();
//    }
//    else
//    {
//        //if added to AUI manager hide ToolBox
//        wxAuiPaneInfo panel = m_layoutManager.GetPane(_T("ToolBox"));
//        if (panel.IsOk())
//        {
//            m_layoutManager.GetPane(_T("ToolBox")).Show(false);
//            m_layoutManager.Update();
//        }
//    }
//}
//
//void MainFrame::OnViewRulers(wxCommandEvent& event)
//{
//    lmScoreView* pView = GetActiveScoreView();
//    pView->SetRulersVisible(event.IsChecked());
//}
//
//void MainFrame::OnViewRulersUI(wxUpdateUIEvent &event)
//{
//    //For now, always disabled in release versions
//    if (g_fReleaseVersion || g_fReleaseBehaviour) {
//        event.Enable(false);
//    }
//    else {
//        lmTDIChildFrame* pChild = GetActiveChild();
//        event.Enable( pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame)) );
//    }
//}
//
//bool MainFrame::ShowRulers()
//{
//    return GetMenuBar()->IsChecked(k_menu_view_rulers);
//}
//
//void MainFrame::OnViewToolBar(wxCommandEvent& WXUNUSED(event))
//{
//    bool fToolBar;
//    if (!m_pToolbar) {
//        create_tool_bar ();
//        fToolBar = true;
//    } else{
//        DeleteToolbar ();
//        fToolBar = false;
//    }
//    g_pPrefs->Write(_T("/MainFrame/ViewToolBar"), fToolBar);
//
//}
//
//void MainFrame::OnToolbarsUI (wxUpdateUIEvent &event)
//{
//    event.Check (m_pToolbar != NULL);
//}
//
//
//void MainFrame::OnViewStatusBar(wxCommandEvent& WXUNUSED(event))
//{
//    bool fStatusBar;
//    if (!m_pStatusBar) {
//        CreateTheStatusBar ();
//        fStatusBar = true;
//    }else{
//        DeleteTheStatusBar ();
//        fStatusBar = false;
//    }
//    g_pPrefs->Write(_T("/MainFrame/ViewStatusBar"), fStatusBar);
//
//}
//
//void MainFrame::OnStatusbarUI (wxUpdateUIEvent &event) {
//    event.Check (m_pStatusBar != NULL);
//}
//
//void MainFrame::NewScoreWindow(lmEditorMode* pMode, lmScore* pScore)
//{
//    //Open a new score editor window in mode pMode
//
//    wxASSERT(pScore);
//    ShowToolBox(true);      //force to display ToolBox
//    m_pDocManager->OpenDocument(pMode, pScore);
//}
//
//void MainFrame::OpenScore(wxString& sFilename, bool fAsNew)
//{
//    if (!sFilename.empty())
//    {
//        if (fAsNew)
//        {
//            lmLDPParser parser;
//            NewScoreWindow((lmEditorMode*)NULL, parser.ParseFile(sFilename));
//        }
//        else
//        {
//            ShowToolBox(true);      //force to display ToolBox
//            m_pDocManager->OpenFile(sFilename, fAsNew);
//        }
//    }
//}
//
////-----------------------------------------------------------------------------------------------
//// Print/preview
////-----------------------------------------------------------------------------------------------
//
//void MainFrame::OnPrintPreview(wxCommandEvent& WXUNUSED(event))
//{
//    lmTDIChildFrame* pChild = GetActiveChild();
//    bool fEditFrame = pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame));
//    bool fTextBookFrame = pChild && pChild->IsKindOf(CLASSINFO(lmTextBookFrame));
//
//    if (fEditFrame) {
//        // Get the active view
//        lmScoreView* pView = GetActiveScoreView();
//
//        // Pass two printout objects: for preview, and possible printing.
//        wxPrintDialogData printDialogData(*g_pPrintData);
//        wxPrintPreview *preview = new wxPrintPreview(new lmPrintout(pView), new lmPrintout(pView), &printDialogData);
//        if (!preview->Ok()) {
//            delete preview;
//            wxMessageBox(_("There is a problem previewing.\nPerhaps your current printer is not set correctly?"), _("Previewing"), wxOK);
//            return;
//        }
//
//        wxPreviewFrame *frame = new wxPreviewFrame(preview, this, _("Preview"), wxPoint(100, 100), wxSize(600, 650));
//        frame->Centre(wxBOTH);
//        frame->Initialize();
//        frame->Show(true);
//    }
//    else if (fTextBookFrame) {
//    }
//}
//
////void MainFrame::OnPageSetup(wxCommandEvent& WXUNUSED(event))
////{
////    (*g_pPaperSetupData) = *g_pPrintData;
////
////    wxPageSetupDialog pageSetupDialog(this, g_pPaperSetupData);
////    pageSetupDialog.ShowModal();
////
////    (*g_pPrintData) = pageSetupDialog.GetPageSetupData().GetPrintData();
////    (*g_pPaperSetupData) = pageSetupDialog.GetPageSetupData();
////
////}
//
//void MainFrame::OnPrintSetup(wxCommandEvent& WXUNUSED(event))
//{
//    //wxPrintDialogData printDialogData(* g_pPrintData);
//    //wxPrintDialog printerDialog(this, &printDialogData);
//    //
//    //printerDialog.GetPrintDialogData().SetSetupDialog(TRUE);
//    //printerDialog.ShowModal();
//
//    //(*g_pPrintData) = printerDialog.GetPrintDialogData().GetPrintData();
//
//}
//
//void MainFrame::OnPrint(wxCommandEvent& event)
//{
//    lmTDIChildFrame* pChild = GetActiveChild();
//    bool fEditFrame = pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame));
//    bool fTextBookFrame = pChild && pChild->IsKindOf(CLASSINFO(lmTextBookFrame));
//
//    if (fEditFrame) {
//        wxPrintDialogData printDialogData(* g_pPrintData);
//        wxPrinter printer(& printDialogData);
//
//        // Get the active view and create the printout object
//        lmScoreView* pView = GetActiveScoreView();
//        lmPrintout printout(pView);
//
//        if (!printer.Print(this, &printout, true)) {
//            if (wxPrinter::GetLastError() == wxPRINTER_ERROR)
//                wxMessageBox(_("There is a problem for printing.\nPerhaps your current printer is not set correctly?"), _("Printing"), wxOK);
//            else
//                wxMessageBox(_("Printing cancelled"), _("Printing"), wxOK);
//
//        } else {
//            (*g_pPrintData) = printer.GetPrintDialogData().GetPrintData();
//        }
//    }
//    else if (fTextBookFrame) {
//        event.SetId(lmMENU_eBook_Print);
//        lmTextBookFrame* pBookFrame = m_pBookController->GetFrame();
//        pBookFrame->OnToolbar(event);
//        event.Skip(false);      //no further processing
//    }
//
//}
//
//void MainFrame::OnEditCut(wxCommandEvent& event)
//{
//    //When invoked, current active child frame must be an lmEditFrame
//
//    lmTDIChildFrame* pChild = GetActiveChild();
//	if (pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame)))
//    {
//        ((lmEditFrame*)pChild)->GetView()->GetController()->DeleteSelection();
//    }
//}
//
//void MainFrame::OnEditCopy(wxCommandEvent& event)
//{
//    //When invoked, current active child frame must be an lmEditFrame
//
//    lmTDIChildFrame* pChild = GetActiveChild();
//	if (pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame)))
//    {
//        lmTODO(_T("[MainFrame::OnEditCopy] All code in this method"));
//    }
//}
//
//void MainFrame::OnEditPaste(wxCommandEvent& event)
//{
//    //When invoked, current active child frame must be an lmEditFrame
//
//    lmTDIChildFrame* pChild = GetActiveChild();
//	if (pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame)))
//    {
//        lmTODO(_T("[MainFrame::OnEditPaste] All code in this method"));
//    }
//}
//
//void MainFrame::OnEditUpdateUI(wxUpdateUIEvent &event)
//{
//    lmTDIChildFrame* pChild = GetActiveChild();
//	bool fEnable = (pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame)));
//    if (!fEnable)
//    {
//        event.Enable(false);
//    }
//    else
//    {
//        switch (event.GetId())
//        {
//            case k_menu_edit_copy:
//            case k_menu_edit_cut:
//                // Copy & cut: enable only if something selected
//                event.Enable( ((lmEditFrame*)pChild)->GetView()->SomethingSelected() );
//                break;
//            case k_menu_edit_paste:
//                //Enable only if something saved in clipboard
//                event.Enable(false);    //TODO
//                break;
//
//			case k_menu_view_page_margins:
//				event.Enable(true);
//				event.Check(g_fShowMargins);	//synchronize check status
//				break;
//
//
//            // Other commnads: always enabled
//            default:
//                event.Enable(true);
//
//
//        }
//    }
//}
//
//void MainFrame::OnFileImport(wxCommandEvent& WXUNUSED(event))
//{
//    // ask for the file to import
//    wxString sFilter = wxT("*.*");
//    wxString sFilename = ::wxFileSelector(_("Choose the file to import"),
//                                        wxT(""),        //default path
//                                        wxT(""),        //default filename
//                                        wxT("xml"),     //default_extension
//                                        sFilter,
//                                        wxFD_OPEN,      //flags
//                                        this);
//    if ( !sFilename.IsEmpty() )
//    {
//        ShowToolBox(true);      //force to display ToolBox
//        m_pDocManager->ImportFile(sFilename);
//    }
//}
//
//void MainFrame::OnFileClose(wxCommandEvent& event)
//{
//    m_pDocManager->OnFileClose(event);
//}
//
//void MainFrame::OnFileSave(wxCommandEvent& event)
//{
//    m_pDocManager->OnFileSave(event);
//}
//
//void MainFrame::OnFileSaveAs(wxCommandEvent& event)
//{
//    m_pDocManager->OnFileSaveAs(event);
//}
//
//void MainFrame::OnFileUpdateUI(wxUpdateUIEvent &event)
//{
//    lmTDIChildFrame* pChild = GetActiveChild();
//    bool fEditFrame = pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame));
//    bool fTextBookFrame = pChild && pChild->IsKindOf(CLASSINFO(lmTextBookFrame));
//    bool fEnableImport = !(g_fReleaseVersion || g_fReleaseBehaviour);
//
//    switch (event.GetId())
//    {
//        // Print related commands: enabled if EditFrame or TextBookFrame
//        case k_menu_print_Preview:
//            event.Enable(fEditFrame);
//            //TODO Add print preview capabilities to TextBookFrame
//            break;
//        case wxID_PRINT_SETUP:
//            //TODO: disabled in 3.3. Incompatibilities with wx2.7.1
//            event.Enable(false);    //fEditFrame || fTextBookFrame);
//            break;
//        case k_menu_print:
//            event.Enable(fEditFrame || fTextBookFrame);
//            break;
//
//        // Save related commands: enabled if EditFrame
//        case wxID_SAVE:
//            event.Enable(fEditFrame);
//            break;
//        case wxID_SAVEAS:
//            event.Enable(fEditFrame);
//            break;
//        case k_menu_file_export:
//            event.Enable(fEditFrame);
//            break;
//		case k_menu_file_export_MusicXML:
//			event.Enable(fEditFrame);
//			break;
//        case k_menu_file_export_bmp:
//            event.Enable(fEditFrame);
//            break;
//        case k_menu_file_export_jpg:
//            event.Enable(fEditFrame);
//            break;
//        case k_menu_file_Import:
//            event.Enable(fEnableImport);
//            break;
//
//        // Other commnads: always enabled
//        default:
//            event.Enable(true);
//    }
//
//    if (g_fReleaseVersion || g_fReleaseBehaviour) {
//        switch (event.GetId())
//        {
//            case k_menu_file_export_MusicXML:
//                event.Enable(false);
//                break;
//        }
//    }
//
//}
//
//void MainFrame::OnSoundUpdateUI(wxUpdateUIEvent &event)
//{
//    lmTDIChildFrame* pChild = GetActiveChild();
//    switch (event.GetId())
//    {
//		case MENU_Metronome:
//			event.Enable(true);
//			event.Check(m_pMtr->IsRunning());
//			break;
//
//        // Other items: only enabled if a score is displayed
//        default:
//            event.Enable( pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame)) );
//    }
//}
//
//void MainFrame::RedirectKeyPressEvent(wxKeyEvent& event)
//{
//	//Redirects a Key Press event to the active child
//    lmTDIChildFrame* pChild = GetActiveChild();
//    if(pChild)
//		pChild->ProcessEvent(event);
//	else
//		event.Skip();
//}
//
//void MainFrame::SetFocusOnActiveView()
//{
//	//Move the focus to the active child
//
//    lmTDIChildFrame* pChild = GetActiveChild();
//    if(pChild)
//		pChild->SetFocus();
//}

//---------------------------------------------------------------------------------------
void MainFrame::on_run_midi_wizard(wxCommandEvent& WXUNUSED(event))
{
    run_midi_wizard();
}

//---------------------------------------------------------------------------------------
void MainFrame::run_midi_wizard()
{
    MidiWizard oWizard(m_appScope, this);
    oWizard.Run();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_all_sounds_off(wxCommandEvent& WXUNUSED(event))
{
    MidiServer* pMidi = m_appScope.get_midi_server();
    if (!pMidi) return;
    wxMidiOutDevice* pMidiOut = pMidi->get_out_device();
    if (!pMidiOut) return;
    pMidiOut->AllSoundsOff();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_sound_test(wxCommandEvent& WXUNUSED(event))
{
    MidiServer* pMidi = m_appScope.get_midi_server();
    if (!pMidi) return;
    pMidi->TestOut();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_play_start(wxCommandEvent& WXUNUSED(event))
{
    ScoreCanvas* pCanvas = dynamic_cast<ScoreCanvas*>(get_active_canvas());
    if (pCanvas)
        pCanvas->start_play();
}
//
//void MainFrame::OnPlayCursorStart(wxCommandEvent& WXUNUSED(event))
//{
//    lmScoreView* pView = GetActiveScoreView();
//    bool fCountOff = GetMenuBar()->IsChecked(k_menu_play_countoff);
//    pView->GetController()->PlayScore(true, fCountOff);	//true: from cursor
//}
//
//void MainFrame::OnPlayStop(wxCommandEvent& WXUNUSED(event))
//{
//    lmScoreView* pView = GetActiveScoreView();
//    pView->GetController()->StopPlaying();
//}
//
//void MainFrame::OnPlayPause(wxCommandEvent& WXUNUSED(event))
//{
//    lmScoreView* pView = GetActiveScoreView();
//    pView->GetController()->PausePlaying();
//}
//


//void MainFrame::OnScoreWizard(wxCommandEvent& WXUNUSED(event))
//{
//    lmScore* pScore = (lmScore*)NULL;
//    lmScoreWizard oWizard(this, &pScore);
//    oWizard.Run();
//
//    if (pScore)
//    {
//        //Wizard finished successfully. A score has been defined.
//        //Create a new score editor window and display it
//        NewScoreWindow((lmEditorMode*)NULL, pScore);
//    }
//}
//
//void MainFrame::OnOptions(wxCommandEvent& WXUNUSED(event))
//{
//    lmOptionsDlg dlg(this, -1);
//    dlg.CentreOnParent();
//    dlg.ShowModal();
//}

//bool MainFrame::IsCountOffChecked()
//{
//    return GetMenuBar()->IsChecked(k_menu_play_countoff);
//}
//void MainFrame::OnScoreTitles(wxCommandEvent& WXUNUSED(event))
//{
//    lmScoreView* pView = GetActiveScoreView();
//    pView->GetController()->AddTitle();
//}
//
//void MainFrame::OnInstrumentProperties(wxCommandEvent& WXUNUSED(event))
//{
//    lmController* pController = GetActiveScoreView()->GetController();
//    GetActiveScore()->OnInstrProperties(-1, pController);    //-1 = select instrument
//}
//
//void MainFrame::OnMetronomeTimer(wxTimerEvent& event)
//{
//    //A metronome click has been produced, and this event is generated so that we
//    //can flash the metronome LED or do any other desired visual efect.
//    //Do not generate sounds as they are done by the lmMetronome object
//
//    //TODO flash metronome LED
//  //  Me.picMtrLEDOff.Visible = false;
//  //  Me.picMtrLEDRojoOn.Visible = true;
////    ::wxMilliSleep(100);
//  //  Me.picMtrLEDOff.Visible = true;
//  //  Me.picMtrLEDRojoOn.Visible = false;
//
//}
//
//void MainFrame::OnMetronomeUpdate(wxSpinEvent& WXUNUSED(event))
//{
//    int nMM = m_pSpinMetronome->GetValue();
//    if (m_pMtr) m_pMtr->SetMM(nMM);
//}
//
//void MainFrame::OnMetronomeUpdateText(wxCommandEvent& WXUNUSED(event))
//{
//    int nMM = m_pSpinMetronome->GetValue();
//    if (m_pMtr) m_pMtr->SetMM(nMM);
//}
//
//void MainFrame::DumpScore(lmScore* pScore)
//{
//    if (!pScore) return;
//    lmDlgDebug dlg(this, _T("lmStaff objects dump"), pScore->Dump());
//    dlg.ShowModal();
//
//}
//
//void MainFrame::OnViewPageMargins(wxCommandEvent& event)
//{
//    g_fShowMargins = event.IsChecked();
//    if (GetActiveDoc())
//    {
//	    GetActiveDoc()->Modify(true);
//        GetActiveDoc()->UpdateAllViews((wxView*)NULL, new lmUpdateHint() );
//    }
//}
//
//
//void MainFrame::OnViewWelcomePage(wxCommandEvent& event)
//{
//    WXUNUSED(event)
//    show_welcome_window();
//}
//
//void MainFrame::OnViewWelcomePageUI(wxUpdateUIEvent &event)
//{
//	bool fEnable = (m_pWelcomeWnd == (WelcomeWindow*)NULL);
//    event.Enable(fEnable);
//}
//
//void MainFrame::OnPaneClose(wxAuiManagerEvent& event)
//{
//    event.Skip();      //continue processing the  event
//}
//
////-----------------------------------------------------------------------------------
//// status bar
////-----------------------------------------------------------------------------------
//
//void MainFrame::SetStatusBarMsg(const wxString& sText)
//{
//    if (m_pStatusBar)
//        m_pStatusBar->SetMsgText(sText);
//}
//
//void MainFrame::SetStatusBarMouseData(int nPage, float rTime, int nMeasure,
//                                        lmUPoint uPos)
//{
//    if (m_pStatusBar)
//        m_pStatusBar->SetMouseData(nPage, rTime, nMeasure, uPos);
//}
//
//void MainFrame::SetStatusBarCaretData(int nPage, float rTime, int nMeasure)
//{
//    if (m_pStatusBar)
//        m_pStatusBar->SetCaretData(nPage, rTime, nMeasure);
//}
//
//void MainFrame::OnKeyPress(wxKeyEvent& event)
//{
//	//if (event.GetEventType()==wxEVT_KEY_DOWN)
//        //if (event.GetKeyCode()==WXK_F1 && IsToolBoxVisible())
//	{
//		RedirectKeyPressEvent(event);
//	}
//}
//
//void MainFrame::OnKeyF1(wxCommandEvent& event)
//{
////		int i = 1;
//}
//
//#if lmUSE_LIBRARY_MVC
//
//void MainFrame::OnCloseDocument(Document* pDoc)
//{
//    //call back to access the MvcCollection
//
//    lmDocManager* pDocManager = this->GetDocumentManager();
//    pDocManager->close_document(pDoc);
//}
//
//MvcCollection* MainFrame::GetMvcCollection()
//{
//    lmDocManager* pDocManager = this->GetDocumentManager();
//    return pDocManager->get_mvc_collection();
//}
//
//#endif
//
//
///*
////------------------------------------------------------------------------------------
//// Tips at application start
////------------------------------------------------------------------------------------
//
//void MainFrame::ShowTips(bool fForceShow)
//{
//    bool fShowTips = false;
//    g_pPrefs->Read(_T("/MainFrame/ShowTips"), &fShowTips);
//    if (fForceShow || fShowTips)
//    {
//        //read data from last run
//        wxLogNull null; // disable error message if tips file does not exist
//        //wxString sTipsFile = g_pPrefs->Read(_T("/data_path")) + _T("/tips.txt");
//        //sTipsFile = g_pPaths->GetHelpPath();
//        //m_pHelp->SetTitleFormat(_("Test mode: using .hhp help file"));
//        long nTipsIndex = g_pPrefs->Read(_T("/MainFrame/NextTip"), 0L);
//
//        //show next tip
//        wxTipProvider oTipDlg = wxCreateFileTipProvider(sTipsFile, nTipsIndex);
//        fShowTips = wxShowTip(this, &oTipDlg, fShowTips);
//
//        //save data for next run
//        g_pPrefs->Write(_T("/MainFrame/ShowTips"), fShowTips);
//        g_pPrefs->Write(_T("/MainFrame/NextTip"), (long)oTipDlg.GetCurrentTip());
//    }
//}
//
//*/
//



}   //namespace lenmus
