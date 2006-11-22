// RCS-ID: $Id: ToolbarsOptPanel.h,v 1.3 2006/02/23 19:22:19 cecilios Exp $
//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the 
//    terms of the GNU General Public License as published by the Free Software Foundation;
//    either version 2 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this 
//    program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, 
//    Fifth Floor, Boston, MA  02110-1301, USA.
//
//    For any comment, suggestion or feature request, please contact the manager of 
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------
/*! @file ToolbarsOptPanel.h
    @brief Header file for class lmToolbarsOptPanel
    @ingroup options_management
*/
#ifndef __TOOLBARSOPTPANEL_H__
#define __TOOLBARSOPTPANEL_H__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "lmToolbarsOptPanel.cpp"
#endif

#include "OptionsPanel.h"


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


class lmToolbarsOptPanel: public lmOptionsPanel
{    
    DECLARE_DYNAMIC_CLASS(lmToolbarsOptPanel)
    DECLARE_EVENT_TABLE()

public:
    lmToolbarsOptPanel() {}    //default constructor for dynamic creation
    lmToolbarsOptPanel(wxWindow* parent);
    ~lmToolbarsOptPanel();
    bool Verify();
    void Apply();

private:
    // member variables
    int     m_nSizeIndex;
    int     m_nLabelsIndex;
};

#endif    // __TOOLBARSOPTPANEL_H__