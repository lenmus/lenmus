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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
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
#include "VStaff.h"


//========================================================================================
// lmSOIterator implementation
//========================================================================================

lmSOIterator::lmSOIterator(lmColStaffObjs* pCSO)
{
    m_pColStaffObjs = pCSO;
	MoveFirst(); 
}

lmSOIterator::lmSOIterator(lmColStaffObjs* pCSO, lmStaffObj* pTargetSO)
{
    m_pColStaffObjs = pCSO;
	m_pSO = pTargetSO; 
    m_fChangeOfMeasure = false;
    m_fEnd = false;
}

lmSOIterator::lmSOIterator(lmColStaffObjs* pCSO, lmVStaffCursor* pVCursor)
{
    //creates an iterator pointing to received cursor position

    m_pColStaffObjs = pCSO;
	m_pSO = pVCursor->GetStaffObj();
    m_fChangeOfMeasure = false;
    m_fEnd = false;
}

lmSOIterator::lmSOIterator(lmSOIterator* pIT)
{
    //Copy constructor

    m_pColStaffObjs = pIT->m_pColStaffObjs;
	m_pSO = pIT->m_pSO;
    m_fChangeOfMeasure = pIT->m_fChangeOfMeasure;
    m_fEnd = false;
}

void lmSOIterator::MoveFirst()
{
    // move cursor to first lmStaffObj

	m_pSO = m_pColStaffObjs->GetFirstSO();
    m_fChangeOfMeasure = false;
    m_fEnd = false;
}

void lmSOIterator::MoveNext()
{
    // advance cursor to next lmStaffObj. If already at end of collection remains there
    
    m_fEnd = false;

    //If already at end of collection remains there
    if (!m_pSO)
        return;     //we are at end

	lmStaffObj* pNext = m_pSO->GetNextSO();
    m_fChangeOfMeasure = (pNext ? m_pSO->IsBarline() : true);
    m_pSO = pNext;
}

void lmSOIterator::MovePrev()
{
    //move cursor back to previous lmStaffObj. If already at start of collection
    //remains there but raises EndOfCollection condition

    //if at start, remain there and set EndOfCollection condition
    if (FirstOfCollection())
    {
        m_fEnd = true;
        return;
    }

    //if at end, move to last
    if (!m_pSO)
    {
        //move to last object
        m_pSO = m_pColStaffObjs->GetLastSO();
        m_fChangeOfMeasure = (m_pSO ? m_pSO->IsBarline() : true);
        m_fEnd = false;
        return;     
    }

    //otherwise, move back
    m_pSO = m_pSO->GetPrevSO();
    wxASSERT(m_pSO);
    m_fChangeOfMeasure = m_pSO->IsBarline();
    m_fEnd = false;
}

void lmSOIterator::MoveLast()
{
    // move cursor to last lmStaffObj

	m_pSO = m_pColStaffObjs->GetLastSO();
    m_fChangeOfMeasure = false;
    m_fEnd = false;
}

void lmSOIterator::MoveTo(lmStaffObj* pSO)
{
    //Move cursor to point to received SO

    m_pSO = pSO;
    m_fChangeOfMeasure = false;
    m_fEnd = false;
}

void lmSOIterator::AdvanceToMeasure(int nMeasure)
{
    // advance cursor to first lmStaffObj in measure number nMeasure (1..n)

	m_pSO = m_pColStaffObjs->GetSegment(nMeasure - 1)->GetFirstSO();
    m_fChangeOfMeasure = false;
    m_fEnd = false;
}

