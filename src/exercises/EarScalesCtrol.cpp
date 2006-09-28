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
    : lmEarExerciseCtrol(parent, id, pConstrains, pos, size, style )
{
    //initializations
    m_pConstrains = pConstrains;

    //create buttons for the answers
    Create();

    //initializatios to allow to play scales
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

    //inform base class about the settings
    SetButtons(m_pAnswerButton, NUM_BUTTONS, ID_BUTTON);

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
    m_sButtonLabel[est_GreekMixolydian] = _("Mixolydian");
    m_sButtonLabel[est_GreekAeolian] = _("Aeolian");
    m_sButtonLabel[est_GreekIonian] = _("Ionian");
    m_sButtonLabel[est_GreekLocrian] = _("Locrian");

}

void lmEarScalesCtrol::ReconfigureButtons()
{
    //Reconfigure buttons keyboard depending on the scales allowed

    int iC;     // real scale. Correspondence to EScaleTypes
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
                m_nRealScale[iB] = iC;
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
                m_nRealScale[iB] = iC;
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
                m_nRealScale[iB] = iC;
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

wxDialog* lmEarScalesCtrol::GetSettingsDlg()
{
    //wxDialog* pDlg = new lmScalesCtrol(this, m_pConstrains, m_fTheoryMode);
    //return pDlg;
    return (wxDialog*)NULL;
}

void lmEarScalesCtrol::PrepareAuxScore(int nButton)
{
    PrepareScore(eclvSol, (EScaleType)m_nRealScale[nButton], &m_pAuxScore);
}

wxString lmEarScalesCtrol::SetNewProblem()
{
    //select a random mode
    m_nMode = m_pConstrains->GetRandomMode();

    // select a random key signature
    lmRandomGenerator oGenerator;
    m_nKey = oGenerator.GenerateKey( m_pConstrains->GetKeyConstrains() );
    
    //Generate a random root note 
    EClefType nClef = eclvSol;
    bool fAllowAccidentals = false;
    m_sRootNote = oGenerator.GenerateRandomRootNote(nClef, m_nKey, fAllowAccidentals);

    // generate a random scale
    EScaleType nScaleType = (EScaleType)1; //m_pConstrains->GetRandomChordType();
    m_nInversion = 0;
    //if (m_pConstrains->AreInversionsAllowed())
    //    m_nInversion = oGenerator.RandomNumber(0, NumNotesInChord(nScaleType) - 1);

    if (!m_pConstrains->DisplayKey()) m_nKey = earmDo;
    m_sAnswer = PrepareScore(nClef, nScaleType, &m_pProblemScore);
    
    //compute the index for the button that corresponds to the right answer
    int i;
    for (i = 0; i < NUM_BUTTONS; i++) {
        if (m_nRealScale[i] == nScaleType) break;
    }
    m_nRespIndex = i;
    
    //return string to introduce the problem
    if (m_fTheoryMode) {
        return _("Identify the next scale:");
    } else {
        //ear training
        return _("Press 'Play' to lesson it again");
    }

}

wxString lmEarScalesCtrol::PrepareScore(EClefType nClef, EScaleType nType, lmScore** pScore)
{
//    //create the scale object
//    lmChordManager oChordMngr(m_sRootNote, nType, m_nInversion, m_nKey);

    //delete the previous score
    if (*pScore) {
        delete *pScore;
        *pScore = (lmScore*)NULL;
    }

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
//    //return the scale name
//    if (m_pConstrains->AreInversionsAllowed())
//        return oChordMngr.GetNameFull();       //name including inversion
//    else 
//        return oChordMngr.GetName();           //only name
//
    *pScore = (lmScore*)NULL;
    return _T("TODO");
}
