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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "ColStaffObjs.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <algorithm>

#include "wx/debug.h"

#include "Score.h"
#include "UndoRedo.h"
#include "StaffObjIterator.h"
#include "../app/ScoreView.h"
#include "../app/ScoreDoc.h"
#include "../app/MainFrame.h"
#include "../app/TheApp.h"

//cursor positioning requires access to shapes
#include "../graphic/BoxScore.h"
#include "../graphic/BoxPage.h"
#include "../graphic/BoxSystem.h"
#include "../graphic/ShapeStaff.h"

//access to logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;

//binary function predicate. I is used to sort the staffobjs list by time
//returns whether a staffobj SO1 has lower timepos than another staffobj SO2
bool TimeCompare(lmStaffObj* pSO1, lmStaffObj* pSO2)
{
    return IsLowerTime(pSO1->GetTimePos(), pSO2->GetTimePos());
}


//=======================================================================================
// lmVStaffCursor implementation
//
// AWARE
//      It is important to note that cursor time is independent of pointed cursor staffobj.
//      Cursor time points to current time, and cursor staffobj points to the
//      insertion/deletetion point, in the staffobjs collection. Therefore
//
//          - if pSO != NULL, m_rTimepos can be lower than or equal to Time(pSO)
//          - if pSO == NULL  m_rTimepos can be > 0
//
//=======================================================================================

lmVStaffCursor::lmVStaffCursor()
{
	m_nStaff = 1;
	m_nSegment = -1;
	m_pSegment = (lmSegment*)NULL;
	m_rTimepos = 0.0f;
	m_pColStaffObjs = (lmColStaffObjs*)NULL;
    m_pScoreCursor = (lmScoreCursor*)NULL;
}

lmVStaffCursor::lmVStaffCursor(lmVStaffCursor& oVCursor)
{
	m_nStaff = oVCursor.GetNumStaff();
	m_nSegment = oVCursor.GetSegment();
	m_rTimepos = oVCursor.GetTimepos();
	m_pColStaffObjs = (lmColStaffObjs*)NULL;
    m_pScoreCursor = oVCursor.GetScoreCursor();
    m_it = oVCursor.GetCurIt();
	m_pSegment = (lmSegment*)NULL;
}

void lmVStaffCursor::AttachToCollection(lmColStaffObjs* pColStaffObjs, bool fReset)
{
	m_pColStaffObjs = pColStaffObjs;
	m_pColStaffObjs->AttachCursor(this);		//link back

    if (m_nSegment != -1)
        m_pSegment = m_pColStaffObjs->m_Segments[m_nSegment];
    else
	    m_pSegment = (lmSegment*)NULL;

    if (fReset)
	    ResetCursor();
}

void lmVStaffCursor::SetNewCursorState(lmScoreCursor* pSCursor, lmVCursorState* pState)
{
    //restore cursor information from save state info.
    //Preconditions:
    //  - cursor must be already attached to the collection of staffobjs

    wxASSERT(m_pColStaffObjs);
    wxASSERT(pState->nSegment < (int)(m_pColStaffObjs->m_Segments.size()));

    m_pScoreCursor = (lmScoreCursor*)NULL;   //supress visual feedback
    m_nStaff = pState->nStaff;
    m_nSegment = pState->nSegment;
    m_pSegment = m_pColStaffObjs->m_Segments[m_nSegment];
    m_rTimepos = pState->rTimepos;

    //locate position
    m_it = m_pSegment->m_StaffObjs.begin();
    if (pState->pSO)
    {
        while(m_it != m_pSegment->m_StaffObjs.end() && *m_it != pState->pSO)
            ++m_it;
    }
    else
        MoveToTime(m_rTimepos);

    //point to segment and restore ScoreCursor
	m_pSegment = m_pColStaffObjs->m_Segments[m_nSegment];
    m_pScoreCursor = pSCursor;

}

lmVCursorState lmVStaffCursor::GetState() 
{   
    lmVCursorState tState = { m_nStaff, m_nSegment, m_rTimepos, GetStaffObj()};
    return tState;
}

lmVStaff* lmVStaffCursor::GetVStaff()
{
    return m_pSegment->m_pOwner->m_pOwner;
}

lmVStaffCursor* lmVStaffCursor::AttachCursor(lmScoreCursor* pSCursor)
{
    m_pScoreCursor = pSCursor;
    return this;
}

void lmVStaffCursor::DetachCursor()
{
    m_pScoreCursor = (lmScoreCursor*)NULL;
}

void lmVStaffCursor::MoveRight(bool fAlsoChordNotes, bool fIncrementIterator)
{
    //Advance cursor constrained by staff.
    //If flag fAlsoChordNotes is true, stops in every chord note; otherwise, chords are
    //treated as a single object
    //Parameter fIncrementIterator is for reusing this method when an staffobj is deleted

	if (IsAtEnd()) return;      //if at end of collection

    if (fIncrementIterator)
        AdvanceIterator();

    while(true)
    {
	    //if end of segment reached, advance to next segment
	    if (m_it == m_pSegment->m_StaffObjs.end())
	    {
			m_nSegment++;
		    if (m_nSegment < (int)m_pColStaffObjs->m_Segments.size())
		    {
			    m_pSegment = m_pColStaffObjs->m_Segments[m_nSegment];
			    m_it = m_pSegment->m_StaffObjs.begin();
		    }
		    else
			{
				m_nSegment--;
                break;      //end of collection reached
			}
	    }

        //end of collection reached
	    if (m_it == m_pSegment->m_StaffObjs.end()) break;

        //check if staffobj is in current staff; otherwise we have to continue advancing
        //TODO: Deal with objects applicable to all staffs (barlines, KS & TS)
        if ((*m_it)->GetStaffNum() == m_nStaff)
        {
            //it is in current staff.
            //if requested to stop in every chord note, we can finish
            if (fAlsoChordNotes) break;
            //else we have to skip notes in chord except base note
            if (!(*m_it)->IsNoteRest() ||
                (*m_it)->IsNoteRest() && !((lmNoteRest*)(*m_it))->IsRest() &&
                ( !((lmNote*)(*m_it))->IsInChord() || ((lmNote*)(*m_it))->IsBaseOfChord())
               ) break;
        }

        //advance to next staffobj
        ++m_it;
    }

    //update cursor timepos and highlight object for visual feedback
    UpdateTimepos();
    if (m_pScoreCursor)
        m_pScoreCursor->OnCursorObjectChanged();
}

void lmVStaffCursor::AdvanceIterator()
{
    //increment internal iterator m_it. If at end of segment, reposition at
    //start of next segment. If at end of collection, remain there.

	if (m_it == m_pSegment->m_StaffObjs.end())
	{
        //at end segment, advance to next segment
		m_nSegment++;
		if (m_nSegment < (int)m_pColStaffObjs->m_Segments.size())
		{
			m_pSegment = m_pColStaffObjs->m_Segments[m_nSegment];
			m_it = m_pSegment->m_StaffObjs.begin();
		}
		else
			m_nSegment--;   //end of collection reached
	}
    else    //not at end of segment/collection: increment iterator
        ++m_it;
}

void lmVStaffCursor::MoveLeft(bool fAlsoChordNotes)
{
    //Advance cursor constrained by staff.
    //If flag fAlsoChordNotes is true, stops in every chord note; otherwise, chords are
    //treated as a single object

    //AWARE: If you change anything in this method verify if the same change should
    //be applied to method GetPreviousStaffobj()

	if (IsAtBeginning()) return;

	//save current position
	int nSegment = m_nSegment;			//current segment
	lmSegment* pSegment = m_pSegment;   //current segment
	float rTimepos = m_rTimepos;        //timepos
	lmItCSO it = m_it;


    while(true)
    {
        //if at start of segment, move back to last SO of previous segment
	    if (m_it == m_pSegment->m_StaffObjs.begin())
	    {
		    if (m_nSegment > 0)
            {
                --m_nSegment;
		        m_pSegment = m_pColStaffObjs->m_Segments[m_nSegment];
		        m_it = m_pSegment->m_StaffObjs.end();
            }
		    else
            {
                //start of collection reached
                break;
            }
	    }

		//move back
		--m_it;

        //if start of collection reached, finish
	    if (m_it == m_pSegment->m_StaffObjs.end()) break;

        //check if staffobj is in current staff; otherwise we have to continue moving back
        if ((*m_it)->GetStaffNum() == m_nStaff)
        {
            //it is in current staff.
            //if requested to stop in every chord note, we can finish
            if (fAlsoChordNotes) break;
            //else we have to skip notes in chord except base note
            if (!(*m_it)->IsNoteRest() ||
                (*m_it)->IsNoteRest() && !((lmNoteRest*)(*m_it))->IsRest() &&
                ( !((lmNote*)(*m_it))->IsInChord() || ((lmNote*)(*m_it))->IsBaseOfChord())
               ) break;
        }
    }

    //if start of collection reached, previous object was the first one in current staff.
	//Remain there
	if (IsAtBeginning())
	{
		m_nSegment = nSegment;
		m_pSegment = pSegment;
		m_rTimepos = rTimepos;
		m_it = it;
		return;
	}


    //update cursor timepos and highlight object for visual feedback
    UpdateTimepos();
    if (m_pScoreCursor)
        m_pScoreCursor->OnCursorObjectChanged();
}

