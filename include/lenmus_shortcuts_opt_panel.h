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

#ifndef __LENMUS_SHORTCUTS_OPT_PANEL_H__
#define __LENMUS_SHORTCUTS_OPT_PANEL_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_options_panel.h"

//wxWidgets
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/grid.h>
#include <wx/button.h>


namespace lenmus
{

class HwxGrid;

//---------------------------------------------------------------------------------------
class ShortcutsOptPanel : public OptionsPanel
{
public:
    ShortcutsOptPanel(wxWindow* parent, ApplicationScope& appScope);
    ~ShortcutsOptPanel();
    bool Verify();
    void Apply();

private:

    //event handlers
    void on_button_reset_all(wxCommandEvent& event);
    void on_button_clear(wxCommandEvent& event);
    void on_button_define(wxCommandEvent& event);
    void on_category_selected(wxCommandEvent& event);


private:
    void create_controls();
    void load_categories();
    void load_current_settings();

    // controls
    wxPanel* m_pTitlePanel;
    wxStaticText* m_pLblTitle;

    wxStaticBitmap* m_pTitleIcon;
    wxStaticText* pCategoryLabel;
    wxChoice* m_pCategoryChoice;
    HwxGrid* m_pGrid;
    wxButton* m_pResetAll;
    wxButton* m_pClear;
    wxButton* m_pDefine;

    wxDECLARE_EVENT_TABLE();
};


}   // namespace lenmus

#endif    // __LENMUS_SHORTCUTS_OPT_PANEL_H__
