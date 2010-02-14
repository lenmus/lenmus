//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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

// Encapsulation of the table and management algoritms to compute the positioning
// data for each lmStaffObj, when a bar column must be rendered.

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "SystemNewFormat.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <vector>
#include <list>
#include <algorithm>
#include <math.h>

#include <wx/debug.h>

#include "../score/Score.h"
#include "../score/Staff.h"
#include "../score/VStaff.h"
#include "../score/Spacer.h"
#include "SystemNewFormat.h"
#include "ShapeNote.h"
#include "BoxSlice.h"

//access to logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;

#if defined(__WXDEBUG__)

#include <wx/file.h>

// access to paths
#include "../globals/Paths.h"
extern lmPaths* g_pPaths;

#endif

class lmBreaksTable;

#define lmDUMP_TABLES   0
#define lmTRACE_CRITICAL_LINE 0

//typedef struct
//{
//} lmLayoutOptions;
//
//const m_LayoutOpt =

//spacing function parameters
//-----------------------------------------------
    //TODO: User options
const float lmDMIN = 8.0f;				//Dmin: min. duration to consider
const lmTenths lmMIN_SPACE = 10.0f;		//Smin: space for Dmin
const lmTenths lmSPACE_AFTER_PROLOG = 25.0f;
const lmTenths lmSPACE_AFTER_INTERMEDIATE_CLEF = 20.0f;
const lmTenths lmMIN_SPACE_BETWEEN_NOTE_AND_CLEF = 10.0f;
const lmTenths lmEXCEPTIONAL_MIN_SPACE = 2.5f;

//The first note in each bar should be about one note-head's width away from the barline.

////=====================================================================================
////lmCriticalLine definition: class to implement the critical line
////  Its entries are owned by the respective lines. DO NOT CHANGE THEM
////=====================================================================================
//
//class lmCriticalLine
//{
//public:
//    lmCriticalLine(lmColumnFormatter* pColFmt);
//    ~lmCriticalLine();
//
//    void AddNonTimed(lmLineFormatter* pLineFmt, float rTime);
//    void AddEntry(lmLineEntry* pEntry);
//    lmLUnits RedistributeSpace(lmLUnits uNewBarSize, lmLUnits uNewStart,
//                               lmLUnits uOldBarSize, lmBoxSlice* pBSlice);
//
//    void InitializeToGetTimepos();
//    lmLUnits GetTimepos(float rTime);
//    lmLUnits GetPosForStaffobj(lmStaffObj* pSO, float rTime);
//    wxString DumpPosTimes();
//
//    //from lmLineFormatter
//    wxString DumpMainTable() { return wxEmptyString;  };
//    inline wxString DumpTimePosTable() { return wxEmptyString;  } //m_pColFmt->DumpColumnData(); }
//    inline lmLineEntry* GetLastEntry() { return m_pLineTable->GetLastEntry(); }
//
//protected:
//    lmLineTable*   m_pLineTable;       //storage for this line
//
//    //table of positions and timepos
//    typedef struct lmPosTimeEntry_Struct
//    {
//        lmLineEntry*    pTPE;
//        float           rTimepos;
//        lmLUnits        uxPos;
//    }
//    lmPosTimeEntry;
//
//    std::vector<lmPosTimeEntry> m_PosTimes;
//
//    //to traverse the table by timepos
//    float   m_rLastTimepos;
//    std::vector<lmPosTimeEntry>::iterator   m_it;
//
//};


//=====================================================================================
//lmBreaksTable definition: table to contain possible break points
//=====================================================================================

//an entry of the BreaksTable
typedef struct lmBeaksTimeEntry_Struct
{
    float       rTimepos;
    float       rPriority;
    lmLUnits    uxStart;
    lmLUnits    uxEnd;
    bool        fInBeam;
    lmLUnits    uxBeam;
}
lmBeaksTimeEntry;

//the breaks table
class lmBreaksTable
{
public:
    lmBreaksTable();
    ~lmBreaksTable();

    void AddEntry(float rTime, lmLUnits uxStart, lmLUnits uWidth, bool fInBeam,
                  lmLUnits uxBeam, float rPriority = 0.8f);
    void AddEntry(lmBeaksTimeEntry* pBTE);
    void SetPriority();
    void ChangePriority(int iEntry, float rMultiplier);
    wxString Dump();
    inline bool IsEmpty() { return m_BreaksTable.empty(); }

    //traversing the table
    lmBeaksTimeEntry* GetFirst();
    lmBeaksTimeEntry* GetNext();


private:

    std::list<lmBeaksTimeEntry*>               m_BreaksTable;      //the table
    std::list<lmBeaksTimeEntry*>::iterator     m_it;               //for GetFirst(), GetNext();


};



//=====================================================================================
//lmBreaksTable implementation
//=====================================================================================


lmBreaksTable::lmBreaksTable()
{
}

lmBreaksTable::~lmBreaksTable()
{
    std::list<lmBeaksTimeEntry*>::iterator it;
    for (it = m_BreaksTable.begin(); it != m_BreaksTable.end(); ++it)
        delete *it;
    m_BreaksTable.clear();
}

void lmBreaksTable::AddEntry(float rTime, lmLUnits uxStart, lmLUnits uWidth, bool fInBeam,
                             lmLUnits uxBeam, float rPriority)
{
    lmBeaksTimeEntry* pBTE = new lmBeaksTimeEntry;
    pBTE->rPriority = rPriority;
    pBTE->rTimepos = rTime;
    pBTE->uxStart = uxStart;
    pBTE->uxEnd = uxStart + uWidth;
    pBTE->fInBeam = fInBeam;
    pBTE->uxBeam = uxBeam;

    m_BreaksTable.push_back(pBTE);
}

void lmBreaksTable::AddEntry(lmBeaksTimeEntry* pBTE)
{
    AddEntry(pBTE->rTimepos, pBTE->uxStart, pBTE->uxEnd - pBTE->uxStart, pBTE->fInBeam,
             pBTE->uxBeam, pBTE->rPriority);
}

void lmBreaksTable::ChangePriority(int iEntry, float rMultiplier)
{
}

wxString lmBreaksTable::Dump()
{
    wxString sMsg = _T("Breaks table\n");
    sMsg += _T("===================================================================\n\n");

    if (m_BreaksTable.size() == 0)
    {
        sMsg += _T("The table is empty.");
        return sMsg;
    }

    //          +....... +....... +....... +....... +..... +.......
    sMsg += _T("Piority  ColumnFormatter  xStart   xEnd     InBeam xBeam\n");
    sMsg += _T("-------- -------- -------- -------- ------ --------\n");
    std::list<lmBeaksTimeEntry*>::iterator it;
    for (it = m_BreaksTable.begin(); it != m_BreaksTable.end(); ++it)
    {
        sMsg += wxString::Format(_T("%8.2f %8.2f %8.2f %8.2f %6s %8.2f\n"),
                    (*it)->rPriority, (*it)->rTimepos, (*it)->uxStart, (*it)->uxEnd,
                    ((*it)->fInBeam ? _T("yes   ") : _T("no    ")), (*it)->uxBeam );
    }
    return sMsg;
}

lmBeaksTimeEntry* lmBreaksTable::GetFirst()
{
    m_it = m_BreaksTable.begin();
    if (m_it == m_BreaksTable.end())
        return (lmBeaksTimeEntry*)NULL;

    return *m_it;
}

lmBeaksTimeEntry* lmBreaksTable::GetNext()
{
    //advance to next one
    ++m_it;
    if (m_it != m_BreaksTable.end())
        return *m_it;

    //no more items
    return (lmBeaksTimeEntry*)NULL;
}






//=====================================================================================
//lmLineEntry implementation
//=====================================================================================

lmLineEntry::lmLineEntry(lmEEntryType nType, lmStaffObj* pSO, lmShape* pShape,
                         bool fProlog)
    : m_nType(nType)
    , m_pSO(pSO)
    , m_pShape(pShape)
	, m_fProlog(fProlog)
    , m_xFinal(0.0f)
    , m_uFixedSpace(0.0f)
    , m_uVariableSpace(0.0f)
    , m_rTimePos((pSO && pSO->IsAligned()) ? pSO->GetTimePos() : -1.0f )
    , m_uSize(pShape ? pShape->GetWidth() : 0.0f )
    , m_xLeft(pShape ? pShape->GetXLeft() : 0.0f )
    , m_uxAnchor((pSO && pSO->IsNoteRest()) ? - pSO->GetAnchorPos(): 0.0f )
{
    m_xInitialLeft = m_xLeft;
}

lmLineEntry::lmLineEntry(lmLineEntry* pEntry)
    : m_nType(pEntry->m_nType)
    , m_pSO(pEntry->m_pSO)
    , m_pShape(pEntry->m_pShape)
	, m_fProlog(pEntry->m_fProlog)
    , m_rTimePos(pEntry->m_rTimePos)
    , m_xInitialLeft(pEntry->m_xInitialLeft)
    , m_xLeft(pEntry->m_xLeft)
    , m_uxAnchor(pEntry->m_uxAnchor)
    , m_xFinal(pEntry->m_xFinal)
    , m_uSize(pEntry->m_uSize)
    , m_uTotalSize(pEntry->m_uTotalSize)
    , m_uFixedSpace(pEntry->m_uFixedSpace)
    , m_uVariableSpace(pEntry->m_uVariableSpace)
{
}

void lmLineEntry::AssignFixedAndVariableSpace(lmColumnFormatter* pTT, float rFactor)
{
	//assign fixed and variable after spaces to this object and compute the xFinal pos

    if (m_nType == lm_eOmega)
    {
		if (m_pSO && m_pSO->IsBarline())    //if exists it must be a barline !!
            ;
        else
            m_uSize = 0.0f;
    }
    else
    {
		if (!m_pSO->IsVisible())
		{
			m_uSize = 0.0f;
            m_uFixedSpace = 0.0f;
            m_uVariableSpace = 0.0f;
		}
		else
		{
			if (m_pSO->IsNoteRest())
			{
				SetNoteRestSpace(pTT, rFactor);
			}
			else if (m_pSO->IsClef()
                     || m_pSO->IsKeySignature()
                     || m_pSO->IsTimeSignature()
                    )
			{
                m_uFixedSpace = pTT->TenthsToLogical(lmEXCEPTIONAL_MIN_SPACE, 1);
                m_uVariableSpace = pTT->TenthsToLogical(lmMIN_SPACE, 1) - m_uFixedSpace;
			}
			else if (m_pSO->IsSpacer() || m_pSO->IsScoreAnchor())
			{
                m_uFixedSpace = 0.0f;
                m_uVariableSpace = m_uSize;
			}
			else if (m_pSO->IsFiguredBass())
			{
                m_uFixedSpace = 0.0f;
                m_uVariableSpace = 0.0f;
			}
			else
                m_uSize = 0.0f;
		}
    }

    //compute final position
    m_xFinal = m_xLeft + GetTotalSize();
}

