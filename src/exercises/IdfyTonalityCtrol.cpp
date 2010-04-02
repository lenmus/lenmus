//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-1010 LenMus project
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
#pragma implementation "IdfyTonalityCtrol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "IdfyTonalityCtrol.h"
#include "Constrains.h"
#include "Generators.h"
#include "../score/VStaff.h"
#include "../score/Instrument.h"
#include "../auxmusic/Conversion.h"
#include "../score/KeySignature.h"
#include "../ldp_parser/LDPParser.h"
#include "../auxmusic/Chord.h"
#include "../auxmusic/Cadence.h"
#include "../auxmusic/ScalesManager.h"
#include "dialogs/DlgCfgIdfyTonality.h"


#include "../globals/Colors.h"
extern lmColors* g_pColors;

//access to error's logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;

//access to MIDI manager to get default settings for instrument to use
#include "../sound/MidiManager.h"


//------------------------------------------------------------------------------------
// Implementation of lmIdfyTonalityCtrol




static wxString m_sButtonLabel[lm_eCadMaxCadence];

//IDs for controls
enum {
    ID_BUTTON = 3010,
};


BEGIN_EVENT_TABLE(lmIdfyTonalityCtrol, lmOneScoreCtrol)
    EVT_COMMAND_RANGE (ID_BUTTON, ID_BUTTON+m_NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmIdfyTonalityCtrol::OnRespButton)
END_EVENT_TABLE()


lmIdfyTonalityCtrol::lmIdfyTonalityCtrol(wxWindow* parent, wxWindowID id,
                           lmTonalityConstrains* pConstrains,
                           const wxPoint& pos, const wxSize& size, int style)
    : lmOneScoreCtrol(parent, id, pConstrains, wxSize(400,200), pos, size, style )
    , m_pConstrains(pConstrains)
{
    CreateControls();
}

lmIdfyTonalityCtrol::~lmIdfyTonalityCtrol()
{
}

void lmIdfyTonalityCtrol::CreateAnswerButtons(int nHeight, int nSpacing, wxFont& font)
{
    //create buttons for the answers, 5 columns, 5 rows
    int iB = 0;
    for (iB=0; iB < m_NUM_BUTTONS; iB++)
        m_pAnswerButton[iB] = (wxButton*)NULL;

    m_pKeyboardSizer = new wxFlexGridSizer(m_NUM_ROWS+1, m_NUM_COLS+1, 2*nSpacing, 0);
    m_pMainSizer->Add(
        m_pKeyboardSizer,
        wxSizerFlags(0).Left().Border(wxALIGN_LEFT|wxTOP, 2*nSpacing) );

    for (int iRow=0; iRow < m_NUM_ROWS; iRow++)
    {
        // the buttons for this row
        for (int iCol=0; iCol < m_NUM_COLS; iCol++)
        {
            iB = iCol + iRow * m_NUM_COLS;    // button index
            if (iB >= m_NUM_BUTTONS) break;
            m_pAnswerButton[iB] = new wxButton( this, ID_BUTTON + iB, _T("Undefined"),
                wxDefaultPosition, wxSize(20*nSpacing, nHeight));
            m_pAnswerButton[iB]->SetFont(font);

            m_pKeyboardSizer->Add(
                m_pAnswerButton[iB],
                wxSizerFlags(0).Border(wxLEFT|wxRIGHT, nSpacing) );
        }
    }

    //inform base class about the settings
    SetButtons(m_pAnswerButton, m_NUM_BUTTONS, ID_BUTTON);
}

void lmIdfyTonalityCtrol::InitializeStrings()
{
}

void lmIdfyTonalityCtrol::OnSettingsChanged()
{
    // The settings have been changed. Reconfigure answer keyboard for the new settings

    //set buttons
    int iB = 0;
    if (m_pConstrains->UseMajorMinorButtons())
    {
        //Only major / minor buttons
        m_pAnswerButton[iB]->SetLabel( _("Major") );
        m_pAnswerButton[iB]->Show(true);
        m_pAnswerButton[iB]->Enable(true);
        iB++;
        m_pAnswerButton[iB]->SetLabel( _("Minor") );
        m_pAnswerButton[iB]->Show(true);
        m_pAnswerButton[iB]->Enable(true);
        iB++;
    }

    else
    {
        //use a button for each enabled key signature
        for (int i = lmMIN_KEY; i <= lmMAX_KEY; i++)
        {
            lmEKeySignatures nKey = static_cast<lmEKeySignatures>(i);
            if (m_pConstrains->IsValidKey(nKey))
            {
                m_nRealKey[iB] = nKey;
                m_pAnswerButton[iB]->SetLabel( GetKeySignatureName(nKey) );
                m_pAnswerButton[iB]->Show(true);
                m_pAnswerButton[iB]->Enable(true);
                iB++;
            }
            else
            {
                m_pAnswerButton[iB]->Show(false);
                m_pAnswerButton[iB]->Enable(false);
            }
        }
    }

    //hide all other buttons
    while (iB < m_NUM_BUTTONS)
    {
        m_pAnswerButton[iB]->Show(false);
        m_pAnswerButton[iB]->Enable(false);
        iB++;
    }

    m_pKeyboardSizer->Layout();
}

wxDialog* lmIdfyTonalityCtrol::GetSettingsDlg()
{
    wxDialog* pDlg = new lmDlgCfgIdfyTonality(this, m_pConstrains);
    return pDlg;
}