lmStaffObj* lmVStaffCursor::GetPreviousStaffobj()
{
    //Method to get previous staffobj in current staff and skipping notes in chord.
    //This method does not interfere with cursor position

    //AWARE: If you change anything in this method verify if the same change should
    //be applied to method MoveLeft()

    if (IsAtBeginning()) return (lmStaffObj*)NULL;

	//save current position
	int nSegment = m_nSegment;			//current segment
	lmSegment* pSegment = m_pSegment;   //current segment
	lmItCSO it = m_it;

	//move back
    while(true)
    {
        //if at start of segment, move back to last SO of previous segment
	    if (it == pSegment->m_StaffObjs.begin())
	    {
		    if (nSegment > 0)
            {
                --nSegment;
		        pSegment = m_pColStaffObjs->m_Segments[nSegment];
		        it = pSegment->m_StaffObjs.end();
            }
		    else
            {
                //start of collection reached
                break;
            }
	    }

		//move back
		--it;

        //if start of collection reached, finish
	    if (it == pSegment->m_StaffObjs.end()) break;

        //check if staffobj is in current staff; otherwise we have to continue moving back
        if ((*it)->GetStaffNum() == m_nStaff)
        {
            //it is in current staff.
            //We have to skip notes in chord except base note
            if (!(*it)->IsNoteRest() ||
                (*it)->IsNoteRest() && !((lmNoteRest*)(*it))->IsRest() &&
                ( !((lmNote*)(*it))->IsInChord() || ((lmNote*)(*it))->IsBaseOfChord())
            ) break;
        }
    }

    //return staffobj
	if (IsAtBeginning())
	{
		//if start of collection reached, previous object was the first one in current staff.
		//So no previous staffobj
		return (lmStaffObj*)NULL;
	}
	else if (it != m_pColStaffObjs->m_Segments[nSegment]->m_StaffObjs.end())
	{
		//if pointing to a staffobj, return it
		return *it;
	}
	else
	    return (lmStaffObj*)NULL;
}

void lmVStaffCursor::MoveToTime(float rNewTime)
{
    //Within the limits of current segment,
    //move cursor to first object with time > rNewTime in staff m_nStaff

    m_it = m_pSegment->m_StaffObjs.begin();
    m_rTimepos = rNewTime;

	if (m_it == m_pSegment->m_StaffObjs.end()) return;      //the segment is empty

	while (m_it != m_pSegment->m_StaffObjs.end() && IsLowerTime((*m_it)->GetTimePos(), rNewTime))
        ++m_it;

    if (m_it != m_pSegment->m_StaffObjs.end() && IsEqualTime((*m_it)->GetTimePos(), rNewTime))
    {
        //time found. Advance to last object in current staff
	    while (m_it != m_pSegment->m_StaffObjs.end() && IsEqualTime((*m_it)->GetTimePos(), rNewTime))
        {
            ++m_it;
            while (m_it != m_pSegment->m_StaffObjs.end() && (*m_it)->GetStaffNum() != m_nStaff)
                ++m_it;
        }
    }
    else
    {
        //time does not exist. Cursor is now at first object > rNewTime
        ;
    }

    //provide visual feedback
    if (m_pScoreCursor)
        m_pScoreCursor->OnCursorObjectChanged();
}

bool lmVStaffCursor::IsAtEnd()
{
	//return true if iterator is at end of score: after last SO in last segment

	return (m_it == m_pSegment->m_StaffObjs.end()
			&& m_nSegment == (int)m_pColStaffObjs->m_Segments.size() - 1);
}

bool lmVStaffCursor::IsAtBeginning()
{
	//return true if iterator is at begining of score: pointing to first SO in first segment

	return (m_nSegment == 0 && m_it == m_pSegment->m_StaffObjs.begin());
}

void lmVStaffCursor::ResetCursor()
{
    //Move cursor to first object in first segment. No constarins on staff.
    //No visual feedback provided.
    //If visual feedback needed use MoveToFirst() instead. It also takes staff into account

	m_nStaff = 1;
	m_rTimepos = 0.0f;
	wxASSERT(m_pColStaffObjs->m_Segments.size() > 0);

	//position in first segment
	m_nSegment = 0;
	m_pSegment = m_pColStaffObjs->m_Segments[0];
	m_it = m_pSegment->m_StaffObjs.begin();
    UpdateTimepos();
}

void lmVStaffCursor::MoveToFirst(int nStaff)
{
    //Move cursor to first object in first segment on staff nStaff.
    //Visual feedback provided.
	//If no staff specified (nStaff==0) remains in current staff

	if (nStaff != 0)
		m_nStaff = nStaff;

	m_rTimepos = 0.0f;
	wxASSERT(m_pColStaffObjs->m_Segments.size() > 0);

	//position in first segment
	m_nSegment = 0;
	m_pSegment = m_pColStaffObjs->m_Segments[0];
	m_it = m_pSegment->m_StaffObjs.begin();
    if (m_it != m_pSegment->m_StaffObjs.end() && (*m_it)->GetStaffNum() == m_nStaff)
    {
        //found. Update time and give visual feedback
        UpdateTimepos();
        if (m_pScoreCursor)
            m_pScoreCursor->OnCursorObjectChanged();
    }
    else
        MoveRight();
}

lmStaffObj* lmVStaffCursor::GetStaffObj()
{
	if (m_it != m_pColStaffObjs->m_Segments[m_nSegment]->m_StaffObjs.end())
		return *m_it;
	else
		return (lmStaffObj*)NULL;
}

void lmVStaffCursor::AdvanceToTime(float rTime)
{
    //Cursor is positioned at end of current segment. So iterator can not be advanced more
    //without moving to next segment. If required time is within the limits
    //of segment duration, it just advances timepos.
    //Else, if required time is greater than segment duration, moves to next segment, at
    //the time resulting from discounting segment duration.

    wxASSERT(m_it == m_pSegment->m_StaffObjs.end());

    float rSegmentDuration = m_pSegment->GetDuration();
    if (rTime > rSegmentDuration)
    {
        //advance to next segment
	    m_nSegment++;
	    m_pSegment = m_pColStaffObjs->m_Segments[m_nSegment];
	    m_it = m_pSegment->m_StaffObjs.begin();
        m_rTimepos = rTime - rSegmentDuration;
    }
    else
	    m_rTimepos = rTime;

}

void lmVStaffCursor::AdvanceToNextSegment()
{
    //Cursor is positioned at end of segment. So no need to advance iterator,
    //but just to advance timepos.
    //Advances cursor to time 0 in next segment

    wxASSERT(m_it == m_pSegment->m_StaffObjs.end());

    m_nSegment++;
	m_pSegment = m_pColStaffObjs->m_Segments[m_nSegment];
	m_it = m_pSegment->m_StaffObjs.begin();
	m_rTimepos = 0.0f;
}

void lmVStaffCursor::SkipClefKey(bool fSkipKey)
{
    //First advance after last clef. Then, if fSkipKey, advance after last key

    //ensure that cursor is at start of segment
	m_it = m_pSegment->m_StaffObjs.begin();

    lmItCSO it = m_it;          //it will be used to save the last valid position
    bool fFound = false;

    //locate last clef
	while (m_it != m_pSegment->m_StaffObjs.end())
	{
        if ((*m_it)->IsClef())
        {
            it = m_it;
            fFound = true;
        }
        else if ((*m_it)->IsKeySignature())
            break;
        else if (!IsEqualTime((*m_it)->GetTimePos(), 0.0f))
            break;
        ++m_it;
    }

    //here 'it' points to last clef, if any, or to start of segment
    //if requested, advance to last key
    if (fSkipKey)
    {
        //locate last key
	    while (m_it != m_pSegment->m_StaffObjs.end())
	    {
            if ((*m_it)->IsKeySignature())
            {
                it = m_it;
                fFound = true;
            }
            else if (!IsEqualTime((*m_it)->GetTimePos(), 0.0f))
                break;
            ++m_it;
        }
    }

    //here 'it' is pointing to last clef/key (fFound == true) or to start of collection if 
    //no clef/key (fFound==false)
    //reposition cursor iterator and advance after last clef/key
    m_it = it;
    if (fFound && m_it != m_pSegment->m_StaffObjs.end())
        ++m_it;
}

