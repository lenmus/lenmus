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
#pragma implementation "TheoIntervalsCtrol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "../score/VStaff.h"
#include "TheoIntervalsCtrol.h"
#include "Constrains.h"
#include "Generators.h"
#include "../auxmusic/Conversion.h"

#include "../ldp_parser/LDPParser.h"
#include "../auxmusic/Interval.h"
#include "dialogs/DlgCfgTheoIntervals.h"

#include "../globals/Colors.h"
extern lmColors* g_pColors;

// access to global external variables
extern bool g_fReleaseVersion;      // in TheApp.cpp
extern bool g_fReleaseBehaviour;    // in TheApp.cpp
extern bool g_fShowDebugLinks;      // in TheApp.cpp
extern bool g_fAutoNewProblem;      // in Preferences.cpp


//Data about intervals to generate for each problem level
static lmFIntval m_aProblemDataL0[] = {
    lm_p1, lm_m2, lm_M2, lm_m3, lm_M3, lm_p4, lm_p5, lm_m6, lm_M6, lm_m7, lm_M7, lm_p8 };
static lmFIntval m_aProblemDataL1[] = {
    lm_p1, lm_m2, lm_M2, lm_m3, lm_M3, lm_p4, lm_p5, lm_m6, lm_M6, lm_m7, lm_M7, lm_p8 };
static lmFIntval m_aProblemDataL2[] = {
    lm_p1, lm_a1, lm_d2, lm_m2, lm_M2, lm_a2, lm_d3, lm_m3, lm_M3, lm_a3, lm_d4, lm_p4, lm_a4,
    lm_d5, lm_p5, lm_a5, lm_d6, lm_m6, lm_M6, lm_a6, lm_d7, lm_m7, lm_M7, lm_a7, lm_d8, lm_p8 };
static lmFIntval m_aProblemDataL3[] = {
    lm_p1, lm_a1, lm_da1, lm_dd2, lm_d2, lm_m2, lm_M2, lm_a2, lm_da2, lm_dd3, lm_d3, lm_m3, lm_M3,
    lm_a3, lm_da3, lm_dd4, lm_d4, lm_p4, lm_a4, lm_da4, lm_dd5, lm_d5, lm_p5, lm_a5, lm_da5, lm_dd6,
    lm_d6, lm_m6, lm_M6, lm_a6, lm_da6, lm_dd7, lm_d7, lm_m7, lm_M7, lm_a7, lm_da7, lm_dd8, lm_d8,
    lm_p8 };

//Questions. Params to generate a question
enum
{
    lmINTVAL_INDEX = 0,
    lmKEY_SIGNATURE,
};

//----------------------------------------------------------------------------------
// Implementation of lmTheoIntervalCtrol
//----------------------------------------------------------------------------------


lmTheoIntervalCtrol::lmTheoIntervalCtrol(wxWindow* parent, wxWindowID id,
                           lmTheoIntervalsConstrains* pConstrains,
                           const wxPoint& pos, const wxSize& size, int style)
    : lmOneScoreCtrol(parent, id, pConstrains, wxSize(380, 150), pos, size, style )
{
    //initializations
    m_pConstrains = pConstrains;
    m_nRespIndex = 0;

    m_pConstrains->SetGenerationModeSupported(lm_eLearningMode, true);
    m_pConstrains->SetGenerationModeSupported(lm_ePractiseMode, true);
    ChangeGenerationMode(lm_eLearningMode);
}

lmTheoIntervalCtrol::~lmTheoIntervalCtrol()
{
}

void lmTheoIntervalCtrol::PrepareAuxScore(int nButton)
{
    // No problem is presented and the user press the button to play a specific
    // sound (chord, interval, scale, etc.)
    // This method is then invoked to prepare the score with the requested sound.
    // At return, base class will play it

    // In theory interval exercises it is not allowed to play an interval so
    // we return a Null score
    m_pAuxScore = (lmScore*)NULL;
}

wxDialog* lmTheoIntervalCtrol::GetSettingsDlg()
{
    // 'Settings' link has been clicked. This method must return the dialog to invoke

    wxDialog* pDlg = new lmDlgCfgTheoIntervals(this, m_pConstrains);
    return pDlg;
}

void lmTheoIntervalCtrol::OnSettingsChanged()
{
    // The settings have been changed.

    //if problem level has changed set up the new problem space
    SetProblemSpace();

    //Reconfigure answer keyboard for the new settings
    ReconfigureKeyboard();
}

