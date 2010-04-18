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
#include "Instrument.h"
#include "Context.h"
#include "ColStaffObjs.h"
#include "StaffObjIterator.h"
#include "ChordLayout.h"
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
// - Keep figured bass objects inmediately before the note/rest at same timepos
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
             && pSO1->IsNote() && pSO2->IsNote()
             && ((lmNote*)pSO1)->GetChord() < ((lmNote*)pSO2)->GetChord() )
        fValue = true, nCheck=3;

    //here they are ordered by timepos and duration. Chords: Ensure base note precedence
    else if (IsEqualTime(pSO1->GetTimePos(), pSO2->GetTimePos())
             && IsEqualTime(pSO1->GetTimePosIncrement(), pSO2->GetTimePosIncrement())
             && pSO1->IsNote() && pSO2->IsNote()
             && ((lmNote*)pSO1)->GetChord() == ((lmNote*)pSO2)->GetChord()
             && ((lmNote*)pSO1)->IsBaseOfChord() && ((lmNote*)pSO2)->IsInChord() )
        fValue = true, nCheck=4;

    //if chord, ensure that last note is the last in chord
    else if (IsEqualTime(pSO1->GetTimePos(), pSO2->GetTimePos())
             && IsEqualTime(pSO1->GetTimePosIncrement(), pSO2->GetTimePosIncrement())
             && pSO1->IsNote() && pSO2->IsNote()
             && ((lmNote*)pSO1)->GetChord() == ((lmNote*)pSO2)->GetChord()
             && ((lmNote*)pSO1)->IsInChord() && ((lmNote*)pSO2)->IsLastOfChord() )
        fValue = true, nCheck=11;

    //If chord and right ordering, do not make more checks
    else if (IsEqualTime(pSO1->GetTimePos(), pSO2->GetTimePos())
             && IsEqualTime(pSO1->GetTimePosIncrement(), pSO2->GetTimePosIncrement())
             && pSO1->IsNote() && pSO2->IsNote()
             && ( ((lmNote*)pSO1)->GetChord() == ((lmNote*)pSO2)->GetChord() )
             && ((lmNote*)pSO1)->IsInChord() && ((lmNote*)pSO2)->IsBaseOfChord() )
        fValue = false, nCheck=5;

    //elements of same type ordered by staff number
    else if (pSO1->GetScoreObjType() == pSO2->GetScoreObjType()
             && IsEqualTime(pSO1->GetTimePos(), pSO2->GetTimePos())
             && IsEqualTime(pSO1->GetTimePosIncrement(), pSO2->GetTimePosIncrement())
             && pSO1->GetStaffNum() < pSO2->GetStaffNum())
        fValue = true, nCheck=6;

    //ordered by timepos and duration. Ensure figured bass go after other objects
    else if (IsEqualTime(pSO1->GetTimePos(), pSO2->GetTimePos())
             && IsEqualTime(pSO1->GetTimePosIncrement(), pSO2->GetTimePosIncrement())
             && !(pSO1->IsNote() || pSO1->IsRest()) && pSO2->IsFiguredBass() )
        fValue = true, nCheck=7;

     //ordered by timepos and duration. Ensure figured bass go before notes/rests
    else if (IsEqualTime(pSO1->GetTimePos(), pSO2->GetTimePos())
             && IsEqualTime(pSO1->GetTimePosIncrement(), pSO2->GetTimePosIncrement())
             && (pSO1->IsNote() || pSO1->IsRest()) && pSO2->IsFiguredBass())
        fValue = false, nCheck=8;

    //ordered by timepos and duration. Ensure figured bass go after other objects
    else if (IsEqualTime(pSO1->GetTimePos(), pSO2->GetTimePos())
             && IsEqualTime(pSO1->GetTimePosIncrement(), pSO2->GetTimePosIncrement())
             && pSO1->IsFiguredBass() && !(pSO2->IsNote() || pSO2->IsRest()) )
        fValue = false, nCheck=9;

   else
        fValue = false, nCheck=10;

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
        && pSO1->IsNote() && pSO2->IsNote()
        && ((lmNote*)pSO1)->GetChord() < ((lmNote*)pSO2)->GetChord() )
        return true;

    //here they are ordered by timepos and duration. Chords: Ensure base note precedence
    if (IsEqualTime(pSO1->GetTimePos(), pSO2->GetTimePos())
        && IsEqualTime(pSO1->GetTimePosIncrement(), pSO2->GetTimePosIncrement())
        && pSO1->IsNote() && pSO2->IsNote()
        && ((lmNote*)pSO1)->GetChord() == ((lmNote*)pSO2)->GetChord()
        && ((lmNote*)pSO1)->IsBaseOfChord() && ((lmNote*)pSO2)->IsInChord() )
        return true;

    //if chord, ensure that last note is the last in chord
    if (IsEqualTime(pSO1->GetTimePos(), pSO2->GetTimePos())
        && IsEqualTime(pSO1->GetTimePosIncrement(), pSO2->GetTimePosIncrement())
        && pSO1->IsNote() && pSO2->IsNote()
        && ((lmNote*)pSO1)->GetChord() == ((lmNote*)pSO2)->GetChord()
        && ((lmNote*)pSO1)->IsInChord() && ((lmNote*)pSO2)->IsLastOfChord() )
        return true;

    //If chord and right ordering, do not make more checks
    if (IsEqualTime(pSO1->GetTimePos(), pSO2->GetTimePos())
        && IsEqualTime(pSO1->GetTimePosIncrement(), pSO2->GetTimePosIncrement())
        && pSO1->IsNote() && pSO2->IsNote()
        && ((lmNote*)pSO1)->GetChord() == ((lmNote*)pSO2)->GetChord()
        && ((lmNote*)pSO1)->IsInChord() && ((lmNote*)pSO2)->IsBaseOfChord() )
        return false;

    //elements of same type ordered by staff number
    if (pSO1->GetScoreObjType() == pSO2->GetScoreObjType()
        && IsEqualTime(pSO1->GetTimePos(), pSO2->GetTimePos())
        && IsEqualTime(pSO1->GetTimePosIncrement(), pSO2->GetTimePosIncrement())
        && pSO1->GetStaffNum() < pSO2->GetStaffNum())
        return true;

    //ordered by timepos and duration. Ensure figured bass go after other objects
    if (IsEqualTime(pSO1->GetTimePos(), pSO2->GetTimePos())
             && IsEqualTime(pSO1->GetTimePosIncrement(), pSO2->GetTimePosIncrement())
             && !(pSO1->IsNote() || pSO1->IsRest()) && pSO2->IsFiguredBass() )
        return true;

     //ordered by timepos and duration. Ensure figured bass go before notes/rests
    if (IsEqualTime(pSO1->GetTimePos(), pSO2->GetTimePos())
             && IsEqualTime(pSO1->GetTimePosIncrement(), pSO2->GetTimePosIncrement())
             && (pSO1->IsNote() || pSO1->IsRest()) && pSO2->IsFiguredBass())
        return false;

    //ordered by timepos and duration. Ensure figured bass go after other objects
    if (IsEqualTime(pSO1->GetTimePos(), pSO2->GetTimePos())
             && IsEqualTime(pSO1->GetTimePosIncrement(), pSO2->GetTimePosIncrement())
             && pSO1->IsFiguredBass() && !(pSO2->IsNote() || pSO2->IsRest()) )
        return false;


    return false;

#endif
}



//=======================================================================================
// lmCursorState implementation
//=======================================================================================

lmCursorState::lmCursorState()
    : m_nInstr(0)
    , m_nStaff(-1)
	, m_rTimepos(0.0f)
    , m_nObjID(lmNULL_ID)
{
}

