//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2013 LenMus project
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

#ifndef __LENMUS_TOOL_BOX_H__        //to avoid nested includes
#define __LENMUS_TOOL_BOX_H__

//lenmus
#include "lenmus_tool_box_theme.h"
#include "lenmus_tool_group.h"
#include "lenmus_injectors.h"

//wxWidgets
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
class wxBoxSizer;

//other
#include <vector>
using namespace std;


namespace lenmus
{

class CheckButton;
class ToolPageNotes;
class ToolPage;


//available tool pages
enum EToolPageID
{
	k_page_none = -1,
	k_page_clefs = 0,
	k_page_notes,
	k_page_barlines,
    k_page_symbols,
    //TO_ADD: Add, before this line, a new k_page_xxxxxx code for the new page
	k_page_max,     //this one MUST BE the last one
};

//---------------------------------------------------------------------------------------
// Group for mouse mode
//---------------------------------------------------------------------------------------

//values for mouse modes
#define lmMM_UNDEFINED      0x0000
#define lmMM_POINTER        0x0001
#define lmMM_DATA_ENTRY     0x0002
#define lmMM_ALL            (lmMM_POINTER | lmMM_DATA_ENTRY)


class GrpMouseMode : public ToolButtonsGroup
{
public:
    GrpMouseMode(wxPanel* pParent, wxBoxSizer* pMainSizer, ToolboxTheme* pColours);
    ~GrpMouseMode() {}

    //implement virtual methods
    void CreateGroupControls(wxBoxSizer* pMainSizer);
    inline EToolGroupID GetToolGroupID() { return k_grp_MouseMode; }

	//access to options
	int GetMouseMode();
	void SetMouseMode(int nMouseMode);

};


//---------------------------------------------------------------------------------------
// ToolBoxConfiguration: Helper class to define a ToolBox configuration
//---------------------------------------------------------------------------------------
class ToolBoxConfiguration
{
public:
    ToolBoxConfiguration() : m_fIsValid(false) {}
    ~ToolBoxConfiguration() {}

    inline bool IsOk() { return m_fIsValid; }


    wxPanel*        m_pSpecialGroup;        //panel for the special group
    EToolPageID     m_nCurPageID;           //selected page
    bool            m_fIsValid;             //this object has valid data
    bool            m_fSpecialGroupVisible;
    int             m_Pages[k_page_max];
};


//---------------------------------------------------------------------------------------
// ToolBox
//---------------------------------------------------------------------------------------

class ToolBox : public wxPanel
{
   DECLARE_DYNAMIC_CLASS(ToolBox)

public:
    ToolBox(wxWindow* parent, wxWindowID id, ApplicationScope& appScope);
    virtual ~ToolBox();

    //event handlers
    void OnButtonClicked(wxCommandEvent& event);
//    void OnKeyPress(wxKeyEvent& event);
    void OnResize(wxSizeEvent& event);
    void OnEraseBackground(wxEraseEvent& event);

    //configuration
    void GetConfiguration(ToolBoxConfiguration* pConfig);
    void SetConfiguration(ToolBoxConfiguration* pConfig);
    void SetDefaultConfiguration();

	//info
	int GetWidth() { return 150; }
	inline ToolboxTheme* GetColors() { return &m_colors; }

	//current tool and its options
	inline EToolPageID GetCurrentPageID() const { return m_nCurPageID; }
    EToolGroupID GetCurrentGroupID();
    EToolID GetCurrentToolID();
    wxString GetToolShortDescription();

    ToolPage* GetSelectedPage() { return (ToolPage*)m_pCurPage; }
	void SelectToolPage(EToolPageID nPageID);
	inline wxPanel* GetToolPanel(EToolPageID nPanel) { return (wxPanel*)m_cPages[nPanel]; }

	ToolPageNotes* GetNoteProperties() const;
    //TO_ADD: Add, before this line, a new method to get new tool properties

    //Contextual menus
    wxMenu* GetContextualMenuForSelectedPage();
    void OnPopUpMenuEvent(wxCommandEvent& event);

    //operations: adding/removing pages
    void AddPage(ToolPage* pPage, int nToolId);
    void SetAsActive(ToolPage* pPage, int nToolId);
    bool process_key(wxKeyEvent& event);

    //Special tools fixed group maganement
    void AddSpecialTools(wxPanel* pPanel, wxEvtHandler* pHandler);

    //interface with mouse mode group
	int GetMouseMode();
    inline void SetMouseMode(int nMouseMode) { m_pMouseModeGroup->SetMouseMode(nMouseMode); }


private:
	void CreateControls();
	void SelectButton(int nTool);
    ToolPage* CreatePage(EToolPageID nPanel);

	enum {
		NUM_BUTTONS = 16,
	};

    ApplicationScope& m_appScope;

    //controls
    GrpMouseMode*   m_pMouseModeGroup;      //mouse mode group
    wxPanel*        m_pSpecialGroup;        //current special group
    wxPanel*		m_pEmptyPage;           //an empty page
    wxPanel*		m_pCurPage;             //currently displayed page
    wxBoxSizer*     m_pPageSizer;           //the sizer for the pages
    EToolPageID     m_nCurPageID;           //currently displayed page ID
	CheckButton*	m_pButton[NUM_BUTTONS];

	//panels for tools' options
	std::vector<ToolPage*>    m_cPages;		                // pages collection
	int                         m_cActivePages[k_page_max];		// active page for each PageID

	//panels for the special group
	std::vector<wxPanel*>       m_cSpecialGroups;

	ToolboxTheme	m_colors;               //colors to use in this toolbox


    DECLARE_EVENT_TABLE()
};



}   // namespace lenmus

#endif    // __LENMUS_TOOL_BOX_H__
