// RCS-ID: $Id: TimeposTable.cpp,v 1.3 2006/02/23 19:24:42 cecilios Exp $
//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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
/*! @file TimeposTable.cpp
    @brief Implementation file for class lmTimeposTable
    @ingroup score_kernel
*/
/*! @class lmTimeposTable
    @ingroup score_kernel
    @brief Table and management algoritms to compute the positioning data for ScoreObjs.

    Encapsulation of the table and of management algoritms to compute the positioning
    data for each lmStaffObj, when a bar column must be rendered.
*/
#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Score.h"
#include "wx/debug.h"
#include "TimeposTable.h"


//=====================================================================================
//public methods
//=====================================================================================

lmTimeposTable::lmTimeposTable()
{
    m_nCurThread = 0;
}

lmTimeposTable::~lmTimeposTable()
{
    WX_CLEAR_ARRAY(m_aTimeAux);
    WX_CLEAR_ARRAY(m_aTimePos);
}

void lmTimeposTable::CleanTable()
{
    /*
    This method must be invoked before using the table. Can be also invoked as
    many times as desired to clean the table and reuse it.
    */
    WX_CLEAR_ARRAY(m_aTimeAux);
    WX_CLEAR_ARRAY(m_aTimePos);
    m_aTimePos.Empty();
    m_aTimeAux.Empty();
    m_nCurThread = 0;
    
}

void lmTimeposTable::NewThread()
{
    //start a new thread (increment counter and add alfa entry)

    m_nCurThread++;
    NewEntry(eAlfa, -1, (lmStaffObj*)NULL);
    
    //the start xpos has been set to zero in NewEntry method
    //set the start xpos to the same value than in the previous thread
    if (m_nCurThread > 1) {
        //find alfa entry of previous thread
        int i = m_aTimePos.GetCount() - 2;      //point to last entry (the previous to this new one)
        lmTimeposEntry* pEntry = m_aTimePos[i];
        for (; pEntry->m_nType != eAlfa; i--) {
            wxASSERT(i >= 0);
            pEntry = m_aTimePos[i];
        }
        
        //here i points to the alfa entry. Copy its xpos
        SetCurXLeft( pEntry->m_xLeft );
    }
    
}

void lmTimeposTable::CloseThread(int rCurX)
{
    //rCurX = current canvas position
    wxASSERT(m_nCurThread > 0);       //there must be a thread
    
    NewEntry(eOmega, -1, (lmStaffObj*)NULL);
    SetCurXLeft(rCurX);
    
}

void lmTimeposTable::AddEntry(float rTimePos, lmStaffObj* pSO)
{
    NewEntry(eStaffobj, rTimePos, pSO);
}

void lmTimeposTable::AddBarline(lmStaffObj* pSO)
{
    /*
    There might be many barline StaffObjs for this bar column, one for each bar in
    each staff. So, in this method the Omega entry of each thread is associated to
    the barline object
    */

    lmTimeposEntry* pEntry = m_aTimePos[m_aTimePos.GetCount() - 1];
    wxASSERT(pEntry->m_nType == eOmega);
    pEntry->m_pSO = pSO;
    pEntry->m_rTimePos = pSO->GetTimePos();

}

//-------------------------------------------------------------------------------------
// methods for accesing/updating current entry
//-------------------------------------------------------------------------------------

void lmTimeposTable::SetCurXLeft(int nValue)
{
    lmTimeposEntry* pEntry = m_aTimePos[m_aTimePos.GetCount() - 1];
    pEntry->m_xLeft = nValue;
    if (pEntry->m_xAnchor == 0) {
        pEntry->m_xAnchor = nValue;   //default value
    }
    if (pEntry->m_nType == eStaffobj) {
        pEntry->m_nSize = pEntry->m_xFinal - pEntry->m_xLeft;
    }
    
}

int lmTimeposTable::GetCurXLeft()
{
    lmTimeposEntry* pEntry = m_aTimePos[m_aTimePos.GetCount() - 1];
    return pEntry->m_xLeft;
}

void lmTimeposTable::SetCurXFinal(int nValue)
{
    lmTimeposEntry* pEntry = m_aTimePos[m_aTimePos.GetCount() - 1];
    pEntry->m_xFinal = nValue;
    pEntry->m_nSize = pEntry->m_xFinal - pEntry->m_xLeft;
    
}

void lmTimeposTable::SetCurXAnchor(int nValue)
{
    lmTimeposEntry* pEntry = m_aTimePos[m_aTimePos.GetCount() - 1];
    pEntry->m_xAnchor = nValue;
    
}

