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
#pragma implementation "IdfyNotesCtrol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "IdfyNotesCtrol.h"

#include "../score/VStaff.h"
#include "../score/Instrument.h"
#include "Constrains.h"
#include "Generators.h"
#include "../auxmusic/Conversion.h"

#include "../ldp_parser/LDPParser.h"
#include "dialogs/DlgCfgIdfyCadence.h"
#include "../auxmusic/Chord.h"
#include "../auxmusic/Cadence.h"


#include "../globals/Colors.h"
extern lmColors* g_pColors;

//access to error's logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;

//access to MIDI manager to get default settings for instrument to use
#include "../sound/MidiManager.h"


//------------------------------------------------------------------------------------
// Implementation of lmIdfyNotesCtrol




static wxString m_sButtonLabel[lm_eCadMaxCadence];

//IDs for controls
enum {
    ID_BUTTON = 3010,
};


BEGIN_EVENT_TABLE(lmIdfyNotesCtrol, lmOneScoreCtrol)
    EVT_COMMAND_RANGE (ID_BUTTON, ID_BUTTON+m_NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmIdfyNotesCtrol::OnRespButton)
END_EVENT_TABLE()


lmIdfyNotesCtrol::lmIdfyNotesCtrol(wxWindow* parent, wxWindowID id,
                           lmNotesConstrains* pConstrains,
                           const wxPoint& pos, const wxSize& size, int style)
    : lmOneScoreCtrol(parent, id, pConstrains, wxSize(400,200), pos, size, style )
{
    //initializations
    m_pConstrains = pConstrains;

    ////initializatios to allow to play cadences when clicking on answer buttons
    ////TODO: Review this
    //m_nKey = earmDo;

    CreateControls();
    if (m_pConstrains->IsTheoryMode())
        NewProblem();
}

lmIdfyNotesCtrol::~lmIdfyNotesCtrol()
{
}

void lmIdfyNotesCtrol::CreateAnswerButtons(int nHeight, int nSpacing, wxFont& font)
{
    //create buttons
    for (int iB=0; iB < m_NUM_BUTTONS; iB++)
    {
        m_pAnswerButton[iB] = new wxButton( this, ID_BUTTON + iB, m_pButtonLabel[iB],
            wxDefaultPosition, wxSize(16*nSpacing, nHeight));
        m_pAnswerButton[iB]->SetFont(font);
    }

    //main sizer: a grid with two rows and one column
    m_pKeyboardSizer = new wxFlexGridSizer(2, 1, 0, 0);
    m_pMainSizer->Add(
        m_pKeyboardSizer,
        wxSizerFlags(0).Left().Border(wxALIGN_LEFT|wxTOP, 2*nSpacing) );

    //first row: sharp/flat notes
	wxBoxSizer* pRowOneSizer = new wxBoxSizer( wxHORIZONTAL );
	pRowOneSizer->Add( 0, 0, 1, wxEXPAND, nSpacing );
	pRowOneSizer->Add( m_pAnswerButton[1], 0, wxALL, nSpacing );
	pRowOneSizer->Add( m_pAnswerButton[3], 0, wxALL, nSpacing );
	pRowOneSizer->Add( 0, 0, 1, wxEXPAND, nSpacing );
	pRowOneSizer->Add( m_pAnswerButton[6], 0, wxALL, nSpacing );
	pRowOneSizer->Add( m_pAnswerButton[8], 0, wxALL, nSpacing );
	pRowOneSizer->Add( m_pAnswerButton[10], 0, wxALL, nSpacing );
	pRowOneSizer->Add( 0, 0, 1, wxEXPAND, nSpacing );
	m_pKeyboardSizer->Add( pRowOneSizer, 0, wxEXPAND, nSpacing );

    //second row: natural notes
	wxBoxSizer* pRowTwoSizer = new wxBoxSizer( wxHORIZONTAL );
    pRowTwoSizer->Add( m_pAnswerButton[0], 0, wxALL, nSpacing );
	pRowTwoSizer->Add( m_pAnswerButton[2], 0, wxALL, nSpacing );
	pRowTwoSizer->Add( m_pAnswerButton[4], 0, wxALL, nSpacing );
	pRowTwoSizer->Add( m_pAnswerButton[5], 0, wxALL, nSpacing );
	pRowTwoSizer->Add( m_pAnswerButton[7], 0, wxALL, nSpacing );
	pRowTwoSizer->Add( m_pAnswerButton[9], 0, wxALL, nSpacing );
	pRowTwoSizer->Add( m_pAnswerButton[11], 0, wxALL, nSpacing );
	m_pKeyboardSizer->Add( pRowTwoSizer, 0, wxEXPAND, nSpacing );

    //inform base class about the settings
    SetButtons(m_pAnswerButton, m_NUM_BUTTONS, ID_BUTTON);
}

