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
#pragma implementation "SystemFormatter.h"
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
#include "SystemFormatter.h"
#include "ShapeNote.h"
#include "BoxSlice.h"

//access to logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;

#if defined(_LM_DEBUG_)

#include <wx/file.h>

// access to paths
#include "../globals/Paths.h"
extern lmPaths* g_pPaths;

#endif

class lmBreaksTable;

#define lmDUMP_TABLES   0

#define lmNO_DURATION   100000000000.0f     //any impossible high value
#define lmNO_TIME       100000000000.0f     //any impossible high value
#define lmNO_POSITION   100000000000.0f     //any impossible high value

//typedef struct
//{
//} lmLayoutOptions;
//
//const m_LayoutOpt =

//spacing function parameters
//-----------------------------------------------
    //TODO: User options
const float lmDMIN = (float)e32thDuration;      //Dmin: min. duration to consider
const lmTenths lmMIN_SPACE = 10.0f;		//Smin: space for Dmin
const lmTenths lmSPACE_AFTER_PROLOG = 25.0f;        //The first note in each bar should be about one note-head's width away from the barline.
const lmTenths lmSPACE_AFTER_INTERMEDIATE_CLEF = 20.0f;
const lmTenths lmMIN_SPACE_BETWEEN_NOTE_AND_CLEF = 10.0f;
const lmTenths lmEXCEPTIONAL_MIN_SPACE = 2.5f;



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

lmLineEntry::lmLineEntry(lmStaffObj* pSO, lmShape* pShape,
                         bool fProlog)
    : m_fIsBarlineEntry(false)
    , m_pSO(pSO)
    , m_pShape(pShape)
	, m_fProlog(fProlog)
    , m_xFinal(0.0f)
    , m_uFixedSpace(0.0f)
    , m_uVariableSpace(0.0f)
    , m_rTimePos((pSO && pSO->IsAligned()) ? pSO->GetTimePos() : -1.0f )
    , m_uSize(pShape ? pShape->GetWidth() : 0.0f )
    , m_xLeft(pShape ? pShape->GetXLeft() : 0.0f )
    , m_uxAnchor((pSO && pSO->IsNote()) ? - pSO->GetAnchorPos(): 0.0f )
{
}

void lmLineEntry::AssignFixedAndVariableSpace(lmColumnFormatter* pColFmt, float rFactor)
{
	//assign fixed and variable after spaces to this object and compute the xFinal pos

    if (m_fIsBarlineEntry)
    {
		if (!m_pSO)
            m_uSize = 0.0f;
    }
    else
    {
		if (!m_pSO->IsVisible())
            AssignNoSpace();
		else
		{
			if (m_pSO->IsNoteRest())
			{
				SetNoteRestSpace(pColFmt, rFactor);
			}
			else if (m_pSO->IsClef() || m_pSO->IsKeySignature() || m_pSO->IsTimeSignature())
			{
                m_uFixedSpace = pColFmt->TenthsToLogical(lmEXCEPTIONAL_MIN_SPACE, 1);
                m_uVariableSpace = pColFmt->TenthsToLogical(lmMIN_SPACE, 1) - m_uFixedSpace;
			}
			else if (m_pSO->IsSpacer() || m_pSO->IsScoreAnchor())
			{
                m_uFixedSpace = 0.0f;
                m_uVariableSpace = m_uSize;
			}
			else
                AssignNoSpace();
		}
    }

    //compute final position
    m_xFinal = m_xLeft + GetTotalSize();
}

void lmLineEntry::SetNoteRestSpace(lmColumnFormatter* pColFmt, float rFactor)
{
    AssignMinimumFixedSpace(pColFmt);
    lmLUnits uIdeal = ComputeIdealDistance(pColFmt, rFactor);
    AssignVariableSpace(uIdeal);
}

void lmLineEntry::AssignMinimumFixedSpace(lmColumnFormatter* pColFmt)
{
    m_uFixedSpace = pColFmt->TenthsToLogical(lmEXCEPTIONAL_MIN_SPACE, 1);
}

void lmLineEntry::AssignVariableSpace(lmLUnits uIdeal)
{
    m_uVariableSpace = uIdeal - m_uSize - m_uFixedSpace - m_uxAnchor;
    if (m_uVariableSpace < 0)
        m_uVariableSpace = 0.0f;
}

void lmLineEntry::AssignNoSpace()
{
    //Doesn't have after space requirements
    m_uFixedSpace = 0.0f;
    m_uVariableSpace = 0.0f;

    //Doesn't consume time-pos grid space.
    m_uSize = 0.0f;
}

lmLUnits lmLineEntry::ComputeIdealDistance(lmColumnFormatter* pColFmt, float rFactor)
{
    if (pColFmt->IsProportionalSpacing())
        return ComputeIdealDistanceProportional(pColFmt, rFactor);
    else
        return ComputeIdealDistanceFixed(pColFmt);
}

lmLUnits lmLineEntry::ComputeIdealDistanceFixed(lmColumnFormatter* pColFmt)
{
	int iStaff = m_pSO->GetStaffNum();
    return pColFmt->TenthsToLogical(pColFmt->GetFixedSpacingValue(), iStaff);
}

lmLUnits lmLineEntry::ComputeIdealDistanceProportional(lmColumnFormatter* pColFmt,
                                                       float rFactor)
{
	static const float rLog2 = 0.3010299956640f;		// log(2)
	int iStaff = m_pSO->GetStaffNum();

	//spacing function:   Space(Di) = Smin*[1 + A*log2(Di/Dmin)]
	lmLUnits uSmin = pColFmt->TenthsToLogical(lmMIN_SPACE, iStaff);
    float rVar = log(((lmNoteRest*)m_pSO)->GetDuration() / lmDMIN) / rLog2;     //log2(Di/Dmin)
    if (rVar > 0.0f)
        return uSmin * (1.0f + rFactor * rVar);
    else
        return uSmin;
}

void lmLineEntry::RepositionAt(lmLUnits uxNewXLeft)
{
    m_xLeft = uxNewXLeft;
    m_xFinal = m_xLeft + GetTotalSize();
}

