//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2010-2020 LenMus project
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
#include "lenmus_standard_header.h"
#include "lenmus_dyncontrol.h"
#include "lenmus_constrains.h"
#include "lenmus_counters_ctrol.h"
#include "lenmus_events.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/event.h>

//lomse
#include <lomse_events.h>
#include <lomse_hyperlink_ctrl.h>
#include <lomse_player_gui.h>
#include <lomse_button_ctrl.h>
using namespace lomse;


namespace lenmus
{

// forward declarations
class DocumentCanvas;
class ProblemManager;
class CountersCtrol;
class ProblemDisplayer;


//--------------------------------------------------------------------------------
// An abstract class for any kind of Ctrol included in an eBook.
class EBookCtrol : public DynControl, public EventHandler, public PlayerNoGui
{
protected:
    DocumentWindow*     m_pCanvas;
    EBookCtrolOptions*  m_pBaseConstrains;
    ImoDynamic*         m_pDyn;
    Document*           m_pDoc;
    ADynamic            m_dyn;
    ADocument           m_doc;
    bool                m_fControlsCreated;
    int                 m_state;        //interal state (FSM)
    ImoId               m_dynId;

    EBookCtrol(long dynId, ApplicationScope& appScope, DocumentWindow* pCanvas);

public:
    virtual ~EBookCtrol();

    //implementation of virtual pure in parent DynControl
    void generate_content(ADynamic dyn, ADocument doc) override;
    void handle_event(SpEventInfo pEvent) override;

    virtual void get_ctrol_options_from_params() = 0;
    virtual void initialize_ctrol() = 0;

    //virtual pure event handlers to be implemented by derived classes
    virtual void on_debug_show_source_score()=0;
    virtual void on_debug_show_midi_events()=0;

    //event handlers. No need to implement in derived classes
    virtual void on_settings_button();
    virtual void on_go_back();

    //overrides of PlayerNoGui for using general metronome for speed settings
    int get_metronome_mm() override;
    Metronome* get_metronome() override;

    //access to parent window
    wxWindow* get_parent_window();

protected:
    //virtual methods to be implemented by derived classes
    virtual void initialize_strings()=0;
    virtual wxDialog* get_settings_dialog()=0;
    virtual void on_settings_changed()=0;

private:
//    void do_stop_sounds();
};



//---------------------------------------------------------------------------------------
// An abstract class for any kind of exercise included in an eBook.
class ExerciseCtrol : public EBookCtrol
{
protected:
    ExerciseCtrol(long dynId, ApplicationScope& appScope, DocumentWindow* pCanvas);

public:
    virtual ~ExerciseCtrol();

    //mandatory override for EventHandler
    virtual void handle_event(SpEventInfo pEvent);

    //event handlers. No need to implement in derived classes
    virtual void on_resp_button(int iButton);
    virtual void on_new_problem();
    virtual void on_display_solution();
    virtual void on_button_mouse_in(SpEventMouse pEvent);
    virtual void on_button_mouse_out(SpEventMouse pEvent);

    //other
    virtual void OnQuestionAnswered(int iQ, bool fSuccess);
    inline int get_generation_mode() { return m_nGenerationMode; }
    void change_generation_mode(int nMode);
    static void on_exercise_activated(void* pThis, SpEventInfo pEvent);


protected:
    //virtual methods to be implemented by derived classes
    virtual void create_answer_buttons(LUnits height, LUnits spacing) = 0;
    virtual wxString set_new_problem()=0;
    virtual void play(bool fVisualTracking=true)=0;
    virtual void stop_sounds()=0;
    virtual void play_specific_sound(int nButton)=0;
    virtual void display_solution()=0;
    virtual void display_problem_score()=0;
    virtual void delete_scores()=0;
    virtual void set_problem_space()=0;

    //methods that, normally, it is not necessary to override in derived classes
    virtual void set_button_color(int i, Color color);
    virtual void enable_buttons(bool fEnable);
    virtual void new_problem();
    virtual void reset_exercise();
    virtual void set_event_handlers();
    virtual void create_problem_display_box(ImoContent* pWrapper, ImoStyle* pStyle=nullptr);
    virtual bool is_play_button_initially_enabled();
    virtual bool check_success(int nButton);
    virtual void display_first_time_content();
    virtual string get_initial_msge();

