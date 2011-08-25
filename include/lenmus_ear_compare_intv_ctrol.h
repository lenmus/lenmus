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

//#ifndef __LENMUS_EARCOMPAREINTVCTROL_H__        //to avoid nested includes
//#define __LENMUS_EARCOMPAREINTVCTROL_H__
//
//#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
//#pragma interface "EarCompareIntvCtrol.cpp"
//#endif
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
//#include "EarIntvalConstrains.h"
//#include "../score/Score.h"
//#include "ExerciseCtrol.h"


namespace lenmus
{

//class EarCompareIntvCtrol : public CompareScoresCtrol
//{
//   DECLARE_DYNAMIC_CLASS(EarCompareIntvCtrol)
//
//public:
//
//    // constructor and destructor
//    EarCompareIntvCtrol(wxWindow* parent, wxWindowID id,
//               EarIntervalsConstrains* pConstrains,
//               const wxPoint& pos = wxDefaultPosition,
//               const wxSize& size = wxDefaultSize, int style = 0);
//
//    ~EarCompareIntvCtrol();
//
//    //implementation of virtual pure in parent EBookCtrol
//    virtual void get_ctrol_options_from_params();
//
//    // implementation of virtual methods
//    wxString set_new_problem();
//    wxDialog* get_settings_dialog();
//    void prepare_aux_score(int nButton) {}
//
//
//private:
//
//        // member variables
//
//    EarIntervalsConstrains* m_pConstrains;    //use same constraints than for intervals
//    MidiPitch        m_ntMidi[2];            //the midi pitch of the two notes
//    MidiPitch        m_ntPitch[2];           //the pitch of the two notes
//
//};


}   // namespace lenmus

//#endif  // __LENMUS_EARCOMPAREINTVCTROL_H__
