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

// Encapsulation of the table and of management algoritms to compute the positioning
// data for each lmStaffObj, when a bar column must be rendered.

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "TimeposTable.h"
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
#include <math.h>

#include "../score/Score.h"
#include "wx/debug.h"
#include "TimeposTable.h"

//helper functions to compare two floating point numbers
bool IsEqualTime(float t1, float t2)
{
    return (fabs(t1 - t2) < 0.1);
}

////spacing function parameters
//double              m_rSpacingFactor;           //for proportional spacing of notes
//lmESpacingMethod    m_nSpacingMethod;           //fixed, proportional, etc.
//lmTenths            m_nSpacingValue;            //spacing for 'fixed' method
float				m_rDmin = 8.0f;				//Dmin: min. duration to consider
lmTenths			m_rMinSpace = 15.0f;		//Space(Dmin):  space for Dmin

//TODO: User options
lmTenths m_rSpaceAfterStartOfMeasure = 7.5f;
lmTenths m_rSpaceAfterProlog = 25.0f;

//=====================================================================================
//lmTimeLine
//=====================================================================================

lmTimeLine::lmTimeLine(lmTimeposTable* pMngr, int nInstr, int nVoice)
{
	m_pOwner = pMngr;
	m_nInstr = nInstr;
	m_nVoice = nVoice;
    NewEntry(eAlfa, -1, (lmStaffObj*)NULL);
}

lmTimeLine::~lmTimeLine()
{
    for (lmItEntries it = m_aMainTable.begin(); it != m_aMainTable.end(); it++) 
	{
		delete *it;
	}
	m_aMainTable.clear();
}

lmTimeposEntry* lmTimeLine::AddEntry(eTimeposEntryType nType, float rTimePos, lmStaffObj* pSO)
{
    return NewEntry(nType, rTimePos, pSO);
}

lmTimeposEntry* lmTimeLine::NewEntry(eTimeposEntryType nType, float rTimePos, lmStaffObj* pSO)
{
    lmTimeposEntry* pEntry = new lmTimeposEntry(nType, pSO, rTimePos);
    m_aMainTable.push_back(pEntry);
	return pEntry;
}

void lmTimeLine::SetCurXLeft(lmLUnits uValue)
{
    //Initialices m_xLeft and m_xInitialLeft.
    //If no anchor yet specified, initialices it with xLeft
    //If it is a StaffObj also initialices its size

    lmTimeposEntry* pEntry = m_aMainTable.back();
    pEntry->m_xLeft = uValue;
    pEntry->m_xInitialLeft = uValue;

    if (pEntry->m_uxAnchor == 0) {
        pEntry->m_uxAnchor = uValue;   //default value
    }

    if (pEntry->m_nType == eStaffobj) {
        pEntry->m_uSize = pEntry->m_xFinal - pEntry->m_xLeft;
    }

}

lmLUnits lmTimeLine::ShiftEntries(lmLUnits uNewBarSize, lmLUnits uNewStart)
{
    lmLUnits uBarPosition = 0;
    lmTimeposEntry* pTPE = m_aMainTable.front();
    lmLUnits uShift = uNewStart - pTPE->m_xLeft;

	wxLogMessage(_T("[lmTimeLine::ShiftEntries] Reposition: uNewBarSize=%.2f  uNewStart=%.2f  Shift=%.2f"),
				 uNewBarSize, uNewStart, uShift );

	//Update table and store the new x positions into the StaffObjs
    for (lmItEntries it = m_aMainTable.begin(); it != m_aMainTable.end(); it++) 
	{
        pTPE = *it;
        if (uShift != 0.0f && pTPE->m_nType == eStaffobj)
        {
			pTPE->m_pSO->StoreOriginAndShiftShapes(uShift);
        }
        else if (pTPE->m_nType == eOmega)
        {
            //there might be no barline.
            if (pTPE->m_pSO)
            {
                lmLUnits uShiftBar = uNewStart + uNewBarSize - pTPE->m_xFinal + 1;
				pTPE->m_pSO->StoreOriginAndShiftShapes(uShiftBar);
                uBarPosition = pTPE->m_xLeft + uShiftBar;
				wxLogMessage(_T("[lmTimeLine::ShiftEntries] Reposition bar: uShiftBar=%.2f  uBarPosition=%.2f"),
							uShiftBar, uBarPosition );
            }
        }
    }
    return uBarPosition;

}

