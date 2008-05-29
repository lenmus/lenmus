//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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
#pragma implementation "EarIntervalsCtrol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "../score/VStaff.h"
#include "EarIntervalsCtrol.h"
#include "Constrains.h"
#include "Generators.h"
#include "../auxmusic/Conversion.h"

#include "../ldp_parser/LDPParser.h"
#include "../app/DlgCfgEarIntervals.h"


//access to MIDI manager to get default settings for instrument to use
#include "../sound/MidiManager.h"

//------------------------------------------------------------------------------------
// Implementation of lmEarIntervalsCtrol



static wxString m_sButtonLabel[lmEarIntervalsCtrol::m_NUM_BUTTONS];


BEGIN_EVENT_TABLE(lmEarIntervalsCtrol, lmOneScoreCtrol)
    EVT_COMMAND_RANGE (m_ID_BUTTON, m_ID_BUTTON+m_NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmEarIntervalsCtrol::OnRespButton)
END_EVENT_TABLE()


IMPLEMENT_CLASS(lmEarIntervalsCtrol, lmOneScoreCtrol)

lmEarIntervalsCtrol::lmEarIntervalsCtrol(wxWindow* parent, wxWindowID id, 
                           lmEarIntervalsConstrains* pConstrains,
                           const wxPoint& pos, const wxSize& size, int style)
    : lmOneScoreCtrol(parent, id, pConstrains, wxSize(360,150), pos, size, style )
{
    //initializations
    m_pConstrains = pConstrains;

    //ctrol options
    m_pConstrains->SetTheoryMode(false);
    m_pConstrains->SetButtonsEnabledAfterSolution(true);
    m_nPlayMM = 320;    //score build with whole notes, so metronome rate 320
                        //will force to play at 80 notes/minute

    //allow to play chords
    m_fAscending = true;
    m_nKey = earmDo;
    m_tNote[0].nAccidentals = 0;    //c4
    m_tNote[0].nOctave = 4;
    m_tNote[0].nStep = 0;
    m_tNote[0].nStepSemitones = 0;

    CreateControls();

}

void lmEarIntervalsCtrol::InitializeStrings()
{
    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    m_sButtonLabel[0] = _("Unison");
    m_sButtonLabel[1] = _("minor 2nd");
    m_sButtonLabel[2] = _("major 2nd");
    m_sButtonLabel[3] = _("minor 3rd");
    m_sButtonLabel[4] = _("major 3rd");
    m_sButtonLabel[5] = _("4th perfect");
    m_sButtonLabel[6] = _("aug.4th/dim.5th");
    m_sButtonLabel[7] = _("perfect 5th");
    m_sButtonLabel[8] = _("minor 6th");
    m_sButtonLabel[9] = _("major 6th");
    m_sButtonLabel[10] = _("minor 7th");
    m_sButtonLabel[11] = _("major 7th");
    m_sButtonLabel[12] = _("perfect 8th");
    m_sButtonLabel[13] = _("minor 9th");
    m_sButtonLabel[14] = _("major 9th");
    m_sButtonLabel[15] = _("minor 10th");
    m_sButtonLabel[16] = _("major 10th");
    m_sButtonLabel[17] = _("perfect 11th");
    m_sButtonLabel[18] = _("aug.11th / dim.12th");
    m_sButtonLabel[19] = _("perfect 12th");
    m_sButtonLabel[20] = _("minor 13th");
    m_sButtonLabel[21] = _("major 13th");
    m_sButtonLabel[22] = _("minor 14th");
    m_sButtonLabel[23] = _("major 14th");
    m_sButtonLabel[24] = _("two octaves");

}

void lmEarIntervalsCtrol::CreateAnswerButtons(int nHeight, int nSpacing, wxFont& font)
{
    //create up to 25 buttons for the answers: five rows, five buttons per row
    //Buttons are created disabled and no visible

    int iB = 0;
    for (iB=0; iB < m_NUM_BUTTONS; iB++)
        m_pAnswerButton[iB] = (wxButton*)NULL;

    m_pKeyboardSizer = new wxFlexGridSizer(m_NUM_ROWS, m_NUM_COLS, 2*nSpacing, 0);
    m_pMainSizer->Add(
        m_pKeyboardSizer,
        wxSizerFlags(0).Left().Border(wxALIGN_LEFT|wxTOP, 2*nSpacing) );

    for (int iRow=0; iRow < m_NUM_ROWS; iRow++)
    {
        for (int iCol=0; iCol < m_NUM_COLS; iCol++)
        {
            int iB = iCol + iRow * m_NUM_COLS;    // button index: 0 .. 24         
            m_pAnswerButton[iB] = new wxButton( this, m_ID_BUTTON + iB, _T("Undefined"),
                wxDefaultPosition, wxSize(26*nSpacing, nHeight));
            m_pAnswerButton[iB]->SetFont(font);
            m_pKeyboardSizer->Add(
                m_pAnswerButton[iB],
                wxSizerFlags(0).Border(wxRIGHT, nSpacing) );
        }
    }

    //inform base class about the settings
    SetButtons(m_pAnswerButton, m_NUM_BUTTONS, m_ID_BUTTON);

}

