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
/*! @file IdfyChordCtrol.cpp
    @brief Implementation file for class lmIdfyChordCtrol
    @ingroup html_controls
*/
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "IdfyChordCtrol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "IdfyChordCtrol.h"
#include "UrlAuxCtrol.h"
#include "Constrains.h"
#include "Generators.h"
#include "../auxmusic/Conversion.h"

#include "../ldp_parser/LDPParser.h"
#include "../auxmusic/Interval.h"
#include "../app/DlgCfgEarIntervals.h"
#include "../auxmusic/ChordManager.h"


#include "../globals/Colors.h"
extern lmColors* g_pColors;

// access to global external variables
extern bool g_fReleaseVersion;            // in TheApp.cpp
extern bool g_fReleaseBehaviour;        // in TheApp.cpp
extern bool g_fShowDebugLinks;            // in TheApp.cpp

//------------------------------------------------------------------------------------
// Implementation of lmIdfyChordCtrol




//Layout definitions
const int BUTTONS_DISTANCE = 5;        //pixels
const int NUM_BUTTONS = ect_Max;
const int NUM_COLS = 4;
const int NUM_ROWS = 5;

static wxString m_sButtonLabel[ect_Max];

//IDs for controls
enum {
    ID_LINK_SEE_SOURCE = 3000,
    ID_LINK_DUMP,
    ID_LINK_MIDI_EVENTS,
    ID_BUTTON,
    ID_LINK = ID_BUTTON + NUM_BUTTONS,
    ID_LINK_NEW_PROBLEM,
    ID_LINK_PLAY,
    ID_LINK_SOLUTION,
    ID_LINK_SETTINGS

};


BEGIN_EVENT_TABLE(lmIdfyChordCtrol, wxWindow)
    EVT_COMMAND_RANGE (ID_BUTTON, ID_BUTTON+NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmIdfyChordCtrol::OnRespButton)
    EVT_SIZE            (lmIdfyChordCtrol::OnSize)

    LM_EVT_URL_CLICK    (ID_LINK_SEE_SOURCE, lmIdfyChordCtrol::OnDebugShowSourceScore)
    LM_EVT_URL_CLICK    (ID_LINK_DUMP, lmIdfyChordCtrol::OnDebugDumpScore)
    LM_EVT_URL_CLICK    (ID_LINK_MIDI_EVENTS, lmIdfyChordCtrol::OnDebugShowMidiEvents)

    LM_EVT_URL_CLICK    (ID_LINK_NEW_PROBLEM, lmIdfyChordCtrol::OnNewProblem)
    LM_EVT_URL_CLICK    (ID_LINK_PLAY, lmIdfyChordCtrol::OnPlay)
    LM_EVT_URL_CLICK    (ID_LINK_SOLUTION, lmIdfyChordCtrol::OnDisplaySolution)
    LM_EVT_URL_CLICK    (ID_LINK_SETTINGS, lmIdfyChordCtrol::OnSettingsButton)
END_EVENT_TABLE()

IMPLEMENT_CLASS(lmIdfyChordCtrol, wxWindow)