lmCursorState::lmCursorState(int nInstr, int nStaff, float rTimepos, lmStaffObj* pSO)
    : m_nInstr(nInstr)
    , m_nStaff(nStaff)
	, m_rTimepos(rTimepos)
{
    if (pSO)
        m_nObjID = pSO->GetID();
    else
        m_nObjID = lmNULL_ID;
}

lmCursorState::lmCursorState(int nInstr, int nStaff, float rTimepos, long nObjID)
    : m_nInstr(nInstr)
    , m_nStaff(nStaff)
	, m_rTimepos(rTimepos)
    , m_nObjID(nObjID)
{
}

lmStaffObj* lmCursorState::GetStaffObj(lmScore* pScore)
{
    return (lmStaffObj*)pScore->GetScoreObj(m_nObjID);
}



//=======================================================================================
// lmScoreCursor implementation
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

lmScoreCursor::lmScoreCursor(lmScore* pOwnerScore)
    : m_pScore(pOwnerScore)
    , m_nInstr(0)
	, m_nStaff(1)
	, m_rTimepos(0.0f)
    , m_nSegment(-1)
	, m_pColStaffObjs((lmColStaffObjs*)NULL)
    , m_pIt((lmSOIterator*)NULL)
{
}

lmScoreCursor::~lmScoreCursor()
{
    if (m_pIt)
        delete m_pIt;
}

void lmScoreCursor::MoveToStartOfInstrument(int nInstr)
{
    //Moves cursor to first instrument, at timepos 0 after prolog.

	if (!(m_pScore->m_cInstruments).empty())
	{
        PointCursorToInstrument(nInstr);
        MoveAfterProlog();
	}
	else
		m_nInstr = 0;		//cursor no OK
}

void lmScoreCursor::MoveToEnd(lmVStaff* pVStaff, int nStaff)
{
    //Move cursor to end of specified staff in instrument VStaff.

	//point to VStaff owner instrument
	lmInstrument* pInstr = pVStaff->GetOwnerInstrument();
    PointCursorToInstrument( m_pScore->GetNumberOfInstrument(pInstr) );

    //move to end of VStaff collection, at specified staff
    MoveCursorToObject( m_pColStaffObjs->GetLastSO() );
    m_nStaff = nStaff;
    MoveRight();
}

lmCursorState lmScoreCursor::GetState()
{
    lmCursorState oState(m_nInstr, m_nStaff, m_rTimepos, m_pIt->GetCurrent());
    return oState;
}

void lmScoreCursor::PointCursorToInstrument(int nInstr)
{
    //If cursor is currently pointing to requested instrument does nothing.
    //Else moves cursor to requested instrument. Normally after calling this method
    //the cursor will be repositioned within the instrument so nothing else would be
    //necessary. But for safety, the cursor is left positioned at start of collection,
    //that is, to first object in first segment.

    if (GetCursorInstrumentNumber() == nInstr)
        return;     //nothing to do. Instrument is already in use

    //point to the requested instrument
    lmInstrument* pInstr = m_pScore->m_cInstruments[nInstr-1];
	m_pColStaffObjs = pInstr->GetCollection();
    m_nInstr = pInstr->GetNumInstr();

    //create iterator if necessary
    if (m_pIt)
    {
        if (!m_pIt->IsManagingCollection(m_pColStaffObjs))
        {
            delete m_pIt;
            m_pIt = new lmSOIterator(m_pColStaffObjs, (lmStaffObj*)NULL);
        }
    }
    else
        m_pIt = new lmSOIterator(m_pColStaffObjs, (lmStaffObj*)NULL);

    //for safety, point to first object in first segment
    ResetCursor();
}

void lmScoreCursor::MoveRight(bool fAlsoChordNotes)
{
    if (!IsAtEnd())
        DoMoveRight(fAlsoChordNotes);
    else
    {
		//if current instrument has more staves, advance to next staff.
		//else to first staff of next instrument
		lmInstrument* pInstr = m_pScore->m_cInstruments[GetCursorInstrumentNumber()-1];
		int nStaff = GetCursorNumStaff();
		if (nStaff < pInstr->GetNumStaves())
		{
			//advance to next staff of current instrument
			DoMoveToFirst(++nStaff);
		}
		else if ((int)((m_pScore->m_cInstruments).size()) > GetCursorInstrumentNumber())
		{
			//advance to first staff of next instrument
            PointCursorToInstrument(GetCursorInstrumentNumber()+1);
            DoMoveToFirst(1);   //move to first object in first staff
		}
        else
        {
            //no more instruments. Remain at the end
        }
    }
}

void lmScoreCursor::MoveLeft(bool fPrevObject)
{
    if (!IsAtBeginning())
        DoMoveLeft(fPrevObject);
    else
    {
        //go back to last staff of prev instrument
        //TODO
    }
}

void lmScoreCursor::MoveAfterProlog(int nStaff)
{
    //move cursor to initial position of current instrument: at timepos 0 after
    //prolog, in staff nStaff [1..n].

    DoMoveToFirst(nStaff);		//move to first object in staff nStaff

    lmStaffObj* pSO = GetStaffObj();
    if (pSO)
    {
        //advance to skip prolog
        while(pSO && (pSO->IsClef() || pSO->IsTimeSignature() || pSO->IsKeySignature()) )
        {
            DoMoveRight();
            pSO = GetStaffObj();
        }
    }
}

void lmScoreCursor::MoveUp()
{
	//Move up to previous staff in current system (or to last staff in previous system if
	//we are in first staff of current system), at aproximately the same horizontal paper
	//position

	//get current paper position and current staff
    lmUPoint uPos = GetCursorPoint();
	int nStaff = GetCursorNumStaff();
	int nMeasure = GetSegment();

	//if current instrument has previous staves, keep instrument and decrement staff
	if (nStaff > 1)
	{
		DoMoveToSegment(nMeasure, --nStaff, uPos);
		return;
	}

	//else if this is not the first instrument, take last staff of prev. instrument
	else if (GetCursorInstrumentNumber() > 1)
	{
		int nInstr = GetCursorInstrumentNumber() - 1;
		nStaff = m_pScore->GetInstrument(nInstr)->GetNumStaves();
		PointCursorToInstrument(nInstr);
		DoMoveToSegment(nMeasure, nStaff, uPos);
		return;
	}

	//else if this is not the first system move up to last staff on previous system
	lmStaffObj* pCursorSO = GetStaffObj();
	if (pCursorSO)
	{
		lmShape* pShape2 = pCursorSO->GetShape();
		if (pShape2)
		{
			lmBoxSystem* pSystem = pShape2->GetOwnerSystem();
			int nSystem = pSystem->GetSystemNumber();
			lmBoxScore* pBScore = pSystem->GetBoxScore();
			if (nSystem > 1)
			{
				pSystem = pBScore->GetSystem(--nSystem);
				nMeasure = pSystem->GetNumMeasureAt(uPos.x);
				if (nMeasure > 0)
				{
					int nInstr = m_pScore->GetNumInstruments();
					nStaff = m_pScore->GetInstrument(nInstr)->GetNumStaves();
					PointCursorToInstrument(nInstr);
					DoMoveToSegment(nMeasure-1, nStaff, uPos);
					return;
				}
			}
		}
	}

	//else, remain at current position
}

