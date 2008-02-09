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

//spacing function parameters
float				m_rDmin = 8.0f;				//Dmin: min. duration to consider
lmTenths			m_rMinSpace = 15.0f;		//Space(Dmin):  space for Dmin

//TODO: User options
lmTenths m_rSpaceAfterStartOfMeasure = 7.5f;
lmTenths m_rSpaceAfterProlog = 25.0f;
lmTenths m_rSpaceAfterIntermediateClef = 20.0f;
lmTenths m_rMinSpaceBetweenNoteAndClef = 10.0f;

//=====================================================================================
//lmTimeposEntry
//=====================================================================================

lmTimeposEntry::lmTimeposEntry(eTimeposEntryType nType, lmStaffObj* pSO, lmShape* pShape,
                               bool fProlog)
{
    m_nType = nType;
    m_pSO = pSO;
    m_pShape = pShape;
	m_fProlog = fProlog;

    m_uSpace = 0.0f;
    m_xFinal = 0.0f;

    if (pSO && (pSO->GetClass() == eSFOT_NoteRest || pSO->GetClass() == eSFOT_Barline))
        m_rTimePos = pSO->GetTimePos();
    else
        m_rTimePos = -1.0f;
    m_uSize = (pShape ? pShape->GetWidth() : 0.0f);
    m_xLeft = (pShape ? pShape->GetXLeft() : 0.0f);
    m_xInitialLeft = m_xLeft;

    if (pSO && pSO->GetClass() == eSFOT_NoteRest)
        m_uxAnchor = m_xLeft - pSO->GetAnchorPos();
    else
        m_uxAnchor = 0.0f;

}

void lmTimeposEntry::AssignSpace(lmTimeposTable* pTT)
{
	//assign spacing to this object

    switch (m_nType)
    {
        case eAlfa:
			m_uSpace = pTT->TenthsToLogical(m_rSpaceAfterStartOfMeasure, 1);
			break;

        case eOmega:
			m_uSpace = 0.0f;
            break;

        default:
            //lmStaffObj entry
			if (!m_pSO->IsVisible())
			{
				m_uSpace = 0.0f;
				m_uSize = 0.0f;
			}
			else
			{
				if (m_pSO->GetClass() == eSFOT_NoteRest)
				{
					SetNoteRestSpace(pTT);
				}
				else if (m_pSO->GetClass() == eSFOT_Clef)
				{
					//m_uSpace = ((lmClef*)m_pSO)->
					m_uSpace = m_uSize + pTT->TenthsToLogical(10, 1);
				}
				else if (m_pSO->GetClass() == eSFOT_KeySignature)
				{
					//m_uSpace = ((lmKeySignature*)m_pSO)->
					m_uSpace = m_uSize + pTT->TenthsToLogical(10, 1);
				}
				else if (m_pSO->GetClass() == eSFOT_TimeSignature)
				{
					//m_uSpace = ((lmTimeSignature*)m_pSO)->
					m_uSpace = m_uSize + pTT->TenthsToLogical(10, 1);
				}
				else
					m_uSpace = 0.0f;
			}
    }
}

void lmTimeposEntry::SetNoteRestSpace(lmTimeposTable* pTT)
{
	static const float rLog2 = 0.3010299956640f;		// log(2)

	float rFactor = pTT->SpacingFactor();
    if (pTT->SpacingMethod() == esm_PropConstantFixed)
    {
        //proportional constant spacing.
        float rVar = rFactor * log(((lmNoteRest*)m_pSO)->GetDuration() / m_rDmin) / rLog2;
	    //wxLogMessage(_T("[lmTimeLine::SetNoteRestSpace] duration=%s, rel.space=%.2f"),
	    //		pSO->GetLDPNoteType(), (1.0f + rVar) );

	    //spacing function
	    m_uSpace = m_rMinSpace;		//Space(Di) = Space(Dmin)
	    if (rVar > 0.0f) {
		    rVar *= m_rMinSpace;
		    m_uSpace += rVar;			//Space(Di) = Space(Dmin)*[1 + lod2(Di/Dmin)]
	    }
    }
    else if (pTT->SpacingMethod() == esm_Fixed)
    {
        // fixed spacing
        m_uSpace = pTT->FixedSpacingValue();
    }
    else
        wxASSERT(false);

	//convert to lmLUnits
	int iStaff = m_pSO->GetStaffNum();
	m_uSpace = pTT->TenthsToLogical(m_uSpace, iStaff);

	//if space is lower than object width force space to be a little more than width
	if (m_uSpace < m_uSize)
		m_uSpace = m_uSize + pTT->TenthsToLogical(5.0f, iStaff);

}

