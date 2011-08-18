//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2011 LenMus project
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
//---------------------------------------------------------------------------------------

#ifndef __LENMUS_EXERCISE_CTROL_H__        //to avoid nested includes
#define __LENMUS_EXERCISE_CTROL_H__

//lenmus
#include "lenmus_dyncontrol.h"

//#include "../score/Score.h"
#include "lenmus_constrains.h"
//#include "auxctrols/ScoreAuxCtrol.h"
//#include "auxctrols/UrlAuxCtrol.h"
//#include "auxctrols/CountersAuxCtrol.h"
//#include "Generators.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>

//lomse
#include <lomse_events.h>
using namespace lomse;


namespace lenmus
{

// forward declarations
class ImoBoxContainer;
class DocumentCanvas;


//--------------------------------------------------------------------------------
// An abstract class for any kind of Ctrol included in an eBook.
class EBookCtrol : public DynControl, public EventHandler
{
protected:
    DocumentCanvas* m_pCanvas;
    EBookCtrolOptions* m_pConstrains;
    ImoDynamic* m_pDyn;
    Document* m_pDoc;
//    std::list<ImoParamInfo*>& m_params;        //options for the exercise
//    lmUrlAuxCtrol*      m_pPlayButton;      // "play" button
//    bool                m_fControlsCreated;
//    bool                m_fDoCountOff;


    EBookCtrol(long dynId, ApplicationScope& appScope, DocumentCanvas* pCanvas);

public:
    virtual ~EBookCtrol();

    //implementation of virtual pure in parent DynControl
    void generate_content(ImoDynamic* pDyn, Document* pDoc);
    virtual void handle_event(EventInfo* pEvent);

    virtual void get_ctrol_options_from_params() = 0;
    virtual void initialize_ctrol() = 0;
    virtual void create_initial_layout() = 0;

//    // virtual pure event handlers to be implemented by derived classes
//    virtual void OnDebugShowSourceScore(EventInfo* pEvent)=0;
//    virtual void OnDebugDumpScore(EventInfo* pEvent)=0;
//    virtual void OnDebugShowMidiEvents(EventInfo* pEvent)=0;

    // event handlers. No need to implement in derived classes
    virtual void on_play();
    virtual void on_settings_button();
//    virtual void OnGoBackButton(EventInfo* pEvent);
//    virtual void OnDoCountoff(EventInfo* pEvent);

protected:
//    //IDs for controls
//    enum {
//        ID_LINK_SEE_SOURCE = 3700,
//        ID_LINK_DUMP,
//        ID_LINK_MIDI_EVENTS,
//        ID_LINK_PLAY,
//        ID_LINK_SETTINGS,
//        ID_LINK_GO_BACK,
//        ID_LINK_COUNTOFF,
//    };

    //virtual methods to be implemented by derived classes
    virtual void initialize_strings()=0;
    virtual wxDialog* get_settings_dialog()=0;
    virtual void play()=0;
    virtual void stop_sounds()=0;
    virtual void on_settings_changed()=0;   // {};

    //methods invoked from derived classes
//    virtual void create_controls()=0;
    virtual void set_buttons(ImoButton* pButton[], int nNumButtons)=0;

private:
//    void DoStopSounds();
};



//---------------------------------------------------------------------------------------
// An abstract class for any kind of exercise included in an eBook.
class ExerciseCtrol : public EBookCtrol
{
protected:
    ExerciseCtrol(long dynId, ApplicationScope& appScope, DocumentCanvas* pCanvas);

public:
    virtual ~ExerciseCtrol();

    //mandatory override for EventHandler
    virtual void handle_event(EventInfo* pEvent);

