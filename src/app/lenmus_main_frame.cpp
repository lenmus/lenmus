//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2020 LenMus project
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
#include "lenmus_midi_server.h"
#include "lenmus_welcome_window.h"
#include "lenmus_dlg_debug.h"
#include "lenmus_dlg_books.h"
#include "lenmus_about_dialog.h"
#include "lenmus_dyncontrol.h"
#include "lenmus_paths.h"
#include "lenmus_options_dlg.h"
#include "lenmus_generators.h"
#include "lenmus_updater.h"
#include "lenmus_command_window.h"
#include "lenmus_tool_box.h"
#include "lenmus_virtual_keyboard.h"
#include "lenmus_dlg_metronome.h"
#include "lenmus_help_system.h"
#include "lenmus_actions.h"
#if (LENMUS_ENABLE_UNIT_TESTS == 1)
    #include "lenmus_test_runner.h"
    #include <UnitTest++.h>
#endif

//lomse
#include <lomse_logger.h>
#include <lomse_score_player.h>
#include <lomse_midi_table.h>
#include <lomse_internal_model.h>
#include <lomse_doorway.h>
#include <lomse_interactor.h>
#include <lomse_staffobjs_table.h>
#include <lomse_metronome.h>
#include <lomse_interactor.h>
#include <lomse_graphical_model.h>
using namespace lomse;

//wxWidgets
#include <wx/numdlg.h>
#include <wx/textdlg.h>

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
#include <wx/osx/printdlg.h>
#endif


namespace lenmus
{

DEFINE_EVENT_TYPE(LM_EVT_CHECK_FOR_UPDATES)
DEFINE_EVENT_TYPE(LM_EVT_OPEN_BOOK)
DEFINE_EVENT_TYPE(LM_EVT_EDIT_COMMAND)

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
    k_menu_file_reload = k_menu_last_public_id,
    k_menu_file_close,
    k_menu_file_close_all,
    k_menu_print,
    //k_menu_print_setup,    wxID_PRINT_SETUP is used instead


     // Menu View
    k_menu_view_rulers,
    k_menu_view_page_margins,
    k_menu_view_welcome_page,
    k_menu_view_counters,
    k_menu_view_virtual_keyboard,
    k_menu_view_toc,

    // Menu Debug
    k_menu_view_console,
    k_menu_debug_do_tests,
    k_menu_debug_draw_box,
    k_menu_debug_draw_box_document,
    k_menu_debug_draw_box_docpage,
    k_menu_debug_draw_box_content,
    k_menu_debug_draw_box_score_page,
    k_menu_debug_draw_box_system,
    k_menu_debug_draw_box_slice,
    k_menu_debug_draw_box_slice_instr,
    k_menu_debug_draw_box_inline,
    k_menu_debug_draw_box_link,
    k_menu_debug_draw_box_paragraph,
    k_menu_debug_draw_box_control,
    k_menu_debug_draw_box_table,
    k_menu_debug_draw_box_table_rows,
    k_menu_debug_remove_boxes,
    k_menu_debug_justify_systems,
	k_menu_debug_spacing_parameters,
    k_menu_debug_trace_lines_break,
    k_menu_debug_dump_column_tables,
    k_menu_debug_force_release_behaviour,
    k_menu_debug_show_debug_links,
    k_menu_debug_draw_shape_bounds,
    k_menu_debug_draw_anchor_objects,
    k_menu_debug_draw_anchor_lines,
    k_menu_debug_draw_slur_ctrol_points,
    k_menu_debug_draw_vertical_profile,
	k_menu_debug_dump_gmodel,
	k_menu_debug_dump_imodel,
    k_menu_see_ldp_source,
    k_menu_see_checkpoint_data,
    k_menu_see_lmd_source,
    k_menu_see_mnx_source,
    k_menu_debug_see_musicxml,
    k_menu_see_spacing_data,
    k_menu_debug_see_document_ids,
    k_menu_debug_see_midi_events,
    k_menu_debug_see_paths,
    k_menu_debug_see_staffobjs,
    k_menu_debug_see_cursor_state,
    k_menu_debug_print_preview,
    k_menu_debug_test_api,

    // Menu Zoom
    k_menu_zoom_in,
    k_menu_zoom_out,
    k_menu_zoom_100,
    k_menu_zoom_fit_full,
    k_menu_zoom_fit_width,
    k_menu_zoom_other,

    // Menu Sound
    k_menu_sound_test,
    k_menu_sound_off,

    // Menu play
    k_menu_play_start,
    k_menu_play_stop,
    k_menu_play_pause,
    k_menu_play_countoff,

    // Menu Options
    k_menu_preferences,

     // Menu Tools
    k_menu_tools_metronome,

    // Menu Help
    k_menu_help_users_guide,
    k_menu_help_search,
    k_menu_help_visit_website,
    k_menu_check_for_updates,

  // other IDs
    k_id_timer_metronome,
    k_id_caret_timer,
	k_id_key_F1,

    k_menu_max
};

//---------------------------------------------------------------------------------------
// other constants
const int VIEW_TYPE = k_view_vertical_book; //k_view_half_page;

//---------------------------------------------------------------------------------------
// events table
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)

    //File menu
    EVT_MENU(k_menu_file_quit, MainFrame::on_file_quit)
    EVT_MENU(k_menu_file_open, MainFrame::on_file_open)
    EVT_MENU      (k_menu_file_reload, MainFrame::on_file_reload)
    EVT_UPDATE_UI (k_menu_file_reload, MainFrame::on_update_UI_file)
    EVT_MENU      (k_menu_file_close, MainFrame::on_file_close)
    EVT_UPDATE_UI (k_menu_file_close, MainFrame::on_update_UI_file)
    EVT_MENU      (k_menu_file_close_all, MainFrame::on_file_close_all)
    EVT_UPDATE_UI (k_menu_file_close_all, MainFrame::on_update_UI_file)

    EVT_MENU      (wxID_PRINT_SETUP, MainFrame::on_print_setup)
    EVT_UPDATE_UI (wxID_PRINT_SETUP, MainFrame::on_update_UI_file)
    EVT_MENU      (k_menu_print, MainFrame::on_print)
    EVT_UPDATE_UI (k_menu_print, MainFrame::on_update_UI_file)
    EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, MainFrame::on_open_recent_file)
    EVT_MENU      (k_menu_open_books, MainFrame::on_open_books)

    //View menu
    EVT_UPDATE_UI (k_menu_view_rulers, MainFrame::disable_tool)   //OnViewRulersUI)
    EVT_MENU      (k_menu_view_toc, MainFrame::on_view_hide_show_toc)
    EVT_UPDATE_UI (k_menu_view_toc, MainFrame::on_update_UI_view_toc)
    EVT_UPDATE_UI (k_menu_view_page_margins, MainFrame::disable_tool)   //on_update_UI_edit)
    EVT_MENU      (k_menu_view_welcome_page, MainFrame::on_view_welcome_page)
    EVT_UPDATE_UI (k_menu_view_welcome_page, MainFrame::on_update_UI_welcome_page)
    EVT_MENU      (k_menu_view_virtual_keyboard, MainFrame::on_view_virtual_keyboard)
    EVT_UPDATE_UI (k_menu_view_virtual_keyboard, MainFrame::on_update_UI_view_virtual_keyboard)

	//Zoom menu
    EVT_MENU(k_menu_zoom_in, MainFrame::on_zoom_in)
    EVT_MENU(k_menu_zoom_out, MainFrame::on_zoom_out)
    EVT_MENU (k_menu_zoom_100, MainFrame::on_zoom_100)
    EVT_MENU (k_menu_zoom_other, MainFrame::on_zoom_other)
    EVT_MENU (k_menu_zoom_fit_full, MainFrame::on_zoom_fit_full)
    EVT_MENU (k_menu_zoom_fit_width, MainFrame::on_zoom_fit_width)

    //Sound menu
    EVT_MENU      (k_menu_sound_test, MainFrame::on_sound_test)
    EVT_MENU      (k_menu_sound_off, MainFrame::on_all_sounds_off)
    EVT_MENU      (k_menu_play_start, MainFrame::on_play_start)
    EVT_UPDATE_UI (k_menu_play_start, MainFrame::on_update_UI_sound)
    EVT_MENU      (k_menu_play_stop, MainFrame::on_play_stop)
    EVT_UPDATE_UI (k_menu_play_stop, MainFrame::on_update_UI_sound)
    EVT_MENU      (k_menu_play_pause, MainFrame::on_play_pause)
    EVT_UPDATE_UI (k_menu_play_pause, MainFrame::on_update_UI_sound)

    //Options menu
    EVT_MENU      (k_menu_preferences, MainFrame::on_options)

    //Tools menu
    EVT_MENU      (k_menu_tools_metronome, MainFrame::on_metronome_tool)

    // Help menu
    EVT_MENU(k_menu_help_about, MainFrame::on_about)
    EVT_MENU      (k_menu_help_users_guide, MainFrame::on_open_help)
    EVT_UPDATE_UI (k_menu_help_users_guide, MainFrame::on_update_UI_help)
    EVT_MENU      (k_menu_help_editor_quick, MainFrame::on_open_help)
    EVT_UPDATE_UI (k_menu_help_editor_quick, MainFrame::on_update_UI_help)
    EVT_MENU      (k_menu_help_study_guide, MainFrame::on_open_help)
    EVT_MENU      (k_menu_help_search, MainFrame::on_open_help)
    EVT_UPDATE_UI (k_menu_help_search, MainFrame::on_update_UI_help)
    EVT_MENU      (k_menu_check_for_updates, MainFrame::on_check_for_updates)
    EVT_MENU      (k_menu_help_visit_website, MainFrame::on_visit_website)


    //debug menu. Only visible in Debug mode or in Release test mode
