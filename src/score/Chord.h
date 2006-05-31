//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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
    void DrawStem(bool fMeasuring, wxDC* pDC, wxColour colorC, wxFont* pFont,
                  lmVStaff* pVStaff, int nStaff);


private:
    lmLUnits DrawFlag(bool fMeasuring, wxDC* pDC, lmNote* pBaseNote, wxPoint pos,
                      wxColour colorC, wxFont* pFont, lmVStaff* pVStaff, int nStaff);
    void ComputeStemDirection();



        // member variables


    NotesList   m_cNotes;           //list of notes that form the chord
    lmNote*     m_pMinNote;         //lowest pitch note
    lmNote*     m_pMaxNote;         //highest pitch note
    bool        m_fStemDown;        //chord stem direction

    lmNote*     m_oNotaPrevia;      //nota que precede a la primera del acorde
                        //m_oNotaPrevia es la nota que precede al comienzo del acorde. Es preciso tener acceso
                        //a ella porque si está ligada a la primera del acorde la dirección de las plicas del acorde
                        //debería hacerse coincidir con la de la nota previa.
};

#endif    // __CHORD_H__

