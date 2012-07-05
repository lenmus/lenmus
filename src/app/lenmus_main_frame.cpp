//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2012 LenMus project
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
#include "lenmus_standard_header.h"

#include "lenmus_string.h"
#include "lenmus_app.h"
#include "lenmus_document_canvas.h"
#include "lenmus_document_frame.h"
#include "lenmus_canvas.h"
#include "lenmus_art_provider.h"
#include "lenmus_midi_wizard.h"
#include "lenmus_midi_server.h"
#include "lenmus_welcome_window.h"
#include "lenmus_dlg_debug.h"
#include "lenmus_about_dialog.h"
#include "lenmus_dyncontrol.h"
#include "lenmus_paths.h"
#include "lenmus_options_dlg.h"
#include "lenmus_generators.h"
#include "lenmus_status_bar.h"
#include "lenmus_updater.h"

//lomse headers
#include <lomse_score_player.h>
#include <lomse_midi_table.h>
#include <lomse_internal_model.h>
#include <lomse_doorway.h>
#include <lomse_interactor.h>
#include <lomse_staffobjs_table.h>
using namespace lomse;

//wxWidgets
#include <wx/numdlg.h>

#if !wxUSE_PRINTING_ARCHITECTURE
#error "You must set wxUSE_PRINTING_ARCHITECTURE to 1 in setup.h, and recompile the library."
#endif

#include <ctype.h>
#include <wx/metafile.h>
#include <wx/print.h>
#include <wx/printdlg.h>
#include <wx/image.h>
#include <wx/accel.h>
#include <wx/filename.h>

#ifdef __WXMAC__
#include <wx/mac/printdlg.h>
#endif

#if wxUSE_LIBGNOMEPRINT
#include <wx/html/forcelnk.h>
FORCE_LINK(gnome_print)
#endif


namespace lenmus
{

DEFINE_EVENT_TYPE(LM_EVT_CHECK_FOR_UPDATES)

//---------------------------------------------------------------------------------------
// helper. Encapsulates the functionality of printing out an application document
class MyPrintout: public wxPrintout
{
protected:
    wxPageSetupDialogData* m_pPageSetupData;
    DocumentWindow* m_pCanvas;
    int m_nMinPage;
    int m_nMaxPage;
    int m_pageWidthPixels;
    int m_pageHeightPixels;

public:
    MyPrintout(wxPageSetupDialogData* pPageSetupData, DocumentWindow* pCanvas)
        : wxPrintout(pCanvas->get_filename())
        , m_pPageSetupData(pPageSetupData)
        , m_pCanvas(pCanvas)
    {
    }

    bool OnPrintPage(int page);
    bool HasPage(int page);
    bool OnBeginDocument(int startPage, int endPage);
    void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);

protected:
    void DrawPage(int page);
};

//=======================================================================================
// MainFrame implementation
//=======================================================================================

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

    //wxID_OPEN is used by Welcome Window
    k_menu_file_open = wxID_OPEN,


    // Menu File
    k_menu_file_new = 10000,  //lmMENU_Last_Public_ID,
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
    k_menu_print,
    //k_menu_print_setup,    wxID_PRINT_SETUP is used instead


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
    k_menu_view_counters,

	// Menu Score
	k_menu_score_titles,

	// Menu Instrument
    k_menu_instr_properties,

    // Menu Debug
    k_menu_debug_do_tests,
    k_menu_debug_draw_box,
    k_menu_debug_draw_box_docpage,
    k_menu_debug_draw_box_content,
    k_menu_debug_draw_box_system,
    k_menu_debug_draw_box_slice,
    k_menu_debug_draw_box_slice_instr,
    k_menu_debug_draw_box_inline,
    k_menu_debug_remove_boxes,
    k_menu_debug_justify_systems,
    k_menu_debug_dump_column_tables,
    k_menu_debug_ForceReleaseBehaviour,
    k_menu_debug_ShowDebugLinks,
    k_menu_debug_ShowBorderOnScores,
    k_menu_debug_recSelec,
    k_menu_debug_CheckHarmony,
    k_menu_debug_draw_anchors,
    k_menu_debug_DumpBitmaps,
	k_menu_debug_dump_gmodel,
    k_menu_debug_SeeSource,
    k_menu_debug_SeeSourceUndo,
    k_menu_debug_SeeXML,
    k_menu_debug_see_midi_events,
    k_menu_debug_see_paths,
    k_menu_debug_see_staffobjs,
    k_menu_debug_SetTraceLevel,
    k_menu_debug_PatternEditor,
    k_menu_debug_ShowDirtyObjects,
    k_menu_debug_TestProcessor,
    k_menu_debug_print_preview,

    // Menu Zoom
    k_menu_zoom_in,
    k_menu_zoom_out,
    k_menu_zoom_100,
    k_menu_zoom_fit_full,
    k_menu_zoom_fit_width,
    k_menu_zoom_other,

    // Menu Sound
    k_menu_sound_midi_wizard,
    k_menu_sound_test,
    k_menu_sound_off,

    // Menu play
    k_menu_play_start,
	k_menu_play_cursor_start,
    k_menu_play_stop,
    k_menu_play_pause,
    k_menu_play_countoff,

    // Menu Options
    k_menu_preferences,

    // Menu Window
    k_menu_windowCloseAll,

    // Menu Help
    k_menu_help_open,
    k_menu_help_quick_guide,
    k_menu_help_visit_website,
    k_menu_check_for_updates,

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

//---------------------------------------------------------------------------------------
// events table
BEGIN_EVENT_TABLE(MainFrame, wxFrame)

    //File menu/toolbar
    EVT_MENU(k_menu_file_quit, MainFrame::on_file_quit)
    EVT_MENU(k_menu_file_open, MainFrame::on_file_open)
    EVT_MENU(k_menu_file_reload, MainFrame::on_file_reload)
//    EVT_UPDATE_UI (k_menu_file_open, MainFrame::on_update_UI_file)
//    EVT_MENU      (k_menu_file_Close, MainFrame::OnFileClose)
//    EVT_UPDATE_UI (k_menu_file_Close, MainFrame::on_update_UI_file)
//    EVT_MENU      (k_menu_file_Save, MainFrame::OnFileSave)
//    EVT_UPDATE_UI (k_menu_file_Save, MainFrame::on_update_UI_file)
//    EVT_MENU      (k_menu_file_SaveAs, MainFrame::OnFileSaveAs)
    EVT_UPDATE_UI (k_menu_file_SaveAs, MainFrame::disable_tool) //on_update_UI_file)
//
//    EVT_MENU      (k_menu_file_new, MainFrame::OnScoreWizard)
//    EVT_MENU      (k_menu_file_Import, MainFrame::OnFileImport)
    EVT_UPDATE_UI (k_menu_file_Import, MainFrame::disable_tool) //on_update_UI_file)
    EVT_UPDATE_UI (k_menu_file_export, MainFrame::disable_tool) //on_update_UI_file)
//    EVT_MENU      (k_menu_file_export_MusicXML, MainFrame::OnExportMusicXML)
    EVT_UPDATE_UI (k_menu_file_export_MusicXML, MainFrame::disable_tool)    //on_update_UI_file)
//    EVT_MENU      (k_menu_file_export_bmp, MainFrame::OnExportBMP)
    EVT_UPDATE_UI (k_menu_file_export_bmp, MainFrame::disable_tool) //on_update_UI_file)
//    EVT_MENU      (k_menu_file_export_jpg, MainFrame::OnExportJPG)
    EVT_UPDATE_UI (k_menu_file_export_jpg, MainFrame::disable_tool) //on_update_UI_file)

    EVT_MENU      (wxID_PRINT_SETUP, MainFrame::on_print_setup)
    EVT_UPDATE_UI (wxID_PRINT_SETUP, MainFrame::on_update_UI_file)
    EVT_MENU      (k_menu_print, MainFrame::on_print)
    EVT_UPDATE_UI (k_menu_print, MainFrame::on_update_UI_file)
    EVT_UPDATE_UI (wxID_SAVE, MainFrame::disable_tool)   //on_update_UI_file)
    EVT_UPDATE_UI (wxID_SAVEAS, MainFrame::disable_tool)   //on_update_UI_file)
    EVT_UPDATE_UI (k_menu_file_new, MainFrame::disable_tool)   //on_update_UI_file)
    EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, MainFrame::on_open_recent_file)

    //Edit menu/toolbar
//    EVT_MENU      (k_menu_edit_copy, MainFrame::OnEditCopy)
    EVT_UPDATE_UI (k_menu_edit_copy, MainFrame::disable_tool)   //OnEditUpdateUI)
//    EVT_MENU      (k_menu_edit_cut, MainFrame::OnEditCut)
    EVT_UPDATE_UI (k_menu_edit_cut, MainFrame::disable_tool)   //OnEditUpdateUI)
//    EVT_MENU      (k_menu_edit_paste, MainFrame::OnEditPaste)
    EVT_UPDATE_UI (k_menu_edit_paste, MainFrame::disable_tool)   //OnEditUpdateUI)
    EVT_UPDATE_UI (wxID_UNDO, MainFrame::disable_tool)   //OnEditUpdateUI)
    EVT_UPDATE_UI (wxID_REDO, MainFrame::disable_tool)   //OnEditUpdateUI)
//
//    //View menu/toolbar
//    EVT_MENU      (k_menu_view_tools, MainFrame::OnViewTools)
    //EVT_UPDATE_UI (k_menu_view_tools, MainFrame::OnEditUpdateUI)
//    EVT_MENU      (k_menu_view_rulers, MainFrame::OnViewRulers)
    EVT_UPDATE_UI (k_menu_view_rulers, MainFrame::disable_tool)   //OnViewRulersUI)
    EVT_MENU      (k_menu_view_toolBar, MainFrame::on_view_tool_bar)
    EVT_UPDATE_UI (k_menu_view_toolBar, MainFrame::on_update_UI_tool_bar)
    EVT_MENU      (k_menu_view_statusBar, MainFrame::on_view_status_bar)
    EVT_UPDATE_UI (k_menu_view_statusBar, MainFrame::on_update_UI_status_bar)
//    EVT_MENU      (k_menu_view_page_margins, MainFrame::OnViewPageMargins)
    EVT_UPDATE_UI (k_menu_view_page_margins, MainFrame::disable_tool)   //OnEditUpdateUI)
    EVT_MENU      (k_menu_view_welcome_page, MainFrame::on_view_welcome_page)
    EVT_UPDATE_UI (k_menu_view_welcome_page, MainFrame::on_update_UI_welcome_page)
    //EVT_MENU      (k_menu_view_counters, MainFrame::on_create_counters_panel)

    //Score menu/toolbar
//    EVT_MENU      (k_menu_score_titles, MainFrame::OnScoreTitles)
    EVT_UPDATE_UI (k_menu_score_titles, MainFrame::disable_tool)   //OnEditUpdateUI)

    // Instrument menu
//	EVT_MENU      (k_menu_instr_properties, MainFrame::OnInstrumentProperties)
    EVT_UPDATE_UI (k_menu_instr_properties, MainFrame::disable_tool)   //OnEditUpdateUI)

	//Zoom menu/toolbar
    EVT_MENU(k_menu_zoom_in, MainFrame::on_zoom_in)
    EVT_MENU(k_menu_zoom_out, MainFrame::on_zoom_out)
    EVT_MENU (k_menu_zoom_100, MainFrame::on_zoom_100)
    EVT_MENU (k_menu_zoom_other, MainFrame::on_zoom_other)
    EVT_MENU (k_menu_zoom_fit_full, MainFrame::on_zoom_fit_full)
    EVT_MENU (k_menu_zoom_fit_width, MainFrame::on_zoom_fit_width)
    EVT_UPDATE_UI_RANGE (k_menu_zoom_in, k_menu_zoom_other, MainFrame::on_update_UI_zoom)
    EVT_COMBOBOX  (k_id_combo_zoom, MainFrame::on_combo_zoom )
    EVT_TEXT_ENTER(k_id_combo_zoom, MainFrame::on_combo_zoom )
    EVT_UPDATE_UI (k_id_combo_zoom, MainFrame::on_update_UI_zoom)

    //Sound menu/toolbar
    EVT_MENU      (k_menu_sound_midi_wizard, MainFrame::on_run_midi_wizard)
    EVT_MENU      (k_menu_sound_test, MainFrame::on_sound_test)
    EVT_MENU      (k_menu_sound_off, MainFrame::on_all_sounds_off)
    EVT_MENU      (k_menu_play_start, MainFrame::on_play_start)
    EVT_UPDATE_UI (k_menu_play_start, MainFrame::on_update_UI_sound)
