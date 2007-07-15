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
#pragma implementation "IdfyCadencesCtrol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "IdfyCadencesCtrol.h"

#include "UrlAuxCtrol.h"
#include "Constrains.h"
#include "Generators.h"
#include "../auxmusic/Conversion.h"

#include "../ldp_parser/LDPParser.h"
//#include "../auxmusic/Interval.h"
#include "../app/DlgCfgIdfyCadence.h"
#include "../auxmusic/ChordManager.h"
#include "../auxmusic/Cadence.h"


#include "../globals/Colors.h"
extern lmColors* g_pColors;

//access to error's logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;

//access to MIDI manager to get default settings for instrument to use
#include "../sound/MidiManager.h"


//------------------------------------------------------------------------------------
// Implementation of lmIdfyCadencesCtrol




//Layout definitions
const int BUTTONS_DISTANCE = 5;        //pixels

static wxString m_sButtonLabel[lm_eCadMaxCadence];

//IDs for controls
enum {
    ID_BUTTON = 3010,
};


BEGIN_EVENT_TABLE(lmIdfyCadencesCtrol, lmIdfyExerciseCtrol)
    EVT_COMMAND_RANGE (ID_BUTTON, ID_BUTTON+m_NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmIdfyCadencesCtrol::OnRespButton)
END_EVENT_TABLE()


lmIdfyCadencesCtrol::lmIdfyCadencesCtrol(wxWindow* parent, wxWindowID id,
                           lmCadencesConstrains* pConstrains,
                           const wxPoint& pos, const wxSize& size, int style)
    : lmIdfyExerciseCtrol(parent, id, pConstrains, pos, size, style )
{
    //initializations
    m_pConstrains = pConstrains;

    //create the controls and buttons for the answers
    Create(400, 150);       //score ctrol size = 400x150 pixels

    //initializatios to allow to play scales
    //TODO: Review this
    m_nKey = earmDo;

    if (m_fTheoryMode) NewProblem();

}

lmIdfyCadencesCtrol::~lmIdfyCadencesCtrol()
{
}

void lmIdfyCadencesCtrol::CreateAnswerButtons()
{
    //create buttons for the answers, two rows
    int iB = 0;
    for (iB=0; iB < m_NUM_BUTTONS; iB++)
        m_pAnswerButton[iB] = (wxButton*)NULL;

    m_pKeyboardSizer = new wxFlexGridSizer(m_NUM_ROWS+1, m_NUM_COLS+1, 10, 0);
    m_pMainSizer->Add(
        m_pKeyboardSizer,
        wxSizerFlags(0).Left().Border(wxALIGN_LEFT|wxTOP, 10) );

    for (int iRow=0; iRow < m_NUM_ROWS; iRow++) {
        // the buttons for this row
        for (int iCol=0; iCol < m_NUM_COLS; iCol++) {
            iB = iCol + iRow * m_NUM_COLS;    // button index
            if (iB >= m_NUM_BUTTONS) break;
            m_pAnswerButton[iB] = new wxButton( this, ID_BUTTON + iB, _T("Undefined"),
                wxDefaultPosition, wxSize(120, 24));
            m_pKeyboardSizer->Add(
                m_pAnswerButton[iB],
                wxSizerFlags(0).Border(wxLEFT|wxRIGHT, BUTTONS_DISTANCE) );
        }
    }

    //inform base class about the settings
    SetButtons(m_pAnswerButton, m_NUM_BUTTONS, ID_BUTTON);

}

void lmIdfyCadencesCtrol::InitializeStrings()
{
}

void lmIdfyCadencesCtrol::ReconfigureButtons()
{
    //Reconfigure buttons keyboard depending on the required answers

    int iB;     // button index: 0 .. m_NUM_BUTTONS-1

    //hide all rows and buttons so that later we only have to enable the valid ones
    for (iB=0; iB < m_NUM_BUTTONS; iB++) {
        m_pAnswerButton[iB]->Show(false);
        m_pAnswerButton[iB]->Enable(false);
    }

    //Terminal cadences
    iB = 0;
    if (m_pConstrains->IsValidButton(lm_eCadButtonTerminal)) {
        iB = DisplayButton(iB, lm_eCadTerminal, lm_eCadLastTerminal, _("Terminal"));
    }
    //Transient cadences
    if (m_pConstrains->IsValidButton(lm_eCadButtonTransient)) {
         iB = DisplayButton(iB, lm_eCadTransient, lm_eCadLastTransient, _("Transient"));
    }

    //Perfect cadences
    if (m_pConstrains->IsValidButton(lm_eCadButtonPerfect)) {
         iB = DisplayButton(iB, lm_eCadPerfect, lm_eCadLastPerfect, _("Perfect"));
    }
    //Plagal cadences
    if (m_pConstrains->IsValidButton(lm_eCadButtonPlagal)) {
         iB = DisplayButton(iB, lm_eCadPlagal, lm_eCadLastPlagal, _("Plagal"));
    }

    //Imperfect cadences
    if (m_pConstrains->IsValidButton(lm_eCadButtonImperfect)) {
         iB = DisplayButton(iB, lm_eCadImperfect, lm_eCadLastImperfect, _("Imperfect"));
    }

    //Deceptive cadences
    if (m_pConstrains->IsValidButton(lm_eCadButtonDeceptive)) {
         iB = DisplayButton(iB, lm_eCadDeceptive, lm_eCadLastDeceptive, _("Deceptive"));
    }

    //Half cadences
    if (m_pConstrains->IsValidButton(lm_eCadButtonHalf)) {
         iB = DisplayButton(iB, lm_eCadHalf, lm_eCadLastHalf, _("Half cadence"));
    }


    m_pKeyboardSizer->Layout();
}

