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
/*! @file EarChordCtrol.cpp
    @brief Implementation file for class lmEarChordCtrol
    @ingroup html_controls
*/
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "EarChordCtrol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "EarChordCtrol.h"
#include "UrlAuxCtrol.h"
#include "Constrains.h"
#include "Generators.h"
#include "../auxmusic/Conversion.h"

#include "../ldp_parser/LDPParser.h"
#include "../auxmusic/Interval.h"
#include "../app/DlgCfgEarIntervals.h"
#include "../auxmusic/ChordManager.h"


#include "../globals/Colors.h"
extern lmColors* g_pColors;

// access to global external variables
extern bool g_fReleaseVersion;            // in TheApp.cpp
extern bool g_fReleaseBehaviour;        // in TheApp.cpp
extern bool g_fShowDebugLinks;            // in TheApp.cpp

//------------------------------------------------------------------------------------
// Implementation of lmEarChordCtrol




//Layout definitions
const int BUTTONS_DISTANCE = 5;        //pixels
const int NUM_BUTTONS = 10;
const int NUM_ROWS = 2;
const int NUM_COLS = 5;

static wxString m_sButtonLabel[NUM_BUTTONS];
static wxString m_sRowLabel[NUM_ROWS];

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


BEGIN_EVENT_TABLE(lmEarChordCtrol, wxWindow)
    EVT_COMMAND_RANGE (ID_BUTTON, ID_BUTTON+NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmEarChordCtrol::OnRespButton)
    EVT_SIZE            (lmEarChordCtrol::OnSize)

    LM_EVT_URL_CLICK    (ID_LINK_SEE_SOURCE, lmEarChordCtrol::OnDebugShowSourceScore)
    LM_EVT_URL_CLICK    (ID_LINK_DUMP, lmEarChordCtrol::OnDebugDumpScore)
    LM_EVT_URL_CLICK    (ID_LINK_MIDI_EVENTS, lmEarChordCtrol::OnDebugShowMidiEvents)

    LM_EVT_URL_CLICK    (ID_LINK_NEW_PROBLEM, lmEarChordCtrol::OnNewProblem)
    LM_EVT_URL_CLICK    (ID_LINK_PLAY, lmEarChordCtrol::OnPlay)
    LM_EVT_URL_CLICK    (ID_LINK_SOLUTION, lmEarChordCtrol::OnDisplaySolution)
    LM_EVT_URL_CLICK    (ID_LINK_SETTINGS, lmEarChordCtrol::OnSettingsButton)
END_EVENT_TABLE()

IMPLEMENT_CLASS(lmEarChordCtrol, wxWindow)

lmEarChordCtrol::lmEarChordCtrol(wxWindow* parent, wxWindowID id, 
                           lmEarChordConstrains* pConstrains,
                           const wxPoint& pos, const wxSize& size, int style)
    : wxWindow(parent, id, pos, size, style )
{
    //initializations
    int i;
    for (i=0; i < NUM_BUTTONS; i++) { m_pAnswerButton[i] = (wxButton*)NULL; }
    m_fProblemCreated = false;
    m_fPlayEnabled = false;
    m_pChordScore = (lmScore*)NULL;
    m_pScoreCtrol = (lmScoreAuxCtrol*)NULL;
    m_pConstrains = pConstrains;

    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated

        // button row labels
    m_sRowLabel[0] = _("Triads:");
    m_sRowLabel[1] = _("Seventh chords:");

        //button labels.
    m_sButtonLabel[0] = _("Major");
    m_sButtonLabel[1] = _("Minor");
    m_sButtonLabel[2] = _("Augmented");
    m_sButtonLabel[3] = _("Diminished");
    m_sButtonLabel[4] = _("Suspended");

    m_sButtonLabel[5] = _("Dominant 7th");
    m_sButtonLabel[6] = _("Major 7th");
    m_sButtonLabel[7] = _("Minor 7th");
    m_sButtonLabel[8] = _("Diminished 7th");
    m_sButtonLabel[9] = _("Half dim. 7th");


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
    m_pScoreCtrol = new lmScoreAuxCtrol(this, -1, (lmScore*)NULL, wxDefaultPosition, wxSize(320,150), eSIMPLE_BORDER);
    m_pScoreCtrol->SetMargins(lmToLogicalUnits(5, lmMILLIMETERS),      //left=1cm
                              lmToLogicalUnits(5, lmMILLIMETERS),      //right=1cm
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
    

    //create buttons for the answers, two rows
    int iB = 0;

    wxFlexGridSizer* pKeyboardSizer = new wxFlexGridSizer(NUM_ROWS+1, NUM_COLS+1, 10, 0);
    pMainSizer->Add(
        pKeyboardSizer,
        wxSizerFlags(0).Left().Border(wxALIGN_LEFT|wxTOP, 10) );

    for (int iRow=0; iRow < NUM_ROWS; iRow++) {
        pKeyboardSizer->Add(
            new wxStaticText(this, -1, m_sRowLabel[iRow]),
            wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, BUTTONS_DISTANCE) );

        // the buttons for this row
        for (int iCol=0; iCol < NUM_COLS; iCol++) {
            iB = iCol + iRow * NUM_COLS;    // button index: 0 .. 9            
            m_pAnswerButton[iB] = new wxButton( this, ID_BUTTON + iB, m_sButtonLabel[iB],
                wxDefaultPosition, wxSize(80, 20));
            pKeyboardSizer->Add(
                m_pAnswerButton[iB],
                wxSizerFlags(0).Border(wxLEFT|wxRIGHT, BUTTONS_DISTANCE) );
            if (m_sButtonLabel[iB].IsEmpty()) {
                m_pAnswerButton[iB]->Show(false);
                m_pAnswerButton[iB]->Enable(false);
            }
        }
    }

    SetSizer( pMainSizer );                 // use the sizer for window layout
    pMainSizer->SetSizeHints( this );       // set size hints to honour minimum size

    m_pScoreCtrol->DisplayMessage(_("Click on 'New problem' to start"), 
                                  lmToLogicalUnits(10, lmMILLIMETERS),
                                  true);

    m_pPlayButton->Enable(false);
    m_pShowSolution->Enable(false);
    EnableButtons(false);

}

