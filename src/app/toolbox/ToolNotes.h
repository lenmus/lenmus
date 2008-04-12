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

#include "ToolPage.h"
#include "../../score/defs.h"

class wxBitmapComboBox;
class lmCheckButton;

class lmToolNotes: public lmToolPage
{
public:
    lmToolNotes(wxWindow* parent);
    ~lmToolNotes();

	//access to options
	lmENoteType GetNoteDuration();
	lmENoteHeads GetNoteheadType();
	lmEAccidentals GetNoteAccidentals();

	void OnNoteButton(wxCommandEvent& event);
	void SelectNoteButton(int iB);



private:

    // controls
    wxChoice*        m_pChoice;

    // other member variables
    int				m_nNumLangs;
    wxArrayString	m_cLangCodes;
    wxArrayString	m_cLangNames;
    wxString        m_sCurLang;
	
	//selected buttons
	int			m_nNoteDuration;

	//options
	wxBitmapComboBox*	m_pCboNotehead;
	wxBitmapComboBox*	m_pCboAccidentals;
	lmCheckButton*		m_pBtDurations[10];


    DECLARE_EVENT_TABLE()
};

#endif    // __LM_TOOLNOTES_H__