void lmScoreCursor::MoveDown()
{
	//Move to next staff in current system (or to first staff in next system if we are in
	//last staff of current system), at aproximately the same horizontal paper position

	//get current paper position and current staff
    lmUPoint uPos = GetCursorPoint();
	int nStaff = GetCursorNumStaff();
	int nSegment = GetSegment();

	//if current instrument has more staves, keep instrument and increment staff
	lmInstrument* pInstr = m_pScore->GetInstrument(GetCursorInstrumentNumber());
	if (pInstr->GetNumStaves() > nStaff)
	{
		DoMoveToSegment(nSegment, ++nStaff, uPos);
		return;
	}

	//else if there are more instrument, take first staff of next instrument
	else if (GetCursorInstrumentNumber() < m_pScore->GetNumInstruments())
	{
		PointCursorToInstrument(GetCursorInstrumentNumber() + 1);
		DoMoveToSegment(nSegment, 1, uPos);
		return;
	}

	//else if there are more systems move to first staff on next system
	lmStaffObj* pCursorSO = GetStaffObj();
	if (pCursorSO)
	{
		lmShape* pShape2 = pCursorSO->GetShape();
		if (pShape2)
		{
			lmBoxSystem* pSystem = pShape2->GetOwnerSystem();
			int nSystem = pSystem->GetSystemNumber();
			lmBoxScore* pBScore = pSystem->GetBoxScore();
			if (nSystem < pBScore->GetNumSystems())
			{
				pSystem = pBScore->GetSystem(++nSystem);
				nSegment = pSystem->GetNumMeasureAt(uPos.x) - 1;
				if (nSegment >= 0)
				{
					PointCursorToInstrument(1);
					DoMoveToSegment(nSegment, 1, uPos);
					return;
				}
			}
		}
	}

	//else, remain at current position
}

void lmScoreCursor::MoveNearTo(lmUPoint uPos, lmVStaff* pVStaff, int nStaff, int nMeasure)
{
    //Move cursor to nearest object to uPos, constrained to specified segment and staff.

	if ((m_pScore->m_cInstruments).empty()) return;

	//get instrument
	lmInstrument* pInstr = pVStaff->GetOwnerInstrument();

	//get cursor and position it at required segment and position
    PointCursorToInstrument( m_pScore->GetNumberOfInstrument(pInstr) );
	DoMoveToSegment(nMeasure - 1, nStaff, uPos);
}

void lmScoreCursor::MoveCursorToObject(lmStaffObj* pSO)
{
    //Move cursor to point to specified staffobj..

    //get instrument
	lmInstrument* pInstr = pSO->GetVStaff()->GetOwnerInstrument();   //get instrument

	//get cursor for that instrument
    PointCursorToInstrument( pInstr->GetNumInstr() );

	//position it at required staffobj
    m_nStaff = pSO->GetStaffNum();
    m_pIt->MoveTo(pSO);
    UpdateTimepos();
}

void lmScoreCursor::MoveTo(lmVStaff* pVStaff, int nStaff, int nMeasure, float rTime,
                           bool fEndOfTime)
{
    //Within the limits of specified segment, move cursor to first object
    //with time >= rTime in current staff. Time is set to rTime.
    //If no object found, cursor is moved to end of segment, with time rTime.
    //If flag fEndOfTime is set, once positioned at time == rTime advaces after
    //last object of current staff with time = rTime

	if ((m_pScore->m_cInstruments).empty()) return;

	//Move to owner instrument of requested VStaff
    PointCursorToInstrument( pVStaff->GetNumInstr() );

    //move to start of required staff/segment
    MoveToStartOfSegment(nMeasure-1, nStaff);

    //Now move to requested time
    MoveToTime(rTime, fEndOfTime);
}

lmStaff* lmScoreCursor::GetCursorStaff()
{
    //return pointer to staff in which cursor is located
    if (IsOK())
        return m_pColStaffObjs->GetOwnerVStaff()->GetStaff(m_nStaff);
    else
        return (lmStaff*)NULL;
}

lmVStaff* lmScoreCursor::GetVStaff()
{
    if (IsOK())
        return m_pColStaffObjs->GetOwnerVStaff();
    else
        return (lmVStaff*)NULL;
}

void lmScoreCursor::SetState(lmCursorState* pState, bool fUpdateTimepos)
{
    //restore cursor information from saved state info. Then if flag fUpdateTimepos is
    //true sets timepos to the pointed object timepos.

    wxASSERT(m_pColStaffObjs);

    //move to requested instrument, if not there
    PointCursorToInstrument(pState->GetNumInstr());

    m_nStaff = pState->GetNumStaff();
    m_rTimepos = pState->GetTimepos();
    m_pIt->MoveTo( pState->GetStaffObj(m_pScore) );
    m_nSegment = m_pIt->GetNumSegment();

    //update timepos, if requested
    if (fUpdateTimepos)
    {
        lmStaffObj* pSO = pState->GetStaffObj(m_pScore);
        if (pSO)
            m_rTimepos = pSO->GetTimePos();
        else
        {
            //Not pointing to an staffobj. Cursor is at end of collection
            m_rTimepos = m_pColStaffObjs->GetSegment(m_nSegment)->GetDuration();
        }
    }
}

lmStaffObj* lmScoreCursor::GetStaffObj()
{
    wxASSERT(m_pIt);
    return m_pIt->GetCurrent();
}

int lmScoreCursor::GetSegment()
{
    //return number of sgment 0..n-1

    wxASSERT(m_pIt);
    return m_pIt->GetNumSegment();
}

lmContext* lmScoreCursor::GetCurrentContext()
{
    //returns context at cursor point.
	//AWARE: This method does not return a context with updated accidentals.
    //       The returned context is valid only for clef, key signature and time
    //       signature.

    lmStaffObj* pSO = GetStaffObj();
    if (!pSO)
        //Not pointing to an staffobj. Cursor is at end of collection
        return m_pColStaffObjs->GetLastContext(m_nStaff);

    //Pointing to an StaffObj. Ensure that pointed object is in this staff
    lmSOIterator it(m_pColStaffObjs, pSO);
    while (pSO)
    {
        if (pSO->IsBarline())
            break;

        if (!pSO->IsKeySignature() && !pSO->IsTimeSignature()
            && (pSO->GetStaffNum() == m_nStaff) )
            break;

        it.MoveNext();
        pSO = it.GetCurrent();
    }

    if (!pSO || pSO->IsBarline())
        //Not pointing to an staffobj. Cursor is at end of collection
        return m_pColStaffObjs->GetLastContext(m_nStaff);
    else
        //Pointing to an StaffObj in right staff.
        return m_pColStaffObjs->GetCurrentContext(pSO);
}

