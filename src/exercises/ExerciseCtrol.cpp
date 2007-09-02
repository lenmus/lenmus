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
#pragma implementation "ExerciseCtrol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "ExerciseCtrol.h"
#include "UrlAuxCtrol.h"
#include "Constrains.h"
#include "Generators.h"
#include "../app/MainFrame.h"
#include "../html/TextBookController.h"

#include "wx/textctrl.h"


#include "../globals/Colors.h"
extern lmColors* g_pColors;

// access to global external variables
extern bool g_fReleaseVersion;          // in TheApp.cpp
extern bool g_fReleaseBehaviour;        // in TheApp.cpp
extern bool g_fShowDebugLinks;          // in TheApp.cpp
extern bool g_fAutoNewProblem;          // in Preferences.cpp



//--------------------------------------------------------------------------------
// Implementation of abstract class for lmExerciseCtrol constraints
//--------------------------------------------------------------------------------
lmExerciseConstrains::lmExerciseConstrains(wxString sSection)
{
    m_sSection = sSection;
    m_fSettingsLink = false;
    m_sGoBackURL = _T("");
    m_fButtonsEnabledAfterSolution = true;
    LoadSettings();
}


//------------------------------------------------------------------------------------
// Implementation of lmExerciseCtrol:
// An abstract class to create exercises
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


BEGIN_EVENT_TABLE(lmExerciseCtrol, wxWindow)
    EVT_SIZE            (lmExerciseCtrol::OnSize)

    LM_EVT_URL_CLICK    (ID_LINK_SEE_SOURCE, lmExerciseCtrol::OnDebugShowSourceScore)
    LM_EVT_URL_CLICK    (ID_LINK_DUMP, lmExerciseCtrol::OnDebugDumpScore)
    LM_EVT_URL_CLICK    (ID_LINK_MIDI_EVENTS, lmExerciseCtrol::OnDebugShowMidiEvents)

    LM_EVT_URL_CLICK    (ID_LINK_NEW_PROBLEM, lmExerciseCtrol::OnNewProblem)
    LM_EVT_URL_CLICK    (ID_LINK_PLAY, lmExerciseCtrol::OnPlay)
    LM_EVT_URL_CLICK    (ID_LINK_SOLUTION, lmExerciseCtrol::OnDisplaySolution)
    LM_EVT_URL_CLICK    (ID_LINK_SETTINGS, lmExerciseCtrol::OnSettingsButton)
    LM_EVT_URL_CLICK    (ID_LINK_GO_BACK, lmExerciseCtrol::OnGoBackButton)

END_EVENT_TABLE()

IMPLEMENT_CLASS(lmExerciseCtrol, wxWindow)

lmExerciseCtrol::lmExerciseCtrol(wxWindow* parent, wxWindowID id, 
                           lmExerciseConstrains* pConstrains, wxSize nDisplaySize,
                           const wxPoint& pos, const wxSize& size, int style)
    : wxWindow(parent, id, pos, size, style )
{
    //initializations
    SetBackgroundColour(*wxWHITE);
    m_nDisplaySize = nDisplaySize;
    m_nNumButtons = 0;
    m_fQuestionAsked = false;
    m_pConstrains = pConstrains;
    m_fControlsCreated = false;
}

void lmExerciseCtrol::CreateControls()
{
    // This is an wxHtmlWidgetsCell. Therefore the window is first created (when the cell
    // is parsed and created) and later, it is displayed (when all html parsing is finished
    // and the html page is rendered. 
    // There is a broblem with this: as the control is buid on a wxWindow, the window
    // gets displayed as soon as it is created. This takes place at html parsing time,
    // when the lmExerciseCtrol is created. This early disply causes a flicker as
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

    // create the display control (ScoreAuxCtrol or wxTextCtrl)
    m_pDisplayCtrol = CreateDisplayCtrol();
    pTopSizer->Add(m_pDisplayCtrol,
                   wxSizerFlags(1).Left().Border(wxTOP|wxBOTTOM, 10));

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

    //show start message
    wxString sMsg = _("Click on 'New problem' to start");
    DisplayMessage(sMsg, true);

    // final buttons/links enable/setup
    m_pPlayButton->Enable(false);
    m_pShowSolution->Enable(false);

    ReconfigureButtons();     //reconfigure buttons in accordance with constraints

    m_fControlsCreated = true;

}

