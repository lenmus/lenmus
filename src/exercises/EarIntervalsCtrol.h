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

#ifndef __EARINTERVALSCTROL_H__        //to avoid nested includes
#define __EARINTERVALSCTROL_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "EarIntervalsCtrol.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "EarIntvalConstrains.h"
#include "../score/Score.h"
#include "ScoreAuxCtrol.h"
#include "UrlAuxCtrol.h"
#include "CountersCtrol.h"
#include "../auxmusic/Interval.h"


const int lmEAR_INVAL_NUM_BUTTONS = 25;               //buttons for answers


class lmEarIntervalsCtrol : public wxWindow    
{
   DECLARE_DYNAMIC_CLASS(lmEarIntervalsCtrol)

public:

    // constructor and destructor    
    lmEarIntervalsCtrol(wxWindow* parent, wxWindowID id,
               lmEarIntervalsConstrains* pConstrains, 
               const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxDefaultSize, int style = 0);

    ~lmEarIntervalsCtrol();

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

    // other methods
    void SetUpButtons();

private:
    void Play();
    void NewProblem();
    void DisplaySolution();
    void ResetExercise();
    void PrepareScore(wxString& sIntvCode, lmScore** pScore);
    void DoStopSounds();

        // member variables

    lmScore*        m_pScore;               // the score with the interval
    lmScore*        m_pAuxScore;            //score to play user selected interval
    lmEarIntervalsConstrains* m_pConstrains;
    bool            m_fProblemCreated;      //there is a problem prepared
    int             m_nRespIndex;           //index to the button with the right answer
    int             m_nRealIntval[lmEAR_INVAL_NUM_BUTTONS]; // intval. that corresponds
                                                            // to each valid button
    int             m_nValidIntervals;      // num of enabled buttons 
    wxString        m_sAnswer;              //name of the interval
    bool            m_fQuestionAsked;       //question asked but not yet answered

    //problem asked
    wxString            m_sIntvCode;
    bool                m_fAscending;
    bool                m_fHarmonic;
    EKeySignatures      m_nKey;
    lmNoteBits          m_tNote[2];

    // controls on the window
    wxButton*           m_pAnswerButton[lmEAR_INVAL_NUM_BUTTONS];
    lmScoreAuxCtrol*    m_pScoreCtrol;
    lmCountersCtrol*    m_pCounters;

    lmUrlAuxCtrol*  m_pPlayButton;       // "play" button
    lmUrlAuxCtrol*  m_pShowSolution;     // "show solution" button


    DECLARE_EVENT_TABLE()
};



#endif  // __EARINTERVALSCTROL_H__
