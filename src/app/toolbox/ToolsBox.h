//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#include "ColorScheme.h"


//available tool pages
enum lmEToolPage 
{
	lmPAGE_NONE = -1,
	lmPAGE_CLEFS =0,
	lmPAGE_NOTES,
	lmPAGE_BARLINES,
    //TO_ADD: Add, before this line, a new lmPAGE_XXXXX code for the new tool
	lmPAGE_MAX,		//this MUST BE the last one
    //not used yet
	lmPAGE_SELECTION,
	lmPAGE_KEY_SIGN,
	lmPAGE_TIME_SIGN,
};

enum lmEToolGroupID
{
	//lmPAGE_NOTES
    lmGRP_Octave,
    lmGRP_Voice,
    lmGRP_NoteDuration,         //notes duration group
    lmGRP_NoteAcc,              //Note accidentals group
    lmGRP_NoteDots,             //Note dots group
    lmGRP_TieTuplet,            //Ties and tuplets group
    lmGRP_Beams,                //tools for beams

	//lmPAGE_CLEFS
    lmGRP_ClefType,
    lmGRP_TimeType,
    lmGRP_KeyType,

	//lmPAGE_BARLINES,
	lmGRP_BarlineType,			//barline type

};

// Only needed for groups needing an ID for each tool
enum lmEToolID
{
    //lmGRP_TieTuplet
    lmTOOL_NOTE_TIE,
    lmTOOL_NOTE_TUPLET,

    //lmGRP_Beams
    lmTOOL_BEAMS_CUT,
    lmTOOL_BEAMS_JOIN,
    lmTOOL_BEAMS_FLATTEN,
    lmTOOL_BEAMS_SUBGROUP,

};

class lmCheckButton;
class lmToolPageNotes;
class wxBoxSizer;

class lmToolBox: public wxPanel
{
   DECLARE_DYNAMIC_CLASS(lmToolBox)

public:
    lmToolBox(wxWindow* parent, wxWindowID id = wxID_ANY);
    virtual ~lmToolBox();

    void OnButtonClicked(wxCommandEvent& event);
    void OnKeyPress(wxKeyEvent& event);
    void OnResize(wxSizeEvent& event);

	//info
	int GetWidth() { return 150; }
	inline lmColorScheme* GetColors() { return &m_colors; }

	//current tool and its options
	inline lmEToolPage GetSelectedToolPage() const { return m_nSelTool; }
	void SelectToolPage(lmEToolPage nTool);
	inline wxPanel* GetToolPanel(lmEToolPage nPanel) { return m_cPanels[nPanel]; }


	inline lmToolPageNotes* GetNoteProperties() const { return (lmToolPageNotes*)m_cPanels[lmPAGE_NOTES]; }
    //TO_ADD: Add, before this line, a new method to get new tool properties

private:
	void CreateControls();
	void SelectButton(int nTool);
    wxPanel* CreatePanel(lmEToolPage nPanel);

	enum {
		NUM_BUTTONS = 16,
	};

    wxPanel*		m_pEmptyPage;           //an empty page
    wxPanel*		m_pCurPage;             //currently displayed page
    wxBoxSizer*     m_pPageSizer;           //the sizer for the pages
    lmEToolPage		m_nSelTool;				//current selected tool
	lmCheckButton*	m_pButton[NUM_BUTTONS];

	//panels for tools' options
	std::vector<wxPanel*>	m_cPanels;		// tools' panels collection

	lmColorScheme	m_colors;               //colors to use in this toolbox


    DECLARE_EVENT_TABLE()
};

#endif    // __LM_TOOLSBOX_H__
