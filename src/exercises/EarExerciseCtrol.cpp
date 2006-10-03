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
/*! @file EarExerciseCtrol.cpp
    @brief Implementation file for class lmEarExerciseCtrol
    @ingroup html_controls
*/
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "EarExerciseCtrol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "EarExerciseCtrol.h"
#include "UrlAuxCtrol.h"
#include "Constrains.h"
#include "Generators.h"
#include "../auxmusic/Conversion.h"

#include "../ldp_parser/LDPParser.h"
#include "../auxmusic/Interval.h"
#include "../app/DlgCfgIdfyChord.h"
#include "../auxmusic/ChordManager.h"


#include "../globals/Colors.h"
extern lmColors* g_pColors;

// access to global external variables
extern bool g_fReleaseVersion;            // in TheApp.cpp
extern bool g_fReleaseBehaviour;        // in TheApp.cpp
extern bool g_fShowDebugLinks;            // in TheApp.cpp

//--------------------------------------------------------------------------------
// Implementation of abstract class for ear exercises constrains
//--------------------------------------------------------------------------------
lmEarConstrains::lmEarConstrains(wxString sSection)
{
    m_sSection = sSection;
    m_fSettingsLink = false;
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
    ID_LINK_SETTINGS

};


BEGIN_EVENT_TABLE(lmEarExerciseCtrol, wxWindow)
    EVT_SIZE            (lmEarExerciseCtrol::OnSize)

    LM_EVT_URL_CLICK    (ID_LINK_SEE_SOURCE, lmEarExerciseCtrol::OnDebugShowSourceScore)
    LM_EVT_URL_CLICK    (ID_LINK_DUMP, lmEarExerciseCtrol::OnDebugDumpScore)
    LM_EVT_URL_CLICK    (ID_LINK_MIDI_EVENTS, lmEarExerciseCtrol::OnDebugShowMidiEvents)

    LM_EVT_URL_CLICK    (ID_LINK_NEW_PROBLEM, lmEarExerciseCtrol::OnNewProblem)
    LM_EVT_URL_CLICK    (ID_LINK_PLAY, lmEarExerciseCtrol::OnPlay)
    LM_EVT_URL_CLICK    (ID_LINK_SOLUTION, lmEarExerciseCtrol::OnDisplaySolution)
    LM_EVT_URL_CLICK    (ID_LINK_SETTINGS, lmEarExerciseCtrol::OnSettingsButton)

    LM_EVT_END_OF_PLAY  (lmEarExerciseCtrol::OnEndOfPlay)

END_EVENT_TABLE()

IMPLEMENT_CLASS(lmEarExerciseCtrol, wxWindow)

lmEarExerciseCtrol::lmEarExerciseCtrol(wxWindow* parent, wxWindowID id, 
                           lmEarConstrains* pConstrains, 
                           const wxPoint& pos, const wxSize& size, int style)
    : wxWindow(parent, id, pos, size, style )
{
    //initializations
    m_nNumButtons = 0;
    m_fQuestionAsked = false;
    m_pProblemScore = (lmScore*)NULL;
    m_pAuxScore = (lmScore*)NULL;
    m_pScoreCtrol = (lmScoreAuxCtrol*)NULL;
    m_pConstrains = pConstrains;
    m_fTheoryMode = m_pConstrains->IsTheoryMode();
    m_nPlayMM = 320;    //it is assumed whole notes
    m_fPlaying = false;
}

