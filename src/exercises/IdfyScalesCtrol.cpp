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
#pragma implementation "IdfyScalesCtrol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "IdfyScalesCtrol.h"

#include "UrlAuxCtrol.h"
#include "Constrains.h"
#include "Generators.h"
#include "../auxmusic/Conversion.h"

#include "../ldp_parser/LDPParser.h"
#include "../auxmusic/Interval.h"
#include "../app/DlgCfgIdfyScale.h"
#include "../auxmusic/ScalesManager.h"


#include "../globals/Colors.h"
extern lmColors* g_pColors;

//access to error's logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;

//access to MIDI manager to get default settings for instrument to use
#include "../sound/MidiManager.h"


//------------------------------------------------------------------------------------
// Implementation of lmIdfyScalesCtrol




static wxString m_sButtonLabel[est_Max];

//IDs for controls
enum {
    ID_BUTTON = 3010,
};


BEGIN_EVENT_TABLE(lmIdfyScalesCtrol, lmOneScoreCtrol)
    EVT_COMMAND_RANGE (ID_BUTTON, ID_BUTTON+m_NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmIdfyScalesCtrol::OnRespButton)
END_EVENT_TABLE()


lmIdfyScalesCtrol::lmIdfyScalesCtrol(wxWindow* parent, wxWindowID id,
                           lmScalesConstrains* pConstrains,
                           const wxPoint& pos, const wxSize& size, int style)
    : lmOneScoreCtrol(parent, id, pConstrains, wxSize(450, 150), pos, size, style )
{
    //initializations
    m_pConstrains = pConstrains;

    //initializatios to allow to play scales
    m_nKey = earmDo;
    m_sRootNote = _T("c4");
    m_fAscending = m_pConstrains->GetRandomPlayMode();

    CreateControls();
    if (m_pConstrains->IsTheoryMode()) NewProblem();
}

lmIdfyScalesCtrol::~lmIdfyScalesCtrol()
{
}