void lmTheoIntervalCtrol::SetProblemSpace()
{
    if (m_sKeyPrefix == _T("")) return;     //Ctrol constructor not yet finished

    //save current problem space data
    m_pProblemManager->SaveProblemSpace();

    //For ThoeIntervals exercises, question sets are defined by combination of
    //problem level and key signature, except for level 0 (only interval names).
    //For level 0 there is only one set
    m_nProblemLevel = m_pConstrains->GetProblemLevel();
    if (m_nProblemLevel == 0)
    {
        SetSpaceLevel0();
    }
    else
    {
        //Problem Space: TheoIntervals
        //Question params:
        //  Param0 - Index on  m_aProblemDataLx[] to define interval
        //  Param1 - Key signature
        //  All others not used -> Mandatory params = 2
        m_pProblemManager->NewSpace(m_sKeyPrefix, 3, 2);
        lmKeyConstrains* pKeyConstrains = m_pConstrains->GetKeyConstrains();
        for (int i=0; i < earmFa+1; i++)
        {
            if ( pKeyConstrains->IsValid((lmEKeySignatures)i) )
            {
                wxString sSetName = wxString::Format(_T("Level%d/Key%d"),
                                                     m_nProblemLevel, i);
                //ask problem manager to load this Set.
                if ( !m_pProblemManager->LoadSet(sSetName) )
                {
                    //No questions saved for this set. Create the set
                    CreateQuestionsSet(sSetName, (lmEKeySignatures)i);
                }
            }
        }
    }
    //new space loaded. Inform problem manager
    m_pProblemManager->OnProblemSpaceChanged();

    //update counters
    if (m_pCounters)
        m_pCounters->UpdateDisplay();

    //discard any currently formulated question
    if (m_fQuestionAsked)
        NewProblem();
}

void lmTheoIntervalCtrol::SetSpaceLevel0()
{
    //Problem Space: Initiation to intervals
    //Question params:
    //  Param0 - Index on  m_aProblemDataL0[] to define interval
    //  All others not used -> Mandatory params = 1

    wxString sSpaceName = m_sKeyPrefix + _T("/Level0");
    m_pProblemManager->NewSpace(sSpaceName, 3, 1);
    wxString sSetName = _T("Level0");
    //ask problem manager to load the set.
    if ( !m_pProblemManager->LoadSet(sSetName) )
    {
        //No questions saved for this set. Create the set
        m_pProblemManager->StartNewSet(sSetName);
        for (int i=0; i < 8; i++)
            m_pProblemManager->AddQuestionToSet(i);

        m_pProblemManager->EndOfNewSet();
    }
}

void lmTheoIntervalCtrol::CreateQuestionsSet(wxString& sSetName,
                                             lmEKeySignatures nKey)
{
    wxASSERT(m_nProblemLevel > 0 && m_nProblemLevel < 4);

    int nNumQuestions;
    if (m_nProblemLevel == 1)
        nNumQuestions = sizeof(m_aProblemDataL1)/sizeof(lmFIntval);
    else if (m_nProblemLevel == 2)
        nNumQuestions = sizeof(m_aProblemDataL2)/sizeof(lmFIntval);
    else
        nNumQuestions = sizeof(m_aProblemDataL3)/sizeof(lmFIntval);

    m_pProblemManager->StartNewSet(sSetName);
    for (int i=0; i <nNumQuestions; i++)
        m_pProblemManager->AddQuestionToSet(i, (long)nKey);

    m_pProblemManager->EndOfNewSet();
}

wxString lmTheoIntervalCtrol::SetNewProblem()
{
    // This method must prepare the interval for the problem and set variables:
    // m_iQ, m_fpIntv, m_fpStart, m_fpEnd, m_sAnswer

    //Get parameters controlled by problem space

    //Param0: index to interval number
    m_iQ = m_pProblemManager->ChooseQuestion();
    wxASSERT(m_iQ>= 0 && m_iQ < m_pProblemManager->GetSpaceSize());

    wxASSERT(m_pProblemManager->IsQuestionParamMandatory(lmINTVAL_INDEX));
    long nIntvNdx = m_pProblemManager->GetQuestionParam(m_iQ, lmINTVAL_INDEX);
    if (m_nProblemLevel <= 1)
        m_fpIntv = m_aProblemDataL1[nIntvNdx];
    else if (m_nProblemLevel == 2)
        m_fpIntv = m_aProblemDataL2[nIntvNdx];
    else
        m_fpIntv = m_aProblemDataL3[nIntvNdx];

    int nIntvNum = FIntval_GetNumber(m_fpIntv);           //get interval number

    //Param1: key signature
    lmRandomGenerator oGenerator;
    if (m_pProblemManager->IsQuestionParamMandatory(lmKEY_SIGNATURE))
        m_nKey = (lmEKeySignatures)m_pProblemManager->GetQuestionParam(m_iQ, lmKEY_SIGNATURE);
    else
        m_nKey = oGenerator.GenerateKey(m_pConstrains->GetKeyConstrains());


    //Get other parameters: selectable by the user

    int nMinPos = 2 - (2 * m_pConstrains->GetLedgerLinesBelow());
    int nMaxPos = 10 + (2 * m_pConstrains->GetLedgerLinesAbove());
    nMaxPos -= nIntvNum - 1;

    //Generate start note and end note
    bool fValid = false;
    m_nClef = oGenerator.GenerateClef(m_pConstrains->GetClefConstrains());
    while (!fValid)
    {
        lmDPitch dpStart = oGenerator.GenerateRandomDPitch(nMinPos, nMaxPos, false, m_nClef);
        m_fpStart = DPitch_ToFPitch(dpStart, m_nKey);
        m_fpEnd = m_fpStart + m_fpIntv;
        fValid = FPitch_IsValid(m_fpEnd);
        if (!fValid)
            wxLogMessage(_T("[lmTheoIntervalCtrol::SetNewProblem] INVALID: m_iQ=%d, nIntvNdx=%d, m_fpIntv=%d, m_fpStart=%d, m_fpEnd=%d"),
                 m_iQ, nIntvNdx, m_fpIntv, m_fpStart, m_fpEnd);
    }

    //compute the interval name
    if (m_fpIntv == 0)
        m_sAnswer = _("Unison");
    else if (m_fpIntv == 1)
        m_sAnswer = _("Chromatic semitone");
    else if (m_fpIntv == 2)
        m_sAnswer = _("Chromatic tone");
    else
        m_sAnswer = FIntval_GetName(m_fpIntv);

    if (m_fpIntv > 0)
        m_sAnswer += (m_fpEnd > m_fpStart ? _(", ascending") : _(", descending") );

    wxLogMessage(_T("[lmTheoIntervalCtrol::SetNewProblem] m_iQ=%d, nIntvNdx=%d, m_fpIntv=%s (%d), m_fpStart=%s (%d), m_fpEnd=%s (%d), sAnswer=%s"),
                 m_iQ, nIntvNdx, FIntval_GetIntvCode(m_fpIntv).c_str(), m_fpIntv,
                 FPitch_ToAbsLDPName(m_fpStart).c_str(), m_fpStart,
                 FPitch_ToAbsLDPName(m_fpEnd).c_str(), m_fpEnd, m_sAnswer.c_str());

    return PrepareScores();
}