void lmTimeposEntry::Reposition(lmLUnits uxPos)
{
	//reposition Shape
    //wxLogMessage(_T("Reposition: old xLeft=%.2f, new xLeft=%.2f"), m_xInitialLeft, uxPos);
    //lmLUnits uShift = uxPos - m_xInitialLeft + m_uxAnchor;
    lmLUnits uShift = uxPos - m_xInitialLeft;
	if (!m_fProlog)
		m_pSO->StoreOriginAndShiftShapes( uShift );
	else
		if (m_pShape) m_pShape->Shift(uShift, 0.0);

	//update entry data
	//m_xLeft = uxPos + m_uxAnchor;
	m_xLeft = uxPos;
	m_xInitialLeft = m_xLeft;
	m_xFinal = uxPos + m_uSpace;
}


//=====================================================================================
//lmTimeLine
//=====================================================================================

lmTimeLine::lmTimeLine(lmTimeposTable* pMngr, int nInstr, int nVoice, lmLUnits uxStart)
{
	m_pOwner = pMngr;
	m_nInstr = nInstr;
	m_nVoice = nVoice;
    NewEntry(eAlfa, (lmStaffObj*)NULL, (lmShape*)NULL, false);
    m_aMainTable.back()->m_uxAnchor = 0.0f;
    m_aMainTable.back()->m_xLeft = uxStart;
    m_aMainTable.back()->m_xInitialLeft = uxStart;
}

lmTimeLine::~lmTimeLine()
{
    for (lmItEntries it = m_aMainTable.begin(); it != m_aMainTable.end(); it++) 
	{
		delete *it;
	}
	m_aMainTable.clear();
}

lmTimeposEntry* lmTimeLine::AddEntry(eTimeposEntryType nType, lmStaffObj* pSO, lmShape* pShape,
									 bool fProlog)
{
    return NewEntry(nType, pSO, pShape, fProlog);
}

lmTimeposEntry* lmTimeLine::NewEntry(eTimeposEntryType nType, lmStaffObj* pSO, lmShape* pShape,
									 bool fProlog)
{
    lmTimeposEntry* pEntry = new lmTimeposEntry(nType, pSO, pShape, fProlog);
    m_aMainTable.push_back(pEntry);
	pEntry->AssignSpace(m_pOwner);
	return pEntry;
}