lmEarChordCtrol::~lmEarChordCtrol()
{
    if (m_pScoreCtrol) {
        delete m_pScoreCtrol;
        m_pScoreCtrol = (lmScoreAuxCtrol*)NULL;
    }

    if (m_pConstrains) {
        delete m_pConstrains;
        m_pConstrains = (lmEarChordConstrains*) NULL;
    }

    if (m_pChordScore) {
        delete m_pChordScore;
        m_pChordScore = (lmScore*)NULL;
    }
}

void lmEarChordCtrol::EnableButtons(bool fEnable)
{
    for (int i=0; i < NUM_BUTTONS; i++) {
        if (m_pAnswerButton[i])
            m_pAnswerButton[i]->Enable(fEnable);
    }
    m_fButtonsEnabled = fEnable;

}

//----------------------------------------------------------------------------------------
// Event handlers

void lmEarChordCtrol::OnSettingsButton(wxCommandEvent& event)
{
    /*
    lmDlgCfgEarIntervals dlg(this, m_pConstrains, true);    // true -> enable First note equal checkbox
    int retcode = dlg.ShowModal();
    if (retcode == wxID_OK) m_pConstrains->SaveSettings();
    */

}

void lmEarChordCtrol::OnSize(wxSizeEvent& event)
{
    //wxLogMessage(_T("OnSize at lmEarChordCtrol"));
    Layout();

}

void lmEarChordCtrol::OnPlay(wxCommandEvent& event)
{
    Play();
}

void lmEarChordCtrol::OnNewProblem(wxCommandEvent& event)
{
    NewProblem();
}

void lmEarChordCtrol::OnDisplaySolution(wxCommandEvent& event)
{
    m_pCounters->IncrementWrong();
    DisplaySolution();
    EnableButtons(false);
}

