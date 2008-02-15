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

#include "Score.h"
#include "wx/debug.h"
#include "StaffObjIterator.h"

//cursor positioning requires access to shapes
#include "../graphic/BoxSystem.h"
#include "../graphic/ShapeStaff.h"

//access to logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;

//binary function predicate. I is used to sort the staffobjs list by time
//returns whether a staffobj SO1 has lower timepos than another staffobj SO2
bool TimeCompare(lmStaffObj* pSO1, lmStaffObj* pSO2)
{
    return pSO1->GetTimePos() < pSO2->GetTimePos();
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
	m_pColStaffObjs = NULL;
}

void lmVStaffCursor::AttachToCollection(lmColStaffObjs* pColStaffObjs)
{
	m_pColStaffObjs = pColStaffObjs;
	m_pColStaffObjs->SetCursor(this);		//link back

	ResetCursor();
}

void lmVStaffCursor::MoveRight()
{
	if (IsAtEnd()) return;

	++m_it;
	if (m_it == m_pSegment->m_StaffObjs.end())
	{
		//advance to next segment
		if (++m_nSegment < (int)m_pColStaffObjs->m_Segments.size())
		{
			m_pSegment = m_pColStaffObjs->m_Segments[m_nSegment];
			m_it = m_pSegment->m_StaffObjs.begin();
		}
		//else nothing to do. End of collection reached
	}
    UpdateTimepos();
}

void lmVStaffCursor::MoveLeft()
{
	if (IsAtBeginning()) return;

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
    UpdateTimepos();
}

void lmVStaffCursor::MoveToTime(float rNewTime)
{
    //move cursor to first object with time > rNewTime

    m_it = m_pSegment->m_StaffObjs.begin();
    m_rTimepos = rNewTime;

	if (m_it == m_pSegment->m_StaffObjs.end()) return;      //the segment is empty

	while (m_it != m_pSegment->m_StaffObjs.end() && IsLowerTime((*m_it)->GetTimePos(), rNewTime))
        ++m_it;

    if (m_it != m_pSegment->m_StaffObjs.end() && IsEqualTime((*m_it)->GetTimePos(), rNewTime))
    {
        //time found. Advance to last object
	    while (m_it != m_pSegment->m_StaffObjs.end() && IsEqualTime((*m_it)->GetTimePos(), rNewTime))
            ++m_it;
    }
    else
    {
        //time does not exist. Cursor is now at first object > rNewTime
        ;
    }
    //UpdateTimepos();
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
	m_nStaff = 1;
	m_rTimepos = 0.0f;
	wxASSERT(m_pColStaffObjs->m_Segments.size() > 0);

	//position in first segment
	m_nSegment = 0;
	m_pSegment = m_pColStaffObjs->m_Segments[0];
	m_it = m_pSegment->m_StaffObjs.begin();
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
    //Cursor is positioned at end of global collection. So no need to advance iterator,
    //but just to advance timepos.
    //Advances cursor to time rTime in current segment. If required time is greater
    //than segment duration, moves to next segment, at the right time.

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

lmStaff* lmVStaffCursor::GetCursorStaff()
{
    //return pointer to staff in which cursor is located
    return m_pColStaffObjs->m_pOwner->GetStaff(m_nStaff);
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
        lmShape* pShape2 = pCursorSO->GetShap2();
        if (pShape2)
        {
	        lmBoxSystem* pSystem = pShape2->GetOwnerSystem();
            uPos.y = pSystem->GetStaffShape(pCursorSO->GetStaffNum())->GetYTop();
            uX2 = pShape2->GetXLeft() + pShape2->GetWidth()/2.0f;
        }
        else
        {
            //End of score
            //Or no shape! --> //TODO: All objects must have a shape, althought it can be not visible
            uPos.y = 0.0f;  //TODO
            uX2 = 0.0f;     //TODO
        }
    }

    //get info from previous staffobj
    lmStaffObj* pPrevSO = (lmStaffObj*)NULL;
    if (!IsAtBeginning())
    {
        MoveLeft();                 //move back
        pPrevSO = GetStaffObj();    //take staffobj
        MoveRight();                //advance to cursor staffobj (to cross possible segment boundary)
        m_rTimepos = rT3;           //and restore time position
    }

    if (pPrevSO)
    {
        rT1 = pPrevSO->GetTimePos();
        lmShape* pShape1 = pPrevSO->GetShap2();
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
        //Between two staffobjs, or on the sencond one
        //decide on positioning, based on cursor time
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
    }

    if (pCursorSO)
    {
        //No prev staffobj. It is the first one and so, cursor must be on it
        if (IsEqualTime(m_rTimepos, pCursorSO->GetTimePos()))
        {
            //Pointing to cursor staffobj. Take positioning information from staffobj
		    uPos.x = uX2;
        }
        else
            //can not be before the first staffobj
		    uPos.x = uX2;
            //wxASSERT(false);
    }

    else
    {
        //Score empty. No current staffobj and no previous staffobj
        //Take positioning information from staff and timepos
        //TODO
    }

    return uPos;
}

