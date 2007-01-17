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
/*! @file Chord.h
    @brief Header file for class lmChord
    @ingroup score_kernel
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __CHORD_H__        //to avoid nested includes
#define __CHORD_H__

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
    void DrawStem(bool fMeasuring, lmPaper* pPaper, wxColour colorC, wxFont* pFont,
                  lmVStaff* pVStaff, int nStaff);
    void ComputeLayout(lmPaper* pPaper, lmUPoint paperPos, wxColour colorC);

    //debug
    wxString Dump();


private:
    lmLUnits DrawFlag(bool fMeasuring, lmPaper* pPaper, lmNote* pBaseNote, lmUPoint pos,
                      wxColour colorC, wxFont* pFont, lmVStaff* pVStaff, int nStaff);
    void ComputeStemDirection();
    void ArrangeNoteheads();
    lmNote* CheckIfCollisionWithAccidentals(bool fOnlyLeftNotes, int iCurNote, lmShapeObj* pShape);

    void ComputeAccidentalLayout(bool fOnlyLeftNotes, lmNote* pNote, int iN, lmPaper* pPaper,
                                 lmUPoint paperPos, wxColour colorC);
    lmNote* CheckIfNoteCollision(lmShapeObj* pShape);




        // member variables


    NotesList   m_cNotes;           //list of notes that form the chord
    lmNote*     m_pBaseNote;        //base note (first note in chord definition)
    lmNote*     m_pMinNote;         //lowest pitch note
    lmNote*     m_pMaxNote;         //highest pitch note
    bool        m_fStemDown;        //chord stem direction
    EStemType   m_nStemType;        //type of stem

    lmNote*     m_oNotaPrevia;      //nota que precede a la primera del acorde
                        //m_oNotaPrevia es la nota que precede al comienzo del acorde. Es preciso tener acceso
                        //a ella porque si está ligada a la primera del acorde la dirección de las plicas del acorde
                        //debería hacerse coincidir con la de la nota previa.
};

#endif    // __CHORD_H__

