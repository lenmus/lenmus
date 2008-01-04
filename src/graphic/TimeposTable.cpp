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
//public methods
//=====================================================================================

lmTimeposTable::lmTimeposTable()
{
    m_nCurVoice = 0;
	for (int i=0; i < lmMAX_VOICE; i++)
	{
		m_aVoices.push_back(false);
	}

}

lmTimeposTable::~lmTimeposTable()
{
	m_aVoices.clear();

    for (int i = 0; i < (int)m_aTimeAux.size(); i++)
	{
        delete m_aTimeAux[i];
    }
    m_aTimeAux.clear();

	for (int j=0; j < lmMAX_VOICE; j++)
	{
		for (int i = 0; i < (int)m_aTimePos[j].size(); i++)
		{
			delete m_aTimePos[j][i];
		}
	    m_aTimePos[j].clear();
	}
}

void lmTimeposTable::CleanTable()
{
    //This method must be invoked before using the table. Can be also invoked as
    //many times as desired to clean the table and reuse it.

    for (int i = 0; i < (int)m_aTimeAux.size(); i++)
	{
        delete m_aTimeAux[i];
    }
    m_aTimeAux.clear();

	for (int j=0; j < lmMAX_VOICE; j++)
	{
		for (int i = 0; i < (int)m_aTimePos[j].size(); i++)
		{
			delete m_aTimePos[j][i];
		}
	    m_aTimePos[j].clear();
	}

    m_nCurVoice = 0;

}

void lmTimeposTable::StartVoice(int nVoice)
{
    //start a new voice (increment counter and add alfa entry)

    m_nCurVoice = nVoice;
	m_aVoices[nVoice-1] = true;
    NewEntry(nVoice, eAlfa, -1, (lmStaffObj*)NULL);

    //the start xpos has been set to zero in NewEntry method
    //set the start xpos to the same value than in the previous voice
    if (m_nCurVoice > 1) {
        //find alfa entry of previous voice
        int i = m_aTimePos[0].size() - 2;      //point to last entry (the previous to this new one)
        lmTimeposEntry* pEntry = m_aTimePos[0][i];
        for (; pEntry->m_nType != eAlfa; i--) {
            wxASSERT(i >= 0);
            pEntry = m_aTimePos[0][i];
        }

        //here i points to the alfa entry. Copy its xpos
        SetCurXLeft( pEntry->m_xLeft );
    }

}

void lmTimeposTable::CloseAllVoices(lmLUnits uCurX)
{
    //rCurX = current canvas position
    wxASSERT(m_nCurVoice > 0);       //there must be a voice

	for (int i=0; i < lmMAX_VOICE; i++)
		NewEntry(i, eOmega, -1, (lmStaffObj*)NULL);

    SetCurXLeft(uCurX);

}

void lmTimeposTable::AddEntry(float rTimePos, lmStaffObj* pSO)
{
	int nVoice = 1;
	if (pSO->GetClass() == eSFOT_NoteRest)
	{
		nVoice = ((lmNoteRest*)pSO)->GetVoice();
		if (m_nCurVoice != nVoice)
           StartVoice(nVoice);
	}
    NewEntry(nVoice, eStaffobj, rTimePos, pSO);
}

void lmTimeposTable::AddBarline(lmStaffObj* pSO)
{
    /*
    There might be many barline StaffObjs for this bar column, one for each bar in
    each staff. So, in this method the Omega entry of each voice is associated to
    the barline object
    */

    lmTimeposEntry* pEntry = m_aTimePos[0].back();
    wxASSERT(pEntry->m_nType == eOmega);
    pEntry->m_pSO = pSO;
    pEntry->m_rTimePos = pSO->GetTimePos();

}

//-------------------------------------------------------------------------------------
// methods for accesing/updating current entry
//-------------------------------------------------------------------------------------

void lmTimeposTable::SetCurXLeft(lmLUnits uValue)
{
    //initialices m_xLeft and m_xInitialLeft.
    //If no anchor yet specified, initialices it with xLeft
    //If it is a staffObj also initialices its size

    lmTimeposEntry* pEntry = m_aTimePos[0].back();
    pEntry->m_xLeft = uValue;
    pEntry->m_xInitialLeft = uValue;

    if (pEntry->m_uxAnchor == 0) {
        pEntry->m_uxAnchor = uValue;   //default value
    }

    if (pEntry->m_nType == eStaffobj) {
        pEntry->m_uSize = pEntry->m_xFinal - pEntry->m_xLeft;
    }

}

