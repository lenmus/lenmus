//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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
#pragma implementation "IdfyCadencesCtrol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "IdfyCadencesCtrol.h"

#include "../score/VStaff.h"
#include "Constrains.h"
#include "Generators.h"
#include "../auxmusic/Conversion.h"

#include "../ldp_parser/LDPParser.h"
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




static wxString m_sButtonLabel[lm_eCadMaxCadence];

//IDs for controls
enum {
    ID_BUTTON = 3010,
};


BEGIN_EVENT_TABLE(lmIdfyCadencesCtrol, lmOneScoreCtrol)
    EVT_COMMAND_RANGE (ID_BUTTON, ID_BUTTON+m_NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmIdfyCadencesCtrol::OnRespButton)
END_EVENT_TABLE()


lmIdfyCadencesCtrol::lmIdfyCadencesCtrol(wxWindow* parent, wxWindowID id,
                           lmCadencesConstrains* pConstrains,
                           const wxPoint& pos, const wxSize& size, int style)
    : lmOneScoreCtrol(parent, id, pConstrains, wxSize(400,200), pos, size, style )
{
    //initializations
    m_pConstrains = pConstrains;

    //initializatios to allow to play cadences when clicking on answer buttons
    //TODO: Review this
    m_nKey = earmDo;

    CreateControls();
    if (m_pConstrains->IsTheoryMode()) NewProblem();

}

lmIdfyCadencesCtrol::~lmIdfyCadencesCtrol()
{
}