int lmIdfyCadencesCtrol::DisplayButton(int iBt, lmECadenceType iStartC,
                                       lmECadenceType iEndC, wxString sButtonLabel)
{
    // Display a button
    // iBt: number of button to display
    // iStartC-iEndC: range of cadences associated to this button
    // sButtonLabel: label for this button


    int iB;     // button index: 0 .. m_NUM_BUTTONS-1

    iB = iBt;
    m_nStartCadence[iB] = iStartC;
    m_nEndCadence[iB] = iEndC;
    m_pAnswerButton[iB]->SetLabel( sButtonLabel );
    m_pAnswerButton[iB]->Show(true);
    m_pAnswerButton[iB]->Enable(true);
    iB++;
    return iB;

}

wxDialog* lmIdfyCadencesCtrol::GetSettingsDlg()
{
    wxDialog* pDlg = new lmDlgCfgIdfyCadence(this, m_pConstrains, m_fTheoryMode);
    return pDlg;
}

void lmIdfyCadencesCtrol::PrepareAuxScore(int nButton)
{
    PrepareScore(eclvSol, m_nStartCadence[nButton], &m_pAuxScore);
}

wxString lmIdfyCadencesCtrol::SetNewProblem()
{
    //This method must prepare the problem score and set variables:
    //  m_pProblemScore, m_sAnswer, m_nRespIndex and m_nPlayMM

    // generate a random cadence
    lmECadenceType nCadenceType = m_pConstrains->GetRandomCadence();

    // select a key signature
    lmRandomGenerator oGenerator;
    m_nKey = oGenerator.GenerateKey( m_pConstrains->GetKeyConstrains() );

    //create the score
    EClefType nClef = eclvSol;
    m_sAnswer = PrepareScore(nClef, nCadenceType, &m_pProblemScore);

	// If it was not possible to cleate the cadence for this key signature, try
	// again with another cadence
	int nTimes = 0;
	while (m_sAnswer == _T("")) {
		nCadenceType = m_pConstrains->GetRandomCadence();
		m_sAnswer = PrepareScore(nClef, nCadenceType, &m_pProblemScore);
		if (++nTimes == 1000) {
			wxLogMessage(_T("[lmIdfyCadencesCtrol::SetNewProblem] Loop. Impossible to get a cadence."));
			break;
		}
	}

	//compute the index for the button that corresponds to the right answer
    int i;
    for (i = 0; i < m_NUM_BUTTONS; i++) {
        if (nCadenceType >= m_nStartCadence[i] && nCadenceType < m_nEndCadence[i]) break;
    }
    m_nRespIndex = i;

    //return string to introduce the problem
    if (m_fTheoryMode) {
        return _("Identify the next cadence:");
    } else {
        //ear training
        return _("Press 'Play' to hear it again");
    }

}

wxString lmIdfyCadencesCtrol::PrepareScore(EClefType nClef, lmECadenceType nType,
                                           lmScore** pScore)
{
    //create the chords
    lmCadence oCad;
    if (!oCad.Create(nType, m_nKey)) return _T("");

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

    *pScore = new lmScore();
    (*pScore)->SetTopSystemDistance( lmToLogicalUnits(5, lmMILLIMETERS) );    //5mm
    (*pScore)->SetOption(_T("Render.SpacingMethod"), (long)esm_Fixed);
    (*pScore)->AddInstrument(1, g_pMidi->DefaultVoiceChannel(),
							 g_pMidi->DefaultVoiceInstr(), _T(""));
    pVStaff = (*pScore)->GetVStaff(1, 1);      //get first vstaff of instr.1
    pVStaff->AddClef( nClef );
    pVStaff->AddKeySignature( m_nKey );
    pVStaff->AddTimeSignature(2 ,4);

    // Loop to add chords
    for (int iC=0; iC < oCad.GetNumChords(); iC++)
    {
        pVStaff->AddSpacer(15);
        if (iC != 0) pVStaff->AddBarline(etb_SimpleBarline);
        lmChordManager* pChord = oCad.GetChord(iC);
        int nNumNotes = pChord->GetNumNotes();
        sPattern = _T("(n ") + pChord->GetPattern(0) + _T(" r)");
        pNode = parserLDP.ParseText( sPattern );
        pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
        for (int i=1; i < nNumNotes; i++) {
            sPattern = _T("(na ");
            sPattern += pChord->GetPattern(i);
            sPattern +=  _T(" r)");
            pNode = parserLDP.ParseText( sPattern );
            pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
        }
    }
    pVStaff->AddSpacer(20);
    pVStaff->AddBarline(etb_EndBarline);

    //return cadence name
    return  oCad.GetName();

}