//    EVT_MENU      (k_menu_play_cursor_start, MainFrame::OnPlayCursorStart)
    EVT_UPDATE_UI (k_menu_play_cursor_start, MainFrame::on_update_UI_sound)
    EVT_MENU      (k_menu_play_stop, MainFrame::on_play_stop)
    EVT_UPDATE_UI (k_menu_play_stop, MainFrame::on_update_UI_sound)
    EVT_MENU      (k_menu_play_pause, MainFrame::on_play_pause)
    EVT_UPDATE_UI (k_menu_play_pause, MainFrame::on_update_UI_sound)
//    EVT_MENU      (MENU_Metronome, MainFrame::OnMetronomeOnOff)
    EVT_UPDATE_UI (MENU_Metronome, MainFrame::on_update_UI_sound)

    EVT_MENU (k_menu_preferences, MainFrame::on_options)

//    EVT_MENU      (k_menu_open_book, MainFrame::OnOpenBook)
    EVT_UPDATE_UI (k_menu_open_book, MainFrame::disable_tool)   //OnOpenBookUI)

    // Window menu
    EVT_MENU (k_menu_windowCloseAll, MainFrame::on_window_close_all)

    // Help menu
    EVT_MENU(k_menu_help_about, MainFrame::on_about)
//    EVT_MENU      (k_menu_help_open, MainFrame::OnHelpOpen)
//    EVT_MENU      (k_menu_help_quick_guide, MainFrame::OnHelpQuickGuide)
    EVT_MENU      (k_menu_check_for_updates, MainFrame::on_check_for_updates)
    EVT_MENU      (k_menu_help_visit_website, MainFrame::on_visit_website)

        //debug menu. Only visible in Debug mode
#if (LENMUS_DEBUG_MENU == 1)
    EVT_MENU      (k_menu_debug_print_preview, MainFrame::on_debug_print_preview)
    EVT_UPDATE_UI (k_menu_debug_print_preview, MainFrame::on_update_UI_file)
    EVT_MENU(k_menu_debug_do_tests, MainFrame::on_do_tests)
    EVT_MENU(k_menu_debug_see_paths, MainFrame::on_see_paths)
    EVT_MENU(k_menu_debug_draw_box_docpage, MainFrame::on_debug_draw_box)
    EVT_MENU(k_menu_debug_draw_box_content, MainFrame::on_debug_draw_box)
    EVT_MENU(k_menu_debug_draw_box_system, MainFrame::on_debug_draw_box)
    EVT_MENU(k_menu_debug_draw_box_slice, MainFrame::on_debug_draw_box)
    EVT_MENU(k_menu_debug_draw_box_slice_instr, MainFrame::on_debug_draw_box)
    EVT_MENU(k_menu_debug_draw_box_inline, MainFrame::on_debug_draw_box)
    EVT_MENU(k_menu_debug_remove_boxes, MainFrame::on_debug_draw_box)
    EVT_MENU(k_menu_debug_justify_systems, MainFrame::on_debug_justify_systems)
    EVT_MENU (k_menu_debug_ForceReleaseBehaviour, MainFrame::on_debug_force_release_behaviour)
    EVT_MENU (k_menu_debug_ShowDebugLinks, MainFrame::on_debug_show_debug_links)
//    EVT_MENU (k_menu_debug_ShowBorderOnScores, MainFrame::OnDebugShowBorderOnScores)
//    EVT_MENU (k_menu_debug_SetTraceLevel, MainFrame::OnDebugSetTraceLevel)
//    EVT_MENU (k_menu_debug_PatternEditor, MainFrame::OnDebugPatternEditor)
//    EVT_MENU (k_menu_debug_recSelec, MainFrame::OnDebugRecSelec)
    EVT_MENU (k_menu_debug_draw_anchors, MainFrame::on_debug_draw_anchors)
//    EVT_MENU (k_menu_debug_do_tests, MainFrame::OnDebugUnitTests)
//    EVT_MENU (k_menu_debug_ShowDirtyObjects, MainFrame::OnDebugShowDirtyObjects)

        //debug events requiring a score to be enabled
    EVT_MENU      (k_menu_debug_dump_column_tables, MainFrame::on_debug_dump_column_tables)
    EVT_UPDATE_UI (k_menu_debug_dump_column_tables, MainFrame::on_update_UI_score)
    EVT_MENU      (k_menu_debug_SeeSource, MainFrame::on_debug_see_source)
    EVT_UPDATE_UI (k_menu_debug_SeeSource, MainFrame::on_update_UI_document)
//    EVT_MENU      (k_menu_debug_SeeSourceUndo, MainFrame::on_debug_see_sourceForUndo)
    EVT_UPDATE_UI (k_menu_debug_SeeSourceUndo, MainFrame::disable_tool)   //on_update_UI_document)
//    EVT_MENU      (k_menu_debug_SeeXML, MainFrame::OnDebugSeeXML)
    EVT_UPDATE_UI (k_menu_debug_SeeXML, MainFrame::disable_tool)   //on_update_UI_document)
    EVT_MENU      (k_menu_debug_see_midi_events, MainFrame::on_debug_see_midi_events)
    EVT_UPDATE_UI (k_menu_debug_see_midi_events, MainFrame::on_update_UI_score)
    EVT_MENU      (k_menu_debug_see_staffobjs, MainFrame::on_debug_see_staffobjs)
    EVT_UPDATE_UI (k_menu_debug_see_staffobjs, MainFrame::on_update_UI_score)
//    EVT_MENU      (k_menu_debug_DumpBitmaps, MainFrame::OnDebugDumpBitmaps)
    EVT_UPDATE_UI (k_menu_debug_DumpBitmaps, MainFrame::disable_tool)   //on_update_UI_score)
//    EVT_MENU      (k_menu_debug_CheckHarmony, MainFrame::OnDebugCheckHarmony)
    EVT_UPDATE_UI (k_menu_debug_CheckHarmony, MainFrame::disable_tool)   //on_update_UI_score)
//    EVT_MENU      (k_menu_debug_TestProcessor, MainFrame::OnDebugTestProcessor)
    EVT_UPDATE_UI (k_menu_debug_TestProcessor, MainFrame::disable_tool)   //on_update_UI_score)

        //debug events requiring a document to be enabled
    EVT_MENU      (k_menu_debug_dump_gmodel, MainFrame::on_debug_dump_gmodel)
    EVT_UPDATE_UI (k_menu_debug_dump_gmodel, MainFrame::on_update_UI_document)
#endif

//    //metronome
//    EVT_SPINCTRL    (k_id_spin_metronome, MainFrame::OnMetronomeUpdate)
//    EVT_TEXT        (k_id_spin_metronome,    MainFrame::OnMetronomeUpdateText)
//    EVT_TIMER       (k_id_timer_metronome,        MainFrame::OnMetronomeTimer)

    //other events
    EVT_CLOSE   (MainFrame::on_close_frame)
    EVT_SIZE    (MainFrame::on_size)
    //LM_EVT_COUNTERS_DLG(MainFrame::on_counters_event)
    EVT_AUI_PANE_CLOSE(MainFrame::OnPaneClose)
    //	EVT_CHAR(MainFrame::OnKeyPress)
    EVT_COMMAND(k_id_check_for_updates, LM_EVT_CHECK_FOR_UPDATES, MainFrame::on_silently_check_for_updates)
//	EVT_MENU(k_id_key_F1, MainFrame::OnKeyF1)

END_EVENT_TABLE()


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
//    , m_pHtmlWin(NULL)
    , m_pStatusBar(NULL)
    , m_pPrintData(NULL)
    , m_pPageSetupData(NULL)
    , m_fileHistory(9, wxID_FILE1)      //max files, id of first file)
{
    create_menu();
    show_status_bar_if_user_preferences();
    set_lomse_callbacks();

#if 0   //chage to '1' to automatically, run tests on start
    wxCommandEvent event;
    on_do_tests(event);
#endif

    // set the app icon
	// All non-MSW platforms use a bitmap. MSW uses an .ico file
    #if (LENMUS_PLATFORM_WIN32 == 1)
        //macro wxICON creates an icon using an icon resource on Windows.
        SetIcon(wxICON(app_icon));
	#else
		SetIcon(wxArtProvider::GetIcon(_T("app_icon"), wxART_OTHER));
	#endif

//	//acceleration keys table
//    wxAcceleratorEntry entries[1];
//    entries[0].Set(wxACCEL_CTRL, WXK_F1, wxID_ABOUT);
//    wxAcceleratorTable accel(1, entries);
//    SetAcceleratorTable(accel);
//
//	// create main metronome and associate it to frame metronome controls
//    //metronome speed. Default MM=60
//    long nMM = pPrefs->Read(_T("/Metronome/MM"), 60);
//    m_pMainMtr = LENMUS_NEW lmMetronome(nMM);
//    m_pMtr = m_pMainMtr;

//    // initialize flags for toggle buttons status
//    m_fHelpOpened = false;

    //initialize printing data
    m_pPrintData = LENMUS_NEW wxPrintData;
    m_pPrintData->SetPaperId(wxPAPER_A4);    //default

    m_pPageSetupData = LENMUS_NEW wxPageSetupDialogData;
    *m_pPageSetupData = *m_pPrintData;
    // Set some initial page margins in mm.
    m_pPageSetupData->SetMarginTopLeft(wxPoint(0, 0));
    m_pPageSetupData->SetMarginBottomRight(wxPoint(0, 0));
}

//---------------------------------------------------------------------------------------
MainFrame::~MainFrame()
{
    // deinitialize the layout manager
    m_layoutManager.UnInit();

//    delete m_pHelp;
    delete_status_bar();
    delete m_pPrintData;
    delete m_pPageSetupData;
}

//---------------------------------------------------------------------------------------
void MainFrame::save_preferences()
{
    //AWARE: MainFrame destructor is invoked from TheApp destructor, after
    //deletion of Preferences and Paths objects. Therefore, DO NOT invoke this
    //from MainFrame destructor: it will not work.
    //This method is invoked from MainFrame::quit() method when going to destroy
    //the mainframe

    wxLogMessage(_T("[MainFrame::save_preferences] Going to save preferences"));

    wxConfigBase* pPrefs = m_appScope.get_preferences();
    if (pPrefs)
    {
//        lmDocManager* pDocManager = this->GetDocumentManager();
//        //save the last selected directories
//        g_pPaths->SetScoresPath( pDocManager->GetLastDirectory() );
//        pDocManager->SaveRecentFiles();

        Paths* pPaths = m_appScope.get_paths();
        wxString msg = wxString::Format(
                            _T("[MainFrame::save_preferences] Doing it at '%s'"),
                            pPaths->GetConfigPath().c_str() );
        wxLogMessage(msg);

        // save the frame size and position
        wxSize wndSize = GetSize();
        wxPoint wndPos = GetPosition();
        bool fMaximized = IsMaximized();
        pPrefs->Write(_T("/MainFrame/Width"), wndSize.GetWidth());
        pPrefs->Write(_T("/MainFrame/Height"), wndSize.GetHeight());
        pPrefs->Write(_T("/MainFrame/Left"), wndPos.x );
        pPrefs->Write(_T("/MainFrame/Top"), wndPos.y );
        pPrefs->Write(_T("/MainFrame/Maximized"), fMaximized);

    //    //save metronome settings and delete main metronome
    //    if (m_pMainMtr) {
    //        pPrefs->Write(_T("/Metronome/MM"), m_pMainMtr->GetMM() );
    //        delete m_pMainMtr;
    //    }

        //save file history
        m_fileHistory.Save(*pPrefs);

        //force to write on disk. This shouldn't be necessary as, according to
        //documentation, wxConfig object does it at destructor. But I was having
        //problems in Linux and this solved it.
        pPrefs->Flush();
   }
}