//-------------------------------------------------------------------------------------
//  methods to compute results
//-------------------------------------------------------------------------------------

int lmTimeposTable::GetGrossBarSize()
{
    //returns the maximum size of all threads
    
    int rxIni, xFinal;
    
    //the bar starts at the xLeft of the alfa entry
    rxIni = GetStartOfBarPosition();
    
    //now compute the maximum xFinal
    lmTimeposEntry* pEntry = m_aTimePos[0];    //the compiler complains if not initialized !!
    xFinal = 0;
    for (int i = 0; i < (int)m_aTimePos.GetCount(); i++) {
        pEntry = m_aTimePos[i];
        xFinal = wxMax(pEntry->m_xFinal, xFinal);
    }
            
    return xFinal - rxIni;
    
}

int lmTimeposTable::GetStartOfBarPosition()
{
    //returns the x position for the start of the bar column

    //the bar starts at the xLeft of the alfa entry
    lmTimeposEntry* pEntry = m_aTimePos[0];    //the alfa entry
    wxASSERT(pEntry->m_nType == eAlfa);
    return pEntry->m_xLeft;
    
}

lmLUnits lmTimeposTable::ArrangeStaffobjsByTime(bool fTrace)
{
    /*
    Pentobjs positions are arranged so that all StaffObjs that must sound at the same time
    will have the same x anchor coordinate.
    The new positions are not stored in the StaffObjs but only in this object tables.
    This method returns the bar column size (including the width of the barline at the end)
    */

    //build timeaux table. Ordered by time
    //-------------------------------------
    int i;
    m_aTimeAux.Empty();
    for (i = 0; i < (int)m_aTimePos.GetCount(); i++) {
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
    int nThread;            //auxiliary. The thread in process
    float rTime;            //auxiliary. The time value in process
    lmLUnits nShift;        //auxiliary. x shift to apply
    int nMinStartPos;        //minimum start position for StaffObjs of current time value
    //As the position for a time value must be greater than the maximum final position of
    //the StaffObjs located at the previous time value we have to:
    // 1. compute the maximum final for each time value, and
    // 2. ensure that the start position for the next time mark is greater than this
    int nMaxFinalPrev;        //maximum final position for previous time mark
    int nMaxFinalCur;        //maximum final position for current time mark
    int nMaxAnchorCur;        //maximum anchor position for current time mark
    nMaxFinalPrev = 0;
    
    int iNext, iFirst;
    int iItem;
    lmTimeposEntry* pTPE;
    lmTimeauxEntry* pTAE;
    iFirst = 0;        //index to the first table entry for current time value
    iNext = 0;        //index to the first entry for the next time value
    
    while (iNext < (int)m_aTimeAux.GetCount())
    {
        i = iFirst;
        pTAE = m_aTimeAux[iFirst];
        rTime = pTAE->rTimePos;        //time to process
        nMinStartPos = nMaxFinalPrev;
        
        //loop to compute the maximum left position of all entries of current time
        while (i < (int)m_aTimeAux.GetCount())
        {
            pTAE = m_aTimeAux[i];
            if (pTAE->rTimePos != rTime) break;
            iItem = pTAE->nItem;
            pTPE = m_aTimePos[iItem];
            nMinStartPos = wxMax(nMinStartPos, pTPE->m_xLeft);
            i++;
        }
        iNext = i;       //save start index for next cicle of main loop
        ///*dbg*/    wxLogMessage(wxString::Format(
        //            _T("[lmTimeposTable::ArrangeStaffobjsByTime] nMinStartPos=%d "),
        //            nMinStartPos ));
       
        //loop to compute the shift to apply in order to align left positions, and
        //the max. anchor position
        nMaxAnchorCur = 0;
        i = iFirst;
        while (i < (int)m_aTimeAux.GetCount())
        {
            pTAE = m_aTimeAux[i];
            if (pTAE->rTimePos != rTime) break;
            iItem = pTAE->nItem;
            pTPE = m_aTimePos[iItem];
            pTAE->nShift = nMinStartPos - pTPE->m_xLeft;
            nMaxAnchorCur = wxMax(nMaxAnchorCur, pTAE->nShift + pTPE->m_xAnchor);
            i++;
        }
        ///*dbg*/    wxLogMessage(wxString::Format(
        //            _T("[lmTimeposTable::ArrangeStaffobjsByTime] nMaxAnchorCur=%d "),
        //            nMaxAnchorCur ));
        
        //loop to compute the final shift to apply in order to align by anchor positions
        i = iFirst;
        while (i < (int)m_aTimeAux.GetCount())
        {
            pTAE = m_aTimeAux[i];
            if (pTAE->rTimePos != rTime) break;
            iItem = pTAE->nItem;
            pTPE = m_aTimePos[iItem];
            pTAE->nShift = nMaxAnchorCur - pTPE->m_xAnchor;
            i++;
        }
        
        //shift the start position of all StaffObjs of current time (and those following
        //them of the same thread with no specific time) and
        //compute the the maximum final position for current time
        nMaxFinalCur = 0;
        i = iFirst;
        while (i < (int)m_aTimeAux.GetCount())
        {
            //process one entry of Time table
            pTAE = m_aTimeAux[i];
            if (pTAE->rTimePos != rTime) break;
            iItem = pTAE->nItem;
            nShift = pTAE->nShift;

            //process this entry at TimePos table
        ///*dbg*/    wxLogMessage(wxString::Format(
        //            _T("[lmTimeposTable::ArrangeStaffobjsByTime] applying shift (%d) to entry %d. rTime=%.2f "),
        //            nShift, i, pTAE->rTimePos ));
            pTPE = m_aTimePos[iItem];
            pTPE->m_xFinal += nShift;
            pTPE->m_xAnchor += nShift;
            pTPE->m_xLeft += nShift;
            pTPE->m_xRight += nShift;
            nMaxFinalCur = wxMax(nMaxFinalCur, pTPE->m_xFinal);
            nThread = pTPE->m_nThread;

            //process any following entry of this thread with no specific time (timepos=-1)
            iItem++;
            while (iItem < (int)m_aTimePos.GetCount())
            {
                pTPE = m_aTimePos[iItem];
                if (pTPE->m_nThread != nThread || pTPE->m_rTimePos != -1 ) break;

                pTPE->m_xFinal += nShift;
                pTPE->m_xAnchor += nShift;
                pTPE->m_xLeft += nShift;
                pTPE->m_xRight += nShift;
                nMaxFinalCur = wxMax(nMaxFinalCur, pTPE->m_xFinal);
                iItem++;
            }
            
            i++;
            
        }
        
        //at this point StaffObjs at current time value StaffObjs and their inmediate
        //following entries with no specific tiem are aligned by its xAnchor position.
        //The maximum final position of all these entries is storet in nMaxFinalCur
        
        //if trace requested dump tables to log
        if (fTrace) {
            wxLogMessage( wxString::Format(
                _T("TimepoTable: arrange loop. iFirst=%d, nMaxFinalCur=%d"),
                iFirst, nMaxFinalCur) );
            wxLogMessage(_T("***************************************\n"));
            wxLogMessage( DumpTimeposTable() );
            wxLogMessage( DumpTimeauxTable() );
        }
    
        //the current time StaffObjs are aligned. Advance to next time value
        iFirst = iNext;
        nMaxFinalPrev = nMaxFinalCur;
        
    }
    
    //return the measure column size
    lmTimeposEntry* pEnd = m_aTimePos[m_aTimePos.GetCount() - 1];
    lmTimeposEntry* pStart = m_aTimePos[0];
    lmLUnits nColumnSize = pEnd->m_xFinal - pStart->m_xLeft;

    //if trace requested dump tables to log
    if (fTrace) {
        wxLogMessage( wxString::Format(
            _T("TimepoTable: After arranging. MeasureWidth=%d"),
            nColumnSize) );
        wxLogMessage(_T("***************************************\n"));
        wxLogMessage( DumpTimeposTable() );
        wxLogMessage( DumpTimeauxTable() );
    }

    return nColumnSize;
    
}

int lmTimeposTable::RedistributeSpace(int nNewBarSize, int nNewStart)
{
    /*
    Shift the position of all StaffObjs by the amount given by the difference between current
    bar start position and the new desired start position.
    In addition, the position of the barline at the end of this bar is also shifted so that
    the new width on the bar becomes nNewBarWidth.
    
    Parameters:
       nNewBarWidth - the new width that this bar will have.
       nNewStart - the new left position for the start of this bar
    
    Results and return value:
       The new positions are stored in the StaffObjs
       The position of the barline at the end of this bar is retuned.

    */
    
    /*! @todo
        The actual process does not distribute space evenly across the bar. It just
        moves the barline so that the bar size get changed; the added space all remains
        at the end, before the barline.
    */
    
    int nBarPosition = 0;
    int nShift, nShiftBar;
    lmTimeposEntry* pTPE = m_aTimePos[0];
    nShift = nNewStart - pTPE->m_xLeft;
    
    //Update table and store the new x positions into the StaffObjs
    for (int i = 0; i < (int)m_aTimePos.GetCount(); i++)
    {
        pTPE = m_aTimePos[i];
        if (pTPE->m_nType == eStaffobj)
        {
            pTPE->m_xLeft += nShift;
            pTPE->m_xAnchor += nShift;
            pTPE->m_xRight += nShift;
            pTPE->m_xFinal += nShift;
            (pTPE->m_pSO)->SetLeft( pTPE->m_xLeft );
        }
        else if (pTPE->m_nType == eOmega)
        {
            //there might be no barline.
            //! @todo this was true in previous versions but, is it still true?
            if (pTPE->m_pSO)
            {
                nShiftBar = nNewStart + nNewBarSize - pTPE->m_xFinal + 1;
                pTPE->m_xLeft += nShiftBar;
                pTPE->m_xAnchor += nShiftBar;
                pTPE->m_xFinal += nShiftBar;
                pTPE->m_xRight += nShiftBar;
                (pTPE->m_pSO)->SetLeft( pTPE->m_xLeft );
                nBarPosition = pTPE->m_xLeft;
            }
        }
    }
    
    //dbg
    //GrabarTrace "Despues de RedistributeSpace:" & sCrLf & _
    //            "***************************************" & sCrLf & _
    //            Me.DumpTimeposTable & sCrLf & _
    //            Me.DumpTimeauxTable

    return nBarPosition;
    
}

void lmTimeposTable::SetxIni(float rTimePos, int xPos)
{        
    int i = FindItem(rTimePos);
    lmTimeposEntry* pTE = m_aTimePos[i];
    pTE->m_xLeft = xPos;
    
}

int lmTimeposTable::GetXFinal(float rTimePos)
{        
    int i = FindItem(rTimePos);
    lmTimeposEntry* pTE = m_aTimePos[i];
    return pTE->m_xRight;
    
}

void lmTimeposTable::SetXFinal(float rTimePos, int xRight)
{      
    int i = FindItem(rTimePos);
    lmTimeposEntry* pTE = m_aTimePos[i];
    pTE->m_xRight = xRight;
    
}

void lmTimeposTable::UpdateEntry(float rTimePos, int xLeft, int xRight)
{
    int i = FindItem(rTimePos);
    lmTimeposEntry* pTE = m_aTimePos[i];
    pTE->m_xLeft = xLeft;
    pTE->m_xRight = xRight;
    
}

int lmTimeposTable::LastFinalX()
{
    //returns the final X position of the last entry
    lmTimeposEntry* pTE = m_aTimePos[m_aTimePos.GetCount() - 1];
    return pTE->m_xRight;
    
}

wxString lmTimeposTable::DumpTimeposTable()
{
    wxString sMsg = _T("TimePos table dump. \n");
    sMsg += _T("===================================================================\n\n");

    if (m_aTimePos.GetCount() == 0)
    {
        sMsg += _T("The table is empty.");
        return sMsg;
    }
        
    //headers
    sMsg += _T("Item\tThread\tType\tTimePos\tSize\txLeft\txAnchor\txRight\txFinal\n");
            
    //loop to dump table entries
    lmTimeposEntry* pTE;
    for (int i = 0; i < (int)m_aTimePos.GetCount(); i++)
    {
        if (i % 4 == 0) {
            sMsg += wxT("----------------------------------------------------------------------------\n");
        }
        pTE = m_aTimePos[i];
        sMsg += wxString::Format(_T("%4d:\t%d\t"), i, pTE->m_nThread);
        switch (pTE->m_nType)
        {
            case eAlfa:
                sMsg += _T("\tAlfa\t");
                break;

            case eOmega:
                sMsg += _T("\tOmega ");
                if (pTE->m_pSO) {
                    sMsg += wxString::Format(_T("%d"), (pTE->m_pSO)->GetType() );    //! @todo convert to string
                } else {
                    sMsg += _T("-");
                }
                sMsg += _T("\t");
                break;

            default:
                //lmStaffObj entry
                sMsg += wxString::Format(_T("\tpSO %d\t"), (pTE->m_pSO)->GetType() );     //! @todo convert to string
        }
            
        sMsg += wxString::Format(_T("%.2f\t%d\t%d\t%d\t%d\t%d\n"),
            pTE->m_rTimePos, pTE->m_nSize, pTE->m_xLeft, pTE->m_xAnchor,
            pTE->m_xRight, pTE->m_xFinal);
    }
    
    return sMsg;

}

wxString lmTimeposTable::DumpTimeauxTable()
{
    wxString sMsg = _T("Dump of Time aux. table. \n");
    sMsg += _T("===================================================================\n\n");

    if (m_aTimeAux.GetCount() == 0)
    {
        sMsg += _T("The table is empty.");
        return sMsg;
    }
        
    //headers
    sMsg += _T("Item\tThread\tRef\tTimePos\tSize\txLeft\txAnchor\txRight\txFinal\n");
            
    //loop to dump table entries
    int iItem;
    float rTimePrev;
    lmTimeauxEntry* pTAE;
    lmTimeposEntry* pTPE;

    rTimePrev = -1;
    for (int i = 0; i < (int)m_aTimeAux.GetCount(); i++)
    {
        pTAE = m_aTimeAux[i];
        if (rTimePrev != pTAE->rTimePos)
        {
            rTimePrev = pTAE->rTimePos;
            sMsg += _T("----------------------------------------------------------------------------\n");
        }
        sMsg += wxString::Format( _T("%4d:\t%d\t%d\t%.2f\t"), 
            i, pTAE->nThread, pTAE->nItem, pTAE->rTimePos);
        iItem = pTAE->nItem;
        
        pTPE = m_aTimePos[iItem];
        sMsg += wxString::Format( _T("%d\t%d\t%d\t%d\t%d\n"),
            pTPE->m_nSize, pTPE->m_xLeft, pTPE->m_xAnchor, pTPE->m_xRight, pTPE->m_xFinal);
    }
        
    return sMsg;

}


//=====================================================================================
//private methods
//=====================================================================================

void lmTimeposTable::AddTimeAuxEntry(int nItem)
{
    /*
    Add a new entry to the auxiliary Timeaux table so that the table is ordered
    by timepos (ascending). Parameter nItemis the index over Timepos table pointing
    to the item to add to the Tiemaux table.
    */

    //the table must be ordered by timepos

    lmTimeposEntry* pTPE = m_aTimePos[nItem];
    if (pTPE->m_rTimePos < 0) return;            //discard non timed entries
    
    // Create entry to be added
    lmTimeauxEntry* pNew = new lmTimeauxEntry(nItem, pTPE->m_nThread, pTPE->m_rTimePos);

    //if there are no entries in Timeaux table or the timePos of last entry is lower
    //or equal than the timepos to be inserted , just create the new entry at the end 
    //of the table
    if (m_aTimeAux.GetCount() == 0)
    {
        //No entries in table. This one is the first one.
        m_aTimeAux.Add(pNew);
    }
    else
    {
        int i = m_aTimeAux.GetCount() - 1;    //index to last entry
        lmTimeauxEntry* pTAE = m_aTimeAux[i];
        if (pTAE->rTimePos <= pTPE->m_rTimePos)
        {
            //all existing entries in Timeaux table have timepos lower or equal than the
            //new one. Add it at the end.
            m_aTimeAux.Add(pNew);
        }
        else
        {
            //the new entry must be inserted between two existing entries.

            //locate insertion point
            for (i=0; i < (int)m_aTimeAux.GetCount(); i++) {
                pTAE = m_aTimeAux[i];
                if (pTAE->rTimePos > pTPE->m_rTimePos) break;
            }
            wxASSERT(i > 0);    
            m_aTimeAux.Insert(pNew, (size_t)i);    //insert before entry number i
        }
    }

}

int lmTimeposTable::FindItem(float rTimePos)
{
    wxASSERT(m_aTimePos.GetCount() > 0);     //table empty !!
    wxASSERT(rTimePos <= (m_aTimePos[m_aTimePos.GetCount() - 1])->m_rTimePos );  //not in table !!
    
    //look up in table
    lmTimeposEntry* pEntry;
    for (int i = 0; i < (int)m_aTimePos.GetCount(); i++)
    {
        pEntry = m_aTimePos[i];
        if (rTimePos == pEntry->m_rTimePos)
        {
            //found. return this index
            return i;
        }
    }
    wxASSERT(false);    //not in table !!
    return 0;            //let the compiler be happy.
    
}

void lmTimeposTable::NewEntry(eTimeposEntryType nType, float rTimePos, lmStaffObj* pSO)
{
    lmTimeposEntry* pEntry = new lmTimeposEntry(nType, m_nCurThread, pSO, rTimePos);
    m_aTimePos.Add(pEntry);

}