lmLUnits lmTimeposTable::GetCurXLeft()
{
    lmTimeposEntry* pEntry = m_aTimePos[0].back();
    return pEntry->m_xLeft;
}

lmLUnits lmTimeposTable::GetCurPaperPosX(int nVoice)
{
	//return current paper position for specified voice
	// or start of measure x pos if no object stored yet for that voice

    for (int i = (int)m_aTimeAux.size()-1; i >=0; i--)
	{
        if (m_aTimeAux[i]->nVoice == nVoice)
		{
			lmTimeposEntry* pEntry = m_aTimePos[0][m_aTimeAux[i]->nItem];
			return pEntry->m_xFinal;
		}
    }
    lmTimeposEntry* pEntry = m_aTimePos[0][0];
    return pEntry->m_xLeft;

}

lmLUnits lmTimeposTable::GetXStart()
{
    lmTimeposEntry* pEntry = m_aTimePos[0][0];
    return pEntry->m_xLeft;
}

void lmTimeposTable::SetCurXFinal(lmLUnits uValue)
{
    lmTimeposEntry* pEntry = m_aTimePos[0].back();
    pEntry->m_xFinal = uValue;
    pEntry->m_uSize = pEntry->m_xFinal - pEntry->m_xLeft;

}

void lmTimeposTable::SetCurXAnchor(lmLUnits uValue)
{
    lmTimeposEntry* pEntry = m_aTimePos[0].back();
    pEntry->m_uxAnchor = uValue;

}

//-------------------------------------------------------------------------------------
//  methods to compute results
//-------------------------------------------------------------------------------------

lmLUnits lmTimeposTable::GetGrossBarSize()
{
    //returns the maximum size of all voices

    lmLUnits rxIni, xFinal;

    //the bar starts at the xLeft of the alfa entry
    rxIni = GetStartOfBarPosition();

    //now compute the maximum xFinal
    lmTimeposEntry* pEntry;
    xFinal = 0;
    for (int i = 0; i < (int)m_aTimePos[0].size(); i++) {
        pEntry = m_aTimePos[0][i];
        xFinal = wxMax(pEntry->m_xFinal, xFinal);
    }

    return xFinal - rxIni;

}

lmLUnits lmTimeposTable::GetStartOfBarPosition()
{
    //returns the x position for the start of the bar column

    //the bar starts at the xLeft of the alfa entry
    lmTimeposEntry* pEntry = m_aTimePos[0][0];    //the alfa entry
    wxASSERT(pEntry->m_nType == eAlfa);
    return pEntry->m_xLeft;

}

