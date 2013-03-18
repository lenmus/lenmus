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

    //implement virtual methods
    void CreateGroupControls(wxBoxSizer* pMainSizer);
    inline EToolGroupID GetToolGroupID() { return k_grp_Octave; }

	//access to options
	inline int GetOctave() { return m_nSelButton; }
	inline void SetOctave(int nOctave) { SelectButton(nOctave); }
    void SetOctave(bool fUp);

};


//---------------------------------------------------------------------------------------
// Group for voice number: base class
//---------------------------------------------------------------------------------------
class GrpVoice : public ToolButtonsGroup
{
public:
    ~GrpVoice() {}

    //implement virtual methods
    //void CreateGroupControls(wxBoxSizer* pMainSizer) = 0;
    inline EToolGroupID GetToolGroupID() { return k_grp_Voice; }

	//access to options
	virtual int GetVoice() { return m_nSelButton; }
	virtual void SetVoice(int nVoice) { SelectButton(nVoice); }
    void SetVoice(bool fUp);

protected:
    GrpVoice(ToolPage* pParent, wxBoxSizer* pMainSizer, int nNumButtons);

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
    void CreateGroupControls(wxBoxSizer* pMainSizer);
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
    void CreateGroupControls(wxBoxSizer* pMainSizer);

    //overrides, to avoide voice 0 (AutoVoice)
    int GetVoice() { return m_nSelButton + 1; }
	void SetVoice(int nVoice) { SelectButton(nVoice - 1); }

};



//---------------------------------------------------------------------------------------
// Group for Note duration
//---------------------------------------------------------------------------------------
class GrpNoteDuration : public ToolButtonsGroup
{
public:
    GrpNoteDuration(ToolPage* pParent, wxBoxSizer* pMainSizer);
    ~GrpNoteDuration() {}

    //implement virtual methods
    void CreateGroupControls(wxBoxSizer* pMainSizer);
    inline EToolGroupID GetToolGroupID() { return k_grp_NoteDuration; }

	//access to options
	ENoteType GetNoteDuration();

    //modify buttons
    void SetButtonsBitmaps(bool fNotes);

};



//---------------------------------------------------------------------------------------
// Group to select notes or rests
//---------------------------------------------------------------------------------------
class GrpNoteRest : public ToolButtonsGroup
{
public:
    GrpNoteRest(ToolPage* pParent, wxBoxSizer* pMainSizer);
    ~GrpNoteRest() {}

    //implement virtual methods
    void CreateGroupControls(wxBoxSizer* pMainSizer);
    inline EToolGroupID GetToolGroupID() { return k_grp_NoteRest; }

	//access to options
	bool IsNoteSelected();
    inline bool IsRestSelected() { return !IsNoteSelected(); }
};


//---------------------------------------------------------------------------------------
// Group for Note accidentals
//---------------------------------------------------------------------------------------
class GrpNoteAcc : public ToolButtonsGroup
{
public:
    GrpNoteAcc(ToolPage* pParent, wxBoxSizer* pMainSizer);
    ~GrpNoteAcc() {}

    //implement virtual methods
    void CreateGroupControls(wxBoxSizer* pMainSizer);
    inline EToolGroupID GetToolGroupID() { return k_grp_NoteAcc; }

	//access to options
	EAccidentals GetNoteAcc();

};


//---------------------------------------------------------------------------------------
// Group for note dots
//---------------------------------------------------------------------------------------
class GrpNoteDots : public ToolButtonsGroup
{
public:
    GrpNoteDots(ToolPage* pParent, wxBoxSizer* pMainSizer);
    ~GrpNoteDots() {}

    //implement virtual methods
    void CreateGroupControls(wxBoxSizer* pMainSizer);
    inline EToolGroupID GetToolGroupID() { return k_grp_NoteDots; }

	//access to options
	int GetNoteDots();

};


//---------------------------------------------------------------------------------------
// Group for tuplets, ties, ...
//---------------------------------------------------------------------------------------
class GrpNoteModifiers : public ToolGroup
{
public:
    GrpNoteModifiers(ToolPage* pParent, wxBoxSizer* pMainSizer);
    ~GrpNoteModifiers() {}

