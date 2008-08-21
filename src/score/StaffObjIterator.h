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

#ifndef __LM_STAFFOBJITERATOR_H__        //to avoid nested includes
#define __LM_STAFFOBJITERATOR_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "StaffObjIterator.cpp"
#endif

#include "StaffObj.h"
#include "ColStaffObjs.h"



class lmSOIterator
{
public:
    lmSOIterator(ETraversingOrder nOrder, lmColStaffObjs* pCSO, int nVoice);
    lmSOIterator(ETraversingOrder nOrder, lmColStaffObjs* pCSO, lmStaffObj* pTargetSO);
    lmSOIterator(ETraversingOrder nOrder, lmColStaffObjs* pCSO, lmVStaffCursor* pVCursor);
	~lmSOIterator() {}

    bool EndOfList();
    bool StartOfList();
    bool EndOfMeasure();
	inline lmStaffObj* GetCurrent() { return *m_it; }
    void AdvanceToMeasure(int nBar);
    void MoveFirst();
    void MoveNext();
    void MovePrev();
    void MoveLast();

private:
    lmColStaffObjs*		m_pColStaffObjs;    //object lmColStaffObjs that is being traversed
    lmItCSO				m_pCurrentNode;     //cursor pointing to current node
	int					m_nVoice;			//voice to recover. 0=all

	lmItCSO				m_it;				//iterator pointing to current object
	int					m_nSegment;			//current segment (0..n-1)
	lmSegment*			m_pSegment;			//current segment
    bool                m_fEndOfMeasure;    
};


#endif    // __LM_STAFFOBJITERATOR_H__

