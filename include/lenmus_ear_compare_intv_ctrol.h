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

#ifndef __LENMUS_EAR_COMPARE_INTV_CTROL_H__        //to avoid nested includes
#define __LENMUS_EAR_COMPARE_INTV_CTROL_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_exercise_ctrol.h"

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

class EarCompareIntvCtrol : public CompareScoresCtrol
{
public:
    EarCompareIntvCtrol(long dynId, ApplicationScope& appScope, DocumentWindow* pCanvas);
    ~EarCompareIntvCtrol();

    //implementation of virtual pure in parent EBookCtrol
    void get_ctrol_options_from_params();
    void initialize_ctrol();
    void on_settings_changed();
    void set_problem_space();

    // implementation of virtual methods
    wxString set_new_problem();
    wxDialog* get_settings_dialog();
    ImoScore* prepare_aux_score(int WXUNUSED(nButton)) { return nullptr; }


private:
    EarIntervalsConstrains* m_pConstrains;    //use same constraints than for intervals

};


}   // namespace lenmus

#endif  // __LENMUS_EAR_COMPARE_INTV_CTROL_H__
