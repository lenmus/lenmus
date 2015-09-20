//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2010-2015 LenMus project
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

//#ifndef __LENMUS_THEOHARMONYCTROL_H__        //to avoid nested includes
//#define __LENMUS_THEOHARMONYCTROL_H__
//
//#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
//#pragma interface "TheoHarmonyCtrol.cpp"
//#endif
//
//// For compilers that support precompilation, includes "wx/wx.h".
//#include "wx/wxprec.h"
//
//#ifdef __BORLANDC__
//#pragma hdrstop
//#endif
//
//#ifndef WX_PRECOMP
//#include "wx/wx.h"
//#endif
//
//#include "lenmus_standard_header.h"
//#include "HarmonyConstrains.h"
//#include "../score/Score.h"
//#include "lenmus_exercise_ctrol.h"


namespace lenmus
{

//class TheoHarmonyCtrol : public lmFullEditorExercise
//{
//   wxDECLARE_DYNAMIC_CLASS(TheoHarmonyCtrol);
//
//public:
//
//    // constructor and destructor
//    TheoHarmonyCtrol(wxWindow* parent, wxWindowID id,
//               lmHarmonyConstrains* pConstrains, wxSize nDisplaySize,
//               const wxPoint& pos = wxDefaultPosition,
//               const wxSize& size = wxDefaultSize, int style = 0);
//
//    ~TheoHarmonyCtrol();
//
//    //implementation of virtual pure in parent EBookCtrol
//    virtual void get_ctrol_options_from_params();
//
//    //event handlers
//    void OnEndOfPlay(lmEndOfPlaybackEvent& WXUNUSED(event));
//
//    //implementation of virtual methods from base class
//    void set_new_problem();
//    wxDialog* get_settings_dialog();
//    void initialize_strings();
//    void on_settings_changed();
//
//
//private:
//
//
//        // member variables
//
//    lmHarmonyConstrains*    m_pConstrains;      //constraints for the exercise
//    EKeySignature        m_nKey;             //key to use for scores
//
//    wxDECLARE_EVENT_TABLE();
//};


}   // namespace lenmus

//#endif  // __LENMUS_THEOHARMONYCTROL_H__