    //methods invoked from derived classes
    virtual void create_controls();
    void set_buttons(ButtonCtrl* pButton[], int nNumButtons);

    //internal methods
    CountersCtrol* create_counters_ctrol(ImoContent* pWrapper);
    void create_problem_manager();
    void change_from_learning_to_practising();
    void display_initial_msge();
    void create_display_and_counters();

    //helper
    inline bool is_learning_mode() { return m_nGenerationMode == k_learning_mode; }
    inline bool is_solution_displayed() { return m_fSolutionDisplayed; }
    inline bool is_theory_mode() {
        return (static_cast<ExerciseOptions*>( m_pBaseConstrains ))->is_theory_mode();
    }

    //wrappers for event handlers
    static void on_new_problem(void* pThis, SpEventInfo pEvent);
    static void on_play_event(void* pThis, SpEventInfo pEvent);
    static void on_display_solution(void* pThis, SpEventInfo pEvent);
    static void on_settings(void* pThis, SpEventInfo pEvent);
    static void on_see_source_score(void* pThis, SpEventInfo pEvent);
    static void on_see_midi_events(void* pThis, SpEventInfo pEvent);
    static void on_go_back_event(void* pThis, SpEventInfo pEvent);

    void change_counters_ctrol();
    void remove_counters_ctrol();


        // member variables

    //display control variables
    ImoScore*       m_pScoreToPlay;
    ProblemDisplayer* m_pDisplay;

    CountersCtrol*  m_pCounters;
    bool            m_fCountersValid;   //when changing mode counters might not be valid
    ImoContent*     m_pCountersWrapper;
    ImoParagraph*   m_pCountersPara;

    bool                m_fQuestionAsked;   //question asked but not yet answered
    int                 m_nRespIndex;       //index to the button with the right answer
    int                 m_nRespAltIndex;    //alternative right answer (i.e. enarmonic answer)

    wxString            m_sAnswer;          //string with the right answer
    bool                m_fSolutionDisplayed;

    HyperlinkCtrl*     m_pPlayButton;      // "play" button
    HyperlinkCtrl*     m_pNewProblem;      //"New problem" link
    HyperlinkCtrl*     m_pShowSolution;    //"Show solution" link

    int                 m_nNumButtons;      //num answer buttons
    ButtonCtrl**        m_pAnswerButtons;   //buttons for the answers
    long                m_nIdFirstButton;   //ID of first button; the others in sequence

    //to generate problems
    int                     m_nGenerationMode;
    int                     m_nProblemLevel;
    ProblemManager*         m_pProblemManager;
    int                     m_iQ;               //index of asked question
    wxString                m_sKeyPrefix;

    //to measure times
    wxDateTime              m_tmAsked;      //when was asked last question

private:
//    void do_stop_sounds();
    void do_display_solution();

};


//---------------------------------------------------------------------------------------
// Abstract class to create exercise to compare scores/sounds
class CompareCtrol : public ExerciseCtrol
{
public:

    // constructor and destructor
    CompareCtrol(long dynId, ApplicationScope& appScope, DocumentWindow* pCanvas);
    virtual ~CompareCtrol();

    enum {
        k_num_rows = 1,
        k_num_cols = 3,
        k_num_buttons = 3,
        m_ID_BUTTON = 3010,
    };


protected:
    //implementation of virtual methods from  ExerciseCtrol
    void create_answer_buttons(LUnits height, LUnits spacing) override;

    //implementation of virtual methods from EBookCtrol
    void initialize_strings() override;

protected:
    // member variables
    ButtonCtrl*     m_pAnswerButton[k_num_buttons];   //buttons for the answers

};


//---------------------------------------------------------------------------------------
// Abstract class to create exercise to compare two scores
class CompareScoresCtrol : public wxEvtHandler, public CompareCtrol
{
public:

    // constructor and destructor
    CompareScoresCtrol(long dynId, ApplicationScope& appScope, DocumentWindow* pCanvas);
    virtual ~CompareScoresCtrol();

    //implementation of virtual methods from EBookCtrol
    void on_debug_show_source_score() override;
    void on_debug_show_midi_events() override;

    virtual void on_end_of_playback();

protected:
    //implementation of virtual methods from ExerciseCtrol
    void play(bool fVisualTracking=true) override;
    void play_specific_sound(int WXUNUSED(nButton)) override {}
    void display_solution() override;
    void display_problem_score() override;
    void delete_scores() override;
    void stop_sounds() override;


protected:
    // wxWidgets event handlers
    void on_timer_event(wxTimerEvent& WXUNUSED(event));

