//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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
#include "auxctrols/UrlAuxCtrol.h"
#include "Constrains.h"
#include "Generators.h"
#include "../app/Processor.h"
#include "../app/MainFrame.h"
extern lmMainFrame* g_pMainFrame;
#include "../html/TextBookController.h"
#include "wx/html/htmlwin.h"

#include "wx/textctrl.h"


#include "../globals/Colors.h"
extern lmColors* g_pColors;

// access to global external variables
extern bool g_fReleaseVersion;          // in TheApp.cpp
extern bool g_fReleaseBehaviour;        // in TheApp.cpp
extern bool g_fShowDebugLinks;          // in TheApp.cpp
extern bool g_fAutoNewProblem;          // in Preferences.cpp

//access to MIDI manager to play MIDI sounds
#include "../sound/MidiManager.h"

//------------------------------------------------------------------------------------
// Implementation of lmEBookCtrol:
//------------------------------------------------------------------------------------


BEGIN_EVENT_TABLE(lmEBookCtrol, wxWindow)
    EVT_SIZE            (lmEBookCtrol::OnSize)
    LM_EVT_URL_CLICK    (ID_LINK_SEE_SOURCE, lmEBookCtrol::OnDebugShowSourceScore)
    LM_EVT_URL_CLICK    (ID_LINK_DUMP, lmEBookCtrol::OnDebugDumpScore)
    LM_EVT_URL_CLICK    (ID_LINK_MIDI_EVENTS, lmEBookCtrol::OnDebugShowMidiEvents)
    LM_EVT_URL_CLICK    (ID_LINK_PLAY, lmEBookCtrol::OnPlay)
    LM_EVT_URL_CLICK    (ID_LINK_SETTINGS, lmEBookCtrol::OnSettingsButton)
    LM_EVT_URL_CLICK    (ID_LINK_GO_BACK, lmEBookCtrol::OnGoBackButton)

END_EVENT_TABLE()

IMPLEMENT_CLASS(lmEBookCtrol, wxWindow)

lmEBookCtrol::lmEBookCtrol(wxWindow* parent, wxWindowID id,
                           lmEBookCtrolOptions* pOptions,
                           const wxPoint& pos, const wxSize& size, int style)
    : wxWindow(parent, id, pos, size, style )
{
    //initializations
    SetBackgroundColour(*wxWHITE);
    m_pOptions = pOptions;
    m_fControlsCreated = false;
    m_rScale = 1.0;
    m_pPlayButton = (lmUrlAuxCtrol*)NULL;

}

lmEBookCtrol::~lmEBookCtrol()
{
    //delete objects
    if (m_pOptions)
        delete m_pOptions;
}

void lmEBookCtrol::OnSettingsButton(wxCommandEvent& event)
{
    wxDialog* pDlg = GetSettingsDlg();
    if (pDlg) {
        int retcode = pDlg->ShowModal();
        if (retcode == wxID_OK) {
            m_pOptions->SaveSettings();
            // When changing settings it could be necessary to review answer buttons
            // or other issues. Give derived classes a chance to do it.
            OnSettingsChanged();
        }
        delete pDlg;
    }

}

void lmEBookCtrol::OnGoBackButton(wxCommandEvent& event)
{
    lmMainFrame* pFrame = GetMainFrame();
    lmTextBookController* pBookController = pFrame->GetBookController();
    pBookController->Display( m_pOptions->GetGoBackURL() );
}

void lmEBookCtrol::OnSize(wxSizeEvent& event)
{
    Layout();
}

void lmEBookCtrol::OnPlay(wxCommandEvent& event)
{
    Play();
}


//------------------------------------------------------------------------------------
// Implementation of lmExerciseCtrol:
//------------------------------------------------------------------------------------


BEGIN_EVENT_TABLE(lmExerciseCtrol, lmEBookCtrol)
    LM_EVT_URL_CLICK    (ID_LINK_NEW_PROBLEM, lmExerciseCtrol::OnNewProblem)
    LM_EVT_URL_CLICK    (ID_LINK_SOLUTION, lmExerciseCtrol::OnDisplaySolution)
    EVT_CHOICE          (lmID_CBO_MODE, lmExerciseCtrol::OnModeChanged)
