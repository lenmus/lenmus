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
extern bool g_fReleaseVersion;            // in TheApp.cpp
extern bool g_fReleaseBehaviour;        // in TheApp.cpp
extern bool g_fShowDebugLinks;            // in TheApp.cpp
extern bool g_fAutoNewProblem;          // in Preferences.cpp


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


BEGIN_EVENT_TABLE(lmBuildIntervalCtrol, lmOneScoreCtrol)
    EVT_COMMAND_RANGE (ID_BUTTON, ID_BUTTON+lm_NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmBuildIntervalCtrol::OnRespButton)
END_EVENT_TABLE()

lmBuildIntervalCtrol::lmBuildIntervalCtrol(wxWindow* parent, wxWindowID id,
                           lmTheoIntervalsConstrains* pConstrains,
                           const wxPoint& pos, const wxSize& size, int style)
    : lmOneScoreCtrol(parent, id, pConstrains, wxSize(380, 150), pos, size, style )
{
    //initializations
    m_pConstrains = pConstrains;
    m_nRespIndex = 0;

    CreateControls();
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

void lmBuildIntervalCtrol::EnableButtons(bool fEnable)
{
    for (int iB=0; iB < lm_NUM_BUTTONS; iB++) {
        m_pAnswerButton[iB]->Enable(fEnable);
    }
}

wxString lmBuildIntervalCtrol::SetNewProblem()
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

    //Generate two random note-pos in range -1 to 7 (from two ledge lines down to two up)
    lmRandomGenerator oGenerator;
    m_nClef = oGenerator.GenerateClef(m_pConstrains->GetClefConstrains());
    m_DPitch[0] = oGenerator.GenerateRandomDPitch(0, 8, false, m_nClef);
    m_DPitch[1] = oGenerator.GenerateRandomDPitch(0, 8, false, m_nClef);

    //Decide accidentals
    wxString sPattern[2];
    wxString sAlter[2];
    lmConverter oConv;
    for (int i=0; i < 2; i++) {
        sAlter[i] = _T("");
        if (m_pConstrains->GetAccidentals() && oGenerator.FlipCoin() ) {
            sAlter[i] = (oGenerator.FlipCoin() ? _T("-") : _T("+"));
        }
    }

    //amendments for unisons
    if (m_DPitch[0] == m_DPitch[1]) {
        //remove two accidentals in unison
        if (sAlter[0] != _T("") && sAlter[1] != _T("")) {
            sAlter[1] = _T("");
        }
        //add natural sign in second accidental for 'chromatic semitone'
        if (sAlter[0] != _T("") && sAlter[1] == _T("")) {
            sAlter[1] = _T("=");
        }
    }

    //prepare LDP pattern
    for (int i=0; i < 2; i++) {
        sPattern[i] = _T("(n ");
        sPattern[i] += sAlter[i];
        sPattern[i] += DPitch_GetEnglishNoteName(m_DPitch[i]) + _T(" w)");
    }

    //save information to identify answer button
    wxString sNoteName = (DPitch_GetEnglishNoteName(m_DPitch[1])).Left(1);

    ////DEBUG: un-comment and modify values for testing a certain interval
    //sPattern[0] = _T("(n -e4 r)");
    //sPattern[1] = _T("(n e4 r)");

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
    pVStaff->AddKeySignature(0, true);                    // 0 fifths, major  ==> earmDo
    pVStaff->AddTimeSignature(4 ,4, lmNO_VISIBLE );
    pVStaff->AddSpacer(30);       // 3 lines
    pNode = parserLDP.ParseText( sPattern[0] );
    pNote[0] = parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddBarline(lm_eBarlineSimple, lmNO_VISIBLE);    //so that accidental doesn't affect 2nd note
    pNode = parserLDP.ParseText( sPattern[1] );
    pNote[1] = parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddSpacer(75);       // 7.5 lines
    pVStaff->AddBarline(lm_eBarlineEnd, lmNO_VISIBLE);

    //compute the interval name
    lmInterval oIntv(pNote[0], pNote[1], earmDo);
    m_sAnswer = oIntv.GetIntervalName() + (oIntv.IsAscending() ? _(", ascending") : _(", descending") );

    //amendments for unisons
    if (m_DPitch[0] == m_DPitch[1]) {
        if (sAlter[0] == sAlter[1])
            m_sAnswer = _("Unison");
        else {
            m_sAnswer = _("Chromatic semitone");
            m_sAnswer += (oIntv.IsAscending() ? _(", ascending") : _(", descending") );
        }
    }


    //for building intervals exercise the created score is the solution and
    //we need to create another score with the problem
    m_pSolutionScore = pScore;
    m_pProblemScore = new lmScore();
    pInstr = m_pProblemScore->AddInstrument(0,0,_T(""));		//MIDI channel 0, MIDI instr 0
    pVStaff = pInstr->GetVStaff();
    m_pProblemScore->SetTopSystemDistance( pVStaff->TenthsToLogical(30, 1) );     // 3 lines
    pVStaff->AddClef( m_nClef );
    pVStaff->AddKeySignature(0, true);                    // 0 fifths, major
    pVStaff->AddTimeSignature(4 ,4, lmNO_VISIBLE );
    pVStaff->AddSpacer(30);       // 3 lines
    pNode = parserLDP.ParseText( sPattern[0] );
    pNote[0] = parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddBarline(lm_eBarlineEnd, lmNO_VISIBLE);

    //cumpute right answer button index
    int iRow, iCol;
    if (sNoteName==_T("c"))         iCol = 0;
    else if (sNoteName==_T("d"))    iCol = 1;
    else if (sNoteName==_T("e"))    iCol = 2;
    else if (sNoteName==_T("f"))    iCol = 3;
    else if (sNoteName==_T("g"))    iCol = 4;
    else if (sNoteName==_T("a"))    iCol = 5;
    else if (sNoteName==_T("b"))    iCol = 6;
    else
        wxASSERT(false);

    if (sAlter[1]==_T("--"))        iRow = 0;
    else if (sAlter[1]==_T("-"))    iRow = 1;
    else if (sAlter[1]==_T(""))     iRow = 2;
    else if (sAlter[1]==_T("="))    iRow = 2;
    else if (sAlter[1]==_T("+"))    iRow = 3;
    else if (sAlter[1]==_T("++"))   iRow = 4;
    else
        wxASSERT(false);

    m_nRespIndex = iCol + iRow * lm_NUM_COLS;

    //return question string
    m_sAnswer = _("Build a ") + m_sAnswer;
    return m_sAnswer;
}