void lmIdfyCadencesCtrol::CreateAnswerButtons(int nHeight, int nSpacing, wxFont& font)
{
    //create buttons for the answers, two rows
    int iB = 0;
    for (iB=0; iB < m_NUM_BUTTONS; iB++)
        m_pAnswerButton[iB] = (wxButton*)NULL;

    m_pKeyboardSizer = new wxFlexGridSizer(m_NUM_ROWS+1, m_NUM_COLS+1, 2*nSpacing, 0);
    m_pMainSizer->Add(
        m_pKeyboardSizer,
        wxSizerFlags(0).Left().Border(wxALIGN_LEFT|wxTOP, 2*nSpacing) );

    for (int iRow=0; iRow < m_NUM_ROWS; iRow++) {
        // the buttons for this row
        for (int iCol=0; iCol < m_NUM_COLS; iCol++) {
            iB = iCol + iRow * m_NUM_COLS;    // button index
            if (iB >= m_NUM_BUTTONS) break;
            m_pAnswerButton[iB] = new wxButton( this, ID_BUTTON + iB, _T("Undefined"),
                wxDefaultPosition, wxSize(24*nSpacing, nHeight));
            m_pAnswerButton[iB]->SetFont(font);

            m_pKeyboardSizer->Add(
                m_pAnswerButton[iB],
                wxSizerFlags(0).Border(wxLEFT|wxRIGHT, nSpacing) );
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
    // iB: number of button to display
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
    wxDialog* pDlg = new lmDlgCfgIdfyCadence(this, m_pConstrains, m_pConstrains->IsTheoryMode());
    return pDlg;
}

void lmIdfyCadencesCtrol::PrepareAuxScore(int nButton)
{
    PrepareScore(lmE_Sol, m_nStartCadence[nButton], &m_pAuxScore);
}

wxString lmIdfyCadencesCtrol::SetNewProblem()
{
    //This method must prepare the problem score and set variables:
    //  m_pProblemScore, m_pSolutionScore, m_sAnswer, m_nRespIndex and m_nPlayMM

    // generate a random cadence
    lmECadenceType nCadenceType = m_pConstrains->GetRandomCadence();

    // select a key signature
    lmRandomGenerator oGenerator;
    m_nKey = oGenerator.GenerateKey( m_pConstrains->GetKeyConstrains() );

    //create the score
    lmEClefType nClef = lmE_Sol;
    if (m_pConstrains->IsTheoryMode())
        m_sAnswer = PrepareScore(nClef, nCadenceType, &m_pProblemScore);
    else
        m_sAnswer = PrepareScore(nClef, nCadenceType, &m_pProblemScore, &m_pSolutionScore);

	// If it was not possible to create the cadence for this key signature, try
	// again with another cadence
	int nTimes = 0;
	while (m_sAnswer == _T("")) {
		nCadenceType = m_pConstrains->GetRandomCadence();
        if (m_pConstrains->IsTheoryMode())
            m_sAnswer = PrepareScore(nClef, nCadenceType, &m_pProblemScore);
        else
            m_sAnswer = PrepareScore(nClef, nCadenceType, &m_pProblemScore, &m_pSolutionScore);
		if (++nTimes == 1000) {
			wxLogMessage(_T("[lmIdfyCadencesCtrol::SetNewProblem] Loop. Impossible to get a cadence."));
			break;
		}
	}

    //// For debugging and testing. Force to display and use the problem score for the
    //// solution score; the tonic chord is then visible
    //if (!m_pConstrains->IsTheoryMode()) {
    //    delete m_pSolutionScore;
    //    m_pSolutionScore = NULL;
    //}


	//compute the index for the button that corresponds to the right answer
    if (m_pConstrains->IsValidButton(lm_eCadButtonTerminal)) {
        //Terminal / transient cadences
        if (nCadenceType >= lm_eCadTerminal && nCadenceType < lm_eCadLastTerminal)
            m_nRespIndex = 0;
        else
            m_nRespIndex = 1;
    }
    else
    {
        //Perfect / Plagal cadences
        m_nRespIndex = -1;      //not set
        int iB = -1;
        if (m_pConstrains->IsValidButton(lm_eCadButtonPerfect)) {
            iB++;
            if (nCadenceType >= lm_eCadPerfect && nCadenceType < lm_eCadLastPerfect)
                m_nRespIndex = iB;
        }
        //Plagal cadences
        if (m_nRespIndex == -1 && m_pConstrains->IsValidButton(lm_eCadButtonPlagal)) {
            iB++;
            if (nCadenceType >= lm_eCadPlagal && nCadenceType < lm_eCadLastPlagal)
                m_nRespIndex = iB;
        }

        //Imperfect cadences
        if (m_nRespIndex == -1 && m_pConstrains->IsValidButton(lm_eCadButtonImperfect)) {
            iB++;
            if (nCadenceType >= lm_eCadImperfect && nCadenceType < lm_eCadLastImperfect)
                m_nRespIndex = iB;
        }

        //Deceptive cadences
        if (m_nRespIndex == -1 && m_pConstrains->IsValidButton(lm_eCadButtonDeceptive)) {
            iB++;
            if (nCadenceType >= lm_eCadDeceptive && nCadenceType < lm_eCadLastDeceptive)
                m_nRespIndex = iB;
        }

        //Half cadences
        if (m_nRespIndex == -1 && m_pConstrains->IsValidButton(lm_eCadButtonHalf)) {
            iB++;
            if (nCadenceType >= lm_eCadHalf && nCadenceType < lm_eCadLastHalf)
                m_nRespIndex = iB;
        }
    }

    //return string to introduce the problem
    if (m_pConstrains->IsTheoryMode()) {
        return _("Identify the next cadence:");
    } else {
        //ear training
		wxString sText;
        if (m_pConstrains->GetKeyDisplayMode() == 0)
            sText = _("An A4 note will be played before the cadence begins.");
        else
            sText = _("A tonic chord will be played before the cadence begins.");
		sText += _T("\n");
		sText += _("Press 'Play' to hear the problem again.");
        return sText;
    }

}

wxString lmIdfyCadencesCtrol::PrepareScore(lmEClefType nClef, lmECadenceType nType,
                                           lmScore** pProblemScore,
                                           lmScore** pSolutionScore)
{
    //create the chords
    lmCadence oCad;
    if (!oCad.Create(nType, m_nKey, true)) return _T("");

    //delete the previous score
    if (*pProblemScore) {
        delete *pProblemScore;
        *pProblemScore = (lmScore*)NULL;
    }
    if (pSolutionScore) {
        delete *pSolutionScore;
        *pSolutionScore = (lmScore*)NULL;
    }

    //create a score with the chord
    wxString sPattern;
    lmNote* pNote;
    lmLDPParser parserLDP;
    lmLDPNode* pNode;
    lmVStaff* pVStaff;

    *pProblemScore = new lmScore();
    (*pProblemScore)->SetOption(_T("Render.SpacingMethod"), (long)esm_Fixed);
    lmInstrument* pInstr = (*pProblemScore)->AddInstrument(g_pMidi->DefaultVoiceChannel(),
							g_pMidi->DefaultVoiceInstr(), _T(""));
    pVStaff = pInstr->GetVStaff();
    (*pProblemScore)->SetTopSystemDistance( pVStaff->TenthsToLogical(30, 1) );     // 3 lines
    pVStaff->AddStaff(5);                       //add second staff: five lines, standard size
    pVStaff->AddClef( lmE_Sol, 1 );
    pVStaff->AddClef( lmE_Fa4, 2 );
    pVStaff->AddKeySignature( m_nKey );
    pVStaff->AddTimeSignature(2 ,4);

    //If ear training add A4/Tonic chord
    if (!m_pConstrains->IsTheoryMode())
    {
        //it is ear training exercise
        if (m_pConstrains->GetKeyDisplayMode() == 0) {
            // Use A4 note
            sPattern = _T("(n a4 r)");
            pNode = parserLDP.ParseText( sPattern );
            pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
        }
        else {
            // Use tonic chord
            lmChordManager* pChord = oCad.GetTonicChord();
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
        pVStaff->AddBarline(lm_eBarlineSimple);

        sPattern = _T("(s r)");
        pNode = parserLDP.ParseText( sPattern );
        pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
        pVStaff->AddBarline(lm_eBarlineSimple);
    }

    // Loop to add chords
    for (int iC=0; iC < oCad.GetNumChords(); iC++)
    {
        pVStaff->AddSpacer(15);
        if (iC != 0) pVStaff->AddBarline(lm_eBarlineSimple);
        // first and second notes on F4 clef staff
        sPattern = _T("(n ") + oCad.GetNotePattern(iC, 0) + _T(" r p2)");
    //wxLogMessage(_T("[lmIdfyCadencesCtrol::PrepareScore] sPattern='%s'"), sPattern.c_str());
        pNode = parserLDP.ParseText( sPattern );
        pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
        sPattern = _T("(na ") + oCad.GetNotePattern(iC, 1) + _T(" r p2)");
    //wxLogMessage(_T("[lmIdfyCadencesCtrol::PrepareScore] sPattern='%s'"), sPattern.c_str());
        pNode = parserLDP.ParseText( sPattern );
        pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
        // third and fourth notes on G clef staff
        sPattern = _T("(na ") + oCad.GetNotePattern(iC, 2) + _T(" r p1)");
    //wxLogMessage(_T("[lmIdfyCadencesCtrol::PrepareScore] sPattern='%s'"), sPattern.c_str());
        pNode = parserLDP.ParseText( sPattern );
        pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
        sPattern = _T("(na ") + oCad.GetNotePattern(iC, 3) + _T(" r p1)");
    //wxLogMessage(_T("[lmIdfyCadencesCtrol::PrepareScore] sPattern='%s'"), sPattern.c_str());
        pNode = parserLDP.ParseText( sPattern );
        pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
    }
    pVStaff->AddSpacer(20);
    pVStaff->AddBarline(lm_eBarlineEnd);

    //Prepare Solution Score
    if (pSolutionScore) {
        *pSolutionScore = new lmScore();
        (*pSolutionScore)->SetOption(_T("Render.SpacingMethod"), (long)esm_Fixed);
        lmInstrument* pInstr = (*pSolutionScore)->AddInstrument(g_pMidi->DefaultVoiceChannel(),
							    g_pMidi->DefaultVoiceInstr(), _T(""));
        pVStaff = pInstr->GetVStaff();
        (*pSolutionScore)->SetTopSystemDistance( pVStaff->TenthsToLogical(30, 1) );     // 3 lines
        pVStaff->AddStaff(5);                       //add second staff: five lines, standard size
        pVStaff->AddClef( lmE_Sol, 1 );
        pVStaff->AddClef( lmE_Fa4, 2 );
        pVStaff->AddKeySignature( m_nKey );
        pVStaff->AddTimeSignature(2 ,4);

        // Loop to add chords
        for (int iC=0; iC < oCad.GetNumChords(); iC++)
        {
            pVStaff->AddSpacer(15);
            if (iC != 0) pVStaff->AddBarline(lm_eBarlineSimple);
            // first and second notes on F4 clef staff
            sPattern = _T("(n ") + oCad.GetNotePattern(iC, 0) + _T(" r p2)");
        //wxLogMessage(_T("[lmIdfyCadencesCtrol::PrepareScore] sPattern='%s'"), sPattern.c_str());
            pNode = parserLDP.ParseText( sPattern );
            pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
            sPattern = _T("(na ") + oCad.GetNotePattern(iC, 1) + _T(" r p2)");
        //wxLogMessage(_T("[lmIdfyCadencesCtrol::PrepareScore] sPattern='%s'"), sPattern.c_str());
            pNode = parserLDP.ParseText( sPattern );
            pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
            // third and fourth notes on G clef staff
            sPattern = _T("(na ") + oCad.GetNotePattern(iC, 2) + _T(" r p1)");
        //wxLogMessage(_T("[lmIdfyCadencesCtrol::PrepareScore] sPattern='%s'"), sPattern.c_str());
            pNode = parserLDP.ParseText( sPattern );
            pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
            sPattern = _T("(na ") + oCad.GetNotePattern(iC, 3) + _T(" r p1)");
        //wxLogMessage(_T("[lmIdfyCadencesCtrol::PrepareScore] sPattern='%s'"), sPattern.c_str());
            pNode = parserLDP.ParseText( sPattern );
            pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
        }
        pVStaff->AddSpacer(20);
        pVStaff->AddBarline(lm_eBarlineEnd);
    }
    
    //return cadence name
    return  oCad.GetName();

}