    ScorePlayer*    m_pPlayer;
    ImoScore*       m_pScore[2];        //the two problem scores
    int             m_nNowPlaying;      //score being played (0, 1)
    wxTimer         m_oPauseTimer;      //timer to do a pause between the two scores
    ImoScore*       m_pSolutionScore;   //solution score
    int             m_nPlayMM;          //metronome setting to play the scores
    bool            m_fPlayingProblem;  //currently playing the score (might be waiting for timer event)

private:
    void PlayScore(int nIntv, bool fVisualTracking);

    wxDECLARE_CLASS(CompareScoresCtrol);
};

//---------------------------------------------------------------------------------------
// Abstract class to create exercises with one problem score
class OneScoreCtrol : public ExerciseCtrol
{
protected:
    OneScoreCtrol(long dynId, ApplicationScope& appScope, DocumentWindow* pCanvas);

public:
    virtual ~OneScoreCtrol();

    //implementation of virtual methods from EBookCtrol
    void on_debug_show_source_score() override;
    void on_debug_show_midi_events() override;

    virtual void on_end_of_playback();

protected:

    //virtual pure methods defined in this class
    virtual ImoScore* prepare_aux_score(int nButton)=0;
    virtual bool are_answer_buttons_allowed_for_playing();
    virtual bool is_play_again_message_allowed();
    virtual bool remove_problem_text_when_displaying_solution();

    //implementation of virtual methods from ExerciseCtrol
    void play(bool fVisualTracking=true) override;
    void play_specific_sound(int nButton) override;
    void display_solution() override;
    void display_problem_score() override;
    void delete_scores() override;
    void stop_sounds() override;

        // member variables

    ScorePlayer* m_pPlayer;
    ImoScore*   m_pProblemScore;    //score with the problem
	ImoScore*   m_pSolutionScore;	//if not nullptr, score with the solution. If nullptr
                                    //   problem score will be used as solution score
    ImoScore*   m_pAuxScore;        //score to play user selected buttons
    int         m_nPlayMM;          //metronome setting to play scores
};

////---------------------------------------------------------------------------------------
//// Abstract class to create exercises to compare two Midi pitches
//class CompareMidiCtrol : public CompareCtrol
//{
//public:
//    CompareMidiCtrol(long dynId, ApplicationScope& appScope, DocumentCanvas* pCanvas);
//    virtual ~CompareMidiCtrol();
//
//    // event handlers
//    void OnTimerEvent(wxTimerEvent& WXUNUSED(event));
//
//    //implementation of virtual methods from EBookCtrol
//    void on_debug_show_source_score() override {}
//    void on_debug_show_midi_events() override {}
//
//protected:
//    //implementation of virtual methods from ExerciseCtrol
//    void play() override;
//    void play_specific_sound(int nButton) override {}
//    void display_solution() override;
//    void display_problem_score() override;
//    void delete_scores() override {}
//    void stop_sounds() override;
//
//protected:
//    void PlaySound(int iSound);
//
//
//        // member variables
//
//    MidiPitch            m_mpPitch[2];
//    int                 m_nChannel[2];
//    int                 m_nInstr[2];
//    long                m_nTimeIntval[2];   //interval between first and second pitch
//    bool                m_fStopPrev;        //stop previous pitch when sounding the next pitch
//
//    wxTimer             m_oTimer;           //timer to control sounds' duration
//    int                 m_nNowPlaying;      //pitch number being played or -1
//};


//---------------------------------------------------------------------------------------
// Abstract class for any kind of exercise using the score editor
class FullEditorCtrol : public EBookCtrol
{
protected:
    ExerciseOptions* m_pConstrains;

    //the scores
    ImoScore* m_pProblemScore;  //(opt) the problem (aural training)
    ImoScore* m_pContextScore;  //(opt) to play before problem, to create pitch/tonality context
    ImoScore* m_pUserScore;     //the one to be displayed and edited by student

    //score player and its settings
    ScorePlayer* m_pPlayer;
    int m_nPlayMM;              //metronome setting to play scores
    int m_midiVoice;            //instrument to use for playing scores

    //display control variables
    ImoScore*           m_pUserScoreToPlay;
    ImoScore*           m_pProblemScoreToPlay;
    ProblemDisplayer*   m_pDisplay;