void lmVStaffCursor::AdvanceToStartOfSegment(int nSegment, int nStaff)
{
    //move cursor to start of segment. Set staff

    wxASSERT(nSegment < (int)(m_pColStaffObjs->m_Segments.size()));

    m_nStaff = nStaff;
    m_nSegment = nSegment;
	m_pSegment = m_pColStaffObjs->m_Segments[m_nSegment];
	m_it = m_pSegment->m_StaffObjs.begin();
	m_rTimepos = 0.0f;

	return;
}

void lmVStaffCursor::AdvanceToStartOfTimepos()
{
    //move cursor to the first object at current timepos. Ignore clefs, key and time signatures

	if (m_it == m_pSegment->m_StaffObjs.begin())
        return;         //cursor is at start of segment. Nothing to do

    lmItCSO itPrev;
    lmItCSO itFinalPos = m_it;
	while (m_it != m_pSegment->m_StaffObjs.begin())
	{
        //get previous object
        itPrev = m_it;
        --itPrev;

        //check if we are at current tiempos
        if ((*itPrev)->GetTimePos() != m_rTimepos)
            break;      //done

        //check if it is a clef, key or time signature
        if (!( (*itPrev)->IsClef() || (*itPrev)->IsKeySignature() || (*itPrev)->IsTimeSignature()) )
            itFinalPos = itPrev;

        //move back
        m_it = itPrev;
    }
    m_it = itFinalPos;
}

void lmVStaffCursor::MoveToSegment(int nSegment, int nStaff, lmUPoint uPos)
{
    //move cursor to nearest object to uPos, constrained to specified segment and staff

    wxASSERT(nSegment < (int)(m_pColStaffObjs->m_Segments.size()));

    m_nStaff = nStaff;
    m_nSegment = nSegment;
	m_pSegment = m_pColStaffObjs->m_Segments[m_nSegment];
	m_it = m_pSegment->m_StaffObjs.begin();
	m_rTimepos = 0.0f;

	//if segment empty finish. We are at start of segment
	if (m_it == m_pSegment->m_StaffObjs.end()) return;      //the segment is empty

    //move cursor to nearest object to uPos, constrained to this segment and staff
	lmLUnits uMinDist = 1000000.0f;	//any too big value
    lmItCSO it = m_pSegment->m_StaffObjs.end();
	while (m_it != m_pSegment->m_StaffObjs.end())
	{
		lmStaffObj* pSO = GetStaffObj();
        if (pSO->GetStaffNum() == m_nStaff)
        {
		    lmLUnits xPos = pSO->GetShape()->GetXLeft();
		    lmLUnits uDist = fabs(uPos.x - xPos);
		    if (uDist < uMinDist)
            {
                it = m_it;
		        uMinDist = uDist;
            }
        }
		++m_it;
	}

	//object found or end of segment reached
	if (it != m_pSegment->m_StaffObjs.end())
        m_it = it;
    UpdateTimepos();
    if (m_pScoreCursor)
        m_pScoreCursor->OnCursorObjectChanged();
}

void lmVStaffCursor::MoveCursorToObject(lmStaffObj* pSO)
{
    //Move cursor to required staffobj
    //Precondition: the staffobj is in current instrument

    m_nStaff = pSO->GetStaffNum();
	m_pSegment = pSO->GetSegment();
    m_nSegment = m_pSegment->m_nNumSegment;
	m_it = m_pSegment->m_StaffObjs.begin();

    //locate the object
    while(m_it != m_pSegment->m_StaffObjs.end() && *m_it != pSO)
        ++m_it;

	//object found 
	wxASSERT(m_it != m_pSegment->m_StaffObjs.end());
    UpdateTimepos();
    if (m_pScoreCursor)
        m_pScoreCursor->OnCursorObjectChanged();
}

lmStaff* lmVStaffCursor::GetCursorStaff()
{
    //return pointer to staff in which cursor is located
    return m_pColStaffObjs->m_pOwner->GetStaff(m_nStaff);
}

int lmVStaffCursor::GetPageNumber()
{
    lmStaffObj* pSO = GetStaffObj();
    if (pSO)
        return pSO->GetPageNumber();
    else
        //TODO
        return 0;
}

lmUPoint lmVStaffCursor::GetCursorPoint()
{
    //compute coordinate for placing cursor and return it

    lmUPoint uPos(0.0f, 0.0f);
    lmStaffObj* pCursorSO = GetStaffObj();

    //variables to contain time and x pos of current, and previous staffobjs. Subindexes:
    //  2 - current staffobj
    //  1 - previous staffobj
    //  3 - current timepos
    lmLUnits uX1, uX2;
    float rT1, rT2, rT3;
    rT3 = m_rTimepos;         //save it, as will be lost when MoveLeft(), etc.

    //
    //collect information about staffobjs and shapes' positions
    //

    //get current staffobj info
    if (pCursorSO)
    {
        //get info from cursor staffobj
        rT2 = pCursorSO->GetTimePos();
        lmShape* pShape2 = pCursorSO->GetShape();
        if (pShape2)
        {
            uPos.y = GetStaffPosY(pCursorSO);
	        //lmBoxSystem* pSystem = pShape2->GetOwnerSystem();
			////GetStaffShape() requires as parameter the staff number, relative to the
			////total number of staves in the system. But we have staff number relative to
			////staves in current instrument. So we have to determine how many instruments
			////there are, and transform staff number.
			//int nRelStaff = pCursorSO->GetStaffNum();
			//int nInstr = m_pScoreCursor->GetCursorInstrumentNumber();
			//if (nInstr > 1)
			//{
			//	lmScore* pScore = m_pScoreCursor->GetCursorScore();
			//	nRelStaff += pScore->GetFirstInstrument()->GetNumStaves();
			//	for (int i=2; i < nInstr; i++)
			//	{
			//		nRelStaff += pScore->GetNextInstrument()->GetNumStaves();
			//	}
			//}
			////here we have the staff number relative to total staves in system
            //uPos.y = pSystem->GetStaffShape(nRelStaff)->GetYTop();
            uX2 = pShape2->GetXLeft() + pShape2->GetWidth()/2.0f;
        }
        else
        {
            //End of score
            //Or no shape! --> //TODO: All objects must have a shape, althought it can be not visible
            wxLogMessage(_T("[lmVStaffCursor::GetCursorPoint] No shape for current sttafobj"));
            uPos.y = 0.0f;  //TODO
            uX2 = 0.0f;     //TODO
        }
    }

    //get info from previous staffobj
    lmStaffObj* pPrevSO = GetPreviousStaffobj();
    if (pPrevSO)
    {
        rT1 = pPrevSO->GetTimePos();
        lmShape* pShape1 = pPrevSO->GetShape();
	    if (!pShape1)
            //NO shape! --> //TODO: All objects must have a shape, althought it can be not visible
            uX1 = 0.0f;     //TODO
	    else
        {
		    uX1 = pShape1->GetXLeft() + pShape1->GetWidth()/2.0f;
        }
    }


    //
    //Compute cursor position based on previously collected information
    //

    if (pCursorSO && pPrevSO)
    {
        //Both staffobjs, previous and current, exist. So curor is between the two staffobjs,
        //or over the sencond one.
        //Decide on positioning, based on cursor time
        if (IsEqualTime(m_rTimepos, pCursorSO->GetTimePos()))
        {
            //Pointing to cursor staffobj. Take positioning information from staffobj
		    uPos.x = uX2;
        }
        else if (IsLowerTime(m_rTimepos, pCursorSO->GetTimePos()))
        {
            //Between current and previous. Interpolate position
            rT1 = pPrevSO->GetTimePos();
            float rTimeIncr = rT2 - rT1;    // At = t2 - t1
            lmLUnits uXIncr = uX2 - uX1;    //Ax = x2-x1
            //At' = t3-t1;   Ax' = x3 - x1 = Ax * (At' / At);   x3 = Ax' + x1
            uPos.x = (uXIncr * ((rT3 - rT1) / rTimeIncr)) + uX1;
        }
        else
            //current cursor time > current staffobj time. Impossible!!
            wxASSERT(false);

        return uPos;
    }

    else if (pCursorSO)
    {
        //No previous staffobj. Current staffobj is the first one and, therefore, cursor 
        //must be on it
        if (IsEqualTime(m_rTimepos, pCursorSO->GetTimePos()))
        {
            //Pointing to cursor staffobj. Take positioning information from staffobj
		    uPos.x = uX2;
        }
        else
            //can not be before the first staffobj
		    uPos.x = uX2;
            //wxASSERT(false);

        return uPos;
    }

    else if (pPrevSO)
    {
        //No current staffobj but previous one exist. Previous one is the last one and 
        //the cursor is at the end of the score.
        //Position cursor four lines (40 tenths) at the right of last staffobj
        uPos.y = GetStaffPosY(pPrevSO);
        uPos.x = uX1 + pPrevSO->TenthsToLogical(40);

        return uPos;
    }

    //No current staffobj and no previous staffobj
    //The score is empty, place cursor at first system of current page (there should be
    //only one page and one system, but let's have the code ready just in case we have
    //many empty pages full of empty systems)

    //Take positioning information from staff position
    lmScore* pScore = m_pColStaffObjs->m_pOwner->GetScore();
    lmBoxScore* pBS = (lmBoxScore*)pScore->GetBox();
    lmBoxPage* pBPage = pBS->GetPage(pBS->GetNumPages());
    lmBoxSystem* pBSystem = pBPage->GetSystem(pBPage->GetFirstSystem());
    uPos.y = pBSystem->GetStaffShape(1)->GetYTop();
    uPos.x = pBSystem->GetXLeft() + pScore->TenthsToLogical(10);

    return uPos;
}

