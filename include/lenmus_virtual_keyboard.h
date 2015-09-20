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

#ifndef __LENMUS_VIRTUAL_KEYBOARD_H__        //to avoid nested includes
#define __LENMUS_VIRTUAL_KEYBOARD_H__

//lenmus
#include "lenmus_standard_header.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/panel.h>
class wxBitmapButton;
class wxButton;

namespace lenmus
{


//---------------------------------------------------------------------------------------
class VirtualKeyboard : public wxPanel
{
protected:
    enum { k_num_buttons = 10, };

    wxButton* m_keys[k_num_buttons];

public:
    VirtualKeyboard(wxWindow* parent, wxWindowID id, const wxPoint& pos=wxDefaultPosition,
                    const wxSize& size=wxDefaultSize, long style=0);
    ~VirtualKeyboard();


protected:
    void create_buttons();
    wxBitmap create_bitmap(int i);

    //event handlers
    void on_button(wxCommandEvent& event);
    void on_key_down(wxKeyEvent& event);

    wxDECLARE_EVENT_TABLE();
};


}   //namespace lenmus


#endif    // __LENMUS_VIRTUAL_KEYBOARD_H__