lmIdfyChordCtrol::lmIdfyChordCtrol(wxWindow* parent, wxWindowID id, 
                           lmChordConstrains* pConstrains,
                           const wxPoint& pos, const wxSize& size, int style)
    : wxWindow(parent, id, pos, size, style )
{
    //initializations
    int i;
    for (i=0; i < NUM_BUTTONS; i++) { m_pAnswerButton[i] = (wxButton*)NULL; }
    m_fQuestionAsked = false;
    m_pChordScore = (lmScore*)NULL;
    m_pAuxScore = (lmScore*)NULL;
    m_pScoreCtrol = (lmScoreAuxCtrol*)NULL;
    m_pConstrains = pConstrains;
    m_fTheoryMode = m_pConstrains->IsTheoryMode();

    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated

        //button labels.

    // Triads
    m_sButtonLabel[ect_MajorTriad] = _("Major ");
    m_sButtonLabel[ect_MinorTriad] = _("Minor ");
    m_sButtonLabel[ect_AugTriad] = _("Augmented ");
    m_sButtonLabel[ect_DimTriad] = _("Diminished ");
    m_sButtonLabel[ect_Suspended_4th] = _("Suspended (4th)");
    m_sButtonLabel[ect_Suspended_2nd] = _("Suspended (2nd)");

    // Seventh chords
    m_sButtonLabel[ect_MajorSeventh] = _("Major 7th");
    m_sButtonLabel[ect_DominantSeventh] = _("Dominant 7th");
    m_sButtonLabel[ect_MinorSeventh] = _("Minor 7th");
    m_sButtonLabel[ect_DimSeventh] = _("Diminished 7th");
    m_sButtonLabel[ect_HalfDimSeventh] = _("Half dim. 7th");
    m_sButtonLabel[ect_AugMajorSeventh] = _("Aug. major 7th");
    m_sButtonLabel[ect_AugSeventh] = _("Augmented 7th");
    m_sButtonLabel[ect_MinorMajorSeventh] = _("Minor-major 7th");

    // Sixth chords
    m_sButtonLabel[ect_MajorSixth] = _("Major 6th");
    m_sButtonLabel[ect_MinorSixth] = _("Minor 6th");
    m_sButtonLabel[ect_AugSixth] = _("Augmented 6th");

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
    m_pScoreCtrol = new lmScoreAuxCtrol(this, -1, (lmScore*)NULL, wxDefaultPosition, wxSize(320,150), eSIMPLE_BORDER);
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
    

    //create buttons for the answers, two rows
    int iB = 0;

    wxFlexGridSizer* pKeyboardSizer = new wxFlexGridSizer(NUM_ROWS+1, NUM_COLS+1, 10, 0);
    pMainSizer->Add(
        pKeyboardSizer,
        wxSizerFlags(0).Left().Border(wxALIGN_LEFT|wxTOP, 10) );

    for (int iRow=0; iRow < NUM_ROWS; iRow++) {
        pKeyboardSizer->Add(
            m_pRowLabel[iRow] = new wxStaticText(this, -1, _T("")),
            wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, BUTTONS_DISTANCE) );

        // the buttons for this row
        for (int iCol=0; iCol < NUM_COLS; iCol++) {
            iB = iCol + iRow * NUM_COLS;    // button index
            if (iB >= NUM_BUTTONS) break;
            m_pAnswerButton[iB] = new wxButton( this, ID_BUTTON + iB, _T("Undefined"),
                wxDefaultPosition, wxSize(120, 20));
            pKeyboardSizer->Add(
                m_pAnswerButton[iB],
                wxSizerFlags(0).Border(wxLEFT|wxRIGHT, BUTTONS_DISTANCE) );
            if (m_sButtonLabel[iB].IsEmpty()) {
                m_pAnswerButton[iB]->Show(false);
                m_pAnswerButton[iB]->Enable(false);
            }
        }
    }

    SetSizer( pMainSizer );                 // use the sizer for window layout
    pMainSizer->SetSizeHints( this );       // set size hints to honour minimum size

    m_pScoreCtrol->DisplayMessage(_("Click on 'New problem' to start"), 
                                  lmToLogicalUnits(10, lmMILLIMETERS),
                                  true);

    m_pPlayButton->Enable(false);
    m_pShowSolution->Enable(false);

    SetUpButtons();     //reconfigure buttons in accordance with constrains

    //allow to play chords
    m_nKey = earmDo;
    m_sRootNote = _T("c4");
    m_nInversion = 0;
    m_nMode = m_pConstrains->GetRandomMode();

    if (m_fTheoryMode) NewProblem();

}

lmIdfyChordCtrol::~lmIdfyChordCtrol()
{
    //stop any possible chord being played
    if (m_pAuxScore) m_pAuxScore->Stop();
    if (m_pChordScore) m_pChordScore->Stop();

    //delete objects

    if (m_pScoreCtrol) {
        delete m_pScoreCtrol;
        m_pScoreCtrol = (lmScoreAuxCtrol*)NULL;
    }

    if (m_pConstrains) {
        delete m_pConstrains;
        m_pConstrains = (lmChordConstrains*) NULL;
    }

    if (m_pChordScore) {
        delete m_pChordScore;
        m_pChordScore = (lmScore*)NULL;
    }
    if (m_pAuxScore) {
        delete m_pAuxScore;
        m_pAuxScore = (lmScore*)NULL;
    }
}

