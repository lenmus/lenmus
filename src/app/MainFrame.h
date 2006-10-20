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
/*! @file MainFrame.h
    @brief Header file for class lmMainFrame
    @ingroup app_gui
*/
#ifdef __GNUG__
// #pragma interface "TheApp.h"
#endif

#ifndef __MYMAINFRAME_H_
#define __MYMAINFRAME_H_

#include "wx/mdi.h"
#include "wx/docview.h"
#include "wx/docmdi.h"
#include "global.h"
#include "ToolsDlg.h"
#include "ScoreCanvas.h"

#include "wx/wizard.h"          //to use wxWizard classes
#include "wx/timer.h"           //to use wxTimer
#include "wx/spinctrl.h"        //to use spin control
#include "wx/html/htmlwin.h"    //to use html

#include "../sound/Metronome.h"

// to use html help controller
#include "wx/html/helpctrl.h"
#include "../html/TextBookController.h"
#include "../html/HelpController.h"

#include "wx/numdlg.h"

//wxAUI
#include "manager.h"

//IDs for menu events that must be public (i.e. to generate them by program)
enum
{
    MENU_CheckForUpdates = 1100,
    MENU_Last_Public_ID
};


// Class lmMainFrame defines the main MDI frame for the application
class lmMainFrame: public wxDocMDIParentFrame
{
    DECLARE_CLASS(lmMainFrame)
public:
    wxLocale*   m_pLocale;            // locale for internationalization
  
    //constructors and destructor
    lmMainFrame(wxDocManager *manager, wxFrame *frame, const wxString& title, 
            const wxPoint& pos, const wxSize& size, long type);
    ~lmMainFrame();

    // tool bars
    void CreateMyToolBar();
    void DeleteToolbar();
    void CreateTextBooksToolBar(long style, wxSize nIconSize);

    //status bar
    void CreateMyStatusBar ();
    void DeleteStatusBar ();

    // menu bar
    wxMenuBar* CreateMenuBar(wxDocument* doc, wxView* view, bool fEdit, bool fDebug);

    // metronome
    void SetMetronome(lmMetronome* pMtr);
    lmMetronome* GetMetronome() { return m_pMtr; }

    // File menu events
    void OnImportFile(wxCommandEvent& WXUNUSED(event));
    void OnExportBMP(wxCommandEvent& WXUNUSED(event));
    void OnExportJPG(wxCommandEvent& WXUNUSED(event));
    void OnPrintPreview(wxCommandEvent& WXUNUSED(event));
    //void OnPageSetup(wxCommandEvent& WXUNUSED(event));
    void OnPrintSetup(wxCommandEvent& WXUNUSED(event));
    void OnPrint(wxCommandEvent& event);
    void OnFileUpdateUI(wxUpdateUIEvent &event);

    void ExportAsImage(int nImgType);


    // Edit menu events
    void OnEditUpdateUI(wxUpdateUIEvent &event);

    // Debug menu events
        // general options, always enabled
    void OnDebugForceReleaseBehaviour(wxCommandEvent& event);
    void OnDebugShowDebugLinks(wxCommandEvent& event);
    void OnDebugRecSelec(wxCommandEvent& event);
    void OnDebugTestMidi(wxCommandEvent& event);
    void OnDebugSetTraceLevel(wxCommandEvent& WXUNUSED(event));
    void OnDebugPatternEditor(wxCommandEvent& WXUNUSED(event));
    void OnDebugUnitTests(wxCommandEvent& event);
    void OnDebugUseAntiAliasing(wxCommandEvent& event);
        // methods requiring a score
    void OnDebugDumpBitmaps(wxCommandEvent& event);
    void OnDebugDumpStaffObjs(wxCommandEvent& event);
    void OnDebugSeeSource(wxCommandEvent& event);
    void OnDebugSeeXML(wxCommandEvent& event);
    void OnDebugSeeMidiEvents(wxCommandEvent& WXUNUSED(event));
    void OnDebugScoreUI(wxUpdateUIEvent& event);

    // Zoom events
    void OnComboZoom(wxCommandEvent& event);
    void OnZoom(wxCommandEvent& event, int nZoom);
    void OnZoom100(wxCommandEvent& event) { OnZoom(event, 100); }
    void OnZoomFitWidth(wxCommandEvent& event);
    void OnZoomFitFull(wxCommandEvent& event);
    void OnZoomOther(wxCommandEvent& event);
    void OnZoomUpdateUI(wxUpdateUIEvent &event);


