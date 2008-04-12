//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#include <vector>

//available tools
enum lmEEditTool 
{
	lmTOOL_NONE = -1,
	lmTOOL_SELECTION = 0,
	lmTOOL_CLEFS,
	lmTOOL_KEY_SIGN,
	lmTOOL_TIME_SIGN,
	lmTOOL_NOTES,
	lmTOOL_BARLINES,
    //TO_ADD: Add, before this line, a new lmTOOL_XXXXX code for the new tool
	lmTOOL_MAX		//this MUST BE the last one
};

class lmCheckButton;
class lmToolNotes;

class lmToolBox: public wxPanel
{
   DECLARE_DYNAMIC_CLASS(lmToolBox)

public:
    lmToolBox(wxWindow* parent, wxWindowID id = wxID_ANY);
    virtual ~lmToolBox();

    void OnButtonClicked(wxCommandEvent& event);
    void OnKeyPress(wxKeyEvent& event);

	//info
	int GetWidth() { return 150; }

	//current tool and its options
	inline lmEEditTool GetSelectedTool() const { return m_nSelTool; }
	void SelectTool(lmEEditTool nTool);
	inline wxPanel* GetToolPanel(lmEEditTool nPanel) { return m_cPanels[nPanel]; }


	inline lmToolNotes* GetNoteProperties() const { return (lmToolNotes*)m_cPanels[lmTOOL_NOTES]; }
    //TO_ADD: Add, before this line, a new method to get new tool properties

private:
	void CreateControls();
	void SelectButton(int nTool);
    wxPanel* CreatePanel(lmEEditTool nPanel);

	enum {
		NUM_BUTTONS = 16,
	};

    wxPanel*		m_pOptionsPanel;
	lmEEditTool		m_nSelTool;				//current selected tool
	lmCheckButton*	m_pButton[NUM_BUTTONS];

	//panels for tools' options
	std::vector<wxPanel*>	m_cPanels;		// tools' panels collection


    DECLARE_EVENT_TABLE()
};

#endif    // __LM_TOOLSBOX_H__
