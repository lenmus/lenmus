//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#ifndef __LM_CHORD_H__        //to avoid nested includes
#define __LM_CHORD_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Chord.cpp"
#endif

#include <list>

class lmBox;
class lmShapeNote;
class lmUndoData;

class lmChord
{
public:
    lmChord(lmNote* pNote);
    ~lmChord();

    lmNote* GetMaxNote();
    lmNote* GetMinNote();
	inline lmNote* GetBaseNote() { return m_Notes.front(); }
    inline int NumNotes() { return (int)m_Notes.size(); }
    void Include(lmNote* pNote, int nIndex = -1);
    void Remove(lmNote* pNote);
	inline bool IsLastNoteOfChord(lmNote* pNote) { return m_Notes.back() == pNote; }
    void SetStemDirection(bool fStemDown);
    void AddStemShape(lmPaper* pPaper, wxColour colorC,
					  wxFont* pFont, lmVStaff* pVStaff, int nStaff);
    void LayoutNoteHeads(lmBox* pBox, lmPaper* pPaper, lmUPoint uPaperPos, wxColour colorC);
    lmLUnits GetXRight();
	inline bool IsBaseNote(lmNote* pNote) { return pNote == m_Notes.front(); }
	int GetNoteIndex(lmNote* pNote);

	//debug
    wxString Dump();

private:
    //lmLUnits DrawFlag(bool fMeasuring, lmPaper* pPaper, lmNote* pBaseNote, lmUPoint uPos,
    //                  wxColour colorC, wxFont* pFont, lmVStaff* pVStaff, int nStaff);
    void ComputeStemDirection();
    void ArrangeNoteheads();
    lmNote* CheckIfCollisionWithAccidentals(bool fOnlyLeftNotes, int iCurNote, lmShape* pShape);

    void ComputeAccidentalLayout(bool fOnlyLeftNotes, lmNote* pNote, int iN, lmPaper* pPaper,
                                 lmUPoint uPaperPos, wxColour colorC);
    lmNote* CheckIfNoteCollision(lmShape* pShape);


        // member variables

    std::list<lmNote*>  m_Notes;		//list of notes that form the chord    
    bool				m_fStemDown;	//chord stem direction
    lmEStemType			m_nStemType;	//type of stem
};

#endif    // __LM_CHORD_H__