//---------------------------------------------------------------------------------
// Implementation of lmBuildIntervalCtrol
//---------------------------------------------------------------------------------


//type of keyboard currently displayed
enum {
    eKeyboardNone = 0,
    eKeyboardIntv,          //identify interval
    eKeyboardNotes,         //build interval
};

//internationalized strings
static wxString m_sNotesButtonLabel[35];
static wxString m_sNotesRowLabel[lmBuildIntervalCtrol::lm_NUM_ROWS];
static wxString m_sNotesColumnLabel[lmBuildIntervalCtrol::lm_NUM_COLS];

//IDs for controls
enum {
    ID_BUTTON = 3010,
};


BEGIN_EVENT_TABLE(lmBuildIntervalCtrol, lmTheoIntervalCtrol)
    EVT_COMMAND_RANGE (ID_BUTTON, ID_BUTTON+lm_NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmBuildIntervalCtrol::OnRespButton)
END_EVENT_TABLE()

lmBuildIntervalCtrol::lmBuildIntervalCtrol(wxWindow* parent, wxWindowID id,
                           lmTheoIntervalsConstrains* pConstrains,
                           const wxPoint& pos, const wxSize& size, int style)
    : lmTheoIntervalCtrol(parent, id, pConstrains, pos, size, style )
{
    //set key
    m_sKeyPrefix = wxString::Format(_T("/BuildIntval/%s/"),
                                    m_pConstrains->GetSection().c_str() );
    //create controls
    CreateControls();

    //update display
    if (m_pCounters)
        m_pCounters->UpdateDisplay();

    if (m_pConstrains->IsTheoryMode()) NewProblem();
}

lmBuildIntervalCtrol::~lmBuildIntervalCtrol()
{
}

void lmBuildIntervalCtrol::CreateAnswerButtons(int nHeight, int nSpacing, wxFont& font)
{
    //
    //create 35 buttons for the answers: five rows, seven buttons per row
    //

    int iB;
    for (iB=0; iB < lm_NUM_BUTTONS; iB++) {
        m_pAnswerButton[iB] = (wxButton*)NULL;
    }

    m_pKeyboardSizer = new wxFlexGridSizer(lm_NUM_ROWS+1, lm_NUM_COLS+1, 0, 0);
    m_pMainSizer->Add(
        m_pKeyboardSizer,
        wxSizerFlags(0).Left().Border(wxALIGN_LEFT|wxTOP, 2*nSpacing)  );

    //row with column labels
    m_pKeyboardSizer->Add(nSpacing, nSpacing, 0);               //spacer for labels column
    for (int iCol=0; iCol < lm_NUM_COLS; iCol++)
    {
        m_pColumnLabel[iCol] = new wxStaticText(this, -1, m_sNotesColumnLabel[iCol]);
        m_pColumnLabel[iCol]->SetFont(font);
        m_pKeyboardSizer->Add(
            m_pColumnLabel[iCol], 0,
            wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE,
            nSpacing);
    }

    //remaining rows with buttons
    for (int iRow=0; iRow < lm_NUM_ROWS; iRow++)
    {
        m_pRowLabel[iRow] = new wxStaticText(this, -1, m_sNotesRowLabel[iRow]);
        m_pRowLabel[iRow]->SetFont(font);
        m_pKeyboardSizer->Add(
            m_pRowLabel[iRow],
            wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, nSpacing) );

        // the buttons for this row
        for (int iCol=0; iCol < lm_NUM_COLS; iCol++) {
            iB = iCol + iRow * lm_NUM_COLS;    // button index: 0 .. 34
            m_pAnswerButton[iB] = new wxButton( this, ID_BUTTON + iB, m_sNotesButtonLabel[iB],
                wxDefaultPosition, wxSize(11*nSpacing, nHeight));
            m_pAnswerButton[iB]->SetFont(font);

            m_pKeyboardSizer->Add(
                m_pAnswerButton[iB],
                wxSizerFlags(0).Border(wxLEFT|wxRIGHT, nSpacing) );
        }
    }

    //inform base class about the settings
    SetButtons(m_pAnswerButton, lm_NUM_BUTTONS, ID_BUTTON);
}

