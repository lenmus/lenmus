//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2015 LenMus project
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
class DocumentWindow;

//---------------------------------------------------------------------------------------
// EditInterface: mandatory interface for any GUI with tools for edition
class EditInterface
{
protected:

public:
    EditInterface() {}
    virtual ~EditInterface() {}

    //configuration modes for the GUI
    enum
    {
        k_full_edition=0,
        k_score_edition,
        k_rhythmic_dictation,
        k_harmony_exercise,
    };

    virtual bool process_key_in_toolbox(wxKeyEvent& event, ToolsInfo* pToolsInfo)=0;
    virtual int translate_key(int key, int keyFlags)=0;
    virtual void set_edition_gui_mode(DocumentWindow* pWnd, int mode) = 0;

};


}   //namespace lenmus

#endif      // __LENMUS_EDIT_INTERFACE_H__
