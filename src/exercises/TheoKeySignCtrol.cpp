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
/*! @file TheoKeySignCtrol.cpp
    @brief Implementation file for class lmTheoKeySignCtrol
    @ingroup html_controls
*/
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "TheoKeySignCtrol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "TheoKeySignCtrol.h"
#include "UrlAuxCtrol.h"
#include "Constrains.h"
#include "Generators.h"
#include "../auxmusic/Conversion.h"

#include "../globals/Colors.h"
extern lmColors* g_pColors;

// access to global external variables
extern bool g_fReleaseVersion;            // in TheApp.cpp
extern bool g_fReleaseBehaviour;        // in TheApp.cpp
extern bool g_fShowDebugLinks;            // in TheApp.cpp
extern bool g_fAutoNewProblem;          // in Preferences.cpp

//access to error's logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;



//------------------------------------------------------------------------------------
// Implementation of lmTheoKeySignCtrol




//Layout definitions
const int BUTTONS_DISTANCE    = 5;        //pixels
const int NUM_LINKS = 3;                //links for actions

//IDs for controls
enum {
    ID_LINK_SEE_SOURCE = 3000,
    ID_LINK_DUMP,
    ID_BUTTON,
    ID_LINK = ID_BUTTON + lmTHEO_KEYSIGN_NUM_BUTTONS,
    ID_LINK_NEW_PROBLEM,
    ID_LINK_SOLUTION
};

BEGIN_EVENT_TABLE(lmTheoKeySignCtrol, wxWindow)
    EVT_COMMAND_RANGE   (ID_BUTTON, ID_BUTTON+lmTHEO_KEYSIGN_NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmTheoKeySignCtrol::OnRespButton)
    EVT_SIZE            (lmTheoKeySignCtrol::OnSize)

    LM_EVT_URL_CLICK    (ID_LINK_SEE_SOURCE, lmTheoKeySignCtrol::OnDebugShowSourceScore)
    LM_EVT_URL_CLICK    (ID_LINK_DUMP, lmTheoKeySignCtrol::OnDebugDumpScore)

    LM_EVT_URL_CLICK    (ID_LINK_NEW_PROBLEM, lmTheoKeySignCtrol::OnNewProblem)
    LM_EVT_URL_CLICK    (ID_LINK_SOLUTION, lmTheoKeySignCtrol::OnDisplaySolution)
END_EVENT_TABLE()

IMPLEMENT_CLASS(lmTheoKeySignCtrol, wxWindow)

static wxString sMajor[15];
static wxString sMinor[15];


