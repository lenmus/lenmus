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
//class lmGroupBox;


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

class lmToolGroup : public wxPanel
{    
public:
    lmToolGroup(wxPanel* pParent, lmColorScheme* pColours);
    virtual ~lmToolGroup();

    //creation
    wxBoxSizer* CreateGroupSizer(wxBoxSizer* pParentSizer);
    virtual void CreateGroupControls(wxBoxSizer* pMainSizer)=0;
    inline void SetGroupTitle(const wxString& sTitle) { m_sText = sTitle; }

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

    //status
    void EnableGroup(bool fEnable);
    //virtual void EnableTool(lmEToolID nToolID, bool fEnable)=0;
    void SetSelected(bool fSelected);

    //identification
    virtual lmEToolGroupID GetToolGroupID()=0;
    //virtual int GetNumTools();
    //virtual lmEToolID GetToolID(int nTool);

	//info
	int GetGroupWitdh();

protected:
    void PostToolBoxEvent(lmEToolID nToolID, bool fSelected);
    void DoRender(wxDC& dc);
    void DoPaintNow();

	wxPanel*		    m_pParent;      //owner ToolPage
    wxPanel*            m_pBoxPanel;
    wxPanel*            m_pGroupPanel; 
    wxBoxSizer*         m_pCtrolsSizer;
    wxBoxSizer*         m_pBoxSizer;
    wxBoxSizer*         m_pGroupSizer;
    lmColorScheme*      m_pColours;
    bool                m_fMousePressedDown;
    bool                m_fSelected;    //this group is the selected one
    wxString            m_sText;        //group title

    DECLARE_EVENT_TABLE()
};
 

class lmToolButtonsGroup: public lmToolGroup
{    
public:
    lmToolButtonsGroup(wxPanel* pParent, int nNumButtons, bool fAllowNone,
                       wxBoxSizer* pMainSizer, int nFirstButtonEventID,
                       lmEToolID nFirstButtonToolID,
                       lmColorScheme* pColours = (lmColorScheme*)NULL);
    ~lmToolButtonsGroup();

    //event handlers
    void OnButton(wxCommandEvent& event);

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


    bool            m_fAllowNone;           //allow no button selected
    int             m_nNumButtons;          //number of buttons in this group
	int             m_nSelButton;           //selected button (0..n). -1 = none selected
    int             m_nFirstButtonEventID;      //even ID of first button
    int             m_nFirstButtonToolID;       //Tool ID of first button
    std::vector<lmCheckButton*> m_pButton;      //buttons
};

#endif   // __LM_TOOLGROUP_H__