float lmVStaffCursor::GetStaffPosY(lmStaffObj* pSO)
{
    //receives a staffobj and returns the y coordinate of the staff on which this staffobj
    //is placed
    
    lmShape* pShape = pSO->GetShape();
	lmBoxSystem* pSystem = pShape->GetOwnerSystem();
	//GetStaffShape() requires as parameter the staff number, relative to the
	//total number of staves in the system. But we have staff number relative to
	//staves in current instrument. So we have to determine how many instruments
	//there are, and transform staff number.
	int nRelStaff = pSO->GetStaffNum();
	int nInstr = m_pScoreCursor->GetCursorInstrumentNumber();
	if (nInstr > 1)
	{
		lmScore* pScore = m_pScoreCursor->GetCursorScore();
		nRelStaff += pScore->GetFirstInstrument()->GetNumStaves();
		for (int i=2; i < nInstr; i++)
		{
			nRelStaff += pScore->GetNextInstrument()->GetNumStaves();
		}
	}
	//here we have the staff number relative to total staves in system
    return pSystem->GetStaffShape(nRelStaff)->GetYTop();
}

void lmVStaffCursor::UpdateTimepos()
{
    if (m_it != m_pSegment->m_StaffObjs.end())
        m_rTimepos = (*m_it)->GetTimePos();
}

void lmVStaffCursor::OnCursorObjectDeleted(lmItCSO itNext)
{
    //the object pointed by the cursor has been deleted. Inform the view and reposition cursor
    //Receives an iterator, pointing to next position after deleted object

    //inform the ScoreCursor
    if (m_pScoreCursor)
        m_pScoreCursor->OnCursorObjectDeleted();

    //reposition cursor at next object
	m_it = itNext;
    MoveRight(true, false);     //stop in all chord notes, do not increment iterator
}



//====================================================================================================
// lmSegment implementation
//====================================================================================================

lmSegment::lmSegment(lmColStaffObjs* pOwner, int nSegment)
{
    m_pOwner = pOwner;
    m_nNumSegment = nSegment;
	for (int i=0; i < lmMAX_STAFF; i++)
		m_pContext[i] = (lmContext*)NULL;

    m_bVoices = 0x00;
    m_rMaxTime = 0.0f;
}

lmSegment::~lmSegment()
{
	//delete staffobjs collection
	lmItCSO itSO;
	for (itSO = m_StaffObjs.begin(); itSO != m_StaffObjs.end(); itSO++)
	{
		delete *itSO;
	}
    m_StaffObjs.clear();
}

int lmSegment::GetNumVoices()
{
    int nNumVoices=0;
    for (int i=0; i < lmMAX_VOICE; i++)
    {
        if (IsVoiceUsed(i))
            nNumVoices++;
    }
    return nNumVoices;
}

bool lmSegment::IsVoiceUsed(int nVoice)
{
    int bMask = 1 << (nVoice-1);
    return (m_bVoices & bMask) != 0;
}

void lmSegment::VoiceUsed(int nVoice)
{
    int bMask = 1 << (nVoice-1);
    m_bVoices |= bMask;
}


void lmSegment::Store(lmStaffObj* pNewSO, lmVStaffCursor* pCursor)
{
    //Store the received StaffObj in this segment, before object pointed by cursor.

    //get object pointed by cursor
    lmStaffObj* pCursorSO = pCursor->GetStaffObj();

    //add staffobj to the collection
    lmItCSO itNewCSO;			//iterator to point to the new inserted object
    if (pCursorSO)
    {
        //Insert pNewSO before pCursorSO
        itNewCSO = pCursor->GetCurIt();
        m_StaffObjs.insert(itNewCSO, pNewSO);	//insert pNewSO before item pointed by cursor
	    itNewCSO--;		                        //now points to the new inserted object
    }
    else
    {
        //push_back pNewSO in the collection
        m_StaffObjs.push_back(pNewSO);		//insert at the end
	    itNewCSO = --m_StaffObjs.end();		//point to inserted object
    }

    //Update pNewSO information (the pointer to this segment)
    pNewSO->SetSegment(this);

    //Update voices used
    if (pNewSO->IsNoteRest())
        VoiceUsed(((lmNoteRest*)pNewSO)->GetVoice());
    else
        VoiceUsed(0);

	//check if added object is a note added to an existing chord
	bool fAddedToChord = false;
	if (pNewSO->IsNoteRest() && !((lmNote*)pNewSO)->IsRest() && ((lmNote*)pNewSO)->IsInChord())
	{
		//added note is part of a chord. Let's check if it is the first note or there
		//are already more notes in the chord
		lmNote* pNote = (lmNote*)pNewSO;
		fAddedToChord = pNote->GetChord()->NumNotes() > 1;
	}
	
	//if not added to chord update segment's duration and shift timepos of 
	//all objects in this voice after the inserted one
	if (!fAddedToChord)
	{
		//update segment's duration
		float rTime = pNewSO->GetTimePos() + pNewSO->GetTimePosIncrement();
		m_rMaxTime = wxMax(m_rMaxTime, rTime);

		//shift timepos of all objects in this voice after the inserted one
		if (pCursorSO && pNewSO->IsNoteRest())
		{
			ShiftRightTimepos(itNewCSO, ((lmNoteRest*)pNewSO)->GetVoice());
			//and sort the collection by timepos
				////DBG
				//wxLogMessage(_T("[lmSegment::Store] Before sort"));
				//wxLogMessage(Dump());
			m_StaffObjs.sort(TimeCompare);
				////DBG
				//wxLogMessage(_T("[lmSegment::Store] After insertion of %s. Sort performed"),
				//			 pNewSO->GetName());
				//wxLogMessage(Dump());
		}
	}
	////DBG  ------------------------------------------------------------------
	//wxLogMessage(_T("[lmSegment::Store] After insertion of %s"),
	//			pNewSO->GetName());
	//wxLogMessage(Dump());
	////END DBG ---------------------------------------------------------------
}

void lmSegment::ShiftRightTimepos(lmItCSO itStart, int nVoice)
{
    //The note/rest pointed by itStart has been inserted. If it is a note/rest and this timepos is
    //ocuppied by other note/rest in the same voice, we must shift the timepos af all
    //consecutive objects in this voice, starting with next SO and up to the barline
    //(but excluding it).

    //Algorithm:
    //  Parameters: pStartSO (inserted object)
    //
    //    1 Asign: TimeIncr = Duration(pStartSO)
    //             iV = Voice(pStartSO)
    //             pCurrentSO = take next object in voice iV after pStartSO
    //
    //    2 While not end of segment
    //        - Assign: t = Timepos(pCurrentSO)
    //                  d = Duration(pCurrentSO)
    //        - if Interval(t, t+d) is occupied by object in voice iV:
    //            - Assign: Timepos(pCurrentSO) += TimeIncr
    //        else
    //            - break
    //        - Assign: pCurrentSO = take next object in voice iV after pCurrentSO

    int iV = ((lmNoteRest*)(*itStart))->GetVoice();
    float rTimeIncr = ((lmNoteRest*)(*itStart))->GetTimePosIncrement();
	lmItCSO it = itStart;
    ++it;
	while (it != m_StaffObjs.end())
	{
        if ((*it)->IsNoteRest() && ((lmNoteRest*)(*it))->GetVoice() == iV)
        {
            float rTime = (*it)->GetTimePos();
            float rDuration = (*it)->GetTimePosIncrement();
            if (m_pOwner->IsTimePosOccupied(this, rTime, rDuration, iV))
            {
                (*it)->SetTimePos(rTime + rTimeIncr);
            }
            else
                break;
        }
        ++it;
	}


    //check segment's duration
	//TODO

    //float rTime = pNewSO->GetTimePos() + pNewSO->GetTimePosIncrement();
    //m_rMaxTime = wxMax(m_rMaxTime, rTime);

}

