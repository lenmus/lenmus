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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "IdfyExerciseCtrol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "IdfyExerciseCtrol.h"
#include "UrlAuxCtrol.h"
#include "Constrains.h"
#include "Generators.h"
#include "../auxmusic/Conversion.h"
#include "../app/MainFrame.h"
#include "../html/TextBookController.h"
#include "../ldp_parser/LDPParser.h"
#include "../auxmusic/Interval.h"
#include "../app/DlgCfgIdfyChord.h"
#include "../auxmusic/ChordManager.h"


#include "../globals/Colors.h"
extern lmColors* g_pColors;

// access to global external variables
extern bool g_fReleaseVersion;          // in TheApp.cpp
extern bool g_fReleaseBehaviour;        // in TheApp.cpp
extern bool g_fShowDebugLinks;          // in TheApp.cpp
extern bool g_fAutoNewProblem;          // in Preferences.cpp



//--------------------------------------------------------------------------------
// Implementation of abstract class for ear exercises constraints
//--------------------------------------------------------------------------------
lmIdfyConstrains::lmIdfyConstrains(wxString sSection)
{
    m_sSection = sSection;
    m_fSettingsLink = false;
    m_sGoBackURL = _T("");
    LoadSettings();
}


//------------------------------------------------------------------------------------
// Implementation of class for ear exercises controls
//------------------------------------------------------------------------------------


//IDs for controls
enum {
    ID_LINK_SEE_SOURCE = 3000,
    ID_LINK_DUMP,
    ID_LINK_MIDI_EVENTS,
    ID_LINK_NEW_PROBLEM,
    ID_LINK_PLAY,
    ID_LINK_SOLUTION,
    ID_LINK_SETTINGS,
    ID_LINK_GO_BACK

};


BEGIN_EVENT_TABLE(lmIdfyExerciseCtrol, wxWindow)
    EVT_SIZE            (lmIdfyExerciseCtrol::OnSize)
    //EVT_PAINT           (lmIdfyExerciseCtrol::OnPaint)

    LM_EVT_URL_CLICK    (ID_LINK_SEE_SOURCE, lmIdfyExerciseCtrol::OnDebugShowSourceScore)
    LM_EVT_URL_CLICK    (ID_LINK_DUMP, lmIdfyExerciseCtrol::OnDebugDumpScore)
    LM_EVT_URL_CLICK    (ID_LINK_MIDI_EVENTS, lmIdfyExerciseCtrol::OnDebugShowMidiEvents)

    LM_EVT_URL_CLICK    (ID_LINK_NEW_PROBLEM, lmIdfyExerciseCtrol::OnNewProblem)
    LM_EVT_URL_CLICK    (ID_LINK_PLAY, lmIdfyExerciseCtrol::OnPlay)
    LM_EVT_URL_CLICK    (ID_LINK_SOLUTION, lmIdfyExerciseCtrol::OnDisplaySolution)
    LM_EVT_URL_CLICK    (ID_LINK_SETTINGS, lmIdfyExerciseCtrol::OnSettingsButton)
    LM_EVT_URL_CLICK    (ID_LINK_GO_BACK, lmIdfyExerciseCtrol::OnGoBackButton)

    LM_EVT_END_OF_PLAY  (lmIdfyExerciseCtrol::OnEndOfPlay)

END_EVENT_TABLE()

IMPLEMENT_CLASS(lmIdfyExerciseCtrol, wxWindow)

lmIdfyExerciseCtrol::lmIdfyExerciseCtrol(wxWindow* parent, wxWindowID id, 
                           lmIdfyConstrains* pConstrains, wxSize nScoreSize,
                           const wxPoint& pos, const wxSize& size, int style)
    : wxWindow(parent, id, pos, size, style )
{
    //initializations
    SetBackgroundColour(*wxWHITE);
    m_nScoreSize = nScoreSize;
    m_nNumButtons = 0;
    m_fQuestionAsked = false;
    m_pProblemScore = (lmScore*)NULL;
	m_pSolutionScore = (lmScore*)NULL;
    m_pAuxScore = (lmScore*)NULL;
    m_pScoreCtrol = (lmScoreAuxCtrol*)NULL;
    m_pConstrains = pConstrains;
    m_fTheoryMode = m_pConstrains->IsTheoryMode();
    m_nPlayMM = 320;    //it is assumed whole notes
    m_fPlaying = false;
    m_fControlsCreated = false;
}