float lmTimeLine::SetTimedObjects(float rTime, lmLUnits uxPos, float rFactor)
{
	//Starting at current position, explores the line.
	//Assings space and sets positions of all time positioned objects
	//placed at time rTime, until we reach a time greater that rTime. The position
	//for this next rTime object is also set tentatively.
	//If during this traversal we find not timed objects they are ignored and when
	//reached next rTime we go back assigning positions to them
	//Returns the maximum xPos reached and updates parameter pNextTime to leave there
	//the next entry time, or -1 if no more entries or next entry is not timed.

	//When entering this method, iterator must point to a timed object. If it is
	//placed at time rTime, process it and all the following having the same timepos,
	//until we find the next timepos or we arrive to end of line.
	//Not-timed objects are skipped
	lmItEntries itLastNotTimed = m_aMainTable.end();
	while (m_it != m_aMainTable.end() && 
			(IsEqualTime((*m_it)->m_rTimePos, rTime) || (*m_it)->m_rTimePos < 0.0f) )
    {
		//if not-timed take note and continue
		if ((*m_it)->m_rTimePos == -1.0f)
			itLastNotTimed = m_it;

        //else if contains a StaffObj (in the omega entry there might be no barline)
		//assign final position
        else if ((*m_it)->m_pSO)
        {
			//assign spacing
			if ((*m_it)->m_pSO->GetClass() == eSFOT_NoteRest)
				(*m_it)->m_uSpace = SetNoteRestSpace(rFactor, m_rDmin,
											(lmNoteRest*)((*m_it)->m_pSO),
											(*m_it)->m_uSize );

			//move the shape and update the entry data
			(*m_it)->Reposition(uxPos);

			//compute new current position
			lmLUnits uxFinal = uxPos + (*m_it)->m_uSpace;
			m_uxCurPos = wxMax(uxFinal, m_uxCurPos);
        }
        m_it++;
    }

	//Here we have arrived to the next timepos or to the end of line.
	if (m_it != m_aMainTable.end())
	{
		//next timepos. Assign position tentatively
		(*m_it)->m_xLeft = m_uxCurPos;

		//go back to assign positions to not-timed objects
		if (itLastNotTimed != m_aMainTable.end())
		{
			//TODO
		}
	}

	//done. Return the next entry time, or -1 if no more entries.
	if (m_it != m_aMainTable.end()) {
        wxLogMessage(_T("Timed. More entries. Next at time=%.2f ID=%d"),
					 (*m_it)->m_rTimePos, (*m_it)->m_pSO->GetID() );
        return (*m_it)->m_rTimePos;
    }
    else {
        wxLogMessage(_T("Timed. No more entries"));
        return -1.0f;
    }

}

lmLUnits lmTimeLine::GetLineWidth()
{
	//Return the size of the measure represented by this line or 0 if
	//no omega entry.

	if (m_aMainTable.back()->m_nType == eOmega)
		return m_aMainTable.back()->m_xLeft
			   + m_aMainTable.back()->m_uSpace - m_aMainTable.front()->m_xLeft;
	else
		return 0.0f;
}

lmLUnits lmTimeLine::GetPosForTime(float rTime)
{
	//When enetring this method iterator m_it points to the next timed entry or
	//to end iterator.
	//If next entry time is rTime, returns its xLeft position. Else returns 0

	if (m_it != m_aMainTable.end() && IsEqualTime((*m_it)->m_rTimePos, rTime) )
		return (*m_it)->m_xLeft;
	else
		return 0.0f;
}

