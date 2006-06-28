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
/*! @file TheoScalesCtrol.cpp
    @brief Implementation file for class lmTheoScalesCtrol
    @ingroup html_controls
*/
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "TheoScalesCtrol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "TheoScalesCtrol.h"
#include "UrlAuxCtrol.h"
#include "ScoreAuxCtrol.h"
#include "Constrains.h"
#include "Generators.h"
#include "../auxmusic/Conversion.h"
#include "../ldp_parser/LDPParser.h"
#include "../score/score.h"


// access to global functions
extern void ComputeAccidentals(EKeySignatures nKeySignature, int    nAccidentals[]);
extern int GetRootNoteIndex(EKeySignatures nKeySignature);
extern const wxString& GetKeySignatureName(EKeySignatures nKeySignature);

// access to global external variables
extern bool g_fReleaseVersion;            // in TheApp.cpp
extern bool g_fReleaseBehaviour;        // in TheApp.cpp
extern bool g_fShowDebugLinks;            // in TheApp.cpp


static wxString sRowLabel[2];
static wxString sButtonLabel[8];

//------------------------------------------------------------------------------------
// Implementation of lmTheoScalesCtrol
//------------------------------------------------------------------------------------

/*! @todo
    It would be nice to add an additional link "Explain solution" to provide guidance
    to the student about the steps followed to find the solution. This link will be enabled
    only after displaying a solution and will be disabled when 'new problem' clicked.
*/

//Layout definitions
const int BUTTONS_DISTANCE = 5;     //pixels
const int NUM_BUTTONS = 8;          //buttons for answers

//IDs for controls
enum {
    ID_BUTTON = 3000,
    ID_LINK = ID_BUTTON + NUM_BUTTONS,
    ID_LINK_NEW_PROBLEM,
    ID_LINK_PLAY,
    ID_LINK_SOLUTION,
    ID_LINK_SEE_SOURCE,
    ID_LINK_DUMP,
    ID_LINK_MIDI_EVENTS,
    ID_LINK_SETTINGS
};


BEGIN_EVENT_TABLE(lmTheoScalesCtrol, wxWindow)
    EVT_COMMAND_RANGE (ID_BUTTON, ID_BUTTON+NUM_BUTTONS-1, wxEVT_COMMAND_BUTTON_CLICKED, lmTheoScalesCtrol::OnRespButton)
    EVT_SIZE            (lmTheoScalesCtrol::OnSize)
    LM_EVT_URL_CLICK    (ID_LINK_NEW_PROBLEM, lmTheoScalesCtrol::OnNewProblem)
    LM_EVT_URL_CLICK    (ID_LINK_PLAY, lmTheoScalesCtrol::OnPlay)
    LM_EVT_URL_CLICK    (ID_LINK_SOLUTION, lmTheoScalesCtrol::OnDisplaySolution)
    LM_EVT_URL_CLICK    (ID_LINK_SEE_SOURCE, lmTheoScalesCtrol::OnDebugShowSourceScore)
    LM_EVT_URL_CLICK    (ID_LINK_DUMP, lmTheoScalesCtrol::OnDebugDumpScore)
    LM_EVT_URL_CLICK    (ID_LINK_MIDI_EVENTS, lmTheoScalesCtrol::OnDebugShowMidiEvents)
    LM_EVT_END_OF_PLAY  (lmTheoScalesCtrol::OnEndOfPlay)
    EVT_CLOSE           (lmTheoScalesCtrol::OnClose)
END_EVENT_TABLE()

IMPLEMENT_CLASS(lmTheoScalesCtrol, wxWindow)