void lmSegment::ShiftLeftTimepos(lmNoteRest* pSO, lmItCSO itStart)
{
    //The note/rest pSO, that was just before the object pointed by itStart, has been deleted.
    //We must shift left the timepos af all consecutive objects in this voice, starting 
    //with the SO pointed by itStart and up to the barline (but excluding it).

    //Algorithm:
    //  Parameters: pSO (deleted object)
    //              pStartSO (next object after it)
    //
    //    1 Asign: TimeIncr = Duration(pSO)
    //             NextTime = Timepos(pSO) + TimeIncr
    //             iV = Voice(pSO)
    //             pCurrentSO = take next object in voice iV starting with pStartSO
    //
    //    2 While not end of segment
    //        - Assign: t = Timepos(pCurrentSO)
    //        - if t == NextTime:
    //            - Assign: Timepos(pCurrentSO) = t - TimeIncr
    //                      NextTime = t + Duration(pCurrentSO)
    //        else
    //            - break
    //        - Assign: pCurrentSO = take next object in voice iV after pCurrentSO

	float rTimeIncr = pSO->GetTimePosIncrement();
    float rNextTime = pSO->GetTimePos() + rTimeIncr;
    int iV = pSO->GetVoice();
	lmItCSO it = itStart;

	while (it != m_StaffObjs.end())
	{
        if ((*it)->IsNoteRest() && ((lmNoteRest*)(*it))->GetVoice() == iV)
        {
            float rTime = (*it)->GetTimePos();
            if (IsEqualTime(rTime, rNextTime))
            {
                (*it)->SetTimePos(rTime - rTimeIncr);
                rNextTime = rTime + (*it)->GetTimePosIncrement();
            }
            else
                break;
        }
        ++it;
	}

    //check segment's duration
	//TODO

    //float rTime = pNewSO->GetTimePos() + pNewSO->GetTimePosIncrement();
    //m_rMaxTime = wxMax(m_rMaxTime, rTime);

}

lmBarline* lmSegment::GetBarline()
{
    //returns the barline of the segment, if exists, or null
    //The barline is always the last staffobj in the segment's collection

	lmItCSO it = m_StaffObjs.end();
    --it;
    if (it != m_StaffObjs.end() && (*it)->IsBarline())
        return (lmBarline*)(*it);
    else
        return (lmBarline*)NULL;
}

void lmSegment::UpdateDuration()
{
	lmItCSO it = m_StaffObjs.end();
    --it;
    wxASSERT(it != m_StaffObjs.end());
    m_rMaxTime = (*it)->GetTimePos() + (*it)->GetTimePosIncrement();
}

wxString lmSegment::Dump()
{
    wxString sDump = wxString::Format(_T("\nSegment %d\n"), m_nNumSegment+1);
    for (int i=0; i < lmMAX_STAFF; i++)
    {
        if (m_pContext[i]) {
            sDump += m_pContext[i]->Dump();
        }
        else
            sDump += _T("Context: NULL\n");
    }
    sDump += _T("\n");

	lmItCSO itSO;
	for (itSO = m_StaffObjs.begin(); itSO != m_StaffObjs.end(); itSO++)
	{
		sDump += (*itSO)->Dump();
	}
	return sDump;
}

//====================================================================================================
// lmColStaffObjs implementation
//====================================================================================================

lmColStaffObjs::lmColStaffObjs(lmVStaff* pOwner, int nNumStaves)
{
	//AWARE: lmColStaffObjs constructor is invoked from lmVStaff constructor, before the
	//lmVStaff is ready. Therefore, you can not invoke lmVStaff methods from here. All
	//required VStaff info must be passed in the constructor parameters list.

	m_pOwner = pOwner;
	m_nNumStaves = nNumStaves;

	//create a first segment
	CreateNewSegment(0);
}

lmColStaffObjs::~lmColStaffObjs()
{
	//delete segments
	std::vector<lmSegment*>::iterator itS;
	for (itS = m_Segments.begin(); itS != m_Segments.end(); itS++)
	{
		delete *itS;
    }
	m_Segments.clear();
}

void lmColStaffObjs::AddStaff()
{
    //collection must be empty
    wxASSERT((int)m_Segments.size()==1 && m_Segments[0]->m_StaffObjs.size() == 0);

    m_nNumStaves++;
}

void lmColStaffObjs::AssignVoice(lmStaffObj* pSO, int nSegment)
{
    //assigns voice (1..n) to the staffobj

    // if it is not a note/rest, return: only note/rests have voice
    if (!pSO->IsNoteRest()) return;

    //if voice already assigned, return: nothing to do
    if (((lmNoteRest*)pSO)->GetVoice() != 0) return;

    //if user has choosen a voice, assign it
    int iV = GetMainFrame()->GetSelectedVoice();
    if (iV == 0)
    {
        //No voice selected. Auto-voice algorithm:
        // we have to assign it a voice. Let's determine if there are note/rests
        // in timepos range [timepos, timepos+object_duration)

        // First, lets check if default staff voice is suitable
        iV = pSO->GetStaffNum();
        bool fOccupied = IsTimePosOccupied(m_Segments[nSegment], pSO->GetTimePos(),
                                        pSO->GetTimePosIncrement(), iV);

        // else, loop to find empty voice in this timepos range
        int nNumVoices = m_Segments[nSegment]->GetNumVoices() - 1;  //AWARE: substract 1 because voice #0 is counted in GetNumVoices()
        for(iV=1; fOccupied && iV <= nNumVoices; iV++)
        {
            fOccupied = IsTimePosOccupied(m_Segments[nSegment], pSO->GetTimePos(),
                                        pSO->GetTimePosIncrement(), iV);
        }

        // if no empty voice found, start a new voice.
        if (fOccupied)
            iV = ++nNumVoices;
    }

    // assign the voice
    ((lmNoteRest*)pSO)->SetVoice(iV);
}

bool lmColStaffObjs::IsTimePosOccupied(lmSegment* pSegment, float rTime, float rDuration,
                                       int nVoice)
{
    //returns true if there is any note/rest in voice nVoice sounding in semi-open interval
    //[timepos, timepos+duration), in current measure.

	lmItCSO it = pSegment->m_StaffObjs.begin();

    //Is time occupied if
    //  coincides:      pSO(timepos) == rTime or
    //  start before:   pSO(timepos) < rTime && pSO(timepos+duration) > rTime
    //  start after:    pSO(timepos) > rTime && pSO(timepos) < (rTime+rDuration)
    while(it != pSegment->m_StaffObjs.end())
    {
        if ((*it)->IsNoteRest() && ((lmNoteRest*)(*it))->GetVoice() == nVoice)
        {
            float rTimeSO = (*it)->GetTimePos();
            if (IsEqualTime(rTimeSO, rTime)) return true;      //start coincides

            float rDurSO = (*it)->GetTimePosIncrement();
            if (rTimeSO < rTime && (rTimeSO + rDurSO - 0.1f) > rTime) return true;

            if (rTimeSO > rTime && rTimeSO < (rTime + rDuration)) return true;

            if (rTimeSO > rTime && (rTimeSO + rDurSO) > rTime) return false;

        }
        ++it;
    }

    return false;
}

void lmColStaffObjs::AssignTime(lmStaffObj* pSO)
{
	// Assigns a timepos to the StaffObj

	//if it is a note in chord and not base note assign it the time assigned to
	//base note
	bool fPartOfChord = false;		//to avoid having to check this many times
    if (pSO->GetClass() == eSFOT_NoteRest && !(((lmNoteRest*)pSO)->IsRest())
       && ((lmNote*)pSO)->IsInChord() && !((lmNote*)pSO)->IsBaseOfChord() )
	{
        lmNote* pNoteBase = ((lmNote*)pSO)->GetChord()->GetBaseNote();
        pSO->SetTimePos(pNoteBase->GetTimePos());
		fPartOfChord = true;
	}

	//else, assign it cursor timepos
	else
		pSO->SetTimePos( m_pVCursor->GetTimepos() );
}


