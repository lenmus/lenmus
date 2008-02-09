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
}

void lmVStaffCursor::MoveToTime(float rNewTime)
{
    //move cursor to first object with time > rNewTime

    m_it = m_pSegment->m_StaffObjs.begin();
    m_rTimepos = rNewTime;

	if (m_it == m_pSegment->m_StaffObjs.end()) return;      //the segment is empty

	while (m_it != m_pSegment->m_StaffObjs.end() && (*m_it)->GetTimePos() < rNewTime)
        ++m_it;

    if (m_it != m_pSegment->m_StaffObjs.end() && (*m_it)->GetTimePos() == rNewTime)
    {
        //time found. Advance to last object
	    while (m_it != m_pSegment->m_StaffObjs.end() && (*m_it)->GetTimePos() == rNewTime)
            ++m_it;
        //--m_it;
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

	//TODO: Compute duration of segment in a better way. Now we are assuming that last
    //staffobj determines the duration
    lmStaffObj* pSO = m_pSegment->m_StaffObjs.back();
    float rSegmentDuration = pSO->GetTimePos() + pSO->GetTimePosIncrement();

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
    //Cursor is positioned at end of global collection. So no need to advance iterator,
    //but just to advance timepos.
    //Advances cursor to time 0 in next segment

    wxASSERT(m_it == m_pSegment->m_StaffObjs.end());

    m_nSegment++;
	m_pSegment = m_pColStaffObjs->m_Segments[m_nSegment];
	m_it = m_pSegment->m_StaffObjs.begin();
	m_rTimepos = 0.0f; 
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

 //   for(int i=0; i < lmMAX_STAFF; i++)
 //       m_nCurVoice[i] = 0;

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
    if (pCursorSO)	// && !pNewSO->IsBarline())
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
    //shift timepos af all objects in this voice starting in SO pointed by itStart.

	//TODO

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
    m_nNumVoices = 0;
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
    //TODO: ASSERT that collection is empty
    m_nNumStaves++;
}

void lmColStaffObjs::AssignVoice(lmStaffObj* pSO)
{
    //assigns voice (1..n) to the staffobj

    // if it is not a note/rest, return: only note/rests have voice
    if (!pSO->IsNoteRest()) return;

    //if voice already assigned, return: nothing to do
    if (((lmNoteRest*)pSO)->GetVoice() != 0) return;

    // we have to assign it a voice. Let's determine if there are note/rests 
    // in timepos range [timepos, timepos+object_duration)
    bool fOccupied = IsTimePosOccupied(pSO->GetSegment(),
                                       pSO->GetTimePos(), pSO->GetTimePosIncrement() );

    // if this timepos range is occupied by a note/rest start a new voice,
    // else use default staff voice.
    if (fOccupied)
        ((lmNoteRest*)pSO)->SetVoice( ++m_nNumVoices );
    else
        ((lmNoteRest*)pSO)->SetVoice( m_nCurVoice[ pSO->GetStaffNum() - 1 ] );
}

bool lmColStaffObjs::IsTimePosOccupied(lmSegment* pSegment, float rTime, float rDuration)
{
    //returns true if there is any note/rest sounding in semi-open interval
    //[timepos, timepos+duration), in current measure.
    
	lmItCSO it = pSegment->m_StaffObjs.begin();

    //Is time occupied if
    //  coincides:      pSO(timepos) == rTime or
    //  start before:   pSO(timepos) < rTime && pSO(timepos+duration) > rTime
    //  start after:    pSO(timepos) > rTime && pSO(timepos) < (rTime+rDuration)
    while(it != pSegment->m_StaffObjs.end()) 
    {
        if ((*it)->IsNoteRest())
        {
            float rTimeSO = (*it)->GetTimePos();
            if (rTimeSO == rTime) return true;      //start coincides

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

	AssignTime(pNewSO);			//assign timepos to the StaffObj
	AssignVoice(pNewSO);		//assign it a voice

	//get segment and add pNewSO into the segment's collection
	int nSegment = m_pVCursor->GetSegment();
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

//void lmColStaffObjs::Insert(lmStaffObj* pNewSO, lmStaffObj* pBeforeSO)
//{
//    //Stores the new StaffObj (pNewSO) in this collection, inserting it BEFORE the 
//	//StaffObj pBeforeSO. Insertion DOES NOT move objects to new mesures, so this
//	//operation could create irregular measures
//
//	//locate insertion point
//	lmItMeasure itBeforeM = pBeforeSO->GetItMeasure();
//	lmSegmentData* pMData = *itBeforeM;
//	lmItCSO itFromCSO = pMData->itStartSO;
//	lmItCSO itToCSO = pMData->itEndSO; itToCSO++;
//	lmItCSO itBeforeCSO = std::find(itFromCSO, itToCSO, pBeforeSO);
//
//	//initialize time counters:
//	float rTime = pBeforeSO->GetTimePos();
//	lmItCSO itEndCSO = pMData->itEndSO;
//	lmStaffObj* pEndSO = *itEndCSO;
//	float rMaxTime = (*(pMData->itEndSO))->GetTimePos();
//	int nNumMeasure = pMData->nNumMeasure;
//	
//    //assign the time position of StattObj pointed by itBeforeCSO
//    //assign time to the StaffObj and increment time counters
//	AssignTime(pNewSO, &rTime, &rMaxTime);
//    
//    //insert the lmStaffObj in the collection, before the specified object
//    m_cStaffobjs.insert(itBeforeCSO, pNewSO);	//insert before item pointed by itBeforeCSO
//    lmItCSO itNewCSO = itBeforeCSO;				//iterator to point to the new inserted object
//	itNewCSO--;								
//
//	//if this StaffObj is a barline it is necessary to update the end of current measure
//	//and to create a new one before this one
//    if (pNewSO->GetClass() == eSFOT_Barline)
//    {
//		//InsertMeasure(itBeforeM, pMData->itStartSO, itNewCSO);
//		m_Segments.InsertSegment(itBeforeM, pMData->itStartSO, itNewCSO);
//		pMData->itStartSO = itBeforeCSO;
//    }
//	else
//	{
//		//if the insertion point StaffObj is the first one of a measure, we have to update
//		//the pointer in the measuresData table, unless the new inserted one is a barline
//		lmItCSO itOldStartCSO = pMData->itStartSO;
//		if (*itOldStartCSO == pBeforeSO) 
//			pMData->itStartSO = itNewCSO;
//	}
//    
//    //store, inside the lmStaffObj, a ptr to the measure data
//	pNewSO->SetItMeasure( m_Segments[nNumMeasure-1]);
//
//	////Debug: force a dump of StaffObjs collection and measures tables
//	//lmItMeasure itMNew = pNewSO->GetItMeasure();
//	//lmSegmentData* pMDataNew = *itMNew;
//	//itFromCSO = pMDataNew->itStartSO;
//	//itToCSO = pMDataNew->itEndSO;
//	//nNumMeasure = pMDataNew->nNumMeasure;
//	//wxLogMessage(_T("[lmColStaffObjs::Insert] (Before) Inserted pSO ID=%d, Measure %d, Start ID=%d, End ID=%d"),
//	//	pNewSO->GetID(), nNumMeasure, (*itFromCSO)->GetID(),  (*itToCSO)->GetID() );
//
//	RepositionObjects(itBeforeCSO, &rTime, &rMaxTime);
//
//	////Debug: force a dump of StaffObjs collection and measures tables
//	//itMNew = pNewSO->GetItMeasure();
//	//pMDataNew = *itMNew;
//	//itFromCSO = pMDataNew->itStartSO;
//	//itToCSO = pMDataNew->itEndSO;
//	//nNumMeasure = pMDataNew->nNumMeasure;
//	//wxLogMessage(_T("[lmColStaffObjs::Insert] (After) Inserted pSO ID=%d, Measure %d, Start ID=%d, End ID=%d"),
//	//	pNewSO->GetID(), nNumMeasure, (*itFromCSO)->GetID(),  (*itToCSO)->GetID() );
// //   #if defined(__WXDEBUG__)
// //   wxString sDump = _T("");
//	//sDump += DumpStaffObjs();
//	//sDump += DumpSegmentsData();
//	//sDump += DumpMeasures();
// //   g_pLogger->LogTrace(_T("lmColStaffObjs::Insert"), sDump );
// //   #endif
//
//}

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


//====================================================================================================
// Debug methods
//====================================================================================================

//wxString lmColStaffObjs::DumpStaffObjs()
//{
//	return DumpVoices();
//
// //   wxString sDump = _T("\nStaffObjs collection:\n");
//	//lmItCSO itSO;
//	//for (itSO = m_cStaffobjs.begin(); itSO != m_cStaffobjs.end(); itSO++)
//	//{
//	//	sDump += (*itSO)->Dump();
//	//}
//	//return sDump;
//}
//
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
	//first segment is [0, rTime], and second segment [++rTime, end]
	//Segments collection is renumbered

	//create empty segment after this one
	AddSegment(nSegment);

	//move to it all staffobjs with time > rTime
	//TODO: as we are dealing with lists this is just to cut first list and
	//		append the cutted part to the new list

	//Re-assign time to all objects in new segment
    //TODO: As ordering will not change this is just udating all staffobjs timepos, by
	// substracting rTime

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
	//TODO: As the segment is being added  at the end it is faster to take the last
	//context. For this, it is necessary to have access to the owner VStaff
    //for (int nStaff=1; nStaff < m_nNumStaves; nStaff++)
    //{
    //    lmItMeasure itMeasure = m_aMeasures.back();
    //    lmStaff* pStaff = pVStaff->GetStaff(nStaff);
    //    (*itMeasure)->pContext[nStaff-1] = pStaff->GetLastContext();
    //}
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
//wxString lmSegmentsTable::DumpSegmentsData()
//{
//    wxString sDump = _T("\nMeasures Data:\n");
//	int i=1;
//	lmItMeasure itM;
//	for (itM = m_aSegmentsData.begin(); itM != m_aSegmentsData.end(); itM++, i++)
//	{
//		sDump += wxString::Format(_T("%d: numMeasure=%d\n"),
//								  i, (*itM)->nNumMeasure );
//        sDump += wxString::Format(_T("\tStarts with object Id %d\n"),
//                                  (*((*itM)->itStartSO))->GetID() );
//        sDump += wxString::Format(_T("\tEnds with object Id %d\n"),
//                                  (*((*itM)->itEndSO))->GetID() );
//    }
//	return sDump;
//}
//
//wxString lmSegmentsTable::DumpMeasures()
//{
//	wxString sDump = _T("\nChecking Measures:\n");
//	bool fOK = true;
//	for (int i=0; i < (int)m_aMeasures.size(); i++)
//	{
//		lmItMeasure itM = m_aMeasures[i];
//		int nNumM = (*itM)->nNumMeasure;
//		if (nNumM != i+1)
//		{
//			sDump += wxString::Format(_T("\tError: Item %d points to measure %d\n"),
//								      i, nNumM);
//			fOK = false;
//		}
//		else
//			sDump += wxString::Format(_T("\tItem %d points to measure %d\n"),
//										i, nNumM);
//
//	}
//
//	if (fOK)
//		sDump += _T("\tNo errors found.");
//
//	return sDump;
//}
//
//void lmSegmentsTable::SetContext(int nMeasure, int nStaff, lmContext* pContext) 
//{ 
//    //store the context applicable to measure nMeasure (1..n) and staff nStaff (1..m)
//
//    wxASSERT(nMeasure > 0 && nMeasure <= (int)m_aMeasures.size());
//    lmItMeasure itMeasure = m_aMeasures[nMeasure-1];
//	(*itMeasure)->pContext[nStaff-1] = pContext; 
//}
//
//lmContext* lmSegmentsTable::GetContext(int nMeasure, int nStaff) 
//{
//    //return the context applicable to measure nMeasure (1..n) and staff nStaff (1..m)
//
//    wxASSERT(nMeasure > 0 && nMeasure <= (int)m_aMeasures.size());
//    lmItMeasure itMeasure = m_aMeasures[nMeasure-1];
//	return (*itMeasure)->pContext[nStaff-1]; 
//}

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

    //update time counters and check that shift is inside current measure
    float rNewTime = m_pVCursor->GetTimepos() + rTimeShift;
    if (rNewTime < 0.0f) 
    {
        m_pOwner->SetError(_("Move backwards: out of measure"));
        m_pVCursor->MoveToTime(0.0f);
        return true;      //can not jump back before the start of current measure
    }

    //TODO Check that does not go to the next measure
    //TODO Display error message if jump out of current measure boundaries
    if (rTimeShift > 0.0f)
    {
        //if (rNewTime > m_rMaxTime[0]) { m_rTime[0] = m_rMaxTime[0]   //can not jump out of this bar
    }

    //move cursor to required time
    m_pVCursor->MoveToTime(rNewTime);
    return false;
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

