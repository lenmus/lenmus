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

#ifndef __LENMUS_EARINTERVALSCTROL_H__        //to avoid nested includes
#define __LENMUS_EARINTERVALSCTROL_H__

//lenmus
#include "lenmus_exercise_ctrol.h"
#include "lenmus_injectors.h"
//#include "EarIntvalConstrains.h"
//#include "../score/Score.h"
//#include "../auxmusic/Interval.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>


namespace lenmus
{

//forward declarations
class EarIntervalsConstrains;
class DocumentCanvas;


//---------------------------------------------------------------------------------------
class EarIntervalsCtrol : public OneScoreCtrol
{
public:
    EarIntervalsCtrol(long dynId, ApplicationScope& appScope, DocumentCanvas* pCanvas);
    ~EarIntervalsCtrol() {}

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
    void prepare_aux_score(int nButton);
    wxString set_new_problem();
    wxDialog* get_settings_dialog();
    void on_settings_changed();
    void set_problem_space();

private:
    void prepare_score(wxString& sIntvCode, ImoScore** pScore);


        // member variables

    EarIntervalsConstrains* m_pConstrains;
    ImoButton*  m_pAnswerButton[k_num_buttons];
    int         m_nValidIntervals;              // num of enabled buttons
    int         m_nRealIntval[k_num_buttons];   // intval. associated to each valid button

    //problem asked
    wxString            m_sIntvCode;
    bool                m_fAscending;
    bool                m_fHarmonic;
    EKeySignature      m_nKey;
//TODO 5.0 commented out
//    NoteBits         m_tNote[2];
};


}   // namespace lenmus

#endif  // __LENMUS_EARINTERVALSCTROL_H__