END_EVENT_TABLE()

IMPLEMENT_CLASS(lmExerciseCtrol, lmEBookCtrol)

lmExerciseCtrol::lmExerciseCtrol(wxWindow* parent, wxWindowID id,
                           lmExerciseOptions* pConstrains, wxSize nDisplaySize,
                           const wxPoint& pos, const wxSize& size, int style)
    : lmEBookCtrol(parent, id, pConstrains, pos, size, style)
      , m_nDisplaySize(nDisplaySize)
      , m_pConstrains(pConstrains)
{
    //initializations
    SetBackgroundColour(*wxWHITE);
    m_nNumButtons = 0;
    m_fQuestionAsked = false;
    m_pProblemManager = (lmProblemManager*)NULL;
    m_sKeyPrefix = _T("");
    m_pCboMode = (wxChoice*)NULL;

    m_pDisplayCtrol =(wxWindow*)NULL;
    m_pCounters = (lmCountersAuxCtrol*)NULL;
    m_pShowSolution = (lmUrlAuxCtrol*)NULL;
    m_pAuxCtrolSizer = (wxBoxSizer*)NULL;

    m_nGenerationMode = m_pConstrains->GetGenerationMode();
}


lmExerciseCtrol::~lmExerciseCtrol()
{
    if (m_pProblemManager)
        delete m_pProblemManager;
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

    //Create the problem manager and the problem space
    CreateProblemManager();

    // ensure that sizes are properly scaled
    m_rScale = g_pMainFrame->GetHtmlWindow()->GetScale();
    m_nDisplaySize.x = (int)((double)m_nDisplaySize.x * m_rScale);
    m_nDisplaySize.y = (int)((double)m_nDisplaySize.y * m_rScale);

    // prepare layout info for answer buttons and spacing
    int nButtonsHeight = (int)(m_rScale * 24.0);    // 24 pixels, scaled
    wxFont oButtonsFont = GetParent()->GetFont();
    oButtonsFont.SetPointSize( (int)((double)oButtonsFont.GetPointSize() * m_rScale) );
    int nSpacing = (int)(5.0 * m_rScale);       //5 pixels, scaled

    //the window is divided into two regions: top, for score on left and counters and links
    //on the right, and bottom region, for answer buttons
    m_pMainSizer = new wxBoxSizer( wxVERTICAL );

        //
        // settings and debug options
        //
    wxBoxSizer* pTopLineSizer = new wxBoxSizer( wxHORIZONTAL );
    m_pMainSizer->Add(pTopLineSizer, wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, nSpacing));

    // settings link
    if (m_pConstrains->IncludeSettingsLink()) {
        lmUrlAuxCtrol* pSettingsLink = new lmUrlAuxCtrol(this, ID_LINK_SETTINGS, m_rScale,
             _("Exercise options") );
        pTopLineSizer->Add(pSettingsLink, wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 2*nSpacing) );
    }
    // "Go back to theory" link
    if (m_pConstrains->IncludeGoBackLink()) {
        lmUrlAuxCtrol* pGoBackLink = new lmUrlAuxCtrol(this, ID_LINK_GO_BACK, m_rScale,
            _("Go back to theory") );
        pTopLineSizer->Add(pGoBackLink, wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 2*nSpacing) );
    }

    // debug links
    if (g_fShowDebugLinks && !g_fReleaseVersion)
    {
        // "See source score"
        pTopLineSizer->Add(
            new lmUrlAuxCtrol(this, ID_LINK_SEE_SOURCE, m_rScale, _("See source score") ),
            wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 2*nSpacing) );

        // "Dump score"
        pTopLineSizer->Add(
            new lmUrlAuxCtrol(this, ID_LINK_DUMP, m_rScale, _("Dump score") ),
            wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 2*nSpacing) );

        // "See MIDI events"
        pTopLineSizer->Add(
            new lmUrlAuxCtrol(this, ID_LINK_MIDI_EVENTS, m_rScale, _("See MIDI events") ),
            wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 2*nSpacing) );
    }


    // sizer for the scoreCtrol and the CountersAuxCtrol
    wxBoxSizer* pTopSizer = new wxBoxSizer( wxHORIZONTAL );
    m_pMainSizer->Add(
        pTopSizer,
        wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 2*nSpacing) );

    // create the display control (ScoreAuxCtrol or wxTextCtrl):
    m_pDisplayCtrol = CreateDisplayCtrol();
    pTopSizer->Add(m_pDisplayCtrol,
                   wxSizerFlags(1).Left().Border(wxTOP|wxBOTTOM, 2*nSpacing));

    // sizer for the CountersAuxCtrol
    if (m_pConstrains->IsUsingCounters())
    {
        wxStaticBoxSizer* pCountersSizer =
	        new wxStaticBoxSizer( new wxStaticBox(this, wxID_ANY, wxEmptyString),
                                  wxVERTICAL);

	    wxBoxSizer* pModeSizer = new wxBoxSizer(wxHORIZONTAL);

	    wxStaticText* pLblMode = new wxStaticText(
            this, wxID_ANY, wxT("Mode:"), wxDefaultPosition, wxDefaultSize, 0);
	    pLblMode->Wrap( -1 );
	    pModeSizer->Add( pLblMode, 0, wxALL|wxALIGN_CENTER_VERTICAL, nSpacing);

        //load strings for Mode combo
        int nNumValidModes = 0;
	    wxString sCboModeChoices[lm_eNumGenerationModes];
        for (long i=0; i < lm_eNumGenerationModes; i++)
        {
            if (m_pConstrains->IsGenerationModeSupported(i))
                sCboModeChoices[nNumValidModes++] = g_sGenerationModeName[i];
        }
	    m_pCboMode = new wxChoice(this, lmID_CBO_MODE, wxDefaultPosition,
                                  wxDefaultSize, nNumValidModes, sCboModeChoices, 0);
	    m_pCboMode->SetSelection( m_nGenerationMode );

	    pModeSizer->Add( m_pCboMode, 1, wxALL|wxALIGN_CENTER_VERTICAL, nSpacing);

	    pCountersSizer->Add( pModeSizer, 0, wxEXPAND, nSpacing);

	    m_pAuxCtrolSizer = new wxBoxSizer( wxVERTICAL );

        m_pCounters = CreateCountersCtrol();
	    m_pAuxCtrolSizer->Add( m_pCounters, 0, wxALL, nSpacing);

	    pCountersSizer->Add( m_pAuxCtrolSizer, 0, wxEXPAND, nSpacing);
	    pTopSizer->Add( pCountersSizer, 0, wxLEFT, 3*nSpacing);
    }

        //
        // links
        //

    wxBoxSizer* pLinksSizer = new wxBoxSizer( wxHORIZONTAL );
    m_pMainSizer->Add(
        pLinksSizer,
        wxSizerFlags(0).Center().Border(wxLEFT|wxALL, 2*nSpacing) );

    // "new problem" button
    pLinksSizer->Add(
        new lmUrlAuxCtrol(this, ID_LINK_NEW_PROBLEM, m_rScale, _("New problem") ),
        wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 4*nSpacing) );

    // "play" button
    if (m_pConstrains->IncludePlayLink()) {
        m_pPlayButton = new lmUrlAuxCtrol(this, ID_LINK_PLAY, m_rScale, _("Play") );
        pLinksSizer->Add(
            m_pPlayButton,
            wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 4*nSpacing) );
    }

    // "show solution" button
    if (m_pConstrains->IncludeSolutionLink()) {
        m_pShowSolution = new lmUrlAuxCtrol(this, ID_LINK_SOLUTION, m_rScale, _("Show solution") );
        pLinksSizer->Add(
            m_pShowSolution,
            wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT|wxBOTTOM, 4*nSpacing) );
    }

    //create buttons for the answers
    CreateAnswerButtons(nButtonsHeight, nSpacing, oButtonsFont);

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
    if (m_pPlayButton) m_pPlayButton->Enable(false);
    if (m_pShowSolution) m_pShowSolution->Enable(false);

    OnSettingsChanged();     //reconfigure buttons in accordance with constraints

    m_fControlsCreated = true;
}

