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

#ifndef __LENMUS_MAIN_FRAME_H__        //to avoid nested includes
#define __LENMUS_MAIN_FRAME_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_injectors.h"
#include "lenmus_canvas.h"
#include "lenmus_test_runner.h"
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
//#include <wx/stopwatch.h>
#include <wx/aui/aui.h>
//#include <wx/timer.h>           //to use wxTimer
#include <wx/spinctrl.h>        //to use spin control
#include <wx/combobox.h>        //to use comboBox control
#include <wx/docview.h>         //to use wxFileHistory

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
#include <UnitTest++.h>


namespace lenmus
{

//forward declaration
class DocumentWindow;
class DocumentFrame;
class WelcomeWindow;
class DlgCounters;
class ProblemManager;
class StatusBar;
class GlobalMetronome;
class CommandWindow;
class ToolBox;


DECLARE_EVENT_TYPE(LM_EVT_CHECK_FOR_UPDATES, -1)
DECLARE_EVENT_TYPE(LM_EVT_OPEN_BOOK, -1)
DECLARE_EVENT_TYPE(LM_EVT_EDIT_COMMAND, -1)

//public identifiers for commands/events sent to MainFrame
enum {
    k_id_check_for_updates = 10000,
    k_id_open_book,
    k_menu_open_books,
    k_id_edit_command,
    k_menu_last_public_id,
};

//---------------------------------------------------------------------------------------
// Define the main frame for the GUI
class MainFrame : public ContentFrame, public PlayerGui
{
protected:
    ApplicationScope& m_appScope;
    wxAuiManager    m_layoutManager;
    string          m_lastOpenFile;     //path for currently open file

    //menus
    wxMenu* m_dbgMenu;
	wxMenu* m_editMenu;
    wxMenu* m_booksMenu;

    //controllers, special windows, and other controls
    ToolBox*          m_pToolBox;           //tool box window
    Canvas*           m_pWelcomeWnd;        //welcome window
    CommandWindow*    m_pCommandLine;       //command line window

//    lmHtmlWindow*           m_pHtmlWin;
    wxSpinCtrl*             m_pSpinMetronome;
    wxComboBox*             m_pComboZoom;

    GlobalMetronome*        m_pMainMtr;   //independent metronome
    GlobalMetronome*        m_pMtr;       //metronome currently associated to frame metronome controls

//    //flags for toggle buttons/menus
//    bool m_fHelpOpened;

    // tool bars
    wxToolBar*      m_pToolbar;         // main toolbar
    wxToolBar*      m_pTbFile;          // file toolbar
    wxToolBar*      m_pTbEdit;          // edit toolbar
    wxToolBar*      m_pTbZoom;          // zoom toolbar
    wxToolBar*      m_pTbPlay;          // play toolbar
    wxToolBar*      m_pTbMtr;           // metronome toolbar
    wxToolBar*      m_pTbTextBooks;     // text books navigation toolbar

    // status bar
    StatusBar*    m_pStatusBar;

//    bool    m_fClosingAll;

    //to remember print settings during the session
    wxPrintData* m_pPrintData;
    wxPageSetupDialogData* m_pPageSetupData;

    //other
    wxFileHistory   m_fileHistory;

public:
    MainFrame(ApplicationScope& appScope, const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize);
    virtual ~MainFrame();

    void create_controls();
    void run_midi_wizard();
    void show_welcome_window();

    //callbacks
    static void wrapper_lomse_event(void* pThis, SpEventInfo pEvent);
    static void wrapper_lomse_request(void* pThis, Request* pRequest);

    //commands from other places
    void update_toolbars_layout();
    void open_file();
    void quit();

    //panels
    void add_new_panel(wxWindow* window, const wxString& caption,
                       const wxPoint& pos = wxDefaultPosition);

    //mandatory overrides from PlayerGui
    void on_end_of_playback();
    int get_play_mode();
    int get_metronome_mm();
    Metronome* get_metronome();
    bool countoff_status();
    bool metronome_status();

protected:
    void disable_tool(wxUpdateUIEvent &event);
    void save_preferences();
    void create_metronome();

    void create_menu();
    void set_lomse_callbacks();
    void load_file(const string& filename);
    Interactor* get_active_canvas_interactor();
    ImoScore* get_active_score();
    DocumentWindow* get_active_document_window();
    DocumentFrame* get_active_document_frame();

    //for serving lomse requests
    void generate_dynamic_content(RequestDynamic* pRequest);
    void get_font_filename(RequestFont* pRequest);

    //zoom related
    void zoom_to(double scale);

    //lomse callbacks
    void on_lomse_event(SpEventInfo pEvent);
    void on_lomse_request(Request* pRequest);

//    wxLocale*   m_pLocale;            // locale for internationalization

    //welcome window
    bool is_welcome_page_displayed();

    //toolbars
    void show_toolbars_if_user_preferences();
    void create_toolbars();
    void delete_toolbars();
//    void CreateTextBooksToolBar(long style, wxSize nIconSize, int nRow);

