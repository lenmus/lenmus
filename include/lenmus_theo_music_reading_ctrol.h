//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2020 LenMus project
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

#ifndef __LENMUS_THEO_MUSIC_READING_CTROL_H__        //to avoid nested includes
#define __LENMUS_THEO_MUSIC_READING_CTROL_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_exercise_ctrol.h"

//lomse
#include <lomse_checkbox_ctrl.h>
using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>


namespace lenmus
{

//forward declarations
class ScoreConstrains;
class MusicReadingConstrains;

//---------------------------------------------------------------------------------------
class TheoMusicReadingCtrol : public OneScoreCtrol
{
protected:
    ScoreConstrains*        m_pScoreConstrains;
    MusicReadingConstrains* m_pConstrains;

    CheckboxCtrl*       m_pChkCountOff;
    CheckboxCtrl*       m_pChkMetronome;

public:
    TheoMusicReadingCtrol(long dynId, ApplicationScope& appScope, DocumentWindow* pCanvas);
    ~TheoMusicReadingCtrol();

    //implementation of virtual pure in parent EBookCtrol
    void get_ctrol_options_from_params() override;
    void on_settings_changed() {}
    void set_problem_space() override;

    //implementation of virtual methods
    void initialize_strings() {}
    void initialize_ctrol() override;
    void create_answer_buttons(LUnits WXUNUSED(height), LUnits WXUNUSED(spacing)) override {}
    ImoScore* prepare_aux_score(int WXUNUSED(nButton)) { return nullptr; }
    wxString set_new_problem() override;
    wxDialog* get_settings_dialog();

    //overrides of virtual methods
    void create_controls() override;

    //overrides to change exercises default behaviour
    bool is_play_button_initially_enabled() override { return true; }
    bool is_play_again_message_allowed() { return false; }

    //overrides of PlayerNoGui for using check boxes
    bool metronome_status();
    bool countoff_status();
};


}   // namespace lenmus

#endif  // __LENMUS_THEO_MUSIC_READING_CTROL_H__
