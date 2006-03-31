// RCS-ID: $Id: EarIntervalsCtrol.cpp,v 1.7 2006/02/23 19:19:15 cecilios Exp $
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

//////contadores de respuestas correctas/fallidas
//    int            m_nRightAnswers;
//    int            m_nWrongAnswers;
//    wxString    m_lblRight As Label
//    wxString    m_lblWrong As Label
//    wxString    m_lblMark As Label
////
////

#include "EarIntervalsCtrol.h"
#include "UrlAuxCtrol.h"
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
extern bool g_fReleaseBehaviour;        // in TheApp.cpp
extern bool g_fShowDebugLinks;            // in TheApp.cpp

//------------------------------------------------------------------------------------
// Implementation of lmEarIntervalsCtrol




//Layout definitions
const int BUTTONS_DISTANCE    = 5;        //pixels
const int NUM_LINKS = 3;                //links for actions


//IDs for controls
enum {
    ID_LINK_SEE_SOURCE = 100,
    ID_LINK_DUMP,
    ID_LINK_MIDI_EVENTS,
    ID_BUTTON,
    ID_LINK = ID_BUTTON + lmEAR_INVAL_NUM_BUTTONS,
    ID_LINK_NEW_PROBLEM,
    ID_LINK_RESET_COUNTERS,
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
    LM_EVT_URL_CLICK    (ID_LINK_RESET_COUNTERS, lmEarIntervalsCtrol::OnResetCounters)
    LM_EVT_URL_CLICK    (ID_LINK_PLAY, lmEarIntervalsCtrol::OnPlay)
    LM_EVT_URL_CLICK    (ID_LINK_SOLUTION, lmEarIntervalsCtrol::OnDisplaySolution)
    LM_EVT_URL_CLICK    (ID_LINK_SETTINGS, lmEarIntervalsCtrol::OnSettingsButton)
END_EVENT_TABLE()

IMPLEMENT_CLASS(lmEarIntervalsCtrol, wxWindow)

static wxString sBtLabel[lmEAR_INVAL_NUM_BUTTONS] = {
        _("Unison"),               // _("Unisono"),
        _("minor 2nd"),            // _("2ª menor"),
        _("major 2nd"),            // _("2ª mayor"),
        _("minor 3rd"),            // _("3ª menor"),
        _("major 3rd"),            // _("3ª mayor"),
        _("4th perfect"),          // _("4ª justa"),
        _("aug.4th/dim.5th"),      // _("4ª aum/ 5ª dism"),
        _("perfect 5th"),          // _("5ª justa"),
        _("minor 6th"),            // _("6ª menor"),
        _("major 6th"),            // _("6ª mayor"),
        _("minor 7th"),            // _("7ª menor"),
        _("major 7th"),            // _("7ª mayor"),
        _("perfect 8th"),          // _("8ª justa"),
        _("minor 9th"),            // _("9ª menor"),
        _("major 9th"),            // _("9ª mayor"),
        _("minor 10th"),           // _("10ª menor"),
        _("major 10th"),           // _("10ª mayor"),
        _("perfect 11th"),         // _("11ª justa"),
        _("aug.11th / dim.12th"),  // _("11ª aum/ 12ª dis"),
        _("perfect 12th"),         // _("12ª justa"),
        _("minor 13th"),           // _("13ª menor"),
        _("major 13th"),           // _("13ª mayor"),
        _("minor 14th"),           // _("14ª menor"),
        _("major 14th"),           // _("14ª mayor"),
        _("two octaves")           // _("Dos octavas")
};

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

    // set interval associated to each button
    int j = 0;
    for (i=0; i < lmEAR_INVAL_NUM_BUTTONS; i++) {
        if (m_pConstrains->IsIntervalAllowed(i)) {
            m_nRealIntval[j] = i;
            j++;
        }
    }
    m_nValidIntervals = j;

    //the window is divided into two regions: top, for score on left and counters and links
    //on the right, and bottom region, for answer buttons 
    wxBoxSizer* pMainSizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer* pTopSizer = new wxBoxSizer( wxHORIZONTAL );

    // Inside TopSizer we set up a vertical sizer: score on top, debugg links bottom
    wxBoxSizer* pScoreSizer = new wxBoxSizer( wxVERTICAL );
    pTopSizer->Add(pScoreSizer, 0, wxALIGN_LEFT|wxALL, 5);

