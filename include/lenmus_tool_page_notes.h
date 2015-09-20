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

#ifndef __LENMUS_TOOL_NOTES_H__
#define __LENMUS_TOOL_NOTES_H__

//lenmus
#include "lenmus_tool_group.h"
#include "lenmus_tool_page.h"

//lomse
#include "lomse_internal_model.h"   //ENoteType
#include "lomse_im_note.h"          //ENoteHead
#include "lomse_pitch.h"            //EAccidentals

using namespace lomse;

//wxWidgets
class wxBitmapComboBox;


using namespace std;


namespace lenmus
{

class CheckButton;
class BitmapButton;

//---------------------------------------------------------------------------------------
// Group for octave number
//---------------------------------------------------------------------------------------
class GrpOctave : public ToolButtonsGroup
{
public:
    GrpOctave(ToolPage* pParent, wxBoxSizer* pMainSizer);
    ~GrpOctave() {}

    //mandatory overrides
    void update_tools_info(ToolsInfo* pInfo);
    void create_controls_in_group(wxBoxSizer* pMainSizer);
    EToolGroupID get_group_id() { return k_grp_Octave; }
    void synchronize_with_cursor(bool fEnable, DocCursor* pCursor);
    void synchronize_with_selection(bool fEnable, SelectionSet* pSelection);

	//access to options
	inline int GetOctave() { return m_nSelButton; }
	inline void SetOctave(int nOctave) { SelectButton(nOctave); }
    void SetOctave(bool fUp);

protected:
    //overrides
    bool process_key(wxKeyEvent& event);

};


//---------------------------------------------------------------------------------------
// Group for voice number: base class
//---------------------------------------------------------------------------------------
class GrpVoice : public ToolButtonsGroup
{
public:
    ~GrpVoice() {}

    //mandatory overrides
    void update_tools_info(ToolsInfo* pInfo);
    EToolGroupID get_group_id() { return k_grp_Voice; }
    void synchronize_with_cursor(bool fEnable, DocCursor* pCursor);
    void synchronize_with_selection(bool fEnable, SelectionSet* pSelection);

	//access to options
    int GetVoice() { return m_nSelButton + 1; }
	void SetVoice(int nVoice) { SelectButton(nVoice - 1); }
    void SetVoice(bool fUp);

protected:
    GrpVoice(ToolPage* pParent, wxBoxSizer* pMainSizer, int nNumButtons);

    //overrides
    bool process_key(wxKeyEvent& event);

};

//---------------------------------------------------------------------------------------
// Group for voice number: standard group
//---------------------------------------------------------------------------------------
class GrpVoiceStd : public GrpVoice
{
public:
    GrpVoiceStd(ToolPage* pParent, wxBoxSizer* pMainSizer);
    ~GrpVoiceStd() {}

    //implement virtual methods
    void create_controls_in_group(wxBoxSizer* pMainSizer);
};

//---------------------------------------------------------------------------------------
// Group for voice number: for harmony exercises
//---------------------------------------------------------------------------------------
class GrpVoiceHarmony : public GrpVoice
{
public:
    GrpVoiceHarmony(ToolPage* pParent, wxBoxSizer* pMainSizer);
    ~GrpVoiceHarmony() {}

    //implement virtual methods
    void create_controls_in_group(wxBoxSizer* pMainSizer);

};



//---------------------------------------------------------------------------------------
// Group for Note duration
//---------------------------------------------------------------------------------------
class GrpNoteDuration : public ToolButtonsGroup
{
public:
    GrpNoteDuration(ToolPage* pParent, wxBoxSizer* pMainSizer);
    ~GrpNoteDuration() {}

    //mandatory overrides
    void update_tools_info(ToolsInfo* pInfo);
    void create_controls_in_group(wxBoxSizer* pMainSizer);
    EToolGroupID get_group_id() { return k_grp_NoteDuration; }
    void synchronize_with_cursor(bool fEnable, DocCursor* pCursor);
    void synchronize_with_selection(bool fEnable, SelectionSet* pSelection);

	//access to options
	ENoteType GetNoteDuration();

    //modify buttons
    void SetButtonsBitmaps(bool fNotes);

protected:
    //overrides
    bool process_command(int cmd);

};


//---------------------------------------------------------------------------------------
// Group for Note accidentals
//---------------------------------------------------------------------------------------
class GrpNoteAcc : public ToolButtonsGroup
{
public:
    GrpNoteAcc(ToolPage* pParent, wxBoxSizer* pMainSizer);
    ~GrpNoteAcc() {}

    //mandatory overrides
    void update_tools_info(ToolsInfo* pInfo);
    void create_controls_in_group(wxBoxSizer* pMainSizer);
    EToolGroupID get_group_id() { return k_grp_NoteAcc; }
    void synchronize_with_cursor(bool fEnable, DocCursor* pCursor);
    void synchronize_with_selection(bool fEnable, SelectionSet* pSelection);

	//access to options
	EAccidentals GetNoteAcc();

protected:
    //overrides
    bool process_command(int cmd);
};


//---------------------------------------------------------------------------------------
// Group for note dots
//---------------------------------------------------------------------------------------
class GrpNoteDots : public ToolButtonsGroup
{
public:
    GrpNoteDots(ToolPage* pParent, wxBoxSizer* pMainSizer);
    ~GrpNoteDots() {}