void lmEarChordCtrol::OnRespButton(wxCommandEvent& event)
{
    int nIndex = event.GetId() - ID_BUTTON;

    //verify if success or failure
    bool fSuccess = (nIndex == m_nRespIndex);
    
    //produce feedback sound, and update counters
    if (fSuccess) {
        m_pCounters->IncrementRight();
    } else {
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

void lmEarChordCtrol::NewProblem()
{
    ResetExercise();

    // select a random key signature
    lmRandomGenerator oGenerator;
    EKeySignatures nKey = earmDo;   //oGenerator.RandomKeySignature();
    
    //Generate a random root note 
    EClefType nClef = eclvSol;
    bool fAllowAccidentals = false;
    wxString sRootNote = oGenerator.GenerateRandomRootNote(nClef, nKey, fAllowAccidentals);

    // generate a random chord
    EChordType nChordType = m_pConstrains->GetRandomChordType();
    lmChordManager oChordMngr(sRootNote, nChordType, nKey);

    //
    //create a score with the chord, for displaying the solution
    //

    wxString sPattern;
    lmNote* pNote;
    lmLDPParser parserLDP;
    lmLDPNode* pNode;
    lmVStaff* pVStaff;

    int nNumNotes = oChordMngr.GetNumNotes();
    m_pChordScore = new lmScore();
    m_pChordScore->SetTopSystemDistance( lmToLogicalUnits(5, lmMILLIMETERS) );    //5mm
    m_pChordScore->AddInstrument(1,0,0,_T(""));                     //one vstaff, MIDI channel 0, MIDI instr 0
    pVStaff = m_pChordScore->GetVStaff(1, 1);      //get first vstaff of instr.1
    pVStaff->AddClef( nClef );
    pVStaff->AddKeySignature( nKey );
    pVStaff->AddTimeSignature(4 ,4, sbNO_VISIBLE );

//    pVStaff->AddEspacio 24
    sPattern = _T("(n ") + oChordMngr.GetPattern(0) + _T(" r)");
    pNode = parserLDP.ParseText( sPattern );
    pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
    int i;
    for (i=1; i < nNumNotes; i++) {
        sPattern = _T("(na ") + oChordMngr.GetPattern(i) +  _T(" r)");
        pNode = parserLDP.ParseText( sPattern );
        pNote = parserLDP.AnalyzeNote(pNode, pVStaff);
    }
    pVStaff->AddBarline(etb_EndBarline, sbNO_VISIBLE);

    //compute the right answer
    m_sAnswer = oChordMngr.GetName();
    m_nRespIndex = 0; //todo
    switch (nChordType)
    {
        // Triads
        case ect_MajorTriad:        m_nRespIndex = 0;   break;
        case ect_MinorTriad:        m_nRespIndex = 1;   break;
        case ect_AugTriad:          m_nRespIndex = 2;   break;
        case ect_DimTriad:          m_nRespIndex = 3;   break;
        case ect_Suspended_4th:     m_nRespIndex = 4;   break;
        case ect_Suspended_2nd:     m_nRespIndex = 4;   break;

        // Seventh chords
        case ect_MajorSeventh:      m_nRespIndex = 6;   break;
        case ect_DominantSeventh:   m_nRespIndex = 5;   break;
        case ect_MinorSeventh:      m_nRespIndex = 7;   break;
        case ect_DimSeventh:        m_nRespIndex = 8;   break;
        case ect_HalfDimSeventh:    m_nRespIndex = 9;   break;
        //case ect_AugMajorSeventh:   m_nRespIndex = -1;  break;
        //case ect_AugSeventh:        m_nRespIndex = -1;  break;
        //case ect_MinorMajorSeventh: m_nRespIndex = -1;  break;

        //// Sixth chords
        //case ect_MajorSixth:        m_nRespIndex = -1;  break;
        //case ect_MinorSixth:        m_nRespIndex = -1;  break;
        //case ect_AugSixth:          m_nRespIndex = -1;  break;

        default:
            wxASSERT(false);
    }

    
    //load total score into the control
    m_pScoreCtrol->SetScore(m_pChordScore, true);   //true: the score must be hidden
    m_pScoreCtrol->DisplayMessage(_T(""), 0, true);     //true: clear the canvas

    m_fPlayEnabled = true;
    m_fProblemCreated = true;
    EnableButtons(true);
    m_pPlayButton->Enable(true);
    m_pShowSolution->Enable(true);

    //play the chord
    Play();

}

void lmEarChordCtrol::Play()
{
    //As scale is built using whole notes, we will play scale at MM=320 so
    //that real note rate will be 80.
    m_pScoreCtrol->PlayScore(lmVISUAL_TRACKING, NO_MARCAR_COMPAS_PREVIO, 
                            ePM_NormalInstrument, 320);

}

void lmEarChordCtrol::DisplaySolution()
{
    wxString sAnswer = m_sAnswer;
    m_pScoreCtrol->HideScore(false);
    m_pScoreCtrol->DisplayMessage(m_sAnswer, lmToLogicalUnits(5, lmMILLIMETERS), false);

    // mark right button in green
    m_pAnswerButton[m_nRespIndex]->SetBackgroundColour(g_pColors->Success());
    
    m_pPlayButton->Enable(true);
    m_fPlayEnabled = true;
    m_fProblemCreated = false;
}

void lmEarChordCtrol::OnDebugShowSourceScore(wxCommandEvent& event)
{
    m_pScoreCtrol->SourceLDP();
}

void lmEarChordCtrol::OnDebugDumpScore(wxCommandEvent& event)
{
    m_pScoreCtrol->Dump();
}

void lmEarChordCtrol::OnDebugShowMidiEvents(wxCommandEvent& event)
{
    m_pScoreCtrol->DumpMidiEvents();
}

void lmEarChordCtrol::ResetExercise()
{
    //clear the canvas
    m_pScoreCtrol->DisplayMessage(_T(""), 0, true);     //true: clear the canvas
    m_pScoreCtrol->Update();    //to force to clear it now

    // restore buttons' normal color
    for (int iB=0; iB < NUM_BUTTONS; iB++) {
        if (!m_sButtonLabel[iB].IsEmpty()) {
            m_pAnswerButton[iB]->SetBackgroundColour( g_pColors->Normal() );
        }
    }
    EnableButtons(false);

    //delete the previous scores
    if (m_pChordScore) {
        delete m_pChordScore;
        m_pChordScore = (lmScore*)NULL;
    }

}
