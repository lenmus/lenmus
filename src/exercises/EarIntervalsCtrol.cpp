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
/*! @file EarIntervalsCtrol.cpp
    @brief Implementation file for class lmEarIntervalsCtrol
    @ingroup html_controls
*/
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "EarIntervalsCtrol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "EarIntervalsCtrol.h"
#include "Constrains.h"
#include "Generators.h"
#include "../auxmusic/Conversion.h"

#include "../ldp_parser/LDPParser.h"
#include "../auxmusic/Interval.h"
#include "../app/DlgCfgEarIntervals.h"


#include "../globals/Colors.h"
extern lmColors* g_pColors;

// access to global external variables
extern bool g_fReleaseVersion;            // in TheApp.cpp
extern bool g_fReleaseBehaviour;          // in TheApp.cpp
extern bool g_fShowDebugLinks;            // in TheApp.cpp

//------------------------------------------------------------------------------------
// Implementation of lmEarIntervalsCtrol




//Layout definitions
const int BUTTONS_DISTANCE    = 5;        //pixels
const int NUM_LINKS = 3;                //links for actions

static wxString sBtLabel[lmEAR_INVAL_NUM_BUTTONS];


//IDs for controls
enum {
    ID_LINK_SEE_SOURCE = 3000,
    ID_LINK_DUMP,
    ID_LINK_MIDI_EVENTS,
    ID_BUTTON,
    ID_LINK = ID_BUTTON + lmEAR_INVAL_NUM_BUTTONS,
    ID_LINK_NEW_PROBLEM,
    ID_LINK_PLAY,
    ID_LINK_SOLUTION,
    ID_LINK_SETTINGS,
};


BEGIN_EVENT_TABLE(lmEarIntervalsCtrol, wxWindow)
    EVT_COMMAND_RANGE (ID_BUTTON, ID_BUTTON+lmEAR_INVAL_NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmEarIntervalsCtrol::OnRespButton)
    EVT_SIZE            (lmEarIntervalsCtrol::OnSize)

    LM_EVT_URL_CLICK    (ID_LINK_SEE_SOURCE, lmEarIntervalsCtrol::OnDebugShowSourceScore)
    LM_EVT_URL_CLICK    (ID_LINK_DUMP, lmEarIntervalsCtrol::OnDebugDumpScore)
    LM_EVT_URL_CLICK    (ID_LINK_MIDI_EVENTS, lmEarIntervalsCtrol::OnDebugShowMidiEvents)

    LM_EVT_URL_CLICK    (ID_LINK_NEW_PROBLEM, lmEarIntervalsCtrol::OnNewProblem)
    LM_EVT_URL_CLICK    (ID_LINK_PLAY, lmEarIntervalsCtrol::OnPlay)
    LM_EVT_URL_CLICK    (ID_LINK_SOLUTION, lmEarIntervalsCtrol::OnDisplaySolution)
    LM_EVT_URL_CLICK    (ID_LINK_SETTINGS, lmEarIntervalsCtrol::OnSettingsButton)
END_EVENT_TABLE()

IMPLEMENT_CLASS(lmEarIntervalsCtrol, wxWindow)