    //implement virtual methods
    void CreateGroupControls(wxBoxSizer* pMainSizer);
    inline EToolGroupID GetToolGroupID() { return k_grp_NoteModifiers; }
    inline EToolID GetCurrentToolID() { return m_nSelectedToolID; }

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

    DECLARE_EVENT_TABLE()
};


//---------------------------------------------------------------------------------------
// Group for beams
//---------------------------------------------------------------------------------------
class GrpBeams : public ToolGroup
{
public:
    GrpBeams(ToolPage* pParent, wxBoxSizer* pMainSizer);
    ~GrpBeams() {}

    //implement virtual methods
    void CreateGroupControls(wxBoxSizer* pMainSizer);
    inline EToolGroupID GetToolGroupID() { return k_grp_Beams; }
    inline EToolID GetCurrentToolID() { return m_nSelectedToolID; }

    //event handlers
    void OnButton(wxCommandEvent& event);

    void EnableTool(EToolID nToolID, bool fEnabled);


protected:
    //void PostToolBoxEvent(EToolID nToolID, bool fSelected);

    BitmapButton*     m_pBtnBeamCut;
    BitmapButton*     m_pBtnBeamJoin;
    BitmapButton*     m_pBtnBeamFlatten;
    BitmapButton*     m_pBtnBeamSubgroup;
    EToolID           m_nSelectedToolID;      //clicked tool

    DECLARE_EVENT_TABLE()
};



//---------------------------------------------------------------------------------------
// ToolPageNotes: Abstract class to build specific Note tools pages
//---------------------------------------------------------------------------------------
class ToolPageNotes : public ToolPage
{
	DECLARE_ABSTRACT_CLASS(ToolPageNotes)

public:
    virtual ~ToolPageNotes();

    //creation
    virtual void CreateGroups() = 0;
    virtual void Create(wxWindow* parent);

	//access to options
    wxString GetToolShortDescription();

    //interface with Octave group
	inline int GetOctave() { return m_pGrpOctave->GetOctave(); }
    inline void SetOctave(bool fUp) { m_pGrpOctave->SetOctave(fUp); }
    inline void SetOctave(int nOctave) { m_pGrpOctave->SetOctave(nOctave); }

    //interface with voice group
	inline int GetVoice() { return m_pGrpVoice->GetVoice(); }
    inline void SetVoice(bool fUp) { m_pGrpVoice->SetVoice(fUp); }
    inline void SetVoice(int nVoice) { m_pGrpVoice->SetVoice(nVoice); }

    //interface with Note/Rest group
	inline bool IsNoteSelected() { return m_pGrpNoteRest->IsNoteSelected(); }
    inline bool IsRestSelected() { return m_pGrpNoteRest->IsRestSelected(); }
    inline void select_notes() { m_pGrpNoteRest->SelectButton(0); }
    inline void select_rests() { m_pGrpNoteRest->SelectButton(1); }

    //interface with NoteDuration group
    inline void EnableGrpNoteDuration(bool fEnabled) { m_pGrpNoteDuration->EnableGroup(fEnabled); }
    inline void SetNoteDuration(ENoteType nNoteType) { m_pGrpNoteDuration->SelectButton((int)nNoteType - 1); }
    inline ENoteType GetNoteDuration() { return m_pGrpNoteDuration->GetNoteDuration(); }
    inline int GetNoteDurationButton() { return m_pGrpNoteDuration->GetSelectedButton(); }
    inline void SetNoteDurationButton(int iB) { m_pGrpNoteDuration->SelectButton(iB); }

    //interface with NoteAccidentals group
    inline void EnableGrpNoteAcc(bool fEnabled) { m_pGrpNoteAcc->EnableGroup(fEnabled); }
    inline void SetNoteAccidentals(EAccidentals nAcc) { m_pGrpNoteAcc->SelectButton((int)nAcc - 1); }
    inline EAccidentals GetNoteAccidentals() { return m_pGrpNoteAcc->GetNoteAcc(); }
    inline void SelectNextAccidental() { m_pGrpNoteAcc->SelectNextButton(); }
    inline void SelectPrevAccidental() { m_pGrpNoteAcc->SelectPrevButton(); }
    inline int GetNoteAccButton() { return m_pGrpNoteAcc->GetSelectedButton(); }
    inline void SetNoteAccButton(int iB) { m_pGrpNoteAcc->SelectButton(iB); }

