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
#include "../app/DlgCfgTheoIntervals.h"

#include "../globals/Colors.h"
extern lmColors* g_pColors;

// access to global external variables
extern bool g_fReleaseVersion;            // in TheApp.cpp
extern bool g_fReleaseBehaviour;        // in TheApp.cpp
extern bool g_fShowDebugLinks;            // in TheApp.cpp
extern bool g_fAutoNewProblem;          // in Preferences.cpp


//------------------------------------------------------------------------------------
// Implementation of lmTheoIntervalsCtrol


//type of keyboard currently displayed
enum {
    eKeyboardNone = 0,
    eKeyboardIntv,
    eKeyboardNotes
};

//internationalized strings
static wxString m_sIntvButtonLabel[lmTheoIntervalsCtrol::m_NUM_BUTTONS];
static wxString m_sNotesButtonLabel[35];
static wxString m_sIntvRowLabel[lmTheoIntervalsCtrol::m_NUM_ROWS];
static wxString m_sNotesRowLabel[lmTheoIntervalsCtrol::m_NUM_ROWS];
static wxString m_sIntvColumnLabel[lmTheoIntervalsCtrol::m_NUM_COLS];
static wxString m_sNotesColumnLabel[lmTheoIntervalsCtrol::m_NUM_COLS];


//IDs for controls
enum {
    ID_BUTTON = 3010,
};


BEGIN_EVENT_TABLE(lmTheoIntervalsCtrol, lmOneScoreCtrol)
    EVT_COMMAND_RANGE (ID_BUTTON, ID_BUTTON+m_NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmTheoIntervalsCtrol::OnRespButton)
END_EVENT_TABLE()



lmTheoIntervalsCtrol::lmTheoIntervalsCtrol(wxWindow* parent, wxWindowID id, 
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

lmTheoIntervalsCtrol::~lmTheoIntervalsCtrol()
{
}

void lmTheoIntervalsCtrol::CreateAnswerButtons(int nHeight, int nSpacing, wxFont& font)
{
    //
    //create 42 buttons for the answers: six rows, seven buttons per row
    //

    int iB;
    for (iB=0; iB < m_NUM_BUTTONS; iB++) {
        m_pAnswerButton[iB] = (wxButton*)NULL; 
    }

    m_pKeyboardSizer = new wxFlexGridSizer(m_NUM_ROWS+1, m_NUM_COLS+1, 0, 0);
    m_pMainSizer->Add(
        m_pKeyboardSizer,
        wxSizerFlags(0).Left().Border(wxALIGN_LEFT|wxTOP, 2*nSpacing)  );

    //row with column labels
    m_pKeyboardSizer->Add(nSpacing, nSpacing, 0);               //spacer for labels column
    for (int iCol=0; iCol < m_NUM_COLS; iCol++)
    {
        m_pColumnLabel[iCol] = new wxStaticText(this, -1, m_sIntvColumnLabel[iCol]);
        m_pColumnLabel[iCol]->SetFont(font);
        m_pKeyboardSizer->Add(
            m_pColumnLabel[iCol], 0,
            wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE,
            nSpacing);
    }

    //remaining rows with buttons
    for (int iRow=0; iRow < m_NUM_ROWS; iRow++)
    {
        m_pRowLabel[iRow] = new wxStaticText(this, -1, m_sIntvRowLabel[iRow]);
        m_pRowLabel[iRow]->SetFont(font);
        m_pKeyboardSizer->Add(
            m_pRowLabel[iRow],
            wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, nSpacing) );

        // the buttons for this row
        for (int iCol=0; iCol < m_NUM_COLS; iCol++) {
            iB = iCol + iRow * m_NUM_COLS;    // button index: 0 .. 41            
            m_pAnswerButton[iB] = new wxButton( this, ID_BUTTON + iB, m_sIntvButtonLabel[iB],
                wxDefaultPosition, wxSize(11*nSpacing, nHeight));
            m_pAnswerButton[iB]->SetFont(font);

            m_pKeyboardSizer->Add(
                m_pAnswerButton[iB],
                wxSizerFlags(0).Border(wxLEFT|wxRIGHT, nSpacing) );
            if (m_sIntvButtonLabel[iB].IsEmpty()) {
                m_pAnswerButton[iB]->Show(false);
                m_pAnswerButton[iB]->Enable(false);
            }
        }
    }


    //Additional row with buttons for unison and related
    wxBoxSizer* pUnisonSizer = new wxBoxSizer( wxHORIZONTAL );
    m_pMainSizer->Add(
        pUnisonSizer,
        wxSizerFlags(0).Left().Border(wxTOP, 2*nSpacing)  );

        //spacer to skip the labels
    pUnisonSizer->Add(28*nSpacing, 4*nSpacing, 0);

        //unison button
    iB = 42;
    m_pAnswerButton[iB] = new wxButton( this, ID_BUTTON + iB, m_sIntvButtonLabel[iB] );
    m_pAnswerButton[iB]->SetFont(font);
    pUnisonSizer->Add(
        m_pAnswerButton[iB],
        wxSizerFlags(0).Border(wxALL, nSpacing) );

        // "chromatic semitone" button
    iB = 43;
    m_pAnswerButton[iB] = new wxButton( this, ID_BUTTON + iB, m_sIntvButtonLabel[iB] );
    m_pAnswerButton[iB]->SetFont(font);
    pUnisonSizer->Add(
        m_pAnswerButton[iB],
        wxSizerFlags(0).Border(wxALL, nSpacing) );

    m_nCurrentKeyboard = eKeyboardIntv;

    //inform base class about the settings
    SetButtons(m_pAnswerButton, m_NUM_BUTTONS, ID_BUTTON);

}