lmLUnits lmTimeLine::IntitializeSpacingAlgorithm()
{
	//Initialize iterator and current position
	m_it = m_aMainTable.begin();
	m_uxCurPos = (*m_it)->m_xLeft + (*m_it)->m_uSpace;
	m_it++;

	//iterate until we reach a timed object or end of line
	//This loop will process non-note/rest objects (clefs, key sign, etc.)
	bool fThereAreObjects = false;
	while (m_it != m_aMainTable.end() && (*m_it)->m_rTimePos < 0.0f)
    {
		fThereAreObjects = true;

		//Update spacing
		(*m_it)->m_uSpace = (*m_it)->m_uSize;

		//move the shape and update the entry data
        (*m_it)->Reposition(m_uxCurPos);

		//advance the width and spacing
		m_uxCurPos += (*m_it)->m_uSpace;

        m_it++;
    }

	//here m_it points to the first note/rest or to end of line
	//If there were not-timed objects before the firts note/rest, I am going to
	//add some additional space before the first note/rest
	if (fThereAreObjects)
	{
		m_uxCurPos += m_pOwner->TenthsToLogical(m_rSpaceAfterProlog, 1);
	}

	//Now we are going to tentatively set position of first note/rest
	if (m_it != m_aMainTable.end())
	{
		(*m_it)->m_xLeft = m_uxCurPos;
	}

	//Returns the maximum xPos reached
	//DBG. log message
	if (m_it != m_aMainTable.end())
        wxLogMessage(_T("Not timed. More entries. Next at time=%.2f ID=%d"), 
					 (*m_it)->m_rTimePos, (*m_it)->m_pSO->GetID() );
    else
        wxLogMessage(_T("Not timed. No more entries."));

    return m_uxCurPos;
}

lmLUnits lmTimeLine::SetNoteRestSpace(float rFactor, float rDmin, lmNoteRest* pSO,
									  lmLUnits uxWidth)
{
	static const float rLog2 = 0.3010299956640f;		// log(2)

    float rVar = rFactor * log(pSO->GetDuration() / rDmin) / rLog2;
	wxLogMessage(_T("[lmTimeLine::SetNoteRestSpace] duration=%s, rel.space=%.2f"),
			pSO->GetLDPNoteType(), (1.0f + rVar) );

	//spacing function
	lmLUnits uSpace = m_rMinSpace;		//Space(Di) = Space(Dmin)
	if (rVar > 0.0f) {
		rVar *= m_rMinSpace;
		uSpace += rVar;			//Space(Di) = Space(Dmin)*[1 + lod2(Di/Dmin)]
	}

	//convert to lmLUnits
	int iStaff = pSO->GetStaffNum();
	uSpace = m_pOwner->TenthsToLogical(uSpace, iStaff);

	//if space is lower than object width force space to be a little more than width
	if (uSpace < uxWidth)
		return uxWidth + m_pOwner->TenthsToLogical(50.0f, iStaff);
	else
		return uSpace;
}

wxString lmTimeLine::DumpMainTable()
{
    wxString sMsg = wxString::Format(_T("TimeLine table dump. Instr=%d, voice=%d \n"),
									 m_nInstr, m_nVoice );
    sMsg += _T("===================================================================\n\n");

    if (m_aMainTable.size() == 0)
    {
        sMsg += _T("The table is empty.");
        return sMsg;
    }

    //headers  
    //                                          ........'........'........¡........'........¡........¡
    sMsg += _T("Item    Type    ID       TimePos    xInit  xAnchor    xLeft     Size    Space   xFinal\n");

    //loop to dump table entries
    lmTimeposEntry* pTE;
    for (int i = 0; i < (int)m_aMainTable.size(); i++)
    {
        if (i % 4 == 0) {
            sMsg += wxT("----------------------------------------------------------------------------\n");
        }
        pTE = m_aMainTable[i];
        sMsg += wxString::Format(_T("%4d: "), i);
        switch (pTE->m_nType)
        {
            case eAlfa:
                sMsg += _T("  Alfa        ");
                break;

            case eOmega:
                sMsg += _T("  Omega");
                if (pTE->m_pSO) {
                    sMsg += wxString::Format(_T("%3d    "), pTE->m_pSO->GetClass() );
                } else {
                    sMsg += _T("  -           ");
                }
                break;

            default:
                //lmStaffObj entry
                sMsg += wxString::Format(_T("  pSO %4d %3d"),
										 pTE->m_pSO->GetClass(),
										 pTE->m_pSO->GetID() );
        }

        sMsg += wxString::Format(_T("%11.2f %8.2f %8.2f %8.2f %8.2f %8.2f %8.2f\n"),
            pTE->m_rTimePos, pTE->m_xInitialLeft, pTE->m_uxAnchor, pTE->m_xLeft,
			pTE->m_uSize, pTE->m_uSpace, pTE->m_xFinal);
    }

    sMsg += _T("=== End of table ==================================================\n\n");
    return sMsg;

}

