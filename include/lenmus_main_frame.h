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

#ifndef __LENMUS_MAIN_FRAME_H__        //to avoid nested includes
#define __LENMUS_MAIN_FRAME_H__

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

#include <iostream>
#include <UnitTest++.h>

//lomse headers
#include "lomse_doorway.h"
#include "lomse_document.h"
#include "lomse_graphic_view.h"
#include "lomse_interactor.h"
#include "lomse_presenter.h"

#include "lomse_events.h"
#include "lomse_internal_model.h"
#include "lomse_analyser.h"

#include "lenmus_injectors.h"
#include "lenmus_canvas.h"
#include "lenmus_test_runner.h"
#include "lenmus_dlg_debug.h"

using namespace lomse;


namespace lenmus
{

//forward declaration
class ScoreCanvas;
class WelcomeWindow;


//---------------------------------------------------------------------------------------
// Define the main frame for the GUI
class MainFrame: public ContentFrame
{
protected:
    ApplicationScope& m_appScope;
    wxAuiManager m_layoutManager;

    static LomseDoorway    m_lomse;            //the Lomse library doorway
    ostringstream   m_reporter;         //to have access to error messages
    streambuf*      m_cout_buffer;      //to restore cout
    string          m_lastOpenFile;     //path for currently open file

    //menus
    wxMenu* m_dbgMenu;
	wxMenu* m_editMenu;
    wxMenu* m_booksMenu;

//    //controllers, special windows, and other controls
//    wxAuiManager            m_mgrAUI;           // wxAUI manager
//    lmToolBox*              m_pToolBox;         //tool box window
    WelcomeWindow*           m_pWelcomeWnd;      //welcome window
//    lmTextBookController*   m_pBookController;
//    lmHtmlWindow*           m_pHtmlWin;
//    lmHelpController*       m_pHelp;
    wxSpinCtrl*             m_pSpinMetronome;
    wxComboBox*             m_pComboZoom;
//
//    lmMetronome*        m_pMainMtr;        //independent metronome
//    lmMetronome*        m_pMtr;            //metronome currently associated to frame metronome controls
//
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
//    lmStatusBar*    m_pStatusBar;
//
//    bool    m_fSilentCheck;
//    bool    m_fClosingAll;

public:
    MainFrame(ApplicationScope& appScope, const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize);
    virtual ~MainFrame();

    void create_controls();
    static string get_font_filename(const string& fontname, bool bold, bool italic);
    static void on_lomse_event(EventInfo& event);
    void run_midi_wizard();
    void show_welcome_window();

protected:
    void on_quit(wxCommandEvent& event);
    void on_file_open(wxCommandEvent& WXUNUSED(event));
    void on_file_reload(wxCommandEvent& WXUNUSED(event));
    void on_about(wxCommandEvent& event);
    void on_size(wxSizeEvent& event);
    void on_zoom_in(wxCommandEvent& WXUNUSED(event));
    void on_zoom_out(wxCommandEvent& WXUNUSED(event));
    void on_do_tests(wxCommandEvent& WXUNUSED(event));
    void on_debug_draw_box(wxCommandEvent& event);
    void on_debug_justify_systems(wxCommandEvent& event);
    void on_debug_dump_column_tables(wxCommandEvent& event);

    void create_menu();
    void create_status_bar();
    void initialize_lomse();
    void load_file(string& filename);
    void create_tool_bar();