lmExerciseCtrol::~lmExerciseCtrol()
{
    //stop any possible chord being played
    StopSounds();

    //delete objects
    if (m_pConstrains) {
        delete m_pConstrains;
        m_pConstrains = (lmExerciseConstrains*) NULL;
    }
    DeleteScores();
}

void lmExerciseCtrol::DisplayMessage(wxString& sMsg, bool fClearDisplay)
{
    if (m_pDisplayCtrol->IsKindOf(CLASSINFO(lmScoreAuxCtrol))) {
        ((lmScoreAuxCtrol*)m_pDisplayCtrol)->DisplayMessage(
                sMsg, lmToLogicalUnits(5, lmMILLIMETERS), fClearDisplay);
    }
    else {
        ((wxTextCtrl*)m_pDisplayCtrol)->ChangeValue(sMsg);
    }
}

void lmExerciseCtrol::OnSettingsButton(wxCommandEvent& event)
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

void lmExerciseCtrol::OnGoBackButton(wxCommandEvent& event)
{
    lmMainFrame* pFrame = GetMainFrame();
    lmTextBookController* pBookController = pFrame->GetBookController();
    pBookController->Display( m_pConstrains->GetGoBackURL() );
}

void lmExerciseCtrol::OnSize(wxSizeEvent& event)
{
    Layout();
}

void lmExerciseCtrol::OnPlay(wxCommandEvent& event)
{
    Play();
}

void lmExerciseCtrol::OnNewProblem(wxCommandEvent& event)
{
    NewProblem();
}

void lmExerciseCtrol::OnDisplaySolution(wxCommandEvent& event)
{
    //First, stop any possible chord being played to avoid crashes
    StopSounds();

    //now proceed
    m_pCounters->IncrementWrong();
    DoDisplaySolution();
}

void lmExerciseCtrol::OnRespButton(wxCommandEvent& event)
{
    //First, stop any possible chord being played to avoid crashes
    StopSounds();

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
            DoDisplaySolution();
       }
        else {
            NewProblem();
        }
    }
    else {
        //TODO
        //// No problem presented. The user press the button to play a specific 
        //// sound (chord, interval, scale, etc.)

        ////prepare the score with the requested sound and play it
        //PrepareAuxScore(nIndex);
        //if (m_pAuxScore) {
        //    m_pAuxScore->Play(lmNO_VISUAL_TRACKING, NO_MARCAR_COMPAS_PREVIO,
        //                        ePM_NormalInstrument, 320, (wxWindow*) NULL);
        //}
    }

}

void lmExerciseCtrol::NewProblem()
{
    ResetExercise();

    //prepare answer buttons and counters
    m_pCounters->NextTeam();
    EnableButtons(true);

    //set m_pProblemScore, m_pSolutionScore, m_sAnswer, m_nRespIndex, m_nPlayMM
    wxString sProblemMessage = SetNewProblem();    

    //display the problem
    m_fQuestionAsked = true;
    DisplayProblem();
    DisplayMessage(sProblemMessage, false);
    m_pPlayButton->Enable(true);
    m_pShowSolution->Enable(true);

}

void lmExerciseCtrol::DoDisplaySolution()
{
    StopSounds();
    DisplaySolution();

    // mark right button in green
    SetButtonColor(m_nRespIndex, g_pColors->Success());
    
    m_pPlayButton->Enable(true);
    m_pShowSolution->Enable(false);
    m_fQuestionAsked = false;
    if (!m_pConstrains->ButtonsEnabledAfterSolution()) EnableButtons(false);

}

void lmExerciseCtrol::ResetExercise()
{
    StopSounds();

    //clear the display ctrol
    if (m_pDisplayCtrol->IsKindOf(CLASSINFO(lmScoreAuxCtrol)))
    {
        ((lmScoreAuxCtrol*)m_pDisplayCtrol)->DisplayMessage(_T(""), 0, true);     //true: clear the canvas
        ((lmScoreAuxCtrol*)m_pDisplayCtrol)->Update();    //to force to clear it now
    }
    else {
        ((wxTextCtrl*)m_pDisplayCtrol)->ChangeValue(_T(""));
    }


    // restore buttons' normal color
    for (int iB=0; iB < m_nNumButtons; iB++) {
        SetButtonColor(iB, g_pColors->Normal() );
    }

    DeleteScores();

}

