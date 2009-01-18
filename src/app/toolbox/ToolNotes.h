//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 Cecilio Salmeron
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

#ifndef __LM_TOOLNOTES_H__
#define __LM_TOOLNOTES_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ToolNotes.cpp"
#endif

#include "ToolGroup.h"
#include "ToolPage.h"
#include "../../score/defs.h"

class wxBitmapComboBox;
class lmCheckButton;
class lmBitmapButton;

//--------------------------------------------------------------------------------
// Group for octave number
//--------------------------------------------------------------------------------
class lmGrpOctave: public lmToolButtonsGroup
{
public:
    lmGrpOctave(lmToolPage* pParent, wxBoxSizer* pMainSizer);
    ~lmGrpOctave() {}

    //implement virtual methods
    void CreateControls(wxBoxSizer* pMainSizer);
    inline lmEToolGroupID GetToolGroupID() { return lmGRP_Octave; }

	//access to options
	inline int GetOctave() { return m_nSelButton; }
	inline void SetOctave(int nOctave) { SelectButton(nOctave); }
    void SetOctave(bool fUp);

};


//--------------------------------------------------------------------------------
// Group for voice number
//--------------------------------------------------------------------------------
class lmGrpVoice: public lmToolButtonsGroup
{
public:
    lmGrpVoice(lmToolPage* pParent, wxBoxSizer* pMainSizer);
    ~lmGrpVoice() {}

    //implement virtual methods
    void CreateControls(wxBoxSizer* pMainSizer);
    inline lmEToolGroupID GetToolGroupID() { return lmGRP_Voice; }

	//access to options
	inline int GetVoice() { return m_nSelButton; }
	inline void SetVoice(int nVoice) { SelectButton(nVoice); }
    void SetVoice(bool fUp);

};


//--------------------------------------------------------------------------------
// Group for Note duration
//--------------------------------------------------------------------------------
class lmGrpNoteDuration: public lmToolButtonsGroup
{
public:
    lmGrpNoteDuration(lmToolPage* pParent, wxBoxSizer* pMainSizer);
    ~lmGrpNoteDuration() {}

    //implement virtual methods
    void CreateControls(wxBoxSizer* pMainSizer);
    inline lmEToolGroupID GetToolGroupID() { return lmGRP_NoteDuration; }
    //inline int GetNumTools();
    //inline long GetToolID(int nTool);

	//access to options
	lmENoteType GetNoteDuration();

};


//--------------------------------------------------------------------------------
// Group for Note accidentals
//--------------------------------------------------------------------------------
class lmGrpNoteAcc: public lmToolButtonsGroup
{
public:
    lmGrpNoteAcc(lmToolPage* pParent, wxBoxSizer* pMainSizer);
    ~lmGrpNoteAcc() {}

    //implement virtual methods
    void CreateControls(wxBoxSizer* pMainSizer);
    inline lmEToolGroupID GetToolGroupID() { return lmGRP_NoteAcc; }

	//access to options
	lmEAccidentals GetNoteAcc();

};


//--------------------------------------------------------------------------------
// Group for note dots
//--------------------------------------------------------------------------------
class lmGrpNoteDots: public lmToolButtonsGroup
{
public:
    lmGrpNoteDots(lmToolPage* pParent, wxBoxSizer* pMainSizer);
    ~lmGrpNoteDots() {}

    //implement virtual methods
    void CreateControls(wxBoxSizer* pMainSizer);
    inline lmEToolGroupID GetToolGroupID() { return lmGRP_NoteDots; }

	//access to options
	int GetNoteDots();

};


//--------------------------------------------------------------------------------
// Group for tuplets, ties, ...
//--------------------------------------------------------------------------------
class lmGrpTieTuplet: public lmToolGroup
{
public:
    lmGrpTieTuplet(lmToolPage* pParent, wxBoxSizer* pMainSizer);
    ~lmGrpTieTuplet() {}

    //implement virtual methods
    void CreateControls(wxBoxSizer* pMainSizer);
    inline lmEToolGroupID GetToolGroupID() { return lmGRP_TieTuplet; }

    //event handlers
    void OnTieButton(wxCommandEvent& event);
    void OnTupletButton(wxCommandEvent& event);

	//access to options
    void SetToolTie(bool fChecked);
    void SetToolTuplet(bool fChecked);
    void EnableTool(lmEToolID nToolID, bool fEnabled);


protected:
    void PostToolBoxEvent(lmEToolID nToolID, bool fSelected);

    lmCheckButton*      m_pBtnTie;
    lmCheckButton*      m_pBtnTuplet;

    DECLARE_EVENT_TABLE()
};


//--------------------------------------------------------------------------------
// Group for beams
//--------------------------------------------------------------------------------
class lmGrpBeams: public lmToolGroup
{
public:
    lmGrpBeams(lmToolPage* pParent, wxBoxSizer* pMainSizer);
    ~lmGrpBeams() {}