void lmLineEntry::SetNoteRestSpace(lmColumnFormatter* pTT, float rFactor)
{
	static const float rLog2 = 0.3010299956640f;		// log(2)

	int iStaff = m_pSO->GetStaffNum();
    lmLUnits uTotalSize;
    if (pTT->SpacingMethod() == esm_PropConstantFixed)
    {
        //proportional constant spacing.
        float rVar = log(((lmNoteRest*)m_pSO)->GetDuration() / lmDMIN) / rLog2;

	    //spacing function
	    uTotalSize = pTT->TenthsToLogical(lmMIN_SPACE, iStaff);		//Space(Di) = Smin
	    if (rVar > 0.0f) {
		    rVar *= uTotalSize;         //rVar = Smin*log2(Di/Dmin)
            rVar *= rFactor;            //rVar = Smin*[A*log2(Di/Dmin)]
		    uTotalSize += rVar;			//Space(Di) = Smin*[1 + A*log2(Di/Dmin)]
	    }
    }
    else if (pTT->SpacingMethod() == esm_Fixed)
    {
        // fixed spacing
        uTotalSize = pTT->TenthsToLogical(pTT->FixedSpacingValue(), iStaff);
    }
    else
        wxASSERT(false);

    //setup minimal space (fixed space) and compute variable space
    m_uFixedSpace = pTT->TenthsToLogical(lmEXCEPTIONAL_MIN_SPACE, 1);
    m_uVariableSpace = uTotalSize - m_uSize - m_uFixedSpace - m_uxAnchor;

    //variable space can not be negative
    if (m_uVariableSpace < 0)
        m_uVariableSpace = 0.0f;
}

void lmLineEntry::Reposition(lmLUnits uxPos)
{
	//reposition Shape
    //wxLogMessage(_T("Reposition: old xLeft=%.2f, new xLeft=%.2f"), m_xInitialLeft, uxPos);
    lmLUnits uShift = uxPos - m_xInitialLeft;
    if (m_pSO && m_pShape)
        m_pSO->StoreOriginAndShiftShapes( uShift, m_pShape->GetOwnerIDX() );

	//update entry data
	m_xLeft = uxPos;
	m_xInitialLeft = m_xLeft;
	m_xFinal = uxPos + GetTotalSize();
}

void lmLineEntry::RepositionAt(lmLUnits uxNewXLeft)
{
    //reposition current entry. Shapes are not moved, just table data

    lmLUnits uShift = uxNewXLeft - m_xLeft;
    m_xLeft = uxNewXLeft;
    m_xFinal = m_xLeft + GetTotalSize();
}

wxString lmLineEntry::DumpHeader()
{
    //         ...+  ..+   ...+ ..+   +  ..........+........+........+........+........+........+........+........+........+......+
    return _T("Item    Type      ID Prolog   Timepos    xInit  xAnchor    xLeft     Size    Space   xFinal  SpFixed   SpVar ShpIdx\n");
}

wxString lmLineEntry::Dump(int iEntry)
{
    wxString sMsg = wxString::Format(_T("%4d: "), iEntry);
    switch (m_nType)
    {
        //case lm_eAlfa:
        //    sMsg += _T("  Alfa              ");
        //    break;

        case lm_eOmega:
            sMsg += _T("  Omega");
            if (m_pSO) {
                sMsg += wxString::Format(_T("%3d          "), m_pSO->GetScoreObjType() );
            } else {
                sMsg += _T("  -          ");
            }
            break;

        default:
            //lmStaffObj entry
			sMsg += wxString::Format(_T("  pSO %4d %3d   %s  "),
									m_pSO->GetScoreObjType(),
									m_pSO->GetID(),
									(m_fProlog ? _T("S") : _T(" ")) );
    }

    sMsg += wxString::Format(_T("%11.2f %8.2f %8.2f %8.2f %8.2f %8.2f %8.2f %8.2f %8.2f"),
                m_rTimePos, m_xInitialLeft, m_uxAnchor, m_xLeft,
		        m_uSize, GetTotalSize(), m_xFinal,
                m_uFixedSpace, m_uVariableSpace);

    if (m_pShape)
        sMsg += wxString::Format(_T("  %4d\n"), m_pShape->GetOwnerIDX());
    else
        sMsg += _T("    --\n");

    return sMsg;
}




//=====================================================================================
//lmColumnSplitter:
//  Algorithm to determine optimum break points to split a column 
//=====================================================================================

lmColumnSplitter::lmColumnSplitter(lmLineTable* pLineTable)
    : m_pLineTable(pLineTable)
{
}

lmColumnSplitter::~lmColumnSplitter()
{
}

void lmColumnSplitter::ComputeBreakPoints(lmBreaksTable* pBT)
{
    //This method computes the break points for this line and adds them to received
    //break points table.
    //
    //Algorithm:
    //
    //In a first approach, add an entry for each timepos at which there is an object placed.
    //Assign priority 0.8 to all entries.
    //
    //Now lower or raise priority of some entries according to empiric rules:
    //
    //  1. If there is a time signature, strongly penalize those timepos not in beat
    //     position (priority *= 0.5)
    //
    //  2. Do not split notes/rests. Penalize those entries occupied in some
    //     line (priority *= 0.7).
    //
    //  3. Do not to break beams. Penalize those entries  in which, at some line, there
    //     is a beam (priority *=0.9).
    //
    //Finally, when all priorities have been computed, sort the table by priority (high to
    //low) and by space (max to min).
    //
    //In order to accelerate the computation of this table, LineTables must have all
    //necesary data so that it doesn't become necessary to traverse the StaffObjs
    //colection again.

    //TODO: Add filters for priority
    const lmLineEntryIterator itEnd = m_pLineTable->End();
	lmLineEntryIterator it = m_pLineTable->Begin();

    //skip initial non-timed entries
	for (it = m_pLineTable->Begin(); it != itEnd && IsLowerTime((*it)->m_rTimePos, 0.0f); ++it);
    if (it == itEnd) return;

    //process current time
    float rTime = (*it)->m_rTimePos;
    lmLUnits uxStart = (*it)->m_xFinal;
    lmLUnits uxWidth = (*it)->m_uSize;
    lmLUnits uxBeam = 0.0f;
    bool fInBeam = false;
    lmStaffObj* pSO = (*it)->m_pSO;
    if (pSO && pSO->IsNoteRest() && ((lmNoteRest*)pSO)->IsBeamed())
    {
        fInBeam = true;
        lmStaffObj* pSOEnd = ((lmNoteRest*)pSO)->GetBeam()->GetEndNoteRest();
        lmShape* pShape = pSOEnd->GetShape();
        uxBeam = pShape->GetXLeft() + pShape->GetWidth();
    }

	while (it != itEnd)
    {
		if (IsEqualTime((*it)->m_rTimePos, rTime) || IsLowerTime((*it)->m_rTimePos, 0.0f))
        {
		    //skip any not-timed entry
            if (IsEqualTime((*it)->m_rTimePos, rTime))
            {
                uxWidth = wxMax(uxWidth, (*it)->m_uSize);
                lmStaffObj* pSO = (*it)->m_pSO;
                if (pSO && pSO->IsNoteRest() && ((lmNoteRest*)pSO)->IsBeamed())
                {
                    fInBeam = true;
                    lmStaffObj* pSOEnd = ((lmNoteRest*)pSO)->GetBeam()->GetEndNoteRest();
                    lmShape* pShape = pSOEnd->GetShape();
                    uxBeam = wxMax(uxBeam, pShape->GetXLeft() + pShape->GetWidth());
                }
            }
        }
        else
        {
            //new timepos. Add entry for previous timepos
            pBT->AddEntry(rTime, uxStart, uxWidth, fInBeam, uxBeam);

            //start collecting data for new timepos
            rTime = (*it)->m_rTimePos;
            uxStart = (*it)->m_xFinal;
            uxWidth = (*it)->m_uSize;
            lmStaffObj* pSO = (*it)->m_pSO;
            if (pSO && pSO->IsNoteRest() && ((lmNoteRest*)pSO)->IsBeamed())
            {
                fInBeam = true;
                lmStaffObj* pSOEnd = ((lmNoteRest*)pSO)->GetBeam()->GetEndNoteRest();
                lmShape* pShape = pSOEnd->GetShape();
                uxBeam = pShape->GetXLeft() + pShape->GetWidth();
            }
            else
            {
                uxBeam = 0.0f;
                fInBeam = false;
            }
       }
		++it;
    }

    pBT->AddEntry(rTime, uxStart, uxWidth, fInBeam, uxBeam);

    //wxLogMessage( pBT->Dump() );
}



//=====================================================================================
//lmLineFormatter:
//  An external iterator to traverse a line by timepos, and 
//  collecting information about position and timed and not timed objects
//=====================================================================================

lmLineFormatter::lmLineFormatter(lmLineTable* pTable)
    : m_pTable(pTable)
    , m_itCur(pTable->End())
    , m_rCurTime(0.0f)
	, m_uxCurPos(0.0f)
    , m_itFirstTimed(pTable->End())
    , m_itFirstObj(pTable->End())
{
}

lmLineFormatter::~lmLineFormatter()
{
}

void lmLineFormatter::StartTraversing(lmLUnits uSpaceAfterProlog)
{
    m_rCurTime = 0.0f;
    m_itCur = m_pTable->Begin();
    wxASSERT(m_itCur != m_pTable->End());       //alpha entry must exist
    m_uxCurPos = (*m_itCur)->m_xLeft + (*m_itCur)->GetTotalSize();    //alpha entry after space
    FindFirstTimed(uSpaceAfterProlog);
}

void lmLineFormatter::FindFirstTimed(lmLUnits uSpaceAfterProlog)
{
    //We are at start of current timepos.
    //Now we are going to process all non-aligned objects at current timepos to find
    //the entry for the first tmed object and determine its minimum start position.
    //If we are procesing the first bar, uSpaceAfterProlog will be > 0. In any case, we
    //are going to add the 'prolog after space'.

    bool fAddSpace = true;
    if (m_itCur != m_pTable->End())
    {
	    m_itFirstObj = ++m_itCur;

	    //iterate until we reach an aligned object or end of line
	    //This loop will process non-aligned objects (clefs, key sign, etc.)
        lmLineEntryIterator itEnd = m_pTable->End();
	    while (m_itCur != itEnd && (*m_itCur)->m_rTimePos < 0.0f)
        {
            //if the prolog (clef, time and key) is processed add some spacing before first
            //non-prolog object
            if (fAddSpace && (*m_itCur)->m_pSO &&
                    !((*m_itCur)->m_pSO->IsClef() ||
                      (*m_itCur)->m_pSO->IsKeySignature() ||
                      (*m_itCur)->m_pSO->IsTimeSignature()) )
            {
                fAddSpace = false;
		        m_uxCurPos += uSpaceAfterProlog;
            }

		    //move the shape and update the entry data
            (*m_itCur)->Reposition(m_uxCurPos);

		    //advance the width and spacing
		    m_uxCurPos += (*m_itCur)->GetTotalSize();

            ++m_itCur;
        }
    }

    //here m_itCurr is positioned at first timed entry with time >= current time, or
    //at end of table if no more timed entries.

    //Tentatively, we are going to assign this position to the first timed object
    m_itFirstTimed = m_itCur;
	if (m_itCur != m_pTable->End())
		(*m_itCur)->m_xLeft = m_uxCurPos;

    //Here:
    // - iterator m_itFirstTimed points to first timed object or to of end of line if none
    // - iterator m_itCur points to the same object than m_itFirstTimed
    // - all non-timed objects at start of line are positioned
    // - the minimun feasible position for the first timed object is asigned to it
}

lmLUnits lmLineFormatter::GetMinFeasiblePosForTime(float rTime)
{
    //Explore all aligned objects at current and return the maximum start xPos found
	//This method doesn't change m_itCur

    lmLUnits uxPos = 0.0f;
	lmLineEntryIterator it = m_itCur;
	lmLineEntryIterator itEnd = m_pTable->End();
	while (it != itEnd && IsEqualTime((*it)->m_rTimePos, rTime) )
    {
	    uxPos = wxMax(uxPos, (*it)->m_xLeft);
        ++it;
    }
    return uxPos;
}

