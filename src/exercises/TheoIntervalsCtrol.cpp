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
#pragma implementation "TheoIntervalsCtrol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

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


//Layout definitions
const int BUTTONS_DISTANCE = 5;     //pixels
const int NUM_BUTTONS = 44;         //buttons for answers

//IDs for controls
enum {
    ID_LINK_SEE_SOURCE = 3000,
    ID_LINK_DUMP,
    ID_LINK_MIDI_EVENTS,
    ID_BUTTON,
    ID_LINK = ID_BUTTON + NUM_BUTTONS,
    ID_LINK_NEW_PROBLEM,
    ID_LINK_PLAY,
    ID_LINK_SOLUTION,
    ID_LINK_SETTINGS
};

const int NUM_ROWS = 6;
const int NUM_COLS = 7;

static wxString m_sIntvButtonLabel[NUM_BUTTONS];
static wxString m_sNotesButtonLabel[35];
static wxString m_sIntvRowLabel[NUM_ROWS];
static wxString m_sNotesRowLabel[NUM_ROWS];
static wxString m_sIntvColumnLabel[NUM_COLS];
static wxString m_sNotesColumnLabel[NUM_COLS];



BEGIN_EVENT_TABLE(lmTheoIntervalsCtrol, wxWindow)
    EVT_COMMAND_RANGE (ID_BUTTON, ID_BUTTON+NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmTheoIntervalsCtrol::OnRespButton)
    EVT_SIZE            (lmTheoIntervalsCtrol::OnSize)

    LM_EVT_URL_CLICK    (ID_LINK_SEE_SOURCE, lmTheoIntervalsCtrol::OnDebugShowSourceScore)
    LM_EVT_URL_CLICK    (ID_LINK_DUMP, lmTheoIntervalsCtrol::OnDebugDumpScore)
    LM_EVT_URL_CLICK    (ID_LINK_MIDI_EVENTS, lmTheoIntervalsCtrol::OnDebugShowMidiEvents)

    LM_EVT_URL_CLICK    (ID_LINK_NEW_PROBLEM, lmTheoIntervalsCtrol::OnNewProblem)
    LM_EVT_URL_CLICK    (ID_LINK_PLAY, lmTheoIntervalsCtrol::OnPlay)
    LM_EVT_URL_CLICK    (ID_LINK_SOLUTION, lmTheoIntervalsCtrol::OnDisplaySolution)
    LM_EVT_URL_CLICK    (ID_LINK_SETTINGS, lmTheoIntervalsCtrol::OnSettingsButton)
END_EVENT_TABLE()

IMPLEMENT_CLASS(lmTheoIntervalsCtrol, wxWindow)