    //mandatory overrides
    void update_tools_info(ToolsInfo* pInfo);
    void create_controls_in_group(wxBoxSizer* pMainSizer);
    EToolGroupID get_group_id() { return k_grp_NoteDots; }
    void synchronize_with_cursor(bool fEnable, DocCursor* pCursor);
    void synchronize_with_selection(bool fEnable, SelectionSet* pSelection);

	//access to options
	int GetNoteDots();

protected:
	//overrides
    bool process_command(int cmd);

};


//---------------------------------------------------------------------------------------
// Group for tuplets, ties, ...
//---------------------------------------------------------------------------------------
class GrpNoteModifiers : public ToolGroup
{
public:
    GrpNoteModifiers(ToolPage* pParent, wxBoxSizer* pMainSizer);
    ~GrpNoteModifiers() {}

    //mandatory overrides
    void update_tools_info(ToolsInfo* pInfo);
    void create_controls_in_group(wxBoxSizer* pMainSizer);
    EToolGroupID get_group_id() { return k_grp_NoteModifiers; }
    EToolID get_selected_tool_id() { return m_nSelectedToolID; }
    void synchronize_with_cursor(bool fEnable, DocCursor* pCursor);
    void synchronize_with_selection(bool fEnable, SelectionSet* pSelection);

    //event handlers
    void OnTieButton(wxCommandEvent& event);
    void OnTupletButton(wxCommandEvent& event);
    void OnToggleStemButton(wxCommandEvent& event);

	//access to options
    void SetToolTie(bool fChecked);
    void SetToolTuplet(bool fChecked);
    void SetToolToggleStem(bool fChecked);
    void EnableTool(EToolID nToolID, bool fEnabled);


protected:
    CheckButton*      m_pBtnTie;
    CheckButton*      m_pBtnTuplet;
    CheckButton*      m_pBtnToggleStem;
    EToolID           m_nSelectedToolID;      //clicked tool

    wxDECLARE_EVENT_TABLE();
};


//---------------------------------------------------------------------------------------
// Group for beams
//---------------------------------------------------------------------------------------
class GrpBeams : public ToolGroup
{
public:
    GrpBeams(ToolPage* pParent, wxBoxSizer* pMainSizer);
    ~GrpBeams() {}

    //mandatory overrides
    void update_tools_info(ToolsInfo* pInfo);
    void create_controls_in_group(wxBoxSizer* pMainSizer);
    EToolGroupID get_group_id() { return k_grp_Beams; }
    EToolID get_selected_tool_id() { return m_nSelectedToolID; }
    void synchronize_with_cursor(bool fEnable, DocCursor* pCursor);
    void synchronize_with_selection(bool fEnable, SelectionSet* pSelection);

    //event handlers
    void OnButton(wxCommandEvent& event);

    void EnableTool(EToolID nToolID, bool fEnabled);


protected:
    //validations on cursor pointed objects
    bool is_valid_for_cut_beam(ImoStaffObj* pSO);

    BitmapButton*     m_pBtnBeamCut;
    BitmapButton*     m_pBtnBeamJoin;
    BitmapButton*     m_pBtnBeamFlatten;
    BitmapButton*     m_pBtnBeamSubgroup;
    EToolID           m_nSelectedToolID;      //clicked tool

    wxDECLARE_EVENT_TABLE();
};



//---------------------------------------------------------------------------------------
// ToolPageNotes: Abstract class to build specific Note tools pages
//---------------------------------------------------------------------------------------
class ToolPageNotes : public ToolPage
{
	wxDECLARE_ABSTRACT_CLASS(ToolPageNotes);

public:
    virtual ~ToolPageNotes() {}

    //creation
    virtual void create_tool_groups() = 0;
    virtual void Create(wxWindow* parent);

//	//access to options
//    wxString GetToolShortDescription();


protected:
    ToolPageNotes(wxWindow* parent);
    ToolPageNotes() {}

    //mandatory overrides
    long get_key_translation_context() { return k_key_context_notes; }

	//options
	wxBitmapComboBox*	m_pCboNotehead;
};



//---------------------------------------------------------------------------------------
// ToolPageNotesStd: Standard page for Notes tools
//---------------------------------------------------------------------------------------
class ToolPageNotesStd : public ToolPageNotes
{
	wxDECLARE_DYNAMIC_CLASS(ToolPageNotesStd);

public:
    ToolPageNotesStd(wxWindow* parent);
    ToolPageNotesStd();
    ~ToolPageNotesStd();

    //implementation of pure virtual base class methods
    void Create(wxWindow* parent);
    void create_tool_groups();

    //overrides to ToolPage
    bool process_key(wxKeyEvent& event);


protected:

};



//--------------------------------------------------------------------------------
// ToolPageNotesHarmony: Notes tools page for harmony exercises
//--------------------------------------------------------------------------------

class ToolPageNotesHarmony : public ToolPageNotes
{
	wxDECLARE_DYNAMIC_CLASS(ToolPageNotesHarmony);

public:
    ToolPageNotesHarmony(wxWindow* parent);
    ToolPageNotesHarmony();                   //default, for dynamic creation
    ~ToolPageNotesHarmony();

    //implementation of pure virtual base class methods
    void create_tool_groups();
    void Create(wxWindow* parent);             //for dynamic creation

    //overrides
    wxMenu* GetContextualMenuForToolPage();
//    void OnPopUpMenuEvent(wxCommandEvent& event);

protected:
    wxMenu*         m_pMenu;        //contextual menu

};



}   // namespace lenmus

#endif    // __LENMUS_TOOL_NOTES_H__
