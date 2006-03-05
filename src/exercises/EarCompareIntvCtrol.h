// RCS-ID: $Id: EarCompareIntvCtrol.h,v 1.4 2006/02/23 19:19:15 cecilios Exp $
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
/*! @file EarCompareIntvCtrol.h
    @brief Header file for class lmEarCompareIntvCtrol
    @ingroup html_controls
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __EARCOMPAREINTVCTROL_H__        //to avoid nested includes
#define __EARCOMPAREINTVCTROL_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "EarIntvalConstrains.h"
#include "../score/score.h"
#include "ScoreAuxCtrol.h"

class lmEarCompareIntvCtrol : public wxWindow    
{
   DECLARE_DYNAMIC_CLASS(lmEarCompareIntvCtrol)

public:

    // constructor and destructor    
    lmEarCompareIntvCtrol(wxWindow* parent, wxWindowID id,
               lmEarIntervalsConstrains* pConstrains, 
               const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxDefaultSize, int style = 0);

    ~lmEarCompareIntvCtrol();

    // event handlers
    void OnSize(wxSizeEvent& event);
    void OnRespButton(wxCommandEvent& event);
    void OnPlay(wxCommandEvent& event);
    void OnNewProblem(wxCommandEvent& event);
    void OnResetCounters(wxCommandEvent& event);
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
    void ResetCounters();

        // member variables

    lmScore*            m_pScore[2];        // two scores, one for each interval

    lmScoreAuxCtrol*    m_pScoreCtrol;
    lmEarIntervalsConstrains* m_pConstrains;    //use same constrains than for intervals
    bool            m_fProblemCreated;      //there is a problem prepared
    lmPitch         m_ntMidi[2];            //the midi pitch of the two notes
    lmPitch         m_ntPitch[2];           //the pitch of the two notes
    bool            m_fPlayEnabled;         //Play enabled
    bool            m_fButtonsEnabled;      //buttons enabled

    //answer
    wxButton*       m_pAnswerButton[3];     //3 buttons for the answers
    bool            m_fFirstGreater;        //two flags to encode the right answer
    bool            m_fBothEqual;
    wxString        m_sAnswer[2];           //The names of each interval

    DECLARE_EVENT_TABLE()
};



#endif  // __EARCOMPAREINTVCTROL_H__