void lmIdfyExerciseCtrol::CreateControls()
{
    // This is an wxHtmlWidgetsCell. Therefore the window is first created (when the cell
    // is parsed and created) and later, it is displayed (when all html parsing is finished
    // and the html page is rendered. 
    // There is a broblem with this: as the control is buid on a wxWindow, the window
    // gets displayed as soon as it is created. This takes place at html parsing time,
    // when the lmIdfyExerciseCtrol is created. This early disply causes a flicker as
    // it is not displayed in the right position.
    // I don't know how to solve this. I have tried to delay the control creation
    // but without succes.
    // TODO:
    // Probably the best solution would be to abandom the idea of inserting wxHtmlWidgetCell
    // for the controls and creating my own html cells. 
    


    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    InitializeStrings();

    //the window is divided into two regions: top, for score on left and counters and links
    //on the right, and bottom region, for answer buttons 
    m_pMainSizer = new wxBoxSizer( wxVERTICAL );

        //
        // settings and debug options
        //
    wxBoxSizer* pTopLineSizer = new wxBoxSizer( wxHORIZONTAL );
    m_pMainSizer->Add(pTopLineSizer, wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 5));

    // settings link
    if (m_pConstrains->IncludeSettingsLink()) {
        lmUrlAuxCtrol* pSettingsLink = new lmUrlAuxCtrol(this, ID_LINK_SETTINGS, _("Settings") );
        pTopLineSizer->Add(pSettingsLink, wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 10) );
    }
    // "Go back to theory" link
    if (m_pConstrains->IncludeGoBackLink()) {
        lmUrlAuxCtrol* pGoBackLink = new lmUrlAuxCtrol(this, ID_LINK_GO_BACK, _("Go back to theory") );
        pTopLineSizer->Add(pGoBackLink, wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 10) );
    }

    // debug links
    if (g_fShowDebugLinks && !g_fReleaseVersion)
    {
        // "See source score"
        pTopLineSizer->Add(
            new lmUrlAuxCtrol(this, ID_LINK_SEE_SOURCE, _("See source score") ),
            wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 10) );

        // "Dump score"
        pTopLineSizer->Add(
            new lmUrlAuxCtrol(this, ID_LINK_DUMP, _("Dump score") ),
            wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 10) );

        // "See MIDI events"
        pTopLineSizer->Add(
            new lmUrlAuxCtrol(this, ID_LINK_MIDI_EVENTS, _("See MIDI events") ),
            wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 10) );
    }


    // sizer for the scoreCtrol and the CountersCtrol
    wxBoxSizer* pTopSizer = new wxBoxSizer( wxHORIZONTAL );
    m_pMainSizer->Add(
        pTopSizer,
        wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 10) );

    // create score ctrl 
    m_pScoreCtrol = new lmScoreAuxCtrol(this, -1, (lmScore*)NULL, wxDefaultPosition,
                                        m_nScoreSize, eSIMPLE_BORDER);
    m_pScoreCtrol->SetMargins(lmToLogicalUnits(5, lmMILLIMETERS),      //left=1cm
                              lmToLogicalUnits(5, lmMILLIMETERS),      //right=1cm
                              lmToLogicalUnits(10, lmMILLIMETERS));     //top=1cm
    pTopSizer->Add(
        m_pScoreCtrol,
        wxSizerFlags(1).Left().Border(wxTOP|wxBOTTOM, 10));

    m_pScoreCtrol->SetScale((float)1.3);


    // sizer for the CountersCtrol and the settings link
    wxBoxSizer* pCountersSizer = new wxBoxSizer( wxVERTICAL );
    pTopSizer->Add(
        pCountersSizer,
        wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 10).Expand() );

    // right/wrong answers counters control
    m_pCounters = new lmCountersCtrol(this, wxID_ANY);
    pCountersSizer->Add(
        m_pCounters,
        wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 10) );


        //
        // links 
        //

    wxBoxSizer* pLinksSizer = new wxBoxSizer( wxHORIZONTAL );
    m_pMainSizer->Add(
        pLinksSizer,
        wxSizerFlags(0).Center().Border(wxLEFT|wxALL, 10) );

    // "new problem" button
    pLinksSizer->Add(
        new lmUrlAuxCtrol(this, ID_LINK_NEW_PROBLEM, _("New problem") ),
        wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 20) );
    
    // "play" button
    m_pPlayButton = new lmUrlAuxCtrol(this, ID_LINK_PLAY, _("Play") );
    pLinksSizer->Add(
        m_pPlayButton,
        wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 20) );
    
    // "show solution" button
    m_pShowSolution = new lmUrlAuxCtrol(this, ID_LINK_SOLUTION, _("Show solution") );
    pLinksSizer->Add(
        m_pShowSolution,
        wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT|wxBOTTOM, 20) );
    
    //create buttons for the answers
    CreateAnswerButtons();

    //finish creation
    SetSizer( m_pMainSizer );                 // use the sizer for window layout
    m_pMainSizer->SetSizeHints( this );       // set size hints to honour minimum size
        //commented out. This sentece causes to display the window but, as the html cell is
        //being created, this causes a 'ghost' display in top left corner of the
        //html window

    m_pScoreCtrol->DisplayMessage(_("Click on 'New problem' to start"), 
                                  lmToLogicalUnits(10, lmMILLIMETERS),
                                  true);

    m_pPlayButton->Enable(false);
    m_pShowSolution->Enable(false);

    ReconfigureButtons();     //reconfigure buttons in accordance with constraints

    m_fControlsCreated = true;

}