lmTheoKeySignCtrol::lmTheoKeySignCtrol(wxWindow* parent, wxWindowID id, 
                           lmTheoKeySignConstrains* pConstrains,
                           const wxPoint& pos, const wxSize& size, int style)
    : wxWindow(parent, id, pos, size, style )
{
    //initializations
    SetBackgroundColour(*wxWHITE);
    for (int i=0; i < lmTHEO_KEYSIGN_NUM_BUTTONS; i++) {
        m_pAnswerButton[i] = (wxButton*)NULL;
    }
    m_nIndexKeyName = 0;
    m_fButtonsEnabled = false;
    m_fProblemCreated = false;
    m_pScore = (lmScore*)NULL;
    m_pScoreCtrol = (lmScoreAuxCtrol*)NULL;
    m_pConstrains = pConstrains;

    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    sMajor[0] = _("C major");
    sMajor[1] = _("G major");
    sMajor[2] = _("D major");
    sMajor[3] = _("A major");
    sMajor[4] = _("E major");
    sMajor[5] = _("B major");
    sMajor[6] = _("F # major");
    sMajor[7] = _("C # major");
    sMajor[8] = _("C b major");
    sMajor[9] = _("G b major");
    sMajor[10] = _("D b major");
    sMajor[11] = _("A b major");
    sMajor[12] = _("E b major");
    sMajor[13] = _("B b major");
    sMajor[14] = _("F major");

    sMinor[0] = _("A minor");
    sMinor[1] = _("E minor");
    sMinor[2] = _("B minor");
    sMinor[3] = _("F # minor");
    sMinor[4] = _("C # minor");
    sMinor[5] = _("G # minor");
    sMinor[6] = _("D # minor");
    sMinor[7] = _("A # minor");
    sMinor[8] = _("A b minor");
    sMinor[9] = _("E b minor");
    sMinor[10] = _("B b minor");
    sMinor[11] = _("F minor");
    sMinor[12] = _("C minor");
    sMinor[13] = _("G minor");
    sMinor[14] = _("D minor");

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
    }

    // sizer for the scoreCtrol and the CountersCtrol
    wxBoxSizer* pTopSizer = new wxBoxSizer( wxHORIZONTAL );
    pMainSizer->Add(
        pTopSizer,
        wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 10) );

    // create score ctrl 
    m_pScoreCtrol = new lmScoreAuxCtrol(this, -1, m_pScore, wxDefaultPosition, wxSize(350,150), eSIMPLE_BORDER);
    pTopSizer->Add(m_pScoreCtrol,
                   wxSizerFlags(1).Left().Border(wxTOP|wxBOTTOM, 10));
    m_pScoreCtrol->SetMargins(lmToLogicalUnits(10, lmMILLIMETERS),      //left=1cm
                              lmToLogicalUnits(10, lmMILLIMETERS),      //right=1cm
                              lmToLogicalUnits(10, lmMILLIMETERS));     //top=1cm
    m_pScoreCtrol->SetScale((float)1.3);

    // right/wrong answers counters control
    m_pCounters = new lmCountersCtrol(this, wxID_ANY);
    pTopSizer->Add(
        m_pCounters,
        wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 10) );
    

        //links 

    wxBoxSizer* pLinksSizer = new wxBoxSizer( wxHORIZONTAL );
    pMainSizer->Add(
        pLinksSizer,
        wxSizerFlags(0).Center().Border(wxALL, 10) );

    // "new problem" button
    pLinksSizer->Add(
        new lmUrlAuxCtrol(this, ID_LINK_NEW_PROBLEM, _("New problem") ),
        wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT|wxBOTTOM, 20) );
    
    // "show solution" button
    pLinksSizer->Add(
        new lmUrlAuxCtrol(this, ID_LINK_SOLUTION, _("Show solution") ),
        wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT|wxBOTTOM, 20) );
    

    //create 15 buttons for the answers: three rows, five buttons per row
    wxBoxSizer* pRowSizer;
    wxButton* pButton;
    int iB = 0;
    const int NUM_ROWS = 3;
    const int NUM_COLS = 5;
    for (int iRow=0; iRow < NUM_ROWS; iRow++) {
        pRowSizer = new wxBoxSizer( wxHORIZONTAL );
        pMainSizer->Add(    
            pRowSizer,
            wxSizerFlags(0).Left());
        pRowSizer->Add(20+BUTTONS_DISTANCE, 24, 0);    //spacer to center labels

        for (int iCol=0; iCol < NUM_COLS; iCol++) {
            iB = iCol + iRow * NUM_COLS;    // button index: 0 .. 24         
            pButton = new wxButton( this, ID_BUTTON + iB, _T(""),
                wxDefaultPosition, wxSize(90, 24));
            m_pAnswerButton[iB++] = pButton;
            pRowSizer->Add(
                pButton,
                wxSizerFlags(0).Border(wxALL, BUTTONS_DISTANCE) );
        }
    }

    SetSizer( pMainSizer );                // use the sizer for window layout
    pMainSizer->SetSizeHints( this );        // set size hints to honour minimum size

    NewProblem();
}

lmTheoKeySignCtrol::~lmTheoKeySignCtrol()
{
    if (m_pScoreCtrol) {
        delete m_pScoreCtrol;
        m_pScoreCtrol = (lmScoreAuxCtrol*)NULL;
    }

    if (m_pConstrains) {
        delete m_pConstrains;
        m_pConstrains = (lmTheoKeySignConstrains*) NULL;
    }

    if (m_pScore) {
        delete m_pScore;
        m_pScore = (lmScore*)NULL;
    }
}

void lmTheoKeySignCtrol::EnableButtons(bool fEnable)
{
    for (int i=0; i < lmTHEO_KEYSIGN_NUM_BUTTONS; i++) {
        if (m_pAnswerButton[i])
            m_pAnswerButton[i]->Enable(fEnable);
    }
    m_fButtonsEnabled = fEnable;

}

//----------------------------------------------------------------------------------------
// Event handlers

void lmTheoKeySignCtrol::OnSize(wxSizeEvent& event)
{
    //wxLogMessage(_T("OnSize en IntrervalsControl"));
    Layout();

}

void lmTheoKeySignCtrol::OnNewProblem(wxCommandEvent& event)
{
    NewProblem();
}

void lmTheoKeySignCtrol::OnDisplaySolution(wxCommandEvent& event)
{
    m_pCounters->IncrementWrong();
    DisplaySolution();
    EnableButtons(false);           //student must not give now the answer
}