    //implement virtual methods
    void CreateControls(wxBoxSizer* pMainSizer);
    inline lmEToolGroupID GetToolGroupID() { return lmGRP_Beams; }

    //event handlers
    void OnButton(wxCommandEvent& event);

	////access to options
 //   void SetToolTie(bool fChecked);
 //   void SetToolTuplet(bool fChecked);
    void EnableTool(lmEToolID nToolID, bool fEnabled);


protected:
    void PostToolBoxEvent(lmEToolID nToolID, bool fSelected);

    lmBitmapButton*      m_pBtnBeamCut;
    lmBitmapButton*      m_pBtnBeamJoin;
    lmBitmapButton*      m_pBtnBeamFlatten;
    lmBitmapButton*      m_pBtnBeamSubgroup;

    DECLARE_EVENT_TABLE()
};



//--------------------------------------------------------------------------------
// The panel
//--------------------------------------------------------------------------------

class lmToolPageNotes: public lmToolPage
{
public:
    lmToolPageNotes(wxWindow* parent);
    ~lmToolPageNotes();

    //implementation of virtual methods
    lmToolGroup* GetToolGroup(lmEToolGroupID nGroupID);

	//access to options

    //interface with Octave group
	inline int GetOctave() { return m_pGrpOctave->GetOctave(); }
    inline void SetOctave(bool fUp) { return m_pGrpOctave->SetOctave(fUp); }
    inline void SetOctave(int nOctave) { return m_pGrpOctave->SetOctave(nOctave); }

    //interface with voice group
	inline int GetVoice() { return m_pGrpVoice->GetVoice(); }
    inline void SetVoice(bool fUp) { return m_pGrpVoice->SetVoice(fUp); }
    inline void SetVoice(int nVoice) { return m_pGrpVoice->SetVoice(nVoice); }

    //interface with NoteDuration group
    inline void EnableGrpNoteDuration(bool fEnabled) { m_pGrpNoteDuration->EnableGroup(fEnabled); }
    inline void SetNoteDuration(lmENoteType nNoteType) { return m_pGrpNoteDuration->SelectButton((int)nNoteType - 1); }
    inline lmENoteType GetNoteDuration() { return m_pGrpNoteDuration->GetNoteDuration(); }
    inline int GetNoteDurationButton() { return m_pGrpNoteDuration->GetSelectedButton(); }
    inline void SetNoteDurationButton(int iB) { m_pGrpNoteDuration->SelectButton(iB); }

    //interface with NoteAccidentals group
    inline void EnableGrpNoteAcc(bool fEnabled) { m_pGrpNoteAcc->EnableGroup(fEnabled); }
    inline void SetNoteAccidentals(lmEAccidentals nAcc) { m_pGrpNoteAcc->SelectButton((int)nAcc - 1); }
    inline lmEAccidentals GetNoteAccidentals() { return m_pGrpNoteAcc->GetNoteAcc(); }
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

    //interface with TieTuplet group
    inline void EnableGrpTieTuplet(bool fEnabled) { m_pGrpTieTuplet->EnableGroup(fEnabled); }
    inline void SetToolTie(bool fChecked) { m_pGrpTieTuplet->SetToolTie(fChecked); }
    inline void SetToolTuplet(bool fChecked) { m_pGrpTieTuplet->SetToolTuplet(fChecked); }

    //interface with Beam tools group
    //inline void EnableGrpTieTuplet(bool fEnabled) { m_pGrpTieTuplet->EnableGroup(fEnabled); }
    //inline void SetToolTie(bool fChecked) { m_pGrpTieTuplet->SetToolTie(fChecked); }
    //inline void SetToolTuplet(bool fChecked) { m_pGrpTieTuplet->SetToolTuplet(fChecked); }


    //interface with NoteheadType group
	lmENoteHeads GetNoteheadType();
    //inline lmENoteHeads GetNoteheadType() { return m_pGrpNoteDuration->GetNoteDuration(); }
    //inline void SetNoteDurationButton(int iB) { m_pGrpNoteDuration->SelectButton(iB); }


private:

    //groups
    lmGrpNoteDuration*  m_pGrpNoteDuration;
    lmGrpNoteAcc*       m_pGrpNoteAcc;
    lmGrpNoteDots*      m_pGrpNoteDots;
    lmGrpTieTuplet*     m_pGrpTieTuplet;
    lmGrpBeams*         m_pGrpBeams;
	lmGrpOctave*		m_pGrpOctave;
	lmGrpVoice*			m_pGrpVoice;

	//options
	wxBitmapComboBox*	m_pCboNotehead;
	wxBitmapComboBox*	m_pCboAccidentals;
};

#endif    // __LM_TOOLNOTES_H__