void lmIdfyNotesCtrol::InitializeStrings()
{
    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated

        // button labels
    m_pButtonLabel[0] = _("C");
    m_pButtonLabel[1] = _("C#/Db");
    m_pButtonLabel[2] = _("D");
    m_pButtonLabel[3] = _("D#/Eb");
    m_pButtonLabel[4] = _("E");
    m_pButtonLabel[5] = _("F");
    m_pButtonLabel[6] = _("F#/Gb");
    m_pButtonLabel[7] = _("G");
    m_pButtonLabel[8] = _("G#/Ab");
    m_pButtonLabel[9] = _("A");
    m_pButtonLabel[10] = _("A#/Bb");
    m_pButtonLabel[11] = _("B");
}

void lmIdfyNotesCtrol::OnSettingsChanged()
{
    //// The settings have been changed. Reconfigure answer keyboard for the new settings

    //int iB;     // button index: 0 .. m_NUM_BUTTONS-1

    ////hide all rows and buttons so that later we only have to enable the valid ones
    //for (iB=0; iB < m_NUM_BUTTONS; iB++) {
    //    m_pAnswerButton[iB]->Show(false);
    //    m_pAnswerButton[iB]->Enable(false);
    //}

    ////Terminal cadences
    //iB = 0;
    //if (m_pConstrains->IsValidButton(lm_eCadButtonTerminal)) {
    //    iB = DisplayButton(iB, lm_eCadTerminal, lm_eCadLastTerminal, _("Terminal"));
    //}
    ////Transient cadences
    //if (m_pConstrains->IsValidButton(lm_eCadButtonTransient)) {
    //     iB = DisplayButton(iB, lm_eCadTransient, lm_eCadLastTransient, _("Transient"));
    //}

    ////Perfect cadences
    //if (m_pConstrains->IsValidButton(lm_eCadButtonPerfect)) {
    //     iB = DisplayButton(iB, lm_eCadPerfect, lm_eCadLastPerfect, _("Perfect"));
    //}
    ////Plagal cadences
    //if (m_pConstrains->IsValidButton(lm_eCadButtonPlagal)) {
    //     iB = DisplayButton(iB, lm_eCadPlagal, lm_eCadLastPlagal, _("Plagal"));
    //}

    ////Imperfect cadences
    //if (m_pConstrains->IsValidButton(lm_eCadButtonImperfect)) {
    //     iB = DisplayButton(iB, lm_eCadImperfect, lm_eCadLastImperfect, _("Imperfect"));
    //}

    ////Deceptive cadences
    //if (m_pConstrains->IsValidButton(lm_eCadButtonDeceptive)) {
    //     iB = DisplayButton(iB, lm_eCadDeceptive, lm_eCadLastDeceptive, _("Deceptive"));
    //}

    ////Half cadences
    //if (m_pConstrains->IsValidButton(lm_eCadButtonHalf)) {
    //     iB = DisplayButton(iB, lm_eCadHalf, lm_eCadLastHalf, _("Half cadence"));
    //}


    //m_pKeyboardSizer->Layout();
}

int lmIdfyNotesCtrol::DisplayButton(int iBt, int iStartC,
                                       int iEndC, wxString sButtonLabel)
{
    //// Display a button
    //// iB: number of button to display
    //// iStartC-iEndC: range of cadences associated to this button
    //// sButtonLabel: label for this button


    //int iB;     // button index: 0 .. m_NUM_BUTTONS-1

    //iB = iBt;
    //m_nStartCadence[iB] = iStartC;
    //m_nEndCadence[iB] = iEndC;
    //m_pAnswerButton[iB]->SetLabel( sButtonLabel );
    //m_pAnswerButton[iB]->Show(true);
    //m_pAnswerButton[iB]->Enable(true);
    //iB++;
    //return iB;

    return 0;
}

wxDialog* lmIdfyNotesCtrol::GetSettingsDlg()
{
    //wxDialog* pDlg = new lmDlgCfgIdfyNotes(this, m_pConstrains, m_pConstrains->IsTheoryMode());
    //return pDlg;
    return NULL;
}

void lmIdfyNotesCtrol::PrepareAuxScore(int nButton)
{
    //PrepareScore(lmE_Sol, m_nStartCadence[nButton], &m_pAuxScore);
}

wxString lmIdfyNotesCtrol::SetNewProblem()
{
    //This method must prepare the problem score and set variables:
    //  m_pProblemScore, m_pSolutionScore, m_sAnswer, m_nRespIndex and m_nPlayMM

    // generate a random note
    wxString sNotePitch = _T("e4");     // = m_pConstrains->GetRandomNote();

    // select a key signature
    lmRandomGenerator oGenerator;
    m_nKey = earmRe;    //oGenerator.GenerateKey( m_pConstrains->GetKeyConstrains() );

    //create the score
    lmEClefType nClef = lmE_Sol;
    if (m_pConstrains->IsTheoryMode())
        m_sAnswer = PrepareScore(nClef, sNotePitch, &m_pProblemScore);
    else
        m_sAnswer = PrepareScore(nClef, sNotePitch, &m_pProblemScore, &m_pSolutionScore);

 //   //// For debugging and testing. Force to display and use the problem score for the
 //   //// solution score; the tonic chord is then visible
 //   //if (!m_pConstrains->IsTheoryMode()) {
 //   //    delete m_pSolutionScore;
 //   //    m_pSolutionScore = NULL;
 //   //}


	//compute the index for the button that corresponds to the right answer
    m_nRespIndex = 1;

    return _("Identify the next note:");
}