//====================================================================================================
//Methods to add and remove StaffObjs
// - For new objects. Assign the received StaffObj a voice and timepos. Advance cursor  
//   after adding the object:
//		- Add: store before object pointed by current cursor.
//
// - Private. For internal management. Just store the object.
//		- Store: store before object pointed by current cursor.
//====================================================================================================

void lmColStaffObjs::Add(lmStaffObj* pNewSO)
{
    //Assign voice and timepos to the received StaffObj, and store it in the collection,
	//before object pointed by current cursor, and advance cursor after inserted object.
    //Manage all details to maintain segments.

	//get segment
	int nSegment = m_pVCursor->GetSegment();

    //assign timepos and voice to the StaffObj
	AssignTime(pNewSO);
	AssignVoice(pNewSO, nSegment);

	//store new object in the collection
	Store(pNewSO);

    //advance cursor
	lmStaffObj* pCursorSO = m_pVCursor->GetStaffObj();
    if (!pCursorSO)
    {
        //Cursor is pointing to end of collection. So no need to advance cursor iterator,
        //just to advance time
        if (pNewSO->IsBarline())
        {
            //Advance cursor to time 0 in next segment
            if ( ((lmBarline*)pNewSO)->GetBarlineType() != lm_eBarlineEOS )
                m_pVCursor->AdvanceToNextSegment();
        }
        else
            //Advance cursor to time t + duration of inserted object
            m_pVCursor->AdvanceToTime(pNewSO->GetTimePos() + pNewSO->GetTimePosIncrement() );
    }
    else
    {
        if (pNewSO->IsBarline())
        {
            //Cursor is pointing to object after the inserted barline. So  it points
            //to the right object. But other cursor info (segment, time,..) is wrong
            //Update cursor info
            m_pVCursor->MoveCursorToObject( pCursorSO );
        }
        else
            //Cursor is pointing to object after the inserted one but could be of different
            //voice. And cursor timepos is not updated as the collection has been altered.
            //So we have to reposition cursor at right time and right object.
            m_pVCursor->MoveToTime( pNewSO->GetTimePos() + pNewSO->GetTimePosIncrement() );
    }

}

void lmColStaffObjs::Insert(lmStaffObj* pNewSO, lmStaffObj* pBeforeSO)
{
    //set cursor
    m_pVCursor->MoveCursorToObject(pBeforeSO);

    //insert at specified point
    Store(pNewSO);
}

void lmColStaffObjs::Store(lmStaffObj* pNewSO)
{
	//add pNewSO into the segment's collection
	int nSegment = m_pVCursor->GetSegment();
	m_Segments[nSegment]->Store(pNewSO, m_pVCursor);

	//manage segments split/creation
    //AWARE: inserting new elements into a vector can cause vector reallocation.
    //All currently in use segment iterators could be invalidated after SplitSegment()
    //or CreateNewSegment().
	lmStaffObj* pCursorSO = m_pVCursor->GetStaffObj();
    if (pNewSO->IsBarline())
    {
        if (pCursorSO)
        {	//split current segment
            SplitSegment(nSegment, pNewSO);
        }
        else
        {	//create a new segment
            if (((lmBarline*)pNewSO)->GetBarlineType() != lm_eBarlineEOS)
                CreateNewSegment(nSegment);
        }
    }
}

void lmColStaffObjs::Delete(lmStaffObj* pSO, bool fDelete)
{
	//Delete the StaffObj pointed by pSO

    //Algorithm:
    //  - get segment and find object to remove
    //  - get all needed information from object to delete, before doing it
    //  - remove the staffobj from the colection
    //  - if removed object was a note not in chord:
    //      Shift left all note/rests in this voice and sort the collection
    //  - if it was a barline:
    //      merge current segment with next one
    //      Adjust timepos of all moved objects
    //      Update segment pointer in all moved objects
    //      remove next segment and renumber segments
    //  - delete the removed staffobj


    //the EOS barline can not be deleted
	if (pSO->IsBarline() && ((lmBarline*)pSO)->GetBarlineType() == lm_eBarlineEOS) return;

    //get segment and find object to remove
    lmSegment* pSegment = pSO->GetSegment();
	lmItCSO itNext = find(pSegment->m_StaffObjs.begin(), pSegment->m_StaffObjs.end(), pSO);
    wxASSERT(itNext != pSegment->m_StaffObjs.end());

	//remove the staffobj from the collection. After removal, iterator 'itNext' is invalidated,
    //so it is necesary to advance it before erase invocation, to leave it pointing
    //to next object
	pSegment->m_StaffObjs.erase(itNext++);

    //if removed object was a note not in chord:
    //  - Shift left all note/rests in this voice and sort the collection
	if (pSO->IsNoteRest() && !((lmNoteRest*)pSO)->IsRest() && !((lmNote*)pSO)->IsInChord())
    {
        pSegment->ShiftLeftTimepos((lmNoteRest*)pSO, itNext);
		pSegment->m_StaffObjs.sort(TimeCompare);
    }

    //if removed object was a barline:
    //  - merge current segment with next one
    //  - Adjust timepos of all moved objects
    //  - Update segment pointer in all moved objects
    //  - remove next segment and renumber segments
	if (pSO->IsBarline())
    {
        //merge current segment with next one
	    //As we are dealing with lists this is just to cut and append the lists (this
        //is done with an 'splice' operation in STL)
        lmSegment* pNextSegment = m_Segments[pSegment->m_nNumSegment + 1];
	    lmItCSO it = pNextSegment->m_StaffObjs.begin();    //point to first object to move
        lmStaffObj* pFirstSO = *it;                        //first object after removed barline
        lmItCSO itLast = (pSegment->m_StaffObjs).end();    //save ptr to last object
        --itLast;
        (pSegment->m_StaffObjs).splice(pSegment->m_StaffObjs.end(),
                    pNextSegment->m_StaffObjs, it, pNextSegment->m_StaffObjs.end() );

        //Adjust timepos and update segment pointer in all moved objects
	    //Re-assign time to all objects in new segment.
        //As objects ordering will not change this is just udating all staffobjs timepos, by
	    //adding timepos of deleted barline
        float rTime = pSO->GetTimePos();
        for(it = ++itLast; it != pSegment->m_StaffObjs.end(); ++it)
        {
            (*it)->SetTimePos( (*it)->GetTimePos() + rTime );
            (*it)->SetSegment(pSegment);
        }

        //re-compute segment duration
        pSegment->UpdateDuration();

        //remove next segment and renumber segments
        RemoveSegment( pNextSegment->m_nNumSegment );

        //iterator itNext is invalid. As removed object was a barline, itNext was pointing
        //to end of segment instaed to next object. We have to reposition it to point to next
        //object
	    itNext = find(pSegment->m_StaffObjs.begin(), pSegment->m_StaffObjs.end(), pFirstSO);
    }

    //delete the removed staffobj
    if (fDelete) delete pSO;
    m_pVCursor->OnCursorObjectDeleted(itNext);

    //wxLogMessage(_T("[lmColStaffObjs::Delete] Forcing a dump:");
    //wxLogMessage( Dump() );
    //#if defined(__WXDEBUG__)
    //g_pLogger->LogTrace(_T("lmColStaffObjs::Delete"), Dump() );
    //#endif
}

void lmColStaffObjs::LogPosition(lmUndoData* pUndoData, lmStaffObj* pSO)
{
    //log info to locate insertion point

    //get segment and find object
    lmSegment* pSegment = pSO->GetSegment();
	lmItCSO it = find(pSegment->m_StaffObjs.begin(), pSegment->m_StaffObjs.end(), pSO);
    wxASSERT(it != pSegment->m_StaffObjs.end());

    ++it;
    pUndoData->AddParam<lmStaffObj*>(*it);
}


//=================================================================================
//Methods for accesing specific items and information about the collection
//=================================================================================

int lmColStaffObjs::GetNumMeasures()
{
    //returns the number of measures in the collection
    return m_Segments.size();
}

int lmColStaffObjs::GetNumVoicesInMeasure(int nMeasure)
{
    wxASSERT(nMeasure <= (int)m_Segments.size());
    return m_Segments[nMeasure-1]->GetNumVoices();
}

bool lmColStaffObjs::IsVoiceUsedInMeasure(int nVoice, int nMeasure)
{
    wxASSERT(nMeasure <= (int)m_Segments.size());
    return m_Segments[nMeasure-1]->IsVoiceUsed(nVoice);
}