    // event handlers. No need to implement in derived classes
    virtual void on_resp_button(int iButton);
    virtual void on_new_problem();
    virtual void on_display_solution();
//    virtual void OnModeChanged(EventInfo* pEvent);
//
//    //other
//    virtual void OnQuestionAnswered(int iQ, bool fSuccess);
//    void ChangeCountersCtrol();
//    void ChangeGenerationMode(int nMode);
//    void ChangeGenerationModeLabel(int nMode);


protected:
//    //IDs for controls
//    enum {
//        ID_LINK_SOLUTION = 3006,
//        ID_LINK_NEW_PROBLEM,
//        lmID_CBO_MODE,
//    };

    //virtual methods to be implemented by derived classes
    virtual void create_answer_buttons(LUnits height, LUnits spacing) = 0;
    virtual wxString set_new_problem()=0;
    virtual void create_problem_display_box()=0;
    virtual bool check_success_or_failure(int nButton);

//    virtual void PlaySpecificSound(int nButton)=0;
    virtual void display_solution()=0;
    virtual void display_problem()=0;
    virtual void display_message(const wxString& sMsg, bool fClearDisplay)=0;
    virtual void delete_scores()=0;
//    virtual void SetProblemSpace();

    //methods that, normally, it is not necessary to implement
    virtual void set_button_color(int i, Color color);
    virtual void enable_buttons(bool fEnable);
    virtual void new_problem();
    virtual void reset_exercise();

    //methods invoked from derived classes
    virtual void create_controls();
    void set_buttons(ImoButton* pButton[], int nNumButtons);

//    //internal methods
//    lmCountersAuxCtrol* CreateCountersCtrol();
//    void CreateProblemManager();


        // member variables

    //display control variables
    ImoContent*     m_pDisplayCtrol;
    ImoParagraph*   m_pCurPara;
    ImoScore*       m_pCurScore;

//    lmCountersAuxCtrol* m_pCounters;
//    bool                m_fCountersValid;   //when changing mode counters might not be valid
//    wxChoice*           m_pCboMode;
//
//    ExerciseOptions*  m_pConstrains;      //constraints for the exercise
    bool                m_fQuestionAsked;   //question asked but not yet answered
    int                 m_nRespIndex;       //index to the button with the right answer
    int                 m_nRespAltIndex;    //alternative right answer (i.e. enarmonic answer)

    wxString            m_sAnswer;          //string with the right answer

//    lmUrlAuxCtrol*      m_pNewProblem;      //"New problem" link
//    lmUrlAuxCtrol*      m_pShowSolution;    //"Show solution" link
    int                 m_nNumButtons;      //num answer buttons

