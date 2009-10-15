//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 Cecilio Salmeron
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
//-------------------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "TheoMusicReadingCtrol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/statline.h"

#include "TheoMusicReadingCtrol.h"
#include "auxctrols/UrlAuxCtrol.h"
#include "auxctrols/ScoreAuxCtrol.h"
#include "Constrains.h"
#include "Generators.h"
#include "../score/Score.h"
#include "../auxmusic/ComposerV6.h"
#include "dialogs/DlgCfgScoreReading.h"
#include "../html/TextBookController.h"
#include "wx/html/htmlwin.h"

#include "../app/MainFrame.h"
extern lmMainFrame* g_pMainFrame;


// access to global functions
extern void lmComputeAccidentals(lmEKeySignatures nKeySignature, int nAccidentals[]);
extern int GetRootNoteIndex(lmEKeySignatures nKeySignature);
extern const wxString& GetKeySignatureName(lmEKeySignatures nKeySignature);

// access to global external variables
extern bool g_fReleaseVersion;            // in TheApp.cpp
extern bool g_fReleaseBehaviour;        // in TheApp.cpp
extern bool g_fShowDebugLinks;            // in TheApp.cpp


//------------------------------------------------------------------------------------
// Implementation of lmTheoMusicReadingCtrol
//------------------------------------------------------------------------------------


IMPLEMENT_CLASS(lmTheoMusicReadingCtrol, lmOneScoreCtrol)

lmTheoMusicReadingCtrol::lmTheoMusicReadingCtrol(wxWindow* parent, wxWindowID id,
                           lmMusicReadingConstrains* pConstrains,
                           const wxPoint& pos, const wxSize& size, int style)
    : lmOneScoreCtrol(parent, id, pConstrains, wxSize(600,800), pos, size, style )
{
    //initializations
    m_pScoreConstrains = pConstrains->GetScoreConstrains();
    m_pConstrains = pConstrains;

    //configuration options
    m_pConstrains->SetTheoryMode(true);
    m_pConstrains->SetSolutionLink(false);
    m_pConstrains->SetUsingCounters(false);
    m_nPlayMM = 0;       //use metronome settings

    CreateControls();
    NewProblem();

}

