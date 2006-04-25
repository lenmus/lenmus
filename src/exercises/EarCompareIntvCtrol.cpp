// RCS-ID: $Id: EarCompareIntvCtrol.cpp,v 1.7 2006/02/23 19:19:15 cecilios Exp $
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
/*! @file EarCompareIntvCtrol.cpp
    @brief Implementation file for class lmEarCompareIntvCtrol
    @ingroup html_controls
*/
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "EarCompareIntvCtrol.h"
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

#include "EarCompareIntvCtrol.h"
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
// Implementation of lmEarCompareIntvCtrol




//Layout definitions
const int BUTTONS_DISTANCE    = 5;        //pixels
const int NUM_LINKS = 3;                //links for actions

//IDs for controls
enum {
    ID_LINK_SEE_SOURCE = 3000,
    ID_LINK_DUMP,
    ID_LINK_MIDI_EVENTS,
    ID_BUTTON,
    ID_LINK = ID_BUTTON + 3,
    ID_LINK_NEW_PROBLEM,
    ID_LINK_RESET_COUNTERS,
    ID_LINK_PLAY,
    ID_LINK_SOLUTION,
    ID_LINK_SETTINGS

};


BEGIN_EVENT_TABLE(lmEarCompareIntvCtrol, wxWindow)
    EVT_COMMAND_RANGE (ID_BUTTON, ID_BUTTON+2, wxEVT_COMMAND_BUTTON_CLICKED, lmEarCompareIntvCtrol::OnRespButton)
    EVT_SIZE            (lmEarCompareIntvCtrol::OnSize)

    LM_EVT_URL_CLICK    (ID_LINK_SEE_SOURCE, lmEarCompareIntvCtrol::OnDebugShowSourceScore)
    LM_EVT_URL_CLICK    (ID_LINK_DUMP, lmEarCompareIntvCtrol::OnDebugDumpScore)
    LM_EVT_URL_CLICK    (ID_LINK_MIDI_EVENTS, lmEarCompareIntvCtrol::OnDebugShowMidiEvents)

    LM_EVT_URL_CLICK    (ID_LINK_NEW_PROBLEM, lmEarCompareIntvCtrol::OnNewProblem)
    LM_EVT_URL_CLICK    (ID_LINK_RESET_COUNTERS, lmEarCompareIntvCtrol::OnResetCounters)
    LM_EVT_URL_CLICK    (ID_LINK_PLAY, lmEarCompareIntvCtrol::OnPlay)
    LM_EVT_URL_CLICK    (ID_LINK_SOLUTION, lmEarCompareIntvCtrol::OnDisplaySolution)
    LM_EVT_URL_CLICK    (ID_LINK_SETTINGS, lmEarCompareIntvCtrol::OnSettingsButton)
END_EVENT_TABLE()

IMPLEMENT_CLASS(lmEarCompareIntvCtrol, wxWindow)

lmEarCompareIntvCtrol::lmEarCompareIntvCtrol(wxWindow* parent, wxWindowID id, 
                           lmEarIntervalsConstrains* pConstrains,
                           const wxPoint& pos, const wxSize& size, int style)
    : wxWindow(parent, id, pos, size, style )
{
    //initializations
    int i;
    for (i=0; i < 3; i++) { m_pAnswerButton[i] = (wxButton*)NULL; }
    EnableButtons(false);
    m_fProblemCreated = false;
    m_fPlayEnabled = false;
    m_pScore[0] = (lmScore*)NULL;
    m_pScore[1] = (lmScore*)NULL;
    m_pScoreCtrol = (lmScoreAuxCtrol*)NULL;
    m_pConstrains = pConstrains;

    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    wxString sBtLabel[3];
    sBtLabel[0] = _("First one greater");
    sBtLabel[1] = _("Second one greater");
    sBtLabel[2] = _("Both are equal");

    //the window is divided into two regions: top, for score on left and counters and links
    //on the right, and bottom region, for answer buttons 
    wxBoxSizer* pMainSizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer* pTopSizer = new wxBoxSizer( wxHORIZONTAL );

    // Inside TopSizer we set up a vertical sizer: score on top, debugg links bottom
    wxBoxSizer* pScoreSizer = new wxBoxSizer( wxVERTICAL );
    pTopSizer->Add(pScoreSizer, 0, wxALIGN_LEFT|wxALL, 5);

    // create score ctrl 
    m_pScoreCtrol = new lmScoreAuxCtrol(this, -1, (lmScore*)NULL, wxDefaultPosition, wxSize(420,200), eSIMPLE_BORDER);
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
    m_pScoreCtrol->SetMargins(
            lmToLogicalUnits(1, lmCENTIMETERS),
            lmToLogicalUnits(1, lmCENTIMETERS),
            lmToLogicalUnits(2, lmCENTIMETERS));        //right=1cm, left=1cm, top=2cm
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

    //create 3 buttons for the answers, in one row
    wxBoxSizer* pRowSizer = new wxBoxSizer( wxHORIZONTAL );
    wxButton* pButton;
    int iB = 0;
    pRowSizer->Add(20+BUTTONS_DISTANCE, 24, 0);    //spacer to center labels

    //CrearControlTexto LitIdioma("03018")    '03018="¿Qué intervalo es mayor?"

    for (iB=0; iB < 3; iB++) {
        pButton = new wxButton( this, ID_BUTTON + iB, sBtLabel[iB],
            wxDefaultPosition, wxSize(134, 24));
        m_pAnswerButton[iB] = pButton;
        pRowSizer->Add(
            pButton,
            wxSizerFlags(0).Border(wxALL, BUTTONS_DISTANCE) );
    }
    pMainSizer->Add(    
        pRowSizer,
        wxSizerFlags(0).Left());

    SetSizer( pMainSizer );                 // use the sizer for window layout
    pMainSizer->SetSizeHints( this );       // set size hints to honour minimum size

    m_pScoreCtrol->DisplayMessage(_("Click on 'New problem' to start"), 1000, true);

}

