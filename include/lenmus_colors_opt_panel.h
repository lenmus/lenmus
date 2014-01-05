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

#ifndef __LENMUS_COLORS_OPT_PANEL_H__
#define __LENMUS_COLORS_OPT_PANEL_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_options_panel.h"


//---------------------------------------------------------------------------------------
// Control identifiers
#define SYMBOL_MIDIWIZARD_IDNAME ID_WIZARD
enum
{
    ID_WIZARD = 10000,
    ID_WIZARD_DEVICES_PAGE,
    ID_COMBO_OUT_DEVICES,
    ID_COMBO_IN_DEVICES,
    ID_WIZARD_INSTRUMENTS_PAGE,
    ID_COMBO_CHANNEL,
    ID_COMBO_SECTION,
    ID_COMBO_INSTRUMENT,
    ID_BUTTON_TEST_SOUND,
    ID_WIZARD_METRONOME_PAGE,
    ID_COMBO_MTR_CHANNEL,
    ID_COMBO_MTR_INSTR1,
    ID_COMBO_MTR_INSTR2,
    ID_BUTTON,
};

namespace lenmus
{

//---------------------------------------------------------------------------------------
class ColorsOptPanel : public OptionsPanel
{
public:
    ColorsOptPanel(wxWindow* parent, ApplicationScope& appScope);
    ~ColorsOptPanel();
    bool Verify();
    void Apply();

private:
    // member variables
    wxChoice*    m_pChoice;
};


}   // namespace lenmus

#endif    // __LENMUS_COLORS_OPT_PANEL_H__