wxString lmBuildIntervalCtrol::PrepareScores()
{
    //The problem interval has been set.
    //This method must prepare the problem score and set variables:
    //  m_pProblemScore - The score with the problem to propose
    //  m_pSolutionScore - The score with the solution or NULL if it is the
    //              same score than the problem score.
    //  m_sAnswer - the message to present when displaying the solution
    //  m_nRespIndex - the number of the button for the right answer
    //  m_nPlayMM - the speed to play the score
    //
    //It must return the message to display to introduce the problem.

    //prepare LDP pattern
    wxString sPattern0 = _T("(n ");
    sPattern0 += FPitch_ToRelLDPName(m_fpStart, m_nKey);
    sPattern0 += _T(" w)");

    wxString sPattern1 = _T("(n ");
    sPattern1 += FPitch_ToRelLDPName(m_fpEnd, m_nKey);
    sPattern1 += _T(" w)");

    //prepare solution score
    lmNote* pNote[2];
    lmLDPParser parserLDP(_T("en"), _T("utf-8"));
    lmLDPNode* pNode;
    lmVStaff* pVStaff;

    lmScore* pScore = new lmScore();
    pScore->SetOption(_T("Render.SpacingMethod"), (long)esm_Fixed);
    lmInstrument* pInstr = pScore->AddInstrument(0,0,_T(""));		//MIDI channel 0, MIDI instr 0
    pVStaff = pInstr->GetVStaff();
    pScore->SetTopSystemDistance( pVStaff->TenthsToLogical(30, 1) );     // 3 lines
    pVStaff->AddClef( m_nClef );
    pVStaff->AddKeySignature(m_nKey);
    pVStaff->AddTimeSignature(4 ,4, lmNO_VISIBLE );
    pVStaff->AddSpacer(30);       // 3 lines
    pNode = parserLDP.ParseText( sPattern0 );
    pNote[0] = parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddBarline(lm_eBarlineSimple, lmNO_VISIBLE);    //so that accidental doesn't affect 2nd note
    pNode = parserLDP.ParseText( sPattern1 );
    pNote[1] = parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddSpacer(50);       // 5 lines
    pVStaff->AddBarline(lm_eBarlineEnd, lmNO_VISIBLE);

    //for building intervals exercise the created score is the solution and
    //we need to create another score with the problem
    m_pSolutionScore = pScore;
    m_pProblemScore = new lmScore();
    pInstr = m_pProblemScore->AddInstrument(0,0,_T(""));		//MIDI channel 0, MIDI instr 0
    pVStaff = pInstr->GetVStaff();
    m_pProblemScore->SetTopSystemDistance( pVStaff->TenthsToLogical(30, 1) );     // 3 lines
    pVStaff->AddClef( m_nClef );
    pVStaff->AddKeySignature(m_nKey);
    pVStaff->AddTimeSignature(4 ,4, lmNO_VISIBLE );
    pVStaff->AddSpacer(30);       // 3 lines
    pNode = parserLDP.ParseText( sPattern0 );
    pNote[0] = parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddSpacer(75);       // 7.5 lines
    pVStaff->AddBarline(lm_eBarlineEnd, lmNO_VISIBLE);

    //cumpute right answer button index
    int iCol = FPitch_Step(m_fpEnd);
    int iRow = FPitch_Accidentals(m_fpEnd) + 2;
    m_nRespIndex = iCol + iRow * lm_NUM_COLS;

    //return question string
    m_sAnswer = _("Build a ") + m_sAnswer;
    return m_sAnswer;
}