void lmBuildIntervalCtrol::PrepareAuxScore(int nButton)
{
    // No problem is presented and the user press the button to play a specific
    // sound (chord, interval, scale, etc.)
    // This method is then invoked to prepare the score with the requested sound.
    // At return, base class will play it

    // In build intervals exercises it is not allowd to play an interval so
    // we return a Null score
    m_pAuxScore = (lmScore*)NULL;
}

wxDialog* lmBuildIntervalCtrol::GetSettingsDlg()
{
    // 'Settings' link has been clicked. This method must return the dialog to invoke

    wxDialog* pDlg = new lmDlgCfgTheoIntervals(this, m_pConstrains);
    return pDlg;
}

void lmBuildIntervalCtrol::ReconfigureButtons()
{
    // The settings has been changed. This method is invoked to reconfigure
    // answer keyboard in case it is needed for new settings

    // In build intervals exercises no reconfiguration is required
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


BEGIN_EVENT_TABLE(lmIdfyIntervalCtrol, lmOneScoreCtrol)
    EVT_COMMAND_RANGE (ID_BUTTON, ID_BUTTON+lm_NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmIdfyIntervalCtrol::OnRespButton)
END_EVENT_TABLE()



lmIdfyIntervalCtrol::lmIdfyIntervalCtrol(wxWindow* parent, wxWindowID id,
                           lmTheoIntervalsConstrains* pConstrains,
                           const wxPoint& pos, const wxSize& size, int style)
    : lmOneScoreCtrol(parent, id, pConstrains, wxSize(380, 150), pos, size, style )
{
    //initializations
    m_pConstrains = pConstrains;
    m_nRespIndex = 0;

    CreateControls();
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

wxString lmIdfyIntervalCtrol::SetNewProblem()
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

    //Generate two random note-pos in range -1 to 7 (from two ledge lines down to two up)
    lmRandomGenerator oGenerator;
    m_nClef = oGenerator.GenerateClef(m_pConstrains->GetClefConstrains());
    m_nKey = oGenerator.GenerateKey(m_pConstrains->GetKeyConstrains());
    m_DPitch[0] = oGenerator.GenerateRandomDPitch(0, 8, false, m_nClef);
    m_DPitch[1] = oGenerator.GenerateRandomDPitch(0, 8, false, m_nClef);

    ////Generate a random interval
    //lmInterval oRndIntv = GenerateRandomInterval(m_pConstrains, m_nClef, m_nKey);

    //Decide accidentals
    wxString sPattern[2];
    wxString sAlter[2];
    lmConverter oConv;
    for (int i=0; i < 2; i++) {
        sAlter[i] = _T("");
        if (m_pConstrains->GetAccidentals() && oGenerator.FlipCoin() ) {
            sAlter[i] = (oGenerator.FlipCoin() ? _T("-") : _T("+"));
        }
    }

    //amendments for unisons
    if (m_DPitch[0] == m_DPitch[1]) {
        //remove two accidentals in unison
        if (sAlter[0] != _T("") && sAlter[1] != _T("")) {
            sAlter[1] = _T("");
        }
        //add natural sign in second accidental for 'chromatic semitone'
        if (sAlter[0] != _T("") && sAlter[1] == _T("")) {
            sAlter[1] = _T("=");
        }
    }

    //prepare LDP pattern
    for (int i=0; i < 2; i++) {
        sPattern[i] = _T("(n ");
        sPattern[i] += sAlter[i];
        sPattern[i] += DPitch_GetEnglishNoteName(m_DPitch[i]) + _T(" w)");
    }

    ////DEBUG: un-comment and modify values for testing a certain interval
    //sPattern[0] = _T("(n -e4 r)");
    //sPattern[1] = _T("(n e4 r)");

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
    pNode = parserLDP.ParseText( sPattern[0] );
    pNote[0] = parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddBarline(lm_eBarlineSimple, lmNO_VISIBLE);    //so that accidental doesn't affect 2nd note
    pNode = parserLDP.ParseText( sPattern[1] );
    pNote[1] = parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddSpacer(75);       // 7.5 lines
    pVStaff->AddBarline(lm_eBarlineEnd, lmNO_VISIBLE);

    //compute the interval name
    lmInterval oIntv(pNote[0], pNote[1], earmDo);
    m_sAnswer = oIntv.GetIntervalName() + (oIntv.IsAscending() ? _(", ascending") : _(", descending") );

    //amendments for unisons
    if (m_DPitch[0] == m_DPitch[1]) {
        if (sAlter[0] == sAlter[1])
            m_sAnswer = _("Unison");
        else {
            m_sAnswer = _("Chromatic semitone");
            m_sAnswer += (oIntv.IsAscending() ? _(", ascending") : _(", descending") );
        }
    }

    //set score with the problem
    m_pProblemScore = pScore;
    m_pSolutionScore = (lmScore*)NULL;

    //cumpute right answer button index
    if (m_pConstrains->GetProblemLevel() == 0)
    {
        //only interval number
        m_nRespIndex = oIntv.GetIntervalNum() - 1;
    }
    else
    {
        int iRow, iCol;
        iCol = oIntv.GetIntervalNum() - 2;
        switch (oIntv.GetIntervalType()) {
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

        //special cases: unison and related
        if (oIntv.GetIntervalNum() == 1) {
            switch (oIntv.GetIntervalType()) {
                case eti_Perfect:
                    m_nRespIndex = lmIDX_UNISON;       //unison
                    break;
                case eti_Augmented:
                    m_nRespIndex = lmIDX_SEMITONE;      //chromatic semitone
                    break;
                default:
                    wxLogMessage(_T("[lmIdfyIntervalCtrol::NewProblem] nInterval=%d, nType=%d"),
                                oIntv.GetIntervalNum(), oIntv.GetIntervalType() );
                    wxASSERT(false);
            }
        }
    }

    //wxLogMessage(wxString::Format(
    //    _T("[lmIdfyIntervalCtrol::NewProblem] m_nRespIndex=%d, oIntv.GetIntervalNum()=%d"),
    //    m_nRespIndex, oIntv.GetIntervalNum() ));

    //return question string
    return _("Identify the next interval:");
}

void lmIdfyIntervalCtrol::PrepareAuxScore(int nButton)
{
    // No problem is presented and the user press the button to play a specific
    // sound (chord, interval, scale, etc.)
    // This method is then invoked to prepare the score with the requested sound.
    // At return, base class will play it

    // In identify interval exercises it is not allowed to play an interval so
    // we return a Null score
    m_pAuxScore = (lmScore*)NULL;
}

wxDialog* lmIdfyIntervalCtrol::GetSettingsDlg()
{
    // 'Settings' link has been clicked. This method must return the dialog to invoke

    wxDialog* pDlg = new lmDlgCfgTheoIntervals(this, m_pConstrains);
    return pDlg;
}

void lmIdfyIntervalCtrol::ReconfigureButtons()
{
    // The settings have been changed. This method is invoked to reconfigure
    // answer keyboard in case it is needed for new settings

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
        if (m_pConstrains->GetProblemLevel() == 1)
        {
            //Only minor and perfect/major
            m_nFirstRow = 2;
            nLastRow = 4;
            fUnison = true;
            fSemitone = false;
        }
        else if (m_pConstrains->GetProblemLevel() == 2)
        {
            //also augmented and diminished
            m_nFirstRow = 1;
            nLastRow = 5;
            fUnison = true;
            fSemitone = true;
        }
        else
        {
            //all columns
            m_nFirstRow = 0;
            nLastRow = 6;
            fUnison = true;
            fSemitone = true;
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
    m_sIntvButtonLabel[4] = _("dd5");
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

/*
lmInterval lmIdfyIntervalCtrol::GenerateRandomInterval(
                        lmTheoIntervalsConstrains* pConstrains,
                        lmEClefType nClef, lmEKeySignatures nKey)
{
    int nLedgerAbove = pConstrains->GetLedgerLinesAbove();
    int nLedgerBelow = pConstrains->GetLedgerLinesBelow();
    int nLevel = pConstrains->GetProblemLevel();

    typedef struct {
        lmFPitch  fpNote1;
        lmFPitch  fpNote2;
        lmFIntval nInterval;
    } lmIntvalInfo;

    //determine min. and max. notes
    lmFPitch  fpMinNote = LineToFPitch(nLedgerBelow, nKey, nClef);
    lmFPitch  fpMaxNote = LineToFPitch(nLedgerAbove, nKey, nClef);

    //level 0: Name just the interval (number)
    // Generate all natural intervals

    //for(iStartNote = min_note; iStartNote <= max_note; iStartNote++)
    //{
    //    for(iEndNote = iStartNote; iEndNote <= max_note; iEndNote++)
    //    {
    //        AddInterval(iStartNote, iEndNote)
    //    }
    //}

    //level 1: Only Perfect, Major and minor intervals.
    // Generate as Level 0
    // Add a sharp to all minor intervals
    // Add a flat to all major intervals
    if (nLevel > 0)
    {

        //for(it=intervals.begin(); it != intervals.end(); ++it)
        //{
        //    if ((*it)->IsMinor())
        //        AddInterval( AddSharpEndNote of *it )
        //    else if ((*it)->IsMajor())
        //        AddInterval( AddFlatStartNote of *it )
        //}
    }

    //level 2: Also augmented/diminished
    // Generate as Level 1
    // Add a sharp to all major intervals
    // Add a flat to all minor intervals
    if (nLevel > 1)
    {
        //for(it=intervals.begin(); it != intervals.end(); ++it)
        //{
        //    if ((*it)->IsMinor())
        //        AddInterval( AddSharpEndNote of *it )
        //    else if ((*it)->IsMajor())
        //        AddInterval( AddFlatStartNote of *it )
        //}
    }

    //level 3: Also double augmented/diminished
    // Generate as level 2
    // Add a sharp to augmented intervals
    // Add a flat to diminished intervals
    if (nLevel > 2)
    {
        //for(it=intervals.begin(); it != intervals.end(); ++it)
        //{
        //    if ((*it)->IsMinor())
        //        AddInterval( AddSharpEndNote of *it )
        //    else if ((*it)->IsMajor())
        //        AddInterval( AddFlatStartNote of *it )
        //}
    }

    //All valid intervals generated. Choose one at random

    return oIntv;
}
*/
