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

#ifndef __BEAM_H__        //to avoid nested includes
#define __BEAM_H__

#ifdef __GNUG__
#pragma interface "Beam.cpp"
#endif

#include "wx/dc.h"

/*------------------------------------------------------------------------------------------------
 lmBeam object
    lmBeam objects are auxiliary objects within lmNote objects to contain the information and
    methods related to beaming: grouping the beams of several consecutive notes
------------------------------------------------------------------------------------------------*/

class lmBeam
{
public:
    lmBeam(lmNoteRest* pNotePrev);
    ~lmBeam();

    void    Include(lmNoteRest* pNR);
    void    Remove(lmNoteRest* pNR);
    int     NumNotes();
    void    ComputeStemsDirection();
    void    TrimStems();
    void    DrawBeamLines(lmPaper* pPaper, lmLUnits uThickness, lmLUnits uBeamSpacing,
                          wxColour color);


private:
    void DrawBeamSegment(lmPaper* pPaper, bool fStemDown,
                         lmLUnits uxStart, lmLUnits uyStart,
                         lmLUnits uxEnd, lmLUnits uyEnd, lmLUnits uThickness,
                         lmNote* pStartNote, lmNote* pEndNote,
                         wxColour color);
    lmLUnits ComputeYPosOfSegment(lmNote* pNote, bool fStemDown, lmLUnits yShift);

    lmNoteRest*     m_pNotePrev;    //the previous note to the group (for ties)
    bool            m_fStemsDown;
    NoteRestsList   m_cNotes;       // list of notes beamed. For the lmBeam object, chords are 
                                    //      just one note and so, only the base note of the chord 
                                    //      is included in the beam notes list

    lmNote*         m_pFirstNote;   // the group could start or end with a rest. Here we 
    lmNote*         m_pLastNote;    //      lets keep pointer to the first and last notes.    

    //beam information to be transferred to each beamed note
    int            m_nPosForRests;        //posición relativa de los silencios dentro del grupo

};

#endif    // __BEAM_H__