    // create score ctrl 
    m_pScoreCtrol = new lmScoreAuxCtrol(this, -1, m_pScore, wxDefaultPosition, wxSize(420,200), eSIMPLE_BORDER);
    pScoreSizer->Add(
        m_pScoreCtrol,
        wxSizerFlags(1).Left().Border(wxALL, 10));

    // debug buttons
    if (g_fShowDebugLinks && !g_fReleaseVersion) {
        wxBoxSizer* pDbgSizer = new wxBoxSizer( wxHORIZONTAL );
        pScoreSizer->Add(pDbgSizer, 0, wxALIGN_LEFT|wxALL, 5);

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

    wxBoxSizer* pCountersSizer = new wxBoxSizer( wxVERTICAL );
    m_pScoreCtrol->SetMargins(10000, 10000, 20000);        //right=1cm, left=1cm, top=2cm
    m_pScoreCtrol->SetScale((float)1.3);


    //control to count failures and right answers
    //Dim iCtrol As Long
    //m_oAtributos.rLeft = rLeft
    //m_oAtributos.rTop = picObj(m_iPic).Top
    //m_oAtributos.nSpace = 0
    //iCtrol = CrearObjetoMarcador
    //m_oTeoIntervalos.SetContadores lblTexto(iCtrol), lblTexto(iCtrol + 1), lblTexto(iCtrol + 2)
    
    // "new problem" button
    pCountersSizer->Add(
        new lmUrlAuxCtrol(this, ID_LINK_NEW_PROBLEM, _("New problem") ),
        wxSizerFlags(0).Left().Border(wxALL, 10) );
    
    // "play" button
    pCountersSizer->Add(
        new lmUrlAuxCtrol(this, ID_LINK_PLAY, _("Play") ),
        wxSizerFlags(0).Left().Border(wxALL, 10) );
    
    // "show solution" button
    pCountersSizer->Add(
        new lmUrlAuxCtrol(this, ID_LINK_SOLUTION, _("Show solution") ),
        wxSizerFlags(0).Left().Border(wxALL, 10) );
    
    // settings link
    lmUrlAuxCtrol* pSettingsLink = new lmUrlAuxCtrol(this, ID_LINK_SETTINGS, _("Settings") );
    pCountersSizer->Add(pSettingsLink, wxSizerFlags(0).Left().Border(wxALL, 10) );

    // "reset counters" button
    //pCountersSizer->Add(
    //    new lmUrlAuxCtrol(this, ID_LINK_RESET_COUNTERS, _("Reset counters") ),
    //    wxSizerFlags(0).Left().Border(wxALL, 10) );

    pTopSizer->Add(
        pCountersSizer,
        wxSizerFlags(0).Right().Border(wxALL, 10) );


    pMainSizer->Add(
        pTopSizer,
        wxSizerFlags(0).Left().Border(wxALL, 10) );

    //create up to 25 buttons for the answers: five rows, five buttons per row
    wxBoxSizer* pRowSizer;
    wxButton* pButton;
    int iB = 0;
    const int NUM_ROWS = 5;
    const int NUM_COLS = 5;
    for (int iRow=0; iRow < NUM_ROWS; iRow++) {
        pRowSizer = new wxBoxSizer( wxHORIZONTAL );
        pRowSizer->Add(20+BUTTONS_DISTANCE, 24, 0);    //spacer to center labels

        for (int iCol=0; iCol < NUM_COLS; iCol++) {
            iB = iCol + iRow * NUM_COLS;    // button index: 0 .. 24         
            pButton = new wxButton( this, ID_BUTTON + iB, sBtLabel[m_nRealIntval[iB]],
                wxDefaultPosition, wxSize(100, 24));
            m_pAnswerButton[iB++] = pButton;
            pRowSizer->Add(
                pButton,
                wxSizerFlags(0).Border(wxALL, BUTTONS_DISTANCE) );
            if (iB == m_nValidIntervals) break;
        }
        pMainSizer->Add(    
            pRowSizer,
            wxSizerFlags(0).Left());
        if (iB == m_nValidIntervals) break;
    }

    SetSizer( pMainSizer );                // use the sizer for window layout
    pMainSizer->SetSizeHints( this );        // set size hints to honour minimum size

    m_pScoreCtrol->DisplayMessage(_("Click on 'New problem' to start"), 1000, true);

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

void lmEarIntervalsCtrol::EnableButtons(bool fEnable)
{
    for (int i=0; i < lmEAR_INVAL_NUM_BUTTONS; i++) {
        if (m_pAnswerButton[i])
            m_pAnswerButton[i]->Enable(fEnable);
    }
    m_fButtonsEnabled = fEnable;

}

//----------------------------------------------------------------------------------------
// Event handlers

void lmEarIntervalsCtrol::OnSettingsButton(wxCommandEvent& event)
{
    lmDlgCfgEarIntervals dlg(this, m_pConstrains );
    int retcode = dlg.ShowModal();
    if (retcode == wxID_OK) m_pConstrains->SaveSettings();

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

void lmEarIntervalsCtrol::OnResetCounters(wxCommandEvent& event)
{
    ResetCounters();
}

void lmEarIntervalsCtrol::OnDisplaySolution(wxCommandEvent& event)
{
    DisplaySolution();
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
        //! @todo update counters & success sound 
        //SonidoAcierto
        //m_nRightAnswers = m_nRightAnswers + 1
    } else {
        pColor = &(g_pColors->Failure());
        //| @todo update counters & failure sound
        //SonidoFallo
        //m_nWrongAnswers = m_nWrongAnswers + 1
    }
        
    //update counters display
    //! @todo update counters display
////    Dim rAciertos As Single
////    rAciertos = 10# * CSng(m_nRightAnswers) / CSng(m_nRightAnswers + m_nWrongAnswers)
////    if (!m_lblRight Is Nothing) { m_lblRight.Caption = m_nRightAnswers
////    if (!m_lblWrong Is Nothing) { m_lblWrong.Caption = m_nWrongAnswers
////    if (!m_lblMark Is Nothing) {
////        if (rAciertos = 10#) {
////            m_lblMark.Caption = "10"
////        } else if { rAciertos = 0#) {
////            m_lblMark.Caption = "0"
////        } else {
////            m_lblMark.Caption = Format$(Round(rAciertos, 1), "#0.0")
////        }
////    }

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
    m_pScore->SetTopSystemDistance(5000);               //5mm
    m_pScore->AddInstrument(1,0,0);                     //one vstaff, MIDI channel 0, MIDI instr 0
    lmVStaff *pVStaff = m_pScore->GetVStaff(1, 1);      //get first vstaff of instr.1
    pVStaff->AddClef( nClef );
    pVStaff->AddKeySignature(nKey);
    pVStaff->AddTimeSignature(4 ,4, sbNO_VISIBLE );
//    pVStaff->AddEspacio 24
    pNode = parserLDP.ParseText( sPattern[0] );
    pNote[0] = parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddBarline(etbBarraNormal, sbNO_VISIBLE);    //so that accidental doesn't affect 2nd note
    pNode = parserLDP.ParseText( sPattern[1] );
    pNote[1] = parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddBarline(etbBarraFinal, sbNO_VISIBLE);

    //compute the right answer
    m_sAnswer = oIntv.GetName();
    if (oIntv.GetInterval() != 1) {
        m_sAnswer += (oIntv.IsAscending() ? _(", ascending") : _(", descending") );
    }
    m_ntMidi[0] = oIntv.GetMidiNote1();
    m_ntMidi[1] = oIntv.GetMidiNote2();
    
    //compute the index for the button that corresponds to the right answer
    for (i = 0; i <= m_nValidIntervals; i++) {
        if (m_nRealIntval[i] == oIntv.GetNumSemitones()) break;
    }
    m_nRespIndex = i;
    
    //play the interval
    m_pScoreCtrol->SetScore(m_pScore, true);            //true: the score must be hidden
    m_pScore = (lmScore*)NULL;    //no longer owned. Now owned by lmScoreAuxCtrol
    m_pScoreCtrol->DisplayMessage(_T(""), 0, true);     //true: clear the canvas
    Play();

    m_fPlayEnabled = true;
    m_fProblemCreated = true;
    EnableButtons(true);
    
    //! @todo Piano feedback
    //if (FMain.fFrmPiano) FPiano.DesmarcarTeclas

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

void lmEarIntervalsCtrol::ResetCounters()
{
    //! @todo When Counters implemented: code this method
//    m_nRightAnswers = 0;
//    m_nWrongAnswers = 0;
//    
//    m_lblRight = _T("0");
//    m_lblWrong = _T("0");
//    m_lblMark = _T("-");
    
}

/*
Private Sub MostrarArmadura()
    oPapel.IniciarHoja
    oPartArm.Dibujar oPapel, False
    
End Sub
*/