float lmColStaffObjs::GetMeasureDuration(int nMeasure)
{
    wxASSERT(nMeasure <= (int)m_Segments.size());
    return m_Segments[nMeasure-1]->GetDuration();
}

lmBarline* lmColStaffObjs::GetBarlineOfMeasure(int nMeasure)
{
    //return the barline in segment nMeasure (1..n)

    wxASSERT(nMeasure <= (int)m_Segments.size());
    lmSegment* pSegment = m_Segments[nMeasure-1];
    return pSegment->GetBarline();
}

lmBarline* lmColStaffObjs::GetBarlineOfLastNonEmptyMeasure()
{
    //return the barline in last non-empty segment

    int i = (int)m_Segments.size() - 1;
    while (i >=0 && m_Segments[i]->IsEmpty())
        i--;

    if (i >= 0)
        return m_Segments[i]->GetBarline();
    else
        return (lmBarline*)NULL;
}



//====================================================================================================
// Debug methods
//====================================================================================================

wxString lmColStaffObjs::Dump()
{
    wxString sDump = wxString::Format(_T("Num.segments = %d"), m_Segments.size());

	//dump segments
	std::vector<lmSegment*>::iterator itS;
	for (itS = m_Segments.begin(); itS != m_Segments.end(); itS++)
	{
		sDump += (*itS)->Dump();
    }

    return sDump;
}





//====================================================================================================
// Traversing the staffobjs collection: iterators
//====================================================================================================

lmSOIterator* lmColStaffObjs::CreateIterator(ETraversingOrder nOrder, int nVoice)
{
    //creates and returns an iterator pointing to the first staffobj
	return new lmSOIterator(nOrder, this, nVoice);
}

lmSOIterator* lmColStaffObjs::CreateIteratorTo(ETraversingOrder nOrder, lmStaffObj* pSO)
{
    //creates and returns an iterator pointing to staffobj pSO

    lmSOIterator* pIter = new lmSOIterator(nOrder, this, pSO);
    return pIter;
}



//====================================================================================================
// Segments' collection management
//====================================================================================================

void lmColStaffObjs::SplitSegment(int nSegment, lmStaffObj* pLastSO)
{
	//Segment nSegment is splitted into two segments after object pLastSO.
    //That is, first segment will keep all objects up to (inluded) pLastSO and
    //new segment will include all objects after pLastSO.
    //Timepos of this second segment objects are re-adjusted to start in 0
	//Segments collection is renumbered

    //AWARE:
    //inserting new elements into a vector (either at the end, with 'push_back', or
    //in the middle, with 'insert', can cause vector reallocation. And this will invalidate
    //all currently in use iterators. But not segment pointers stored in the staffobjs as
    //m_Segments is a vector of pointer to segments, created with 'new' operator.

    CreateNewSegment(nSegment);
    lmSegment* pOldSegment = m_Segments[nSegment];
    lmSegment* pNewSegment = m_Segments[nSegment+1];

    //wxLogMessage(_T("[lmColStaffObjs::SplitSegment] Before splitting:"));
    //wxLogMessage(pOldSegment->Dump());

    //find last staffobj and move to new segment all staffobjs after it
	//As we are dealing with lists this is just to cut first list and
	//append the cutted part to the new list (splice operation in STL)
	lmItCSO it = find((pOldSegment->m_StaffObjs).begin(), (pOldSegment->m_StaffObjs).end(),
                      pLastSO);
    ++it;   //point to first object to move
    wxASSERT(it != (pOldSegment->m_StaffObjs).end());
    (pNewSegment->m_StaffObjs).splice((pNewSegment->m_StaffObjs).begin(),
                pOldSegment->m_StaffObjs, it, (pOldSegment->m_StaffObjs).end() );

	//Re-assign time to all objects in new segment, and update segment pointer.
    //As ordering will not change this is just udating all staffobjs timepos, by
	//substracting timepos of pLastSO
    float rTime = pLastSO->GetTimePos();
    for(it = (pNewSegment->m_StaffObjs).begin(); it != (pNewSegment->m_StaffObjs).end(); ++it)
    {
        (*it)->SetTimePos( (*it)->GetTimePos() - rTime );
        (*it)->SetSegment(pNewSegment);
    }

    //re-compute segment duration for both segments
    pOldSegment->UpdateDuration();
    pNewSegment->UpdateDuration();

	//Update contexts in new segment
    UpdateContexts(pNewSegment);

    //wxLogMessage(_T("[lmColStaffObjs::SplitSegment] After splitting:"));
    //wxLogMessage(pOldSegment->Dump());
    //wxLogMessage(pNewSegment->Dump());

}

void lmColStaffObjs::CreateNewSegment(int nSegment)
{
	//Add a new segment after segment number nSegment. If nSegment is the last segment,
	//the new created segment is added at the end; otherwise, the new segment is
	//inserted after segment nSegment and all the segments are renumbered.

    //AWARE:
    //inserting new elements into a vector (either at the end, with 'push_back', or
    //in the middle, with 'insert', can cause vector reallocation. And this will invalidate
    //all currently in use iterators. But not segment pointers stored in the staffobjs as
    //m_Segments is a vector of pointer to segments, created with 'new' operator.


    //create the segment
    lmSegment* pS = new lmSegment(this, (m_Segments.size() == 0 ? 0 : nSegment+1));

	if ((int)m_Segments.size() == nSegment)
    {
		//the new segment is added at the end
	    m_Segments.push_back(pS);
    }
	else
    {
	    //insert the new segment after segment nSegment
	    std::vector<lmSegment*>::iterator itS;
        itS = find(m_Segments.begin(), m_Segments.end(), m_Segments[nSegment+1]);
        itS = m_Segments.insert(itS, pS);         //insert before itS

        //renumber all following segments
        for (++itS; itS != m_Segments.end(); ++itS)
            (*itS)->m_nNumSegment++;
    }

    //Set contexts at start of this new segment.
    UpdateContexts(pS);
}

void lmColStaffObjs::RemoveSegment(int nSegment)
{
	//Remove segment nSegment (0..n) and renumber all segments

	//remove the segment
	std::vector<lmSegment*>::iterator itS, itNext;
    itS = find(m_Segments.begin(), m_Segments.end(), m_Segments[nSegment]);
    m_Segments.erase(itS);

    //renumber all following segments
    for (int i=nSegment; i < (int)m_Segments.size(); i++)
        m_Segments[i]->m_nNumSegment--;
}

void lmColStaffObjs::UpdateContexts(lmSegment* pSegment)
{
    //When add/inserting a new segment it is necessary to find current contexts at
	//start of that segment and to store them in the segment. This method
    //does it. nNewSegment (0..n-1) is the number of the segment to update.

	int nNewSegment = pSegment->m_nNumSegment;

    //first segment never has contexts (as it is the first one, no StaffObjs yet)
    if (nNewSegment == 0)
    {
        for (int iStaff=0; iStaff < m_nNumStaves; iStaff++)
	        pSegment->SetContext(iStaff, (lmContext*)NULL);
        return;
    }


	//AWARE: Following code only executes if segment > 0


    //initialize contexts with contexts at start of previous segment
	lmSegment* pPrevSegment = m_Segments[nNewSegment-1];
    bool fStaffDone[lmMAX_STAFF];
    for (int iS=0; iS < m_nNumStaves; iS++)
	{
        fStaffDone[iS] = false;
		pSegment->SetContext(iS, pPrevSegment->GetContext(iS));
	}

    //get last StaffObj of previous segment and start backwards iterator
	lmItCSO it = --(pPrevSegment->m_StaffObjs.end());
    bool fDone = false;
    while(!fDone && it != pPrevSegment->m_StaffObjs.end())
	{
        lmContext* pCT = (lmContext*)NULL;
        lmStaffObj* pSO = *it;
        if (pSO->GetClass() == eSFOT_Clef)
        {
            int nStaff = pSO->GetStaffNum();
            if (!fStaffDone[nStaff]-1)
            {
                pCT = ((lmClef*)pSO)->GetContext();
	            pSegment->SetContext(nStaff-1, pCT);
                fStaffDone[nStaff-1] = true;
            }
        }

        if (!pCT)
        {
		    if (pSO->GetClass() == eSFOT_KeySignature
                || pSO->GetClass() == eSFOT_TimeSignature)
            {
                for (int nStaff=1; nStaff <= m_nNumStaves; nStaff++)
                {
                    if (!fStaffDone[nStaff-1])
                    {
		                if (pSO->GetClass() == eSFOT_KeySignature)
                            pCT = ((lmKeySignature*)pSO)->GetContext(nStaff);
		                else // eSFOT_TimeSignature)
                            pCT = ((lmTimeSignature*)pSO)->GetContext(nStaff);
	                    pSegment->SetContext(nStaff-1, pCT);
                        fStaffDone[nStaff-1] = true;
                    }
                }
                break;  //fast finish
            }
        }

        //check if we can finish
		if(it == pPrevSegment->m_StaffObjs.begin())
			break;
		else
		{
			--it;
			fDone = true;
			for (int iS=0; iS < m_nNumStaves; iS++)
				fDone &= fStaffDone[iS];
		}
    }
}