wxString lmIdfyNotesCtrol::PrepareScore(lmEClefType nClef, wxString& sNotePitch,
                                           lmScore** pProblemScore,
                                           lmScore** pSolutionScore)
{
    //delete the previous score
    if (*pProblemScore) {
        delete *pProblemScore;
        *pProblemScore = (lmScore*)NULL;
    }
    if (pSolutionScore) {
        delete *pSolutionScore;
        *pSolutionScore = (lmScore*)NULL;
    }

    //create the score with the note
    wxString sPattern;
    lmNote* pNote;
    lmLDPParser parserLDP;
    lmLDPNode* pNode;
    lmVStaff* pVStaff;

    *pProblemScore = new_score();
    (*pProblemScore)->SetOption(_T("Render.SpacingMethod"), (long)esm_Fixed);
    lmInstrument* pInstr = (*pProblemScore)->AddInstrument(g_pMidi->DefaultVoiceChannel(),
							g_pMidi->DefaultVoiceInstr(), _T(""));
    pVStaff = pInstr->GetVStaff();
    (*pProblemScore)->SetTopSystemDistance( pVStaff->TenthsToLogical(30, 1) );     // 3 lines
    pVStaff->AddClef( lmE_Sol );
    pVStaff->AddKeySignature( m_nKey );
    pVStaff->AddTimeSignature(2 ,4);

    //If ear training add A4 note
    if (m_pConstrains->IsEarTrainingMode())
    {
        sPattern = _T("(n a4 w)");
        pNode = parserLDP.ParseText( sPattern );
        pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
        pVStaff->AddBarline(lm_eBarlineSimple);

        sPattern = _T("(r w)");
        pNode = parserLDP.ParseText( sPattern );
        pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
        pVStaff->AddBarline(lm_eBarlineSimple);
    }

    // Now add the note to identify
    sPattern = _T("(n ") + sNotePitch + _T(" w)");
    pNode = parserLDP.ParseText( sPattern );
    pNote = parserLDP.AnalyzeNote(pNode, pVStaff);

    ////Prepare Solution Score
    //if (pSolutionScore) {
    //    *pSolutionScore = new lmScore();
    //    (*pSolutionScore)->SetOption(_T("Render.SpacingMethod"), (long)esm_Fixed);
    //    lmInstrument* pInstr = (*pSolutionScore)->AddInstrument(g_pMidi->DefaultVoiceChannel(),
				//			    g_pMidi->DefaultVoiceInstr(), _T(""));
    //    pVStaff = pInstr->GetVStaff();
    //    (*pSolutionScore)->SetTopSystemDistance( pVStaff->TenthsToLogical(30, 1) );     // 3 lines
    //    pVStaff->AddStaff(5);                       //add second staff: five lines, standard size
    //    pVStaff->AddClef( lmE_Sol, 1 );
    //    pVStaff->AddClef( lmE_Fa4, 2 );
    //    pVStaff->AddKeySignature( m_nKey );
    //    pVStaff->AddTimeSignature(2 ,4);

    //    // Loop to add chords
    //    for (int iC=0; iC < oCad.GetNumChords(); iC++)
    //    {
    //        pVStaff->AddSpacer(15);
    //        if (iC != 0) pVStaff->AddBarline(lm_eBarlineSimple);
    //        // first and second notes on F4 clef staff
    //        sPattern = _T("(n ") + oCad.GetNotePattern(iC, 0) + _T(" w p2)");
    //    //wxLogMessage(_T("[lmIdfyNotesCtrol::PrepareScore] sPattern='%s'"), sPattern.c_str());
    //        pNode = parserLDP.ParseText( sPattern );
    //        pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
    //        sPattern = _T("(na ") + oCad.GetNotePattern(iC, 1) + _T(" w p2)");
    //    //wxLogMessage(_T("[lmIdfyNotesCtrol::PrepareScore] sPattern='%s'"), sPattern.c_str());
    //        pNode = parserLDP.ParseText( sPattern );
    //        pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
    //        // third and fourth notes on G clef staff
    //        sPattern = _T("(na ") + oCad.GetNotePattern(iC, 2) + _T(" w p1)");
    //    //wxLogMessage(_T("[lmIdfyNotesCtrol::PrepareScore] sPattern='%s'"), sPattern.c_str());
    //        pNode = parserLDP.ParseText( sPattern );
    //        pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
    //        sPattern = _T("(na ") + oCad.GetNotePattern(iC, 3) + _T(" w p1)");
    //    //wxLogMessage(_T("[lmIdfyNotesCtrol::PrepareScore] sPattern='%s'"), sPattern.c_str());
    //        pNode = parserLDP.ParseText( sPattern );
    //        pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
    //    }
    //    pVStaff->AddSpacer(20);
    //    pVStaff->AddBarline(lm_eBarlineEnd);
    //}

    ////return cadence name
    //return  oCad.GetName();

    return _("The note is: ") + sNotePitch;

}
