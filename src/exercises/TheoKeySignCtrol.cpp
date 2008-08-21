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
#pragma implementation "TheoKeySignCtrol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "../score/VStaff.h"
#include "TheoKeySignCtrol.h"
#include "Constrains.h"
#include "Generators.h"
#include "../auxmusic/Conversion.h"

//access to error's logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;



//------------------------------------------------------------------------------------
// Implementation of lmTheoKeySignCtrol


//IDs for controls
enum {
    ID_BUTTON = 3010,
};

BEGIN_EVENT_TABLE(lmTheoKeySignCtrol, lmOneScoreCtrol)
    EVT_COMMAND_RANGE   (ID_BUTTON, ID_BUTTON+m_NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmTheoKeySignCtrol::OnRespButton)
END_EVENT_TABLE()

IMPLEMENT_CLASS(lmTheoKeySignCtrol, lmOneScoreCtrol)

static wxString m_sMajor[15];
static wxString m_sMinor[15];


lmTheoKeySignCtrol::lmTheoKeySignCtrol(wxWindow* parent, wxWindowID id,
                           lmTheoKeySignConstrains* pConstrains,
                           const wxPoint& pos, const wxSize& size, int style)
    : lmOneScoreCtrol(parent, id, pConstrains, wxSize(350, 150), pos, size, style)
{
    //initializations
    m_nRespIndex = 0;
    m_pConstrains = pConstrains;

    //exercise configuration options
    pConstrains->SetPlayLink(false);        //no play link

    CreateControls();
    if (m_pConstrains->IsTheoryMode()) NewProblem();
}

void lmTheoKeySignCtrol::InitializeStrings()
{
    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated

    m_sMajor[0] = _("C major");
    m_sMajor[1] = _("G major");
    m_sMajor[2] = _("D major");
    m_sMajor[3] = _("A major");
    m_sMajor[4] = _("E major");
    m_sMajor[5] = _("B major");
    m_sMajor[6] = _("F # major");
    m_sMajor[7] = _("C # major");
    m_sMajor[8] = _("C b major");
    m_sMajor[9] = _("G b major");
    m_sMajor[10] = _("D b major");
    m_sMajor[11] = _("A b major");
    m_sMajor[12] = _("E b major");
    m_sMajor[13] = _("B b major");
    m_sMajor[14] = _("F major");

    m_sMinor[0] = _("A minor");
    m_sMinor[1] = _("E minor");
    m_sMinor[2] = _("B minor");
    m_sMinor[3] = _("F # minor");
    m_sMinor[4] = _("C # minor");
    m_sMinor[5] = _("G # minor");
    m_sMinor[6] = _("D # minor");
    m_sMinor[7] = _("A # minor");
    m_sMinor[8] = _("A b minor");
    m_sMinor[9] = _("E b minor");
    m_sMinor[10] = _("B b minor");
    m_sMinor[11] = _("F minor");
    m_sMinor[12] = _("C minor");
    m_sMinor[13] = _("G minor");
    m_sMinor[14] = _("D minor");

}

void lmTheoKeySignCtrol::CreateAnswerButtons(int nHeight, int nSpacing, wxFont& font)
{
    //create 15 buttons for the answers: three rows, five buttons per row

    m_pKeyboardSizer = new wxFlexGridSizer(m_NUM_ROWS, m_NUM_COLS, 2*nSpacing, 0);
    m_pMainSizer->Add(
        m_pKeyboardSizer,
        wxSizerFlags(0).Left().Border(wxALIGN_LEFT|wxTOP, 2*nSpacing) );

    int iB = 0;
    for (int iRow=0; iRow < m_NUM_ROWS; iRow++)
    {
        for (int iCol=0; iCol < m_NUM_COLS; iCol++)
        {
            iB = iCol + iRow * m_NUM_COLS;    // button index: 0 .. 24
            m_pAnswerButton[iB] = new wxButton( this, ID_BUTTON + iB, _T(""),
                wxDefaultPosition, wxSize(18*nSpacing, nHeight));
            m_pAnswerButton[iB]->SetFont(font);

            m_pKeyboardSizer->Add(
                m_pAnswerButton[iB],
                wxSizerFlags(0).Border(wxALL, nSpacing) );
        }
    }


    //inform base class about the settings
    SetButtons(m_pAnswerButton, m_NUM_BUTTONS, ID_BUTTON);

}

