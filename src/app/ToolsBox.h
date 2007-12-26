//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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

#ifndef __LM_TOOLSBOX_H__        //to avoid nested includes
#define __LM_TOOLSBOX_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ToolsBox.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

//available tools
enum lmEEditTool 
{
	lmTOOL_NONE = -1,
	lmTOOL_NOTES = 0,
	lmTOOL_CLEFS,
	lmTOOL_BARLINES,
	lmTOOL_MAX		//this MUST BE the last one
};

class lmCheckButton;

class lmToolBox: public wxPanel
{
   DECLARE_DYNAMIC_CLASS(lmToolBox)

public:
    lmToolBox(wxWindow* parent, wxWindowID id = wxID_ANY);
    virtual ~lmToolBox() {}

    void OnButtonClicked(wxCommandEvent& event);

	//current tool and its options
	inline lmEEditTool GetSelectedTool() const { return m_nSelTool; }
	void SelectTool(lmEEditTool nTool);

private:
	void SelectButton(int nTool);

	enum {
		NUM_BUTTONS = 16,
	};

    wxPanel*		m_pOptionsPanel;
	lmEEditTool		m_nSelTool;
	lmCheckButton*	m_pButton[NUM_BUTTONS];

    DECLARE_EVENT_TABLE()
};

#endif    // __LM_TOOLSBOX_H__
