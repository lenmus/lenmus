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

#ifndef __EXERCISECTROL_H__        //to avoid nested includes
#define __EXERCISECTROL_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ExerciseCtrol.cpp"
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


//--------------------------------------------------------------------------------
// Abstract class to create constrains/options objects for lmExerciseCtrols
//--------------------------------------------------------------------------------
class lmExerciseConstrains
{
public:
    lmExerciseConstrains(wxString sSection);
    virtual ~lmExerciseConstrains() {}

    virtual void SaveSettings() {}

    void SetSettingsLink(bool fValue) { m_fSettingsLink = fValue; }
    bool IncludeSettingsLink() { return m_fSettingsLink; }
    void SetSection(wxString sSection) {
                m_sSection = sSection;
                LoadSettings();
            }

    void SetGoBackLink(wxString sURL) { m_sGoBackURL = sURL; }
    bool IncludeGoBackLink() { return m_sGoBackURL != _T(""); }
    wxString GetGoBackURL() { return m_sGoBackURL; }

    void SetTheoryMode(bool fValue) { m_fTheoryMode = fValue; }
    bool IsTheoryMode() { return m_fTheoryMode; }

    void SetButtonsEnabledAfterSolution(bool fValue) {
            m_fButtonsEnabledAfterSolution = fValue;
    }
    bool ButtonsEnabledAfterSolution() { return m_fButtonsEnabledAfterSolution; }


protected:
    virtual void LoadSettings() {}

    wxString    m_sSection;         //section name to save the constraints
    wxString    m_sGoBackURL;       //URL for "Go back" link of empty string if no link
    bool        m_fSettingsLink;    //include 'settings' link
    bool        m_fButtonsEnabledAfterSolution;

    //The Ctrol could be used both for ear training exercises and for theory exercises.
    //Following variables are used for configuration
    bool        m_fTheoryMode;


};



//--------------------------------------------------------------------------------
// Abstract class to create exercises controls
//--------------------------------------------------------------------------------
class lmExerciseCtrol : public wxWindow    
{
   DECLARE_DYNAMIC_CLASS(lmExerciseCtrol)

public:

    // constructor and destructor    
    lmExerciseCtrol(wxWindow* parent, wxWindowID id,
               lmExerciseConstrains* pConstrains, wxSize nDisplaySize, 
               const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxDefaultSize, int style = 0);

    virtual ~lmExerciseCtrol();

    // event handlers. No need to implement in derived classes
    void OnPaint(wxPaintEvent& WXUNUSED(event));
    virtual void OnSize(wxSizeEvent& event);
    virtual void OnRespButton(wxCommandEvent& event);
    virtual void OnPlay(wxCommandEvent& event);
    virtual void OnNewProblem(wxCommandEvent& event);
    virtual void OnDisplaySolution(wxCommandEvent& event);
    virtual void OnSettingsButton(wxCommandEvent& event);
    virtual void OnGoBackButton(wxCommandEvent& event);

    // event handlers to be implemented by derived classes
    virtual void OnDebugShowSourceScore(wxCommandEvent& event)=0;
    virtual void OnDebugDumpScore(wxCommandEvent& event)=0;
    virtual void OnDebugShowMidiEvents(wxCommandEvent& event)=0;

protected:

    //virtual pure methods to be implemented by derived classes
    virtual void InitializeStrings()=0;   
    virtual void CreateAnswerButtons(int nHeight, int nSpacing, wxFont& font)=0;
    virtual void ReconfigureButtons()=0;
    virtual wxString SetNewProblem()=0;    
    virtual wxDialog* GetSettingsDlg()=0;
    virtual wxWindow* CreateDisplayCtrol()=0;
    virtual void Play()=0;
    virtual void PlaySpecificSound(int nButton)=0;
    virtual void DisplaySolution()=0;
    virtual void DisplayProblem()=0;
    virtual void DisplayMessage(wxString& sMsg, bool fClearDisplay)=0;
    virtual void DeleteScores() {}     //should be virtual pure but the linker complains !!!
    virtual void StopSounds() {}     //should be virtual pure but the linker complains !!!

    //methods that, normally, it is not necessary to implement
    virtual void SetButtonColor(int i, wxColour& color);
    virtual void EnableButtons(bool fEnable);
    virtual void NewProblem();
    virtual void ResetExercise();

    //methods invoked from derived classes
    void CreateControls();
    void SetButtons(wxButton* pButton[], int nNumButtons, int nIdFirstButton);



        // member variables

    wxWindow*           m_pDisplayCtrol;    //TextCtrl or ScoreAuxCtrol
    lmCountersCtrol*    m_pCounters;
    wxBoxSizer*         m_pMainSizer;
    wxFlexGridSizer*    m_pKeyboardSizer;