lmIdfyExerciseCtrol::~lmIdfyExerciseCtrol()
{
    //stop any possible chord being played
    DoStopSounds();

    //delete objects

    if (m_pScoreCtrol) {
        delete m_pScoreCtrol;
        m_pScoreCtrol = (lmScoreAuxCtrol*)NULL;
    }

    if (m_pConstrains) {
        delete m_pConstrains;
        m_pConstrains = (lmIdfyConstrains*) NULL;
    }

    if (m_pProblemScore) {
        delete m_pProblemScore;
        m_pProblemScore = (lmScore*)NULL;
    }
    if (m_pSolutionScore) {
        delete m_pSolutionScore;
        m_pSolutionScore = (lmScore*)NULL;
    }
    if (m_pAuxScore) {
        delete m_pAuxScore;
        m_pAuxScore = (lmScore*)NULL;
    }
}

void lmIdfyExerciseCtrol::OnSettingsButton(wxCommandEvent& event)
{
    wxDialog* pDlg = GetSettingsDlg(); 
    if (pDlg) {
        int retcode = pDlg->ShowModal();
        if (retcode == wxID_OK) {
            m_pConstrains->SaveSettings();
            // When changing interval settings it is necessary review the buttons
            // as number of buttons and/or its name could have changed.
            ReconfigureButtons();
        }
        delete pDlg;
    }

}

void lmIdfyExerciseCtrol::OnGoBackButton(wxCommandEvent& event)
{
    //wxLogMessage(_T("[lmIdfyExerciseCtrol::OnGoBackButton] back URL = '%s'"), m_pConstrains->GetGoBackURL());
    lmMainFrame* pFrame = GetMainFrame();
    lmTextBookController* pBookController = pFrame->GetBookController();
    pBookController->Display( m_pConstrains->GetGoBackURL() );
}

void lmIdfyExerciseCtrol::OnSize(wxSizeEvent& event)
{
    //wxLogMessage(_T("OnSize at lmIdfyExerciseCtrol"));
    Layout();
}

