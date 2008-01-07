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
#include "../score/Score.h"
#include "wx/debug.h"
#include "TimeposTable.h"

//helper functions to compare two floating point numbers
bool IsEqualTime(float t1, float t2)
{
    return (fabs(t1 - t2) < 0.1);
}


//=====================================================================================
//lmTimeLine
//=====================================================================================

lmTimeLine::lmTimeLine(int nInstr, int nVoice, lmLUnits uxStart)
{
	m_nInstr = nInstr;
	m_nVoice = nVoice;
    NewEntry(eAlfa, -1, (lmStaffObj*)NULL);
	SetCurXLeft(uxStart);
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

	//Update table and store the new x positions into the StaffObjs
    for (lmItEntries it = m_aMainTable.begin(); it != m_aMainTable.end(); it++) 
	{
        pTPE = *it;
        if (pTPE->m_nType == eStaffobj)
        {
            pTPE->ShiftEntry(uShift);
            (pTPE->m_pSO)->StoreOriginAndShiftShapes( pTPE->m_xLeft - pTPE->m_xInitialLeft);
			pTPE->m_xInitialLeft = pTPE->m_xLeft;
        }
        else if (pTPE->m_nType == eOmega)
        {
            //there might be no barline.
            if (pTPE->m_pSO)
            {
                lmLUnits nShiftBar = uNewStart + uNewBarSize - pTPE->m_xFinal + 1;
                pTPE->ShiftEntry(nShiftBar);
                (pTPE->m_pSO)->StoreOriginAndShiftShapes(  pTPE->m_xLeft - pTPE->m_xInitialLeft );
				pTPE->m_xInitialLeft = pTPE->m_xLeft;
                uBarPosition = pTPE->m_xLeft;
            }
        }
    }
    return uBarPosition;

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
    sMsg += _T("Item    Type         TimePos    Size   xLeft xAnchor  xRight  xFinal\n");

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
                sMsg += _T("  Alfa     ");
                break;

            case eOmega:
                sMsg += _T("  Omega");
                if (pTE->m_pSO) {
                    sMsg += wxString::Format(_T("%3d "), (pTE->m_pSO)->GetClass() );    //TODO convert to string
                } else {
                    sMsg += _T("  -        ");
                }
                break;

            default:
                //lmStaffObj entry
                sMsg += wxString::Format(_T("  pSO %4d "), (pTE->m_pSO)->GetClass() );     //TODO convert to string
        }

        sMsg += wxString::Format(_T("%11.2f %.2f %.2f %.2f %.2f %.2f\n"),
            pTE->m_rTimePos, pTE->m_uSize, pTE->m_xLeft, pTE->m_uxAnchor,
            pTE->m_xRight, pTE->m_xFinal);
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
    m_nCurVoice = 0;
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

    m_nCurVoice = 0;

    for (int i = 0; i < (int)m_aTimeAux.size(); i++)
	{
        delete m_aTimeAux[i];
    }
    m_aTimeAux.clear();

	for (lmItTimeLine it=m_aLines.begin(); it != m_aLines.end(); it++)
	{
		delete *it;
	}
	m_aLines.clear();

}

void lmTimeposTable::StartLine(int nInstr, lmLUnits uxStart, int nVoice)
{
    //Start a new line for instrument nInstr (0..n-1)
	//If this is the first line for instrument nInstr, all non-voiced StaffObj
	//will be assigned to this line, as well as the first voice encountered

	lmTimeLine* pLine = new lmTimeLine(nInstr, nVoice, uxStart);
	m_aLines.push_back(pLine);
	m_pCurEntry = pLine->m_aMainTable.back(); 
	m_itCurLine = m_aLines.end();
	m_itCurLine--;
}