    lmExerciseConstrains*   m_pConstrains;  //constraints for the exercise
    bool                m_fQuestionAsked;   //question asked but not yet answered
    int                 m_nRespIndex;       //index to the button with the right answer
    wxString            m_sAnswer;          //string with the right answer

    lmUrlAuxCtrol*      m_pPlayButton;      // "play" button
    lmUrlAuxCtrol*      m_pShowSolution;    // "show solution" button
    int                 m_nNumButtons;      //num answer buttons

    wxButton**          m_pAnswerButtons;   //buttons for the answers
    int                 m_nIdFirstButton;   //ID of first button; the others in sequence

    bool                m_fControlsCreated; 


    wxSize              m_nDisplaySize;     // DisplayCtrol size (pixels at 1:1)
    double              m_rScale;           // Current scaling factor

private:
    void DoStopSounds();
    void DoDisplaySolution();


    DECLARE_EVENT_TABLE()
};


//--------------------------------------------------------------------------------
// Abstract class to create exercise to compare scores/sounds
//--------------------------------------------------------------------------------
class lmCompareCtrol : public lmExerciseCtrol    
{
   DECLARE_DYNAMIC_CLASS(lmCompareCtrol)

public:

    // constructor and destructor    
    lmCompareCtrol(wxWindow* parent, wxWindowID id,
               lmExerciseConstrains* pConstrains, wxSize nDisplaySize, 
               const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxDefaultSize, int style = 0);

    virtual ~lmCompareCtrol();

    //virtual pure methods to be implemented by derived classes
    virtual void OnDebugShowSourceScore(wxCommandEvent& event)=0;
    virtual void OnDebugDumpScore(wxCommandEvent& event)=0;
    virtual void OnDebugShowMidiEvents(wxCommandEvent& event)=0;

    enum {
        m_NUM_ROWS = 1,
        m_NUM_COLS = 3,
        m_NUM_BUTTONS = 3,
        m_ID_BUTTON = 3010,
    };


protected:
    //virtual pure methods to be implemented by derived classes
    virtual wxString SetNewProblem()=0;    
    virtual wxDialog* GetSettingsDlg()=0;
    virtual wxWindow* CreateDisplayCtrol()=0;
    virtual void Play()=0;
    virtual void PlaySpecificSound(int nButton)=0;
    virtual void DisplaySolution()=0;
    virtual void DisplayProblem()=0;
    virtual void DisplayMessage(wxString& sMsg, bool fClearDisplay)=0;
    virtual void DeleteScores() {}     //should be virtual pure but the linker complains !!!
    virtual void StopSounds() {}   //should be virtual pure but the linker complains !!!

    //virtual methods implemented in this class
    void CreateAnswerButtons(int nHeight, int nSpacing, wxFont& font);
    virtual void InitializeStrings();   
    void ReconfigureButtons() {}

protected:
    // member variables
    wxButton*       m_pAnswerButton[m_NUM_BUTTONS];   //buttons for the answers



    DECLARE_EVENT_TABLE()
};


//--------------------------------------------------------------------------------
// Abstract class to create exercise to compare two scores
//--------------------------------------------------------------------------------
class lmCompareScoresCtrol : public lmCompareCtrol    
{
   DECLARE_DYNAMIC_CLASS(lmCompareScoresCtrol)

public:

    // constructor and destructor    
    lmCompareScoresCtrol(wxWindow* parent, wxWindowID id,
               lmExerciseConstrains* pConstrains, wxSize nDisplaySize, 
               const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxDefaultSize, int style = 0);

    virtual ~lmCompareScoresCtrol();

    // event handlers
    void OnEndOfPlay(lmEndOfPlayEvent& WXUNUSED(event));
    void OnTimerEvent(wxTimerEvent& WXUNUSED(event));

    //implementation of virtual event handlers
    virtual void OnDebugShowSourceScore(wxCommandEvent& event);
    virtual void OnDebugDumpScore(wxCommandEvent& event);
    virtual void OnDebugShowMidiEvents(wxCommandEvent& event);

protected:
    //virtual pure methods to be implemented by derived classes
    virtual wxString SetNewProblem()=0;    
    virtual wxDialog* GetSettingsDlg()=0;
    
    //implementation of some virtual methods
    void Play();
    void PlaySpecificSound(int nButton) {}
    void DisplaySolution();
    void DisplayProblem();
    void DeleteScores();
    void StopSounds();
    wxWindow* CreateDisplayCtrol();
    void DisplayMessage(wxString& sMsg, bool fClearDisplay);


protected:
   // member variables
    lmScore*    m_pScore[2];        //the two problem scores
    int         m_nNowPlaying;      //score being played (0, 1)
    wxTimer     m_oPauseTimer;      //timer to do a pause between the two scores
    lmScore*    m_pSolutionScore;   //solution score
    int         m_nPlayMM;          //metronome setting to play the scores
    bool        m_fPlaying;         //currently playing the score

private:
    void PlayScore(int nIntv);