void lmIdfyChordCtrol::SetUpButtons()
{
    //Reconfigure buttons keyboard depending on the chords allowed

    int iC;     // real chord. Correspondence to EChordTypes
    int iB;     // button index: 0 .. NUM_BUTTONS-1
    int iR;     // row index: 0 .. NUM_ROWS-1
    
    //hide all rows and buttons so that later we only have to enable the valid ones
    for (iB=0; iB < NUM_BUTTONS; iB++) {
        m_pAnswerButton[iB]->Show(false);
        m_pAnswerButton[iB]->Enable(false);
    }

    //triads
    iB = 0;
    if (m_pConstrains->IsValidGroup(ecg_Triads)) {
        iR = 0;
        m_pRowLabel[iR]->SetLabel(_("Triads:"));
        for (iC=0; iC <= ect_LastTriad; iC++) {
            if (m_pConstrains->IsValid((EChordType)iC)) {
                m_nRealChord[iB] = iC;
                m_pAnswerButton[iB]->SetLabel( m_sButtonLabel[iC] );
                m_pAnswerButton[iB]->Show(true);
                m_pAnswerButton[iB]->Enable(true);
                iB++;
                if (iB % NUM_COLS == 0) {
                    iR++;
                    m_pRowLabel[iR]->SetLabel(_(""));
                }
           }
        }
    }
    if (iB % NUM_COLS != 0) iB += (NUM_COLS - (iB % NUM_COLS));

    //sevenths
    if (m_pConstrains->IsValidGroup(ecg_Sevenths)) {
        iR = iB / NUM_COLS;
        m_pRowLabel[iR]->SetLabel(_("Seventh chords:"));
        for (iC=ect_LastTriad+1; iC <= ect_LastSeventh; iC++) {
            if (m_pConstrains->IsValid((EChordType)iC)) {
                m_nRealChord[iB] = iC;
                m_pAnswerButton[iB]->SetLabel( m_sButtonLabel[iC] );
                m_pAnswerButton[iB]->Show(true);
                m_pAnswerButton[iB]->Enable(true);
                iB++;
                if (iB % NUM_COLS == 0) {
                    iR++;
                    m_pRowLabel[iR]->SetLabel(_(""));
                }
           }
        }
    }
    if (iB % NUM_COLS != 0) iB += (NUM_COLS - (iB % NUM_COLS));

    //Other
    if (m_pConstrains->IsValidGroup(ecg_Sixths)) {
        iR = iB / NUM_COLS;
        m_pRowLabel[iR]->SetLabel(_("Other chords:"));
        for (iC=ect_LastSeventh+1; iC <= ect_Max; iC++) {
            if (m_pConstrains->IsValid((EChordType)iC)) {
                m_nRealChord[iB] = iC;
                m_pAnswerButton[iB]->SetLabel( m_sButtonLabel[iC] );
                m_pAnswerButton[iB]->Show(true);
                m_pAnswerButton[iB]->Enable(true);
                iB++;
                if (iB % NUM_COLS == 0) {
                    iR++;
                    m_pRowLabel[iR]->SetLabel(_(""));
                }
           }
        }
    }

}

//----------------------------------------------------------------------------------------
// Event handlers

void lmIdfyChordCtrol::OnSettingsButton(wxCommandEvent& event)
{
    /*
    lmDlgCfgEarIntervals dlg(this, m_pConstrains, true);    // true -> enable First note equal checkbox
    int retcode = dlg.ShowModal();
    if (retcode == wxID_OK) m_pConstrains->SaveSettings();
    */

}

void lmIdfyChordCtrol::OnSize(wxSizeEvent& event)
{
    //wxLogMessage(_T("OnSize at lmIdfyChordCtrol"));
    Layout();

}

void lmIdfyChordCtrol::OnPlay(wxCommandEvent& event)
{
    Play();
}