lmTheoScalesCtrol::lmTheoScalesCtrol(wxWindow* parent, wxWindowID id, 
                           lmTheoScalesConstrains* pConstrains,
                           const wxPoint& pos, const wxSize& size, int style)
    : wxWindow(parent, id, pos, size, style )
{
    //initializations
    m_fButtonsEnabled = false;
    m_fProblemCreated = false;
    m_fPlayEnabled = false;
    m_pScore = (lmScore*)NULL;
    m_pScoreCtrol = (lmScoreAuxCtrol*)NULL;
    m_pConstrains = pConstrains;
    m_pChkKeySignature = (wxCheckBox*)NULL;
    m_fPlaying = false;
    m_fClosing = false;

    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    sRowLabel[0] = _("Major");
    sRowLabel[1] = _("minor");

    //      major scales
    sButtonLabel[0] = _("Natural");
    sButtonLabel[1] = _("Type II");
    sButtonLabel[2] = _("Type III");
    sButtonLabel[3] = _("Type IV");
    //      minor scales
    sButtonLabel[4] = _("Natural");
    sButtonLabel[5] = _("Dorian");
    sButtonLabel[6] = _("Harmonic");
    sButtonLabel[7] = _("Melodic");


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

    // sizer for the ScoreCtrol and the checkBox to "Display scales without key signature"
    wxBoxSizer* pScoreSizer = new wxBoxSizer( wxVERTICAL );
    pTopSizer->Add(
        pScoreSizer,
        wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 10).Expand() );

    // create score ctrl 
    m_pScoreCtrol = new lmScoreAuxCtrol(this, -1, m_pScore, wxDefaultPosition, wxSize(320,150), eSIMPLE_BORDER);
    m_pScoreCtrol->SetMargins(lmToLogicalUnits(5, lmMILLIMETERS),      //left=5mm
                              lmToLogicalUnits(5, lmMILLIMETERS),      //right=5mm
                              lmToLogicalUnits(15, lmMILLIMETERS));    //top=15mm
    pScoreSizer->Add(
        m_pScoreCtrol,
        wxSizerFlags(1).Left().Border(wxTOP|wxBOTTOM, 10));

    m_pScoreCtrol->SetScale((float)1.3);

    // checkBox to "Display scales without key signature accidentals"
    if (m_pConstrains->CtrolKeySignature()) {
        m_pChkKeySignature = new wxCheckBox(this, -1, 
            _("Display scales without key signature accidentals"),
            wxDefaultPosition);
        m_pChkKeySignature->SetValue(false);
        pScoreSizer->Add(
            m_pChkKeySignature,
            wxSizerFlags(0).Border(wxALL, 10));
    }
        

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
    m_pShowSolution = new lmUrlAuxCtrol(this, ID_LINK_SOLUTION, _("Show solution") );
    pLinksSizer->Add(
        m_pShowSolution,
        wxSizerFlags(0).Left().Border(wxLEFT|wxRIGHT, 20) );
    
    //create 8 buttons for the answers: two rows, four buttons per row
    wxBoxSizer* pRowSizer;
    wxButton* pButton;
    int iB = 0;
    const int NUM_ROWS = 2;
    const int NUM_COLS = 4;
    for (int iRow=0; iRow < NUM_ROWS; iRow++) {
        pRowSizer = new wxBoxSizer( wxHORIZONTAL );
        pMainSizer->Add(    
            pRowSizer,
            wxSizerFlags(0).Left());

        //add label for type of scale
        pRowSizer->Add(
            new wxStaticText(this, -1, sRowLabel[iRow], wxDefaultPosition, wxSize(60, 24)),
            wxSizerFlags(0).Left().Border(wxRIGHT|wxLEFT|wxTOP, BUTTONS_DISTANCE) );

        //add this row buttons
        for (int iCol=0; iCol < NUM_COLS; iCol++) {
            iB = iCol + iRow * NUM_COLS;    // button index: 0 .. 7         
            pButton = new wxButton( this, ID_BUTTON + iB, sButtonLabel[iB],
                wxDefaultPosition, wxSize(90, 24));
            m_pAnswerButton[iB++] = pButton;
            pRowSizer->Add(
                pButton,
                wxSizerFlags(0).Border(wxRIGHT|wxLEFT|wxTOP, BUTTONS_DISTANCE) );
        }
    }

    SetSizer( pMainSizer );                // use the sizer for window layout
    pMainSizer->SetSizeHints( this );        // set size hints to honour minimum size

    pSettingsLink->Enable(false);   //disabled until settings implemented

    NewProblem();

}

lmTheoScalesCtrol::~lmTheoScalesCtrol()
{
    if (m_pScoreCtrol) {
        delete m_pScoreCtrol;
        m_pScoreCtrol = (lmScoreAuxCtrol*)NULL;
    }

    if (m_pConstrains) {
        delete m_pConstrains;
        m_pConstrains = (lmTheoScalesConstrains*) NULL;
    }

    if (m_pScore) {
        delete m_pScore;
        m_pScore = (lmScore*)NULL;
    }
}

void lmTheoScalesCtrol::EnableButtons(bool fEnable)
{
    for (int i=0; i < NUM_BUTTONS; i++) {
        if (m_pAnswerButton[i])
            m_pAnswerButton[i]->Enable(fEnable);
    }
    m_fButtonsEnabled = fEnable;

}

//----------------------------------------------------------------------------------------
// Event handlers