#if (LENMUS_DEBUG_BUILD == 1 || LENMUS_RELEASE_INSTALL == 0)

        //debug events always enabled
    EVT_MENU(k_menu_debug_test_api, MainFrame::on_debug_test_api)
    EVT_MENU(k_menu_debug_dump_column_tables, MainFrame::on_debug_dump_column_tables)
    EVT_MENU(k_menu_debug_do_tests, MainFrame::on_do_tests)
    EVT_MENU(k_menu_debug_see_paths, MainFrame::on_see_paths)
    EVT_MENU(k_menu_debug_draw_box_document, MainFrame::on_debug_draw_box)
    EVT_MENU(k_menu_debug_draw_box_docpage, MainFrame::on_debug_draw_box)
    EVT_MENU(k_menu_debug_draw_box_content, MainFrame::on_debug_draw_box)
    EVT_MENU(k_menu_debug_draw_box_score_page, MainFrame::on_debug_draw_box)
    EVT_MENU(k_menu_debug_draw_box_system, MainFrame::on_debug_draw_box)
    EVT_MENU(k_menu_debug_draw_box_slice, MainFrame::on_debug_draw_box)
    EVT_MENU(k_menu_debug_draw_box_slice_instr, MainFrame::on_debug_draw_box)
    EVT_MENU(k_menu_debug_draw_box_inline, MainFrame::on_debug_draw_box)
    EVT_MENU(k_menu_debug_draw_box_link, MainFrame::on_debug_draw_box)
    EVT_MENU(k_menu_debug_draw_box_paragraph, MainFrame::on_debug_draw_box)
    EVT_MENU(k_menu_debug_draw_box_control, MainFrame::on_debug_draw_box)
    EVT_MENU(k_menu_debug_draw_box_table, MainFrame::on_debug_draw_box)
    EVT_MENU(k_menu_debug_draw_box_table_rows, MainFrame::on_debug_draw_box)
    EVT_MENU(k_menu_debug_remove_boxes, MainFrame::on_debug_draw_box)
    EVT_MENU(k_menu_debug_justify_systems, MainFrame::on_debug_justify_systems)
    EVT_UPDATE_UI (k_menu_debug_justify_systems, MainFrame::on_update_UI_debug)
    EVT_MENU(k_menu_debug_spacing_parameters, MainFrame::on_debug_spacing_parameters)
    EVT_MENU(k_menu_debug_trace_lines_break, MainFrame::on_debug_trace_lines_break)
    EVT_MENU (k_menu_debug_force_release_behaviour, MainFrame::on_debug_force_release_behaviour)
    EVT_MENU (k_menu_debug_show_debug_links, MainFrame::on_debug_show_debug_links)
    EVT_MENU (k_menu_debug_draw_shape_bounds, MainFrame::on_debug_draw_shape_bounds)
    EVT_MENU (k_menu_debug_draw_anchor_objects, MainFrame::on_debug_draw_anchor_objects)
    EVT_MENU (k_menu_debug_draw_anchor_lines, MainFrame::on_debug_draw_anchor_lines)
    EVT_MENU (k_menu_debug_draw_slur_ctrol_points, MainFrame::on_debug_draw_slur_ctrol_points)
    EVT_MENU (k_menu_debug_draw_vertical_profile, MainFrame::on_debug_draw_vertical_profile)

        //debug events requiring a score to be enabled
    EVT_MENU      (k_menu_see_ldp_source, MainFrame::on_debug_see_ldp_source)
    EVT_UPDATE_UI (k_menu_see_ldp_source, MainFrame::on_update_UI_document)
    EVT_MENU      (k_menu_see_lmd_source, MainFrame::on_debug_see_lmd_source)
    EVT_UPDATE_UI (k_menu_see_lmd_source, MainFrame::on_update_UI_document)
    EVT_MENU      (k_menu_see_mnx_source, MainFrame::on_debug_see_mnx_source)
    EVT_UPDATE_UI (k_menu_see_mnx_source, MainFrame::on_update_UI_document)
    EVT_MENU      (k_menu_see_checkpoint_data, MainFrame::on_debug_see_checkpoint_data)
    EVT_UPDATE_UI (k_menu_see_checkpoint_data, MainFrame::on_update_UI_document)
    EVT_UPDATE_UI (k_menu_debug_see_musicxml, MainFrame::disable_tool)   //on_update_UI_document)
    EVT_MENU      (k_menu_see_spacing_data, MainFrame::on_debug_see_spacing_data)
    EVT_UPDATE_UI (k_menu_see_spacing_data, MainFrame::on_update_UI_document)
    EVT_MENU      (k_menu_debug_see_document_ids, MainFrame::on_debug_see_document_ids)
    EVT_UPDATE_UI (k_menu_debug_see_document_ids, MainFrame::on_update_UI_document)
    EVT_MENU      (k_menu_debug_see_midi_events, MainFrame::on_debug_see_midi_events)
    EVT_UPDATE_UI (k_menu_debug_see_midi_events, MainFrame::on_update_UI_score)
    EVT_MENU      (k_menu_debug_see_staffobjs, MainFrame::on_debug_see_staffobjs)
    EVT_UPDATE_UI (k_menu_debug_see_staffobjs, MainFrame::on_update_UI_score)
    EVT_MENU      (k_menu_debug_see_cursor_state, MainFrame::on_debug_see_cursor_state)
    EVT_UPDATE_UI (k_menu_debug_see_cursor_state, MainFrame::on_update_UI_document)

        //debug events requiring a document
    EVT_MENU      (k_menu_debug_dump_gmodel, MainFrame::on_debug_dump_gmodel)
    EVT_UPDATE_UI (k_menu_debug_dump_gmodel, MainFrame::on_update_UI_document)
    EVT_MENU      (k_menu_debug_dump_imodel, MainFrame::on_debug_dump_imodel)
    EVT_UPDATE_UI (k_menu_debug_dump_imodel, MainFrame::on_update_UI_document)
    EVT_MENU      (k_menu_debug_print_preview, MainFrame::on_debug_print_preview)
    EVT_UPDATE_UI (k_menu_debug_print_preview, MainFrame::on_update_UI_document)
#endif

    //other events
    EVT_TIMER   (k_id_caret_timer, MainFrame::on_caret_timer_event)
    EVT_CLOSE   (MainFrame::on_close_frame)
    EVT_AUI_PANE_CLOSE(MainFrame::on_tab_close)
    EVT_CHAR(MainFrame::on_key_press)
    EVT_COMMAND(k_id_check_for_updates, LM_EVT_CHECK_FOR_UPDATES, MainFrame::on_silently_check_for_updates)
    EVT_COMMAND(k_id_open_book, LM_EVT_OPEN_BOOK, MainFrame::on_open_book)
    EVT_COMMAND(k_id_edit_command, LM_EVT_EDIT_COMMAND, MainFrame::on_edit_command)
    LM_EVT_TOOLBOX_TOOL_SELECTED(MainFrame::on_toolbox_tool_selected)
    LM_EVT_TOOLBOX_PAGE_CHANGED(MainFrame::on_toolbox_page_changed)
    EVT_AUINOTEBOOK_PAGE_CHANGING(wxID_ANY, MainFrame::on_active_canvas_changing)
wxEND_EVENT_TABLE()


//---------------------------------------------------------------------------------------
MainFrame::MainFrame(ApplicationScope& appScope, const wxPoint& pos,
                     const wxSize& size)
    : ContentFrame(nullptr, wxID_ANY, appScope.get_app_full_name(), pos, size)
    , PlayerGui()
    , m_appScope(appScope)
    , m_lastOpenFile("")
    , m_pWelcomeWnd(nullptr)
    , m_pConsole(nullptr)
    , m_pVirtualKeyboard(nullptr)
    , m_pSpacingParamsDlg(nullptr)
    , m_pMetronomeDlg(nullptr)
    , m_pToolBox(nullptr)
    , m_pPrintData(nullptr)
    , m_pPageSetupData(nullptr)
    , m_fileHistory(9, wxID_FILE1)      //max files, id of first file)
    , m_nblinkTime(500)     //milliseconds
{
    create_menu();
    set_lomse_callbacks();
    load_global_options();

#if 0   //chage to '1' to automatically, run tests on start
    wxCommandEvent event;
    on_do_tests(event);
#endif

    // set the app icon
	// All non-MSW platforms use a bitmap. MSW uses an .ico file
 //   #if (LENMUS_PLATFORM_WIN32 == 1)
 //       //macro wxICON creates an icon using an icon resource on Windows.
 //       SetIcon(wxICON(app_icon));
	//#else
		SetIcon(wxArtProvider::GetIcon("app_icon", wxART_OTHER));
//	#endif

//	//acceleration keys table
//    wxAcceleratorEntry entries[1];
//    entries[0].Set(wxACCEL_CTRL, WXK_F1, wxID_ABOUT);
//    wxAcceleratorTable accel(1, entries);
//    SetAcceleratorTable(accel);

    create_metronome();
    m_appScope.initialize_help(this);

    //initialize printing data
    m_pPrintData = LENMUS_NEW wxPrintData;
    m_pPrintData->SetPaperId(wxPAPER_A4);    //default

    m_pPageSetupData = LENMUS_NEW wxPageSetupDialogData;
    *m_pPageSetupData = *m_pPrintData;
    // Set some initial page margins in mm.
    m_pPageSetupData->SetMarginTopLeft(wxPoint(0, 0));
    m_pPageSetupData->SetMarginBottomRight(wxPoint(0, 0));

    //start timer for carets
//    m_caretTimer.Start(m_nblinkTime, wxTIMER_CONTINUOUS);
}

//---------------------------------------------------------------------------------------
MainFrame::~MainFrame()
{
//    m_caretTimer.Stop();

    //ensure no score is being playedback
    ScorePlayer* pPlayer = m_appScope.get_score_player();
    if (pPlayer->is_playing())
        pPlayer->quit();

    //ensure the spacing dialog is destroyed
    if (m_pSpacingParamsDlg)
        m_pSpacingParamsDlg->Destroy();

    //ensure the metronome dialog is destroyed
    if (m_pMetronomeDlg)
        m_pMetronomeDlg->Destroy();

    // deinitialize the layout manager
    m_layoutManager.UnInit();

    delete m_pPrintData;
    delete m_pPageSetupData;

    //delete document windows data
    m_docWindows.clear();
    vector<ToolBoxConfiguration*>::iterator it;
    for (it=m_toolsCfg.begin(); it != m_toolsCfg.end(); ++it)
        delete *it;
    m_toolsCfg.clear();

}

//---------------------------------------------------------------------------------------
void MainFrame::create_metronome()
{
	// create main metronome and associate it to frame metronome controls
    //metronome speed. Default MM=60

    long nMM = 60L;
    wxConfigBase* pPrefs = m_appScope.get_preferences();
    if (pPrefs)
        nMM = pPrefs->Read("/Metronome/MM", 60L);
    m_pMainMtr = LENMUS_NEW GlobalMetronome(m_appScope, nMM);
    m_pMtr = m_pMainMtr;

    //inform Lomse, to use global metronome in ScorePlayerCtrl
    LomseDoorway& lomse = m_appScope.get_lomse();
    lomse.set_global_metronome_and_replace_local(m_pMainMtr);

    //store in ApplicationScope, to be used in dynamic exercises (EBookCtrol objects)
    m_appScope.set_metronome(m_pMainMtr);
}

