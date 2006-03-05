// RCS-ID: $Id: TheoKeySignCtrol.h,v 1.4 2006/02/23 19:19:53 cecilios Exp $
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
/*! @file TheoKeySignCtrol.h
    @brief Header file for class lmTheoKeySignCtrol
    @ingroup html_controls
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __THEOKEYSIGNCTROL_H__        //to avoid nested includes
#define __THEOKEYSIGNCTROL_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Constrains.h"
#include "TheoKeySignConstrains.h"
#include "../score/score.h"
#include "ScoreAuxCtrol.h"


const int lmTHEO_KEYSIGN_NUM_BUTTONS = 15;               //buttons for answers

class lmTheoKeySignCtrol : public wxWindow    
{
   DECLARE_DYNAMIC_CLASS(lmTheoKeySignCtrol)

public:

    // constructor and destructor    
    lmTheoKeySignCtrol(wxWindow* parent, wxWindowID id,
               lmTheoKeySignConstrains* pConstrains, 
               const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxDefaultSize, int style = 0);

    ~lmTheoKeySignCtrol();

    // event handlers
    void OnSize(wxSizeEvent& event);
    void OnRespButton(wxCommandEvent& event);
    void OnPlay(wxCommandEvent& event);
    void OnNewProblem(wxCommandEvent& event);
    void OnResetCounters(wxCommandEvent& event);
    void OnDisplaySolution(wxCommandEvent& event);

    // event handlers related to debugging
    void OnDebugShowSourceScore(wxCommandEvent& event);
    void OnDebugDumpScore(wxCommandEvent& event);

private:
    void EnableButtons(bool fEnable);
    void NewProblem();
    void DisplaySolution();
    void ResetExercise();
    void ResetCounters();

        // member variables

    lmScore*            m_pScore;           // the score with the interval
    lmScoreAuxCtrol*    m_pScoreCtrol;
    lmTheoKeySignConstrains* m_pConstrains;
    bool            m_fProblemCreated;      //there is a problem prepared

    //buttons for the answers: 3 rows, 5 buttons per row
    wxButton*       m_pAnswerButton[lmTHEO_KEYSIGN_NUM_BUTTONS];
    int             m_nIndexKeyName;        //index to right answer button
    bool            m_fButtonsEnabled;      //buttons enabled

    wxString        m_sAnswer;              //name of the interval
    bool            m_fMajorMode;           //major mode
    bool            m_fIdentifyKey;         //type of problem

    DECLARE_EVENT_TABLE()
};



#endif  // __THEOKEYSIGNCTROL_H__
