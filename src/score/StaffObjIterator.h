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

#ifndef __STAFFOBJITERATOR_H__        //to avoid nested includes
#define __STAFFOBJITERATOR_H__

#ifdef __GNUG__
#pragma interface "StaffObjIterator.cpp"
#endif

//#include "Score.h"
#include "StaffObj.h"

class lmColStaffObjs;


// Para facilitar las futuras modificaciones en las estructuras de datos que implementan esta
// clase, cuando se crea un iterador se especifica el tipo de recorrido necesario. La implementación
// debe siempre respetar esos criterios. Los códigos y sus significados son:
//
/*! @enum ETraversingOrder
    eTR_ByTime:
       The StaffObjs are recovered ordered by increasing time position.

    eTR_OptimizeAccess:
       The recovery order inside a bar is not important. So use the ordering that results in
       the fastest access time

    Ordering is internal to bars and never afect to the barline, who is always the last item
    recovered on each bar. That is, in the previous ordering methods, the following
    restrictions always applies:
    1. StaffObjs in a bar are always traversed before than those of the next bar.
    2. StaffBojs of type lmBarline are the last item traversed in each bar.

    eTR_AsStored:
        Items are recovered in the order in which they are stored in the internal
        data structure used to implement the collection. This ordering method MUST ONLY
        BE USED for debugging purposes.

*/
enum ETraversingOrder
{
    eTR_AsStored = 1,        //se recorren por orden de almacenamiento, sin restricciones.
    eTR_ByTime,                //se recorren por marca de tiempo
    eTR_OptimizeAccess        //se recorren en el orden en que resulte más rápido
};

class lmStaffObjIterator
{
public:
    lmStaffObjIterator(ETraversingOrder nOrder, lmColStaffObjs* pCSO);
    bool        EndOfList();
    lmStaffObj*    GetCurrent();
    void        AdvanceToMeasure(int nBar);
    void        MoveFirst();
    void        MoveNext();
    void        MovePrev();
    void        MoveLast();
    void        BackToItemOfType(EStaffObjType nType);
    void        GoToItem(lmStaffObj* pSO);

private:
    lmColStaffObjs*            m_pColStaffobjs;    //object lmColStaffObjs that is being traversed
    wxStaffObjsListNode*    m_pCurrentNode;        //cursor pointing to curren node

};

#endif    // __STAFFOBJITERATOR_H__