//---------------------------------------------------------------------------------------
void MainFrame::create_menu()
{
    //Centralized code to create the menu bar.
    //bitmaps on menus are supported only on Windows and GTK+

    m_booksMenu = NULL;

    wxMenuItem* pItem;
    wxSize nIconSize(16, 16);
    wxConfigBase* pPrefs = m_appScope.get_preferences();

    // file menu --------------------------------------------------------------------------

    wxMenu* pMenuFile = LENMUS_NEW wxMenu;

    create_menu_item(pMenuFile, k_menu_file_new, _("&New\tCtrl+N"),
                    _("Open LENMUS_NEW blank document"), wxITEM_NORMAL, _T("tool_new"));
    create_menu_item(pMenuFile, k_menu_file_open, _("&Open ...\tCtrl+O"),
                    _("Open a document"), wxITEM_NORMAL, _T("tool_open"));
    create_menu_item(pMenuFile, k_menu_file_reload, _T("&Reload"),
                    _("Reload document"), wxITEM_NORMAL, _T(""));   //_T("tool_reload"));
    ////TODO 5.0
    //create_menu_item(pMenuFile, k_menu_open_book, _("Open &books"),
    //                _("Hide/show eMusicBooks"), wxITEM_NORMAL, _T("tool_open_ebook"));
    //create_menu_item(pMenuFile, k_menu_file_Import, _("&Import..."),
    //                _("Open a MusicXML score"), wxITEM_NORMAL);

    ////-- export submenu --
    //wxMenu* pSubmenuExport = LENMUS_NEW wxMenu;
    //create_menu_item(pSubmenuExport, k_menu_file_export_MusicXML, _("MusicXML format"),
    //                _("Save score as a MusicXML file"), wxITEM_NORMAL);
    //create_menu_item(pSubmenuExport, k_menu_file_export_bmp, _("As &bmp image"),
    //                _("Save score as BMP images"), wxITEM_NORMAL, _T("tool_save_as_bmp"));
    //create_menu_item(pSubmenuExport, k_menu_file_export_jpg, _("As &jpg image"),
    //                _("Save score as JPG images"), wxITEM_NORMAL, _T("tool_save_as_jpg"));

    //pItem = LENMUS_NEW wxMenuItem(pMenuFile, k_menu_file_export, _("&Export ..."),
    //                      _("Save score in other formats"), wxITEM_NORMAL, pSubmenuExport);
    //pItem->SetBitmap( wxArtProvider::GetBitmap(_T("empty"), wxART_TOOLBAR, nIconSize) );
    //pMenuFile->Append(pItem);

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
    pMenuFile->AppendSeparator();

    create_menu_item(pMenuFile, wxID_EXIT, _("&Quit\tCtrl+Q"),
                    _("Exit program"), wxITEM_NORMAL, _T("tool_exit"));


    // history of files visited.
    m_fileHistory.UseMenu(pMenuFile);
    if (pPrefs)
        m_fileHistory.Load(*pPrefs);


//    // eBooks menu -------------------------------------------------------------------
//    m_booksMenu = LENMUS_NEW wxMenu;
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


    //// edit menu -------------------------------------------------------------------
    ////TODO 5.0

    //m_editMenu = LENMUS_NEW wxMenu;
    //create_menu_item(m_editMenu, wxID_UNDO, _("&Undo"),
    //            _("Undo"), wxITEM_NORMAL, _T("tool_undo"));
    //create_menu_item(m_editMenu, wxID_REDO, _("&Redo"),
    //            _("Redo"), wxITEM_NORMAL, _T("tool_redo"));


    // View menu -------------------------------------------------------------------

    wxMenu* pMenuView = LENMUS_NEW wxMenu;
    create_menu_item(pMenuView, k_menu_view_toolBar, _("Tool &bar"),
                _("Hide/show the tools bar"), wxITEM_CHECK);
    create_menu_item(pMenuView, k_menu_view_statusBar, _("&Status bar"),
                _("Hide/show the status bar"), wxITEM_CHECK);
    pMenuView->AppendSeparator();
    ////TODO 5.0
    //create_menu_item(pMenuView, k_menu_view_tools, _("&Tool box"),
    //            _("Hide/show edition tool box window"), wxITEM_CHECK);
    //create_menu_item(pMenuView, k_menu_view_rulers, _("&Rulers"),
    //            _("Hide/show rulers"), wxITEM_CHECK);
    create_menu_item(pMenuView, k_menu_view_welcome_page, _("&Welcome page"),
                _("Hide/show welcome page"));
    //TO_REMOVE
    //create_menu_item(pMenuView, k_menu_view_counters, _("&Counters panel"),
    //            _("Hide/show counters panel"));

    //// score menu ------------------------------------------------------------------
    ////TODO 5.0

    //wxMenu* pMenuScore = LENMUS_NEW wxMenu;
    //create_menu_item(pMenuScore, k_menu_score_titles, _("Add title"),
				//_("Add a title to the score"), wxITEM_NORMAL, _T("tool_add_text"));
    //create_menu_item(pMenuScore, k_menu_view_page_margins, _("Margins and spacers"),
				//_("Show/hide page margins and spacers"), wxITEM_CHECK, _T("tool_page_margins"));


    //// instrument menu ------------------------------------------------------------------
    ////TODO 5.0

    //wxMenu* pMenuInstr = LENMUS_NEW wxMenu;
    //create_menu_item(pMenuInstr, k_menu_instr_properties, _("Properties"),
				//_("Edit name, abbreviation, MIDI settings and other properties"), wxITEM_NORMAL);


    // debug menu --------------------------------------------------------------------

    // Debug strings will not be translatable. It is mandatory that all development is
    // in English
    m_dbgMenu = NULL;

#if (LENMUS_DEBUG_MENU == 1)
    m_dbgMenu = LENMUS_NEW wxMenu;

    create_menu_item(m_dbgMenu, k_menu_debug_do_tests, _T("Run unit tests"));
    create_menu_item(m_dbgMenu, k_menu_debug_see_paths, _T("See paths") );
    m_dbgMenu->AppendSeparator();

    create_menu_item(m_dbgMenu, k_menu_debug_justify_systems, _("Justify systems"),
                    _T(""), wxITEM_CHECK);
    create_menu_item(m_dbgMenu, k_menu_debug_DumpBitmaps, _T("Save offscreen bitmaps") );
    create_menu_item(m_dbgMenu, k_menu_debug_CheckHarmony, _T("Check harmony") );
    create_menu_item(m_dbgMenu, k_menu_debug_TestProcessor, _T("Run test processor") );
    create_menu_item(m_dbgMenu, k_menu_debug_print_preview, _("Print Pre&view"),
                    _T(""), wxITEM_NORMAL);
    create_menu_item(m_dbgMenu, k_menu_debug_ForceReleaseBehaviour, _T("&Release Behaviour"),
        _T("Force release behaviour for certain functions"), wxITEM_CHECK);

    m_dbgMenu->AppendSeparator();   //draw marks
    create_menu_item(m_dbgMenu, k_menu_debug_ShowDebugLinks, _T("&Include debug links"),
        _T("Include debug controls in exercises"), wxITEM_CHECK);
    //create_menu_item(m_dbgMenu, k_menu_debug_ShowBorderOnScores, _T("&Border on ScoreAuxCtrol"),
    //    _T("Show border on ScoreAuxCtrol"), wxITEM_CHECK);
    //create_menu_item(m_dbgMenu, k_menu_debug_recSelec, _T("&Draw recSelec"),
    //    _T("Force to draw selection rectangles around staff objects"), wxITEM_CHECK);
    //create_menu_item(m_dbgMenu, k_menu_debug_SetTraceLevel, _T("Set trace level ...") );
    //create_menu_item(m_dbgMenu, k_menu_debug_PatternEditor, _T("Test Pattern Editor") );

    //-- Draw bounds submenu --
    wxMenu* pSubmenuDrawBox = LENMUS_NEW wxMenu;

    create_menu_item(pSubmenuDrawBox, k_menu_debug_draw_box_docpage, _T("Draw document page box"));
    create_menu_item(pSubmenuDrawBox, k_menu_debug_draw_box_content, _T("Draw score page box"));
    create_menu_item(pSubmenuDrawBox, k_menu_debug_draw_box_system, _T("Draw system box"));
    create_menu_item(pSubmenuDrawBox, k_menu_debug_draw_box_slice, _T("Draw slice box"));
    create_menu_item(pSubmenuDrawBox, k_menu_debug_draw_box_slice_instr, _T("Draw slice intrs box"));
    create_menu_item(pSubmenuDrawBox, k_menu_debug_remove_boxes, _T("Remove drawn boxes"));
    create_menu_item(pSubmenuDrawBox, k_menu_debug_draw_box_inline, _("Draw inline boxes"));

    pItem = LENMUS_NEW wxMenuItem(m_dbgMenu, k_menu_debug_draw_box, _T("Draw box ..."),
                        _T("Force to draw box rectangles"), wxITEM_NORMAL, pSubmenuDrawBox);
    m_dbgMenu->Append(pItem);

    //create_menu_item(m_dbgMenu, k_menu_debug_DrawBounds, _T("&Draw bounds"),
    //    _T("Force to draw bound rectangles around staff objects"), wxITEM_CHECK);
    create_menu_item(m_dbgMenu, k_menu_debug_draw_anchors, _T("Draw anchors"),
        _T("Draw a red line to show anchor objects"), wxITEM_CHECK);
    //create_menu_item(m_dbgMenu, k_menu_debug_ShowDirtyObjects, _T("&Show dirty objects"),
    //    _T("Render 'dirty' objects in red colour"), wxITEM_CHECK);

    m_dbgMenu->AppendSeparator();   //dump tables
    create_menu_item(m_dbgMenu, k_menu_debug_see_staffobjs, _T("See staffobjs table") );
    create_menu_item(m_dbgMenu, k_menu_debug_see_midi_events, _T("See &MIDI events") );
	create_menu_item(m_dbgMenu, k_menu_debug_dump_gmodel, _T("See graphical model") );
    create_menu_item(m_dbgMenu, k_menu_debug_dump_column_tables, _("See column tables"));

    m_dbgMenu->AppendSeparator();   //exporters
    create_menu_item(m_dbgMenu, k_menu_debug_SeeSource, _T("See &LDP source") );
    create_menu_item(m_dbgMenu, k_menu_debug_SeeSourceUndo, _T("See LDP source for &Undo/Redo") );
    create_menu_item(m_dbgMenu, k_menu_debug_SeeXML, _T("See &XML") );
#endif


    // Zoom menu -----------------------------------------------------------------------

//    m_zoomMenu->Append(k_menu_zoom_out);
//
    wxMenu* pMenuZoom = LENMUS_NEW wxMenu;
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

    wxMenu* pMenuSound = LENMUS_NEW wxMenu;

    create_menu_item(pMenuSound, k_menu_play_start, _("&play"),
                _("Start/resume play back. From selection of full score"), wxITEM_NORMAL,
                _T("tool_play"));
    create_menu_item(pMenuSound, k_menu_play_cursor_start, _("play from cursor"),
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
                    _("play an scale to test sound"), wxITEM_NORMAL, _T("tool_test_sound"));
    create_menu_item(pMenuSound, k_menu_sound_off, _("&All sounds off"),
                    _("Stop inmediatly all sounds"), wxITEM_NORMAL, _T("tool_stop_sounds"));


    // Options menu ---------------------------------------------------------------------

    wxMenu* pMenuOptions = LENMUS_NEW wxMenu;
    create_menu_item(pMenuOptions, k_menu_preferences,  _("&Preferences"),
                    _("Open help book"), wxITEM_NORMAL, _T("tool_options"));


    // Window menu -----------------------------------------------------------------------
    wxMenu* pMenuWindow = LENMUS_NEW wxMenu;
    pMenuWindow->Append(k_menu_windowCloseAll, _("Close all documents"));
    pMenuWindow->AppendSeparator();


    // Help menu -------------------------------------------------------------------------

    wxMenu* pMenuHelp = LENMUS_NEW wxMenu;

    create_menu_item(pMenuHelp, k_menu_help_about, _("&About"),
				_("Display information about program version and credits"), wxITEM_NORMAL,
                _T("tool_about"));
    pMenuHelp->AppendSeparator();

    ////TODO 5.0
    //create_menu_item(pMenuHelp, k_menu_help_quick_guide,  _("Editor quick guide"),
    //            _("Show editor reference card"), wxITEM_NORMAL, _T("tool_quick_guide"));
    //create_menu_item(pMenuHelp, k_menu_help_open,  _("&Content"),
    //            _("Open help book"), wxITEM_NORMAL, _T("tool_help"));
    //pMenuHelp->AppendSeparator();

	create_menu_item(pMenuHelp, k_menu_check_for_updates, _("Check now for &updates"),
				_("Connect to the Internet and check for program updates"), wxITEM_NORMAL,
                _T("tool_web_update"));
    create_menu_item(pMenuHelp, k_menu_help_visit_website,  _("&Visit LenMus website"),
                _("Open the Internet browser and go to LenMus website"), wxITEM_NORMAL,
                _T("tool_website"));


    // set up the menubar ---------------------------------------------------------------

    // AWARE: As MainFrame is derived from wxFrame, in MSWindows the menu
    // bar automatically inherits a "Window" menu inserted in the second last position.
    // To suppress it (under MSWindows) it is necessary to add style
    // wxFRAME_NO_WINDOW_MENU in frame creation.
    wxMenuBar* pMenuBar = LENMUS_NEW wxMenuBar;
    pMenuBar->Append(pMenuFile, _("&File"));
    //pMenuBar->Append(m_booksMenu, _("e&Books"));
    ////TODO 5.0
    //pMenuBar->Append(m_editMenu, _("&Edit"));
    pMenuBar->Append(pMenuView, _("&View"));
    //TODO 5.0
	//pMenuBar->Append(pMenuScore, _("S&core"));
	//pMenuBar->Append(pMenuInstr, _("&Instrument"));
    pMenuBar->Append(pMenuSound, _("&Sound"));

#if (LENMUS_DEBUG_MENU == 1)
    pMenuBar->Append(m_dbgMenu, _T("&Debug"));     //DO NOT TRANSLATE
#endif

    pMenuBar->Append(pMenuZoom, _("&Zoom"));
    pMenuBar->Append(pMenuOptions, _("&Options"));
    pMenuBar->Append(pMenuWindow, _("&Window"));
    pMenuBar->Append(pMenuHelp, _("&Help"));

//        //
//        // items initially checked
//        //
//
//    g_fDrawSelRect = false;    //true;

    //debug toolbar
#if (LENMUS_DEBUG_MENU == 1)
    pMenuBar->Check(k_menu_debug_ForceReleaseBehaviour,
                    m_appScope.is_release_behaviour());
    pMenuBar->Check(k_menu_debug_ShowDebugLinks, m_appScope.show_debug_links());
//        pMenuBar->Check(k_menu_debug_recSelec, g_fDrawSelRect);
#endif

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
void MainFrame::show_status_bar_if_user_preferences()
{
    bool fStatusBar = false;
    wxConfigBase* pPrefs = m_appScope.get_preferences();
    pPrefs->Read(_T("/MainFrame/ViewStatusBar"), &fStatusBar);
    if (!m_pStatusBar && fStatusBar)
        create_status_bar();
}

//---------------------------------------------------------------------------------------
void MainFrame::create_status_bar(int nType)
{
    //if the status bar exists and it is of same type, nothing to do
    if (m_pStatusBar && m_pStatusBar->GetType() == nType)
        return;

    m_pStatusBar = LENMUS_NEW StatusBar(this, (EStatusBarLayout)nType, k_menu_view_statusBar);
    SetStatusBar(m_pStatusBar);

    //use status bar as status reporter & transfer ownership
    m_appScope.set_status_reporter(m_pStatusBar);

    //the status bar pane is used by wxWidgets to display menu and toolbar help.
    //Using -1 disables help display.
    SetStatusBarPane(-1);

    SendSizeEvent();
}

//---------------------------------------------------------------------------------------
void MainFrame::delete_status_bar()
{
    if (!m_pStatusBar) return;

    m_pStatusBar = NULL;
    m_appScope.set_status_reporter(m_pStatusBar);   //this deletes status bar
    SetStatusBar(m_pStatusBar);
}

//---------------------------------------------------------------------------------------
void MainFrame::create_controls()
{
    m_layoutManager.SetManagedWindow(this);     //inform wxAUI which frame to use
    show_toolbars_if_user_preferences();

    //create the ContentWindow (main pane, a notebook) for scores and other content
    long style = wxAUI_NB_CLOSE_ON_ACTIVE_TAB |     //put close button on the active tab
                 wxAUI_NB_WINDOWLIST_BUTTON |       //drop-down list of tabs
                 wxAUI_NB_SCROLL_BUTTONS |          //scroll buttons for tabs
                 wxAUI_NB_TAB_MOVE ;                //tabs can be dragged horizontally

    m_pContentWindow = LENMUS_NEW ContentWindow(this, style);

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
void MainFrame::set_lomse_callbacks()
{
    LomseDoorway& lib = m_appScope.get_lomse();
    lib.set_notify_callback(this, wrapper_lomse_event);
    lib.set_request_callback(this, wrapper_lomse_request);
}

//---------------------------------------------------------------------------------------
void MainFrame::on_close_frame(wxCloseEvent& WXUNUSED(event))
{
    quit();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_file_quit(wxCommandEvent& WXUNUSED(event))
{
    quit();
}

//---------------------------------------------------------------------------------------
void MainFrame::quit()
{
    ScorePlayer* pPlayer = m_appScope.get_score_player();
    if (pPlayer->is_playing())
        pPlayer->stop();

    save_preferences();

    Destroy();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_about(wxCommandEvent& WXUNUSED(event))
{
   AboutDialog dlg(this, m_appScope);
   dlg.ShowModal();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_file_open(wxCommandEvent& WXUNUSED(event))
{
    open_file();
}

//---------------------------------------------------------------------------------------
void MainFrame::open_file()
{
    // LenMus files are either simple files, usually scores (.lms - LenMus Score) or
    // eMusicBooks (.lmb - LenMus Book) files
    // All eMusicBooks are located in 'locale/xx/books' folder

    Paths* pPaths = m_appScope.get_paths();
    wxString defaultPath = pPaths->GetBooksPath();
    wxString sFile = wxFileSelector(_("Open LenMus document"), defaultPath,
        wxEmptyString, wxEmptyString, wxT("LenMus files|*.lms;*.lmb"));

    if (!sFile.empty())
    {
        string filename( sFile.mb_str(wxConvUTF8) );
        load_file(filename);
        m_fileHistory.AddFileToHistory(sFile);
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::on_file_reload(wxCommandEvent& WXUNUSED(event))
{
    load_file(m_lastOpenFile);
}

//---------------------------------------------------------------------------------------
void MainFrame::load_file(const string& filename)
{
    //clear reporter
    LomseDoorway& lib = m_appScope.get_lomse();
    ostringstream& reporter = m_appScope.get_lomse_reporter();
    reporter.str(std::string());      //remove any previous content

    //create canvas and show document
    DocumentLoader loader(m_pContentWindow, m_appScope, lib);
    int viewType = ViewFactory::k_view_vertical_book;
    loader.create_canvas(filename, viewType);
    m_lastOpenFile = filename;

    //show errors, if any
    if (!reporter.str().empty())
    {
        wxString msg = to_wx_string( reporter.str() );
        wxString title = _("Errors in file ");
        title += to_wx_string(filename);
        DlgDebug dlg(this, title, msg, true /*show 'Save' button*/);
        dlg.ShowModal();
    }
    reporter.str(std::string());      //remove any previous content
    wxLogMessage(_T("[MainFrame::load_file] exiting"));
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
    int nAvailable = size.x;

    //Pointers, in presentation order:
    wxToolBar* pTool[7];
    pTool[0] = m_pTbFile;       //File tools
    pTool[1] = m_pTbEdit;       //Edit tools
    pTool[2] = m_pTbZoom;       //Zooming tools
    pTool[3] = m_pToolbar;      //Main tools
    pTool[4] = m_pTbPlay;       //play tools
    pTool[5] = m_pTbMtr;        //Metronome tools
    pTool[6] = m_pTbTextBooks;  //Textbooks

    //determine longest toolbar panel
    int nWidth = 0;
    for (int i=0; i < 7; ++i)
    {
        if (pTool[i])
            nWidth = max(nWidth, pTool[i]->GetSize().GetWidth());
    }
    nWidth += nGripWidth;

    //check that there is enough space for the longest toolbar panel
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
void MainFrame::wrapper_lomse_event(void* pThis, SpEventInfo pEvent)
{
    static_cast<MainFrame*>(pThis)->on_lomse_event(pEvent);
}

//---------------------------------------------------------------------------------------
void MainFrame::on_lomse_event(SpEventInfo pEvent)
{
    DocumentWindow* pCanvas = get_active_document_window();

    switch (pEvent->get_event_type())
    {
        case k_highlight_event:
        {
            if (pCanvas)
            {
                SpEventScoreHighlight pEv(
                    boost::static_pointer_cast<EventScoreHighlight>(pEvent) );
                lmScoreHighlightEvent event(pEv);
                ::wxPostEvent(pCanvas, event);
            }
            break;
        }

        case k_end_of_playback_event:
        {
            if (pCanvas)
            {
                SpEventPlayScore pEv( boost::static_pointer_cast<EventPlayScore>(pEvent) );
                lmEndOfPlaybackEvent event(pEv);
                ::wxPostEvent(pCanvas, event);
            }
            break;
        }

        case k_on_click_event:
        {
            DocumentFrame* pFrame = get_active_document_frame();
            if (pFrame)
                pFrame->on_hyperlink_event(pEvent);
            break;
        }

        default:
            wxString msg = wxString::Format(_T("[MainFrame::on_lomse_event] Event %d received from Lomse"),
                                            pEvent->get_event_type());
            wxMessageBox(msg);
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::wrapper_lomse_request(void* pThis, Request* pRequest)
{
    static_cast<MainFrame*>(pThis)->on_lomse_request(pRequest);
}

//---------------------------------------------------------------------------------------
void MainFrame::on_lomse_request(Request* pRequest)
{
    int type = pRequest->get_request_type();
    switch (type)
    {
        case k_dynamic_content_request:
            generate_dynamic_content( dynamic_cast<RequestDynamic*>(pRequest) );
            break;

        case k_get_font_filename:
            get_font_filename( dynamic_cast<RequestFont*>(pRequest) );
            break;

        default:
            wxLogMessage(_T("[MainFrame::on_lomse_request] Unknown request %d"), type);
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::generate_dynamic_content(RequestDynamic* pRD)
{
    ImoDynamic* pDyn = dynamic_cast<ImoDynamic*>( pRD->get_object() );
    string& classid = pDyn->get_classid();
    Document* pDoc = pRD->get_document();
    Canvas* pCanvas = get_active_canvas();
    DocumentWindow* pWnd = NULL;
    DocumentFrame* pFrame = dynamic_cast<DocumentFrame*>( pCanvas );
    if (pFrame)
        pWnd = pFrame->get_document_window();
    else
        pWnd = dynamic_cast<DocumentCanvas*>( pCanvas );

    DynControl* pControl
        = DynControlFactory::create_dyncontrol(m_appScope, classid, pWnd);
    pControl->generate_content(pDyn, pDoc);
    pDyn->set_generator(pControl);
}

//---------------------------------------------------------------------------------------
void MainFrame::get_font_filename(RequestFont* pRequest)
{
    //This is just a trivial example. In real applications you should
    //use operating system services to find a suitable font

    //notes on parameters received:
    // - fontname can be either the face name (i.e. "Book Antiqua") or
    //   the familly name (i.e. "sans-serif")

    const string& fontname = pRequest->get_fontname();
    bool bold = pRequest->get_bold();
    bool italic = pRequest->get_italic();

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
//        fontfile = "wqy/wqy-zenhei.ttc";
//        //fontfile = "arphic/uming.ttc";
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


    pRequest->set_font_fullname( path + fontfile );
//    wxLogMessage(_T("[MainFrame::get_font_filename] fontfile %s"), fontfile.c_str());


#elif (LENMUS_PLATFORM_WIN32 == 1)

    pRequest->set_font_fullname("");

    string path = "C:\\WINNT\\Fonts\\";

    //if family name, choose a font name
    string name = fontname;
    if (name == "serif")
        name = "Times New Roman";
    else if (name == "sans-serif")
        name = "Tahoma";
    else if (name == "handwritten")
        name = "Lucida Handwriting";
    else if (name == "cursive")
        name = "Monotype Corsiva";
    else if (name == "monospaced")
        name = "Courier New";

    //choose a suitable font file
    string fontfile;
    if (name == "Times New Roman")
    {
        if (italic && bold)
            fontfile = "timesbi.ttf";
        else if (italic)
            fontfile = "timesi.ttf";
        else if (bold)
            fontfile = "timesbd.ttf";
        else
            fontfile = "times.ttf";
    }

    else if (name == "Tahoma")
    {
        if (bold)
            fontfile = "tahomabd.ttf";
        else
            fontfile = "tahoma.ttf";
    }

    else if (name == "Lucida Handwriting")
    {
        fontfile = "lhandw.ttf";
    }

    else if (name == "Monotype Corsiva")
    {
        fontfile = "mtcorsva.ttf";
    }

    else if (name == "Courier New")
    {
        if (italic && bold)
            fontfile = "courbi.ttf";
        else if (italic)
            fontfile = "couri.ttf";
        else if (bold)
            fontfile = "courbd.ttf";
        else
            fontfile = "cour.ttf";
    }

    else
        fontfile = "times.ttf";

    pRequest->set_font_fullname( path + fontfile );


#else

    pRequest->set_font_fullname("");

#endif
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

//---------------------------------------------------------------------------------------
void MainFrame::update_toolbars_layout()
{
    // Recreate toolbars if visible. User has changed visualization options
	if (m_pToolbar)
    {
		delete_toolbars();
		create_toolbars();
	}
}

//---------------------------------------------------------------------------------------
void MainFrame::show_toolbars_if_user_preferences()
{
    bool fToolBar = true;
    wxConfigBase* pPrefs = m_appScope.get_preferences();
    pPrefs->Read(_T("/MainFrame/ViewToolBar"), &fToolBar);
    if (fToolBar)
        create_toolbars();
}

//---------------------------------------------------------------------------------------
void MainFrame::create_toolbars()
{
    if (m_pToolbar) return;

    //prepare style
    wxConfigBase* pPrefs = m_appScope.get_preferences();
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
    m_pToolbar = LENMUS_NEW wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize, style);
    m_pToolbar->SetToolBitmapSize(nSize);
    m_pToolbar->AddTool(k_menu_preferences, _T("Preferences"), wxArtProvider::GetBitmap(_T("tool_options"), wxART_TOOLBAR, nSize), _("Set user preferences"));
    m_pToolbar->AddTool(k_menu_help_open, _T("Help"), wxArtProvider::GetBitmap(_T("tool_help"), wxART_TOOLBAR, nSize), _("Help button"));
    m_pToolbar->Realize();

    //File toolbar
    m_pTbFile = LENMUS_NEW wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize, style);
    m_pTbFile->SetToolBitmapSize(nSize);
    ////TODO 5.0
    //m_pTbFile->AddTool(k_menu_file_new, _T("New"),
    //        wxArtProvider::GetBitmap(_T("tool_new"), wxART_TOOLBAR, nSize),
    //        wxArtProvider::GetBitmap(_T("tool_new_dis"), wxART_TOOLBAR, nSize),
    //        wxITEM_NORMAL, _("New score"));
    m_pTbFile->AddTool(k_menu_file_open, _T("Open"), wxArtProvider::GetBitmap(_T("tool_open"),
            wxART_TOOLBAR, nSize), _("Open a score"));
    ////TODO 5.0
    //m_pTbFile->AddTool(k_menu_open_book, _T("Books"),
    //        wxArtProvider::GetBitmap(_T("tool_open_ebook"), wxART_TOOLBAR, nSize),
    //        wxArtProvider::GetBitmap(_T("tool_open_ebook_dis"), wxART_TOOLBAR, nSize),
    //        wxITEM_NORMAL, _("Open the music books"));
    m_pTbFile->AddTool(wxID_SAVE, _T("Save"),
            wxArtProvider::GetBitmap(_T("tool_save"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_save_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Save current score to disk"));
    m_pTbFile->AddTool(k_menu_print, _T("Print"),
            wxArtProvider::GetBitmap(_T("tool_print"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_print_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Print document"));
    m_pTbFile->Realize();

    ////TODO 5.0
    ////Edit toolbar
    //m_pTbEdit = LENMUS_NEW wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize, style);
    //m_pTbEdit->SetToolBitmapSize(nSize);
    //m_pTbEdit->AddTool(k_menu_edit_copy, _T("Copy"),
    //        wxArtProvider::GetBitmap(_T("tool_copy"), wxART_TOOLBAR, nSize),
    //        wxArtProvider::GetBitmap(_T("tool_copy_dis"), wxART_TOOLBAR, nSize),
    //        wxITEM_NORMAL, _("Copy"));
    //m_pTbEdit->AddTool(k_menu_edit_cut, _T("Cut"),
    //        wxArtProvider::GetBitmap(_T("tool_cut"), wxART_TOOLBAR, nSize),
    //        wxArtProvider::GetBitmap(_T("tool_cut_dis"), wxART_TOOLBAR, nSize),
    //        wxITEM_NORMAL, _("Cut"));
    //m_pTbEdit->AddTool(k_menu_edit_paste, _T("Paste"),
    //        wxArtProvider::GetBitmap(_T("tool_paste"), wxART_TOOLBAR, nSize),
    //        wxArtProvider::GetBitmap(_T("tool_paste_dis"), wxART_TOOLBAR, nSize),
    //        wxITEM_NORMAL, _("Paste"));
    //m_pTbEdit->AddTool(wxID_UNDO, _T("Undo"),
    //        wxArtProvider::GetBitmap(_T("tool_undo"), wxART_TOOLBAR, nSize),
    //        wxArtProvider::GetBitmap(_T("tool_undo_dis"), wxART_TOOLBAR, nSize),
    //        wxITEM_NORMAL, _("Undo"));
    //m_pTbEdit->AddTool(wxID_REDO, _T("Redo"),
    //        wxArtProvider::GetBitmap(_T("tool_redo"), wxART_TOOLBAR, nSize),
    //        wxArtProvider::GetBitmap(_T("tool_redo_dis"), wxART_TOOLBAR, nSize),
    //        wxITEM_NORMAL, _("Redo"));
    //m_pTbEdit->AddSeparator();
    //m_pTbEdit->AddTool(k_menu_view_page_margins, _T("Page margins"),
    //        wxArtProvider::GetBitmap(_T("tool_page_margins"), wxART_TOOLBAR, nSize),
    //        wxArtProvider::GetBitmap(_T("tool_page_margins"), wxART_TOOLBAR, nSize),
    //        wxITEM_CHECK, _("Show/hide page margins and spacers"));
    //m_pTbEdit->Realize();

    //Zoom toolbar
    m_pTbZoom = LENMUS_NEW wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize, style);
    m_pTbZoom->SetToolBitmapSize(nSize);
    m_pTbZoom->AddTool(k_menu_zoom_fit_full, _T("Fit full"),
            wxArtProvider::GetBitmap(_T("tool_zoom_fit_full"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_zoom_fit_full_dis"), wxART_TOOLBAR, nSize),
            wxITEM_CHECK, _("Zoom so that the full page is displayed"));
    m_pTbZoom->AddTool(k_menu_zoom_fit_width, _T("Fit width"),
            wxArtProvider::GetBitmap(_T("tool_zoom_fit_width"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_zoom_fit_width_dis"), wxART_TOOLBAR, nSize),
            wxITEM_CHECK, _("Zoom so that page width equals window width"));
    m_pTbZoom->AddTool(k_menu_zoom_in, _T("Zoom in"),
            wxArtProvider::GetBitmap(_T("tool_zoom_in"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_zoom_in_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Enlarge image size"));
    m_pTbZoom->AddTool(k_menu_zoom_out, _T("Zoom out"),
            wxArtProvider::GetBitmap(_T("tool_zoom_out"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_zoom_out_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Reduce image size"));

    m_pComboZoom = LENMUS_NEW wxComboBox(m_pTbZoom, k_id_combo_zoom, _T(""),
                                         wxDefaultPosition, wxSize(90, -1) );
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

    //play toolbar
    m_pTbPlay = LENMUS_NEW wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize, style);
    m_pTbPlay->SetToolBitmapSize(nSize);
    m_pTbPlay->AddTool(k_menu_play_start, _T("play"),
            wxArtProvider::GetBitmap(_T("tool_play"), wxART_TOOLBAR, nSize),
            wxArtProvider::GetBitmap(_T("tool_play_dis"), wxART_TOOLBAR, nSize),
            wxITEM_NORMAL, _("Start/resume play back. From selection of full score"));
    m_pTbPlay->AddTool(k_menu_play_cursor_start, _T("play from cursor"),
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
    m_pTbMtr = LENMUS_NEW wxToolBar(this, -1, wxDefaultPosition, wxDefaultSize, style);
    m_pTbMtr->SetToolBitmapSize(nSize);
    m_pTbMtr->AddTool(MENU_Metronome, _T("Metronome"),
        wxArtProvider::GetBitmap(_T("tool_metronome"),
        wxART_TOOLBAR, nSize), _("Turn metronome on/off"),
        wxITEM_CHECK);
    m_pSpinMetronome = LENMUS_NEW wxSpinCtrl(m_pTbMtr, k_id_spin_metronome, _T(""), wxDefaultPosition,
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
    wxSize sizeZoomTb(5 * (sizeButton.GetWidth() + m_pTbZoom->GetToolSeparation()) +
                      sizeCombo.GetWidth() +
                      m_pTbZoom->GetToolSeparation() + 10,
                      wxMax(sizeCombo.GetHeight(), sizeButton.GetHeight()));

    // add the toolbars to the manager
    const int ROW_1 = 0;
    //const int ROW_2 = 1;
#if (LENMUS_PLATFORM_UNIX == 1)
    //In gtk reverse creation order
        // row 1
//    CreateTextBooksToolBar(style, nSize, ROW_1);

    m_layoutManager.AddPane(m_pTbMtr, wxAuiPaneInfo().
                Name(wxT("Metronome")).Caption(_("Metronome tools")).
                ToolbarPane().Top().Row(ROW_1).BestSize( sizeBest ).
                LeftDockable(false).RightDockable(false));
    m_layoutManager.AddPane(m_pTbPlay, wxAuiPaneInfo().
                Name(wxT("play")).Caption(_("play tools")).
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
    ////TODO 5.0
    //m_layoutManager.AddPane(m_pTbEdit, wxAuiPaneInfo().
    //            Name(wxT("Edit tools")).Caption(_("Edit tools")).
    //            ToolbarPane().Top().
    //            LeftDockable(false).RightDockable(false));
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
    ////TODO 5.0
    //m_layoutManager.AddPane(m_pTbEdit, wxAuiPaneInfo().
    //            Name(wxT("Edit tools")).Caption(_("Edit tools")).
    //            ToolbarPane().Top().
    //            LeftDockable(false).RightDockable(false));
    m_layoutManager.AddPane(m_pTbZoom, wxAuiPaneInfo().
                Name(wxT("Zooming tools")).Caption(_("Zooming tools")).
                ToolbarPane().Top().BestSize( sizeZoomTb ).
                LeftDockable(false).RightDockable(false));
    m_layoutManager.AddPane(m_pToolbar, wxAuiPaneInfo().
                Name(wxT("toolbar")).Caption(_("Main tools")).
                ToolbarPane().Top().
                LeftDockable(false).RightDockable(false));
    m_layoutManager.AddPane(m_pTbPlay, wxAuiPaneInfo().
                Name(wxT("play")).Caption(_("play tools")).
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

//---------------------------------------------------------------------------------------
void MainFrame::delete_toolbars()
{
    // main toolbar
    if (m_pToolbar)
    {
        m_layoutManager.DetachPane(m_pToolbar);
        delete m_pToolbar;
        m_pToolbar = (wxToolBar*)NULL;
    }

    // file toolbar
    if (m_pTbFile)
    {
        m_layoutManager.DetachPane(m_pTbFile);
        delete m_pTbFile;
        m_pTbFile = (wxToolBar*)NULL;
    }

    // edit toolbar
    if (m_pTbEdit)
    {
        m_layoutManager.DetachPane(m_pTbEdit);
        delete m_pTbEdit;
        m_pTbEdit = (wxToolBar*)NULL;
    }

    // play toolbar
    if (m_pTbPlay)
    {
        m_layoutManager.DetachPane(m_pTbPlay);
        delete m_pTbPlay;
        m_pTbPlay = (wxToolBar*)NULL;
    }

    // metronome toolbar
    if (m_pTbMtr)
    {
        m_layoutManager.DetachPane(m_pTbMtr);
        delete m_pTbMtr;
        m_pTbMtr = (wxToolBar*)NULL;
    }

    // zoom toolbar
    if (m_pTbZoom)
    {
        m_layoutManager.DetachPane(m_pTbZoom);
        delete m_pTbZoom;
        m_pTbZoom = (wxToolBar*)NULL;
    }

    // Text books navigation toolbar
    if (m_pTbTextBooks)
    {
        m_layoutManager.DetachPane(m_pTbTextBooks);
        delete m_pTbTextBooks;
        m_pTbTextBooks = (wxToolBar*)NULL;
    }

    // tell the manager to "commit" all the changes just made
    m_layoutManager.Update();
}

//void MainFrame::CreateTextBooksToolBar(long style, wxSize nIconSize, int nRow)
//{
//    m_pTbTextBooks = LENMUS_NEW wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, style);
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

//---------------------------------------------------------------------------------------
void MainFrame::create_menu_item(wxMenu* pMenu, int nId, const wxString& sItemName,
                                 const wxString& sToolTip, wxItemKind nKind,
                                 const wxString& sIconName)
{
    //Create a menu item and add it to the received menu

    wxMenuItem* pItem = LENMUS_NEW wxMenuItem(pMenu, nId, sItemName, sToolTip, nKind);


    //icons are supported only in Windows and Linux, and only in wxITEM_NORMAL items
    #if (LENMUS_PLATFORM_WIN32 == 1 || LENMUS_PLATFORM_UNIX == 1)
    if (nKind == wxITEM_NORMAL && sIconName != _T(""))
        pItem->SetBitmap( wxArtProvider::GetBitmap(sIconName, wxART_TOOLBAR, wxSize(16, 16)) );
    #endif

    pMenu->Append(pItem);
}

//---------------------------------------------------------------------------------------
void MainFrame::add_new_panel(wxWindow* window, const wxString& caption,
                              const wxPoint& position)
{
    m_layoutManager.AddPane(window, wxAuiPaneInfo().Caption(caption).
                            Float().FloatingPosition(position) );
    m_layoutManager.Update();
}

//---------------------------------------------------------------------------------------
void MainFrame::OnPaneClose(wxAuiManagerEvent& evt)
{
    if (evt.pane->caption == _("Counters & options"))
    {
        int res = wxMessageBox(wxT("Are you sure you want to close/hide this pane?"),
                               wxT("wxAUI"),
                               wxYES_NO,
                               this);
        if (res != wxYES)
            evt.Veto();
    }
}

////---------------------------------------------------------------------------------------
//void MainFrame::on_create_counters_panel(wxCommandEvent& WXUNUSED(event))
//{
//    ExerciseCtrol* pExerciseCtrol = NULL;
//    QuizManager* pProblemMngr = LENMUS_NEW QuizManager(m_appScope, pExerciseCtrol);
//    DlgCounters* pDlg = create_counters_dlg(k_quiz_mode, pProblemMngr);
//
//    add_new_panel(pDlg, _("Counters & options"), get_counters_position());
//}

////---------------------------------------------------------------------------------------
//void MainFrame::on_counters_event(CountersEvent& event)
//{
//    //ExerciseCtrol* pExercise = event.get_exercise();
//    //ExerciseOptions* pConstrains = event.get_constrains();
//    //ProblemManager* pManager = event.get_problem_manager();
//
//    //QuizManager* pQuizMngr = (QuizManager*)pManager;
//    //int numTeams = 2;
//    //DlgCounters* pDlg = LENMUS_NEW QuizCounters(this, wxID_ANY, numTeams, pExercise,
//    //                                     pConstrains, pQuizMngr, wxPoint(0,0));
//
//    wxWindow* pDlg = event.get_dialog();
//    add_new_panel(pDlg, _("Counters & options"), get_counters_position());
//    pDlg->Show(true);
//}

////---------------------------------------------------------------------------------------
//DlgCounters* MainFrame::create_counters_dlg(int mode, ProblemManager* pManager)
//{
//    int numTeams = 2;
//    ExerciseCtrol* pExerciseCtrol = NULL;
//    ExerciseOptions* pConstrains = NULL;
//    return LENMUS_NEW QuizCounters(this, wxID_ANY, numTeams, pExerciseCtrol, pConstrains,
//                            (QuizManager*)pManager, wxPoint(0,0));
//
//}
//
////---------------------------------------------------------------------------------------
//wxPoint MainFrame::get_counters_position()
//{
//    static int x = 0;
//    x += 20;
//    wxPoint pt = ClientToScreen(wxPoint(0,0));
//    return wxPoint(pt.x + x, pt.y + x);
//}

////---------------------------------------------------------------------------------------
//void MainFrame::InitializeHelp()
//{
//    // create the help window
//    //m_pHelp = LENMUS_NEW lmHelpController(wxHF_DEFAULT_STYLE | wxHF_FLAT_TOOLBAR );
//    // previous sentence commented out and replaced by next one to remove
//    // index panel.
//    m_pHelp = LENMUS_NEW lmHelpController(wxHF_TOOLBAR | wxHF_FLAT_TOOLBAR | wxHF_CONTENTS |
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
//    if (m_appScope.is_release_behaviour()) {
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
//    m_pBookController = LENMUS_NEW TextBookController();
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

//---------------------------------------------------------------------------------------
void MainFrame::show_welcome_window()
{
    if (!is_welcome_page_displayed())
    {
        m_pWelcomeWnd = LENMUS_NEW WelcomeWindow(m_pContentWindow, m_appScope, &m_fileHistory, wxNewId());
        m_pContentWindow->add_canvas(m_pWelcomeWnd, _("Start page"));
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::on_open_recent_file(wxCommandEvent &event)
{
    wxString sFile = m_fileHistory.GetHistoryFile(event.GetId() - wxID_FILE1);
    load_file( to_std_string(sFile) );
}

////---------------------------------------------------------------------------------------
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

////---------------------------------------------------------------------------------------
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

//---------------------------------------------------------------------------------------
void MainFrame::on_visit_website(wxCommandEvent& WXUNUSED(event))
{
    ::wxLaunchDefaultBrowser( _T(LENMUS_VENDOR_SITE) );
}

//---------------------------------------------------------------------------------------
void MainFrame::on_check_for_updates(wxCommandEvent& WXUNUSED(event))
{
    Updater oUpdater(m_appScope);
    oUpdater.check_for_updates(this, false /*interactive*/);
}

//---------------------------------------------------------------------------------------
void MainFrame::on_silently_check_for_updates(wxCommandEvent& WXUNUSED(event))
{
    Updater oUpdater(m_appScope);
    oUpdater.check_for_updates(this, true /*silently*/);
}

////---------------------------------------------------------------------------------------
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

////---------------------------------------------------------------------------------------
//void MainFrame::OnExportMusicXML(wxCommandEvent& WXUNUSED(event))
//{
//	//TODO
//}

////---------------------------------------------------------------------------------------
//void MainFrame::OnExportBMP(wxCommandEvent& WXUNUSED(event))
//{
//    ExportAsImage(wxBITMAP_TYPE_BMP);
//}

////---------------------------------------------------------------------------------------
//void MainFrame::OnExportJPG(wxCommandEvent& WXUNUSED(event))
//{
//    ExportAsImage(wxBITMAP_TYPE_JPEG);
//}

////---------------------------------------------------------------------------------------
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

////---------------------------------------------------------------------------------------
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
//        // create the LENMUS_NEW controller
//        InitializeHelp();
//        wxASSERT(m_pHelp);
//
//        // open it
//        m_pHelp->Display(_T("index.htm"));
//        m_fHelpOpened = true;
//    }
//
//}

////---------------------------------------------------------------------------------------
//void MainFrame::SetOpenHelpButton(bool fPressed)
//{
//    m_fHelpOpened = fPressed;
//}

////---------------------------------------------------------------------------------------
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

////---------------------------------------------------------------------------------------
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

//---------------------------------------------------------------------------------------
void MainFrame::on_window_close_all(wxCommandEvent& WXUNUSED(event))
{
    // Invoked from menu: Window > Close all

    close_all();
    //show_welcome_window();
}



//=======================================================================================
//methods only available in debug version
//=======================================================================================

#if (LENMUS_DEBUG_MENU == 1)

//---------------------------------------------------------------------------------------
void MainFrame::on_do_tests(wxCommandEvent& WXUNUSED(event))
{
    MyTestRunner oTR(this);
    oTR.RunTests();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_draw_box(wxCommandEvent& event)
{
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
    {
        switch (event.GetId())
        {
            case k_menu_debug_draw_box_docpage:
                pCanvas->set_debug_draw_box(GmoObj::k_box_doc_page_content);
                break;
            case k_menu_debug_draw_box_content:
                //pCanvas->set_debug_draw_box(GmoObj::k_box_paragraph);
                pCanvas->set_debug_draw_box(GmoObj::k_box_score_page);
                //pCanvas->set_debug_draw_box(GmoObj::k_box_table);
                break;
            case k_menu_debug_draw_box_system:
                pCanvas->set_debug_draw_box(GmoObj::k_box_system);
                break;
            case k_menu_debug_draw_box_slice:
                pCanvas->set_debug_draw_box(GmoObj::k_box_slice);
                break;
            case k_menu_debug_draw_box_slice_instr:
                pCanvas->set_debug_draw_box(GmoObj::k_box_slice_instr);
                break;
            case k_menu_debug_draw_box_inline:
                pCanvas->set_debug_draw_box(GmoObj::k_box_inline);
                break;
            default:
                pCanvas->set_debug_draw_box(GmoObj::k_box_paragraph);
                break;
        }
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_justify_systems(wxCommandEvent& event)
{
    bool fChecked = m_dbgMenu->IsChecked(k_menu_debug_justify_systems);
    LomseDoorway& lib = m_appScope.get_lomse();
    LibraryScope* pScope = lib.get_library_scope();
    pScope->set_justify_systems(fChecked);

    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
        pCanvas->on_document_updated();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_dump_column_tables(wxCommandEvent& event)
{
    LomseDoorway& lib = m_appScope.get_lomse();
    LibraryScope* pScope = lib.get_library_scope();
    pScope->set_dump_column_tables(true);
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
        pCanvas->on_document_updated();
    pScope->set_dump_column_tables(false);
    wxMessageBox(_T("Tables are saved in file dbg_tables.txt, in the same folder than this project"));
}

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_force_release_behaviour(wxCommandEvent& event)
{
    m_appScope.force_release_behaviour( event.IsChecked() );
}

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_show_debug_links(wxCommandEvent& event)
{
    m_appScope.set_show_debug_links( event.IsChecked() );
}

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
//        GetActiveDoc()->UpdateAllViews((wxView*)NULL, LENMUS_NEW lmUpdateHint() );
//    }
//}

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_draw_anchors(wxCommandEvent& event)
{
    bool fChecked = m_dbgMenu->IsChecked(k_menu_debug_draw_anchors);
    LomseDoorway& lib = m_appScope.get_lomse();
    LibraryScope* pScope = lib.get_library_scope();
    pScope->set_draw_anchors(fChecked);
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
        pCanvas->on_document_updated();
}

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


//---------------------------------------------------------------------------------------
void MainFrame::on_debug_dump_gmodel(wxCommandEvent& WXUNUSED(event))
{
    Interactor* pInt = get_active_canvas_interactor();
    if (pInt)
    {
        GraphicModel* pGM = pInt->get_graphic_model();
        stringstream out;
        pGM->dump_page(0, out);
        DlgDebug dlg(this, _T("graphic model dump"), to_wx_string(out.str()) );
        dlg.ShowModal();
    }
}

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

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_see_source(wxCommandEvent& WXUNUSED(event))
{
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
        pCanvas->debug_display_ldp_source();

//    ImoScore* pScore = get_active_score();
//    wxASSERT(pScore);
//
//    DlgDebug dlg(this, _T("Generated source code"), pScore->SourceLDP(false));    //false: do not include undo/redo data
//    dlg.ShowModal();
}

//void MainFrame::on_debug_see_sourceForUndo(wxCommandEvent& event)
//{
//    ImoScore* pScore = get_active_score();
//    wxASSERT(pScore);
//
//    DlgDebug dlg(this, _T("Generated source code"), pScore->SourceLDP(true));     //true: include undo/redo data
//    dlg.ShowModal();
//}
//
//void MainFrame::OnDebugSeeXML(wxCommandEvent& event)
//{
//    ImoScore* pScore = get_active_score();
//    wxASSERT(pScore);
//
//    DlgDebug dlg(this, _T("Generated MusicXML code"), pScore->SourceXML());
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

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_see_midi_events(wxCommandEvent& WXUNUSED(event))
{
    ImoScore* pScore = get_active_score();
    if (pScore)
    {
        SoundEventsTable* pTable = pScore->get_midi_table();
        DlgDebug dlg(this, _T("MIDI events table"), to_wx_string(pTable->dump_midi_events()) );
        dlg.ShowModal();
    }
    else
        wxMessageBox(_T("No active score"));
}

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_see_staffobjs(wxCommandEvent& WXUNUSED(event))
{
    ImoScore* pScore = get_active_score();
    if (pScore)
    {
        ColStaffObjs* pTable = pScore->get_staffobjs_table();
        DlgDebug dlg(this, _T("MIDI events table"), to_wx_string(pTable->dump()) );
        dlg.ShowModal();
    }
    else
        wxMessageBox(_T("No active score"));
}

//---------------------------------------------------------------------------------------
void MainFrame::on_see_paths(wxCommandEvent& WXUNUSED(event))
{
    Paths* pPaths = m_appScope.get_paths();
    DlgDebug dlg(this, _T("Paths"), to_wx_string(pPaths->dump_paths()) );
    dlg.ShowModal();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_print_preview(wxCommandEvent& WXUNUSED(event))
{
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
    {
        wxPrintDialogData printDialogData(*m_pPrintData);

        // Pass two printout objects: for preview, and possible printing.
        wxPrintPreview *preview =
            LENMUS_NEW wxPrintPreview( LENMUS_NEW MyPrintout(m_pPageSetupData, pCanvas),
                                LENMUS_NEW MyPrintout(m_pPageSetupData, pCanvas),
                                &printDialogData);
        if (!preview->Ok())
        {
            delete preview;
            wxMessageBox(_T("There was a problem previewing.\nPerhaps your current printer is not set correctly?"),
                         _T("Previewing"), wxOK);
            return;
        }

        wxPreviewFrame *frame = LENMUS_NEW wxPreviewFrame(preview, this, _T("Print Preview"));
        frame->Centre(wxBOTH);
        frame->Initialize();
        frame->Show();
    }
}



//void MainFrame::OnDebugSetTraceLevel(wxCommandEvent& WXUNUSED(event))
//{
//    lmDlgDebugTrace dlg(this);
//    dlg.ShowModal();
//}

#endif   // END OF METHODS INCLUDED ONLY IN DEBUG BUILD ---------------------------------


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
//    SetFocusOnActiveView();
//
//    //wxLogMessage(_T("[MainFrame::OnActiveChildChanged] Is kind of lmDocTDIChildFrame: %s"),
//    //    pFrame->IsKindOf(CLASSINFO(lmDocTDIChildFrame)) ? _T("yes") : _T("No") );
//}

//---------------------------------------------------------------------------------------
void MainFrame::on_zoom_in(wxCommandEvent& WXUNUSED(event))
{
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
    {
        pCanvas->zoom_in();
        pCanvas->Refresh(false /* don't erase background */);
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::on_zoom_out(wxCommandEvent& WXUNUSED(event))
{
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
    {
        pCanvas->zoom_out();
        pCanvas->Refresh(false /* don't erase background */);
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::on_zoom_fit_width(wxCommandEvent& event)
{
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
    {
        pCanvas->zoom_fit_width();
        pCanvas->Refresh(false /* don't erase background */);
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::zoom_to(double scale)
{
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
    {
        pCanvas->zoom_to(scale);
        pCanvas->Refresh(false /* don't erase background */);
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::on_zoom_fit_full(wxCommandEvent& event)
{
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
    {
        pCanvas->zoom_fit_full();
        pCanvas->Refresh(false /* don't erase background */);
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::on_update_UI_zoom(wxUpdateUIEvent &event)
{
    DocumentWindow* pCanvas = get_active_document_window();
    event.Enable(pCanvas != NULL);

    if (pCanvas)
    {
        //toolbar buttons
        int zoomMode = pCanvas->get_zoom_mode();
        if (zoomMode == DocumentWindow::k_zoom_fit_full)
        {
            m_pTbZoom->ToggleTool(k_menu_zoom_fit_full, true);
            m_pTbZoom->ToggleTool(k_menu_zoom_fit_width, false);
        }
        else if (zoomMode == DocumentWindow::k_zoom_fit_width)
        {
            m_pTbZoom->ToggleTool(k_menu_zoom_fit_full, false);
            m_pTbZoom->ToggleTool(k_menu_zoom_fit_width, true);
        }
        else    //k_zoom_user
        {
            m_pTbZoom->ToggleTool(k_menu_zoom_fit_full, false);
            m_pTbZoom->ToggleTool(k_menu_zoom_fit_width, false);
        }

        //zoom combo box
        Interactor* pInteractor = pCanvas->get_interactor();
        if (pInteractor && m_pComboZoom)
        {
            double scale = pInteractor->get_scale();
            m_pComboZoom->SetValue(wxString::Format(_T("%.2f%%"), scale * 100.0));
        }
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::on_zoom_other(wxCommandEvent& event)
{
    Interactor* pInt = get_active_canvas_interactor();
    if (pInt)
    {
        double scale = pInt->get_scale() * 100.0;
        int nZoom = int( ::wxGetNumberFromUser(_T(""),
                            _("Zooming? (10 to 800)"), _T(""),
                            int(scale), 10, 800) );
        if (nZoom != -1)    // -1 means invalid input or user canceled
            zoom_to( double(nZoom) / 100.0 );
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::on_combo_zoom(wxCommandEvent& event)
{
    wxString sValue = event.GetString();
    if (sValue == _("Fit page full"))
    {
        on_zoom_fit_full(event);
    }
    else if (sValue == _("Fit page width"))
    {
        on_zoom_fit_width(event);
    }
    else if (sValue == _("Actual size"))
    {
        zoom_to(1.0);
    }
    else
    {
        sValue.Replace(_T("%"), _T(""));
        sValue.Trim();
        double rZoom;
        if (!sValue.ToDouble(&rZoom))
        {
            wxMessageBox(wxString::Format(_("Invalid zooming factor '%s'"), sValue.c_str()),
                         _("Error message"), wxOK || wxICON_HAND );
            return;
        }
        if (rZoom < 9.9 || rZoom > 801.0)
        {
            wxMessageBox(_("Zooming factor must be greater that 10% and lower than 800%"),
                         _("Error message"), wxOK || wxICON_HAND );
            return;
        }
        zoom_to(rZoom/100.0);
    }
	event.Skip();      //continue processing the  event
}

////---------------------------------------------------------------------------------------
//void MainFrame::OnComboVoice(wxCommandEvent& event)
//{
//    //int nVoice = event.GetSelection();
//    //wxMessageBox(wxString::Format(_T("Voice %d selected"), nVoice));
//    SetFocusOnActiveView();
//}
//
//// View menu event handlers

////---------------------------------------------------------------------------------------
//bool MainFrame::IsToolBoxVisible()
//{
//	return (m_pToolBox && m_layoutManager.GetPane(_T("ToolBox")).IsShown());
//}
//

////---------------------------------------------------------------------------------------
//void MainFrame::OnViewTools(wxCommandEvent& event)
//{
//    ShowToolBox(event.IsChecked());
//}

////---------------------------------------------------------------------------------------
//void MainFrame::ShowToolBox(bool fShow)
//{
//    //create the ToolBox
//    if (!m_pToolBox)
//    {
//        m_pToolBox =  LENMUS_NEW lmToolBox(this, wxID_ANY);
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

////---------------------------------------------------------------------------------------
//void MainFrame::OnViewRulers(wxCommandEvent& event)
//{
//    lmScoreView* pView = GetActiveScoreView();
//    pView->SetRulersVisible(event.IsChecked());
//}

////---------------------------------------------------------------------------------------
//void MainFrame::OnViewRulersUI(wxUpdateUIEvent &event)
//{
//    //For now, always disabled in release versions
//    if (m_appScope.is_release_behaviour()) {
//        event.Enable(false);
//    }
//    else {
//        lmTDIChildFrame* pChild = GetActiveChild();
//        event.Enable( pChild && pChild->IsKindOf(CLASSINFO(lmEditFrame)) );
//    }
//}

////---------------------------------------------------------------------------------------
//bool MainFrame::ShowRulers()
//{
//    return GetMenuBar()->IsChecked(k_menu_view_rulers);
//}

//---------------------------------------------------------------------------------------
void MainFrame::on_view_tool_bar(wxCommandEvent& WXUNUSED(event))
{
    if (!m_pToolbar)
        create_toolbars();
    else
        delete_toolbars();

    bool fToolBar = (m_pToolbar != NULL);
    wxConfigBase* pPrefs = m_appScope.get_preferences();
    pPrefs->Write(_T("/MainFrame/ViewToolBar"), fToolBar);
}

//---------------------------------------------------------------------------------------
void MainFrame::on_update_UI_tool_bar(wxUpdateUIEvent &event)
{
    event.Check(m_pToolbar != NULL);
}


//---------------------------------------------------------------------------------------
void MainFrame::on_view_status_bar(wxCommandEvent& WXUNUSED(event))
{
    if (!m_pStatusBar)
        create_status_bar();
    else
        delete_status_bar();

    bool fStatusBar = (m_pStatusBar != NULL);
    wxConfigBase* pPrefs = m_appScope.get_preferences();
    pPrefs->Write(_T("/MainFrame/ViewStatusBar"), fStatusBar);
}

//---------------------------------------------------------------------------------------
void MainFrame::on_update_UI_status_bar(wxUpdateUIEvent &event)
{
    event.Check (m_pStatusBar != NULL);
}

////---------------------------------------------------------------------------------------
//void MainFrame::NewScoreWindow(lmEditorMode* pMode, ImoScore* pScore)
//{
//    //Open a LENMUS_NEW score editor window in mode pMode
//
//    wxASSERT(pScore);
//    ShowToolBox(true);      //force to display ToolBox
//    m_pDocManager->OpenDocument(pMode, pScore);
//}

////---------------------------------------------------------------------------------------
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

//---------------------------------------------------------------------------------------
void MainFrame::on_print_setup(wxCommandEvent& WXUNUSED(event))
{
    wxPrintDialogData printDialogData(*m_pPrintData);
    wxPrintDialog printerDialog(this, &printDialogData);
    printerDialog.ShowModal();

    (*m_pPrintData) = printerDialog.GetPrintDialogData().GetPrintData();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_print(wxCommandEvent& WXUNUSED(event))
{
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
    {
        wxPrintDialogData printDialogData(*m_pPrintData);
        wxPrinter printer(&printDialogData);

        //TODO: get document name or file name
        //wxString title = pCanvas->
        MyPrintout printout(m_pPageSetupData, pCanvas);

        if (!printer.Print(this, &printout, true)) {
            if (wxPrinter::GetLastError() == wxPRINTER_ERROR)
                wxMessageBox(_("There is a problem for printing.\nPerhaps your current printer is not set correctly?"), _("Printing"), wxOK);
            else
                wxMessageBox(_("Printing cancelled"), _("Printing"), wxOK);

        } else {
            (*m_pPrintData) = printer.GetPrintDialogData().GetPrintData();
        }
    }
}

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

//---------------------------------------------------------------------------------------
void MainFrame::on_update_UI_file(wxUpdateUIEvent &event)
{
    DocumentFrame* pCanvas = dynamic_cast<DocumentFrame*>(get_active_canvas());
    bool fDocumentFrame = (pCanvas != NULL);
    bool fDebug = (LENMUS_DEBUG_MENU == 1);

    switch (event.GetId())
    {
        // Print related commands: enabled if DocumentFrame visible
        case k_menu_debug_print_preview:
            event.Enable(fDebug && fDocumentFrame);
            break;

        case wxID_PRINT_SETUP:
            //TODO: disabled in 3.3. Incompatibilities with wx2.7.1
            event.Enable(fDocumentFrame);
            break;

        case k_menu_print:
            event.Enable(fDocumentFrame);
            break;

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

        // Other commnads: always enabled
        default:
            event.Enable(true);
    }

//    if (m_appScope.is_release_behaviour()) {
//        switch (event.GetId())
//        {
//            case k_menu_file_export_MusicXML:
//                event.Enable(false);
//                break;
//        }
//    }

}

//---------------------------------------------------------------------------------------
void MainFrame::on_update_UI_score(wxUpdateUIEvent &event)
{
    ImoScore* pScore = get_active_score();
    event.Enable(pScore != NULL);
}

//---------------------------------------------------------------------------------------
void MainFrame::on_update_UI_document(wxUpdateUIEvent &event)
{
    DocumentWindow* pCanvas = get_active_document_window();
    event.Enable(pCanvas != NULL);
}

//---------------------------------------------------------------------------------------
void MainFrame::on_update_UI_sound(wxUpdateUIEvent &event)
{
    ImoScore* pScore = get_active_score();
    switch (event.GetId())
    {
//		case MENU_Metronome:
//			event.Enable(true);
//			event.Check(m_pMtr->IsRunning());
//			break;
//
        case k_menu_play_start:
        {
            ScorePlayer* pPlayer = m_appScope.get_score_player();
            event.Enable( pScore != NULL && !pPlayer->is_playing());
            break;
        }
        case k_menu_play_cursor_start:
            event.Enable(false);        //for now, disabled

        case k_menu_play_stop:
        case k_menu_play_pause:
        {
            ScorePlayer* pPlayer = m_appScope.get_score_player();
            event.Enable( pScore != NULL && pPlayer->is_playing());
            break;
        }

        default:
            // Other items: only enabled if a score is displayed
            event.Enable( pScore != NULL );
    }
}

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
    ImoScore* pScore = get_active_score();
    if (pScore)
    {
        ScorePlayer* pPlayer  = m_appScope.get_score_player();
        pPlayer->load_score(pScore, NULL);

        bool fVisualTracking = true;
        bool fCountOff = GetMenuBar()->IsChecked(k_menu_play_countoff);
        int playMode = k_play_normal_instrument;
        long nMM = 60;    //60000/pMtr->GetInterval();
        DocumentCanvas* pCanvas = dynamic_cast<DocumentCanvas*>( get_active_canvas() );
        Interactor* pInteractor = NULL;
        if (pCanvas)
            pInteractor = pCanvas->get_interactor();

        pPlayer->play(fVisualTracking, fCountOff, playMode, nMM, pInteractor);
    }
}
//
//void MainFrame::OnPlayCursorStart(wxCommandEvent& WXUNUSED(event))
//{
//    lmScoreView* pView = GetActiveScoreView();
//    bool fCountOff = GetMenuBar()->IsChecked(k_menu_play_countoff);
//    pView->GetController()->PlayScore(true, fCountOff);	//true: from cursor
//}

//---------------------------------------------------------------------------------------
void MainFrame::on_play_stop(wxCommandEvent& WXUNUSED(event))
{
    ScorePlayer* pPlayer  = m_appScope.get_score_player();
    pPlayer->stop();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_play_pause(wxCommandEvent& WXUNUSED(event))
{
    ScorePlayer* pPlayer  = m_appScope.get_score_player();
    pPlayer->pause();
}

//void MainFrame::OnScoreWizard(wxCommandEvent& WXUNUSED(event))
//{
//    ImoScore* pScore = (ImoScore*)NULL;
//    lmScoreWizard oWizard(this, &pScore);
//    oWizard.Run();
//
//    if (pScore)
//    {
//        //Wizard finished successfully. A score has been defined.
//        //Create a LENMUS_NEW score editor window and display it
//        NewScoreWindow((lmEditorMode*)NULL, pScore);
//    }
//}

//---------------------------------------------------------------------------------------
void MainFrame::on_options(wxCommandEvent& WXUNUSED(event))
{
    OptionsDlg dlg(this, m_appScope);
    dlg.CentreOnParent();
    dlg.ShowModal();
}

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
//    get_active_score()->OnInstrProperties(-1, pController);    //-1 = select instrument
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
//void MainFrame::DumpScore(ImoScore* pScore)
//{
//    if (!pScore) return;
//    DlgDebug dlg(this, _T("lmStaff objects dump"), pScore->Dump());
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
//        GetActiveDoc()->UpdateAllViews((wxView*)NULL, LENMUS_NEW lmUpdateHint() );
//    }
//}

//---------------------------------------------------------------------------------------
void MainFrame::on_view_welcome_page(wxCommandEvent& WXUNUSED(event))
{
    show_welcome_window();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_update_UI_welcome_page(wxUpdateUIEvent &event)
{
    event.Enable( !is_welcome_page_displayed() );
}

//---------------------------------------------------------------------------------------
bool MainFrame::is_welcome_page_displayed()
{
    if (m_pWelcomeWnd)
    {
        int idx = get_canvas_index(m_pWelcomeWnd);
        if (idx == wxNOT_FOUND)
            m_pWelcomeWnd = NULL;
    }
    return (m_pWelcomeWnd != NULL);
}

////---------------------------------------------------------------------------------------
//void MainFrame::OnPaneClose(wxAuiManagerEvent& event)
//{
//    event.Skip();      //continue processing the  event
//}
//
////---------------------------------------------------------------------------------------
//void MainFrame::SetStatusBarMsg(const wxString& sText)
//{
//    if (m_pStatusBar)
//        m_pStatusBar->report_status(sText);
//}
//
////---------------------------------------------------------------------------------------
//void MainFrame::SetStatusBarMouseData(int nPage, float rTime, int nMeasure,
//                                        lmUPoint uPos)
//{
//    if (m_pStatusBar)
//        m_pStatusBar->report_mouse_data(nPage, rTime, nMeasure, uPos);
//}
//
////---------------------------------------------------------------------------------------
//void MainFrame::SetStatusBarCaretData(int nPage, float rTime, int nMeasure)
//{
//    if (m_pStatusBar)
//        m_pStatusBar->report_caret_data(nPage, rTime, nMeasure);
//}
//
////---------------------------------------------------------------------------------------
//void MainFrame::OnKeyPress(wxKeyEvent& event)
//{
//	//if (event.GetEventType()==wxEVT_KEY_DOWN)
//        //if (event.GetKeyCode()==WXK_F1 && IsToolBoxVisible())
//	{
//		RedirectKeyPressEvent(event);
//	}
//}
//
////---------------------------------------------------------------------------------------
//void MainFrame::OnKeyF1(wxCommandEvent& event)
//{
////		int i = 1;
//}
//
//
//
////------------------------------------------------------------------------------------
//// Tips at application start
////------------------------------------------------------------------------------------
//
////---------------------------------------------------------------------------------------
//void MainFrame::ShowTips(bool fForceShow)
//{
//    bool fShowTips = false;
//    pPrefs->Read(_T("/MainFrame/ShowTips"), &fShowTips);
//    if (fForceShow || fShowTips)
//    {
//        //read data from last run
//        wxLogNull null; // disable error message if tips file does not exist
//        //wxString sTipsFile = pPrefs->Read(_T("/data_path")) + _T("/tips.txt");
//        //sTipsFile = g_pPaths->GetHelpPath();
//        //m_pHelp->SetTitleFormat(_("Test mode: using .hhp help file"));
//        long nTipsIndex = pPrefs->Read(_T("/MainFrame/NextTip"), 0L);
//
//        //show next tip
//        wxTipProvider oTipDlg = wxCreateFileTipProvider(sTipsFile, nTipsIndex);
//        fShowTips = wxShowTip(this, &oTipDlg, fShowTips);
//
//        //save data for next run
//        pPrefs->Write(_T("/MainFrame/ShowTips"), fShowTips);
//        pPrefs->Write(_T("/MainFrame/NextTip"), (long)oTipDlg.GetCurrentTip());
//    }
//}
//
//*/


//---------------------------------------------------------------------------------------
void MainFrame::disable_tool(wxUpdateUIEvent &event)
{
    event.Enable(false);
}

//---------------------------------------------------------------------------------------
ImoScore* MainFrame::get_active_score()
{
    DocumentWindow* pWnd = get_active_document_window();
    if (pWnd)
        return pWnd->get_active_score();
    else
        return NULL;
}

//---------------------------------------------------------------------------------------
Interactor* MainFrame::get_active_canvas_interactor()
{
    DocumentWindow* pWnd = get_active_document_window();
    if (pWnd)
        return pWnd->get_interactor();
    else
        return NULL;
}

//---------------------------------------------------------------------------------------
DocumentWindow* MainFrame::get_active_document_window()
{
    Canvas* pCanvas = get_active_canvas();
    DocumentFrame* pFrame = dynamic_cast<DocumentFrame*>( pCanvas );
    if (pFrame)
        return pFrame->get_document_window();
    else
        return dynamic_cast<DocumentWindow*>( pCanvas );
}

//---------------------------------------------------------------------------------------
DocumentFrame* MainFrame::get_active_document_frame()
{
    Canvas* pCanvas = get_active_canvas();
    return dynamic_cast<DocumentFrame*>( pCanvas );
}


//=======================================================================================
// MyPrintout implementation
//=======================================================================================

bool MyPrintout::OnPrintPage(int page)
{
    //Called by the wxWidgets framework when a page should be printed. Returning
    //false cancels the print job.

    wxDC *dc = GetDC();
    if (dc)
    {
        DrawPage(page);
        return true;
    }
    else
        return false;
}

//---------------------------------------------------------------------------------------
bool MyPrintout::OnBeginDocument(int startPage, int endPage)
{
    //Called by the wxWidgets framework at the start of document printing.
    //Return false from this function cancels the print job.
    //OnBeginDocument is called once for every copy printed.

    if (!wxPrintout::OnBeginDocument(startPage, endPage))
        return false;

    return true;
}

//---------------------------------------------------------------------------------------
void MyPrintout::GetPageInfo(int *pMinPage, int *pMaxPage,
                             int *pSelPageFrom, int *pSelPageTo)
{
    //Called by the wxWidgets framework to obtain information from the application
    //about minimum and maximum page values that the user can select, and the
    //required page range to be printed.

    m_pCanvas->get_pages_info(pMinPage, pMaxPage, pSelPageFrom, pSelPageTo);
    m_nMinPage = *pMinPage;
    m_nMaxPage = *pMaxPage;
}

//---------------------------------------------------------------------------------------
bool MyPrintout::HasPage(int nPage)
{
    return (nPage >= m_nMinPage && nPage <= m_nMaxPage);
}

//---------------------------------------------------------------------------------------
void MyPrintout::DrawPage(int page)
{
    //set the printer DC
    wxDC* pDC = GetDC();
    if (!pDC) return;
    pDC->SetUserScale(1.0f, 1.0f);
    pDC->SetDeviceOrigin(0, 0);

    //do print
    int w, h;
    pDC->GetSize(&w, &h);
    m_pCanvas->do_print(pDC, page, w, h);
}


}   //namespace lenmus