lmEarIntervalsCtrol::lmEarIntervalsCtrol(wxWindow* parent, wxWindowID id, 
                           lmEarIntervalsConstrains* pConstrains,
                           const wxPoint& pos, const wxSize& size, int style)
    : wxWindow(parent, id, pos, size, style )
{
    //initializations
    int i;
    for (i=0; i < lmEAR_INVAL_NUM_BUTTONS; i++) { m_pAnswerButton[i] = (wxButton*)NULL; }
    m_nRespIndex = 0;
    m_fProblemCreated = false;
    m_fPlayEnabled = false;
    m_pScore = (lmScore*)NULL;
    m_pScoreCtrol = (lmScoreAuxCtrol*)NULL;
    m_pConstrains = pConstrains;

    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    sBtLabel[0] = _("Unison");
    sBtLabel[1] = _("minor 2nd");
    sBtLabel[2] = _("major 2nd");
    sBtLabel[3] = _("minor 3rd");
    sBtLabel[4] = _("major 3rd");
    sBtLabel[5] = _("4th perfect");
    sBtLabel[6] = _("aug.4th/dim.5th");
    sBtLabel[7] = _("perfect 5th");
    sBtLabel[8] = _("minor 6th");
    sBtLabel[9] = _("major 6th");
    sBtLabel[10] = _("minor 7th");
    sBtLabel[11] = _("major 7th");
    sBtLabel[12] = _("perfect 8th");
    sBtLabel[13] = _("minor 9th");
    sBtLabel[14] = _("major 9th");
    sBtLabel[15] = _("minor 10th");
    sBtLabel[16] = _("major 10th");
    sBtLabel[17] = _("perfect 11th");
    sBtLabel[18] = _("aug.11th / dim.12th");
    sBtLabel[19] = _("perfect 12th");
    sBtLabel[20] = _("minor 13th");
    sBtLabel[21] = _("major 13th");
    sBtLabel[22] = _("minor 14th");
    sBtLabel[23] = _("major 14th");
    sBtLabel[24] = _("two octaves");


        // create the controls

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
    m_pScoreCtrol = new lmScoreAuxCtrol(this, -1, m_pScore, wxDefaultPosition, wxSize(360,150), eSIMPLE_BORDER);
    m_pScoreCtrol->SetMargins(lmToLogicalUnits(10, lmMILLIMETERS),      //left=1cm
                              lmToLogicalUnits(10, lmMILLIMETERS),      //right=1cm
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

        //
        //links 
        //

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
    m_pShowSolution = new lmUrlAuxCtrol(this, ID_LINK_SOLUTION, _("Show solution") );
    pLinksSizer->Add(
        m_pShowSolution,
        wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT|wxBOTTOM, 20) );
    

    //create up to 25 buttons for the answers: five rows, five buttons per row
    //Buttons are created disabled and no visible
    wxButton* pButton;
    wxBoxSizer* pRowSizer;
    int iB = 0;
    const int NUM_ROWS = 5;
    const int NUM_COLS = 5;
    for (int iRow=0; iRow < NUM_ROWS; iRow++) {
        pRowSizer = new wxBoxSizer( wxHORIZONTAL );

        for (int iCol=0; iCol < NUM_COLS; iCol++) {
            iB = iCol + iRow * NUM_COLS;    // button index: 0 .. 24         
            pButton = new wxButton( this, ID_BUTTON + iB, _T("provisional"),
                wxDefaultPosition, wxSize(100, 24));
            m_pAnswerButton[iB++] = pButton;
            pRowSizer->Add(
                pButton,
                wxSizerFlags(0).Border(wxTOP|wxLEFT|wxRIGHT, BUTTONS_DISTANCE) );
        }
        pMainSizer->Add(    
            pRowSizer,
            wxSizerFlags(0).Left());
    }

    SetSizer( pMainSizer );                // use the sizer for window layout
    pMainSizer->SetSizeHints( this );        // set size hints to honour minimum size


    // now put labels and enable the answer buttons
    SetUpButtons();

    m_pScoreCtrol->DisplayMessage(_("Click on 'New problem' to start"), 
                                  lmToLogicalUnits(10, lmMILLIMETERS),
                                  true);

}

lmEarIntervalsCtrol::~lmEarIntervalsCtrol()
{
    if (m_pScoreCtrol) {
        delete m_pScoreCtrol;
        m_pScoreCtrol = (lmScoreAuxCtrol*)NULL;
    }

    if (m_pConstrains) {
        delete m_pConstrains;
        m_pConstrains = (lmEarIntervalsConstrains*) NULL;
    }

    if (m_pScore) {
        delete m_pScore;
        m_pScore = (lmScore*)NULL;
    }
}

