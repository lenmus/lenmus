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

#ifndef __LENMUS_IDFYTONALITYCTROL_H__        //to avoid nested includes
#define __LENMUS_IDFYTONALITYCTROL_H__

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
//#include "TonalityConstrains.h"
//#include "../score/Score.h"
//#include "ExerciseCtrol.h"


namespace lenmus
{

//class IdfyTonalityCtrol : public OneScoreCtrol
//{
//public:
//
//    // constructor and destructor
//    IdfyTonalityCtrol(long dynId, ApplicationScope& appScope, DocumentCanvas* pCanvas);
//
//    ~IdfyTonalityCtrol();
//
//    //implementation of virtual pure in parent EBookCtrol
//    virtual void get_ctrol_options_from_params();
//
//    //implementation of virtual methods
//    void initialize_strings();
//    void initialize_ctrol();
//    void create_answer_buttons(LUnits height, LUnits spacing);
//    void prepare_aux_score(int nButton);
//    wxString set_new_problem();
//    wxDialog* get_settings_dialog();
//    void on_settings_changed();
//    void EnableButtons(bool value);
//
//private:
//    wxString prepare_score(EClefExercise nClef, EKeySignature nType,
//                          ImoScore** pProblemScore,
//                          ImoScore** pSolutionScore = NULL );
//    void ComputeRightAnswerButtons();
//
//        // member variables
//
//    enum {
//        m_NUM_COLS = 5,
//        m_NUM_ROWS = 7,
//        m_NUM_BUTTONS = 35,     // <= NUM_COLS * NUM_ROWS;
//    };
//
//    TonalityConstrains* m_pConstrains;       //constraints for the exercise
//
//    //problem asked
//    EKeySignature  m_nKey;
//
//    //answer
//    ImoButton*       m_pAnswerButton[m_NUM_BUTTONS];     //buttons for the answers
//    ImoTextItem*    m_pRowLabel[m_NUM_ROWS];            //labels for rows
//    //key signature that corresponds to each button
//    EKeySignature  m_nRealKey[m_NUM_BUTTONS];
//
//    DECLARE_EVENT_TABLE()
//};


}   // namespace lenmus

#endif  // __LENMUS_IDFYTONALITYCTROL_H__