void lmExerciseCtrol::ChangeGenerationMode(int nMode)
{
    m_nGenerationMode = nMode;          //set new generation mode
    CreateProblemManager();             //change problem manager
    ChangeCountersCtrol();              //replace statistics control
}

void lmExerciseCtrol::ChangeGenerationModeLabel(int nMode)
{
    m_nGenerationMode = nMode;
    if (m_pCboMode)
        m_pCboMode->SetSelection( m_nGenerationMode );
}

void lmExerciseCtrol::ChangeCountersCtrol()
{
    //replace current control if exists
    if (m_fControlsCreated)
    {
        lmCountersAuxCtrol* pNewCtrol = CreateCountersCtrol();
        m_pAuxCtrolSizer->Replace(m_pCounters, pNewCtrol);
        delete m_pCounters;
        m_pCounters = pNewCtrol;
        m_pMainSizer->Layout();
        m_pCounters->UpdateDisplay();
    }
}

void lmExerciseCtrol::CreateProblemManager()
{
    if (m_pProblemManager)
        delete m_pProblemManager;

    switch(m_nGenerationMode)
    {
        case lm_eQuizMode:
        case lm_eExamMode:
            m_pProblemManager = new lmQuizManager(this);
            break;

        case lm_eLearningMode:
            m_pProblemManager = new lmLeitnerManager(this, true);
            break;

        case lm_ePractiseMode:
            m_pProblemManager = new lmLeitnerManager(this, false);
            break;

        default:
            wxASSERT(false);
    }

    SetProblemSpace();
}