void lmTheoKeySignCtrol::OnRespButton(wxCommandEvent& event)
{
    int nIndex = event.GetId() - ID_BUTTON;

    //verify if success or failure
    bool fSuccess = (nIndex == m_nIndexKeyName);
    
    //produce feedback sound, and update counters
    if (fSuccess) {
        m_pCounters->IncrementRight();
    } else {
        m_pCounters->IncrementWrong();
    }
        
    //if failure or not auto-new problem, display the solution.
    //Else, if success and auto-new problem, generate a new problem
    if (!fSuccess || !g_fAutoNewProblem) {
        if (!fSuccess) {
            //failure: mark wrong button in red and right one in green
            m_pAnswerButton[m_nIndexKeyName]->SetBackgroundColour(g_pColors->Success());
            m_pAnswerButton[nIndex]->SetBackgroundColour(g_pColors->Failure());
        }
        //show the solucion
        DisplaySolution();
        EnableButtons(false);

    } else {
        NewProblem();
    }
    
}

void lmTheoKeySignCtrol::NewProblem()
{

    ResetExercise();

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
    EKeySignatures nKey;
    if (m_fMajorMode) {
        if (fFlats) {
            // Major mode, flats
            switch(nAccidentals)
            {
                case 0:
                    nKey = earmDo;
                    nAnswer = 0;            // Do Mayor, La menor, no accidentals
                    m_nIndexKeyName = 1;
                    break;
                case 1:
                    nKey = earmFa;
                    nAnswer = 14;           // Fa Mayor, Re menor"
                    m_nIndexKeyName = 7;
                    break;
               case 2:
                    nKey = earmSib;
                    nAnswer = 13;   // Si b Mayor, Sol menor"
                    m_nIndexKeyName = 13;
                    break;
                case 3:
                    nKey = earmMib;
                    nAnswer = 12;   // Mi b Mayor, Do menor"
                    m_nIndexKeyName = 5;
                    break;
                case 4:
                    nKey = earmLab;
                    nAnswer = 11;   // La b Mayor, Fa menor"
                    m_nIndexKeyName = 11;
                    break;
                case 5:
                    nKey = earmReb;
                    nAnswer = 10;   // Re b Mayor, Si b menor"
                    m_nIndexKeyName = 3;
                    break;
                case 6:
                    nKey = earmSolb;
                    nAnswer = 9;   // Sol b Mayor, Mi b menor"
                    m_nIndexKeyName = 9;
                    break;
                case 7:
                    nKey = earmDob;
                    nAnswer = 8;   // Do b Mayor, La b menor"
                    m_nIndexKeyName = 0;
                    break;
            }
        } else {
            // Major mode, sharps
            switch(nAccidentals)
            {
                case 0:
                    nKey = earmDo;
                    nAnswer = 0;   // Do Mayor, La menor"
                    m_nIndexKeyName = 1;
                    break;
                case 1:
                    nKey = earmSol;
                    nAnswer = 1;   // Sol Mayor, Mi menor"
                    m_nIndexKeyName = 10;
                    break;
                case 2:
                    nKey = earmRe;
                    nAnswer = 2;   // Re Mayor, Si menor"
                    m_nIndexKeyName = 4;
                    break;
                case 3:
                    nKey = earmLa;
                    nAnswer = 3;   // La Mayor, Fa # menor"
                    m_nIndexKeyName = 12;
                    break;
                case 4:
                    nKey = earmMi;
                    nAnswer = 4;   // Mi Mayor, Do # menor"
                    m_nIndexKeyName = 6;
                    break;
                case 5:
                    nKey = earmSi;
                    nAnswer = 5;   // Si Mayor, Sol # menor"
                    m_nIndexKeyName = 14;
                    break;
                case 6:
                    nKey = earmFas;
                    nAnswer = 6;   // Fa # Mayor, Re # menor"
                    m_nIndexKeyName = 8;
                    break;
                case 7:
                    nKey = earmDos;
                    nAnswer = 7;   // Do # Mayor, La # menor"
                    m_nIndexKeyName = 2;
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
                    m_nIndexKeyName = 11;
                    break;
                case 1:
                    nKey = earmRem;
                    nAnswer = 14;   // Fa Mayor, Re menor"
                    m_nIndexKeyName = 2;
                    break;
                case 2:
                    nKey = earmSolm;
                    nAnswer = 13;   // Si b Mayor, Sol menor"
                    m_nIndexKeyName = 8;
                    break;
                case 3:
                    nKey = earmDom;
                    nAnswer = 12;   // Mi b Mayor, Do menor"
                    m_nIndexKeyName = 0;
                    break;
                case 4:
                    nKey = earmFam;
                    nAnswer = 11;   // La b Mayor, Fa menor"
                    m_nIndexKeyName = 6;
                    break;
                case 5:
                    nKey = earmSibm;
                    nAnswer = 10;   // Re b Mayor, Si b menor"
                    m_nIndexKeyName = 13;
                    break;
                case 6:
                    nKey = earmMibm;
                    nAnswer = 9;   // Sol b Mayor, Mi b menor"
                    m_nIndexKeyName = 4;
                    break;
                case 7:
                    nKey = earmLabm;
                    nAnswer = 8;   // Do b Mayor, La b menor"
                    m_nIndexKeyName = 10;
                    break;
            }
        } else {
            // Minor mode, sharps
            switch(nAccidentals)
            {
                case 0:
                    nKey = earmLam;
                    nAnswer = 0;   // Do Mayor, La menor"
                    m_nIndexKeyName = 11;
                    break;
                case 1:
                    nKey = earmMim;
                    nAnswer = 1;   // Sol Mayor, Mi menor"
                    m_nIndexKeyName = 5;
                    break;
                case 2:
                    nKey = earmSim;
                    nAnswer = 2;   // Re Mayor, Si menor"
                    m_nIndexKeyName = 14;
                    break;
                case 3:
                    nKey = earmFasm;
                    nAnswer = 3;   // La Mayor, Fa # menor"
                    m_nIndexKeyName = 7;
                    break;
                case 4:
                    nKey = earmDosm;
                    nAnswer = 4;   // Mi Mayor, Do # menor"
                    m_nIndexKeyName = 1;
                    break;
                case 5:
                    nKey = earmSolsm;
                    nAnswer = 5;   // Si Mayor, Sol # menor"
                    m_nIndexKeyName = 9;
                    break;
                case 6:
                    nKey = earmResm;
                    nAnswer = 6;   // Fa # Mayor, Re # menor"
                    m_nIndexKeyName = 3;
                    break;
                case 7:
                    nKey = earmLasm;
                    nAnswer = 7;   // Do # Mayor, La # menor"
                    m_nIndexKeyName = 12;
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
        _T("[lmTheoKeySignCtrol::NewProblem] m_fIdentifyKey=%s, m_fMajorMode=%s, fFlats=%s, nKey=%d, nAnswer=%d, m_nIndexKeyName=%d"),
            (m_fIdentifyKey ? _T("yes") : _T("no")),
            (m_fMajorMode ? _T("yes") : _T("no")),
            (fFlats ? _T("yes") : _T("no")),
            nKey, nAnswer, m_nIndexKeyName);


    // store index to right answer button (for guess-number-of-accidentals problems)
    if (!m_fIdentifyKey) {
        m_nIndexKeyName = KeySignatureToNumFifths(nKey);
        if (m_nIndexKeyName < 0) m_nIndexKeyName = 7 - m_nIndexKeyName;
    }

    // choose clef
    EClefType nClef = oGenerator.GenerateClef(m_pConstrains->GetClefConstrains());

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
    m_pScore = new lmScore();
    m_pScore->SetTopSystemDistance( lmToLogicalUnits(5, lmMILLIMETERS) );   //5mm
    m_pScore->AddInstrument(1,0,0,_T(""));                   //one vstaff, MIDI channel 0, MIDI instr 0
    lmVStaff *pVStaff = m_pScore->GetVStaff(1, 1);    //get first vstaff of instr.1
    pVStaff->AddClef( nClef );
    pVStaff->AddKeySignature(nKey);
    pVStaff->AddBarline(etb_EndBarline, lmNO_VISIBLE);

    //wxLogMessage(wxString::Format(
    //    _T("[lmTheoKeySignCtrol::NewProblem] m_nIndexKeyName=%d, oIntv.GetIntervalNum()=%d"),
    //    m_nIndexKeyName, oIntv.GetIntervalNum() ));
    
    //display the problem
    m_pCounters->NextTeam();
    if (m_fIdentifyKey) {
        //direct problem
        m_sAnswer = sMajor[nAnswer] + _T(", ") + sMinor[nAnswer];
        m_pScoreCtrol->DisplayScore(m_pScore);
    } else {
        //inverse problem
        m_sAnswer = (m_fMajorMode ? sMajor[nAnswer] : sMinor[nAnswer] );
        m_pScoreCtrol->DisplayMessage(m_sAnswer, lmToLogicalUnits(5, lmMILLIMETERS));
    }
    m_fProblemCreated = true;
    EnableButtons(true);
    
}

void lmTheoKeySignCtrol::DisplaySolution()
{
    if (m_fIdentifyKey) {
        m_pScoreCtrol->DisplayMessage(m_sAnswer, lmToLogicalUnits(5, lmMILLIMETERS), false);
    } else {
        m_pScoreCtrol->DisplayScore(m_pScore, false);
    }
    
    m_fProblemCreated = false;
    
}

void lmTheoKeySignCtrol::OnDebugShowSourceScore(wxCommandEvent& event)
{
    m_pScoreCtrol->SourceLDP();
}

void lmTheoKeySignCtrol::OnDebugDumpScore(wxCommandEvent& event)
{
    m_pScoreCtrol->Dump();
}

void lmTheoKeySignCtrol::ResetExercise()
{
    for (int i=0; i < lmTHEO_KEYSIGN_NUM_BUTTONS; i++) {
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