void lmIdfyScalesCtrol::CreateAnswerButtons(int nHeight, int nSpacing, wxFont& font)
{
    //create buttons for the answers, two rows
    int iB = 0;
    for (iB=0; iB < m_NUM_BUTTONS; iB++)
        m_pAnswerButton[iB] = (wxButton*)NULL;

    m_pKeyboardSizer = new wxFlexGridSizer(m_NUM_ROWS+1, m_NUM_COLS+1, 2*nSpacing, 0);
    m_pMainSizer->Add(
        m_pKeyboardSizer,
        wxSizerFlags(0).Left().Border(wxALIGN_LEFT|wxTOP, 2*nSpacing) );

    for (int iRow=0; iRow < m_NUM_ROWS; iRow++)
    {
        m_pRowLabel[iRow] = new wxStaticText(this, -1, _T(""));
        m_pRowLabel[iRow]->SetFont(font);
        m_pKeyboardSizer->Add(
            m_pRowLabel[iRow],
            wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, nSpacing) );

        // the buttons for this row
        for (int iCol=0; iCol < m_NUM_COLS; iCol++)
        {
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

void lmIdfyScalesCtrol::InitializeStrings()
{

        //button labels.

    // Major scales
    m_sButtonLabel[est_MajorNatural] = _("Natural");
    m_sButtonLabel[est_MajorTypeII] = _("Type II");
    m_sButtonLabel[est_MajorTypeIII] = _("Type III");
    m_sButtonLabel[est_MajorTypeIV] = _("Type IV");

    // Minor scales
    m_sButtonLabel[est_MinorNatural] = _("Natural");
    m_sButtonLabel[est_MinorDorian] = _("Dorian");
    m_sButtonLabel[est_MinorHarmonic] = _("Harmonic");
    m_sButtonLabel[est_MinorMelodic] = _("Melodic");

    // Gregorian modes
    m_sButtonLabel[est_GreekIonian] = _("Ionian");
    m_sButtonLabel[est_GreekDorian] = _("Dorian");
    m_sButtonLabel[est_GreekPhrygian] = _("Phrygian");
    m_sButtonLabel[est_GreekLydian] = _("Lydian");
    m_sButtonLabel[est_GreekMixolydian] = _("Mixolydian");
    m_sButtonLabel[est_GreekAeolian] = _("Aeolian");
    m_sButtonLabel[est_GreekLocrian] = _("Locrian");

    // Other scales
    m_sButtonLabel[est_PentatonicMinor] = _("Pentatonic minor");
    m_sButtonLabel[est_PentatonicMajor] = _("Pentatonic major");
    m_sButtonLabel[est_Blues] = _("Blues");
    m_sButtonLabel[est_WholeTones] = _("Whole tones");
    m_sButtonLabel[est_Chromatic] = _("Chromatic");

}

void lmIdfyScalesCtrol::ReconfigureButtons()
{
    //Reconfigure buttons keyboard depending on the scales allowed

    int iB;     // button index: 0 .. m_NUM_BUTTONS-1

    //hide all rows and buttons so that later we only have to enable the valid ones
    for (iB=0; iB < m_NUM_BUTTONS; iB++) {
        m_pAnswerButton[iB]->Show(false);
        m_pAnswerButton[iB]->Enable(false);
    }
    for (int iRow=0; iRow < m_NUM_ROWS; iRow++) {
        m_pRowLabel[iRow]->SetLabel(_T(""));
    }

    //major scales
    iB = 0;
    if (m_pConstrains->IsValidGroup(esg_Major)) {
        iB = ReconfigureGroup(iB, 0, est_LastMajor, _("Major:"));
    }
    //minor scales
    if (m_pConstrains->IsValidGroup(esg_Minor)) {
         iB = ReconfigureGroup(iB, est_LastMajor+1, est_LastMinor, _("Minor:"));
    }
    //Gregorian modes
    if (m_pConstrains->IsValidGroup(esg_Gregorian)) {
         iB = ReconfigureGroup(iB, est_LastMinor+1, est_LastGreek, _("Gregorian modes:"));
    }
    //Other scales
    if (m_pConstrains->IsValidGroup(esg_Other)) {
         iB = ReconfigureGroup(iB, est_LastGreek+1, est_Max-1, _("Other scales:"));
    }

    m_pKeyboardSizer->Layout();
}

int lmIdfyScalesCtrol::ReconfigureGroup(int iBt, int iStartC, int iEndC, wxString sRowLabel)
{
    //Reconfigure a group of buttons


    int iC;     // real scale. Correspondence to EScaleTypes
    int iB;     // button index: 0 .. m_NUM_BUTTONS-1
    int iR;     // row index: 0 .. m_NUM_ROWS-1

    iB = iBt;
    iR = iB / m_NUM_COLS;
    m_pRowLabel[iR]->SetLabel(sRowLabel);
    for (iC=iStartC; iC <= iEndC; iC++) {
        if (m_pConstrains->IsScaleValid((EScaleType)iC)) {
            m_nRealScale[iB] = iC;
            m_pAnswerButton[iB]->SetLabel( m_sButtonLabel[iC] );
            m_pAnswerButton[iB]->Show(true);
            m_pAnswerButton[iB]->Enable(true);
            iB++;
            if (iB % m_NUM_COLS == 0) {
                iR++;
                m_pRowLabel[iR]->SetLabel(_T(""));
            }
        }
    }

    if (iB % m_NUM_COLS != 0) iB += (m_NUM_COLS - (iB % m_NUM_COLS));
    return iB;

}

wxDialog* lmIdfyScalesCtrol::GetSettingsDlg()
{
    wxDialog* pDlg = new lmDlgCfgIdfyScale(this, m_pConstrains, m_pConstrains->IsTheoryMode());
    return pDlg;
}

void lmIdfyScalesCtrol::PrepareAuxScore(int nButton)
{
    PrepareScore(eclvSol, (EScaleType)m_nRealScale[nButton], &m_pAuxScore);
}

wxString lmIdfyScalesCtrol::SetNewProblem()
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


    //select a random mode
    m_fAscending = m_pConstrains->GetRandomPlayMode();

    // generate a random scale
    EScaleType nScaleType = m_pConstrains->GetRandomScaleType();

    // select a key signature
    lmRandomGenerator oGenerator;
    m_nKey = oGenerator.GenerateKey( m_pConstrains->GetKeyConstrains() );
    // for minor scales use minor key signature
    if (nScaleType >= est_MinorNatural && nScaleType <= est_LastMinor)
        m_nKey = GetRelativeMinorKey(m_nKey);

    //Generate a random root note
    EClefType nClef = eclvSol;
    m_sRootNote = oGenerator.GenerateRandomRootNote(nClef, m_nKey, false);  //false = do not allow accidentals. Only those in the key signature

    //create the score
    bool fDisplayKey = m_pConstrains->DisplayKey() && IsTonalScale(nScaleType);
    if (!fDisplayKey) m_nKey = earmDo;
    m_sAnswer = PrepareScore(nClef, nScaleType, &m_pProblemScore);

    //compute the index for the button that corresponds to the right answer
    int i;
    for (i = 0; i < m_NUM_BUTTONS; i++) {
        if (m_nRealScale[i] == nScaleType) break;
    }
    m_nRespIndex = i;

    //if two solutions (minor/major or Gregorian mode) disable answer buttons
    //for the not valid answer
    DisableGregorianMajorMinor(nScaleType);

    //return string to introduce the problem
    if (m_pConstrains->IsTheoryMode()) {
        return _("Identify the next scale:");
    } else {
        //ear training
        return _("Press 'Play' to hear it again");
    }

}

wxString lmIdfyScalesCtrol::PrepareScore(EClefType nClef, EScaleType nType, lmScore** pScore)
{
//    //create the scale object
    lmScalesManager oScaleMngr(m_sRootNote, nType, m_nKey);

    //dbg------------------------------------------------------
    g_pLogger->LogTrace(_T("lmIdfyScalesCtrol"), _T("nClef = %d, nType = %d, m_sRootNote='%s', m_nKey=%d"),
                    nClef, nType, m_sRootNote.c_str(), m_nKey );
    //end dbg------------------------------------------------


    //delete the previous score
    if (*pScore) {
        delete *pScore;
        *pScore = (lmScore*)NULL;
    }

    //create a score with the scale
    wxString sPattern;
    lmNote* pNote;
    lmLDPParser parserLDP;
    lmLDPNode* pNode;
    lmVStaff* pVStaff;

    int nNumNotes = oScaleMngr.GetNumNotes();
    *pScore = new lmScore();
    (*pScore)->SetOption(_T("Render.SpacingMethod"), (long)esm_Fixed);
    (*pScore)->AddInstrument(1, g_pMidi->DefaultVoiceChannel(),
							 g_pMidi->DefaultVoiceInstr(), _T(""));
    pVStaff = (*pScore)->GetVStaff(1, 1);       //get first vstaff of instr.1
    (*pScore)->SetTopSystemDistance( pVStaff->TenthsToLogical(30, 1) );     // 3 lines
    pVStaff->AddClef( eclvSol );
    pVStaff->AddKeySignature( m_nKey );
    pVStaff->AddTimeSignature(4 ,4, lmNO_VISIBLE );

//    pVStaff->AddSpacer(30);       // 3 lines
    int i = (m_fAscending ? 0 : nNumNotes-1);
    sPattern = _T("(n ") + oScaleMngr.GetPattern(i) + _T(" r)");
    pNode = parserLDP.ParseText( sPattern );
    pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddSpacer(10);       // 1 lines
    for (i=1; i < nNumNotes; i++) {
        sPattern = _T("(n ");
        sPattern += oScaleMngr.GetPattern((m_fAscending ? i : nNumNotes-1-i));
        sPattern +=  _T(" r)");
        pNode = parserLDP.ParseText( sPattern );
        pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
        pVStaff->AddSpacer(10);       // 1 lines
        pVStaff->AddBarline(etb_SimpleBarline, lmNO_VISIBLE);   //so accidentals doesn't affect a 2nd note
    }
    pVStaff->AddBarline(etb_EndBarline, lmNO_VISIBLE);

    //use simple renderer; otherwise chromatic scale does not fit in available space
    (*pScore)->SetRenderizationType(eRenderSimple);

    //(*pScore)->Dump(_T("lmIdfyScalesCtrol.PrepareScore.ScoreDump.txt"));  //dbg

    //set metronome. As the problem score is built using whole notes, we will
    //set metronome at MM=400 so the resulting note rate will be 100.
    m_nPlayMM = 400;

    //return the scale name
    return oScaleMngr.GetName();

}

void lmIdfyScalesCtrol::DisableGregorianMajorMinor(EScaleType nType)
{
    // Gregorian scale Ionian has the same notes than Major natural and
    // Gregorian scale Aeolian has the same notes than the Minor natural.
    // When Gregorian answer buttons and maor or minor are enabled there
    // will be cases in which the answer is nor clear as two anserws are
    // posible. To solve this, we are going to disable one of the answer
    // buttons

    if ((m_pConstrains->IsValidGroup(esg_Major) || m_pConstrains->IsValidGroup(esg_Minor)) &&
         m_pConstrains->IsValidGroup(esg_Gregorian) )
    {
        EScaleType nDisable; 
        if (nType == est_GreekIonian && m_pConstrains->IsScaleValid(est_MajorNatural))
        {
            //disable major natural
            nDisable = est_MajorNatural;
        }
        else if (nType == est_GreekAeolian && m_pConstrains->IsScaleValid(est_MinorNatural))
        {
            //disable minor natural
            nDisable = est_MinorNatural;
        }
        else if (nType == est_MajorNatural && m_pConstrains->IsScaleValid(est_GreekIonian))
        {
            //disable Gregorian ionian
            nDisable = est_GreekIonian;
        }
        else if (nType == est_MinorNatural && m_pConstrains->IsScaleValid(est_GreekAeolian))
        {
            //disable Gregorian aeolian
            nDisable = est_GreekAeolian;
        }
        else
            return;     //not necessary to disable any button

        //compute the index for the button to disable
        int iB;
        for (iB = 0; iB < m_NUM_BUTTONS; iB++) {
            if (m_nRealScale[iB] == nDisable) break;
        }

        //disable button iB
        m_pAnswerButton[iB]->Enable(false);

    }

}

