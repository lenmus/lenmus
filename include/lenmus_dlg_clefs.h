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

#ifndef __LENMUS_DLG_CLEFS_H__
#define __LENMUS_DLG_CLEFS_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_injectors.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/dialog.h>

namespace lenmus
{


//---------------------------------------------------------------------------------------
class DlgClefs : public wxDialog
{
protected:
    ApplicationScope& m_appScope;

    enum { k_num_clefs=8, };
    wxButton*       m_clefs[k_num_clefs+1];
    wxStaticText*   m_letter[k_num_clefs+1];
    int m_index;

public:
    DlgClefs(ApplicationScope& appScope, wxWindow* parent);

    inline int get_selected_button() { return m_index; }

protected:
    void create_dialog();
    void initialize_strings();
    wxBitmap create_bitmap(int i);

    void on_button(wxCommandEvent& event);
    void on_key_down(wxKeyEvent& event);

    wxDECLARE_EVENT_TABLE();
};


}   //namespace lenmus

#endif // __LENMUS_DLG_CLEFS_H__