    //menu bar
    void create_menu_item(wxMenu* pMenu, int nId, const wxString& sItemName,
                          const wxString& sToolTip = _T(""),
                          wxItemKind nKind = wxITEM_NORMAL,
                          const wxString& sIconName = _T("empty") );

    //status bar
    void show_status_bar_if_user_preferences();
    void create_status_bar(int nType=0);
    void delete_status_bar();
//    void SetStatusBarMsg(const wxString& sText);
//    void SetStatusBarMouseData(int nPage, float rTime, int nMeasure, lmUPoint uPos);
//    void SetStatusBarCaretData(int nPage, float rTime, int nMeasure);

    //ToolBox
	inline ToolBox* get_active_toolbox() { return m_pToolBox; }
	bool is_toolbox_visible();
	void show_tool_box(bool fShow);

//    // metronome
//    void SetMetronome(GlobalMetronome* pMtr);
//    GlobalMetronome* GetMetronome() { return m_pMtr; }

    // File menu events
    void on_file_quit(wxCommandEvent& event);
    void on_file_open(wxCommandEvent& WXUNUSED(event));
    void on_file_reload(wxCommandEvent& WXUNUSED(event));
//    void OnFileClose(wxCommandEvent& event);
//    void OnFileSave(wxCommandEvent& event);
//    void OnFileSaveAs(wxCommandEvent& event);
//    void OnScoreWizard(wxCommandEvent& WXUNUSED(event));
//    void OnFileImport(wxCommandEvent& WXUNUSED(event));
//	void OnExportMusicXML(wxCommandEvent& WXUNUSED(event));
//	void OnExportBMP(wxCommandEvent& WXUNUSED(event));
//    void OnExportJPG(wxCommandEvent& WXUNUSED(event));
    void on_print_setup(wxCommandEvent& WXUNUSED(event));
    void on_print(wxCommandEvent& WXUNUSED(event));
    void on_update_UI_file(wxUpdateUIEvent& event);
    void on_open_recent_file(wxCommandEvent& event);
    void on_open_book(wxCommandEvent& event);
//    void ExportAsImage(int nImgType);


    // Edit menu events
//    void OnEditCut(wxCommandEvent& event);
//    void OnEditCopy(wxCommandEvent& event);
//    void OnEditPaste(wxCommandEvent& event);
    void OnEditUpdateUI(wxUpdateUIEvent& event);

//	// Score Menu events
//	void OnScoreTitles(wxCommandEvent& WXUNUSED(event));
//
//	// Instrument menu events
//	void OnInstrumentProperties(wxCommandEvent& WXUNUSED(event));

    // Debug menu events
#if (LENMUS_DEBUG_BUILD == 1 || LENMUS_RELEASE_INSTALL == 0)
    void on_show_command_window(wxCommandEvent& WXUNUSED(event));
    void on_do_tests(wxCommandEvent& WXUNUSED(event));
    void on_see_paths(wxCommandEvent& WXUNUSED(event));
    void on_debug_draw_box(wxCommandEvent& event);
    void on_debug_justify_systems(wxCommandEvent& event);
    void on_debug_dump_column_tables(wxCommandEvent& event);
    void on_debug_force_release_behaviour(wxCommandEvent& event);
    void on_debug_show_debug_links(wxCommandEvent& event);
//    void OnDebugShowBorderOnScores(wxCommandEvent& event);
//    void OnDebugRecSelec(wxCommandEvent& event);
//    void OnDebugDrawBounds(wxCommandEvent& event);
    void on_debug_draw_anchors(wxCommandEvent& event);
//    void OnDebugTestMidi(wxCommandEvent& event);
//    void OnDebugSetTraceLevel(wxCommandEvent& WXUNUSED(event));
//    void OnDebugPatternEditor(wxCommandEvent& WXUNUSED(event));
//    void OnDebugUnitTests(wxCommandEvent& event);
//    void OnDebugShowDirtyObjects(wxCommandEvent& event);
//        // methods requiring a score
//    void OnDebugCheckHarmony(wxCommandEvent& WXUNUSED(event));
//    void OnDebugDumpBitmaps(wxCommandEvent& event);
    void on_debug_dump_gmodel(wxCommandEvent& WXUNUSED(event));
    void on_debug_see_midi_events(wxCommandEvent& WXUNUSED(event));
    void on_debug_see_ldp_source(wxCommandEvent& WXUNUSED(event));
    void on_debug_see_lmd_source(wxCommandEvent& WXUNUSED(event));
    void on_debug_see_staffobjs(wxCommandEvent& WXUNUSED(event));
//    void on_debug_see_sourceForUndo(wxCommandEvent& event);
//    void OnDebugSeeXML(wxCommandEvent& event);
//    void OnDebugTestProcessor(wxCommandEvent& WXUNUSED(event));
//    void OnDebugScoreUI(wxUpdateUIEvent& event);
    void on_debug_print_preview(wxCommandEvent& WXUNUSED(event));
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
    void on_view_tools(wxCommandEvent& event);
//    void OnViewRulers(wxCommandEvent& event);
//    void OnViewRulersUI(wxUpdateUIEvent& event);
    void on_view_tool_bar(wxCommandEvent& WXUNUSED(event));
    void on_view_status_bar(wxCommandEvent& WXUNUSED(event));
    void on_update_UI_tool_bar(wxUpdateUIEvent& event);
    void on_update_UI_status_bar(wxUpdateUIEvent& event);
//    void OnViewPageMargins(wxCommandEvent& event);
    void on_view_welcome_page(wxCommandEvent& WXUNUSED(event));
    void on_update_UI_welcome_page(wxUpdateUIEvent& event);

