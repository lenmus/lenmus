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

#ifndef __LENMUS_EAR_INTERVALS_CTROL_H__        //to avoid nested includes
#define __LENMUS_EAR_INTERVALS_CTROL_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_exercise_ctrol.h"
#include "lenmus_injectors.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>

//lomse
#include <lomse_pitch.h>
using namespace lomse;


namespace lenmus
{

//forward declarations
class EarIntervalsConstrains;
class DocumentWindow;
class Interval;


//---------------------------------------------------------------------------------------
class EarIntervalsCtrol : public OneScoreCtrol
{
public:
    EarIntervalsCtrol(long dynId, ApplicationScope& appScope, DocumentWindow* pCanvas);
    ~EarIntervalsCtrol();

    //implementation of virtual pure in parent EBookCtrol
    virtual void get_ctrol_options_from_params();

    enum {
        k_num_cols = 5,
        k_num_rows = 5,
        k_num_buttons = 25,     // NUM_COLS * NUM_ROWS;
    };

protected:
    //implementation of virtual methods
    void initialize_strings();
    void initialize_ctrol();
    void create_answer_buttons(LUnits height, LUnits spacing);
    ImoScore* prepare_aux_score(int nButton);
    wxString set_new_problem();
    wxDialog* get_settings_dialog();
    void on_settings_changed();
    void set_problem_space();

private:
    ImoScore* prepare_score(FPitch note1, FPitch note2);


        // member variables

    EarIntervalsConstrains* m_pConstrains;
    ButtonCtrl* m_pAnswerButton[k_num_buttons];
    int         m_nValidIntervals;              // num of enabled buttons
    int         m_nRealIntval[k_num_buttons];   // intval. associated to each valid button

    //problem asked
    bool        m_fAscending;
    bool        m_fHarmonic;
    EKeySignature m_nKey;
    FPitch      m_pitch[2];
};


}   // namespace lenmus

#endif  // __LENMUS_EAR_INTERVALS_CTROL_H__
