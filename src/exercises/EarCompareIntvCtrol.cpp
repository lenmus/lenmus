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
#pragma implementation "EarCompareIntvCtrol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "EarCompareIntvCtrol.h"
#include "UrlAuxCtrol.h"
#include "Constrains.h"
#include "Generators.h"
#include "../auxmusic/Conversion.h"

#include "../ldp_parser/LDPParser.h"
#include "../auxmusic/Interval.h"
#include "../app/DlgCfgEarIntervals.h"


#include "../globals/Colors.h"
extern lmColors* g_pColors;

// access to global external variables
extern bool g_fReleaseVersion;          // in TheApp.cpp
extern bool g_fReleaseBehaviour;        // in TheApp.cpp
extern bool g_fShowDebugLinks;          // in TheApp.cpp
extern bool g_fAutoNewProblem;          // in Preferences.cpp

//------------------------------------------------------------------------------------
// Implementation of lmEarCompareIntvCtrol




//Layout definitions
const int BUTTONS_DISTANCE    = 5;        //pixels
const int NUM_LINKS = 3;                //links for actions

//IDs for controls
enum {
    ID_LINK_SEE_SOURCE = 3000,
    ID_LINK_DUMP,
    ID_LINK_MIDI_EVENTS,
    ID_BUTTON,
    ID_LINK = ID_BUTTON + 3,
    ID_LINK_NEW_PROBLEM,
    ID_LINK_PLAY,
    ID_LINK_SOLUTION,
    ID_LINK_SETTINGS

};


BEGIN_EVENT_TABLE(lmEarCompareIntvCtrol, wxWindow)
    EVT_COMMAND_RANGE (ID_BUTTON, ID_BUTTON+2, wxEVT_COMMAND_BUTTON_CLICKED, lmEarCompareIntvCtrol::OnRespButton)
    EVT_SIZE            (lmEarCompareIntvCtrol::OnSize)

    LM_EVT_URL_CLICK    (ID_LINK_SEE_SOURCE, lmEarCompareIntvCtrol::OnDebugShowSourceScore)
    LM_EVT_URL_CLICK    (ID_LINK_DUMP, lmEarCompareIntvCtrol::OnDebugDumpScore)
    LM_EVT_URL_CLICK    (ID_LINK_MIDI_EVENTS, lmEarCompareIntvCtrol::OnDebugShowMidiEvents)

    LM_EVT_URL_CLICK    (ID_LINK_NEW_PROBLEM, lmEarCompareIntvCtrol::OnNewProblem)
    LM_EVT_URL_CLICK    (ID_LINK_PLAY, lmEarCompareIntvCtrol::OnPlay)
    LM_EVT_URL_CLICK    (ID_LINK_SOLUTION, lmEarCompareIntvCtrol::OnDisplaySolution)
    LM_EVT_URL_CLICK    (ID_LINK_SETTINGS, lmEarCompareIntvCtrol::OnSettingsButton)
END_EVENT_TABLE()

IMPLEMENT_CLASS(lmEarCompareIntvCtrol, wxWindow)