void lmExerciseCtrol::EnableButtons(bool fEnable)
{
    wxButton* pButton;
    for (int iB=0; iB < m_nNumButtons; iB++) {
        pButton = *(m_pAnswerButtons + iB);
        if (pButton) pButton->Enable(fEnable);
    }
}

void lmExerciseCtrol::SetButtons(wxButton* pButtons[], int nNumButtons, int nIdFirstButton)
{
    m_pAnswerButtons = pButtons;
    m_nIdFirstButton = nIdFirstButton;
    m_nNumButtons = nNumButtons;
}

void lmExerciseCtrol::SetButtonColor(int i, wxColour& color)
{
    wxButton* pButton = *(m_pAnswerButtons + i);
    if (pButton) pButton->SetBackgroundColour(color);
}





//------------------------------------------------------------------------------------
// Implementation of lmCompareCtrol
//  A control with three answer buttons
//------------------------------------------------------------------------------------

IMPLEMENT_CLASS(lmCompareCtrol, lmExerciseCtrol)

static wxString m_sButtonLabel[lmCompareCtrol::m_NUM_BUTTONS];

BEGIN_EVENT_TABLE(lmCompareCtrol, lmExerciseCtrol)
    EVT_COMMAND_RANGE (m_ID_BUTTON, m_ID_BUTTON+m_NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmExerciseCtrol::OnRespButton)
END_EVENT_TABLE()


lmCompareCtrol::lmCompareCtrol(wxWindow* parent, wxWindowID id,
               lmExerciseConstrains* pConstrains, wxSize nDisplaySize, 
               const wxPoint& pos, const wxSize& size, int style)
    : lmExerciseCtrol(parent, id, pConstrains, nDisplaySize, pos, size, style )
{
}

void lmCompareCtrol::InitializeStrings()
{

    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    m_sButtonLabel[0] = _("First one greater");
    m_sButtonLabel[1] = _("Second one greater");
    m_sButtonLabel[2] = _("Both are equal");
}

void lmCompareCtrol::CreateAnswerButtons()
{

    //create buttons for the answers: three buttons in one row
    m_pKeyboardSizer = new wxFlexGridSizer(m_NUM_ROWS, m_NUM_COLS, 10, 0);
    m_pMainSizer->Add(
        m_pKeyboardSizer,
        wxSizerFlags(0).Left().Border(wxALIGN_LEFT|wxTOP, 10) );

    // the buttons 
    for (int iB=0; iB < m_NUM_COLS; iB++) {
        m_pAnswerButton[iB] = new wxButton( this, m_ID_BUTTON + iB, m_sButtonLabel[iB],
            wxDefaultPosition, wxSize(134, 24));
        m_pKeyboardSizer->Add(
            m_pAnswerButton[iB],
            wxSizerFlags(0).Border(wxLEFT|wxRIGHT, m_BUTTONS_DISTANCE) );
    }

    //inform base class about the settings
    SetButtons(m_pAnswerButton, m_NUM_BUTTONS, m_ID_BUTTON);

}

lmCompareCtrol::~lmCompareCtrol()
{
}




//------------------------------------------------------------------------------------
// Implementation of lmCompareScoresCtrol
//  A CompareCtrol with two scores
//------------------------------------------------------------------------------------

IMPLEMENT_CLASS(lmCompareScoresCtrol, lmCompareCtrol)

BEGIN_EVENT_TABLE(lmCompareScoresCtrol, lmCompareCtrol)
    LM_EVT_END_OF_PLAY  (lmCompareScoresCtrol::OnEndOfPlay)
    EVT_TIMER           (wxID_ANY, lmCompareScoresCtrol::OnTimerEvent)
END_EVENT_TABLE()

