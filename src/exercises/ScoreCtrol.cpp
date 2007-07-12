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

/*  A control to embed a score in an HTML page by using an \<object\> directive
    of type="Application/LenMusScore" 

    A control to display a score in an HTML page. It displays a score with three optional 
    links: Play, Solfa, and Play measures. Options are defined by constructor
    parameter lmScoreCtrolOptions

    - Score window: size = 90% of lmScoreCtrol window
    - Play link: under score, on the left
    - Solfa link: next to Play link. Gap 40px
    - Play measures: next to Solfa link. Gap 20px

    @todo
    For now it only displays Play link

*/
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "ScoreCtrol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "ScoreCtrol.h"
#include "UrlAuxCtrol.h"
#include "ScoreAuxCtrol.h"
#include "Constrains.h"

// access to global external variables
extern bool g_fReleaseVersion;            // in TheApp.cpp
extern bool g_fReleaseBehaviour;        // in TheApp.cpp
extern bool g_fShowDebugLinks;            // in TheApp.cpp


//Layout definitions
const int BUTTONS_DISTANCE    = 5;        //pixels

//IDs for controls
enum {
    ID_LINK_SEE_SOURCE = 3500,
    ID_LINK_DUMP,
    ID_LINK_MIDI_EVENTS,
    ID_LINK_PLAY,
    ID_LINK_SOLFA,
    ID_LINK_MEASURE         // AWARE this one must be the last one, as its ID is going
                            // to be incremented from 0 .. 9 to deal with the 10 possible
                            // 'play_measure' links
};


BEGIN_EVENT_TABLE(lmScoreCtrol, wxWindow)
    EVT_SIZE            (lmScoreCtrol::OnSize)
    LM_EVT_URL_CLICK    (ID_LINK_SEE_SOURCE, lmScoreCtrol::OnDebugShowSourceScore)
    LM_EVT_URL_CLICK    (ID_LINK_DUMP, lmScoreCtrol::OnDebugDumpScore)
    LM_EVT_URL_CLICK    (ID_LINK_MIDI_EVENTS, lmScoreCtrol::OnDebugShowMidiEvents)

    LM_EVT_URL_CLICK    (ID_LINK_PLAY, lmScoreCtrol::OnPlay)
    LM_EVT_URL_CLICK    (ID_LINK_SOLFA, lmScoreCtrol::OnSolfa)
    LM_EVT_URL_CLICK    (ID_LINK_MEASURE, lmScoreCtrol::OnPlayMeasure)
    LM_EVT_URL_CLICK    (ID_LINK_MEASURE+1, lmScoreCtrol::OnPlayMeasure)
    LM_EVT_URL_CLICK    (ID_LINK_MEASURE+2, lmScoreCtrol::OnPlayMeasure)
    LM_EVT_URL_CLICK    (ID_LINK_MEASURE+3, lmScoreCtrol::OnPlayMeasure)
    LM_EVT_URL_CLICK    (ID_LINK_MEASURE+4, lmScoreCtrol::OnPlayMeasure)
    LM_EVT_URL_CLICK    (ID_LINK_MEASURE+5, lmScoreCtrol::OnPlayMeasure)
    LM_EVT_URL_CLICK    (ID_LINK_MEASURE+6, lmScoreCtrol::OnPlayMeasure)
    LM_EVT_URL_CLICK    (ID_LINK_MEASURE+7, lmScoreCtrol::OnPlayMeasure)
    LM_EVT_URL_CLICK    (ID_LINK_MEASURE+8, lmScoreCtrol::OnPlayMeasure)
    LM_EVT_URL_CLICK    (ID_LINK_MEASURE+9, lmScoreCtrol::OnPlayMeasure)
    LM_EVT_END_OF_PLAY  (lmScoreCtrol::OnEndOfPlay)

END_EVENT_TABLE()

IMPLEMENT_CLASS(lmScoreCtrol, wxWindow)