lmLUnits lmTimeLine::GetMaxXFinal()
{
    //returns the maximum x final of this line

    //now compute the maximum xFinal
    lmLUnits xFinal = 0;
    for (lmItEntries it = m_aMainTable.begin(); it != m_aMainTable.end(); it++) 
	{
		lmLUnits xPos = (*it)->m_xFinal;
        xFinal = wxMax(xPos, xFinal);
    }

    return xFinal;

}




//=====================================================================================
//lmTimeposTable
//=====================================================================================

lmTimeposTable::lmTimeposTable()
{
    for(int i=0; i < lmMAX_STAFF; i++)
        m_nCurVoice[i] = 0;
	m_pCurEntry = (lmTimeposEntry*)NULL;
}

lmTimeposTable::~lmTimeposTable()
{
	CleanTable();
}

void lmTimeposTable::CleanTable()
{
    //This method must be invoked before using the table. Can be also invoked as
    //many times as desired to clean the table and reuse it.

    for(int i=0; i < lmMAX_STAFF; i++)
        m_nCurVoice[i] = 0;

	for (lmItTimeLine it=m_aLines.begin(); it != m_aLines.end(); it++)
	{
		delete *it;
	}
	m_aLines.clear();

}

void lmTimeposTable::StartLines(int nInstr, lmVStaff* pVStaff)
{
	int nNumStaves = pVStaff->GetNumStaves();
    wxASSERT(nNumStaves < lmMAX_STAFF);

    for(int iS=0; iS < nNumStaves; iS++)
    {
        m_nCurVoice[iS] = iS+1;
		m_pStaff[iS] = pVStaff->GetStaff(iS+1);
        StartLine(nInstr, iS+1);
    }
}

void lmTimeposTable::StartLine(int nInstr, int nVoice)
{
    //Start a new line for instrument nInstr (0..n-1)
	//If this is the first line for instrument nInstr, all non-voiced StaffObj
	//will be assigned to this line, as well as the first voice encountered

	lmTimeLine* pLine = new lmTimeLine(this, nInstr, nVoice);
	m_aLines.push_back(pLine);
	m_pCurEntry = pLine->m_aMainTable.back(); 
	m_itCurLine = m_aLines.end();
	m_itCurLine--;

	//add spacing after start of measure
	SetSpace( TenthsToLogical(m_rSpaceAfterStartOfMeasure, 1) );

	wxLogMessage(_T("[lmTimeposTable::StartLine] New line started for instr=%d, voice=%d"),
        nInstr, nVoice);
}

void lmTimeposTable::CloseLine(lmStaffObj* pSO)
{
	//close current line.

	if (pSO)
		m_pCurEntry = (*m_itCurLine)->AddEntry(eOmega, pSO->GetTimePos(), pSO);
	else
		m_pCurEntry = (*m_itCurLine)->AddEntry(eOmega, -1, pSO);
}

lmTimeLine* lmTimeposTable::FindLine(int nInstr, int nVoice)
{
	//Find the line for this nInstr/nVoice

	//find instrument
	for (lmItTimeLine it=m_aLines.begin(); it != m_aLines.end(); it++)
	{
		if ((*it)->m_nInstr == nInstr
			&& ((*it)->m_nVoice == 0 || (*it)->m_nVoice == nVoice ))
		{
			return *it;
		}
	}
	//instrument not found
	return (lmTimeLine*) NULL;
}

