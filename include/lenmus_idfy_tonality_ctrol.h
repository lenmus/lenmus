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

#ifndef __LENMUS_IDFY_TONALITY_CTROL_H__        //to avoid nested includes
#define __LENMUS_IDFY_TONALITY_CTROL_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_exercise_ctrol.h"

//lomse
#include <lomse_pitch.h>
using namespace lomse;


namespace lenmus
{

class TonalityConstrains;

//---------------------------------------------------------------------------------------
class IdfyTonalityCtrol : public OneScoreCtrol
{
public:

    // constructor and destructor
    IdfyTonalityCtrol(long dynId, ApplicationScope& appScope, DocumentWindow* pCanvas);

    ~IdfyTonalityCtrol();

    //implementation of virtual pure in parent EBookCtrol
    void get_ctrol_options_from_params();
    void set_problem_space();

    //implementation of virtual methods
    void initialize_strings();
    void initialize_ctrol();
    void create_answer_buttons(LUnits height, LUnits spacing);
    ImoScore* prepare_aux_score(int nButton);
    wxString set_new_problem();
    wxDialog* get_settings_dialog();
    void on_settings_changed();
    void EnableButtons(bool value);
    bool are_answer_buttons_allowed_for_playing() { return false; }

private:
    wxString prepare_score(EClef nClef, EKeySignature nType,
                          ImoScore** pProblemScore);
    void ComputeRightAnswerButtons();

        // member variables

    enum {
        k_num_cols = 5,
        k_num_rows = 7,
        k_num_buttons = 35,     // <= NUM_COLS * NUM_ROWS;
    };

    TonalityConstrains* m_pConstrains;       //constraints for the exercise

    //problem asked
    EKeySignature  m_nKey;

    //answer
    ButtonCtrl*    m_pAnswerButton[k_num_buttons];  //buttons for the answers
    EKeySignature  m_nRealKey[k_num_buttons];       //key signature for each button

};


}   // namespace lenmus

#endif  // __LENMUS_IDFY_TONALITY_CTROL_H__