void lmVStaffCursor::UpdateTimepos()
{ 
    if (m_it != m_pSegment->m_StaffObjs.end())
        m_rTimepos = (*m_it)->GetTimePos();
}


//====================================================================================================
// lmSegment implementation
//====================================================================================================

lmSegment::lmSegment(lmVStaff* pOwner, int nSegment)
{
    m_nNumSegment = nSegment;
	m_pBarline = (lmBarline*)NULL;
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

    //update segment's duration
    float rTime = pNewSO->GetTimePos() + pNewSO->GetTimePosIncrement();
    m_rMaxTime = wxMax(m_rMaxTime, rTime); 

    //shift timepos af all objects in this voice after the inserted one
	if (pCursorSO && pNewSO->IsNoteRest())
	{
        ShiftTimepos(pCursor->GetCurIt(), ((lmNoteRest*)pNewSO)->GetVoice());
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
	////DBG  ------------------------------------------------------------------
	//wxLogMessage(_T("[lmSegment::Store] After insertion of %s"),
	//			pNewSO->GetName());
	//wxLogMessage(Dump());
	////END DBG ---------------------------------------------------------------
}

void lmSegment::ShiftTimepos(lmItCSO itStart, int nVoice)
{
    //An object has been iserted at timepos ?. If it is a note/rest and this timepos is 
    //ocuppied by other note/rest in the same voice, we must shift the timepos af all 
    //consecutive objects in this voice, starting in SO pointed by itStart.

    //Return maximum timepos+duration reached by last shifted object

	//TODO

    //Parameters: pStartSO (inserted object)

    //    1 Asign: TimeIncr = Duration(pStartSO)
    //            iV = Voice(pStartSO)
    //            pCurrentSO = take next object in voice iV after pStartSO
    //             
    //    2 While not end of measure
    //        - Assign: t = Timepos(pCurrentSO)
    //                d = Duration(pCurrentSO)
    //        - if Interval(t, t+d) is occupied by object in voice iV:
    //            - Assign: Timepos(pCurrentSO) += TimeIncr
    //                    pCurrentSO = take next object in voice iV after pCurrentSO
    //        else
    //            - break
            
    ////update segment's duration
    //float rTime = pNewSO->GetTimePos() + pNewSO->GetTimePosIncrement();
    //m_rMaxTime = wxMax(m_rMaxTime, rTime); 

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

    // we have to assign it a voice. Let's determine if there are note/rests 
    // in timepos range [timepos, timepos+object_duration)

    // First, lets check if default staff voice is suitable
    int iV = pSO->GetStaffNum();
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
//====================================================================================================

void lmColStaffObjs::Add(lmStaffObj* pNewSO)
{
    //Assign the received StaffObj a voice and timepos, store it in the collection,
	//before object pointed by current cursor, and advance cursor after inserted object.
    //Manage all details to maintain segments.

	//get segment
	int nSegment = m_pVCursor->GetSegment();

    //assign timepos and voice to the StaffObj
	AssignTime(pNewSO);
	AssignVoice(pNewSO, nSegment);

	//add pNewSO into the segment's collection
	m_Segments[nSegment]->Store(pNewSO, m_pVCursor);

	//manage segments split/creation
	lmStaffObj* pCursorSO = m_pVCursor->GetStaffObj();
    if (pNewSO->IsBarline())
    {
        if (pCursorSO)
        {	//split current segment
            SplitSegment(nSegment++);
        }
        else
        {	//create a new segment
            if (((lmBarline*)pNewSO)->GetBarlineType() != lm_eBarlineEOS)
                CreateNewSegment(++nSegment);
        }
    }
		
    //advance cursor
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
        //Cursor is pointing to object after the inserted one but could be of different
        //voice. And cursor timepos is not updated as the collection has been altered.
        //So we have to reposition cursor at right time and right object.
        m_pVCursor->MoveToTime( pNewSO->GetTimePos() + pNewSO->GetTimePosIncrement() );
    }

}

void lmColStaffObjs::Delete(lmStaffObj* pCursorSO)
{
	//Delete the StaffObj pointed by pCursorSO

	//TODO

//	//get some data
//	EStaffObjType nDeletedObjType = pCursorSO->GetClass();
//	lmItMeasure itMCursor = pCursorSO->GetItMeasure();
//	lmSegmentData* pMDataCursor = *itMCursor;
//	lmItCSO itFromCSO = pMDataCursor->itStartSO;
//	lmItCSO itToCSO = pMDataCursor->itEndSO;
//	lmStaffObj* pBarlineSO = *itToCSO;			//barline for current measure
//	itToCSO++;
//	lmItCSO itCursorCSO = std::find(itFromCSO, itToCSO, pCursorSO);
//	lmItCSO itNextCSO = itCursorCSO; itNextCSO++;
//	lmStaffObj* pNextSO = *itNextCSO;
//	lmItMeasure itMNext = pNextSO->GetItMeasure();
//	int nMeasure = pMDataCursor->nNumMeasure;
//
//	//the EOS Barline can not be deleted
//	if (nDeletedObjType == eSFOT_Barline 
//		&& ((lmBarline*)pCursorSO)->GetBarlineType() == lm_eBarlineEOS) return;
//
//	// 2. initialize time counters:
//	//		current time = time assigned to CursorSO
//	//		max.time = time assigned to barline of current measure
//	float rTime = pCursorSO->GetTimePos();
//	float rMaxTime = pBarlineSO->GetTimePos();
//	
//	// 3. if object to delete is the first one of measure update MeasureData
//	if (*(pMDataCursor->itStartSO) == pCursorSO)
//		pMDataCursor->itStartSO = itNextCSO;
//
//	// 4. Remove the StaffObj CursorSO
//	m_cStaffobjs.erase(itCursorCSO);
//
//	// 5. if CursorSO is a barline we have to update the end of current measureData
//	if (nDeletedObjType == eSFOT_Barline)
//		pMDataCursor->itEndSO = (*itMNext)->itEndSO;
//
//	// 4. Update time & MeasureData ptr of all following StaffObjs (from NextSO to 
//	//	  first barline):
//	// 4.1 Asign: Cur_StaffObj = NextSO
//	// 4.2 While not end of collection
//	while (itNextCSO != m_cStaffobjs.end())
//	{
//		// 4.2.1. Assign time to it
//		AssignTime(*itNextCSO, &rTime, &rMaxTime);
//
//		// 4.2.2. Update StaffObj information: MeasureData = MeasureData[CursorSO]
//		(*itNextCSO)->SetItMeasure( itMCursor );
//
//		// 4.2.3 if Cur_StaffObj is a barline break loop
//		//		 else take next StaffObj
//		if ((*itNextCSO)->GetClass() == eSFOT_Barline) break;
//		itNextCSO++;
//	}
//
//	// If CursorSO is a barline we have to update measures:
//	if (nDeletedObjType == eSFOT_Barline)
//        m_Segments.RemoveSegment(itMNext, nMeasure);
//
// //   #if defined(__WXDEBUG__)
// //   wxString sDump = _T("");
//	//sDump += DumpStaffObjs();
//	//sDump += DumpSegmentsData();
//	//sDump += DumpMeasures();
// //   g_pLogger->LogTrace(_T("lmColStaffObjs::Delete"), sDump );
// //   #endif
}

//void lmColStaffObjs::RepositionObjects(lmItCSO itCSO, float* pTime, float* pMaxTime)
//{
//	//B6. Update timePos of all following StaffObjs (from pItem to first barline):
//
//	while (itCSO != m_cStaffobjs.end())
//	{
//		AssignTime(*itCSO, pTime, pMaxTime);
//		if ((*itCSO)->GetClass() == eSFOT_Barline) break;
//		itCSO++;
//	}
//}
//
//void lmColStaffObjs::ShiftObjects(lmItCSO itFirst, lmItCSO itLast, int nVoice, 
//                                         float rTimeShift)
//{
//    //Shift the timepos of all objects of voice nVoice in closed range [itFirst, itLast]
//
//    while (itFirst != m_cStaffobjs.end())
//	{
//		AssignTime(*itFirst, pTime, pMaxTime);
//		if (itFirst == itLast) break;
//		itFirst++;
//	}
//
//}


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


//====================================================================================================
// Debug methods
//====================================================================================================

//wxString lmColStaffObjs::DumpSegmentsData()
//{
//	return m_Segments.DumpSegmentsData();
//}
//
//wxString lmColStaffObjs::DumpMeasures()
//{
//	return m_Segments.DumpMeasures();
//}
//
//wxString lmColStaffObjs::DumpVoices()
//{
//	wxString sDump = wxString::Format(_T("\nThis instrument has %d voices:\n\n"), m_nNumVoiceLines);
//    for(int iV=0; iV < m_nNumVoiceLines; iV++)
//    {
//		sDump += wxString::Format(_T("\nVoices %d:\n"), iV+1);
//		sDump += m_pVoiceLine[iV]->Dump();
//	}
//	return sDump;
//}

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

void lmColStaffObjs::CreateNewSegment(int nSegment)
{
	//Add a new segment after segment number nSegment. If nSegment is the last segment,
	//the new created segment is added at the end; otherwise, the new segment is
	//inserted after segment nSegment and all the segments are renumbered.

	if ((int)m_Segments.size() == nSegment)
		AddSegment(nSegment);		//the new segment is added at the end
	else
		InsertSegment(nSegment);	//insert the new segment after segment nSegment
}

void lmColStaffObjs::SplitSegment(int nSegment)
{
	//Segment nSegment is splitted into two segments at timepos rTime, so that
	//first segment is formed by objects in timepos range [0, rTime], and second 
    //segment by objects in timepos range [++rTime, end]
    //Timepos of this second segment are re-adjusted to start in 0
	//Segments collection is renumbered

	//create empty segment after this one
	AddSegment(nSegment);

	//move to it all staffobjs with time > rTime
	//TODO: as we are dealing with lists this is just to cut first list and
	//		append the cutted part to the new list

	//Re-assign time to all objects in new segment.
    //As ordering will not change this is just udating all staffobjs timepos, by
	//substracting rTime
    //TODO: 

    //re-compute segment duration for both segments
    //TODO

	//Update contexts in new segment
	//TODO
}

void lmColStaffObjs::AddSegment(int nSegment)
{
	//Add a new segment at the end

	//create the segment
	lmSegment* pSegment = new lmSegment(m_pOwner, nSegment);
	m_Segments.push_back(pSegment);

    //Set contexts at start of this new segment.
    UpdateContexts(pSegment);
}

void lmColStaffObjs::InsertSegment(int nSegment)
{
	//Inserts a new segment after segment nSegment, and all the segments are renumbered.

	//create the segment
	lmSegment* pS = new lmSegment(m_pOwner, nSegment);
	m_Segments.push_back(pS);

//	m_aSegmentsData.insert(itMBefore, pData);
//
//	//update measures table:
//	//1) make room for a new measure by inserting a new element at the end
//	m_aMeasures.push_back( m_aSegmentsData.end() );
//	//2) move data from element i to element i+1
//	for (int i=(int)m_aMeasures.size()-2; i >= nNumMeasure-1; i--)
//		m_aMeasures[i+1] = m_aMeasures[i];
//	//3)update the inserted element
//	m_aMeasures[nNumMeasure-1] = --itMBefore;		//new inserted item
//
//	//update measure numbers in m_aSegmentsData
//	for (lmItMeasure itM = ++itMBefore; itM != m_aSegmentsData.end(); itM++)
//	{
//		(*itM)->nNumMeasure++;
//    }
//
//    //set contexts 
//    UpdateContexts(nNumMeasure);

}

//void lmSegmentsTable::RemoveSegment(lmItMeasure itMNext, int nMeasure)
//{
//	// 6. From current MeasureData (NextSO) to end of MeasureData collection
//	//    MeasureData[i].numMeasure --
//	lmItMeasure itM = itMNext; itM++;
//	while (itM != m_aSegmentsData.end())
//	{
//		(*itM)->nNumMeasure--;
//		itM++;
//	}
//
//	// 7. Delete MeasureData[NextSO]
//	m_aSegmentsData.erase( itMNext );
//
//	// 8. From Measure nMeasure+1 to end of vector aMeasures - 1:
//	//	  element[i] = element[i+1]
//	for (int i=nMeasure; i < (int)m_aMeasures.size()-1; i++)
//		m_aMeasures[i] = m_aMeasures[i+1];
//
//	// 9. Remove last element of aMeasures
//	m_aMeasures.pop_back();
//}
//

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
    if (rNewTime < 0.0f) 
    {
        //Move backwards: out of measure
        if (rTimeShift != lmTIME_SHIFT_START_END)
        {
            m_pOwner->SetError(_("Move backwards: out of measure"));
            fError = true;
        }
        rNewTime = 0.0f;
    }
    else if (rTimeShift > rMaxTime)
    {
        //Move forward: out of measure
        if (rTimeShift != lmTIME_SHIFT_START_END)
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