lmLUnits lmTimeposTable::ArrangeStaffobjsByTime(bool fTrace)
{
    /*
    StaffObjs positions are arranged so that all StaffObjs that must sound at the same time
    will have the same x anchor coordinate.
    The new positions are not stored in the StaffObjs but only in this object tables.
    This method returns the bar column size (including the width of the barline at the end)
    */

    //build timeaux table. Ordered by time
    //-------------------------------------
    int i;
    m_aTimeAux.clear();
    for (i = 0; i < (int)m_aTimePos[0].size(); i++) {
        AddTimeAuxEntry(i);
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
    int nVoice;            //auxiliary. The voice in process
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

    int iNext, iFirst;
    int iItem;
    lmTimeposEntry* pTPE;
    lmTimeauxEntry* pTAE;
    iFirst = 0;        //index to the first table entry for current time value
    iNext = 0;        //index to the first entry for the next time value

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
            pTPE = m_aTimePos[0][iItem];
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
            pTPE = m_aTimePos[0][iItem];
            pTAE->uShift = uMinStartPos - pTPE->m_xLeft;
            uMaxAnchorCur = wxMax(uMaxAnchorCur, pTAE->uShift + pTPE->m_uxAnchor);
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
            pTPE = m_aTimePos[0][iItem];
            pTAE->uShift = uMaxAnchorCur - pTPE->m_uxAnchor;
            i++;
        }

        //shift the start position of all StaffObjs of current time (and those following
        //them of the same voice with no specific time) and
        //compute the the maximum final position for current time
        uMaxFinalCur = 0;
        i = iFirst;
        while (i < (int)m_aTimeAux.size())
        {
            //process one entry of Time table
            pTAE = m_aTimeAux[i];
            if (!IsEqualTime(pTAE->rTimePos, rTime)) break;
            iItem = pTAE->nItem;
            uShift = pTAE->uShift;

            //process this entry at TimePos table
			///*dbg*/    wxLogMessage(wxString::Format(
			//            _T("[lmTimeposTable::ArrangeStaffobjsByTime] applying shift (%.2f) to entry %d. rTime=%.2f "),
			//            uShift, i, pTAE->rTimePos ));
            pTPE = m_aTimePos[0][iItem];
            pTPE->m_xFinal += uShift;
            pTPE->m_uxAnchor += uShift;
            pTPE->m_xLeft += uShift;
            pTPE->m_xRight += uShift;
            uMaxFinalCur = wxMax(uMaxFinalCur, pTPE->m_xFinal);
            nVoice = pTPE->m_nVoice;

            //process any following entry of this voice with no specific time (timepos=-1)
            iItem++;
            while (iItem < (int)m_aTimePos[0].size())
            {
                pTPE = m_aTimePos[0][iItem];
                if (pTPE->m_nVoice != nVoice || pTPE->m_rTimePos != -1 ) break;

                pTPE->m_xFinal += uShift;
                pTPE->m_uxAnchor += uShift;
                pTPE->m_xLeft += uShift;
                pTPE->m_xRight += uShift;
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
    lmTimeposEntry* pEnd = m_aTimePos[0].back();
    lmTimeposEntry* pStart = m_aTimePos[0][0];
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
    lmLUnits uShift, nShiftBar;
    lmTimeposEntry* pTPE = m_aTimePos[0][0];
    uShift = uNewStart - pTPE->m_xLeft;

    //Update table and store the new x positions into the StaffObjs
    for (int i = 0; i < (int)m_aTimePos[0].size(); i++)
    {
        pTPE = m_aTimePos[0][i];
        if (pTPE->m_nType == eStaffobj)
        {
            pTPE->m_xLeft += uShift;
            pTPE->m_uxAnchor += uShift;
            pTPE->m_xRight += uShift;
            pTPE->m_xFinal += uShift;
            (pTPE->m_pSO)->StoreOriginAndShiftShapes( pTPE->m_xLeft - pTPE->m_xInitialLeft);
        }
        else if (pTPE->m_nType == eOmega)
        {
            //there might be no barline.
            //TODO this was true in previous versions but, is it still true?
            if (pTPE->m_pSO)
            {
                nShiftBar = uNewStart + uNewBarSize - pTPE->m_xFinal + 1;
                pTPE->m_xLeft += nShiftBar;
                pTPE->m_uxAnchor += nShiftBar;
                pTPE->m_xFinal += nShiftBar;
                pTPE->m_xRight += nShiftBar;
                (pTPE->m_pSO)->StoreOriginAndShiftShapes(  pTPE->m_xLeft - pTPE->m_xInitialLeft );
                uBarPosition = pTPE->m_xLeft;
            }
        }
    }

    //dbg
    //GrabarTrace "Despues de RedistributeSpace:" & sCrLf & _
    //            "***************************************" & sCrLf & _
    //            Me.DumpTimeposTable & sCrLf & _
    //            Me.DumpTimeauxTable

    return uBarPosition;

}

void lmTimeposTable::SetxIni(float rTimePos, lmLUnits xPos)
{
    int i = FindItem(rTimePos);
    lmTimeposEntry* pTE = m_aTimePos[0][i];
    pTE->m_xLeft = xPos;

}

lmLUnits lmTimeposTable::GetXFinal(float rTimePos)
{
    int i = FindItem(rTimePos);
    lmTimeposEntry* pTE = m_aTimePos[0][i];
    return pTE->m_xRight;

}

void lmTimeposTable::SetXFinal(float rTimePos, lmLUnits xRight)
{
    int i = FindItem(rTimePos);
    lmTimeposEntry* pTE = m_aTimePos[0][i];
    pTE->m_xRight = xRight;

}

void lmTimeposTable::UpdateEntry(float rTimePos, lmLUnits xLeft, lmLUnits xRight)
{
    int i = FindItem(rTimePos);
    lmTimeposEntry* pTE = m_aTimePos[0][i];
    pTE->m_xLeft = xLeft;
    pTE->m_xRight = xRight;

}

lmLUnits lmTimeposTable::LastFinalX()
{
    //returns the final X position of the last entry
    lmTimeposEntry* pTE = m_aTimePos[0].back();
    return pTE->m_xRight;

}

wxString lmTimeposTable::DumpTimeposTable()
{
    wxString sMsg = _T("TimePos table dump. \n");
    sMsg += _T("===================================================================\n\n");

    if (m_aTimePos[0].size() == 0)
    {
        sMsg += _T("The table is empty.");
        return sMsg;
    }

    //headers
    sMsg += _T("Item    Voice  Type         TimePos    Size   xLeft xAnchor  xRight  xFinal\n");

    //loop to dump table entries
    lmTimeposEntry* pTE;
    for (int i = 0; i < (int)m_aTimePos[0].size(); i++)
    {
        if (i % 4 == 0) {
            sMsg += wxT("----------------------------------------------------------------------------\n");
        }
        pTE = m_aTimePos[0][i];
        sMsg += wxString::Format(_T("%4d: %8d"), i, pTE->m_nVoice);
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
    sMsg += _T("Item    Voice  Ref     TimePos    Size   xLeft xAnchor  xRight  xFinal\n");

    //loop to dump table entries
    int iItem;
    float rTimePrev;
    lmTimeauxEntry* pTAE;
    lmTimeposEntry* pTPE;

    rTimePrev = -1;
    for (int i = 0; i < (int)m_aTimeAux.size(); i++)
    {
        pTAE = m_aTimeAux[i];
        if (!IsEqualTime(rTimePrev, pTAE->rTimePos))
        {
            rTimePrev = pTAE->rTimePos;
            sMsg += _T("----------------------------------------------------------------------------\n");
        }
        sMsg += wxString::Format( _T("%4d: %8d %4d %11.2f "),
            i, pTAE->nVoice, pTAE->nItem, pTAE->rTimePos);
        iItem = pTAE->nItem;

        pTPE = m_aTimePos[0][iItem];
        sMsg += wxString::Format( _T("%.2f %.2f %.2f %.2f %.2f\n"),
            pTPE->m_uSize, pTPE->m_xLeft, pTPE->m_uxAnchor, pTPE->m_xRight, pTPE->m_xFinal);
    }

    return sMsg;

}


//=====================================================================================
//private methods
//=====================================================================================

void lmTimeposTable::AddTimeAuxEntry(int nItem)
{
    //Add a new entry to the auxiliary Timeaux table so that the table is ordered
    //by timepos (ascending). Parameter nItemis the index over Timepos table pointing
    //to the item to add to the Tiemaux table.

    //the table must be ordered by timepos

    lmTimeposEntry* pTPE = m_aTimePos[0][nItem];
    if (pTPE->m_rTimePos < 0) return;            //discard non timed entries

    // Create entry to be added
    lmTimeauxEntry* pNew = new lmTimeauxEntry(nItem, pTPE->m_nVoice, pTPE->m_rTimePos);

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
            wxASSERT(i > 0);
            //m_aTimeAux.Insert(pNew, (size_t)i);
			//insert before entry number i
			std::vector<lmTimeauxEntry*>::iterator it =
				std::find(m_aTimeAux.begin(), m_aTimeAux.end(), m_aTimeAux[i]);
			m_aTimeAux.insert(it, pNew);
        }
    }

}

int lmTimeposTable::FindItem(float rTimePos)
{
    wxASSERT(m_aTimePos[0].size() > 0);     //table empty !!
    //wxASSERT(rTimePos <= (m_aTimePos[0].back())->m_rTimePos );  //not in table !!

    //look up in table
    lmTimeposEntry* pEntry;
    for (int i = 0; i < (int)m_aTimePos[0].size(); i++)
    {
        pEntry = m_aTimePos[0][i];
        if (IsEqualTime(rTimePos, pEntry->m_rTimePos))
        {
            //found. return this index
            return i;
        }
    }
    wxASSERT(false);    //not in table !!
    return 0;            //let the compiler be happy.

}

void lmTimeposTable::NewEntry(int nVoice, eTimeposEntryType nType, float rTimePos,
							  lmStaffObj* pSO)
{
    lmTimeposEntry* pEntry = new lmTimeposEntry(nType, nVoice, pSO, rTimePos);
    m_aTimePos[0].push_back(pEntry);

}



