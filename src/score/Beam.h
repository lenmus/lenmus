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

#ifndef __LM_BEAM_H__        //to avoid nested includes
#define __LM_BEAM_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Beam.cpp"
#endif

#include <list>

class lmShapeBeam;
class lmShapeStem;
class lmShapeNote;
class lmUndoData;


class lmBeam : public lmMultipleRelationship<lmNoteRest>
{
public:
    lmBeam(lmNote* pNote);
    lmBeam(lmNoteRest* pFirstNote, lmUndoData* pUndoData);
    ~lmBeam();

	//implementation of lmMultipleRelationship virtual methods
    inline void Save(lmUndoData* pUndoData) {}
	inline lmERelationshipClass GetClass() { return lm_eBeamClass; }
	void OnRelationshipModified();

	//specific methods
    void CreateShape();
    lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, wxColour color);
	void AddNoteAndStem(lmShapeStem* pStem, lmShapeNote* pNote, lmTBeamInfo* pBeamInfo);
    void AddRestShape(lmShape* pRestShape);
    void AutoSetUp();
    void NeedsSetUp(bool fValue) { m_fNeedsSetUp = fValue; }
    bool NeedsSetUp() { return m_fNeedsSetUp; }

private:
    int GetBeamingLevel(lmNote* pNote);

        //member variables

    bool            m_fStemsDown;
    bool            m_fNeedsSetUp;
	lmShapeBeam*	m_pBeamShape;

    //beam information to be transferred to each beamed note
    int            m_nPosForRests;        //relative position for rests

};

#endif    // __LM_BEAM_H__