void lmTheoMusicReadingCtrol::CreateControls()
{
    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    InitializeStrings();

    // ensure that sizes are properly scaled
    m_rScale = g_pMainFrame->GetHtmlWindow()->GetScale();
    m_nDisplaySize.x = (int)((double)m_nDisplaySize.x * m_rScale);
    m_nDisplaySize.y = (int)((double)m_nDisplaySize.y * m_rScale);

    // prepare layout info for answer buttons and spacing
    //int nButtonsHeight = (int)(m_rScale * 24.0);    // 24 pixels, scaled
    wxFont oButtonsFont = GetParent()->GetFont();
    oButtonsFont.SetPointSize( (int)((double)oButtonsFont.GetPointSize() * m_rScale) );
    int nSpacing = (int)(5.0 * m_rScale);       //5 pixels, scaled

    //the window is divided into two regions: top for links, and bottom for the score
    wxBoxSizer* m_pMainSizer = new wxBoxSizer( wxVERTICAL );

    // buttons and links
    wxBoxSizer* m_pButtonsSizer = new wxBoxSizer( wxHORIZONTAL );
    m_pMainSizer->Add(
        m_pButtonsSizer,
        wxSizerFlags(0).Left().Expand().Border(wxLEFT|wxRIGHT|wxTOP, nSpacing) );

    // "Go back to theory" button
    if (m_pConstrains->IncludeGoBackLink()) {
        m_pButtonsSizer->Add(
            new lmUrlAuxCtrol(this, ID_LINK_GO_BACK, m_rScale, _("Go back to theory") ),
            wxSizerFlags(0).Left().Border(wxALL, nSpacing) );
    }

    // "new problem" button
    m_pButtonsSizer->Add(
        new lmUrlAuxCtrol(this, ID_LINK_NEW_PROBLEM, m_rScale, _("New problem") ),
        wxSizerFlags(0).Left().Border(wxALL, nSpacing) );

    // "play" button
    m_pPlayButton = new lmUrlAuxCtrol(this, ID_LINK_PLAY, m_rScale, m_pConstrains->sPlayLabel, m_pConstrains->sStopPlayLabel );
    m_pButtonsSizer->Add(
        m_pPlayButton,
        wxSizerFlags(0).Left().Border(wxALL, nSpacing) );

    // "solfa" button
    if (m_pConstrains->fSolfaCtrol) {
        //m_pSolfaLink = new lmUrlAuxCtrol(this, ID_LINK_SOLFA, m_rScale, m_pConstrains->sSolfaLabel, m_pConstrains->sStopSolfaLabel );
        //m_pButtonsSizer->Add(
        //    m_pPlayButton,
        //    wxSizerFlags(0).Left().Border(wxALL, nSpacing) );
    }

    // debug buttons
    if (g_fShowDebugLinks && !g_fReleaseVersion) {
        // "See source score"
        m_pButtonsSizer->Add(
            new lmUrlAuxCtrol(this, ID_LINK_SEE_SOURCE, m_rScale, _("See source score") ),
            wxSizerFlags(0).Left().Border(wxALL, nSpacing) );
        // "Dump score"
        m_pButtonsSizer->Add(
            new lmUrlAuxCtrol(this, ID_LINK_DUMP, m_rScale, _("Dump score") ),
            wxSizerFlags(0).Left().Border(wxALL, nSpacing) );
        // "See MIDI events"
        m_pButtonsSizer->Add(
            new lmUrlAuxCtrol(this, ID_LINK_MIDI_EVENTS, m_rScale, _("See MIDI events") ),
            wxSizerFlags(0).Left().Border(wxALL, nSpacing) );
    }

    // spacer
    m_pButtonsSizer->Add(nSpacing, nSpacing, 1, wxALIGN_CENTER_VERTICAL|wxALL, nSpacing);

    // settings link
    if (m_pConstrains->IncludeSettingsLink()) {
        lmUrlAuxCtrol* pSettingsLink = new lmUrlAuxCtrol(this, ID_LINK_SETTINGS, m_rScale,
                                                         _("Exercise options") );
        m_pButtonsSizer->Add(pSettingsLink, wxSizerFlags(0).Left().Border(wxALL, nSpacing) );
    }

    // horizontal line
    wxStaticLine* pStaticLine1 = new wxStaticLine(this, wxID_ANY,
                            wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    ((wxWindow*)pStaticLine1)->SetBackgroundColour(*wxRED);
    ((wxWindow*)pStaticLine1)->SetForegroundColour(*wxBLUE);
    m_pMainSizer->Add(pStaticLine1, 0, wxGROW|wxALL, nSpacing);


    // create score ctrl
    wxBoxSizer* pScoreSizer = new wxBoxSizer( wxHORIZONTAL );
    m_pMainSizer->Add(
        pScoreSizer,
        wxSizerFlags(0).Left().Expand().Border(wxALL, nSpacing) );

    lmScoreAuxCtrol* pScoreCtrol = new lmScoreAuxCtrol(this, -1, m_pProblemScore,
        wxDefaultPosition, m_nDisplaySize, eNO_BORDER);
    pScoreCtrol->SetMargins(lmToLogicalUnits(0, lmMILLIMETERS),         //left=0cm
                              lmToLogicalUnits(0, lmMILLIMETERS),      //right=1cm
                              lmToLogicalUnits(10, lmMILLIMETERS));     //top=1cm
    pScoreCtrol->SetScale( pScoreCtrol->GetScale() * (float)m_rScale );
    pScoreSizer->Add(
        pScoreCtrol,
        wxSizerFlags(1).Left().Border(0, nSpacing));
    m_pDisplayCtrol = pScoreCtrol;

    SetSizer( m_pMainSizer );                // use the sizer for window layout
    m_pMainSizer->SetSizeHints( this );        // set size hints to honour minimum size

    m_pCounters = (lmCountersAuxCtrol*) NULL;

    m_fControlsCreated = true;

    //inform base class about the settings
    SetButtons(NULL, 0, wxID_ANY);
}


lmTheoMusicReadingCtrol::~lmTheoMusicReadingCtrol()
{
}

wxDialog* lmTheoMusicReadingCtrol::GetSettingsDlg()
{
    return new lmDlgCfgScoreReading(this, m_pScoreConstrains, m_pConstrains->sSettingsKey);
}

wxString lmTheoMusicReadingCtrol::SetNewProblem()
{
    //This method must prepare the problem score and set variables:
    //  m_pProblemScore - The score with the problem to propose
    //  m_pSolutionScore - The score with the solution or NULL if it is the
    //              same score than the problem score.
    //  m_sAnswer - the message to present when displaying the solution
    //  m_nRespIndex - the number of the button for the right answer
    //  m_nPlayMM - the speed to play the score
    //
    //It must return the message to display to introduce the problem.

    //Generate a random score
    lmComposer6 oComposer;
    m_pProblemScore = oComposer.GenerateScore(m_pScoreConstrains);

    return _T("");

}

void lmTheoMusicReadingCtrol::Play()
{
    DoPlay( g_pMainFrame->IsCountOffChecked() ); 
}

