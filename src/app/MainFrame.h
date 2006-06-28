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
    wxLocale& m_locale;            // locale for internationalization
  
    //constructors and destructor
    lmMainFrame(wxDocManager *manager, wxFrame *frame, const wxString& title, 
            const wxPoint& pos, const wxSize& size, long type, wxLocale& m_locale);
    ~lmMainFrame();

    // tool bars
    void CreateMyToolBar();
    void DeleteToolbar();
    void CreateNavigationToolBar();

    //status bar
    void CreateMyStatusBar ();
    void DeleteStatusBar ();

    // menu bar
    wxMenuBar* CreateMenuBar(wxDocument* doc, wxView* view, bool fEdit, bool fDebug);

    // metronome
    void SetMetronome(lmMetronome* pMtr);
    lmMetronome* GetMetronome() { return m_pMtr; }

    // to process menu events
    void OnImportFile(wxCommandEvent& WXUNUSED(event));
    void OnOpenBook(wxCommandEvent& event);
    void OnOpenBookUI(wxUpdateUIEvent &event);

    void OnDebugForceReleaseBehaviour(wxCommandEvent& event);
    void OnDebugShowDebugLinks(wxCommandEvent& event);
    void OnDebugRecSelec(wxCommandEvent& event);
    void OnDebugTestMidi(wxCommandEvent& event);
    void OnDebugSetTraceLevel(wxCommandEvent& WXUNUSED(event));
    void OnDebugPatternEditor(wxCommandEvent& WXUNUSED(event));
    void OnDebugDumpStaffObjs(wxCommandEvent& event);
    void OnDebugDumpStaffObjsUI(wxUpdateUIEvent& event);
    void OnDebugSeeSource(wxCommandEvent& event);
    void OnDebugSeeSourceUI(wxUpdateUIEvent& event);
    void OnDebugSeeXML(wxCommandEvent& event);
    void OnDebugSeeXMLUI(wxUpdateUIEvent& event);
    void OnDebugSeeMidiEvents(wxCommandEvent& WXUNUSED(event));
    void OnDebugSeeMidiEventsUI(wxUpdateUIEvent& event);

    void OnComboZoom(wxCommandEvent& event);
    void OnZoom(wxCommandEvent& event, int nZoom);
    void OnZoom75(wxCommandEvent& event) { OnZoom(event, 75); }
    void OnZoom100(wxCommandEvent& event) { OnZoom(event, 100); }
    void OnZoom150(wxCommandEvent& event) { OnZoom(event, 150); }
    void OnZoom200(wxCommandEvent& event); // { OnZoom(event, 200); }

    void OnViewTools(wxCommandEvent& WXUNUSED(event));
    void OnViewRulers(wxCommandEvent& event);
    void OnViewToolBar(wxCommandEvent& WXUNUSED(event));
    void OnViewStatusBar(wxCommandEvent& WXUNUSED(event));
    void OnToolbarsUI(wxUpdateUIEvent &event);
    void OnStatusbarUI(wxUpdateUIEvent &event);

    void OnPrintPreview(wxCommandEvent& WXUNUSED(event));
    void OnPrintPreviewUI(wxUpdateUIEvent &event);
    //void OnPageSetup(wxCommandEvent& WXUNUSED(event));
    void OnPrintSetup(wxCommandEvent& WXUNUSED(event));
    void OnPrint(wxCommandEvent& WXUNUSED(event));
    void OnPrintUI(wxUpdateUIEvent &event);

    void OnPlayStart(wxCommandEvent& WXUNUSED(event));
    void OnPlayStop(wxCommandEvent& WXUNUSED(event));
    void OnPlayPause(wxCommandEvent& WXUNUSED(event));

    void OnOptions(wxCommandEvent& WXUNUSED(event));

    void OnSoundTest(wxCommandEvent& WXUNUSED(event));
    void OnAllSoundsOff(wxCommandEvent& WXUNUSED(event));
    void OnRunMidiWizard(wxCommandEvent& WXUNUSED(event));
    //void OnMidiWizardCancel(wxWizardEvent& event);
    //void OnMidiWizardFinished(wxWizardEvent& event);
    void DoRunMidiWizard();

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


    // other methods
    void SetOpenBookButton(bool fButtonPressed);
    void SetOpenHelpButton(bool fButtonPressed);
    void UpdateMenuAndToolbar();
    void UpdateToolbarsLayout();
    void SilentlyCheckForUpdates(bool fSilent);


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


    lmToolsDlg*             m_pToolsDlg;
    lmTextBookController*   m_pBookController;
    lmHtmlWindow*           m_pHtmlWin;
    lmHelpController*       m_pHelp;
    wxSpinCtrl*             m_pSpinMetronome;

    lmMetronome*        m_pMainMtr;        //independent metronome
    lmMetronome*        m_pMtr;            //metronome currently associated to frame metronome controls

    //flags for toggle buttons/menus
    bool m_fBookOpened;
    bool m_fHelpOpened;

    // tool abr, status bar
    wxToolBar*      m_pToolbar;
    wxStatusBar*    m_pStatusbar;
    wxToolBar*      m_pNavigationToolbar;

    bool    m_fSilentCheck;


    DECLARE_EVENT_TABLE()

};


#endif    // __MYMAINFRAME_H_