void lmIdfyExerciseCtrol::OnPlay(wxCommandEvent& event)
{
    Play();
}

void lmIdfyExerciseCtrol::OnEndOfPlay(lmEndOfPlayEvent& WXUNUSED(event))
{
    m_pPlayButton->SetLabel(_("Play"));
    m_fPlaying = false;
}

void lmIdfyExerciseCtrol::OnNewProblem(wxCommandEvent& event)
{
    NewProblem();
}

void lmIdfyExerciseCtrol::OnDisplaySolution(wxCommandEvent& event)
{
    //First, stop any possible chord being played to avoid crashes
    DoStopSounds();

    //now proceed
    m_pCounters->IncrementWrong();
    DisplaySolution();
}

void lmIdfyExerciseCtrol::OnRespButton(wxCommandEvent& event)
{
    //First, stop any possible chord being played to avoid crashes
    DoStopSounds();

    //identify button pressed
    int nIndex = event.GetId() - m_nIdFirstButton;

    if (m_fQuestionAsked)
    {
        // There is a question asked. The user press the button to give the answer

        //verify if success or failure
        bool fSuccess = (nIndex == m_nRespIndex);
        
        //produce feedback sound, and update counters
        if (fSuccess) {
            m_pCounters->IncrementRight();
        } else {
            m_pCounters->IncrementWrong();
        }
            
        //if failure or not auto-new problem, display the solution.
        //Else, if success and auto-new problem, generate a new problem
        if (!fSuccess || !g_fAutoNewProblem) {
            if (!fSuccess) {
                //failure: mark wrong button in red and right one in green
                SetButtonColor(m_nRespIndex, g_pColors->Success() );
                SetButtonColor(nIndex, g_pColors->Failure() );
            }

            //show the solucion
            DisplaySolution();
       }
        else {
            NewProblem();
        }
    }
    else {
        // No problem presented. The user press the button to play a specific 
        // sound (chord, interval, scale, etc.)

        //prepare the score with the requested sound and play it
        PrepareAuxScore(nIndex);
        if (m_pAuxScore) {
            m_pAuxScore->Play(lmNO_VISUAL_TRACKING, NO_MARCAR_COMPAS_PREVIO,
                                ePM_NormalInstrument, 320, (wxWindow*) NULL);
        }
    }

}

void lmIdfyExerciseCtrol::NewProblem()
{
    ResetExercise();

    //prepare answer buttons and counters
    m_pCounters->NextTeam();
    EnableButtons(true);

    //set m_pProblemScore, m_pSolutionScore, m_sAnswer, m_nRespIndex, m_nPlayMM
    wxString sProblemMessage = SetNewProblem();    

    //load total score into the control
    m_pScoreCtrol->SetScore(m_pProblemScore, true);   //true: the score must be hidden
    m_pScoreCtrol->DisplayMessage(_T(""), 0, true);     //true: clear the canvas

    //display the problem
    if (m_fTheoryMode) {
        //theory
        m_pScoreCtrol->DisplayScore(m_pProblemScore);
        m_pScoreCtrol->DisplayMessage(sProblemMessage, lmToLogicalUnits(5, lmMILLIMETERS), false);
    } 
    else {
        //ear training
        Play();
        m_pScoreCtrol->DisplayMessage(sProblemMessage, lmToLogicalUnits(5, lmMILLIMETERS), false);
    }

    m_fQuestionAsked = true;
    m_pPlayButton->Enable(true);
    m_pShowSolution->Enable(true);

}

void lmIdfyExerciseCtrol::Play()
{
    if (!m_fPlaying) {
        // Play button pressed
        //m_pProblemScore->Dump(_T("lmIdfyExerciseCtrol.Play.ScoreDump.txt"));  //dbg

        //change link from "Play" to "Stop"
        m_pPlayButton->SetLabel(_("Stop"));

        //play the score
        m_pScoreCtrol->PlayScore(lmVISUAL_TRACKING, NO_MARCAR_COMPAS_PREVIO, 
                                ePM_NormalInstrument, m_nPlayMM);
        m_fPlaying = true;

        //! AWARE The link label is restored to "Play" when the EndOfPlay event is
        //! received. Flag m_fPlaying is also reset there
    }
    else {
        // "Stop" button pressed
        m_pScoreCtrol->Stop();
    }
}