lmScoreCtrol::lmScoreCtrol(wxWindow* parent, wxWindowID id, lmScore* pScore, 
                           lmScoreCtrolOptions* pOptions,
                           const wxPoint& pos, const wxSize& size, int style)
    : wxWindow(parent, id, pos, size, style )
{

    // save parameters
    m_pScore = pScore;
    m_pOptions = pOptions;

    //initializations
    SetBackgroundColour(*wxWHITE);
    m_pScoreCtrol = (lmScoreAuxCtrol*)NULL;
    m_fPlaying = false;
    m_pPlayLink = (lmUrlAuxCtrol*) NULL;
    m_pSolfaLink = (lmUrlAuxCtrol*) NULL;
    for (int i=0; i < 10; i++) {
        m_pMeasureLink[i] = (lmUrlAuxCtrol*) NULL;
    }

    //the window is divided into two regions: top, for score
    //and bottom region, for links
    wxBoxSizer* pMainSizer = new wxBoxSizer( wxVERTICAL );

    // create score ctrl 
    m_pScoreCtrol = new lmScoreAuxCtrol(this, -1, pScore, wxDefaultPosition, size,
                            (m_pOptions->fMusicBorder ? eSIMPLE_BORDER : eNO_BORDER) );
    pMainSizer->Add(m_pScoreCtrol, 1, wxGROW|wxALL, 5);

    m_pScoreCtrol->SetMargins(lmToLogicalUnits(10, lmMILLIMETERS),
                              lmToLogicalUnits(10, lmMILLIMETERS),
                              - lmToLogicalUnits(10, lmMILLIMETERS));
    m_pScoreCtrol->SetScale((float)m_pOptions->rScale);


    //
    //Optionally, add links
    //
    wxBoxSizer* pLinksSizer = new wxBoxSizer( wxHORIZONTAL );
    pMainSizer->Add(pLinksSizer, 0, wxALIGN_LEFT|wxALL, 5);

    // "play" link
    if (pOptions->fPlayCtrol)
    {
        m_pPlayLink = new lmUrlAuxCtrol(this, ID_LINK_PLAY, pOptions->sPlayLabel,
                                        pOptions->sStopPlayLabel );
        pLinksSizer->Add(m_pPlayLink,
                    0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
    }

    // "solfa" link
    if (pOptions->fSolfaCtrol)
    {
        m_pSolfaLink = new lmUrlAuxCtrol(this, ID_LINK_SOLFA, pOptions->sSolfaLabel,
                                         pOptions->sStopSolfaLabel );
        pLinksSizer->Add(m_pSolfaLink,
                    0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
    }

    /*! @todo Play_measure links are not positioned under each measure, as the lmScoreCtrol
        has no knowledge of measure boundaries. Perhaps this information have to be
        provided by the Score and the links have to be repositioned after the score
        is rendered.
        For now, play-measure links are positioned at regular intervals. This is
        the v2.0 behaviour
    */
    if (pOptions->fMeasuresCtrol)
    {
        int nNumMeasures = wxMin(pScore->GetNumMeasures(), 10);
        for (int i=0; i < nNumMeasures; i++) {
            m_pMeasureLink[i] =
                new lmUrlAuxCtrol(this, ID_LINK_MEASURE+i,
                        wxString::Format(pOptions->sMeasuresLabel, i+1),
                        wxString::Format(pOptions->sStopMeasureLabel, i+1) );
            pLinksSizer->Add(m_pMeasureLink[i],
                        0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);
        }
    }
    
    // debug buttons
    if (g_fShowDebugLinks && !g_fReleaseVersion) {
        wxBoxSizer* pDbgSizer = new wxBoxSizer( wxHORIZONTAL );
        pMainSizer->Add(pDbgSizer, 0, wxALIGN_LEFT|wxALL, 5);

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

    SetSizer( pMainSizer );                // use the sizer for window layout
    pMainSizer->SetSizeHints( this );    // set size hints to honour minimum size

}

lmScoreCtrol::~lmScoreCtrol()
{
    if (m_pScoreCtrol) {
        delete m_pScoreCtrol;
        m_pScoreCtrol = (lmScoreAuxCtrol*)NULL;
    }

    if (m_pOptions) {
        delete m_pOptions;
        m_pOptions = (lmScoreCtrolOptions*) NULL;
    }

    if (m_pScore) {
        delete m_pScore;
        m_pScore = (lmScore*)NULL;
    }

}


//----------------------------------------------------------------------------------------
// Event handlers

void lmScoreCtrol::OnSize(wxSizeEvent& event)
{
    //wxLogMessage(_T("OnSize en lmScoreCtrol"));
    Layout();

}

void lmScoreCtrol::OnPlay(wxCommandEvent& event)
{
    Play(ePM_NormalInstrument, m_pPlayLink);
}

void lmScoreCtrol::OnSolfa(wxCommandEvent& event)
{
    Play(ePM_RhythmHumanVoice, m_pSolfaLink);
}

void lmScoreCtrol::OnPlayMeasure(wxCommandEvent& event)
{
    int i = event.GetId() - ID_LINK_MEASURE;
    Play(ePM_NormalInstrument, m_pMeasureLink[i], i+1);
}

void lmScoreCtrol::Play(EPlayMode nPlayMode, lmUrlAuxCtrol* pLink, int nMeasure)
{
    if (!m_fPlaying) {
        // is not playing. "Play" pressed
        m_CurPlayLink = pLink;

        //change link from "Play" to "Stop playing" label
        pLink->SetAlternativeLabel();

        //play
        if (nMeasure == 0) {
            m_pScoreCtrol->PlayScore(lmVISUAL_TRACKING, NO_MARCAR_COMPAS_PREVIO, 
                                nPlayMode, m_pOptions->GetMetronomeMM());
        }
        else {
            m_pScoreCtrol->PlayMeasure(nMeasure, lmVISUAL_TRACKING,  
                                nPlayMode, m_pOptions->GetMetronomeMM());
        }
        m_fPlaying = true;

        //AWARE Link label is restored to Normal_label when the EndOfPlay event is
        //received. Flag m_fPlaying is also reset there
    }
    else {
        // is playing. "Stop playing" button pressed
        m_pScoreCtrol->Stop();
    }
    //! @todo Piano form
    //    if (FMain.fFrmPiano) { FPiano.HabilitarMarcado = false;
}

void lmScoreCtrol::OnEndOfPlay(lmEndOfPlayEvent& WXUNUSED(event))
{
    m_CurPlayLink->SetNormalLabel();
    m_fPlaying = false;
}

void lmScoreCtrol::OnDebugShowSourceScore(wxCommandEvent& event)
{
    m_pScoreCtrol->SourceLDP();
}

void lmScoreCtrol::OnDebugDumpScore(wxCommandEvent& event)
{
    m_pScoreCtrol->Dump();
}

void lmScoreCtrol::OnDebugShowMidiEvents(wxCommandEvent& event)
{
    m_pScoreCtrol->DumpMidiEvents();
}