void lmTheoScalesCtrol::OnClose(wxCloseEvent& event)
{
    /*! @todo
        The window does not receive the event. Therefore this code never get executed.
        And this produces a crash if lenmus is closed while playing.
        It is not clear if it will be possible to detect the closing request in a parent
        window. The TheoScoreCtrol's parent is an wxHtmlCell and it derives from wxObject,
        not from wxWindow.
    */
    if (m_fPlaying) {
        //ongoing playing.
        if (!m_fClosing) m_pScoreCtrol->Stop();        // Stop playing
        ::wxMilliSleep(500);                //wait for 500 ms to give time for stopping
        m_fClosing = true;
        //re-send a close event
        wxCloseEvent newEvent(event);
        AddPendingEvent(newEvent);
        event.Veto(true);
    }
    else {
        //Not playing. close the window
        Destroy();
    }

}

void lmTheoScalesCtrol::OnSize(wxSizeEvent& event)
{
    Layout();
}

void lmTheoScalesCtrol::OnPlay(wxCommandEvent& event)
{
    Play();
}

void lmTheoScalesCtrol::OnNewProblem(wxCommandEvent& event)
{
    if (m_fPlaying) return;
    NewProblem();
}

void lmTheoScalesCtrol::OnDisplaySolution(wxCommandEvent& event)
{
    m_pCounters->IncrementWrong();
    DisplaySolution();
    EnableButtons(false);           //student must not give now the answer
    m_pPlayButton->Enable(true);    //but allow to play the scale

}