lmTheoKeySignCtrol::~lmTheoKeySignCtrol()
{
}

wxString lmTheoKeySignCtrol::SetNewProblem()
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


    // choose mode
    lmRandomGenerator oGenerator;
    if (m_pConstrains->GetScaleMode() == eMayorAndMinorModes) {
        m_fMajorMode = oGenerator.FlipCoin();
    }
    else {
        m_fMajorMode = (m_pConstrains->GetScaleMode() == eMajorMode);
    }

    // choose key signature and prepare answer
    bool fFlats = oGenerator.FlipCoin();
    int nAnswer;
    int nAccidentals = oGenerator.RandomNumber(0, m_pConstrains->GetMaxAccidentals());
    lmEKeySignatures nKey;
    if (m_fMajorMode) {
        if (fFlats) {
            // Major mode, flats
            switch(nAccidentals)
            {
                case 0:
                    nKey = earmDo;
                    nAnswer = 0;            // Do Mayor, La menor, no accidentals
                    m_nRespIndex = 1;
                    break;
                case 1:
                    nKey = earmFa;
                    nAnswer = 14;           // Fa Mayor, Re menor"
                    m_nRespIndex = 7;
                    break;
               case 2:
                    nKey = earmSib;
                    nAnswer = 13;   // Si b Mayor, Sol menor"
                    m_nRespIndex = 13;
                    break;
                case 3:
                    nKey = earmMib;
                    nAnswer = 12;   // Mi b Mayor, Do menor"
                    m_nRespIndex = 5;
                    break;
                case 4:
                    nKey = earmLab;
                    nAnswer = 11;   // La b Mayor, Fa menor"
                    m_nRespIndex = 11;
                    break;
                case 5:
                    nKey = earmReb;
                    nAnswer = 10;   // Re b Mayor, Si b menor"
                    m_nRespIndex = 3;
                    break;
                case 6:
                    nKey = earmSolb;
                    nAnswer = 9;   // Sol b Mayor, Mi b menor"
                    m_nRespIndex = 9;
                    break;
                case 7:
                    nKey = earmDob;
                    nAnswer = 8;   // Do b Mayor, La b menor"
                    m_nRespIndex = 0;
                    break;
            }
        } else {
            // Major mode, sharps
            switch(nAccidentals)
            {
                case 0:
                    nKey = earmDo;
                    nAnswer = 0;   // Do Mayor, La menor"
                    m_nRespIndex = 1;
                    break;
                case 1:
                    nKey = earmSol;
                    nAnswer = 1;   // Sol Mayor, Mi menor"
                    m_nRespIndex = 10;
                    break;
                case 2:
                    nKey = earmRe;
                    nAnswer = 2;   // Re Mayor, Si menor"
                    m_nRespIndex = 4;
                    break;
                case 3:
                    nKey = earmLa;
                    nAnswer = 3;   // La Mayor, Fa # menor"
                    m_nRespIndex = 12;
                    break;
                case 4:
                    nKey = earmMi;
                    nAnswer = 4;   // Mi Mayor, Do # menor"
                    m_nRespIndex = 6;
                    break;
                case 5:
                    nKey = earmSi;
                    nAnswer = 5;   // Si Mayor, Sol # menor"
                    m_nRespIndex = 14;
                    break;
                case 6:
                    nKey = earmFas;
                    nAnswer = 6;   // Fa # Mayor, Re # menor"
                    m_nRespIndex = 8;
                    break;
                case 7:
                    nKey = earmDos;
                    nAnswer = 7;   // Do # Mayor, La # menor"
                    m_nRespIndex = 2;
                    break;
            }
        }
    } else {
        if (fFlats) {
            // Minor mode, flats
            switch(nAccidentals)
            {
                case 0:
                    nKey = earmLam;
                    nAnswer = 0;   // Do Mayor, La menor"
                    m_nRespIndex = 11;
                    break;
                case 1:
                    nKey = earmRem;
                    nAnswer = 14;   // Fa Mayor, Re menor"
                    m_nRespIndex = 2;
                    break;
                case 2:
                    nKey = earmSolm;
                    nAnswer = 13;   // Si b Mayor, Sol menor"
                    m_nRespIndex = 8;
                    break;
                case 3:
                    nKey = earmDom;
                    nAnswer = 12;   // Mi b Mayor, Do menor"
                    m_nRespIndex = 0;
                    break;
                case 4:
                    nKey = earmFam;
                    nAnswer = 11;   // La b Mayor, Fa menor"
                    m_nRespIndex = 6;
                    break;
                case 5:
                    nKey = earmSibm;
                    nAnswer = 10;   // Re b Mayor, Si b menor"
                    m_nRespIndex = 13;
                    break;
                case 6:
                    nKey = earmMibm;
                    nAnswer = 9;   // Sol b Mayor, Mi b menor"
                    m_nRespIndex = 4;
                    break;
                case 7:
                    nKey = earmLabm;
                    nAnswer = 8;   // Do b Mayor, La b menor"
                    m_nRespIndex = 10;
                    break;
            }
        } else {
            // Minor mode, sharps
            switch(nAccidentals)
            {
                case 0:
                    nKey = earmLam;
                    nAnswer = 0;   // Do Mayor, La menor"
                    m_nRespIndex = 11;
                    break;
                case 1:
                    nKey = earmMim;
                    nAnswer = 1;   // Sol Mayor, Mi menor"
                    m_nRespIndex = 5;
                    break;
                case 2:
                    nKey = earmSim;
                    nAnswer = 2;   // Re Mayor, Si menor"
                    m_nRespIndex = 14;
                    break;
                case 3:
                    nKey = earmFasm;
                    nAnswer = 3;   // La Mayor, Fa # menor"
                    m_nRespIndex = 7;
                    break;
                case 4:
                    nKey = earmDosm;
                    nAnswer = 4;   // Mi Mayor, Do # menor"
                    m_nRespIndex = 1;
                    break;
                case 5:
                    nKey = earmSolsm;
                    nAnswer = 5;   // Si Mayor, Sol # menor"
                    m_nRespIndex = 9;
                    break;
                case 6:
                    nKey = earmResm;
                    nAnswer = 6;   // Fa # Mayor, Re # menor"
                    m_nRespIndex = 3;
                    break;
                case 7:
                    nKey = earmLasm;
                    nAnswer = 7;   // Do # Mayor, La # menor"
                    m_nRespIndex = 12;
                    break;
            }
        }
    }

    // choose type of problem
    if (m_pConstrains->GetProblemType() == eBothKeySignProblems) {
        m_fIdentifyKey = oGenerator.FlipCoin();
    }
    else {
        m_fIdentifyKey = (m_pConstrains->GetProblemType() == eIdentifyKeySignature);
    }

    g_pLogger->LogTrace(_T("lmTheoKeySignCtrol"),
        _T("[lmTheoKeySignCtrol::NewProblem] m_fIdentifyKey=%s, m_fMajorMode=%s, fFlats=%s, nKey=%d, nAnswer=%d, m_nRespIndex=%d"),
            (m_fIdentifyKey ? _T("yes") : _T("no")),
            (m_fMajorMode ? _T("yes") : _T("no")),
            (fFlats ? _T("yes") : _T("no")),
            nKey, nAnswer, m_nRespIndex);


    // store index to right answer button (for guess-number-of-accidentals problems)
    if (!m_fIdentifyKey) {
        m_nRespIndex = KeySignatureToNumFifths(nKey);
        if (m_nRespIndex < 0) m_nRespIndex = 7 - m_nRespIndex;
    }

    // choose clef
    lmEClefType nClef = oGenerator.GenerateClef(m_pConstrains->GetClefConstrains());

    // write buttons' labels, depending on mode
    if (m_fIdentifyKey) {
        if (m_fMajorMode) {
            m_pAnswerButton[0]->SetLabel(_("C flat major"));
            m_pAnswerButton[1]->SetLabel(_("C major"));
            m_pAnswerButton[2]->SetLabel(_("C sharp major"));
            m_pAnswerButton[3]->SetLabel(_("D flat major"));
            m_pAnswerButton[4]->SetLabel(_("D major"));
            m_pAnswerButton[5]->SetLabel(_("E flat major"));
            m_pAnswerButton[6]->SetLabel(_("E major"));
            m_pAnswerButton[7]->SetLabel(_("F major"));
            m_pAnswerButton[8]->SetLabel(_("F sharp major"));
            m_pAnswerButton[9]->SetLabel(_("G flat major"));
            m_pAnswerButton[10]->SetLabel(_("G major"));
            m_pAnswerButton[11]->SetLabel(_("A flat major"));
            m_pAnswerButton[12]->SetLabel(_("A major"));
            m_pAnswerButton[13]->SetLabel(_("B flat major"));
            m_pAnswerButton[14]->SetLabel(_("B major"));
            //14,0,7,12,2,10,4,8,6,13,1,11,3,9,5
        }
        else {
            m_pAnswerButton[0]->SetLabel(_("C minor"));
            m_pAnswerButton[1]->SetLabel(_("C sharp minor"));
            m_pAnswerButton[2]->SetLabel(_("D minor"));
            m_pAnswerButton[3]->SetLabel(_("D sharp minor"));
            m_pAnswerButton[4]->SetLabel(_("E flat minor"));
            m_pAnswerButton[5]->SetLabel(_("E minor"));
            m_pAnswerButton[6]->SetLabel(_("F minor"));
            m_pAnswerButton[7]->SetLabel(_("F sharp minor"));
            m_pAnswerButton[8]->SetLabel(_("G minor"));
            m_pAnswerButton[9]->SetLabel(_("G sharp minor"));
            m_pAnswerButton[10]->SetLabel(_("A flat minor"));
            m_pAnswerButton[11]->SetLabel(_("A minor"));
            m_pAnswerButton[12]->SetLabel(_("A sharp minor"));
            m_pAnswerButton[13]->SetLabel(_("B flat minor"));
            m_pAnswerButton[14]->SetLabel(_("B minor"));
        }
    }
    else {
        // type of problem: write key
        m_pAnswerButton[0]->SetLabel(_("No accidentals"));
        m_pAnswerButton[1]->SetLabel(_("1 #"));
        m_pAnswerButton[2]->SetLabel(_("2 #"));
        m_pAnswerButton[3]->SetLabel(_("3 #"));
        m_pAnswerButton[4]->SetLabel(_("4 #"));
        m_pAnswerButton[5]->SetLabel(_("5 #"));
        m_pAnswerButton[6]->SetLabel(_("6 #"));
        m_pAnswerButton[7]->SetLabel(_("7 #"));
        m_pAnswerButton[8]->SetLabel(_("1 b"));
        m_pAnswerButton[9]->SetLabel(_("2 b"));
        m_pAnswerButton[10]->SetLabel(_("3 b"));
        m_pAnswerButton[11]->SetLabel(_("4 b"));
        m_pAnswerButton[12]->SetLabel(_("5 b"));
        m_pAnswerButton[13]->SetLabel(_("6 b"));
        m_pAnswerButton[14]->SetLabel(_("7 b"));
    }

    //create the score
    m_pProblemScore = new lmScore();
    lmInstrument* pInstr = m_pProblemScore->AddInstrument(0,0,_T(""));                   //one vstaff, MIDI channel 0, MIDI instr 0
    lmVStaff *pVStaff = pInstr->GetVStaff();
    m_pProblemScore->SetTopSystemDistance( pVStaff->TenthsToLogical(30, 1) );     // 3 lines
    pVStaff->AddClef( nClef );
    pVStaff->AddKeySignature(nKey);
    pVStaff->AddBarline(lm_eBarlineEnd, lmNO_VISIBLE);

    //wxLogMessage(wxString::Format(
    //    _T("[lmTheoKeySignCtrol::NewProblem] m_nRespIndex=%d, oIntv.GetIntervalNum()=%d"),
    //    m_nRespIndex, oIntv.GetIntervalNum() ));

    if (m_fIdentifyKey)
    {
        //direct problem
        m_sAnswer = m_sMajor[nAnswer] + _T(", ") + m_sMinor[nAnswer];
        return _T("");
    }
    else
    {
        //inverse problem
        m_sAnswer = (m_fMajorMode ? m_sMajor[nAnswer] : m_sMinor[nAnswer] );
        m_pSolutionScore = m_pProblemScore;
        m_pProblemScore = (lmScore*)NULL;
        return m_sAnswer;
    }

}

wxDialog* lmTheoKeySignCtrol::GetSettingsDlg()
{
    return (wxDialog*)NULL;
}

