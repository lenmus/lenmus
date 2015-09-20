//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2011 LenMus project
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation,
//    either version 3 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program. If not, see <http://www.gnu.org/licenses/>.
//
//    For any comment, suggestion or feature request, please contact the manager of
//    the project at cecilios@users.sourceforge.net
//
//---------------------------------------------------------------------------------------

///*  A control to embed a score in an HTML page by using an \<object\> directive
//    of type="Application/LenMusScore"
//
//    A control to display a score in an HTML page. It displays a score with three optional
//    links: Play, Solfa, and Play measures. Options are defined by constructor
//    parameter ScoreCtrolOptions
//
//    - Score window: size = 90% of ScoreCtrol window
//    - Play link: under score, on the left
//    - Solfa link: next to Play link. Gap 40px
//    - Play measures: next to Solfa link. Gap 20px
//
//    TODO
//    For now it only displays Play link
//
//*/
//#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
//#pragma implementation "ScoreCtrol.h"
//#endif
//
//// For compilers that support precompilation, includes <wx.h>.
//#include <wx/wxprec.h>
//
//#ifdef __BORLANDC__
//#pragma hdrstop
//#endif
//
//#include "ScoreCtrol.h"
//#include "lenmus_url_aux_ctrol.h"
//#include "auxctrols/ScoreAuxCtrol.h"
//#include "lenmus_constrains.h"