    //action buttons
    HyperlinkCtrl*  m_pPlayProblem;     //"Play problem again" button
    HyperlinkCtrl*  m_pPlayUserScore;   //"Play my solution" button
    HyperlinkCtrl*  m_pNewProblem;      //"New problem" link
    HyperlinkCtrl*  m_pDoneButton;      //"Done" link

    //other
    int m_numTimesProblemPlayed;

    //pre-translated strings for button labels
    string m_label_new_problem;
    string m_label_play_problem;
    string m_label_done;
    string m_label_play_user_score;
    string m_label_stop_playing;

    FullEditorCtrol(long dynId, ApplicationScope& appScope, DocumentWindow* pCanvas);

public:
    virtual ~FullEditorCtrol();

    //implementation of virtual pure in parent EBookCtrol
    void on_debug_show_source_score() override;
    void on_debug_show_midi_events() override;

    //specific

    //event handlers. No need to implement in derived classes
    virtual void on_end_of_playback();

protected:
    //virtual methods to be implemented by derived classes
    virtual wxString generate_new_problem()=0;
    virtual void do_correct_exercise()=0;
    virtual void delete_specific_scores()=0;

    //overridable, for changing default exercise layout
    ImoStyle* m_pLinksSpacerStyle;
    virtual void layout_exercise();
    virtual void add_settings_goback_and_debug_links(ImoParagraph* pContainer,
                                                     ImoStyle* pSpacerStyle=nullptr);
    virtual void add_new_problem_link(ImoParagraph* pContainer, ImoStyle* pSpacerStyle=nullptr);
    virtual void add_play_problem_link(ImoParagraph* pContainer, ImoStyle* pSpacerStyle=nullptr);
    virtual void add_done_link(ImoParagraph* pContainer, ImoStyle* pSpacerStyle=nullptr);
    virtual void add_play_solution_link(ImoParagraph* pContainer, ImoStyle* pSpacerStyle=nullptr);
    virtual void add_optional_content_block_1() {}
    virtual void add_optional_content_block_2() {}

    //methods that, normally, it is not necessary to override in derived classes
    virtual void process_event(int event);
    virtual void new_problem();
    virtual void initial_play_of_problem();
    virtual void reset_exercise();
    virtual void correct_exercise();
    virtual void create_problem_display_box(ImoContent* pWrapper, ImoStyle* pStyle=nullptr);
    virtual void enable_links_for_current_state();

    void create_controls();
    void display_user_score();
    void enable_edition();
    void disable_edition();
    void play_or_stop_user_score();
    void play_or_stop_problem(bool fDoCountOff=false);
    void do_play_problem(bool fDoCountOff);
    void stop_sounds();
    void delete_scores();
    void mover_cursor_to_end();
    void change_state(int newState);

    //events
    enum {
        k_click_new_problem=0,
        k_click_play_or_stop_problem,
        k_click_done,
        k_click_play_or_stop_user_score,
        k_end_of_playback,
        //
        k_first_unused_event,
    };

    //exercise states
    enum {
        k_state_start=0,            //user has entered in exercise page
        k_state_playing_problem,    //program is playing the problem score
        k_state_ready,              //user can start solving the problem
        k_state_solution,           //solution is displayed
        k_state_playing_user,       //program is playing user solution
        k_state_replaying_problem,  //program is playing again problem score

        k_state_first_unused
    };


    //helper
//    inline bool is_solution_displayed() { return m_fSolutionDisplayed; }
    inline bool is_theory_mode() {
        return (static_cast<ExerciseOptions*>( m_pBaseConstrains ))->is_theory_mode();
    }

    //wrappers for event handlers
    static void on_new_problem(void* pThis, SpEventInfo pEvent);
    static void on_play_problem(void* pThis, SpEventInfo pEvent);
    static void on_play_user_score(void* pThis, SpEventInfo pEvent);
    static void on_correct_exercise(void* pThis, SpEventInfo pEvent);
    static void on_settings(void* pThis, SpEventInfo pEvent);
    static void on_see_source_score(void* pThis, SpEventInfo pEvent);
    static void on_see_midi_events(void* pThis, SpEventInfo pEvent);
    static void on_go_back_event(void* pThis, SpEventInfo pEvent);
};


}   //namespace lenmus

#endif  // __LENMUS_EXERCISE_CTROL_H__
