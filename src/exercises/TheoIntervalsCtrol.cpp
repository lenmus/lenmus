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
const int NUM_BUTTONS = 48;                //buttons for answers
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

    wxString sButtonLabel[NUM_BUTTONS];
    sButtonLabel[0] = _("Unison");
    sButtonLabel[1] = _("dd2");
    sButtonLabel[2] = _("dd3");
    sButtonLabel[3] = _("dd4");
    sButtonLabel[4] = _("dd5");
    sButtonLabel[5] = _("dd5");
    sButtonLabel[6] = _("dd7");
    sButtonLabel[7] = _("dd8");
    sButtonLabel[8] = _T("");
    sButtonLabel[9] = _("dim2");
    sButtonLabel[10] = _("dim3");
    sButtonLabel[11] = _("dim4");
    sButtonLabel[12] = _("dim5");
    sButtonLabel[13] = _("dim6");
    sButtonLabel[14] = _("dim7");
    sButtonLabel[15] = _("dim8");
    sButtonLabel[16] = _T("");
    sButtonLabel[17] = _("m2");
    sButtonLabel[18] = _("m3");
    sButtonLabel[19] = _T("");
    sButtonLabel[20] = _T("");
    sButtonLabel[21] = _("m6");
    sButtonLabel[22] = _("m7");
    sButtonLabel[23] = _T("");
    sButtonLabel[24] = _T("");
    sButtonLabel[25] = _("M2");
    sButtonLabel[26] = _("M3");
    sButtonLabel[27] = _("P4");
    sButtonLabel[28] = _("P5");
    sButtonLabel[29] = _("M6");
    sButtonLabel[30] = _("M7");
    sButtonLabel[31] = _("P8");
    sButtonLabel[32] = _T("");
    sButtonLabel[33] = _("aug2");
    sButtonLabel[34] = _("aug3");
    sButtonLabel[35] = _("aug4");
    sButtonLabel[36] = _("aug5");
    sButtonLabel[37] = _("aug6");
    sButtonLabel[38] = _("aug7");
    sButtonLabel[39] = _("aug8");
    sButtonLabel[40] = _T("");
    sButtonLabel[41] = _("da2");
    sButtonLabel[42] = _("da3");
    sButtonLabel[43] = _("da4");
    sButtonLabel[44] = _("da5");
    sButtonLabel[45] = _("da6");
    sButtonLabel[46] = _("da7");
    sButtonLabel[47] = _("da7");


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
    
    //create 48 buttons for the answers: six rows, eight buttons per row
    
    //unison button
    wxBoxSizer* pUnisonSizer = new wxBoxSizer( wxHORIZONTAL );
    pMainSizer->Add(
        pUnisonSizer,
        wxSizerFlags(0).Left() );

    wxButton* pButton;
    int iB = 0;
    pButton = new wxButton( this, ID_BUTTON, _("Unison") );
    m_pAnswerButton[iB++] = pButton;
    pUnisonSizer->Add(
        pButton,
        wxSizerFlags(0).Border(wxALL, BUTTONS_DISTANCE) );


    // labels for columns, with interval number
    wxBoxSizer* pRowSizer = new wxBoxSizer( wxHORIZONTAL );
    pMainSizer->Add(    
        pRowSizer,
        wxSizerFlags(0).Left());

    pRowSizer->Add(15+BUTTONS_DISTANCE, 24, 0);    //spacer to center labels
    for (int iRow=0; iRow < 7; iRow++) {        // six rows
        pRowSizer->Add(
            new wxStaticText(this, -1, sColumnLabel[iRow],
                        wxDefaultPosition, wxSize(54, 15)),
            wxSizerFlags(0).Left().Border(wxRIGHT | wxLEFT, BUTTONS_DISTANCE) );
    }

    //remaining buttons
    for (int iRow=0; iRow < 6; iRow++) {        // six rows
        pRowSizer = new wxBoxSizer( wxHORIZONTAL );
        pMainSizer->Add(    
            pRowSizer,
            wxSizerFlags(0).Left());

        for (int iCol=0; iCol < 8; iCol++) {        //eight columns
            iB = iCol + iRow * 8;    // button index: 0 .. 47            
            //column (unisons) is not created. Button for unison already created
            if (iCol != 0) {
                if (sButtonLabel[iB].IsEmpty()) {
                    pRowSizer->Add(54+BUTTONS_DISTANCE+BUTTONS_DISTANCE, 24, 0);    //spacer
                }
                else {
                    pButton = new wxButton( this, ID_BUTTON + iB, sButtonLabel[iB],
                        wxDefaultPosition, wxSize(54, 20));
                    m_pAnswerButton[iB++] = pButton;
                    pRowSizer->Add(
                        pButton,
                        wxSizerFlags(0).Border(wxALL, BUTTONS_DISTANCE) );
                }
            }
        }

        pRowSizer->Add(
            new wxStaticText(this, -1, sRowLabel[iRow] ),
            wxSizerFlags(0).Left().Border(wxALL, 10) );

    }

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
    for (int i=0; i < NUM_BUTTONS; i++) {
        if (m_pAnswerButton[i])
            m_pAnswerButton[i]->Enable(fEnable);
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
    DisplaySolution();
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
    switch (m_pConstrains->GetProblemType())
    {
        case ePT_DeduceInterval:
            m_fIntervalKnown = true;
            break;
        case ePT_BuildInterval:
            m_fIntervalKnown = false;
            break;
        case ePT_Both:
            m_fIntervalKnown = ((rand() % 2) == 0);
            break;
    }
    
    //Generate two random note-pos in range -1 to 7 (from two ledge lines down to two up)
    lmRandomGenerator oGenerator;
    EClefType nClef = oGenerator.GenerateClef(m_pConstrains->GetClefConstrains());
    m_ntPitch[0] = oGenerator.GenerateRandomPitch(0, 8, false, nClef);
    m_ntPitch[1] = oGenerator.GenerateRandomPitch(0, 8, false, nClef);
    while (m_ntPitch[0] == m_ntPitch[1]) {
        m_ntPitch[1] = oGenerator.GenerateRandomPitch(0, 8, false, nClef);
    }
    
    //Convert problem to LDP pattern
    wxString sPatron[2], sAlter[2];
    lmConverter oConv;
    for (int i=0; i < 2; i++) {
        sPatron[i] = _T("(n ");
        sAlter[i] = _T("");
        if (m_pConstrains->GetAccidentals() && ((rand() % 2) == 0) ) {
            sAlter[i] = ((rand() % 2) == 0 ? _T("-") : _T("+"));
            sPatron[i] += sAlter[i];
        }
        sPatron[i] += oConv.GetEnglishNoteName(m_ntPitch[i]) + _T(" r)");
    }
    
    //sPatron[0] = _T("(n +e4 r)");
    //sPatron[1] = _T("(n f4 r)");

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
    pVStaff->AddBarline(etbBarraNormal, sbNO_VISIBLE);    //so that accidental doesn't affect 2nd note
    pNode = parserLDP.ParseText( sPatron[1] );
    pNote[1] = parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddBarline(etbBarraFinal, sbNO_VISIBLE);

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
    
    //! @todo Piano feedback
    //if (FMain.fFrmPiano) FPiano.DesmarcarTeclas

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
    } else {
        m_pScoreCtrol->DisplayScore(m_pScore, false);
        m_pScore = (lmScore*)NULL;    //no longer owned. Now owned by lmScoreAuxCtrol
    }
    
    //if piano visile, mark the notes
    //! @todo piano feedback
////    if (FMain.fFrmPiano) {
////        With FPiano
////            .HabilitarMarcado = true
////            .MarcarTecla m_ntMidi[0]
////            .MarcarTecla m_ntMidi[1]
////            .HabilitarMarcado = false
////        End With
////    }

    m_pPlayButton->Enable(true);
    m_fPlayEnabled = true;
    m_fProblemCreated = false;
    EnableButtons(false);           //student must not give now the answer
    
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
    for (int i=0; i < NUM_BUTTONS; i++) {
        if (m_pAnswerButton[i]) {
            m_pAnswerButton[i]->SetBackgroundColour( g_pColors->Normal() );
        }
    }
    EnableButtons(false);

    if (m_pScore) {
        delete m_pScore;
        m_pScore = (lmScore*)NULL;
    }
    
}
