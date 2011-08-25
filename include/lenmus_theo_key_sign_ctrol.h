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

#ifndef __LENMUS_THEOKEYSIGNCTROL_H__        //to avoid nested includes
#define __LENMUS_THEOKEYSIGNCTROL_H__

//lenmus
#include "lenmus_exercise_ctrol.h"
//
//// For compilers that support precompilation, includes <wx/wx.h>.
//#include <wx/wxprec.h>
//
//#ifdef __BORLANDC__
//#pragma hdrstop
//#endif
//
//#ifndef WX_PRECOMP
//#include <wx/wx.h>
//#endif
//
//#include "lenmus_constrains.h"
//#include "TheoKeySignConstrains.h"
//#include "../score/Score.h"
//#include "ExerciseCtrol.h"


namespace lenmus
{

//class TheoKeySignCtrol : public OneScoreCtrol
//{
//   DECLARE_DYNAMIC_CLASS(TheoKeySignCtrol)
//
//public:
//
//    // constructor and destructor
//    TheoKeySignCtrol(long dynId, ApplicationScope& appScope, DocumentCanvas* pCanvas);
//
//    ~TheoKeySignCtrol();
//
//    //implementation of virtual pure in parent EBookCtrol
//    virtual void get_ctrol_options_from_params();
//
//    //implementation of virtual methods
//    void initialize_strings();
//    void initialize_ctrol();
//    void create_answer_buttons(LUnits height, LUnits spacing);
//    void prepare_aux_score(int nButton) {}
//    wxString set_new_problem();
//    wxDialog* get_settings_dialog();
//
//private:
//
//    enum {
//        m_NUM_COLS = 5,
//        m_NUM_ROWS = 3,
//        m_NUM_BUTTONS = 15,     // NUM_COLS * NUM_ROWS;
//    };
//
//    // member variables
//
//    TheoKeySignConstrains* m_pConstrains;
//
//    //buttons for the answers: 3 rows, 5 buttons per row
//    ImoButton*       m_pAnswerButton[m_NUM_BUTTONS];
//    int             m_nIndexKeyName;        //index to right answer button
//    bool            m_fButtonsEnabled;      //buttons enabled
//
//    wxString        m_sAnswer;              //name of the interval
//    bool            m_fMajorMode;           //major mode
//    bool            m_fIdentifyKey;         //type of problem
//
//    DECLARE_EVENT_TABLE()
//};


}   // namespace lenmus

#endif  // __LENMUS_THEOKEYSIGNCTROL_H__