lmEarCompareIntvCtrol::~lmEarCompareIntvCtrol()
{
    if (m_pScoreCtrol) {
        delete m_pScoreCtrol;
        m_pScoreCtrol = (lmScoreAuxCtrol*)NULL;
    }

    if (m_pConstrains) {
        delete m_pConstrains;
        m_pConstrains = (lmEarIntervalsConstrains*) NULL;
    }

    if (m_pScore[0]) {
        delete m_pScore[0];
        m_pScore[0] = (lmScore*)NULL;
    }
    if (m_pScore[1]) {
        delete m_pScore[1];
        m_pScore[1] = (lmScore*)NULL;
    }
}

void lmEarCompareIntvCtrol::EnableButtons(bool fEnable)
{
    for (int i=0; i < 3; i++) {
        if (m_pAnswerButton[i])
            m_pAnswerButton[i]->Enable(fEnable);
    }
    m_fButtonsEnabled = fEnable;

}

//----------------------------------------------------------------------------------------
// Event handlers

void lmEarCompareIntvCtrol::OnSettingsButton(wxCommandEvent& event)
{
    lmDlgCfgEarIntervals dlg(this, m_pConstrains, true);    // true -> enable First note equal checkbox
    int retcode = dlg.ShowModal();
    if (retcode == wxID_OK) m_pConstrains->SaveSettings();

}

void lmEarCompareIntvCtrol::OnSize(wxSizeEvent& event)
{
    //wxLogMessage(_T("OnSize at lmEarCompareIntvCtrol"));
    Layout();

}

void lmEarCompareIntvCtrol::OnPlay(wxCommandEvent& event)
{
    Play();
}

void lmEarCompareIntvCtrol::OnNewProblem(wxCommandEvent& event)
{
    NewProblem();
}

void lmEarCompareIntvCtrol::OnResetCounters(wxCommandEvent& event)
{
    ResetCounters();
}

void lmEarCompareIntvCtrol::OnDisplaySolution(wxCommandEvent& event)
{
    DisplaySolution();
}

