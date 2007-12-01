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

/*! @class lmStaffObjIterator
    @ingroup score_kernel
    @brief Encapsulates access and traverse of a collection of StaffObjs

    lmStaffObjIterator encapsulates access and traverse of a collection of StaffObjs (lmColStaffObjs
    object) without exposing the internal structure of the collection. This lets us
    define different traversal algorithms and allows us to change the internal representation
    of a StaffObjs collection without affecting the rest of the program.
*/
#ifdef __GNUG__
#pragma implementation "StaffObjIterator.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Score.h"


lmStaffObjIterator::lmStaffObjIterator(ETraversingOrder nOrder, lmColStaffObjs* pCSO)
{
    // Currently, StaffObjs are stored in a list and the list is always ordered by time.
    // Therefore all three types of ETraversingOrder are the equivalent and can be implemented
    // just by traversing the list

    m_pColStaffobjs = pCSO;
    MoveFirst();

}

bool lmStaffObjIterator::EndOfList()
{
    // returns true if cursor does not point to a valid item
    return (m_pCurrentNode ? false : true);
}

lmStaffObj* lmStaffObjIterator::GetCurrent()
{
    // returns the lmStaffObj pointed by cursor
    wxASSERT(m_pCurrentNode);
    return (lmStaffObj*) m_pCurrentNode->GetData();
}

void lmStaffObjIterator::AdvanceToMeasure(int nMeasure)
{
    // advance cursor to first lmStaffObj in measure number nMeasure (1..n)
	if (nMeasure > m_pColStaffobjs->GetNumMeasures()) return;
    m_pCurrentNode = m_pColStaffobjs->GetFirstInMeasure(nMeasure);

}

void lmStaffObjIterator::MoveFirst()
{
    // move cursor to first lmStaffObj
    m_pCurrentNode = m_pColStaffobjs->GetFirst();
}

void lmStaffObjIterator::MoveNext()
{
    // advance cursor to next lmStaffObj
    if (m_pCurrentNode)
        m_pCurrentNode = m_pCurrentNode->GetNext();
}

void lmStaffObjIterator::MovePrev()
{
    // move cursor back to previous lmStaffObj
    if (m_pCurrentNode)
        m_pCurrentNode = m_pCurrentNode->GetPrevious();
}

void lmStaffObjIterator::MoveLast()
{
    // move cursor to last lmStaffObj
    m_pCurrentNode = m_pColStaffobjs->GetLast();
}

void lmStaffObjIterator::BackToItemOfType(EStaffObjType nType)
{
    // goes back until an staffObj of type nType is found or until passing the start of
    // the collection (cursor == 0) if none is found
    if (EndOfList()) return;
    MovePrev();
    for (; !EndOfList(); ) {
        if (GetCurrent()->GetClass() == nType) break;
        MovePrev();
    }

}

void lmStaffObjIterator::GoToItem(lmStaffObj* pSO)
{
    //locate staffObj received as parameter and leave iterator positioned on that item.

    /*TODO
        Review implementation and usage. Is it still true what is said in the
        "aware" note?
    */
    //AWARE This method is only used in FindPossibleStartOfTie. Therefore, search is
    // done backwards to optimize.

 //   lmStaffObj* pX;
 //    m_pCurrentNode = m_pColStaffobjs->GetFirst();
    //for (; m_pCurrentNode; ) {
    //    pX = (lmStaffObj *)m_pCurrentNode->GetData();
    //    if (pX->GetId() == pSO->GetId()) break;
    //    m_pCurrentNode = m_pCurrentNode->GetNext();
    //}

}