lmTheoIntervalsCtrol::lmTheoIntervalsCtrol(wxWindow* parent, wxWindowID id, 
                           lmTheoIntervalsConstrains* pConstrains,
                           const wxPoint& pos, const wxSize& size, int style)
    : wxWindow(parent, id, pos, size, style )
{
    //initializations
    SetBackgroundColour(*wxWHITE);
    for (int i=0; i < NUM_BUTTONS; i++) { m_pAnswerButton[i] = (wxButton*)NULL; }
    m_nRespIndex = 0;
    m_fButtonsEnabled = false;
    m_fProblemCreated = false;
    m_fPlayEnabled = false;
    m_pIntervalScore = (lmScore*)NULL;
    m_pProblemScore = (lmScore*)NULL;
    m_pScoreCtrol = (lmScoreAuxCtrol*)NULL;
    m_pConstrains = pConstrains;

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

    m_sIntvButtonLabel[7] = _T("");         //dim2
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



    //the window is divided into two regions: top, for score on left and counters and links
    //on the right, and bottom region, for answer buttons 
    wxBoxSizer* pMainSizer = new wxBoxSizer( wxVERTICAL );

    // debug buttons
    if (g_fShowDebugLinks && !g_fReleaseVersion) {
        wxBoxSizer* pDbgSizer = new wxBoxSizer( wxHORIZONTAL );
        pMainSizer->Add(pDbgSizer, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

        // "See source score"
        pDbgSizer->Add(
            new lmUrlAuxCtrol(this, ID_LINK_SEE_SOURCE, _("See source score") ),
            wxSizerFlags(0).Left().Border(wxALL, 10) );
        // "Dump score"
        pDbgSizer->Add(
            new lmUrlAuxCtrol(this, ID_LINK_DUMP, _("Dump score") ),
            wxSizerFlags(0).Left().Border(wxALL, 10) );
        // "See MIDI events"
        pDbgSizer->Add(
            new lmUrlAuxCtrol(this, ID_LINK_MIDI_EVENTS, _("See MIDI events") ),
            wxSizerFlags(0).Left().Border(wxALL, 10) );
    }

    // sizer for the scoreCtrol and the CountersCtrol
    wxBoxSizer* pTopSizer = new wxBoxSizer( wxHORIZONTAL );
    pMainSizer->Add(
        pTopSizer,
        wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 10) );

    // create score ctrl 
    m_pScoreCtrol = new lmScoreAuxCtrol(this, -1, m_pIntervalScore, wxDefaultPosition, wxSize(380,150), eSIMPLE_BORDER);
    m_pScoreCtrol->SetMargins(lmToLogicalUnits(10, lmMILLIMETERS),      //left=10mm
                              lmToLogicalUnits(25, lmMILLIMETERS),      //right=25mm
                              lmToLogicalUnits(10, lmMILLIMETERS));     //top=15mm
    pTopSizer->Add(
        m_pScoreCtrol,
        wxSizerFlags(1).Left().Border(wxTOP|wxBOTTOM, 10));

    m_pScoreCtrol->SetScale((float)1.3);


    // sizer for the CountersCtrol and the settings link
    wxBoxSizer* pCountersSizer = new wxBoxSizer( wxVERTICAL );
    pTopSizer->Add(
        pCountersSizer,
        wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 10).Expand() );

    // right/wrong answers counters control
    m_pCounters = new lmCountersCtrol(this, wxID_ANY);
    pCountersSizer->Add(
        m_pCounters,
        wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 10) );

    // spacer to move the settings link to bottom
    pCountersSizer->Add(5, 5, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    // settings link
    lmUrlAuxCtrol* pSettingsLink = new lmUrlAuxCtrol(this, ID_LINK_SETTINGS, _("Settings") );
    pCountersSizer->Add(pSettingsLink, wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 10) );

    // spacer to move the settings link a little up
    pCountersSizer->Add(5, 5, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    

        //links 

    wxBoxSizer* pLinksSizer = new wxBoxSizer( wxHORIZONTAL );
    pMainSizer->Add(
        pLinksSizer,
        wxSizerFlags(0).Center().Border(wxLEFT|wxALL, 10) );


    // "new problem" button
    pLinksSizer->Add(
        new lmUrlAuxCtrol(this, ID_LINK_NEW_PROBLEM, _("New problem") ),
        wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 20) );
    
    // "play" button
    m_pPlayButton = new lmUrlAuxCtrol(this, ID_LINK_PLAY, _("Play") );
    pLinksSizer->Add(
        m_pPlayButton,
        wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 20) );
    
    // "show solution" button
    pLinksSizer->Add(
        new lmUrlAuxCtrol(this, ID_LINK_SOLUTION, _("Show solution") ),
        wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 20) );

        //
        //create 42 buttons for the answers: six rows, seven buttons per row
        //

    int iB;

    m_pKeyboardSizer = new wxFlexGridSizer(NUM_ROWS+1, NUM_COLS+1, 0, 0);
    pMainSizer->Add(
        m_pKeyboardSizer,
        wxSizerFlags(0).Left().Border(wxALIGN_LEFT|wxTOP, 10)  );

    //row with column labels
    m_pKeyboardSizer->Add(5, 5, 0);               //spacer for labels column
    for (int iCol=0; iCol < NUM_COLS; iCol++) {
        m_pColumnLabel[iCol] = new wxStaticText(this, -1, m_sIntvColumnLabel[iCol]);
        m_pKeyboardSizer->Add(
            m_pColumnLabel[iCol],
            0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
    }

    //remaining rows with buttons
    for (int iRow=0; iRow < NUM_ROWS; iRow++) {
        m_pRowLabel[iRow] = new wxStaticText(this, -1, m_sIntvRowLabel[iRow]);
        m_pKeyboardSizer->Add(
            m_pRowLabel[iRow],
            wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, BUTTONS_DISTANCE) );

        // the buttons for this row
        for (int iCol=0; iCol < NUM_COLS; iCol++) {
            iB = iCol + iRow * NUM_COLS;    // button index: 0 .. 41            
            m_pAnswerButton[iB] = new wxButton( this, ID_BUTTON + iB, m_sIntvButtonLabel[iB],
                wxDefaultPosition, wxSize(54, 20));
            m_pKeyboardSizer->Add(
                m_pAnswerButton[iB],
                wxSizerFlags(0).Border(wxLEFT|wxRIGHT, BUTTONS_DISTANCE) );
            if (m_sIntvButtonLabel[iB].IsEmpty()) {
                m_pAnswerButton[iB]->Show(false);
                m_pAnswerButton[iB]->Enable(false);
            }
        }
    }


    //Additional row with buttons for unison and related
    wxBoxSizer* pUnisonSizer = new wxBoxSizer( wxHORIZONTAL );
    pMainSizer->Add(
        pUnisonSizer,
        wxSizerFlags(0).Left().Border(wxTOP, 10)  );

        //spacer to skip the labels
    pUnisonSizer->Add(120+BUTTONS_DISTANCE+BUTTONS_DISTANCE+10, 20, 0);

        //unison button
    iB = 42;
    m_pAnswerButton[iB] = new wxButton( this, ID_BUTTON + iB, m_sIntvButtonLabel[iB] );
    pUnisonSizer->Add(
        m_pAnswerButton[iB],
        wxSizerFlags(0).Border(wxALL, BUTTONS_DISTANCE) );

        // "chromatic semitone" button
    iB = 43;
    m_pAnswerButton[iB] = new wxButton( this, ID_BUTTON + iB, m_sIntvButtonLabel[iB] );
    pUnisonSizer->Add(
        m_pAnswerButton[iB],
        wxSizerFlags(0).Border(wxALL, BUTTONS_DISTANCE) );

    m_nCurrentKeyboard = eKeyboardIntv;



    SetSizer( pMainSizer );                 // use the sizer for window layout
    pMainSizer->SetSizeHints( this );       // set size hints to honour minimum size

    NewProblem();
}