    DECLARE_EVENT_TABLE()


//    wxLocale*   m_pLocale;            // locale for internationalization
//
//    void DeleteToolbar();
//    void CreateTextBooksToolBar(long style, wxSize nIconSize, int nRow);
//
//    //status bar
//    void SetStatusBarMsg(const wxString& sText);
//    void SetStatusBarMouseData(int nPage, float rTime, int nMeasure, lmUPoint uPos);
//    void SetStatusBarCaretData(int nPage, float rTime, int nMeasure);
//
//    //ToolBox
//	inline lmToolBox* GetActiveToolBox() { return m_pToolBox; }
//	bool IsToolBoxVisible();
//	void ShowToolBox(bool fShow);
//
//    // metronome
//    void SetMetronome(lmMetronome* pMtr);
//    lmMetronome* GetMetronome() { return m_pMtr; }
//
//    // File menu events
//    void OnFileOpen(wxCommandEvent& event);
//    void OnFileClose(wxCommandEvent& event);
//    void OnFileSave(wxCommandEvent& event);
//    void OnFileSaveAs(wxCommandEvent& event);
//    void OnScoreWizard(wxCommandEvent& WXUNUSED(event));
//    void OnFileImport(wxCommandEvent& WXUNUSED(event));
//	void OnExportMusicXML(wxCommandEvent& WXUNUSED(event));
//	void OnExportBMP(wxCommandEvent& WXUNUSED(event));
//    void OnExportJPG(wxCommandEvent& WXUNUSED(event));
//    void OnPrintPreview(wxCommandEvent& WXUNUSED(event));
//    //void OnPageSetup(wxCommandEvent& WXUNUSED(event));
//    void OnPrintSetup(wxCommandEvent& WXUNUSED(event));
//    void OnPrint(wxCommandEvent& event);
//    void OnFileUpdateUI(wxUpdateUIEvent& event);
//    void OnOpenRecentFile(wxCommandEvent& event);
//
//    void ExportAsImage(int nImgType);
//
//
//    // Edit menu events
//    void OnEditCut(wxCommandEvent& event);
//    void OnEditCopy(wxCommandEvent& event);
//    void OnEditPaste(wxCommandEvent& event);
//    void OnEditUpdateUI(wxUpdateUIEvent& event);
//
//	// Score Menu events
//	void OnScoreTitles(wxCommandEvent& WXUNUSED(event));
//
//	// Instrument menu events
//	void OnInstrumentProperties(wxCommandEvent& WXUNUSED(event));
//
//    // Debug menu events
//#ifdef _LM_DEBUG_
//    void OnDebugForceReleaseBehaviour(wxCommandEvent& event);
//    void OnDebugShowDebugLinks(wxCommandEvent& event);
//    void OnDebugShowBorderOnScores(wxCommandEvent& event);
//    void OnDebugRecSelec(wxCommandEvent& event);
//    void OnDebugDrawBounds(wxCommandEvent& event);
//    void OnDebugDrawAnchors(wxCommandEvent& event);
//    void OnDebugTestMidi(wxCommandEvent& event);
//    void OnDebugSetTraceLevel(wxCommandEvent& WXUNUSED(event));
//    void OnDebugPatternEditor(wxCommandEvent& WXUNUSED(event));
//    void OnDebugUnitTests(wxCommandEvent& event);
//    void OnDebugShowDirtyObjects(wxCommandEvent& event);
//        // methods requiring a score
//    void OnDebugCheckHarmony(wxCommandEvent& WXUNUSED(event));
//    void OnDebugDumpBitmaps(wxCommandEvent& event);
//    void OnDebugDumpGMObjects(wxCommandEvent& event);
//    void OnDebugDumpStaffObjs(wxCommandEvent& event);
//    void OnDebugSeeMidiEvents(wxCommandEvent& WXUNUSED(event));
//    void OnDebugSeeSource(wxCommandEvent& event);
//    void OnDebugSeeSourceForUndo(wxCommandEvent& event);
//    void OnDebugSeeXML(wxCommandEvent& event);
//    void OnDebugTestProcessor(wxCommandEvent& WXUNUSED(event));
//    void OnDebugScoreUI(wxUpdateUIEvent& event);
//#endif
//
//    // Zoom events
//    void OnComboZoom(wxCommandEvent& event);
//    void OnZoom(wxCommandEvent& event, double rScale);
//    void OnZoom100(wxCommandEvent& event) { OnZoom(event, 1.0); }
//    void OnZoomFitWidth(wxCommandEvent& event);
//    void OnZoomFitFull(wxCommandEvent& event);
//    void OnZoomOther(wxCommandEvent& event);
//    void OnZoomIncrease(wxCommandEvent& event);
//    void OnZoomDecrease(wxCommandEvent& event);
//    void OnZoomUpdateUI(wxUpdateUIEvent& event);
//
//    //zoom related
//    void UpdateZoomControls(double rScale);
//
//    // View menu events
//    void OnViewTools(wxCommandEvent& event);
//    void OnViewRulers(wxCommandEvent& event);
//    void OnViewRulersUI(wxUpdateUIEvent& event);
//    void OnViewToolBar(wxCommandEvent& WXUNUSED(event));
//    void OnViewStatusBar(wxCommandEvent& WXUNUSED(event));
//    void OnToolbarsUI(wxUpdateUIEvent& event);
//    void OnStatusbarUI(wxUpdateUIEvent& event);
//    void OnViewPageMargins(wxCommandEvent& event);
//    void OnViewWelcomePage(wxCommandEvent& event);
//    void OnViewWelcomePageUI(wxUpdateUIEvent& event);