void lmBuildIntervalCtrol::InitializeStrings()
{
    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated

        // button row labels
    m_sNotesRowLabel[0] = _("Double flat");
    m_sNotesRowLabel[1] = _("Flat");
    m_sNotesRowLabel[2] = _("Natural");
    m_sNotesRowLabel[3] = _("Sharp");
    m_sNotesRowLabel[4] = _("Double sharp");

        // button column labels
    m_sNotesColumnLabel[0] = _("C");
    m_sNotesColumnLabel[1] = _("D");
    m_sNotesColumnLabel[2] = _("E");
    m_sNotesColumnLabel[3] = _("F");
    m_sNotesColumnLabel[4] = _("G");
    m_sNotesColumnLabel[5] = _("A");
    m_sNotesColumnLabel[6] = _("B");

        //button labels (for notes)
    m_sNotesButtonLabel[0] = _("bb C");
    m_sNotesButtonLabel[1] = _("bb D");
    m_sNotesButtonLabel[2] = _("bb E");
    m_sNotesButtonLabel[3] = _("bb F");;
    m_sNotesButtonLabel[4] = _("bb G");
    m_sNotesButtonLabel[5] = _("bb A");
    m_sNotesButtonLabel[6] = _("bb B");

    m_sNotesButtonLabel[7] = _("b C");
    m_sNotesButtonLabel[8] = _("b D");
    m_sNotesButtonLabel[9] = _("b E");
    m_sNotesButtonLabel[10] = _("b F");
    m_sNotesButtonLabel[11] = _("b G");
    m_sNotesButtonLabel[12] = _("b A");
    m_sNotesButtonLabel[13] = _("b B");

    m_sNotesButtonLabel[14] = _("C");
    m_sNotesButtonLabel[15] = _("D");
    m_sNotesButtonLabel[16] = _("E");
    m_sNotesButtonLabel[17] = _("F");
    m_sNotesButtonLabel[18] = _("G");
    m_sNotesButtonLabel[19] = _("A");
    m_sNotesButtonLabel[20] = _("B");

    m_sNotesButtonLabel[21] = _("# C");
    m_sNotesButtonLabel[22] = _("# D");
    m_sNotesButtonLabel[23] = _("# E");
    m_sNotesButtonLabel[24] = _("# F");
    m_sNotesButtonLabel[25] = _("# G");
    m_sNotesButtonLabel[26] = _("# A");
    m_sNotesButtonLabel[27] = _("# B");

    m_sNotesButtonLabel[28] = _("x C");
    m_sNotesButtonLabel[29] = _("x D");
    m_sNotesButtonLabel[30] = _("x E");
    m_sNotesButtonLabel[31] = _("x F");
    m_sNotesButtonLabel[32] = _("x G");
    m_sNotesButtonLabel[33] = _("x A");
    m_sNotesButtonLabel[34] = _("x B");
}




//----------------------------------------------------------------------------------
// Implementation of lmIdfyIntervalCtrol
//----------------------------------------------------------------------------------


//internationalized strings
static wxString m_sIntvButtonLabel[lmIdfyIntervalCtrol::lm_NUM_BUTTONS];
static wxString m_sIntvRowLabel[lmIdfyIntervalCtrol::lm_NUM_ROWS];
static wxString m_sIntvColumnLabel[lmIdfyIntervalCtrol::lm_NUM_COLS];
static wxString m_sIntvNumber[8];


//index for special buttons
#define lmIDX_UNISON        48
#define lmIDX_SEMITONE      49
#define lmIDX_TONE          50


BEGIN_EVENT_TABLE(lmIdfyIntervalCtrol, lmTheoIntervalCtrol)
    EVT_COMMAND_RANGE (ID_BUTTON, ID_BUTTON+lm_NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmIdfyIntervalCtrol::OnRespButton)
END_EVENT_TABLE()

lmIdfyIntervalCtrol::lmIdfyIntervalCtrol(wxWindow* parent, wxWindowID id,
                           lmTheoIntervalsConstrains* pConstrains,
                           const wxPoint& pos, const wxSize& size, int style)
    : lmTheoIntervalCtrol(parent, id, pConstrains, pos, size, style)
{
    //set key
    m_sKeyPrefix = wxString::Format(_T("/IdfyIntval/%s/"),
                                    m_pConstrains->GetSection().c_str() );
    //create controls
    CreateControls();

    //update display
    if (m_pCounters)
        m_pCounters->UpdateDisplay();

    if (m_pConstrains->IsTheoryMode()) NewProblem();
}

lmIdfyIntervalCtrol::~lmIdfyIntervalCtrol()
{
}