void lmLineEntry::MoveShape()
{
    if (m_pSO && m_pShape)
    {
        lmLUnits uShift = m_xLeft - m_pShape->GetXLeft();
        m_pSO->StoreOriginAndShiftShapes( uShift, m_pShape->GetOwnerIDX() );
    }
}

wxString lmLineEntry::DumpHeader()
{
    //         ...+  ..+   ...+ ..+   +  ..........+........+........+........+........+........+........+........+......+
    return _T("Item    Type      ID Prolog   Timepos  xAnchor    xLeft     Size  SpFixed    SpVar    Space   xFinal ShpIdx\n");
}

lmLUnits lmLineEntry::GetShiftToNoteRestCenter()
{
    if (m_pSO && m_pSO->IsNoteRest())
    {
        //determine notehead width or rest width
        lmLUnits uxWidth = 0.0f;
        if (m_pSO->IsRest())
            uxWidth = m_pShape->GetWidth();
        else if (m_pSO->IsNote())
            uxWidth = ((lmShapeNote*)m_pShape)->GetNoteHead()->GetWidth();

        return uxWidth / 2.0f;
    }
    else
        return 0.0f;
}

wxString lmLineEntry::Dump(int iEntry)
{
    wxString sMsg = wxString::Format(_T("%4d: "), iEntry);
    if (m_fIsBarlineEntry)
    {
        sMsg += _T("  Omega");
        if (m_pSO)
            sMsg += wxString::Format(_T("%3d          "), m_pSO->GetScoreObjType() );
        else
            sMsg += _T("  -          ");
    }
    else
    {
		sMsg += wxString::Format(_T("  pSO %4d %3d   %s  "),
								m_pSO->GetScoreObjType(),
								m_pSO->GetID(),
								(m_fProlog ? _T("S") : _T(" ")) );
    }

    sMsg += wxString::Format(_T("%11.2f %8.2f %8.2f %8.2f %8.2f %8.2f %8.2f %8.2f"),
                m_rTimePos, m_uxAnchor, m_xLeft, m_uSize, m_uFixedSpace,
                m_uVariableSpace, GetTotalSize(), m_xFinal );

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
	for (it = m_pLineTable->Begin(); it != itEnd && IsLowerTime((*it)->GetTimepos(), 0.0f); ++it);
    if (it == itEnd) return;

    //process current time
    float rTime = (*it)->GetTimepos();
    lmLUnits uxStart = (*it)->GetPosition();
    lmLUnits uxWidth = (*it)->GetShapeSize();
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
		if (IsEqualTime((*it)->GetTimepos(), rTime) || IsLowerTime((*it)->GetTimepos(), 0.0f))
        {
		    //skip any not-timed entry
            if (IsEqualTime((*it)->GetTimepos(), rTime))
            {
                uxWidth = wxMax(uxWidth, (*it)->GetShapeSize());
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
            rTime = (*it)->GetTimepos();
            uxStart = (*it)->GetPosition();
            uxWidth = (*it)->GetShapeSize();
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
//lmLineTable:
//  An object to encapsulate positioning data for a line
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

lmLineEntry* lmLineTable::AddEntry(lmStaffObj* pSO, lmShape* pShape, bool fProlog)
{
    lmLineEntry* pEntry = new lmLineEntry(pSO, pShape, fProlog);
    PushBack(pEntry);
	return pEntry;
}

lmLineEntry* lmLineTable::AddFinalEntry(lmStaffObj* pSO, lmShape* pShape)
{
    lmLineEntry* pEntry = new lmLineEntry(pSO, pShape, false);
    pEntry->MarkAsBarlineEntry();
    PushBack(pEntry);
	return pEntry;
}

bool lmLineTable::ContainsBarline()
{
    lmLineEntry* pEntry = GetLastEntry();
    return pEntry->IsBarlineEntry() && pEntry->HasBarline();
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
	//Return the size of the measure represented by this line or zero if invalid line

	if (m_LineEntries.size() > 0 && m_LineEntries.back()->IsBarlineEntry())
        return m_LineEntries.back()->m_xFinal - GetLineStartPosition();
    else
        return 0.0f;
}


//=====================================================================================
//lmColumnFormatter
//=====================================================================================

lmColumnFormatter::lmColumnFormatter(lmColumnStorage* pStorage, float rSpacingFactor,
                                     lmESpacingMethod nSpacingMethod, lmTenths nSpacingValue)
    : m_rSpacingFactor(rSpacingFactor)
    , m_nSpacingMethod(nSpacingMethod)
    , m_rSpacingValue(nSpacingValue)
    , m_pColStorage(pStorage)
{
}

lmColumnFormatter::~lmColumnFormatter()
{
    DeleteLineSpacers();
}

lmLUnits lmColumnFormatter::TenthsToLogical(lmTenths rTenths, int nStaff)
{
    return m_pColStorage->TenthsToLogical(rTenths, nStaff);
}

void lmColumnFormatter::DeleteLineSpacers()
{
    lmLineSpacersIterator it;
    for (it = m_LineSpacers.begin(); it != m_LineSpacers.end(); ++it)
        delete *it;
    m_LineSpacers.clear();
}

bool lmColumnFormatter::IsThereBarline()
{
    //returns true if there is at least one line containing a barline

    for (lmLinesIterator it=m_pColStorage->Begin(); it != m_pColStorage->End(); ++it)
    {
        if ((*it)->ContainsBarline())
            return true;
    }
    return false;
}

void lmColumnFormatter::DoSpacing(bool fTrace)
{
    //computes the minimum space required by this column

#if lmDUMP_TABLES
    wxLogMessage( m_pColStorage->DumpColumnStorage() );
#endif

    m_uMinColumnSize = ComputeSpacing();

#if lmDUMP_TABLES
    wxLogMessage( m_pColStorage->DumpColumnStorage() );
#endif
}

lmLUnits lmColumnFormatter::ComputeSpacing()
{
    //Spacing algorithm. Returns the resulting minimum column width

    CreateLineSpacers();
    ProcessNonTimedAtProlog();
    ProcessTimedAtCurrentTimepos();
    while (ThereAreObjects())
    {
        ProcessNonTimedAtCurrentTimepos();
        ProcessTimedAtCurrentTimepos();
    }

    DeleteLineSpacers();
	return m_pColStorage->GetColumnWitdh();
}

void lmColumnFormatter::CreateLineSpacers()
{
    const lmLinesIterator itEnd = m_pColStorage->End();
    for (lmLinesIterator it=m_pColStorage->Begin(); it != itEnd; ++it)
	{
        lmLineSpacer* pLinSpacer = new lmLineSpacer(*it, this, m_rSpacingFactor);
        m_LineSpacers.push_back(pLinSpacer);
    }
}

void lmColumnFormatter::ProcessNonTimedAtProlog()
{
    lmLUnits uSpaceAfterProlog = TenthsToLogical(lmSPACE_AFTER_PROLOG, 1);
    m_rCurrentTime = lmNO_TIME;           //any impossible high value
    m_rCurrentPos = 0.0f;
    for (lmLineSpacersIterator it=m_LineSpacers.begin(); it != m_LineSpacers.end(); ++it)
	{
        (*it)->ProcessNonTimedAtProlog(uSpaceAfterProlog);
        lmLUnits uxNextPos = (*it)->GetNextPosition();
        m_rCurrentTime = wxMin(m_rCurrentTime, (*it)->GetNextAvailableTime());
        m_rCurrentPos = wxMax(m_rCurrentPos, uxNextPos);
    }
}

void lmColumnFormatter::ProcessTimedAtCurrentTimepos()
{
    m_fThereAreObjects = false;
    float rNextTime = lmNO_TIME;           //any impossible high value
    lmLUnits uxPosForNextTime = lmNO_POSITION;    //any impossible high value
    for (lmLineSpacersIterator it=m_LineSpacers.begin(); it != m_LineSpacers.end(); ++it)
	{
        if ((*it)->CurrentTimeIs(m_rCurrentTime) && (*it)->ThereAreTimedObjs())
        {
            (*it)->ProcessTimedAtCurrentTimepos(m_rCurrentPos);
            lmLUnits uxNextPos = (*it)->GetNextPosition();
            uxPosForNextTime = wxMin(uxPosForNextTime, uxNextPos);
        }
        if ((*it)->ThereAreMoreObjects())
        {
            m_fThereAreObjects = true;
            rNextTime = wxMin(rNextTime, (*it)->GetNextAvailableTime());
        }
    }

    m_rCurrentTime = rNextTime;
    if (uxPosForNextTime < lmNO_POSITION)
        m_rCurrentPos = uxPosForNextTime;
}

void lmColumnFormatter::ProcessNonTimedAtCurrentTimepos()
{
    lmLUnits uxPosForNextTime = 0.0f;
    for (lmLineSpacersIterator it=m_LineSpacers.begin(); it != m_LineSpacers.end(); ++it)
	{
        (*it)->ProcessNonTimedAtCurrentTimepos(m_rCurrentPos);
        lmLUnits uxNextPos = (*it)->GetNextPosition();
        uxPosForNextTime = wxMax(uxPosForNextTime, uxNextPos);
    }
    m_rCurrentPos = uxPosForNextTime;
}




//-------------------------------------------------------------------------------------
// lmSystemFormatter implementation
//-------------------------------------------------------------------------------------

lmSystemFormatter::lmSystemFormatter(float rSpacingFactor, lmESpacingMethod nSpacingMethod,
                                     lmTenths rSpacingValue)
    : m_rSpacingFactor(rSpacingFactor)
    , m_nSpacingMethod(nSpacingMethod)
    , m_rSpacingValue(rSpacingValue)
{
}

lmSystemFormatter::~lmSystemFormatter()
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

void lmSystemFormatter::EndOfSystemMeasurements()
{
    //caller informs that all data for this system has been suplied.
    //This is the right place to do any preparatory work, not to be repeated if re-spacing.

    //Nothing to do for current implementation
}

void lmSystemFormatter::StarBarMeasurements(int iCol, int nInstr, lmLUnits uxStart,
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

void lmSystemFormatter::IncludeObject(int iCol, int nInstr, lmStaffObj* pSO,
                                      lmShape* pShape, bool fProlog, int nStaff)
{
    //caller sends data about one staffobj in current bar, for column iCol [0..n-1]

    m_LinesBuilder[iCol]->IncludeObject(nInstr, pSO, pShape, fProlog, nStaff);
}

void lmSystemFormatter::IncludeBarlineAndTerminateBarMeasurements(int iCol, lmStaffObj* pSO,
                                                         lmShape* pShape, lmLUnits xStart)
{
    //caller sends lasts object to store in current bar, for column iCol [0..n-1].

    m_LinesBuilder[iCol]->CloseLine(pSO, pShape, xStart);
}

void lmSystemFormatter::TerminateBarMeasurementsWithoutBarline(int iCol, lmLUnits xStart)
{
    //caller informs that there are no barline and no more objects in column iCol [0..n-1].

    m_LinesBuilder[iCol]->CloseLine((lmStaffObj*)NULL, (lmShape*)NULL, xStart);
}

void lmSystemFormatter::DiscardMeasurementsForColumn(int iCol)
{
    //caller request to ignore measurements for column iCol [0..n-1]

    m_ColStorage[iCol]->Initialize();
    m_ColFormatters[iCol]->Initialize();
    m_LinesBuilder[iCol]->Initialize();
}

void lmSystemFormatter::DoColumnSpacing(int iCol, bool fTrace)
{
    m_ColFormatters[iCol]->DoSpacing(fTrace);
}

lmLUnits lmSystemFormatter::RedistributeSpace(int iCol, lmLUnits uNewStart)
{
    lmLUnits uNewBarSize = m_ColFormatters[iCol]->GetMinimumSize();
    lmColumnResizer oResizer(m_ColStorage[iCol], uNewBarSize);
	oResizer.RepositionShapes(uNewStart);

    lmLUnits uBarFinalPosition = uNewStart + uNewBarSize;
    return uBarFinalPosition;
}

void lmSystemFormatter::AddTimeGridToBoxSlice(int iCol, lmBoxSlice* pBSlice)
{
    //create the time-grid table and transfer it (and its ownership) to BoxSlice
    pBSlice->SetTimeGridTable( new lmTimeGridTable(m_ColStorage[iCol]) );
}

void lmSystemFormatter::IncrementColumnSize(int iCol, lmLUnits uIncr)
{
    m_ColFormatters[iCol]->IncrementColumnSize(uIncr);
}

lmLUnits lmSystemFormatter::GetStartPositionForColumn(int iCol)
{
    return m_ColStorage[iCol]->GetStartOfBarPosition();
}

lmLUnits lmSystemFormatter::GetMinimumSize(int iCol)
{
    return m_ColFormatters[iCol]->GetMinimumSize();
}

bool lmSystemFormatter::GetOptimumBreakPoint(int iCol, lmLUnits uAvailable,
                                        float* prTime, lmLUnits* puWidth)
{
    //return m_ColFormatters[iCol]->GetOptimumBreakPoint(uAvailable, prTime, puWidth);
    lmBreakPoints oBreakPoints(m_ColStorage[iCol]);
    if (oBreakPoints.FindOptimunBreakPointForSpace(uAvailable))
    {
        *prTime = oBreakPoints.GetOptimumTimeForFoundBreakPoint();
        *puWidth = oBreakPoints.GetOptimumPosForFoundBreakPoint();
        return false;
    }
    else
        return true;
}

bool lmSystemFormatter::ColumnHasBarline(int iCol)
{
    return m_ColFormatters[iCol]->IsThereBarline();
}

void lmSystemFormatter::ClearDirtyFlags(int iCol)
{
    m_ColStorage[iCol]->ClearDirtyFlags();
}

wxString lmSystemFormatter::DumpColumnData(int iCol)
{
    return m_ColStorage[iCol]->DumpColumnStorage();
}

//------------------------------------------------
// Debug build: methods coded only for Unit Tests
//------------------------------------------------
#if defined(_LM_DEBUG_)

int lmSystemFormatter::GetNumObjectsInColumnLine(int iCol, int iLine)
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

void lmColumnStorage::Initialize()
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

lmLUnits lmColumnStorage::GetColumnWitdh()
{
    lmLUnits uColWidth = 0;
	for (lmLinesIterator it = m_Lines.begin(); it != m_Lines.end(); ++it)
        uColWidth = wxMax(uColWidth, (*it)->GetLineWidth());

    return uColWidth;
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
        m_nStaffVoice[iS] = iS+1;
        StartLine(nInstr, iS+1, uxStart, uSpace);
    }
}

void lmLinesBuilder::StartLineInheritInitialPostionAndSpace(int nInstr, int nVoice)
{
    lmLUnits uxStart = m_pColStorage->Front()->GetLineStartPosition();
    lmLUnits uSpace = m_pColStorage->Front()->GetSpaceAtBeginning();
    StartLine(nInstr, nVoice, uxStart, uSpace);
}

void lmLinesBuilder::StartLine(int nInstr, int nVoice, lmLUnits uxStart, lmLUnits uSpace)
{
    //Start a new line for instrument nInstr (0..n-1), to be used for voice nVoice.
    //The line starts at position uxStart and space before first object must be uSpace.

    //create the line and store it
    lmLineTable* pLineTable = m_pColStorage->OpenNewLine(nInstr, nVoice, uxStart, uSpace);

    //created line is set as 'current line' to receive new data.
    m_itCurLine = m_pColStorage->GetLastLine();

    //as line is empty, pointer to last added entry is NULL
	m_pCurEntry = (lmLineEntry*)NULL;
}

void lmLinesBuilder::CloseLine(lmStaffObj* pSO, lmShape* pShape, lmLUnits xStart)
{
	//close current line.

    //m_pCurEntry = (*m_itCurLine)->AddEntry(lm_eOmega, pSO, pShape, false);
    m_pCurEntry = (*m_itCurLine)->AddFinalEntry(pSO, pShape);
    m_pCurEntry->SetPosition(xStart);
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
        StartLineInheritInitialPostionAndSpace(nInstr, nVoice);
    }

    //add new entry for this object
	m_pCurEntry = (*m_itCurLine)->AddEntry(pSO, pShape, fProlog);

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

lmLineResizer::lmLineResizer(lmLineTable* pTable, lmLUnits uOldBarSize,
                             lmLUnits uNewBarSize, lmLUnits uNewStart)
    : m_pTable(pTable)
    , m_uOldBarSize(uOldBarSize)
    , m_uNewBarSize(uNewBarSize)
    , m_uNewStart(uNewStart)
{
}

float lmLineResizer::MovePrologShapes()
{
    //all non-timed entries, at beginning, marked as fProlog must be only re-located
    //returns the first timepos found after the prolog or 0 if no valid timepos

    lmLUnits uLineStartPos = m_pTable->GetLineStartPosition();
    lmLUnits uLineShift = m_uNewStart - uLineStartPos;
    lmLineEntryIterator it = m_pTable->Begin();
    while (it != m_pTable->End() && (*it)->GetTimepos() < 0.0f)
    {
        if ((*it)->m_pShape)
        {
            if ((*it)->m_fProlog)
            {
                lmLUnits uNewPos = uLineShift + (*it)->GetPosition();
                (*it)->RepositionAt(uNewPos);
                (*it)->MoveShape();
            }
            else
			    break;
        }
        ++it;
    }
    m_itCurrent = it;

    //return first timepos in this line
    if (it != m_pTable->End())
    {
        if ((*it)->GetTimepos() < 0.0f)
            return 0.0f;
        else
            return (*it)->GetTimepos();
    }
    else
        return 0.0f;
}

lmLUnits lmLineResizer::GetTimeLinePositionIfTimeIs(float rFirstTime)
{
    if (m_itCurrent != m_pTable->End() && (*m_itCurrent)->GetTimepos() == rFirstTime)
        return (*m_itCurrent)->GetPosition() - (*m_itCurrent)->GetAnchor();
    else
        return 0.0f;
}

void lmLineResizer::ReassignPositionToAllOtherObjects(lmLUnits uFizedSizeAtStart)
{
    if (m_itCurrent == m_pTable->End())
        return;

    //Compute proportion factor
    lmLUnits uLineStartPos = m_pTable->GetLineStartPosition();
    lmLUnits uLineShift = m_uNewStart - uLineStartPos;
    lmLUnits uDiscount = uFizedSizeAtStart - uLineStartPos;
    float rProp = (m_uNewBarSize-uDiscount) / (m_uOldBarSize-uDiscount);

	//Reposition the remainder entries
    for (lmLineEntryIterator it = m_itCurrent; it != m_pTable->End(); ++it)
	{
        if ((*it)->IsBarlineEntry())
        {
            lmLUnits uNewPos = m_uNewStart + m_uNewBarSize - (*it)->GetShapeSize();
            (*it)->RepositionAt(uNewPos);
            (*it)->MoveShape();
        }
        else
        {
            lmLUnits uOldPos = (*it)->GetPosition() - (*it)->GetAnchor();
            lmLUnits uShift = uDiscount + (m_uNewStart + (uOldPos - uFizedSizeAtStart) * rProp) - uOldPos;
            lmLUnits uNewPos = uOldPos + uShift + (*it)->GetAnchor();;
            (*it)->RepositionAt(uNewPos);
            (*it)->MoveShape();
        }
    }
}

void lmLineResizer::InformAttachedObjs()
{
    //StaffObj shapes has been moved to their final positions. This method is invoked
    //to inform some attached AuxObjs (i.e. ties) so that they can compute their
    //final positions.

    for (lmLineEntryIterator it = m_pTable->Begin(); it != m_pTable->End(); ++it)
	{
        if (!(*it)->IsBarlineEntry())
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
    , m_itCur(pLineTable->End())
    , m_rCurTime(0.0f)
	, m_uxCurPos(0.0f)
    , m_uxRemovable(0.0f)
{
    InitializeForTraversing();
}

void lmLineSpacer::InitializeForTraversing()
{
    //initialize iteration control data, to traverse by timepos

    m_itCur = m_pTable->Begin();
    m_rCurTime = GetNextAvailableTime();
    m_uxCurPos = m_pTable->GetLineStartPosition() + m_pTable->GetSpaceAtBeginning();
    m_itNonTimedAtCurPos = m_pTable->End();
}

void lmLineSpacer::ProcessNonTimedAtCurrentTimepos(lmLUnits uxPos)
{
    //update current pos with new xPos required for column alignment
    m_uxRemovable += uxPos - m_uxCurPos;
    m_uxCurPos = uxPos;

    //proceed if there are non-timed objects
    if (CurrentObjectIsNonTimed())
    {
        ComputeMaxAndMinOcuppiedSpace();
        PositionNonTimed();
    }
}

lmLUnits lmLineSpacer::GetNextPosition()
{
    return m_uxCurPos;
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
        (*it)->AssignFixedAndVariableSpace(m_pColFmt, m_rFactor);
        lmLUnits uxMax = (*it)->GetTotalSize();
        m_uxMaxOcuppiedSpace += uxMax;
        m_uxMinOcuppiedSpace += uxMax - (*it)->GetVariableSpace();
        ++it;
    }
}

void lmLineSpacer::PositionNonTimed()
{
    m_itNonTimedAtCurPos = m_itCur;
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
    m_uxNotTimedFinalPos = m_uxCurPos;
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

void lmLineSpacer::ProcessTimedAtCurrentTimepos(lmLUnits uxPos)
{
	//Starting at current position, explores the line to set the position of all timed
    //objects placed at current time, until we reach a time greater that current
    //time or end of line

    //update current pos with new xPos required for column alignment
    m_uxRemovable += uxPos - m_uxCurPos;
    m_uxCurPos = uxPos;

    DragAnyPreviousCleftToPlaceItNearThisNote();

    //procced to process this timepos
    lmLUnits uxRequiredPos = m_uxCurPos + ComputeShiftToAvoidOverlapWithPrevious();
    lmLUnits uxNextPos = uxRequiredPos;
    lmLUnits uxMinNextPos = 0.0f;
    lmLUnits uxMargin = 0.0f;
    lmLineEntryIterator itLast;
	while (ThereAreTimedObjs())
    {
        //AssignPositionToCurrentEntry();
		(*m_itCur)->SetPosition( uxRequiredPos + (*m_itCur)->GetAnchor() );

        //AssignFixedAndVariableSpacingToCurrentEntry();
        (*m_itCur)->AssignFixedAndVariableSpace(m_pColFmt, m_rFactor);

        //DetermineSpaceRequirementsForCurrentEntry();
        if ((*m_itCur)->IsNoteRest())
		    uxNextPos = wxMax(uxNextPos, (*m_itCur)->m_xFinal);
        else
            uxMinNextPos = wxMax(uxMinNextPos, (*m_itCur)->m_xFinal);

        uxMargin = (uxMargin==0.0f ?
                        (*m_itCur)->m_uVariableSpace
                        : wxMin(uxMargin, (*m_itCur)->m_uVariableSpace) );

        //AdvanceToNextEntry();
        itLast = m_itCur++;
    }

    //update iteration data
    if (uxNextPos == uxRequiredPos)     //No note/rest found
        m_uxCurPos = uxRequiredPos + uxMinNextPos;
    else
        m_uxCurPos = uxNextPos;

    m_uxRemovable = uxMargin;
    m_rCurTime = GetNextAvailableTime();
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
        lmLUnits uAnchor = - (*it)->GetAnchor();     // > 0 if need to shift left
        if (uAnchor > 0.0f && m_uxRemovable < uAnchor)
            uxShift = wxMax(uxShift, uAnchor - m_uxRemovable);

        it++;
    }
    return uxShift;
}

void lmLineSpacer::ShiftNonTimed(lmLUnits uxShift)
{
    lmLineEntryIterator it = m_itNonTimedAtCurPos;
	while (IsNonTimedObject(it))
    {
        lmLUnits uxCurPos = (*it)->GetPosition();
        (*it)->RepositionAt(uxCurPos + uxShift);
        ++it;
    }
}

float lmLineSpacer::GetNextAvailableTime()
{
	lmLineEntryIterator it = m_itCur;
    if (it != m_pTable->End())
    {
        while (IsNonTimedObject(it))
            ++it;

        if (IsTimedObject(it))
            return (*it)->GetTimepos();
        else
            return lmNO_TIME;
    }
    else
        return lmNO_TIME;
}

void lmLineSpacer::DragAnyPreviousCleftToPlaceItNearThisNote()
{
    if (m_itNonTimedAtCurPos != m_pTable->End() && m_uxCurPos > m_uxNotTimedFinalPos)
    {
        ShiftNonTimed(m_uxCurPos - m_uxNotTimedFinalPos);
    }
    m_itNonTimedAtCurPos = m_pTable->End();     //no longer needed. Discart value now to avoid problmes at next timepos
}



//----------------------------------------------------------------------------------------
//lmBreakPoints:
//  encloses the algorithm to determine optimum break points to split a column
//----------------------------------------------------------------------------------------

lmBreakPoints::lmBreakPoints(lmColumnStorage* pColStorage)
    : m_pColStorage(pColStorage)
    , m_pOptimumEntry((lmBeaksTimeEntry*)NULL)
    , m_pPossibleBreaks((lmBreaksTable*)NULL)
{
}

lmBreakPoints::~lmBreakPoints()
{
    DeleteBreaksTable();
}

void lmBreakPoints::DeleteBreaksTable()
{
    if (m_pPossibleBreaks)
    {
        delete m_pPossibleBreaks;
        m_pPossibleBreaks = (lmBreaksTable*)NULL;
    }
}

bool lmBreakPoints::FindOptimunBreakPointForSpace(lmLUnits uAvailable)
{
    //returns false if no break point found (exceptional case).
    //In all other cases updates m_pOptimumEntry and returns true

    if (!m_pPossibleBreaks)
        ComputeBreaksTable();

    //select highest entry with space <= uAvailable
    lmBeaksTimeEntry* pBTE = m_pPossibleBreaks->GetFirst();
    m_pOptimumEntry = (lmBeaksTimeEntry*)NULL;
    while (pBTE && pBTE->uxEnd <= uAvailable)
    {
        m_pOptimumEntry = pBTE;
        pBTE = m_pPossibleBreaks->GetNext();
    }
    if (!m_pOptimumEntry)
        return false;        //big problem: no break points!

    //wxLogMessage(_T("[lmColumnFormatter::GetOptimumBreakPoint] uAvailable=%.2f, returned=%.2f, time=%.2f"),
    //             uAvailable, m_pOptimumEntry->uxEnd, m_pOptimumEntry->rTimepos);

    return true;       //no problems. There are break points
}

float lmBreakPoints::GetOptimumTimeForFoundBreakPoint()
{
    return m_pOptimumEntry->rTimepos;
}

lmLUnits lmBreakPoints::GetOptimumPosForFoundBreakPoint()
{
    return m_pOptimumEntry->uxEnd;
}

void lmBreakPoints::ComputeBreaksTable()
{
    //This method computes the BreaksTable. This is a table sumarizing break points
    //information, that is, suitable places through all staves and voices where it is
    //possible to break a system and start a new one. The best break locations are
    //usually are the bar lines common to all staves. But in certain rare cases (i.e.
    //scores without time signature or having instrumens not sharing a common
    //time signature, or when it is requested to render the score in very narrow
    //paper, etc.) it is necessary to split music in unnusual points.

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
    if (m_pPossibleBreaks)
        DeleteBreaksTable();
    m_pPossibleBreaks = new lmBreaksTable();

    std::vector<lmBreaksTable*>::iterator itBT;
    for (itBT = partialTables.begin(); itBT != partialTables.end(); ++itBT)
    {
        if (m_pPossibleBreaks->IsEmpty())
        {
            //just copy entries
            lmBeaksTimeEntry* pEP = (*itBT)->GetFirst();       //pEP Entry from Partial list
            while (pEP)
            {
                m_pPossibleBreaks->AddEntry(pEP);
                pEP = (*itBT)->GetNext();
            }
        }
        else
        {
            //merge current table with total table
            //lmBeaksTimeEntry* pEP = (*itBT)->GetFirst();       //pEP Entry from Partial list
            //while (pEP)
            //{
            //    m_pPossibleBreaks->AddEntry(pEP);
            //    pEP = (*itBT)->GetNext();
            //}
        }
    }


    //Delete partial tables, no longer needed
    for (itBT = partialTables.begin(); itBT != partialTables.end(); ++itBT)
        delete *itBT;
    partialTables.clear();

    //wxLogMessage(_T("Total Breaks Table:"));
    //wxLogMessage( m_pPossibleBreaks->Dump() );

    //Step 3. Sort breaks table by priority and final x position
    //TODO
}



//----------------------------------------------------------------------------------------
//lmDirtyFlagsCleaner:
//
//----------------------------------------------------------------------------------------

lmDirtyFlagsCleaner::lmDirtyFlagsCleaner(lmColumnStorage* pColStorage)
    : m_pColStorage(pColStorage)
{
}

void lmDirtyFlagsCleaner::ClearDirtyFlags()
{
    //Clear flag 'Dirty' in all StaffObjs of this column. This has nothing to do with
    //formatting, but its is a convenient place for doing this as all affected objects
    //are those in the column

    m_pColStorage->ClearDirtyFlags();

	//for (lmLinesIterator it = m_Lines.begin(); it != m_Lines.end(); ++it)
	//	(*it)->ClearDirtyFlags();
}




//----------------------------------------------------------------------------------------
//lmTimeGridTable:
//  A table with the relation timepos <-> position for all valid positions to insert
//  a note.
//  This object is responsible for supplying all valid timepos and their positions so
//  that other objects (in fact only lmBoxSlice) could:
//      a) Determine the timepos to assign to a mouse click in a certain position.
//      b) Draw a grid of valid timepos
//----------------------------------------------------------------------------------------

lmTimeGridTable::lmTimeGridTable(lmColumnStorage* pColStorage)
    : m_pColStorage(pColStorage)
{
    //build the table

    CreateLineExplorers();
    while (ThereAreObjects())
    {
        SkipNonTimedAtCurrentTimepos();
        if (TimedObjectsFound())
        {
            FindShortestNoteRestAtCurrentTimepos();
            CreateTableEntry();
        }
    }
    InterpolateMissingTimes();
    DeleteLineExplorers();
}

lmTimeGridTable::~lmTimeGridTable()
{
    m_PosTimes.clear();
}

bool lmTimeGridTable::ThereAreObjects()
{
    std::vector<lmTimeGridLineExplorer*>::iterator it;
    for (it = m_LineExplorers.begin(); it != m_LineExplorers.end(); ++it)
    {
        if ((*it)->ThereAreObjects())
            return true;
    }
    return false;
}

void lmTimeGridTable::CreateTableEntry()
{
    lmPosTimeItem tPosTime = {m_rCurrentTime, m_rMinDuration, m_uCurPos };
    m_PosTimes.push_back(tPosTime);
}

void lmTimeGridTable::DeleteLineExplorers()
{
    std::vector<lmTimeGridLineExplorer*>::iterator it;
    for (it = m_LineExplorers.begin(); it != m_LineExplorers.end(); ++it)
        delete *it;
    m_LineExplorers.clear();
}

void lmTimeGridTable::CreateLineExplorers()
{
    const lmLinesIterator itEnd = m_pColStorage->End();
    for (lmLinesIterator it=m_pColStorage->Begin(); it != itEnd; ++it)
	{
        lmTimeGridLineExplorer* pLinExplorer = new lmTimeGridLineExplorer(*it);
        m_LineExplorers.push_back(pLinExplorer);
    }
}

void lmTimeGridTable::SkipNonTimedAtCurrentTimepos()
{
    m_fTimedObjectsFound = false;
    std::vector<lmTimeGridLineExplorer*>::iterator it;
    for (it = m_LineExplorers.begin(); it != m_LineExplorers.end(); ++it)
	{
        m_fTimedObjectsFound |= (*it)->SkipNonTimedAtCurrentTimepos();
    }
}

void lmTimeGridTable::FindShortestNoteRestAtCurrentTimepos()
{
    GetCurrentTime();
    m_rMinDuration = lmNO_DURATION;
    m_uCurPos = lmNO_POSITION;
    std::vector<lmTimeGridLineExplorer*>::iterator it;
    for (it = m_LineExplorers.begin(); it != m_LineExplorers.end(); ++it)
	{
        if (m_rCurrentTime == (*it)->GetCurrentTime())
        {
            (*it)->FindShortestNoteRestAtCurrentTimepos();
            if (m_rMinDuration > (*it)->GetDurationForFoundEntry())
            {
                m_rMinDuration = (*it)->GetDurationForFoundEntry();
                m_uCurPos = wxMin(m_uCurPos, (*it)->GetPositionForFoundEntry());
            }
        }
    }
}

void lmTimeGridTable::GetCurrentTime()
{
    m_rCurrentTime = lmNO_TIME;
    std::vector<lmTimeGridLineExplorer*>::iterator it;
    for (it = m_LineExplorers.begin(); it != m_LineExplorers.end(); ++it)
	{
        m_rCurrentTime = wxMin(m_rCurrentTime, (*it)->GetCurrentTime());
    }
}

wxString lmTimeGridTable::Dump()
{
                      //   .......+.......+.......+
    wxString sDump = _T("\n timepos     Dur     Pos\n");
    std::vector<lmPosTimeItem>::iterator it;
    for (it = m_PosTimes.begin(); it != m_PosTimes.end(); ++it)
    {
        sDump += wxString::Format(_T("%8.2f %8.2f %8.2f\n"),
                                (*it).rTimepos, (*it).rDuration, (*it).uxPos );
    }
    return sDump;
}

float lmTimeGridTable::GetTimeForPosititon(lmLUnits uxPos)
{
    //timepos = 0 if measure is empty
    if (m_PosTimes.size() == 0)
        return 0.0f;

    //timepos = 0 if xPos < first entry xPos
    float rTime = 0.0f;
    lmLUnits uxPrev = m_PosTimes.front().uxPos;
    if (uxPos <= uxPrev)
        return rTime;

    //otherwise find in table
    std::vector<lmPosTimeItem>::iterator it = m_PosTimes.begin();
    for (++it; it != m_PosTimes.end(); ++it)
    {
        int uxLimit = uxPrev + ((*it).uxPos - uxPrev) / 2.0;
        if (uxPos <= uxLimit)
            return rTime;
        uxPrev = (*it).uxPos;
        rTime = (*it).rTimepos;
    }

    //if not found return last entry timepos
    return m_PosTimes.back().rTimepos;
}

void lmTimeGridTable::InterpolateMissingTimes()
{
    lmTimeInserter oInserter(m_PosTimes);
    oInserter.InterpolateMissingTimes();
}



//----------------------------------------------------------------------------------------
//lmTimeInserter
// helper class to interpolate missing entries
//----------------------------------------------------------------------------------------

lmTimeInserter::lmTimeInserter(std::vector<lmPosTimeItem>& oPosTimes)
    : m_PosTimes(oPosTimes)
{
}

void lmTimeInserter::InterpolateMissingTimes()
{
    for (int i=0; i < (int)m_PosTimes.size(); ++i)
    {
        float rNextTime = m_PosTimes[i].rTimepos + m_PosTimes[i].rDuration;
        if (!IsTimeInTable(rNextTime))
        {
            FindInsertionPoint(rNextTime);
            InsertTimeInterpolatingPosition(rNextTime);
        }
    }
}

bool lmTimeInserter::IsTimeInTable(float rTimepos)
{
    if (m_PosTimes.size() == 0)
        return false;

    std::vector<lmPosTimeItem>::iterator it;
    for (it=m_PosTimes.begin(); it != m_PosTimes.end(); ++it)
    {
        if (IsEqualTime(rTimepos, (*it).rTimepos))
            return true;
    }
    return false;
}

void lmTimeInserter::FindInsertionPoint(float rTimepos)
{
    m_uPositionBeforeInsertionPoint = m_PosTimes.front().uxPos;
    m_rTimeBeforeInsertionPoint = m_PosTimes.front().rTimepos;

    std::vector<lmPosTimeItem>::iterator it;
    for (it=m_PosTimes.begin(); it != m_PosTimes.end(); ++it)
    {
        if (IsHigherTime((*it).rTimepos, rTimepos))
            break;
        m_uPositionBeforeInsertionPoint = (*it).uxPos;
        m_rTimeBeforeInsertionPoint = (*it).rTimepos;
    }
    m_itInsertionPoint = it;
}

void lmTimeInserter::InsertTimeInterpolatingPosition(float rTimepos)
{
    lmPosTimeItem oItem;
    oItem.rTimepos = rTimepos;
    oItem.rDuration = 0.0f;
    oItem.uxPos = m_uPositionBeforeInsertionPoint;

    if (m_itInsertionPoint == m_PosTimes.end())
    {
        //insert at the end
        oItem.uxPos += 1000;       //TODO: Estimate space based on measure duration
        m_PosTimes.push_back(oItem);
    }
    else
    {
        //insert before item pointed by iterator
        float rTimeGap = (*m_itInsertionPoint).rTimepos - m_rTimeBeforeInsertionPoint;
        float rPosGap = (*m_itInsertionPoint).uxPos - m_uPositionBeforeInsertionPoint;
        float rTimeIncrement = rTimepos - m_rTimeBeforeInsertionPoint;
        oItem.uxPos += rTimeIncrement * (rPosGap / rTimeGap);
        m_PosTimes.insert(m_itInsertionPoint, oItem);
    }
}


//----------------------------------------------------------------------------------------
//lmTimeGridLineExplorer:
//  line traversal algorithm for creating the time-pos table
//----------------------------------------------------------------------------------------

lmTimeGridLineExplorer::lmTimeGridLineExplorer(lmLineTable* pLineTable)
    : m_pTable(pLineTable)
{
    m_itCur = m_pTable->Begin();
}

lmTimeGridLineExplorer::~lmTimeGridLineExplorer()
{
}

bool lmTimeGridLineExplorer::SkipNonTimedAtCurrentTimepos()
{
    //returns true if there are timed objects after the skipped non-timed

	while (CurrentObjectIsNonTimed())
        ++m_itCur;

    return CurrentObjectIsTimed();
}

bool lmTimeGridLineExplorer::FindShortestNoteRestAtCurrentTimepos()
{
    //returns true if there are more objects after current timepos

	if (CurrentObjectIsTimed())
    {
        m_rCurTime = (*m_itCur)->GetTimepos();
        m_uCurPos = (*m_itCur)->GetPosition() - (*m_itCur)->GetAnchor();
        m_rMinDuration = (*m_itCur)->GetDuration();
        m_uShiftToNoteRestCenter = (*m_itCur)->GetShiftToNoteRestCenter();

	    while (CurrentObjectIsTimed() && (*m_itCur)->GetTimepos() == m_rCurTime)
        {
            m_rMinDuration = wxMin(m_rMinDuration, (*m_itCur)->GetDuration());
            if (m_uShiftToNoteRestCenter == 0.0f)
                m_uShiftToNoteRestCenter = (*m_itCur)->GetShiftToNoteRestCenter();

            ++m_itCur;
        }
    }
    return ThereAreObjects();
}

float lmTimeGridLineExplorer::GetCurrentTime()
{
    if (CurrentObjectIsTimed())
        return (*m_itCur)->GetTimepos();
    else
        return lmNO_TIME;
}

float lmTimeGridLineExplorer::GetDurationForFoundEntry()
{
    return m_rMinDuration;
}

lmLUnits lmTimeGridLineExplorer::GetPositionForFoundEntry()
{
    return m_uCurPos + m_uShiftToNoteRestCenter;
}



//----------------------------------------------------------------------------------------
//lmColumnResizer: encapsulates the methods to recompute shapes positions so that the
//column will have the desired width, and to move the shapes to those positions
//----------------------------------------------------------------------------------------

lmColumnResizer::lmColumnResizer(lmColumnStorage* pColStorage, lmLUnits uNewBarSize)
    : m_pColStorage(pColStorage)
    , m_uNewBarSize(uNewBarSize)
{
}

void lmColumnResizer::RepositionShapes(lmLUnits uNewStart)
{
    m_uNewStart = uNewStart;
    m_uOldBarSize = m_pColStorage->GetColumnWitdh();

    CreateLineResizers();
    MovePrologShapesAndGetInitialTime();
    DetermineFixedSizeAtStartOfColumn();
    RepositionAllOtherShapes();
    DeleteLineResizers();
}

void lmColumnResizer::CreateLineResizers()
{
	for (lmLinesIterator it=m_pColStorage->Begin(); it != m_pColStorage->End(); ++it)
	{
        lmLineResizer* pResizer = new lmLineResizer(*it, m_uOldBarSize, m_uNewBarSize,
                                                    m_uNewStart);
        m_LineResizers.push_back(pResizer);
    }
}

void lmColumnResizer::MovePrologShapesAndGetInitialTime()
{
    m_rFirstTime = lmNO_TIME;
    std::vector<lmLineResizer*>::iterator itR;
	for (itR = m_LineResizers.begin(); itR != m_LineResizers.end(); ++itR)
	{
        m_rFirstTime = wxMin(m_rFirstTime, (*itR)->MovePrologShapes());
    }
}

void lmColumnResizer::DetermineFixedSizeAtStartOfColumn()
{
    m_uFixedPart = 0.0f;
    std::vector<lmLineResizer*>::iterator itR;
	for (itR = m_LineResizers.begin(); itR != m_LineResizers.end(); ++itR)
	{
        m_uFixedPart = wxMax(m_uFixedPart, (*itR)->GetTimeLinePositionIfTimeIs(m_rFirstTime));
    }
}

void lmColumnResizer::RepositionAllOtherShapes()
{
    std::vector<lmLineResizer*>::iterator itR;
	for (itR = m_LineResizers.begin(); itR != m_LineResizers.end(); ++itR)
		(*itR)->ReassignPositionToAllOtherObjects(m_uFixedPart);
}

void lmColumnResizer::DeleteLineResizers()
{
    std::vector<lmLineResizer*>::iterator itR;
	for (itR = m_LineResizers.begin(); itR != m_LineResizers.end(); ++itR)
		delete *itR;
    m_LineResizers.clear();
}