    //interface with NoteDots group
    inline void EnableGrpNoteDots(bool fEnabled) { m_pGrpNoteDots->EnableGroup(fEnabled); }
    inline void SetNoteDots(int nDots) { m_pGrpNoteDots->SelectButton(nDots - 1); }
    inline int GetNoteDots() { return m_pGrpNoteDots->GetNoteDots(); }
    inline void SelectNextDot() { m_pGrpNoteDots->SelectNextButton(); }
    inline void SelectPrevDot() { m_pGrpNoteDots->SelectPrevButton(); }
    inline int GetNoteDotsButton() { return m_pGrpNoteDots->GetSelectedButton(); }
    inline void SetNoteDotsButton(int iB) { m_pGrpNoteDots->SelectButton(iB); }

    //interface with Modifiers group
    inline void EnabelGrpModifiers(bool fEnabled) { m_pGrpModifiers->EnableGroup(fEnabled); }
    inline void SetToolTie(bool fChecked) { m_pGrpModifiers->SetToolTie(fChecked); }
    inline void SetToolTuplet(bool fChecked) { m_pGrpModifiers->SetToolTuplet(fChecked); }
    inline void SetToolToggleStem(bool fChecked) { m_pGrpModifiers->SetToolToggleStem(fChecked); }

    //interface with Beam tools group
    //inline void EnabelGrpModifiers(bool fEnabled) { m_pGrpModifiers->EnableGroup(fEnabled); }
    //inline void SetToolTie(bool fChecked) { m_pGrpModifiers->SetToolTie(fChecked); }
    //inline void SetToolTuplet(bool fChecked) { m_pGrpModifiers->SetToolTuplet(fChecked); }


    //interface with NoteheadType group
	ENoteHeads GetNoteheadType();
    //inline ENoteHeads GetNoteheadType() { return m_pGrpNoteDuration->GetNoteDuration(); }
    //inline void SetNoteDurationButton(int iB) { m_pGrpNoteDuration->SelectButton(iB); }


protected:
    ToolPageNotes(wxWindow* parent);
    ToolPageNotes();

    //groups
    GrpNoteRest*      m_pGrpNoteRest;
    GrpNoteDuration*  m_pGrpNoteDuration;
    GrpNoteAcc*       m_pGrpNoteAcc;
    GrpNoteDots*      m_pGrpNoteDots;
    GrpNoteModifiers*     m_pGrpModifiers;
    GrpBeams*         m_pGrpBeams;
	GrpOctave*		m_pGrpOctave;
	GrpVoice*			m_pGrpVoice;
	//GrpMouseMode*     m_pGrpEntryMode;

	//options
	wxBitmapComboBox*	m_pCboNotehead;
	wxBitmapComboBox*	m_pCboAccidentals;
};



//---------------------------------------------------------------------------------------
// ToolPageNotesStd: Standard page for Notes tools
//---------------------------------------------------------------------------------------
class ToolPageNotesStd : public ToolPageNotes
{
	DECLARE_DYNAMIC_CLASS(ToolPageNotesStd)

public:
    ToolPageNotesStd(wxWindow* parent);
    ToolPageNotesStd();
    ~ToolPageNotesStd();

    //implementation of pure virtual base class methods
    void Create(wxWindow* parent);
    void CreateGroups();

    //overrides to ToolPage
    bool process_key(wxKeyEvent& event);


protected:

};



//--------------------------------------------------------------------------------
// ToolPageNotesHarmony: Notes tools page for harmony exercises
//--------------------------------------------------------------------------------

class ToolPageNotesHarmony : public ToolPageNotes
{
	DECLARE_DYNAMIC_CLASS(ToolPageNotesHarmony)

public:
    ToolPageNotesHarmony(wxWindow* parent);
    ToolPageNotesHarmony();                   //default, for dynamic creation
    ~ToolPageNotesHarmony();

    //implementation of pure virtual base class methods
    void CreateGroups();
    void Create(wxWindow* parent);             //for dynamic creation

    //overrides
    wxMenu* GetContextualMenuForToolPage();
    void OnPopUpMenuEvent(wxCommandEvent& event);

protected:
    wxMenu*         m_pMenu;        //contextual menu

};



}   // namespace lenmus

#endif    // __LENMUS_TOOL_NOTES_H__
