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

#ifndef __LM_STAFFOBJITERATOR_H__        //to avoid nested includes
#define __LM_STAFFOBJITERATOR_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "StaffObjIterator.cpp"
#endif

#include "StaffObj.h"
#include "ColStaffObjs.h"


class lmStaffObjIterator
{
public:
    lmStaffObjIterator(ETraversingOrder nOrder, lmColStaffObjs* pCSO);
    lmStaffObjIterator(lmStaffObj* pSO, lmColStaffObjs* pCSO);

    bool        EndOfList();
    bool        StartOfList();
    lmStaffObj* GetCurrent();
    void        AdvanceToMeasure(int nBar);
    void        MoveFirst();
    void        MoveNext();
    void        MovePrev();
    void        MoveLast();
    void        BackToItemOfType(EStaffObjType nType);
    void        GoToItem(lmStaffObj* pSO);

private:
    lmColStaffObjs*     m_pColStaffobjs;    //object lmColStaffObjs that is being traversed
    lmItCSO           m_pCurrentNode;     //cursor pointing to curren node

};

#endif    // __LM_STAFFOBJITERATOR_H__