//====================================================================================================
//Other methods
//====================================================================================================

bool lmColStaffObjs::ShiftTime(float rTimeShift)
{
    // This method deals with a source LDP command (or XML commnd) to shift time counters,
    // forward or backwards. What it does is to reposition cursor at required time.
    // returns true if error

    bool fError = false;

    //compute new time
    float rNewTime = m_pVCursor->GetTimepos() + rTimeShift;

    //check that new time is in current measure boundaries
    float rMaxTime = m_Segments[m_pVCursor->GetSegment()]->GetDuration();
    if (IsLowerTime(rNewTime, 0.0f))
    {
        //Move backwards: out of measure
        if (!IsEqualTime(-rTimeShift, lmTIME_SHIFT_START_END))
        {
            m_pOwner->SetError(_("Move backwards: out of measure"));
            fError = true;
        }
        rNewTime = 0.0f;
    }
    else if (IsHigherTime(rTimeShift, rMaxTime))
    {
        //Move forward: out of measure
        if (!IsEqualTime(rTimeShift, lmTIME_SHIFT_START_END))
        {
            m_pOwner->SetError(_("Move forward: out of measure"));
            fError = true;
        }
        rNewTime = rMaxTime;
    }

    //move cursor to required time
    m_pVCursor->MoveToTime(rNewTime);
    return fError;
}


//====================================================================================================
// lmColStaffObjs: contexts management
//====================================================================================================

lmContext* lmColStaffObjs::GetCurrentContext(lmStaffObj* pTargetSO)
{
	// Returns the context that is applicable to the received StaffObj.
	// AWARE: This method does not return a context with updated accidentals;
    // the returned context is valid only for clef, key signature and time signature.
    // To get applicable accidentals use NewUpdatedContext() instead.

    //get start of segment context
	lmSegment* pSegment = pTargetSO->GetSegment();
	lmContext* pCT = pSegment->GetContext(pTargetSO->GetStaffNum() - 1);

    //look for any clef, KS or TS changing the context
    int nStaff = pTargetSO->GetStaffNum();
	lmItCSO it = pSegment->m_StaffObjs.begin();
    while(it != pSegment->m_StaffObjs.end() && *it != pTargetSO)
	{
        lmStaffObj* pSO = *it;
		if (pSO->GetClass() == eSFOT_Clef && nStaff == pSO->GetStaffNum())
            pCT = ((lmClef*)pSO)->GetContext();
		else if (pSO->GetClass() == eSFOT_KeySignature)
            pCT = ((lmKeySignature*)pSO)->GetContext(nStaff);
		else if (pSO->GetClass() == eSFOT_TimeSignature)
            pCT = ((lmTimeSignature*)pSO)->GetContext(nStaff);

        ++it;
    }
	return pCT;
}

lmContext* lmColStaffObjs::NewUpdatedContext(lmStaffObj* pThisSO)
{
	//returns the applicable context for this StaffObj, updated with all
	//accidentals introduced by previous notes.
	//AWARE: context ownership is transferred to the caller.
	//       The returned context MUST BE deleted by the invoking method.


    //start backwards iterator from this SO. We are going to see if in this
    //segment there is a previous clef, time or key signature,
	//as they define the last context. Otherwise, we will take context at
    //start of segment
	lmSegment* pSegment = pThisSO->GetSegment();
	lmItCSO it = --(pSegment->m_StaffObjs.end());
    //find this SO
    while(*it != pThisSO)
        --it;

    //go backwards to see if in this segment there is a previous clef, time or key signature
    int nStaff = pThisSO->GetStaffNum();
    lmContext* pCT = (lmContext*)NULL;
    while(it != pSegment->m_StaffObjs.end())
	{
        lmStaffObj* pSO = *it;
		if (pSO->IsClef() && nStaff == pSO->GetStaffNum())
            pCT = ((lmClef*)pSO)->GetContext();
		else if (pSO->IsKeySignature())
            pCT = ((lmKeySignature*)pSO)->GetContext(nStaff);
		else if (pSO->IsTimeSignature())
            pCT = ((lmTimeSignature*)pSO)->GetContext(nStaff);

        if (pCT)
		{
			it++;	//point to staffobj after clef, TS or KS
			break;
		}
		--it;
	}

	if (!pCT)
	{
		//we have arrived to start of segment and no clef, time or key signatures
		//found. Therefore take context at start of segment and point iterator to
        //first staffobj of this segment
		pCT = pSegment->GetContext(nStaff - 1);
		it = pSegment->m_StaffObjs.begin();
	}

	//Here, iterator 'it' is pointing to the first staffobj able to modify current context.
	//Now we have to go forward, updating accidentals until we reach target SO

	lmContext* pUpdated = new lmContext(pCT);
    while(*it != pThisSO)
	{
        lmStaffObj* pSO = *it;
		if (pSO->IsNoteRest() && !((lmNote*)pSO)->IsRest())
		{
			//Note found. Update context
			lmAPitch apPitch = ((lmNote*)pSO)->GetAPitch();
			pUpdated->SetAccidental(apPitch.Step(), apPitch.Accidentals());
		}
		++it;
	}

	return pUpdated;
}

lmContext* lmColStaffObjs::NewUpdatedLastContext(int nStaff)
{
	//returns the last context for staff nStaff, updated with all
	//accidentals introduced by previous notes.
	//AWARE: context ownership is transferred to the caller.
	//       The returned context MUST BE deleted by the invoking method.

    //get last StaffObj of last segment and start backwards iterator. We are
	//going to see if in this last segment there is a clef, time or key signature,
	//as they define the last context
    lmContext* pCT = (lmContext*)NULL;
	lmSegment* pSegment = m_Segments.back();
	lmItCSO it = --(pSegment->m_StaffObjs.end());
    while(it != pSegment->m_StaffObjs.end())
	{
        lmStaffObj* pSO = *it;
		if (pSO->GetClass() == eSFOT_Clef && nStaff == pSO->GetStaffNum())
            pCT = ((lmClef*)pSO)->GetContext();
		else if (pSO->GetClass() == eSFOT_KeySignature)
            pCT = ((lmKeySignature*)pSO)->GetContext(nStaff);
		else if (pSO->GetClass() == eSFOT_TimeSignature)
            pCT = ((lmTimeSignature*)pSO)->GetContext(nStaff);

        if (pCT)
		{
			it++;	//point to staffobj after clef, TS or KS
			break;
		}
		--it;
	}

	if (!pCT)
	{
		//we have arrived to start of segment and no clef, time or key signatures
		//found. Therefore take last context and point it to first staffobj of
		//last segment
		pCT = m_pOwner->GetLastContext(nStaff);
		it = pSegment->m_StaffObjs.begin();

        //if the score is empty there is no context yet. Return NULL
        if (!pCT)
            return pCT;
	}

	//Here, iterator 'it' is pointing to the first staffobj able to modify current context.
	//Now we have to go forward, updating accidentals until we reach end of collection

	lmContext* pUpdated = new lmContext(pCT);
    while(it != pSegment->m_StaffObjs.end())
	{
        lmStaffObj* pSO = *it;
		if (pSO->GetClass() == eSFOT_NoteRest && !((lmNote*)pSO)->IsRest())
		{
			//Note found. Update context
			lmAPitch apPitch = ((lmNote*)pSO)->GetAPitch();
			pUpdated->SetAccidental(apPitch.Step(), apPitch.Accidentals());
		}
		++it;
	}

	return pUpdated;
}

lmContext* lmColStaffObjs::GetLastContext(int nStaff)
{
	//returns the last context applicable to staff nStaff (1..n)

	wxASSERT(nStaff > 0);
	return m_pOwner->GetStaff(nStaff)->GetLastContext();
}

lmContext* lmColStaffObjs::GetStartOfSegmentContext(int nMeasure, int nStaff)
{
    return m_Segments[nMeasure-1]->GetContext(nStaff-1);
}