lmLineEntry* lmLineFormatter::GetMinAlignedObjForTime(float rTime)
{
    //Explore all aligned objects at time rTime and return the entry having the
    //aligned object with minimum time duration. If there are several objects with this
    //minimal duration, returns the one occupying more space, unless it is a barline.
    //
	//When entering this method iterator m_itCur points to the next aligned entry or
	//to end iterator. This method doen't change m_itCur

    float rMinDuration = 100000000.0f;     //any too big value
    lmLUnits uMaxSpace = 0.0f;
    lmLineEntry* pEntry = (lmLineEntry*)NULL;
	lmLineEntryIterator itEnd = m_pTable->End();
	for (lmLineEntryIterator it = m_itCur; it != itEnd && IsEqualTime((*it)->m_rTimePos, rTime); ++it)
    {
        float rDuration = (*it)->m_pSO->GetTimePosIncrement();
        if (IsHigherTime(rMinDuration, rDuration))
        {
            //this object consumes less time. Select it unless it is a barline and there is
            //already an object selected
            if (rMinDuration == 100000000.0f || !(*it)->m_pSO->IsBarline())
            {
                pEntry = *it;
                uMaxSpace = (*it)->GetTotalSize();
                rMinDuration = rDuration;
            }
        }
        else if (IsEqualTime(rMinDuration, rDuration))
        {
            //this object consumes the same time than current selected one. Select it
            //if it needs more space
            lmLUnits uSpace = (*it)->GetTotalSize();
            if (uSpace > uMaxSpace)
            {
	            rMinDuration = rDuration;
                uMaxSpace = uSpace;
                pEntry = *it;
            }
        }
        else if (pEntry && (!(*it)->m_pSO->IsBarline() && pEntry->m_pSO->IsBarline()))
        {
            //There is a selected barline. But current object is not a barline. Select it
            //instead of the barline.
            pEntry = *it;
            uMaxSpace = (*it)->GetTotalSize();
            rMinDuration = rDuration;
        }
    }
    return pEntry;
}

float lmLineFormatter::AssignPositionToObjectsAtTime(float rTime,
                                    lmLUnits uxPos, float rFactor,
                                    lmTenths rSpaceAfterClef,
                                    lmTenths rMinSpaceBetweenNoteAndClef,
                                    lmColumnFormatter* pColFmt,
                                    lmLUnits* pMaxPos)
{
	//Starting at current position, explores the line.
	//Set the position of all time positioned objects placed at time rTime, until
    //we reach a time greater that rTime. The position for this next rTime object is
    //also assigned tentatively.
	//If during this traversal we find not timed objects, they are ignored and when
	//reached next rTime we go back assigning positions to them.
	//Returns the next timepos in this line, or -1 if no more entries.
    //Updates *pMaxPos with the maximum xPos reached after adding its width to
    //current pos

	//When entering this method, iterator m_itCur must point to a timed object. If it is
	//placed at time rTime, process it and all the following having the same timepos,
	//until we find the next timepos or we arrive to end of line.
	//Not-timed objects are skipped
    *pMaxPos = 0.0f;
    const lmLineEntryIterator itEnd = m_pTable->End();
	lmLineEntryIterator itLastNotTimed = itEnd;
	while (m_itCur != itEnd &&
			(IsEqualTime((*m_itCur)->m_rTimePos, rTime) || (*m_itCur)->m_rTimePos < 0.0f) )
    {
		//if not-timed take note and continue
		if ((*m_itCur)->m_rTimePos == -1.0f)
			itLastNotTimed = m_itCur;

        //else if contains a StaffObj (in the omega entry there might be no barline)
		//assign it a final position
        else if ((*m_itCur)->m_pSO)
        {
			//move the shape and update the entry data
            //FIX for 000.00.error4 [28/Dec/2008]
            //It doesn't matter: both produce the same results!
            //(*m_itCur)->Reposition(uxPos + (*m_itCur)->m_uxAnchor);
            (*m_itCur)->Reposition(uxPos);

			//compute new current position
			lmLUnits uxFinal = uxPos + (*m_itCur)->GetTotalSize();
			m_uxCurPos = wxMax(uxFinal, m_uxCurPos);
            uxFinal = uxPos + (*m_itCur)->m_uSize;
            *pMaxPos = wxMax(*pMaxPos, uxFinal);
        }
        m_itCur++;
    }

	//Here we have arrived to the next timepos or to the end of line.
    m_itFirstObj = itLastNotTimed;
    m_itFirstTimed = m_itCur;
	if (m_itCur != m_pTable->End())
	{
		//next timepos. Assign position tentatively
		(*m_itCur)->m_xLeft = m_uxCurPos;

		//go back to assign positions to not-timed objects
		if (itLastNotTimed != m_pTable->End())
		{
            lmLUnits uxCurPos = m_uxCurPos;
            lmLUnits uSpaceAfterClef =
                pColFmt->TenthsToLogical(rSpaceAfterClef,
                                         (*m_itCur)->m_pSO->GetStaffNum() );
            if ((*itLastNotTimed)->m_pSO->IsClef())
                uxCurPos -= uSpaceAfterClef;

            lmLineEntryIterator itStart = m_itCur;
            lmLineEntryIterator it = m_itCur;
            it--;
            while (it != m_pTable->Begin() && (*it)->m_rTimePos < 0.0f)
            {
                uxCurPos -= (*it)->GetTotalSize();
			    (*it)->Reposition(uxCurPos);
                it--;
            }
            //Here we have arrived to a note/rest.

            //save ptr to first non-timed object
            m_itFirstObj = it;
            ++m_itFirstObj;

            //Verify that note/rest left position is lower than the last assigned
            //position. Otherwise we have to shift the not-timed objects.
            if (it != m_pTable->Begin())
            {
                if ((*it)->m_xLeft + (*it)->m_uSize >= uxCurPos)
                {
                    //Overlap. We have to shift the not-timed objects.
                    uxCurPos = (*it)->m_xLeft + (*it)->m_uSize +
                        pColFmt->TenthsToLogical(rMinSpaceBetweenNoteAndClef,
                                                 (*it)->m_pSO->GetStaffNum() );
                    ++it;
                    while (it != itStart)
                    {
 			            (*it)->Reposition(uxCurPos);
                        uxCurPos += (*it)->GetTotalSize();
                        ++it;
                    }
		            //We have arrived to next timepos. Assign position tentatively
                    m_uxCurPos = uxCurPos;
                    if ((*itLastNotTimed)->m_pSO->IsClef())
                        m_uxCurPos += uSpaceAfterClef;
                    *pMaxPos = uxCurPos;
		            (*itStart)->m_xLeft = m_uxCurPos;
                }
            }

		}
	}
    else
    {
        //we have arrived to the end of line, but it could be an omega entry without barline.
        //In that case, it is necessary to update its information
		if (itLastNotTimed != m_pTable->End() &&
            (*itLastNotTimed)->m_nType == lm_eOmega &&
            (*itLastNotTimed)->m_rTimePos < 0.0f )
		{
		    (*itLastNotTimed)->Reposition(m_uxCurPos);
        }
    }

	//done. Return the next timepos in this line, or -1 if no more entries.
	if (m_itCur != m_pTable->End()) {
      //  wxLogMessage(_T("Timed. More entries. Next at time=%.2f ID=%d"),
					 //(*m_itCur)->m_rTimePos, (*m_itCur)->m_pSO->GetID() );
        return (*m_itCur)->m_rTimePos;
    }
    else {
        //wxLogMessage(_T("Timed. No more entries"));
        return -1.0f;
    }

}

void lmLineFormatter::FromCurrentPosAddNonTimedToLine(lmLineTable* pLineTable)
{
	lmLineEntryIterator it = m_itFirstObj;
	lmLineEntryIterator itEnd = m_pTable->End();
	while (it != m_itFirstTimed && it != itEnd)
    {
        pLineTable->PushBack(*it);
        ++it;
    }
}

lmLUnits lmLineFormatter::GetSpaceNonTimedForTime(float rTime)
{
    //returns the space required for non-timed objects placed at time rTime

    if (m_itFirstObj == m_pTable->End() ||
        ( m_itFirstTimed != m_pTable->End() 
          && !IsEqualTime((*m_itFirstTimed)->m_rTimePos, rTime)) )
        return 0.0f;        //there are no objects
    else
    {
        lmLineEntryIterator it = m_itFirstTimed;      //to point to last non-timed object
        --it;
        return (*it)->m_xFinal - (*m_itFirstObj)->m_xLeft;
    }
}

lmLUnits lmLineFormatter::GetAnchorForTime(float rTime)
{
	//When entering this method iterator m_itCur points to the next timed entry or
	//to end iterator.
	//If next entry time is rTime, returns its xAnchor position. Else returns 0

	if (m_itCur != m_pTable->End() && IsEqualTime((*m_itCur)->m_rTimePos, rTime) )
		return (*m_itCur)->m_uxAnchor;
	else
		return 0.0f;
}

lmLUnits lmLineFormatter::IntitializeSpacingAlgorithm(lmLUnits uSpaceAfterProlog)
{
    // - All non-aligned objects at timepos 0 are processed to:
    //      - assign spacing to them
    //      - determine the minimum start position for the first aligned object.
    // - This position is provisionally assigned to the first aligned object
    // - This minimum tentativally position is returned
    // - iterator m_itNote is initialized to point to first aligned object or to end of line
	//Returns the minimun feasible position for the first aligned object

    //start iterator to traverse by timepos
    //This processes all non-aligned objects at timepos 0 to determine the
    //minimum start position for the first aligned object
    StartTraversing(uSpaceAfterProlog); 

    return m_uxCurPos;
}

void lmLineFormatter::AssignSpace(lmColumnFormatter* pColFmt, float rFactor)
{
	//Explores all entries in this line and assign space to each object

	//Update table and store the new x positions into the StaffObjs
    for (lmLineEntryIterator it = m_pTable->Begin(); it != m_pTable->End(); ++it)
	{
        (*it)->AssignFixedAndVariableSpace(pColFmt, rFactor);
    }
}

//void lmLineFormatter::AddLongestNonTimedObjectsToCriticalLine(
//                                    lmCriticalLine* pCriticalLine,
//                                    float rTime)
//{
//    pCriticalLine->AddNonTimed( this, rTime );
//}






//=====================================================================================
//lmLineTable:
//  An object to encapsulate the positions table
//=====================================================================================

lmLineTable::lmLineTable(int nInstr, int nVoice, lmLUnits uxStart, lmLUnits uSpace)
    : m_nInstr(nInstr)
	, m_nVoice(nVoice)
    , m_uxLineStart(uxStart)      
    , m_uInitialSpace(uSpace)
{
}

lmLineTable::~lmLineTable()
{
    for (lmLineEntryIterator it = m_LineEntries.begin(); it != m_LineEntries.end(); ++it)
		delete *it;

    m_LineEntries.clear();
}

lmLineEntry* lmLineTable::AddEntry(lmEEntryType nType, lmStaffObj* pSO,
                                          lmShape* pShape, bool fProlog)
{
    return NewEntry(nType, pSO, pShape, fProlog);
}

lmLineEntry* lmLineTable::NewEntry(lmEEntryType nType, lmStaffObj* pSO,
                                   lmShape* pShape, bool fProlog, lmLUnits uSpace)
{
    lmLineEntry* pEntry = new lmLineEntry(nType, pSO, pShape, fProlog);
    PushBack(pEntry);
    //if (nType == lm_eAlfa)
    //    pEntry->m_uFixedSpace = uSpace;
	return pEntry;
}