void lmTheoIntervalsCtrol::EnableButtons(bool fEnable)
{
    if (m_nCurrentKeyboard == eKeyboardIntv)
    {
        // Intervals keyboard
        for (int iB=0; iB < m_NUM_BUTTONS; iB++) {
            if (!m_sIntvButtonLabel[iB].IsEmpty())
                m_pAnswerButton[iB]->Enable(fEnable);
        }
    }
    else {
        // Notes keyboard
        for (int iB=0; iB < 35; iB++) {
            m_pAnswerButton[iB]->Enable(fEnable);
        }
    }

}

wxString lmTheoIntervalsCtrol::SetNewProblem()
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

    // choose type of problem
    lmRandomGenerator oGenerator;
    switch (m_pConstrains->GetProblemType())
    {
        case ePT_DeduceInterval:
            m_fIntervalKnown = true;
            break;
        case ePT_BuildInterval:
            m_fIntervalKnown = false;
            break;
        case ePT_Both:
            m_fIntervalKnown = oGenerator.FlipCoin();
            break;
    }
    
    //Generate two random note-pos in range -1 to 7 (from two ledge lines down to two up)
    m_nClef = oGenerator.GenerateClef(m_pConstrains->GetClefConstrains());
    m_DPitch[0] = oGenerator.GenerateRandomDPitch(0, 8, false, m_nClef);
    m_DPitch[1] = oGenerator.GenerateRandomDPitch(0, 8, false, m_nClef);
    //while (m_DPitch[0] == m_DPitch[1]) {
    //    m_DPitch[1] = oGenerator.GenerateRandomDPitch(0, 8, false, m_nClef);
    //}
    
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
        sPattern[i] += DPitch_GetEnglishNoteName(m_DPitch[i]) + _T(" r)");
    }

    //save information to identify answer button in 'build interval' problems
    wxString sNoteName;
    if (!m_fIntervalKnown) {
        sNoteName = (DPitch_GetEnglishNoteName(m_DPitch[1])).Left(1);
    }
    
    ////DEBUG: un-comment and modify values for testing a certain interval
    //sPattern[0] = _T("(n -e4 r)");
    //sPattern[1] = _T("(n e4 r)");

    //create the score with the interval
    lmNote* pNote[2];
    lmLDPParser parserLDP;
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
    //pVStaff->AddSpacer(30);       // 3 lines
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
    if (m_fIntervalKnown)
    {
        m_pProblemScore = pScore;
        m_pSolutionScore = (lmScore*)NULL;
    }
    else
    {
        m_pSolutionScore = pScore;
        m_pProblemScore = new lmScore();
        lmInstrument* pInstr = m_pProblemScore->AddInstrument(0,0,_T(""));		//MIDI channel 0, MIDI instr 0
        pVStaff = pInstr->GetVStaff();
        m_pProblemScore->SetTopSystemDistance( pVStaff->TenthsToLogical(30, 1) );     // 3 lines
        pVStaff->AddClef( m_nClef );
        pVStaff->AddKeySignature(0, true);                    // 0 fifths, major
        pVStaff->AddTimeSignature(4 ,4, lmNO_VISIBLE );
        //pVStaff->AddSpacer(30);       // 3 lines
        pNode = parserLDP.ParseText( sPattern[0] );
        pNote[0] = parserLDP.AnalyzeNote(pNode, pVStaff);
        pVStaff->AddBarline(lm_eBarlineEnd, lmNO_VISIBLE);
    }

    //cumpute right answer button index
    if (m_fIntervalKnown)
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
        m_nRespIndex = iCol + iRow * m_NUM_COLS;

        //special cases: unison and related
        if (oIntv.GetIntervalNum() == 1) {
            switch (oIntv.GetIntervalType()) {
                case eti_Perfect:
                    m_nRespIndex = 42;       //unison
                    break;                
                case eti_Augmented:
                    m_nRespIndex = 43;      //chromatic semitone
                    break;                
                default:
                    wxLogMessage(_T("[lmTheoIntervalsCtrol::NewProblem] nInterval=%d, nType=%d"),
                                oIntv.GetIntervalNum(), oIntv.GetIntervalType() );
                    wxASSERT(false);
            }
        }

        //wxLogMessage(wxString::Format(
        //    _T("[lmTheoIntervalsCtrol::NewProblem] m_nRespIndex=%d, oIntv.GetIntervalNum()=%d"),
        //    m_nRespIndex, oIntv.GetIntervalNum() ));
    }
    else {
        // Solution for 'build interval' problems
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

        m_nRespIndex = iCol + iRow * m_NUM_COLS;
    }
    
    //prepare appropriate answer buttons for the problem type
    if (m_fIntervalKnown)
    {
        //direct problem: identify interval
        SetButtonsForIntervals();
        return _("Identify the next interval:");
    }
    else
    {
        //inverse problem: build interval
        SetButtonsForNotes();
        m_sAnswer = _("Build a ") + m_sAnswer;
        return m_sAnswer;
    }
    
}

