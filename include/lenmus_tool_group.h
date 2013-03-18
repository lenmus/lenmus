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

#ifndef __LENMUS_TOOL_GROUP_H__
#define __LENMUS_TOOL_GROUP_H__

//wxWidgtes
#include <wx/panel.h>
#include <wx/sizer.h>

//some helper definitions de define the behaviour of ToolButtonsGroup
#define lmTBG_ALLOW_NONE    true
#define lmTBG_ONE_SELECTED  false


//other
#include <vector>
using namespace std;


namespace lenmus
{

class ToolPage;
class CheckButton;
class ToolboxTheme;


enum EToolGroupID
{
    k_grp_Undefined = 0,        //initial status

    //on ToolBox main panel
    k_grp_MouseMode,

	//in k_page_notes
    k_grp_Octave,
    k_grp_Voice,
    k_grp_NoteRest,             //note or rest selection
    k_grp_NoteDuration,         //notes duration group
    k_grp_NoteAcc,              //Note accidentals group
    k_grp_NoteDots,             //Note dots group
    k_grp_NoteModifiers,            //Ties and tuplets group
    k_grp_Beams,                //tools for beams

	//in k_page_clefs
    k_grp_ClefType,
    k_grp_TimeType,
    k_grp_KeyType,

	//in k_page_barlines,
	k_grp_BarlineType,			//barline type

    //in k_page_symbols
    k_grp_Symbols,              //texts, symbols & graphic objects
    k_grp_Harmony,              //figured bass & harmony symbols

};

// Only needed for groups needing an ID for each tool
enum EToolID
{
    k_tool_none = 0,        //to signal not to use a ToolID

    //k_grp_NoteModifiers
    k_tool_note_tie,
    k_tool_note_tuplet,
    k_tool_note_toggle_stem,

    //k_grp_Beams
    k_tool_beams_cut,
    k_tool_beams_join,
    k_tool_beams_flatten,
    k_tool_beams_subgroup,

    //k_grp_Symbols
    k_tool_text,
    k_tool_lines,
    k_tool_textbox,

    //k_grp_Harmony
    k_tool_figured_bass,
    k_tool_fb_line

};

//Group type
enum EGroupType
{
    k_group_type_tool_selector = 0,    //tool-selector group
    k_group_type_options,             //options group
};

//---------------------------------------------------------------------------------------
class ToolGroup : public wxPanel
{
public:
    ToolGroup(wxPanel* pParent, EGroupType nGroupType,
                ToolboxTheme* pColours, int nValidMouseModes = 0xFFFF);
    virtual ~ToolGroup();

    //creation
    wxBoxSizer* CreateGroupSizer(wxBoxSizer* pParentSizer);
    virtual void CreateGroupControls(wxBoxSizer* pMainSizer)=0;
    inline void SetGroupTitle(const wxString& sTitle) { m_sTitle = sTitle; }

    //event handlers
    void OnPaintEvent(wxPaintEvent & event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseReleased(wxMouseEvent& event);
    void OnMouseLeftWindow(wxMouseEvent& event);
    //void OnMouseWheelMoved(wxMouseEvent& event);
    //void OnMouseMoved(wxMouseEvent& event);
    //void OnRightClick(wxMouseEvent& event);
    //void OnKeyPressed(wxKeyEvent& event);
    //void OnKeyReleased(wxKeyEvent& event);

    //state
    void EnableGroup(bool fEnable);
    void EnableForMouseMode(int nMode);
    inline void SetAsAlwaysDisabled() { m_fAlwaysDisabled = true; }

    //identification
    inline bool IsToolSelectorGroup() { return m_nGroupType == k_group_type_tool_selector; }
    inline bool IsOptionsGroup() { return m_nGroupType == k_group_type_options; }
    virtual EToolGroupID GetToolGroupID()=0;
    virtual EToolID GetCurrentToolID()=0;

	//info
	int GetGroupWitdh();

protected:
    void PostToolBoxEvent(EToolID nToolID, bool fSelected);
    void DoRender(wxDC& dc);
    void DoPaintNow();

    friend class ToolPage;
    virtual void SetSelected(bool fSelected);


	wxPanel*        m_pParent;      //owner ToolPage
    wxPanel*        m_pBoxPanel;
    wxPanel*        m_pGroupPanel;
    wxBoxSizer*     m_pCtrolsSizer;
    wxBoxSizer*     m_pBoxSizer;
    wxBoxSizer*     m_pGroupSizer;
    ToolboxTheme*    m_pColours;
    bool            m_fMousePressedDown;
    int             m_nValidMouseModes;  //to enable this group in valid modes
    EGroupType      m_nGroupType;       //tool-selector or options
    bool            m_fSelected;        //(only tool-selector) selected / deselected state when enabled
    bool            m_fSaveSelected;    //(only tool-selector) to save/restore state when disabled/enabled
    bool            m_fGuiControl;      //group not used in toolbox, but as a GUI control
    bool            m_fAlwaysDisabled;  //for groups under development
    wxString        m_sTitle;           //group title

    DECLARE_EVENT_TABLE()
};


//---------------------------------------------------------------------------------------
class ToolButtonsGroup: public ToolGroup
{
public:
    ToolButtonsGroup(wxPanel* pParent, EGroupType nGroupType, int nNumButtons,
                       bool fAllowNone, wxBoxSizer* pMainSizer, int nFirstButtonEventID,
                       EToolID nFirstButtonToolID,
                       ToolboxTheme* pColours = (ToolboxTheme*)NULL,
                       int nValidMouseModes = 0xFFFF);
    ~ToolButtonsGroup();

    //event handlers
    void OnButton(wxCommandEvent& event);

    //implement virtual methods
    virtual EToolID GetCurrentToolID() { return GetSelectedToolID(); }

	//buttons
    inline int GetSelectedButton() { return m_nSelButton; }
    EToolID GetSelectedToolID() { return (EToolID)(m_nSelButton + m_nFirstButtonToolID); }


	void SelectButton(int iB);
    void SelectNextButton();
    void SelectPrevButton();

protected:
    virtual void CreateGroupControls(wxBoxSizer* pMainSizer)=0;
    virtual void OnButtonSelected(int nSelButton);
    void ConnectButtonEvents();
    inline int GetFirstButtonEventID() { return m_nFirstButtonEventID; }
    inline bool IsNoneAllowed() { return m_fAllowNone; }
    virtual void SetSelected(bool fSelected);


    bool            m_fAllowNone;           //allow no button selected
    int             m_nNumButtons;          //number of buttons in this group
	int             m_nSelButton;           //selected button (0..n). -1 = none selected
    int             m_nSelButtonSave;       //to save selected button when group is inactive
    int             m_nFirstButtonEventID;      //even ID of first button
    int             m_nFirstButtonToolID;       //Tool ID of first button
    std::vector<CheckButton*> m_pButton;      //buttons
};



}   // namespace lenmus

#endif   // __LENMUS_TOOL_GROUP_H__