lmCompareScoresCtrol::lmCompareScoresCtrol(wxWindow* parent, wxWindowID id,
               lmExerciseConstrains* pConstrains, wxSize nDisplaySize, 
               const wxPoint& pos, const wxSize& size, int style)
    : lmCompareCtrol(parent, id, pConstrains, nDisplaySize, pos, size, style )
{
    //initializations
    m_pSolutionScore = (lmScore*)NULL;
    m_pScore[0] = (lmScore*)NULL;
    m_pScore[1] = (lmScore*)NULL;
    m_fPlaying = false;
    m_nPlayMM = 80;          //metronome default setting to play the scores

}

lmCompareScoresCtrol::~lmCompareScoresCtrol()
{
}

wxWindow* lmCompareScoresCtrol::CreateDisplayCtrol()
{
    // Using scores and ScoreAuxCtrol
    lmScoreAuxCtrol* pScoreCtrol = new lmScoreAuxCtrol(this, -1, (lmScore*)NULL, wxDefaultPosition,
                                        m_nDisplaySize, eSIMPLE_BORDER);
    pScoreCtrol->SetMargins(lmToLogicalUnits(5, lmMILLIMETERS),      //left=1cm
                            lmToLogicalUnits(5, lmMILLIMETERS),      //right=1cm
                            lmToLogicalUnits(10, lmMILLIMETERS));     //top=1cm

    pScoreCtrol->SetScale((float)1.3);
    return pScoreCtrol;
}

void lmCompareScoresCtrol::Play()
{
    if (!m_fPlaying) {
        // Play button pressed

        //change link from "Play" to "Stop"
        m_pPlayButton->SetLabel(_("Stop"));
        m_fPlaying = true;

        //AWARE: The link label is restored to "Play" when the EndOfPlay() event is
        //       received. Flag m_fPlaying is also reset there

        if (m_fQuestionAsked)
        {
            //Introducing the problem. Play the first score
            PlayScore(0);
            //AWARE:
            // when 1st score is finished an event will be generated. Then method 
            // OnEndOfPlay() will handle the event and play the second score.
            // It is programmed this way (asynchonously) to avoid crashes if program/exercise
            // is closed. 
        }
        else
        {
            //Playing the solution. Play total score
            ((lmScoreAuxCtrol*)m_pDisplayCtrol)->PlayScore(lmVISUAL_TRACKING,
                    NO_MARCAR_COMPAS_PREVIO, ePM_NormalInstrument, m_nPlayMM);
        }
    }
    else {
        // "Stop" button pressed. Inform that no more sound is desired
        m_fPlaying = false;
    }

}

void lmCompareScoresCtrol::PlayScore(int nIntv)
{
    m_nNowPlaying = nIntv;
    m_pAnswerButton[nIntv]->SetBackgroundColour( g_pColors->ButtonHighlight() );
    m_pAnswerButton[nIntv]->Update();    //Refresh works by events and, so, it is not inmediate

    //AWARE: As the intervals are built using whole notes, we will play them at
    // MM=320 so that real note rate will be 80.
    m_pScore[nIntv]->Play(lmNO_VISUAL_TRACKING, NO_MARCAR_COMPAS_PREVIO, 
                             ePM_NormalInstrument, 320, this);

}

void lmCompareScoresCtrol::OnEndOfPlay(lmEndOfPlayEvent& WXUNUSED(event))
{
    if (m_fQuestionAsked)
    {
        //if introducing the problem remove highlight in button
        m_pAnswerButton[m_nNowPlaying]->SetBackgroundColour( g_pColors->Normal() );
        m_pAnswerButton[m_nNowPlaying]->Update();

        if (m_nNowPlaying == 0 && m_fPlaying) {
            //wxLogMessage(_T("EndOfPlay event: Starting timer"));
            m_oPauseTimer.SetOwner( this, wxID_ANY );
            m_oPauseTimer.Start(1000, wxTIMER_CONTINUOUS );     //wait for 1sec (1000ms)
        }
        else {
            //wxLogMessage(_T("EndOfPlay event: play stopped"));
            m_fPlaying = false;
            m_pPlayButton->SetLabel(_("Play"));
        }
    }
    else
    {
        //playing after solution is displayed: just change link label
        m_fPlaying = false;
        m_pPlayButton->SetLabel(_("Play"));
    }

}