void lmTheoIntervalsCtrol::PrepareAuxScore(int nButton)
{
    // No problem is presented and the user press the button to play a specific 
    // sound (chord, interval, scale, etc.)
    // This method is then invoked to prepare the score with the requested sound.
    // At return, base class will play it

    // In Theo Intervals exercises it is not allowd to play an interval so
    // we return a Null score
    m_pAuxScore = (lmScore*)NULL;

}

wxDialog* lmTheoIntervalsCtrol::GetSettingsDlg()
{
    // 'Settings' link has been clicked. This method must return the dialog to invoke

    wxDialog* pDlg = new lmDlgCfgTheoIntervals(this, m_pConstrains);
    return pDlg;
}

void lmTheoIntervalsCtrol::ReconfigureButtons()
{
    // The settings has been changed. This method is invoked to reconfigure
    // answer keyboard in case it is needed for new settings

    // In ThoIntervals no reconfiguration is required

}

void lmTheoIntervalsCtrol::SetButtonsForNotes()
{
    if (m_nCurrentKeyboard == eKeyboardNotes) return;

    int iB;
    for (int iRow=0; iRow < 5; iRow++) {
        for (int iCol=0; iCol < m_NUM_COLS; iCol++) {
            iB = iCol + iRow * m_NUM_COLS;    // button index: 0 .. 34            
            m_pAnswerButton[iB]->Show(true);
            m_pAnswerButton[iB]->Enable(true);
            m_pAnswerButton[iB]->SetLabel(m_sNotesButtonLabel[iB]);
            m_pAnswerButton[iB]->SetBackgroundColour( g_pColors->Normal() );
        }
    }

    //hide un-used buttons
    for (int iRow=5; iRow < m_NUM_ROWS; iRow++) {
        for (int iCol=0; iCol < m_NUM_COLS; iCol++) {
            iB = iCol + iRow * m_NUM_COLS;    // button index: 35 .. 41            
            m_pAnswerButton[iB]->Show(false);
            m_pAnswerButton[iB]->Enable(false);
        }
    }

    //hide extra buttons for unisons
    for (iB=42; iB < 44; iB++) {
        m_pAnswerButton[iB]->Show(false);
        m_pAnswerButton[iB]->Enable(false);
    }

    // row lables
    for (int iRow=0; iRow < m_NUM_ROWS; iRow++) {
        m_pRowLabel[iRow]->SetLabel( m_sNotesRowLabel[iRow] );
    }

    // column lables
    for (int iCol=0; iCol < m_NUM_COLS; iCol++) {
        m_pColumnLabel[iCol]->SetLabel( m_sNotesColumnLabel[iCol] );
    }

    m_nCurrentKeyboard = eKeyboardNotes;
    m_pKeyboardSizer->Layout();

}

