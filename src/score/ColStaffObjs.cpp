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
#include "Staff.h"
#include "VStaff.h"
#include "Context.h"
#include "ColStaffObjs.h"
#include "UndoRedo.h"
#include "StaffObjIterator.h"
#include "../app/ScoreView.h"
#include "../app/ScoreDoc.h"
#include "../app/MainFrame.h"
#include "../app/TheApp.h"
#include "../app/Preferences.h"

//cursor positioning requires access to shapes
#include "../graphic/BoxScore.h"
#include "../graphic/BoxPage.h"
#include "../graphic/BoxSystem.h"
#include "../graphic/ShapeStaff.h"

//access to logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;

//-------------------------------------------------------------------------------------
//Binary function predicate, that replace '<' (lower than) function for sort algorithm.
//It must return true if the first element has to go before the second one in the
//required ordering criteria, that is, if objects position should be interchanged.
//
//It is used to sort the staffobjs list by timepos, duration and staff, taking also
//into account some peculiarities:
// - Keep precedence order of clef, key and time signature
// - keep notes in chord inmediately after base note
// - Keep the barline at the end
//
//-------------------------------------------------------------------------------------
bool SortCompare(lmStaffObj* pSO1, lmStaffObj* pSO2)
{

#if 0       //implementation for debugging. Slower
    bool fValue;
    int nCheck;

    //barlines always at end
    if (pSO1->IsBarline())
        fValue = false, nCheck=0;

    //order by timepos
    else if (IsLowerTime(pSO1->GetTimePos(), pSO2->GetTimePos()))
        fValue = true, nCheck=1;

    //SO1 has lower or equal time than SO2. If equal time order by duration.
    else if (IsEqualTime(pSO1->GetTimePos(), pSO2->GetTimePos())
             && IsLowerTime(pSO1->GetTimePosIncrement(), pSO2->GetTimePosIncrement()))
        fValue = true, nCheck=2;

    //ordered by timepos and duration. Chords: Ensure that all notes
    //of a chord go in sequence
    else if (IsEqualTime(pSO1->GetTimePos(), pSO2->GetTimePos())
             && IsEqualTime(pSO1->GetTimePosIncrement(), pSO2->GetTimePosIncrement())
             && pSO1->IsNoteRest() && pSO2->IsNoteRest()
             && ((lmNoteRest*)pSO1)->IsNote() && ((lmNoteRest*)pSO2)->IsNote()
             && ((lmNote*)pSO1)->GetChord() < ((lmNote*)pSO2)->GetChord() )
        fValue = true, nCheck=3;

    //here they are ordered by timepos and duration. Chords: Ensure base note precedence
    else if (IsEqualTime(pSO1->GetTimePos(), pSO2->GetTimePos())
             && IsEqualTime(pSO1->GetTimePosIncrement(), pSO2->GetTimePosIncrement())
             && pSO1->IsNoteRest() && pSO2->IsNoteRest()
             && ((lmNoteRest*)pSO1)->IsNote() && ((lmNoteRest*)pSO2)->IsNote()
             && ((lmNote*)pSO1)->GetChord() == ((lmNote*)pSO2)->GetChord()
             && ((lmNote*)pSO1)->IsBaseOfChord() && ((lmNote*)pSO2)->IsInChord() )
        fValue = true, nCheck=4;

    //If chord and right ordering, do not make more checks
    else if (IsEqualTime(pSO1->GetTimePos(), pSO2->GetTimePos())
             && IsEqualTime(pSO1->GetTimePosIncrement(), pSO2->GetTimePosIncrement())
             && pSO1->IsNoteRest() && pSO2->IsNoteRest()
             && ((lmNoteRest*)pSO1)->IsNote() && ((lmNoteRest*)pSO2)->IsNote()
             && ( ((lmNote*)pSO1)->GetChord() == ((lmNote*)pSO2)->GetChord() )
             && ((lmNote*)pSO1)->IsInChord() && ((lmNote*)pSO2)->IsBaseOfChord() )
        fValue = false, nCheck=5;

    //elements of same type ordered by staff number
    else if (pSO1->GetClass() == pSO2->GetClass()
             && IsEqualTime(pSO1->GetTimePos(), pSO2->GetTimePos())
             && IsEqualTime(pSO1->GetTimePosIncrement(), pSO2->GetTimePosIncrement())
             && pSO1->GetStaffNum() < pSO2->GetStaffNum())
        fValue = true, nCheck=6;

    else
        fValue = false, nCheck=7;

    wxLogMessage(_T("[SortCompare] SO1=%d (%s at t=%.2f), SO2=%d (%s at t=%.2f) %s (check %d)"),
                 pSO1->GetID(), pSO1->GetName(), pSO1->GetTimePos(),
                 pSO2->GetID(), pSO2->GetName(), pSO2->GetTimePos(),
                 (fValue ? _T("true") : _T("false")), nCheck );

    return fValue;

#else   //faster implementation

    //barlines always at end
    if (pSO1->IsBarline())
        return false;

    //order by timepos
    if (IsLowerTime(pSO1->GetTimePos(), pSO2->GetTimePos()))
        return true;

    //SO1 has lower or equal time than SO2. Now order by duration.
    if (IsEqualTime(pSO1->GetTimePos(), pSO2->GetTimePos())
        && IsLowerTime(pSO1->GetTimePosIncrement(), pSO2->GetTimePosIncrement()))
        return true;

    //here they are ordered by timepos and duration. Chords: Ensure that all notes
    //of a chord go in sequence. Chord address is used to identify the chord and, thus,
    //avoiding to mix notes from different chords
    if (IsEqualTime(pSO1->GetTimePos(), pSO2->GetTimePos())
        && IsEqualTime(pSO1->GetTimePosIncrement(), pSO2->GetTimePosIncrement())
        && pSO1->IsNoteRest() && pSO2->IsNoteRest()
        && ((lmNoteRest*)pSO1)->IsNote() && ((lmNoteRest*)pSO2)->IsNote()
        && ((lmNote*)pSO1)->GetChord() < ((lmNote*)pSO2)->GetChord() )
        return true;

    //here they are ordered by timepos and duration. Chords: Ensure base note precedence
    if (IsEqualTime(pSO1->GetTimePos(), pSO2->GetTimePos())
        && IsEqualTime(pSO1->GetTimePosIncrement(), pSO2->GetTimePosIncrement())
        && pSO1->IsNoteRest() && pSO2->IsNoteRest()
        && ((lmNoteRest*)pSO1)->IsNote() && ((lmNoteRest*)pSO2)->IsNote()
        && ((lmNote*)pSO1)->GetChord() == ((lmNote*)pSO2)->GetChord()
        && ((lmNote*)pSO1)->IsBaseOfChord() && ((lmNote*)pSO2)->IsInChord() )
        return true;

    //If chord and right ordering, do not make more checks
    if (IsEqualTime(pSO1->GetTimePos(), pSO2->GetTimePos())
        && IsEqualTime(pSO1->GetTimePosIncrement(), pSO2->GetTimePosIncrement())
        && pSO1->IsNoteRest() && pSO2->IsNoteRest()
        && ((lmNoteRest*)pSO1)->IsNote() && ((lmNoteRest*)pSO2)->IsNote()
        && ((lmNote*)pSO1)->GetChord() == ((lmNote*)pSO2)->GetChord()
        && ((lmNote*)pSO1)->IsInChord() && ((lmNote*)pSO2)->IsBaseOfChord() )
        return false;

    //elements of same type ordered by staff number
    if (pSO1->GetClass() == pSO2->GetClass()
        && IsEqualTime(pSO1->GetTimePos(), pSO2->GetTimePos())
        && IsEqualTime(pSO1->GetTimePosIncrement(), pSO2->GetTimePosIncrement())
        && pSO1->GetStaffNum() < pSO2->GetStaffNum())
        return true;

    return false;

#endif
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

//global variable used as default initializator
lmVCursorState g_tNoVCursorState = { -1, 0.0f, (lmStaffObj*)NULL };



lmVStaffCursor::lmVStaffCursor()
{
	m_nStaff = 1;
	m_rTimepos = 0.0f;
	m_pColStaffObjs = (lmColStaffObjs*)NULL;
    m_pScoreCursor = (lmScoreCursor*)NULL;
    m_pIt = (lmSOIterator*)NULL;
}

lmVStaffCursor::lmVStaffCursor(lmVStaffCursor& oVCursor)
{
	m_nStaff = oVCursor.GetNumStaff();
	m_rTimepos = oVCursor.GetTimepos();
	m_pColStaffObjs = (lmColStaffObjs*)NULL;
    m_pScoreCursor = oVCursor.GetScoreCursor();
    m_pIt = new lmSOIterator(oVCursor.m_pIt);
}

lmVStaffCursor::~lmVStaffCursor()
{
    if (m_pIt)
        delete m_pIt;
}

void lmVStaffCursor::AttachToCollection(lmColStaffObjs* pColStaffObjs, bool fReset)
{
	m_pColStaffObjs = pColStaffObjs;
	m_pColStaffObjs->AttachCursor(this);		//link back

    //create iterator if necessary
    if (m_pIt)
    {
        if (!m_pIt->IsManagingCollection(pColStaffObjs))
        {
            delete m_pIt;
            m_pIt = new lmSOIterator(pColStaffObjs, (lmStaffObj*)NULL);
        }
    }
    else
        m_pIt = new lmSOIterator(pColStaffObjs, (lmStaffObj*)NULL);

    //reset cursor if requested
    if (fReset)
	    ResetCursor();
}

void lmVStaffCursor::SetNewCursorState(lmScoreCursor* pSCursor, lmVCursorState* pState,
                                       bool fUpdateTimepos)
{
    //restore cursor information from save state info.
    //Preconditions:
    //  - cursor must be already attached to the collection of staffobjs

    wxASSERT(m_pColStaffObjs);

    m_pScoreCursor = (lmScoreCursor*)NULL;   //supress visual feedback
    m_nStaff = pState->nStaff;
    m_rTimepos = pState->rTimepos;
    m_pIt->MoveTo(pState->pSO);

    //update timepos, if requested
    if (fUpdateTimepos)
    {
        if (pState->pSO)
            m_rTimepos = pState->pSO->GetTimePos();
        else
        {
            //Not pointing to an staffobj. Cursor is at end of collection
            int nSegment = m_pIt->GetNumSegment();
            m_rTimepos = m_pColStaffObjs->GetSegment(nSegment)->GetDuration();
        }
    }

    //restore ScoreCursor
    m_pScoreCursor = pSCursor;
}

lmVCursorState lmVStaffCursor::GetState()
{
    lmVCursorState tState = { m_nStaff, m_rTimepos, m_pIt->GetCurrent() };
    return tState;
}

lmStaffObj* lmVStaffCursor::GetStaffObj() 
{ 
    wxASSERT(m_pIt);
    return m_pIt->GetCurrent();
}

int lmVStaffCursor::GetSegment()
{ 
    //return number of sgment 0..n-1

    wxASSERT(m_pIt);
    return m_pIt->GetNumSegment();
}

lmVStaff* lmVStaffCursor::GetVStaff()
{
    return m_pColStaffObjs->GetOwnerVStaff();
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

void lmVStaffCursor::MoveRightToNextTime()
{
    //Cursor movement by occupied timepos in current staff and at end of score.
    //When an empty measure is found, cursor will stop at begining (timepos 0)
    //and before the barline, and then move to next measure.


    //(**1) if at end of collection remain there
	if (m_pIt->EndOfCollection()) return;      //Done. Case [e1]

    lmStaffObj* pCurSO = m_pIt->GetCurrent();
    wxASSERT(pCurSO);

    //cursor could be placed at lower time than current object (i.e. if caret has been
    //positioned by time increment). Check if current object could be the target place
    if (pCurSO->IsOnStaff(m_nStaff))
    {
        if ( IsLowerTime(m_rTimepos, pCurSO->GetTimePos()) )
        {
            //we are at right place. Just advance cursor time
            m_rTimepos = pCurSO->GetTimePos();
            return;     //Done. Case [n1]
        }
        else
        {
            //same timepos than CursorObj
            if (pCurSO->IsBarline())
            {
                //Advance to next segment.
                //As there is a barline, there also exist a next segment
                m_pIt->MoveNext();
                pCurSO = m_pIt->GetCurrent();

                //move at timepos 0, at right place for TargetStaff
                while (pCurSO
                       && IsEqualTime(pCurSO->GetTimePos(), 0.0f)
                       && pCurSO->GetStaffNum() < m_nStaff
                       && !( pCurSO->IsBarline()
                             || pCurSO->IsKeySignature()
                             || pCurSO->IsTimeSignature() )
                      )
                {
                    m_pIt->MoveNext();
                    pCurSO = m_pIt->GetCurrent();
                }

                m_rTimepos = 0.0f;
                return;     //Done. Case [n2]
            }
        }
    }

    //Here CursorObj is not a valid position. We have to advance to next object
    //in this staff with time equal or greater than current time.
    //If CursorObj is applicable to current staff, next target time is
    //CursorObjTimepos + CursorObj.Duration. Otherwise, we do not have a target
    //timepos but the first valid position we find.
    float rTargetTimepos;
    if (pCurSO->IsOnStaff(m_nStaff))
        rTargetTimepos = pCurSO->GetTimePos() + pCurSO->GetTimePosIncrement();
    else
        rTargetTimepos = -1.0f;      //to stop at next valid position

    //advance to first position in this staff with timepos >= TargetTime.
    lmStaffObj* pLastSO = pCurSO;        //save current object
    m_pIt->MoveNext();               //increment iterator
    pCurSO = m_pIt->GetCurrent();
    while (pCurSO
           && ( IsHigherTime(rTargetTimepos, pCurSO->GetTimePos())
                || !pCurSO->IsOnStaff(m_nStaff)))
    {
        m_pIt->MoveNext();
        pCurSO = m_pIt->GetCurrent();
    }

    //(**2)
    //if we reach end of collection is because there is no barline in last segment. Remain there
    if (!pCurSO)
    {
        //We are at end of collection.
        //Get last staffobj and assign its timepos plus its duration
        if (pLastSO && pLastSO->IsOnStaff(m_nStaff))
            m_rTimepos = pLastSO->GetTimePos() + pLastSO->GetTimePosIncrement();
        return;     //Done. Case [e2]
    }

    //Set new cursor timepos
    if (IsEqualTime(rTargetTimepos, -1.0f))
        //no target time. New cursor timepos is object timepos
        m_rTimepos = pCurSO->GetTimePos();
    else
        //New cursor timepos at target timepos
        m_rTimepos = rTargetTimepos;

    return;     //Done. Case [n3]
}

void lmVStaffCursor::MoveLeftToPrevTime()
{
    //Cursor movement by occupied timepos in current staff and at start of score.
    //Cursor will always stop in each measure at timepos 0 (even if no objects there)
    //and then move to prev measure and stop before barline.


    //(**1) if at beginning of collection remain there. Done. Case [e1]
	if (m_pIt->FirstOfCollection()) return;        //Done. Case [e1]

    //determine target timepos
    //move back to previous object in target staff
    bool fPrevObjFound = false;
    m_pIt->MovePrev();
    while(!m_pIt->ChangeOfMeasure())
    {
        //check if staffobj is in current staff; otherwise we have to continue moving back
        if (m_pIt->GetCurrent()->IsOnStaff(m_nStaff))
        {
            //it is in current staff. PrevObj found. Finish loop
            fPrevObjFound = true;
            break;
        }
        m_pIt->MovePrev();
    }

    if (fPrevObjFound)
    {
        //Cursor has to be positioned at timepos PrevObj.Timepos
        //if PrevObj is not note in chord
        lmStaffObj* pSO = m_pIt->GetCurrent();
        if (!pSO->IsNoteRest()
            || !((lmNoteRest*)pSO)->IsNote()
            || !((lmNote*)pSO)->IsInChord()
            || ((lmNote*)pSO)->IsBaseOfChord() )
        {
            //Done. Place cursor at PrevObj
            m_rTimepos = pSO->GetTimePos();
            return;     //Done. Case [n2]
        }
        else
        {
            //place cursor at base note of chord
            //while (PrevObj is note in chord and is not base note)
            while (pSO->IsNoteRest()
                    && ((lmNoteRest*)pSO)->IsNote()
                    && ((lmNote*)pSO)->IsInChord()
                    && !((lmNote*)pSO)->IsBaseOfChord() )
            {
                m_pIt->MovePrev();
                pSO = m_pIt->GetCurrent();
            }
            //Done. Place cursor at base note of chord
            m_rTimepos = pSO->GetTimePos();
            return;     //Done. Case [n3]
        }
    }
    else
    {
        //PrevObj not found. We have moved to prev segment or we are at start of collection.
        //If current time is > 0 or we are at start of collection we should position at 
        //start of segment
        if (IsHigherTime(m_rTimepos, 0.0f) || m_pIt->FirstOfCollection())
        {
            //set at begin of segment
            if (!m_pIt->FirstOfCollection())
                m_pIt->MoveNext();

            //move to position 0 in this segment
            PositionAt(0.0f);
            return;                                 //Done. Case [n3]
        }
        else
        {
            //we have to move to end of previous segment but
            //as we are not at start of collection we are already there

            //current SO must be the barline (it MUST exist)
            lmStaffObj* pSO = m_pIt->GetCurrent();
            wxASSERT(pSO && pSO->IsBarline());

            //done
            m_rTimepos = pSO->GetTimePos();
            return;     //Done. Case [n4]
        }
    }
}

void lmVStaffCursor::PositionAt(float rTargetTimepos)
{
    //cursor has to be positioned at timepos TargetTimepos, at appropiate position
    //for current staff.
    //movement is restricted to current segment.
    //current position is a valid position, so target position is current position
    //or any position after it.
    //we are not at end of segment

    wxASSERT(m_pIt && m_pIt->GetCurrent());

    //advance to first position in this staff with timepos >= TargetTime.
    lmStaffObj* pLastSO = m_pIt->GetCurrent();        //save current object
    lmStaffObj* pSO = pLastSO;
    while (pSO
           && ( IsHigherTime(rTargetTimepos, pSO->GetTimePos())
                || !pSO->IsOnStaff(m_nStaff)))
    {
        m_pIt->MoveNext();
        pSO = m_pIt->GetCurrent();
    }

    //if we reach end of collection is because we have not reached a barline.
    //Remain there
    if (!pSO)
    {
        //We are at end of collection.
        //Get last staffobj and assign its timepos plus its duration
        if (pLastSO && pLastSO->IsOnStaff(m_nStaff))
            m_rTimepos = pLastSO->GetTimePos() + pLastSO->GetTimePosIncrement();
        return;
    }

    //Set new cursor timepos
    m_rTimepos = rTargetTimepos;
    return;
}

void lmVStaffCursor::MoveRight(bool fAlsoChordNotes)
{
    MoveRightToNextTime();
}

void lmVStaffCursor::MoveLeft(bool fAlsoChordNotes)
{
    MoveLeftToPrevTime();
}

lmStaffObj* lmVStaffCursor::GetPreviousStaffobj()
{
    //Method to get previous staffobj in current staff and skipping notes in chord.
    //This method does not interfere with cursor position

    if (m_pIt->FirstOfCollection())
        return (lmStaffObj*)NULL;

    lmSOIterator it(m_pColStaffObjs, m_pIt->GetCurrent()); 
    it.MovePrev();
    while(!it.FirstOfCollection() && !it.EndOfCollection())
    {
        lmStaffObj* pSO = it.GetCurrent();

        //check if staffobj is in current staff; otherwise we have to continue moving back
        if (pSO->IsOnStaff(m_nStaff))
        {
            //it is in current staff.
            //We have to skip notes in chord except base note
            if (!pSO->IsNoteRest()
                || pSO->IsNoteRest() 
                && ((lmNoteRest*)pSO)->IsNote() 
                     && ( !((lmNote*)pSO)->IsInChord() || ((lmNote*)pSO)->IsBaseOfChord())
                
                )
                break;
        }
        it.MovePrev();
    }

    //return staffobj
	if (m_pIt->FirstOfCollection())
	{
		//if start of collection reached, previous object was the first one in current staff.
		//So no previous staffobj
		return (lmStaffObj*)NULL;
	}
	else
	    return it.GetCurrent();
}

void lmVStaffCursor::MoveToTime(float rNewTime)
{
    //Within the limits of current segment, move cursor to first object
    //with time > rNewTime in staff m_nStaff.
    //If no object found, cursor is moved to end of segment, with time rNewTime

    //Move to start of current segment
    lmSegment* pSegment = m_pColStaffObjs->GetSegment(m_pIt->GetNumSegment());
    m_pIt->MoveTo(pSegment->GetFirstSO());
    m_rTimepos = rNewTime;
	if (m_pIt->EndOfCollection()) 
        return;      //the segment is empty. We are at end and time has been updated to rNewTime

    //skip staffobjs with time lower than rNewTime
    while (!m_pIt->ChangeOfMeasure() && !m_pIt->EndOfCollection()
	       && IsLowerTime(m_pIt->GetCurrent()->GetTimePos(), rNewTime) )
        m_pIt->MoveNext();

    if (!m_pIt->ChangeOfMeasure() && !m_pIt->EndOfCollection()
        && IsEqualTime(m_pIt->GetCurrent()->GetTimePos(), rNewTime))
    {
        //time found. Advance to first object at this time and in current staff

        lmStaffObj* pSO = m_pIt->GetCurrent();
	    while (pSO
               && IsEqualTime(pSO->GetTimePos(), rNewTime)
               && !pSO->IsOnStaff(m_nStaff) )
        {
            m_pIt->MoveNext();
            pSO = m_pIt->GetCurrent();
        }

        //if object found, return.
	    if (pSO
            && IsEqualTime(pSO->GetTimePos(), rNewTime)
            && pSO->IsOnStaff(m_nStaff) )
        {
            //object found.
            return;
        }

        //No object satisfying the constrains is found.
        //If we are not at end of segment, advance to first object with time > rNewTime.
	    while (pSO && IsEqualTime(pSO->GetTimePos(), rNewTime))
        {
            m_pIt->MoveNext();
            pSO = m_pIt->GetCurrent();
        }
    }
    //else
        //time does not exist. Cursor is now at first object > rNewTime
}

void lmVStaffCursor::ResetCursor()
{
    //Move cursor to first object in first segment. No constrains on staff.
    //No visual feedback provided.
    //If visual feedback needed use MoveToFirst() instead. It also takes staff into account

	m_nStaff = 1;
	m_rTimepos = 0.0f;
	wxASSERT(m_pColStaffObjs->GetNumMeasures() > 0);

	//position in first object
	m_pIt->MoveFirst();
    UpdateTimepos();
}

void lmVStaffCursor::MoveToFirst(int nStaff)
{
    //Move cursor to first object in first segment on staff nStaff.
	//If no staff specified (nStaff==0) remains in current staff

	if (nStaff != 0)
		m_nStaff = nStaff;

	m_rTimepos = 0.0f;
	wxASSERT(m_pColStaffObjs->GetNumMeasures() > 0);

	//position in first segment
	m_pIt->MoveFirst();
    if (m_nStaff != 0)
    {
        while(!m_pIt->EndOfCollection()
              && !m_pIt->GetCurrent()->IsOnStaff(m_nStaff))
        {
            m_pIt->MoveNext();
        }
    }
    //found. Update time
    UpdateTimepos();
}

void lmVStaffCursor::AdvanceToTime(float rTime)
{
    //Cursor is positioned at end of current segment. So iterator can not be advanced more
    //without moving to next segment. If required time is within the limits
    //of segment duration, it just advances timepos.
    //Else, if required time is greater than segment duration, moves to next segment, at
    //the time resulting from discounting segment duration.

	m_rTimepos = rTime;
    if (m_pIt->EndOfCollection())
        return;

    float rSegmentDuration = m_pIt->GetCurrent()->GetSegment()->GetDuration();
    if (IsHigherTime(rTime, rSegmentDuration))
    {
        //advance to next segment
	    m_pIt->MoveNext();
        wxASSERT(m_pIt->ChangeOfMeasure() || m_pIt->EndOfCollection());
        m_rTimepos = rTime - rSegmentDuration;
    }

}

void lmVStaffCursor::AdvanceToNextSegment()
{
    //Cursor is positioned at end of segment. 
    //Advances cursor to time 0 in next segment

	m_pIt->MoveNext();
    wxASSERT(m_pIt->ChangeOfMeasure() || m_pIt->EndOfCollection());
	m_rTimepos = 0.0f;
}

void lmVStaffCursor::SkipClefKey(bool fSkipKey)
{
    //First advance after last clef. Then, if fSkipKey, advance after last key

    //ensure that cursor is at start of segment
    lmSegment* pSegment = m_pColStaffObjs->GetSegment(m_pIt->GetNumSegment());
    m_pIt->MoveTo(pSegment->GetFirstSO());

    lmStaffObj* pLast = m_pIt->GetCurrent();      //it will be used to save the last valid position
    bool fFound = false;

    //locate last clef
	while (!m_pIt->EndOfCollection())
	{
        lmStaffObj* pSO = m_pIt->GetCurrent();
        if (pSO->IsClef())
        {
            pLast = pSO;
            fFound = true;
        }
        else if (pSO->IsKeySignature())
            break;
        else if (!IsEqualTime(pSO->GetTimePos(), 0.0f))
            break;
        m_pIt->MoveNext();
    }

    //here pLast points to last clef, if any, or to start of segment
    //if requested, advance to last key
    if (fSkipKey)
    {
        //locate last key
	    while (!m_pIt->EndOfCollection())
	    {
            lmStaffObj* pSO = m_pIt->GetCurrent();
            if (pSO->IsKeySignature())
            {
                pLast = pSO;
                fFound = true;
            }
            else if (!IsEqualTime(pSO->GetTimePos(), 0.0f))
                break;
            m_pIt->MoveNext();
        }
    }

    //here pLast is pointing to last clef/key (fFound == true) or to start of collection if
    //no clef/key (fFound==false)
    //reposition cursor iterator and advance after last clef/key
    m_pIt->MoveTo(pLast);
    if (fFound)
        m_pIt->MoveNext();
}

void lmVStaffCursor::AdvanceToStartOfSegment(int nSegment, int nStaff)
{
    //move cursor to start of segment. Set staff

    wxASSERT(nStaff > 0);
    wxASSERT(nSegment < m_pColStaffObjs->GetNumMeasures());

	m_pIt->MoveTo(m_pColStaffObjs->GetSegment(nSegment)->GetFirstSO());
	m_rTimepos = 0.0f;
}

void lmVStaffCursor::AdvanceToStartOfTimepos()
{
    //This methods moves the cursor to the first SO at current timepos.
    //For example, barline is going to be inserted at current position. Assume a
    //piano grand staff with a C note on first staff and a G note on second staff.
    //Also assume that cursor is pointing to second staff, G note. As both notes
    //C & G are at the same timepos, it would be wrong to insert the barline before
    //the G note. This method will move the cursor to the C note.

    lmStaffObj* pSO = m_pIt->GetCurrent();
    if (pSO && pSO == pSO->GetSegment()->GetFirstSO())
        return;         //cursor is at start of segment. Nothing to do

    lmStaffObj* pFinalPos = pSO;
    m_pIt->MovePrev();
    lmStaffObj* pPrev = m_pIt->GetCurrent();
	while (!m_pIt->ChangeOfMeasure())
	{
        //check if we are at current tiempos
        if (pPrev->GetTimePos() != m_rTimepos)
            break;      //done

        //check if it is a clef, key or time signature
        if (!( pPrev->IsClef() || pPrev->IsKeySignature() || pPrev->IsTimeSignature()) )
            pFinalPos = pPrev;

        //move back
        m_pIt->MovePrev();
    }
    m_pIt->MoveTo(pFinalPos);
}

void lmVStaffCursor::MoveToSegment(int nSegment, int iStaff, lmUPoint uPos)
{
    //move cursor to nearest object to uPos, constrained to specified segment and staff.

    wxASSERT(nSegment < m_pColStaffObjs->GetNumMeasures());

    m_nStaff = iStaff;
	m_pIt->MoveTo(m_pColStaffObjs->GetSegment(nSegment)->GetFirstSO());
	m_rTimepos = 0.0f;

	//if segment empty finish. We are at start of segment
	if (m_pIt->EndOfCollection())
        return;      //the segment is empty

    //move cursor to nearest object to uPos, constrained to this segment and staff
	lmLUnits uMinDist = 1000000.0f;	//any too big value
	lmStaffObj* pFoundSO = (lmStaffObj*)NULL;
	while (!m_pIt->EndOfCollection() && !m_pIt->ChangeOfMeasure())
	{
        lmStaffObj* pSO = m_pIt->GetCurrent();
        if (pSO->IsOnStaff(m_nStaff))
        {
		    lmLUnits xPos = pSO->GetShape()->GetXLeft();
		    lmLUnits uDist = xPos - uPos.x;
		    if (uDist > 0.0f && uDist < uMinDist)
            {
                pFoundSO = pSO;
		        uMinDist = uDist;
            }
        }
		m_pIt->MoveNext();
	}

	//object found or end of segment reached
	if (pFoundSO)
        m_pIt->MoveTo(pFoundSO);
    UpdateTimepos();
}

void lmVStaffCursor::MoveCursorToObject(lmStaffObj* pSO)
{
    //Move cursor to required staffobj
    //Precondition: the staffobj is in current instrument

    m_pIt->MoveTo(pSO);
    UpdateTimepos();
}

lmStaff* lmVStaffCursor::GetCursorStaff()
{
    //return pointer to staff in which cursor is located
    return m_pColStaffObjs->GetOwnerVStaff()->GetStaff(m_nStaff);
}

lmUPoint lmVStaffCursor::GetCursorPoint(int* pNumPage)
{
    //compute coordinate for placing cursor and return it
    //Cursor knows nothing about the graphic model. So it is necessary to interact with
    //it and get the necessary information.

    lmUPoint uPos(0.0f, 0.0f);
    lmStaffObj* pCursorSO = GetStaffObj();

    //variables to contain time and x pos of previous and current staffobjs.
    //I will use subindex 1 for previous and 2 for current staffobj.
    lmLUnits uxStart1, uxStart2;
    lmLUnits uxEnd1, uxEnd2;
    float rTime1, rTime2;
    float rTimeCursor = m_rTimepos;  //save it, as will be lost when MoveLeft(), etc.
    int nPage1=0, nPage2=0;

    //
    //collect information about staffobjs and shapes' positions
    //

    lmScore* pScore = m_pColStaffObjs->GetOwnerVStaff()->GetScore();
    wxASSERT(pScore->GetGraphicObject()->IsBox());
    lmLUnits uCaretSpace = pScore->TenthsToLogical(5.0f);   //distance between caret and object

    //get current staffobj info
    if (pCursorSO)
    {
        //get info from cursor staffobj
        rTime2 = pCursorSO->GetTimePos();
        lmShape* pShape2 = pCursorSO->GetShape(m_nStaff);
        wxASSERT(pShape2);      // No shape for current sttafobj !!!
        uPos.y = GetStaffPosY(pCursorSO);
        uxStart2 = pShape2->GetXLeft();
        uxEnd2 = pShape2->GetXRight();
        nPage2 = pShape2->GetPageNumber();
    }

    //get info from previous staffobj
    lmStaffObj* pPrevSO = GetPreviousStaffobj();
    if (pPrevSO)
    {
        rTime1 = pPrevSO->GetTimePos();
        lmShape* pShape1 = pPrevSO->GetShape(m_nStaff);
	    wxASSERT(pShape1);            // No shape for current sttafobj !!!
		uxStart1 = pShape1->GetXLeft();
        uxEnd1 = pShape1->GetXRight();
        nPage1 = pShape1->GetPageNumber();
    }


    //
    //Compute cursor position based on previously collected information
    //

    if (pCursorSO && pPrevSO)
    {
        //Both staffobjs, previous and current, exist. So cursor is between the two staffobjs,
        //or over the sencond one.
        //Decide on positioning, based on cursor time
        if (IsEqualTime(m_rTimepos, pCursorSO->GetTimePos()))
        {
            //Pointing to cursor staffobj. Take positioning information from staffobj
		    uPos.x = uxStart2 - uCaretSpace;    //+ pShape2->GetWidth()/2.0f;
        }
        else if (IsLowerTime(m_rTimepos, pCursorSO->GetTimePos()))
        {
            //Between current and previous. Interpolate position
            rTime1 = pPrevSO->GetTimePos();
            float rTimeIncr = rTime2 - rTime1;      // At = t2 - t1
            lmLUnits uXIncr = uxStart2 - uxEnd1;    //Ax = x2-x1
            //At' = t3-t1;   Ax' = x3 - x1 = Ax * (At' / At);   x3 = Ax' + x1
            uPos.x = (uXIncr * ((rTimeCursor - rTime1) / rTimeIncr)) + uxEnd1;
        }
        else
            //current cursor time > current staffobj time. Impossible!!
            wxASSERT(false);

        if (pNumPage)
            *pNumPage = nPage2;

        return uPos;
    }

    else if (pCursorSO)
    {
        //No previous staffobj. Current staffobj is the first one and, therefore, cursor
        //must be on it
        if (IsEqualTime(m_rTimepos, pCursorSO->GetTimePos()))
        {
            //Pointing to cursor staffobj. Take positioning information from staffobj
		    uPos.x = uxStart2 - uCaretSpace;    //+ pShape2->GetWidth()/2.0f;
        }
        else
            //can not be before the first staffobj !!
            //wxASSERT(false);
		    uPos.x = uxStart2 - uCaretSpace;    //+ pShape2->GetWidth()/2.0f;

        if (pNumPage)
            *pNumPage = nPage2;

        return uPos;
    }

    else if (pPrevSO)
    {
        //No current staffobj but previous one exist. Previous one is the last one and
        //the cursor is at the end of the score.
        //Position cursor four lines (40 tenths) at the right of last staffobj
        uPos.y = GetStaffPosY(pPrevSO);
        uPos.x = uxEnd1 + pPrevSO->TenthsToLogical(40);

        if (pNumPage)
            *pNumPage = nPage1;

        return uPos;
    }

    //No current staffobj and no previous staffobj
    //The score is empty, place cursor at first system of current page (there should be
    //only one page and one system, but let's have the code ready just in case we have
    //many empty pages full of empty systems)

    //Take positioning information from staff position
    lmBoxScore* pBS = (lmBoxScore*)pScore->GetGraphicObject();
    lmBoxPage* pBPage = pBS->GetPage(pBS->GetNumPages());
    lmBoxSystem* pBSystem = pBPage->GetSystem(pBPage->GetFirstSystem());
    lmShape* pShape = pBSystem->GetStaffShape(1);
    uPos.y = pShape->GetYTop();
    uPos.x = pShape->GetXLeft() + pScore->TenthsToLogical(20);

    if (pNumPage)
        *pNumPage = pBPage->GetPageNumber();

    return uPos;
}

float lmVStaffCursor::GetStaffPosY(lmStaffObj* pSO)
{
    //receives a staffobj and returns the y coordinate of the staff on which this staffobj
    //is placed

    lmShape* pShape = pSO->GetShape(m_nStaff);
	lmBoxSystem* pSystem = pShape->GetOwnerSystem();
	//GetStaffShape() requires as parameter the staff number, relative to the
	//total number of staves in the system. But we have staff number relative to
	//staves in current instrument. So we have to determine how many instruments
	//there are, and transform staff number.
	int nRelStaff = m_nStaff;
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
    lmStaffObj* pSO = m_pIt->GetCurrent();
    if (pSO)
        m_rTimepos = pSO->GetTimePos();
    else
    {
        //We are at end of collection.
        //Get last staffobj and assign its timepos plus its duration
        pSO = GetPreviousStaffobj();
        if (pSO)
        { 
            //segment is not empty
            if (pSO->IsBarline())
            {
                wxASSERT(false);        //we cannot be after the barline !!
                m_rTimepos = pSO->GetTimePos();
            }
            else
                m_rTimepos = pSO->GetTimePos() + pSO->GetTimePosIncrement();
        }
        else
            //segment is empty. Place cursor at start of segment
            m_rTimepos = 0.0f;
    }
}

bool lmVStaffCursor::IsAtEnd() 
{ 
    return m_pIt->EndOfCollection(); 
}

bool lmVStaffCursor::IsAtBeginning() 
{ 
    return m_pIt->FirstOfCollection(); 
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

    //initialize this segment's collection
    m_pFirstSO = (lmStaffObj*)NULL;
    m_pLastSO = (lmStaffObj*)NULL;
    m_nNumSO = 0;
}

lmSegment::~lmSegment()
{
}

lmSegment* lmSegment::GetNextSegment()
{
    return m_pOwner->GetNextSegment(m_nNumSegment);
}

int lmSegment::GetNumVoices()
{
    int nNumVoices=0;
    for (int i=0; i <= lmMAX_VOICE; i++)
    {
        if (IsVoiceUsed(i))
            nNumVoices++;
    }
    return nNumVoices;
}

bool lmSegment::IsVoiceUsed(int nVoice)
{
    int bMask = 1 << nVoice;
    return (m_bVoices & bMask) != 0;
}

void lmSegment::VoiceUsed(int nVoice)
{
    int bMask = 1 << nVoice;
    m_bVoices |= bMask;
}

int lmSegment::StartNewVoice()
{
    //starts a new voice in the range 1..lmMAX_VOICE (voice 0 is reserved).
    //returns new voice number
    for (int i=1; i <= lmMAX_VOICE; i++)
    {
        if (!IsVoiceUsed(i))
            return i;
    }
    wxASSERT(false);    //not enough voices!!
    return 0;
}

void lmSegment::Remove(lmStaffObj* pSO, bool fDelete, bool fClefKeepPosition,
                       bool fKeyKeepPitch)
{
	//Remove the staffobj pSO from the segment
    //The removed object is deleted or not, depending on flag 'fDelete'
    //VCursor is neither used nor updated


    wxASSERT(pSO->GetSegment() == this);

	//remove the staffobj from the collection
    lmStaffObj* pNext = pSO->GetNextSO();
    lmStaffObj* pPrev = pSO->GetPrevSO();
    bool fUpdateFirstInCollection = (m_pFirstSO == m_pOwner->GetFirstSO());
    bool fUpdateLastInCollection = (m_pLastSO == m_pOwner->GetLastSO());
    if (pPrev)
        pPrev->SetNextSO(pNext);
    if (pNext)
        pNext->SetPrevSO(pPrev);
    if (m_pFirstSO == pSO)
    {
        m_pFirstSO = pNext;
        if (fUpdateFirstInCollection)
            m_pOwner->SetFirstSO(pNext);
    }
    if (m_pLastSO == pSO)
    {
        m_pLastSO = pPrev;
        if (fUpdateLastInCollection)
            m_pOwner->SetLastSO(pPrev);
    }
    --m_nNumSO;


    //if removed object is a note not in chord:
    //  - Shift left all note/rests in this voice and sort the collection
	if (pSO->IsNoteRest() && ((lmNoteRest*)pSO)->IsNote() && !((lmNote*)pSO)->IsInChord())
    {
        ShiftLeftTimepos(pNext,
                         ((lmNoteRest*)pSO)->GetDuration(),
                         ((lmNoteRest*)pSO)->GetTimePos() + ((lmNoteRest*)pSO)->GetDuration(),
                         ((lmNoteRest*)pSO)->GetVoice()  );

        SortMeasure();
    }

    //if removed object is a time signature, re-bar the collection. As this implies removing
    //current segments and creating new ones, all needed context propagation and update is
    //done at the same time. Therefore, no later steps are necessary for this
    //If there are no objects after the removed TS, nothing to re-bar
    if (pSO->IsTimeSignature())
    {
        if (pNext)
        {
            //we have to re-bar from current segment to next time signature.
            lmTimeSignature* pNewTS = pNext->GetApplicableTimeSignature();
            lmStaffObj* pLastSO = m_pOwner->FindFwdTimeSignature(pNext);
            if (pLastSO)
                pLastSO = m_pOwner->FindPrevStaffObj(pLastSO);

            m_pOwner->AutoReBar(pNext, pLastSO, pNewTS);
        }
    }
    else
    {
        //If removed staffobj created contexts (clef, TS or KS) the contexts are already
        //unchained from the staves context chain [This was acomplished at
        //VStaff::Cmd_DeleteXxxxx method, when invoking pSO->RemoveCreatedContexts() ].
        //But it is necessary:
        //- to update pointers to contexts at start of segment; and
        //- to update staffobjs in segment, if affected by the context change
        if (pSO->IsClef())
            DoContextRemoval((lmClef*)pSO, pNext, fClefKeepPosition);
        else if (pSO->IsKeySignature())
            DoContextRemoval((lmKeySignature*)pSO, pNext, fKeyKeepPitch);
    }

    //finally, if requested, invoke destructor for removed staffobj
    if (fDelete) delete pSO;
}

void lmSegment::Store(lmStaffObj* pNewSO, lmVStaffCursor* pCursor)
{
    //Store the received StaffObj in this segment, at right position to keep the
    //segment ordered by timepos, duration and staff.
    //If added object is a note/rest and it is not added to a chord, we also have to
    //update segment's duration and shift timepos of all objects in this voice
    //after the inserted one
    //Cursor points to the insertion point (the new object will be inserted before it).
    //If it is a note in chord, it is inserted after object pointed by cursor.
    //If no cursor received or pointed object is null (en of collection) the new object
    //is inserted at the end

    lmStaffObj* pNextSO;        //ptr to next SO in the collection
    lmStaffObj* pCursorSO = (pCursor ? pCursor->GetStaffObj() : (lmStaffObj*)NULL);

    //notes in chord must be added after note pointed by cursor
    bool fAfter = pNewSO->IsNoteRest() && ((lmNoteRest*)pNewSO)->IsNote() &&
                  ((lmNote*)pNewSO)->IsInChord();

    //add staffobj to the collection
    if (pCursorSO)
    {
        //cursor must be pointing to an object in this segment. Otherwise cursor is
        //at end of collection
        wxASSERT(pCursorSO->GetSegment() == this);

        //Insert pNewSO before/after pCursorSO
        pNextSO = pCursorSO;
        if (fAfter)
            pNextSO = pNextSO->GetNextSO();

        AddToCollection(pNewSO, pNextSO);     //insert pNewSO before item pNextSO
    }
    else
    {
        //Collection is empty. Add at the end of segment.
        //This code is only used when reorganizing the collection (AutoRebar) or
        //during edition, when adding objects at the end
        AddToCollection(pNewSO, (lmStaffObj*)NULL);     //insert pNewSO at the end
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
	if (pNewSO->IsNoteRest() && ((lmNote*)pNewSO)->IsNote() && ((lmNote*)pNewSO)->IsInChord())
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
			ShiftRightTimepos(pNewSO, ((lmNoteRest*)pNewSO)->GetDuration() );
		}

        //update barline timepos, to allow for irregular measures
        lmBarline* pBL = GetBarline();
        if (pBL)
            pBL->SetTimePos(m_rMaxTime);
	}

	//ensure right ordering
	if (pNewSO->IsNoteRest())
        SortMeasure();


	////DBG  ------------------------------------------------------------------
	//wxLogMessage(_T("[lmSegment::Store] After insertion of %s"),
	//			pNewSO->GetName());
	//wxLogMessage(Dump());
	////END DBG ---------------------------------------------------------------
}

void lmSegment::AddToCollection(lmStaffObj* pNewSO, lmStaffObj* pNextSO)
{
    //insert pNewSO before item pNextSO. If pNewSO is NULL, inserts it at the end

    m_pOwner->AddToCollection(pNewSO, pNextSO);

    //update ptrs to first and last nodes of segment
    lmStaffObj* pPrev = pNewSO->GetPrevSO();
    lmStaffObj* pNext = pNewSO->GetNextSO();
	if(!pPrev || !m_pFirstSO || pNext == m_pFirstSO)
		m_pFirstSO = pNewSO;
    if (!pNext || !m_pLastSO || pPrev == m_pLastSO)
		m_pLastSO = pNewSO;
    ++m_nNumSO;         //increment objects count
}

void lmSegment::RecomputeSegmentDuration(lmNoteRest* pNR, float rTimeIncr)
{
    //Duration of note/rest pNR has been changed. Therefore it could be necessary to recompute
    //segment duration and reposition (in time) all objects after pNR.

    if (IsHigherTime(rTimeIncr, 0.0f))
        ShiftRightTimepos(pNR, rTimeIncr);
    else
    {
        lmStaffObj* pSO = pNR->GetNextSO();
        ShiftLeftTimepos(pSO, -rTimeIncr, pNR->GetTimePos() + pNR->GetDuration() - rTimeIncr,
                         pNR->GetVoice() );
    }
}

void lmSegment::ShiftLeftTimepos(lmStaffObj* pStartSO, float rTimeShift, float rStartTimepos,
                                 int nVoice)
{
    //The timepos of all consecutive notes/rests, in voice nVoice, starting with the one
    //pointed by pStartSO must be modified, because either the previous note/rest has been
    //deleted or its duration has been decreased.
    //
    //To check that next note/rest is consecutive, we need to know what was the next timepos
    //after modified/deleted note. This value is received in parameter rStartTimepos.
    //
    //The amount of time to shift notes/rests is in rTimeShift (always positive). It is
    //the duration of the removed note/rest or its decrement in duration. Always positive


    //Algorithm:
    //  Parameters: nVoice (the affected voice)
    //              rTimeShift (the duration of the removed note/rest or its decrement in
    //                          duration. Always positive)
    //              rStartTimepos (the next timepos after modified/deleted note)
    //              pStartSO (the first object to start shifting -- check its voice --)
    //
    //    1 Asign: NextTime = rStartTimepos
    //             pCurrentSO = take next object in voice nVoice starting with pStartSO
    //
    //    2 While not end of segment
    //        - Assign: t = Timepos(pCurrentSO)
    //        - if t == NextTime:
    //            - Assign: Timepos(pCurrentSO) = t - TimeShift
    //                      NextTime = t + Duration(pCurrentSO)
    //          else
    //            - break
    //        - Assign: pCurrentSO = take next object in voice nVoice after pCurrentSO


    float rNextTime = rStartTimepos;
	lmStaffObj* pSO = pStartSO;
    bool fChanges = false;

	while (pSO)
	{
        if (pSO->IsNoteRest() && ((lmNoteRest*)pSO)->GetVoice() == nVoice)
        {
            float rTime = pSO->GetTimePos();
            if (IsEqualTime(rTime, rNextTime))
            {
                pSO->SetTimePos(rTime - rTimeShift);
                rNextTime = rTime + pSO->GetTimePosIncrement();
                fChanges = true;
            }
            else
                break;
        }
        pSO = pSO->GetNextSO();
	}


    //if any change, update measure duration and review AutoBeam
    if (fChanges)
    {
        UpdateMeasureDuration();
        //if (g_fAutoBeam)              //TODO
        //    AutoBeam(nVoice);
    }
}

void lmSegment::UpdateMeasureDuration()
{
    //determines the new duration of this segment, and updates barline timepos, if
    //barline exists

    //if segment is empty, duration is zero
    if (!m_pLastSO)
    {
        m_rMaxTime = 0.0f;
        return;
    }

    //move backwards form last object to skip barline, if exists
    float rDuration = 0.0f;
    if (m_pLastSO->IsBarline())
    {
        lmSOIterator it(m_pOwner, m_pLastSO);
        it.MovePrev();
        lmStaffObj* pSO = it.GetCurrent();
	    if (!it.ChangeOfMeasure() && pSO != m_pLastSO)
            rDuration = pSO->GetTimePos() + pSO->GetTimePosIncrement();
    }
    m_rMaxTime = rDuration;

    //if segment's duration is irregular attach a warning tag to it
    //TODO: if segment's duration is irregular attach a warning tag to it"));

    //Update barline timepos if barline exists.
    if (m_pLastSO->IsBarline())
        m_pLastSO->SetTimePos( m_rMaxTime );
}

lmBarline* lmSegment::GetBarline()
{
    //returns the barline of the segment, if exists, or null
    //The barline is always the last staffobj in the segment's collection

    if (m_pLastSO && m_pLastSO->IsBarline())
        return (lmBarline*)m_pLastSO;
    else
        return (lmBarline*)NULL;
}

void lmSegment::UpdateDuration()
{
    if (!m_pLastSO)
        m_rMaxTime = 0.0f;          //segment is empty
    else
        m_rMaxTime = m_pLastSO->GetTimePos() + m_pLastSO->GetTimePosIncrement();
}

float lmSegment::GetMaximumTime()
{
    //returns theoretical segment duration for current time signature

    //get last SO in this segment
    if (!m_pLastSO)
        return lmNO_TIME_LIMIT;

    //get applicable time signature
    lmTimeSignature* pTS = m_pLastSO->GetApplicableTimeSignature();
    if (!pTS)
        return lmNO_TIME_LIMIT;   //no time signature -> infinite segment

    return pTS->GetMeasureDuration();
}

bool lmSegment::HasBarline()
{
    return (m_pLastSO ? m_pLastSO->IsBarline() : false);
}

bool lmSegment::IsSegmentFull()
{
    //determine if segment is full
    float rMeasureDuration = GetMaximumTime();
    return !IsLowerTime(m_rMaxTime, rMeasureDuration);
}

wxString lmSegment::Dump()
{
    wxString sDump = wxString::Format(_T("\nSegment %d\n"), m_nNumSegment+1);
    for (int i=0; i < lmMAX_STAFF; i++)
    {
        if (m_pContext[i]) {
            sDump += m_pContext[i]->DumpContext();
        }
        else
            sDump += _T("Context: NULL\n");
    }
    sDump += _T("\n");

    lmSOIterator it(m_pOwner, m_pFirstSO);
    while(!it.EndOfCollection() && !it.ChangeOfMeasure())
    {
        lmStaffObj* pSO = it.GetCurrent();
        sDump += pSO->Dump();
        it.MoveNext();
    }

    sDump += wxString::Format(_T("\nNum. objects %d. Bwd chain:\n"), m_nNumSO);
    it.MoveTo(m_pLastSO);
    while(!it.EndOfCollection() && !it.ChangeOfMeasure())
    {
        lmStaffObj* pSO = it.GetCurrent();
        sDump += wxString::Format(_T("%d, "), pSO->GetID());
        it.MovePrev();
    }
	return sDump;
}

void lmSegment::DoContextInsertion(lmKeySignature* pNewKey, lmStaffObj* pNextSO,
                                   bool fKeyKeepPitch)
{
    //When a key signature is added to the collection, the created contexts are already
    //chained in the staves  [This was acomplished at VStaff::Cmd_InsertXxxxx method,
    //when creating the new contexts() ].
    //But it is necessary:
    //- to update pointers to contexts at start of segment; and
    //- to update staffobjs in segment, if affected by the context change


    //Update current segment notes, if requested.
    //if pNextSO is NULL it means that we are at end of segment and, therefore, there are
    //no notes in this segment after the inserted key
    lmKeySignature* pOldKey = pNewKey->GetApplicableKeySignature();
    if (pNextSO)
    {
        if (fKeyKeepPitch)
            AddRemoveAccidentals(pNewKey, pNextSO);
        else
            ChangePitch(pOldKey, pNewKey, pNextSO);
    }

    //determine staves affected by the context change and propagate context change
    //to all following segments. This is necessary so that segments can update pointers
    //to start of segment applicable contexts, and update their contained staffobjs, if
    //affected by the change.
    lmSegment* pNextSegment = GetNextSegment();
    if (pNextSegment)
    {
        //key signature: all staves
        for (int nStaff=1; nStaff <= lmMAX_STAFF; nStaff++)
        {
            //determine context for current staff at end of segment
            lmContext* pLastContext = FindEndOfSegmentContext(nStaff);

            //inform next segment
            pNextSegment->PropagateContextChange(pLastContext, nStaff,
                                                 pNewKey, pOldKey, fKeyKeepPitch);
        }
    }

}

void lmSegment::DoContextInsertion(lmClef* pNewClef, lmStaffObj* pNextSO,
                                   bool fClefKeepPosition)
{
    //When a clef is added to the collection, the created contexts are already chained
    //in the staves  [This was acomplished at VStaff::Cmd_InsertXxxxx method, when
    //creating the new contexts() ].
    //But it is necessary:
    //- to update pointers to contexts at start of segment; and
    //- to update staffobjs in segment, if affected by the context change


    //Update current segment notes, if requested.
    //if pNextSO is NULL it means that we are at end of segment and, therefore, there are
    //no notes in this segment after the inserted clef
    lmClef* pOldClef = pNewClef->GetApplicableClef();
    lmEClefType nOldClefType;
    if (!pOldClef)
    {
        int nStaff = pNewClef->GetStaffNum();
        lmStaff* pStaff = m_pOwner->GetOwnerVStaff()->GetStaff(nStaff);
        nOldClefType = pStaff->GetPreviousFirstClefType();
    }
    else
        nOldClefType = pOldClef->GetClefType();

    if (pNextSO && fClefKeepPosition)
    {
        int nStaff = pNewClef->GetStaffNum();
        Transpose(pNewClef->GetClefType(), nOldClefType, pNextSO, nStaff);
    }

    //determine staves affected by the context change and propagate context change
    //to all following segments. This is necessary so that segments can update pointers
    //to start of segment applicable contexts, and update their contained staffobjs, if
    //affected by the change.
    lmSegment* pNextSegment = GetNextSegment();
    if (pNextSegment)
    {
        //just one staff. Determine context for this staff (end of previous segment context)
        int nStaff = pNewClef->GetStaffNum();
        lmContext* pLastContext = FindEndOfSegmentContext(nStaff);

        //inform next segment
        pNextSegment->PropagateContextChange(pLastContext, nStaff, pNewClef->GetClefType(),
                                             nOldClefType, fClefKeepPosition);
    }
}

void lmSegment::PropagateContextChange(lmContext* pStartContext, int nStaff,
                                       lmEClefType nNewClefType, lmEClefType nOldClefType,
                                       bool fClefKeepPosition)
{
    //The context for staff nStaff at start of this segment has changed because a clef has been
    //added or removed.
    //This method updates pointers to contexts at start of segment and, if requested, updates
    //the contained staffobjs if affected by the change.
    //Then, propagates the change to next segment if the start of segment context pointer
    //has been updated.

    wxASSERT(nStaff > 0);

    if (m_pContext[nStaff-1] == pStartContext && !fClefKeepPosition)
        return;     //nothing to do. Context is updated and no need to transpose notes

    //update context pointer
    m_pContext[nStaff-1] = pStartContext;

    //Update current segment notes, if requested
    if (fClefKeepPosition && nNewClefType != lmE_Undefined)
        Transpose(nNewClefType, nOldClefType, (lmStaffObj*)NULL, nStaff);

    ////mark context as processed (not modified)
    //m_pContext[nStaff-1]->SetModified(false);

    //propagate to next segment
    lmSegment* pNextSegment = GetNextSegment();
    if (pNextSegment)
        pNextSegment->PropagateContextChange(pStartContext, nStaff, nNewClefType, nOldClefType,
                                             fClefKeepPosition);
}

void lmSegment::PropagateContextChange(lmContext* pStartContext, int nStaff,
                                       lmKeySignature* pNewKey, lmKeySignature* pOldKey,
                                       bool fKeyKeepPitch)
{
    //The context for staff nStaff at start of this segment has changed because a key signature
    //has been added or removed.
    //This method updates pointers to contexts at start of segment and, if requested, updates
    //the contained staffobjs if affected by the change.
    //Then, propagates the change to next segment if the start of segment context pointer
    //has been updated.

    wxASSERT(nStaff > 0);

    //if (!m_pContext[nStaff-1]->IsModified())
    //    return;     //nothing to do. Context are updated

    //update context pointer
    m_pContext[nStaff-1] = pStartContext;

    //Update current segment notes, if requested
    if (fKeyKeepPitch)
        AddRemoveAccidentals(pNewKey, (lmStaffObj*)NULL);
    else
        ChangePitch(pOldKey, pNewKey, (lmStaffObj*)NULL);

    ////mark context as processed (not modified)
    //m_pContext[nStaff-1]->SetModified(false);

    //propagate to next segment
    lmSegment* pNextSegment = GetNextSegment();
    if (pNextSegment)
        pNextSegment->PropagateContextChange(pStartContext, nStaff, pNewKey, pOldKey,
                                             fKeyKeepPitch);
}

void lmSegment::PropagateContextChange(lmContext* pStartContext, int nStaff)
{
    //The context for staff nStaff at start of this segment has changed because a time signature
    //has been added or removed.
    //This method updates pointers to contexts at start of segment. Then, propagates
    //the change to next segment if the start of segment context pointer has been updated.

    wxASSERT(nStaff > 0);

    if (m_pContext[nStaff-1] == pStartContext)
        return;     //nothing to do. Context are updated

    //update context pointer
    m_pContext[nStaff-1] = pStartContext;

    //propagate to next segment
    lmSegment* pNextSegment = GetNextSegment();
    if (pNextSegment)
        pNextSegment->PropagateContextChange(pStartContext, nStaff);
}

void lmSegment::Transpose(lmEClefType nNewClefType, lmEClefType nOldClefType,
                          lmStaffObj* pStartSO, int nStaff)
{
    //A clef has been inserted. Iterate along the staffobjs of this segment and re-pitch
    //the notes to maintain its staff position.

    wxASSERT(nStaff > 0);

    //locate start point
    lmStaffObj* pFirst = (pStartSO ? pStartSO : m_pFirstSO);
    if (!pFirst) return;

    //iterate until end of segment or new cleft
    lmSOIterator it(m_pOwner, pFirst);
	while (!it.ChangeOfMeasure() && !it.EndOfCollection() && !it.GetCurrent()->IsClef())
	{
        lmStaffObj* pSO = it.GetCurrent();
		if (pSO->IsNoteRest() && pSO->GetStaffNum() == nStaff &&
            ((lmNote*)pSO)->IsNote())
        {
            //note in the affected staff. Re-pitch it to keep staff position
            ((lmNote*)pSO)->ModifyPitch(nNewClefType, nOldClefType);
        }
        it.MoveNext();
	}
}

void lmSegment::AddRemoveAccidentals(lmKeySignature* pNewKey, lmStaffObj* pStartSO)
{
    //A key signature has been added/removed or changed. As a consequence, the pitch of
    //notes after the key signature might be affected. User has requested to add/remove
    //accidentals to those notes, so that their pitch doesn't change.
    //This method iterates along the staffobjs of this segment and add/remove accidentals
    //to the notes to maintain their pitch.

    // we need to know:
    // - first affected object: object after deleted key, inserted key, changed key
    // - new context to apply (new key signature)
    // - what to do if no key signature ? -> do not change anything

    if (!pNewKey) return;       //Nothing to do

    //determine new applicable accidentals
    int nAccidentals[7];
    ComputeAccidentals(pNewKey->GetKeyType(), nAccidentals);

    //locate start point
    lmStaffObj* pFirst = (pStartSO ? pStartSO : m_pFirstSO);
    if (!pFirst) return;

    //iterate until end of segment or new key
    lmSOIterator it(m_pOwner, pFirst);
	while (!it.ChangeOfMeasure() && !it.EndOfCollection() && !it.GetCurrent()->IsKeySignature())
	{
        lmStaffObj* pSO = it.GetCurrent();
		if (pSO->IsNoteRest() && ((lmNote*)pSO)->IsNote())
        {
            //note. Review displayed accidentals
            int nStep = ((lmNote*)pSO)->GetStep();
            ((lmNote*)pSO)->ComputeAccidentalsToKeepPitch(nAccidentals[nStep]);
        }
        it.MoveNext();
	}
}

void lmSegment::ChangePitch(lmKeySignature* pOldKey, lmKeySignature* pNewKey,
                            lmStaffObj* pStartSO)
{
    //A key signature has been added/removed or changed. As a consequence, the pitch of
    //notes after the key signature might be affected. User has requested to change the
    //pitch of affected notes, as specified by the new key signature
    //This method iterates along the staffobjs of this segment and changes pitch of
    //affected notes.

    // we need to know:
    // - first affected object: object after deleted key, inserted key, changed key
    // - old and new key signatures that apply.
    // - what to do if no new key signature or no old key signature? -> do not change anything

    if (!pNewKey || !pOldKey) return;       //Nothing to do

    //determine increment/decrement of pitch for each step
    int nNewAcc[7], nOldAcc[7], nPitchDiff[7];
    ComputeAccidentals(pNewKey->GetKeyType(), nNewAcc);
    ComputeAccidentals(pOldKey->GetKeyType(), nOldAcc);
    for (int i=0; i < 7; i++)
        nPitchDiff[i] = nNewAcc[i] - nOldAcc[i];

    //locate start point
    lmStaffObj* pFirst = (pStartSO ? pStartSO : m_pFirstSO);
    if (!pFirst) return;

    //iterate until end of segment or new key
    lmSOIterator it(m_pOwner, pFirst);
	while (!it.ChangeOfMeasure() && !it.EndOfCollection() && !it.GetCurrent()->IsKeySignature())
	{
        lmStaffObj* pSO = it.GetCurrent();
		if (pSO->IsNoteRest() && ((lmNote*)pSO)->IsNote())
        {
            //note. Review displayed accidentals
            int nStep = ((lmNote*)pSO)->GetStep();
            if (nPitchDiff[nStep] != 0)
            {
                //change pitch by the difference and compute new accidentals to display
                ((lmNote*)pSO)->ModifyPitch(nPitchDiff[nStep]);
                ((lmNote*)pSO)->ComputeAccidentalsToKeepPitch(nNewAcc[nStep]);
            }
        }
        it.MoveNext();
	}
}

void lmSegment::AutoBeam(int nVoice)
{
    //A note/rest in voice nVoice has been added/removed in this segment. Review beaming
    //in specified voice

    //vector to store notes that form a beam
    std::list<lmNoteRest*> cBeamedNotes;

    //find first note in voice nVoice in this segment
    if (!m_pFirstSO) return;

    lmSOIterator it(m_pOwner, m_pFirstSO);
    lmStaffObj* pSO = (lmStaffObj*)NULL;
    while (!it.ChangeOfMeasure() && !it.EndOfCollection())
	{
        pSO = it.GetCurrent();
		if (pSO->IsNoteRest() &&
            ((lmNoteRest*)pSO)->IsNote() &&
            ((lmNoteRest*)pSO)->GetVoice() == nVoice)
            break;
        it.MoveNext();
    }

    if (!pSO)
        //no notes in voice nVoice. Only rests. Nothing to do
        return;

    //get context and get current time signature.
    lmContext* pContext = pSO->GetCurrentContext();
    if (!pContext) return;           //no context yet
    lmTimeSignature* pTS = pContext->GetTime();
    if (!pTS) return;               //no time signature defined yet
    int nBeatType = pTS->GetBeatType();
    int nBeats = pTS->GetNumBeats();

   //Explore all segment (only involved voice) and select notes/rests that could be beamed
    while (!it.ChangeOfMeasure() && !it.EndOfCollection())
	{
        pSO = it.GetCurrent();

        //ignore notes in chord that are not base of chord
		if (pSO->IsNoteRest()
            && ((lmNoteRest*)pSO)->GetVoice() == nVoice
            && ( ((lmNoteRest*)pSO)->IsRest()
                 || !((lmNote*)pSO)->IsInChord()
                 || ((lmNote*)pSO)->IsBaseOfChord() ))
        {
            //note/rest in the affected voice
            lmNoteRest* pNR = (lmNoteRest*)pSO;
            int nPos = GetNoteBeatPosition(pNR->GetTimePos(), nBeats, nBeatType);
            if (nPos != lmOFF_BEAT)     //nPos = lmOFF_BEAT or number of beat (0..n)
            {
                //A new beat could start. Terminate previous beam and start a new one
                AutoBeam_CreateBeam(cBeamedNotes);
                cBeamedNotes.clear();
            }

            //add note to current beam if smaller than an eighth.
            //if note, it must not be in chord or must be base of chord.
            //Ignore rests if cNeamedNotes is empty, as a beam cannot start by rest
            if (pNR->GetNoteType() > eQuarter 
                && (pNR->IsNote() || cBeamedNotes.size() > 0))
            {
                cBeamedNotes.push_back(pNR);
            }
            else
            {
                //current note/rest cannot be beamed. Crete the beam with previous notes
                AutoBeam_CreateBeam(cBeamedNotes);
                cBeamedNotes.clear();
            }
        }
        it.MoveNext();
	}

    //terminate last beam
    AutoBeam_CreateBeam(cBeamedNotes);
    cBeamedNotes.clear();
}

void lmSegment::AutoBeam_CreateBeam(std::list<lmNoteRest*>& cBeamedNotes)
{
    //When this method is invoked, list cBeamedNotes contains the notes/rests that should
    //be beamed toghether. Review current beam information and modify it so that they
    //form a beamed group.

    if (cBeamedNotes.size() == 0) return;

    //remove rests at the end
    int nLastNote = (int)cBeamedNotes.size() - 1;       //0..n-1
    std::list<lmNoteRest*>::iterator it = cBeamedNotes.end();
    for (--it; nLastNote > 0 && (*it)->IsRest(); --it, --nLastNote);

    if (nLastNote < 1) return;      //only one note. No beam possible

    //Real beam information will be computed by the beam, when method AutoSetUp is invoked.
    //Therefore, We do not have to worry here about the beam details. So, it is enough to
    //create the beam and to add the notes to the beam.

    lmBeam* pBeam = (lmBeam*)NULL;
    it = cBeamedNotes.begin();
    for (int i=0; it != cBeamedNotes.end() && i <= nLastNote; ++it, ++i)
    {
        //remove old beam
        //OPTIMIZATION: preserve current beam information when beam exists and is correct.
        if ((*it)->IsBeamed())
        {
            lmBeam* pBeam = (*it)->GetBeam();
            pBeam->Remove(*it);
		    if (pBeam->NumNotes() <= 1)
			    delete pBeam;
        }

        //add to new one
        if (it == cBeamedNotes.begin())
            pBeam = (*it)->IncludeOnBeam(eBeamBegin);
        else
            (*it)->IncludeOnBeam((i == nLastNote ? eBeamEnd : eBeamContinue), pBeam);
    }
}

lmContext* lmSegment::FindEndOfSegmentContext(int nStaff)
{
    //A new staffobj has introduced a new context in the pointed staff. This method goes
    //backwards, looking for the context applicable at the end of this segment, that is,
    //the context applicable to first staffobj of next segment, and returns it.

    wxASSERT(nStaff > 0);

    lmSOIterator it(m_pOwner, m_pLastSO);
    while (!it.ChangeOfMeasure() && !it.EndOfCollection())
	{
        lmStaffObj* pSO = it.GetCurrent();
        if (pSO->IsClef() && pSO->GetStaffNum() == nStaff)
            return ((lmClef*)pSO)->GetContext();
        else if (pSO->IsTimeSignature())
            return ((lmTimeSignature*)pSO)->GetContext(nStaff);
        else if (pSO->IsKeySignature())
            return ((lmKeySignature*)pSO)->GetContext(nStaff);
        else
            it.MovePrev();
    }

    return m_pContext[nStaff-1];
}

void lmSegment::DoContextRemoval(lmKeySignature* pOldKey, lmStaffObj* pNextSO, bool fKeyKeepPitch)
{
    //When a key signature is removed from the collection, the contexts that it created
    //has been removed from the staves contexts chain. But it is necessary:
    //- to update pointers to contexts at start of segment; and
    //- to update staffobjs in segment, if affected by the context change


    //Update current segment notes, if requested.
    //if pNextSO is NULL it means that we are at end of segment and, therefore, there are
    //no notes in this segment after the deleted key
    lmKeySignature* pNewKey = pOldKey->GetApplicableKeySignature();
    if (pNextSO)
    {
        if (fKeyKeepPitch)
            AddRemoveAccidentals(pOldKey, pNextSO);
        else
            ChangePitch(pOldKey, pNewKey, pNextSO);
    }

    //determine staves affected by the context change and propagate context change
    //to all following segments. This is necessary so that segments can update pointers
    //to start of segment applicable contexts, and update their contained staffobjs, if
    //affected by the change.
    lmSegment* pNextSegment = GetNextSegment();
    if (pNextSegment)
    {
        //key signature: all staves affected
        for (int nStaff=1; nStaff <= lmMAX_STAFF; nStaff++)
        {
            //determine context for current staff at end of segment
            lmContext* pLastContext = FindEndOfSegmentContext(nStaff);

            //inform next segment
            pNextSegment->PropagateContextChange(pLastContext, nStaff, pNewKey,
                                                 pOldKey, fKeyKeepPitch);
        }
    }
}

void lmSegment::DoContextRemoval(lmClef* pOldClef, lmStaffObj* pNextSO, bool fClefKeepPosition)
{
    //When a clef is removed from the collection, the contexts it created
    //has been removed from the staves contexts chain. But it is necessary:
    //- to update pointers to contexts at start of segment; and
    //- to update staffobjs in segment, if affected by the context change


    //Update current segment notes, if requested.
    //if pNextSO is NULL it means that we are at end of segment and, therefore, there are
    //no notes in this segment after the deleted clef
    lmClef* pNewClef = pOldClef->GetApplicableClef();
    lmEClefType nNewClefType = pOldClef->GetApplicableClefType();
    if (pNextSO && fClefKeepPosition)
    {
        int nStaff = pNextSO->GetStaffNum();
        if (nNewClefType != lmE_Undefined)
            Transpose(nNewClefType, pOldClef->GetClefType(), pNextSO, nStaff);
    }

    //determine staves affected by the context change and propagate context change
    //to all following segments. This is necessary so that segments can update pointers
    //to start of segment applicable contexts, and update their contained staffobjs, if
    //affected by the change.
    lmSegment* pNextSegment = GetNextSegment();
    if (pNextSegment)
    {
        //just one staff. Determine context for this staff at end of segment
        int nStaff = pOldClef->GetStaffNum();
        lmContext* pLastContext = FindEndOfSegmentContext(nStaff);

        //inform next segment
        pNextSegment->PropagateContextChange(pLastContext, nStaff, nNewClefType,
                                             pOldClef->GetClefType(), fClefKeepPosition);
    }
}

void lmSegment::SortMeasure()
{
    //Sort measure to ensure right order
    //Bubble method. Optimized algorithm (from http://en.wikipedia.org/wiki/Bubble_sort)
    //
    //  procedure bubbleSort( A : list of sortable items ) defined as:
    //  n := length( A )
    //  do
    //      swapped := false
    //      n := n - 1
    //      for each i in 0 to n  do:
    //      if A[ i ] > A[ i + 1 ] then
    //          swap( A[ i ], A[ i + 1 ] )
    //          swapped := true
    //      end if
    //      end for
    //  while swapped
    //  end procedure

    bool fSwapped = true;
    bool fUpdateFirstInCollection = (m_pFirstSO == m_pOwner->GetFirstSO());
    bool fUpdateLastInCollection = (m_pLastSO == m_pOwner->GetLastSO());
    int n = m_nNumSO;
    while(fSwapped)
    {
        fSwapped = false;
        --n;
        lmStaffObj* pSO1 = m_pFirstSO;
        for (int i=0; i < n; i++)
        {
            lmStaffObj* pSO2 = pSO1->GetNextSO();
            if (SortCompare(pSO2, pSO1))
            {
                //Swap(pSO1, pSO2);
                lmStaffObj* pPrev = pSO1->GetPrevSO();
                lmStaffObj* pNext = pSO2->GetNextSO();
                if (m_pFirstSO == pSO1)
                {
                    m_pFirstSO = pSO2;
                    if (fUpdateFirstInCollection)
                        m_pOwner->SetFirstSO(pSO2);
                }
                if (pPrev)
                    pPrev->SetNextSO(pSO2);
                pSO2->SetPrevSO(pPrev);
                pSO2->SetNextSO(pSO1);
                pSO1->SetPrevSO(pSO2);
                pSO1->SetNextSO(pNext);
                if (pNext)
                    pNext->SetPrevSO(pSO1);
                if (m_pLastSO == pSO2)
                {
                    m_pLastSO = pSO1;
                    if (fUpdateLastInCollection)
                        m_pOwner->SetLastSO(pSO1);
                }

                fSwapped = true;
            }
            else
                pSO1 = pSO2;
        }
    }
}

void lmSegment::ShiftRightTimepos(lmStaffObj* pStartSO, float rTimeShift)
{
    //Either, the note/rest pointed by pStartSO has been inserted, or its duration has been
    //increased.
    //In both cases, we must shift the timepos af all next objects in this voice, starting with
    //staffobj after pStartSO, and up to the barline (included).
    //The amount of time to shift objects right is rTimeShift. This value is either the duration
    //of the inserted note (if a note inserted) or the increment in duration (positive) if
    //the duration of pStartSO note/rest has been modified.

    //Algorithm:
    //  Parameters: pStartSO (inserted/modified object)
    //
    //    1 Asign: iV = Voice(pStartSO)
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


    if (IsEqualTime(rTimeShift, 0.0f)) return;       //nothing to do

    //increment in duration must be always positive
    wxASSERT(IsHigherTime(rTimeShift, 0.0f));
    //StartSO must be a NoteRest
    wxASSERT(pStartSO->IsNoteRest());

    bool fChanges = false;
    int iV = ((lmNoteRest*)pStartSO)->GetVoice();
    lmSOIterator it(m_pOwner, pStartSO);
    it.MoveNext();
    it.ResetFlags();
	while (!it.ChangeOfMeasure() && !it.EndOfCollection())
	{
        lmStaffObj* pSO = it.GetCurrent();
        if (pSO->IsNoteRest() && ((lmNoteRest*)pSO)->GetVoice() == iV)
        {
            float rTime = pSO->GetTimePos();
            float rDuration = pSO->GetTimePosIncrement();
            if (m_pOwner->IsTimePosOccupied(this, rTime, rDuration, iV))
            {
                pSO->SetTimePos(rTime + rTimeShift);
                fChanges = true;
            }
            else
                break;
        }
        it.MoveNext();
	}

    //if any change, update measure duration and review AutoBeam
    if (fChanges)
    {
        UpdateMeasureDuration();
        //if (g_fAutoBeam)          //TODO
        //    AutoBeam(iV);
    }
}

void lmSegment::SetCollection(lmStaffObj* pFirstSO, lmStaffObj* pLastSO)
{
    //A barline is added to the collection. As consequence, the segment is splitted. This
    //method on the new created segment is invoked to do whatever is necessary. 
    //All staffobjs, from pFirstSO to pLastSO are already chained and ordered.

    wxASSERT(pFirstSO);
    wxASSERT(pLastSO);

    m_pFirstSO = pFirstSO;
    m_pLastSO = pLastSO;
    CountObjects();

    //compute segment duration
    UpdateDuration();

    //Re-assign time to all objects and update segment pointer.
    float rTime = pFirstSO->GetTimePos();
    if (IsHigherTime(rTime, 0.0f))
    {
        lmStaffObj* pSO = m_pFirstSO;
        while(pSO)
        {
            pSO->SetTimePos( pSO->GetTimePos() - rTime );
            pSO->SetSegment(this);
            pSO = pSO->GetNextSO();
        }
    }
}

void lmSegment::FinishSegmentAt(lmStaffObj* pLastSO)
{
    //A barline is added to the collection. As consequence, the segment is splitted. This
    //method on the old segment (the one in which the barline has been inserted) is 
    //invoked to do whatever is necessary. 

    wxASSERT(pLastSO);

    m_pLastSO = pLastSO;
    CountObjects();

    //compute segment duration
    UpdateDuration();
}

float lmSegment::JoinSegment(lmSegment* pSegment)
{
    //A barline is removed from the collection. As consequence, this segment must be
    //joined with next one (pSegment). This method is invoked to do whatever is necessary.
    //All staffobjs are already chained and ordered.
    //Returns the time increment added to first object of appended segment


    lmStaffObj* pFirst = pSegment->GetFirstSO();
    lmStaffObj* pStart = m_pLastSO;
    m_pLastSO = pSegment->GetLastSO();
    CountObjects();

    //Adjust timepos and update segment pointer in all moved objects
	//Re-assign time to all appended objects and chage ptr to segment.
    float rTime = pStart->GetTimePos() + pStart->GetTimePosIncrement();
    while(pFirst)
    {
        pFirst->SetTimePos( pFirst->GetTimePos() + rTime );
        pFirst->SetSegment(this);
        pFirst = pFirst->GetNextSO();
    }

    //re-compute segment duration
    UpdateDuration();

    return rTime;
}

void lmSegment::CountObjects()
{
    //count the number of objects in the collection

    m_nNumSO = 0;
    if (!m_pFirstSO) return;

    lmSOIterator it(m_pOwner, m_pFirstSO);
	while (!it.ChangeOfMeasure() && !it.EndOfCollection())
	{
        ++m_nNumSO;
        it.MoveNext();
    }
}

bool lmSegment::IsTimePosOccupied(float rTime, float rDuration, int nVoice)
{
    //returns true if there is any note/rest in voice nVoice sounding in semi-open interval
    //[timepos, timepos+duration), in current measure.

    if (!m_pFirstSO) return false;

    //Is time occupied if
    //  coincides:      pSO(timepos) == rTime or
    //  start before:   pSO(timepos) < rTime && pSO(timepos+duration) > rTime
    //  start after:    pSO(timepos) > rTime && pSO(timepos) < (rTime+rDuration)

    lmSOIterator it(m_pOwner, m_pFirstSO);
	while (!it.ChangeOfMeasure() && !it.EndOfCollection())
	{
    	lmStaffObj* pSO = it.GetCurrent();
        if (pSO->IsNoteRest() && ((lmNoteRest*)pSO)->GetVoice() == nVoice)
        {
            float rTimeSO = pSO->GetTimePos();
            if (IsEqualTime(rTimeSO, rTime)) return true;      //start coincides

            float rDurSO = pSO->GetTimePosIncrement();
            if (rTimeSO < rTime && (rTimeSO + rDurSO - 0.1f) > rTime) return true;

            if (rTimeSO > rTime && rTimeSO < (rTime + rDuration)) return true;

            if (rTimeSO > rTime && (rTimeSO + rDurSO) > rTime) return false;

        }
        it.MoveNext();
    }

    return false;
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

    //initialize the collection
    m_pFirstSO = (lmStaffObj*)NULL;
    m_pLastSO = (lmStaffObj*)NULL;

	//create a first segment
	CreateNewSegment(0);
}

lmColStaffObjs::~lmColStaffObjs()
{
    //delete StaffObjs in the collection
    lmStaffObj* pSO = m_pFirstSO;
    while(pSO)
    {
        lmStaffObj* pNextSO = pSO->GetNextSO();
        delete pSO;
        pSO = pNextSO;
    }

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
    wxASSERT((int)m_Segments.size()==1 && m_Segments[0]->GetNumObjects() == 0);

    m_nNumStaves++;
}

int lmColStaffObjs::AssignVoice(lmStaffObj* pSO, int nSegment)
{
    //assigns voice (1..n) to the staffobj.

    wxASSERT(pSO->IsNoteRest());     //only note/rests have voice

    //Auto-voice algorithm. Rules:
    //  1. if a measure is not full do not change from voice 1 (from voice assigned to this staff)
    //  2. if a note is in staff 1 assign voice 1 (assign voice = num.staff if possible)
    //  3. if in staff 2 do not use voice 1 (do not use voices assigned to other staves)

    //rule 1: if measure is not full do not change from voice assigned to this staff
    int iV = pSO->GetStaffNum();
    if (!m_Segments[nSegment]->IsSegmentFull())
        return iV;

    //Determine if there are note/rests in timepos range [timepos, timepos+object_duration)

    //Check if default staff voice is suitable
    bool fOccupied = IsTimePosOccupied(m_Segments[nSegment], pSO->GetTimePos(),
                                    pSO->GetTimePosIncrement(), iV);

    //else, loop to find empty voice in this timepos range
    for(; fOccupied && iV <= lmMAX_VOICE; iV++)
    {
        if (m_Segments[nSegment]->IsVoiceUsed(iV))
            fOccupied = IsTimePosOccupied(m_Segments[nSegment], pSO->GetTimePos(),
                                    pSO->GetTimePosIncrement(), iV);
    }

    //if no empty voice found, start a new voice.
    if (fOccupied)
        iV = m_Segments[nSegment]->StartNewVoice();

    //done
    return iV;
}

bool lmColStaffObjs::IsTimePosOccupied(lmSegment* pSegment, float rTime, float rDuration,
                                       int nVoice)
{
    //returns true if there is any note/rest in voice nVoice sounding in semi-open interval
    //[timepos, timepos+duration), in current measure.

	return pSegment->IsTimePosOccupied(rTime, rDuration, nVoice);
}

void lmColStaffObjs::AssignTime(lmStaffObj* pSO)
{
	// Assigns a timepos to the StaffObj

	//if it is a note in chord and not base note assign it the time assigned to
	//base note
	bool fPartOfChord = false;		//to avoid having to check this many times
    if (pSO->GetClass() == eSFOT_NoteRest && (((lmNoteRest*)pSO)->IsNote())
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

void lmColStaffObjs::Add(lmStaffObj* pNewSO, bool fClefKeepPosition, bool fKeyKeepPitch)
{
    //AWARE:
    //  This methods is intended to be used for normal add/insert operations, that is,
    //  for operations not related to undo/redo.
    //
    //  Assign voice and timepos to the received StaffObj, and store it in the collection,
	//  before object pointed by current cursor, and advance cursor after inserted object.
    //  Manage all details to maintain segments.

	//get segment
	int nSegment = m_pVCursor->GetSegment();

    //assign timepos and voice to the StaffObj
	AssignTime(pNewSO);

    // if it is a note/rest without voice assigned, assign it a voice
    if (pNewSO->IsNoteRest() && ((lmNoteRest*)pNewSO)->GetVoice() == 0)
    {
	    int iV = AssignVoice(pNewSO, nSegment);
        ((lmNoteRest*)pNewSO)->SetVoice(iV);
    }

	//store new object in the collection
    lmStaffObj* pCursorObj = m_pVCursor->GetStaffObj();
	Store(pNewSO, fClefKeepPosition, fKeyKeepPitch);

    //if pNewSO is a time signature, Store() could call Autorebar, and this will
    //invalidate pSgement in VCursor. We have to update it
    if (pNewSO->IsTimeSignature())
    {
        if (pCursorObj)
            m_pVCursor->MoveCursorToObject(pCursorObj);
        //else
            //TODO
    }

    //advance cursor
	lmStaffObj* pCursorSO = m_pVCursor->GetStaffObj();
    if (!pCursorSO)
    {
        //Cursor is pointing to end of collection. So no need to advance cursor iterator,
        //just to advance time
        if (pNewSO->IsBarline())
        {
            //Advance cursor to time 0 in next segment
            m_pVCursor->AdvanceToNextSegment();
        }
        else
        {
            //We are at end of score. Advance cursor to time t + duration of inserted object
            m_pVCursor->AdvanceToTime( pNewSO->GetTimePos() + pNewSO->GetTimePosIncrement() );
        }
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
        {
            //Cursor is pointing to object after the inserted one but could be of different
            //voice. And cursor timepos is not updated as the collection has been altered.
            //So we have to reposition cursor at right time and right object.
            //Advance cursor to time t + duration of inserted object or to next measure
            //if we are at end of measure
            float rTime = pNewSO->GetTimePos() + pNewSO->GetTimePosIncrement();
            if (!IsLowerTime(rTime, m_Segments[nSegment]->GetMaximumTime())
                && nSegment < GetNumSegments()-1 )
                m_pVCursor->AdvanceToNextSegment();
            else
                m_pVCursor->MoveToTime(rTime);
        }
    }
}

void lmColStaffObjs::Insert(lmStaffObj* pNewSO, lmStaffObj* pBeforeSO)
{
    //AWARE:
    //  This method is intended only for undo/redo, to re-insert deleted objects when user
    //  request to undo an action.

    //set cursor
    m_pVCursor->MoveCursorToObject(pBeforeSO);

    //insert at specified point
    Store(pNewSO);
}

void lmColStaffObjs::Store(lmStaffObj* pNewSO, bool fClefKeepPosition, bool fKeyKeepPitch)
{
	//add pNewSO into the segment's collection.
	int nSegment = m_pVCursor->GetSegment();
	m_Segments[nSegment]->Store(pNewSO, m_pVCursor);

	//if barline added manage segment split/creation
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
            CreateNewSegment(nSegment);
        }
    }

    //If added staffobj is a note/rest and AutoBeam is on, apply auto-beam to current voice
    if (pNewSO->IsNoteRest() && g_fAutoBeam)
    {
        m_Segments[nSegment]->AutoBeam( ((lmNoteRest*)pNewSO)->GetVoice() );
    }

    //if time signature added, re-bar the collection. As this implies removing segments and
    //creating new ones, all needed context propagation and update, is done at the same time.
    //Therefore, no later steps are necessary for this
    //If there are no objects after the added TS, nothing to re-bar
    if (pNewSO->IsTimeSignature() && pCursorSO)
    {
        //we have to re-bar from current segment to next time signature.
        lmStaffObj* pLastSO = FindFwdTimeSignature(pCursorSO);
        if (pLastSO)
            pLastSO = FindPrevStaffObj(pLastSO);

        AutoReBar(pCursorSO, pLastSO, (lmTimeSignature*)pNewSO);
    }

    //If inserted staffobj created contexts (clef, TS or KS) the created contexts are already
    //chained in the staves context chain. But it is necessary:
    //- to update pointers to contexts at start of segment; and
    //- to update staffobjs in segment, if affected by the context change
    //But for time signatures, all needed context propagation and update is done when
    //doing the Re-Bar operation. So no need to do anything here.
    if (pNewSO->IsKeySignature())
        m_Segments[nSegment]->DoContextInsertion((lmKeySignature*)pNewSO, pCursorSO,
                                                 fKeyKeepPitch);
    else if (pNewSO->IsClef())
        m_Segments[nSegment]->DoContextInsertion((lmClef*)pNewSO, pCursorSO, fClefKeepPosition);
}

void lmColStaffObjs::Delete(lmStaffObj* pSO, bool fDelete, bool fClefKeepPosition,
                            bool fKeyKeepPitch)
{
	// Delete the StaffObj pointed by pSO.
    // - If 'fDelete' is true the object destructor is invoked. This flag is used to avoid
    // deleting the object when Delete() requires Undo/Redo capabilities.
    // - Flags fClefKeepPosition and fKeyKeepPitch are only meaninful when Delete() is invoked
    // to undo an 'insert clef' or 'insert key' command.

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


    //leave cursor positioned on object after object to remove
    m_pVCursor->MoveCursorToObject(pSO);
    m_pVCursor->MoveRight(true);
    lmStaffObj* pCursorSO = m_pVCursor->GetStaffObj();
    lmVCursorState tVCState = m_pVCursor->GetState();

    //get segment and remove object
    lmSegment* pSegment = pSO->GetSegment();
	pSegment->Remove(pSO, false, fClefKeepPosition, fKeyKeepPitch); //false -> do not delete object

    //if removed object is a barline:
    //  - merge current segment with next one
    //  - Adjust timepos of all moved objects
    //  - Update segment pointer in all moved objects
    //  - remove next segment and renumber segments
	if (pSO->IsBarline())
    {
        //merge current segment with next one
	    //As we are dealing with lists this is just to cut and append the lists
        lmSegment* pNextSegment = m_Segments[pSegment->GetNumSegment() + 1];
        float rTime = pSegment->JoinSegment(pNextSegment);

        //remove next segment and renumber segments. Notice that if we removed last barline
        //there will be no next segment
        RemoveSegment( pNextSegment->GetNumSegment());

        //As a consequence of joining segments, saved cursor information is no longer
        //valid. In particular we have to fix rTimePos.
        tVCState.rTimepos += rTime;
    }

    //after removing the staffobj, VCursor iterator is pointing to next staffobj but
    //other VCursor variables (rTimePos, in particular) are no longer valid and should be
    //updated.
    pSegment->UpdateDuration();     //ensure it is updated
    m_pVCursor->SetNewCursorState(m_pVCursor->GetScoreCursor(), &tVCState, true);  //true->update timepos

    //finally, if requested, invoke destructor for removed staffobj
    pSO->SetDirty(true);
    if (fDelete) delete pSO;


    //wxLogMessage(_T("[lmColStaffObjs::Delete] Forcing a dump:");
    //wxLogMessage( Dump() );
    //#if defined(__WXDEBUG__)
    //g_pLogger->LogTrace(_T("lmColStaffObjs::Delete"), Dump() );
    //#endif
}


void lmColStaffObjs::AddToCollection(lmStaffObj* pNewSO, lmStaffObj* pNextSO)
{
    //insert pNewSO before item pNextSO. If pNextSO is NULL, inserts it at the end

    //Get ptr to previous SO
    lmStaffObj* pPrevSO = (pNextSO ? pNextSO->GetPrevSO() : m_pLastSO);

    //Chain the new SO
	pNewSO->SetPrevSO(pPrevSO);
	pNewSO->SetNextSO(pNextSO);

    //update links in prev and next nodes
	if (pPrevSO)
		pPrevSO->SetNextSO(pNewSO);
    if (pNextSO) 
        pNextSO->SetPrevSO(pNewSO);

    //update ptrs to first and last nodes
	if(!pPrevSO)
		m_pFirstSO = pNewSO;
    if (!pNextSO)
		m_pLastSO = pNewSO;
}

void lmColStaffObjs::LogObjectToDeletePosition(lmUndoData* pUndoData, lmStaffObj* pSO)
{
    //log info to locate insertion point when undoing delete command

    //get segment
    lmSegment* pSegment = pSO->GetSegment();

    //advance to next object
    pSO = pSO->GetNextSO();
    if (pSO)
    {
        //not at end of segment. done
        pUndoData->AddParam<lmStaffObj*>(pSO);
        return;
    }

    //we are at end of segment. If we are deleting the barline advance to next segment
    if (pSO->IsBarline())
    {
        //advance to first object of next segment
        pSegment = pSegment->GetNextSegment();
        if (pSegment)
            pSO = pSegment->GetFirstSO();

        if (pSegment && pSO)
        {
            //deleting a barline and there is an object in next segment. done
            pUndoData->AddParam<lmStaffObj*>(pSO);
        }
        else
        {
            //deleting a barline a no next segment or it is empty. End of score
            pUndoData->AddParam<lmStaffObj*>((lmStaffObj*)NULL);
        }
        return;     //done
    }

    //we are at end of segment. No barline and no next segment. End of score
    pUndoData->AddParam<lmStaffObj*>((lmStaffObj*)NULL);
}


//=================================================================================
//Methods for accesing specific items and information about the collection
//=================================================================================

int lmColStaffObjs::GetNumMeasures()
{
    //AWARE This method returns the number of segments but the last one is empty
    //if the score ends with a barline.
    //TODO. Create a new method GetNumRealMeasures and change name of this method
    //to GetNumSegments
    return m_Segments.size();
}

int lmColStaffObjs::GetNumSegments()
{
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
    return m_Segments[nMeasure-1]->GetBarline();
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

lmStaffObj* lmColStaffObjs::FindFwdStaffObj(lmStaffObj* pCurSO, EStaffObjType nType)
{
    //Find next staffobj of type nType, starting from pCurSO (including it).

    //define iterator from current StaffObj
    lmSOIterator it(this, pCurSO);
    while(!it.EndOfCollection())
    {
        lmStaffObj* pSO = it.GetCurrent();
        if (pSO->GetClass() == nType)
        {
            //object found
            return pSO;
        }
        it.MoveNext();
    }

    //not found
    return (lmStaffObj*)NULL;
}

lmTimeSignature* lmColStaffObjs::FindFwdTimeSignature(lmStaffObj* pCurSO)
{
    return (lmTimeSignature*)FindFwdStaffObj(pCurSO, eSFOT_TimeSignature);
}

lmKeySignature* lmColStaffObjs::FindFwdKeySignature(lmStaffObj* pCurSO)
{
    return (lmKeySignature*)FindFwdStaffObj(pCurSO, eSFOT_KeySignature);
}

lmClef* lmColStaffObjs::FindFwdClef(lmStaffObj* pCurSO)
{
    return (lmClef*)FindFwdStaffObj(pCurSO, eSFOT_Clef);
}

lmStaffObj* lmColStaffObjs::FindNextStaffObj(lmStaffObj* pCurSO)
{
    //Find staffobj after pCurSO

    if (!pCurSO)
        return (lmStaffObj*)NULL;

    //define iterator from current StaffObj
    lmSOIterator it(this, pCurSO);
    it.MoveNext();

    if(!it.EndOfCollection())
        return it.GetCurrent();
    else
        return (lmStaffObj*)NULL;
}

lmStaffObj* lmColStaffObjs::FindPrevStaffObj(lmStaffObj* pCurSO)
{
    //Find staffobj before pCurSO

    if (!pCurSO)
        return (lmStaffObj*)NULL;

    //define iterator from current StaffObj
    lmSOIterator it(this, pCurSO);
    it.MovePrev();

    if(!it.FirstOfCollection())
        return it.GetCurrent();
    else
        return (lmStaffObj*)NULL;
}





//====================================================================================================
// Debug methods
//====================================================================================================

wxString lmColStaffObjs::Dump()
{
    wxString sDump = wxString::Format(_T("Num.segments = %d"), m_Segments.size());

//#if defined(__WXDEBUG__)
	//dump segments
	std::vector<lmSegment*>::iterator itS;
	for (itS = m_Segments.begin(); itS != m_Segments.end(); itS++)
	{
		sDump += (*itS)->Dump();
    }
//#endif
    return sDump;
}






//====================================================================================================
// Traversing the staffobjs collection: iterators
//====================================================================================================

lmSOIterator* lmColStaffObjs::CreateIterator()
{
    lmSOIterator* pIter = new lmSOIterator(this);
    return pIter;
}

lmSOIterator* lmColStaffObjs::CreateIteratorTo(lmStaffObj* pSO)
{
    //creates and returns an iterator pointing to staffobj pSO

    lmSOIterator* pIter = new lmSOIterator(this, pSO);
    return pIter;
}

lmSOIterator* lmColStaffObjs::CreateIteratorFrom(lmVStaffCursor* pVCursor)
{
    //creates an iterator pointing to received cursor position

    lmSOIterator* pIter = new lmSOIterator(this, pVCursor);
    return pIter;
}

lmSOIterator* lmColStaffObjs::CreateIteratorFrom(lmStaffObj* pSO)
{
    //creates an iterator pointing to received staffobj

    lmSOIterator* pIter = new lmSOIterator(this, pSO);
    return pIter;
}


//====================================================================================================
// Segments' collection management
//====================================================================================================

void lmColStaffObjs::SplitSegment(int nSegment, lmStaffObj* pLastSO)
{
    // Segment nSegment is splitted into two segments after object pLastSO. That is,
    // first segment will keep all objects up to (inluded) pLastSO and new segment will
    // include all objects after pLastSO.
    // - Timepos of this second segment objects are re-adjusted to start in 0.
    // - All segments after the inserted one are renumbered, and segments vector is updated.
    // - Contexts are not affected by the split operation and therefore, remain updated.

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

    //Split the list: cut first list and append the cutted part to the new list
    pNewSegment->SetCollection(pLastSO->GetNextSO(), pOldSegment->GetLastSO());
    pOldSegment->FinishSegmentAt(pLastSO);

	//Update contexts in new segment
    UpdateSegmentContexts(pNewSegment);

    //wxLogMessage(_T("[lmColStaffObjs::SplitSegment] After splitting:"));
    //wxLogMessage(pOldSegment->Dump());
    //wxLogMessage(pNewSegment->Dump());

}

void lmColStaffObjs::CreateNewSegment(int nSegment)
{
    //Creates a new empty segment after segment number nSegment.
    //
    //   -  If nSegment is the last segment, the new created segment is added at the
    //      end; otherwise, the new segment is inserted after segment nSegment and all
    //      segments after it are renumbered.
    //
    //   -  Segments vector is updated.
    //
    //   -  Contexts at start of new segment are updated in this method. As the new
    //      segment is empty, contexts chain is not affeceted by this insertion and
    //      contexts in all reamining segments remain updated.
    //

    //AWARE:
    //inserting new elements into a vector (either at the end, with 'push_back', or
    //in the middle, with 'insert', can cause vector reallocation. And this will invalidate
    //all currently in use iterators. But not segment pointers stored in the staffobjs as
    //m_Segments is a vector of pointer to segments, created with 'new' operator.


    //create the segment
    lmSegment* pS = new lmSegment(this, (m_Segments.size() == 0 ? 0 : nSegment+1));

    ////DBG
    //{
	//    std::vector<lmSegment*>::iterator itS;
    //    for (itS=m_Segments.begin(); itS != m_Segments.end(); ++itS)
    //    {
    //        wxLogMessage((*itS)->Dump());
    //    }
    //}

	if ((int)m_Segments.size() == 0 || (int)m_Segments.size()-1 == nSegment)
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
            (*itS)->IncrementNumber();
    }

    //Set contexts at start of this new segment.
    UpdateSegmentContexts(pS);
}

lmSegment* lmColStaffObjs::GetNextSegment(int nCurSegment)
{
    //retutn next segment after nCurSegment (0..n-1) or NULL if this is the last one

    int i = nCurSegment + 1;
    if (i < (int)m_Segments.size())
        return m_Segments[i];
    else
        return (lmSegment*) NULL;
}

void lmColStaffObjs::RemoveSegment(int nSegment)
{
	// Remove segment nSegment (0..n) and renumber all remaining segments.
    // No need to update staffobjs in remaining segments, as they contain pointers
    // the their container segment.
    // The removed segment is deleted. Contained staffobjs are not deleted.

    //AWARE: Contexts in remaining segments are not updated.
    //  When removing a segment, if contexts at start of segment to remove are
    //  different from contexts at start of next segment, implies that the segment to
    //  remove has clefs, time signatures and/or key signatures. Therefore, the procedures
    //  to remove these context creator objects should be applied.
    //  Nevertheless, this method is internal, oriented to delete a segment when deleting
    //  a barline or when reorgainizing segments (Auto-Rebar). And in this cases it is not
    //  necessary to take care of context as the content of segment to remove has been
    //  added into another segment.


	//remove the segment
	std::vector<lmSegment*>::iterator itS, itNext;
    itS = find(m_Segments.begin(), m_Segments.end(), m_Segments[nSegment]);

    //delete the segment
    delete *itS;

    //remove segmentt from segments' vector
    m_Segments.erase(itS);

    //renumber all following segments
    for (int i=nSegment; i < (int)m_Segments.size(); i++)
    {
        m_Segments[i]->DecrementNumber();
    }
}

void lmColStaffObjs::AutoReBar(lmStaffObj* pFirstSO, lmStaffObj* pLastSO,
                               lmTimeSignature* pNewTS)
{
    ////determine involved segments
    //int nFirstSegment = pFirstSO->GetSegment()->m_nNumSegment;
    //int nLastSegment = (pLastSO ? pLastSO->GetSegment()->m_nNumSegment :
    //                              m_Segments.size()-1 );

    ////define a list to contain all staffobjs in the segments to re-bar
    //std::list<lmStaffObj*> oSource;

    ////loop to fill the list with the objects in the segments to re-bar
    //float rTimeIncr = 0.0f;
    //for (int i=nFirstSegment; i <= nLastSegment; i++)
    //{
    //    //copy objects in list, except barlines. Delete the barlines.
    //    //TODO: what if barlines have AuxObjs attached? They will be lost.
    //    std::list<lmStaffObj*>::iterator it = m_Segments[i]->m_StaffObjs.begin();
    //    while(it != m_Segments[i]->m_StaffObjs.end())
    //    {
    //        if (!(*it)->IsBarline())
    //        {
    //            //When deleting a barline all staffobjs timepos in next segments must be
    //            //incremented by current barline timepos. So, we need to adjust current
    //            //staffobj timepos
    //            (*it)->SetTimePos( (*it)->GetTimePos() + rTimeIncr );
    //            oSource.push_back(*it);     //copy object in list
    //        }
    //        else
    //        {
    //            rTimeIncr += (*it)->GetTimePos();
    //            delete *it;                 //delete barline
    //        }
    //        ++it;
    //    }
    //}


    ////At this point we have all staffobjs included in the auxiliary list. We can
    ////proceed to remove the segments to re-bar from the collection

    ////remove the segments, but do not delete its staffobjs.
    ////All but contexts will be updated in RemoveSegment() method
    //for (int iS=nFirstSegment; iS <= nLastSegment; iS++)
    //{
    //    //when removing a segment, all remaining segments get renumbered. For instance,
    //    //after deleting segment 3, segment 4 will be segment 3. Therefore, we have
    //    //always to delete first segments as all others will, consecutively, occupy
    //    //its place.
    //    RemoveSegment(nFirstSegment);
    //}


    ////At this point the collection is ready for including the new segments. We are
    ////going to proceed to prepare the new segments.

    ////determine the new required measure duration
    //float rMeasureDuration;
    //if (!pNewTS)
    //{
    //    //no time signature. Do not add barlines, but create one segment. To get
    //    //it done, I will set a too big measure duration
    //    rMeasureDuration = 10000000000000000.0f;
    //    //THINK: When no TS, we could try to insert hidden barlines to deal with no time
    //    //      signature scores
    //}
    //else
    //    rMeasureDuration = pNewTS->GetMeasureDuration();

    ////loop to create new segments.

    ////insert the first segment. It will occupy the place of first removed segment
    //int nCurSegment = nFirstSegment-1;
    //CreateNewSegment(nCurSegment++);                        //it is inserted after segment nFirstSegment-1
    //lmSegment* pCurSegment = m_Segments[nCurSegment];       //get the new segment

    //std::list<lmStaffObj*>::iterator it = oSource.begin();
    //while(it != oSource.end())
    //{
    //    //checking if a segment is full is tricky. It is not enough just to check,
    //    //after adding a new SO, that the measure duration has been reached.
    //    //Next objects to add could be on a different voice or be a goBack ctrol object.
    //    //Therefore, we have can not be sure if a measure is full until we add a staffobj
    //    //and its assigned timepos results equal or greater than the required measure duration.

    //    //add staffobj to segment
    //    lmStaffObj* pSO = *it;
    //    pCurSegment->Store(pSO, (lmVStaffCursor*)NULL);       << NULL means add at the end

    //    //now we can reliable check if measure is full
    //    if (IsHigherTime(pSO->GetTimePos(), rMeasureDuration) ||
    //        IsEqualTime(pSO->GetTimePos(), rMeasureDuration) )
    //    {
    //        //segment was full. Remove added staffobj
    //        pCurSegment->Remove(pSO, false, false, false);  //false -> do not delete object
    //                                                        //false -> fClefKeepPosition
    //                                                        //false -> fKeyKeepPitch

    //        //add barline
    //        lmBarline* pBar = new lmBarline(lm_eBarlineSimple, m_pOwner, lmVISIBLE);
    //        pBar->SetTimePos( pSO->GetTimePos() );
    //        pCurSegment->Store(pBar, (lmVStaffCursor*)NULL);

    //        //adjust timepos of remaining staffobjs
    //        float rTime = pSO->GetTimePos();
    //        std::list<lmStaffObj*>::iterator itAux = it;
    //        while(itAux != oSource.end())
    //        {
    //            (*itAux)->SetTimePos( (*itAux)->GetTimePos() - rTime );
    //            ++itAux;
    //        }

    //        //insert a new segment and add the removed staffobj to it
    //        CreateNewSegment(nCurSegment++);                    //it is inserted after segment nCurSegment
    //        pCurSegment = m_Segments[nCurSegment];              //get the new segment
    //        pCurSegment->Store(pSO, (lmVStaffCursor*)NULL);
    //    }

    //    //get next staffobj to add
    //    ++it;
    //}

    ////At this point all new segments have been created and inserted in the collection.
    ////But last inserted segment has no barline.
    ////If there are more segments after last inserted one, it is necessary:
    ////1. to add a barline, and in that case, if it is not full, fill it with rests.
    ////2. propagate contexts and update segment pointers after last inserted segment

    //if (nCurSegment < (int)m_Segments.size()-1)
    //{
    //    //There are more segments.

    //    //Fill with rests
    //    lmTODO(_T("[lmColStaffObjs::AutoReBar] Fill last segment with rests"));

    //    //and add barline
    //    lmBarline* pBar = new lmBarline(lm_eBarlineSimple, m_pOwner, lmVISIBLE);
    //    pBar->SetTimePos( rMeasureDuration );
    //    pCurSegment->Store(pBar, (lmVStaffCursor*)NULL);

    //    //propagate contexts and update segment pointers after last inserted segment
    //    //This is necessary so that segments can update pointers to start of segment
    //    //applicable contexts.
    //    for (int nStaff=1; nStaff <= lmMAX_STAFF; nStaff++)
    //    {
    //        wxLogMessage( m_pOwner->Dump() );
    //        //determine context for current staff at end of segment
    //        lmContext* pLastContext = pCurSegment->FindEndOfSegmentContext(nStaff);

    //        //inform next segment
    //        pCurSegment->PropagateContextChange(pLastContext, nStaff);
    //    }
    //}

    ////clear the source list. StaffObjs in it can not be deleted as they have been included
    ////in the new segments
    //oSource.clear();
}

void lmColStaffObjs::UpdateSegmentContexts(lmSegment* pSegment)
{
    //When add/inserting a new segment it is necessary to find the applicable contexts at
	//start of that segment and to store them in the segment. This method
    //does it. nNewSegment (0..n-1) is the number of the segment to update.

	int nNewSegment = pSegment->GetNumSegment();

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
	lmStaffObj* pSO = pPrevSegment->GetLastSO();
    bool fDone = false;
    while(!fDone && pSO)
	{
        lmContext* pCT = (lmContext*)NULL;
        if (pSO->GetClass() == eSFOT_Clef)
        {
            int nStaff = pSO->GetStaffNum();
            if (!fStaffDone[nStaff-1])
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
		if(pSO == pPrevSegment->GetFirstSO())
			break;
		else
		{
			pSO = pSO->GetPrevSO();
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
    else if (IsHigherTime(rNewTime, rMaxTime))
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

void lmColStaffObjs::RecomputeSegmentDuration(lmNoteRest* pNR, float rTimeIncr)
{
    //Duration of note/rest pNR has been changed. Therefore it could be necessary to recompute
    //segment duration and reposition (in time) all objects after pNR.

    pNR->GetSegment()->RecomputeSegmentDuration(pNR, rTimeIncr);
}


//====================================================================================================
// lmColStaffObjs: contexts management
//====================================================================================================

lmContext* lmColStaffObjs::GetCurrentContext(lmStaffObj* pTargetSO, int nStaff)
{
	// Returns the context that is applicable to the received StaffObj.
	// AWARE: This method does not return a context with updated accidentals;
    // the returned context is valid only for clef, key signature and time signature.
    // To get applicable accidentals use NewUpdatedContext() instead.

    //if no staff specified use pTargetSO staff
    if (nStaff == -1)
        nStaff = pTargetSO->GetStaffNum();

    //get start of segment context
	lmSegment* pSegment = pTargetSO->GetSegment();
	lmContext* pCT = pSegment->GetContext(nStaff - 1);

    //look for any clef, KS or TS changing the context
    lmSOIterator it(this, pSegment->GetFirstSO());
    while(!it.EndOfCollection() && !it.ChangeOfMeasure() && it.GetCurrent() != pTargetSO)
	{
        lmStaffObj* pSO = it.GetCurrent();
		if (pSO->IsClef() && nStaff == pSO->GetStaffNum())
            pCT = ((lmClef*)pSO)->GetContext();
		else if (pSO->IsKeySignature())
            pCT = ((lmKeySignature*)pSO)->GetContext(nStaff);
		else if (pSO->IsTimeSignature())
            pCT = ((lmTimeSignature*)pSO)->GetContext(nStaff);

        it.MoveNext();
    }
	return pCT;
}

lmContext* lmColStaffObjs::NewUpdatedContext(int nStaff, lmStaffObj* pThisSO)
{
	//returns the applicable context for staff nStaff and the timepos of object pThisSO,
    //updated with all accidentals introduced by previous notes in staff nStaff.
	//AWARE: context ownership is transferred to the caller.
	//       The returned context MUST BE deleted by the invoking method.


    wxASSERT(nStaff > 0);

    //start backwards iterator from this SO. We are going to see if in this
    //segment there is a previous clef, time or key signature,
	//as they define the last context. Otherwise, we will take context at
    //start of segment
	lmSegment* pSegment = pThisSO->GetSegment();

    //go backwards to see if in this segment there is a previous clef, time or key signature
    lmStaffObj* pSO;
    lmContext* pCT = (lmContext*)NULL;
    lmSOIterator it(this, pThisSO);
    while(!it.EndOfCollection() && !it.ChangeOfMeasure())
	{
        pSO = it.GetCurrent();
		if (pSO->IsClef() && nStaff == pSO->GetStaffNum())
            pCT = ((lmClef*)pSO)->GetContext();
		else if (pSO->IsKeySignature())
            pCT = ((lmKeySignature*)pSO)->GetContext(nStaff);
		else if (pSO->IsTimeSignature())
            pCT = ((lmTimeSignature*)pSO)->GetContext(nStaff);

        if (pCT)
		{
			it.MoveNext();      //point to staffobj after clef, TS or KS
			break;
		}
		it.MovePrev();
	}

	if (!pCT)
	{
		//we have arrived to start of segment and no clef, time or key signatures
		//found. Therefore take context at start of segment and point iterator to
        //first staffobj of this segment
		pCT = pSegment->GetContext(nStaff - 1);
		pSO = pSegment->GetFirstSO();
	}

	//Here, pSO is pointing to the first staffobj able to modify current context.
	//Now we have to go forward, updating accidentals until we reach target SO

	lmContext* pUpdated = new lmContext(pCT);
    float rTime = pThisSO->GetTimePos();
    it.MoveTo(pSO);
    while(pSO && IsLowerTime(pSO->GetTimePos(), rTime))
	{
		if (pSO->GetStaffNum() == nStaff && pSO->IsNoteRest() && ((lmNote*)pSO)->IsNote())
		{
			//Note found. Update context
			lmAPitch apPitch = ((lmNote*)pSO)->GetAPitch();
			pUpdated->SetAccidental(apPitch.Step(), apPitch.Accidentals());
		}
		it.MoveNext();
        pSO = it.GetCurrent();
	}

	return pUpdated;
}

lmContext* lmColStaffObjs::NewUpdatedLastContext(int nStaff)
{
	//returns the last context for staff nStaff, updated with all
	//accidentals introduced by previous notes.
	//AWARE: context ownership is transferred to the caller.
	//       The returned context MUST BE deleted by the invoking method.

    wxASSERT(nStaff > 0);

    //get last StaffObj of last segment and start backwards iterator. We are
	//going to see if in this last segment there is a clef, time or key signature,
	//as they define the last context
    lmContext* pCT = (lmContext*)NULL;
	lmSegment* pSegment = m_Segments.back();
	lmStaffObj* pSO = pSegment->GetLastSO();
    lmSOIterator it(this, pSO);
    while(!it.EndOfCollection() && !it.ChangeOfMeasure())
	{
        pSO = it.GetCurrent();
		if (pSO->GetClass() == eSFOT_Clef && nStaff == pSO->GetStaffNum())
            pCT = ((lmClef*)pSO)->GetContext();
		else if (pSO->GetClass() == eSFOT_KeySignature)
            pCT = ((lmKeySignature*)pSO)->GetContext(nStaff);
		else if (pSO->GetClass() == eSFOT_TimeSignature)
            pCT = ((lmTimeSignature*)pSO)->GetContext(nStaff);

        if (pCT)
		{
		    it.MoveNext();
            pSO = it.GetCurrent();
			break;
		}
		it.MovePrev();
	}

	if (!pCT)
	{
		//we have arrived to start of segment and no clef, time or key signatures
		//found. Therefore take last context and point it to first staffobj of
		//last segment
		pCT = m_pOwner->GetLastContext(nStaff);
		pSO = pSegment->GetFirstSO();

        //if the score is empty there is no context yet. Return NULL
        if (!pCT)
            return pCT;
	}

	//Here, pSO is pointing to the first staffobj able to modify current context.
	//Now we have to go forward, updating accidentals until we reach end of collection

	lmContext* pUpdated = new lmContext(pCT);
    it.MoveTo(pSO);
    while(pSO)
	{
		if (pSO->GetClass() == eSFOT_NoteRest && ((lmNote*)pSO)->IsNote())
		{
			//Note found. Update context
			lmAPitch apPitch = ((lmNote*)pSO)->GetAPitch();
			pUpdated->SetAccidental(apPitch.Step(), apPitch.Accidentals());
		}
		it.MoveNext();
        pSO = it.GetCurrent();
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
    wxASSERT(nStaff > 0);

    return m_Segments[nMeasure-1]->GetContext(nStaff-1);
}