void lmEarIntervalsCtrol::SetUpButtons()
{
    int i;

    // compute interval associated to each button
    int j = 0;
    for (i=0; i < lmEAR_INVAL_NUM_BUTTONS; i++) {
        if (m_pConstrains->IsIntervalAllowed(i)) {
            m_nRealIntval[j] = i;
            j++;
        }
    }
    m_nValidIntervals = j;


    //set up the button labels
    int iB;     // button index: 0 .. 24 
    for (iB = 0; iB < m_nValidIntervals; iB++) {
        m_pAnswerButton[iB]->SetLabel( sBtLabel[m_nRealIntval[iB]] );
        m_pAnswerButton[iB]->Show(true);
        m_pAnswerButton[i]->Enable(false);
    }

    // hide all non used buttons
    if (m_nValidIntervals < lmEAR_INVAL_NUM_BUTTONS) {
        for (iB = m_nValidIntervals; iB < lmEAR_INVAL_NUM_BUTTONS; iB++) {
            m_pAnswerButton[iB]->Show(false);
        }
    }

    m_pPlayButton->Enable(false);
    m_pShowSolution->Enable(false);

}

void lmEarIntervalsCtrol::EnableButtons(bool fEnable)
{
    for (int i=0; i < lmEAR_INVAL_NUM_BUTTONS; i++) {
        if (m_pAnswerButton[i])
            m_pAnswerButton[i]->Enable(fEnable);
    }
    m_fButtonsEnabled = fEnable;

    m_pPlayButton->Enable(fEnable);
    m_pShowSolution->Enable(fEnable);

}

//----------------------------------------------------------------------------------------
// Event handlers

void lmEarIntervalsCtrol::OnSettingsButton(wxCommandEvent& event)
{
    lmDlgCfgEarIntervals dlg(this, m_pConstrains );
    int retcode = dlg.ShowModal();
    if (retcode == wxID_OK) {
        m_pConstrains->SaveSettings();
        // When changing interval settings it is necessary review the buttons
        // as number of buttons and/or its name could have changed.
        SetUpButtons();
    }

}

void lmEarIntervalsCtrol::OnSize(wxSizeEvent& event)
{
    //wxLogMessage(_T("OnSize at lmEarIntervalsCtrol"));
    Layout();

}

void lmEarIntervalsCtrol::OnPlay(wxCommandEvent& event)
{
    Play();
}

void lmEarIntervalsCtrol::OnNewProblem(wxCommandEvent& event)
{
    NewProblem();
}

void lmEarIntervalsCtrol::OnDisplaySolution(wxCommandEvent& event)
{
    m_pCounters->IncrementWrong();
    DisplaySolution();
    EnableButtons(false);
}

void lmEarIntervalsCtrol::OnRespButton(wxCommandEvent& event)
{
    int nIndex = event.GetId() - ID_BUTTON;

    bool fSuccess;
    wxColour* pColor;
    
    //verify if success or failure
    fSuccess = (nIndex == m_nRespIndex);
    
    //prepare sound and color, and update counters
    if (fSuccess) {
        pColor = &(g_pColors->Success());
        m_pCounters->IncrementRight();
    } else {
        pColor = &(g_pColors->Failure());
        m_pCounters->IncrementWrong();
    }
        
    //if failure, display the solution. If succsess, generate a new problem
    if (!fSuccess) {
        //failure: mark wrong button in red and right one in green
        m_pAnswerButton[m_nRespIndex]->SetBackgroundColour(g_pColors->Success());
        m_pAnswerButton[nIndex]->SetBackgroundColour(g_pColors->Failure());

        //show the solucion
        DisplaySolution();
        EnableButtons(true);

    } else {
        NewProblem();
    }
    
}