void lmExerciseCtrol::SetProblemSpace()
{
}

lmCountersAuxCtrol* lmExerciseCtrol::CreateCountersCtrol()
{
    lmCountersAuxCtrol* pNewCtrol = (lmCountersAuxCtrol*)NULL;
    if (m_pConstrains->IsUsingCounters() )
    {
        switch(m_nGenerationMode)
        {
            case lm_eQuizMode:
            case lm_eExamMode:
                pNewCtrol = new lmQuizAuxCtrol(this, wxID_ANY, m_rScale,
                                              (lmQuizManager*)m_pProblemManager);
                break;

            case lm_eLearningMode:
                if (((lmLeitnerManager*)m_pProblemManager)->IsLearningMode())
                    pNewCtrol = new lmLeitnerAuxCtrol(this, wxID_ANY, m_rScale,
                                                 (lmLeitnerManager*)m_pProblemManager);
                else
                    pNewCtrol = new lmPractiseAuxCtrol(this, wxID_ANY, m_rScale,
                                                 (lmLeitnerManager*)m_pProblemManager);
                break;

            case lm_ePractiseMode:
                pNewCtrol = new lmPractiseAuxCtrol(this, wxID_ANY, m_rScale,
                                                (lmLeitnerManager*)m_pProblemManager);
                break;

            default:
                wxASSERT(false);
        }
    }
    return pNewCtrol;
}

void lmExerciseCtrol::OnNewProblem(wxCommandEvent& event)
{
    NewProblem();
}

void lmExerciseCtrol::OnModeChanged(wxCommandEvent& event)
{
    int nMode = m_pCboMode->GetSelection();
    if (m_nGenerationMode != nMode)
        this->ChangeGenerationMode(nMode);
}

void lmExerciseCtrol::OnDisplaySolution(wxCommandEvent& event)
{
    //First, stop any possible chord being played to avoid crashes
    StopSounds();

    //inform problem manager of the result
    OnQuestionAnswered(m_iQ, false);

    //produce feedback sound, and update statistics display
    if (m_pCounters)
    {
        m_pCounters->UpdateDisplay();
        m_pCounters->RightWrongSound(false);
    }

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

        //inform problem manager of the result
        OnQuestionAnswered(m_iQ, fSuccess);

        //produce feedback sound, and update statistics display
        if (m_pCounters)
        {
            m_pCounters->UpdateDisplay();
            m_pCounters->RightWrongSound(fSuccess);
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
        // No problem presented. The user press the button to play a specific
        // sound (chord, interval, scale, etc.)
        PlaySpecificSound(nIndex);
    }

}

