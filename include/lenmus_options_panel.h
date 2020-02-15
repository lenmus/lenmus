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
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/panel.h>

class wxWindow;
class wxStaticBoxSizer;

namespace lenmus
{


//---------------------------------------------------------------------------------------
/** Abstract class from which all options dialogs must derive
*/
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
        , topSizer(nullptr)
        , outSizer(nullptr)

    {
    }

    virtual ~OptionsPanel() {}
    virtual bool Verify() = 0;
    virtual void Apply() = 0;

};

//---------------------------------------------------------------------------------------
/** Abstract class from which all panels in a notebook in an OptionsPanel must derive
*/
class OptionsTab : public wxPanel
{
protected:
    ApplicationScope& m_appScope;
    bool    m_fSettingsChanged;         //settings changed but not yet applied

public:
    OptionsTab(ApplicationScope& appScope, wxWindow* parent)
        : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL)
        , m_appScope(appScope)
        , m_fSettingsChanged(false)
    {
    }
    virtual ~OptionsTab() {}

    void on_change_settings(wxCommandEvent& WXUNUSED(event))
    {
        change_settings();
        m_fSettingsChanged = true;
    }
    virtual void initialize_controls() = 0;
    virtual void change_settings() = 0;
    virtual void restore_old_settings() = 0;
    virtual void apply_settings()
    {
        change_settings();
        save_current_settings();
        m_fSettingsChanged = false;
    }
    virtual void cancel_settings()
    {
        if (m_fSettingsChanged)
            restore_old_settings();
        m_fSettingsChanged = false;
    }
    virtual void save_current_settings() = 0;

};


}   // namespace lenmus

#endif        // __LENMUS_OPTIONS_PANEL_H__