void lmTimeposTable::AddEntry(int nInstr, int nVoice, float rTimePos, lmStaffObj* pSO)
{
	//Add an entry to current line for this nInstr/nVoice
    wxLogMessage(_T("[lmTimeposTable::AddEntry] instr=%d, voice=%d, SO type = %d, staff=%d"),
        nInstr, nVoice, pSO->GetClass(), pSO->GetStaffNum() );

    //use default voice when not specified [mainly for clefs]
    if (nVoice == 0) 
    {
        nVoice = m_nCurVoice[ pSO->GetStaffNum() - 1 ];
    }

	//find line
	lmItTimeLine itInstr = m_aLines.end();
	for (lmItTimeLine it=m_aLines.begin(); it != m_aLines.end(); it++)
	{
		if ((*it)->m_nInstr == nInstr)
		{
			//first line for this instrument found, Save ptr to it
			if (itInstr == m_aLines.end()) itInstr = it;

			if ( (*it)->m_nVoice == 0 || (*it)->m_nVoice == nVoice )
			{	//voice found. Save current line and add entry
				m_itCurLine = it;
				m_pCurEntry = (*it)->AddEntry(eStaffobj, rTimePos, pSO);
                wxLogMessage(_T("\tEntry added to line for voice=%d"), (*it)->m_nVoice);

				//update voice
                if ((*it)->m_nVoice == 0 && nVoice != 0) {
					(*it)->m_nVoice = nVoice;
                    //m_nCurVoice = nVoice;
                }
				return;
			}
		}
	}
	//not found. Start new line
	wxASSERT(itInstr != m_aLines.end());	//a line for the instrument must exist
	StartLine(nInstr, nVoice);
	m_pCurEntry = (*m_itCurLine)->AddEntry(eStaffobj, rTimePos, pSO);
}

//-------------------------------------------------------------------------------------
// methods for accesing/updating current entry
//-------------------------------------------------------------------------------------

void lmTimeposTable::SetCurXLeft(lmLUnits uValue)
{
    //initialices m_xLeft and m_xInitialLeft.
    //If no anchor yet specified, initialices it with xLeft
    //If it is a staffObj also initialices its size

    m_pCurEntry->m_xLeft = uValue;
    m_pCurEntry->m_xInitialLeft = uValue;

    if (m_pCurEntry->m_uxAnchor == 0) {
        m_pCurEntry->m_uxAnchor = uValue;   //default value
    }

    if (m_pCurEntry->m_nType == eStaffobj) {
        m_pCurEntry->m_uSize = m_pCurEntry->m_xFinal - m_pCurEntry->m_xLeft;
    }
}

lmLUnits lmTimeposTable::GetCurXLeft()
{
    return m_pCurEntry->m_xLeft;
}

lmLUnits lmTimeposTable::GetCurPaperPosX(int nInstr, int nVoice)
{
	//return current paper position for specified line (instr+voice)
	// or start of measure x pos if no object stored yet for that line

	lmTimeLine* pLine = FindLine(nInstr, nVoice);
    if (pLine)
	{
		lmTimeposEntry* pEntry = pLine->m_aMainTable.back();
		return pEntry->m_xFinal;
    }
    lmTimeposEntry* pEntry = m_aLines[0]->m_aMainTable[0];		//first alpha entry
    return pEntry->m_xLeft;
}

void lmTimeposTable::SetCurXFinal(lmLUnits uValue)
{
    m_pCurEntry->m_xFinal = uValue;
    m_pCurEntry->m_uSize = m_pCurEntry->m_xFinal - m_pCurEntry->m_xLeft;
}

void lmTimeposTable::SetSpace(lmLUnits uValue)
{
    m_pCurEntry->m_uSpace = uValue;
}

void lmTimeposTable::SetCurXAnchor(lmLUnits uValue)
{
    m_pCurEntry->m_uxAnchor = uValue;
}

void lmTimeposTable::SetWidth(lmLUnits uValue)
{
    m_pCurEntry->m_uSize = uValue;
}



//-------------------------------------------------------------------------------------
//  methods to compute results
//-------------------------------------------------------------------------------------

lmLUnits lmTimeposTable::GetGrossBarSize()
{
    //returns the maximum measure size of all lines

    //compute the maximum xFinal
    lmLUnits xFinal = 0;
    for (lmItTimeLine it = m_aLines.begin(); it != m_aLines.end(); it++) 
	{
		lmLUnits xPos = (*it)->GetMaxXFinal();
        xFinal = wxMax(xFinal, xPos);
    }

    return xFinal - GetStartOfBarPosition();
}

lmLUnits lmTimeposTable::GetStartOfBarPosition()
{
    //returns the x position for the start of the bar column

    //the bar starts at the xLeft of the alfa entry
    lmTimeposEntry* pEntry = m_aLines[0]->m_aMainTable.front();    //the alfa entry
    wxASSERT(pEntry->m_nType == eAlfa);
    return pEntry->m_xLeft;
}