wxString lmLineTable::DumpMainTable()
{
    wxString sMsg = wxString::Format(_T("Line table dump. Instr=%d, voice=%d, xStart=%.2f, initSpace=%.2f\n"),
									 GetInstrument(), GetVoice(), GetLineStartPosition(),
                                     GetSpaceAtBeginning() );
    sMsg += _T("===================================================================\n\n");

    if (Size() == 0)
    {
        sMsg += _T("The table is empty.");
        return sMsg;
    }

    //headers
    sMsg += lmLineEntry::DumpHeader();

    //loop to dump table entries
    lmLineEntry* pTE;
    for (int i = 0; i < (int)Size(); i++)
    {
        if (i % 4 == 0) {
            sMsg += wxT("----------------------------------------------------------------------------\n");
        }
        pTE = Item(i);
        sMsg += pTE->Dump(i);
    }

    sMsg += _T("=== End of table ==================================================\n\n");
    return sMsg;

}

void lmLineTable::ClearDirtyFlags()
{
    // Clear the 'Dirty' flag of all StaffObjs in this line

    for (lmLineEntryIterator it = m_LineEntries.begin(); it != m_LineEntries.end(); ++it)
	{
        if ((*it)->m_pSO)
		    (*it)->m_pSO->SetDirty(false, true);    //true-> propagate change
    }
}

lmLUnits lmLineTable::GetLineWidth()
{
	//Return the size of the measure represented by this line or 0 if
	//no omega entry.

	if (m_LineEntries.size() > 0 && m_LineEntries.back()->m_nType == lm_eOmega)
		return m_LineEntries.back()->m_xLeft
			   + m_LineEntries.back()->GetTotalSize() - m_LineEntries.front()->m_xLeft;
	else
		return 0.0f;
}

lmLUnits lmLineTable::GetMaxXFinal()
{
    //returns the maximum x final of this line

    //now compute the maximum xFinal
    lmLUnits xFinal = 0;
    for (lmLineEntryIterator it = m_LineEntries.begin(); it != m_LineEntries.end(); ++it)
	{
		lmLUnits xPos = (*it)->m_xFinal;
        xFinal = wxMax(xPos, xFinal);
    }

    return xFinal;
}

//void lmLineTable::CreateAlphaEntry(lmLUnits uxStart, lmLUnits uSpace)
//{
//    NewEntry(lm_eAlfa, (lmStaffObj*)NULL, (lmShape*)NULL, false, uSpace);
//    m_LineEntries.back()->m_uxAnchor = 0.0f;
//    m_LineEntries.back()->m_xLeft = uxStart;
//    m_LineEntries.back()->m_xInitialLeft = uxStart;
//}



////=====================================================================================
////lmCriticalLine
////=====================================================================================
//
//lmCriticalLine::lmCriticalLine(lmColumnFormatter* pColFmt)
//{
//    m_pLineTable = new lmLineTable(0, 0, 0, 0);
//}
//
//lmCriticalLine::~lmCriticalLine()
//{
//    //do not delete entries. They are owned by real lines!
//    m_pLineTable->Clear();
//    delete m_pLineTable;
//}
//
//void lmCriticalLine::AddNonTimed(lmLineFormatter* pLineFmt, float rTime)
//{
//    //Add to this critical line all non-timed objects at time rTime in received line 
//
//    if (!pLineFmt)
//        return;
//
//    pLineFmt->FromCurrentPosAddNonTimedToLine(m_pLineTable);
//}
//
//void lmCriticalLine::AddEntry(lmLineEntry* pEntry)
//{
//    //Add the received timed entry to this critical line
//
//    if (!pEntry) return;
//    m_pLineTable->PushBack(pEntry);
//    //wxLogMessage(pEntry->Dump(0));
//}
//
//lmLUnits lmCriticalLine::RedistributeSpace(lmLUnits uNewBarSize, lmLUnits uNewStart,
//                                           lmLUnits uOldBarSize, lmBoxSlice* pBSlice)
//{
//    //Creates the Pos<->Time table (m_PosTimes)
//
//#if lmDUMP_TABLES
//    wxLogMessage(_T("[lmCriticalLine::RedistributeSpace] Before space redistribution. uNewBarSize=%.2f, uNewStart=%.2f, uOldBarSize=%.2f"),
//                 uNewBarSize, uNewStart, uOldBarSize );
//    wxLogMessage( DumpMainTable() );
//#endif
//
//    lmLUnits uBarPosition = 0.0f;
//    lmLUnits uOldStart = m_pLineTable->Front()->m_xLeft;
//    pBSlice->ClearPosTimeTable();
//
//    //all non-timed entries, at beginning, marked as fProlog must be only re-located
//    lmLUnits uShiftReloc = uNewStart - uOldStart;
//    lmLineEntryIterator it = m_pLineTable->Begin();
//    while (it != m_pLineTable->End() && (*it)->m_rTimePos < 0.0f)
//    {
//        if ((*it)->m_pShape)
//        {
//            if ((*it)->m_fProlog)
//                (*it)->m_xLeft += uShiftReloc;
//            else
//			    break;
//        }
//        ++it;
//    }
//
//    if (it == m_pLineTable->End())
//         return uBarPosition;
//
//    //first timed entry marks the start point for repositioning.
//    //Compute proportion factor
//    lmLUnits uStartPos = (*it)->m_xLeft - (*it)->m_uxAnchor;
//    lmLUnits uDiscount = uStartPos - uOldStart;
//    float rProp = (uNewBarSize-uDiscount) / (uOldBarSize-uDiscount);
//
//	//Reposition the remainder entries
//    for (; it != m_pLineTable->End(); ++it)
//	{
//        lmLineEntry* pTPE = *it;
//        if (pTPE->m_nType == lm_eStaffobj)
//        {
//            lmLUnits uOldPos = pTPE->m_xLeft - pTPE->m_uxAnchor;
//            lmLUnits uShift = uDiscount + (uNewStart + (uOldPos - uStartPos) * rProp) - uOldPos;
//            lmPosTimeEntry tPosTime = {pTPE, pTPE->m_rTimePos, pTPE->m_xLeft - pTPE->m_uxAnchor + uShift};
//            m_PosTimes.push_back(tPosTime);
//            float rDuration = pTPE->m_pSO->GetTimePosIncrement();
//
//            //determine notehead width or rest width
//            lmLUnits uxWidth = 0.0f;
//            if (pTPE->m_pSO->IsRest())
//                uxWidth = pTPE->m_pShape->GetWidth();
//            else if (pTPE->m_pSO->IsNote())
//                uxWidth = ((lmShapeNote*)pTPE->m_pShape)->GetNoteHead()->GetWidth();
//
//            //add entry to box slice table
//            pBSlice->AddPosTimeEntry(tPosTime.uxPos, tPosTime.rTimepos, rDuration, uxWidth);
//        }
//        else if (pTPE->m_nType == lm_eOmega)
//        {
//            //there might be no barline.
//            if (pTPE->m_pSO)
//            {
//                uBarPosition = uNewStart + uNewBarSize - pTPE->m_uSize;
//                lmLUnits uShiftBar = uBarPosition - pTPE->m_xLeft + pTPE->m_uxAnchor;
//                lmPosTimeEntry tPosTime = {pTPE, pTPE->m_rTimePos, pTPE->m_xLeft - pTPE->m_uxAnchor + uShiftBar};
//                m_PosTimes.push_back(tPosTime);
//				//wxLogMessage(_T("[lmCriticalLine::RedistributeSpace] Reposition bar: uBarPosition=%.2f, uShiftBar=%.2f, xLeft=%.2f, uxAnchor=%.2f"),
//				//			uBarPosition, uShiftBar, pTPE->m_xLeft, pTPE->m_uxAnchor);
//            }
//        }
//    }
//
//#if lmDUMP_TABLES
//    wxLogMessage(_T("Critical line. After space redistribution"));
//    wxLogMessage( DumpMainTable() );
//    wxLogMessage( DumpPosTimes() );
//#endif
//
//    //BoxSlice timepos table data finished. Inform about this.
//    pBSlice->ClosePosTimeTable();
//
//    return uBarPosition;
//}
//
//void lmCriticalLine::InitializeToGetTimepos()
//{
//    m_rLastTimepos = -1.0f;
//    m_it = m_PosTimes.begin();
//}
//
//lmLUnits lmCriticalLine::GetPosForStaffobj(lmStaffObj* pSO, float rTime)
//{
//    //find position for non-timed object pSO.
//    //Iterator is pointing to non-timed object after time rTime
//
//    //save current entry (first non-timed object after time rTime)
//    std::vector<lmPosTimeEntry>::iterator itNonTimed = m_it;
//
//    //find object pSO
//    for (; m_it != m_PosTimes.end() && !IsHigherTime((*m_it).rTimepos, rTime); ++m_it)
//	{
//        if ((*m_it).pTPE->m_pSO == pSO)
//            return (*m_it).uxPos;
//    }
//
//    //the object is not in critical line.
//    //Return position for first non-timed object after current time
//    wxASSERT(itNonTimed != m_PosTimes.end());
//    return -(*itNonTimed).uxPos;
//}
//
//lmLUnits lmCriticalLine::GetTimepos(float rTime)
//{
//    wxASSERT(IsHigherTime(rTime, m_rLastTimepos));
//
//    #ifdef __WXDEBUG__
//        std::vector<lmPosTimeEntry>::iterator it = m_it;
//    #endif
//
//    for (; m_it != m_PosTimes.end(); ++m_it)
//	{
//        if (IsEqualTime((*m_it).rTimepos, rTime))
//            return (*m_it).uxPos;
//    }
//
//    //Error
//    #ifdef __WXDEBUG__
//        g_pLogger->LogForensic(_T("[lmCriticalLine::GetTimepos] Looking for time %.2f"), rTime);
//        if (it != m_PosTimes.end())
//            g_pLogger->LogForensic(_T("[lmCriticalLine::GetTimepos] m_it is pointing to rTime %.2f"), (*it).rTimepos);
//        else
//            g_pLogger->LogForensic(_T("[lmCriticalLine::GetTimepos] m_it is pointing to end"));
//
//        g_pLogger->LogForensic( DumpPosTimes() );
//        g_pLogger->LogForensic( this->DumpMainTable() );
//        g_pLogger->LogForensic( this->DumpTimePosTable() );
//    #endif
//
//    wxASSERT(false);
//    return 0.0f;       //compiler happy
//}
//
//wxString lmCriticalLine::DumpPosTimes()
//{
//    //dump Pos-Times table
//
//    wxString sMsg = _T("Pos-Times table\n");
//    sMsg += _T("===============================\n\n");
//
//    if (m_PosTimes.size() == 0)
//    {
//        sMsg += _T("The table is empty.");
//        return sMsg;
//    }
//
//    //headers
//    //           .......+ .......+   ...+ ..+   +  ..........+........+........+........+........+........+........+........+........+......+
//    sMsg +=  _T(" ColumnFormatter    uxPos\n");
//
//    //loop to dump entries
//    int iE = 0;
//    std::vector<lmPosTimeEntry>::iterator itDbg;
//    for (itDbg = m_PosTimes.begin(); itDbg != m_PosTimes.end(); ++itDbg, ++iE)
//	{
//        sMsg += wxString::Format(_T("%8.2f %8.2f "), (*itDbg).rTimepos, (*itDbg).uxPos );
//        sMsg += (*itDbg).pTPE->Dump(iE);
//    }
//
//    return sMsg;
//}




//=====================================================================================
//lmColumnFormatter
//=====================================================================================

lmColumnFormatter::lmColumnFormatter(lmColumnStorage* pStorage, float rSpacingFactor,
                                     lmESpacingMethod nSpacingMethod, lmTenths nSpacingValue)
    : m_rSpacingFactor(rSpacingFactor)
    , m_nSpacingMethod(nSpacingMethod)
    , m_rSpacingValue(nSpacingValue)
    //, m_pCriticalLine((lmCriticalLine*)NULL)
    , m_pColStorage(pStorage)
{
}

