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

#ifndef __LENMUS_IDFY_NOTES_CTROL_H__        //to avoid nested includes
#define __LENMUS_IDFY_NOTES_CTROL_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_exercise_ctrol.h"

//lomse
#include <lomse_hyperlink_ctrl.h>
using namespace lomse;


namespace lenmus
{

//forward declarations
class NotesConstrains;

//---------------------------------------------------------------------------------------
class IdfyNotesCtrol : public OneScoreCtrol
{
public:

    // constructor and destructor
    IdfyNotesCtrol(long dynId, ApplicationScope& appScope, DocumentWindow* pCanvas);

    ~IdfyNotesCtrol();

    //implementation of virtual pure in parent EBookCtrol
     void get_ctrol_options_from_params();
    void initialize_ctrol();
    void on_settings_changed();

    //implementation of virtual methods
    void initialize_strings();
    void create_answer_buttons(LUnits height, LUnits spacing);
    ImoScore* prepare_aux_score(int nButton);
    wxString set_new_problem();
    wxDialog* get_settings_dialog();
    void EnableButtons(bool fEnable);
    bool are_answer_buttons_allowed_for_playing() { return false; }

    //to serve event handlers
    void play_a4();
    void play_all_notes();
    void on_continue();

    //overrides
    void on_new_problem();
    void on_resp_button(int iButton);
    void display_solution();

protected:
    void prepare_score(EClef nClef, const string& sNotePitch,
                       ImoScore** pProblemScore, ImoScore** pSolutionScore = nullptr );
    int get_first_octave_for_clef(EClef nClef);
    void prepare_score_with_all_notes();
    void set_initial_state();
    void display_all_notes();
    void set_problem_space();

    //wrappers for event handlers
    static void on_play_all_notes_event(void* pThis, SpEventInfo pEvent);
    static void on_play_a4_event(void* pThis, SpEventInfo pEvent);
    static void on_continue_event(void* pThis, SpEventInfo pEvent);


        // member variables

    enum {
        k_num_buttons = 12,
    };

    NotesConstrains* m_pConstrains;       //constraints for the exercise

    //problem asked
    EKeySignature  m_nKey;

    //answer
    ButtonCtrl* m_pAnswerButton[k_num_buttons];     //buttons for the answers
    string      m_sButtonLabel[k_num_buttons];

    //specific controls and data
    HyperlinkCtrl*  m_pPlayA4;          //"Play A4 reference note" link
    HyperlinkCtrl*  m_pPlayAllNotes;    //"Play all notes to identify" link
    HyperlinkCtrl*  m_pContinue;        //"Continue" link
};


}   // namespace lenmus

#endif  // __LENMUS_IDFY_NOTES_CTROL_H__