void lmIdfyChordCtrol::OnNewProblem(wxCommandEvent& event)
{
    NewProblem();
}

void lmIdfyChordCtrol::OnDisplaySolution(wxCommandEvent& event)
{
    //First, stop any possible chord being played to avoid crashes
    if (m_pAuxScore) m_pAuxScore->Stop();
    if (m_pChordScore) m_pChordScore->Stop();

    //now proceed
    m_pCounters->IncrementWrong();
    DisplaySolution();
}

void lmIdfyChordCtrol::OnRespButton(wxCommandEvent& event)
{
    //First, stop any possible chord being played to avoid crashes
    if (m_pAuxScore) m_pAuxScore->Stop();
    if (m_pChordScore) m_pChordScore->Stop();

    //identify button pressed
    int nIndex = event.GetId() - ID_BUTTON;

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
            m_pAnswerButton[m_nRespIndex]->SetBackgroundColour(g_pColors->Success());
            m_pAnswerButton[nIndex]->SetBackgroundColour(g_pColors->Failure());

            //show the solucion
            DisplaySolution();
            if (m_fTheoryMode) EnableButtons(false);
       }
        else {
            NewProblem();
        }
    }
    else {
        // No problem presented. The user press the button to play a chord

        //prepare the new chord and play it
        PrepareChord(eclvSol, (EChordType)m_nRealChord[nIndex], &m_pAuxScore);
        m_pAuxScore->Play(lmNO_VISUAL_TRACKING, NO_MARCAR_COMPAS_PREVIO,
                            ePM_NormalInstrument, 320, (wxWindow*) NULL);
    }

}

void lmIdfyChordCtrol::NewProblem()
{
    ResetExercise();

    //select a random mode
    m_nMode = m_pConstrains->GetRandomMode();

    // select a random key signature
    lmRandomGenerator oGenerator;
    m_nKey = oGenerator.RandomKeySignature();
    
    //Generate a random root note 
    EClefType nClef = eclvSol;
    bool fAllowAccidentals = false;
    m_sRootNote = oGenerator.GenerateRandomRootNote(nClef, m_nKey, fAllowAccidentals);

    // generate a random chord
    EChordType nChordType = m_pConstrains->GetRandomChordType();
    m_nInversion = 0;
    if (m_pConstrains->InversionsAllowed())
        m_nInversion = oGenerator.RandomNumber(0, NumNotesInChord(nChordType) - 1);

    if (!m_pConstrains->DisplayKey()) m_nKey = earmDo;
    m_sAnswer = PrepareChord(nClef, nChordType, &m_pChordScore);
    
    //compute the index for the button that corresponds to the right answer
    int i;
    for (i = 0; i < NUM_BUTTONS; i++) {
        if (m_nRealChord[i] == nChordType) break;
    }
    m_nRespIndex = i;
    
    
    //load total score into the control
    m_pScoreCtrol->SetScore(m_pChordScore, true);   //true: the score must be hidden
    m_pScoreCtrol->DisplayMessage(_T(""), 0, true);     //true: clear the canvas

    //display the problem
    if (m_fTheoryMode) {
        //theory
        m_pScoreCtrol->DisplayScore(m_pChordScore);
        m_pScoreCtrol->DisplayMessage(_("Identify the next chord:"), lmToLogicalUnits(5, lmMILLIMETERS), false);
        EnableButtons(true);
    } else {
        //ear training
        Play();
        wxString sProblem = _("Press 'Play' to lesson it again");
        m_pScoreCtrol->DisplayMessage(sProblem, lmToLogicalUnits(5, lmMILLIMETERS), false);
    }

    m_fQuestionAsked = true;
    m_pPlayButton->Enable(true);
    m_pShowSolution->Enable(true);


}

