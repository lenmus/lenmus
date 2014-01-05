//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2014 LenMus project
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

#ifndef __LENMUS_DICTATION_CTROL_H__        //to avoid nested includes
#define __LENMUS_DICTATION_CTROL_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_exercise_ctrol.h"

//lomse
#include <lomse_pitch.h>
using namespace lomse;


namespace lenmus
{

//forward declarations
class DictationConstrains;
class ScoreConstrains;


//---------------------------------------------------------------------------------------
// Base class for all dictation exercises
class DictationCtrol : public FullEditorCtrol
{
protected:

    //problem parameters
    EClef           m_clefType;
    EKeySignature   m_keyType;
    ETimeSignature  m_timeType;

    //playing fragments
    HyperlinkCtrl*  m_pPlayFragment[4];         //"Play fragment" links
    int             m_numTimesFragmentPlayed[4];

    //labels for links
    string  m_label_play[4];
    string  m_label_stop[4];

    //??????????
    ScoreConstrains*        m_pScoreConstrains;
    DictationConstrains*    m_pConstrains;       //constraints for the exercise

    DictationCtrol(long dynId, ApplicationScope& appScope, DocumentWindow* pCanvas);

public:
    virtual ~DictationCtrol();

    //mandatory overrides from EBookCtrol
    void get_ctrol_options_from_params();
    void initialize_ctrol();

protected:
    //mandatory overrides from EBookCtrol
    void initialize_strings();
    wxDialog* get_settings_dialog();
    void on_settings_changed();

    //mandatory overrides from FullEditorCtrol
    wxString generate_new_problem();
    void do_correct_exercise();
    void display_solution();
    void delete_specific_scores() {}

    //overrides, to change exercise layout and play fragments
    void add_optional_content_block_1();
    void enable_links_for_current_state();
    void process_event(int event);
    void initial_play_of_problem();

    //virtual pure, to be implemented in derived classes
    virtual int num_fragments()=0;
    virtual bool is_melodic_dictation()=0;
    virtual void prepare_problem_score()=0;
    virtual void prepare_user_score()=0;

    //other
    void play_or_stop_fragment(int num);
    void add_play_fragment_link(int i, ImoParagraph* pContainer);
    void clear_play_counters();
    void play_context_score();
    void prepare_context_score();
    void add_chord(int chordRoot, ImoInstrument* pInstr, const string& noteType);
    void display_problem_score();
    void display_no_error_message();

    //exercise states
    enum {
        k_state_start=0,            //user has entered in exercise page
        k_state_playing_fragment_1, //program is playing fragment 1
        k_state_playing_fragment_2, //program is playing fragment 2
        k_state_playing_fragment_3, //program is playing fragment 3
        k_state_playing_fragment_4, //program is playing fragment 4
        k_state_playing_complete,   //program is playing complete problem score
        k_state_ready,              //user can start solving the problem
        k_state_solution,           //solution is displayed
        k_state_playing_user,       //program is playing user solution
        k_state_replaying_problem,  //program is playing again the problem score
        k_state_replaying_fragment_1, //program is replaying fragment 1
        k_state_replaying_fragment_2, //program is replaying fragment 2
        k_state_replaying_fragment_3, //program is replaying fragment 3
        k_state_replaying_fragment_4, //program is replaying fragment 4
        k_state_playing_context,    //program is playing tonal context

        k_state_first_unused
    };

    //more states
    enum {
        k_event_play_context_score = FullEditorCtrol::k_first_unused_event,
        k_click_play_or_stop_fragment_1,
        k_click_play_or_stop_fragment_2,
        k_click_play_or_stop_fragment_3,
        k_click_play_or_stop_fragment_4,
    };

    //wrappers for event handlers
    static void on_play_fragment_1(void* pThis, SpEventInfo pEvent);
    static void on_play_fragment_2(void* pThis, SpEventInfo pEvent);
    static void on_play_fragment_3(void* pThis, SpEventInfo pEvent);
    static void on_play_fragment_4(void* pThis, SpEventInfo pEvent);
};

//---------------------------------------------------------------------------------------
class RhythmicDictationCtrol : public DictationCtrol
{
protected:

public:
    RhythmicDictationCtrol(long dynId, ApplicationScope& appScope, DocumentWindow* pCanvas);
    virtual ~RhythmicDictationCtrol();

protected:
    //mandatory overrides from DictationCtrol
    int num_fragments() { return 4; }
    bool is_melodic_dictation() { return false; }
//    bool is_rhythmic_dictation() { return true; }
//    bool is_harmonic_dictation() { return false; }
    void prepare_problem_score();
    void prepare_user_score();

};

//---------------------------------------------------------------------------------------
class MelodicDictationCtrol : public DictationCtrol
{
protected:

public:
    MelodicDictationCtrol(long dynId, ApplicationScope& appScope, DocumentWindow* pCanvas);
    virtual ~MelodicDictationCtrol();

protected:
    //mandatory overrides from DictationCtrol
    int num_fragments() { return 4; }
    bool is_melodic_dictation() { return true; }
    void prepare_problem_score();
    void prepare_user_score();

};

//---------------------------------------------------------------------------------------
class HarmonicDictationCtrol : public DictationCtrol
{
protected:

public:
    HarmonicDictationCtrol(long dynId, ApplicationScope& appScope, DocumentWindow* pCanvas);
    virtual ~HarmonicDictationCtrol();

protected:
    //mandatory overrides from DictationCtrol
    int num_fragments() { return 4; }
    bool is_melodic_dictation() { return true; }
    void prepare_problem_score();
    void prepare_user_score();

};


}   // namespace lenmus

#endif  // __LENMUS_DICTATION_CTROL_H__