lmLUnits lmTimeLine::ShiftEntries(lmLUnits uNewBarSize, lmLUnits uNewStart)
{
    lmLUnits uBarPosition = 0;
    lmTimeposEntry* pTPE = m_aMainTable.front();
    lmLUnits uShift = uNewStart - pTPE->m_xLeft;

	//wxLogMessage(_T("[lmTimeLine::ShiftEntries] Reposition: uNewBarSize=%.2f  uNewStart=%.2f  Shift=%.2f"),
	//			 uNewBarSize, uNewStart, uShift );

	//Update table and store the new x positions into the StaffObjs
    for (lmItEntries it = m_aMainTable.begin(); it != m_aMainTable.end(); it++) 
	{
        pTPE = *it;
        if (uShift != 0.0f && pTPE->m_nType == eStaffobj)
        {
			if (!pTPE->m_fProlog)
				pTPE->m_pSO->StoreOriginAndShiftShapes( uShift );
			else
				pTPE->m_pShape->Shift(uShift, 0.0);

        }
        else if (pTPE->m_nType == eOmega)
        {
            //there might be no barline.
            if (pTPE->m_pSO)
            {
                uBarPosition = uNewStart + uNewBarSize - pTPE->m_uSize;
                lmLUnits uShiftBar = uBarPosition - pTPE->m_xLeft;
				pTPE->m_pSO->StoreOriginAndShiftShapes(uShiftBar);
                //uBarPosition = pTPE->m_xLeft + uShiftBar;
				//wxLogMessage(_T("[lmTimeLine::ShiftEntries] Reposition bar: uBarPosition=%.2f, uShiftBar=%.2f"),
				//			uBarPosition, uShiftBar );
            }
        }
    }
    return uBarPosition;

}