void lmTheoScalesCtrol::NewProblem()
{
    int i, j;

    ResetExercise();

    //Generate a problem
    lmRandomGenerator oGenerator;
    EClefType nClef = oGenerator.GenerateClef(m_pConstrains->GetClefConstrains());
    EKeySignatures nKeySignature = oGenerator.RandomKeySignature();    
    int nContext[7];
    ComputeAccidentals(nKeySignature, nContext);        //create the context

    //determine if the key signature must be drawn
    bool fDrawWithoutKeySignature;
    if (m_pChkKeySignature) {
        fDrawWithoutKeySignature = m_pChkKeySignature->IsChecked();
    } else {
        fDrawWithoutKeySignature = m_pConstrains->DrawWithoutKeySignature();
    }

    bool fMajorScale = IsMajor(nKeySignature);

    //Compute index to root note (0=Do, 1=Re, 2=Mi, 3=Fa, 4=Sol, 5=La, 6=Si)
    int iRoot = GetRootNoteIndex(nKeySignature);

    //Convert index to pitch; depends on clef
    lmPitch     ntPitch[8];           //the pitch of the scale notes
    //! @todo Adjust scales for clefs Do1, Do2, Do3 and Do4
    switch(nClef) {
        case eclvSol:
            //b3 to a4
            ntPitch[0] = (iRoot==6 ? iRoot + lmC3PITCH : iRoot + lmC4PITCH);
            break;
        case eclvFa4:
            //e2 to d3
            ntPitch[0] = (iRoot > 1 ? iRoot + lmC2PITCH : iRoot + lmC3PITCH);
            break;
        case eclvFa3:
            //g2 to f3
            ntPitch[0] = (iRoot > 3 ? iRoot + lmC2PITCH : iRoot + lmC3PITCH);
            break;
        case eclvDo1:
            //b3 to a4
            ntPitch[0] = (iRoot==6 ? iRoot + lmC4PITCH : iRoot + lmC4PITCH);
            break;
        case eclvDo2:
            //b3 to a4
            ntPitch[0] = (iRoot==6 ? iRoot + lmC4PITCH : iRoot + lmC4PITCH);
            break;
        case eclvDo3:
            //b3 to a4
            ntPitch[0] = (iRoot==6 ? iRoot + lmC4PITCH : iRoot + lmC4PITCH -6);
            break;
        case eclvDo4:
            //b3 to a4
            ntPitch[0] = (iRoot==6 ? iRoot + lmC4PITCH : iRoot + lmC4PITCH -6);
            break;
        default:
            wxASSERT(false);
    }

    //Generate pitch for the other notes of the scale
    for(i=1; i<8; i++) {
        ntPitch[i] = ntPitch[i-1] + 1;
    }
    
    //Convert problem to LDP pattern
    wxString sAccidentals;        // key signature accidentals
    wxString sAlter[8];            // optional accidentals to change scale type
    wxString sSource[8];
    lmConverter oConv;
    for (i=0; i < 8; i++) {
        sSource[i] = _T("(n ");
        sAccidentals = _T("");
        if (fDrawWithoutKeySignature) {
            j = (iRoot + i) % 7;
            if (nContext[j] == -1) {
                sAccidentals = _T("-");
            } else if (nContext[j] == 1) {
                sAccidentals = _T("+");
            } else {
                sAccidentals = _T("");
            }
        }
        sAlter[i] = _T("");        //by default no accidentals

        //optional accidentals in VI and VII grades
        if ((i == 5) || (i == 6)) {
            if ((rand() % 2) == 0) {    //generate accidental with 50% probability
                if (fMajorScale) {
                    //Major scale: accidentals will be flats
                    if (nContext[(iRoot + i) % 7] == 1) {    //if it is a sharp put a natural sign
                        sAlter[i] = (fDrawWithoutKeySignature ? _T("") : _T("="));
                    } else if (nContext[(iRoot + i) % 7] == -1) {    //if it is a flat put a double flat
                        sAlter[i] = _T("--");
                    } else {
                        sAlter[i] = _T("-");    //it is a natural. Put a flat
                    }
                }
                else {
                    //Minor scale: accidentals will be sharps
                    if (nContext[(iRoot + i) % 7] == -1) {    //if it is a flat put a natural sign
                        sAlter[i] = (fDrawWithoutKeySignature ? _T("") : _T("="));
                    } else if (nContext[(iRoot + i) % 7] == 1) {    //if it is a sharp put a double sharp
                        sAlter[i] = _T("x");
                    } else {
                        sAlter[i] = _T("+");    //it is a natural. Put a sharp
                    }
                }
            }
        }

        if (fDrawWithoutKeySignature) {
            sSource[i] += (sAlter[i] == _T("") ? sAccidentals : sAlter[i]);
        }
        else {
            sSource[i] += sAlter[i];
        }
        sSource[i] += oConv.GetEnglishNoteName(ntPitch[i]) + _T(" r)");
    }

    //compute number of accidentals in key signature
    int nNumFiths = 0;
    for (i=0; i < 7; i++) {
        nNumFiths += nContext[i];
    }

    //
    //create the score
    //

    lmNote* pNote[8];
    lmLDPParser parserLDP;
    lmLDPNode* pNode;
    m_pScore = new lmScore();
    m_pScore->SetTopSystemDistance( lmToLogicalUnits(5, lmMILLIMETERS) );   //5mm
    m_pScore->AddInstrument(1,0,0,_T(""));                    //one vstaff, MIDI channel 0, MIDI instr 0
    lmVStaff *pVStaff = m_pScore->GetVStaff(1, 1);    //get first vstaff of instr.1
    pVStaff->AddClef( nClef );
    // if the scale is going to be drawn without key signature, use Do major / La minor
    if (fDrawWithoutKeySignature) {
        pVStaff->AddKeySignature(0, fMajorScale);            // num.of fifths, major/minor
    } else {
        pVStaff->AddKeySignature(nNumFiths, fMajorScale);    // num.of fifths, major/minor
    }
    pVStaff->AddTimeSignature(4 ,4, sbNO_VISIBLE );
//    pVStaff->AddEspacio 24
    //as we are using whole notes there is one in each measure ==> 8 measures
    for (i=0; i < 7; i++) {
        pNode = parserLDP.ParseText( sSource[i] );
        pNote[i] = parserLDP.AnalyzeNote(pNode, pVStaff);
        pVStaff->AddBarline(etb_SimpleBarline, sbNO_VISIBLE);    //so that accidental doesn't affect 2nd note
    }
    pNode = parserLDP.ParseText( sSource[7] );
    pNote[7] = parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddBarline(etb_EndBarline, sbNO_VISIBLE);

    //use simple renderer
    m_pScore->SetRenderizationType(eRenderSimple);
    
    //    
    //compute the right answer
    //

    /*
        Medievals modes         Mode    Greek modes
        ---------------------   ----    --------------------------
        Protus      auténtico   I       Dorico          dorian
                    plagal      II      Hipodórico
        Deuterus    auténtico   III     Frigio          phrygian
                    plagal      IV      Hipofrigio
        Tritus      auténtico   V       Lidio           lydian
                    plagal      VI      Hipolidio
        Tetrardus   auténtico   VII     Mixolidio       mixolydian
                    plagal      VIII    Hipomixolidio

        Modes introduced in 1547:

                    auténtico   IX      Eolio           aeolian
                    plagal      X       Hipoeolio
                    auténtico   XI      Jónico          ionian
                    plagal      XII     Hipojónico

        Later introduced modes (rarely used):

                                XIII    Locrio          locrian
                                XIV     Hipolocrio
    */


    m_sAnswer = GetKeySignatureName(nKeySignature);
    if (fMajorScale) {
        //major scale
        if (sAlter[6] != _T("") && sAlter[5] != _T("")) {
            m_sAnswer += _(", Type III");
            m_nRespIndex = 2;
        } else if (sAlter[5] != _T("")) {
            m_sAnswer += _(", harmonic (Type II)");
            m_nRespIndex = 1;
        } else if (sAlter[6] != _T("")) {
            m_sAnswer += _(", mixolydian (Type IV)");
            m_nRespIndex = 3;
        } else {
            m_sAnswer += _(", natural (Type I)");
            m_nRespIndex = 0;
        }
    }
    else {
        //minor scale
        if (sAlter[6] != _T("") && sAlter[5] != _T("")) {
            m_sAnswer += _(", melodic");
            m_nRespIndex = 7;
       } else if (sAlter[5] != _T("")) {
            m_sAnswer += _(", dorian");
            m_nRespIndex = 5;
        } else if (sAlter[6] != _T("")) {
            m_sAnswer += _(", harmonic");
            m_nRespIndex = 6;
        } else {
            m_sAnswer += _(", natural");
            m_nRespIndex = 4;
        }
    }
    
    //display the problem
    m_fDeduceScale = oGenerator.FlipCoin();
    if (m_fDeduceScale) {
        //direct problem
        m_pScoreCtrol->DisplayScore(m_pScore);
        m_fPlayEnabled = true;
    } else {
        //inverse problem
        m_pScoreCtrol->DisplayMessage(m_sAnswer, lmToLogicalUnits(5, lmMILLIMETERS));
        m_fPlayEnabled = false;
    }
    m_fProblemCreated = true;

    EnableButtons(m_fDeduceScale);  //enable buttons only for deduce scale
    m_pShowSolution->Enable(true);
    m_pPlayButton->Enable(m_fDeduceScale);
    
}