lmEarIntervalsCtrol::~lmEarIntervalsCtrol()
{
}

void lmEarIntervalsCtrol::ReconfigureButtons()
{
    // The settings has been changed. This method is invoked to reconfigure
    // answer keyboard in case it is needed for new settings

    // compute interval associated to each button
    int j = 0;
    for (int i=0; i < m_NUM_BUTTONS; i++) {
        if (m_pConstrains->IsIntervalAllowed(i)) {
            m_nRealIntval[j] = i;
            j++;
        }
    }
    m_nValidIntervals = j;


    //set up the button labels
    for (int iB = 0; iB < m_nValidIntervals; iB++) {
        m_pAnswerButton[iB]->SetLabel( m_sButtonLabel[m_nRealIntval[iB]] );
        m_pAnswerButton[iB]->Show(true);
        m_pAnswerButton[iB]->Enable(false);
    }

    // hide all non used buttons
    if (m_nValidIntervals < m_NUM_BUTTONS) {
        for (int iB = m_nValidIntervals; iB < m_NUM_BUTTONS; iB++) {
            m_pAnswerButton[iB]->Show(false);
        }
    }

}

wxDialog* lmEarIntervalsCtrol::GetSettingsDlg()
{
    return new lmDlgCfgEarIntervals(this, m_pConstrains);
}

void lmEarIntervalsCtrol::PrepareAuxScore(int nButton)
{

    // Get the interval associated to the pressed button
    wxString sCode;
    switch (m_nRealIntval[nButton]) {
        case ein_1:         sCode = _T("p1");    break;
        case ein_2min:      sCode = _T("m2");    break;
        case ein_2maj:      sCode = _T("M2");    break;
        case ein_3min:      sCode = _T("m3");    break;
        case ein_3maj:      sCode = _T("M3");    break;
        case ein_4:         sCode = _T("p4");    break;
        case ein_4aug:      sCode = _T("a4");    break;
        case ein_5:         sCode = _T("p5");    break;
        case ein_6min:      sCode = _T("m6");    break;
        case ein_6maj:      sCode = _T("M6");    break;
        case ein_7min:      sCode = _T("m7");    break;
        case ein_7maj:      sCode = _T("M7");    break;
        case ein_8:         sCode = _T("p8");    break;
        case ein_9min:      sCode = _T("m9");    break;
        case ein_9maj:      sCode = _T("M9");    break;
        case ein_10min:     sCode = _T("m10");   break;
        case ein_10maj:     sCode = _T("M10");   break;
        case ein_11:        sCode = _T("p11");   break;
        case ein_11aug:     sCode = _T("a11");   break;
        case ein_12:        sCode = _T("p12");   break;
        case ein_13min:     sCode = _T("m13");   break;
        case ein_13maj:     sCode = _T("M13");   break;
        case ein_14min:     sCode = _T("m14");   break;
        case ein_14maj:     sCode = _T("M14");   break;
        case ein_2oct:      sCode = _T("p15");   break;
    }

    //prepare the requested interval
    PrepareScore(sCode, &m_pAuxScore);

}