void lmTheoIntervalsCtrol::SetButtonsForIntervals()
{
    if (m_nCurrentKeyboard == eKeyboardIntv) return;

    int iB;
    for (iB=0; iB < m_NUM_BUTTONS; iB++) {
        if (m_sIntvButtonLabel[iB].IsEmpty()) {
            m_pAnswerButton[iB]->Show(false);
            m_pAnswerButton[iB]->Enable(false);
        }
        else {
            m_pAnswerButton[iB]->Show(true);
            m_pAnswerButton[iB]->Enable(true);
            m_pAnswerButton[iB]->SetLabel(m_sIntvButtonLabel[iB]);
            m_pAnswerButton[iB]->SetBackgroundColour( g_pColors->Normal() );
        }
    }

    // row lables
    for (int iRow=0; iRow < m_NUM_ROWS; iRow++) {
        m_pRowLabel[iRow]->SetLabel( m_sIntvRowLabel[iRow] );
    }

    // column lables
    for (int iCol=0; iCol < m_NUM_COLS; iCol++) {
        m_pColumnLabel[iCol]->SetLabel( m_sIntvColumnLabel[iCol] );
    }

    m_nCurrentKeyboard = eKeyboardIntv;
    m_pKeyboardSizer->Layout();

}

void lmTheoIntervalsCtrol::InitializeStrings()
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

    m_sNotesRowLabel[0] = _("Double flat");
    m_sNotesRowLabel[1] = _("Flat");
    m_sNotesRowLabel[2] = _("Natural");
    m_sNotesRowLabel[3] = _("Sharp");
    m_sNotesRowLabel[4] = _("Double sharp");
    m_sNotesRowLabel[5] = _T("");

        // button column labels
    m_sIntvColumnLabel[0] = _("2nd");
    m_sIntvColumnLabel[1] = _("3rd");
    m_sIntvColumnLabel[2] = _("4th");
    m_sIntvColumnLabel[3] = _("5th");
    m_sIntvColumnLabel[4] = _("6th");
    m_sIntvColumnLabel[5] = _("7th");
    m_sIntvColumnLabel[6] = _("8th");

    m_sNotesColumnLabel[0] = _("C");
    m_sNotesColumnLabel[1] = _("D");
    m_sNotesColumnLabel[2] = _("E");
    m_sNotesColumnLabel[3] = _("F");
    m_sNotesColumnLabel[4] = _("G");
    m_sNotesColumnLabel[5] = _("A");
    m_sNotesColumnLabel[6] = _("B");

        //button labels (intervals)
    m_sIntvButtonLabel[0] = _T("");         //dd2
    m_sIntvButtonLabel[1] = _("dd3");
    m_sIntvButtonLabel[2] = _("dd4");
    m_sIntvButtonLabel[3] = _("dd5");
    m_sIntvButtonLabel[4] = _("dd5");
    m_sIntvButtonLabel[5] = _("dd7");
    m_sIntvButtonLabel[6] = _("dd8");

    m_sIntvButtonLabel[7] = _T("dim2");         //dim2
    m_sIntvButtonLabel[8] = _("dim3");
    m_sIntvButtonLabel[9] = _("dim4");
    m_sIntvButtonLabel[10] = _("dim5");
    m_sIntvButtonLabel[11] = _("dim6");
    m_sIntvButtonLabel[12] = _("dim7");
    m_sIntvButtonLabel[13] = _("dim8");

    m_sIntvButtonLabel[14] = _("m2");
    m_sIntvButtonLabel[15] = _("m3");
    m_sIntvButtonLabel[16] = _T("");
    m_sIntvButtonLabel[17] = _T("");
    m_sIntvButtonLabel[18] = _("m6");
    m_sIntvButtonLabel[19] = _("m7");
    m_sIntvButtonLabel[20] = _T("");

    m_sIntvButtonLabel[21] = _("M2");
    m_sIntvButtonLabel[22] = _("M3");
    m_sIntvButtonLabel[23] = _("P4");
    m_sIntvButtonLabel[24] = _("P5");
    m_sIntvButtonLabel[25] = _("M6");
    m_sIntvButtonLabel[26] = _("M7");
    m_sIntvButtonLabel[27] = _("P8");

    m_sIntvButtonLabel[28] = _("aug2");
    m_sIntvButtonLabel[29] = _("aug3");
    m_sIntvButtonLabel[30] = _("aug4");
    m_sIntvButtonLabel[31] = _("aug5");
    m_sIntvButtonLabel[32] = _("aug6");
    m_sIntvButtonLabel[33] = _("aug7");
    m_sIntvButtonLabel[34] = _("aug8");

    m_sIntvButtonLabel[35] = _("da2");
    m_sIntvButtonLabel[36] = _("da3");
    m_sIntvButtonLabel[37] = _("da4");
    m_sIntvButtonLabel[38] = _("da5");
    m_sIntvButtonLabel[39] = _("da6");
    m_sIntvButtonLabel[40] = _("da7");
    m_sIntvButtonLabel[41] = _("da7");

    m_sIntvButtonLabel[42] = _("Unison");
    m_sIntvButtonLabel[43] = _("Chromatic semitone");


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

