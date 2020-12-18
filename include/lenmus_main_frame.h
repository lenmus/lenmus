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

#ifndef __LENMUS_MAIN_FRAME_H__        //to avoid nested includes
#define __LENMUS_MAIN_FRAME_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_injectors.h"
#include "lenmus_canvas.h"
#include "lenmus_dlg_debug.h"
#include "lenmus_events.h"
#include "lenmus_metronome.h"

//wxWidgets
#include "wx/wxprec.h"
#include <wx/app.h>
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <wx/image.h>
#include <wx/dc.h>
#include <wx/dcmemory.h>
#include <wx/event.h>
#include <wx/aui/aui.h>
#include <wx/timer.h>           //to use wxTimer
#include <wx/spinctrl.h>        //to use spin control
#include <wx/combobox.h>        //to use comboBox control
#include <wx/docview.h>         //to use wxFileHistory
#include <wx/bmpcbox.h>         //for wxBitmapComboBox

class wxPrintData;
class wxPageSetupDialogData;


//lomse
#include <lomse_doorway.h>
#include <lomse_document.h>
#include <lomse_graphic_view.h>
#include <lomse_interactor.h>
#include <lomse_presenter.h>
#include <lomse_events.h>
#include <lomse_internal_model.h>
#include <lomse_analyser.h>
#include "lomse_player_gui.h"
using namespace lomse;

//other
#include <iostream>


namespace lenmus
{

//forward declaration
class DocumentWindow;
class DocumentFrame;
class WelcomeWindow;
class DlgCounters;
class ProblemManager;
class GlobalMetronome;
class DlgSpacingParams;
class DlgMetronome;


DECLARE_EVENT_TYPE(LM_EVT_OPEN_BOOK, -1)

//public identifiers for commands/events sent to MainFrame
enum {
    k_id_open_book = 10000,
    k_menu_help_study_guide,

