//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation;
//    either version 2 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street,
//    Fifth Floor, Boston, MA  02110-1301, USA.
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

protected:
    void PostToolBoxEvent(lmEToolID nToolID, bool fSelected);

    lmCheckButton*      m_pBtnTie;
    lmCheckButton*      m_pBtnTuplet;

    DECLARE_EVENT_TABLE()
};



//--------------------------------------------------------------------------------
// The panel
//--------------------------------------------------------------------------------

class lmToolNotes: public lmToolPage
{
public:
    lmToolNotes(wxWindow* parent);
    ~lmToolNotes();

	//access to options

    //interface with NoteDuration group
    inline void SetNoteDuration(lmENoteType nNoteType) { return m_pGrpNoteDuration->SelectButton((int)nNoteType - 1); }
    inline lmENoteType GetNoteDuration() { return m_pGrpNoteDuration->GetNoteDuration(); }
    inline int GetNoteDurationButton() { return m_pGrpNoteDuration->GetSelectedButton(); }
    inline void SetNoteDurationButton(int iB) { m_pGrpNoteDuration->SelectButton(iB); }

    //interface with NoteAccidentals group
    inline void SetNoteAccidentals(lmEAccidentals nAcc) { m_pGrpNoteAcc->SelectButton((int)nAcc - 1); }
    inline lmEAccidentals GetNoteAccidentals() { return m_pGrpNoteAcc->GetNoteAcc(); }
    inline void SelectNextAccidental() { m_pGrpNoteAcc->SelectNextButton(); }
    inline void SelectPrevAccidental() { m_pGrpNoteAcc->SelectPrevButton(); }
    inline int GetNoteAccButton() { return m_pGrpNoteAcc->GetSelectedButton(); }
    inline void SetNoteAccButton(int iB) { m_pGrpNoteAcc->SelectButton(iB); }

    //interface with NoteDots group
    inline void SetNoteDots(int nDots) { m_pGrpNoteDots->SelectButton(nDots - 1); }
    inline int GetNoteDots() { return m_pGrpNoteDots->GetNoteDots(); }
    inline void SelectNextDot() { m_pGrpNoteDots->SelectNextButton(); }
    inline void SelectPrevDot() { m_pGrpNoteDots->SelectPrevButton(); }
    inline int GetNoteDotsButton() { return m_pGrpNoteDots->GetSelectedButton(); }
    inline void SetNoteDotsButton(int iB) { m_pGrpNoteDots->SelectButton(iB); }



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

	//options
	wxBitmapComboBox*	m_pCboNotehead;
	wxBitmapComboBox*	m_pCboAccidentals;
};

#endif    // __LM_TOOLNOTES_H__