//---------------------------------------------------------------------------------------
void MainFrame::save_preferences()
{
    //AWARE: MainFrame destructor is invoked from TheApp destructor, after
    //deletion of Preferences and Paths objects. Therefore, DO NOT invoke this
    //from MainFrame destructor: it will not work.
    //This method is invoked from MainFrame::quit() method when going to destroy
    //the mainframe

    //wxLogMessage("[MainFrame::save_preferences] Going to save preferences");

    wxConfigBase* pPrefs = m_appScope.get_preferences();
    if (pPrefs)
    {
//        lmDocManager* pDocManager = this->GetDocumentManager();
//        //save the last selected directories
//        g_pPaths->SetScoresPath( pDocManager->GetLastDirectory() );
//        pDocManager->SaveRecentFiles();

        Paths* pPaths = m_appScope.get_paths();
        LOMSE_LOG_INFO("Saving preferences at '%s'",
                       pPaths->GetConfigPath().ToStdString().c_str() );

        // save the frame size and position
        wxSize wndSize = GetSize();
        wxPoint wndPos = GetPosition();
        bool fMaximized = IsMaximized();
        pPrefs->Write("/MainFrame/Width", wndSize.GetWidth());
        pPrefs->Write("/MainFrame/Height", wndSize.GetHeight());
        pPrefs->Write("/MainFrame/Left", wndPos.x );
        pPrefs->Write("/MainFrame/Top", wndPos.y );
        pPrefs->Write("/MainFrame/Maximized", fMaximized);

        //save metronome settings and delete main metronome
        if (m_pMainMtr) {
            pPrefs->Write("/Metronome/MM", m_pMainMtr->get_mm() );
            delete m_pMainMtr;
        }

        //save file history
        m_fileHistory.Save(*pPrefs);

        //force to write on disk. This shouldn't be necessary as, according to
        //documentation, wxConfig object does it at destructor. But I was having
        //problems in Linux and this solved it.
        pPrefs->Flush();
   }
}

//---------------------------------------------------------------------------------------
void MainFrame::load_global_options()
{
    wxConfigBase* pPrefs = m_appScope.get_preferences();

    double width;
    pPrefs->Read("/Playback/TempoLineWidth", &width, 1.5);
    m_appScope.set_tempo_line_width( LUnits(width * 100.0) );

    int mode = 0;
    bool fValue;
    pPrefs->Read("/Playback/HighlightMode", &fValue, true);
    if (fValue)
        mode |= k_tracking_highlight_notes;
    pPrefs->Read("/Playback/TempoLineMode", &fValue, false);
    if (fValue)
        mode |= k_tracking_tempo_line;
    m_appScope.set_visual_tracking_mode(mode);
}

//---------------------------------------------------------------------------------------
void MainFrame::create_menu()
{
    //Centralized code to create the menu bar.
    //bitmaps on menus are supported only on Windows and GTK+

    wxSize nIconSize(16, 16);
    wxConfigBase* pPrefs = m_appScope.get_preferences();

    // file menu --------------------------------------------------------------------------

    wxMenu* pMenuFile = LENMUS_NEW wxMenu;
    create_menu_item(pMenuFile, k_menu_file_open, k_cmd_file_open,
                     _("Open"), "tool_open", wxITEM_NORMAL, true /*dots*/);
    create_menu_item(pMenuFile, k_menu_file_reload, k_cmd_file_reload,
                     _("Reload"), "", wxITEM_NORMAL);
    create_menu_item(pMenuFile, k_menu_open_books, k_cmd_open_books,
                     _("Open books"), "tool_open_ebook", wxITEM_NORMAL, true /*dots*/);
    create_menu_item(pMenuFile, k_menu_file_close, k_cmd_file_close,
                     _("Close"), "", wxITEM_NORMAL);
    create_menu_item(pMenuFile, k_menu_file_close_all, k_cmd_file_close_all,
                     _("Close all"), "", wxITEM_NORMAL);
    pMenuFile->AppendSeparator();

    create_menu_item(pMenuFile, k_menu_print, k_cmd_print,
                     _("Print"), "tool_print", wxITEM_NORMAL, true /*dots*/);
    create_menu_item(pMenuFile, wxID_PRINT_SETUP, k_cmd_print_setup,
                     _("Print Setup"), "", wxITEM_NORMAL, true /*dots*/);
    pMenuFile->AppendSeparator();

    create_menu_item(pMenuFile, wxID_EXIT, k_cmd_exit_app,
                     _("Quit"), "tool_exit", wxITEM_NORMAL);


    // history of files visited.
    m_fileHistory.UseMenu(pMenuFile);
    if (pPrefs)
        m_fileHistory.Load(*pPrefs);


    // View menu -------------------------------------------------------------------

    wxMenu* pMenuView = LENMUS_NEW wxMenu;
    create_menu_item(pMenuView, k_menu_view_toc, _("Table of content"),
                _("Hide/show the TOC for current book"), wxITEM_CHECK);
    pMenuView->AppendSeparator();
    create_menu_item(pMenuView, k_menu_view_welcome_page, _("Welcome page"),
                _("Hide/show welcome page"));

    // debug menu --------------------------------------------------------------------

    // Debug strings will not be translatable. It is mandatory that all development is
    // in English
    m_dbgMenu = nullptr;

    LOMSE_LOG_INFO("LENMUS_DEBUG_BUILD = %d", LENMUS_DEBUG_BUILD);
    LOMSE_LOG_INFO("LENMUS_RELEASE_INSTALL = %d", LENMUS_RELEASE_INSTALL);

#if (LENMUS_DEBUG_BUILD == 1 || LENMUS_RELEASE_INSTALL == 0)
    m_dbgMenu = LENMUS_NEW wxMenu;

    create_menu_item(m_dbgMenu, k_menu_debug_do_tests, "Run unit tests");
    create_menu_item(m_dbgMenu, k_menu_debug_see_paths, "See paths" );
    create_menu_item(m_dbgMenu, k_menu_debug_test_api, "Run low level API test");

    m_dbgMenu->AppendSeparator();   //Spacing and justification -------------------------
    create_menu_item(m_dbgMenu, k_menu_debug_justify_systems, "Justify systems",
                    "", wxITEM_CHECK);
    create_menu_item(m_dbgMenu, k_menu_debug_spacing_parameters, "Spacing parameters",
                    "", wxITEM_CHECK);
    create_menu_item(m_dbgMenu, k_menu_see_spacing_data, "See spacing data" );
    create_menu_item(m_dbgMenu, k_menu_debug_dump_column_tables, "Trace column tables",
                    "", wxITEM_CHECK);
    create_menu_item(m_dbgMenu, k_menu_debug_trace_lines_break, "Trace lines breaker" );

    m_dbgMenu->AppendSeparator();   //Miscellaneous -------------------------------------
    create_menu_item(m_dbgMenu, k_menu_debug_force_release_behaviour, "Release Behaviour",
        "Force release behaviour for certain functions", wxITEM_CHECK);
    create_menu_item(m_dbgMenu, k_menu_debug_print_preview, "Print Preview",
                    "", wxITEM_NORMAL);

    m_dbgMenu->AppendSeparator();   //Draw marks ----------------------------------------
    create_menu_item(m_dbgMenu, k_menu_debug_show_debug_links, "Include debug links",
        "Include debug controls in exercises", wxITEM_CHECK);
    create_menu_item(m_dbgMenu, k_menu_debug_draw_shape_bounds, "Draw shape bounds",
        "Force to draw bound rectangles around selected shapes", wxITEM_CHECK);

    //-- Draw bounds submenu --
    wxMenu* pSubmenuDrawBox = LENMUS_NEW wxMenu;

    create_menu_item(pSubmenuDrawBox, k_menu_debug_draw_box_document, "Draw document box");
    create_menu_item(pSubmenuDrawBox, k_menu_debug_draw_box_docpage, "Draw document page box");
    create_menu_item(pSubmenuDrawBox, k_menu_debug_draw_box_content, "Draw page content box");
    create_menu_item(pSubmenuDrawBox, k_menu_debug_draw_box_score_page, "Draw score page box");
    create_menu_item(pSubmenuDrawBox, k_menu_debug_draw_box_system, "Draw system box");
    create_menu_item(pSubmenuDrawBox, k_menu_debug_draw_box_slice, "Draw slice box");
    create_menu_item(pSubmenuDrawBox, k_menu_debug_draw_box_slice_instr, "Draw slice intrs box");
    create_menu_item(pSubmenuDrawBox, k_menu_debug_draw_box_inline, "Draw inline boxes");
    create_menu_item(pSubmenuDrawBox, k_menu_debug_draw_box_link, "Draw link boxes");
    create_menu_item(pSubmenuDrawBox, k_menu_debug_draw_box_paragraph, "Draw paragraph boxes");
    create_menu_item(pSubmenuDrawBox, k_menu_debug_draw_box_control, "Draw control boxes");
    create_menu_item(pSubmenuDrawBox, k_menu_debug_draw_box_table, "Draw table boxes");
    create_menu_item(pSubmenuDrawBox, k_menu_debug_draw_box_table_rows, "Draw table row boxes");
    create_menu_item(pSubmenuDrawBox, k_menu_debug_remove_boxes, "Remove drawn boxes");

    wxMenuItem* pItem = LENMUS_NEW wxMenuItem(m_dbgMenu, k_menu_debug_draw_box, "Draw box ...",
                    "Force to draw box rectangles", wxITEM_NORMAL, pSubmenuDrawBox);
    m_dbgMenu->Append(pItem);

    create_menu_item(m_dbgMenu, k_menu_debug_draw_anchor_objects, "Draw anchor objects",
        "Draw a red line to show anchor objects", wxITEM_CHECK);
    create_menu_item(m_dbgMenu, k_menu_debug_draw_anchor_lines, "Draw anchor lines",
        "Draw a red line to show anchor line position", wxITEM_CHECK);
    create_menu_item(m_dbgMenu, k_menu_debug_draw_slur_ctrol_points, "Draw slur ctrol. points",
        "Draw control points and reference points for slurs", wxITEM_CHECK);
    create_menu_item(m_dbgMenu, k_menu_debug_draw_vertical_profile, "Draw vertical profile",
        "Draw the vertical profile for staves", wxITEM_CHECK);
    m_dbgMenu->AppendSeparator();   //dump tables ---------------------------------------
    create_menu_item(m_dbgMenu, k_menu_debug_see_staffobjs, "See staffobjs table" );
    create_menu_item(m_dbgMenu, k_menu_debug_see_document_ids, "See document ids" );
    create_menu_item(m_dbgMenu, k_menu_debug_see_midi_events, "See MIDI events" );
	create_menu_item(m_dbgMenu, k_menu_debug_dump_gmodel, "See graphical model" );
	create_menu_item(m_dbgMenu, k_menu_debug_dump_imodel, "See internal model" );
    create_menu_item(m_dbgMenu, k_menu_debug_see_cursor_state, "See cursor state");

    m_dbgMenu->AppendSeparator();   //exporters -----------------------------------------
    create_menu_item(m_dbgMenu, k_menu_see_ldp_source, "See LDP source" );
    create_menu_item(m_dbgMenu, k_menu_see_checkpoint_data, "See checkpoint data" );
    create_menu_item(m_dbgMenu, k_menu_see_lmd_source, "See LMD source" );
    create_menu_item(m_dbgMenu, k_menu_see_mnx_source, "See MNX source" );
    create_menu_item(m_dbgMenu, k_menu_debug_see_musicxml, "See XML" );
#endif


    // Zoom menu -----------------------------------------------------------------------

//    m_zoomMenu->Append(k_menu_zoom_out);
//
    wxMenu* pMenuZoom = LENMUS_NEW wxMenu;
    create_menu_item(pMenuZoom, k_menu_zoom_100, _("Actual size"),
                "Zoom to real print size", wxITEM_NORMAL, "tool_zoom_actual");
    create_menu_item(pMenuZoom, k_menu_zoom_fit_full, _("Fit page full"),
                _("Zoom so that the full page is displayed"), wxITEM_NORMAL,
                "tool_zoom_fit_full");
    create_menu_item(pMenuZoom, k_menu_zoom_fit_width, _("Fit page width"),
                _("Zoom so that page width equals window width"), wxITEM_NORMAL,
                "tool_zoom_fit_width");
    create_menu_item(pMenuZoom, k_menu_zoom_in, "Zoom in\tCtrl++",
                _("Enlarge image size"), wxITEM_NORMAL, "tool_zoom_in");
    create_menu_item(pMenuZoom, k_menu_zoom_out, "Zoom out\tCtrl+-",
                _("Reduce image size"), wxITEM_NORMAL, "tool_zoom_out");
    create_menu_item(pMenuZoom, k_menu_zoom_other, _("Zoom to ..."));


    //Sound menu -------------------------------------------------------------------------

    wxMenu* pMenuSound = LENMUS_NEW wxMenu;

    create_menu_item(pMenuSound, k_menu_play_start, _("play"),
                _("Start/resume play back. From selection of full score"), wxITEM_NORMAL,
                "tool_play");
    create_menu_item(pMenuSound, k_menu_play_stop, _("Stop"),
                _("Stop playing back"), wxITEM_NORMAL, "tool_stop");
    create_menu_item(pMenuSound, k_menu_play_pause, _("Pause"),
                _("Pause playing back"), wxITEM_NORMAL, "tool_pause");
    create_menu_item(pMenuSound, k_menu_play_countoff, _("Do count off"),
                    _("Do count off before starting the play back"), wxITEM_CHECK);
    pMenuSound->AppendSeparator();

    create_menu_item(pMenuSound, k_menu_sound_test, _("Test sound"),
                    _("play an scale to test sound"), wxITEM_NORMAL, "tool_test_sound");
    create_menu_item(pMenuSound, k_menu_sound_off, _("All sounds off"),
                    _("Stop inmediatly all sounds"), wxITEM_NORMAL, "tool_stop_sounds");


    // Options menu ---------------------------------------------------------------------

    wxMenu* pMenuOptions = LENMUS_NEW wxMenu;
    create_menu_item(pMenuOptions, k_menu_preferences,  _("Preferences"),
                    _("Open preferences dialog"), wxITEM_NORMAL, "tool_options");

    // Tools menu ---------------------------------------------------------------------

    wxMenu* pMenuTools = LENMUS_NEW wxMenu;
    create_menu_item(pMenuTools, k_menu_tools_metronome,  _("Metronome"),
                    _("Open metronome tool"), wxITEM_NORMAL, "tool_metronome");


    // Help menu -------------------------------------------------------------------------

    wxMenu* pMenuHelp = LENMUS_NEW wxMenu;

    bool fInclude = m_appScope.are_experimental_features_enabled();
    if (fInclude)
    {
        create_menu_item(pMenuHelp, k_menu_help_users_guide,  _("LenMus manual"),
                    _("Open user's manual"), wxITEM_NORMAL, "tool_help", "\tF1");
        create_menu_item(pMenuHelp, k_menu_help_editor_quick,  _("Editor quick guide"),
                    _("Show editor quick guide"), wxITEM_NORMAL, "tool_quick_guide", "\tCtrl+H");
    }
    create_menu_item(pMenuHelp, k_menu_help_study_guide,  _("Study guide"),
                _("How to use LenMus in your studies"), wxITEM_NORMAL, "tool_study_guide");
    if (fInclude)
        create_menu_item(pMenuHelp, k_menu_help_search,  _("Search help"),
                    _("Search help documentation"), wxITEM_NORMAL, "tool_search_help");
    pMenuHelp->AppendSeparator();
	create_menu_item(pMenuHelp, k_menu_check_for_updates, _("Check now for updates"),
				_("Connect to the Internet and check for program updates"), wxITEM_NORMAL,
                "tool_web_update");
    create_menu_item(pMenuHelp, k_menu_help_visit_website,  _("Visit LenMus website"),
                _("Open the Internet browser and go to LenMus website"), wxITEM_NORMAL,
                "tool_website");
    pMenuHelp->AppendSeparator();
    create_menu_item(pMenuHelp, k_menu_help_about, _("About"),
				_("Display information about program version and credits"), wxITEM_NORMAL,
                "tool_about");


    // set up the menubar ---------------------------------------------------------------

    // AWARE: As MainFrame is derived from wxFrame, in MSWindows the menu
    // bar automatically inherits a "Window" menu inserted in the second last position.
    // To suppress it (under MSWindows) it is necessary to add style
    // wxFRAME_NO_WINDOW_MENU in frame creation.
    wxMenuBar* pMenuBar = LENMUS_NEW wxMenuBar;
    pMenuBar->Append(pMenuFile, _("File"));
    pMenuBar->Append(pMenuView, _("View"));
    pMenuBar->Append(pMenuSound, _("Sound"));

#if (LENMUS_DEBUG_BUILD == 1 || LENMUS_RELEASE_INSTALL == 0)
    pMenuBar->Append(m_dbgMenu, "Debug");     //DO NOT TRANSLATE
#endif

    pMenuBar->Append(pMenuZoom, _("Zoom"));
    pMenuBar->Append(pMenuOptions, _("Options"));
    pMenuBar->Append(pMenuTools, _("Tools"));
    pMenuBar->Append(pMenuHelp, _("Help"));

        //
        // items initially checked, not controlled by update_UI events
        //

    //debug toolbar
#if (LENMUS_DEBUG_BUILD == 1 || LENMUS_RELEASE_INSTALL == 0)
    pMenuBar->Check(k_menu_debug_force_release_behaviour,
                    m_appScope.is_release_behaviour());
    pMenuBar->Check(k_menu_debug_show_debug_links, m_appScope.show_debug_links());
#endif

    // do count off
    pMenuBar->Check(k_menu_play_countoff, true);


#ifdef __WXMAC__
    wxMenuBar::MacSetCommonMenuBar(pMenuBar);
#endif
    SetMenuBar(pMenuBar);
}