void lmExerciseCtrol::OnQuestionAnswered(int iQ, bool fSuccess)
{
    //inform problem manager of the result
    if (m_pProblemManager)
    {
        //determine user response time
        wxTimeSpan tsResponse = wxDateTime::Now().Subtract( m_tmAsked );
        wxASSERT(!tsResponse.IsNegative());
        m_pProblemManager->UpdateQuestion(m_iQ, fSuccess, tsResponse);
    }
}


void lmExerciseCtrol::NewProblem()
{
    ResetExercise();

    //prepare answer buttons and counters
    if (m_pCounters) m_pCounters->OnNewQuestion();
    EnableButtons(true);

    //set m_pProblemScore, m_pSolutionScore, m_sAnswer, m_nRespIndex, m_nPlayMM
    wxString sProblemMessage = SetNewProblem();

    //display the problem
    m_fQuestionAsked = true;
    DisplayProblem();
    DisplayMessage(sProblemMessage, false);
    if (m_pPlayButton) m_pPlayButton->Enable(true);
    if (m_pShowSolution) m_pShowSolution->Enable(true);

    //save time
    m_tmAsked = wxDateTime::Now();
}

void lmExerciseCtrol::DoDisplaySolution()
{
    StopSounds();
    DisplaySolution();

    // mark right button in green
    SetButtonColor(m_nRespIndex, g_pColors->Success());

    if (m_pPlayButton) m_pPlayButton->Enable(true);
    if (m_pShowSolution) m_pShowSolution->Enable(false);
    m_fQuestionAsked = false;
    if (!m_pConstrains->ButtonsEnabledAfterSolution()) EnableButtons(false);

}