void lmTimeposTable::CloseLine(lmLUnits uxLeft, lmLUnits uxFinal, lmStaffObj* pSO)
{
	//close current line.

	if (pSO)
		m_pCurEntry = (*m_itCurLine)->AddEntry(eOmega, pSO->GetTimePos(), pSO);
	else
		m_pCurEntry = (*m_itCurLine)->AddEntry(eOmega, -1, pSO);

	SetCurXLeft(uxLeft);
	SetCurXFinal(uxFinal);
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

				//update voice
				if ((*it)->m_nVoice == 0 && nVoice != 0)
					(*it)->m_nVoice = nVoice;
				return;
			}
		}
	}
	//not found. Start new line
	wxASSERT(itInstr != m_aLines.end());	//a line for the instrument must exist
	StartLine(nInstr, (*itInstr)->GetXStartLine(), nVoice);
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

void lmTimeposTable::SetCurXAnchor(lmLUnits uValue)
{
    m_pCurEntry->m_uxAnchor = uValue;
}


void lmTimeposTable::AddTimeAuxEntry(int nLine, int nItem, lmTimeposEntry* pTPE)
{
    //Add a new entry to the auxiliary Timeaux table so that the table is ordered
    //by timepos (ascending). Parameter nItem is the index over line table pointing
    //to the item to add to the Tiemaux table and ptr pTPE point to that item.

    //the table must be ordered by timepos

    if (pTPE->m_rTimePos < 0) return;            //discard non timed entries

    // Create entry to be added
    lmTimeauxEntry* pNew = new lmTimeauxEntry(nItem, nLine, pTPE->m_rTimePos);

    //if there are no entries in Timeaux table or the timePos of last entry is lower
    //or equal than the timepos to be inserted , just create the new entry at the end
    //of the table
    if (m_aTimeAux.size() == 0)
    {
        //No entries in table. This one is the first one.
        m_aTimeAux.push_back(pNew);
    }
    else
    {
        int i = m_aTimeAux.size() - 1;    //index to last entry
        lmTimeauxEntry* pTAE = m_aTimeAux[i];
        if (pTAE->rTimePos < pTPE->m_rTimePos || IsEqualTime(pTAE->rTimePos, pTPE->m_rTimePos))
        {
            //all existing entries in Timeaux table have timepos lower or equal than the
            //new one. Add it at the end.
            m_aTimeAux.push_back(pNew);
        }
        else
        {
            //the new entry must be inserted between two existing entries.

            //locate insertion point
            for (i=0; i < (int)m_aTimeAux.size(); i++) {
                pTAE = m_aTimeAux[i];
                if (pTAE->rTimePos > pTPE->m_rTimePos && !IsEqualTime(pTAE->rTimePos, pTPE->m_rTimePos)) break;
            }
            wxASSERT(i >= 0);
			//insert before entry number i
			std::vector<lmTimeauxEntry*>::iterator it =
				std::find(m_aTimeAux.begin(), m_aTimeAux.end(), m_aTimeAux[i]);
			m_aTimeAux.insert(it, pNew);
        }
    }

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

lmLUnits lmTimeposTable::ArrangeStaffobjsByTime(bool fTrace)
{
    //StaffObjs positions are arranged so that all StaffObjs that must sound at the same time
    //will have the same x anchor coordinate.
    //The new positions are not stored in the StaffObjs but only in these tables.
    //This method returns the bar column size (including the width of the barline at the end)

    //build timeaux table. Ordered by time
    //-------------------------------------
    int i;
    m_aTimeAux.clear();
    for (int iL=0; iL < (int)m_aLines.size(); iL++) 
	{
		for (int iE=0; iE < (int)(m_aLines[iL]->m_aMainTable).size(); iE++) 
			AddTimeAuxEntry(iL, iE, m_aLines[iL]->m_aMainTable[iE] );
	}

    //if trace requested dump tables to log
    if (fTrace) {
        wxLogMessage(_T("TimepoTable: Before arranging"));
        wxLogMessage(_T("***************************************\n"));
        wxLogMessage( DumpTimeposTable() );
        wxLogMessage( DumpTimeauxTable() );
    }

    //loop to arrange StaffObjs positions. Each iteration corresponds to a time value
    //-----------------------------------------------------------------------------------
    int nLine;              //auxiliary. The line in process
    float rTime;            //auxiliary. The time value in process
    lmLUnits uShift;        //auxiliary. x shift to apply
    lmLUnits uMinStartPos;        //minimum start position for StaffObjs of current time value

    //As the position for a time value must be greater than the maximum final position of
    //the StaffObjs located at the previous time value we have to:
    // 1. compute the maximum final for each time value, and
    // 2. ensure that the start position for the next time mark is greater than this
    lmLUnits uMaxFinalPrev;        //maximum final position for previous time mark
    lmLUnits uMaxFinalCur;        //maximum final position for current time mark
    lmLUnits uMaxAnchorCur;        //maximum anchor position for current time mark
    uMaxFinalPrev = 0;

    int iItem;
    lmTimeposEntry* pTPE;
    lmTimeauxEntry* pTAE;
    int iFirst = 0;        //index to the first entry for current time value
    int iNext = 0;        //index to the first entry for the next time value

    while (iNext < (int)m_aTimeAux.size())
    {
        i = iFirst;
        pTAE = m_aTimeAux[iFirst];
        rTime = pTAE->rTimePos;        //time to process
        uMinStartPos = uMaxFinalPrev;

        //loop to compute the maximum left position of all entries of current time
        while (i < (int)m_aTimeAux.size())
        {
            pTAE = m_aTimeAux[i];
            if (!IsEqualTime(pTAE->rTimePos, rTime)) break;
            iItem = pTAE->nItem;
			nLine = pTAE->nLine;
            pTPE = m_aLines[nLine]->m_aMainTable[iItem];
            uMinStartPos = wxMax(uMinStartPos, pTPE->m_xLeft);
            i++;
        }
        iNext = i;       //save start index for next cicle of main loop
        ///*dbg*/    wxLogMessage(wxString::Format(
        //            _T("[lmTimeposTable::ArrangeStaffobjsByTime] uMinStartPos=%.2f "),
        //            uMinStartPos ));

        //loop to compute the shift to apply in order to align left positions, and
        //the max. anchor position
        uMaxAnchorCur = 0;
        i = iFirst;
        while (i < (int)m_aTimeAux.size())
        {
            pTAE = m_aTimeAux[i];
            if (!IsEqualTime(pTAE->rTimePos, rTime)) break;
            iItem = pTAE->nItem;
			nLine = pTAE->nLine;
            pTPE = m_aLines[nLine]->m_aMainTable[iItem];
            pTAE->uShift = uMinStartPos - pTPE->m_xLeft;
			lmLUnits xPos = pTAE->uShift + pTPE->m_uxAnchor;
            uMaxAnchorCur = wxMax(uMaxAnchorCur, xPos);
            i++;
        }
        ///*dbg*/    wxLogMessage(wxString::Format(
        //            _T("[lmTimeposTable::ArrangeStaffobjsByTime] uMaxAnchorCur=%.2f "),
        //            uMaxAnchorCur ));

        //loop to compute the final shift to apply in order to align by anchor positions
        i = iFirst;
        while (i < (int)m_aTimeAux.size())
        {
            pTAE = m_aTimeAux[i];
            if (!IsEqualTime(pTAE->rTimePos, rTime)) break;
            iItem = pTAE->nItem;
			nLine = pTAE->nLine;
            pTPE = m_aLines[nLine]->m_aMainTable[iItem];
            pTAE->uShift = uMaxAnchorCur - pTPE->m_uxAnchor;
            i++;
        }

        //shift the start position of all StaffObjs of current time (and those following
        //them of the same line with no specific time) and
        //compute the the maximum final position for current time
        uMaxFinalCur = 0;
        i = iFirst;
        while (i < (int)m_aTimeAux.size())
        {
            //process one entry of Time table
            pTAE = m_aTimeAux[i];
            if (!IsEqualTime(pTAE->rTimePos, rTime)) break;
            iItem = pTAE->nItem;
			nLine = pTAE->nLine;
            uShift = pTAE->uShift;

            //process this entry at TimePos table
			///*dbg*/    wxLogMessage(wxString::Format(
			//            _T("[lmTimeposTable::ArrangeStaffobjsByTime] applying shift (%.2f) to entry %d. rTime=%.2f "),
			//            uShift, i, pTAE->rTimePos ));
            pTPE = m_aLines[nLine]->m_aMainTable[iItem];
            pTPE->ShiftEntry(uShift);
            uMaxFinalCur = wxMax(uMaxFinalCur, pTPE->m_xFinal);

            //process any following entry of this line with no specific time (timepos=-1)
            iItem++;
            while (iItem < (int)m_aLines[nLine]->m_aMainTable.size())
            {
                pTPE = m_aLines[nLine]->m_aMainTable[iItem];
                if (pTPE->m_rTimePos != -1 ) break;

				pTPE->ShiftEntry(uShift);
                uMaxFinalCur = wxMax(uMaxFinalCur, pTPE->m_xFinal);
                iItem++;
            }

            i++;

        }

        //at this point StaffObjs at current time value StaffObjs and their inmediate
        //following entries with no specific tiem are aligned by its xAnchor position.
        //The maximum final position of all these entries is storet in uMaxFinalCur

        //if trace requested dump tables to log
        if (fTrace) {
            wxLogMessage( wxString::Format(
                _T("TimepoTable: arrange loop. iFirst=%d, uMaxFinalCur=%.2f"),
                iFirst, uMaxFinalCur) );
            wxLogMessage(_T("***************************************\n"));
            wxLogMessage( DumpTimeposTable() );
            wxLogMessage( DumpTimeauxTable() );
        }

        //the current time StaffObjs are aligned. Advance to next time value
        iFirst = iNext;
        uMaxFinalPrev = uMaxFinalCur;

    }

    //return the measure column size
    lmTimeposEntry* pEnd = m_aLines[nLine]->m_aMainTable.back();
    lmTimeposEntry* pStart = m_aLines[nLine]->m_aMainTable[0];		//alpha entry
    lmLUnits uColumnSize = pEnd->m_xFinal - pStart->m_xLeft;

    //if trace requested dump tables to log
    if (fTrace) {
        wxLogMessage( wxString::Format(
            _T("TimepoTable: After arranging. MeasureWidth=%.2f"),
            uColumnSize) );
        wxLogMessage(_T("***************************************\n"));
        wxLogMessage( DumpTimeposTable() );
        wxLogMessage( DumpTimeauxTable() );
    }

    return uColumnSize;

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

wxString lmTimeposTable::DumpTimeauxTable()
{
    wxString sMsg = _T("Dump of Time aux. table. \n");
    sMsg += _T("===================================================================\n\n");

    if (m_aTimeAux.size() == 0)
    {
        sMsg += _T("The table is empty.");
        return sMsg;
    }

    //headers
    sMsg += _T("Item    Line   Ref     TimePos    Size   xLeft xAnchor  xRight  xFinal\n");

    //loop to dump table entries
    float rTimePrev = -1;
    for (int i = 0; i < (int)m_aTimeAux.size(); i++)
    {
        lmTimeauxEntry* pTAE = m_aTimeAux[i];
        if (!IsEqualTime(rTimePrev, pTAE->rTimePos))
        {
            rTimePrev = pTAE->rTimePos;
            sMsg += _T("----------------------------------------------------------------------------\n");
        }
        sMsg += wxString::Format( _T("%4d: %8d %4d %11.2f "),
            i, pTAE->nLine, pTAE->nItem, pTAE->rTimePos);
		int iItem = pTAE->nItem;
		int nLine = pTAE->nLine;

        lmTimeposEntry* pTPE = m_aLines[nLine]->m_aMainTable[iItem];
        sMsg += wxString::Format( _T("%.2f %.2f %.2f %.2f %.2f\n"),
            pTPE->m_uSize, pTPE->m_xLeft, pTPE->m_uxAnchor, pTPE->m_xRight, pTPE->m_xFinal);
    }

    return sMsg;

}
