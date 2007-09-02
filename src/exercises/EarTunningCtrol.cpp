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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "EarTunningCtrol.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "EarTunningCtrol.h"
#include "UrlAuxCtrol.h"
#include "Constrains.h"
#include "Generators.h"
#include "../auxmusic/Conversion.h"

#include "../ldp_parser/LDPParser.h"
#include "../auxmusic/Interval.h"
#include "../app/DlgCfgEarIntervals.h"

#include "wx/textctrl.h"


#include "../globals/Colors.h"
extern lmColors* g_pColors;

// access to global external variables
extern bool g_fReleaseVersion;          // in TheApp.cpp
extern bool g_fReleaseBehaviour;        // in TheApp.cpp
extern bool g_fShowDebugLinks;          // in TheApp.cpp
extern bool g_fAutoNewProblem;          // in Preferences.cpp

//access to MIDI manager to get default settings for instrument to use
#include "../sound/MidiManager.h"


//------------------------------------------------------------------------------------
// Implementation of lmEarTunningCtrol: a CompareMidiCtrol
//------------------------------------------------------------------------------------



//Layout definitions
const int BUTTONS_DISTANCE    = 5;        //pixels
const int NUM_LINKS = 3;                //links for actions

//IDs for controls
enum {
    ID_BUTTON = 3010,
};


IMPLEMENT_CLASS(lmEarTunningCtrol, lmCompareMidiCtrol)

BEGIN_EVENT_TABLE(lmEarTunningCtrol, lmCompareMidiCtrol)
    EVT_COMMAND_RANGE (ID_BUTTON, ID_BUTTON+2, wxEVT_COMMAND_BUTTON_CLICKED, lmEarTunningCtrol::OnRespButton)

    LM_EVT_END_OF_PLAY  (lmEarTunningCtrol::OnEndOfPlay)
    EVT_TIMER           (wxID_ANY, lmEarTunningCtrol::OnTimerEvent)

END_EVENT_TABLE()

lmEarTunningCtrol::lmEarTunningCtrol(wxWindow* parent, wxWindowID id, 
                           lmEarIntervalsConstrains* pConstrains,
                           const wxPoint& pos, const wxSize& size, int style)
    : lmCompareMidiCtrol(parent, id, pConstrains, wxSize(400, 150), pos, size, style )
{
    //initializations
    m_pConstrains = pConstrains;

    //ask parent class to create the controls
    CreateControls();
}

void lmEarTunningCtrol::InitializeStrings()
{

    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    wxString sBtLabel[3];
    sBtLabel[0] = _("First one higher");
    sBtLabel[1] = _("Second one higher");
    sBtLabel[2] = _("Both are equal");
}

void lmEarTunningCtrol::CreateAnswerButtons()
{

    //create buttons for the answers: three buttons in one row
    m_pKeyboardSizer = new wxFlexGridSizer(m_NUM_ROWS, m_NUM_COLS, 10, 0);
    m_pMainSizer->Add(
        m_pKeyboardSizer,
        wxSizerFlags(0).Left().Border(wxALIGN_LEFT|wxTOP, 10) );

    // the buttons 
    for (int iB=0; iB < m_NUM_COLS; iB++) {
        m_pAnswerButton[iB] = new wxButton( this, ID_BUTTON + iB, sBtLabel[iB],
            wxDefaultPosition, wxSize(134, 24));
        m_pKeyboardSizer->Add(
            m_pAnswerButton[iB],
            wxSizerFlags(0).Border(wxLEFT|wxRIGHT, BUTTONS_DISTANCE) );
    }

    //inform base class about the settings
    SetButtons(m_pAnswerButton, m_NUM_BUTTONS, ID_BUTTON);

}

lmEarTunningCtrol::~lmEarTunningCtrol()
{
}


void lmEarTunningCtrol::OnRespButton(wxCommandEvent& event)
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
        m_pCounters->IncrementRight();
    } else {
        m_pCounters->IncrementWrong();
    }
        
    // if failure mark pushed button in red
    if (!fSuccess) {
        m_pAnswerButton[nIndex]->SetBackgroundColour(g_pColors->Failure());
    }

    EnableButtons(false);
    m_pShowSolution->Enable(false);

}

void lmEarTunningCtrol::SetNewProblem()
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

    m_mpPitch[0] = 50;
    m_mpPitch[1] = 60;

    //compute the right answer
    m_sAnswer = _T("Second pitch is higher:\n   First: MIDI 50\n   Second: MIDI 60");
    m_nRespIndex = 2;   
    
}

void lmEarTunningCtrol::PlayInterval(int nIntv)
{
    m_nNowPlaying = nIntv;
    m_pAnswerButton[nIntv]->SetBackgroundColour( g_pColors->ButtonHighlight() );
    m_pAnswerButton[nIntv]->Update();    //Refresh works by events and, so, it is not inmediate

    //AWARE: As the intervals are built using whole notes, we will play them at
    // MM=320 so that real note rate will be 80.
    m_pScore[nIntv]->Play(lmNO_VISUAL_TRACKING, NO_MARCAR_COMPAS_PREVIO, 
                             ePM_NormalInstrument, 320, this);

}

void lmEarTunningCtrol::OnEndOfPlay(lmEndOfPlayEvent& WXUNUSED(event))
{
    // remove highlight in button
    m_pAnswerButton[m_nNowPlaying]->SetBackgroundColour( g_pColors->Normal() );
    m_pAnswerButton[m_nNowPlaying]->Update();

    if (m_nNowPlaying == 0 && m_fPlaying) {
        //wxLogMessage(_T("EndOfPlay event: Starting timer"));
        m_oPauseTimer.SetOwner( this, wxID_ANY );
        m_oPauseTimer.Start(1000, wxTIMER_CONTINUOUS );     //wait for 1sec (1000ms)
    }
    else {
        //wxLogMessage(_T("EndOfPlay event: play stopped"));
        m_fPlaying = false;
        m_pPlayButton->SetLabel(_("Play"));
    }

}

void lmEarTunningCtrol::OnTimerEvent(wxTimerEvent& WXUNUSED(event))
{
    m_oPauseTimer.Stop();
    if (m_fPlaying) {
        //wxLogMessage(_T("Timer event: play(1)"));
        PlayInterval(1);
    }
    else {
        //wxLogMessage(_T("Timer event: play stopped"));
        m_pPlayButton->SetLabel(_("Play"));
    }
}