wxString lmIdfyChordCtrol::PrepareChord(EClefType nClef, EChordType nType, lmScore** pScore)
{
    //create the chord
    lmChordManager oChordMngr(m_sRootNote, nType, m_nInversion, m_nKey);

    //delete the previous score
    if (*pScore) {
        delete *pScore;
        *pScore = (lmScore*)NULL;
    }

    //create a score with the chord
    wxString sPattern;
    lmNote* pNote;
    lmLDPParser parserLDP;
    lmLDPNode* pNode;
    lmVStaff* pVStaff;

    int nNumNotes = oChordMngr.GetNumNotes();
    *pScore = new lmScore();
    (*pScore)->SetTopSystemDistance( lmToLogicalUnits(5, lmMILLIMETERS) );    //5mm
    (*pScore)->AddInstrument(1,0,0,_T(""));                     //one vstaff, MIDI channel 0, MIDI instr 0
    pVStaff = (*pScore)->GetVStaff(1, 1);      //get first vstaff of instr.1
    pVStaff->AddClef( eclvSol );
    pVStaff->AddKeySignature( m_nKey );
    pVStaff->AddTimeSignature(4 ,4, sbNO_VISIBLE );

//    pVStaff->AddEspacio 24
    int i = (m_nMode == 2 ? nNumNotes-1 : 0);   // 2= melodic descending
    sPattern = _T("(n ") + oChordMngr.GetPattern(i) + _T(" r)");
    pNode = parserLDP.ParseText( sPattern );
    pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
    for (i=1; i < nNumNotes; i++) {
        sPattern = (m_nMode == 0 ? _T("(na ") : _T("(n "));     // mode=0 -> harmonic
        sPattern += oChordMngr.GetPattern((m_nMode == 2 ? nNumNotes-1-i : i));
        sPattern +=  _T(" r)");
        pNode = parserLDP.ParseText( sPattern );
        pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
    }
    pVStaff->AddBarline(etb_EndBarline, sbNO_VISIBLE);

    //return the chord name
    if (m_pConstrains->InversionsAllowed())
        return oChordMngr.GetNameFull();       //name including inversion
    else 
        return oChordMngr.GetName();           //only name

}

void lmIdfyChordCtrol::EnableButtons(bool fEnable)
{
    for (int iB=0; iB < NUM_BUTTONS; iB++) {
        m_pAnswerButton[iB]->Enable(fEnable);
    }
}


void lmIdfyChordCtrol::Play()
{
    //As scale is built using whole notes, we will play scale at MM=320 so
    //that real note rate will be 80.
    m_pScoreCtrol->PlayScore(lmVISUAL_TRACKING, NO_MARCAR_COMPAS_PREVIO, 
                            ePM_NormalInstrument, 320);

}

void lmIdfyChordCtrol::DisplaySolution()
{
    m_pScoreCtrol->HideScore(false);
    m_pScoreCtrol->DisplayMessage(m_sAnswer, lmToLogicalUnits(5, lmMILLIMETERS), false);

    // mark right button in green
    m_pAnswerButton[m_nRespIndex]->SetBackgroundColour(g_pColors->Success());
    
    m_pPlayButton->Enable(true);
    m_pShowSolution->Enable(false);
    m_fQuestionAsked = false;
    if (m_fTheoryMode) EnableButtons(false);

}

void lmIdfyChordCtrol::OnDebugShowSourceScore(wxCommandEvent& event)
{
    m_pScoreCtrol->SourceLDP();
}

void lmIdfyChordCtrol::OnDebugDumpScore(wxCommandEvent& event)
{
    m_pScoreCtrol->Dump();
}

void lmIdfyChordCtrol::OnDebugShowMidiEvents(wxCommandEvent& event)
{
    m_pScoreCtrol->DumpMidiEvents();
}

void lmIdfyChordCtrol::ResetExercise()
{
    //clear the canvas
    m_pScoreCtrol->DisplayMessage(_T(""), 0, true);     //true: clear the canvas
    m_pScoreCtrol->Update();    //to force to clear it now

    // restore buttons' normal color
    for (int iB=0; iB < NUM_BUTTONS; iB++) {
        if (!m_sButtonLabel[iB].IsEmpty()) {
            m_pAnswerButton[iB]->SetBackgroundColour( g_pColors->Normal() );
        }
    }

    //delete the previous score
    if (m_pChordScore) {
        delete m_pChordScore;
        m_pChordScore = (lmScore*)NULL;
    }

}