    DECLARE_EVENT_TABLE()
};

//--------------------------------------------------------------------------------
// Abstract class to create exercises with one problem score
//--------------------------------------------------------------------------------
class lmOneScoreCtrol : public lmExerciseCtrol      //lmExerciseCtrol    
{
   DECLARE_DYNAMIC_CLASS(lmOneScoreCtrol)

public:

    // constructor and destructor    
    lmOneScoreCtrol(wxWindow* parent, wxWindowID id,
               lmExerciseConstrains* pConstrains, wxSize nDisplaySize, 
               const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxDefaultSize, int style = 0);

    virtual ~lmOneScoreCtrol();

    //event handlers
    void OnEndOfPlay(lmEndOfPlayEvent& WXUNUSED(event));

    //implementation of virtual event handlers
    virtual void OnDebugShowSourceScore(wxCommandEvent& event);
    virtual void OnDebugDumpScore(wxCommandEvent& event);
    virtual void OnDebugShowMidiEvents(wxCommandEvent& event);


protected:
    //virtual pure methods from parent class to be implemented by derived classes
    virtual wxString SetNewProblem() {return _T(""); }     //should be virtual pure but the linker doesn't do its job properly !!! 
    virtual wxDialog* GetSettingsDlg() {return NULL; }     //should be virtual pure but the linker doesn't do its job properly !!! 
    virtual void CreateAnswerButtons(int nHeight, int nSpacing, wxFont& font) {}     //should be virtual pure but the linker doesn't do its job properly !!! 
    virtual void InitializeStrings() {}     //should be virtual pure but the linker doesn't do its job properly !!!   
    virtual void ReconfigureButtons() {}     //should be virtual pure but the linker doesn't do its job properly !!! 

    //virtual pure methods defined in this class
    virtual void PrepareAuxScore(int nButton)=0;

    //implementation of some virtual methods
    void Play();
    void PlaySpecificSound(int nButton);
    void DisplaySolution();
    void DisplayProblem();
    void DeleteScores();
    void StopSounds();
    wxWindow* CreateDisplayCtrol();
    void DisplayMessage(wxString& sMsg, bool fClearDisplay);

        // member variables

    lmScore*    m_pProblemScore;    //score with the problem
	lmScore*    m_pSolutionScore;	//if not NULL, score with the solution. If NULL
                                    //   problem score will be used as solution score
    lmScore*    m_pAuxScore;        //score to play user selected buttons
    int         m_nPlayMM;          //metronome setting to play scores
    bool        m_fPlaying;         //currently playing the score

    DECLARE_EVENT_TABLE()
};

//--------------------------------------------------------------------------------
// Abstract class to create exercises to compare two Midi pitches
//--------------------------------------------------------------------------------
class lmCompareMidiCtrol : public lmCompareCtrol    
{
   DECLARE_DYNAMIC_CLASS(lmCompareMidiCtrol)

public:

    // constructor and destructor    
    lmCompareMidiCtrol(wxWindow* parent, wxWindowID id,
               lmExerciseConstrains* pConstrains, wxSize nDisplaySize, 
               const wxPoint& pos = wxDefaultPosition, 
               const wxSize& size = wxDefaultSize, int style = 0);

    virtual ~lmCompareMidiCtrol();

    // event handlers
    void OnTimerEvent(wxTimerEvent& WXUNUSED(event));

    //implementation of virtual event handlers
    virtual void OnDebugShowSourceScore(wxCommandEvent& event) {}
    virtual void OnDebugDumpScore(wxCommandEvent& event) {}
    virtual void OnDebugShowMidiEvents(wxCommandEvent& event) {}

protected:
    //virtual pure methods to be implemented by derived classes
    virtual wxString SetNewProblem()=0;    
    virtual wxDialog* GetSettingsDlg()=0;
    
    //implementation of some virtual methods
    virtual void Play();
    void PlaySpecificSound(int nButton) {}
    void DisplaySolution();
    void DisplayProblem();
    void DeleteScores() {}
    void StopSounds();
    wxWindow* CreateDisplayCtrol();
    void DisplayMessage(wxString& sMsg, bool fClearDisplay);

protected:
    void PlaySound(int iSound);


        // member variables

    lmMPitch            m_mpPitch[2];
    int                 m_nChannel[2];
    int                 m_nInstr[2];
    long                m_nTimeIntval[2];   //interval between first and second pitch
    bool                m_fStopPrev;        //stop previous pitch when sounding the next pitch

    wxTimer             m_oTimer;           //timer to control sounds' duration
    int                 m_nNowPlaying;      //pitch number being played or -1

    DECLARE_EVENT_TABLE()
};


#endif  // __EXERCISECTROL_H__