lmEarCompareIntvCtrol::lmEarCompareIntvCtrol(wxWindow* parent, wxWindowID id, 
                           lmEarIntervalsConstrains* pConstrains,
                           const wxPoint& pos, const wxSize& size, int style)
    : wxWindow(parent, id, pos, size, style )
{
    //initializations
    m_fCancel = false;
    SetBackgroundColour(*wxWHITE);
    int i;
    for (i=0; i < 3; i++) { m_pAnswerButton[i] = (wxButton*)NULL; }
    EnableButtons(false);
    m_fProblemCreated = false;
    m_fPlayEnabled = false;
    m_pScore[0] = (lmScore*)NULL;
    m_pScore[1] = (lmScore*)NULL;
    m_pTotalScore = (lmScore*)NULL;
    m_pScoreCtrol = (lmScoreAuxCtrol*)NULL;
    m_pConstrains = pConstrains;

    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    wxString sBtLabel[3];
    sBtLabel[0] = _("First one greater");
    sBtLabel[1] = _("Second one greater");
    sBtLabel[2] = _("Both are equal");

    //the window is divided into two regions: top, for score on left and counters and links
    //on the right, and bottom region, for answer buttons 
    wxBoxSizer* pMainSizer = new wxBoxSizer( wxVERTICAL );

    // debug buttons
    if (g_fShowDebugLinks && !g_fReleaseVersion) {
        wxBoxSizer* pDbgSizer = new wxBoxSizer( wxHORIZONTAL );
        pMainSizer->Add(pDbgSizer, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

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
    pMainSizer->Add(
        pTopSizer,
        wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 10) );

    // create score ctrl 
    m_pScoreCtrol = new lmScoreAuxCtrol(this, -1, (lmScore*)NULL, wxDefaultPosition, wxSize(400,150), eSIMPLE_BORDER);
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
    lmUrlAuxCtrol* pSettingsLink = new lmUrlAuxCtrol(this, ID_LINK_SETTINGS, _("Settings") );
    pCountersSizer->Add(pSettingsLink, wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 10) );

    // spacer to move the settings link a little up
    pCountersSizer->Add(5, 5, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

        //
        //links 
        //

    wxBoxSizer* pLinksSizer = new wxBoxSizer( wxHORIZONTAL );
    pMainSizer->Add(
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
    

    //create 3 buttons for the answers, in one row
    wxBoxSizer* pRowSizer = new wxBoxSizer( wxHORIZONTAL );
    wxButton* pButton;
    int iB = 0;
    //pRowSizer->Add(20+BUTTONS_DISTANCE, 24, 0);    //spacer to center labels

    for (iB=0; iB < 3; iB++) {
        pButton = new wxButton( this, ID_BUTTON + iB, sBtLabel[iB],
            wxDefaultPosition, wxSize(134, 24));
        m_pAnswerButton[iB] = pButton;
        pRowSizer->Add(
            pButton,
            wxSizerFlags(0).Border(wxALL, BUTTONS_DISTANCE) );
    }
    pMainSizer->Add(    
        pRowSizer,
        wxSizerFlags(0).Left());

    SetSizer( pMainSizer );                 // use the sizer for window layout
    pMainSizer->SetSizeHints( this );       // set size hints to honour minimum size

    m_pScoreCtrol->DisplayMessage(_("Click on 'New problem' to start"), 
                                  lmToLogicalUnits(10, lmMILLIMETERS),
                                  true);

    m_pPlayButton->Enable(false);
    m_pShowSolution->Enable(false);

}

lmEarCompareIntvCtrol::~lmEarCompareIntvCtrol()
{
    m_fCancel = true;       //inform that the exercise is cancelled
    wxMilliSleep(3000);     //wait for 1000ms for any score being played
    DoStopSounds();         //stop any possible score being played

    if (m_pScoreCtrol) {
        delete m_pScoreCtrol;
        m_pScoreCtrol = (lmScoreAuxCtrol*)NULL;
    }

    if (m_pConstrains) {
        delete m_pConstrains;
        m_pConstrains = (lmEarIntervalsConstrains*) NULL;
    }

    if (m_pScore[0]) {
        delete m_pScore[0];
        m_pScore[0] = (lmScore*)NULL;
    }
    if (m_pScore[1]) {
        delete m_pScore[1];
        m_pScore[1] = (lmScore*)NULL;
    }
    if (m_pTotalScore) {
        delete m_pTotalScore;
        m_pTotalScore = (lmScore*)NULL;
    }
}

void lmEarCompareIntvCtrol::EnableButtons(bool fEnable)
{
    for (int i=0; i < 3; i++) {
        if (m_pAnswerButton[i])
            m_pAnswerButton[i]->Enable(fEnable);
    }
    m_fButtonsEnabled = fEnable;

}

//----------------------------------------------------------------------------------------
// Event handlers

void lmEarCompareIntvCtrol::OnSettingsButton(wxCommandEvent& event)
{
    lmDlgCfgEarIntervals dlg(this, m_pConstrains, true);    // true -> enable First note equal checkbox
    int retcode = dlg.ShowModal();
    if (retcode == wxID_OK) m_pConstrains->SaveSettings();

}

void lmEarCompareIntvCtrol::OnSize(wxSizeEvent& event)
{
    //wxLogMessage(_T("OnSize at lmEarCompareIntvCtrol"));
    Layout();

}

void lmEarCompareIntvCtrol::OnPlay(wxCommandEvent& event)
{
    Play();
}

void lmEarCompareIntvCtrol::OnNewProblem(wxCommandEvent& event)
{
    NewProblem();
}

void lmEarCompareIntvCtrol::OnDisplaySolution(wxCommandEvent& event)
{
    m_pCounters->IncrementWrong();
    DisplaySolution();
    EnableButtons(false);
}

void lmEarCompareIntvCtrol::OnRespButton(wxCommandEvent& event)
{
    int nIndex = event.GetId() - ID_BUTTON;

    //show the solucion
    DisplaySolution();
    EnableButtons(true);

    //verify if success or failure
    bool fSecondGreater = !(m_fFirstGreater || m_fBothEqual);
    bool fSuccess = (nIndex == 0 && m_fFirstGreater) ||
               (nIndex == 1 && fSecondGreater) ||
               (nIndex == 2 && m_fBothEqual);
                

    //produce feedback sound, and update counters
    if (fSuccess) {
        m_pCounters->IncrementRight();
    } else {
        m_pCounters->IncrementWrong();
    }
        
    // if failure mark pushed button in red
    if (!fSuccess) {
        m_pAnswerButton[nIndex]->SetBackgroundColour(g_pColors->Failure());
    }

    EnableButtons(false);
    m_pShowSolution->Enable(false);

}

void lmEarCompareIntvCtrol::NewProblem()
{

    ResetExercise();
    
    //
    //generate the two intervals to compare
    //

    EClefType nClef = eclvSol;

    // select interval type: ascending, descending or both
    lmRandomGenerator oGenerator;
    bool fAscending;
    if (m_pConstrains->IsTypeAllowed(0) || 
        (m_pConstrains->IsTypeAllowed(1) && m_pConstrains->IsTypeAllowed(2)))
    {
        // if harmonic scale or melodic ascending and descending,
        // both, ascending and descending, are allowed. Choose one randomly
        fAscending = oGenerator.FlipCoin();
    }
    else if (m_pConstrains->IsTypeAllowed(1)) {
        // if melodic ascendig, allow only ascending intervals
        fAscending = true;
    }
    else {
        // allow only descending intervals
        fAscending = false;
    }
    // select a random key signature satisfying the constraints
    EKeySignatures nKey;
    if (m_pConstrains->OnlyNatural()) {
        nKey = oGenerator.GenerateKey(m_pConstrains->GetKeyConstrains());
    }
    else {
        nKey = earmDo;
    }
    // generate the two intervals
    lmInterval oIntv0(m_pConstrains->OnlyNatural(), m_pConstrains->MinNote(),
        m_pConstrains->MaxNote(), m_pConstrains->AllowedIntervals(), fAscending, nKey);

    bool fOnlyNatural;
    int nMidi0;
    if (m_pConstrains->FirstNoteEqual()) {
        fOnlyNatural = false;
        nMidi0 = oIntv0.GetMidiNote1();
    }
    else {
        fOnlyNatural = m_pConstrains->OnlyNatural();
        nMidi0 = 0;
    }
    lmInterval oIntv1(fOnlyNatural, m_pConstrains->MinNote(), m_pConstrains->MaxNote(),
            m_pConstrains->AllowedIntervals(), fAscending, nKey, nMidi0);

    //Convert problem to LDP pattern
    wxString sPattern[2][2];
    int i;
    for (i=0; i < 2; i++) {
        sPattern[0][i] = _T("(n ") + oIntv0.GetPattern(i) + _T(" r)");
        sPattern[1][i] = _T("(n ") + oIntv1.GetPattern(i) + _T(" r)");
    }

    //
    //create two scores, one for each interval to be played, and a third score with
    //both intervals for displaying the solution
    //

    lmNote* pNote[2];
    lmLDPParser parserLDP;
    lmLDPNode* pNode;
    lmVStaff* pVStaff;

    //create the two single-interval scores
    for (i=0; i<2; i++) {
        m_pScore[i] = new lmScore();
        m_pScore[i]->SetTopSystemDistance( lmToLogicalUnits(5, lmMILLIMETERS) ); //5mm
        m_pScore[i]->AddInstrument(1,0,0,_T(""));                     //one vstaff, MIDI channel 0, MIDI instr 0
        pVStaff = m_pScore[i]->GetVStaff(1, 1);      //get first vstaff of instr.1
        pVStaff->AddClef( nClef );
        pVStaff->AddKeySignature(nKey);
        pVStaff->AddTimeSignature(4 ,4, lmNO_VISIBLE );
    //    pVStaff->AddSpacer(30);       // 3 lines
        pNode = parserLDP.ParseText( sPattern[i][0] );
        pNote[0] = parserLDP.AnalyzeNote(pNode, pVStaff);
        pVStaff->AddBarline(etb_SimpleBarline, lmNO_VISIBLE);    //so that accidental doesn't affect 2nd note
        pNode = parserLDP.ParseText( sPattern[i][1] );
        pNote[1] = parserLDP.AnalyzeNote(pNode, pVStaff);
        pVStaff->AddBarline(etb_EndBarline, lmNO_VISIBLE);
    }

    //create the answer score with both intervals
    m_pTotalScore = new lmScore();
    m_pTotalScore->SetTopSystemDistance( lmToLogicalUnits(5, lmMILLIMETERS) );    //5mm
    m_pTotalScore->SetOption(_T("Render.SpacingMethod"), (long)esm_Fixed);
    m_pTotalScore->AddInstrument(1,0,0,_T(""));                     //one vstaff, MIDI channel 0, MIDI instr 0
    pVStaff = m_pTotalScore->GetVStaff(1, 1);      //get first vstaff of instr.1
    pVStaff->AddClef( nClef );
    pVStaff->AddKeySignature(nKey);
    pVStaff->AddTimeSignature(4 ,4, lmNO_VISIBLE );

//    pVStaff->AddSpacer(30);       // 3 lines
    pNode = parserLDP.ParseText( sPattern[0][0] );
    pNote[0] = parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddBarline(etb_SimpleBarline, lmNO_VISIBLE);    //so that accidental doesn't affect 2nd note
    pNode = parserLDP.ParseText( sPattern[0][1] );
    pNote[1] = parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddBarline(etb_DoubleBarline);

//    pVStaff->AddSpacer(30);       // 3 lines
    pNode = parserLDP.ParseText( sPattern[1][0] );
    parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddBarline(etb_SimpleBarline, lmNO_VISIBLE);    //so that accidental doesn't affect 2nd note
    pNode = parserLDP.ParseText( sPattern[1][1] );
    parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddBarline(etb_EndBarline, lmNO_VISIBLE);    //so that accidental doesn't affect 2nd note

    //compute the right answer
    m_sAnswer[0] = oIntv0.GetIntervalName();
    m_sAnswer[1] = oIntv1.GetIntervalName();
    m_fFirstGreater = (oIntv0.GetNumSemitones() > oIntv1.GetNumSemitones());
    m_fBothEqual = (oIntv0.GetNumSemitones() == oIntv1.GetNumSemitones());
    
    //load total score into the control
    m_pScoreCtrol->SetScore(m_pTotalScore, true);   //true: the score must be hidden
    m_pScoreCtrol->DisplayMessage(_T(""), 0, true);     //true: clear the canvas

    m_fPlayEnabled = true;
    m_fProblemCreated = true;
    EnableButtons(true);
    m_pPlayButton->Enable(true);
    m_pShowSolution->Enable(true);

    m_pCounters->NextTeam();


    //play the interval
    Play();

}

void lmEarCompareIntvCtrol::Play()
{
    //@attention As the intervals are built using whole notes, we will play them at MM=240 so
    //that real note rate will be 80.

    //first interval
#if 0
    m_pScoreCtrol->SetScore(m_pScore[0], true);   //true: the score must be hidden
    m_pAnswerButton[0]->SetBackgroundColour( g_pColors->ButtonHighlight() );
    m_pScoreCtrol->PlayScore(lmNO_VISUAL_TRACKING, NO_MARCAR_COMPAS_PREVIO, 
                             ePM_NormalInstrument, 320);
    //m_pAnswerButton[0]->SetBackgroundColour( g_pColors->Normal() );
#else
    m_pAnswerButton[0]->SetBackgroundColour( g_pColors->ButtonHighlight() );
    m_pAnswerButton[0]->Update();    //Refresh works vie events and, so, it is not inmediate
    if (m_fCancel) return;
    m_pScore[0]->Play(lmNO_VISUAL_TRACKING, NO_MARCAR_COMPAS_PREVIO, 
                             ePM_NormalInstrument, 320);
    m_pScore[0]->WaitForTermination();
    m_pAnswerButton[0]->SetBackgroundColour( g_pColors->Normal() );
    m_pAnswerButton[0]->Update();

    wxMilliSleep(1000);     //wait for 1sec (1000ms)

    //second interval
    if (m_fCancel) return;
    m_pAnswerButton[1]->SetBackgroundColour( g_pColors->ButtonHighlight() );
    m_pAnswerButton[1]->Update();
    m_pScore[1]->Play(lmNO_VISUAL_TRACKING, NO_MARCAR_COMPAS_PREVIO, 
                             ePM_NormalInstrument, 320);
    m_pScore[1]->WaitForTermination();
    m_pAnswerButton[1]->SetBackgroundColour( g_pColors->Normal() );
    m_pAnswerButton[1]->Update();
#endif
}

void lmEarCompareIntvCtrol::DisplaySolution()
{
    DoStopSounds();     //stop any possible score being played
    wxString sAnswer = m_sAnswer[0] + _T(", ") + m_sAnswer[1];
    m_pScoreCtrol->HideScore(false);
    m_pScoreCtrol->DisplayMessage(sAnswer, lmToLogicalUnits(5, lmMILLIMETERS), false);

    // mark right button in green
    if (m_fFirstGreater) 
        m_pAnswerButton[0]->SetBackgroundColour(g_pColors->Success());
    else if (m_fBothEqual)
        m_pAnswerButton[2]->SetBackgroundColour(g_pColors->Success());
    else
        m_pAnswerButton[1]->SetBackgroundColour(g_pColors->Success());
    
    m_fPlayEnabled = true;
    m_fProblemCreated = false;
    
}

void lmEarCompareIntvCtrol::OnDebugShowSourceScore(wxCommandEvent& event)
{
    m_pScoreCtrol->SourceLDP();
}

void lmEarCompareIntvCtrol::OnDebugDumpScore(wxCommandEvent& event)
{
    m_pScoreCtrol->Dump();
}

void lmEarCompareIntvCtrol::OnDebugShowMidiEvents(wxCommandEvent& event)
{
    m_pScoreCtrol->DumpMidiEvents();
}

void lmEarCompareIntvCtrol::ResetExercise()
{
    DoStopSounds();     //stop any possible score being played

    //clear the canvas
    m_pScoreCtrol->DisplayMessage(_T(""), 0, true);     //true: clear the canvas
    m_pScoreCtrol->Update();    //to force to clear it now

    // restore buttons' normal color
    for (int i=0; i < 3; i++) {
        if (m_pAnswerButton[i]) {
            m_pAnswerButton[i]->SetBackgroundColour( g_pColors->Normal() );
        }
    }
    EnableButtons(false);

    //delete the previous scores
    if (m_pScore[0]) {
        delete m_pScore[0];
        m_pScore[0] = (lmScore*)NULL;
    }
    if (m_pScore[1]) {
        delete m_pScore[1];
        m_pScore[1] = (lmScore*)NULL;
    }
    if (m_pTotalScore) {
        delete m_pTotalScore;
        m_pTotalScore = (lmScore*)NULL;
    }

}

void lmEarCompareIntvCtrol::DoStopSounds()
{
    //Stop any possible score being played to avoid crashes
    if (m_pScore[0]) m_pScore[0]->Stop();
    if (m_pScore[1]) m_pScore[1]->Stop();

}