void lmEarIntervalsCtrol::NewProblem()
{

    ResetExercise();
    
    //
    //generate the problem interval
    //

    EClefType nClef = eclvSol;
    // select interval type: ascending, descending or both
    EIntervalDirection nDir;
    if (m_pConstrains->IsTypeAllowed(0) || 
        (m_pConstrains->IsTypeAllowed(1) && m_pConstrains->IsTypeAllowed(2)))
    {
        // if harmonic scale or melodic ascending and descending, allow for 
        // both types of intervals: ascending and descending
        nDir = edi_Both;
    }
    else if (m_pConstrains->IsTypeAllowed(1)) {
        // if melodic ascendig, allow only ascending intervals
        nDir = edi_Ascending;
    }
    else {
        // allow only descending intervals
        nDir = edi_Descending;
    }
    // select a random key signature satisfying the constrains
    lmRandomGenerator oGenerator;
    EKeySignatures nKey;
    if (m_pConstrains->OnlyNatural()) {
        nKey = oGenerator.GenerateKey(m_pConstrains->GetKeyConstrains());
    }
    else {
        nKey = earmDo;
    }
    // generate interval
    lmInterval oIntv(m_pConstrains->OnlyNatural(), m_pConstrains->MinNote(),
        m_pConstrains->MaxNote(), m_pConstrains->AllowedIntervals(), nDir, nKey);

    //Convert problem to LDP pattern
    wxString sPattern[2];
    int i;
    for (i=0; i < 2; i++) {
        sPattern[i] = _T("(n ") + oIntv.GetPattern(i) + _T(" r)");
    }
    
    //create the score
    lmNote* pNote[2];
    lmLDPParser parserLDP;
    lmLDPNode* pNode;
    m_pScore = new lmScore();
    m_pScore->SetTopSystemDistance( lmToLogicalUnits(5, lmMILLIMETERS) );   //5mm
    m_pScore->AddInstrument(1,0,0);                     //one vstaff, MIDI channel 0, MIDI instr 0
    lmVStaff *pVStaff = m_pScore->GetVStaff(1, 1);      //get first vstaff of instr.1
    pVStaff->AddClef( nClef );
    pVStaff->AddKeySignature(nKey);
    pVStaff->AddTimeSignature(4 ,4, sbNO_VISIBLE );
//    pVStaff->AddEspacio 24
    pNode = parserLDP.ParseText( sPattern[0] );
    pNote[0] = parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddBarline(etb_SimpleBarline, sbNO_VISIBLE);    //so that accidental doesn't affect 2nd note
    pNode = parserLDP.ParseText( sPattern[1] );
    pNote[1] = parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddBarline(etb_EndBarline, sbNO_VISIBLE);

    //compute the right answer
    m_sAnswer = oIntv.GetName();

    m_ntMidi[0] = oIntv.GetMidiNote1();
    m_ntMidi[1] = oIntv.GetMidiNote2();
    
    //compute the index for the button that corresponds to the right answer
    for (i = 0; i <= m_nValidIntervals; i++) {
        if (m_nRealIntval[i] == oIntv.GetNumSemitones()) break;
    }
    m_nRespIndex = i;
    
    //play the interval
    m_pScoreCtrol->SetScore(m_pScore, true);            //true: the score must be hidden
    m_pScoreCtrol->DisplayMessage(_T(""), 0, true);     //true: clear the canvas
    Play();

    m_fPlayEnabled = true;
    m_fProblemCreated = true;
    EnableButtons(true);
    
}

void lmEarIntervalsCtrol::Play()
{
    //@attention As the interval is built using whole notes, we will play it at MM=240 so
    //that real note rate will be 80.
    m_pScoreCtrol->PlayScore(lmVISUAL_TRACKING, NO_MARCAR_COMPAS_PREVIO, 
                             ePM_NormalInstrument, 320);
}

void lmEarIntervalsCtrol::DisplaySolution()
{
    m_pScoreCtrol->HideScore(false);
    m_pScoreCtrol->DisplayMessage(m_sAnswer, lmToLogicalUnits(5, lmMILLIMETERS), false);
    m_fPlayEnabled = true;
    m_fProblemCreated = false;
    
}

void lmEarIntervalsCtrol::OnDebugShowSourceScore(wxCommandEvent& event)
{
    m_pScoreCtrol->SourceLDP();
}

void lmEarIntervalsCtrol::OnDebugDumpScore(wxCommandEvent& event)
{
    m_pScoreCtrol->Dump();
}

void lmEarIntervalsCtrol::OnDebugShowMidiEvents(wxCommandEvent& event)
{
    m_pScoreCtrol->DumpMidiEvents();
}

void lmEarIntervalsCtrol::ResetExercise()
{
    for (int i=0; i < lmEAR_INVAL_NUM_BUTTONS; i++) {
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
