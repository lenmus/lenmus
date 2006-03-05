// RCS-ID: $Id: OptionsPanel.h,v 1.5 2006/02/23 19:22:19 cecilios Exp $
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
/*! @file OptionsPanel.h
    @brief Header file for abstract class lmOptionsPanel
    @ingroup options_management
*/
//---------------------------------------------------------------------------------
/*! @class lmOptionsPanel
    @ingroup options_management
    @brief Abstract class from which all options dialogs must derive

    Each panel in the options dialog (lmOptionsDlg) must derive from this abstract
    class lmOptionsPanel. You must override Verify() with code
    to validate fields (returning true if any are bad), and Apply() with code for
    updating the     global preferences object g_pPrefs, and instructing the applicable parts
    of the program to re-read the preference options.

    See text in lmOptionsDlg.cpp for instructions to add a new options panel

-----------------------------------------------------------------------------------*/

#ifndef __OPTIONSPANEL_H__
#define __OPTIONSPANEL_H__

#include <wx/panel.h>

class wxWindow;
class wxStaticBoxSizer;

// A few constants for an attempt at semi-uniformity 
#define PREFS_FONT_SIZE     8

// these are spacing guidelines: ie. radio buttons should have a 5 pixel
// border on each side
#define RADIO_BUTTON_BORDER    5
#define TOP_LEVEL_BORDER       5
#define GENERIC_CONTROL_BORDER 5

class lmOptionsPanel : public wxPanel
{
public:
    lmOptionsPanel() {}    //default constructor for dynamic creation of derived classes
    lmOptionsPanel(wxWindow* parent)
        : wxPanel(parent, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL )
    {
      /* I'm not sure if we should be setting this... I'll play around
       * and see what looks best on different platforms under
       * differing circumstances...*/
//    SetFont(wxFont(PREFS_FONT_SIZE, wxDEFAULT, wxNORMAL, wxNORMAL));
    }
   
    virtual ~lmOptionsPanel() {}
    virtual bool Verify() = 0;
    virtual void Apply() = 0;

protected:
    wxBoxSizer* topSizer;
    wxBoxSizer* outSizer;

};

#endif        // __OPTIONSPANEL_H__
