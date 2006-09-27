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
/*! @file EarScalesCtrol.cpp
    @brief Implementation file for class lmEarScalesCtrol
    @ingroup html_controls
*/
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "EarScalesCtrol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "EarScalesCtrol.h"
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
extern bool g_fReleaseVersion;          // in TheApp.cpp
extern bool g_fReleaseBehaviour;        // in TheApp.cpp
extern bool g_fShowDebugLinks;          // in TheApp.cpp

//------------------------------------------------------------------------------------
// Implementation of lmEarScalesCtrol




//Layout definitions
const int BUTTONS_DISTANCE = 5;        //pixels
const int NUM_BUTTONS = est_Max;
const int NUM_COLS = 4;
const int NUM_ROWS = 5;

static wxString m_sButtonLabel[est_Max];

//IDs for controls
enum {
    ID_BUTTON = 3010,
    ID_LINK = ID_BUTTON + NUM_BUTTONS,
};


BEGIN_EVENT_TABLE(lmEarScalesCtrol, lmEarExerciseCtrol)
    EVT_COMMAND_RANGE (ID_BUTTON, ID_BUTTON+NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmEarScalesCtrol::OnRespButton)
END_EVENT_TABLE()


lmEarScalesCtrol::lmEarScalesCtrol(wxWindow* parent, wxWindowID id, 
                           lmScalesConstrains* pConstrains,
                           const wxPoint& pos, const wxSize& size, int style)
    : lmEarExerciseCtrol(parent, id, pConstrains, NUM_BUTTONS, pos, size, style )
{
    //initializations
    m_pConstrains = pConstrains;

    //create buttons for the answers
    Create();

    //allow to play scales
    m_nKey = earmDo;
    m_sRootNote = _T("c4");
    m_nInversion = 0;
    m_nMode = m_pConstrains->GetRandomMode();

    if (m_fTheoryMode) NewProblem();

}

lmEarScalesCtrol::~lmEarScalesCtrol()
{
}

void lmEarScalesCtrol::CreateAnswerButtons()
{
    //create buttons for the answers, two rows
    int iB = 0;
    for (iB=0; iB < NUM_BUTTONS; iB++)
        m_pAnswerButton[iB] = (wxButton*)NULL;

    m_pKeyboardSizer = new wxFlexGridSizer(NUM_ROWS+1, NUM_COLS+1, 10, 0);
    m_pMainSizer->Add(
        m_pKeyboardSizer,
        wxSizerFlags(0).Left().Border(wxALIGN_LEFT|wxTOP, 10) );

    for (int iRow=0; iRow < NUM_ROWS; iRow++) {
        m_pKeyboardSizer->Add(
            m_pRowLabel[iRow] = new wxStaticText(this, -1, _T("")),
            wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, BUTTONS_DISTANCE) );

        // the buttons for this row
        for (int iCol=0; iCol < NUM_COLS; iCol++) {
            iB = iCol + iRow * NUM_COLS;    // button index
            if (iB >= NUM_BUTTONS) break;
            m_pAnswerButton[iB] = new wxButton( this, ID_BUTTON + iB, _T("Undefined"),
                wxDefaultPosition, wxSize(120, 20));
            m_pKeyboardSizer->Add(
                m_pAnswerButton[iB],
                wxSizerFlags(0).Border(wxLEFT|wxRIGHT, BUTTONS_DISTANCE) );
            if (m_sButtonLabel[iB].IsEmpty()) {
                m_pAnswerButton[iB]->Show(false);
                m_pAnswerButton[iB]->Enable(false);
            }
        }
    }

}
void lmEarScalesCtrol::InitializeStrings()
{

        //button labels.

    // Major scales
    m_sButtonLabel[est_MajorTriad] = _("Natural");
    m_sButtonLabel[est_MajorTypeII] = _("Type II");
    m_sButtonLabel[est_MajorTypeIII] = _("Type III");
    m_sButtonLabel[est_MajorTypeIV] = _("Type IV");

    // Minor scales
    m_sButtonLabel[est_MinorNatural] = _("Natural");
    m_sButtonLabel[est_MinorDorian] = _("Dorian");
    m_sButtonLabel[est_MinorHarmonic] = _("Harmonic");
    m_sButtonLabel[est_MinorMelodic] = _("Melodic");

    // Greek scales
    m_sButtonLabel[est_GreekDorian] = _("Dorian");
    m_sButtonLabel[est_GreekPhrygian] = _("Phrygian");
    m_sButtonLabel[est_GreekLydian] = _("Lydian");

}

