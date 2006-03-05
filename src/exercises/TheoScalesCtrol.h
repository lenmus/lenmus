// RCS-ID: $Id: TheoScalesCtrol.h,v 1.3 2006/02/23 19:19:53 cecilios Exp $
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
/*! @file TheoScalesCtrol.h
    @brief Header file for class lmTheoScalesCtrol
    @ingroup html_controls
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __THEOSCALESCTROL_H__        //to avoid nested includes
#define __THEOSCALESCTROL_H__

// for (compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/checkbox.h"

#include "Constrains.h"
#include "../score/score.h"
#include "ScoreAuxCtrol.h"
#include "UrlAuxCtrol.h"
#include "../sound/SoundEvents.h"


class lmTheoScalesCtrol : public wxWindow    
{
   DECLARE_DYNAMIC_CLASS(lmTheoScalesCtrol)

public:

    // constructor and destructor    
    lmTheoScalesCtrol(wxWindow* parent, wxWindowID id,
               lmTheoScalesConstrains* pConstrains, 
               const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxDefaultSize, int style = 0);

    ~lmTheoScalesCtrol();

    // event handlers
    void OnClose(wxCloseEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnRespButton(wxCommandEvent& event);
    void OnPlay(wxCommandEvent& event);
    void OnNewProblem(wxCommandEvent& event);
    void OnResetCounters(wxCommandEvent& event);
    void OnDisplaySolution(wxCommandEvent& event);

    // event handlers related to debugging
    void OnDebugShowSourceScore(wxCommandEvent& event);
    void OnDebugDumpScore(wxCommandEvent& event);
    void OnDebugShowMidiEvents(wxCommandEvent& event);

    // event handlers related with playing a score
    void OnEndOfPlay(lmEndOfPlayEvent& WXUNUSED(event));


private:
    void EnableButtons(bool fEnable);
    void Play();
    void NewProblem();
    void DisplaySolution();
    void ResetExercise();
    void ResetCounters();

        // member variables

    lmScore*                    m_pScore;            // the score with the scale
    lmScoreAuxCtrol*            m_pScoreCtrol;
    wxCheckBox*                m_pChkKeySignature;
    lmTheoScalesConstrains*    m_pConstrains;
    bool        m_fProblemCreated;    //there is a problem prepared
    lmPitch        m_ntMidi[8];        //the midi pitch of the two notes
    lmPitch        m_ntPitch[8];        //the pitch of the two notes
    bool        m_fPlayEnabled;        //Play enabled
    bool        m_fDeduceScale;

    //buttons for the answers: 6 rows, 8 buttons per row
    wxButton*    m_pAnswerButton[48];
    int            m_nRespIndex;           //index to the button with the right answer
    bool        m_fButtonsEnabled;        //buttons enabled

    wxString        m_sAnswer;            //scale name
    lmUrlAuxCtrol*    m_pPlayLink;    
    bool            m_fPlaying;            //playing
    bool            m_fClosing;        // waiting for play stopped to close the window

    DECLARE_EVENT_TABLE()
};



#endif  // __THEOSCALESCTROL_H__

