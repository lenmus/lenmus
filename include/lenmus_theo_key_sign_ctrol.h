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

#ifndef __LENMUS_THEO_KEY_SIGN_CTROL_H__        //to avoid nested includes
#define __LENMUS_THEO_KEY_SIGN_CTROL_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_exercise_ctrol.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>


namespace lenmus
{

//forward declarations
class TheoKeySignConstrains;


//---------------------------------------------------------------------------------------
class TheoKeySignCtrol : public OneScoreCtrol
{
public:
    TheoKeySignCtrol(long dynId, ApplicationScope& appScope, DocumentWindow* pCanvas);
    ~TheoKeySignCtrol();

    //implementation of virtual pure in parent EBookCtrol
    void get_ctrol_options_from_params();
    void on_settings_changed();
    void set_problem_space();

    //implementation of virtual methods
    void initialize_strings();
    void initialize_ctrol();
    void create_answer_buttons(LUnits height, LUnits spacing);
    ImoScore* prepare_aux_score(int WXUNUSED(nButton)) { return nullptr; }
    wxString set_new_problem();
    wxDialog* get_settings_dialog();

private:

    enum {
        k_num_cols = 4,
        k_num_rows = 4,
        k_num_buttons = 15,
    };

    // member variables

    TheoKeySignConstrains* m_pConstrains;

    //buttons for the answers: 3 rows, 5 buttons per row
    ButtonCtrl*     m_pAnswerButton[k_num_buttons];
    int             m_nIndexKeyName;        //index to right answer button
    bool            m_fButtonsEnabled;      //buttons enabled

    bool            m_fMajorMode;           //major mode
    bool            m_fIdentifyKey;         //type of problem
};


}   // namespace lenmus

#endif  // __LENMUS_THEO_KEY_SIGN_CTROL_H__