void lmScoreCursor::MoveRightToNextTime()
{
    //Cursor movement in current instrument and staff, by occupied timepos. Stops at
    //end of score. When an empty measure is found, cursor will stop at begining (timepos 0)
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

void lmScoreCursor::MoveLeftToPrevTime()
{
    //Cursor movement in current instrument and staff, by occupied timepos. Stops at
    //start of instrument. Cursor will always stop in each measure at timepos 0 (even
    //if no objects there) and then move to prev measure and stop before barline.


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
        if (!pSO->IsNote()
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
            while (pSO->IsNote()
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
        //PrevObj not found. We are at beginning of segment and we have tried to move to
        //prev segment or we are at start of collection.
        //If CursorTime > 0 or this is first segment, move to position 0 in
        //this segment. Else, move to end of previous segment
        if (IsHigherTime(m_rTimepos, 0.0f) || m_pIt->StartOfCollection())
        {
            //if not at start of segment move to position 0 in this segment
            if (!m_pIt->StartOfCollection())
            {
                PositionAt(0.0f);
            }
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

void lmScoreCursor::PositionAt(float rTargetTimepos)
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

void lmScoreCursor::DoMoveRight(bool fAlsoChordNotes)
{
    MoveRightToNextTime();
    //TODO: Take flag fAlsoChordNotes into account
}

void lmScoreCursor::DoMoveLeft(bool fAlsoChordNotes)
{
    MoveLeftToPrevTime();
    //TODO: Take flag fAlsoChordNotes into account
}

lmStaffObj* lmScoreCursor::GetPreviousStaffobj()
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
            if (!pSO->IsNote()
                || ( !((lmNote*)pSO)->IsInChord() || ((lmNote*)pSO)->IsBaseOfChord())
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

void lmScoreCursor::MoveToTime(float rNewTime, bool fEndOfTime)
{
    //Within the limits of current segment, move cursor to first object
    //with time >= rNewTime in staff m_nStaff.
    //If no object found, cursor is moved to end of segment, with time rNewTime
    //If flag fEndOfTime is set, once positioned at time == rNewTime advaces after
    //last object of current staff with time = rNewTime

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

        //Object found, if not fEndOfTime return. Else advance to end of current time for
        //current staff
	    if (pSO
            && IsEqualTime(pSO->GetTimePos(), rNewTime)
            && pSO->IsOnStaff(m_nStaff) )
        {
            //object found.
            if(!fEndOfTime || pSO->IsBarline()) return;

            //advance to end of current time for current staff
	        while (pSO && IsEqualTime(pSO->GetTimePos(), rNewTime) && pSO->IsOnStaff(m_nStaff))
            {
                m_pIt->MoveNext();
                pSO = m_pIt->GetCurrent();
            }
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

void lmScoreCursor::ResetCursor()
{
    //Move cursor to first object in first segment. No constrains on staff.

	m_nStaff = 1;
	m_rTimepos = 0.0f;
	wxASSERT(m_pColStaffObjs->GetNumMeasures() > 0);

	//position in first object
	m_pIt->MoveFirst();
    UpdateTimepos();
}

void lmScoreCursor::DoMoveToFirst(int nStaff)
{
    //Move cursor to first object in staff nStaff. Segment will be that of found
    //object.
    //If no object found in requested staff, cursor will be moved to first
    //object in instrument.
	//If no staff specified (nStaff==0) remains in current staff

	if (nStaff != 0)
		m_nStaff = nStaff;

	m_rTimepos = 0.0f;
    m_nSegment = -1;        //no StaffObj
	wxASSERT(m_pColStaffObjs->GetNumMeasures() > 0);

	//position in first object
	m_pIt->MoveFirst();
    if (m_nStaff != 0)
    {
        while(!m_pIt->EndOfCollection()
              && !m_pIt->GetCurrent()->IsOnStaff(m_nStaff))
        {
            m_pIt->MoveNext();
        }
    }

    //either found or end of collection. Update time and segment
    lmStaffObj* pSO = m_pIt->GetCurrent();
    if (pSO)
    {
        m_nSegment = pSO->GetSegment()->GetNumSegment();
        UpdateTimepos();
    }
}

void lmScoreCursor::AdvanceToTime(float rTime)
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

void lmScoreCursor::MoveToNextSegment()
{
    //Advances cursor to time 0 in next segment

	m_pIt->MoveNext();
    while(!(m_pIt->ChangeOfMeasure() || m_pIt->EndOfCollection()))
	    m_pIt->MoveNext();

	m_rTimepos = 0.0f;
}

void lmScoreCursor::SkipClefKey(bool fSkipKey)
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

void lmScoreCursor::MoveToStartOfSegment(int nSegment, int nStaff, bool fSkipClef,
                                         bool fSkipKey)
{
    //Limited to current instrument. Move cursor to start of segment, that is to first
    //SO and timepos 0. Set staff. Then, if fSkipClef, advances after last clef in this
    //segment, if any. And then, if fSkipKey, advances after last key, if any.

    wxASSERT(nStaff > 0);
    wxASSERT(nSegment < m_pColStaffObjs->GetNumMeasures());

	m_pIt->MoveTo(m_pColStaffObjs->GetSegment(nSegment)->GetFirstSO());
    m_nStaff = nStaff;
	m_rTimepos = 0.0f;
    m_nSegment = nSegment;

    //skip clef/key if requested
    if (fSkipClef)
        SkipClefKey(fSkipKey);
}

void lmScoreCursor::MoveToStartOfTimepos()
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
	while (!m_pIt->ChangeOfMeasure() && !m_pIt->FirstOfCollection())
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

void lmScoreCursor::DoMoveToSegment(int nSegment, int nStaff, lmUPoint uPos)
{
    //move cursor to nearest object to uPos, constrained to specified segment
    //and staff.

    wxASSERT(nSegment < m_pColStaffObjs->GetNumMeasures());

    m_nStaff = nStaff;
	m_pIt->MoveTo(m_pColStaffObjs->GetSegment(nSegment)->GetFirstSO());
	m_rTimepos = 0.0f;
    m_nSegment = nSegment;

	//if segment empty finish. We are at start of segment
	if (m_pIt->EndOfCollection())
        return;      //the segment is empty

    //move cursor to nearest object to uPos, constrained to this segment and staff
	lmLUnits uMinDist = 1000000.0f;	//any too big value
	lmStaffObj* pFoundSO = (lmStaffObj*)NULL;
    lmStaffObj* pLastNR = (lmStaffObj*)NULL;
	while (!m_pIt->EndOfCollection() && !m_pIt->ChangeOfMeasure())
	{
        lmStaffObj* pSO = m_pIt->GetCurrent();
        if (pSO->IsOnStaff(m_nStaff))
        {
            //save last note
            if (pSO->IsNoteRest())
                pLastNR = pSO;

            //compute distance
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
    {
        //if found object is a barline, the caret will be moved after last
        //note rest found
        if (pFoundSO->IsBarline())
        {
            if (pLastNR)
            {
                //move after the note/rest
                MoveCursorToObject(pLastNR);
                MoveRightToNextTime();
                return;
            }
            else
            {
                //move to timepos 0
                if (nSegment == 0)
                    MoveAfterProlog(m_nStaff);
                else
	                MoveToStartOfSegment(nSegment, m_nStaff, true, true);  //true=skip clef and key if any
                return;
            }
        }
        else
            m_pIt->MoveTo(pFoundSO);
    }
    UpdateTimepos();
}

lmUPoint lmScoreCursor::GetCursorPoint(int* pNumPage)
{
    //compute coordinate for placing cursor and return it
    //Cursor knows nothing about the graphic model. So it is necessary to interact with
    //it and get the necessary information.

    if (!IsOK())
        return lmUPoint(0.0f, 0.0f);

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
    int nSystem = pBPage->GetFirstSystem();
    if (nSystem > 0)
    {
        lmBoxSystem* pBSystem = pBPage->GetSystem(pBPage->GetFirstSystem());
        lmShape* pShape = pBSystem->GetStaffShape(1);
        uPos.y = pShape->GetYTop();
        uPos.x = pShape->GetXLeft() + pScore->TenthsToLogical(20);
    }
    else
    {
        //score totally empty. No system displayed!
        //position cursors at start of page
        uPos.y = pBPage->GetYTop();
        uPos.x = pBPage->GetXLeft() + pScore->TenthsToLogical(20);
    }

    if (pNumPage)
        *pNumPage = pBPage->GetPageNumber();

    return uPos;
}

float lmScoreCursor::GetStaffPosY(lmStaffObj* pSO)
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
	int nInstr = GetCursorInstrumentNumber();
	if (nInstr > 1)
	{
		nRelStaff += m_pScore->GetFirstInstrument()->GetNumStaves();
		for (int i=2; i < nInstr; i++)
		{
			nRelStaff += m_pScore->GetNextInstrument()->GetNumStaves();
		}
	}
	//here we have the staff number relative to total staves in system
    return pSystem->GetStaffShape(nRelStaff)->GetYTop();
}

void lmScoreCursor::UpdateTimepos()
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
                m_rTimepos = 0.0f;  //pSO->GetTimePos();
            else
                m_rTimepos = pSO->GetTimePos() + pSO->GetTimePosIncrement();
        }
        else
            //segment is empty. Place cursor at start of segment
            m_rTimepos = 0.0f;
    }
}

bool lmScoreCursor::IsAtEnd()
{
    return m_pIt->EndOfCollection();
}

bool lmScoreCursor::IsAtBeginning()
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

int lmSegment::GetNumInstr()
{
    return m_pOwner->GetOwnerVStaff()->GetNumInstr();
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

    //if removed object is a note/rest update used voices
	if (pSO->IsNoteRest())
        CountObjectsAndVoices();

    //if removed object is a note not in chord:
    //  - Shift left all note/rests in this voice and sort the collection
	if (pSO->IsNote() && !((lmNote*)pSO)->IsInChord())
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

            //m_pOwner->AutoReBar(pNext, pLastSO, pNewTS);
            OnContextRemoved(pSO, pNext, fClefKeepPosition);
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
            OnContextRemoved(pSO, pNext, fClefKeepPosition);
        else if (pSO->IsKeySignature())
            OnContextRemoved(pSO, pNext, fKeyKeepPitch);
    }

    //finally, if requested, invoke destructor for removed staffobj
    if (fDelete) delete pSO;
}

void lmSegment::Store(lmStaffObj* pNewSO, lmScoreCursor* pCursor)
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
    bool fAfter = pNewSO->IsNote() && ((lmNote*)pNewSO)->IsInChord();

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
	if (pNewSO->IsNote() && ((lmNote*)pNewSO)->IsInChord())
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
	if (pNewSO->IsNoteRest() || pNewSO->IsFiguredBass())
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

    //TODO: if segment's duration is irregular attach a warning tag to it
    //AttachWarning();

    //Update barline timepos if barline exists.
    if (m_pLastSO->IsBarline())
        m_pLastSO->SetTimePos( m_rMaxTime );
}

void lmSegment::AttachWarning()
{
    //The measure is irregular. Mark it.

    if (!m_pFirstSO) return;

    lmSOIterator it(m_pOwner, m_pFirstSO);
	while (!it.ChangeOfMeasure() && !it.EndOfCollection())
	{
        it.GetCurrent()->SetColour(*wxRED);
        it.MoveNext();
    }
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
    //if the segment is ended with a barline, returns barline timepos.
    //Else returns theoretical segment duration for current time signature

    //TODO: Whait is the logic of this? if segment is empty we should return
    //the theoretical duration, based on time signature!
    //get last SO in this segment
    if (!m_pLastSO)
        return lmNO_TIME_LIMIT;

    //if it is a barline, return its timepos
    if (m_pLastSO->IsBarline())
        return m_pLastSO->GetTimePos();

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
    wxString sVoices = _T("Used voices:");
    for (int i=0; i <= lmMAX_VOICE; i++)
    {
        if (IsVoiceUsed(i))
            sVoices += wxString::Format(_T(" %d"), i);
    }
    sDump += sVoices;
    sDump += _T("\n");

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

//-------------
// Contexts management
//-------------

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

void lmSegment::OnContextInserted(lmStaffObj* pNewCCSO, lmStaffObj* pNextSO,
                                  bool fKeepPitchPosition)
{
    OnContextAddedRemoved(true, pNewCCSO, pNextSO, fKeepPitchPosition);
                       // true -> context added
}

void lmSegment::OnContextRemoved(lmStaffObj* pOldCCSO, lmStaffObj* pNextSO,
                                 bool fKeepPitchPosition)
{
    OnContextAddedRemoved(false, pOldCCSO, pNextSO, fKeepPitchPosition);
                       // false -> context removed
}

void lmSegment::OnContextAddedRemoved(bool fAdded, lmStaffObj* pCCSO, lmStaffObj* pNextSO,
                                      bool fKeepPitchPosition)
{
    //When a Contex Creator StaffObj is added to or removed fromo the collection, the
    //contexts chain in lmStaff is updated [this was done at lmVStaff::Cmd_InsertXxxxx
    //or lmVStaff::Cmd_DeleteXxxxx method, when creating/deleting the contexts. ]
    //But it is still necessary:
    //  - to update pointers to contexts at start of segment; and
    //  - to update staffobjs in segment, if affected by the context change
    //This method do these things.
    //Receives:
    //  - fAdded:   true when context insertion, false when context deletion
    //  - pCCSO:    the added/deleted Contex Creator StaffObj, either a Clef or a Key
    //              Signature, as Time signatures are processed in a differente way.
    //  - pNextSO:  The StaffObj that follows pNewCCSO in the collection.
    //  - fKeepPitchPosition: if true, for key signatures will keep notes pitch
    //              (add/remove accidentals, as necessary) and for clefs will keep
    //              notes position (change notes pitch).
    //


    lmKeySignature* pOldKey;
    lmKeySignature* pNewKey;
    lmClef* pOldClef;
    lmClef* pNewClef;

    //1. Update notes (to transpose,change accidentals, etc.) in current segment,
    //if necessary
    if (pCCSO->IsKeySignature())
    {
        lmKeySignature* pKey = (lmKeySignature*)pCCSO;
        pNewKey = (fAdded ? pKey : pKey->GetApplicableKeySignature());
        pOldKey = (fAdded ? pKey->GetApplicableKeySignature() : pKey);
        lmEKeySignatures nOldKeyType = (pOldKey ? pOldKey ->GetKeyType() : earmDo);
        lmEKeySignatures nNewKeyType = (pNewKey ? pNewKey ->GetKeyType() : earmDo);
        UpdateNotesKeyChange(nOldKeyType, nNewKeyType, pNextSO, fKeepPitchPosition);
    }
    else if (pCCSO->IsClef())
    {
        lmClef* pClef = (lmClef*)pCCSO;
        pOldClef = (fAdded ? pClef->GetApplicableClef() : pClef);
        pNewClef = (fAdded ? pClef : pClef->GetApplicableClef());
        lmEClefType nNewClefType = (fAdded ? pClef->GetClefType()
                                           : pClef->GetCtxApplicableClefType());
        lmEClefType nOldClefType = (fAdded ? pClef->GetCtxApplicableClefType()
                                           : pClef->GetClefType());

        int nStaff = pClef->GetStaffNum();
        if (nNewClefType == lmE_Undefined)
            nNewClefType = m_pOwner->GetOwnerVStaff()->GetStaff(nStaff)->GetDefaultClef();
        if (nOldClefType == lmE_Undefined)
            nOldClefType = m_pOwner->GetOwnerVStaff()->GetStaff(nStaff)->GetDefaultClef();

        UpdateNotesClefChange(nOldClefType, nNewClefType, pNextSO, nStaff,
                              fKeepPitchPosition);
    }
    else if (pCCSO->IsTimeSignature())
        ;   //nothing to do with the notes
    else
        wxASSERT(false);

    //2. if no more segments, return. Nothing else to do
    lmSegment* pNextSegment = GetNextSegment();
    if (!pNextSegment)
        return;

    //3. determine staves affected by the context change, and
    //4. propagate context change to next segment.
    if (pCCSO->IsKeySignature())
    {
        //key signature: all staves affected
        for (int nStaff=1; nStaff <= lmMAX_STAFF; nStaff++)
        {
            //4.1. compute new context for this staff at end of segment
            lmContext* pLastContext = FindEndOfSegmentContext(nStaff);

            //4.2 inform next segment
            pNextSegment->OnContextChanged(pLastContext, nStaff,
                                           pOldKey, pNewKey, fKeepPitchPosition);
        }
    }
    else if (pCCSO->IsClef())
    {
        //Clef: only one staff affected
        int nStaff = (pNewClef ? pNewClef->GetStaffNum() : pOldClef->GetStaffNum());

        //4.1. compute new context for this staff at end of segment
        lmContext* pLastContext = FindEndOfSegmentContext(nStaff);

        //4.2 inform next segment
        pNextSegment->OnContextChanged(pLastContext, nStaff, pOldClef, pNewClef,
                                       fKeepPitchPosition);
    }
    else if (pCCSO->IsTimeSignature())
    {
        //time signature: all staves affected
        for (int nStaff=1; nStaff <= lmMAX_STAFF; nStaff++)
        {
            //4.1. compute new context for this staff at end of segment
            lmContext* pLastContext = FindEndOfSegmentContext(nStaff);

            //4.2 inform next segment
            pNextSegment->OnContextChanged(pLastContext, nStaff);
        }
    }
    else
        wxASSERT(false);

}

void lmSegment::UpdateNotesKeyChange(lmEKeySignatures nOldKeyType,
                                     lmEKeySignatures nNewKeyType,
                                     lmStaffObj* pNextSO, bool fKeepPitch)
{
    if (pNextSO)
    {
        if (fKeepPitch)
            AddRemoveAccidentals(nNewKeyType, pNextSO);
        else
            ChangePitch(nOldKeyType, nNewKeyType, pNextSO);
    }
}

void lmSegment::UpdateNotesClefChange(lmEClefType nOldClefType, lmEClefType nNewClefType,
                                      lmStaffObj* pNextSO, int nStaff,
                                      bool fClefKeepPosition)
{
    //if pNextSO is NULL it means that there are no StaffObjs after the inserted clef.
    //Therefore, nothing to do. Also, if it has been requested not to keep notes
    //positions (fClefKeepPosition == false), there is nothing to do.
    if (!(pNextSO && fClefKeepPosition))
        return;

    Transpose(nNewClefType, nOldClefType, pNextSO, nStaff);
}

void lmSegment::OnContextChanged(lmContext* pStartContext, int nStaff,
                                 lmStaffObj* pOldCCSO, lmStaffObj* pNewCCSO,
                                 bool fKeepPitchPosition)
{
    //The context for staff nStaff at start of this segment has changed because a
    //clef, a key signature or a time signature has been added or removed.
    //This method updates pointers to contexts at start of segment and, if requested,
    //updates the contained staffobjs if affected by the change.
    //Then, propagates the change to next segment if necessary.
    //Receives:
    //  - pStartContext: the context that now applies at start of this segment
    //  - nStaff:   the staff affected by the change
    //  - pOldCCSO: the object that no longer applies (i.e the removed clef or the
    //              clef before the new added one)
    //  - pNewCCSO: is the new object that now applies (i.e. the clef before
    //              the removed clef or the new added clef)
    //  - fKeepPitchPosition: if true, for key signatures will keep notes pitch
    //              (add/remove accidentals, as necessary) and for clefs will keep
    //              notes position (change notes pitch).
    //

    wxASSERT(nStaff > 0);

    //1. if context received == current context, return. Done for this staff and
    //for remaining ones!
    if (m_pContext[nStaff-1] == pStartContext)
        return;     //nothing to do. Context is updated. No need to update notes

    //2. update context pointer and save old pointer
    lmContext* pOldContext = m_pContext[nStaff-1];
    m_pContext[nStaff-1] = pStartContext;

    //3. Update notes in current segment (to transpose, change accidentals, etc.),
    //if requested
    if (pNewCCSO)
    {
        if (pNewCCSO->IsClef())
        {
            if (fKeepPitchPosition)
            {
                Transpose( ((lmClef*)pNewCCSO)->GetClefType(),
                           ((lmClef*)pOldCCSO)->GetClefType(),
                           (lmStaffObj*)NULL,
                           nStaff );
            }
        }
        else if (pNewCCSO->IsKeySignature())
        {
            if (fKeepPitchPosition)
            {
                AddRemoveAccidentals( ((lmKeySignature*)pNewCCSO)->GetKeyType(),
                                      (lmStaffObj*)NULL );
            }
            else
            {
                if (!pOldCCSO)
                    return;       //Nothing to do
                ChangePitch( ((lmKeySignature*)pOldCCSO)->GetKeyType(),
                             ((lmKeySignature*)pNewCCSO)->GetKeyType(),
                             (lmStaffObj*)NULL  );
            }
        }
        else if (pNewCCSO->IsTimeSignature())
        {
            //for time signature changes no notes to update
        }
        else
            wxASSERT(false);
    }

        //propagate to next segment if necessary

    //4. if no more segments, nothing else to do. Done for this staff
    lmSegment* pNextSegment = GetNextSegment();
    if (!pNextSegment)
        return;

    //5. Determine applicable context at start of next segment.
    //If next segment start context == old saved context this implies that
    //there are no context creator objects in this segment. Therefore
    //applicable context for next segment is the same than for this segment.
    //Else, there are context creator objects in this segment and it is necessary
    //to compute the new context for this staff at end of segment (We can not assume
    //that the next context is the one created by the found object. Be aware that,
    //for instance, we could be propagating a clef change and the found object is
    //a new key signature)
    lmContext* pNextContext = pStartContext;    //assume no context creators objects
    if (pOldContext != pNextSegment->GetContext(nStaff))
    {
        //there are context creator objects in this segment. Need to
        //compute the context for this staff at end of segment
        pNextContext = FindEndOfSegmentContext(nStaff);
    }

    //6. inform next segment
    pNextSegment->OnContextChanged(pNextContext, nStaff, pOldCCSO, pNewCCSO,
                                   fKeepPitchPosition);
}

void lmSegment::Transpose(lmEClefType nNewClefType, lmEClefType nOldClefType,
                          lmStaffObj* pStartSO, int nStaff)
{
    //A clef has been inserted. Iterate along the staffobjs of this segment and re-pitch
    //the notes to maintain its staff position.
    //pStartSO is the first StaffObj to process, so that this method can be used
    //to transpose only the notes in a segment from a given one. If pStartSO is
    //NULL the whole segment will be transposed.

    wxASSERT(nStaff > 0);
    if (nOldClefType == lmE_Undefined || nNewClefType == lmE_Undefined)
        return;         //the only valid option is to do nothing (keep pitch)

    //locate start point
    lmStaffObj* pFirst = (pStartSO ? pStartSO : m_pFirstSO);
    if (!pFirst) return;

    //iterate until end of segment or new cleft
    lmSOIterator it(m_pOwner, pFirst);
	while (!it.ChangeOfMeasure() && !it.EndOfCollection() && !it.GetCurrent()->IsClef())
	{
        lmStaffObj* pSO = it.GetCurrent();
		if (pSO->IsNote() && pSO->GetStaffNum() == nStaff)
        {
            //note in the affected staff. Re-pitch it to keep staff position
            ((lmNote*)pSO)->ModifyPitch(nNewClefType, nOldClefType);
        }
        it.MoveNext();
	}
}

void lmSegment::AddRemoveAccidentals(lmEKeySignatures nNewKeyType, lmStaffObj* pStartSO)
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


    //determine new applicable accidentals
    int nAccidentals[7];
    lmComputeAccidentals(nNewKeyType, nAccidentals);

    //locate start point
    lmStaffObj* pFirst = (pStartSO ? pStartSO : m_pFirstSO);
    if (!pFirst) return;

    //iterate until end of segment or new key
    lmSOIterator it(m_pOwner, pFirst);
	while (!it.ChangeOfMeasure() && !it.EndOfCollection() && !it.GetCurrent()->IsKeySignature())
	{
        lmStaffObj* pSO = it.GetCurrent();
		if (pSO->IsNote())
        {
            //note. Review displayed accidentals
            int nStep = ((lmNote*)pSO)->GetStep();
            ((lmNote*)pSO)->ComputeAccidentalsToKeepPitch(nAccidentals[nStep]);
        }
        it.MoveNext();
	}
}

void lmSegment::ChangePitch(lmEKeySignatures nOldKeyType, lmEKeySignatures nNewKeyType,
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

    //determine increment/decrement of pitch for each step
    int nNewAcc[7], nOldAcc[7], nPitchDiff[7];
    lmComputeAccidentals(nNewKeyType, nNewAcc);
    lmComputeAccidentals(nOldKeyType, nOldAcc);
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
		if (pSO->IsNote())
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

int lmSegment::FindPosition(lmStaffObj* pSO)
{
    //returns the position of the StaffObj in the segment list (0..n).
    //If not found, returns -1

    wxASSERT(pSO);

    //is segment empty?
    if (!m_pFirstSO)
        return -1;

    //let's find pSO
    int nPos = 0;
    lmSOIterator it(m_pOwner, m_pFirstSO);
	while (!it.EndOfCollection())
	{
        lmStaffObj* pCurSO = it.GetCurrent();
		if (pCurSO == pSO)
            return nPos;    //found

        if (pCurSO == m_pLastSO)
            return -1;      //not found

        ++nPos;
        it.MoveNext();
	}
    wxASSERT(false);    //impossible
    return -1;          //compiler happy
}

//lmStaffObj* lmSegment::GetStaffObj(int nPosition)
//{
//    //returns the StaffObj that occupies position nPosition (0..n) in this segment
//
//    wxASSERT(nPosition >= 0);
//
//    //advance to requested position
//    int nPos = 0;
//    lmSOIterator it(m_pOwner, m_pFirstSO);
//	while (!it.EndOfCollection() && nPos != nPosition)
//	{
//        ++nPos;
//        it.MoveNext();
//	}
//    wxASSERT(nPos == nPosition);
//
//    //found. return it
//    return it.GetCurrent();
//}

lmStaffObj* lmSegment::GetStaffObj(long nID)
{
    //returns the StaffObj with Id nID. It is stored in this segment

    //look for the requested StaffObj
    lmSOIterator it(m_pOwner, m_pFirstSO);
	while (!it.EndOfCollection() && it.GetCurrent()->GetID() != nID)
	{
        it.MoveNext();
	}
    wxASSERT(!it.EndOfCollection());

    //found. return it
    return it.GetCurrent();
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
		if (pSO->IsNote() &&
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
            (*it)->DetachAuxObj(pBeam);
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
    CountObjectsAndVoices();

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
            if (pSO == pLastSO)
                break;
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
    CountObjectsAndVoices();

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
    if (!pFirst)
    {
        //segment is empty: we have deleted final barline
        float rTime = 0.0f;
        if (m_pLastSO)
            rTime = m_pLastSO->GetTimePos() + m_pLastSO->GetTimePosIncrement();
        return rTime;
    }

    lmStaffObj* pStart = m_pLastSO;
    m_pLastSO = pSegment->GetLastSO();
    wxASSERT(m_pLastSO);    //must exists as segment is not empty
    CountObjectsAndVoices();

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

void lmSegment::CountObjectsAndVoices()
{
    //count the number of objects in the collection and recompute the voices
    //used in this segment

    m_nNumSO = 0;
    m_bVoices = 0x00;
    if (!m_pFirstSO)
        return;

    lmSOIterator it(m_pOwner, m_pFirstSO);
	while (!it.ChangeOfMeasure() && !it.EndOfCollection())
	{
        //count object
        ++m_nNumSO;

        //update used voices
        lmStaffObj* pSO = it.GetCurrent();
        if (pSO->IsNoteRest())
            VoiceUsed( ((lmNoteRest*)pSO)->GetVoice() );
        else
            VoiceUsed(0);

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
    : m_pOwner(pOwner)
	, m_nNumStaves(nNumStaves)
{
	//AWARE: lmColStaffObjs constructor is invoked from lmVStaff constructor, before the
	//lmVStaff is ready. Therefore, you can not invoke lmVStaff methods from here. All
	//required VStaff info must be passed in the constructor parameters list.


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
    if (pSO->IsNote() && ((lmNote*)pSO)->IsInChord() && !((lmNote*)pSO)->IsBaseOfChord() )
	{
        lmNote* pNoteBase = ((lmNote*)pSO)->GetChord()->GetBaseNote();
        pSO->SetTimePos(pNoteBase->GetTimePos());
	}

	//else, assign it cursor timepos
	else
		pSO->SetTimePos( GetCursorTimepos() );
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
    //  Assign voice and timepos to the received StaffObj, and store it in the collection,
	//  before object pointed by current cursor, and advance cursor after inserted object.
    //  Manage all details to maintain segments.

	//get segment
	int nSegment = GetCursorSegment();

    //assign timepos and voice to the StaffObj
	AssignTime(pNewSO);

    // if it is a note/rest without voice assigned, assign it a voice
    if (pNewSO->IsNoteRest() && ((lmNoteRest*)pNewSO)->GetVoice() == 0)
    {
	    int iV = AssignVoice(pNewSO, nSegment);
        ((lmNoteRest*)pNewSO)->SetVoice(iV);
    }

	//store new object in the collection
    lmStaffObj* pCursorObj = GetCursorStaffObj();
	Store(pNewSO, fClefKeepPosition, fKeyKeepPitch);

    //if pNewSO is a time signature, Store() could call Autorebar, and this will
    //invalidate pSgement in VCursor. We have to update it
    if (pNewSO->IsTimeSignature())
    {
        if (pCursorObj)
            MoveCursorToObject(pCursorObj);
        //else
            //TODO
    }

    //advance cursor
	lmStaffObj* pCursorSO = GetCursorStaffObj();
    if (!pCursorSO)
    {
        //Cursor is pointing to end of collection. So no need to advance cursor iterator,
        //just to advance time
        if (pNewSO->IsBarline())
        {
            //Advance cursor to time 0 in next segment
            AdvanceCursorToNextSegment();
        }
        else
        {
            //We are at end of score. Advance cursor to time t + duration of inserted object
            AdvanceCursorToTime( pNewSO->GetTimePos() + pNewSO->GetTimePosIncrement() );
        }
    }
    else
    {
        if (pNewSO->IsBarline())
        {
            //Cursor is pointing to object after the inserted barline. So  it points
            //to the right object. But other cursor info (segment, time,..) is wrong
            //Update cursor info
            MoveCursorToObject( pCursorSO );
        }
        else
        {
            //Cursor is pointing to object after the inserted one but could be of different
            //voice. And cursor timepos is not updated as the collection has been altered.
            //So we have to reposition cursor at right time and right object.
            //Advance cursor to time t + duration of inserted object or to next measure
            //if we are at end of measure
            float rTime = pNewSO->GetTimePos() + pNewSO->GetTimePosIncrement();
            //wxLogMessage(_T("[lmColStaffObjs::Add] NumSegments=%d, nSegment=%d, MaximunTime=%.2f, rTime=%.2f, IsLowerTime=%s"),
            //    GetNumSegments(), nSegment, m_Segments[nSegment]->GetMaximumTime(),
            //    rTime, (IsLowerTime(rTime, m_Segments[nSegment]->GetMaximumTime()) ? _T("yes") : _T("no")) );
            if (!IsLowerTime(rTime, m_Segments[nSegment]->GetMaximumTime())
                && nSegment < GetNumSegments()-1 )
                AdvanceCursorToNextSegment();
            else
                MoveCursorToTime(rTime);
        }
    }
}

void lmColStaffObjs::Insert(lmStaffObj* pNewSO, lmStaffObj* pBeforeSO)
{
    //AWARE:
    //  This method is intended only for undo/redo, to re-insert deleted objects when user
    //  request to undo an action.

    //set cursor
    MoveCursorToObject(pBeforeSO);

    //insert at specified point
    Store(pNewSO);
}

void lmColStaffObjs::Store(lmStaffObj* pNewSO, bool fClefKeepPosition, bool fKeyKeepPitch)
{
    //This is the only method for including objects in the collection. All other methods (Add,
    //AddToCollection, Insert, etc.) end up invoking this method.

	//add pNewSO into the segment's collection.
	int nSegment = GetCursorSegment();
	m_Segments[nSegment]->Store(pNewSO, GetCursor());

	//if barline added manage segment split/creation
    //AWARE: inserting new elements into a vector can cause vector reallocation.
    //All currently in use segment iterators could be invalidated after SplitSegment()
    //or CreateNewSegment().
	lmStaffObj* pCursorSO = GetCursorStaffObj();
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

    //If inserted staffobj created contexts (clef, TS or KS) the created contexts are
    //already chained in the staves context chain. But it is necessary:
    //  - to update pointers to contexts at start of segment; and
    //  - to update staffobjs in segment, if affected by the context change
    //
    //But for time signatures, all needed context propagation and segments
    //update is done when doing the Re-Bar operation. So no need to do anything here.
    if (pNewSO->IsKeySignature())
        m_Segments[nSegment]->OnContextInserted(pNewSO, pCursorSO, fKeyKeepPitch);
    else if (pNewSO->IsClef())
        m_Segments[nSegment]->OnContextInserted(pNewSO, pCursorSO, fClefKeepPosition);
}

void lmColStaffObjs::Delete(lmStaffObj* pSO, bool fClefKeepPosition, bool fKeyKeepPitch)
{
	// Delete the StaffObj pointed by pSO.
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
    MoveCursorToObject(pSO);
    MoveCursorRight(true);      //true: stop in all chord notes
    lmStaffObj* pCursorSO = GetCursorStaffObj();
    lmCursorState oVCState = GetCursor()->GetState();

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
        oVCState.IncrementTimepos(rTime);
    }

    //after removing the staffobj, VCursor iterator is pointing to next staffobj but
    //other VCursor variables (rTimePos, in particular) are no longer valid and should be
    //updated.
    pSegment->UpdateDuration();     //ensure it is updated
    GetCursor()->SetState(&oVCState, true);  //true->update timepos with pSO timepos

    //finally, invoke destructor for removed staffobj
    delete pSO;


    //wxLogMessage(_T("[lmColStaffObjs::Delete] Forcing a dump:");
    //wxLogMessage( Dump() );
    //#if defined(_LM_DEBUG_)
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

lmStaffObj* lmColStaffObjs::FindFwdStaffObj(lmStaffObj* pCurSO, lmEScoreObjType nType)
{
    //Find next staffobj of type nType, starting from pCurSO (including it).

    //define iterator from current StaffObj
    lmSOIterator it(this, pCurSO);
    while(!it.EndOfCollection())
    {
        lmStaffObj* pSO = it.GetCurrent();
        if (pSO->GetScoreObjType() == nType)
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
    return (lmTimeSignature*)FindFwdStaffObj(pCurSO, lm_eSO_TimeSignature);
}

lmKeySignature* lmColStaffObjs::FindFwdKeySignature(lmStaffObj* pCurSO)
{
    return (lmKeySignature*)FindFwdStaffObj(pCurSO, lm_eSO_KeySignature);
}

lmClef* lmColStaffObjs::FindFwdClef(lmStaffObj* pCurSO)
{
    return (lmClef*)FindFwdStaffObj(pCurSO, lm_eSO_Clef);
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

//#if defined(_LM_DEBUG_)
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

lmSOIterator* lmColStaffObjs::CreateIteratorFrom(lmScoreCursor* pCursor)
{
    //creates an iterator pointing to received cursor position

    lmSOIterator* pIter = new lmSOIterator(this, pCursor);
    return pIter;
}

//lmSOIterator* lmColStaffObjs::CreateIteratorFrom(lmStaffObj* pSO)
//{
//    //creates an iterator pointing to received staffobj
//
//    lmSOIterator* pIter = new lmSOIterator(this, pSO);
//    return pIter;
//}


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
    //    pCurSegment->Store(pSO, (lmScoreCursor*)NULL);       << NULL means add at the end

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
    //        pCurSegment->Store(pBar, (lmScoreCursor*)NULL);

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
    //        pCurSegment->Store(pSO, (lmScoreCursor*)NULL);
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
    //    pCurSegment->Store(pBar, (lmScoreCursor*)NULL);

    //    //propagate contexts and update segment pointers after last inserted segment
    //    //This is necessary so that segments can update pointers to start of segment
    //    //applicable contexts.
    //    for (int nStaff=1; nStaff <= lmMAX_STAFF; nStaff++)
    //    {
    //        wxLogMessage( m_pOwner->Dump() );
    //        //determine context for current staff at end of segment
    //        lmContext* pLastContext = pCurSegment->FindEndOfSegmentContext(nStaff);

    //        //inform next segment
    //        pCurSegment->OnContextChanged(pLastContext, nStaff);
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
        if (pSO->IsClef())
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
		    if (pSO->IsKeySignature() || pSO->IsTimeSignature())
            {
                for (int nStaff=1; nStaff <= m_nNumStaves; nStaff++)
                {
                    if (!fStaffDone[nStaff-1])
                    {
		                if (pSO->IsKeySignature())
                            pCT = ((lmKeySignature*)pSO)->GetContext(nStaff);
		                else //TimeSignature
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
    bool fToEndOfTime = false;
    float rNewTime;
    float rMaxTime = m_Segments[GetCursorSegment()]->GetDuration();


    if (IsEqualTime(-rTimeShift, lmTIME_SHIFT_START_END))
    {
        //move to start of segment
        rNewTime = 0.0f;
    }
    else if (IsEqualTime(rTimeShift, lmTIME_SHIFT_START_END))
    {
        //move to end of segment
        rNewTime = rMaxTime;
        fToEndOfTime = true;
    }
    else
    {
        rNewTime = GetCursorTimepos() + rTimeShift;

        //check that new time is in current measure boundaries
        if (IsLowerTime(rNewTime, 0.0f))
        {
            m_pOwner->SetError(_("Move backwards: out of measure"));
            fError = true;
            rNewTime = 0.0f;
        }
        else if (IsHigherTime(rNewTime, rMaxTime))
        {
            m_pOwner->SetError(_("Move forward: out of measure"));
            fError = true;
            rNewTime = rMaxTime;
            fToEndOfTime = true;
        }
    }

    //move cursor to required time
    MoveCursorToTime(rNewTime, fToEndOfTime);

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
		if (pSO->GetStaffNum() == nStaff && pSO->IsNote())
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
		if (pSO->IsClef() && nStaff == pSO->GetStaffNum())
            pCT = ((lmClef*)pSO)->GetContext();
		else if (pSO->IsKeySignature())
            pCT = ((lmKeySignature*)pSO)->GetContext(nStaff);
		else if (pSO->IsTimeSignature())
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
		if (pSO->IsNote())
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

lmScoreCursor* lmColStaffObjs::GetCursor()
{
    //get cursor from the score
    lmScoreCursor* pCursor = m_pOwner->GetScore()->GetCursor();

    //verify that it is pointing to this collection
    int nInstr = pCursor->GetCursorInstrumentNumber();
    wxASSERT(m_pOwner->GetScore()->GetInstrument(nInstr) == GetOwnerVStaff()->GetOwnerInstrument());

    return pCursor;
}