void lmTheoScalesCtrol::OnRespButton(wxCommandEvent& event)
{
    int nIndex = event.GetId() - ID_BUTTON;

    //buttons are only enabled in m_fDeduceScale type problems
    bool fSuccess;
    wxColour* pColor;
    
    //verify if success or failure
    if (!m_fDeduceScale) return;
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
        EnableButtons(false);
        m_pShowSolution->Enable(false);

    } else {
        NewProblem();
    }
    
}


void lmTheoScalesCtrol::DisplaySolution()
{
    if (m_fDeduceScale) {
        m_pScoreCtrol->DisplayMessage(m_sAnswer, lmToLogicalUnits(5, lmMILLIMETERS), false);
    } else {
        m_pScoreCtrol->DisplayScore(m_pScore, false);
    }
    
    m_fPlayEnabled = true;
    m_fProblemCreated = false;
    
}

/*! Playback the score.
    Play() method is called either to play or to stop playing. 
*/
void lmTheoScalesCtrol::Play()
{
    
    if (!m_fPlaying) {
        // Play button pressed

        //change link from "Play" to "Stop playing"
        m_pPlayButton->SetLabel(_("Stop playing"));

        //As scale is built using whole notes, we will play scale at MM=320 so
        //that real note rate will be 80.
        m_pScoreCtrol->PlayScore(lmVISUAL_TRACKING, NO_MARCAR_COMPAS_PREVIO, 
                                ePM_NormalInstrument, 320);
        m_fPlaying = true;

        //@attention The link label is restored to "Play" when the EndOfPlay event is
        //received. Flag m_fPlaying is also reset there
    }
    else {
        // "Stop playing" button pressed
        m_pScoreCtrol->Stop();
    }

}

void lmTheoScalesCtrol::OnEndOfPlay(lmEndOfPlayEvent& WXUNUSED(event))
{
    m_pPlayButton->SetLabel(_("Play"));
    m_fPlaying = false;
}

void lmTheoScalesCtrol::OnDebugShowSourceScore(wxCommandEvent& event)
{
    m_pScoreCtrol->SourceLDP();
}

void lmTheoScalesCtrol::OnDebugDumpScore(wxCommandEvent& event)
{
    m_pScoreCtrol->Dump();
}

void lmTheoScalesCtrol::OnDebugShowMidiEvents(wxCommandEvent& event)
{
    m_pScoreCtrol->DumpMidiEvents();
}

void lmTheoScalesCtrol::ResetExercise()
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
