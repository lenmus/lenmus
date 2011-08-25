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

#ifndef __LENMUS_IDFYSCALESCTROL_H__        //to avoid nested includes
#define __LENMUS_IDFYSCALESCTROL_H__

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
//#include "ScalesConstrains.h"
//#include "../score/Score.h"
//#include "ExerciseCtrol.h"


namespace lenmus
{

//class IdfyScalesCtrol : public OneScoreCtrol
//{
//public:
//
//    // constructor and destructor
//    IdfyScalesCtrol(long dynId, ApplicationScope& appScope, DocumentCanvas* pCanvas);
//
//    ~IdfyScalesCtrol();
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
//
//private:
//    wxString prepare_score(EClefExercise nClef, lmEScaleType nType, ImoScore** pScore);
//    int ReconfigureGroup(int iBt, int iStartC, int iEndC, wxString sRowLabel);
//    void DisableGregorianMajorMinor(lmEScaleType nType);
//
//        // member variables
//
//    enum {
//        m_NUM_COLS = 4,
//        m_NUM_ROWS = 6,
//        m_NUM_BUTTONS = 24,     // NUM_COLS * NUM_ROWS;
//    };
//
//    ScalesConstrains* m_pConstrains;       //constraints for the exercise
//
//    //problem asked
//    EKeySignature  m_nKey;
//    wxString        m_sRootNote;
//    bool            m_fAscending;
//
//    //answer
//    ImoTextItem*    m_pRowLabel[m_NUM_ROWS];
//    ImoButton*       m_pAnswerButton[m_NUM_BUTTONS];     //buttons for the answers
//    int             m_nRealScale[m_NUM_BUTTONS];        //scale that corresponds
//                                                        //   to each valid button
//
//    DECLARE_EVENT_TABLE()
//};


}   // namespace lenmus

#endif  // __LENMUS_IDFYSCALESCTROL_H__