    ImoButton**         m_pAnswerButtons;   //buttons for the answers
    long                m_nIdFirstButton;   //ID of first button; the others in sequence

//    wxSize              m_nDisplaySize;     // DisplayCtrol size (pixels at 1:1)
//
//    //to generate problems
//    int                     m_nGenerationMode;
//    int                     m_nProblemLevel;
//    lmProblemManager*       m_pProblemManager;
//    int                     m_iQ;               //index of asked question
//    wxString                m_sKeyPrefix;
//
//    //to measure times
//    wxDateTime              m_tmAsked;      //when was asked last question

private:
//    void DoStopSounds();
    void do_display_solution();

};


////---------------------------------------------------------------------------------------
//// Abstract class to create exercise to compare scores/sounds
//class CompareCtrol : public ExerciseCtrol
//{
//   DECLARE_DYNAMIC_CLASS(CompareCtrol)
//
//public:
//
//    // constructor and destructor
//    CompareCtrol(wxWindow* parent, wxWindowID id,
//               ExerciseOptions* pConstrains, wxSize nDisplaySize,
//               const wxPoint& pos = wxDefaultPosition,
//               const wxSize& size = wxDefaultSize, int style = 0);
//
//    virtual ~CompareCtrol();
//
//    enum {
//        m_NUM_ROWS = 1,
//        m_NUM_COLS = 3,
//        m_NUM_BUTTONS = 3,
//        m_ID_BUTTON = 3010,
//    };
//
//
//protected:
//    //virtual methods implemented in this class
//    void create_answer_buttons(int nHeight, int nSpacing, wxFont& font);
//    virtual void initialize_strings();
//
//protected:
//    // member variables
//    ImoButton*       m_pAnswerButton[m_NUM_BUTTONS];   //buttons for the answers
//
//
//
//    DECLARE_EVENT_TABLE()
//};
//
//
////---------------------------------------------------------------------------------------
//// Abstract class to create exercise to compare two scores
//class lmCompareScoresCtrol : public CompareCtrol
//{
//   DECLARE_DYNAMIC_CLASS(lmCompareScoresCtrol)
//
//public:
//
//    // constructor and destructor
//    lmCompareScoresCtrol(wxWindow* parent, wxWindowID id,
//               ExerciseOptions* pConstrains, wxSize nDisplaySize,
//               const wxPoint& pos = wxDefaultPosition,
//               const wxSize& size = wxDefaultSize, int style = 0);
//
//    virtual ~lmCompareScoresCtrol();
//
//    // event handlers
//    void OnEndOfPlay(lmEndOfPlayEvent& WXUNUSED(event));
//    void OnTimerEvent(wxTimerEvent& WXUNUSED(event));
//
//    //implementation of virtual event handlers
//    virtual void OnDebugShowSourceScore(EventInfo* pEvent);
//    virtual void OnDebugDumpScore(EventInfo* pEvent);
//    virtual void OnDebugShowMidiEvents(EventInfo* pEvent);
//
//protected:
//    //implementation of some virtual methods
//    void play();
//    void PlaySpecificSound(int nButton) {}
//    void display_solution();
//    void display_problem();
//    void delete_scores();
//    void stop_sounds();
//    ImoBoxContainer* create_problem_display_box();
//    void display_message(const wxString& sMsg, bool fClearDisplay);
//
//
//protected:
//   // member variables
//    ImoScore*    m_pScore[2];        //the two problem scores
//    int         m_nNowPlaying;      //score being played (0, 1)
//    wxTimer     m_oPauseTimer;      //timer to do a pause between the two scores
//    ImoScore*    m_pSolutionScore;   //solution score
//    int         m_nPlayMM;          //metronome setting to play the scores
//    bool        m_fPlaying;         //currently playing the score
//
//private:
//    void PlayScore(int nIntv);
//
//    DECLARE_EVENT_TABLE()
//};

//---------------------------------------------------------------------------------------
// Abstract class to create exercises with one problem score
class OneScoreCtrol : public ExerciseCtrol
{
protected:
    OneScoreCtrol(long dynId, ApplicationScope& appScope, DocumentCanvas* pCanvas);

public:
    virtual ~OneScoreCtrol();

    //event handlers
//    void OnEndOfPlay(lmEndOfPlayEvent& WXUNUSED(event));
//
//    //implementation of virtual event handlers
//    virtual void OnDebugShowSourceScore(EventInfo* pEvent);
//    virtual void OnDebugDumpScore(EventInfo* pEvent);
//    virtual void OnDebugShowMidiEvents(EventInfo* pEvent);


protected:

//    //virtual pure methods from parent class to be implemented by derived classes
//    virtual wxString set_new_problem() {return _T(""); }     //should be virtual pure but the linker doesn't do its job properly !!!
//    virtual wxDialog* get_settings_dialog() {return NULL; }     //should be virtual pure but the linker doesn't do its job properly !!!

    //virtual pure methods defined in this class
    virtual void prepare_aux_score(int nButton)=0;

    //implementation of some virtual methods
    virtual void play();
//    void PlaySpecificSound(int nButton);
    void display_solution();
    void display_problem();
    void delete_scores();
    void stop_sounds();
    void create_problem_display_box();
    void display_message(const wxString& sMsg, bool fClearDisplay);

    //specific methods
    void do_play(bool fCountOff);

        // member variables

