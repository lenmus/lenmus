//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2013 LenMus project
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

#ifndef __LENMUS_COMMAND_WINDOW_H__        //to avoid nested includes
#define __LENMUS_COMMAND_WINDOW_H__

//lenmus
#include "lenmus_standard_header.h"

//wxWidgets
#include <wx/panel.h>

//forward declarations
class wxTextCtrl;
class wxWindow;

namespace lenmus
{

//---------------------------------------------------------------------------------------
class CommandWindow : public wxPanel
{
protected:
    wxTextCtrl* m_textctrl;

public:
    CommandWindow(wxWindow* parent);

    void on_enter(wxCommandEvent& WXUNUSED(event));
    wxString get_value() const;

private:

    DECLARE_EVENT_TABLE()
};


}   // namespace lenmus

#endif    // __LENMUS_COMMAND_WINDOW_H__
