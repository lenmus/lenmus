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

	//access to options
	int GetNoteDots();

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
    inline lmENoteType GetNoteDuration() { return m_pGrpNoteDuration->GetNoteDuration(); }
    inline void SetNoteDuration(int iB) { m_pGrpNoteDuration->SelectButton(iB); }

    //interface with NoteAccidentals group
    inline lmEAccidentals GetNoteAccidentals() { return m_pGrpNoteAcc->GetNoteAcc(); }
    inline void SetNoteAcc(int iB) { m_pGrpNoteAcc->SelectButton(iB); }
    inline void SelectNextAccidental() { m_pGrpNoteAcc->SelectNextButton(); }
    inline void SelectPrevAccidental() { m_pGrpNoteAcc->SelectPrevButton(); }

    //interface with NoteDots group
    inline int GetNoteDots() { return m_pGrpNoteDots->GetNoteDots(); }
    inline void SetNoteDots(int iB) { m_pGrpNoteDots->SelectButton(iB); }
    inline void SelectNextDot() { m_pGrpNoteDots->SelectNextButton(); }
    inline void SelectPrevDot() { m_pGrpNoteDots->SelectPrevButton(); }


    //interface with NoteheadType group
	lmENoteHeads GetNoteheadType();
    //inline lmENoteHeads GetNoteheadType() { return m_pGrpNoteDuration->GetNoteDuration(); }
    //inline void SetNoteDuration(int iB) { m_pGrpNoteDuration->SelectButton(iB); }


private:

    //groups
    lmGrpNoteDuration*  m_pGrpNoteDuration;
    lmGrpNoteAcc*       m_pGrpNoteAcc;
    lmGrpNoteDots*      m_pGrpNoteDots;

	//options
	wxBitmapComboBox*	m_pCboNotehead;
	wxBitmapComboBox*	m_pCboAccidentals;
};

#endif    // __LM_TOOLNOTES_H__