void lmEarExerciseCtrol::Create(int nCtrolWidth, int nCtrolHeight)
{
    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    InitializeStrings();

    //the window is divided into two regions: top, for score on left and counters and links
    //on the right, and bottom region, for answer buttons 
    m_pMainSizer = new wxBoxSizer( wxVERTICAL );

    // debug buttons
    if (g_fShowDebugLinks && !g_fReleaseVersion) {
        wxBoxSizer* pDbgSizer = new wxBoxSizer( wxHORIZONTAL );
        m_pMainSizer->Add(pDbgSizer, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

        // "See source score"
        pDbgSizer->Add(
            new lmUrlAuxCtrol(this, ID_LINK_SEE_SOURCE, _("See source score") ),
            wxSizerFlags(0).Left().Border(wxALL, 10) );

        // "Dump score"
        pDbgSizer->Add(
            new lmUrlAuxCtrol(this, ID_LINK_DUMP, _("Dump score") ),
            wxSizerFlags(0).Left().Border(wxALL, 10) );

        // "See MIDI events"
        pDbgSizer->Add(
            new lmUrlAuxCtrol(this, ID_LINK_MIDI_EVENTS, _("See MIDI events") ),
            wxSizerFlags(0).Left().Border(wxALL, 10) );
    }

    // sizer for the scoreCtrol and the CountersCtrol
    wxBoxSizer* pTopSizer = new wxBoxSizer( wxHORIZONTAL );
    m_pMainSizer->Add(
        pTopSizer,
        wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 10) );

    // create score ctrl 
    m_pScoreCtrol = new lmScoreAuxCtrol(this, -1, (lmScore*)NULL, wxDefaultPosition,
                                        wxSize(nCtrolWidth, nCtrolHeight), eSIMPLE_BORDER);
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

    // spacer to move the settings link to bottom
    pCountersSizer->Add(5, 5, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    // settings link
    if (m_pConstrains->IncludeSettingsLink()) {
        lmUrlAuxCtrol* pSettingsLink = new lmUrlAuxCtrol(this, ID_LINK_SETTINGS, _("Settings") );
        pCountersSizer->Add(pSettingsLink, wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 10) );
    }

    // spacer to move the settings link a little up
    pCountersSizer->Add(5, 5, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

        //
        //links 
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


    m_pScoreCtrol->DisplayMessage(_("Click on 'New problem' to start"), 
                                  lmToLogicalUnits(10, lmMILLIMETERS),
                                  true);

    m_pPlayButton->Enable(false);
    m_pShowSolution->Enable(false);

    ReconfigureButtons();     //reconfigure buttons in accordance with constrains

}

lmEarExerciseCtrol::~lmEarExerciseCtrol()
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
        m_pConstrains = (lmEarConstrains*) NULL;
    }

    if (m_pProblemScore) {
        delete m_pProblemScore;
        m_pProblemScore = (lmScore*)NULL;
    }
    if (m_pAuxScore) {
        delete m_pAuxScore;
        m_pAuxScore = (lmScore*)NULL;
    }
}

void lmEarExerciseCtrol::OnSettingsButton(wxCommandEvent& event)
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

void lmEarExerciseCtrol::OnSize(wxSizeEvent& event)
{
    //wxLogMessage(_T("OnSize at lmEarExerciseCtrol"));
    Layout();
}

void lmEarExerciseCtrol::OnPlay(wxCommandEvent& event)
{
    Play();
}

void lmEarExerciseCtrol::OnEndOfPlay(lmEndOfPlayEvent& WXUNUSED(event))
{
    m_pPlayButton->SetLabel(_("Play"));
    m_fPlaying = false;
}

void lmEarExerciseCtrol::OnNewProblem(wxCommandEvent& event)
{
    NewProblem();
}

void lmEarExerciseCtrol::OnDisplaySolution(wxCommandEvent& event)
{
    //First, stop any possible chord being played to avoid crashes
    DoStopSounds();

    //now proceed
    m_pCounters->IncrementWrong();
    DisplaySolution();
}

