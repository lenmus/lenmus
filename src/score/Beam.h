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

#ifndef __LM_BEAM_H__        //to avoid nested includes
#define __LM_BEAM_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Beam.cpp"
#endif

#include <list>
#include "wx/dc.h"

class lmShapeBeam;
class lmShapeStem;
class lmShapeNote;
class lmUndoData;


class lmBeam
{
public:
    lmBeam(lmNote* pNote);
    lmBeam(lmNoteRest* pFirstNote, lmUndoData* pUndoData);
    ~lmBeam();

    inline void Save(lmUndoData* pUndoData) {}

    void Include(lmNoteRest* pNR, int nIndex = -1);
    void Remove(lmNoteRest* pNR);
    inline int NumNotes() { return (int)m_Notes.size(); }
	int GetNoteIndex(lmNoteRest* pNR);
    inline lmNoteRest* GetStartNoteRest() { return m_Notes.front(); }
    inline lmNoteRest* GetEndNoteRest() { return m_Notes.back(); }

    void CreateShape();
    lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, wxColour color);
	void AddNoteAndStem(lmShapeStem* pStem, lmShapeNote* pNote, lmTBeamInfo* pBeamInfo);
    void RemoveAllNotes();
    void AutoSetUp();

private:
    int GetBeamingLevel(lmNote* pNote);

        //member variables

    bool            m_fStemsDown;
	lmShapeBeam*	m_pBeamShape;

	//notes/rests in this beam (if chord, only base note)
	std::list<lmNoteRest*>	m_Notes;

    //beam information to be transferred to each beamed note
    int            m_nPosForRests;        //relative position for rests

};

#endif    // __LM_BEAM_H__