    // Sound menu events
    void on_update_UI_sound(wxUpdateUIEvent& event);
    void on_sound_test(wxCommandEvent& WXUNUSED(event));
    void on_all_sounds_off(wxCommandEvent& WXUNUSED(event));
    void on_run_midi_wizard(wxCommandEvent& WXUNUSED(event));
    void on_play_start(wxCommandEvent& WXUNUSED(event));
//    void OnPlayCursorStart(wxCommandEvent& WXUNUSED(event));
    void on_play_stop(wxCommandEvent& WXUNUSED(event));
    void on_play_pause(wxCommandEvent& WXUNUSED(event));

    //Window menu events
    void on_window_close_all(wxCommandEvent& WXUNUSED(event));

//    // Voice events
//    void OnComboVoice(wxCommandEvent& event);

    //Help menu
    void on_about(wxCommandEvent& event);
//    void OnHelpQuickGuide(wxCommandEvent& WXUNUSED(event));
//    void OnHelpOpen(wxCommandEvent& event);
    void on_check_for_updates(wxCommandEvent& WXUNUSED(event));
    void on_silently_check_for_updates(wxCommandEvent& WXUNUSED(event));
    void on_visit_website(wxCommandEvent& WXUNUSED(event));

    // Other menu items events
    void on_options(wxCommandEvent& WXUNUSED(event));
    void on_open_books(wxCommandEvent& event);
//    void OnOpenBookUI(wxUpdateUIEvent& event);

    //other even managers
    void on_metronome_timer(wxTimerEvent& event);
    void on_metronome_on_off(wxCommandEvent& WXUNUSED(event));
    void on_metronome_update(wxSpinEvent& WXUNUSED(event));
    void on_metronome_update_text(wxCommandEvent& WXUNUSED(event));
//    void OnPaneClose(wxAuiManagerEvent& event);
    void on_key_press(wxKeyEvent& event);
//	void OnKeyF1(wxCommandEvent& event);
//
//
//    //textbook events and methods
//    void OnDocumentFrame(wxCommandEvent& event);
//    void OnDocumentFrameUpdateUI(wxUpdateUIEvent& event);
//    void OnCloseDocumentFrame();

    //other events
    void on_close_frame(wxCloseEvent& WXUNUSED(event));
    void on_size(wxSizeEvent& WXUNUSED(event));
    void on_edit_command(wxCommandEvent& event);
    //void on_create_counters_panel(wxCommandEvent& WXUNUSED(event));
    //void on_counters_event(CountersEvent& event);
    void OnPaneClose(wxAuiManagerEvent& evt);

    // other methods
    //DlgCounters* create_counters_dlg(int mode, ProblemManager* pManager);
    //wxPoint get_counters_position();
//    void SetOpenHelpButton(bool fButtonPressed);
//    void SilentlyCheckForUpdates(bool fSilent);
//	inline wxMenu* GetEditMenu() {return m_editMenu; }
//    void NewScoreWindow(lmEditorMode* pMode, ImoScore* pScore);
//    void OpenScore(wxString& sFilename, bool fAsNew);
//    void OpenBook(const wxString& sPageName);
//    void RunUnitTests();
//
//    //options
//    bool ShowRulers();
//    bool IsCountOffChecked();

    //welcome window
//    void OnCloseWelcomeWnd();
//
//    //eBooks controller
//    void SetHtmlWindow(lmHtmlWindow* pHtmlWin) { m_pHtmlWin = pHtmlWin; }
//    lmHtmlWindow* GetHtmlWindow() { return m_pHtmlWin; }
//
//    //debug method
//    void DumpScore(ImoScore* pScore);
//
//	//access to information
//    inline TextBookController* GetBookController() { return m_pBookController; }
//    lmController* GetActiveController();
//    inline wxFileHistory* GetFileHistory() { return GetDocumentManager()->GetFileHistory(); }
//    lmDocument* GetActiveDoc();
//
//	// call backs
//	void OnActiveChildChanged(lmTDIChildFrame* pFrame);
//
//	//other
//	void RedirectKeyPressEvent(wxKeyEvent& event);
//    void SetFocusOnActiveView();
//
//    //access to current active MDI Child
//    lmScoreView* GetActiveScoreView();
//
//#if lmUSE_LIBRARY_MVC
//
//    //call back to access the MvcCollection
//    MvcCollection* GetMvcCollection();
//    void OnCloseDocument(Document* pDoc);
//
//#endif
//
//protected:
//    void InitializeHelp();
//    void InitializeBooks();
//    void ScanForBooks(wxString sPath, wxString sPattern);
//    void CloseAllWindows();


    DECLARE_EVENT_TABLE()
};



}   // namespace lenmus

#endif    // __LENMUS_MAIN_FRAME_H__