void lmIdfyExerciseCtrol::DisplaySolution()
{
    DoStopSounds();

    //show the score
	if (m_pSolutionScore) {
		//There is a solution score. Display it
		m_pScoreCtrol->SetScore(m_pSolutionScore);
	}
	else {
		//No solution score. Display problem score
		m_pScoreCtrol->HideScore(false);
	}
    m_pScoreCtrol->DisplayMessage(m_sAnswer, lmToLogicalUnits(5, lmMILLIMETERS), false);

    // mark right button in green
    SetButtonColor(m_nRespIndex, g_pColors->Success());
    
    m_pPlayButton->Enable(true);
    m_pShowSolution->Enable(false);
    m_fQuestionAsked = false;
    if (m_fTheoryMode) EnableButtons(false);

}

void lmIdfyExerciseCtrol::OnDebugShowSourceScore(wxCommandEvent& event)
{
    m_pScoreCtrol->SourceLDP();
}

void lmIdfyExerciseCtrol::OnDebugDumpScore(wxCommandEvent& event)
{
    m_pScoreCtrol->Dump();
}

void lmIdfyExerciseCtrol::OnDebugShowMidiEvents(wxCommandEvent& event)
{
    m_pScoreCtrol->DumpMidiEvents();
}

void lmIdfyExerciseCtrol::ResetExercise()
{
    DoStopSounds();

    //clear the canvas
    m_pScoreCtrol->DisplayMessage(_T(""), 0, true);     //true: clear the canvas
    m_pScoreCtrol->Update();    //to force to clear it now

    // restore buttons' normal color
    for (int iB=0; iB < m_nNumButtons; iB++) {
        SetButtonColor(iB, g_pColors->Normal() );
    }

    //delete the previous score
    if (m_pProblemScore) {
        delete m_pProblemScore;
        m_pProblemScore = (lmScore*)NULL;
    }

}

void lmIdfyExerciseCtrol::EnableButtons(bool fEnable)
{
    wxButton* pButton;
    for (int iB=0; iB < m_nNumButtons; iB++) {
        pButton = *(m_pAnswerButtons + iB);
        if (pButton) pButton->Enable(fEnable);
    }
}

void lmIdfyExerciseCtrol::SetButtons(wxButton* pButtons[], int nNumButtons, int nIdFirstButton)
{
    m_pAnswerButtons = pButtons;
    m_nIdFirstButton = nIdFirstButton;
    m_nNumButtons = nNumButtons;
}

void lmIdfyExerciseCtrol::SetButtonColor(int i, wxColour& color)
{
    wxButton* pButton = *(m_pAnswerButtons + i);
    if (pButton) pButton->SetBackgroundColour(color);
}

void lmIdfyExerciseCtrol::DoStopSounds()
{
    //Stop any possible chord being played to avoid crashes
    if (m_pAuxScore) m_pAuxScore->Stop();
    if (m_pProblemScore) m_pProblemScore->Stop();
    if (m_pSolutionScore) m_pSolutionScore->Stop();

    //Require to do it for other scores in the derived class
    StopSounds();       

}

// Repainting behaviour
//void lmIdfyExerciseCtrol::OnPaint(wxPaintEvent& event)
//{
//    // In a paint event handler, the application must always create a wxPaintDC object,
//    // even if it is not used. Otherwise, under MS Windows, refreshing for this and
//    // other windows will go wrong.
//    wxPaintDC dc(this);
//
//    if (!m_fControlsCreated) {
//        CreateControls();
//        OnStartingExercise();       //inform derived class that it is going to be displayed
//    }
//
//    wxWindow::OnPaint(event);
//
//}