lmTheoIntervalsCtrol::~lmTheoIntervalsCtrol()
{
    DoStopSounds();     //stop any possible score being played

    if (m_pScoreCtrol) {
        delete m_pScoreCtrol;
        m_pScoreCtrol = (lmScoreAuxCtrol*)NULL;
    }

    if (m_pConstrains) {
        delete m_pConstrains;
        m_pConstrains = (lmTheoIntervalsConstrains*) NULL;
    }

    if (m_pIntervalScore) {
        delete m_pIntervalScore;
        m_pIntervalScore = (lmScore*)NULL;
    }

    if (m_pProblemScore) {
        delete m_pProblemScore;
        m_pProblemScore = (lmScore*)NULL;
    }
}

void lmTheoIntervalsCtrol::EnableButtons(bool fEnable)
{
    if (m_nCurrentKeyboard == eKeyboardIntv)
    {
        // Intervals keyboard
        for (int iB=0; iB < NUM_BUTTONS; iB++) {
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
    m_fButtonsEnabled = fEnable;

}

//----------------------------------------------------------------------------------------
// Event handlers

void lmTheoIntervalsCtrol::OnSettingsButton(wxCommandEvent& event)
{
    lmDlgCfgTheoIntervals dlg(this, m_pConstrains);
    int retcode = dlg.ShowModal();
    if (retcode == wxID_OK) m_pConstrains->SaveSettings();
}

void lmTheoIntervalsCtrol::OnSize(wxSizeEvent& event)
{
    //wxLogMessage(_T("OnSize en IntrervalsControl"));
    Layout();

}

void lmTheoIntervalsCtrol::OnPlay(wxCommandEvent& event)
{
    Play();
}

void lmTheoIntervalsCtrol::OnNewProblem(wxCommandEvent& event)
{
    NewProblem();
}

void lmTheoIntervalsCtrol::OnDisplaySolution(wxCommandEvent& event)
{
    m_pCounters->IncrementWrong();
    DisplaySolution();
    EnableButtons(false);           //student must not give now the answer
}

void lmTheoIntervalsCtrol::OnRespButton(wxCommandEvent& event)
{
    DoStopSounds();     //stop any possible score being played

    int nIndex = event.GetId() - ID_BUTTON;

    //buttons are only enabled in m_fIntervalKnow type problems
    bool fSuccess;
    wxColour* pColor;
    
    //verify if success or failure
    fSuccess = (nIndex == m_nRespIndex);
    
    //prepare color, and update counters
    if (fSuccess) {
        pColor = &(g_pColors->Success());
        m_pCounters->IncrementRight();
    } else {
        pColor = &(g_pColors->Failure());
        m_pCounters->IncrementWrong();
    }
        
    //if failure or not auto-new problem, display the solution.
    //Else, if success and auto-new problem, generate a new problem
    if (!fSuccess || !g_fAutoNewProblem) {
        if (!fSuccess) {
            //failure: mark wrong button in red and right one in green
            m_pAnswerButton[m_nRespIndex]->SetBackgroundColour(g_pColors->Success());
            m_pAnswerButton[nIndex]->SetBackgroundColour(g_pColors->Failure());
        }
        //show the solucion
        DisplaySolution();
        EnableButtons(false);

    } else {
        NewProblem();
    }
    
}

void lmTheoIntervalsCtrol::NewProblem()
{

    ResetExercise();        //reset button colors and delete scores

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
    m_ntPitch[0] = oGenerator.GenerateRandomPitch(0, 8, false, m_nClef);
    m_ntPitch[1] = oGenerator.GenerateRandomPitch(0, 8, false, m_nClef);
    //while (m_ntPitch[0] == m_ntPitch[1]) {
    //    m_ntPitch[1] = oGenerator.GenerateRandomPitch(0, 8, false, m_nClef);
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
    if (m_ntPitch[0] == m_ntPitch[1]) {
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
        sPattern[i] += oConv.GetEnglishNoteName(m_ntPitch[i]) + _T(" r)");
    }

    //save information to identify answer button in 'build interval' problems
    wxString sNoteName;
    if (!m_fIntervalKnown) {
        sNoteName = (oConv.GetEnglishNoteName(m_ntPitch[1])).Left(1);
    }
    
    ////DEBUG: un-comment and modify values for testing a certain interval
    //sPattern[0] = _T("(n -e4 r)");
    //sPattern[1] = _T("(n e4 r)");

    //create the score with the interval
    lmNote* pNote[2];
    lmLDPParser parserLDP;
    lmLDPNode* pNode;
    lmVStaff* pVStaff;

    m_pIntervalScore = new lmScore();
    m_pIntervalScore->SetTopSystemDistance( lmToLogicalUnits(5, lmMILLIMETERS) );   //5mm
    m_pIntervalScore->SetOption(_T("Render.SpacingMethod"), (long)esm_Fixed);
    m_pIntervalScore->AddInstrument(1,0,0,_T(""));                    //one vstaff, MIDI channel 0, MIDI instr 0
    pVStaff = m_pIntervalScore->GetVStaff(1, 1);    //get first vstaff of instr.1
    pVStaff->AddClef( m_nClef );
    pVStaff->AddKeySignature(0, true);                    // 0 fifths, major
    pVStaff->AddTimeSignature(4 ,4, lmNO_VISIBLE );
    //pVStaff->AddSpacer(30);       // 3 lines
    pNode = parserLDP.ParseText( sPattern[0] );
    pNote[0] = parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddBarline(etb_SimpleBarline, lmNO_VISIBLE);    //so that accidental doesn't affect 2nd note
    pNode = parserLDP.ParseText( sPattern[1] );
    pNote[1] = parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddSpacer(75);       // 7.5 lines
    pVStaff->AddBarline(etb_EndBarline, lmNO_VISIBLE);

    //compute the interval name
    lmInterval oIntv(pNote[0], pNote[1], earmDo);
    m_sAnswer = oIntv.GetIntervalName() + (oIntv.IsAscending() ? _(", ascending") : _(", descending") ); 

    //amendments for unisons
    if (m_ntPitch[0] == m_ntPitch[1]) {
        if (sAlter[0] == sAlter[1])
            m_sAnswer = _("Unison"); 
        else {
            m_sAnswer = _("Chromatic semitone");
            m_sAnswer += (oIntv.IsAscending() ? _(", ascending") : _(", descending") ); 
        }
    }


    //for building intervals exercise create the score with the problem
    if (!m_fIntervalKnown) {
        m_pProblemScore = new lmScore();
        m_pProblemScore->SetTopSystemDistance( lmToLogicalUnits(5, lmMILLIMETERS) );   //5mm
        m_pProblemScore->AddInstrument(1,0,0,_T(""));                    //one vstaff, MIDI channel 0, MIDI instr 0
        pVStaff = m_pProblemScore->GetVStaff(1, 1);    //get first vstaff of instr.1
        pVStaff->AddClef( m_nClef );
        pVStaff->AddKeySignature(0, true);                    // 0 fifths, major
        pVStaff->AddTimeSignature(4 ,4, lmNO_VISIBLE );
        //pVStaff->AddSpacer(30);       // 3 lines
        pNode = parserLDP.ParseText( sPattern[0] );
        pNote[0] = parserLDP.AnalyzeNote(pNode, pVStaff);
        pVStaff->AddBarline(etb_EndBarline, lmNO_VISIBLE);
    }

    //cumpute right answer button index
    if (m_fIntervalKnown) {
        m_ntMidi[0] = oIntv.GetMidiNote1();
        m_ntMidi[1] = oIntv.GetMidiNote2();
        
        //compute the index for the button that corresponds to the right answer
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
        m_nRespIndex = iCol + iRow * NUM_COLS;

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

        m_nRespIndex = iCol + iRow * NUM_COLS;
    }
    
    //display the problem
    m_pCounters->NextTeam();
    if (m_fIntervalKnown) {
        //direct problem: identify interval
        //wxLogMessage(_T("[lmTheoIntervalsCtrol::NewProblem] IntervalScoreID=%d"), m_pIntervalScore->GetID() );
        m_pScoreCtrol->DisplayScore(m_pIntervalScore);
        m_pScoreCtrol->DisplayMessage(_("Identify the next interval:"), lmToLogicalUnits(5, lmMILLIMETERS), false);
        m_pPlayButton->Enable(true);
        SetButtonsForIntervals();
    } else {
        //inverse problem: build interval
        //wxLogMessage(_T("[lmTheoIntervalsCtrol::NewProblem] ProblemScoreID=%d"), m_pProblemScore->GetID() );
        m_pScoreCtrol->DisplayScore(m_pProblemScore);
        wxString sProblem = _("Build a ") + m_sAnswer;
        m_pScoreCtrol->DisplayMessage(sProblem, lmToLogicalUnits(5, lmMILLIMETERS), false);
        m_pPlayButton->Enable(false);
        SetButtonsForNotes();
    }
    EnableButtons(true);
    m_fPlayEnabled = false;
    m_fProblemCreated = true;

    
}

void lmTheoIntervalsCtrol::Play()
{
    //@attention As the interval is built using whole notes, we will play it at MM=240 so
    //that real note rate will be 80.
    m_pScoreCtrol->PlayScore(lmVISUAL_TRACKING, NO_MARCAR_COMPAS_PREVIO, 
                             ePM_NormalInstrument, 320);
}

void lmTheoIntervalsCtrol::DisplaySolution()
{
    DoStopSounds();     //stop any possible score being played

    if (m_fIntervalKnown) {
        m_pScoreCtrol->DisplayMessage(m_sAnswer, lmToLogicalUnits(5, lmMILLIMETERS), false);
        m_pAnswerButton[m_nRespIndex]->SetBackgroundColour(g_pColors->Success());
    }
    else {
        m_pScoreCtrol->DisplayScore(m_pIntervalScore, false);
    }
    
    m_pPlayButton->Enable(true);
    m_fPlayEnabled = true;
    m_fProblemCreated = false;
    
}

void lmTheoIntervalsCtrol::OnDebugShowSourceScore(wxCommandEvent& event)
{
    m_pScoreCtrol->SourceLDP();
}

void lmTheoIntervalsCtrol::OnDebugDumpScore(wxCommandEvent& event)
{
    m_pScoreCtrol->Dump();
}

void lmTheoIntervalsCtrol::OnDebugShowMidiEvents(wxCommandEvent& event)
{
    m_pScoreCtrol->DumpMidiEvents();
}

void lmTheoIntervalsCtrol::ResetExercise()
{
    DoStopSounds();     //stop any possible score being played

    //restore colors
    if (m_nCurrentKeyboard == eKeyboardIntv)
    {
        // Intervals keyboard
        for (int iB=0; iB < NUM_BUTTONS; iB++) {
            if (!m_sIntvButtonLabel[iB].IsEmpty()) {
                m_pAnswerButton[iB]->SetBackgroundColour( g_pColors->Normal() );
            }
        }
    }
    else {
        // Notes keyboard
        for (int iB=0; iB < 35; iB++) {
            m_pAnswerButton[iB]->SetBackgroundColour( g_pColors->Normal() );
        }
    }

    if (m_pIntervalScore) {
        delete m_pIntervalScore;
        m_pIntervalScore = (lmScore*)NULL;
    }
    
    if (m_pProblemScore) {
        delete m_pProblemScore;
        m_pProblemScore = (lmScore*)NULL;
    }
    

}

void lmTheoIntervalsCtrol::SetButtonsForNotes()
{
    if (m_nCurrentKeyboard == eKeyboardNotes) return;

    int iB;
    for (int iRow=0; iRow < 5; iRow++) {
        for (int iCol=0; iCol < NUM_COLS; iCol++) {
            iB = iCol + iRow * NUM_COLS;    // button index: 0 .. 34            
            m_pAnswerButton[iB]->Show(true);
            m_pAnswerButton[iB]->Enable(true);
            m_pAnswerButton[iB]->SetLabel(m_sNotesButtonLabel[iB]);
            m_pAnswerButton[iB]->SetBackgroundColour( g_pColors->Normal() );
        }
    }

    //hide un-used buttons
    for (int iRow=5; iRow < NUM_ROWS; iRow++) {
        for (int iCol=0; iCol < NUM_COLS; iCol++) {
            iB = iCol + iRow * NUM_COLS;    // button index: 35 .. 41            
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
    for (int iRow=0; iRow < NUM_ROWS; iRow++) {
        m_pRowLabel[iRow]->SetLabel( m_sNotesRowLabel[iRow] );
    }

    // column lables
    for (int iCol=0; iCol < NUM_COLS; iCol++) {
        m_pColumnLabel[iCol]->SetLabel( m_sNotesColumnLabel[iCol] );
    }

    m_nCurrentKeyboard = eKeyboardNotes;
    m_pKeyboardSizer->Layout();

}

void lmTheoIntervalsCtrol::SetButtonsForIntervals()
{
    if (m_nCurrentKeyboard == eKeyboardIntv) return;

    int iB;
    for (iB=0; iB < NUM_BUTTONS; iB++) {
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
    for (int iRow=0; iRow < NUM_ROWS; iRow++) {
        m_pRowLabel[iRow]->SetLabel( m_sIntvRowLabel[iRow] );
    }

    // column lables
    for (int iCol=0; iCol < NUM_COLS; iCol++) {
        m_pColumnLabel[iCol]->SetLabel( m_sIntvColumnLabel[iCol] );
    }

    m_nCurrentKeyboard = eKeyboardIntv;
    m_pKeyboardSizer->Layout();

}

void lmTheoIntervalsCtrol::DoStopSounds()
{
    //Stop any possible score being played to avoid crashes
    if (m_pIntervalScore) m_pIntervalScore->Stop();
    if (m_pProblemScore) m_pProblemScore->Stop();

}

