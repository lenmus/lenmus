//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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
/*! @file TheoIntervalsCtrol.cpp
    @brief Implementation file for class lmTheoIntervalsCtrol
    @ingroup html_controls
*/
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


//------------------------------------------------------------------------------------
// Implementation of lmTheoIntervalsCtrol




//Layout definitions
const int BUTTONS_DISTANCE    = 5;        //pixels
const int NUM_BUTTONS = 50;                //buttons for answers
const int NUM_LINKS = 3;                //links for actions

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

static wxString m_sIntvButtonLabel[NUM_BUTTONS];
static wxString m_sNotesButtonLabel[NUM_BUTTONS];


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
    for (int i=0; i < NUM_BUTTONS; i++) { m_pAnswerButton[i] = (wxButton*)NULL; }
    m_nRespIndex = 0;
    m_fButtonsEnabled = false;
    m_fProblemCreated = false;
    m_fPlayEnabled = false;
    m_pScore = (lmScore*)NULL;
    m_pScoreCtrol = (lmScoreAuxCtrol*)NULL;
    m_pConstrains = pConstrains;

    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    wxString sRowLabel[6];
    sRowLabel[0] = _("dd - double diminished");
    sRowLabel[1] = _("dim - diminished");
    sRowLabel[2] = _("m - minor");
    sRowLabel[3] = _("M, P - Major, Perfect");
    sRowLabel[4] = _("aug - augmented");
    sRowLabel[5] = _("da - double augmented");

    wxString sColumnLabel[7];
    sColumnLabel[0] = _("2nd");
    sColumnLabel[1] = _("3rd");
    sColumnLabel[2] = _("4th");
    sColumnLabel[3] = _("5th");
    sColumnLabel[4] = _("6th");
    sColumnLabel[5] = _("7th");
    sColumnLabel[6] = _("8th");

        //button labels (intervals)
    m_sIntvButtonLabel[0] = _("Unison");
    m_sIntvButtonLabel[1] = _T("");         //dd2
    m_sIntvButtonLabel[2] = _("dd3");
    m_sIntvButtonLabel[3] = _("dd4");
    m_sIntvButtonLabel[4] = _("dd5");
    m_sIntvButtonLabel[5] = _("dd5");
    m_sIntvButtonLabel[6] = _("dd7");
    m_sIntvButtonLabel[7] = _("dd8");
    m_sIntvButtonLabel[8] = _T("");     
    m_sIntvButtonLabel[9] = _T("");         //dim2
    m_sIntvButtonLabel[10] = _("dim3");
    m_sIntvButtonLabel[11] = _("dim4");
    m_sIntvButtonLabel[12] = _("dim5");
    m_sIntvButtonLabel[13] = _("dim6");
    m_sIntvButtonLabel[14] = _("dim7");
    m_sIntvButtonLabel[15] = _("dim8");
    m_sIntvButtonLabel[16] = _T("");
    m_sIntvButtonLabel[17] = _("m2");
    m_sIntvButtonLabel[18] = _("m3");
    m_sIntvButtonLabel[19] = _T("");
    m_sIntvButtonLabel[20] = _T("");
    m_sIntvButtonLabel[21] = _("m6");
    m_sIntvButtonLabel[22] = _("m7");
    m_sIntvButtonLabel[23] = _T("");
    m_sIntvButtonLabel[24] = _T("");
    m_sIntvButtonLabel[25] = _("M2");
    m_sIntvButtonLabel[26] = _("M3");
    m_sIntvButtonLabel[27] = _("P4");
    m_sIntvButtonLabel[28] = _("P5");
    m_sIntvButtonLabel[29] = _("M6");
    m_sIntvButtonLabel[30] = _("M7");
    m_sIntvButtonLabel[31] = _("P8");
    m_sIntvButtonLabel[32] = _T("");
    m_sIntvButtonLabel[33] = _("aug2");
    m_sIntvButtonLabel[34] = _("aug3");
    m_sIntvButtonLabel[35] = _("aug4");
    m_sIntvButtonLabel[36] = _("aug5");
    m_sIntvButtonLabel[37] = _("aug6");
    m_sIntvButtonLabel[38] = _("aug7");
    m_sIntvButtonLabel[39] = _("aug8");
    m_sIntvButtonLabel[40] = _T("");
    m_sIntvButtonLabel[41] = _("da2");
    m_sIntvButtonLabel[42] = _("da3");
    m_sIntvButtonLabel[43] = _("da4");
    m_sIntvButtonLabel[44] = _("da5");
    m_sIntvButtonLabel[45] = _("da6");
    m_sIntvButtonLabel[46] = _("da7");
    m_sIntvButtonLabel[47] = _("da7");
    m_sIntvButtonLabel[48] = _("Chromatic semitone");
    m_sIntvButtonLabel[49] = _T("");  //("Prime double augmented");

        //button labels (for notes)
    m_sNotesButtonLabel[0] = _T("bb");
    m_sNotesButtonLabel[1] = _T("b");         
    m_sNotesButtonLabel[2] = _("C");
    m_sNotesButtonLabel[3] = _T("#");
    m_sNotesButtonLabel[4] = _T("x");

    m_sNotesButtonLabel[5] = _T("bb");
    m_sNotesButtonLabel[6] = _T("b");
    m_sNotesButtonLabel[7] = _("D");
    m_sNotesButtonLabel[8] = _T("#");
    m_sNotesButtonLabel[9] = _T("x");

    m_sNotesButtonLabel[10] = _T("bb");
    m_sNotesButtonLabel[11] = _T("b");
    m_sNotesButtonLabel[12] = _("E");
    m_sNotesButtonLabel[13] = _T("#");
    m_sNotesButtonLabel[14] = _T("x");

    m_sNotesButtonLabel[15] = _T("bb");
    m_sNotesButtonLabel[16] = _T("b");
    m_sNotesButtonLabel[17] = _("F");
    m_sNotesButtonLabel[18] = _T("#");
    m_sNotesButtonLabel[19] = _T("x");

    m_sNotesButtonLabel[20] = _T("bb");
    m_sNotesButtonLabel[21] = _T("b");
    m_sNotesButtonLabel[22] = _("G");
    m_sNotesButtonLabel[23] = _T("#");
    m_sNotesButtonLabel[24] = _T("x");

    m_sNotesButtonLabel[25] = _T("bb");
    m_sNotesButtonLabel[26] = _T("b");
    m_sNotesButtonLabel[27] = _("A");
    m_sNotesButtonLabel[28] = _T("#");
    m_sNotesButtonLabel[29] = _T("x");

    m_sNotesButtonLabel[30] = _T("bb");
    m_sNotesButtonLabel[31] = _T("b");
    m_sNotesButtonLabel[32] = _("B");
    m_sNotesButtonLabel[33] = _T("#");
    m_sNotesButtonLabel[34] = _T("x");



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
    m_pScoreCtrol = new lmScoreAuxCtrol(this, -1, m_pScore, wxDefaultPosition, wxSize(400,150), eSIMPLE_BORDER);
    m_pScoreCtrol->SetMargins(lmToLogicalUnits(10, lmMILLIMETERS),      //left=1cm
                              lmToLogicalUnits(20, lmMILLIMETERS),      //right=2cm
                              lmToLogicalUnits(10, lmMILLIMETERS));     //top=1cm
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
        //create 48 buttons for the answers: six rows, eight buttons per row
        //

    const int NUM_ROWS = 6;
    const int NUM_COLS = 8;
    wxButton* pButton;
    int iB;

    wxFlexGridSizer* pGridSizer = new wxFlexGridSizer(NUM_ROWS+1, NUM_COLS, 0, 0);
    pMainSizer->Add(
        pGridSizer,
        wxSizerFlags(0).Left().Border(wxALIGN_LEFT|wxTOP, 10)  );

    //row with column labels
    pGridSizer->Add(5, 5, 0);               //spacer for labels column
    for (int iCol=0; iCol < 7; iCol++) {
        pGridSizer->Add(
            new wxStaticText(this, -1, sColumnLabel[iCol]),
            0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
    }

    //remaining rows with buttons
    for (int iRow=0; iRow < NUM_ROWS; iRow++) {
        pGridSizer->Add(
            new wxStaticText(this, -1, sRowLabel[iRow]),
            wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, BUTTONS_DISTANCE) );

        // the buttons for this row
        for (int iCol=0; iCol < NUM_COLS; iCol++) {
            iB = iCol + iRow * NUM_COLS;    // button index: 0 .. 47            
            //column 0 (unisons) is not created, as buttons for it will be created later
            if (iCol != 0) {
                m_pAnswerButton[iB] = new wxButton( this, ID_BUTTON + iB, m_sIntvButtonLabel[iB],
                    wxDefaultPosition, wxSize(54, 20));
                pGridSizer->Add(
                    m_pAnswerButton[iB],
                    wxSizerFlags(0).Border(wxLEFT|wxRIGHT, BUTTONS_DISTANCE) );
                if (m_sIntvButtonLabel[iB].IsEmpty()) {
                    m_pAnswerButton[iB]->Show(false);
                    m_pAnswerButton[iB]->Enable(false);
                }
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
    iB = 0;
    pButton = new wxButton( this, ID_BUTTON + iB, m_sIntvButtonLabel[iB] );
    m_pAnswerButton[iB] = pButton;
    pUnisonSizer->Add(
        pButton,
        wxSizerFlags(0).Border(wxALL, BUTTONS_DISTANCE) );

        //two additional buttons for "chromatic semitone" and "Prime double augmented"
    iB = 48;
    m_pAnswerButton[iB] = new wxButton( this, ID_BUTTON + iB, m_sIntvButtonLabel[iB] );
    pUnisonSizer->Add(
        m_pAnswerButton[iB],
        wxSizerFlags(0).Border(wxALL, BUTTONS_DISTANCE) );

    //iB = 49;
    //pButton = new wxButton( this, ID_BUTTON + iB, m_sIntvButtonLabel[iB] );
    //m_pAnswerButton[iB] = pButton;
    //pUnisonSizer->Add(
    //    pButton,
    //    wxSizerFlags(0).Border(wxALL, BUTTONS_DISTANCE) );



    SetSizer( pMainSizer );                // use the sizer for window layout
    pMainSizer->SetSizeHints( this );        // set size hints to honour minimum size

    NewProblem();
}

lmTheoIntervalsCtrol::~lmTheoIntervalsCtrol()
{
    if (m_pScoreCtrol) {
        delete m_pScoreCtrol;
        m_pScoreCtrol = (lmScoreAuxCtrol*)NULL;
    }

    if (m_pConstrains) {
        delete m_pConstrains;
        m_pConstrains = (lmTheoIntervalsConstrains*) NULL;
    }

    if (m_pScore) {
        delete m_pScore;
        m_pScore = (lmScore*)NULL;
    }
}

void lmTheoIntervalsCtrol::EnableButtons(bool fEnable)
{
    for (int iB=0; iB < NUM_BUTTONS; iB++) {
        if (!m_sIntvButtonLabel[iB].IsEmpty())
            m_pAnswerButton[iB]->Enable(fEnable);
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
    //! @todo Sound for failure
    m_pCounters->IncrementWrong();
    DisplaySolution();
    EnableButtons(false);           //student must not give now the answer
}

void lmTheoIntervalsCtrol::OnRespButton(wxCommandEvent& event)
{
    int nIndex = event.GetId() - ID_BUTTON;

    //buttons are only enabled in m_fIntervalKnow type problems
    bool fSuccess;
    wxColour* pColor;
    
    //verify if success or failure
    if (!m_fIntervalKnown) return;
    fSuccess = (nIndex == m_nRespIndex);
    
    //prepare sound and color, and update counters
    if (fSuccess) {
        pColor = &(g_pColors->Success());
        //! @todo Sound for sucess
        m_pCounters->IncrementRight();
    } else {
        pColor = &(g_pColors->Failure());
        //! @todo Sound for failure
        m_pCounters->IncrementWrong();
    }
        
    //if failure, display the solution. If succsess, generate a new problem
    if (!fSuccess) {
        //failure: mark wrong button in red and right one in green
        m_pAnswerButton[m_nRespIndex]->SetBackgroundColour(g_pColors->Success());
        m_pAnswerButton[nIndex]->SetBackgroundColour(g_pColors->Failure());

        //show the solucion
        DisplaySolution();
        EnableButtons(false);

    } else {
        NewProblem();
    }
    
}

void lmTheoIntervalsCtrol::NewProblem()
{

    ResetExercise();

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
    EClefType nClef = oGenerator.GenerateClef(m_pConstrains->GetClefConstrains());
    m_ntPitch[0] = oGenerator.GenerateRandomPitch(0, 8, false, nClef);
    m_ntPitch[1] = oGenerator.GenerateRandomPitch(0, 8, false, nClef);
    //while (m_ntPitch[0] == m_ntPitch[1]) {
    //    m_ntPitch[1] = oGenerator.GenerateRandomPitch(0, 8, false, nClef);
    //}
    
    //Decide accidentals
    wxString sPatron[2], sAlter[2];
    lmConverter oConv;
    for (int i=0; i < 2; i++) {
        sAlter[i] = _T("");
        if (m_pConstrains->GetAccidentals() && oGenerator.FlipCoin() ) {
            sAlter[i] = (oGenerator.FlipCoin() ? _T("-") : _T("+"));
        }
    }

    //remove two accidentals in unison
    if (m_ntPitch[0] == m_ntPitch[1] && sAlter[0] != _T("") && sAlter[1] != _T("")) {
        sAlter[1] = _T("");
    }

    //prepare LDP pattern
    for (int i=0; i < 2; i++) {
        sPatron[i] = _T("(n ");
        sPatron[i] += sAlter[i];
        sPatron[i] += oConv.GetEnglishNoteName(m_ntPitch[i]) + _T(" r)");
    }
    
    ////DEBUG: un-comment and modify values for testing a certain interval
    //sPatron[0] = _T("(n -e4 r)");
    //sPatron[1] = _T("(n e4 r)");

    //create the score
    lmNote* pNote[2];
    lmLDPParser parserLDP;
    lmLDPNode* pNode;
    m_pScore = new lmScore();
    m_pScore->SetTopSystemDistance( lmToLogicalUnits(5, lmMILLIMETERS) );   //5mm
    m_pScore->AddInstrument(1,0,0);                    //one vstaff, MIDI channel 0, MIDI instr 0
    lmVStaff *pVStaff = m_pScore->GetVStaff(1, 1);    //get first vstaff of instr.1
    pVStaff->AddClef( nClef );
    pVStaff->AddKeySignature(0, true);                    // 0 fifths, major
    pVStaff->AddTimeSignature(4 ,4, sbNO_VISIBLE );
//    pVStaff->AddEspacio 24
    pNode = parserLDP.ParseText( sPatron[0] );
    pNote[0] = parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddBarline(etb_SimpleBarline, sbNO_VISIBLE);    //so that accidental doesn't affect 2nd note
    pNode = parserLDP.ParseText( sPatron[1] );
    pNote[1] = parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddBarline(etb_EndBarline, sbNO_VISIBLE);

    //compute the right answer
    lmInterval oIntv(pNote[0], pNote[1], earmDo);
    m_sAnswer = oIntv.GetName() + (oIntv.IsAscending() ? _(", ascending") :
                                                         _(", descending") );
    m_ntMidi[0] = oIntv.GetMidiNote1();
    m_ntMidi[1] = oIntv.GetMidiNote2();
    
    //compute the index for the button that corresponds to the right answer
    switch (oIntv.GetType()) {
        case eti_DoubleDiminished:    //row 1
            m_nRespIndex = oIntv.GetInterval() - 1;
            break;                
        case eti_Diminished:        //row 2
            m_nRespIndex = oIntv.GetInterval() + 7;
            break;                
        case eti_Minor:                //row 3
            m_nRespIndex = oIntv.GetInterval() + 15;
            break;                
        case eti_Major:
        case eti_Perfect:            //row 4
            m_nRespIndex = oIntv.GetInterval() + 23;
            break;                
        case eti_Augmented:            //row 5
            m_nRespIndex = oIntv.GetInterval() + 31;
            break;                
        case eti_DoubleAugmented:    //row 6
            m_nRespIndex = oIntv.GetInterval() + 39;
            break;                
        default:
            wxASSERT(false);
    }
    //special cases: unison and related
    if (oIntv.GetInterval() == 1) {
        switch (oIntv.GetType()) {
            case eti_Perfect:
                m_nRespIndex = 0;       //unison
                break;                
            case eti_Augmented:
                m_nRespIndex = 48;      //chromatic semitone
                break;                
            case eti_DoubleAugmented:
                m_nRespIndex = 49;      //prime double augmented
                break;                
            default:
                wxASSERT(false);
        }
    }

    //wxLogMessage(wxString::Format(
    //    _T("[lmTheoIntervalsCtrol::NewProblem] m_nRespIndex=%d, oIntv.GetInterval()=%d"),
    //    m_nRespIndex, oIntv.GetInterval() ));
    
    //display the problem
    if (m_fIntervalKnown) {
        //direct problem
        m_pScoreCtrol->DisplayScore(m_pScore);
        m_pScore = (lmScore*)NULL;    //no longer owned. Now owned by lmScoreAuxCtrol
        EnableButtons(true);
        m_pPlayButton->Enable(true);
    } else {
        //inverse problem
        m_sAnswer += _(" starting at ") + oConv.GetNoteName(m_ntPitch[0]);
        if (sAlter[0] == _T("+")) m_sAnswer += _T(" #");
        if (sAlter[0] == _T("-")) m_sAnswer += _T(" b");
        m_pScoreCtrol->DisplayMessage(m_sAnswer, lmToLogicalUnits(5, lmMILLIMETERS));
        m_pPlayButton->Enable(false);
    }
    m_fPlayEnabled = false;
    m_fProblemCreated = true;

    SetUpButtons();
    
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
    if (m_fIntervalKnown) {
        m_pScoreCtrol->DisplayMessage(m_sAnswer, lmToLogicalUnits(5, lmMILLIMETERS), false);
        m_pAnswerButton[m_nRespIndex]->SetBackgroundColour(g_pColors->Success());
    } else {
        m_pScoreCtrol->DisplayScore(m_pScore, false);
        m_pScore = (lmScore*)NULL;    //no longer owned. Now owned by lmScoreAuxCtrol
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
    for (int iB=0; iB < NUM_BUTTONS; iB++) {
        if (!m_sIntvButtonLabel[iB].IsEmpty()) {
            m_pAnswerButton[iB]->SetBackgroundColour( g_pColors->Normal() );
        }
    }
    EnableButtons(false);

    if (m_pScore) {
        delete m_pScore;
        m_pScore = (lmScore*)NULL;
    }
    
}

void lmTheoIntervalsCtrol::SetUpButtons()
{
    int iB;
    const int NUM_ROWS = 6;
    const int NUM_COLS = 8;
    for (int iRow=0; iRow < 5; iRow++) {
        for (int iCol=0; iCol < NUM_COLS; iCol++) {
            iB = iCol + iRow * NUM_COLS;    // button index: 0 .. 47            
            if (iCol != 0) {
                m_pAnswerButton[iB]->Show(true);
                m_pAnswerButton[iB]->SetLabel(m_sNotesButtonLabel[iB]);
                m_pAnswerButton[iB]->Enable(true);
            }
        }
    }
    for (int iRow=5; iRow < NUM_ROWS; iRow++) {
        for (int iCol=0; iCol < NUM_COLS; iCol++) {
            iB = iCol + iRow * NUM_COLS;    // button index: 0 .. 47            
            if (iCol != 0) {
                m_pAnswerButton[iB]->Show(false);
                m_pAnswerButton[iB]->Enable(false);
            }
        }
    }

}
