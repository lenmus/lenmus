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
#pragma implementation "IdfyChordCtrol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "IdfyChordCtrol.h"
#include "Constrains.h"
#include "Generators.h"
#include "../auxmusic/Conversion.h"

#include "../ldp_parser/LDPParser.h"
#include "../auxmusic/Interval.h"
#include "../app/DlgCfgIdfyChord.h"
#include "../auxmusic/ChordManager.h"


#include "../globals/Colors.h"
extern lmColors* g_pColors;

//access to MIDI manager to get default settings for instrument to use
#include "../sound/MidiManager.h"

//------------------------------------------------------------------------------------
// Implementation of lmIdfyChordCtrol




static wxString m_sButtonLabel[ect_Max];

//IDs for controls
enum {
    ID_BUTTON = 3010,
};


BEGIN_EVENT_TABLE(lmIdfyChordCtrol, lmOneScoreCtrol)
    EVT_COMMAND_RANGE (ID_BUTTON, ID_BUTTON+m_NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmIdfyChordCtrol::OnRespButton)
END_EVENT_TABLE()


lmIdfyChordCtrol::lmIdfyChordCtrol(wxWindow* parent, wxWindowID id,
                           lmChordConstrains* pConstrains,
                           const wxPoint& pos, const wxSize& size, int style)
    : lmOneScoreCtrol(parent, id, pConstrains, wxSize(320, 150), pos, size, style )
{
    //initializations
    m_pConstrains = pConstrains;

    //allow to play chords
    m_nKey = earmDo;
    m_sRootNote = _T("c4");
    m_nInversion = 0;
    m_nMode = m_pConstrains->GetRandomMode();

    CreateControls();
    if (m_pConstrains->IsTheoryMode()) NewProblem();
}

lmIdfyChordCtrol::~lmIdfyChordCtrol()
{
}

void lmIdfyChordCtrol::InitializeStrings()
{
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

}

void lmIdfyChordCtrol::CreateAnswerButtons(int nHeight, int nSpacing, wxFont& font)
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

void lmIdfyChordCtrol::ReconfigureButtons()
{
    // The settings has been changed. This method is invoked to reconfigure
    // answer keyboard in case it is needed for new settings

    //Reconfigure buttons keyboard depending on the chords allowed

    int iC;     // real chord. Correspondence to EChordTypes
    int iB;     // button index: 0 .. m_NUM_BUTTONS-1
    int iR;     // row index: 0 .. m_NUM_ROWS-1

    //hide all rows and buttons so that later we only have to enable the valid ones
    for (iB=0; iB < m_NUM_BUTTONS; iB++) {
        m_pAnswerButton[iB]->Show(false);
        m_pAnswerButton[iB]->Enable(false);
    }
    for (int iRow=0; iRow < m_NUM_ROWS; iRow++) {
        m_pRowLabel[iRow]->SetLabel(_T(""));
    }

    //triads
    iB = 0;
    if (m_pConstrains->IsValidGroup(ecg_Triads)) {
        iR = 0;
        m_pRowLabel[iR]->SetLabel(_("Triads:"));
        for (iC=0; iC <= ect_LastTriad; iC++) {
            if (m_pConstrains->IsChordValid((EChordType)iC)) {
                m_nRealChord[iB] = iC;
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
    }
    if (iB % m_NUM_COLS != 0) iB += (m_NUM_COLS - (iB % m_NUM_COLS));

    //sevenths
    if (m_pConstrains->IsValidGroup(ecg_Sevenths)) {
        iR = iB / m_NUM_COLS;
        m_pRowLabel[iR]->SetLabel(_("Seventh chords:"));
        for (iC=ect_LastTriad+1; iC <= ect_LastSeventh; iC++) {
            if (m_pConstrains->IsChordValid((EChordType)iC)) {
                m_nRealChord[iB] = iC;
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
    }
    if (iB % m_NUM_COLS != 0) iB += (m_NUM_COLS - (iB % m_NUM_COLS));

    //Other
    if (m_pConstrains->IsValidGroup(ecg_Sixths)) {
        iR = iB / m_NUM_COLS;
        m_pRowLabel[iR]->SetLabel(_("Other chords:"));
        for (iC=ect_LastSeventh+1; iC < ect_Max; iC++) {
            if (m_pConstrains->IsChordValid((EChordType)iC)) {
                m_nRealChord[iB] = iC;
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
    }

    m_pKeyboardSizer->Layout();
}

wxDialog* lmIdfyChordCtrol::GetSettingsDlg()
{
    wxDialog* pDlg = new lmDlgCfgIdfyChord(this, m_pConstrains, m_pConstrains->IsTheoryMode());
    return pDlg;
}

void lmIdfyChordCtrol::PrepareAuxScore(int nButton)
{
    // No problem is presented and the user press the button to play a specific 
    // sound (chord, interval, scale, etc.)
    // This method is then invoked to prepare the score with the requested sound.
    // At return, base class will play it

    PrepareScore(lmE_Sol, (EChordType)m_nRealChord[nButton], &m_pAuxScore);
}

wxString lmIdfyChordCtrol::SetNewProblem()
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
    m_nMode = m_pConstrains->GetRandomMode();

    // select a random key signature
    lmRandomGenerator oGenerator;
    m_nKey = oGenerator.GenerateKey( m_pConstrains->GetKeyConstrains() );

    //Generate a random root note
    lmEClefType nClef = lmE_Sol;
    bool fAllowAccidentals = false;
    m_sRootNote = oGenerator.GenerateRandomRootNote(nClef, m_nKey, fAllowAccidentals);

    // generate a random chord
    EChordType nChordType = m_pConstrains->GetRandomChordType();
    m_nInversion = 0;
    if (m_pConstrains->AreInversionsAllowed())
        m_nInversion = oGenerator.RandomNumber(0, NumNotesInChord(nChordType) - 1);

    if (!m_pConstrains->DisplayKey()) m_nKey = earmDo;
    m_sAnswer = PrepareScore(nClef, nChordType, &m_pProblemScore);

    //compute the index for the button that corresponds to the right answer
    int i;
    for (i = 0; i < m_NUM_BUTTONS; i++) {
        if (m_nRealChord[i] == nChordType) break;
    }
    m_nRespIndex = i;

    //return message to introduce the problem
    if (m_pConstrains->IsTheoryMode()) {
        //theory
        return _("Identify the next chord:");
    } else {
        //ear training
        return _("Press 'Play' to hear it again");
    }

}

wxString lmIdfyChordCtrol::PrepareScore(lmEClefType nClef, EChordType nType, lmScore** pScore)
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
    (*pScore)->SetOption(_T("Render.SpacingMethod"), (long)esm_Fixed);
    (*pScore)->AddInstrument(1, g_pMidi->DefaultVoiceChannel(),
							 g_pMidi->DefaultVoiceInstr(), _T(""));
    pVStaff = (*pScore)->GetVStaff(1, 1);      //get first vstaff of instr.1
    (*pScore)->SetTopSystemDistance( pVStaff->TenthsToLogical(30, 1) );     // 3 lines
    pVStaff->AddClef( lmE_Sol );
    pVStaff->AddKeySignature( m_nKey );
    pVStaff->AddTimeSignature(4 ,4, lmNO_VISIBLE );

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
    pVStaff->AddSpacer(30);       // 5 lines
    pVStaff->AddBarline(lm_eBarlineEnd, lmNO_VISIBLE);

    //(*pScore)->Dump();  //dbg

    //return the chord name
    if (m_pConstrains->AreInversionsAllowed())
        return oChordMngr.GetNameFull();       //name including inversion
    else
        return oChordMngr.GetName();           //only name

}