void lmIdfyIntervalCtrol::CreateAnswerButtons(int nHeight, int nSpacing, wxFont& font)
{
    //create 48 buttons for the answers: six rows, eight buttons per row,
    //plus two additional buttons, for 'unison' and 'chromatic semitone'

    int iB;
    for (iB=0; iB < lm_NUM_BUTTONS; iB++) {
        m_pAnswerButton[iB] = (wxButton*)NULL;
    }

    //row with buttons for unison and related
    m_pUnisonSizer = new wxBoxSizer( wxHORIZONTAL );
    m_pMainSizer->Add(
        m_pUnisonSizer,
        wxSizerFlags(0).Left().Border(0));  //wxTOP, 2*nSpacing)  );

        //spacer to skip the labels
    m_pUnisonSizer->Add(28*nSpacing, 4*nSpacing, 0);

        //unison button
    iB = lmIDX_UNISON;
    m_pAnswerButton[iB] = new wxButton( this, ID_BUTTON + iB, m_sIntvButtonLabel[iB] );
    m_pAnswerButton[iB]->SetFont(font);
    m_pUnisonSizer->Add(
        m_pAnswerButton[iB],
        wxSizerFlags(0).Border(wxLEFT|wxRIGHT, nSpacing) );

        // "chromatic semitone" button
    iB = lmIDX_SEMITONE;
    m_pAnswerButton[iB] = new wxButton( this, ID_BUTTON + iB, m_sIntvButtonLabel[iB] );
    m_pAnswerButton[iB]->SetFont(font);
    m_pUnisonSizer->Add(
        m_pAnswerButton[iB],
        wxSizerFlags(0).Border(wxLEFT|wxRIGHT, nSpacing) );

        // "chromatic tone" button
    iB = lmIDX_TONE;
    m_pAnswerButton[iB] = new wxButton( this, ID_BUTTON + iB, m_sIntvButtonLabel[iB] );
    m_pAnswerButton[iB]->SetFont(font);
    m_pUnisonSizer->Add(
        m_pAnswerButton[iB],
        wxSizerFlags(0).Border(wxLEFT|wxRIGHT, nSpacing) );



    //Now main keyboard with all other buttons

    m_pKeyboardSizer = new wxFlexGridSizer(lm_NUM_ROWS+1, lm_NUM_COLS+1, 0, 0);
    m_pMainSizer->Add(
        m_pKeyboardSizer,
        wxSizerFlags(0).Left().Border(wxALIGN_LEFT|wxTOP, 2*nSpacing)  );

    //row with column labels
    m_pKeyboardSizer->Add(nSpacing, nSpacing, 0);               //spacer for labels column
    for (int iCol=0; iCol < lm_NUM_COLS; iCol++)
    {
        m_pColumnLabel[iCol] = new wxStaticText(this, -1, m_sIntvColumnLabel[iCol]);
        m_pColumnLabel[iCol]->SetFont(font);
        m_pKeyboardSizer->Add(
            m_pColumnLabel[iCol], 0,
            wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE,
            nSpacing);
    }

    //remaining rows with buttons
    for (int iRow=0; iRow < lm_NUM_ROWS; iRow++)
    {
        m_pRowLabel[iRow] = new wxStaticText(this, -1, m_sIntvRowLabel[iRow]);
        m_pRowLabel[iRow]->SetFont(font);
        m_pKeyboardSizer->Add(
            m_pRowLabel[iRow],
            wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, nSpacing) );

        // the buttons for this row
        for (int iCol=0; iCol < lm_NUM_COLS; iCol++) {
            iB = iCol + iRow * lm_NUM_COLS;    // button index: 0 .. 47
            m_pAnswerButton[iB] = new wxButton( this, ID_BUTTON + iB, m_sIntvButtonLabel[iB],
                wxDefaultPosition, wxSize(11*nSpacing, nHeight));
            m_pAnswerButton[iB]->SetFont(font);

            m_pKeyboardSizer->Add(
                m_pAnswerButton[iB],
                wxSizerFlags(0).Border(wxLEFT|wxRIGHT, nSpacing) );
            if (m_sIntvButtonLabel[iB].IsEmpty())
            {
                m_pAnswerButton[iB]->Show(false);
                m_pAnswerButton[iB]->Enable(false);
            }
        }
    }


    //inform base class about the settings
    SetButtons(m_pAnswerButton, lm_NUM_BUTTONS, ID_BUTTON);

}

void lmIdfyIntervalCtrol::EnableButtons(bool fEnable)
{
    if (m_pConstrains->GetProblemLevel() == 0)
    {
        for (int iB=0; iB < 7; iB++)
            m_pAnswerButton[iB]->Enable(fEnable);
        m_pAnswerButton[44]->Enable(fEnable);
    }
    else
    {
        for (int iB=0; iB < lm_NUM_BUTTONS; iB++) {
            if (!m_sIntvButtonLabel[iB].IsEmpty())
                m_pAnswerButton[iB]->Enable(fEnable);
        }
    }
}

wxString lmIdfyIntervalCtrol::PrepareScores()
{
    //The problem interval has been set.
    //This method must prepare the problem score and set variables:
    //  m_pProblemScore - The score with the problem to propose
    //  m_pSolutionScore - The score with the solution or NULL if it is the
    //              same score than the problem score.
    //  m_sAnswer - the message to present when displaying the solution
    //  m_nRespIndex - the number of the button for the right answer
    //  m_nPlayMM - the speed to play the score
    //
    //It must return the message to display to introduce the problem.

    //prepare LDP pattern
    wxString sPattern0 = _T("(n ");
    sPattern0 += FPitch_ToRelLDPName(m_fpStart, m_nKey);
    sPattern0 += _T(" w)");

    wxString sPattern1 = _T("(n ");
    sPattern1 += FPitch_ToRelLDPName(m_fpEnd, m_nKey);
    sPattern1 += _T(" w)");

    //create the score with the interval
    lmNote* pNote[2];
    lmLDPParser parserLDP(_T("en"), _T("utf-8"));
    lmLDPNode* pNode;
    lmVStaff* pVStaff;

    lmScore* pScore = new lmScore();
    pScore->SetOption(_T("Render.SpacingMethod"), (long)esm_Fixed);
    lmInstrument* pInstr = pScore->AddInstrument(0,0,_T(""));		//MIDI channel 0, MIDI instr 0
    pVStaff = pInstr->GetVStaff();
    pScore->SetTopSystemDistance( pVStaff->TenthsToLogical(30, 1) );     // 3 lines
    pVStaff->AddClef( m_nClef );
    pVStaff->AddKeySignature(m_nKey);
    pVStaff->AddTimeSignature(4 ,4, lmNO_VISIBLE );
    pVStaff->AddSpacer(30);       // 3 lines
    pNode = parserLDP.ParseText( sPattern0 );
    pNote[0] = parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddBarline(lm_eBarlineSimple, lmNO_VISIBLE);    //so that accidental doesn't affect 2nd note
    pNode = parserLDP.ParseText( sPattern1 );
    pNote[1] = parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddSpacer(75);       // 7.5 lines
    pVStaff->AddBarline(lm_eBarlineEnd, lmNO_VISIBLE);

    //compute button index for right answer
    if (m_fpIntv == 0)
        m_nRespIndex = lmIDX_UNISON;
    else if (m_fpIntv == 1)
        m_nRespIndex = lmIDX_SEMITONE;
    else if (m_fpIntv == 2)
        m_nRespIndex = lmIDX_TONE;
    else
    {
        int iRow, iCol;
        iCol = FIntval_GetNumber(m_fpIntv) - 2;
        switch (FIntval_GetType(m_fpIntv)) {
            case eti_DoubleDiminished:      iRow = 0;   break;
            case eti_Diminished:            iRow = 1;   break;
            case eti_Minor:                 iRow = 2;   break;
            case eti_Major:                 iRow = 3;   break;
            case eti_Perfect:               iRow = 3;   break;
            case eti_Augmented:             iRow = 4;   break;
            case eti_DoubleAugmented:       iRow = 5;   break;
            default:
                wxASSERT(false);
        }
        m_nRespIndex = iCol + (iRow-m_nFirstRow) * lm_NUM_COLS;
    }
    //fix button index for level 0 (only numbers)
    if (m_pConstrains->GetProblemLevel() == 0)
        m_nRespIndex = FIntval_GetNumber(m_fpIntv) - 1;

    //set score with the problem
    m_pProblemScore = pScore;
    m_pSolutionScore = (lmScore*)NULL;

    //return question string
    return _("Identify the next interval:");
}