void lmCompareScoresCtrol::OnTimerEvent(wxTimerEvent& WXUNUSED(event))
{
    m_oPauseTimer.Stop();
    if (m_fPlaying) {
        //wxLogMessage(_T("Timer event: play(1)"));
        PlayScore(1);
    }
    else {
        //wxLogMessage(_T("Timer event: play stopped"));
        m_pPlayButton->SetLabel(_("Play"));
    }
}

void lmCompareScoresCtrol::DisplaySolution()
{
    //show the solution score
	if (m_pSolutionScore) {
		//There must be a solution score. Display it
		((lmScoreAuxCtrol*)m_pDisplayCtrol)->SetScore(m_pSolutionScore);
	}
    ((lmScoreAuxCtrol*)m_pDisplayCtrol)->DisplayMessage(
                    m_sAnswer, lmToLogicalUnits(5, lmMILLIMETERS), false);
}

void lmCompareScoresCtrol::DisplayProblem()
{
    if (m_pConstrains->IsTheoryMode()) {
        //TODO
        ////theory
        //m_pDisplayCtrol->DisplayScore(m_pProblemScore);
    } 
    else {
        //ear training
        Play();
    }
}

void lmCompareScoresCtrol::DeleteScores()
{
    if (m_pSolutionScore) {
        delete m_pSolutionScore;
        m_pSolutionScore = (lmScore*)NULL;
    }
    if (m_pScore[0]) {
        delete m_pScore[0];
        m_pScore[0] = (lmScore*)NULL;
    }
    if (m_pScore[1]) {
        delete m_pScore[1];
        m_pScore[1] = (lmScore*)NULL;
    }
}

void lmCompareScoresCtrol::StopSounds()
{
    //Stop any possible score being played to avoid crashes
    if (m_pScore[0]) m_pScore[0]->Stop();
    if (m_pScore[1]) m_pScore[1]->Stop();
    if (m_pSolutionScore) m_pSolutionScore->Stop();

}

void lmCompareScoresCtrol::OnDebugShowSourceScore(wxCommandEvent& event)
{
    ((lmScoreAuxCtrol*)m_pDisplayCtrol)->SourceLDP();
}

void lmCompareScoresCtrol::OnDebugDumpScore(wxCommandEvent& event)
{
    ((lmScoreAuxCtrol*)m_pDisplayCtrol)->Dump();
}

void lmCompareScoresCtrol::OnDebugShowMidiEvents(wxCommandEvent& event)
{
    ((lmScoreAuxCtrol*)m_pDisplayCtrol)->DumpMidiEvents();
}




//------------------------------------------------------------------------------------
// Implementation of lmOneScoreCtrol
//  An ExerciseCtrol with one score for the problem and one optional score for
//  the solution. If no solution score the the problem score is used to present
//  the solution
//------------------------------------------------------------------------------------

IMPLEMENT_CLASS(lmOneScoreCtrol, lmExerciseCtrol)

BEGIN_EVENT_TABLE(lmOneScoreCtrol, lmExerciseCtrol)
    LM_EVT_END_OF_PLAY  (lmOneScoreCtrol::OnEndOfPlay)
END_EVENT_TABLE()


lmOneScoreCtrol::lmOneScoreCtrol(wxWindow* parent, wxWindowID id,
               lmExerciseConstrains* pConstrains, wxSize nDisplaySize, 
               const wxPoint& pos, const wxSize& size, int style)
    : lmExerciseCtrol(parent, id, pConstrains, nDisplaySize, pos, size, style )
{
    //initializations
    m_pProblemScore = (lmScore*)NULL;
	m_pSolutionScore = (lmScore*)NULL;
    m_pAuxScore = (lmScore*)NULL;
    m_nPlayMM = 320;    //it is assumed whole notes
    m_fPlaying = false;
}

lmOneScoreCtrol::~lmOneScoreCtrol()
{
}

wxWindow* lmOneScoreCtrol::CreateDisplayCtrol()
{
    // Using scores and ScoreAuxCtrol
    lmScoreAuxCtrol* pScoreCtrol = new lmScoreAuxCtrol(this, -1, (lmScore*)NULL, wxDefaultPosition,
                                        m_nDisplaySize, eSIMPLE_BORDER);
    pScoreCtrol->SetMargins(lmToLogicalUnits(5, lmMILLIMETERS),      //left=1cm
                            lmToLogicalUnits(5, lmMILLIMETERS),      //right=1cm
                            lmToLogicalUnits(10, lmMILLIMETERS));     //top=1cm

    pScoreCtrol->SetScale((float)1.3);
    return pScoreCtrol;
}

