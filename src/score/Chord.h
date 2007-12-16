//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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

#ifndef __LM_CHORD_H__        //to avoid nested includes
#define __LM_CHORD_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Chord.cpp"
#endif

class lmBox;
class lmShapeNote;

class lmChord
{
public:
    lmChord(lmNote* pNote);
    ~lmChord();

    lmNote* GetMaxNote() { return m_pMaxNote; }
    lmNote* GetMinNote() { return m_pMinNote; }
    lmNote* GetBaseNote();
    int GetNumNotes();
    void AddNote(lmNote* pNote);
    void RemoveNote(lmNote* pNote);
    bool IsLastNoteOfChord(lmNote* pNote);
    void SetStemDirection(bool fStemDown);
    void AddStemShape(lmPaper* pPaper, wxColour colorC,
					  wxFont* pFont, lmVStaff* pVStaff, int nStaff);
    void LayoutNoteHeads(lmBox* pBox, lmPaper* pPaper, lmUPoint uPaperPos, wxColour colorC);
    lmLUnits GetXRight();

	//debug
    wxString Dump();

private:
    lmLUnits DrawFlag(bool fMeasuring, lmPaper* pPaper, lmNote* pBaseNote, lmUPoint uPos,
                      wxColour colorC, wxFont* pFont, lmVStaff* pVStaff, int nStaff);
    void ComputeStemDirection();
    void ArrangeNoteheads();
    lmNote* CheckIfCollisionWithAccidentals(bool fOnlyLeftNotes, int iCurNote, lmShape* pShape);

    void ComputeAccidentalLayout(bool fOnlyLeftNotes, lmNote* pNote, int iN, lmPaper* pPaper,
                                 lmUPoint uPaperPos, wxColour colorC);
    lmNote* CheckIfNoteCollision(lmShape* pShape);




        // member variables


    NotesList		m_cNotes;		//list of notes that form the chord
    lmNote*			m_pBaseNote;	//base note (first note in chord definition)
    lmNote*			m_pMinNote;		//lowest pitch note
    lmNote*			m_pMaxNote;		//highest pitch note
    lmNote*			m_pLastNote;	//the last note added the chord
    bool			m_fStemDown;	//chord stem direction
    lmEStemType		m_nStemType;	//type of stem
};

#endif    // __LM_CHORD_H__