void lmIdfyIntervalCtrol::ReconfigureKeyboard()
{
    // Reconfigure answer keyboard for the new settings

    if (m_pConstrains->GetProblemLevel() == 0)
    {
        //Level 0: answer buttons only to name interval number, no qualification

        //hide all column labels
        for (int iCol=0; iCol < lm_NUM_COLS; iCol++)
        {
            m_pColumnLabel[iCol]->Show(false);
        }

        //show first eight buttons and change their labels
        int iB;
        for (iB=0; iB < 8; iB++)
        {
            m_pAnswerButton[iB]->SetLabel( m_sIntvNumber[iB] );
            m_pAnswerButton[iB]->Show(true);
            m_pAnswerButton[iB]->Enable(true);
        }
        //hide all buttons remaining buttons
        for (; iB < lm_NUM_BUTTONS; iB++)
        {
            m_pAnswerButton[iB]->Show(false);
            m_pAnswerButton[iB]->Enable(false);
        }

        //hide row labels
        for (int iRow=0; iRow < lm_NUM_ROWS; iRow++)
        {
            m_pRowLabel[iRow]->Show(false);
        }
    }

    else
    {
        //show all column labels
        for (int iCol=0; iCol < lm_NUM_COLS; iCol++)
        {
            m_pColumnLabel[iCol]->Show(true);
        }

        //determine rows to show
        m_nFirstRow = 0;
        int nLastRow = lm_NUM_ROWS;
        bool fUnison = true;
        bool fSemitone = true;
        bool fTone = true;
        if (m_pConstrains->GetProblemLevel() == 1)
        {
            //Only minor and perfect/major
            m_nFirstRow = 2;
            nLastRow = 4;
            fUnison = true;
            fSemitone = false;
            fTone = false;
        }
        else if (m_pConstrains->GetProblemLevel() == 2)
        {
            //also augmented and diminished
            m_nFirstRow = 1;
            nLastRow = 5;
            fUnison = true;
            fSemitone = true;
            fTone = false;
        }
        else
        {
            //all columns
            m_nFirstRow = 0;
            nLastRow = 6;
            fUnison = true;
            fSemitone = true;
            fTone = true;
        }

        //show all buttons and change their labels
        int nShift = m_nFirstRow * lm_NUM_COLS;
        int iLast = (nLastRow-m_nFirstRow) * lm_NUM_COLS;
        int iB;
        for (iB=0; iB < iLast; iB++)
        {
            int iLBL = iB + nShift;
            m_pAnswerButton[iB]->SetLabel( m_sIntvButtonLabel[iLBL] );
            bool fEnable = !m_sIntvButtonLabel[iLBL].IsEmpty();
            m_pAnswerButton[iB]->Show(fEnable);
            m_pAnswerButton[iB]->Enable(fEnable);
        }
        for (; iB < lm_NUM_BUTTONS-2; iB++)
        {
            m_pAnswerButton[iB]->Show(false);
            m_pAnswerButton[iB]->Enable(false);
        }
        m_pAnswerButton[lmIDX_UNISON]->Show(fUnison);
        m_pAnswerButton[lmIDX_UNISON]->Enable(fUnison);
        m_pAnswerButton[lmIDX_SEMITONE]->Show(fSemitone);
        m_pAnswerButton[lmIDX_SEMITONE]->Enable(fSemitone);
        m_pAnswerButton[lmIDX_TONE]->Show(fTone);
        m_pAnswerButton[lmIDX_TONE]->Enable(fTone);

        //show row labels
        int iRow, iLBL;
        for (iRow=0, iLBL = m_nFirstRow; iLBL < nLastRow; iRow++, iLBL++)
        {
            m_pRowLabel[iRow]->SetLabel( m_sIntvRowLabel[iLBL] );
            m_pRowLabel[iRow]->Show(true);
        }
        for (; iRow < lm_NUM_ROWS; iRow++)
        {
            m_pRowLabel[iRow]->Show(false);
        }
    }
    m_pUnisonSizer->Layout();
    m_pKeyboardSizer->Layout();
}