void lmExerciseCtrol::ResetExercise()
{
    StopSounds();

    //clear the display ctrol
    wxString sMsg = _T("");
    DisplayMessage(sMsg, true);   //true: clear the display ctrol

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
               lmExerciseOptions* pConstrains, wxSize nDisplaySize,
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

void lmCompareCtrol::CreateAnswerButtons(int nHeight, int nSpacing, wxFont& font)
{

    //create buttons for the answers: three buttons in one row
    m_pKeyboardSizer = new wxFlexGridSizer(m_NUM_ROWS, m_NUM_COLS, 2*nSpacing, 0);
    m_pMainSizer->Add(
        m_pKeyboardSizer,
        wxSizerFlags(0).Left().Border(wxALIGN_LEFT|wxTOP, 2*nSpacing) );

    // the buttons
    for (int iB=0; iB < m_NUM_COLS; iB++) {
        m_pAnswerButton[iB] = new wxButton( this, m_ID_BUTTON + iB, m_sButtonLabel[iB],
            wxDefaultPosition, wxSize(38*nSpacing, nHeight));
        m_pAnswerButton[iB]->SetFont(font);

        m_pKeyboardSizer->Add(
            m_pAnswerButton[iB],
            wxSizerFlags(0).Border(wxLEFT|wxRIGHT, nSpacing) );
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
               lmExerciseOptions* pConstrains, wxSize nDisplaySize,
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
    StopSounds();
    DeleteScores();
    ((lmScoreAuxCtrol*)m_pDisplayCtrol)->SetScore((lmScore*)NULL);
}

wxWindow* lmCompareScoresCtrol::CreateDisplayCtrol()
{
    // Using scores and ScoreAuxCtrol
    lmScoreAuxCtrol* pScoreCtrol = new lmScoreAuxCtrol(this, -1, (lmScore*)NULL, wxDefaultPosition,
                                        m_nDisplaySize, eSIMPLE_BORDER);
    pScoreCtrol->SetMargins(lmToLogicalUnits(5, lmMILLIMETERS),     //left
                            lmToLogicalUnits(5, lmMILLIMETERS),     //right
                            lmToLogicalUnits(10, lmMILLIMETERS));   //top
    pScoreCtrol->SetScale( pScoreCtrol->GetScale() * (float)m_rScale );
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
                    lmNO_COUNTOFF, ePM_NormalInstrument, m_nPlayMM);
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
    m_pScore[nIntv]->Play(lmNO_VISUAL_TRACKING, lmNO_COUNTOFF,
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

void lmCompareScoresCtrol::DisplayMessage(wxString& sMsg, bool fClearDisplay)
{
    ((lmScoreAuxCtrol*)m_pDisplayCtrol)->DisplayMessage(
            sMsg, lmToLogicalUnits(5, lmMILLIMETERS), fClearDisplay);
}




//------------------------------------------------------------------------------------
// Implementation of lmOneScoreCtrol
//  An ExerciseCtrol with one score for the problem and one optional score for
//  the solution. If no solution score is defined the problem score is used as
//  solution.
//------------------------------------------------------------------------------------

IMPLEMENT_CLASS(lmOneScoreCtrol, lmExerciseCtrol)

BEGIN_EVENT_TABLE(lmOneScoreCtrol, lmExerciseCtrol)
    LM_EVT_END_OF_PLAY  (lmOneScoreCtrol::OnEndOfPlay)
END_EVENT_TABLE()


lmOneScoreCtrol::lmOneScoreCtrol(wxWindow* parent, wxWindowID id,
               lmExerciseOptions* pConstrains, wxSize nDisplaySize,
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
    StopSounds();
    DeleteScores();
    ((lmScoreAuxCtrol*)m_pDisplayCtrol)->SetScore((lmScore*)NULL);
}

wxWindow* lmOneScoreCtrol::CreateDisplayCtrol()
{
    // Using scores and ScoreAuxCtrol
    lmScoreAuxCtrol* pScoreCtrol = new lmScoreAuxCtrol(this, -1, (lmScore*)NULL, wxDefaultPosition,
                                        m_nDisplaySize, eSIMPLE_BORDER);
    pScoreCtrol->SetMargins(lmToLogicalUnits(5, lmMILLIMETERS),     //left
                            lmToLogicalUnits(5, lmMILLIMETERS),     //right
                            lmToLogicalUnits(10, lmMILLIMETERS));   //top
    pScoreCtrol->SetScale( pScoreCtrol->GetScale() * (float)m_rScale );
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
        ((lmScoreAuxCtrol*)m_pDisplayCtrol)->PlayScore(lmVISUAL_TRACKING, lmNO_COUNTOFF,
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

void lmOneScoreCtrol::PlaySpecificSound(int nButton)
{
    StopSounds();

    //delete any previous score
    if (m_pAuxScore) {
        delete m_pAuxScore;
        m_pAuxScore = (lmScore*)NULL;
    }

    //prepare the score with the requested sound and play it
    PrepareAuxScore(nButton);
    if (m_pAuxScore) {
        m_pAuxScore->Play(lmNO_VISUAL_TRACKING, lmNO_COUNTOFF,
                            ePM_NormalInstrument, m_nPlayMM, (wxWindow*) NULL);
    }
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

void lmOneScoreCtrol::DisplayMessage(wxString& sMsg, bool fClearDisplay)
{
    ((lmScoreAuxCtrol*)m_pDisplayCtrol)->DisplayMessage(
            sMsg, lmToLogicalUnits(5, lmMILLIMETERS), fClearDisplay);
}


//------------------------------------------------------------------------------------
// Implementation of lmCompareMidiCtrol
//  An ExerciseCtrol without scores. It uses MIDI pitches for the problem and
//  the solution.
//------------------------------------------------------------------------------------

IMPLEMENT_CLASS(lmCompareMidiCtrol, lmCompareCtrol)

BEGIN_EVENT_TABLE(lmCompareMidiCtrol, lmCompareCtrol)
    EVT_TIMER           (wxID_ANY, lmCompareMidiCtrol::OnTimerEvent)
END_EVENT_TABLE()

lmCompareMidiCtrol::lmCompareMidiCtrol(wxWindow* parent, wxWindowID id,
               lmExerciseOptions* pConstrains, wxSize nDisplaySize,
               const wxPoint& pos, const wxSize& size, int style)
    : lmCompareCtrol(parent, id, pConstrains, nDisplaySize, pos, size, style )
{
    //initializations
    m_oTimer.SetOwner( this, wxID_ANY );    //needed to receive the timer events

    m_mpPitch[0] = m_mpPitch[1] = -1;
    m_nTimeIntval[0] = 500;     //500 ms between first and second pitch
    m_nTimeIntval[1] = 2000;    //stop all sounds after 2s from start of second pitch
    m_fStopPrev = false;        //do not stop first sound when sounding the second pitch
    m_nNowPlaying = -1;

    //default channels and instruments
    m_nChannel[0] = m_nChannel[1] = g_pMidi->DefaultVoiceChannel();
    m_nInstr[0] = m_nInstr[1] = g_pMidi->DefaultVoiceInstr();

}

lmCompareMidiCtrol::~lmCompareMidiCtrol()
{
    StopSounds();
}

wxWindow* lmCompareMidiCtrol::CreateDisplayCtrol()
{
    // Using MIDI Pitches and Static Text box
    return new wxStaticText(this, -1, _T(""), wxDefaultPosition,
                          m_nDisplaySize,
                          wxBORDER_SIMPLE | wxST_NO_AUTORESIZE );

}

void lmCompareMidiCtrol::Play()
{
    //wxLogMessage(_T("[lmCompareMidiCtrol::Play] m_nNowPlaying=%d"), m_nNowPlaying);
    if (m_nNowPlaying == -1)
    {
        // Starting to play

        //change link from "Play" to "Stop"
        m_pPlayButton->SetLabel(_("Stop"));

        //AWARE: The link label is restored to "Play" when the OnTimerEvent() event is
        //       received. Flag m_fPlaying is also reset there

        if (m_fQuestionAsked)
        {
            //Introducing the problem. Play the first sound
            PlaySound(0);
            //AWARE: method OnTimerEvent() will handle the event and play the
            //next sound.
        }
    }
    else {
        // "Stop" button pressed
        m_oTimer.Stop();
        m_nNowPlaying = -1;
        m_pPlayButton->SetLabel(_("Play"));
        StopSounds();
    }

}

void lmCompareMidiCtrol::PlaySound(int iSound)
{
    //wxLogMessage(_T("[lmCompareMidiCtrol::PlaySound] iSound=%d"), iSound);
    m_nNowPlaying = iSound;
    g_pMidiOut->NoteOn(m_nChannel[iSound], m_mpPitch[iSound], 127);
    m_oTimer.Start(m_nTimeIntval[iSound], wxTIMER_ONE_SHOT);
}

void lmCompareMidiCtrol::DisplaySolution()
{
    DisplayMessage(m_sAnswer, true);
}


void lmCompareMidiCtrol::DisplayProblem()
{
    Play();
}

void lmCompareMidiCtrol::StopSounds()
{
    g_pMidiOut->AllSoundsOff();
}

void lmCompareMidiCtrol::DisplayMessage(wxString& sMsg, bool fClearDisplay)
{
    ((wxStaticText*)m_pDisplayCtrol)->SetLabel(sMsg);
}

void lmCompareMidiCtrol::OnTimerEvent(wxTimerEvent& WXUNUSED(event))
{
    //wxLogMessage(_T("[lmCompareMidiCtrol::OnTimerEvent] m_nNowPlaying=%d"), m_nNowPlaying);
    m_oTimer.Stop();
    if (m_nNowPlaying == -1) return;

    if (m_nNowPlaying == 0)
    {
        //play next sound
        //wxLogMessage(_T("Timer event: play(1)"));
        if (m_fStopPrev)
            g_pMidiOut->NoteOff(m_nChannel[m_nNowPlaying], m_mpPitch[m_nNowPlaying], 127);
        PlaySound(++m_nNowPlaying);
    }
    else
    {
        //wxLogMessage(_T("Timer event: play stopped"));
        m_nNowPlaying = -1;
        StopSounds();
        m_pPlayButton->SetLabel(_("Play"));
    }
}



//------------------------------------------------------------------------------------
// Implementation of lmFullEditorExercise:
//------------------------------------------------------------------------------------


BEGIN_EVENT_TABLE(lmFullEditorExercise, wxWindow)
    EVT_SIZE            (lmFullEditorExercise::OnSize)
    LM_EVT_URL_CLICK    (ID_LINK_SETTINGS, lmFullEditorExercise::OnSettingsButton)
    LM_EVT_URL_CLICK    (ID_LINK_GO_BACK, lmFullEditorExercise::OnGoBackButton)
    LM_EVT_URL_CLICK    (ID_LINK_NEW_PROBLEM, lmFullEditorExercise::OnNewProblem)

END_EVENT_TABLE()

IMPLEMENT_CLASS(lmFullEditorExercise, wxWindow)

lmFullEditorExercise::lmFullEditorExercise(wxWindow* parent, wxWindowID id,
                           lmExerciseOptions* pConstrains,
                           const wxPoint& pos, const wxSize& size, int style)
    : wxWindow(parent, id, pos, size, style )
{
    //initializations
    SetBackgroundColour(*wxWHITE);
    m_pConstrains = pConstrains;
}

lmFullEditorExercise::~lmFullEditorExercise()
{
    //delete objects
    if (m_pConstrains) delete m_pConstrains;
}

void lmFullEditorExercise::OnSettingsButton(wxCommandEvent& event)
{
    wxDialog* pDlg = GetSettingsDlg();
    if (pDlg) {
        int retcode = pDlg->ShowModal();
        if (retcode == wxID_OK) {
            m_pConstrains->SaveSettings();
            // When changing settings it could be necessary to review answer buttons
            // or other issues. Give derived classes a chance to do it.
            OnSettingsChanged();
        }
        delete pDlg;
    }

}

void lmFullEditorExercise::OnGoBackButton(wxCommandEvent& event)
{
    lmMainFrame* pFrame = GetMainFrame();
    lmTextBookController* pBookController = pFrame->GetBookController();
    pBookController->Display( m_pConstrains->GetGoBackURL() );
}

void lmFullEditorExercise::OnSize(wxSizeEvent& event)
{
    Layout();
}

void lmFullEditorExercise::OnNewProblem(wxCommandEvent& event)
{
    SetNewProblem();
    lmMainFrame* pMainFrame = GetMainFrame();
    pMainFrame->NewScoreWindow((lmEditorMode*)NULL, m_pProblemScore);
    m_pScoreProc->SetTools();
}

void lmFullEditorExercise::CreateControls()
{
    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    InitializeStrings();

    // ensure that sizes are properly scaled
    m_rScale = g_pMainFrame->GetHtmlWindow()->GetScale();

    //the window contains just a sizer to add links
    m_pMainSizer = new wxBoxSizer( wxVERTICAL );

    // settings link
    if (m_pConstrains->IncludeSettingsLink()) {
        lmUrlAuxCtrol* pSettingsLink = new lmUrlAuxCtrol(this, ID_LINK_SETTINGS, m_rScale,
             _("Exercise options") );
        m_pMainSizer->Add(pSettingsLink, wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 5) );
    }
    // "Go back to theory" link
    if (m_pConstrains->IncludeGoBackLink()) {
        lmUrlAuxCtrol* pGoBackLink = new lmUrlAuxCtrol(this, ID_LINK_GO_BACK, m_rScale,
            _("Go back to theory") );
        m_pMainSizer->Add(pGoBackLink, wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 5) );
    }

    // "new problem" button
    m_pMainSizer->Add(
        new lmUrlAuxCtrol(this, ID_LINK_NEW_PROBLEM, m_rScale, _("New problem") ),
        wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 5) );

    //finish creation
    SetSizer( m_pMainSizer );                 // use the sizer for window layout
    m_pMainSizer->SetSizeHints( this );       // set size hints to honour minimum size
}