void lmEarCompareIntvCtrol::OnRespButton(wxCommandEvent& event)
{
    int nIndex = event.GetId() - ID_BUTTON;

    //show the solucion
    DisplaySolution();
    EnableButtons(true);

    //verify if success or failure
    bool fSecondGreater = !(m_fFirstGreater || m_fBothEqual);
    bool fSuccess = (nIndex == 0 && m_fFirstGreater) ||
               (nIndex == 1 && fSecondGreater) ||
               (nIndex == 2 && m_fBothEqual);
                

    //produce feedback sound, and update counters
    if (fSuccess) {
        //! @todo update counters & success sound 
        //SonidoAcierto
        //m_nRightAnswers++;
    } else {
        //| @todo update counters & failure sound
        //SonidoFallo
        //m_nWrongAnswers++;
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

    // if failure mark pushed button in red
    if (!fSuccess) {
        m_pAnswerButton[nIndex]->SetBackgroundColour(g_pColors->Failure());
    }

}

void lmEarCompareIntvCtrol::NewProblem()
{

    ResetExercise();
    
    //
    //generate the two intervals to compare
    //

    EClefType nClef = eclvSol;

    // select interval type: ascending, descending or both
    lmRandomGenerator oGenerator;
    EIntervalDirection nDir;
    if (m_pConstrains->IsTypeAllowed(0) || 
        (m_pConstrains->IsTypeAllowed(1) && m_pConstrains->IsTypeAllowed(2)))
    {
        // if harmonic scale or melodic ascending and descending,
        // both, ascending and descending, are allowed. Choose one randomly
        nDir = (oGenerator.FlipCoin() ? edi_Ascending : edi_Descending );
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
    EKeySignatures nKey;
    if (m_pConstrains->OnlyNatural()) {
        nKey = oGenerator.GenerateKey(m_pConstrains->GetKeyConstrains());
    }
    else {
        nKey = earmDo;
    }
    // generate the two intervals
    lmInterval oIntv0(m_pConstrains->OnlyNatural(), m_pConstrains->MinNote(),
        m_pConstrains->MaxNote(), m_pConstrains->AllowedIntervals(), nDir, nKey);
    lmInterval oIntv1(m_pConstrains->OnlyNatural(), m_pConstrains->MinNote(),
        m_pConstrains->MaxNote(), m_pConstrains->AllowedIntervals(), nDir, nKey);

    //Convert problem to LDP pattern
    wxString sPattern[2][2];
    int i;
    for (i=0; i < 2; i++) {
        sPattern[0][i] = _T("(n ") + oIntv0.GetPattern(i) + _T(" r)");
        sPattern[1][i] = _T("(n ") + oIntv1.GetPattern(i) + _T(" r)");
    }

    //
    //create two scores, one for each interval to be played, and a third score with
    //both intervals for displaying the solution
    //

    lmNote* pNote[2];
    lmLDPParser parserLDP;
    lmLDPNode* pNode;
    lmVStaff* pVStaff;

    //create the two single-interval scores
    for (i=0; i<2; i++) {
        m_pScore[i] = new lmScore();
        m_pScore[i]->SetTopSystemDistance( lmToLogicalUnits(5, lmMILLIMETERS) ); //5mm
        m_pScore[i]->AddInstrument(1,0,0);                     //one vstaff, MIDI channel 0, MIDI instr 0
        pVStaff = m_pScore[i]->GetVStaff(1, 1);      //get first vstaff of instr.1
        pVStaff->AddClef( nClef );
        pVStaff->AddKeySignature(nKey);
        pVStaff->AddTimeSignature(4 ,4, sbNO_VISIBLE );
    //    pVStaff->AddEspacio 24
        pNode = parserLDP.ParseText( sPattern[i][0] );
        pNote[0] = parserLDP.AnalyzeNote(pNode, pVStaff);
        pVStaff->AddBarline(etbBarraNormal, sbNO_VISIBLE);    //so that accidental doesn't affect 2nd note
        pNode = parserLDP.ParseText( sPattern[i][1] );
        pNote[1] = parserLDP.AnalyzeNote(pNode, pVStaff);
        pVStaff->AddBarline(etbBarraFinal, sbNO_VISIBLE);
    }

    //create the answer score with both intervals
    lmScore* pTotalScore = new lmScore();
    pTotalScore->SetTopSystemDistance( lmToLogicalUnits(5, lmMILLIMETERS) );    //5mm
    pTotalScore->AddInstrument(1,0,0);                     //one vstaff, MIDI channel 0, MIDI instr 0
    pVStaff = pTotalScore->GetVStaff(1, 1);      //get first vstaff of instr.1
    pVStaff->AddClef( nClef );
    pVStaff->AddKeySignature(nKey);
    pVStaff->AddTimeSignature(4 ,4, sbNO_VISIBLE );

//    pVStaff->AddEspacio 24
    pNode = parserLDP.ParseText( sPattern[0][0] );
    pNote[0] = parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddBarline(etbBarraNormal, sbNO_VISIBLE);    //so that accidental doesn't affect 2nd note
    pNode = parserLDP.ParseText( sPattern[0][1] );
    pNote[1] = parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddBarline(etbBarraDoble);

//    pVStaff->AddEspacio 24
    pNode = parserLDP.ParseText( sPattern[1][0] );
    parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddBarline(etbBarraNormal, sbNO_VISIBLE);    //so that accidental doesn't affect 2nd note
    pNode = parserLDP.ParseText( sPattern[1][1] );
    parserLDP.AnalyzeNote(pNode, pVStaff);
    pVStaff->AddBarline(etbBarraFinal, sbNO_VISIBLE);    //so that accidental doesn't affect 2nd note

    //compute the right answer
    m_sAnswer[0] = oIntv0.GetName();
    m_sAnswer[1] = oIntv1.GetName();
    m_fFirstGreater = (oIntv0.GetNumSemitones() > oIntv1.GetNumSemitones());
    m_fBothEqual = (oIntv0.GetNumSemitones() == oIntv1.GetNumSemitones());
    
    //load total score into the control
    m_pScoreCtrol->SetScore(pTotalScore, true);   //true: the score must be hidden
    pTotalScore = (lmScore*)NULL;         //no longer owned. Now owned by lmScoreAuxCtrol
    m_pScoreCtrol->DisplayMessage(_T(""), 0, true);     //true: clear the canvas

    m_fPlayEnabled = true;
    m_fProblemCreated = true;
    EnableButtons(true);

    //play the interval
    Play();

    //! @todo Piano feedback
    //if (FMain.fFrmPiano) FPiano.DesmarcarTeclas

}

void lmEarCompareIntvCtrol::Play()
{
    //@attention As the intervals are built using whole notes, we will play them at MM=240 so
    //that real note rate will be 80.

    //first interval
#if 0
    m_pScoreCtrol->SetScore(m_pScore[0], true);   //true: the score must be hidden
    m_pAnswerButton[0]->SetBackgroundColour( g_pColors->ButtonHighlight() );
    m_pScoreCtrol->PlayScore(lmNO_VISUAL_TRACKING, NO_MARCAR_COMPAS_PREVIO, 
                             ePM_NormalInstrument, 320);
    //m_pAnswerButton[0]->SetBackgroundColour( g_pColors->Normal() );
#else
    m_pAnswerButton[0]->SetBackgroundColour( g_pColors->ButtonHighlight() );
    m_pAnswerButton[0]->Update();    //Refresh works vie events and, so, it is not inmediate
    m_pScore[0]->Play(lmNO_VISUAL_TRACKING, NO_MARCAR_COMPAS_PREVIO, 
                             ePM_NormalInstrument, 320);
    m_pScore[0]->WaitForTermination();
    m_pAnswerButton[0]->SetBackgroundColour( g_pColors->Normal() );
    m_pAnswerButton[0]->Update();

    wxMilliSleep(1000);     //wait for 1sec (1000ms)

    //second interval
    m_pAnswerButton[1]->SetBackgroundColour( g_pColors->ButtonHighlight() );
    m_pAnswerButton[1]->Update();
    m_pScore[1]->Play(lmNO_VISUAL_TRACKING, NO_MARCAR_COMPAS_PREVIO, 
                             ePM_NormalInstrument, 320);
    m_pScore[1]->WaitForTermination();
    m_pAnswerButton[1]->SetBackgroundColour( g_pColors->Normal() );
    m_pAnswerButton[1]->Update();
#endif
}

void lmEarCompareIntvCtrol::DisplaySolution()
{
    m_pScoreCtrol->HideScore(false);
    
    // mark right button in green
    if (m_fFirstGreater) 
        m_pAnswerButton[0]->SetBackgroundColour(g_pColors->Success());
    else if (m_fBothEqual)
        m_pAnswerButton[2]->SetBackgroundColour(g_pColors->Success());
    else
        m_pAnswerButton[1]->SetBackgroundColour(g_pColors->Success());
    

    //if piano visile, mark the notes
    //! @todo piano feedback
    //Dim colorX As Long
    //If FMain.fFrmPiano Then
    //    With FPiano
    //        .HabilitarMarcado = True
    //        .DesmarcarTeclas
    //        colorX = IIf(m_fFirstGreater, colorVerde, colorRojo)
    //        .MarcarTecla oIntv(0).NotaMidi1, colorX
    //        .MarcarTecla oIntv(0).NotaMidi2, colorX
    //        colorX = IIf(m_fFirstGreater, colorRojo, colorVerde)
    //        .MarcarTecla oIntv(1).NotaMidi1, colorX
    //        .MarcarTecla oIntv(1).NotaMidi2, colorX
    //        .HabilitarMarcado = False
    //    End With
    //End If

    m_fPlayEnabled = true;
    m_fProblemCreated = false;
    
}

void lmEarCompareIntvCtrol::OnDebugShowSourceScore(wxCommandEvent& event)
{
    m_pScoreCtrol->SourceLDP();
}

void lmEarCompareIntvCtrol::OnDebugDumpScore(wxCommandEvent& event)
{
    m_pScoreCtrol->Dump();
}

void lmEarCompareIntvCtrol::OnDebugShowMidiEvents(wxCommandEvent& event)
{
    m_pScoreCtrol->DumpMidiEvents();
}

void lmEarCompareIntvCtrol::ResetExercise()
{
    //clear the canvas
    m_pScoreCtrol->DisplayMessage(_T(""), 0, true);     //true: clear the canvas
    m_pScoreCtrol->Update();    //to force to clear it now

    // restore buttons' normal color
    for (int i=0; i < 3; i++) {
        if (m_pAnswerButton[i]) {
            m_pAnswerButton[i]->SetBackgroundColour( g_pColors->Normal() );
        }
    }
    EnableButtons(false);

    //delete the previous scores
    if (m_pScore[0]) {
        delete m_pScore[0];
        m_pScore[0] = (lmScore*)NULL;
    }
    if (m_pScore[1]) {
        delete m_pScore[1];
        m_pScore[1] = (lmScore*)NULL;
    }
    
}

void lmEarCompareIntvCtrol::ResetCounters()
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