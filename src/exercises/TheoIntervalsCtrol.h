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
/*! @file TheoIntervalsCtrol.h
    @brief Header file for class lmTheoIntervalsCtrol
    @ingroup html_controls
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __THEOINTERVALSCTROL_H__        //to avoid nested includes
#define __THEOINTERVALSCTROL_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Constrains.h"
#include "../score/score.h"
#include "ScoreAuxCtrol.h"
#include "CountersCtrol.h"
#include "UrlAuxCtrol.h"


class lmTheoIntervalsCtrol : public wxWindow    
{
   DECLARE_DYNAMIC_CLASS(lmTheoIntervalsCtrol)

public:

    // constructor and destructor    
    lmTheoIntervalsCtrol(wxWindow* parent, wxWindowID id,
               lmTheoIntervalsConstrains* pConstrains, 
               const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxDefaultSize, int style = 0);

    ~lmTheoIntervalsCtrol();

    // event handlers
    void OnSize(wxSizeEvent& event);
    void OnRespButton(wxCommandEvent& event);
    void OnPlay(wxCommandEvent& event);
    void OnNewProblem(wxCommandEvent& event);
    void OnDisplaySolution(wxCommandEvent& event);
    void OnSettingsButton(wxCommandEvent& event);

    // event handlers related to debugging
    void OnDebugShowSourceScore(wxCommandEvent& event);
    void OnDebugDumpScore(wxCommandEvent& event);
    void OnDebugShowMidiEvents(wxCommandEvent& event);

private:
    void EnableButtons(bool fEnable);
    void Play();
    void NewProblem();
    void DisplaySolution();
    void ResetExercise();
    void SetUpButtons();


        // member variables

    lmScore*            m_pScore;           // the score with the interval
    lmScoreAuxCtrol*    m_pScoreCtrol;
    lmCountersCtrol*    m_pCounters;

    lmUrlAuxCtrol*      m_pPlayButton;      // "play" button

    lmTheoIntervalsConstrains* m_pConstrains;
    bool            m_fProblemCreated;      //there is a problem prepared
    lmPitch         m_ntMidi[2];            //the midi pitch of the two notes
    lmPitch         m_ntPitch[2];           //the pitch of the two notes
    bool            m_fPlayEnabled;         //Play enabled
    bool            m_fIntervalKnown;

    //buttons for the answers: 6 rows, 8 buttons per row + 2 buttons
    wxButton*       m_pAnswerButton[50];
    int             m_nRespIndex;           //index to the button with the right answer
    bool            m_fButtonsEnabled;      //buttons enabled

    wxString        m_sAnswer;              //name of the interval

    DECLARE_EVENT_TABLE()
};



#endif  // __THEOINTERVALSCTROL_H__