void lmIdfyTonalityCtrol::PrepareAuxScore(int nButton)
{
    if (m_pConstrains->UseMajorMinorButtons())
    {
        //if major/minor buttons do not generate score
        if (m_pAuxScore)
            delete m_pAuxScore;
        m_pAuxScore = (lmScore*)NULL;
    }
    else
        PrepareScore(lmE_Sol, m_nRealKey[nButton], &m_pAuxScore);
}

wxString lmIdfyTonalityCtrol::SetNewProblem()
{
    //This method must prepare the problem score and set variables:
    //  m_pProblemScore, m_pSolutionScore, m_sAnswer, m_nRespIndex and m_nPlayMM

    // select a key signature
    lmRandomGenerator oGenerator;
    m_nKey = oGenerator.GenerateKey( m_pConstrains->GetKeyConstrains() );

    //create the score
    lmEClefType nClef = lmE_Sol;
    m_sAnswer = PrepareScore(nClef, m_nKey, &m_pProblemScore);

	//compute the index for the button that corresponds to the right answer
    m_nRespIndex = -1;
    if (m_pConstrains->UseMajorMinorButtons())
    {
        m_nRespIndex = (lmIsMajorKey(m_nKey) ? 0 : 1);
    }
    else
    {
        for (int iB=0; iB < m_NUM_BUTTONS; iB++)
        {
            if (m_nRealKey[iB] == m_nKey)
            {
                m_nRespIndex = iB;
                break;
            }
        }
    }
    wxASSERT(m_nRespIndex >=0 && m_nRespIndex < m_NUM_BUTTONS);

    //return string to introduce the problem
	wxString sText = _("Press 'Play' to hear the problem again.");
    return sText;
}

wxString lmIdfyTonalityCtrol::PrepareScore(lmEClefType nClef, lmEKeySignatures nKey,
                                           lmScore** pProblemScore,
                                           lmScore** pSolutionScore)
{
    //delete the previous score
    if (*pProblemScore) {
        delete *pProblemScore;
        *pProblemScore = (lmScore*)NULL;
    }

    //determine tonic note
    //                             1 1 1 1 1 2 2 2 2 2 3 3 3 3 3 4 4 4 4 4 5 5 5 5 5 6 6 6 6 6
    //                   0 2 4 6 8 0 2 4 6 8 0 2 4 6 8 0 2 4 6 8 0 2 4 6 8 0 2 4 6 8 0 2 4 6 8
    wxString notes = _T("c2d2e2f2g2a2b2c3d3e3f3g3a3b3c4d4e4f4g4a4b4c5d5e5f5g5a5b5c6d6e6f6g6a6b6");
    int nRoot = lmGetRootNoteStep(nKey)* 2 + 14;  //note in octave 3
    wxString note[16];  //4 notes per chord

    //I
    note[0] = notes.substr(nRoot, 2);       //I
    note[1] = notes.substr(nRoot+8, 2);     //V
    note[2] = notes.substr(nRoot+14, 2);    //I
    note[3] = notes.substr(nRoot+18, 2);    //III
    //IV
    note[4] = notes.substr(nRoot-8, 2);     //IV
    note[5] = notes.substr(nRoot, 2);       //I
    note[6] = notes.substr(nRoot+6, 2);     //IV
    note[7] = notes.substr(nRoot+10, 2);    //VI
    //V
    note[8] = notes.substr(nRoot-6, 2);     //V
    note[9] = notes.substr(nRoot+2, 2);     //II
    note[10] = notes.substr(nRoot+8, 2);    //V
    note[11] = notes.substr(nRoot+12, 2);   //VII
    //I
    note[12] = notes.substr(nRoot, 2);       //I
    note[13] = notes.substr(nRoot+8, 2);     //V
    note[14] = notes.substr(nRoot+14, 2);    //I
    note[15] = notes.substr(nRoot+18, 2);    //III

    //create the score
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
    pVStaff->AddClef( lmE_Sol );
    pVStaff->AddKeySignature( nKey );
    pVStaff->AddTimeSignature(2 ,4);

    //add A4 note
    sPattern = _T("(n =a4 w)");
    pNode = parserLDP.ParseText( sPattern );
    pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddBarline(lm_eBarlineSimple);

    sPattern = _T("(r w)");
    pNode = parserLDP.ParseText( sPattern );
    pNote = parserLDP.AnalyzeNote(pNode, pVStaff);

    // Loop to add chords
    int i=0;
    for (int iC=0; iC < 4; iC++)
    {
        pVStaff->AddSpacer(15);
        pVStaff->AddBarline(lm_eBarlineSimple);

        sPattern = _T("(n ") + note[i++] + _T(" w)");
        pNode = parserLDP.ParseText( sPattern );
        parserLDP.AnalyzeNote(pNode, pVStaff);

        for (int iN=1; iN < 4; iN++)
        {
            sPattern = _T("(na ") + note[i++] + _T(" w)");
            pNode = parserLDP.ParseText( sPattern );
            parserLDP.AnalyzeNote(pNode, pVStaff);
        }
    }
    pVStaff->AddSpacer(20);
    pVStaff->AddBarline(lm_eBarlineEnd);

    //return key signature name
    return GetKeySignatureName(nKey);
}
