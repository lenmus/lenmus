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

#ifndef __LENMUS_EDIT_INTERFACE_H__
#define __LENMUS_EDIT_INTERFACE_H__

//lenmus
#include "lenmus_standard_header.h"

//wxWidgets
#include <wx/event.h>

//lomse
using namespace lomse;


namespace lenmus
{

class ToolsInfo;

//---------------------------------------------------------------------------------------
// commands associated to key strokes
enum EKeyCommands
{
    k_cmd_null=0,

    //cursor
    k_cmd_cursor_move_prev,
    k_cmd_cursor_move_next,
    k_cmd_cursor_enter,
    k_cmd_cursor_exit,
    k_cmd_cursor_move_up,
    k_cmd_cursor_move_down,
    k_cmd_cursor_to_start_of_system,
    k_cmd_cursor_to_end_of_system,
    k_cmd_cursor_to_next_page,
    k_cmd_cursor_to_prev_page,
    k_cmd_cursor_to_next_measure,
    k_cmd_cursor_to_prev_measure,
    k_cmd_cursor_to_first_staff,
    k_cmd_cursor_to_last_staff,
    k_cmd_cursor_to_first_measure,
    k_cmd_cursor_to_last_measure,

    //delete
    k_cmd_delete_selection_or_pointed_object,
    k_cmd_move_prev_and_delete_pointed_object,

    //zoom
    k_cmd_zoom_in,
    k_cmd_zoom_out,

    //page Clefs
    k_cmd_clef_ask,

    //page Notes
    k_cmd_note_step_a,
    k_cmd_note_step_b,
    k_cmd_note_step_c,
    k_cmd_note_step_d,
    k_cmd_note_step_e,
    k_cmd_note_step_f,
    k_cmd_note_step_g,

    //increment/decrement/change octave

    //insert object
    k_cmd_note,
    k_cmd_rest,
    k_cmd_clef,
    k_cmd_barline,
};

//---------------------------------------------------------------------------------------
// EditInterface: mandatory interface for any GUI with tools for edition
class EditInterface
{
protected:

public:
    EditInterface() {}
    virtual ~EditInterface() {}

    //configuration modes for th GUI
    enum
    {
        k_full_edition=0,
        k_score_edition,
        k_rhythmic_dictation,
        k_harmony_exercise,
    };

    virtual bool process_key_in_toolbox(wxKeyEvent& event, ToolsInfo* pToolsInfo)=0;
    virtual int translate_key(int key, unsigned keyFlags)=0;

};


}   //namespace lenmus

#endif      // __LENMUS_EDIT_INTERFACE_H__
