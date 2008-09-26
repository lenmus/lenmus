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

////binary function predicate. I is used to sort the staffobjs list by time
////returns whether a staffobj SO1 has lower timepos than another staffobj SO2
//bool TimeCompare(lmStaffObj* pSO1, lmStaffObj* pSO2)
//{
//    return IsLowerTime(pSO1->GetTimePos(), pSO2->GetTimePos());
//}


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

    //SO1 has lower or equal time than SO2. Now order by duration.
    else if (IsEqualTime(pSO1->GetTimePos(), pSO2->GetTimePos())
             && IsLowerTime(pSO1->GetTimePosIncrement(), pSO2->GetTimePosIncrement()))
        fValue = true, nCheck=2;

    //here they are ordered by timepos and duration. Chords: Ensure base note precedence
    else if (IsEqualTime(pSO1->GetTimePos(), pSO2->GetTimePos())
             && pSO1->IsNoteRest() && pSO2->IsNoteRest() 
             && ((lmNoteRest*)pSO1)->IsNote() && ((lmNoteRest*)pSO2)->IsNote() 
             && ((lmNote*)pSO1)->IsBaseOfChord() && ((lmNote*)pSO2)->IsInChord() )
        fValue = true, nCheck=3;

    //If chord and right ordering, do not make more checks
    else if (IsEqualTime(pSO1->GetTimePos(), pSO2->GetTimePos())
             && pSO1->IsNoteRest() && pSO2->IsNoteRest() 
             && ((lmNoteRest*)pSO1)->IsNote() && ((lmNoteRest*)pSO2)->IsNote() 
             && ((lmNote*)pSO1)->IsInChord() && ((lmNote*)pSO2)->IsBaseOfChord() )
        fValue = false, nCheck=4;

    //elements of same type ordered by staff number
    else if (pSO1->GetClass() == pSO2->GetClass()
             && IsEqualTime(pSO1->GetTimePos(), pSO2->GetTimePos())
             && !IsHigherTime(pSO2->GetTimePosIncrement(), pSO1->GetTimePosIncrement())
             && pSO1->GetStaffNum() < pSO2->GetStaffNum())
        fValue = true, nCheck=5;

    else
        fValue = false, nCheck=6;

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

    //here they are ordered by timepos and duration. Chords: Ensure base note precedence
    if (IsEqualTime(pSO1->GetTimePos(), pSO2->GetTimePos())
        && pSO1->IsNoteRest() && pSO2->IsNoteRest() 
        && ((lmNoteRest*)pSO1)->IsNote() && ((lmNoteRest*)pSO2)->IsNote() 
        && ((lmNote*)pSO1)->IsBaseOfChord() && ((lmNote*)pSO2)->IsInChord() )
        return true;

    //If chord and right ordering, do not make more checks
    if (IsEqualTime(pSO1->GetTimePos(), pSO2->GetTimePos())
        && pSO1->IsNoteRest() && pSO2->IsNoteRest() 
        && ((lmNoteRest*)pSO1)->IsNote() && ((lmNoteRest*)pSO2)->IsNote() 
        && ((lmNote*)pSO1)->IsInChord() && ((lmNote*)pSO2)->IsBaseOfChord() )
        return false;

    //elements of same type ordered by staff number
    if (pSO1->GetClass() == pSO2->GetClass()
        && IsEqualTime(pSO1->GetTimePos(), pSO2->GetTimePos())
        && !IsHigherTime(pSO2->GetTimePosIncrement(), pSO1->GetTimePosIncrement())
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
lmVCursorState g_tNoVCursorState = { -1, -1, 0.0f, (lmStaffObj*)NULL };



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

void lmVStaffCursor::SetNewCursorState(lmScoreCursor* pSCursor, lmVCursorState* pState,
                                       bool fUpdateTimepos)
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

    //position m_it to point to staffobj
    if (pState->pSO)
    {
        //Pointing to an staffobj. Locate it 
        m_it = m_pSegment->m_StaffObjs.begin();
        while(m_it != m_pSegment->m_StaffObjs.end() && *m_it != pState->pSO)
            ++m_it;
        //update timepos
        if (fUpdateTimepos)
            m_rTimepos = pState->pSO->GetTimePos();
    }
    else
    {
        //Not pointing to an staffobj. Cursor is at end of collection
        m_it = m_pSegment->m_StaffObjs.end();
        if (fUpdateTimepos)
            m_rTimepos = m_pSegment->GetDuration();
    }

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

void lmVStaffCursor::MoveRightToNextTime()
{
    //Cursor movement by occupied timepos in current staff and at end of score.
    //When an empty measure is found, cursor will stop in at begining (timepos 0)
    //and before the barline, and then move to next measure.


    //(**1) if at end of collection remain there
	if (IsAtEnd()) return;      //Done. Case [e1]
    wxASSERT(m_it != m_pSegment->m_StaffObjs.end());

    //cursor could be placed at lower time than current object (i.e. if caret has been
    //positioned by time increment). Check if current object could be the target place
    if ((*m_it)->GetStaffNum() == m_nStaff 
        || (*m_it)->IsBarline()
        || (*m_it)->IsKeySignature()
        || (*m_it)->IsTimeSignature() )
    {
        if ( IsLowerTime(m_rTimepos, (*m_it)->GetTimePos()) )
        {
            //we are at right place. Just advance cursor time
            m_rTimepos = (*m_it)->GetTimePos();
            return;     //Done. Case [n1]
        }
        else
        {
            //same timepos than CursorObj
            if ((*m_it)->IsBarline())
            {
                //Advance to next segment.
                //As there is a barline, there also exist a next segment
                AdvanceToNextSegment();
    
                //move at timepos 0, at right place for TargetStaff
                while (m_it != m_pSegment->m_StaffObjs.end()
                       && IsEqualTime((*m_it)->GetTimePos(), 0.0f)
                       && (*m_it)->GetStaffNum() < m_nStaff
                       && !( (*m_it)->IsBarline()
                             || (*m_it)->IsKeySignature()
                             || (*m_it)->IsTimeSignature() )
                      )
                {
                    ++m_it;
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
    if ((*m_it)->GetStaffNum() == m_nStaff 
        || (*m_it)->IsKeySignature()
        || (*m_it)->IsTimeSignature() )
        rTargetTimepos = (*m_it)->GetTimePos() + (*m_it)->GetTimePosIncrement();
    else
        rTargetTimepos = -1.0f;      //to stop at next valid position
        
    //advance to first position in this staff with timepos >= TargetTime.
    lmStaffObj* pLastSO = *m_it;        //save current object
    ++m_it;         //increment iterator
    while (m_it != m_pSegment->m_StaffObjs.end()
           && ( IsHigherTime(rTargetTimepos, (*m_it)->GetTimePos())
                || ( (*m_it)->GetStaffNum() != m_nStaff 
                        && !( (*m_it)->IsBarline()
                            || (*m_it)->IsKeySignature()
                            || (*m_it)->IsTimeSignature() )
                    )
              )
           )
    {
        ++m_it;
    }

    //(**2)
    //if we reach end of segment is because we have not reached a barline
    //Therefore, we are at end of collection. Remain there
    if (m_it == m_pSegment->m_StaffObjs.end())
    {
        //We are at end of collection.
        //Get last staffobj and assign its timepos plus its duration
        if (pLastSO && pLastSO->GetStaffNum() == m_nStaff)
            m_rTimepos = pLastSO->GetTimePos() + pLastSO->GetTimePosIncrement();
        return;     //Done. Case [e2] 
    }

    //Set new cursor timepos
    if (IsEqualTime(rTargetTimepos, -1.0f))
        //no target time. New cursor timepos is object timepos
        m_rTimepos = (*m_it)->GetTimePos();
    else
        //New cursor timepos at target timepos
        m_rTimepos = rTargetTimepos;

    return;     //Done. Case [n3]
}

void lmVStaffCursor::MoveRightIncrementingTime(float rTimeincr)
{
    //Caret movement by occupied timepos in current staff and to end of
    //score (empty entry point)
}

void lmVStaffCursor::MoveRightToNextObject(float rTimeincr)
{
    //Advance to next object in current time (i.e. stopping in every chord note).
    //If no more objects in current time advance to next occupied timepos.
}

void lmVStaffCursor::MoveLeftToPrevTime()
{
    //Cursor movement by occupied timepos in current staff and at start of score.
    //Cursor will always stop in each measure at timepos 0 (even if no objects there)
    //and then move to prev measure and stop before barline.


    //(**1) if at beginning of collection remain there. Done. Case [e1]
	if (IsAtBeginning()) return;        //Done. Case [e1]
    
    //determine target timepos
    //move back to previous object in target staff
    bool fPrevObjFound = false;     
    while(m_it != m_pSegment->m_StaffObjs.begin())
    {
		//move back
		--m_it;

        //check if staffobj is in current staff; otherwise we have to continue moving back
        //This check has to be ignored for objects applicable to all staffs (barlines,
        //KS & TS), as they are always in first staff but are applicable to all.
        if ((*m_it)->GetStaffNum() == m_nStaff || 
            (*m_it)->IsBarline() ||
            (*m_it)->IsKeySignature() ||
            (*m_it)->IsTimeSignature() )
        {
            //it is in current staff. PrevObj found. Finish loop
            fPrevObjFound = true;
            break;
        }
    }

    if (fPrevObjFound)
    {
        ////PrevObj found
        //float rTargetTimepos = (*m_it)->GetTimePos() + (*m_it)->GetTimePosIncrement();
        //if (IsLowerTime(rTargetTimepos, m_rTimepos))
        //{
        //    //cursor has to be positioned after PrevObj, at timepos TargetTimepos,
        //    //at appropiate position for current staff
        //    ++m_it;     //set iterator after PrevObj

        //    //move at timepos TargetTimepos, at right place for TargetStaff
        //    PositionAt(rTargetTimepos);
        //    return;     //Done. Case [n1]
        //}
        //else
        {
            //Cursor has to be positioned at timepos PrevObj.Timepos
            //if PrevObj is not note in chord
            if (!(*m_it)->IsNoteRest()
                || !((lmNoteRest*)(*m_it))->IsNote()
                || !((lmNote*)(*m_it))->IsInChord()
                || ((lmNote*)(*m_it))->IsBaseOfChord() )
            {
                //Done. Place cursor at PrevObj
                m_rTimepos = (*m_it)->GetTimePos();
                return;     //Done. Case [n2]
            }
            else
            {
                //place cursor at base note of chord
                //while (PrevObj is note in chord and is not base note)
                while ((*m_it)->IsNoteRest()
                        && ((lmNoteRest*)(*m_it))->IsNote()
                        && ((lmNote*)(*m_it))->IsInChord()
                        && !((lmNote*)(*m_it))->IsBaseOfChord() )
                {
                    --m_it;
                }
                //Done. Place cursor at base note of chord
                m_rTimepos = (*m_it)->GetTimePos();
                return;     //Done. Case [n3]
            }
        }
    }
    else
    {
        //PrevObj not found. We are at beginning of segment
        //if CursorTime > 0 or this is first segment
        if (IsHigherTime(m_rTimepos, 0.0f) || m_nSegment == 0) 
        {
            //move to position 0 in this segment
            m_it = m_pSegment->m_StaffObjs.begin();     //set iterator at begin of segment
            PositionAt(0.0f);                           //move at timepos 0, at right place for TargetStaff
            return;                                     //Done. Case [n3]
        }
        else
        {
            //move to end of previous segment

            //set iterator at end of previous segment
            --m_nSegment;
		    m_pSegment = m_pColStaffObjs->m_Segments[m_nSegment];
		    m_it = m_pSegment->m_StaffObjs.end();

            //Move iterator to barline (it MUST exist)
            --m_it;
            wxASSERT(m_it != m_pSegment->m_StaffObjs.end());

            //done
            m_rTimepos = (*m_it)->GetTimePos();
            return;     //Done. Case [n4]
        }
    }
}

void lmVStaffCursor::MoveLeftDecrementingTime(float rTimeincr)
{
}

void lmVStaffCursor::MoveLeftToPrevObject(float rTimeincr)
{
}

void lmVStaffCursor::PositionAt(float rTargetTimepos)
{
    //cursor has to be positioned at timepos TargetTimepos, at appropiate position
    //for current staff.
    //movement is restricted to current segment.
    //current position is a valid position, so target position is current position
    //or any position after it.
    //we are not at end of segment

    wxASSERT(m_it != m_pSegment->m_StaffObjs.end());

    //advance to first position in this staff with timepos >= TargetTime.
    lmStaffObj* pLastSO = *m_it;        //save current object
    while (m_it != m_pSegment->m_StaffObjs.end()
           && ( IsHigherTime(rTargetTimepos, (*m_it)->GetTimePos())
                || ( (*m_it)->GetStaffNum() != m_nStaff 
                        && !( (*m_it)->IsBarline()
                            || (*m_it)->IsKeySignature()
                            || (*m_it)->IsTimeSignature() )
                    )
              )
           )
    {
        ++m_it;
    }

    //if we reach end of segment is because we have not reached a barline
    //Therefore, we are at end of collection. Remain there
    if (m_it == m_pSegment->m_StaffObjs.end())
    {
        //We are at end of collection.
        //Get last staffobj and assign its timepos plus its duration
        if (pLastSO && !pLastSO->GetStaffNum() == m_nStaff)
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
    
 //   //Advance cursor constrained by staff.
 //   //Cursor should advance to next timepos in current staff.
 //   //If flag fAlsoChordNotes is true, stops in every chord note; otherwise, chords are
 //   //treated as a single object

 //   //(**1) if at end of collection remain there
	//if (IsAtEnd()) return;
 //   wxASSERT(m_it != m_pSegment->m_StaffObjs.end());

 //   //determine target timepos
 //   //------------------------
 //   float rTargetTimepos;
 //   if (IsLowerTime(m_rTimepos, (*m_it)->GetTimePos()))
 //   {
 //       //we are before CursorObj, so next timepos is CursorObj timepos
 //       rTargetTimepos = (*m_it)->GetTimePos();
 //   }
 //   else
 //   {
 //       //same timepos than CursorObj. Target is next available time
 //       if ((*m_it)->IsBarline())
 //       {
 //           rTargetTimepos = 0.0f;
 //           //(**2)
 //           //Advance to next segment.
 //           //As there is a barline, there also exist a next segment
 //           AdvanceToNextSegment();
 //           //But next segment could be empty and, in that case, we are
 //           //finished
 //           if (m_it == m_pSegment->m_StaffObjs.end())
 //           {
 //               m_rTimepos = rTargetTimepos;
 //               return;     //Done. Case [9]
 //           }
 //       }
 //       else
 //       {
 //           //TargetTimepos will be the next available timepos in the collection.
 //           //As the collection is ordered by timepos and duration, next available
 //           //timepos is just current obj + its duration
 //           rTargetTimepos = (*m_it)->GetTimePos() + (*m_it)->GetTimePosIncrement();

 //           //if TargetTimepos is greater than Barline timepos move to next segment
 //           if (IsHigherTime(rTargetTimepos, m_pSegment->GetDuration()))
 //           {
 //               rTargetTimepos = 0.0f; 
 //               //(**3)
 //               //if next segment exists move to it. Otherwise we are at end of collection,
 //               //we have surpassed measure duration and user has no entered a barline. Anyway,
 //               //remain at end of collection
 //               if (m_nSegment < (int)m_pColStaffObjs->m_Segments.size() - 1)
 //               {
 //                   //next segment exists
 //                   AdvanceToNextSegment();
 //                   //But next segment could be empty and, in that case, we are
 //                   //finished
 //                   if (m_it == m_pSegment->m_StaffObjs.end())
 //                   {
 //                       m_rTimepos = rTargetTimepos;
 //                       return;     //Done. Case [8]
 //                   }
 //               }
 //               else
 //               {
 //                   //move to end of collection
 //                   m_it = m_pSegment->m_StaffObjs.end();
 //                   m_rTimepos = rTargetTimepos;
 //                   return;     //Done. Case [4]
 //               }
 //           }
 //       }
 //   }

	//////save current position
	////int nPrevSegment = m_nSegment;          //current segment
	////lmSegment* pPrevSegment = m_pSegment;   //current segment
	////float rPrevTimepos = m_rTimepos;        //timepos
	////lmItCSO itPrev = m_it;


 //   //advance to target timepos
 //   //-------------------------
 //   //here if CursorTimepos == TargetTimepos is because current staffobj has no
 //   //duration, Therefore we has to skep current staffobj
 //   if (IsEqualTime(m_rTimepos, rTargetTimepos) && 
 //       IsEqualTime((*m_it)->GetTimePosIncrement(), 0.0f))
 //   {
 //       //advance cursor, unless currently pointed object has been deleted.
 //       AdvanceIterator();
 //       UpdateTimepos();
 //       //(**4)
 //       //if we arrive to end of segment is because we have not reached a barline.
 //       //Therefore, we are at end of collection. Remain there
 //       if (m_it == m_pSegment->m_StaffObjs.end())
 //       {
 //           //end of segment reached
 //           m_rTimepos = rTargetTimepos;
 //           return;     //Done. Case [5] 
 //       }
 //   }
 //   
 //   while (IsLowerTime((*m_it)->GetTimePos(), rTargetTimepos))
 //   {
 //       AdvanceIterator();
 //       UpdateTimepos();
 //       //(**5)
 //       //if we arrive to end of segment is because we have not reached a barline
 //       //(remember that TargetTimepos is at maximum Barline timepos)
 //       //Therefore, we are at end of collection. Remain there
 //       if (m_it == m_pSegment->m_StaffObjs.end())
 //       {
 //           //end of segment reached
 //           m_rTimepos = rTargetTimepos;
 //           return;     //Done. Case [6] 
 //       }
 //   }
 //       
 //       
 //   //here CursorObjTiempos is equal or greater than target timepos.
 //   //If it is greater, we are done. We should stay pointing to this object
 //   //but at target time. If times are equal, we should move to right staff
 //   //------------------------
 //   if (IsHigherTime(m_rTimepos, rTargetTimepos))
 //   {
 //       m_rTimepos = rTargetTimepos;
 //       return;     //Done. Case [1]
 //   }
 //   else
 //   {
 //       //CursorObjTiempos == TargetTimepos. Move to right staff
 //       while (IsEqualTime(m_rTimepos, rTargetTimepos)
 //              && (*m_it)->GetStaffNum() < m_nStaff
 //              && !( (*m_it)->IsBarline()
 //                    || (*m_it)->IsKeySignature()
 //                    || (*m_it)->IsTimeSignature() )
 //             )
 //       {
 //           AdvanceIterator();
 //           UpdateTimepos();
 //           //(**6)
 //            //if we reach end of segment is because we have not reached a barline
 //           //(remember that TargetTimepos is at maximum Barline timepos)
 //           //Therefore, we are at end of collection. Remain there
 //           if (m_it == m_pSegment->m_StaffObjs.end())
 //           {
 //               //end of segment reached
 //               m_rTimepos = rTargetTimepos;
 //               return;     //Done. Case [7] 
 //           }
 //      }
 //           
 //       //here we are either at a greater timepos or at right position. In both
 //       //cases we are at the right staffobj
 //       m_rTimepos = rTargetTimepos;
 //       return;     //Done. Case [2]
 //   }
 //           





 //  // //loop to advance to next segment, if necessary, and to advance to next valid position
 //  // while(true)
 //  // {
	//  //  //if end of segment reached, advance to next segment
	//  //  if (m_it == m_pSegment->m_StaffObjs.end())
	//  //  {
	//		//m_nSegment++;
	//	 //   if (m_nSegment < (int)m_pColStaffObjs->m_Segments.size())
	//	 //   {
	//		//    m_pSegment = m_pColStaffObjs->m_Segments[m_nSegment];
	//		//    m_it = m_pSegment->m_StaffObjs.begin();
	//	 //   }
	//	 //   else
	//		//{
	//		//	m_nSegment--;
 //  //             UpdateTimepos();
 //  //             return;      //end of collection reached
	//		//}
	//  //  }

 //  //     //end of collection reached
	//  //  if (m_it == m_pSegment->m_StaffObjs.end())
 //  //     {
 //  //         UpdateTimepos();
 //  //         return;
 //  //     }

 //  //     //check if staffobj is in current staff or we passed target timepos;
 //  //     //otherwise we have to continue advancing
 //  //     //This check has to be ignored for objects applicable to all staffs (barlines,
 //  //     //KS & TS), as they are always in first staff but are applicable to all.
 //  //     if ((*m_it)->GetStaffNum() == m_nStaff || 
 //  //         (*m_it)->IsBarline() ||
 //  //         (*m_it)->IsKeySignature() ||
 //  //         (*m_it)->IsTimeSignature() ||
 //  //         IsHigherTime((*m_it)->GetTimePos(), rTargetTimepos) )
 //  //     {
 //  //         //it is in current staff or we passed target timepos.
 //  //         if (IsHigherTime((*m_it)->GetTimePos(), rTargetTimepos))
 //  //         {
 //  //             //we passed target timepos. If previous object was at target timepos
 //  //             //move to it; otherwise previous object was at a lower timepos, so we
 //  //             //must remain here but at target timepos
 //  //             if (IsEqualTime((*itPrev)->GetTimePos(), rTargetTimepos))
 //  //             {
	//  //              m_nSegment = nPrevSegment;
	//  //              m_pSegment = pPrevSegment;
	//  //              m_rTimepos = rTargetTimepos;
	//  //              m_it = itPrev;
 //  //                 return;
 //  //             }
 //  //             else
 //  //             {
	//  //              m_rTimepos = rTargetTimepos;
 //  //                 return;
 //  //             }
 //  //         }
 //  //         else
 //  //         {
 //  //             //it is in current staff
 //  //             //if requested to stop in every chord note, we can finish
 //  //             if (fAlsoChordNotes)
 //  //                 break;
 //  //             //else we have to skip notes in chord except base note
 //  //             if (!(*m_it)->IsNoteRest() ||
 //  //                 (*m_it)->IsNoteRest() && ((lmNoteRest*)(*m_it))->IsNote() &&
 //  //                 ( !((lmNote*)(*m_it))->IsInChord() ||
 //  //                   ((lmNote*)(*m_it))->IsBaseOfChord() ))
 //  //                 break;
 //  //         }
 //  //     }

	//  //  //save current position
	//  //  nPrevSegment = m_nSegment;
	//  //  pPrevSegment = m_pSegment;
	//  //  rPrevTimepos = m_rTimepos;
	//  //  itPrev = m_it;

 //  //     //advance to next staffobj
 //  //     ++m_it;
 //  // }

 //   ////update cursor timepos
 //   //UpdateTimepos();
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
    MoveLeftToPrevTime();

 //   //Move back cursor constrained by staff.
 //   //If flag fAlsoChordNotes is true, stops in every chord note; otherwise, chords are
 //   //treated as a single object

 //   //AWARE: If you change anything in this method verify if the same change should
 //   //be applied to method GetPreviousStaffobj()

	//if (IsAtBeginning()) return;

	////save current position
	//int nSegment = m_nSegment;			//current segment
	//lmSegment* pSegment = m_pSegment;   //current segment
	//float rTimepos = m_rTimepos;        //timepos
	//lmItCSO it = m_it;


 //   while(true)
 //   {
 //       //if at start of segment, move back to last SO of previous segment
	//    if (m_it == m_pSegment->m_StaffObjs.begin())
	//    {
	//	    if (m_nSegment > 0)
 //           {
 //               --m_nSegment;
	//	        m_pSegment = m_pColStaffObjs->m_Segments[m_nSegment];
	//	        m_it = m_pSegment->m_StaffObjs.end();
 //           }
	//	    else
 //           {
 //               //start of collection reached
 //               break;
 //           }
	//    }

	//	//move back
	//	--m_it;

 //       //if start of collection reached, finish
	//    if (m_it == m_pSegment->m_StaffObjs.end()) break;

 //       //check if staffobj is in current staff; otherwise we have to continue moving back
 //       //This check has to be ignored for objects applicable to all staffs (barlines,
 //       //KS & TS), as they are always in first staff but are applicable to all.
 //       if ((*m_it)->GetStaffNum() == m_nStaff || 
 //           (*m_it)->IsBarline() ||
 //           (*m_it)->IsKeySignature() ||
 //           (*m_it)->IsTimeSignature() )
 //       {
 //           //it is in current staff.
 //           //if requested to stop in every chord note, we can finish
 //           if (fAlsoChordNotes) break;
 //           //else we have to skip notes in chord except base note
 //           if (!(*m_it)->IsNoteRest() ||
 //               (*m_it)->IsNoteRest() && ((lmNoteRest*)(*m_it))->IsNote() &&
 //               ( !((lmNote*)(*m_it))->IsInChord() || ((lmNote*)(*m_it))->IsBaseOfChord())
 //              ) break;
 //       }
 //   }

 //   //if start of collection reached, previous object was the first one in current staff.
	////Remain there
	//if (IsAtBeginning())
	//{
	//	m_nSegment = nSegment;
	//	m_pSegment = pSegment;
	//	m_rTimepos = rTimepos;
	//	m_it = it;
	//	return;
	//}

 //   //update cursor timepos
 //   UpdateTimepos();
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
        //This check has to be ignored for objects applicable to all staffs (barlines,
        //KS & TS), as they are always in first staff but are applicable to all.
        if ((*it)->GetStaffNum() == m_nStaff || 
            (*it)->IsBarline() ||
            (*it)->IsKeySignature() ||
            (*it)->IsTimeSignature() )
        {
            //it is in current staff.
            //We have to skip notes in chord except base note
            if (!(*it)->IsNoteRest() ||
                (*it)->IsNoteRest() && ((lmNoteRest*)(*it))->IsNote() &&
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
    //move cursor to first object with time > rNewTime in staff m_nStaff.
    //If no object found, cursor is moved to end of segment, with time rNewTime

    m_it = m_pSegment->m_StaffObjs.begin();
    m_rTimepos = rNewTime;

	if (m_it == m_pSegment->m_StaffObjs.end()) return;      //the segment is empty

    //skip staffobjs with time lower than rNewTime
	while (m_it != m_pSegment->m_StaffObjs.end() && IsLowerTime((*m_it)->GetTimePos(), rNewTime))
        ++m_it;

    if (m_it != m_pSegment->m_StaffObjs.end() && IsEqualTime((*m_it)->GetTimePos(), rNewTime))
    {
        //time found. Advance to first object at this time and in current staff
	    while (m_it != m_pSegment->m_StaffObjs.end() && 
               IsEqualTime((*m_it)->GetTimePos(), rNewTime) &&
               (*m_it)->GetStaffNum() != m_nStaff )
        {
                ++m_it;
        }
        //if object found, return.
	    if (m_it != m_pSegment->m_StaffObjs.end() &&  
            IsEqualTime((*m_it)->GetTimePos(), rNewTime) &&
            (*m_it)->GetStaffNum() == m_nStaff )
        {
            //object found.
            return;
        }

        //No object satisfying the constrains is found.
        //If we are not at end of segment, advance to first object with time > rNewTime.
	    while (m_it != m_pSegment->m_StaffObjs.end() &&
                IsEqualTime((*m_it)->GetTimePos(), rNewTime))
        {
            ++m_it;
        }
    }
    else
    {
        //time does not exist. Cursor is now at first object > rNewTime
        ;
    }
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
        //found. Update time
        UpdateTimepos();
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
		    lmLUnits uDist = xPos - uPos.x;
		    if (uDist > 0.0f && uDist < uMinDist)
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
}

void lmVStaffCursor::MoveCursorToObject(lmStaffObj* pSO)
{
    //Move cursor to required staffobj
    //Precondition: the staffobj is in current instrument

    if (!pSO)
    {
        //No object. Move to end of score
        //m_nStaff = pSO->GetStaffNum();        //do not change Staff TODO: is this ok?
        m_nSegment = m_pColStaffObjs->m_Segments.size() - 1;
	    m_pSegment = m_pColStaffObjs->m_Segments[m_nSegment];
	    m_it = m_pSegment->m_StaffObjs.end();
    }
    else
    {
        //Object exists. Restore cursor info to point to that object
        m_nStaff = pSO->GetStaffNum();
	    m_pSegment = pSO->GetSegment();
        m_nSegment = m_pSegment->m_nNumSegment;
	    m_it = m_pSegment->m_StaffObjs.begin();

        //locate the object
        while(m_it != m_pSegment->m_StaffObjs.end() && *m_it != pSO)
            ++m_it;

	    //object found
	    wxASSERT(m_it != m_pSegment->m_StaffObjs.end());
    }
    UpdateTimepos();
}

lmStaff* lmVStaffCursor::GetCursorStaff()
{
    //return pointer to staff in which cursor is located
    return m_pColStaffObjs->m_pOwner->GetStaff(m_nStaff);
}

//int lmVStaffCursor::GetPageNumber()
//{
//    lmStaffObj* pSO = GetStaffObj();
//    if (pSO)
//        return pSO->GetPageNumber();
//    else
//    {
//        lmTODO(_T("[lmVStaffCursor::GetPageNumber] TODO: Get page number when no SO"));
//        return 0;
//    }
//}
//
lmUPoint lmVStaffCursor::GetCursorPoint()
{
    //compute coordinate for placing cursor and return it

    lmUPoint uPos(0.0f, 0.0f);
    lmStaffObj* pCursorSO = GetStaffObj();

    //variables to contain time and x pos of previous and current staffobjs.
    //I will use subindex 1 for previous and 2 for current staffobj.
    lmLUnits uxStart1, uxStart2;
    lmLUnits uxEnd1, uxEnd2;
    float rTime1, rTime2;
    float rTimeCursor = m_rTimepos;  //save it, as will be lost when MoveLeft(), etc.

    //
    //collect information about staffobjs and shapes' positions
    //

    lmScore* pScore = m_pColStaffObjs->m_pOwner->GetScore();
    wxASSERT(pScore->GetGraphicObject()->IsBox());
    lmLUnits uCaretSpace = pScore->TenthsToLogical(5.0f);   //distance between caret and object

    //get current staffobj info
    if (pCursorSO)
    {
        //get info from cursor staffobj
        rTime2 = pCursorSO->GetTimePos();
        lmShape* pShape2 = pCursorSO->GetShape(m_nStaff);
        if (pShape2)
        {
            uPos.y = GetStaffPosY(pCursorSO);
            uxStart2 = pShape2->GetXLeft();
            uxEnd2 = pShape2->GetXRight();
        }
        else
        {
            //End of score
            //Or no shape! --> //TODO: All objects must have a shape, althought it can be not visible
            wxASSERT(false);
            lmTODO(_T("[lmVStaffCursor::GetCursorPoint] No shape for current sttafobj"));
            uPos.y = 0.0f;  //TODO
            uxStart2 = 0.0f;     //TODO
            uxEnd2 = 0.0f;
        }
    }

    //get info from previous staffobj
    lmStaffObj* pPrevSO = GetPreviousStaffobj();
    if (pPrevSO)
    {
        rTime1 = pPrevSO->GetTimePos();
        lmShape* pShape1 = pPrevSO->GetShape(m_nStaff);
	    if (!pShape1)
            wxASSERT(false);
            //NO shape! --> //TODO: All objects must have a shape, althought it can be not visible
            //uxStart1 = 0.0f;     //TODO
	    else
        {
		    uxStart1 = pShape1->GetXLeft();
            uxEnd1 = pShape1->GetXRight();
        }
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

        return uPos;
    }

    else if (pPrevSO)
    {
        //No current staffobj but previous one exist. Previous one is the last one and
        //the cursor is at the end of the score.
        //Position cursor four lines (40 tenths) at the right of last staffobj
        uPos.y = GetStaffPosY(pPrevSO);
        uPos.x = uxEnd1 + pPrevSO->TenthsToLogical(40);

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
    uPos.y = pBSystem->GetStaffShape(1)->GetYTop();
    uPos.x = pBSystem->GetXLeft() + pScore->TenthsToLogical(10);

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
    if (m_it != m_pSegment->m_StaffObjs.end())
        m_rTimepos = (*m_it)->GetTimePos();
    else
    {
        //We are at end of collection.
        //Get last staffobj and assign its timepos plus its duration
        lmStaffObj* pSO = GetPreviousStaffobj();
        if (pSO && !pSO->IsBarline())
            m_rTimepos = pSO->GetTimePos() + pSO->GetTimePosIncrement();
        else
            m_rTimepos = 0.0f;
    }
}

void lmVStaffCursor::RefreshInternalInfo()
{
    //during insert/remove operations, although m_it is correctly pointing to
    //current object, all other VCursor information could be wrong. So, it
    //is necessary to syncronize all this information.

    //FIX_ME: this is a bad design. Each VCursor method must ensure this coherence before
    //using an internal member variable. If value can not be maintained automatically, it
    //would be better to remove those member variables and to compute the needed
    //information when required

    UpdateTimepos();
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


    //find object to remove
	lmItCSO itDel = find(m_StaffObjs.begin(), m_StaffObjs.end(), pSO);
    wxASSERT(itDel != m_StaffObjs.end());

    //save an iterator pointing to object after object to remove
    lmItCSO itNext = itDel;
    ++itNext;

	//remove the staffobj from the collection. After removal, iterator 'itDel' is invalid
	m_StaffObjs.erase(itDel);

    //if removed object is a note not in chord:
    //  - Shift left all note/rests in this voice and sort the collection
	if (pSO->IsNoteRest() && ((lmNoteRest*)pSO)->IsNote() && !((lmNote*)pSO)->IsInChord())
    {
        //ShiftLeftTimepos((lmNoteRest*)pSO, itNext, ((lmNoteRest*)pSO)->GetTimePosIncrement() );
        ShiftLeftTimepos(itNext,
                         ((lmNoteRest*)pSO)->GetDuration(), 
                         ((lmNoteRest*)pSO)->GetTimePos() + ((lmNoteRest*)pSO)->GetDuration(),
                         ((lmNoteRest*)pSO)->GetVoice()  );

        m_StaffObjs.sort(SortCompare);
    }

    //get object after the removed one
    lmStaffObj* pNextSO = (lmStaffObj*)NULL;
    if (itNext != m_StaffObjs.end())
        pNextSO = *itNext;

    //if removed object is a time signature, re-bar the collection. As this implies removing
    //current segments and creating new ones, all needed context propagation and update is
    //done at the same time. Therefore, no later steps are necessary for this
    //If there are no objects after the removed TS, nothing to re-bar
    if (pSO->IsTimeSignature() && pNextSO)
    {
        //we have to re-bar from current segment to next time signature.
        lmTimeSignature* pNewTS = pNextSO->GetApplicableTimeSignature();
        lmStaffObj* pLastSO = m_pOwner->FindFwdTimeSignature(pNextSO);
        if (pLastSO)
            pLastSO = m_pOwner->FindPrevStaffObj(pLastSO);

        m_pOwner->AutoReBar(pNextSO, pLastSO, pNewTS);
    }

    //If removed staffobj created contexts (clef, TS or KS) the contexts are already
    //unchained from the staves context chain. But it is necessary:
    //- to update pointers to contexts at start of segment; and
    //- to update staffobjs in segment, if affected by the context change
    DoContextRemoval(pSO, pNextSO, fClefKeepPosition, fKeyKeepPitch);

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
    //If not object is pointed by cursor, new object is inserted at the end

    lmItCSO itNewCSO;			//iterator to point to the new inserted object
    lmStaffObj* pCursorSO = (pCursor ? pCursor->GetStaffObj() : (lmStaffObj*)NULL);

    //notes in chord must be added after note pointed by cursor
    bool fAfter = pNewSO->IsNoteRest() && ((lmNoteRest*)pNewSO)->IsNote() && 
                  ((lmNote*)pNewSO)->IsInChord();

    //add staffobj to the collection
    if (pCursorSO)
    {
        //Insert pNewSO before pCursorSO
        itNewCSO = pCursor->GetCurIt();
        if (fAfter)
            ++itNewCSO;
        m_StaffObjs.insert(itNewCSO, pNewSO);	//insert pNewSO before item pointed by cursor
	    --itNewCSO;		                        //now points to the new inserted object
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
			ShiftRightTimepos(itNewCSO, ((lmNoteRest*)pNewSO)->GetDuration() );
			//and sort the collection by timepos
			m_StaffObjs.sort(SortCompare);
		}

        //update barline timepos, to allow for irregular measures
        lmBarline* pBL = GetBarline();
        if (pBL)
            pBL->SetTimePos(m_rMaxTime);
	}

	////DBG  ------------------------------------------------------------------
	//wxLogMessage(_T("[lmSegment::Store] After insertion of %s"),
	//			pNewSO->GetName());
	//wxLogMessage(Dump());
	////END DBG ---------------------------------------------------------------
}

void lmSegment::RecomputeSegmentDuration(lmNoteRest* pNR, float rTimeIncr)
{
    //Duration of note/rest pNR has been changed. Therefore it could be necessary to recompute
    //segment duration and reposition (in time) all objects after pNR.

    //find pNR
	lmItCSO itNR = find(m_StaffObjs.begin(), m_StaffObjs.end(), pNR);
    wxASSERT(itNR != m_StaffObjs.end());

    if (IsHigherTime(rTimeIncr, 0.0f))
        ShiftRightTimepos(itNR, rTimeIncr);
    else
    {
        ++itNR;
        ShiftLeftTimepos(itNR, -rTimeIncr, pNR->GetTimePos() + pNR->GetDuration() - rTimeIncr,
                         pNR->GetVoice() );
    }
}

void lmSegment::ShiftRightTimepos(lmItCSO itStart, float rTimeShift)
{
    //Either, the note/rest pointed by itStart has been inserted, or its duration has been
    //increased.
    //In both cases, we must shift the timepos af all next objects in this voice, starting with
    //staffobj after itStart, and up to the barline (included).
    //The amount of time to shift objects right is rTimeShift. This value is either the duration
    //of the inserted note (if a note inserted) or the increment in duration (positive) if 
    //the duration of itStart note/rest has been modified.

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

    bool fChanges = false;
    int iV = ((lmNoteRest*)(*itStart))->GetVoice();
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
                (*it)->SetTimePos(rTime + rTimeShift);
                fChanges = true;
            }
            else
                break;
        }
        ++it;
	}

    //if any change, update measure duration and review AutoBeam
    if (fChanges)
    {
        UpdateMeasureDuration();
        //if (g_fAutoBeam)
        //    AutoBeam(iV);
    }
}

void lmSegment::ShiftLeftTimepos(lmItCSO itStart, float rTimeShift, float rStartTimepos,
                                 int nVoice)
{
    //The timepos of all consecutive notes/rests, in voice nVoice, starting with the one 
    //pointed by itStart must be modified, because either the previous note/rest has been
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
	lmItCSO it = itStart;
    bool fChanges = false;

	while (it != m_StaffObjs.end())
	{
        if ((*it)->IsNoteRest() && ((lmNoteRest*)(*it))->GetVoice() == nVoice)
        {
            float rTime = (*it)->GetTimePos();
            if (IsEqualTime(rTime, rNextTime))
            {
                (*it)->SetTimePos(rTime - rTimeShift);
                rNextTime = rTime + (*it)->GetTimePosIncrement();
                fChanges = true;
            }
            else
                break;
        }
        ++it;
	}


    //if any change, update measure duration and review AutoBeam
    if (fChanges)
    {
        UpdateMeasureDuration();
        //if (g_fAutoBeam)
        //    AutoBeam(nVoice);
    }
}

void lmSegment::UpdateMeasureDuration()
{
    //determines the new duration of this segment, and updates barline timepos, if
    //barline exists

	lmItCSO it = m_StaffObjs.end();
    --it;
    while (it != m_StaffObjs.end() && (*it)->IsBarline())
        --it;

    float rDuration = 0.0f;
    if (it != m_StaffObjs.end())
    {
        rDuration = (*it)->GetTimePos() + (*it)->GetTimePosIncrement();
    }

    //if segment's duration is irregular attach a warning tag to it
    lmTODO(_T("[lmSegment::UpdateMeasureDuration] TODO: if segment's duration is irregular attach a warning tag to it"));
    m_rMaxTime = rDuration;

    //Update barline timepos if barline exists.
	lmBarline* pBL = GetBarline();
    if (pBL)
        pBL->SetTimePos( m_rMaxTime );
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
    if (it == m_StaffObjs.end())
        m_rMaxTime = 0.0f;          //segment is empty 
    else
        m_rMaxTime = (*it)->GetTimePos() + (*it)->GetTimePosIncrement();
}

float lmSegment::GetMaximumTime()
{
    //returns theoretical segment duration for current time signature

    //get last SO in this segment
	lmItCSO it = m_StaffObjs.end();
    --it;
    if (it == m_StaffObjs.end())
        return lmNO_TIME_LIMIT;

    //get applicable time signature
    lmTimeSignature* pTS = (*it)->GetApplicableTimeSignature();
    if (!pTS)
        return lmNO_TIME_LIMIT;   //no time signature -> infinite segment

    return pTS->GetMeasureDuration();
}

bool lmSegment::HasBarline()
{
    //get last SO in this segment
	lmItCSO it = m_StaffObjs.end();
    --it;
    if (it == m_StaffObjs.end())
        return false;

    return (*it)->IsBarline();
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

void lmSegment::DoContextInsertion(lmStaffObj* pNewSO, lmStaffObj* pNextSO,
                                     bool fClefKeepPosition, bool fKeyKeepPitch)
{
    //When a staffobj is added to the collection, if it created contexts (clef, TS or KS) the
    //created contexts are already chained in the staves. But it is necessary:
    //- to update pointers to contexts at start of segment; and
    //- to update staffobjs in segment, if affected by the context change


    //AWARE: For time signatures, all needed context propagation and update is done when
    //doing the Re-Bar operation. So no need to do anything here.
    if (!(pNewSO->IsClef() || pNewSO->IsKeySignature()) )
        return;     //nothing to do

    //invalidate not applicable flags, so that at maximum, only one of the flags
    //will be true
    if (!pNewSO->IsClef())
        fClefKeepPosition = false;
    if (!pNewSO->IsKeySignature())
        fKeyKeepPitch = false;

    //Update current segment notes, if requested.
    //if pNextSO is NULL it means that we are at end of segment and, therefore, there are
    //no notes in this segment after the inserted clef
    lmClef* pOldClef = pNewSO->GetApplicableClef();
    if (pNextSO && fClefKeepPosition)
    {
        int nStaff = pNewSO->GetStaffNum();
        Transpose((lmClef*)pNewSO, pOldClef, pNextSO, nStaff);
    }
    else if (fKeyKeepPitch)
    {
        //TODO
        //AddRemoveAccidentals((lmKeySignature*)pNewSO);
        lmTODO(_T("[lmSegment::DoContextInsertion] TODO. Key: keep pitch"));
    }

    //determine staves affected by the context change and propagate context change
    //to all following segments. This is necessary so that segments can update pointers
    //to start of segment applicable contexts, and update their contained staffobjs, if
    //affected by the change.
    lmSegment* pNextSegment = GetNextSegment();
    if (pNextSegment)
    {
        if (pNewSO->IsClef())
        {
            //just one staff. Determine context for this staff at end of segment
            int nStaff = pNewSO->GetStaffNum();
            lmContext* pLastContext = FindEndOfSegmentContext(nStaff);

            //inform next segment
            pNextSegment->PropagateContextChange(pLastContext, nStaff, (lmClef*)pNewSO,
                                                 pOldClef, fClefKeepPosition);
        }
        else
        {
            //key signature: all staves
            for (int nStaff=0; nStaff < lmMAX_STAFF; nStaff++)
            {
                //determine context for current staff at end of segment
                lmContext* pLastContext = FindEndOfSegmentContext(nStaff);

                //inform next segment
                pNextSegment->PropagateContextChange(pLastContext, nStaff,
                                                     fKeyKeepPitch);
            }
        }
    }

}

void lmSegment::PropagateContextChange(lmContext* pStartContext, int nStaff, lmClef* pNewClef,
                                       lmClef* pOldClef, bool fClefKeepPosition)
{
    //The context for staff nStaff at start of this segment has changed because a clef has been
    //added or removed.
    //This method updates pointers to contexts at start of segment and, if requested, updates
    //the contained staffobjs if affected by the change.
    //Then, propagates the change to next segment if the start of segment context pointer
    //has been updated.

    if (m_pContext[nStaff-1] == pStartContext)
        return;     //nothing to do. Context are updated

    //update context pointer
    m_pContext[nStaff-1] = pStartContext;

    //Update current segment notes, if requested
    if (fClefKeepPosition)
        Transpose(pNewClef, pOldClef, (lmStaffObj*)NULL, nStaff);

    //propagate to next segment
    lmSegment* pNextSegment = GetNextSegment();
    if (pNextSegment)
        pNextSegment->PropagateContextChange(pStartContext, nStaff, pNewClef, pOldClef,
                                             fClefKeepPosition);
}

void lmSegment::PropagateContextChange(lmContext* pStartContext, int nStaff,
                                       bool fKeyKeepPitch)
{
    //The context for staff nStaff at start of this segment has changed because a key signature
    //has been added or removed.
    //This method updates pointers to contexts at start of segment and, if requested, updates
    //the contained staffobjs if affected by the change.
    //Then, propagates the change to next segment if the start of segment context pointer
    //has been updated.

    if (m_pContext[nStaff-1] == pStartContext)
        return;     //nothing to do. Context are updated

    //update context pointer
    m_pContext[nStaff-1] = pStartContext;

    //Update current segment notes, if requested
    if (fKeyKeepPitch)
    {
        //TODO
        //AddRemoveAccidentals((lmKeySignature*)pNewSO);
        lmTODO(_T("[lmSegment::PropagateContextChange] TODO Key keep Pitch"));
    }

    //propagate to next segment
    lmSegment* pNextSegment = GetNextSegment();
    if (pNextSegment)
        pNextSegment->PropagateContextChange(pStartContext, nStaff,
                                             fKeyKeepPitch);
}

void lmSegment::Transpose(lmClef* pNewClef, lmClef* pOldClef, lmStaffObj* pStartSO,
                          int nStaff)
{
    //A clef has been inserted. Iterate along the staffobjs of this segment and re-pitch
    //the notes to maintain its staff position.

    //locate start point
	lmItCSO itSO;
    if (pStartSO)
    {
	    itSO = find(m_StaffObjs.begin(), m_StaffObjs.end(), pStartSO);
        wxASSERT(itSO != m_StaffObjs.end());
    }
    else
        itSO = m_StaffObjs.begin();

    //iterate until end of segment or new cleft
	while (itSO != m_StaffObjs.end() && !(*itSO)->IsClef())
	{
		if ((*itSO)->IsNoteRest() && (*itSO)->GetStaffNum() == nStaff &&
            ((lmNote*)(*itSO))->IsNote())
        {
            //note in the affected staff. Re-pitch it to keep staff position
            ((lmNote*)(*itSO))->ChangePitch(pOldClef, pNewClef);
        }
        ++itSO;
	}
}

void lmSegment::AutoBeam(int nVoice)
{
    //A note/rest in voice nVoice has been added/removed in this segment. Review beaming
    //in specified voice

    //vector to store notes that form a beam
    std::vector<lmNoteRest*> cBeamedNotes;

    //find first note in voice nVoice in this segment
	lmItCSO it = m_StaffObjs.begin();
	for (; it != m_StaffObjs.end(); ++it)
	{
		if ((*it)->IsNoteRest() && 
            ((lmNoteRest*)(*it))->IsNote() &&
            ((lmNoteRest*)(*it))->GetVoice() == nVoice)
            break;
    }
    if (it == m_StaffObjs.end())
        //no notes in voice nVoice. Only rests. Nothing to do
        return;

    //get context and get current time signature.
    lmContext* pContext = (*it)->GetCurrentContext();
    if (!pContext) return;           //no context yet
    lmTimeSignature* pTS = pContext->GetTime();
    if (!pTS) return;               //no time signature defined yet
    int nBeatType = pTS->GetBeatType();
    int nBeats = pTS->GetNumBeats();

    //Explore all segment (only involved voice) and select notes/rests that should be beamed
	for (; it != m_StaffObjs.end(); ++it)
	{
		if ((*it)->IsNoteRest() && ((lmNoteRest*)(*it))->GetVoice() == nVoice)
        {
            //note in the affected voice
            lmNoteRest* pNR = (lmNoteRest*)(*it);
            int nPos = GetNoteBeatPosition(pNR->GetTimePos(), nBeats, nBeatType);
            if (nPos != lmOFF_BEAT)     //nPos = lmOFF_BEAT or number of beat (0..n)
            {
                //A new beat starts. Terminate previous beam and start a new one
                AutoBeam_CreateBeam(cBeamedNotes);
                cBeamedNotes.clear();
            }

            //add note to current beam if smaller than an eighth
            if (pNR->GetNoteType() > eQuarter)
                cBeamedNotes.push_back(pNR);
        }
	}

    //terminate last beam
    AutoBeam_CreateBeam(cBeamedNotes);
    cBeamedNotes.clear();
}

void lmSegment::AutoBeam_CreateBeam(std::vector<lmNoteRest*>& cBeamedNotes)
{
    //When this method is invoked, vector cBeamedNotes contains the notes/rests that should
    //be beamed toghether. Review current beam information and modify it so that they
    //form a beamed group.

    if (cBeamedNotes.size() == 0) return;

    //remove rests at the end
    int nLastNote = (int)cBeamedNotes.size() - 1;       //0..n-1
    while (cBeamedNotes[nLastNote]->IsRest() && nLastNote > 0)
        --nLastNote;

    if (nLastNote < 1) return;      //only one note. No beam possible

    //Real beam information will be computed by the beam, when method AutoSetUp is invoked.
    //Therefore, We do not have to worry here about the beam details. So, it is enough to
    //create the beam and to add the notes to the beam.

    lmBeam* pBeam = (lmBeam*)NULL;
    std::vector<lmNoteRest*>::iterator it = cBeamedNotes.begin();
    for (int i=0; it != cBeamedNotes.end(); ++it, ++i)
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

	lmItCSO it = m_StaffObjs.end();
    --it;
    while (it != m_StaffObjs.end())
    {
        if ((*it)->IsClef() && (*it)->GetStaffNum() == nStaff)
            return ((lmClef*)(*it))->GetContext();
        else if ((*it)->IsTimeSignature())
            return ((lmTimeSignature*)(*it))->GetContext(nStaff);
        else if ((*it)->IsKeySignature())
            return ((lmKeySignature*)(*it))->GetContext(nStaff);
        else
            --it;
    }

    return m_pContext[nStaff-1];
}

void lmSegment::DoContextRemoval(lmStaffObj* pOldSO, lmStaffObj* pNextSO, bool fClefKeepPosition,
                                 bool fKeyKeepPitch)

{
    //When a staffobj is removed from the collection, if it created contexts (clef, TS or KS)
    //the created contexts has been removed from the staves contexts chain. But it is necessary:
    //- to update pointers to contexts at start of segment; and
    //- to update staffobjs in segment, if affected by the context change


    //AWARE: For time signatures, all needed context propagation and update, is done when
    //doing the Re-Bar operation. So no need to do anything here.
    if (!(pOldSO->IsClef() || pOldSO->IsKeySignature()) )
        return;     //nothing to do

    //invalidate not applicable flags, so that at maximum, only one of the flags
    //will be true
    if (!pOldSO->IsClef())
        fClefKeepPosition = false;
    if (!pOldSO->IsKeySignature())
        fKeyKeepPitch = false;

    //Update current segment notes, if requested.
    //if pNextSO is NULL it means that we are at end of segment and, therefore, there are
    //no notes in this segment after the deleted clef
    lmClef* pNewClef = pOldSO->GetApplicableClef();
    if (pNextSO && fClefKeepPosition)
    {
        int nStaff = pNextSO->GetStaffNum();
        if (pNewClef)
            Transpose(pNewClef, (lmClef*)pOldSO, pNextSO, nStaff);
        //else
        //TODO: No clef in the score. What to do with existing notes?
    }
    else if (fKeyKeepPitch)
    {
        //TODO
        //AddRemoveAccidentals((lmKeySignature*)pOldSO);
        lmTODO(_T("[lmSegment::DoContextRemoval] TODO. Key: keep pitch"));
    }

    //determine staves affected by the context change and propagate context change
    //to all following segments. This is necessary so that segments can update pointers
    //to start of segment applicable contexts, and update their contained staffobjs, if
    //affected by the change.
    lmSegment* pNextSegment = GetNextSegment();
    if (pNextSegment)
    {
        if (pOldSO->IsClef())
        {
            //just one staff. Determine context for this staff at end of segment
            int nStaff = pOldSO->GetStaffNum();
            lmContext* pLastContext = FindEndOfSegmentContext(nStaff);

            //inform next segment
            pNextSegment->PropagateContextChange(pLastContext, nStaff, pNewClef,
                                                 (lmClef*)pOldSO, fClefKeepPosition);
        }
        else
        {
            //key signature: all staves
            for (int nStaff=0; nStaff < lmMAX_STAFF; nStaff++)
            {
                //determine context for current staff at end of segment
                lmContext* pLastContext = FindEndOfSegmentContext(nStaff);

                //inform next segment
                pNextSegment->PropagateContextChange(pLastContext, nStaff,
                                                     fKeyKeepPitch);
            }
        }
    }
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
        else
            ;   //TODO
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
            if (!IsLowerTime(rTime, m_Segments[nSegment]->GetMaximumTime()) )
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
    m_Segments[nSegment]->DoContextInsertion(pNewSO, pCursorSO, fClefKeepPosition, fKeyKeepPitch);
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

    //AWARE. If object to remove is a Time Signature, after removal the score could have
    //been re-bar. Therefore, all segments might have been recreated and, therefore, all
    //pointers to segments are invalid. Get them again
    if (pSO->IsTimeSignature())
    {
        if (pCursorSO)
        {
            pSegment = pCursorSO->GetSegment();
            //saved cursor information is no longer valid. Update it
            tVCState.nSegment = pSegment->GetNumSegment();
        }
        //TODO: else case
    }

    //if removed object is a barline:
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

        //As a consequence of joining segments, saved cursor information is no longer
        //valid. In particular we have to fix rTimePos and nSegment.
        --tVCState.nSegment;
        tVCState.rTimepos += rTime;
    }

    //after removing the staffobj, VCursor iterator is pointing to next staffobj but
    //other VCursor variables (rTimePos, in particular) are no longer valid and should be
    //updated.
    pSegment->UpdateDuration();     //ensure it is updated
    m_pVCursor->SetNewCursorState(m_pVCursor->GetScoreCursor(), &tVCState, true);  //true->update timepos

    //finally, if requested, invoke destructor for removed staffobj
    if (fDelete) delete pSO;


    //wxLogMessage(_T("[lmColStaffObjs::Delete] Forcing a dump:");
    //wxLogMessage( Dump() );
    //#if defined(__WXDEBUG__)
    //g_pLogger->LogTrace(_T("lmColStaffObjs::Delete"), Dump() );
    //#endif
}

void lmColStaffObjs::LogObjectToDeletePosition(lmUndoData* pUndoData, lmStaffObj* pSO)
{
    //log info to locate insertion point when undoing delete command

    //get segment and find object
    lmSegment* pSegment = pSO->GetSegment();
	lmItCSO it = find(pSegment->m_StaffObjs.begin(), pSegment->m_StaffObjs.end(), pSO);
    wxASSERT(it != pSegment->m_StaffObjs.end());        //object to delete must exists

    //advance to next object
    ++it;
    if (it != pSegment->m_StaffObjs.end())
    {
        //not at end of segment. done
        pUndoData->AddParam<lmStaffObj*>(*it);
        return;
    }

    //we are at end of segment. If we are deleting the barline advance to next segment
    if (pSO->IsBarline())
    {
        //advance to first object of next segment
        pSegment = pSegment->GetNextSegment();
        if (pSegment)
            it = pSegment->m_StaffObjs.begin();

        if (pSegment && it != pSegment->m_StaffObjs.end())
        {
            //deleting a barline and there is an object in next segment. done
            pUndoData->AddParam<lmStaffObj*>(*it);
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

lmStaffObj* lmColStaffObjs::FindFwdStaffObj(lmStaffObj* pCurSO, EStaffObjType nType)
{
    //Find next staffobj of type nType, starting from pCurSO (including it).

    //define iterator from current StaffObj
    lmSOIterator* pIter = CreateIteratorFrom(eTR_ByTime, pCurSO);
    while(!pIter->EndOfList())
    {
        lmStaffObj* pSO = pIter->GetCurrent();
        if (pSO->GetClass() == nType)
        {
            //object found
            delete pIter;
            return pSO;
        }
        pIter->MoveNext();
    }

    //not found
    delete pIter;
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
    lmSOIterator* pIter = CreateIteratorFrom(eTR_ByTime, pCurSO);
    wxASSERT(!pIter->EndOfList());
    pIter->MoveNext();

    if(!pIter->EndOfList())
    {
        lmStaffObj* pSO = pIter->GetCurrent();
        delete pIter;
        return pSO;
    }
    else
    {
        delete pIter;
        return (lmStaffObj*)NULL;
    }
}

lmStaffObj* lmColStaffObjs::FindPrevStaffObj(lmStaffObj* pCurSO)
{
    //Find staffobj before pCurSO

    if (!pCurSO)
        return (lmStaffObj*)NULL;

    //define iterator from current StaffObj
    lmSOIterator* pIter = CreateIteratorFrom(eTR_ByTime, pCurSO);
    wxASSERT(!pIter->StartOfList());
    pIter->MovePrev();

    if(!pIter->StartOfList())
    {
        lmStaffObj* pSO = pIter->GetCurrent();
        delete pIter;
        return pSO;
    }
    else
    {
        delete pIter;
        return (lmStaffObj*)NULL;
    }
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

lmSOIterator* lmColStaffObjs::CreateIteratorFrom(ETraversingOrder nOrder, lmVStaffCursor* pVCursor)
{
    //creates an iterator pointing to received cursor position

    lmSOIterator* pIter = new lmSOIterator(nOrder, this, pVCursor);
    return pIter;
}

lmSOIterator* lmColStaffObjs::CreateIteratorFrom(ETraversingOrder nOrder, lmStaffObj* pSO)
{
    //creates an iterator pointing to received staffobj

    lmSOIterator* pIter = new lmSOIterator(nOrder, this, pSO);
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

void lmColStaffObjs::RemoveSegment(int nSegment, bool fDeleteStaffObjs)
{
	//      Remove segment nSegment (0..n) and renumber all remaining segments
    // No need to update staffobjs in remaining segments, as they contain pointers
    // the their container segment.
    //      The removed segment is deleted. Contained staffobjs are deleted or not,
    //  depending on flag.

    //
    //AWARE: Contexts in remaining segments are not updated.
    //TODO: When removing a segment, if contexts at start of segment to remove are
    //different from contexts at start of next segment, implies that the segment to
    //remove has clefs, time signatures and/or key signatures. Therefore, the procedures
    //to remove these context creator objects should be applied.


	//remove the segment
	std::vector<lmSegment*>::iterator itS, itNext;
    itS = find(m_Segments.begin(), m_Segments.end(), m_Segments[nSegment]);

    //delete the segment
    if (!fDeleteStaffObjs)
        (*itS)->m_StaffObjs.clear();
    delete *itS;

    //remove segmentt from segments' vector
    m_Segments.erase(itS);

    //renumber all following segments
    for (int i=nSegment; i < (int)m_Segments.size(); i++)
    {
        m_Segments[i]->m_nNumSegment--;
    }
}

void lmColStaffObjs::AutoReBar(lmStaffObj* pFirstSO, lmStaffObj* pLastSO,
                               lmTimeSignature* pNewTS)
{
    //determine involved segments
    int nFirstSegment = pFirstSO->GetSegment()->m_nNumSegment;
    int nLastSegment = (pLastSO ? pLastSO->GetSegment()->m_nNumSegment :
                                  m_Segments.size()-1 );

    //define a list to contain all staffobjs in the segments to re-bar
    std::list<lmStaffObj*> oSource;

    //loop to fill the list with the objects in the segments to re-bar
    float rTimeIncr = 0.0f;
    for (int i=nFirstSegment; i <= nLastSegment; i++)
    {
        //copy objects in list, except barlines. Delete the barlines.
        //TODO: what if barlines have AuxObjs attached? They will be lost.
        std::list<lmStaffObj*>::iterator it = m_Segments[i]->m_StaffObjs.begin();
        while(it != m_Segments[i]->m_StaffObjs.end())
        {
            if (!(*it)->IsBarline())
            {
                //When deleting a barline all staffobjs timepos in next segments must be
                //incremented by current barline timepos. So, we need to adjust current
                //staffobj timepos
                (*it)->SetTimePos( (*it)->GetTimePos() + rTimeIncr );
                oSource.push_back(*it);     //copy object in list
            }
            else
            {
                rTimeIncr += (*it)->GetTimePos();
                delete *it;                 //delete barline
            }
            ++it;
        }
    }


    //At this point we have all staffobjs included in the auxiliary list. We can
    //proceed to remove the segments to re-bar from the collection

    //remove the segments, but do not delete its staffobjs.
    //All but contexts will be updated in RemoveSegment() method
    for (int iS=nFirstSegment; iS <= nLastSegment; iS++)
    {
        //when removing a segment, all remaining segments get renumbered. For instance,
        //after deleting segment 3, segment 4 will be segment 3. Therefore, we have
        //always to delete first segments as all others will, consecutively, occupy
        //its place.
        RemoveSegment(nFirstSegment, false);       //false->do not delete staffobjs
    }


    //At this point the collection is ready for including the new segments. We are
    //going to proceed to prepare the new segments.

    //determine the new required measure duration
    float rMeasureDuration;
    if (!pNewTS)
    {
        //no time signature. Do not add barlines, but create one segment. To get
        //it done, I will set a too big measure duration
        rMeasureDuration = 10000000000000000.0f;
        //TODO: When no TS, we could try to insert hidden barlines to deal with no time
        //      signature scores
    }
    else
        rMeasureDuration = pNewTS->GetMeasureDuration();

    //loop to create new segments.

    //insert the first segment. It will occupy the place of first removed segment
    int nCurSegment = nFirstSegment-1;
    CreateNewSegment(nCurSegment++);                        //it is inserted after segment nFirstSegment-1
    lmSegment* pCurSegment = m_Segments[nCurSegment];       //get the new segment

    std::list<lmStaffObj*>::iterator it = oSource.begin();
    while(it != oSource.end())
    {
        //checking if a segment is full is tricky. It is not enough just to check,
        //after adding a new SO, that the measure duration has been reached.
        //Next objects to add could be on a different voice or be a goBack ctrol object.
        //Therefore, we have can not be sure if a measure is full until we add a staffobj
        //and its assigned timepos results equal or greater than the required measure duration.

        //add staffobj to segment
        lmStaffObj* pSO = *it;
        pCurSegment->Store(pSO, (lmVStaffCursor*)NULL);

        //now we can reliable check if measure is full
        if (IsHigherTime(pSO->GetTimePos(), rMeasureDuration) ||
            IsEqualTime(pSO->GetTimePos(), rMeasureDuration) )
        {
            //segment was full. Remove added staffobj
            pCurSegment->Remove(pSO, false, false, false);  //false -> do not delete object
                                                            //false -> fClefKeepPosition
                                                            //false -> fKeyKeepPitch

            //add barline
            lmBarline* pBar = new lmBarline(lm_eBarlineSimple, m_pOwner, lmVISIBLE);
            pBar->SetTimePos( pSO->GetTimePos() );
            pCurSegment->Store(pBar, (lmVStaffCursor*)NULL);

            //adjust timepos of remaining staffobjs
            float rTime = pSO->GetTimePos();
            std::list<lmStaffObj*>::iterator itAux = it;
            while(itAux != oSource.end())
            {
                (*itAux)->SetTimePos( (*itAux)->GetTimePos() - rTime );
                ++itAux;
            }

            //insert a new segment and add the removed staffobj to it
            CreateNewSegment(nCurSegment++);                    //it is inserted after segment nCurSegment
            pCurSegment = m_Segments[nCurSegment];              //get the new segment
            pCurSegment->Store(pSO, (lmVStaffCursor*)NULL);
        }

        //get next staffobj to add
        ++it;
    }

    //At this point all new segments have been created and inserted in the collection.
    //But last inserted segment has no barline.
    //If there are more segments after last inserted one, it is necessary:
    //1. to add a barline, and in that case, if it is not full, fill it with rests.
    //2. propagate contexts and update segment pointers after last inserted segment

    if (nCurSegment < (int)m_Segments.size()-1)
    {
        //There are more segments.

        //Fill with rests
        lmTODO(_T("[lmColStaffObjs::AutoReBar] Fill last segment with rests"));

        //and add barline
        lmBarline* pBar = new lmBarline(lm_eBarlineSimple, m_pOwner, lmVISIBLE);
        pBar->SetTimePos( rMeasureDuration );
        pCurSegment->Store(pBar, (lmVStaffCursor*)NULL);

        //propagate contexts and update segment pointers after last inserted segment
        lmTODO(_T("[lmColStaffObjs::AutoReBar] propagate contexts and update segment pointers after last inserted segment"));
    }

    //clear the source list. StaffObjs in it can not be deleted as they have been included
    //in the new segments
    oSource.clear();
}

void lmColStaffObjs::UpdateSegmentContexts(lmSegment* pSegment)
{
    //When add/inserting a new segment it is necessary to find the applicable contexts at
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
    while(*it != pThisSO && it != pSegment->m_StaffObjs.end())
	{
        lmStaffObj* pSO = *it;
		if (pSO->IsNoteRest() && ((lmNote*)pSO)->IsNote())
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
		if (pSO->GetClass() == eSFOT_NoteRest && ((lmNote*)pSO)->IsNote())
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

