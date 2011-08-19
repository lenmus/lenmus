//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2011 LenMus project
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

//---------------------------------------------------------------------------------------
// OptionsPanel: Abstract class from which all options dialogs must derive
//
//    Each panel in the options dialog (OptionsDlg) must derive from this abstract
//    class OptionsPanel. You must override Verify() with code
//    to validate fields (returning true if any are bad), and Apply() with code for
//    updating the global preferences object, and instructing the applicable
//    parts of the program to re-read the preference options.
//
//    See text in lenmus_options_dlg.cpp for instructions to add a new options panel
//
//---------------------------------------------------------------------------------------

#ifndef __LENMUS_OPTIONS_PANEL_H__
#define __LENMUS_OPTIONS_PANEL_H__

#include "lenmus_injectors.h"

//wxWidgets
#include <wx/panel.h>

class wxWindow;
class wxStaticBoxSizer;

namespace lenmus
{


//---------------------------------------------------------------------------------------
// A few constants for an attempt at semi-uniformity
#define PREFS_FONT_SIZE     8

// these are spacing guidelines: ie. radio buttons should have a 5 pixel
// border on each side
#define RADIO_BUTTON_BORDER    5
#define TOP_LEVEL_BORDER       5
#define GENERIC_CONTROL_BORDER 5

//---------------------------------------------------------------------------------------
class OptionsPanel : public wxPanel
{
protected:
    ApplicationScope& m_appScope;
    wxBoxSizer* topSizer;
    wxBoxSizer* outSizer;

public:
    OptionsPanel(wxWindow* parent, ApplicationScope& appScope)
        : wxPanel(parent, -1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL )
        , m_appScope(appScope)
    {
    }

    virtual ~OptionsPanel() {}
    virtual bool Verify() = 0;
    virtual void Apply() = 0;

};


}   // namespace lenmus

#endif        // __LENMUS_OPTIONS_PANEL_H__