float lmTimeLine::ProcessTimepos(float rTime, lmLUnits uxPos, float rFactor, lmLUnits* pMaxPos)
{
	//Starting at current position, explores the line.
	//Set the position of all time positioned objects placed at time rTime, until 
    //we reach a time greater that rTime. The position for this next rTime object is
    //also assigned tentatively.
	//If during this traversal we find not timed objects they are ignored and when
	//reached next rTime we go back assigning positions to them.
	//Returns the next timepos in this line, or -1 if no more entries.
    //Updates *pMaxPos with the maximum xPos reached after adding its width to
    //current pos

	//When entering this method, iterator m_it must point to a timed object. If it is
	//placed at time rTime, process it and all the following having the same timepos,
	//until we find the next timepos or we arrive to end of line.
	//Not-timed objects are skipped
    *pMaxPos = 0.0f;
	lmItEntries itLastNotTimed = m_aMainTable.end();
	while (m_it != m_aMainTable.end() && 
			(IsEqualTime((*m_it)->m_rTimePos, rTime) || (*m_it)->m_rTimePos < 0.0f) )
    {
		//if not-timed take note and continue
		if ((*m_it)->m_rTimePos == -1.0f)
			itLastNotTimed = m_it;

        //else if contains a StaffObj (in the omega entry there might be no barline)
		//assign it a final position
        else if ((*m_it)->m_pSO)
        {
			//move the shape and update the entry data
            (*m_it)->Reposition(uxPos + (*m_it)->m_uxAnchor);

			//compute new current position
			lmLUnits uxFinal = uxPos + (*m_it)->m_uSpace;
			m_uxCurPos = wxMax(uxFinal, m_uxCurPos);
            uxFinal = uxPos + (*m_it)->m_uSize;
            *pMaxPos = wxMax(*pMaxPos, uxFinal);
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
            lmLUnits rSpaceAfterClef = 
                m_pOwner->TenthsToLogical(m_rSpaceAfterIntermediateClef, 
                                          (*m_it)->m_pSO->GetStaffNum() );
            lmLUnits uxCurPos = m_uxCurPos - rSpaceAfterClef;
            lmItEntries itStart = m_it;
            lmItEntries it = m_it;
            it--;
            while (it != m_aMainTable.begin() && (*it)->m_rTimePos < 0.0f)
            {
                uxCurPos -= (*it)->m_uSize + (*it)->m_uSpace;
			    (*it)->Reposition(uxCurPos);
                it--;
            }
            //Here we have arrived to a note/rest. Verify that is left position is
            //lower than the last assigned position. Otherwise we have to shift
            //the not-timed objects.
            if (it != m_aMainTable.begin())
            {
                if ((*it)->m_xLeft + (*it)->m_uSize >= uxCurPos)
                {
                    //Overlap. We have to shift the not-timed objects.
                    uxCurPos = (*it)->m_xLeft + (*it)->m_uSize +
                        m_pOwner->TenthsToLogical(m_rMinSpaceBetweenNoteAndClef, 
                                                  (*it)->m_pSO->GetStaffNum() );
                    it++;
                    while (it != itStart)
                    {
 			            (*it)->Reposition(uxCurPos);
                        uxCurPos += (*it)->m_uSize + (*it)->m_uSpace;
                        it++;
                    }
		            //We have arrived to next timepos. Assign position tentatively
                    m_uxCurPos = uxCurPos + rSpaceAfterClef;
                    *pMaxPos = uxCurPos;
		            (*itStart)->m_xLeft = m_uxCurPos;
                }
            }

		}
	}

	//done. Return the next timepos in this line, or -1 if no more entries.
	if (m_it != m_aMainTable.end()) {
      //  wxLogMessage(_T("Timed. More entries. Next at time=%.2f ID=%d"),
					 //(*m_it)->m_rTimePos, (*m_it)->m_pSO->GetID() );
        return (*m_it)->m_rTimePos;
    }
    else {
        //wxLogMessage(_T("Timed. No more entries"));
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

lmLUnits lmTimeLine::GetAnchorForTime(float rTime)
{
	//When enetring this method iterator m_it points to the next timed entry or
	//to end iterator.
	//If next entry time is rTime, returns its xAnchor position. Else returns 0

	if (m_it != m_aMainTable.end() && IsEqualTime((*m_it)->m_rTimePos, rTime) )
		return (*m_it)->m_uxAnchor;
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
	////DBG. log message
	//if (m_it != m_aMainTable.end())
 //       wxLogMessage(_T("Not timed. More entries. Next at time=%.2f ID=%d"), 
	//				 //(*m_it)->m_rTimePos, (*m_it)->m_pSO->GetID() );
 //   else
 //       wxLogMessage(_T("Not timed. No more entries."));

    return m_uxCurPos;
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
    //          ...+  ..+   ...+ ..+   +  ..........+........+........+........+........+........+........+
    sMsg += _T("Item    Type      ID Prolog   TimePos    xInit  xAnchor    xLeft     Size    Space   xFinal\n");

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
                sMsg += _T("  Alfa              ");
                break;

            case eOmega:
                sMsg += _T("  Omega");
                if (pTE->m_pSO) {
                    sMsg += wxString::Format(_T("%3d          "), pTE->m_pSO->GetClass() );
                } else {
                    sMsg += _T("  -           ");
                }
                break;

            default:
                //lmStaffObj entry
				sMsg += wxString::Format(_T("  pSO %4d %3d   %s  "),
										pTE->m_pSO->GetClass(),
										pTE->m_pSO->GetID(),
										(pTE->m_fProlog ? _T("S") : _T(" ")) );
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

void lmTimeposTable::SetParameters(float rSpacingFactor, lmESpacingMethod nSpacingMethod,
                                   lmTenths nSpacingValue)
{
    m_rSpacingFactor = rSpacingFactor;
    m_nSpacingMethod = nSpacingMethod;
    m_rSpacingValue = nSpacingValue;
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

void lmTimeposTable::StartLines(int nInstr, lmLUnits uxStart, lmVStaff* pVStaff)
{
	int nNumStaves = pVStaff->GetNumStaves();
    wxASSERT(nNumStaves < lmMAX_STAFF);

    for(int iS=0; iS < nNumStaves; iS++)
    {
        m_nCurVoice[iS] = iS+1;
		m_pStaff[iS] = pVStaff->GetStaff(iS+1);
        StartLine(nInstr, iS+1, uxStart);
    }
}

void lmTimeposTable::StartLine(int nInstr, int nVoice, lmLUnits uxStart)
{
    //Start a new line for instrument nInstr (0..n-1)
	//If this is the first line for instrument nInstr, all non-voiced StaffObj
	//will be assigned to this line, as well as the first voice encountered

    if (uxStart < 0.0f)
    {
        //Copy start position from first line
        uxStart = m_aLines.front()->m_aMainTable.front()->m_xLeft;
    }


	lmTimeLine* pLine = new lmTimeLine(this, nInstr, nVoice, uxStart);
	m_aLines.push_back(pLine);
	m_pCurEntry = pLine->m_aMainTable.back(); 
	m_itCurLine = m_aLines.end();
	m_itCurLine--;

	//wxLogMessage(_T("[lmTimeposTable::StartLine] New line started for instr=%d, voice=%d"),
 //       nInstr, nVoice);
}

void lmTimeposTable::CloseLine(lmStaffObj* pSO, lmShape* pShape, lmLUnits xStart)
{
	//close current line.

    m_pCurEntry = (*m_itCurLine)->AddEntry(eOmega, pSO, pShape, false);
    m_pCurEntry->m_xLeft = xStart;
    m_pCurEntry->m_xInitialLeft = xStart;
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

void lmTimeposTable::AddEntry(int nInstr, lmStaffObj* pSO, lmShape* pShape, bool fProlog, int nStaff)
{
    //determine voice
    int nVoice;
	if (nStaff == 0)
	{
		if (pSO->GetClass() == eSFOT_NoteRest)
			nVoice = ((lmNoteRest*)pSO)->GetVoice();
		else
			nVoice = m_nCurVoice[ pSO->GetStaffNum() - 1 ];
	}
	else
		nVoice = m_nCurVoice[nStaff - 1];

	//create the entry
	AddEntry(nInstr, nVoice, pSO, pShape, fProlog);
}

void lmTimeposTable::AddEntry(int nInstr, int nVoice, lmStaffObj* pSO, lmShape* pShape, bool fProlog)
{
	//Add an entry to current line for the specified nInstr/nVoice.

    //wxLogMessage(_T("[lmTimeposTable::AddEntry] instr=%d, voice=%d, SO type = %d, staff=%d"),
    //    nInstr, nVoice, pSO->GetClass(), pSO->GetStaffNum() );

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
				m_pCurEntry = (*it)->AddEntry(eStaffobj, pSO, pShape, fProlog);
                //wxLogMessage(_T("\tEntry added to line for voice=%d"), (*it)->m_nVoice);

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
	m_pCurEntry = (*m_itCurLine)->AddEntry(eStaffobj, pSO, pShape, fProlog);
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

lmLUnits lmTimeposTable::DoSpacing(bool fTrace)
{
    //wxLogMessage( DumpTimeposTable() );
    lmLUnits uSize = ComputeSpacing(m_rSpacingFactor);
    //wxLogMessage( DumpTimeposTable() );
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
			lmLUnits uxObjPos = (*it)->GetPosForTime(rTime);
            lmLUnits uxObjAnchor = (*it)->GetAnchorForTime(rTime);
            if (uxObjAnchor < 0.0f) uxObjPos -= uxObjAnchor;
			uxPos = wxMax(uxPos, uxObjPos); 
		}
		//wxLogMessage(_T("[lmTimeposTable::ComputeSpacing] Setting timed objects at time %.2f, xPos=%.2f"),
		//	rTime, uxPos);

		//Process all timed objects placed at time rTime
		fContinue = false;
		float rNextTime = 10000.0f;		//any too big value
        lmLUnits uxMaxPos = 0.0f;
		for (lmItTimeLine it=m_aLines.begin(); it != m_aLines.end(); it++)
		{
            lmLUnits uxObjMaxPos;
			float rNewTime = (*it)->ProcessTimepos(rTime, uxPos, rFactor, &uxObjMaxPos);
			if (rNewTime > 0.0f)
			{
				fContinue = true;
				rNextTime = wxMin(rNextTime, rNewTime);
			}
            uxMaxPos = wxMax(uxMaxPos, uxObjMaxPos);
		}
        uxPos = uxMaxPos;
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

