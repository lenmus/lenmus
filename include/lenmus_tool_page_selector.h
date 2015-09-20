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

#ifndef __LENMUS_TOOL_PAGE_SELECTOR_H__
#define __LENMUS_TOOL_PAGE_SELECTOR_H__

//lenmus
#include "lenmus_tool_box.h"

//wxWidgets
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/string.h>
    #include <wx/button.h>
    #include "wx/combo.h"
    #include "wx/odcombo.h"
#endif
class wxStaticText;
class wxMenu;
class wxButton;

namespace lenmus
{


//---------------------------------------------------------------------------------------
// PageSelector: a simulated tab book with a button that opens a popup menu
class PageSelector : public wxPanel
{
protected:
    wxMenu* m_pMenu;
    wxStaticText* m_pPageTitle;
    wxButton* m_pSelector;
    wxMenuItem* m_menuitems[k_page_max];

public:
    PageSelector();
    PageSelector(wxWindow *parent,
                    wxWindowID id = wxID_ANY,
                    const wxString& value = wxEmptyString,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = 0);
    virtual ~PageSelector();

    void add_page_selector(int idPage, wxWindowID idEvent, const wxString& name);
    void add_separator();
    void set_page_title(const wxString& title);
    void enable_page(int id, bool fEnable);
    void enable(bool fEnable);
    void selector_visible(bool fVisible);

    virtual void on_button_click(wxCommandEvent& event);
    virtual void on_paint_event(wxPaintEvent& event);

private:
    void initialize();

    wxDECLARE_EVENT_TABLE();

};


}   // namespace lenmus

#endif //__LENMUS_TOOL_PAGE_SELECTOR_H__

