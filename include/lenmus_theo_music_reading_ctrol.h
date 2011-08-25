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

#ifndef __LENMUS_THOEMUSICREADINGCTROL_H__        //to avoid nested includes
#define __LENMUS_THOEMUSICREADINGCTROL_H__

//lenmus
#include "lenmus_exercise_ctrol.h"
//// for (compilers that support precompilation, includes <wx/wx.h>.
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
//#include "MusicReadingConstrains.h"
//#include "ExerciseCtrol.h"


namespace lenmus
{

//// TheoMusicReadingCtrol is an OneScoreCtrol but the controls are created
//// by this class by overriding virtual method CreateControls()
//
//class TheoMusicReadingCtrol : public OneScoreCtrol
//{
//   DECLARE_DYNAMIC_CLASS(TheoMusicReadingCtrol)
//
//public:
//
//    // constructor and destructor
//    TheoMusicReadingCtrol(long dynId, ApplicationScope& appScope, DocumentCanvas* pCanvas);
//
//    ~TheoMusicReadingCtrol();
//
//    //implementation of virtual pure in parent EBookCtrol
//    virtual void get_ctrol_options_from_params();
//
//    //implementation of virtual methods
//    void initialize_strings() {}
//    void initialize_ctrol();
//    void create_answer_buttons(LUnits height, LUnits spacing) {}
//    void prepare_aux_score(int nButton) {}
//    wxString set_new_problem();
//    wxDialog* get_settings_dialog();
//
//    //overrides of virtual methods
//    void CreateControls();
//    void Play();
//
//
//private:
//
//        // member variables
//
//    ImoScoreConstrains*          m_pScoreConstrains;
//    MusicReadingConstrains*   m_pConstrains;
//
//    //layout
//    wxBoxSizer*     m_pButtonsSizer;
//
//
//};


}   // namespace lenmus

#endif  // __LENMUS_THOEMUSICREADINGCTROL_H__
