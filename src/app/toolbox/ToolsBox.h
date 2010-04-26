//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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

// For compilers that support precompilation, includes <wx/wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <vector>

#include "ColorScheme.h"
#include "ToolGroup.h"


//available tool pages
enum lmEToolPageID
{
	lmPAGE_NONE = -1,
	lmPAGE_CLEFS =0,
	lmPAGE_NOTES,
	lmPAGE_BARLINES,
    lmPAGE_SYMBOLS,
    //TO_ADD: Add, before this line, a new lmPAGE_XXXXX code for the new page
	lmPAGE_MAX,		//this one MUST BE the last one
};

//--------------------------------------------------------------------------------
// Group for mouse mode
//--------------------------------------------------------------------------------

//values for mouse modes
#define lmMM_UNDEFINED      0x0000
#define lmMM_POINTER        0x0001
#define lmMM_DATA_ENTRY     0x0002
#define lmMM_ALL            (lmMM_POINTER | lmMM_DATA_ENTRY)


class lmGrpMouseMode : public lmToolButtonsGroup
{
public:
    lmGrpMouseMode(wxPanel* pParent, wxBoxSizer* pMainSizer, lmColorScheme* pColours);
    ~lmGrpMouseMode() {}

    //implement virtual methods
    void CreateGroupControls(wxBoxSizer* pMainSizer);
    inline lmEToolGroupID GetToolGroupID() { return lmGRP_MouseMode; }

	//access to options
	int GetMouseMode();
	void SetMouseMode(int nMouseMode);

};


//-------------------------------------------------------------------------------------------------
// lmToolBoxConfiguration: Helper class to define a ToolBox configuration
//-------------------------------------------------------------------------------------------------

class lmToolBoxConfiguration
{
public:
    lmToolBoxConfiguration() : m_fIsValid(false) {}
    ~lmToolBoxConfiguration() {}

    inline bool IsOk() { return m_fIsValid; }


    wxPanel*        m_pSpecialGroup;        //panel for the special group
    lmEToolPageID   m_nCurPageID;           //selected page
    bool            m_fIsValid;             //this object has valid data
    bool            m_fSpecialGroupVisible;
    int             m_Pages[lmPAGE_MAX];
};


//-------------------------------------------------------------------------------------------------
// lmToolBox
//-------------------------------------------------------------------------------------------------

class lmCheckButton;
class lmToolPageNotes;
class wxBoxSizer;
class lmToolPage;

class lmToolBox: public wxPanel
{
   DECLARE_DYNAMIC_CLASS(lmToolBox)

public:
    lmToolBox(wxWindow* parent, wxWindowID id = wxID_ANY);
    virtual ~lmToolBox();

    //event handlers
    void OnButtonClicked(wxCommandEvent& event);
    void OnKeyPress(wxKeyEvent& event);
    void OnResize(wxSizeEvent& event);
    void OnEraseBackground(wxEraseEvent& event);

    //configuration
    void GetConfiguration(lmToolBoxConfiguration* pConfig);
    void SetConfiguration(lmToolBoxConfiguration* pConfig);
    void SetDefaultConfiguration();

	//info
	int GetWidth() { return 150; }
	inline lmColorScheme* GetColors() { return &m_colors; }

	//current tool and its options
	inline lmEToolPageID GetCurrentPageID() const { return m_nCurPageID; }
    lmEToolGroupID GetCurrentGroupID();
    lmEToolID GetCurrentToolID();
    wxString GetToolShortDescription();

    lmToolPage* GetSelectedPage() { return (lmToolPage*)m_pCurPage; }
	void SelectToolPage(lmEToolPageID nPageID);
	inline wxPanel* GetToolPanel(lmEToolPageID nPanel) { return (wxPanel*)m_cPages[nPanel]; }

	lmToolPageNotes* GetNoteProperties() const;
    //TO_ADD: Add, before this line, a new method to get new tool properties

    //Contextual menus
    wxMenu* GetContextualMenuForSelectedPage();
    void OnPopUpMenuEvent(wxCommandEvent& event);

    //operations: adding/removing pages
    void AddPage(lmToolPage* pPage, int nToolId);
    void SetAsActive(lmToolPage* pPage, int nToolId);

    //Special tools fixed group maganement
    void AddSpecialTools(wxPanel* pPanel, wxEvtHandler* pHandler);

    //interface with mouse mode group
	int GetMouseMode();
    inline void SetMouseMode(int nMouseMode) { m_pMouseModeGroup->SetMouseMode(nMouseMode); }


private:
	void CreateControls();
	void SelectButton(int nTool);
    lmToolPage* CreatePage(lmEToolPageID nPanel);

	enum {
		NUM_BUTTONS = 16,
	};

    //controls
    lmGrpMouseMode* m_pMouseModeGroup;      //mouse mode group
    wxPanel*        m_pSpecialGroup;        //current special group
    wxPanel*		m_pEmptyPage;           //an empty page
    wxPanel*		m_pCurPage;             //currently displayed page
    wxBoxSizer*     m_pPageSizer;           //the sizer for the pages
    lmEToolPageID   m_nCurPageID;           //currently displayed page ID
	lmCheckButton*	m_pButton[NUM_BUTTONS];

	//panels for tools' options
	std::vector<lmToolPage*>    m_cPages;		                // pages collection
	int                         m_cActivePages[lmPAGE_MAX];		// active page for each PageID

	//panels for the special group
	std::vector<wxPanel*>       m_cSpecialGroups;

	lmColorScheme	m_colors;               //colors to use in this toolbox


    DECLARE_EVENT_TABLE()
};

#endif    // __LM_TOOLSBOX_H__
