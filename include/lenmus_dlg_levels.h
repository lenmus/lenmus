//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2010-2021 LenMus project
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

#ifndef __LENMUS_DLG_LEVELS_H__        //to avoid nested includes
#define __LENMUS_DLG_LEVELS_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_injectors.h"

//wxWidgets
#include <wx/dialog.h>
#include <wx/html/htmlwin.h>        //GCC complains if forward declaration of wxHtmlWindow !

//forward declarations
class wxButton;
class wxWindow;
class wxFileName;
class wxStaticText;

namespace lenmus
{


//---------------------------------------------------------------------------------------
class LevelsDlg : public wxDialog
{
protected:
    ApplicationScope& m_appScope;
    wxWindow* m_pParent;
    int m_level = 1;
    wxStaticText* m_pLabel;
    wxButton* m_pButtonL1;
    wxButton* m_pButtonL2;
    wxButton* m_pButtonCustom;

public:
	LevelsDlg(wxWindow* pParent, ApplicationScope& appScope);
	~LevelsDlg();

    void CreateControls();

    inline int get_level() { return m_level; };

    // event handlers
    void on_click_level1(wxCommandEvent& event);
    void on_click_level2(wxCommandEvent& event);
    void on_click_custom_level(wxCommandEvent& event);


protected:
};


}   // namespace lenmus

#endif    // __LENMUS_DLG_LEVELS_H__