void lmOneScoreCtrol::Play()
{
    if (!m_fPlaying)
    {
        // Play button pressed

        //change link from "Play" to "Stop"
        m_pPlayButton->SetLabel(_("Stop"));

        //play the score
        ((lmScoreAuxCtrol*)m_pDisplayCtrol)->PlayScore(lmVISUAL_TRACKING, NO_MARCAR_COMPAS_PREVIO, 
                                ePM_NormalInstrument, m_nPlayMM);
        m_fPlaying = true;

        //! AWARE The link label is restored to "Play" when the EndOfPlay event is
        //! received. Flag m_fPlaying is also reset there
    }
    else 
    {
        // "Stop" button pressed
        ((lmScoreAuxCtrol*)m_pDisplayCtrol)->Stop();
    }
}

void lmOneScoreCtrol::OnEndOfPlay(lmEndOfPlayEvent& WXUNUSED(event))
{
    m_pPlayButton->SetLabel(_("Play"));
    m_fPlaying = false;
}

void lmOneScoreCtrol::DisplaySolution()
{
    //show the score
	if (m_pSolutionScore) {
		//There is a solution score. Display it
		((lmScoreAuxCtrol*)m_pDisplayCtrol)->SetScore(m_pSolutionScore);
	}
	else {
		//No solution score. Display problem score
		((lmScoreAuxCtrol*)m_pDisplayCtrol)->HideScore(false);
	}
    ((lmScoreAuxCtrol*)m_pDisplayCtrol)->DisplayMessage(m_sAnswer, lmToLogicalUnits(5, lmMILLIMETERS), false);

}

void lmOneScoreCtrol::DisplayProblem()
{
    //load the problem score into the control
    ((lmScoreAuxCtrol*)m_pDisplayCtrol)->SetScore(m_pProblemScore, true);   //true: the score must be hidden
    ((lmScoreAuxCtrol*)m_pDisplayCtrol)->DisplayMessage(_T(""), 0, true);   //true: clear the canvas

    if (m_pConstrains->IsTheoryMode())
    {
        //theory
        ((lmScoreAuxCtrol*)m_pDisplayCtrol)->DisplayScore(m_pProblemScore);
    } 
    else {
        //ear training
        Play();
    }
}

void lmOneScoreCtrol::DeleteScores()
{
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

void lmOneScoreCtrol::StopSounds()
{
    //Stop any possible chord being played to avoid crashes
    if (m_pAuxScore) m_pAuxScore->Stop();
    if (m_pProblemScore) m_pProblemScore->Stop();
    if (m_pSolutionScore) m_pSolutionScore->Stop();
}

void lmOneScoreCtrol::OnDebugShowSourceScore(wxCommandEvent& event)
{
    ((lmScoreAuxCtrol*)m_pDisplayCtrol)->SourceLDP();
}

void lmOneScoreCtrol::OnDebugDumpScore(wxCommandEvent& event)
{
    ((lmScoreAuxCtrol*)m_pDisplayCtrol)->Dump();
}

void lmOneScoreCtrol::OnDebugShowMidiEvents(wxCommandEvent& event)
{
    ((lmScoreAuxCtrol*)m_pDisplayCtrol)->DumpMidiEvents();
}



//MIDI cuts =========================================================================

//constructor
//    //MIDI case
//    m_mpPitch[0] = m_mpPitch[1] = -1;
//    m_pMessagesCtrol = (wxTextCtrl*)NULL;
//    m_nTimeIntval = 500;        //500 ms between first and second pitch
//    m_fStopFirst = false;       // do not stop first sound when sounding the second pitch
//    m_nTimeSecond = 2000;       // stop all sounds after 2s from start of second pitch

//wxWindow* lmCompareMidiCtrol::CreateDisplayCtrol()
//{
//    // Using MIDI Pitches and Static Text box
//    return new wxTextCtrl(this, -1, _T(""), wxDefaultPosition,
//                          m_nDisplaySize, wxSIMPLE_BORDER | wxTE_MULTILINE);
//
//}

