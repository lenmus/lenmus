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

#ifndef __IDFYEXERCISECTROL_H__        //to avoid nested includes
#define __IDFYEXERCISECTROL_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "IdfyExerciseCtrol.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../score/Score.h"
#include "ScoreAuxCtrol.h"
#include "UrlAuxCtrol.h"
#include "CountersCtrol.h"
#include "../sound/SoundEvents.h"


//--------------------------------------------------------------------------------
// Abstract class for ear exercises constraints
//--------------------------------------------------------------------------------
class lmIdfyConstrains
{
public:
    lmIdfyConstrains(wxString sSection);
    virtual ~lmIdfyConstrains() {}

    virtual void SaveSettings() {};

    void SetSettingsLink(bool fValue) { m_fSettingsLink = fValue; }
    bool IncludeSettingsLink() { return m_fSettingsLink; }

    void SetTheoryMode(bool fValue) { m_fTheoryMode = fValue; }
    bool IsTheoryMode() { return m_fTheoryMode; }


protected:
    virtual void LoadSettings() {};

    wxString    m_sSection;         //section name to save the constraints
    bool        m_fSettingsLink;    //include settings link

    //The Ctrol could be used both for ear training exercises and for theory exercises.
    //Following variables are used for configuration
    bool        m_fTheoryMode;


};


//--------------------------------------------------------------------------------
// Abstract class for ear exercises controls
//--------------------------------------------------------------------------------


class lmIdfyExerciseCtrol : public wxWindow    
{
   DECLARE_DYNAMIC_CLASS(lmIdfyExerciseCtrol)

public:

    // constructor and destructor    
    lmIdfyExerciseCtrol(wxWindow* parent, wxWindowID id,
               lmIdfyConstrains* pConstrains, 
               const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxDefaultSize, int style = 0);

    virtual ~lmIdfyExerciseCtrol();

    // event handlers
    virtual void OnSize(wxSizeEvent& event);
    virtual void OnRespButton(wxCommandEvent& event);
    virtual void OnPlay(wxCommandEvent& event);
    virtual void OnNewProblem(wxCommandEvent& event);
    virtual void OnDisplaySolution(wxCommandEvent& event);
    virtual void OnSettingsButton(wxCommandEvent& event);

    // event handlers related to debugging
    virtual void OnDebugShowSourceScore(wxCommandEvent& event);
    virtual void OnDebugDumpScore(wxCommandEvent& event);
    virtual void OnDebugShowMidiEvents(wxCommandEvent& event);

    // event handlers related with playing a score
    void OnEndOfPlay(lmEndOfPlayEvent& WXUNUSED(event));


protected:
    //virtual pure methods
    virtual void InitializeStrings() {};   
    virtual void CreateAnswerButtons()=0;
    virtual void ReconfigureButtons()=0;
    virtual wxString SetNewProblem()=0;    
    virtual wxDialog* GetSettingsDlg()=0;
    virtual void PrepareAuxScore(int nButton)=0;

    //methods that, normally, it is not necessary to implement
    virtual void SetButtonColor(int i, wxColour& color);
    virtual void EnableButtons(bool fEnable);
    virtual void Play();
    virtual void NewProblem();
    virtual void DisplaySolution();
    virtual void ResetExercise();
    virtual void StopSounds() {};

    void Create(int nCtrolWidth, int nCtrolHeight);
    void SetButtons(wxButton* pButton[], int nNumButtons, int nIdFirstButton);

        // member variables

    int                 m_nNumButtons;      //num answer buttons
    lmScore*            m_pProblemScore;    //score with the problem
	lmScore*			m_pSolutionScore;	//if not NULL, score with the solution. If NULL
											//   problem score will be used as solution score
    lmScore*            m_pAuxScore;        //score to play user selected buttons 
    lmScoreAuxCtrol*    m_pScoreCtrol;
    lmCountersCtrol*    m_pCounters;
    wxBoxSizer*         m_pMainSizer;
    wxFlexGridSizer*    m_pKeyboardSizer;

    lmIdfyConstrains*    m_pConstrains;      //constraints for the exercise
    bool                m_fQuestionAsked;   //question asked but not yet answered
    int                 m_nRespIndex;       //index to the button with the right answer
    wxString            m_sAnswer;          //string with the right answer

    lmUrlAuxCtrol*      m_pPlayButton;      // "play" button
    lmUrlAuxCtrol*      m_pShowSolution;    // "show solution" button
    int                 m_nPlayMM;          //metronome setting to play scores


    //lmIdfyExerciseCtrol can used both for ear training exercises and for theory exercises.
    //Following variable is used for configuration
    bool                m_fTheoryMode;

private:
    void DoStopSounds();

    wxButton**      m_pAnswerButtons;   //buttons for the answers
    int             m_nIdFirstButton;   //ID of first button; the others in sequence
    bool            m_fPlaying;         //currently playing the score

    DECLARE_EVENT_TABLE()
};



#endif  // __IDFYEXERCISECTROL_H__