void lmIdfyIntervalCtrol::InitializeStrings()
{
    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated

        // button row labels
    m_sIntvRowLabel[0] = _("dd - double diminished");
    m_sIntvRowLabel[1] = _("dim - diminished");
    m_sIntvRowLabel[2] = _("m - minor");
    m_sIntvRowLabel[3] = _("M, P - Major, Perfect");
    m_sIntvRowLabel[4] = _("aug - augmented");
    m_sIntvRowLabel[5] = _("da - double augmented");

        // button column labels
    m_sIntvColumnLabel[0] = _("2nd");
    m_sIntvColumnLabel[1] = _("3rd");
    m_sIntvColumnLabel[2] = _("4th");
    m_sIntvColumnLabel[3] = _("5th");
    m_sIntvColumnLabel[4] = _("6th");
    m_sIntvColumnLabel[5] = _("7th");
    m_sIntvColumnLabel[6] = _("8ve");
    m_sIntvColumnLabel[7] = _T("");

        //button labels (intervals)
    m_sIntvButtonLabel[0] = _("dd2");
    m_sIntvButtonLabel[1] = _("dd3");
    m_sIntvButtonLabel[2] = _("dd4");
    m_sIntvButtonLabel[3] = _("dd5");
    m_sIntvButtonLabel[4] = _("dd6");
    m_sIntvButtonLabel[5] = _("dd7");
    m_sIntvButtonLabel[6] = _("dd8");
    m_sIntvButtonLabel[7] = _T("");

    m_sIntvButtonLabel[8]  = _("dim2");
    m_sIntvButtonLabel[9]  = _("dim3");
    m_sIntvButtonLabel[10] = _("dim4");
    m_sIntvButtonLabel[11] = _("dim5");
    m_sIntvButtonLabel[12] = _("dim6");
    m_sIntvButtonLabel[13] = _("dim7");
    m_sIntvButtonLabel[14] = _("dim8");
    m_sIntvButtonLabel[15] = _T("");

    m_sIntvButtonLabel[16] = _("m2");
    m_sIntvButtonLabel[17] = _("m3");
    m_sIntvButtonLabel[18] = _T("");
    m_sIntvButtonLabel[19] = _T("");
    m_sIntvButtonLabel[20] = _("m6");
    m_sIntvButtonLabel[21] = _("m7");
    m_sIntvButtonLabel[22] = _T("");
    m_sIntvButtonLabel[23] = _T("");

    m_sIntvButtonLabel[24] = _("M2");
    m_sIntvButtonLabel[25] = _("M3");
    m_sIntvButtonLabel[26] = _("P4");
    m_sIntvButtonLabel[27] = _("P5");
    m_sIntvButtonLabel[28] = _("M6");
    m_sIntvButtonLabel[29] = _("M7");
    m_sIntvButtonLabel[30] = _("P8");
    m_sIntvButtonLabel[31] = _T("");

    m_sIntvButtonLabel[32] = _("aug2");
    m_sIntvButtonLabel[33] = _("aug3");
    m_sIntvButtonLabel[34] = _("aug4");
    m_sIntvButtonLabel[35] = _("aug5");
    m_sIntvButtonLabel[36] = _("aug6");
    m_sIntvButtonLabel[37] = _("aug7");
    m_sIntvButtonLabel[38] = _("aug8");
    m_sIntvButtonLabel[39] = _T("");

    m_sIntvButtonLabel[40] = _("da2");
    m_sIntvButtonLabel[41] = _("da3");
    m_sIntvButtonLabel[42] = _("da4");
    m_sIntvButtonLabel[43] = _("da5");
    m_sIntvButtonLabel[44] = _("da6");
    m_sIntvButtonLabel[45] = _("da7");
    m_sIntvButtonLabel[46] = _("da7");
    m_sIntvButtonLabel[47] = _T("");

    m_sIntvButtonLabel[48] = _("Unison");
    m_sIntvButtonLabel[49] = _("Chromatic semitone");
    m_sIntvButtonLabel[50] = _("Chromatic tone");

    //Buttons for interval number
    m_sIntvNumber[0] = _("Unison");
    m_sIntvNumber[1] = _("2nd");
    m_sIntvNumber[2] = _("3rd");
    m_sIntvNumber[3] = _("4th");
    m_sIntvNumber[4] = _("5th");
    m_sIntvNumber[5] = _("6th");
    m_sIntvNumber[6] = _("7th");
    m_sIntvNumber[7] = _("8ve");
}