void lmEarScalesCtrol::SetUpButtons()
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
    for (int iRow=0; iRow < NUM_ROWS; iRow++) {
        m_pRowLabel[iRow]->SetLabel(_T(""));
    }

    //triads
    iB = 0;
    if (m_pConstrains->IsValidGroup(esg_Major)) {
        iR = 0;
        m_pRowLabel[iR]->SetLabel(_("Triads:"));
        for (iC=0; iC <= est_LastMajor; iC++) {
            if (m_pConstrains->IsScaleValid((EScaleType)iC)) {
                m_nRealChord[iB] = iC;
                m_pAnswerButton[iB]->SetLabel( m_sButtonLabel[iC] );
                m_pAnswerButton[iB]->Show(true);
                m_pAnswerButton[iB]->Enable(true);
                iB++;
                if (iB % NUM_COLS == 0) {
                    iR++;
                    m_pRowLabel[iR]->SetLabel(_T(""));
                }
           }
        }
    }
    if (iB % NUM_COLS != 0) iB += (NUM_COLS - (iB % NUM_COLS));

    //sevenths
    if (m_pConstrains->IsValidGroup(esg_Minor)) {
        iR = iB / NUM_COLS;
        m_pRowLabel[iR]->SetLabel(_("Seventh chords:"));
        for (iC=est_LastMajor+1; iC <= est_LastMinor; iC++) {
            if (m_pConstrains->IsScaleValid((EScaleType)iC)) {
                m_nRealChord[iB] = iC;
                m_pAnswerButton[iB]->SetLabel( m_sButtonLabel[iC] );
                m_pAnswerButton[iB]->Show(true);
                m_pAnswerButton[iB]->Enable(true);
                iB++;
                if (iB % NUM_COLS == 0) {
                    iR++;
                    m_pRowLabel[iR]->SetLabel(_T(""));
                }
           }
        }
    }
    if (iB % NUM_COLS != 0) iB += (NUM_COLS - (iB % NUM_COLS));

    //Other
    if (m_pConstrains->IsValidGroup(esg_Other)) {
        iR = iB / NUM_COLS;
        m_pRowLabel[iR]->SetLabel(_("Other scales:"));
        for (iC=est_LastMinor+1; iC < est_Max; iC++) {
            if (m_pConstrains->IsScaleValid((EScaleType)iC)) {
                m_nRealChord[iB] = iC;
                m_pAnswerButton[iB]->SetLabel( m_sButtonLabel[iC] );
                m_pAnswerButton[iB]->Show(true);
                m_pAnswerButton[iB]->Enable(true);
                iB++;
                if (iB % NUM_COLS == 0) {
                    iR++;
                    m_pRowLabel[iR]->SetLabel(_T(""));
                }
           }
        }
    }

    m_pKeyboardSizer->Layout();
}

//----------------------------------------------------------------------------------------
// Event handlers

void lmEarScalesCtrol::OnSettingsButton(wxCommandEvent& event)
{
    //lmScalesCtrol dlg(this, m_pConstrains, m_fTheoryMode);   
    //int retcode = dlg.ShowModal();
    //if (retcode == wxID_OK) {
    //    m_pConstrains->SaveSettings();
    //    // When changing interval settings it is necessary review the buttons
    //    // as number of buttons and/or its name could have changed.
    //    SetUpButtons();
    //}

}

void lmEarScalesCtrol::OnRespButton(wxCommandEvent& event)
{
    //First, stop any possible chord being played to avoid crashes
    if (m_pAuxScore) m_pAuxScore->Stop();
    if (m_pProblemScore) m_pProblemScore->Stop();

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
       }
        else {
            NewProblem();
        }
    }
    else {
        // No problem presented. The user press the button to play a chord

        //prepare the new chord and play it
        PrepareScore(eclvSol, (EScaleType)m_nRealChord[nIndex], &m_pAuxScore);
        m_pAuxScore->Play(lmNO_VISUAL_TRACKING, NO_MARCAR_COMPAS_PREVIO,
                            ePM_NormalInstrument, 320, (wxWindow*) NULL);
    }

}

