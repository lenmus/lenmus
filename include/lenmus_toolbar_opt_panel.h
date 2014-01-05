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

#ifndef __LENMUS_TOOLBARS_OPT_PANEL_H__
#define __LENMUS_TOOLBARS_OPT_PANEL_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_options_panel.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>

// Control identifiers
#define ID_WIZARD 10000
#define SYMBOL_MIDIWIZARD_IDNAME ID_WIZARD
#define ID_WIZARD_DEVICES_PAGE 10001
#define ID_COMBO_OUT_DEVICES 10004
#define ID_COMBO_IN_DEVICES 10005
#define ID_WIZARD_INSTRUMENTS_PAGE 10002
#define ID_COMBO_CHANNEL 10008
#define ID_COMBO_SECTION 10006
#define ID_COMBO_INSTRUMENT 10007
#define ID_BUTTON_TEST_SOUND 10009
#define ID_WIZARD_METRONOME_PAGE 10003
#define ID_COMBO_MTR_CHANNEL 10010
#define ID_COMBO_MTR_INSTR1 10011
#define ID_COMBO_MTR_INSTR2 10012
#define ID_BUTTON 10013



namespace lenmus
{

//---------------------------------------------------------------------------------------
class ToolbarsOptPanel : public OptionsPanel
{
public:
    ToolbarsOptPanel(wxWindow* parent, ApplicationScope& appScope);
    ~ToolbarsOptPanel();
    bool Verify();
    void Apply();

private:
    void create_controls();

	//controls
    wxPanel* m_pMainPanel;
    wxStaticText* m_pLblTitle;
    wxStaticBitmap* m_pBmpIconTitle;
    wxRadioBox* m_pOptIconSize;
    wxRadioBox* m_pOptLabels;

    // oher member variables
    int     m_nSizeIndex;
    int     m_nLabelsIndex;
};


}   // namespace lenmus

#endif    // __LENMUS_TOOLBARS_OPT_PANEL_H__