lmLUnits lmTimeposTable::DoSpacingFixed(lmTenths nSpacingValue, bool fTrace)
{
    //return ArrangeStaffobjsByTime(fTrace);
	return 10000;
}

lmLUnits lmTimeposTable::DoSpacingProportional(double nSpacingFactor, bool fTrace)
{

    wxLogMessage( DumpTimeposTable() );
    lmLUnits uSize = ComputeSpacing(0.45f);
    wxLogMessage( DumpTimeposTable() );
    return uSize;
}

lmLUnits lmTimeposTable::ComputeSpacing(float rFactor)
{
	//Initialize algorithm iterators and process any not timed 
	//objects before time rTime = 0.0
	lmLUnits uxPos = 0.0f;
    float rTime = 0.0f;
	for (lmItTimeLine it=m_aLines.begin(); it != m_aLines.end(); it++)
	{
		lmLUnits uxStartPos = (*it)->IntitializeSpacingAlgorithm();
		uxPos = wxMax(uxStartPos, uxPos); 
	}

	bool fContinue = true;
	while(fContinue)
    {
		//Determine minimum common x position for timepos rTime
		for (lmItTimeLine it=m_aLines.begin(); it != m_aLines.end(); it++)
		{
			lmLUnits uxMinPos = (*it)->GetPosForTime(rTime);
			uxPos = wxMax(uxPos, uxMinPos); 
		}
		wxLogMessage(_T("[lmTimeposTable::ComputeSpacing] Setting timed objects at time %.2f, xPos=%.2f"),
			rTime, uxPos);

		//Process all timed objects placed at time rTime
		fContinue = false;
		float rNextTime = 10000.0f;		//any too big value
		for (lmItTimeLine it=m_aLines.begin(); it != m_aLines.end(); it++)
		{
			float rNewTime = (*it)->SetTimedObjects(rTime, uxPos, rFactor);
			if (rNewTime > 0.0f)
			{
				fContinue = true;
				rNextTime = wxMin(rNextTime, rNewTime);
			}
		}
		//wxLogMessage( DumpTimeposTable() );

		//advance to next time
		rTime = rNextTime;
	}

	//Get measure size
	lmLUnits uMeasureSize = 0.0f;
	for (lmItTimeLine it=m_aLines.begin(); it != m_aLines.end(); it++)
	{
		lmLUnits uSize = (*it)->GetLineWidth();
		uMeasureSize = wxMax(uMeasureSize, uSize);
	}

	return uMeasureSize;
}


lmLUnits lmTimeposTable::RedistributeSpace(lmLUnits uNewBarSize, lmLUnits uNewStart)
{
    //Shift the position of all StaffObjs by the amount given by the difference between current
    //bar start position and the new desired start position.
    //In addition, the position of the barline at the end of this bar is also shifted so that
    //the new width on the bar becomes nNewBarWidth.
    //
    //Parameters:
    //   nNewBarWidth - the new width that this bar will have.
    //   uNewStart - the new left position for the start of this bar
    //
    //Results and return value:
    //   The new positions are stored in the StaffObjs
    //   The position of the barline at the end of this bar is retuned.

    //@todo
    //    The actual process does not distribute space evenly across the bar. It just
    //    moves the barline so that the bar size get changed; the added space all remains
    //    at the end, before the barline.

    lmLUnits uBarPosition = 0;
	for (lmItTimeLine it=m_aLines.begin(); it != m_aLines.end(); it++)
	{
		lmLUnits uPos = (*it)->ShiftEntries(uNewBarSize, uNewStart);
		uBarPosition = wxMax(uBarPosition, uPos);
    }

    return uBarPosition;

}

lmLUnits lmTimeposTable::TenthsToLogical(lmTenths rTenths, int nStaff)
{
	return m_pStaff[nStaff-1]->TenthsToLogical(rTenths);
}

//-------------------------------------------------------------------------------------
//  debug methods
//-------------------------------------------------------------------------------------

wxString lmTimeposTable::DumpTimeposTable()
{
    wxString sMsg = _T("");
	for (lmItTimeLine it=m_aLines.begin(); it != m_aLines.end(); it++)
	{
		sMsg += (*it)->DumpMainTable();
    }
    return sMsg;
}

