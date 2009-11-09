//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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

#ifndef __LM_TOOLGROUP_H__
#define __LM_TOOLGROUP_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ToolGroup.cpp"
#endif

#include <vector>


//some helper definitions de define the behaviour of lmToolButtonsGroup
#define lmTBG_ALLOW_NONE    true
#define lmTBG_ONE_SELECTED  false


class lmToolPage;
class lmCheckButton;
class lmColorScheme;


enum lmEToolGroupID
{
    lmGRP_Undefined = 0,        //initial status

    //on ToolBox main panel
    lmGRP_MouseMode,

	//in lmPAGE_NOTES
    lmGRP_Octave,
    lmGRP_Voice,
    lmGRP_NoteRest,             //note or rest selection
    lmGRP_NoteDuration,         //notes duration group
    lmGRP_NoteAcc,              //Note accidentals group
    lmGRP_NoteDots,             //Note dots group
    lmGRP_TieTuplet,            //Ties and tuplets group
    lmGRP_Beams,                //tools for beams

	//in lmPAGE_CLEFS
    lmGRP_ClefType,
    lmGRP_TimeType,
    lmGRP_KeyType,

	//in lmPAGE_BARLINES,
	lmGRP_BarlineType,			//barline type

    //in lmPAGE_SYMBOLS
    lmGRP_Symbols,              //texts, figured bass, symbols & graphic objects

};

// Only needed for groups needing an ID for each tool
enum lmEToolID
{
    lmTOOL_NONE = 0,        //to signal not to use a ToolID

    //lmGRP_TieTuplet
    lmTOOL_NOTE_TIE,
    lmTOOL_NOTE_TUPLET,

    //lmGRP_Beams
    lmTOOL_BEAMS_CUT,
    lmTOOL_BEAMS_JOIN,
    lmTOOL_BEAMS_FLATTEN,
    lmTOOL_BEAMS_SUBGROUP,

    //lmGRP_Symbols
    lmTOOL_FIGURED_BASS,
    lmTOOL_TEXT,
    lmTOOL_LINES,
    lmTOOL_TEXTBOX,

};

//Group type
enum lmEGroupType
{
    lm_eGT_ToolSelector = 0,    //tool-selector group
    lm_eGT_Options,             //options group
};

class lmToolGroup : public wxPanel
{    
public:
    lmToolGroup(wxPanel* pParent, lmEGroupType nGroupType,
                lmColorScheme* pColours, int nValidMouseModes = 0xFFFF);
    virtual ~lmToolGroup();

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

    //identification
    inline bool IsToolSelectorGroup() { return m_nGroupType == lm_eGT_ToolSelector; }
    inline bool IsOptionsGroup() { return m_nGroupType == lm_eGT_Options; }
    virtual lmEToolGroupID GetToolGroupID()=0;
    virtual lmEToolID GetCurrentToolID()=0;

	//info
	int GetGroupWitdh();

protected:
    void PostToolBoxEvent(lmEToolID nToolID, bool fSelected);
    void DoRender(wxDC& dc);
    void DoPaintNow();

    friend class lmToolPage;
    virtual void SetSelected(bool fSelected);


	wxPanel*        m_pParent;      //owner ToolPage
    wxPanel*        m_pBoxPanel;
    wxPanel*        m_pGroupPanel; 
    wxBoxSizer*     m_pCtrolsSizer;
    wxBoxSizer*     m_pBoxSizer;
    wxBoxSizer*     m_pGroupSizer;
    lmColorScheme*  m_pColours;
    bool            m_fMousePressedDown;
    int             m_nValidMouseModes;  //to enable this group in valid modes
    lmEGroupType    m_nGroupType;       //tool-selector or options
    bool            m_fSelected;        //(only tool-selector) selected / deselected state when enabled
    bool            m_fSaveSelected;    //(only tool-selector) to save/restore state when disabled/enabled
    bool            m_fGuiControl;      //group not used in toolbox, but as a GUI control
    wxString        m_sTitle;           //group title

    DECLARE_EVENT_TABLE()
};
 

class lmToolButtonsGroup: public lmToolGroup
{    
public:
    lmToolButtonsGroup(wxPanel* pParent, lmEGroupType nGroupType, int nNumButtons,
                       bool fAllowNone, wxBoxSizer* pMainSizer, int nFirstButtonEventID,
                       lmEToolID nFirstButtonToolID,
                       lmColorScheme* pColours = (lmColorScheme*)NULL,
                       int nValidMouseModes = 0xFFFF);
    ~lmToolButtonsGroup();

    //event handlers
    void OnButton(wxCommandEvent& event);

    //implement virtual methods
    virtual lmEToolID GetCurrentToolID() { return GetSelectedToolID(); }

	//buttons
    inline int GetSelectedButton() { return m_nSelButton; }
    lmEToolID GetSelectedToolID() { return (lmEToolID)(m_nSelButton + m_nFirstButtonToolID); }


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
    std::vector<lmCheckButton*> m_pButton;      //buttons
};

#endif   // __LM_TOOLGROUP_H__