    // View menu events
    void OnViewTools(wxCommandEvent& WXUNUSED(event));
    void OnViewRulers(wxCommandEvent& event);
    void OnViewRulersUI(wxUpdateUIEvent &event);
    void OnViewToolBar(wxCommandEvent& WXUNUSED(event));
    void OnViewStatusBar(wxCommandEvent& WXUNUSED(event));
    void OnToolbarsUI(wxUpdateUIEvent &event);
    void OnStatusbarUI(wxUpdateUIEvent &event);

    // Sound menu events
    void OnSoundUpdateUI(wxUpdateUIEvent &event);
    void OnSoundTest(wxCommandEvent& WXUNUSED(event));
    void OnAllSoundsOff(wxCommandEvent& WXUNUSED(event));
    void OnRunMidiWizard(wxCommandEvent& WXUNUSED(event));
    //void OnMidiWizardCancel(wxWizardEvent& event);
    //void OnMidiWizardFinished(wxWizardEvent& event);
    void DoRunMidiWizard();
    void OnPlayStart(wxCommandEvent& WXUNUSED(event));
    void OnPlayStop(wxCommandEvent& WXUNUSED(event));
    void OnPlayPause(wxCommandEvent& WXUNUSED(event));
    void OnPlayUI(wxUpdateUIEvent &event);

    // Other menu items events
    void OnOptions(wxCommandEvent& WXUNUSED(event));
    void OnOpenBook(wxCommandEvent& event);
    void OnOpenBookUI(wxUpdateUIEvent &event);
    void OnAbout(wxCommandEvent& WXUNUSED(event));
    void OnOpenHelp(wxCommandEvent& event);
    void OnOpenHelpUI(wxUpdateUIEvent& event);
    void OnCheckForUpdates(wxCommandEvent& WXUNUSED(event));
    void OnVisitWebsite(wxCommandEvent& WXUNUSED(event));

    //other even managers
    void OnMetronomeTimer(wxTimerEvent& event);
    void OnMetronomeOnOff(wxCommandEvent& WXUNUSED(event));
    void OnMetronomeUpdate(wxSpinEvent& WXUNUSED(event));
    void OnMetronomeUpdateText(wxCommandEvent& WXUNUSED(event));

    //textbook events
    void OnBookFrame(wxCommandEvent& event);
    void OnBookFrameUpdateUI(wxUpdateUIEvent& event);

    // other methods
    void SetOpenBookButton(bool fButtonPressed);
    void SetOpenHelpButton(bool fButtonPressed);
    void UpdateToolbarsLayout();
    void SilentlyCheckForUpdates(bool fSilent);

    //options
    bool ShowRulers();


    //eBooks controller
    void SetHtmlWindow(lmHtmlWindow* pHtmlWin) { m_pHtmlWin = pHtmlWin; }
    lmHtmlWindow* GetHtmlWindow() { return m_pHtmlWin; }

    //debug method
    void DumpScore(lmScore* pScore);


    lmTextBookController* GetBookController() { return m_pBookController; }


protected:
    void InitializeHelp();
    void InitializeBooks();
    void ScanForBooks(wxString sPath, wxString sPattern);



    wxFrameManager          m_mgrAUI;       // wxAUI manager

    lmToolsDlg*             m_pToolsDlg;
    lmTextBookController*   m_pBookController;
    lmHtmlWindow*           m_pHtmlWin;
    lmHelpController*       m_pHelp;
    wxSpinCtrl*             m_pSpinMetronome;
    wxComboBox*             m_pComboZoom;

    lmMetronome*        m_pMainMtr;        //independent metronome
    lmMetronome*        m_pMtr;            //metronome currently associated to frame metronome controls

    //flags for toggle buttons/menus
    bool m_fBookOpened;
    bool m_fHelpOpened;

    // tool bars
    wxToolBar*      m_pTbFile;          // file toolbar
    wxToolBar*      m_pTbEdit;          // edit toolbar
    wxToolBar*      m_pTbZoom;          // zoom toolbar
    wxToolBar*      m_pTbPlay;          // play toolbar
    wxToolBar*      m_pTbMtr;           // metronome toolbar
    wxToolBar*      m_pToolbar;         // main toolbar
    wxToolBar*      m_pTbTextBooks;     // text books navigation toolbar

    // status bar
    wxStatusBar*    m_pStatusbar;

    bool    m_fSilentCheck;


    DECLARE_EVENT_TABLE()

};


#endif    // __MYMAINFRAME_H_
