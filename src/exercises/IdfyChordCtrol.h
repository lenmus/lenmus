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
/*! @file IdfyChordCtrol.h
    @brief Header file for class lmIdfyChordCtrol
    @ingroup html_controls
*/

#ifndef __IDFYCHORDCTROL_H__        //to avoid nested includes
#define __IDFYCHORDCTROL_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "ChordConstrains.h"
#include "../score/score.h"
#include "ScoreAuxCtrol.h"
#include "UrlAuxCtrol.h"
#include "CountersCtrol.h"



class lmIdfyChordCtrol : public wxWindow    
{
   DECLARE_DYNAMIC_CLASS(lmIdfyChordCtrol)

public:

    // constructor and destructor    
    lmIdfyChordCtrol(wxWindow* parent, wxWindowID id,
               lmChordConstrains* pConstrains, 
               const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxDefaultSize, int style = 0);

    ~lmIdfyChordCtrol();

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
    void SetUpButtons();
    void EnableButtons(bool fEnable);
    void Play();
    void NewProblem();
    void DisplaySolution();
    void ResetExercise();
    void ResetCounters();
    wxString PrepareChord(EClefType nClef, EChordType nType, lmScore** pScore);

        // member variables

    lmScore*            m_pChordScore;      //solution score with the chord
    lmScore*            m_pAuxScore;        //score to play user selected chords
    lmScoreAuxCtrol*    m_pScoreCtrol;
    lmCountersCtrol*    m_pCounters;

    lmChordConstrains* m_pConstrains;       //constrains for the exercise
    bool            m_fQuestionAsked;       //question asked but not yet answered

    //problem asked
    EKeySignatures  m_nKey;
    wxString        m_sRootNote;
    int             m_nInversion;
    int             m_nMode;

    //answer
    wxStaticText*   m_pRowLabel[5];
    wxButton*       m_pAnswerButton[ect_Max];   //buttons for the answers
    int             m_nRealChord[ect_Max];      // chord that corresponds
                                                // to each valid button
    int             m_nRespIndex;           //index to the button with the right answer
    wxString        m_sAnswer;              //The names of each interval

    lmUrlAuxCtrol*  m_pPlayButton;       // "play" button
    lmUrlAuxCtrol*  m_pShowSolution;     // "show solution" button

    //lmIdfyChordCtrol is used both for ear training exercises and for theory exercises.
    //Following variables are used for configuration
    bool        m_fTheoryMode;

    DECLARE_EVENT_TABLE()
};



#endif  // __IDFYCHORDCTROL_H__