lmColumnFormatter::~lmColumnFormatter()
{
    Initialize();
}

lmLUnits lmColumnFormatter::TenthsToLogical(lmTenths rTenths, int nStaff)
{
    return m_pColStorage->TenthsToLogical(rTenths, nStaff);
}

void lmColumnFormatter::Initialize()
{
    //Initialize the object to reuse it (???)

    //if (m_pCriticalLine)
    //{
    //    delete m_pCriticalLine;
    //    m_pCriticalLine = (lmCriticalLine*)NULL;
    //}
}

lmBarline* lmColumnFormatter::GetBarline()
{
    //returns the barline object in the omega entry, if any

    //we know that the bar is in the critical line
    //if (!m_pCriticalLine)
        return (lmBarline*)NULL;

    ////lmLineEntry* pEntry = m_pCriticalLine->GetLastEntry();
    //wxASSERT(pEntry->m_nType == lm_eOmega);
    //return (lmBarline*)pEntry->m_pSO;
}

void lmColumnFormatter::DoSpacing(bool fTrace)
{
    //computes the minimum space required by this column and stores it
    //in m_uMinColumnSize

#if lmDUMP_TABLES
    wxLogMessage( DumpColumnData() );
#endif

    m_uMinColumnSize = ComputeSpacing(m_rSpacingFactor);

#if lmDUMP_TABLES
    wxLogMessage( DumpColumnData() );
#endif
}

lmLUnits lmColumnFormatter::ComputeSpacing(float rFactor)
{
    //Spacing algorithm. Returns the resulting minimum column width

	//Step 1: Assign spacing and positions to each object in each line, independently of 
    //any other line
    ComputeSpacingForEachLine(rFactor);
    AlignObjectsAtSameTimePos();

    //In following loop, the minimum start position for the first note/rest in each line is
    //computed and we take the maximum of them.
    //Also, we determine in which line the non-timed objects take more space.

	//lmLUnits uxCurPos = 0.0f;
 //   m_pLongestNonTimedLineFmt = (lmLineFormatter*)NULL;

 //   lmLinesIterator itEnd = m_pColStorage->End();
 //   for (lmLinesIterator it=m_pColStorage->Begin(); it != itEnd; ++it)
	//{
 //       //create formatters for the lines
 //       lmLineFormatter* pLineFmt = new lmLineFormatter(*it);
 //       m_LineFormatters.push_back(pLineFmt);

 //       //Assign space to all entries
 //       pLineFmt->AssignSpace(this, rFactor);

  //      //start processing timepos 0. This processes all non-aligned objects at
  //      //timepos 0 to determine the minimum start position for the first timed object
  //      lmLUnits uSpaceAfterProlog = TenthsToLogical(lmSPACE_AFTER_PROLOG, 1);
		//lmLUnits uxStartPos = pLineFmt->IntitializeSpacingAlgorithm(uSpaceAfterProlog);

  //      if (uxCurPos < uxStartPos)
  //      {
  //          m_pLongestNonTimedLineFmt = pLineFmt;
		//    uxCurPos = uxStartPos;
  //      }
	//}

 //   //Enter a loop to process timed
 //   //objects at m_rCurTime and advance to next m_rCurTime.
 //   //The loop is exited when no more timepos are found

 //   m_rCurTime = 0.0f;
	//bool fContinue = true;
	//while(fContinue)
 //   {
 //       //each loop cycle is to process a timepos in the column. We start at m_rCurTime=0

 //       //Here the first aligned object in each line, at current timepos, has been positioned.
 //       //Proceed to determine which one is most at right and assign that position to
 //       //all aligned objects at current timepos.

	//	//Loop to determine minimum common x position for timepos m_rCurTime.
 //       //If computing critical line, also find the aligned object at timepos m_rCurTime, with minimum
 //       //time duration; if several objects with this minimum duration, take the widest one.
 //       m_pShortestEntry = (lmLineEntry*)NULL;
 //       m_rShortestObjectDuration = 100000000.0f;       //any too big value
	//	for (lmLineFormattersIterator it = m_LineFormatters.begin(); it != m_LineFormatters.end(); ++it)
	//	{
 //           (*it)->SetCurrentTime(m_rCurTime);    //needed while refactoring

	//		lmLUnits uxMinPos = (*it)->GetMinFeasiblePosForTime(m_rCurTime);
 //           lmLUnits uxObjAnchor = (*it)->GetAnchorForTime(m_rCurTime);
 //           if (uxObjAnchor < 0.0f) uxMinPos -= uxObjAnchor;
	//		uxCurPos = wxMax(uxCurPos, uxMinPos);


 //       //Here uxCurPos contains the minimum xPos to align all aligned objects at current timepos.
 //       //If creating the critical line, also following variables are meaningful:
 //       //  m_pShortestEntry: points to entry for the aligned object requiring less time
 //       //  m_pLongestNonTimedLineFmt: point to the line containing the longest non-aligned object
 //       //
 //       //Now we will add entries to critical line (if creation requested) and process
 //       //all timed objects, placed at m_rCurTime, to reposition them.




	//    //Process all timed objects placed at time m_rCurTime to reposition them and determine
 //       //next timepos (m_rNextTime) and its start position (m_uxNextTimeStartPosition).
 //       //returns true if there is a next timepos
 //       fContinue = AlignObjectsAtCurrentTimeAndGetNextTime(uxCurPos, rFactor);
 //       uxCurPos = m_uxNextTimeStartPosition;




	//	//advance to next time
	//	m_rCurTime = m_rNextTime;
	//}

	//Get bar size
	lmLUnits uMeasureSize = 0.0f;
	for (lmLinesIterator it=m_pColStorage->Begin(); it != m_pColStorage->End(); ++it)
	{
		lmLUnits uSize = (*it)->GetLineWidth();
		uMeasureSize = wxMax(uMeasureSize, uSize);

	}

 //   //wxLogMessage(_T("[lmColumnFormatter::ComputeSpacing] measure size = %.2f"), uMeasureSize);


    ////delete line formatters
    //std::vector<lmLineFormatter*>::iterator itL;
    //for (itL=m_LineFormatters.begin(); itL != m_LineFormatters.end(); ++itL)
    //{
    //    delete *itL;
    //}
    //m_LineFormatters.clear();

	return uMeasureSize;
}

void lmColumnFormatter::ComputeSpacingForEachLine(float rFactor)
{
	//Assign spacing and positions to each object in each line, independently of 
    //any other line

    lmLinesIterator itEnd = m_pColStorage->End();
    for (lmLinesIterator it=m_pColStorage->Begin(); it != itEnd; ++it)
	{
        lmLUnits uSpaceAfterProlog = TenthsToLogical(lmSPACE_AFTER_PROLOG, 1);
        lmLineSpacer oSpacer(*it, this, rFactor);
        oSpacer.ComputeLineSpacing(uSpaceAfterProlog);

        ////create formatters for the lines
        //lmLineFormatter* pLineFmt = new lmLineFormatter(*it);
        //m_LineFormatters.push_back(pLineFmt);

        ////Assign space to all entries
        //pLineFmt->AssignSpace(this, rFactor);
    }
}

void lmColumnFormatter::AlignObjectsAtSameTimePos()
{
}

lmLUnits lmColumnFormatter::RedistributeSpace(lmLUnits uNewStart, lmBoxSlice* pBSlice)
{
    //the new width that this bar will have has been computed and is stored in m_uMinColumnSize
    lmLUnits uNewBarSize = m_uMinColumnSize;
    lmLUnits uOldBarSize = m_pColStorage->GetGrossBarSize();


    //Step 2. Move the shapes to their final positions
	for (lmLinesIterator it=m_pColStorage->Begin(); it != m_pColStorage->End(); ++it)
	{
        lmLineResizer oResizer(*it);
		oResizer.RepositionShapes(uNewBarSize, uNewStart, uOldBarSize);
        //oResizer.InformAttachedObjs();
    }


    //Shift the position of all StaffObjs by the amount given by the difference between current
    //bar start position and the new desired start position.
    //In addition:
    // - the position of the barline at the end of this bar is also shifted so that
    //   the new width on the bar becomes nNewBarWidth.
    // - A table xPos/timepos is transferred to the received BoxSlice
    //
    //Parameters:
    //   uNewStart - the new left position for the start of this bar
    //   pBoxSlice - ptr. to the box slice
    //
    //Results and return value:
    //   The new positions are stored in the StaffObjs
    //   The start position for next measure is retuned.

//    //Critical line contains all existing timepos in this column.
//    //Compute new positions for each timepos
    //lmLUnits uOldBarSize = m_pColStorage->GetGrossBarSize();
//    m_pCriticalLine->RedistributeSpace(uNewBarSize, uNewStart, uOldBarSize, pBSlice);
//
//    //transfer the new positions to all lines and to BoxSlice
//	for (lmLinesIterator it=m_pColStorage->Begin(); it != m_pColStorage->End(); ++it)
//	{
//        lmLineResizer oResizer(*it);
//		oResizer.RepositionShapes(m_pCriticalLine, uNewBarSize, uNewStart, uOldBarSize);
//        oResizer.InformAttachedObjs();
//    }
//
//    //wxLogMessage(_T("[lmColumnFormatter::RedistributeSpace] uNewBarSize=%.2f, uNewStart=%.2f, uOldBarSize=%.2f"),
//    //             uNewBarSize, uNewStart, uOldBarSize );
//
//#if lmDUMP_TABLES
//    wxLogMessage(_T("[lmColumnFormatter::RedistributeSpace] After repositioning shapes"));
//    wxLogMessage( DumpColumnData() );
//#endif

    //4. Shift all objects, in all lines, by the difference between new x position and old x position.


    return uNewStart + uNewBarSize;
}

bool lmColumnFormatter::GetOptimumBreakPoint(lmLUnits uAvailable, float* prTime,
                                          lmLUnits* puWidth)
{
    //returns true if no break point found (exceptional case). In all other cases
    //returns the information (rTime, xPos) for the first entry with space <= uAvailable
    //and with maximum priority

    lmBreaksTable* pBreaks = ComputeBreaksTable();

    //wxLogMessage( DumpColumnData() );

    //select highest entry with space <= uAvailable
    lmBeaksTimeEntry* pBTE = pBreaks->GetFirst();
    lmBeaksTimeEntry* pSel = (lmBeaksTimeEntry*)NULL;
    while (pBTE && pBTE->uxEnd <= uAvailable)
    {
        pSel = pBTE;
        pBTE = pBreaks->GetNext();
    }
    if (!pSel)
        return true;        //big problem: no break points!

    //wxLogMessage(_T("[lmColumnFormatter::GetOptimumBreakPoint] uAvailable=%.2f, returned=%.2f, time=%.2f"),
    //             uAvailable, pSel->uxEnd, pSel->rTimepos);

    //return information
	*prTime = pSel->rTimepos;
    *puWidth = pSel->uxEnd;

    //breaks table no longer needed. Delete it
    delete pBreaks;

    return false;       //no problems. There are break points
}