    // Sound menu events
//    void OnSoundUpdateUI(wxUpdateUIEvent& event);
    void on_sound_test(wxCommandEvent& WXUNUSED(event));
    void on_all_sounds_off(wxCommandEvent& WXUNUSED(event));
    void on_run_midi_wizard(wxCommandEvent& WXUNUSED(event));
    void on_play_start(wxCommandEvent& WXUNUSED(event));
//    void OnPlayCursorStart(wxCommandEvent& WXUNUSED(event));
//    void OnPlayStop(wxCommandEvent& WXUNUSED(event));
//    void OnPlayPause(wxCommandEvent& WXUNUSED(event));
//
//    //Window menu events
//    void OnWindowClose(wxCommandEvent& WXUNUSED(event));
//    void OnWindowCloseAll(wxCommandEvent& WXUNUSED(event));
//    void OnWindowNext(wxCommandEvent& WXUNUSED(event));
//    void OnWindowPrev(wxCommandEvent& WXUNUSED(event));
//
//    // Voice events
//    void OnComboVoice(wxCommandEvent& event);
//
//    //Help menu
//    void OnAbout(wxCommandEvent& WXUNUSED(event));
//    void OnHelpQuickGuide(wxCommandEvent& WXUNUSED(event));
//    void OnHelpOpen(wxCommandEvent& event);
//    void OnCheckForUpdates(wxCommandEvent& WXUNUSED(event));
//    void OnVisitWebsite(wxCommandEvent& WXUNUSED(event));
//
//    // Other menu items events
//    void OnOptions(wxCommandEvent& WXUNUSED(event));
//    void OnOpenBook(wxCommandEvent& event);
//    void OnOpenBookUI(wxUpdateUIEvent& event);
//
//    //other even managers
//    void OnMetronomeTimer(wxTimerEvent& event);
//    void OnMetronomeOnOff(wxCommandEvent& WXUNUSED(event));
//    void OnMetronomeUpdate(wxSpinEvent& WXUNUSED(event));
//    void OnMetronomeUpdateText(wxCommandEvent& WXUNUSED(event));
//    void OnPaneClose(wxAuiManagerEvent& event);
//    void OnKeyPress(wxKeyEvent& event);
//	void OnKeyF1(wxCommandEvent& event);
//
//
//    //textbook events and methods
//    void OnBookFrame(wxCommandEvent& event);
//    void OnBookFrameUpdateUI(wxUpdateUIEvent& event);
//    void OnCloseBookFrame();
//
//    //other events
//    void OnCloseWindow(wxCloseEvent& event);
//    void OnSize(wxSizeEvent& event);
//
//    // other methods
//    void SetOpenHelpButton(bool fButtonPressed);
//    void UpdateToolbarsLayout();
//    void SilentlyCheckForUpdates(bool fSilent);
//	inline wxMenu* GetEditMenu() {return m_editMenu; }
//    void NewScoreWindow(lmEditorMode* pMode, lmScore* pScore);
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
//    void DumpScore(lmScore* pScore);
//
//	//access to information
//    inline lmTextBookController* GetBookController() { return m_pBookController; }
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
//    lmScore* GetActiveScore();
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

    //menu bar
    void create_menu_item(wxMenu* pMenu, int nId, const wxString& sItemName,
                          const wxString& sToolTip = _T(""),
                          wxItemKind nKind = wxITEM_NORMAL,
                          const wxString& sIconName = _T("empty") );

//    //status bar
//    void CreateTheStatusBar(int nType=0);
//    void DeleteTheStatusBar();
//
//
//
//
//    DECLARE_EVENT_TABLE()
};



}   // namespace lenmus

#endif    // __LENMUS_MAIN_FRAME_H__