    k_menu_last_public_id,
};

//---------------------------------------------------------------------------------------
// Define the main frame for the GUI
class MainFrame : public ContentFrame
                , public PlayerGui
{
protected:
    ApplicationScope& m_appScope;
    wxAuiManager    m_layoutManager;
    string          m_lastOpenFile;     //path for currently open file

    //menus
    wxMenu* m_dbgMenu;

    //controllers, special windows, and other controls
    Canvas*           m_pWelcomeWnd;        //welcome window
    DlgSpacingParams* m_pSpacingParamsDlg;  //dialog for spacing params
    DlgMetronome*     m_pMetronomeDlg;      //dialog for metronome settings


    GlobalMetronome*        m_pMainMtr;   //independent metronome
    GlobalMetronome*        m_pMtr;       //metronome currently associated to frame metronome controls

    //to remember print settings during the session
    wxPrintData* m_pPrintData;
    wxPageSetupDialogData* m_pPageSetupData;

    //filehistory
    wxFileHistory   m_fileHistory;

public:
    MainFrame(ApplicationScope& appScope, const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize);
    virtual ~MainFrame();

    void create_controls();
    void show_welcome_window();

    //callbacks
    static void wrapper_lomse_event(void* pThis, SpEventInfo pEvent);
    static void wrapper_lomse_request(void* pThis, Request* pRequest);

    //commands from other places
    void open_file();
    void quit();
    void update_spacing_params(float force, float alpha, float dmin);
    void update_metronome_beat();

    //panels
    void add_new_panel(wxWindow* window, const wxString& caption,
                       const wxPoint& pos = wxDefaultPosition);

    //mandatory overrides from PlayerGui
    void on_end_of_playback();
    int get_play_mode();
    int get_metronome_mm();
    Metronome* get_metronome();
    bool countoff_status() { return false; }
    bool metronome_status();

protected:
    void disable_tool(wxUpdateUIEvent &event);
    void save_preferences();
    void create_metronome();
    void load_global_options();

    void create_menu();
    void set_lomse_callbacks();
    void load_file(const string& filename);
    Interactor* get_active_canvas_interactor();
    AScore get_active_score();
    DocumentWindow* get_active_document_window();
    DocumentFrame* get_active_document_frame();
    DocumentWindow* get_document_window_for_page(int iCanvas);
    bool close_active_document_window();
    void set_focus_on_active_document_window();

    //for serving lomse requests
    void generate_dynamic_content(RequestDynamic* pRequest);
    void get_font_filename(RequestFont* pRequest);

    //zoom related
    void zoom_to(double scale);

    //lomse callbacks
    void on_lomse_event(SpEventInfo pEvent);
    void on_lomse_request(Request* pRequest);

    //welcome window
    bool is_welcome_page_displayed();

    //menu bar
    void create_menu_item(wxMenu* pMenu, int nId, const wxString& sItemName,
                          const wxString& sToolTip = "",
                          wxItemKind nKind = wxITEM_NORMAL,
                          const wxString& sIconName = "empty",
                          const wxString& sShortcut = "" );

    // File menu events
    void on_file_quit(wxCommandEvent& event);
    void on_file_open(wxCommandEvent& WXUNUSED(event));
    void on_file_reload(wxCommandEvent& WXUNUSED(event));
    void on_file_close(wxCommandEvent& event);
    void on_file_close_all(wxCommandEvent& WXUNUSED(event));
    void on_print_setup(wxCommandEvent& WXUNUSED(event));
    void on_print(wxCommandEvent& WXUNUSED(event));
    void on_update_UI_file(wxUpdateUIEvent& event);
    void on_open_recent_file(wxCommandEvent& event);
    void on_open_book(wxCommandEvent& event);

    // Debug menu events
#if (LENMUS_DEBUG_BUILD == 1 || LENMUS_RELEASE_INSTALL == 0)
    void on_debug_test_api(wxCommandEvent& WXUNUSED(event));
    void on_do_tests(wxCommandEvent& WXUNUSED(event));
    void on_see_paths(wxCommandEvent& WXUNUSED(event));
    void on_debug_draw_box(wxCommandEvent& event);
    void on_debug_justify_systems(wxCommandEvent& event);
    void on_debug_spacing_parameters(wxCommandEvent& event);
    void on_debug_trace_lines_break(wxCommandEvent& event);
    void on_debug_dump_column_tables(wxCommandEvent& event);
    void on_debug_force_release_behaviour(wxCommandEvent& event);
    void on_debug_show_debug_links(wxCommandEvent& event);
    void on_debug_draw_shape_bounds(wxCommandEvent& event);
    void on_debug_draw_anchor_objects(wxCommandEvent& event);
    void on_debug_draw_anchor_lines(wxCommandEvent& event);
    void on_debug_draw_slur_ctrol_points(wxCommandEvent& event);
    void on_debug_draw_vertical_profile(wxCommandEvent& event);
    void on_debug_dump_gmodel(wxCommandEvent& WXUNUSED(event));
    void on_debug_dump_imodel(wxCommandEvent& WXUNUSED(event));
    void on_debug_see_spacing_data(wxCommandEvent& WXUNUSED(event));
    void on_debug_see_document_ids(wxCommandEvent& WXUNUSED(event));
    void on_debug_see_midi_events(wxCommandEvent& WXUNUSED(event));
    void on_debug_see_ldp_source(wxCommandEvent& WXUNUSED(event));
    void on_debug_see_lmd_source(wxCommandEvent& WXUNUSED(event));
    void on_debug_see_mnx_source(wxCommandEvent& WXUNUSED(event));
    void on_debug_see_checkpoint_data(wxCommandEvent& WXUNUSED(event));
    void on_debug_see_staffobjs(wxCommandEvent& WXUNUSED(event));
    void on_debug_see_cursor_state(wxCommandEvent& WXUNUSED(event));
    void on_debug_print_preview(wxCommandEvent& WXUNUSED(event));
    void on_update_UI_debug(wxUpdateUIEvent &event);
#endif

    void on_update_UI_score(wxUpdateUIEvent &event);
    void on_update_UI_document(wxUpdateUIEvent &event);

    // Zoom events
    void on_combo_zoom(wxCommandEvent& event);
    void on_zoom_100(wxCommandEvent& WXUNUSED(event)) { zoom_to(1.0); }
    void on_zoom_fit_width(wxCommandEvent& event);
    void on_zoom_fit_full(wxCommandEvent& event);
    void on_zoom_other(wxCommandEvent& event);
    void on_zoom_in(wxCommandEvent& WXUNUSED(event));
    void on_zoom_out(wxCommandEvent& WXUNUSED(event));
    void on_update_UI_zoom(wxUpdateUIEvent& event);

    // View menu events
    void on_view_hide_show_toc(wxCommandEvent& event);
    void on_update_UI_view_toc(wxUpdateUIEvent& event);
    void on_view_welcome_page(wxCommandEvent& WXUNUSED(event));
    void on_update_UI_welcome_page(wxUpdateUIEvent& event);

    // Sound menu events
    void on_update_UI_sound(wxUpdateUIEvent& event);
    void on_sound_test(wxCommandEvent& WXUNUSED(event));
    void on_all_sounds_off(wxCommandEvent& WXUNUSED(event));
    void on_play_start(wxCommandEvent& WXUNUSED(event));
    void on_play_stop(wxCommandEvent& WXUNUSED(event));
    void on_play_pause(wxCommandEvent& WXUNUSED(event));

    //Tools menu
    void on_metronome_tool(wxCommandEvent& WXUNUSED(event));

    //Help menu
    void on_about(wxCommandEvent& event);
    void on_show_study_guide(wxCommandEvent& event);
    void on_visit_website(wxCommandEvent& WXUNUSED(event));

    // Options menu
    void on_options(wxCommandEvent& WXUNUSED(event));

    //other even managers
    void on_key_press(wxKeyEvent& event);
    void on_caret_timer_event(wxTimerEvent& WXUNUSED(event));
    void on_active_canvas_changing(wxAuiNotebookEvent& event);

    //other events
    void on_close_frame(wxCloseEvent& WXUNUSED(event));
    void on_edit_command(wxCommandEvent& event);
    void on_tab_close(wxAuiManagerEvent& evt);

    wxDECLARE_EVENT_TABLE();
};



}   // namespace lenmus

#endif    // __LENMUS_MAIN_FRAME_H__

