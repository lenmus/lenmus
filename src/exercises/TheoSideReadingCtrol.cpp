// RCS-ID: $Id: TheoSideReadingCtrol.cpp,v 1.15 2006/03/03 14:59:44 cecilios Exp $
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
/*! @file TheoSideReadingCtrol.cpp
    @brief Implementation file for class lmTheoSideReadingCtrol
    @ingroup html_controls
*/
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "TheoSideReadingCtrol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/statline.h"

#include "TheoSideReadingCtrol.h"
#include "UrlAuxCtrol.h"
#include "ScoreAuxCtrol.h"
#include "Constrains.h"
#include "Generators.h"
#include "../auxmusic/Conversion.h"
#include "../ldp_parser/LDPParser.h"
#include "../score/score.h"
#include "../auxmusic/ComposerV5.h"
#include "../app/DlgCfgScoreReading.h"
#include "../app/MainFrame.h"
#include "../html/TextBookController.h"


// access to global functions
extern void ComputeAccidentals(EKeySignatures nKeySignature, int nAccidentals[]);
extern int GetRootNoteIndex(EKeySignatures nKeySignature);
extern const wxString& GetKeySignatureName(EKeySignatures nKeySignature);

// access to global external variables
extern bool g_fReleaseVersion;            // in TheApp.cpp
extern bool g_fReleaseBehaviour;        // in TheApp.cpp
extern bool g_fShowDebugLinks;            // in TheApp.cpp

// access to main frame to get text book controller
extern lmMainFrame *GetMainFrame();


//------------------------------------------------------------------------------------
// Implementation of lmTheoSideReadingCtrol
//------------------------------------------------------------------------------------

//IDs for controls
enum {
    ID_LINK_NEW_PROBLEM = 100,
    ID_LINK_PLAY,
    ID_LINK_SEE_SOURCE,
    ID_LINK_DUMP,
    ID_LINK_MIDI_EVENTS,
    ID_LINK_SETTINGS,
    ID_LINK_GO_BACK,
};


BEGIN_EVENT_TABLE(lmTheoSideReadingCtrol, wxWindow)
    EVT_SIZE            (lmTheoSideReadingCtrol::OnSize)
    LM_EVT_URL_CLICK    (ID_LINK_NEW_PROBLEM, lmTheoSideReadingCtrol::OnNewProblem)
    LM_EVT_URL_CLICK    (ID_LINK_PLAY, lmTheoSideReadingCtrol::OnPlay)
    LM_EVT_URL_CLICK    (ID_LINK_SEE_SOURCE, lmTheoSideReadingCtrol::OnDebugShowSourceScore)
    LM_EVT_URL_CLICK    (ID_LINK_DUMP, lmTheoSideReadingCtrol::OnDebugDumpScore)
    LM_EVT_URL_CLICK    (ID_LINK_MIDI_EVENTS, lmTheoSideReadingCtrol::OnDebugShowMidiEvents)
    LM_EVT_URL_CLICK    (ID_LINK_SETTINGS, lmTheoSideReadingCtrol::OnSettingsButton)
    LM_EVT_URL_CLICK    (ID_LINK_GO_BACK, lmTheoSideReadingCtrol::OnGoBackButton)
    LM_EVT_END_OF_PLAY  (lmTheoSideReadingCtrol::OnEndOfPlay)
    EVT_CLOSE           (lmTheoSideReadingCtrol::OnClose)
END_EVENT_TABLE()

IMPLEMENT_CLASS(lmTheoSideReadingCtrol, wxWindow)

