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

/*! @class lmSOIterator
    @ingroup score_kernel
    @brief Encapsulates access and traverse of a collection of StaffObjs

    lmSOIterator encapsulates access and traverse of a collection of StaffObjs (lmColStaffObjs
    object) without exposing the internal structure of the collection. This lets us
    define different traversal algorithms and allows us to change the internal representation
    of a StaffObjs collection without affecting the rest of the program.
*/
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


//========================================================================================
// lmSOIterator implementation
//========================================================================================

lmSOIterator::lmSOIterator(ETraversingOrder nOrder, lmColStaffObjs* pCSO, int nVoice)
{
    // Currently, StaffObjs are stored in a list, always ordered by time.
    // Therefore all three types of ETraversingOrder are the equivalent
	// I will keep parameter ETraversingOrder for documentation purposes about
	// the inded usage, and just in case in future it become necessary.

    m_pColStaffObjs = pCSO;
	m_nVoice = nVoice;
	m_pSegment = (lmSegment*)NULL;

	//move to first staffobj
    MoveFirst();
    m_fEndOfMeasure = false;
}

lmSOIterator::lmSOIterator(ETraversingOrder nOrder, lmColStaffObjs* pCSO, lmStaffObj* pTargetSO)
{
    // Currently, StaffObjs are stored in a list, always ordered by time.
    // Therefore all three types of ETraversingOrder are the equivalent
	// I will keep parameter ETraversingOrder for documentation purposes about
	// the inded usage, and just in case in future it become necessary.

    m_pColStaffObjs = pCSO;
    m_nVoice = (pTargetSO->IsNoteRest() ? ((lmNoteRest*)pTargetSO)->GetVoice() : -1);
	m_pSegment = pTargetSO->GetSegment();
	m_nSegment = m_pSegment->m_nNumSegment;
	m_it = --(m_pSegment->m_StaffObjs.end());

    //find target SO
    while(*m_it != pTargetSO) 
        --m_it;
}

void lmSOIterator::MoveFirst()
{
    // move cursor to first lmStaffObj

	m_nSegment = 0;
	m_pSegment = m_pColStaffObjs->m_Segments[0];
	m_it = m_pSegment->m_StaffObjs.begin();
}

void lmSOIterator::MoveNext()
{
    // advance cursor to next lmStaffObj

    if (EndOfList()) {
        m_fEndOfMeasure = true;
        return;
    }

	++m_it;
	if (m_it == m_pSegment->m_StaffObjs.end())
	{
		//advance to next segment
        m_fEndOfMeasure = true;
		if (++m_nSegment < (int)m_pColStaffObjs->m_Segments.size())
		{
			m_pSegment = m_pColStaffObjs->m_Segments[m_nSegment];
			m_it = m_pSegment->m_StaffObjs.begin();
		}
		//else nothing to do. End of collection reached
	}
    else
        m_fEndOfMeasure = false;
}

void lmSOIterator::MovePrev()
{
    // move cursor back to previous lmStaffObj

	if (StartOfList()) return;

	if (m_it == m_pSegment->m_StaffObjs.begin())
	{
		//currently pointing to first SO of this segment. Move to last SO of previous segment
		m_nSegment--;
		m_pSegment = m_pColStaffObjs->m_Segments[m_nSegment];
		m_it = --(m_pSegment->m_StaffObjs.end());
	}
	else
	{
		//move back
		--m_it;
	}
}

void lmSOIterator::MoveLast()
{
    // move cursor to last lmStaffObj

	m_nSegment = m_pColStaffObjs->m_Segments.size() - 1;
	m_pSegment = m_pColStaffObjs->m_Segments[m_nSegment];
	m_it = m_pSegment->m_StaffObjs.end();
    MovePrev();
}

bool lmSOIterator::EndOfList()
{
	//return true if iterator is at end of score: after last SO in last segment

	return (m_it == m_pSegment->m_StaffObjs.end() 
			&& m_nSegment == (int)m_pColStaffObjs->m_Segments.size() - 1);
}

bool lmSOIterator::StartOfList()
{
    // returns true if cursor points to first item

	return (m_nSegment == 0 && m_it == m_pSegment->m_StaffObjs.begin());
}

bool lmSOIterator::EndOfMeasure()
{
    //returns true if last MoveNext() call crossed a segment boundary
    return m_fEndOfMeasure;
}

void lmSOIterator::AdvanceToMeasure(int nMeasure)
{
    // advance cursor to first lmStaffObj in measure number nMeasure (1..n)

	m_nSegment = nMeasure - 1;
	m_pSegment = m_pColStaffObjs->m_Segments[m_nSegment];
	m_it = m_pSegment->m_StaffObjs.begin();
    m_fEndOfMeasure = (m_it == m_pSegment->m_StaffObjs.end() );
}