lmBreaksTable* lmColumnFormatter::ComputeBreaksTable()
{
    //This method computes the BreaksTable. This is a table sumarizing break points
    //information, that is, suitable places through all staves and voices where it is
    //possible to break a system and start a new one. The best break locations are
    //usually are the bar lines common to all staves. But in certain rare cases (i.e.
    //scores without time signature or having instrumens not sharing a common
    //time signature, or when it is requested to render the score in very narrow
    //paper, etc.) it is necessary to split music in unnusual points.
    //Returns the breaks table. CALLER MUST DELETE the table when no longer needed

    //Step 1. Build a table for each line
    std::vector<lmBreaksTable*> partialTables;
	for (lmLinesIterator itTL = m_pColStorage->Begin(); itTL != m_pColStorage->End(); ++itTL)
	{
        lmBreaksTable* pBT = new lmBreaksTable();
        lmColumnSplitter oSplitter(*itTL);
        oSplitter.ComputeBreakPoints(pBT);
        partialTables.push_back(pBT);
    }


    //Step 2. Combine the partial tables
    lmBreaksTable* pTotalBT = new lmBreaksTable();
    std::vector<lmBreaksTable*>::iterator itBT;
    for (itBT = partialTables.begin(); itBT != partialTables.end(); ++itBT)
    {
        if (pTotalBT->IsEmpty())
        {
            //just copy entries
            lmBeaksTimeEntry* pEP = (*itBT)->GetFirst();       //pEP Entry from Partial list
            while (pEP)
            {
                pTotalBT->AddEntry(pEP);
                pEP = (*itBT)->GetNext();
            }
        }
        else
        {
            //merge current table with total table
            //lmBeaksTimeEntry* pEP = (*itBT)->GetFirst();       //pEP Entry from Partial list
            //while (pEP)
            //{
            //    pTotalBT->AddEntry(pEP);
            //    pEP = (*itBT)->GetNext();
            //}
        }
    }


    //Delete partial tables, no longer needed
    for (itBT = partialTables.begin(); itBT != partialTables.end(); ++itBT)
        delete *itBT;
    partialTables.clear();

    //wxLogMessage(_T("Total Breaks Table:"));
    //wxLogMessage( pTotalBT->Dump() );

    //Step 3. Sort breaks table by priority and final x position
    //TODO

    return pTotalBT;
}

void lmColumnFormatter::InitializeCriticalLine()
{
    ////create the line object
    //m_pCriticalLine = new lmCriticalLine(this);

    //////add alpha entry copying it from first line
    ////m_pCriticalLine->AddEntry( m_pColStorage->Front()->GetAlphaEntry() );

    //#if lmTRACE_CRITICAL_LINE
    //    //wxLogMessage(_T("[lmColumnFormatter::ComputeSpacing] Start creating Critical Line. Added alpha entry"));
    //    wxLogMessage( m_pCriticalLine->DumpMainTable() );
    //#endif
}

//void lmColumnFormatter::AddShorterTimedObjectToCriticalLine(lmLineEntry* pShortestEntry)
//{
//    m_pCriticalLine->AddEntry( pShortestEntry );
//    #if lmTRACE_CRITICAL_LINE
//        wxLogMessage(_T("Added non-timed and timed for current timepos %.2f"), m_rCurTime);
//        wxLogMessage( m_pCriticalLine->DumpMainTable() );
//    #endif
//}

bool lmColumnFormatter::AlignObjectsAtCurrentTimeAndGetNextTime(lmLUnits uxPos,
                                                                float rFactor)
{
	//Process all timed objects placed at time m_rCurTime to reposition them and determine
    //next timepos (m_rNextTime) and its start position (m_uxNextTimeStartPosition).
    //returns true if there is a next timepos

    bool fContinue = false;
    m_rNextTime = 100000000.0f;		//any too big value
    m_uxNextTimeStartPosition = 0.0f;
    for (lmLineFormattersIterator it = m_LineFormatters.begin(); it != m_LineFormatters.end(); ++it)
	{
        lmLUnits uxMaxPos;
        float rNewTime = (*it)->AssignPositionToObjectsAtTime(m_rCurTime, uxPos, rFactor, 
                                                       lmSPACE_AFTER_INTERMEDIATE_CLEF,
                                                       lmMIN_SPACE_BETWEEN_NOTE_AND_CLEF,
                                                       this, &uxMaxPos);

		if (IsHigherTime(rNewTime, 0.0f))
        {
            fContinue = true;
			m_rNextTime = wxMin(m_rNextTime, rNewTime);
        }

        m_uxNextTimeStartPosition = wxMax(m_uxNextTimeStartPosition, uxMaxPos);
	}
    return fContinue;
}

void lmColumnFormatter::DetermineLineWithLongestNonTimedObjects()
{
    //Determine in which line the non-timed objects take more space.
    //Result is left in m_pLongestNonTimedLineFmt.

	lmLUnits udxNonTimed = -1.0f;   //any negative value
    m_pLongestNonTimedLineFmt = (lmLineFormatter*)NULL;
    for (lmLineFormattersIterator it = m_LineFormatters.begin(); it != m_LineFormatters.end(); ++it)
	{
        #if lmTRACE_CRITICAL_LINE
            wxLogMessage(_T("Looking for widest non-timed objs. for current timepos"));
        #endif

		lmLUnits uxNonTimedSpace = (*it)->GetSpaceNonTimedForTime(m_rNextTime);
        if (udxNonTimed < uxNonTimedSpace)
        {
            m_pLongestNonTimedLineFmt = *it;
		    udxNonTimed = uxNonTimedSpace;
        }
    }
}

void lmColumnFormatter::SelectTimedObjIfItHasMinimalDuration(lmLineEntry* pEntry)
{
    //If the aligned object at current timepos with minimum time duration. If several
    //objects with this minimum duration, take the widest one. Found values are left
    //in m_rShortestObjectDuration and m_pShortestEntry.

    if (pEntry)
    {
        #if lmTRACE_CRITICAL_LINE
            wxLogMessage(_T("Looking for minimum note/rest for current timepos %.2f"), m_rCurTime);
            wxLogMessage(pEntry->Dump(0));
        #endif

        float rDuration = pEntry->m_pSO->GetTimePosIncrement();
        if (IsHigherTime(m_rShortestObjectDuration, rDuration))
        {
            //this object consumes less time. Select it unless it is a barline and there is
            //already an object selected
            if (m_rShortestObjectDuration == 100000000.0f || !pEntry->m_pSO->IsBarline())
            {
                m_rShortestObjectDuration = rDuration;
                m_pShortestEntry = pEntry;
            }
        }
        else if (IsEqualTime(m_rShortestObjectDuration, rDuration))
        {
            //this object consumes the same time than current selected one. Select it
            //if it needs more space
            if (pEntry->GetTotalSize() > m_pShortestEntry->GetTotalSize())
            {
	            m_rShortestObjectDuration = rDuration;
                m_pShortestEntry = pEntry;
            }
        }
        else if (m_pShortestEntry && m_pShortestEntry->m_pSO->IsBarline()
                    && !pEntry->m_pSO->IsBarline() )
        {
            //There is a selected barline. But current object is not a barline. Select it
            //instead of the barline.
	        m_rShortestObjectDuration = rDuration;
            m_pShortestEntry = pEntry;
        }

        #if lmTRACE_CRITICAL_LINE
            wxLogMessage(_T("For now, shortest note/rest for current timepos is this one:"));
            wxLogMessage(pEntry->Dump(0));
        #endif
    }
}



//-------------------------------------------------------------------------------------
// lmSystemNewFormat implementation
//-------------------------------------------------------------------------------------

lmSystemNewFormat::lmSystemNewFormat(float rSpacingFactor, lmESpacingMethod nSpacingMethod,
                                     lmTenths nSpacingValue)
    : lmSystemFormatter(rSpacingFactor, nSpacingMethod, nSpacingValue)
    //: m_rSpacingFactor(rSpacingFactor)
    //, m_nSpacingMethod(nSpacingMethod)
    //, m_rSpacingValue(nSpacingValue)
{
}

lmSystemNewFormat::~lmSystemNewFormat()
{
    std::vector<lmColumnFormatter*>::iterator itF;
    for (itF=m_ColFormatters.begin(); itF != m_ColFormatters.end(); ++itF)
        delete *itF;
    m_ColFormatters.clear();

    std::vector<lmColumnStorage*>::iterator itS;
    for (itS=m_ColStorage.begin(); itS != m_ColStorage.end(); ++itS)
        delete *itS;
    m_ColStorage.clear();

    std::vector<lmLinesBuilder*>::iterator itLB;
    for (itLB=m_LinesBuilder.begin(); itLB != m_LinesBuilder.end(); ++itLB)
        delete *itLB;
    m_LinesBuilder.clear();
}

void lmSystemNewFormat::EndOfSystemMeasurements()
{
    //caller informs that all data for this system has been suplied.
    //This is the right place to do any preparatory work, not to be repeated if re-spacing.

    //Nothing to do for current implementation
}

void lmSystemNewFormat::StarBarMeasurements(int iCol, int nInstr, lmLUnits uxStart,
                                            lmVStaff* pVStaff, lmLUnits uSpace)
{
    //prepare to receive data for a new bar in column iCol [0..n-1].

    //If not yet created, create ColumnFormatter object to store measurements
    lmLinesBuilder* pLB;
    if (m_ColFormatters.size() == (size_t)iCol)
    {
        //create storage for this column
        lmColumnStorage* pStorage = new lmColumnStorage();
        m_ColStorage.push_back(pStorage);

        //create a lines builder object for this column
        pLB = new lmLinesBuilder(pStorage);
        m_LinesBuilder.push_back(pLB);

        //create the column formatter object
        lmColumnFormatter* pColFmt = new lmColumnFormatter(pStorage, m_rSpacingFactor,
                                                           m_nSpacingMethod, m_rSpacingValue);
        m_ColFormatters.push_back(pColFmt);
    }
    else
        pLB = m_LinesBuilder[iCol];

    //start lines
    pLB->StarMeasurementsForInstrument(nInstr, uxStart, pVStaff, uSpace);
}

void lmSystemNewFormat::IncludeObject(int iCol, int nInstr, lmStaffObj* pSO,
                                      lmShape* pShape, bool fProlog, int nStaff)
{
    //caller sends data about one staffobj in current bar, for column iCol [0..n-1]

    m_LinesBuilder[iCol]->IncludeObject(nInstr, pSO, pShape, fProlog, nStaff);
}

void lmSystemNewFormat::IncludeBarlineAndTerminateBarMeasurements(int iCol, lmStaffObj* pSO, 
                                                         lmShape* pShape, lmLUnits xStart)
{
    //caller sends lasts object to store in current bar, for column iCol [0..n-1]. 

    m_LinesBuilder[iCol]->CloseLine(pSO, pShape, xStart);
}

void lmSystemNewFormat::TerminateBarMeasurementsWithoutBarline(int iCol, lmLUnits xStart)
{
    //caller informs that there are no barline and no more objects in column iCol [0..n-1]. 

    m_LinesBuilder[iCol]->CloseLine((lmStaffObj*)NULL, (lmShape*)NULL, xStart);
}

void lmSystemNewFormat::DiscardMeasurementsForColumn(int iCol)
{
    //caller request to ignore measurements for column iCol [0..n-1]

    m_ColFormatters[iCol]->Initialize();
    m_LinesBuilder[iCol]->Initialize();
}

void lmSystemNewFormat::DoColumnSpacing(int iCol, bool fTrace)
{
    m_ColFormatters[iCol]->DoSpacing(fTrace);
}

lmLUnits lmSystemNewFormat::RedistributeSpace(int iCol, lmLUnits uNewStart,
                                              lmBoxSlice* pBSlice)
{
    return m_ColFormatters[iCol]->RedistributeSpace(uNewStart, pBSlice);
}

void lmSystemNewFormat::IncrementColumnSize(int iCol, lmLUnits uIncr)
{
    m_ColFormatters[iCol]->IncrementColumnSize(uIncr);
}

lmLUnits lmSystemNewFormat::GetStartPositionForColumn(int iCol)
{
    return m_ColStorage[iCol]->GetStartOfBarPosition();
}

lmLUnits lmSystemNewFormat::GetMinimumSize(int iCol)
{
    return m_ColFormatters[iCol]->GetMinimumSize();
}