void lmEarScalesCtrol::NewProblem()
{
    ResetExercise();

    //select a random mode
    m_nMode = m_pConstrains->GetRandomMode();

    // select a random key signature
    lmRandomGenerator oGenerator;
    m_nKey = oGenerator.GenerateKey( m_pConstrains->GetKeyConstrains() );
    
    //Generate a random root note 
    EClefType nClef = eclvSol;
    bool fAllowAccidentals = false;
    m_sRootNote = oGenerator.GenerateRandomRootNote(nClef, m_nKey, fAllowAccidentals);

    // generate a random chord
    EScaleType nScaleType = (EScaleType)1; //m_pConstrains->GetRandomChordType();
    m_nInversion = 0;
    //if (m_pConstrains->AreInversionsAllowed())
    //    m_nInversion = oGenerator.RandomNumber(0, NumNotesInChord(nScaleType) - 1);

    if (!m_pConstrains->DisplayKey()) m_nKey = earmDo;
    m_sAnswer = PrepareScore(nClef, nScaleType, &m_pProblemScore);
    
    //compute the index for the button that corresponds to the right answer
    int i;
    for (i = 0; i < NUM_BUTTONS; i++) {
        if (m_nRealChord[i] == nScaleType) break;
    }
    m_nRespIndex = i;
    
    
    //load total score into the control
    m_pScoreCtrol->SetScore(m_pProblemScore, true);   //true: the score must be hidden
    m_pScoreCtrol->DisplayMessage(_T(""), 0, true);     //true: clear the canvas

    //display the problem
    if (m_fTheoryMode) {
        //theory
        m_pScoreCtrol->DisplayScore(m_pProblemScore);
        m_pScoreCtrol->DisplayMessage(_("Identify the next scale:"), lmToLogicalUnits(5, lmMILLIMETERS), false);
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

wxString lmEarScalesCtrol::PrepareScore(EClefType nClef, EScaleType nType, lmScore** pScore)
{
//    //create the chord
//    lmChordManager oChordMngr(m_sRootNote, nType, m_nInversion, m_nKey);
//
//    //delete the previous score
//    if (*pScore) {
//        delete *pScore;
//        *pScore = (lmScore*)NULL;
//    }
//
//    //create a score with the chord
//    wxString sPattern;
//    lmNote* pNote;
//    lmLDPParser parserLDP;
//    lmLDPNode* pNode;
//    lmVStaff* pVStaff;
//
//    int nNumNotes = oChordMngr.GetNumNotes();
//    *pScore = new lmScore();
//    (*pScore)->SetTopSystemDistance( lmToLogicalUnits(5, lmMILLIMETERS) );    //5mm
//    (*pScore)->AddInstrument(1,0,0,_T(""));                     //one vstaff, MIDI channel 0, MIDI instr 0
//    pVStaff = (*pScore)->GetVStaff(1, 1);      //get first vstaff of instr.1
//    pVStaff->AddClef( eclvSol );
//    pVStaff->AddKeySignature( m_nKey );
//    pVStaff->AddTimeSignature(4 ,4, sbNO_VISIBLE );
//
////    pVStaff->AddEspacio 24
//    int i = (m_nMode == 2 ? nNumNotes-1 : 0);   // 2= melodic descending
//    sPattern = _T("(n ") + oChordMngr.GetPattern(i) + _T(" r)");
//    pNode = parserLDP.ParseText( sPattern );
//    pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
//    for (i=1; i < nNumNotes; i++) {
//        sPattern = (m_nMode == 0 ? _T("(na ") : _T("(n "));     // mode=0 -> harmonic
//        sPattern += oChordMngr.GetPattern((m_nMode == 2 ? nNumNotes-1-i : i));
//        sPattern +=  _T(" r)");
//        pNode = parserLDP.ParseText( sPattern );
//        pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
//    }
//    pVStaff->AddBarline(etb_EndBarline, sbNO_VISIBLE);
//
//    (*pScore)->Dump();  //dbg
//
//    //return the chord name
//    if (m_pConstrains->AreInversionsAllowed())
//        return oChordMngr.GetNameFull();       //name including inversion
//    else 
//        return oChordMngr.GetName();           //only name
//
    return _T("TODO");
}

void lmEarScalesCtrol::EnableButtons(bool fEnable)
{
    for (int iB=0; iB < NUM_BUTTONS; iB++) {
        m_pAnswerButton[iB]->Enable(fEnable);
    }
}

void lmEarScalesCtrol::ResetExercise()
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
    if (m_pProblemScore) {
        delete m_pProblemScore;
        m_pProblemScore = (lmScore*)NULL;
    }

}