wxString lmEarIntervalsCtrol::SetNewProblem()
{
    //This method must prepare the problem score and set variables:
    //  m_pProblemScore - The score with the problem to propose
    //  m_pSolutionScore - The score with the solution or NULL if it is the
    //              same score than the problem score.
    //  m_sAnswer - the message to present when displaying the solution
    //  m_nRespIndex - the number of the button for the right answer
    //  m_nPlayMM - the speed to play the score
    //
    //It must return the message to display to introduce the problem.


    //choose if harmonic or melodic
    lmRandomGenerator oGenerator;
    if (m_pConstrains->IsTypeAllowed(0) && 
        !(m_pConstrains->IsTypeAllowed(1) || m_pConstrains->IsTypeAllowed(2)))
    {
        // if only harmonic (harmonic && !(melodic ascending or descending))
        // force harmonic
        m_fHarmonic = true;
    }
    else {
        m_fHarmonic = m_pConstrains->IsTypeAllowed(0) && oGenerator.FlipCoin();
    }

    // select interval type: ascending or descending
    if (m_fHarmonic) {
        // if harmonic it doesn't matter. Choose ascending
        m_fAscending = true;
    }
    else {
        if (m_pConstrains->IsTypeAllowed(1) && !m_pConstrains->IsTypeAllowed(2))
            m_fAscending = true;
        else if (!m_pConstrains->IsTypeAllowed(1) && m_pConstrains->IsTypeAllowed(2))
            m_fAscending = false;
        else
            m_fAscending = oGenerator.FlipCoin();
    }

    // select a random key signature satisfying the constraints
    if (m_pConstrains->OnlyNatural()) {
        m_nKey = oGenerator.GenerateKey(m_pConstrains->GetKeyConstrains());
    }
    else {
        m_nKey = earmDo;
    }
    // generate interval
    lmInterval oIntv(m_pConstrains->OnlyNatural(), m_pConstrains->MinNote(),
        m_pConstrains->MaxNote(), m_pConstrains->AllowedIntervals(), m_fAscending, m_nKey);

    //save the interval data
    m_sIntvCode = oIntv.GetIntervalCode();
    oIntv.GetNoteBits(0, &m_tNote[0]);
    oIntv.GetNoteBits(1, &m_tNote[1]);

    // all data ready to prepare the score: proceed
    PrepareScore(m_sIntvCode, &m_pProblemScore);

    //compute the right answer
    m_sAnswer = oIntv.GetIntervalName();

    //compute the index for the button that corresponds to the right answer
    int i;
    for (i = 0; i <= m_nValidIntervals; i++) {
        if (m_nRealIntval[i] == oIntv.GetNumSemitones()) break;
    }
    m_nRespIndex = i;

    return _T("");

}

void lmEarIntervalsCtrol::PrepareScore(wxString& sIntvCode, lmScore** pScore)
{
    //create the interval
    lmNoteBits tBits[2];
    tBits[0] = m_tNote[0];
    ComputeInterval( &tBits[0], sIntvCode, m_fAscending, &tBits[1] );

    //delete the previous score
    if (*pScore) {
        delete *pScore;
        *pScore = (lmScore*)NULL;
    }

    //create a score with the interval
    wxString sPattern;
    lmNote* pNote;
    lmLDPParser parserLDP;
    lmLDPNode* pNode;
    *pScore = new lmScore();
    (*pScore)->SetOption(_T("Render.SpacingMethod"), (long)esm_Fixed);
    lmInstrument* pInstr = (*pScore)->AddInstrument(g_pMidi->DefaultVoiceChannel(),
							 g_pMidi->DefaultVoiceInstr(), _T(""));
    lmVStaff *pVStaff = pInstr->GetVStaff();
    (*pScore)->SetTopSystemDistance( pVStaff->TenthsToLogical(30, 1) );     // 3 lines
    pVStaff->AddClef( lmE_Sol );
    pVStaff->AddKeySignature(m_nKey);
    pVStaff->AddTimeSignature(4 ,4, lmNO_VISIBLE );
//    pVStaff->AddSpacer(30);       // 3 lines
    //First note
    sPattern = _T("(n ") + lmConverter::NoteBitsToName(tBits[0], m_nKey) + _T(" r)");
    pNode = parserLDP.ParseText( sPattern );
    pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
    //second note
    if (m_fHarmonic)
        sPattern = _T("(na ");
        //todo: is it necessary to avoid propagation of the accidental to the second note
    else {
        pVStaff->AddSpacer(20);
        pVStaff->AddBarline(lm_eBarlineSimple, lmNO_VISIBLE);    //so that accidental doesn't affect 2nd note
        sPattern = _T("(n ");
    }
    sPattern += lmConverter::NoteBitsToName(tBits[1], m_nKey) + _T(" r)");
    pNode = parserLDP.ParseText( sPattern );
    pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddSpacer(60);
    pVStaff->AddBarline(lm_eBarlineSimple, lmNO_VISIBLE);

}