bool lmSystemNewFormat::GetOptimumBreakPoint(int iCol, lmLUnits uAvailable,
                                        float* prTime, lmLUnits* puWidth)
{
    return m_ColFormatters[iCol]->GetOptimumBreakPoint(uAvailable, prTime, puWidth);
}

lmBarline* lmSystemNewFormat::GetColumnBarline(int iCol)
{
    return m_ColFormatters[iCol]->GetBarline();
}

void lmSystemNewFormat::ClearDirtyFlags(int iCol)
{
    m_ColStorage[iCol]->ClearDirtyFlags();
}

wxString lmSystemNewFormat::DumpColumnData(int iCol)
{
    return m_ColStorage[iCol]->DumpColumnStorage();
}

//------------------------------------------------
// Debug build: methods coded only for Unit Tests
//------------------------------------------------
#if defined(__WXDEBUG__)

int lmSystemNewFormat::GetNumObjectsInColumnLine(int iCol, int iLine)
{
    //iCol, iLine = [0..n-1]
    return m_ColStorage[iCol]->GetNumObjectsInLine(iLine);
}

#endif



//-------------------------------------------------------------------------------------
// lmColumnStorage implementation: encapsulates the table of lines for a column
//-------------------------------------------------------------------------------------

lmColumnStorage::lmColumnStorage()
{
}

lmColumnStorage::~lmColumnStorage()
{
    DeleteLines();
}

void lmColumnStorage::DeleteLines()
{
	for (lmLinesIterator it=m_Lines.begin(); it != m_Lines.end(); ++it)
	{
		delete *it;
	}
	m_Lines.clear();
}

lmLUnits lmColumnStorage::TenthsToLogical(lmTenths rTenths, int nStaff)
{
    wxASSERT(nStaff > 0);
	return m_pStaff[nStaff-1]->TenthsToLogical(rTenths);
}

lmLinesIterator lmColumnStorage::FindLineForInstrAndVoice(int nInstr, int nVoice)
{
    //return m_pColStorage->FindLineForInstrAndVoice(nInstr, nVoice);
	for (lmLinesIterator it=m_Lines.begin(); it != m_Lines.end(); ++it)
    {
		if ((*it)->IsLineForInstrument(nInstr) && (*it)->IsLineForVoice(nVoice) )
            return it;
	}
    return m_Lines.end();
}

lmLineTable* lmColumnStorage::OpenNewLine(int nInstr, int nVoice, lmLUnits uxStart,
                                               lmLUnits uSpace)
{
    lmLineTable* pLineTable = new lmLineTable(nInstr, nVoice, uxStart, uSpace);
    m_Lines.push_back(pLineTable);
    return pLineTable;
}

wxString lmColumnStorage::DumpColumnStorage()
{
    wxString sMsg = _T("Start of dump. ColumnStorage\n");
	for (lmLinesIterator it = m_Lines.begin(); it != m_Lines.end(); ++it)
	{
        sMsg += (*it)->DumpMainTable();
    }
    return sMsg;
}

void lmColumnStorage::ClearDirtyFlags()
{
    //Clear flag 'Dirty' in all StaffObjs of this table. This has nothing to do with 
    //lmColumnStorage purposes, but its is a convenient place to write a method
    //for doing this.

	for (lmLinesIterator it = m_Lines.begin(); it != m_Lines.end(); ++it)
		(*it)->ClearDirtyFlags();
}

lmLUnits lmColumnStorage::GetGrossBarSize()
{
    //returns the maximum measure size of all lines

    //compute the maximum xFinal
    lmLUnits xFinal = 0;
	for (lmLinesIterator it = m_Lines.begin(); it != m_Lines.end(); ++it)
	{
		lmLUnits xPos = (*it)->GetMaxXFinal();
        xFinal = wxMax(xFinal, xPos);
    }

    return xFinal - GetStartOfBarPosition();
}

lmLUnits lmColumnStorage::GetStartOfBarPosition()
{
    //returns the x position for the start of the bar column

    return m_Lines.front()->GetLineStartPosition();
}



//-------------------------------------------------------------------------------------
// lmLinesBuilder implementation: encapsulates the algorithms to split a column
// into lines and to store them in the received column storage
//-------------------------------------------------------------------------------------

lmLinesBuilder::lmLinesBuilder(lmColumnStorage* pStorage)
    : m_pColStorage(pStorage)
	, m_pCurEntry((lmLineEntry*)NULL)
{
    Initialize();
}

lmLinesBuilder::~lmLinesBuilder()
{
}

void lmLinesBuilder::Initialize()
{
    ResetDefaultStaffVoices();
}

void lmLinesBuilder::ResetDefaultStaffVoices()
{
    for(int i=0; i < lmMAX_STAFF; i++)
        m_nStaffVoice[i] = 0;
}

void lmLinesBuilder::StarMeasurementsForInstrument(int nInstr, lmLUnits uxStart,
                                                      lmVStaff* pVStaff, lmLUnits uSpace)
{
    CreateLinesForEachStaff(nInstr, uxStart, pVStaff, uSpace);
}

void lmLinesBuilder::CreateLinesForEachStaff(int nInstr, lmLUnits uxStart,
                                                lmVStaff* pVStaff, lmLUnits uSpace)
{
    //We need at least one line for each staff, for the music on each staff. 
    //As we don'y know yet which voice number will be the first note/rest on each staff we
    //cannot yet assign voice to these lines. Therefore, we will assign voice 0 (meaning 
    //'no voice assigned yet') and voice will be updated when finding the first note/rest.

	int nNumStaves = pVStaff->GetNumStaves();
    wxASSERT(nNumStaves < lmMAX_STAFF);

    for(int iS=0; iS < nNumStaves; iS++)
    {
        m_pColStorage->SaveStaffPointer(iS, pVStaff->GetStaff(iS+1));
  //      m_nStaffVoice[iS] = 0;  //no voice assigned yet to each deafult staff line
  //      StartLine(nInstr, 0, uxStart, uSpace);
        m_nStaffVoice[iS] = iS+1;
        StartLine(nInstr, iS+1, uxStart, uSpace);
    }
}

void lmLinesBuilder::StartLine(int nInstr, int nVoice, lmLUnits uxStart, lmLUnits uSpace)
{
    //Start a new line for instrument nInstr (0..n-1), to be used for voice nVoice.
    //The line starts at x position uxStart and space before first object must be uSpace.

	//If this is the first line for instrument nInstr, all non-voiced StaffObj
	//will be assigned to this line, as well as the first voice encountered

    //created line is set as 'current line' to receive new data. 

    //If no start position specified inherit it from first line for this column
    if (uxStart < 0.0f)
        uxStart = m_pColStorage->Front()->GetLineStartPosition();

    //create the line and store it
    lmLineTable* pLineTable = m_pColStorage->OpenNewLine(nInstr, nVoice, uxStart, uSpace);

    //as line is empty, pointer to last added entry is NULL
	m_pCurEntry = (lmLineEntry*)NULL;

    //created line is set as 'current line' to receive new data. 
    m_itCurLine = m_pColStorage->GetLastLine();
}

void lmLinesBuilder::CloseLine(lmStaffObj* pSO, lmShape* pShape, lmLUnits xStart)
{
	//close current line.

    m_pCurEntry = (*m_itCurLine)->AddEntry(lm_eOmega, pSO, pShape, false);
    m_pCurEntry->m_xLeft = xStart;
    m_pCurEntry->m_xInitialLeft = xStart;
}

void lmLinesBuilder::IncludeObject(int nInstr, lmStaffObj* pSO, lmShape* pShape,
                                   bool fProlog, int nStaff)
{
    int nVoice = DecideVoiceToUse(pSO, nStaff);
    m_itCurLine = m_pColStorage->FindLineForInstrAndVoice(nInstr, nVoice);

    //if doesn't exist, start it
    if (m_pColStorage->IsEndOfTable(m_itCurLine))
    {
        wxASSERT(nVoice != 0);          //it must be a valid voice. Otherwise the default
                                        //line must have been found!
        StartLine(nInstr, nVoice);
    }

    //add new entry for this object
	m_pCurEntry = (*m_itCurLine)->AddEntry(lm_eStaffobj, pSO, pShape, fProlog);

	//if line found was the default one for the staff, assigne voice to this line and to
    //the staff if not yet assigned
    if (nVoice != 0 && (*m_itCurLine)->IsVoiceNotYetDefined())
    {
		(*m_itCurLine)->SetVoice(nVoice);
    }
}

int lmLinesBuilder::DecideVoiceToUse(lmStaffObj* pSO, int nStaff)
{
	if (nStaff != 0)    //multi-shaped object (clef, key)
    {
        wxASSERT(pSO->IsMultishaped());
        return m_nStaffVoice[nStaff - 1];
    }
    else    //single shape object
    {
		if (pSO->IsNoteRest())
            //pSO has voice: return it
			return ((lmNoteRest*)pSO)->GetVoice();
		else
            //pSO has no voice. Use voice assigned to the staff in which this pSO is placed
			return m_nStaffVoice[ pSO->GetStaffNum() - 1 ];
	}
}

void lmLinesBuilder::EndOfData()
{
    //this method is invoked to inform that all data has been suplied. Therefore, we
    //can do any preparatory work, not to be repeated if re-spacing.
}



//-------------------------------------------------------------------------------------
// lmLineResizer implementation:
//      encapsulates the methods to recompute shapes positions so that the column
//      will have the desired width, and to move the shapes to those positions
//-------------------------------------------------------------------------------------

lmLineResizer::lmLineResizer(lmLineTable* pLineTable)
    : m_pLineTable(pLineTable)
{
}

lmLineResizer::~lmLineResizer()
{
}

