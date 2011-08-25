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

#ifndef __LENMUS_IDFYCADENCECTROL_H__        //to avoid nested includes
#define __LENMUS_IDFYCADENCECTROL_H__

//lenmus
#include "lenmus_exercise_ctrol.h"
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
//#include "CadencesConstrains.h"
//#include "../score/Score.h"
//#include "ExerciseCtrol.h"


namespace lenmus
{

//class IdfyCadencesCtrol : public OneScoreCtrol
//{
//public:
//
//    // constructor and destructor
//    IdfyCadencesCtrol(long dynId, ApplicationScope& appScope, DocumentCanvas* pCanvas);
//
//    ~IdfyCadencesCtrol();
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
//
//private:
//    wxString prepare_score(EClefExercise nClef, lmECadenceType nType, ImoScore** pProblemScore,
//                          ImoScore** pSolutionScore = NULL );
//    int DisplayButton(int iBt, lmECadenceType iStartC, lmECadenceType iEndC, wxString sButtonLabel);
//
//        // member variables
//
//    enum {
//        m_NUM_COLS = 4,
//        m_NUM_ROWS = 2,
//        m_NUM_BUTTONS = 8,     // NUM_COLS * NUM_ROWS;
//    };
//
//    lmCadencesConstrains* m_pConstrains;       //constraints for the exercise
//
//    //problem asked
//    EKeySignature  m_nKey;
//
//    //answer
//    ImoButton*       m_pAnswerButton[m_NUM_BUTTONS];     //buttons for the answers
//    //cadence that corresponds to each valid button
//    lmECadenceType  m_nStartCadence[m_NUM_BUTTONS];
//    lmECadenceType  m_nEndCadence[m_NUM_BUTTONS];
//
//    DECLARE_EVENT_TABLE()
//};


}   // namespace lenmus

#endif  // __LENMUS_IDFYCADENCECTROL_H__