lmTheoSideReadingCtrol::lmTheoSideReadingCtrol(wxWindow* parent, wxWindowID id,
                           lmSideReadingCtrolOptions* pOptions,
                           lmScoreConstrains* pConstrains,
                           const wxPoint& pos, const wxSize& size, int style)
    : wxWindow(parent, id, pos, size, style )
{
    //initializations
    m_fProblemCreated = false;
    m_fPlayEnabled = false;
    m_pScore = (lmScore*)NULL;
    m_pScoreCtrol = (lmScoreAuxCtrol*)NULL;
    m_pConstrains = pConstrains;
    m_pOptions = pOptions;
    m_fPlaying = false;
    m_fClosing = false;

    //the window is divided into two regions: top for links, and bottom for the score
    wxBoxSizer* pMainSizer = new wxBoxSizer( wxVERTICAL );

    // buttons and links
    wxBoxSizer* pButtonsSizer = new wxBoxSizer( wxHORIZONTAL );
    pMainSizer->Add(
        pButtonsSizer,
        wxSizerFlags(0).Left().Expand().Border(wxLEFT|wxRIGHT|wxTOP, 5) );

    // "Go back to theory" button
    if (pOptions->fGoBackLink) {
        pButtonsSizer->Add(
            new lmUrlAuxCtrol(this, ID_LINK_GO_BACK, _("Go back to theory") ),
            wxSizerFlags(0).Left().Border(wxALL, 5) );
    }

    // "new problem" button
    pButtonsSizer->Add(
        new lmUrlAuxCtrol(this, ID_LINK_NEW_PROBLEM, _("New problem") ),
        wxSizerFlags(0).Left().Border(wxALL, 5) );
    
    // "play" button
    m_pPlayLink = new lmUrlAuxCtrol(this, ID_LINK_PLAY, m_pOptions->sPlayLabel, m_pOptions->sStopPlayLabel );
    pButtonsSizer->Add(
        m_pPlayLink,
        wxSizerFlags(0).Left().Border(wxALL, 5) );
    
    // "solfa" button
    if (pOptions->fSolfaCtrol) {
        //m_pSolfaLink = new lmUrlAuxCtrol(this, ID_LINK_SOLFA, m_pOptions->sSolfaLabel, m_pOptions->sStopSolfaLabel );
        //pButtonsSizer->Add(
        //    m_pPlayLink,
        //    wxSizerFlags(0).Left().Border(wxALL, 5) );
    }

    // debug buttons
    if (g_fShowDebugLinks && !g_fReleaseVersion) {
        // "See source score"
        pButtonsSizer->Add(
            new lmUrlAuxCtrol(this, ID_LINK_SEE_SOURCE, _("See source score") ),
            wxSizerFlags(0).Left().Border(wxALL, 5) );
        // "Dump score"
        pButtonsSizer->Add(
            new lmUrlAuxCtrol(this, ID_LINK_DUMP, _("Dump score") ),
            wxSizerFlags(0).Left().Border(wxALL, 5) );
        // "See MIDI events"
        pButtonsSizer->Add(
            new lmUrlAuxCtrol(this, ID_LINK_MIDI_EVENTS, _("See MIDI events") ),
            wxSizerFlags(0).Left().Border(wxALL, 5) );
    }

    // spacer
    pButtonsSizer->Add(5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    // settings link
    if (pOptions->fSettingsLink) {
        m_pSettingsLink = new lmUrlAuxCtrol(this, ID_LINK_SETTINGS, _("Settings") );
        pButtonsSizer->Add(m_pSettingsLink, wxSizerFlags(0).Left().Border(wxALL, 5) );
    }

    // horizontal line
    wxStaticLine* pStaticLine1 = new wxStaticLine(this, wxID_ANY, 
                            wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    ((wxWindow*)pStaticLine1)->SetBackgroundColour(*wxRED);
    ((wxWindow*)pStaticLine1)->SetForegroundColour(*wxBLUE);
    pMainSizer->Add(pStaticLine1, 0, wxGROW|wxALL, 5);


    // create score ctrl 
    wxBoxSizer* pScoreSizer = new wxBoxSizer( wxHORIZONTAL );
    pMainSizer->Add(
        pScoreSizer,
        wxSizerFlags(0).Left().Expand().Border(wxALL, 5) );

    m_pScoreCtrol = new lmScoreAuxCtrol(this, -1, m_pScore, wxDefaultPosition, 
        wxSize(600,800), eNO_BORDER);
    m_pScoreCtrol->SetMargins(10000, 10000, 10000);        //right=1cm, left=1cm, top=1cm
    m_pScoreCtrol->SetScale((float)1.3);
    pScoreSizer->Add(
        m_pScoreCtrol,
        wxSizerFlags(1).Left().Border(0, 5));

    SetSizer( pMainSizer );                // use the sizer for window layout
    pMainSizer->SetSizeHints( this );        // set size hints to honour minimum size

    NewProblem();

}


lmTheoSideReadingCtrol::~lmTheoSideReadingCtrol()
{
    if (m_pScoreCtrol) {
        delete m_pScoreCtrol;
        m_pScoreCtrol = (lmScoreAuxCtrol*)NULL;
    }

    if (m_pConstrains) {
        delete m_pConstrains;
        m_pConstrains = (lmScoreConstrains*) NULL;
    }

    if (m_pOptions) {
        delete m_pOptions;
        m_pOptions = (lmSideReadingCtrolOptions*) NULL;
    }

    if (m_pScore) {
        delete m_pScore;
        m_pScore = (lmScore*)NULL;
    }
}


//----------------------------------------------------------------------------------------
// Event handlers

void lmTheoSideReadingCtrol::OnGoBackButton(wxCommandEvent& event)
{
    //wxLogMessage(_T("[lmTheoSideReadingCtrol::OnGoBackButton] back URL = '%s'"), m_pOptions->sGoBackURL);
    lmMainFrame* pFrame = GetMainFrame();
    lmTextBookController* pBookController = pFrame->GetBookController(); 
    pBookController->Display( m_pOptions->sGoBackURL );
}

void lmTheoSideReadingCtrol::OnSettingsButton(wxCommandEvent& event)
{
    lmDlgCfgScoreReading dlg(this, m_pConstrains, m_pOptions->sSettingsKey);
    int retcode = dlg.ShowModal();
    if (retcode == wxID_OK) m_pConstrains->SaveSettings();
}

void lmTheoSideReadingCtrol::OnClose(wxCloseEvent& event)
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

void lmTheoSideReadingCtrol::OnSize(wxSizeEvent& event)
{
    Layout();
}

void lmTheoSideReadingCtrol::OnPlay(wxCommandEvent& event)
{
    Play();
}

void lmTheoSideReadingCtrol::OnNewProblem(wxCommandEvent& event)
{
    if (m_fPlaying) return;
    NewProblem();
}

void lmTheoSideReadingCtrol::NewProblem()
{
    //Generate a random score
    lmComposer5 oComposer;
    m_pScore = oComposer.GenerateScore(m_pConstrains);

    //display the score
    m_pScoreCtrol->DisplayScore(m_pScore);
    m_pScore = (lmScore*)NULL;    //no longer owned. Now owned by lmScoreAuxCtrol
    m_fPlayEnabled = true;
    m_fProblemCreated = true;
    
    //! @todo piano dlg
    //if (FMain.fFrmPiano) { FPiano.DesmarcarTeclas
        
}

/*! Playback the score.
    Play() method is called either to play or to stop playing. 
*/
void lmTheoSideReadingCtrol::Play()
{
    
    if (!m_fPlaying) {
        // Play button pressed

        //change link from "Play" to "Stop playing"
        m_pPlayLink->SetAlternativeLabel();

        //play score
        m_pScoreCtrol->PlayScore(lmVISUAL_TRACKING, NO_MARCAR_COMPAS_PREVIO, 
                                ePM_NormalInstrument);
        m_fPlaying = true;

        //@attention The link label is restored to "Play" when the EndOfPlay event is
        //received. Flag m_fPlaying is also reset there
    }
    else {
        // "Stop playing" button pressed
        m_pScoreCtrol->Stop();
    }
    //! @todo Piano form
//    if (FMain.fFrmPiano) { FPiano.HabilitarMarcado = false;

}

void lmTheoSideReadingCtrol::OnEndOfPlay(lmEndOfPlayEvent& WXUNUSED(event))
{
    m_pPlayLink->SetNormalLabel();
    m_fPlaying = false;
}

void lmTheoSideReadingCtrol::OnDebugShowSourceScore(wxCommandEvent& event)
{
    m_pScoreCtrol->SourceLDP();
}

void lmTheoSideReadingCtrol::OnDebugDumpScore(wxCommandEvent& event)
{
    m_pScoreCtrol->Dump();
}

void lmTheoSideReadingCtrol::OnDebugShowMidiEvents(wxCommandEvent& event)
{
    m_pScoreCtrol->DumpMidiEvents();
}