lmLUnits lmLineResizer::RepositionShapes(lmLUnits uNewBarSize, lmLUnits uNewStart,
                                         lmLUnits uOldBarSize)
{
    //All layouting computations done. Proceed to move shapes to their final positions.

    lmLUnits uLineShift = uNewStart - m_pLineTable->GetLineStartPosition();
    for (lmLineEntryIterator it = m_pLineTable->Begin(); it != m_pLineTable->End(); ++it)
    {
        lmLUnits uShift = uLineShift + (*it)->m_xLeft - (*it)->m_xInitialLeft;
        if ((*it)->m_pSO && (*it)->m_pShape)
            (*it)->m_pSO->StoreOriginAndShiftShapes( uShift, (*it)->m_pShape->GetOwnerIDX() );
    }


    lmLUnits uBarPosition = 0.0f;
    //   lmLineEntry* pTPE = m_pLineTable->Front();
 //   lmLUnits uOldStart = pTPE->m_xLeft;

 //   //all non-timed entries, at beginning, marked as fProlog must be only re-located
 //   lmLUnits uShiftReloc = uNewStart - uOldStart;
 //   lmLineEntryIterator it = m_pLineTable->Begin();
 //   while (it != m_pLineTable->End() && (*it)->m_rTimePos < 0.0f)
 //   {
 //       if ((*it)->m_pShape)
 //       {
 //           if ((*it)->m_fProlog)
	//		    (*it)->m_pSO->StoreOriginAndShiftShapes( uShiftReloc, (*it)->m_pShape->GetOwnerIDX() );
 //           else
	//		    break;
 //       }
 //       ++it;
 //   }

 //   if (it == m_pLineTable->End())
 //        return uBarPosition;

	////wxLogMessage(_T("[lmLineResizer::RepositionShapes] Reposition: uNewBarSize=%.2f  uNewStart=%.2f  uOldBarSize=%.2f"),
	////			 uNewBarSize, uNewStart, uOldBarSize );

 //   //we are at the start point for repositioning shapes
 //   pCriticalLine->InitializeToGetTimepos();
 //   float rCurTime = (*it)->m_rTimePos;
 //   lmLUnits uxCurPos;
 //   if (rCurTime != -1.00f)
 //       uxCurPos = pCriticalLine->GetTimepos(rCurTime);
 //   else
 //       uxCurPos = pCriticalLine->GetPosForStaffobj((*it)->m_pSO, rCurTime);


 //   for (; it != m_pLineTable->End(); ++it)
	//{
 //       lmLineEntry* pTPE = *it;
 //       //wxLogMessage(_T("Reposition shapes: pTPE->m_rTimePos=%.2f, rCurTime=%.2f, pTPE->m_nType=%d"),
	//    			// pTPE->m_rTimePos, rCurTime, pTPE->m_nType );

 //       //advance to current time
 //       if (!IsEqualTime(pTPE->m_rTimePos, rCurTime) && !(pTPE->m_rTimePos < 0.0f))
 //       {
 //           rCurTime = pTPE->m_rTimePos;
 //           uxCurPos = pCriticalLine->GetTimepos(rCurTime);
 //       }
 //       else if (pTPE->m_rTimePos < 0.0f && pTPE->m_nType == lm_eStaffobj)
 //       {
 //           //if this SO is in critical line, take position from it.
 //           //Otherwise it should be right aligned to next timed SO
 //           uxCurPos = pCriticalLine->GetPosForStaffobj(pTPE->m_pSO, rCurTime);
 //           if (uxCurPos < 0.0f)
 //           {
 //               //The SO is not in critical line
 //               //TODO:  align it to right, joint to next timed SO
 //               //[28/Dec/2008] Following code is a provisional fix for [000.00.error5?] 
 //               //It doesn't affect 001.16 problem (chromatic scale spacing)
 //               uxCurPos = pTPE->m_xLeft - pTPE->m_uxAnchor;
 //           }
 //       }

 //       //reposition this entry
 //       if (pTPE->m_nType == lm_eStaffobj)
 //       {
 //           lmLUnits uShift = uxCurPos - pTPE->m_xLeft + pTPE->m_uxAnchor;
 //           if (pTPE->m_pShape)
	//			pTPE->m_pSO->StoreOriginAndShiftShapes( uShift, pTPE->m_pShape->GetOwnerIDX() );
 //       }
 //       else if (pTPE->m_nType == lm_eOmega)
 //       {
 //           //there might be no barline.
 //           if (pTPE->m_pSO)
 //           {
 //               uBarPosition = uNewStart + uNewBarSize - pTPE->m_uSize;
 //               lmLUnits uShiftBar = uBarPosition - pTPE->m_xLeft;
 //               if (pTPE->m_pShape)
	//			    pTPE->m_pSO->StoreOriginAndShiftShapes(uShiftBar, pTPE->m_pShape->GetOwnerIDX() );

 //               //wxLogMessage(_T("[lmLineResizer::RepositionShapes] Reposition bar: uBarPosition=%.2f, uShiftBar=%.2f"),
	//			//			uBarPosition, uShiftBar );
 //           }
 //       }
 //   }
    return uBarPosition;
}

void lmLineResizer::InformAttachedObjs()
{
    //StaffObj shapes has been moved to their final positions. This method is invoked
    //to inform some attached AuxObjs (i.e. ties) so that they can compute their positions.

    for (lmLineEntryIterator it = m_pLineTable->Begin(); it != m_pLineTable->End(); ++it)
	{
        if ((*it)->m_nType == lm_eStaffobj)
        {
            if ((*it)->m_pSO->IsNote()
                && ((lmNote*)(*it)->m_pSO)->IsTiedToPrev() )
            {
                //End of tie note. Inform the tie shape.
                wxASSERT((*it)->m_pShape);
				((lmShapeNote*)(*it)->m_pShape)->ApplyUserShiftsToTieShape();
            }
        }
    }
}



//----------------------------------------------------------------------------------------
//lmLineSpacer:
//  encapsulates the algorithm to assign spaces and positions to a single line
//----------------------------------------------------------------------------------------

lmLineSpacer::lmLineSpacer(lmLineTable* pLineTable, lmColumnFormatter* pColFmt,
                           float rFactor)
    : m_pTable(pLineTable)
    , m_pColFmt(pColFmt)
    , m_rFactor(rFactor)
//
    , m_itCur(pLineTable->End())
    , m_rCurTime(0.0f)
	, m_uxCurPos(0.0f)
    , m_uxRemovable(0.0f)
    //, m_itFirstTimed(pLineTable->End())
    //, m_itFirstObj(pLineTable->End())
{
}

lmLineSpacer::~lmLineSpacer()
{
}

void lmLineSpacer::ComputeLineSpacing(lmLUnits uSpaceAfterProlog)
{
    InitializeForTraversing();
    ProcessNonTimedAtProlog(uSpaceAfterProlog);
    ProcessTimedAtCurrentTimepos();
    while (m_itCur != m_pTable->End())
    {
        ProcessNonTimedAtCurrentTimepos();
        ProcessTimedAtCurrentTimepos();
    }
}

void lmLineSpacer::InitializeForTraversing()
{
    //initialize iteration control data, to traverse by timepos

    m_rCurTime = 0.0f;
    m_itCur = m_pTable->Begin();
    m_uxCurPos = m_pTable->GetLineStartPosition() + m_pTable->GetSpaceAtBeginning();
}

void lmLineSpacer::ProcessNonTimedAtCurrentTimepos()
{
    //Non-timed objects behave as if they were right aligned, joined to next timed
    //object to arrive. Spacing between the timed objects that enclose these non-timed
    //should be maintained as if the non-timed objs didn't exist. If not enough space,
    //variable space should be removed, starting from last non-timed and continuing 
    //backwards.

    if (CurrentObjectIsNonTimed())
    {
        ComputeMaxAndMinOcuppiedSpace();
        PositionNonTimed();
    }
}

void lmLineSpacer::ComputeMaxAndMinOcuppiedSpace()
{
	//Starting at current position, explores the not-timed objects until next timed
    //or end of line. Computes the maximum and minimum space they could occupy.
    //Current position is not altered

    m_uxMaxOcuppiedSpace = 0.0f;
    m_uxMinOcuppiedSpace = 0.0f;
    lmLineEntryIterator it = m_itCur;
	while (IsNonTimedObject(it))
    {
        (*m_itCur)->AssignFixedAndVariableSpace(m_pColFmt, m_rFactor);
        lmLUnits uxMax = (*m_itCur)->GetTotalSize();
        m_uxMaxOcuppiedSpace += uxMax;
        m_uxMinOcuppiedSpace += uxMax - (*m_itCur)->GetVariableSpace();
        ++it;
    }
}

void lmLineSpacer::PositionNonTimed()
{
    if (m_uxRemovable >= m_uxMaxOcuppiedSpace)
    {
        PositionUsingMaxSpaceWithShift(m_uxRemovable - m_uxMaxOcuppiedSpace);
    }
    else if (m_uxRemovable >= m_uxMinOcuppiedSpace)
    {
        lmLUnits uShift = m_uxRemovable - m_uxMinOcuppiedSpace;
        PositionUsingMinSpaceWithShift(uShift);
    }
    else
    {
        PositionUsingMinSpaceWithShift(0.0f);
    }
}

void lmLineSpacer::PositionUsingMaxSpaceWithShift(lmLUnits uShift)
{
    lmLUnits uxNextPos = m_uxCurPos - m_uxRemovable + uShift;
	while (CurrentObjectIsNonTimed())
    {
        (*m_itCur)->AssignFixedAndVariableSpace(m_pColFmt, m_rFactor);
        (*m_itCur)->RepositionAt(uxNextPos);

        uxNextPos += (*m_itCur)->GetTotalSize();
        ++m_itCur;
    }

    //update iteration data
    m_uxCurPos = uxNextPos;
    m_uxRemovable = 0.0f;
}

void lmLineSpacer::PositionUsingMinSpaceWithShift(lmLUnits uShift)
{
    lmLUnits uxNextPos = m_uxCurPos - m_uxRemovable + uShift;
	while (CurrentObjectIsNonTimed())
    {
        (*m_itCur)->AssignFixedAndVariableSpace(m_pColFmt, m_rFactor);
        (*m_itCur)->SetVariableSpace(0.0f);
        (*m_itCur)->RepositionAt(uxNextPos);

        uxNextPos += (*m_itCur)->GetTotalSize();
        ++m_itCur;
    }

    //update iteration data
    m_uxCurPos = uxNextPos;
    m_uxRemovable = 0.0f;
}

void lmLineSpacer::ProcessNonTimedAtProlog(lmLUnits uSpaceAfterProlog)
{
    if (CurrentObjectIsNonTimed())
    {
        lmLUnits uxNextPos = m_uxCurPos;
	    while (CurrentObjectIsNonTimed())
        {
            (*m_itCur)->AssignFixedAndVariableSpace(m_pColFmt, m_rFactor);
            (*m_itCur)->RepositionAt(uxNextPos);

            uxNextPos += (*m_itCur)->GetTotalSize();
            ++m_itCur;
        }

        //update iteration data and add some additional space after prolog
        m_uxCurPos = uxNextPos + uSpaceAfterProlog;
        m_uxRemovable = uSpaceAfterProlog;
   }
}

void lmLineSpacer::ProcessTimedAtCurrentTimepos()
{
	//Starting at current position, explores the line to set the position of all timed
    //objects placed at current time, until we reach a time greater that current
    //time or end of line

    lmLUnits uxRequiredPos = m_uxCurPos + ComputeShiftToAvoidOverlapWithPrevious();
    lmLUnits uxNextPos = uxRequiredPos;
    lmLUnits uxMargin = 0.0f;
    const lmLineEntryIterator itEnd = m_pTable->End();
	while (m_itCur != itEnd && IsEqualTime((*m_itCur)->m_rTimePos, m_rCurTime))
    {
        //assign it the required position
		(*m_itCur)->m_xLeft = uxRequiredPos + (*m_itCur)->m_uxAnchor;

        //assign fixed and variable after space to current object
        (*m_itCur)->AssignFixedAndVariableSpace(m_pColFmt, m_rFactor);

		//determine start position for next object
		uxNextPos = wxMax(uxNextPos, (*m_itCur)->m_xFinal);

        //determine margin for accidentals, clefs, etc. after this timed obj
        uxMargin = (uxMargin==0.0f ? 
                        (*m_itCur)->m_uVariableSpace
                        : wxMin(uxMargin, (*m_itCur)->m_uVariableSpace) );

        m_itCur++;
    }

    //update iteration data
    m_uxCurPos = uxNextPos;
    m_uxRemovable = uxMargin;
    if (m_itCur != itEnd)
        m_rCurTime = (*m_itCur)->m_rTimePos;

}

lmLUnits lmLineSpacer::ComputeShiftToAvoidOverlapWithPrevious()
{
	//Starting at current position, explores the objects placed at current time
    //to check if there is enought removable space to deal with any anchor left shifted
    //object. If not, computes the required additional space that should be added to
    //'removable' space.

    lmLineEntryIterator it = m_itCur;
    lmLUnits uxNextPos = m_uxCurPos;
    lmLUnits uxShift = 0.0f;
    const lmLineEntryIterator itEnd = m_pTable->End();
	while (it != itEnd && IsEqualTime((*it)->m_rTimePos, m_rCurTime))
    {
        lmLUnits uAnchor = - (*it)->m_uxAnchor;     // > 0 if need to shift left
        if (uAnchor > 0.0f && m_uxRemovable < uAnchor)
            uxShift = wxMax(uxShift, uAnchor - m_uxRemovable);

        it++;
    }
    return uxShift;
}