//---------------------------------------------------------------------------------------
void MainFrame::create_controls()
{
    m_layoutManager.SetManagedWindow(this);     //inform wxAUI which frame to use

    //create the ContentWindow (main pane, a notebook) for scores and other content
    long style = wxAUI_NB_CLOSE_ON_ACTIVE_TAB |     //put close button on the active tab
                 wxAUI_NB_WINDOWLIST_BUTTON |       //drop-down list of tabs
                 wxAUI_NB_SCROLL_BUTTONS |          //scroll buttons for tabs
                 wxAUI_NB_TAB_MOVE ;                //tabs can be dragged horizontally

    m_pContentWindow = LENMUS_NEW ContentWindow(this, style);

    m_layoutManager.AddPane(m_pContentWindow, wxAuiPaneInfo().Name("MainPane").
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
        pPlayer->quit();

    save_preferences();

    bool fTry = true;
    while (fTry)
        fTry = close_active_document_window();

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
    wxConfigBase* pPrefs = m_appScope.get_preferences();
    wxString lastUsedPath = pPrefs->Read("/User/LastUsedPath", "");
    if (lastUsedPath.empty())
    {
        Paths* pPaths = m_appScope.get_paths();
        lastUsedPath = pPaths->GetSamplesPath();
    }

    // ask for the file to open/import
    wxString sFilter = "All supported files";
    if (m_appScope.are_experimental_features_enabled())
    {
        sFilter += "|*.lms;*.lmb;*.lmd;*.xml;*.mxl;*.musicxml;*.mnx|";
        sFilter += _("LenMus files");
        sFilter += "|*.lms;*.lmb;*.lmd|";
        sFilter += _("MusicXML files");
        sFilter += "|*.xml;*.mxl;*.musicxml|";
        sFilter += _("MNX files");
        sFilter += "|*.mnx";
    }
    else
    {
        sFilter += "|*.lms;*.lmb;*.lmd;*.xml;*.mxl;*.musicxml|";
        sFilter += _("LenMus files");
        sFilter += "|*.lms;*.lmb;*.lmd|";
        sFilter += _("MusicXML files");
        sFilter += "|*.xml;*.mxl;*.musicxml";
    }
    wxString sFile = ::wxFileSelector(_("Choose the file to open"),
                                      lastUsedPath,   //default path
                                      "",        //default filename
                                      "",        //default_extension
                                      sFilter,
                                      wxFD_OPEN,      //flags
                                      this);

    if (!sFile.empty())
    {
        wxFileName fn(sFile);
        pPrefs->Write("/User/LastUsedPath", fn.GetPath());

        string filename( sFile.mb_str(wxConvUTF8) );
        load_file(filename);
        m_fileHistory.AddFileToHistory(sFile);
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::on_open_book(wxCommandEvent& event)
{
    wxString sFullpath = event.GetString();
    string filename( to_std_string(sFullpath) );
    load_file(filename);
    m_fileHistory.AddFileToHistory(sFullpath);
}

//---------------------------------------------------------------------------------------
void MainFrame::on_file_reload(wxCommandEvent& WXUNUSED(event))
{
    DocumentWindow* pWnd = dynamic_cast<DocumentWindow*>( get_active_canvas() );
    if (pWnd)
        pWnd->display_document(to_std_string(pWnd->get_full_filename()), VIEW_TYPE);
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
    loader.create_canvas(filename, VIEW_TYPE);
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
        case k_tracking_event:
        {
            if (pCanvas)
            {
                LOMSE_LOG_DEBUG(Logger::k_score_player, "Visual tracking event");
                SpEventVisualTracking pEv(
                    static_pointer_cast<EventVisualTracking>(pEvent) );
                lmVisualTrackingEvent event(pEv);
                ::wxPostEvent(pCanvas, event);
            }
            break;
        }

        case k_update_viewport_event:
        {
            if (pCanvas)
            {
                LOMSE_LOG_DEBUG(Logger::k_score_player, "Update viewport event");
                SpEventUpdateViewport pEv(
                    static_pointer_cast<EventUpdateViewport>(pEvent) );
                lmUpdateViewportEvent event(pEv);
                ::wxPostEvent(pCanvas, event);
            }
            break;
        }

        case k_end_of_playback_event:
        {
            if (pCanvas)
            {
                SpEventEndOfPlayback pEv( static_pointer_cast<EventEndOfPlayback>(pEvent) );
                lmEndOfPlaybackEvent event(pEv);
                ::wxPostEvent(pCanvas, event);
            }
            break;
        }

        case k_link_clicked_event:
        {
            //AWARE: Only clicks on ImoLink objects arrive here (ImoLink objects
            //are only generated by <link> tags in LMD files). Hyperlink controls
            //for exercises are associated to ImoControl objects and their events
            //must be handled by registering at the Document.
            DocumentFrame* pFrame = get_active_document_frame();
            if (pFrame)
                pFrame->on_hyperlink_event(pEvent);
            break;
        }

        case k_show_contextual_menu_event:
        {
            if (pCanvas)
            {
                SpEventMouse pEv( static_pointer_cast<EventMouse>(pEvent) );
                lmShowContextualMenuEvent event(pEv);
                ::wxPostEvent(pCanvas, event);
            }
            break;
        }

        case k_selection_set_change:
        case k_pointed_object_change:
        {
            if (is_toolbox_visible())
            {
                SpEventUpdateUI pEv( static_pointer_cast<EventUpdateUI>(pEvent) );
                lmUpdateUIEvent event(pEv);
                ::wxPostEvent(m_pToolBox, event);
            }
            break;
        }

        default:
            wxString msg = wxString::Format("[MainFrame::on_lomse_event] Event %d received from Lomse",
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
            LOMSE_LOG_ERROR("Unknown request %d", type);
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::generate_dynamic_content(RequestDynamic* pRD)
{
    AObject obj = pRD->get_object();
    if (obj.is_dynamic())
    {
        ADynamic dyn = obj.downcast_to_dynamic();
        string& classid = dyn.classid();
        ADocument doc = pRD->get_document();
        Canvas* pCanvas = get_active_canvas();
        DocumentWindow* pWnd = nullptr;
        DocumentFrame* pFrame = dynamic_cast<DocumentFrame*>( pCanvas );
        if (pFrame)
            pWnd = pFrame->get_document_window();
        else
            pWnd = dynamic_cast<DocumentCanvas*>( pCanvas );

        DynControl* pControl
            = DynControlFactory::create_dyncontrol(m_appScope, classid, pWnd);
        pControl->generate_content(dyn, doc);
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::get_font_filename(RequestFont* pRequest)
{
    //This is just a trivial example. In real applications you should
    //use operating system services to find a suitable font

    //notes on parameters received:
    // - fontname can be either the face name (i.e. "Book Antiqua") or
    //   the familly name (i.e. "Liberation sans")

#if (LENMUS_PLATFORM_UNIX == 1 || LENMUS_PLATFORM_MAC == 1)

    //method get_font_filename() is not invoked in Linux

#elif (LENMUS_PLATFORM_WIN32 == 1)

    const string& fontname = pRequest->get_fontname();
    bool bold = pRequest->get_bold();
    bool italic = pRequest->get_italic();

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

//---------------------------------------------------------------------------------------
void MainFrame::create_menu_item(wxMenu* pMenu, int nId, const wxString& sItemName,
                                 const wxString& sToolTip, wxItemKind nKind,
                                 const wxString& sIconName,
                                 const wxString& sShortcut)
{
    //Create a menu item and add it to the received menu

    wxString name = sItemName + sShortcut;
    wxMenuItem* pItem = LENMUS_NEW wxMenuItem(pMenu, nId, name, sToolTip, nKind);


    //icons are supported only in Windows and Linux, and only in wxITEM_NORMAL items
    #if (LENMUS_PLATFORM_WIN32 == 1 || LENMUS_PLATFORM_UNIX == 1 || LENMUS_PLATFORM_MAC == 1)
    if (nKind == wxITEM_NORMAL && sIconName != "")
        pItem->SetBitmap( wxArtProvider::GetBitmap(sIconName, wxART_TOOLBAR, wxSize(16, 16)) );
    #endif

    pMenu->Append(pItem);
}

//---------------------------------------------------------------------------------------
void MainFrame::create_menu_item(wxMenu* pMenu, int menuId, int actionId,
                                 const wxString& sLabel, const wxString& sIconName,
                                 wxItemKind nKind, bool dots)
{
    //Create a menu item and add it to the received menu

    KeyTranslator* table = m_appScope.get_key_translator();
    AppAction* a = table->find(actionId);
    if (a)
    {
        wxString name = sLabel;
        if (dots)
            name += " ...";
        if (a->has_shortcut())
            name += "\t" + a->shortcut_as_string();
        wxMenuItem* pItem = LENMUS_NEW wxMenuItem(pMenu, menuId, name, a->description(), nKind);


        //icons are supported only in Windows and Linux, and only in wxITEM_NORMAL items
        #if (LENMUS_PLATFORM_WIN32 == 1 || LENMUS_PLATFORM_UNIX == 1 || LENMUS_PLATFORM_MAC == 1)
        if (nKind == wxITEM_NORMAL && !sIconName.empty())
            pItem->SetBitmap( wxArtProvider::GetBitmap(sIconName, wxART_TOOLBAR, wxSize(16, 16)) );
        #endif

        pMenu->Append(pItem);
    }
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
void MainFrame::on_tab_close(wxAuiManagerEvent& evt)
{
    if (evt.pane->caption == _("Counters & options"))
    {
        int res = wxMessageBox("Are you sure you want to close/hide this pane?",
                               "wxAUI",
                               wxYES_NO,
                               this);
        if (res != wxYES)
            evt.Veto();
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::on_active_canvas_changing(wxAuiNotebookEvent& WXUNUSED(event))
{
    //The active canvas is about to be changed. This event can be vetoed.

    //save ToolBox state for document to be de-activated
    DocumentWindow* pWnd = get_active_document_window();
    if (pWnd)
        save_toolbox_for(pWnd);
}

//---------------------------------------------------------------------------------------
void MainFrame::save_toolbox_for(DocumentWindow* pWnd)
{
    if (m_pToolBox)
    {
        ToolBoxConfiguration* pCfg = get_toolbox_cfg_for(pWnd);
        m_pToolBox->save_configuration(pCfg);
    }
}

//---------------------------------------------------------------------------------------
ToolBoxConfiguration* MainFrame::get_toolbox_cfg_for(DocumentWindow* pWnd)
{
    int i=0;
    vector<DocumentWindow*>::iterator it;
    for (it=m_docWindows.begin(); it != m_docWindows.end(); ++it, ++i)
    {
        if (*it == pWnd)
            return m_toolsCfg[i];
    }

    //not found. Create
    m_docWindows.push_back(pWnd);
    ToolBoxConfiguration* pCfg = LENMUS_NEW ToolBoxConfiguration();
    m_toolsCfg.push_back(pCfg);
    return pCfg;
}

//---------------------------------------------------------------------------------------
void MainFrame::restore_toolbox_for(DocumentWindow* pWnd)
{
    if (pWnd && m_pToolBox)
    {
        int i=0;
        vector<DocumentWindow*>::iterator it;
        for (it=m_docWindows.begin(); it != m_docWindows.end(); ++it, ++i)
        {
            if (*it == pWnd)
            {
                m_pToolBox->load_configuration( *m_toolsCfg[i] );
                return;
            }
        }
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::on_view_virtual_keyboard(wxCommandEvent& WXUNUSED(event))
{
    show_virtual_keyboard();
}

//---------------------------------------------------------------------------------------
VirtualKeyboard* MainFrame::create_virtual_keyboard()
{
    return LENMUS_NEW VirtualKeyboard(this, wxID_ANY);
}

//---------------------------------------------------------------------------------------
void MainFrame::hide_virtual_keyboard()
{
    //if added to AUI manager hide console
    wxAuiPaneInfo panel = m_layoutManager.GetPane("Virtual keyboard");
    if (panel.IsOk())
    {
        m_layoutManager.GetPane("Virtual keyboard").Show(false);
        m_layoutManager.Update();
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::show_virtual_keyboard()
{
    //if not yet created, do it
    if (!m_pVirtualKeyboard)
        m_pVirtualKeyboard = create_virtual_keyboard();

    //if not yet added to AUI manager do it now
    wxAuiPaneInfo panel = m_layoutManager.GetPane("Virtual keyboard");
    if (!panel.IsOk())
        m_layoutManager.AddPane(m_pVirtualKeyboard,
                                wxAuiPaneInfo().Name("Virtual keyboard").
                                    Caption(_("Virtual keyboard")).
                                    Bottom().Layer(1).
                                    TopDockable(true).LeftDockable(true).
                                    BottomDockable(true).RightDockable(true).
                                    Floatable(true).FloatingSize(wxSize(410, 200)).
                                    MinSize(wxSize(100,100)).
                                    Movable(true).Resizable(true).
                                    CloseButton(true).MaximizeButton(false));

    //show console
    m_layoutManager.GetPane("Virtual keyboard").Show(true);
    m_layoutManager.Update();
    m_pVirtualKeyboard->SetFocus();
}

////---------------------------------------------------------------------------------------
//void MainFrame::on_create_counters_panel(wxCommandEvent& WXUNUSED(event))
//{
//    ExerciseCtrol* pExerciseCtrol = nullptr;
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
//    ExerciseCtrol* pExerciseCtrol = nullptr;
//    ExerciseOptions* pConstrains = nullptr;
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

//---------------------------------------------------------------------------------------
void MainFrame::show_welcome_window()
{
    if (!is_welcome_page_displayed())
    {
        //m_pWelcomeWnd = LENMUS_NEW WelcomeWindow(m_pContentWindow, m_appScope, &m_fileHistory, wxNewId());
        m_pWelcomeWnd = LENMUS_NEW WelcomeWindow(m_pContentWindow, m_appScope, wxNewId());
        m_pContentWindow->add_canvas(m_pWelcomeWnd, _("Start page"));
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::on_open_recent_file(wxCommandEvent &event)
{
    int iFile = event.GetId() - wxID_FILE1;
    wxString sFile = m_fileHistory.GetHistoryFile(iFile);
    if (wxFile::Exists(sFile))
        load_file( to_std_string(sFile) );
    else
    {
        wxMessageBox(wxString::Format(_("File '%s' not found."), sFile.wx_str()),
                     _("Error message"), wxOK || wxICON_HAND );
        m_fileHistory.RemoveFileFromHistory(iFile);
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::on_visit_website(wxCommandEvent& WXUNUSED(event))
{
    ::wxLaunchDefaultBrowser( LENMUS_VENDOR_SITE );
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

//---------------------------------------------------------------------------------------
void MainFrame::on_open_help(wxCommandEvent& event)
{
    HelpSystem* pHelp = m_appScope.get_help_controller();
    switch (event.GetId())
    {
        case k_menu_help_editor_quick:
            pHelp->display_section(10300);  //general-rules
            break;
        case k_menu_help_study_guide:
        {
            //pHelp->display_section(10101);  //study-guide
            Paths* pPaths = m_appScope.get_paths();
            wxString sPath = pPaths->GetLocalePath();
            wxFileName oFile(sPath, "study-guide.htm", wxPATH_NATIVE);
            if (!oFile.FileExists())
            {
                //try to display the english version
                sPath = pPaths->GetLocaleRootPath();
                oFile.AssignDir(sPath);
                oFile.AppendDir("en");
                oFile.SetFullName("study-guide.htm");
                if (!oFile.FileExists())
                {
                    wxMessageBox(_("Sorry: File not found!"));
                    wxLogMessage("[MainFrame::on_open_help] File %s' not found!",
                                 oFile.GetFullPath().wx_str() );
                    return;
                }
            }
            ::wxLaunchDefaultBrowser( oFile.GetFullPath() );
            break;
        }
        case k_menu_help_users_guide:
            pHelp->display_section(10000);  //index
            break;

        case k_menu_help_search:
            pHelp->display_section(3);      //search
            break;

        default:
            pHelp->display_section(10000);  //index
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::on_open_books(wxCommandEvent& WXUNUSED(event))
{
    BooksDlg dlg(this, m_appScope);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxString filename = dlg.get_full_path();
        wxCommandEvent myEvent(LM_EVT_OPEN_BOOK, k_id_open_book);
        myEvent.SetEventObject(this);
        myEvent.SetString(filename);
        ::wxPostEvent(this, myEvent);
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::on_metronome_tool(wxCommandEvent& WXUNUSED(event))
{
    if (!m_pMetronomeDlg)
        m_pMetronomeDlg = new DlgMetronome(m_appScope, this, m_pMtr);

    m_pMetronomeDlg->load_current_values();
    m_pMetronomeDlg->Show();
}


//=======================================================================================
//methods only available in debug version
//=======================================================================================

#if (LENMUS_DEBUG_BUILD == 1 || LENMUS_RELEASE_INSTALL == 0)

//---------------------------------------------------------------------------------------
void MainFrame::on_do_tests(wxCommandEvent& WXUNUSED(event))
{
    #if (LENMUS_ENABLE_UNIT_TESTS == 1)
        MyTestRunner oTR(this);
        oTR.RunTests();
    #else
        wxMessageBox("Unit tests are no included in the build!");
    #endif
}

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_draw_box(wxCommandEvent& event)
{
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
    {
        switch (event.GetId())
        {
            case k_menu_debug_draw_box_document:
                pCanvas->set_debug_draw_box(GmoObj::k_box_document);
                break;
            case k_menu_debug_draw_box_docpage:
                pCanvas->set_debug_draw_box(GmoObj::k_box_doc_page);
                break;
            case k_menu_debug_draw_box_content:
                pCanvas->set_debug_draw_box(GmoObj::k_box_doc_page_content);
                break;
            case k_menu_debug_draw_box_score_page:
                pCanvas->set_debug_draw_box(GmoObj::k_box_score_page);
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
            case k_menu_debug_draw_box_link:
                pCanvas->set_debug_draw_box(GmoObj::k_box_link);
                break;
            case k_menu_debug_draw_box_paragraph:
                pCanvas->set_debug_draw_box(GmoObj::k_box_paragraph);
                break;
            case k_menu_debug_draw_box_control:
                pCanvas->set_debug_draw_box(GmoObj::k_box_control);
                break;
            case k_menu_debug_draw_box_table:
                pCanvas->set_debug_draw_box(GmoObj::k_box_table);
                break;
            case k_menu_debug_draw_box_table_rows:
                pCanvas->set_debug_draw_box(GmoObj::k_box_table_rows);
                break;

            case k_menu_debug_remove_boxes:
                pCanvas->remove_drawn_boxes();
                break;

            default:
                pCanvas->set_debug_draw_box(GmoObj::k_box_paragraph);
                break;
        }
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_justify_systems(wxCommandEvent& WXUNUSED(event))
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
void MainFrame::on_debug_spacing_parameters(wxCommandEvent& event)
{
    bool fShow = GetMenuBar()->IsChecked(event.GetId());

    if (fShow)
    {
        if (!m_pSpacingParamsDlg)
        {
            LomseDoorway& lib = m_appScope.get_lomse();
            LibraryScope* pScope = lib.get_library_scope();
            float force = pScope->get_optimum_force();
            float alpha = pScope->get_spacing_alpha();
            float dmin =  pScope->get_spacing_dmin();

            m_pSpacingParamsDlg = LENMUS_NEW DlgSpacingParams(this, force, alpha, dmin);
        }

        m_pSpacingParamsDlg->Show(true);
    }
    else // hide
    {
        if (m_pSpacingParamsDlg)
            m_pSpacingParamsDlg->Hide();
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_trace_lines_break(wxCommandEvent& UNUSED(event))
{
    LomseDoorway& lib = m_appScope.get_lomse();
    LibraryScope* pScope = lib.get_library_scope();
    int level = k_trace_breaks_table            //dump of final breaks table
                | k_trace_breaks_computation    //trace computation of breaks
                | k_trace_breaks_penalties;     //trace penalties computation

    pScope->set_trace_level_for_lines_breaker(level);
}

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_dump_column_tables(wxCommandEvent& WXUNUSED(event))
{
    bool fChecked = m_dbgMenu->IsChecked(k_menu_debug_dump_column_tables);
    LomseDoorway& lib = m_appScope.get_lomse();
    LibraryScope* pScope = lib.get_library_scope();
    pScope->set_dump_column_tables(fChecked);

    //force to redraw current document
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
        pCanvas->on_document_updated();

    wxMessageBox("Tables are saved in file lomse-log.txt, in the same folder than this project");
}

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_see_spacing_data(wxCommandEvent& WXUNUSED(event))
{
    LomseDoorway& lib = m_appScope.get_lomse();
    LibraryScope* pScope = lib.get_library_scope();
    pScope->set_dump_column_tables(true);

    //force to redraw current document
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
        pCanvas->on_document_updated();

    wxMessageBox("Tables are saved in file lomse-log.txt, in the same folder than this project");
    pScope->set_dump_column_tables(false);
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

    DocumentFrame* pFrame = get_active_document_frame();
    if (pFrame)
        event.IsChecked() ? pFrame->hide_toc() : pFrame->show_toc();
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

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_draw_shape_bounds(wxCommandEvent& WXUNUSED(event))
{
    bool fChecked = m_dbgMenu->IsChecked(k_menu_debug_draw_shape_bounds);
    LomseDoorway& lib = m_appScope.get_lomse();
    LibraryScope* pScope = lib.get_library_scope();
    pScope->set_draw_shape_bounds(fChecked);
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
        pCanvas->on_document_updated();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_draw_anchor_objects(wxCommandEvent& WXUNUSED(event))
{
    bool fChecked = m_dbgMenu->IsChecked(k_menu_debug_draw_anchor_objects);
    LomseDoorway& lib = m_appScope.get_lomse();
    LibraryScope* pScope = lib.get_library_scope();
    pScope->set_draw_anchor_objecs(fChecked);
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
        pCanvas->on_document_updated();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_draw_anchor_lines(wxCommandEvent& WXUNUSED(event))
{
    bool fChecked = m_dbgMenu->IsChecked(k_menu_debug_draw_anchor_lines);
    LomseDoorway& lib = m_appScope.get_lomse();
    LibraryScope* pScope = lib.get_library_scope();
    pScope->set_draw_anchor_lines(fChecked);
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
        pCanvas->on_document_updated();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_draw_slur_ctrol_points(wxCommandEvent& WXUNUSED(event))
{
    bool fChecked = m_dbgMenu->IsChecked(k_menu_debug_draw_slur_ctrol_points);
    LomseDoorway& lib = m_appScope.get_lomse();
    LibraryScope* pScope = lib.get_library_scope();
    pScope->set_draw_slur_ctrol_points(fChecked);
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
        pCanvas->on_document_updated();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_draw_vertical_profile(wxCommandEvent& WXUNUSED(event))
{
    bool fChecked = m_dbgMenu->IsChecked(k_menu_debug_draw_vertical_profile);
    LomseDoorway& lib = m_appScope.get_lomse();
    LibraryScope* pScope = lib.get_library_scope();
    pScope->set_draw_vertical_profile(fChecked);
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
        pCanvas->on_document_updated();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_dump_gmodel(wxCommandEvent& WXUNUSED(event))
{
    Interactor* pInt = get_active_canvas_interactor();
    if (pInt)
    {
        GraphicModel* pGM = pInt->get_graphic_model();
        stringstream out;
        int pages = pGM->get_num_pages();
        for (int i=0; i < pages; ++i)
        {
            out << "Page " << i
                << " ==================================================================="
                << endl;
            pGM->dump_page(i, out);
        }
        DlgDebug dlg(this, "graphical model dump", to_wx_string(out.str()) );
        dlg.ShowModal();
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_dump_imodel(wxCommandEvent& WXUNUSED(event))
{
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
        pCanvas->debug_dump_internal_model();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_see_ldp_source(wxCommandEvent& WXUNUSED(event))
{
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
        pCanvas->debug_display_ldp_source();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_see_lmd_source(wxCommandEvent& WXUNUSED(event))
{
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
        pCanvas->debug_display_lmd_source();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_see_mnx_source(wxCommandEvent& WXUNUSED(event))
{
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
        pCanvas->debug_display_mnx_source();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_see_checkpoint_data(wxCommandEvent& WXUNUSED(event))
{
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
        pCanvas->debug_display_checkpoint_data();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_see_cursor_state(wxCommandEvent& WXUNUSED(event))
{
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
        pCanvas->debug_display_cursor_state();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_see_midi_events(wxCommandEvent& WXUNUSED(event))
{
    AScore score = get_active_score();
    if (score.is_valid())
    {
        ImoScore* pScore = score.internal_object();
        SoundEventsTable* pTable = pScore->get_midi_table();
        DlgDebug dlg(this, "MIDI events table", to_wx_string(pTable->dump_midi_events()) );
        dlg.ShowModal();
    }
    else
        wxMessageBox("No active score");
}

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_see_staffobjs(wxCommandEvent& WXUNUSED(event))
{
    AScore score = get_active_score();
    if (score.is_valid())
    {
        ImoScore* pScore = score.internal_object();
        ColStaffObjs* pTable = pScore->get_staffobjs_table();
        DlgDebug dlg(this, "MIDI events table", to_wx_string(pTable->dump()) );
        dlg.ShowModal();
    }
    else
        wxMessageBox("No active score");
}

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_see_document_ids(wxCommandEvent& WXUNUSED(event))
{
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
        pCanvas->debug_display_document_ids();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_see_paths(wxCommandEvent& WXUNUSED(event))
{
    Paths* pPaths = m_appScope.get_paths();
    DlgDebug dlg(this, "Paths", to_wx_string(pPaths->dump_paths()) );
    dlg.ShowModal();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_debug_test_api(wxCommandEvent& WXUNUSED(event))
{
//    load_file("/datos/cecilio/lm/projects/lomse/trunk/samples/greensleeves_v16.lms");
//    load_file("/datos/cecilio/lm/projects/lomse/trunk/test-scores/00136-clef-follows-note-when-note-displaced.lms");
    load_file("/datos/cecilio/lm/projects/lomse/trunk/test-scores/MusicXML/recordare/MozartTrio.musicxml");

    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
        pCanvas->debug_do_api_test();
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
            wxMessageBox("There was a problem previewing.\nPerhaps your current printer is not set correctly?",
                         "Previewing", wxOK);
            return;
        }

        wxPreviewFrame *frame = LENMUS_NEW wxPreviewFrame(preview, this, "Print Preview");
        frame->Centre(wxBOTH);
        frame->Initialize();
        frame->Show();
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::on_update_UI_debug(wxUpdateUIEvent &event)
{
    switch (event.GetId())
    {
		case k_menu_debug_justify_systems:
        {
            LomseDoorway& lib = m_appScope.get_lomse();
            LibraryScope* pScope = lib.get_library_scope();
            event.Check(pScope->justify_systems());
			break;
        }
        default:
            ;
    }
    event.Enable(true);
}

#endif   // END OF METHODS INCLUDED ONLY IN DEBUG BUILD ---------------------------------


//---------------------------------------------------------------------------------------
void MainFrame::update_spacing_params(float force, float alpha, float dmin)
{
    LomseDoorway& lib = m_appScope.get_lomse();
    LibraryScope* pScope = lib.get_library_scope();
    pScope->set_optimum_force(force);
    pScope->set_spacing_alpha(alpha);
    pScope->set_spacing_dmin(dmin);

    wxCommandEvent event;   //not used
    on_file_reload(event);
}

//----------------------------------------------------------------------------------
void MainFrame::on_view_console(wxCommandEvent& WXUNUSED(event))
{
    wxAuiPaneInfo panel = m_layoutManager.GetPane("Console");
    if (panel.IsOk() && panel.IsShown())
        hide_console();
    else
        show_console();
}

//---------------------------------------------------------------------------------------
void MainFrame::hide_console()
{
    //if added to AUI manager hide console
    wxAuiPaneInfo panel = m_layoutManager.GetPane("Console");
    if (panel.IsOk())
    {
        m_layoutManager.GetPane("Console").Show(false);
        m_layoutManager.Update();
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::show_console()
{
    //if not yet created, do it
    if (!m_pConsole)
        m_pConsole = LENMUS_NEW CommandWindow(this);

    //if not yet added to AUI manager do it now
    wxAuiPaneInfo panel = m_layoutManager.GetPane("Console");
    if (!panel.IsOk())
        m_layoutManager.AddPane(m_pConsole, wxAuiPaneInfo().Name("Console").
                                Caption(_("Command console")).
                                Bottom().Layer(1).
                                TopDockable(true).LeftDockable(true).
                                BottomDockable(true).RightDockable(true).
                                Floatable(true).FloatingSize(wxSize(410, 200)).
                                MinSize(wxSize(100,100)).
                                Movable(true).Resizable(true).
                                CloseButton(true).MaximizeButton(false));

    //show console
    m_layoutManager.GetPane("Console").Show(true);
    m_layoutManager.Update();
    m_pConsole->SetFocus();
}

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
void MainFrame::on_zoom_fit_width(wxCommandEvent& WXUNUSED(event))
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
void MainFrame::on_zoom_fit_full(wxCommandEvent& WXUNUSED(event))
{
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
    {
        pCanvas->zoom_fit_full();
        pCanvas->Refresh(false /* don't erase background */);
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::on_zoom_other(wxCommandEvent& WXUNUSED(event))
{
    Interactor* pInt = get_active_canvas_interactor();
    if (pInt)
    {
        double scale = pInt->get_scale() * 100.0;
        int nZoom = int( ::wxGetNumberFromUser("",
                            _("Zooming? (10 to 800)"), "",
                            int(scale), 10, 800) );
        if (nZoom != -1)    // -1 means invalid input or user canceled
            zoom_to( double(nZoom) / 100.0 );
    }
}

//---------------------------------------------------------------------------------------
bool MainFrame::is_toolbox_visible()
{
	return (m_pToolBox && m_layoutManager.GetPane("ToolBox").IsShown());
}

//---------------------------------------------------------------------------------------
void MainFrame::show_tool_box(bool fShow)
{
    if (!m_pToolBox)
        create_tool_box();

    if (fShow)
        show_tool_box();
    else
        hide_tool_box();
}

//---------------------------------------------------------------------------------------
void MainFrame::show_tool_box()
{
    //if not yet added to AUI manager do it now
    wxAuiPaneInfo panel = m_layoutManager.GetPane("ToolBox");
    if (!panel.IsOk())
        m_layoutManager.AddPane(m_pToolBox, wxAuiPaneInfo(). Name("ToolBox").
                            Caption(_("Edit toolbox")).Left().
                            Floatable(true).
                            Resizable(false).
                            TopDockable(false).
                            BottomDockable(false).
                            CloseButton(false).
                            Right().
                            MaxSize(wxSize(m_pToolBox->GetWidth(), -1)).
                            MinSize(wxSize(m_pToolBox->GetWidth(), -1)) );

    //show ToolBox
    m_layoutManager.GetPane("ToolBox").Show(true);
    m_layoutManager.Update();
    m_pToolBox->SetFocus();

    //enable/disable tools
    SelectionSet* pSelection = nullptr;
    DocCursor* pCursor = nullptr;
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
    {
        if (SpInteractor spIntor = pCanvas->get_interactor_shared_ptr())
        {
            pSelection = spIntor->get_selection_set();
            pCursor = spIntor->get_cursor();
        }
    }
    m_pToolBox->synchronize_tools(pSelection, pCursor);
}

//---------------------------------------------------------------------------------------
void MainFrame::hide_tool_box()
{
    //if added to AUI manager hide ToolBox
    wxAuiPaneInfo panel = m_layoutManager.GetPane("ToolBox");
    if (panel.IsOk())
    {
        m_layoutManager.GetPane("ToolBox").Show(false);
        m_layoutManager.Update();
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::create_tool_box()
{
    m_pToolBox =  LENMUS_NEW ToolBox(this, wxID_ANY, m_appScope);
    m_pToolBox->SelectToolPage(k_page_notes);
    m_pToolBox->Hide();
}

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

//---------------------------------------------------------------------------------------
void MainFrame::on_toolbox_tool_selected(ToolBoxToolSelectedEvent& event)
{
	ToolBox* pToolBox = get_active_toolbox();
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas && pToolBox)
        pCanvas->on_tool_selected_in_toolbox(event, pToolBox);
}

//---------------------------------------------------------------------------------------
void MainFrame::on_toolbox_page_changed(ToolBoxPageChangedEvent& event)
{
	ToolBox* pToolBox = get_active_toolbox();
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas && pToolBox)
        pCanvas->on_page_changed_in_toolbox(event, pToolBox);
    set_focus_on_active_document_window();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_edit_command(wxCommandEvent& event)
{
    exec_command( to_std_string(event.GetString()) );
}

//---------------------------------------------------------------------------------------
void MainFrame::exec_command(const string& cmd)
{
    if (cmd == "quit" || cmd == "exit")
    {
        hide_console();
        return;
    }

    //exec commnad
    wxString msg;
    int error;
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
    {
        msg = pCanvas->exec_command(cmd);
        error = pCanvas->get_last_error_code();
    }
    else
    {
        msg = "No active document!";
        error = 1;
    }

    //display results
    if (m_pConsole)
        m_pConsole->display_command( to_wx_string(cmd) );

    if (msg != wxEmptyString)
    {
        if (m_pConsole)
            error != 0 ? m_pConsole->display_error(msg)
                       : m_pConsole->display_message(msg);
        else
            wxMessageBox(msg);
    }

    if (m_pConsole)
        m_pConsole->SetFocus();
}

//---------------------------------------------------------------------------------------
void MainFrame::set_toolbox_for_active_page()
{
    static DocumentWindow* pPrevCanvas = nullptr;
    DocumentWindow* pCanvas = get_active_document_window();

    bool fEditionEnabled = pCanvas ? pCanvas->is_edition_enabled()
                                     || pCanvas->is_edition_gui_forced()
                                   : false;
    bool fToolBoxVisible = is_toolbox_visible();
    bool fChangeState = (fToolBoxVisible && !fEditionEnabled)
                        || (!fToolBoxVisible && fEditionEnabled);

    if (fChangeState)
    {
        fToolBoxVisible = !fToolBoxVisible;
        show_tool_box(fToolBoxVisible);
    }

    if (fToolBoxVisible && (fChangeState || pCanvas != pPrevCanvas))
    {
        //Need to do this here. When trying to do it in on_active_page_changed() the
        //toolbox forces a relayout and it forces to select back the old page.
        pPrevCanvas = pCanvas;
        restore_toolbox_for(pCanvas);
    }

    if (fToolBoxVisible)
        m_pToolBox->enable_tools(fEditionEnabled);

}

//---------------------------------------------------------------------------------------
void MainFrame::on_file_close(wxCommandEvent& WXUNUSED(event))
{
    close_active_document_window();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_file_close_all(wxCommandEvent& WXUNUSED(event))
{
    bool fMoreDocs = true;
    while (fMoreDocs)
        fMoreDocs = close_active_document_window();

    close_all();
    show_welcome_window();
}

//---------------------------------------------------------------------------------------
bool MainFrame::close_active_document_window()
{
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas && m_pContentWindow)
    {
        wxCloseEvent event;     //content doesn't matter. It is not used
        pCanvas->on_window_closing(event);
        m_pContentWindow->close_active_canvas();
    }
    return pCanvas != nullptr;
}

//---------------------------------------------------------------------------------------
void MainFrame::on_update_UI_file(wxUpdateUIEvent &event)
{
    DocumentWindow* pCanvas = get_active_document_window();
    DocumentFrame* pFrame = dynamic_cast<DocumentFrame*>( get_active_canvas() );
    bool fEnable = (pCanvas != nullptr);
    bool fSimpleDocument = (pFrame == nullptr) && (pCanvas != nullptr);
    bool fEdit = fEnable && pCanvas->is_edition_enabled();

    switch (event.GetId())
    {
        //Print related commands: enabled if DocumentFrame visible
        case wxID_PRINT_SETUP:
            event.Enable(fEnable);
            break;

        case k_menu_print:
            event.Enable(fEnable);
            break;

        //commands enabled if a page is displayed
        case k_menu_file_close:
        case k_menu_file_close_all:
            event.Enable(fEnable);
            break;

        //commands enabled when a simple document is displayed
        case k_menu_file_reload:
            event.Enable(fEnable && fSimpleDocument);
            break;

        // Other commands: always enabled
        default:
            event.Enable(true);
    }

}

//---------------------------------------------------------------------------------------
void MainFrame::on_update_UI_score(wxUpdateUIEvent &event)
{
    AScore score = get_active_score();
    event.Enable(score.is_valid());
}

//---------------------------------------------------------------------------------------
void MainFrame::on_update_UI_document(wxUpdateUIEvent &event)
{
    DocumentWindow* pCanvas = get_active_document_window();
    event.Enable(pCanvas != nullptr);
}

//---------------------------------------------------------------------------------------
void MainFrame::on_update_UI_sound(wxUpdateUIEvent &event)
{
    AScore score = get_active_score();
    switch (event.GetId())
    {
        case k_menu_play_start:
        {
            ScorePlayer* pPlayer = m_appScope.get_score_player();
            event.Enable( score.is_valid() && !pPlayer->is_playing());
            break;
        }

        case k_menu_play_stop:
        case k_menu_play_pause:
        {
            ScorePlayer* pPlayer = m_appScope.get_score_player();
            event.Enable( score.is_valid() && pPlayer->is_playing());
            break;
        }

        default:
            // Other items: only enabled if a score is displayed
            event.Enable(score.is_valid());
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::on_update_UI_help(wxUpdateUIEvent& event)
{
    event.Enable( m_appScope.are_experimental_features_enabled() );
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
void MainFrame::on_all_sounds_off(wxCommandEvent& WXUNUSED(event))
{
    MidiServer* pMidi = m_appScope.get_midi_server();
    Synthesizer* pSynth = pMidi->get_current_synth();
    if (pSynth)
        pSynth->all_sounds_off();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_sound_test(wxCommandEvent& WXUNUSED(event))
{
    MidiServer* pMidi = m_appScope.get_midi_server();
    if (pMidi)
        pMidi->do_sound_test();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_play_start(wxCommandEvent& WXUNUSED(event))
{
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
        pCanvas->play_active_score(this);
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
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
        pCanvas->play_stop();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_play_pause(wxCommandEvent& WXUNUSED(event))
{
    DocumentWindow* pCanvas = get_active_document_window();
    if (pCanvas)
        pCanvas->play_pause();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_options(wxCommandEvent& WXUNUSED(event))
{
    OptionsDlg dlg(this, m_appScope);
    dlg.CentreOnParent();
    dlg.ShowModal();
}

//---------------------------------------------------------------------------------------
void MainFrame::on_view_hide_show_toc(wxCommandEvent& event)
{
    DocumentFrame* pFrame = get_active_document_frame();
    if (pFrame)
        event.IsChecked() ? pFrame->show_toc() : pFrame->hide_toc();
}

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
void MainFrame::on_update_UI_view_toc(wxUpdateUIEvent& event)
{
    DocumentFrame* pFrame = get_active_document_frame();
    if (pFrame)
    {
        event.Enable(true);
        event.Check(pFrame->IsSplit());
    }
    else
    {
        event.Enable(false);
        event.Check(false);
    }
}

//---------------------------------------------------------------------------------------
void MainFrame::on_update_UI_view_virtual_keyboard(wxUpdateUIEvent &event)
{
    event.Enable( m_appScope.are_experimental_features_enabled() );
}

//---------------------------------------------------------------------------------------
bool MainFrame::is_welcome_page_displayed()
{
    if (m_pWelcomeWnd)
    {
        int idx = get_canvas_index(m_pWelcomeWnd);
        if (idx == wxNOT_FOUND)
            m_pWelcomeWnd = nullptr;
    }
    return (m_pWelcomeWnd != nullptr);
}

//---------------------------------------------------------------------------------------
void MainFrame::on_key_press(wxKeyEvent& WXUNUSED(event))
{
    //AWARE: All key events aree handled by the DocumentWindow. So no key events
    //       should arrive here
//	//if (event.GetEventType()==wxEVT_KEY_DOWN)
//        //if (event.GetKeyCode()==WXK_F1 && is_toolbox_visible())
//	{
//		RedirectKeyPressEvent(event);
//	}
//    //wxLogMessage("EVT_KEY_DOWN");
#if (LENMUS_DEBUG_BUILD == 1)
    wxMessageBox("[MainFrame::on_key_press] Key pressed!");
#endif
}

//------------------------------------------------------------------------------------
// mandatory overrides for PlayerGui
//------------------------------------------------------------------------------------
void MainFrame::on_end_of_playback()
{
    m_pMainMtr->mute(false);
}

//------------------------------------------------------------------------------------
bool MainFrame::countoff_status()
{
    return GetMenuBar()->IsChecked(k_menu_play_countoff);
}

//------------------------------------------------------------------------------------
bool MainFrame::metronome_status()
{
    return m_pMainMtr->is_running();
}

//------------------------------------------------------------------------------------
int MainFrame::get_play_mode()
{
    return k_play_normal_instrument;
}

//------------------------------------------------------------------------------------
int MainFrame::get_metronome_mm()
{
    return m_pMainMtr->get_mm();
}

//------------------------------------------------------------------------------------
Metronome* MainFrame::get_metronome()
{
    return m_pMainMtr;
}

//------------------------------------------------------------------------------------
bool MainFrame::process_key_in_toolbox(wxKeyEvent& event, ToolsInfo* pToolsInfo)
{
    //returns true if event is accepted and processed by ToolBox GUI

    if (!m_pToolBox)
        return false;

    //ask toolbox to process the key press
    wxAuiPaneInfo panel = m_layoutManager.GetPane("ToolBox");
    bool fProcessed = false;
    if ( panel.IsOk() && panel.IsShown() )
        fProcessed = m_pToolBox->process_key(event);

    pToolsInfo->update_toolbox_info(m_pToolBox);
    set_focus_on_active_document_window();
    return fProcessed;
}

//------------------------------------------------------------------------------------
int MainFrame::translate_key(int key, int keyFlags)
{
    if (!m_pToolBox)
        return k_cmd_null;
    else
        return m_pToolBox->translate_key(key, keyFlags);
}

//------------------------------------------------------------------------------------
void MainFrame::set_focus_on_active_document_window()
{
    //ensure focus is on document being edited
    Canvas* pCanvas = get_active_canvas();
    pCanvas->SetFocus();
}

////------------------------------------------------------------------------------------
//// Tips at application start
////------------------------------------------------------------------------------------
//
////---------------------------------------------------------------------------------------
//void MainFrame::ShowTips(bool fForceShow)
//{
//    bool fShowTips = false;
//    pPrefs->Read("/MainFrame/ShowTips", &fShowTips);
//    if (fForceShow || fShowTips)
//    {
//        //read data from last run
//        wxLogNull null; // disable error message if tips file does not exist
//        //wxString sTipsFile = pPrefs->Read("/data_path") + "/tips.txt";
//        //sTipsFile = g_pPaths->GetHelpPath();
//        //m_pHelp->SetTitleFormat(_("Test mode: using .hhp help file"));
//        long nTipsIndex = pPrefs->Read("/MainFrame/NextTip", 0L);
//
//        //show next tip
//        wxTipProvider oTipDlg = wxCreateFileTipProvider(sTipsFile, nTipsIndex);
//        fShowTips = wxShowTip(this, &oTipDlg, fShowTips);
//
//        //save data for next run
//        pPrefs->Write("/MainFrame/ShowTips", fShowTips);
//        pPrefs->Write("/MainFrame/NextTip", (long)oTipDlg.GetCurrentTip());
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
AScore MainFrame::get_active_score()
{
    DocumentWindow* pWnd = get_active_document_window();
    if (pWnd)
        return pWnd->get_active_score();
    else
        return AScore();
}

//---------------------------------------------------------------------------------------
Interactor* MainFrame::get_active_canvas_interactor()
{
    DocumentWindow* pWnd = get_active_document_window();
    if (pWnd)
        return pWnd->get_interactor();
    else
        return nullptr;
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
DocumentWindow* MainFrame::get_document_window_for_page(int iCanvas)
{
    Canvas* pCanvas = get_canvas_for_index(iCanvas);
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

//---------------------------------------------------------------------------------------
void MainFrame::on_caret_timer_event(wxTimerEvent& WXUNUSED(event))
{
    Interactor* pInteractor = get_active_canvas_interactor();
    if (pInteractor)
        pInteractor->blink_caret();
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