namespace lenmus
{

////IDs for controls
//enum {
//    ID_LINK_SOLFA = 3500,
//    ID_LINK_MEASURE         // AWARE this one must be the last one, as its ID is going
//                            // to be incremented from 0 .. 9 to deal with the 10 possible
//                            // 'play_measure' links
//};
//
//
//BEGIN_EVENT_TABLE(ScoreCtrol, lmEBookCtrol)
//    LM_EVT_URL_CLICK    (ID_LINK_SOLFA, ScoreCtrol::OnSolfa)
//    LM_EVT_URL_CLICK    (ID_LINK_MEASURE, ScoreCtrol::OnPlayMeasure)
//    LM_EVT_URL_CLICK    (ID_LINK_MEASURE+1, ScoreCtrol::OnPlayMeasure)
//    LM_EVT_URL_CLICK    (ID_LINK_MEASURE+2, ScoreCtrol::OnPlayMeasure)
//    LM_EVT_URL_CLICK    (ID_LINK_MEASURE+3, ScoreCtrol::OnPlayMeasure)
//    LM_EVT_URL_CLICK    (ID_LINK_MEASURE+4, ScoreCtrol::OnPlayMeasure)
//    LM_EVT_URL_CLICK    (ID_LINK_MEASURE+5, ScoreCtrol::OnPlayMeasure)
//    LM_EVT_URL_CLICK    (ID_LINK_MEASURE+6, ScoreCtrol::OnPlayMeasure)
//    LM_EVT_URL_CLICK    (ID_LINK_MEASURE+7, ScoreCtrol::OnPlayMeasure)
//    LM_EVT_URL_CLICK    (ID_LINK_MEASURE+8, ScoreCtrol::OnPlayMeasure)
//    LM_EVT_URL_CLICK    (ID_LINK_MEASURE+9, ScoreCtrol::OnPlayMeasure)
//    LM_EVT_END_OF_PLAYBACK  (ScoreCtrol::OnEndOfPlay)
//
//END_EVENT_TABLE()
//
//IMPLEMENT_CLASS(ScoreCtrol, lmEBookCtrol)
//
//
//ScoreCtrol::ScoreCtrol(wxWindow* parent, wxWindowID id, ImoScore* pScore,
//                           ScoreCtrolOptions* pOptions,
//                           const wxPoint& pos, const wxSize& size, int style)
//    : lmEBookCtrol(parent, id, pOptions, pos, size, style)
//{
//
//    // save parameters
//    m_pScore = pScore;
//    m_nScoreSize = size;
//    m_pOptions = pOptions;
//
//    //initializations
//    SetBackgroundColour(*wxWHITE);
//    m_pScoreCtrol = (ImoScoreAuxCtrol*)NULL;
//    m_fPlaying = false;
//    m_pPlayLink = (UrlAuxCtrol*) NULL;
//    m_pSolfaLink = (UrlAuxCtrol*) NULL;
//    for (int i=0; i < 10; i++) {
//        m_pMeasureLink[i] = (UrlAuxCtrol*) NULL;
//    }
//
//    create_controls();
//
//}
//
//void ScoreCtrol::CreateControls()
//{
//
//        //Create the controls
//
//    //ensure that sizes are properly scaled.
//    //not necessary to scale the size as it is received alreday scaled
//    double rScale = m_pOptions->rScale;
//    double rTopMargin = m_pOptions->rTopMargin;
//    int nSpacing = (int)(5.0 * rScale + 0.5);       //5 pixels, scaled
//
//    //the window is divided into two regions: top, for score
//    //and bottom region, for links
//    wxBoxSizer* pMainSizer = new wxBoxSizer( wxVERTICAL );
//
//    // create score ctrl
//    m_pScoreCtrol = new ImoScoreAuxCtrol(this, -1, m_pScore, wxDefaultPosition, m_nScoreSize,
//                            (m_pOptions->fMusicBorder ? eSIMPLE_BORDER : eNO_BORDER) );
//    pMainSizer->Add(m_pScoreCtrol, 1, wxGROW, nSpacing);
//
//    m_pScore->SetTopSystemDistance( lmToLogicalUnits(10, lmMILLIMETERS) );
//
//    m_pScoreCtrol->SetMargins(lmToLogicalUnits(10, lmMILLIMETERS) * rScale,
//                              lmToLogicalUnits(10, lmMILLIMETERS) * rScale,
//                              lmToLogicalUnits(rTopMargin, lmMILLIMETERS) * rScale);
//    m_pScoreCtrol->SetScale( (float)rScale * m_pScoreCtrol->GetScale() );
//
//
//    //
//    //Optionally, add links
//    //
//    wxBoxSizer* pLinksSizer = new wxBoxSizer( wxHORIZONTAL );
//    pMainSizer->Add(pLinksSizer, 0, wxALIGN_LEFT|wxALL, nSpacing);
//
//    // "play" link
//    if (m_pOptions->fPlayCtrol)
//    {
//        m_pPlayLink = new UrlAuxCtrol(this, ID_LINK_PLAY, rScale,
//                                        m_pOptions->sPlayLabel, "link_play",
//                                        m_pOptions->sStopPlayLabel, "link_stop" );
//        pLinksSizer->Add(m_pPlayLink,
//                    0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, nSpacing);
//    }
//
//    // "solfa" link
//    if (m_pOptions->fSolfaCtrol)
//    {
//        m_pSolfaLink = new UrlAuxCtrol(this, ID_LINK_SOLFA, rScale, m_pOptions->sSolfaLabel,
//                                         m_pOptions->sStopSolfaLabel, lmNO_BITMAP);
//        pLinksSizer->Add(m_pSolfaLink,
//                    0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, nSpacing);
//    }
//
//    //TODO
//	//Play_measure links are not positioned under each measure, as the ScoreCtrol
//	//has no knowledge of measure boundaries. Perhaps this information have to be
//	//provided by the Score and the links have to be repositioned after the score
//	//is rendered.
//	//They could be links in the score (text with link) instead of external objects
//	//For now, play-measure links are positioned at regular intervals. This is
//	//the v2.0 behaviour
//    if (m_pOptions->fMeasuresCtrol)
//    {
//        //GetNumMeasures returns the number of segments but the last one is empty
//        //if the score ends with a barline. Therefore, we need to substract 1
//        int nNumMeasures = wxMin(m_pScore->GetNumMeasures() - 1, 10);
//        for (int i=0; i < nNumMeasures; i++) {
//            m_pMeasureLink[i] =
//                new UrlAuxCtrol(this, ID_LINK_MEASURE+i, rScale,
//                        wxString::Format(m_pOptions->sMeasuresLabel, i+1),
//                        lmNO_BITMAP,
//                        wxString::Format(m_pOptions->sStopMeasureLabel, i+1) );
//            pLinksSizer->Add(m_pMeasureLink[i],
//                        0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, nSpacing);
//        }
//    }
//
//    // debug buttons
//    if (m_appScope.show_debug_links()) {
//        wxBoxSizer* pDbgSizer = new wxBoxSizer( wxHORIZONTAL );
//        pMainSizer->Add(pDbgSizer, 0, wxALIGN_LEFT|wxALL, nSpacing);
//
//        // "See source score"
//        pDbgSizer->Add(
//            new UrlAuxCtrol(this, ID_LINK_SEE_SOURCE, rScale, _("See source score"),
//                              lmNO_BITMAP),
//            wxSizerFlags(0).Left().Border(wxALL, 2*nSpacing) );
//        // "Dump score"
//        pDbgSizer->Add(
//            new UrlAuxCtrol(this, ID_LINK_DUMP, rScale, _("Dump score"), lmNO_BITMAP),
//            wxSizerFlags(0).Left().Border(wxALL, 2*nSpacing) );
//        // "See MIDI events"
//        pDbgSizer->Add(
//            new UrlAuxCtrol(this, ID_LINK_MIDI_EVENTS, rScale, _("See MIDI events"),
//                              lmNO_BITMAP),
//            wxSizerFlags(0).Left().Border(wxALL, 2*nSpacing) );
//    }
//
//    SetSizer( pMainSizer );                // use the sizer for window layout
//    pMainSizer->SetSizeHints( this );    // set size hints to honour minimum size
//
//}
//
//ScoreCtrol::~ScoreCtrol()
//{
//    stop_sounds();
//    if (m_pScore) {
//        delete m_pScore;
//        m_pScore = (ImoScore*)NULL;
//    }
//    m_pScoreCtrol->SetScore((ImoScore*)NULL);
//}
//
//void ScoreCtrol::OnSolfa(wxCommandEvent& event)
//{
//    DoPlay(ePM_RhythmHumanVoice, m_pSolfaLink);
//}
//
//void ScoreCtrol::OnPlayMeasure(wxCommandEvent& event)
//{
//    int i = event.GetId() - ID_LINK_MEASURE;
//    DoPlay(k_play_normal_instrument, m_pMeasureLink[i], i+1);
//}
//
//void ScoreCtrol::DoPlay(lmEPlayMode nPlayMode, UrlAuxCtrol* pLink, int nMeasure)
//{
//    if (!m_fPlaying) {
//        // is not playing. "Play" pressed
//        m_CurPlayLink = pLink;
//
//        //change link from "Play" to "Stop playing" label
//        pLink->set_alternative_label();
//
//        //play
//        if (nMeasure == 0) {
//            m_pScoreCtrol->PlayScore(lmVISUAL_TRACKING, lmNO_COUNTOFF,
//                                nPlayMode, m_pOptions->GetMetronomeMM());
//        }
//        else {
//            m_pScoreCtrol->PlayMeasure(nMeasure, lmVISUAL_TRACKING,
//                                nPlayMode, m_pOptions->GetMetronomeMM());
//        }
//        m_fPlaying = true;
//
//        //AWARE Link label is restored to Normal_label when the EndOfPlay event is
//        //received. Flag m_fPlaying is also reset there
//    }
//    else {
//        // is playing. "Stop playing" button pressed
//        m_pScoreCtrol->Stop();
//        pLink->set_normal_label();
//    }
//    //TODO Piano form
//    //    if (FMain.fFrmPiano) { FPiano.HabilitarMarcado = false;
//}
//
//void ScoreCtrol::OnEndOfPlay(lmEndOfPlaybackEvent& event)
//{
//    m_CurPlayLink->set_normal_label();
//    m_fPlaying = false;
//    event.Skip(true);     //do not continue propagating the event
//}
//
//void ScoreCtrol::stop_sounds()
//{
//    //Stop any possible chord being played to avoid crashes
//    if (m_pScore) m_pScore->Stop();
//}
//
//void ScoreCtrol::OnDebugShowSourceScore(wxCommandEvent& event)
//{
//    m_pScoreCtrol->SourceLDP(false);  //false: do not export undo data
//}
//
//void ScoreCtrol::OnDebugDumpScore(wxCommandEvent& event)
//{
//    m_pScoreCtrol->Dump();
//}
//
//void ScoreCtrol::OnDebugShowMidiEvents(wxCommandEvent& event)
//{
//    m_pScoreCtrol->DumpMidiEvents();
//}


}  //namespace lenmus