    ScorePlayer* m_pPlayer;
    ImoScore*   m_pProblemScore;    //score with the problem
	ImoScore*   m_pSolutionScore;	//if not NULL, score with the solution. If NULL
                                    //   problem score will be used as solution score
    ImoScore*   m_pAuxScore;        //score to play user selected buttons
    int         m_nPlayMM;          //metronome setting to play scores
};

////---------------------------------------------------------------------------------------
//// Abstract class to create exercises to compare two Midi pitches
//class CompareMidiCtrol : public CompareCtrol
//{
//public:
//
//    // constructor and destructor
//    CompareMidiCtrol(wxWindow* parent, wxWindowID id,
//               ExerciseOptions* pConstrains, wxSize nDisplaySize,
//               const wxPoint& pos = wxDefaultPosition,
//               const wxSize& size = wxDefaultSize, int style = 0);
//
//    virtual ~CompareMidiCtrol();
//
//    // event handlers
//    void OnTimerEvent(wxTimerEvent& WXUNUSED(event));
//
//    //implementation of virtual event handlers
//    virtual void OnDebugShowSourceScore(EventInfo* pEvent) {}
//    virtual void OnDebugDumpScore(EventInfo* pEvent) {}
//    virtual void OnDebugShowMidiEvents(EventInfo* pEvent) {}
//
//protected:
//    //implementation of some virtual methods
//    virtual void play();
//    void PlaySpecificSound(int nButton) {}
//    void display_solution();
//    void display_problem();
//    void delete_scores() {}
//    void stop_sounds();
//    ImoBoxContainer* create_problem_display_box();
//    void display_message(const wxString& sMsg, bool fClearDisplay);
//
//protected:
//    void PlaySound(int iSound);
//
//
//        // member variables
//
//    lmMPitch            m_mpPitch[2];
//    int                 m_nChannel[2];
//    int                 m_nInstr[2];
//    long                m_nTimeIntval[2];   //interval between first and second pitch
//    bool                m_fStopPrev;        //stop previous pitch when sounding the next pitch
//
//    wxTimer             m_oTimer;           //timer to control sounds' duration
//    int                 m_nNowPlaying;      //pitch number being played or -1
//};
//
//
////---------------------------------------------------------------------------------------
//// An abstract class for any kind of exercise included in an eBook, that uses
//// the full score editor for the exercise
//class FullEditorExercise : public wxWindow
//{
//public:
//
//    // constructor and destructor
//    FullEditorExercise(wxWindow* parent, wxWindowID id,
//               ExerciseOptions* pConstrains,
//               const wxPoint& pos = wxDefaultPosition,
//               const wxSize& size = wxDefaultSize, int style = 0);
//
//    virtual ~FullEditorExercise();
//
//    //event handlers
//    void OnSize(wxSizeEvent& event);
//    void on_settings_button(EventInfo* pEvent);
//    void OnGoBackButton(EventInfo* pEvent);
//    void on_new_problem(EventInfo* pEvent);
//
//protected:
//    //IDs for controls
//    enum {
//        ID_LINK_SETTINGS = 3000,
//        ID_LINK_GO_BACK,
//        ID_LINK_NEW_PROBLEM,
//    };
//
//    //virtual pure methods to be implemented by derived classes
//    virtual void initialize_strings()=0;
//    virtual wxDialog* get_settings_dialog()=0;
//    virtual void on_settings_changed()=0;
//    virtual void set_new_problem()=0;
//    virtual lmEditorMode* CreateEditMode() = 0;
//
//
//    //methods invoked from derived classes
//    virtual void create_controls();
//
//
//    // member variables
//
//    ImoScore*            m_pProblemScore;    //score with the problem
//    wxBoxSizer*         m_pMainSizer;
//    ExerciseOptions*  m_pConstrains;      //constraints for the exercise
//    double              m_rScale;           //current scaling factor
//};


}   //namespace lenmus

#endif  // __LENMUS_EXERCISE_CTROL_H__
