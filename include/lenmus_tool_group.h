//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2015 LenMus project
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

//lomse
#include <lomse_interactor.h>
using namespace lomse;

//wxWidgtes
#define system ::system         //bypass for bug in wxcrtbase.h: "reference to 'system' is ambiguous"
#include <wx/panel.h>
#include <wx/sizer.h>
#undef system                   //bypass for bug in wxcrtbase.h: "reference to 'system' is ambiguous"

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
class ToolsInfo;

//---------------------------------------------------------------------------------------
enum EToolGroupID
{
    k_grp_Undefined = 0,        //initial status

    //on ToolBox main panel
    k_grp_MouseMode,

    //k_page_top_level
    k_grp_top_level,

	//k_page_notes
    k_grp_Octave,
    k_grp_Voice,
    k_grp_NoteDuration,         //notes duration group
    k_grp_NoteAcc,              //Note accidentals group
    k_grp_NoteDots,             //Note dots group
    k_grp_NoteModifiers,            //Ties and tuplets group
    k_grp_Beams,                //tools for beams

	//k_page_clefs
    k_grp_ClefType,
    k_grp_TimeType,
    k_grp_KeyType,

	//k_page_barlines,
	k_grp_BarlineType,			//barline type

    //k_page_symbols
    k_grp_Symbols,              //texts, symbols & graphic objects
    k_grp_Harmony,              //figured bass & harmony symbols

    //k_page_rhythmic_dictation
    k_grp_rhythmic_dictation_tools,
    k_grp_rhythmic_dictation_opts,
};

//---------------------------------------------------------------------------------------
//tool ids
enum EToolID
{
    k_tool_none = 0,

    //cursor
    k_tool_cursor_move_prev,
    k_tool_cursor_move_next,
    k_tool_cursor_enter,
    k_tool_cursor_exit,
    k_tool_cursor_move_up,
    k_tool_cursor_move_down,
    k_tool_cursor_to_start_of_system,
    k_tool_cursor_to_end_of_system,
    k_tool_cursor_to_next_page,
    k_tool_cursor_to_prev_page,
    k_tool_cursor_to_next_measure,
    k_tool_cursor_to_prev_measure,
    k_tool_cursor_to_first_staff,
    k_tool_cursor_to_last_staff,
    k_tool_cursor_to_first_measure,
    k_tool_cursor_to_last_measure,

    //delete
    k_tool_delete_selection_or_pointed_object,
    k_tool_move_prev_and_delete_pointed_object,

    //zoom
    k_tool_zoom_in,
    k_tool_zoom_out,

    //notes / rests
    k_tool_note_step_a,
    k_tool_note_step_b,
    k_tool_note_step_c,
    k_tool_note_step_d,
    k_tool_note_step_e,
    k_tool_note_step_f,
    k_tool_note_step_g,
    k_tool_note,        //as previous, but not specifying step

    //insert rest
    k_tool_rest,

    //note modifiers
    k_tool_note_tie,
    k_tool_note_tuplet,
    k_tool_note_toggle_stem,

    //beams
    k_tool_beams_cut,
    k_tool_beams_join,
    k_tool_beams_flatten,
    k_tool_beams_subgroup,

    //symbols
    k_tool_text,
    k_tool_lines,
    k_tool_textbox,

    //harmony
    k_tool_figured_bass,
    k_tool_fb_line,

    //tool selector
    k_tool_mouse_mode,
    k_tool_time_signature,
    k_tool_note_duration,
    k_tool_note_or_rest,
    k_tool_octave,
    k_tool_voice,
    k_tool_accidentals,
    k_tool_dots,
    k_tool_clef,
    k_tool_barline,
    k_tool_key_signature,

};

//Group type
enum EGroupType
{
    k_group_type_tool_selector = 0,     //tool-selector group
    k_group_type_options,               //options group
};

//---------------------------------------------------------------------------------------
class ToolGroup : public wxPanel
{
public:
    ToolGroup(wxPanel* pParent, EGroupType nGroupType,
                ToolboxTheme* pColours, int nValidMouseModes = 0xFFFF);
    virtual ~ToolGroup();

    //creation
    virtual void create_controls_in_group(wxBoxSizer* pMainSizer)=0;
    inline void set_group_title(const wxString& sTitle) { m_sTitle = sTitle; }

    //event handlers
    void OnPaintEvent(wxPaintEvent & event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseReleased(wxMouseEvent& event);
    void OnMouseLeftWindow(wxMouseEvent& event);

    //state
    void EnableGroup(bool fEnable);
    void EnableForMouseMode(int nMode);

    //identification
    inline bool is_tool_selector_group() { return m_nGroupType == k_group_type_tool_selector; }
    virtual EToolGroupID get_group_id()=0;
    virtual EToolID get_selected_tool_id()=0;

    //operations
    virtual void update_tools_info(ToolsInfo* pInfo)=0;
    virtual void synchronize_with_cursor(bool fEnable, DocCursor* pCursor)=0;
    virtual void synchronize_with_selection(bool fEnable, SelectionSet* pSelection)=0;

	//info
	int GetGroupWitdh();

protected:
    wxBoxSizer* create_main_sizer_for_group(wxBoxSizer* pParentSizer);
    void PostToolBoxEvent(EToolID nToolID, bool fSelected);
    void DoRender(wxDC& dc);
    void DoPaintNow();
    virtual bool process_key(wxKeyEvent& event);
    virtual bool process_command(int cmd);

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
    wxString        m_sTitle;           //group title

    wxDECLARE_EVENT_TABLE();
};


//---------------------------------------------------------------------------------------
class ToolButtonsGroup: public ToolGroup
{
public:
    ToolButtonsGroup(wxPanel* pParent, EGroupType nGroupType, int nNumButtons,
                       bool fAllowNone, wxBoxSizer* pMainSizer, int nFirstButtonEventID,
                       EToolID nFirstButtonToolID,
                       ToolboxTheme* pColours = (ToolboxTheme*)nullptr,
                       int nValidMouseModes = 0xFFFF);
    ~ToolButtonsGroup();

    //event handlers
    void OnButton(wxCommandEvent& event);

    //implement virtual methods
    virtual EToolID get_selected_tool_id() { return GetSelectedToolID(); }

	//buttons
    inline int GetSelectedButton() { return m_nSelButton; }
    EToolID GetSelectedToolID() { return (EToolID)(m_nSelButton + m_nFirstButtonToolID); }


	void SelectButton(int iB);
    void SelectNextButton();
    void SelectPrevButton();

protected:
    virtual void create_controls_in_group(wxBoxSizer* pMainSizer)=0;
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