void lmEarExerciseCtrol::OnRespButton(wxCommandEvent& event)
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
            
        //if failure, display the solution. If succsess, generate a new problem
        if (!fSuccess) {
            //failure: mark wrong button in red and right one in green
            SetButtonColor(m_nRespIndex, g_pColors->Success() );
            SetButtonColor(nIndex, g_pColors->Failure() );

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

void lmEarExerciseCtrol::NewProblem()
{
    ResetExercise();

    //set m_pProblemScore, m_sAnswer, m_nRespIndex, m_nPlayMM
    wxString sProblemMessage = SetNewProblem();    

    //load total score into the control
    m_pScoreCtrol->SetScore(m_pProblemScore, true);   //true: the score must be hidden
    m_pScoreCtrol->DisplayMessage(_T(""), 0, true);     //true: clear the canvas

    //display the problem
    if (m_fTheoryMode) {
        //theory
        m_pScoreCtrol->DisplayScore(m_pProblemScore);
        m_pScoreCtrol->DisplayMessage(sProblemMessage, lmToLogicalUnits(5, lmMILLIMETERS), false);
        EnableButtons(true);
    } else {
        //ear training
        Play();
        m_pScoreCtrol->DisplayMessage(sProblemMessage, lmToLogicalUnits(5, lmMILLIMETERS), false);
    }

    m_fQuestionAsked = true;
    m_pPlayButton->Enable(true);
    m_pShowSolution->Enable(true);


}

void lmEarExerciseCtrol::Play()
{
    if (!m_fPlaying) {
        // Play button pressed
        //m_pProblemScore->Dump(_T("lmEarExerciseCtrol.Play.ScoreDump.txt"));  //dbg

        //change link from "Play" to "Stop"
        m_pPlayButton->SetLabel(_("Stop"));

        //play the score
        m_pScoreCtrol->PlayScore(lmVISUAL_TRACKING, NO_MARCAR_COMPAS_PREVIO, 
                                ePM_NormalInstrument, m_nPlayMM);
        m_fPlaying = true;

        //! @attention The link label is restored to "Play" when the EndOfPlay event is
        //! received. Flag m_fPlaying is also reset there
    }
    else {
        // "Stop" button pressed
        m_pScoreCtrol->Stop();
    }
}

void lmEarExerciseCtrol::DisplaySolution()
{
    DoStopSounds();

    //show the score
    m_pScoreCtrol->HideScore(false);
    m_pScoreCtrol->DisplayMessage(m_sAnswer, lmToLogicalUnits(5, lmMILLIMETERS), false);

    // mark right button in green
    SetButtonColor(m_nRespIndex, g_pColors->Success());
    
    m_pPlayButton->Enable(true);
    m_pShowSolution->Enable(false);
    m_fQuestionAsked = false;
    if (m_fTheoryMode) EnableButtons(false);

}

void lmEarExerciseCtrol::OnDebugShowSourceScore(wxCommandEvent& event)
{
    m_pScoreCtrol->SourceLDP();
}

void lmEarExerciseCtrol::OnDebugDumpScore(wxCommandEvent& event)
{
    m_pScoreCtrol->Dump();
}

void lmEarExerciseCtrol::OnDebugShowMidiEvents(wxCommandEvent& event)
{
    m_pScoreCtrol->DumpMidiEvents();
}

void lmEarExerciseCtrol::ResetExercise()
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

void lmEarExerciseCtrol::EnableButtons(bool fEnable)
{
    wxButton* pButton;
    for (int iB=0; iB < m_nNumButtons; iB++) {
        pButton = *(m_pAnswerButtons + iB);
        if (pButton) pButton->Enable(fEnable);
    }
}

void lmEarExerciseCtrol::SetButtons(wxButton* pButtons[], int nNumButtons, int nIdFirstButton)
{
    m_pAnswerButtons = pButtons;
    m_nIdFirstButton = nIdFirstButton;
    m_nNumButtons = nNumButtons;
}

void lmEarExerciseCtrol::SetButtonColor(int i, wxColour& color)
{
    wxButton* pButton = *(m_pAnswerButtons + i);
    if (pButton) pButton->SetBackgroundColour(color);
}

void lmEarExerciseCtrol::DoStopSounds()
{
    //Stop any possible chord being played to avoid crashes
    if (m_pAuxScore) m_pAuxScore->Stop();
    if (m_pProblemScore) m_pProblemScore->Stop();

    //Require to do it for other scores in the derived class
    StopSounds();       

}
