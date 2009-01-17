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

#ifndef __LM_THEOHARMONYCTROL_H__        //to avoid nested includes
#define __LM_THEOHARMONYCTROL_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "TheoHarmonyCtrol.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "HarmonyConstrains.h"
#include "../score/Score.h"
#include "ExerciseCtrol.h"
#include "auxctrols/EditScoreAuxCtrol.h"



class lmTheoHarmonyCtrol : public lmExerciseCtrol   
{
   DECLARE_DYNAMIC_CLASS(lmTheoHarmonyCtrol)

public:

    // constructor and destructor    
    lmTheoHarmonyCtrol(wxWindow* parent, wxWindowID id,
               lmHarmonyConstrains* pConstrains, wxSize nDisplaySize, 
               const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxDefaultSize, int style = 0);

    ~lmTheoHarmonyCtrol();

    //event handlers
    void OnEndOfPlay(lmEndOfPlayEvent& WXUNUSED(event));

    //implementation of virtual event handlers
    virtual void OnDebugShowSourceScore(wxCommandEvent& event);
    virtual void OnDebugDumpScore(wxCommandEvent& event);
    virtual void OnDebugShowMidiEvents(wxCommandEvent& event);


public:

    //implementation of virtual methods
    void InitializeStrings();
    void CreateAnswerButtons(int nHeight, int nSpacing, wxFont& font);
    void PrepareAuxScore(int nButton);
    wxString SetNewProblem();    
    wxDialog* GetSettingsDlg();
    void ReconfigureButtons();


private:
    wxString PrepareScore(lmEClefType nClef, lmECadenceType nType, lmScore** pProblemScore,
                          lmScore** pSolutionScore = NULL );
    int DisplayButton(int iBt, lmECadenceType iStartC, lmECadenceType iEndC, wxString sButtonLabel);

    void Play();
    void PlaySpecificSound(int nButton);
    void DisplaySolution();
    void DisplayProblem();
    void DeleteScores();
    void StopSounds();
    wxWindow* CreateDisplayCtrol();
    void DisplayMessage(wxString& sMsg, bool fClearDisplay);


        // member variables

    enum {
        m_NUM_COLS = 4,
        m_NUM_ROWS = 2,
        m_NUM_BUTTONS = 8,     // NUM_COLS * NUM_ROWS;
    };

    lmHarmonyConstrains* m_pConstrains;       //constraints for the exercise

    //problem asked
    lmEKeySignatures  m_nKey;

    //answer
    wxButton*       m_pAnswerButton[m_NUM_BUTTONS];     //buttons for the answers
    //cadence that corresponds to each valid button
    lmECadenceType  m_nStartCadence[m_NUM_BUTTONS];
    lmECadenceType  m_nEndCadence[m_NUM_BUTTONS];

    lmScore*    m_pProblemScore;    //score with the problem
	lmScore*    m_pSolutionScore;	//if not NULL, score with the solution. If NULL
                                    //   problem score will be used as solution score
    lmScore*    m_pAuxScore;        //score to play user selected buttons
    int         m_nPlayMM;          //metronome setting to play scores
    bool        m_fPlaying;         //currently playing the score

    DECLARE_EVENT_TABLE()
};



#endif  // __LM_THEOHARMONYCTROL_H__
