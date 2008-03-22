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

#ifndef __LM_TUPLETBRACKET_H__        //to avoid nested includes
#define __LM_TUPLETBRACKET_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "TupletBracket.cpp"
#endif


#include "NotesRelationship.h"
#include "../graphic/ShapeTuplet.h"

class lmUndoData;

// lmTupletBracket
//    A lmTupletBracket represents the optional bracket graphically associated
//    to tuplets. The lmTupletBracket object does not have any effect on sound. It is
//    only to describe how a tuplet must be displayed.

class lmTupletBracket : public lmMultipleRelationship<lmNoteRest>
{
public:
    lmTupletBracket(bool fShowNumber, int nNumber, bool fBracket, lmEPlacement nAbove,
                    int nActualNotes, int nNormalNotes);
    lmTupletBracket(lmNoteRest* pFirstNote, lmUndoData* pUndoData);
    ~lmTupletBracket();

	//creation related methods
    void Create(bool fShowNumber, int nNumber, bool fBracket, lmEPlacement nAbove,
                int nActualNotes, int nNormalNotes);

	//implementation of lmMultipleRelationship virtual methods
    void Save(lmUndoData* pUndoData);
	inline lmERelationshipClass GetClass() { return lm_eTupletClass; }
	inline void OnRelationshipModified() {}

    //info
    inline int GetTupletNumber() { return m_nTupletNumber; }
    inline int GetActualNotes() { return m_nActualNotes; }
    inline int GetNormalNotes() { return m_nNormalNotes; }

    //layout
	lmShape* LayoutObject(lmBox* pBox, lmPaper* pPaper, wxColour color);
	lmShape* GetShape() { return m_pShape; }


private:
    //time modifiers
    int             m_nActualNotes;     //number of notes to play in the time ...
    int             m_nNormalNotes;     //... allotted for this number of normal notes

    // graphical attributes
    bool    	    m_fShowNumber;      // display tuplet number
    bool    	    m_fBracket;         // display bracket
    bool            m_fBold;
    bool            m_fItalic;
    int     	    m_nTupletNumber;    // number to display
    int             m_nFontSize;
    lmEPlacement	m_nAbove;		    // bracket positioned above the notes
    wxString        m_sFontName;        // font info for rendering tuplet number
	lmShapeTuplet*	m_pShape;		    //the shape to render the tuplet bracket

};

#endif    // __LM_TUPLETBRACKET_H__

