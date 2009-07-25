//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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



//lmSOIterator encapsulates access and traverse of a collection of StaffObjs (lmColStaffObjs
//object) without exposing the internal structure of the collection. This lets us
//define different traversal algorithms and allows us to change the internal representation
//of a StaffObjs collection without affecting the rest of the program.


class lmSOIterator
{
public:
    lmSOIterator(lmColStaffObjs* pCSO);
    lmSOIterator(lmColStaffObjs* pCSO, lmStaffObj* pTargetSO);
    lmSOIterator(lmColStaffObjs* pCSO, lmScoreCursor* pCursor);
    lmSOIterator(lmSOIterator* pIT);    //Copy constructor
	~lmSOIterator() {}

    inline bool FirstOfCollection() { 
                    return (m_pSO && m_pSO == m_pColStaffObjs->GetFirstSO()); }
    inline bool LastOfCollection() { 
                    return (m_pSO && m_pSO == m_pColStaffObjs->GetLastSO()); }
    inline bool StartOfCollection() { 
                    return FirstOfCollection() || m_pColStaffObjs->IsEmpty(); }
    inline bool EndOfCollection() { 
                    return (m_pSO == (lmStaffObj*)NULL || (FirstOfCollection() && m_fEnd)); }

    inline bool ChangeOfMeasure() { return m_fChangeOfMeasure; }
	inline lmStaffObj* GetCurrent() { return m_pSO; }
    inline int GetNumSegment() { return (m_pSO ? m_pSO->GetSegment()->GetNumSegment() 
                                               : m_pColStaffObjs->GetNumSegments()-1 ); }
    inline void ResetFlags() { m_fChangeOfMeasure = false; }
    inline bool IsManagingCollection(lmColStaffObjs* pCSO) { 
                    return pCSO == m_pColStaffObjs; }


    void AdvanceToMeasure(int nBar);
    void MoveFirst();
    void MoveNext();
    void MovePrev();
    void MoveLast();
    void MoveTo(lmStaffObj* pSO);

private:
    lmColStaffObjs*		m_pColStaffObjs;    //object lmColStaffObjs that is being traversed
	lmStaffObj*         m_pSO;				//points to current object
    bool                m_fChangeOfMeasure;
    bool                m_fEnd;             //we are at end of collection
};


#endif    // __LM_STAFFOBJITERATOR_H__

