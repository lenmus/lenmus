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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "Formatter4.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#else
#include <wx/debug.h>
#include <wx/list.h>
#include <wx/numdlg.h>      // for ::wxGetNumberFromUser
#endif

#include <vector>

#include "../score/Score.h"
#include "../score/Context.h"
#include "../score/Staff.h"
#include "../score/VStaff.h"
#include "../score/Instrument.h"
#include "SystemFormatter.h"
#include "SystemFormatter.h"
#include "Formatter4.h"
#include "BoxScore.h"
#include "BoxPage.h"
#include "BoxSystem.h"
#include "BoxSlice.h"
#include "BoxSliceInstr.h"
#include "ShapeLine.h"

//access to logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;

//-----------------------------------------------------------------------------------------
// Helper class: To determine "possible break locations" we have to traverse all staves in
// in parallel. This class keeps information about the current traversing position:
// num. of segment and position in the segment
//-----------------------------------------------------------------------------------------

#define lmNO_BREAK_TIME  100000000000000.0f         //any too big value

class lmSystemCursor
{
public:
    lmSystemCursor(lmScore* pScore);
    ~lmSystemCursor();

    bool ThereAreObjects();

    //locate context for first note in this staff, in current segment
	lmContext* GetStartOfColumnContext(int iInstr, int nStaff);

    //locate previous barline in this instrument
    lmBarline* GetPreviousBarline(int iInstr);

    //returns current absolute measure number (1..n) for VStaff
    int GetNumMeasure(int iInstr);

    //break time
    inline float GetBreakTime() { return m_rBreakTime; }
    inline void SetBreakTime(float rBreakTime) { m_rBreakTime = rBreakTime; }

    //iterators management
    inline lmSOIterator* GetIterator(int iInstr) { return m_Iterators[iInstr]; }
    void GoBackPrevPosition();
    void CommitCursors();
    void AdvanceAfterTimepos(float rTimepos);


private:
    std::vector<lmSOIterator*>  m_Iterators;
    std::vector<lmSOIterator*>  m_SavedIterators;
    float                       m_rBreakTime;       //last time to include in current column

};

lmSystemCursor::lmSystemCursor(lmScore* pScore)
{
    //create iterators and point to start of each instrument
    lmInstrument* pInstr = pScore->GetFirstInstrument();
    for (; pInstr; pInstr = pScore->GetNextInstrument())
    {
        lmSOIterator* pIT = pInstr->GetVStaff()->CreateIterator();
        pIT->AdvanceToMeasure(1);
        m_Iterators.push_back(pIT);
        m_SavedIterators.push_back( new lmSOIterator(pIT) );
    }

    m_rBreakTime = lmNO_BREAK_TIME;
}

lmSystemCursor::~lmSystemCursor()
{
    std::vector<lmSOIterator*>::iterator it;
    for (it = m_Iterators.begin(); it != m_Iterators.end(); ++it)
        delete *it;
    m_Iterators.clear();

    for (it = m_SavedIterators.begin(); it != m_SavedIterators.end(); ++it)
        delete *it;
    m_SavedIterators.clear();
}

bool lmSystemCursor::ThereAreObjects()
{
    //Returns true if there are any object not yet processed in any staff

    for (int i=0; i < (int)m_Iterators.size(); i++)
    {
        if (!m_Iterators[i]->EndOfCollection())
            return true;
    }
    return false;
}

lmContext* lmSystemCursor::GetStartOfColumnContext(int iInstr, int nStaff)
{
    //locate context for first note in this staff, in current segment

    lmSOIterator* pIT = new lmSOIterator( GetIterator(iInstr) );
    lmStaffObj* pSO = (lmStaffObj*)NULL;
    //AWARE: if we are in an empty segment (last segment) and we move back to previous
    //segment, it doesn't matter. In any case the context applying to found SO is the
    //right context!
    while(!pIT->EndOfCollection())
    {
        pSO = pIT->GetCurrent();
        if (pSO->IsOnStaff(nStaff))
            break;
        pIT->MovePrev();
    }
    delete pIT;

    if (pSO)
        return pSO->GetCurrentContext(nStaff);
    else
        return (lmContext*)NULL;
}

void lmSystemCursor::CommitCursors()
{
    //A column has been processed and it has been verified that it will be included in
    //currente system. Therefore, SystemCursor is informed to consolidate current
    //cursors' positions.
    //Old saved positions are no longer needed. Current position is going to be the
    //backup point, so save current cursors positions just in case we have to go back

    for (int i=0; i < (int)m_Iterators.size(); ++i)
    {
        delete m_SavedIterators[i];
        m_SavedIterators[i] = new lmSOIterator(m_Iterators[i]);
    }

    m_rBreakTime = lmNO_BREAK_TIME;
}

void lmSystemCursor::GoBackPrevPosition()
{
    //A column has been processed but there is not enough space for it in current system.
    //This method is invoked to reposition cursors back to last saved positions

    for (int i=0; i < (int)m_SavedIterators.size(); ++i)
    {
        delete m_Iterators[i];
        m_Iterators[i] = new lmSOIterator(m_SavedIterators[i]);
    }
}

lmBarline* lmSystemCursor::GetPreviousBarline(int iInstr)
{
    lmSOIterator* pIT = new lmSOIterator( GetIterator(iInstr) );
    lmStaffObj* pSO = (lmStaffObj*)NULL;
    while (!pIT->EndOfCollection())
    {
        pSO = pIT->GetCurrent();
        if (pSO->IsBarline() || pIT->FirstOfCollection())
            break;

        pIT->MovePrev();
    }
    delete pIT;

    if (pSO && pSO->IsBarline())
        return (lmBarline*)pSO;
    else
        return (lmBarline*)NULL;
}

int lmSystemCursor::GetNumMeasure(int iInstr)
{
    //returns current absolute measure number (1..n) for VStaff

    return m_Iterators[iInstr]->GetNumSegment() + 1;
}

void lmSystemCursor::AdvanceAfterTimepos(float rTimepos)
{
    //advance all iterators so that last processed timepos is rTimepos. That is, pointed
    //objects will be the firsts ones with timepos > rTimepos.

    //THIS METHOD IS NO LONGER USED. BUT IT WORKS.
    //Leaved just in case there is a need to use it again

    for (int i=0; i < (int)m_Iterators.size(); ++i)
    {
        lmSOIterator* pIT = m_Iterators[i];
        pIT->ResetFlags();
        while (!pIT->ChangeOfMeasure() && !pIT->EndOfCollection())
        {
            lmStaffObj* pSO = pIT->GetCurrent();
            if (IsHigherTime(pSO->GetTimePos(), rTimepos))
                break;
            pIT->MoveNext();
        }
    }
}




//-----------------------------------------------------------------------------------------
// lmFormatter5 implementation
//-----------------------------------------------------------------------------------------

lmFormatter5::lmFormatter5(lmPaper* pPaper)
    : lmFormatter(pPaper)
    , m_pSysCursor((lmSystemCursor*)NULL)
    , m_pBoxScore((lmBoxScore*)NULL)
{
    // set debugging options
    m_fDebugMode = g_pLogger->IsAllowedTraceMask(_T("Formater4"));
    m_nTraceMeasure = 1;

    //if (m_fDebugMode) {
    //    m_nTraceMeasure = ::wxGetNumberFromUser(
    //                            _T("Specify the measure to trace (0 for all measures)"),
    //                            _T("Measure: "),
    //                            _T("Debug Formatter4"),
    //                            0L);        // default value: all measures
    //}

}

lmFormatter5::~lmFormatter5()
{
    if (m_pSysCursor)
        delete m_pSysCursor;
    DeleteSystemFormatters();
}

void lmFormatter5::DeleteSystemFormatters()
{
    std::vector<lmSystemFormatter*>::iterator it;
    for (it=m_SysFormatters.begin(); it != m_SysFormatters.end(); ++it)
        delete *it;
    m_SysFormatters.clear();

}

lmBoxScore* lmFormatter5::LayoutScore(lmScore* pScore)
{
    //Build the graphical model for a score justifying measures, so that they fit exactly
    //in page width.
    //This method encapsulates the line breaking algorithm and the spacing algorithm.
    //Page filling is not yet implemented.
    //This method is only invoked from lmGraphicManager::Layout()

    wxASSERT(pScore);
	//pScore->Dump(_T("dump.txt"));

    //prepare things
    m_pScore = pScore;
    Initializations();

    do
    {
        //Each loop cycle computes and justifies one system

        //Form and add columns until no more space in current system
        bool fFirstSystemInPage = AddNewPageIfRequired();
        CreateSystemBox(fFirstSystemInPage);
        bool fThisIsLastSystem = FillCurrentSystemWithColumns();

        //Justify system (distribute space across all columns)
        ComputeMeasuresSizesToJustifyCurrentSystem(fThisIsLastSystem);
        RepositionStaffObjs();

        //Final details for this system
        SetCurrentSystemLenght(fThisIsLastSystem);
        AddInitialLineJoiningAllStavesInSystem();
        UpdateBoxSlicesSizes();
        GetSystemHeightAndAdvancePaperCursor();

    } while (m_pSysCursor->ThereAreObjects());


    if (RequestedToFillScoreWithEmptyStaves())
        FillPageWithEmptyStaves();

    m_pBoxScore->PopulateLayers();      //reorganize shapes in layers for renderization

    return m_pBoxScore;
}

void lmFormatter5::AddColumnToSystem()
{
    //A column has been processed, it has been checked that there is enough space to include it
    //in current system and, finally, decided to include it. This method does whatever
    //is necessary to include the column and consolidate the situation.

    //Add column to current system and discount the space that the measure will take
    m_uFreeSpace -= m_SysFormatters[m_nCurSystem-1]->GetMinimumSize(m_nRelColumn);
    m_nColumnsInSystem++;
    m_pSysCursor->CommitCursors();

    //mark all objects in column as 'non dirty'
    m_SysFormatters[m_nCurSystem-1]->ClearDirtyFlags(m_nRelColumn);

    //prepare to create a new column
    m_nRelColumn++;
    m_nAbsColumn++;
}

void lmFormatter5::SplitColumn(lmLUnits uAvailable)
{
    //We have measured a column and it doesn't fit in current system. Split it.
    //All information about column to split is stored in the SystemFormatter.
    //Parameter uAvailable is the available space in current system

    float rTime;
    lmLUnits uWidth;
    if (m_SysFormatters[m_nCurSystem-1]->GetOptimumBreakPoint(m_nRelColumn, uAvailable, &rTime, &uWidth))
    {
        wxString sMsg = _("Program failure: not enough space for drawing just one bar.");
        ::wxLogFatalError(sMsg);
    }

    m_pSysCursor->SetBreakTime(rTime);
}

bool lmFormatter5::SizeBarColumn(int nSystem, lmBoxSystem* pBoxSystem,
                                     lmLUnits nSystemIndent)
{
    // For each instrument and staff it is computed the size of this measure.
    // Also the hierarchy of BoxSlice objects is created for this measure.
    //
    // All measurements are stored in the global object SystemFormatter, so that other
    // procedures can take decisions about the final number of measures to include in a system
    // and for repositioning the StaffObjs.
    //
    // Input parameters:
    //   nSystem - System number
    //   nSystemIndent - indentation for first measure
    //
    // Results:
    //  - Returns true if newSystem tag found in this measure
    //  - SystemFormatter is updated:
    //      - The minimum size required by this column is computed.
    //      - Positioning information for this column is stored.
    //

    bool fNewSystemTagFound = false;

    //wxLogMessage(_T("[lmFormatter5::SizeBarColumn] m_nAbsColumn=%d, xPaper=%.2f "),
    //                m_nAbsColumn, m_pPaper->GetCursorX() );

    //determine xLeft position for this measure:
    //      if fisrt measure: xPaper + system indent
    //      for other measures it doesnt'n matter: use xPaper
    lmLUnits xStartPos = m_pPaper->GetCursorX() + (m_nRelColumn == 0 ? nSystemIndent : 0.0f);
    lmLUnits yPaperPos = m_pPaper->GetCursorY();

    // create an empty BoxSlice to contain this column
    lmBoxSlice* pBoxSlice = pBoxSystem->AddSlice(m_nAbsColumn);
	pBoxSlice->SetXLeft( xStartPos );

    // explore all instruments in the score
	lmLUnits yBottomLeft = 0.0f;
    lmLUnits uStaffMargin;
	int nInstr = 0;
    lmInstrument* pInstr;
    lmBoxSliceInstr* pPrevBSI;
    lmBoxSliceInstr* pBSI;
    for (pInstr = m_pScore->GetFirstInstrument(); pInstr; pInstr=m_pScore->GetNextInstrument(), nInstr++)
    {
        m_pPaper->SetCursorX( xStartPos );    //to align start of all staves in this system
        yPaperPos = m_pPaper->GetCursorY();

        lmVStaff* pVStaff = pInstr->GetVStaff();
        if (nInstr == 0)
        {
            //First instrument of this system. Set y position of BoxSlice
            uStaffMargin = pBoxSystem->GetTopSpace();
            pBoxSlice->SetYTop( yPaperPos + uStaffMargin );
            pBoxSlice->SetTopSpace(uStaffMargin);

        }
        else
        {
            //Not first instrument of system. Split distance: half for each instrument
            uStaffMargin = pVStaff->GetFirstStaff()->GetStaffDistance() / 2.0f;

            //set bottom limits of previous instrument
            pPrevBSI->SetBottomSpace( uStaffMargin );

            //advance paper to top limit of current instrument
            m_pPaper->IncrementCursorY(uStaffMargin);
            yPaperPos = m_pPaper->GetCursorY();
        }

        //Here yPaperPos is positioned at top limit of current instrument

        // create an empty BoxSliceInstr to contain this instrument slice
        pBSI = pBoxSlice->AddInstrument(pInstr);
        pBSI->SetTopSpace(uStaffMargin);
		pBSI->SetYTop( yPaperPos + uStaffMargin );
		pBSI->SetXLeft( pBoxSlice->GetXLeft() );

        //advance paper to top line of first staff (instrument top bounds)
        m_pPaper->SetCursorY( pBSI->GetYTop() );
        yPaperPos = m_pPaper->GetCursorY();

        //explore its VStaff to size the column m_nAbsColumn.
        //All collected information is stored in SystemFormatter

        // if first measure in system add the ShapeStaff
		if (m_nRelColumn == 0)
		{
			// Final xPos is yet unknown, so I use zero.
			// It will be updated when the system is completed
			yBottomLeft = pVStaff->LayoutStaffLines(pBoxSystem, pInstr, 
                                                    xStartPos, 0.0, yPaperPos);
		}
        else
			yBottomLeft = yPaperPos + pVStaff->GetVStaffHeight();

        fNewSystemTagFound |= SizeBar(pBSI, pVStaff, nInstr);

        //update bounds of boxes
		pBSI->SetYBottom(yBottomLeft);

        // if first measure in system add instrument name and bracket/brace.
        // Instrument is also responsible for managing group name and bracket/brace layout
		if (m_nRelColumn == 0)
            pInstr->AddNameAndBracket(pBoxSystem, pBSI, m_pPaper, nSystem);

        //advance paper in height of this lmVStaff
        m_pPaper->SetCursorY( yBottomLeft );

        pPrevBSI = pBSI;

    } // next lmInstrument

    //set slice bottom position and system bottom position
    pBoxSlice->SetYBottom(yBottomLeft);
    if (m_nRelColumn == 0)
        pBoxSystem->SetYBottom(yBottomLeft);

    //restore x cursor to paper left margin
    m_pPaper->SetCursorX( m_pScore->GetPageLeftMargin() );

    //all measures in column number m_nAbsColumn have been sized. The information is stored in
    //object SystemFormatter. Now proced to re-position the StaffObjs so that all StaffObjs
    //sounding at the same time will have the same x coordinate.
    bool fTrace =  m_fDebugMode && (m_nTraceMeasure == 0  || m_nTraceMeasure == m_nAbsColumn);

    m_SysFormatters[m_nCurSystem-1]->EndOfSystemMeasurements();
    m_SysFormatters[m_nCurSystem-1]->DoColumnSpacing(m_nRelColumn, fTrace);

    return fNewSystemTagFound;
}

lmLUnits lmFormatter5::AddEmptySystem(int nSystem, lmBoxSystem* pBoxSystem)
{
    //Add staff lines to received BoxSystem. Returns system height including bottom margin.
    //Paper is left positioned at next staff position

    lmLUnits uSystemHeight = - m_pPaper->GetCursorY();

    // explore all instruments in the score
    lmLUnits xStartPos = m_pPaper->GetCursorX();
    lmLUnits uyBottom;
    int nInstr = 0;
    lmInstrument* pInstr = m_pScore->GetFirstInstrument();
    while (pInstr)
    {
        m_pPaper->SetCursorX( xStartPos );    //align start of staves in this system

        lmLUnits yPaperPos = m_pPaper->GetCursorY();

        lmVStaff* pVStaff = pInstr->GetVStaff();

        //Top space
        lmLUnits uTopMargin;
        if (nInstr == 0)
        {
            //First instrument of this system
            uTopMargin = pBoxSystem->GetTopSpace();

        }
        else
        {
            //Not first instrument of system. Split distance: half for each instrument
            uTopMargin = pVStaff->GetFirstStaff()->GetStaffDistance() / 2.0f;

            //advance paper to top limit of current instrument
            m_pPaper->IncrementCursorY(uTopMargin);
            yPaperPos = m_pPaper->GetCursorY();
        }

        //Here paper is positioned at top limit of current instrument

        //advance paper to top line of first staff (instrument top bounds)
        m_pPaper->IncrementCursorY(uTopMargin);
        yPaperPos = m_pPaper->GetCursorY();

        //Add staff lines for current instrument. As final xPos is yet unknown, so I use zero.
		//It will be updated when the system is completed
		uyBottom = pVStaff->LayoutStaffLines(pBoxSystem, pInstr, xStartPos,
                                             0.0f, m_pPaper->GetCursorY());

        //advance paper in height of this lmVStaff
        m_pPaper->SetCursorY( uyBottom );

        //proceed with next instrument
        pInstr = m_pScore->GetNextInstrument();
        ++nInstr;
    }

    //restore x cursor to paper left margin
    m_pPaper->SetCursorX( m_pScore->GetPageLeftMargin() );

    //set system bottom position
    pBoxSystem->SetYBottom(uyBottom);


    uSystemHeight += m_pPaper->GetCursorY();
    return uSystemHeight;
}

void lmFormatter5::RedistributeFreeSpace(lmLUnits nAvailable, bool fLastSystem)
{
    //Step 3: Justify measures (distribute remainnig space across all measures)
    //-------------------------------------------------------------------------------
    //In summary, the algoritm computes the average size of all columns and
    //increases the size of all columns that are narrower than the average size,
    //so that their size is grown to the average size. The process of computing the new
    //average and enlarging the narrower than average measures is repeated until all
    //space is distributed.
    //The system is not justified if this is the last system and there is no barline
    //in the last measure.
    //
    //on entering in this function:
    // - object SystemFormatter stores the minimum size for each column for
    //   the current system.
    // - nAvailable stores the free space remaining at the end of this system
    //
    //on exit:
    // - the values stored in SystemFormatter are modified to reflect the new size
    //   for the bar columns, so that the line get justified.
    //
    //-------------------------------------------------------------------------------------

    if (nAvailable <= 0) return;       //no space to distribute

    //The system must not be justified if this is the last system and there is no barline
    //in the last bar. Check this.
    lmSystemFormatter* pSysFmt = m_SysFormatters[m_nCurSystem-1];
    if (fLastSystem && !pSysFmt->ColumnHasBarline(m_nColumnsInSystem-1))
            return;     //no need to justify

    //compute average column size
    std::vector<lmLUnits> nDif(m_nColumnsInSystem, 0.0f);
    lmLUnits nAverage = 0;
    for (int i = 0; i < m_nColumnsInSystem; i++)
    {
        nAverage += pSysFmt->GetMinimumSize(i);
    }
    nAverage /= m_nColumnsInSystem;

    lmLUnits nMeanPrev = 0;
    lmLUnits nDifTotal = 0;
    while (nAvailable > 0 && nAverage != nMeanPrev) {
        //for each column, compute the diference between its size and the average size
        //sum up all the diferences in nDifTotal
        nDifTotal = 0;
        for (int i = 0; i < m_nColumnsInSystem; i++)
        {
            nDif[i] = nAverage - pSysFmt->GetMinimumSize(i);
            if (nDif[i] > 0)
                nDifTotal += nDif[i];
        }

        //if the sum of all diferences is greater than the space to distribute
        //reduce the differences
        while (nDifTotal > nAvailable) {
            nDifTotal = 0;
            for (int i = 0; i < m_nColumnsInSystem; i++)
            {
                if (nDif[i] > 0)
                {
                    nDif[i]--;
                    nDifTotal += nDif[i];
                }
            }
        }

        //The size of all columns whose size is lower than the average
        //is going to be increased by the amount stated in the differences table
        for (int i = 0; i < m_nColumnsInSystem; i++)
        {
            if (nDif[i] > 0)
                pSysFmt->IncrementColumnSize(i, nDif[i]);
        }
        nAvailable -= nDifTotal;

        //compute the new column size average
        nMeanPrev = nAverage;
        nAverage = 0;
        for (int i = 0; i < m_nColumnsInSystem; i++)
        {
            nAverage += pSysFmt->GetMinimumSize(i);
        }
        nAverage /= m_nColumnsInSystem;
    }

    //divide up the remaining space between all bars
    if (nAvailable > 0)
    {
        nDifTotal = nAvailable / m_nColumnsInSystem;
        for (int i = 0; i < m_nColumnsInSystem; i++)
        {
            pSysFmt->IncrementColumnSize(i, nDifTotal);
            nAvailable -= nDifTotal;
        }
        pSysFmt->IncrementColumnSize(m_nColumnsInSystem-1, nAvailable);
    }

}


//=========================================================================================
// Methods to deal with measures
//=========================================================================================

bool lmFormatter5::SizeBar(lmBoxSliceInstr* pBSI, lmVStaff* pVStaff,
							   int nInstr)
{
    // Compute the width of the current measure (or remaining part of it) of the lmVStaff
    // Input variables:
    //   pVStaff - lmVStaff to process
	//   nInstr - instrument number 0..n
    //   m_pSysCursor is pointing to current position
    //   m_nRelColumn and m_nAbsColumn have the relative and absolute numbers for current
    //      column beign measured
    //   flag m_nRelColumn == 0 signals if this is the first column of current system
    //
    // Results:
    //   all measurements are stored in SystemFormatter
    //   Return bool: true if newSystem tag found in this measure

    //wxLogMessage(_T("[lmFormatter5::SizeBar] m_nAbsColumn=%d, First column in system = %s"),
    //             m_nAbsColumn, (m_nRelColumn == 0 ? _T("yes") : _T("no")));

    //add some space at start of measure, if necessary
    lmLUnits uSpaceAfterStart = 0.0f;
    if (m_nRelColumn == 0)
    {
        //if first measure of system, add some space before prolog
        uSpaceAfterStart = m_uSpaceBeforeProlog;
    }
    else
    {
        //Not first measure of system. Get the previous barline and add some space if
        //the previous barline is visible.
        lmBarline* pBar = m_pSysCursor->GetPreviousBarline(nInstr);
        if (pBar)
        {
            if (pBar->IsVisible())
                uSpaceAfterStart = pVStaff->TenthsToLogical(20.0f);    // TODO: user options
        }
    }

    //ask system formatter to prepare to receive data for this instrument objects in this column
    lmLUnits uxStart = m_pPaper->GetCursorX();
    lmSystemFormatter* pSysFormatter = m_SysFormatters[m_nCurSystem-1];
    pSysFormatter->StarBarMeasurements(m_nRelColumn, nInstr, uxStart, pVStaff, uSpaceAfterStart);

    //The prolog (clef and key signature) must be rendered on each system, but the
    //matching StaffObjs only exist in the first system. In the first system the prolog
	//is rendered as part as the normal lmStaffObj rendering process, so there is nothig
	//special to do to render the prolog But for the other systems we must force the
	//rendering of the prolog because there are no StaffObjs representing the prolog.
    bool fProlog = (m_nRelColumn == 0);        //fProlog -> we are adding prolog objects
    if (m_nAbsColumn != 1 && m_nRelColumn == 0)
	{
		AddProlog(pBSI, false, pVStaff, nInstr);
        fProlog = false;                    //prolog added
	}

    //loop to process all StaffObjs in this measure
    bool fNewSystemTagFound = false;                //newSystem tag found
    lmStaffObj* pSO = (lmStaffObj*)NULL;
    lmSOIterator* pIT = m_pSysCursor->GetIterator(nInstr);
    pIT->ResetFlags();
    while(!pIT->EndOfCollection() && !pIT->ChangeOfMeasure())
    {
        pSO = pIT->GetCurrent();

        if (pSO->IsBarline() || IsHigherTime(pSO->GetTimePos(), m_pSysCursor->GetBreakTime()) )
        {
             break;         //End of measure: exit loop.
        }

        else if (pSO->IsControl())
        {
            ESOCtrolType nCtrolType = ((lmSOControl*)pSO)->GetCtrolType();
            if(lmNEW_SYSTEM == nCtrolType)
            {
                //new system tag found in this measure
                fNewSystemTagFound = true;
            }
			else {
				wxLogMessage(_T("lmFormatter5::SizeBar] Bad SOControl type"));
				wxASSERT(false);
			}
        }

        else if (pSO->IsClef())
		{
			m_pPaper->SetCursorX(uxStart);
			pSO->Layout(pBSI, m_pPaper);
			lmShape* pShape = pSO->GetShape();
            pSysFormatter->IncludeObject(m_nRelColumn, nInstr, pSO, pShape, fProlog);
		}
        else if (pSO->IsKeySignature())
		{
			m_pPaper->SetCursorX(uxStart);
			AddKey((lmKeySignature*)pSO, pBSI, pVStaff, nInstr, fProlog);
        }

        else if (pSO->IsTimeSignature())
		{
			m_pPaper->SetCursorX(uxStart);
			AddTime((lmTimeSignature*)pSO, pBSI, pVStaff, nInstr, fProlog);
		}

		else
		{
            //it is neither clef, key signature nor time signature. Finish prologue
            fProlog = false;

			//create this lmStaffObj shape and add to table
			m_pPaper->SetCursorX(uxStart);
			pSO->Layout(pBSI, m_pPaper);
			lmShape* pShape = pSO->GetShape();
            pSysFormatter->IncludeObject(m_nRelColumn, nInstr, pSO, pShape, fProlog);
        }

        pIT->MoveNext();
    }

    //The barline lmStaffObj is not included in the loop as it might not exist in the last
    //bar of a score. In theses cases, the loop is exited because the end of the score is
    //reached. In any case we have to close the line
    if (pSO && pSO->IsBarline())
    {
        pIT->MoveNext();    //leave cursor pointing to next measure

        m_pPaper->SetCursorX(uxStart);
        pSO->Layout(pBSI, m_pPaper);
        lmShape* pShape = pSO->GetShape();
        pSysFormatter->IncludeBarlineAndTerminateBarMeasurements(m_nRelColumn, pSO, pShape, uxStart);
    }
    else
	{
        // no barline at the end of the measure.
        pSysFormatter->TerminateBarMeasurementsWithoutBarline(m_nRelColumn, uxStart);

        //force new system if a break point reached
        if (pSO && IsHigherTime(pSO->GetTimePos(), m_pSysCursor->GetBreakTime()))
            fNewSystemTagFound = true;
    }

    return fNewSystemTagFound;
}

void lmFormatter5::AddProlog(lmBoxSliceInstr* pBSI, bool fDrawTimekey, lmVStaff* pVStaff,
                             int nInstr)
{
    // The prolog (clef and key signature) must be rendered on each system,
    // but the matching StaffObjs only exist in the first system. Therefore, in the
    // normal staffobj rendering process, the prolog would be rendered only in
    // the first system.
    // So, for the other systems it is necessary to force the rendering
    // of the prolog because there are no StaffObjs representing it.
    // This method does it.
    //
    // To know what clef, key and time signature to draw we take this information from the
    // context associated to first note of the measure on each staff. If there are no notes,
    // the context is taken from the barline. If, finally, no context is found, no prolog
    // is drawn.

    lmLUnits uPrologWidth = 0.0f;
    lmClef* pClef = (lmClef*)NULL;
    lmEClefType nClef = lmE_Undefined;
    lmKeySignature* pKey = (lmKeySignature*)NULL;
    lmTimeSignature* pTime = (lmTimeSignature*)NULL;

    //AWARE when this method is invoked the paper position must be at the left marging,
    //at the start of a new system.
    lmLUnits xStartPos = m_pPaper->GetCursorX();      //Save x to align all clefs
    lmLUnits yStartPos = m_pPaper->GetCursorY();

    //iterate over the collection of lmStaff objects to draw current clef and key signature

    lmStaff* pStaff = pVStaff->GetFirstStaff();
    lmLUnits uyOffset = 0.0f;
    lmLUnits xPos = 0.0f;

    lmSystemFormatter* pSysFormatter = m_SysFormatters[m_nCurSystem-1];
    lmContext* pContext = (lmContext*)NULL;
    for (int nStaff=1; nStaff <= pVStaff->GetNumStaves(); pStaff = pVStaff->GetNextStaff(), nStaff++)
    {
        xPos = xStartPos;
        if (nStaff > 1)
            uyOffset += pStaff->GetStaffDistance();

            //locate context for first note in this staff
        pContext = m_pSysCursor->GetStartOfColumnContext(nInstr, nStaff);

        if (pContext)
        {
            pClef = pContext->GetClef();
            pKey = pContext->GetKey();
            pTime = pContext->GetTime();

            //render clef
            if (pClef)
            {
                nClef = pClef->GetClefType();
				if (pClef->IsVisible())
                {
					lmUPoint uPos = lmUPoint(xPos, yStartPos+uyOffset);        //absolute position
					lmShape* pShape = pClef->CreateShape(pBSI, m_pPaper, uPos);
                    pShape->SetShapeLevel(lm_ePrologShape);
					xPos += pShape->GetWidth();
                    pSysFormatter->IncludeObject(m_nRelColumn, nInstr, pClef, pShape, true);
				}
            }

            //render key signature
            if (pKey && pKey->IsVisible())
            {
                lmUPoint uPos = lmUPoint(xPos, yStartPos+uyOffset);        //absolute position
                lmShape* pShape = pKey->CreateShape(pBSI, m_pPaper, uPos, nClef, pStaff);
                pShape->SetShapeLevel(lm_ePrologShape);
				xPos += pShape->GetWidth();
                pSysFormatter->IncludeObject(m_nRelColumn, nInstr, pKey, pShape, true, nStaff);
            }

        }

        //compute prolog width
        uPrologWidth = wxMax(uPrologWidth, xPos - xStartPos);

        //compute vertical displacement for next staff
        uyOffset += pStaff->GetHeight();

    }

    // update paper cursor position
    m_pPaper->SetCursorX(xStartPos + uPrologWidth);

}

void lmFormatter5::AddKey(lmKeySignature* pKey, lmBox* pBox,
						  lmVStaff* pVStaff, int nInstr, bool fProlog)
{
    // This method is responsible for creating the key signature shapes for
    // all staves of this instrument. And also, of adding them to the graphical
    // model and to the Timepos table

    //create the shapes
    pKey->Layout(pBox, m_pPaper);

	//add the shapes to the timepos table
    lmSystemFormatter* pSysFormatter = m_SysFormatters[m_nCurSystem-1];
	lmShape* pMainShape = ((lmStaffObj*)pKey)->GetShape();          //cast forced because otherwise the compiler complains
    for (int nStaff=1; nStaff <= pVStaff->GetNumStaves(); nStaff++)
    {
        lmShape* pShape = pKey->GetShape(nStaff);
        pSysFormatter->IncludeObject(m_nRelColumn, nInstr, pKey, pShape, fProlog, nStaff);
    }

}


void lmFormatter5::AddTime(lmTimeSignature* pTime, lmBox* pBox,
						   lmVStaff* pVStaff, int nInstr, bool fProlog)
{
    // This method is responsible for creating the time signature shapes for
    // all staves of this instrument. And also, of adding them to the graphical
    // model and to the Timepos table

    //create the shapes
    pTime->Layout(pBox, m_pPaper);

	//add the shapes to the timepos table
    lmSystemFormatter* pSysFormatter = m_SysFormatters[m_nCurSystem-1];
	lmShape* pMainShape = ((lmStaffObj*)pTime)->GetShape();          //cast forced because otherwise the compiler complains
    for (int nStaff=1; nStaff <= pVStaff->GetNumStaves(); nStaff++)
    {
        lmShape* pShape = pTime->GetShape(nStaff);
        pSysFormatter->IncludeObject(m_nRelColumn, nInstr, pTime, pShape, fProlog, nStaff);
    }

}

void lmFormatter5::AddScoreTitlesToCurrentPage()
{
    //TO AddScoreTitlesToCurrentPage move cursor to top left corner and ask the
    //score to layout its attached objects.

    m_pPaper->SetCursor(m_pScore->GetPageLeftMargin(), m_pScore->GetPageTopMargin());
    m_pScore->LayoutAttachedObjects(m_pBoxScore->GetCurrentPage(), m_pPaper);
}

void lmFormatter5::PositionCursorsAfterHeaders()
{
    //TO PositionCursorsAfterHeaders set cursor at top-left corner and add
    //headers height.

    MoveCursorToTopLeftCorner();
    m_pPaper->IncrementCursorY(m_pScore->GetHeadersHeight());
}

void lmFormatter5::RepositionStaffObjs()
{
    //SystemFormatter stores the final size that must have each column
    //of this system. This method changes StaffObjs locations so that they are evenly
    //distributed across the the bar.

    //dbg ------------------------------------------------------------------------------
    if (m_fDebugMode) {
        wxLogMessage(_T("Before repositioning objects"));
        wxLogMessage(_T("***************************************\n"));
        wxLogMessage( m_pScore->Dump() );
    }
    //dbg ------------------------------------------------------------------------------

    lmLUnits uxStartOfMeasure = m_SysFormatters[m_nCurSystem-1]->GetStartPositionForColumn(0);
    for (int i=0; i < m_nColumnsInSystem; i++)
    {
        lmBoxSlice* pBSlice = (lmBoxSlice*)m_pCurrentBoxSystem->GetChildBox(i);
        uxStartOfMeasure = m_SysFormatters[m_nCurSystem-1]->RedistributeSpace(i, uxStartOfMeasure);
        m_SysFormatters[m_nCurSystem-1]->AddTimeGridToBoxSlice(i, pBSlice);
    }

    //dbg ------------------------------------------------------------------------------
    if (m_fDebugMode) {
        wxLogMessage(_T("After repositioning objects"));
        wxLogMessage(_T("***************************************\n"));
        wxLogMessage( m_pScore->Dump() );
    }
    //dbg ------------------------------------------------------------------------------

}

bool lmFormatter5::AddNewPageIfRequired()
{
    //A page is started if no page yet created or not enough space in current page.
    //returns true is new page created


    //TODO ************
    //  If paper height is smaller than system height it is impossible to fit
    //  one system in a page. We have to split system horizontally (some staves in
    //  one page and the others in next page).

    //TODO By using m_uLastSystemHeight in following call we are assuming that next
    //system height is equal to last finished system.

    //Here Paper is positioned at the start of the new current system.
    lmLUnits yNew = m_pPaper->GetCursorY() + m_uLastSystemHeight;
    if (m_nCurrentPageNumber == 0 || yNew > m_pScore->GetMaximumY() )
    {
        //wxLogMessage(_T("Page break needed. yCur=%.2f, yNew=%.2f, MaximumY=%.2f"), m_pPaper->GetCursorY(), yNew, m_pScore->GetMaximumY());
        //start a new page
		m_pScore->SetPageInfo(++m_nCurrentPageNumber);
        m_pCurrentBoxPage = m_pBoxScore->AddPage();

        if (m_nCurrentPageNumber == 1)
        {
            AddScoreTitlesToCurrentPage();
            PositionCursorsAfterHeaders();
        }
        else
            MoveCursorToTopLeftCorner();

        return true;
    }
    return false;
}

void lmFormatter5::CreateSystemBox(bool fFirstSystemInPage)
{
    //create the system container at current x,y position and move x cursor to
    //system left marging

    //create the formatter object for the system
    lmSystemFormatter* pSF = new lmSystemFormatter(m_rSpacingFactor, m_nSpacingMethod, m_nSpacingValue);
    m_SysFormatters.push_back(pSF);

    //reset columns counter
    m_nColumnsInSystem = 0;

    //create system box
    m_uStartOfCurrentSystem = m_pPaper->GetCursorY();
    m_pCurrentBoxSystem = 
        m_pCurrentBoxPage->AddSystem(++m_nCurSystem, m_pPaper->GetCursorX(), m_uStartOfCurrentSystem,
                                     fFirstSystemInPage);
    m_pCurrentBoxSystem->SetFirstMeasure(m_nAbsColumn);

    //move x cursor to system left marging
    m_pPaper->IncrementCursorX( m_pCurrentBoxSystem->GetLeftSpace() );
}

void lmFormatter5::MoveCursorToTopLeftCorner()
{
    m_pPaper->SetCursor(m_pScore->GetPageLeftMargin(), m_pScore->GetPageTopMargin());
}

void lmFormatter5::GetScoreRenderizationOptions()
{
    // get options for renderization

    m_fStopStaffLinesAtFinalBarline = m_pScore->GetOptionBool(_T("StaffLines.StopAtFinalBarline"));
    m_fJustifyFinalBarline = m_pScore->GetOptionBool(_T("Score.JustifyFinalBarline"));
    m_rSpacingFactor = (float) m_pScore->GetOptionDouble(_T("Render.SpacingFactor"));
    m_nSpacingMethod = (lmESpacingMethod) m_pScore->GetOptionLong(_T("Render.SpacingMethod"));
    m_nSpacingValue = (lmTenths) m_pScore->GetOptionLong(_T("Render.SpacingValue"));
}

void lmFormatter5::PrepareFontsThatMatchesStavesSizes()
{
    //for each staff size, setup fonts of right point size for that staff size
    lmInstrument *pInstr;
    for (pInstr = m_pScore->GetFirstInstrument(); pInstr; pInstr=m_pScore->GetNextInstrument())
    {
        pInstr->GetVStaff()->SetUpFonts(m_pPaper);
    }
}

void lmFormatter5::DecideSystemsIndentation()
{
    //get the maximum indentation required by any instrument

    m_uFirstSystemIndent = 0.0f;
    m_uOtherSystemIndent = 0.0f;
    lmInstrument *pI;
    for (pI = m_pScore->GetFirstInstrument(); pI; pI=m_pScore->GetNextInstrument())
    {
        pI->MeasureNames(m_pPaper);
        m_uFirstSystemIndent = wxMax(m_uFirstSystemIndent, pI->GetIndentFirst());
        m_uOtherSystemIndent = wxMax(m_uOtherSystemIndent, pI->GetIndentOther());
    }
}

void lmFormatter5::DecideSpaceBeforeProlog()
{
    //TODO. Now a fixed value of 7.5 tenths is used. User options ?

    lmTenths rSpaceBeforeProlog = 7.5f;			
	lmInstrument* pInstr = m_pScore->GetFirstInstrument();
	lmVStaff* pVStaff = pInstr->GetVStaff();
	m_uSpaceBeforeProlog = pVStaff->TenthsToLogical(rSpaceBeforeProlog, 1);
}

void lmFormatter5::CreateSystemCursor()
{
    //create new cursor and initialize it
    if (m_pSysCursor)
        delete m_pSysCursor;
    m_pSysCursor = new lmSystemCursor(m_pScore);
}

void lmFormatter5::ComputeMeasuresSizesToJustifyCurrentSystem(bool fThisIsLastSystem)
{
    //TO ComputeMeasuresSizesToJustifyCurrentSystem divide up the remaining space 
    //between all bars, except if current system is the last one and flag
    //"JustifyFinalBarline" is not set or there is no final barline.

    //At this point the number of measures to include in current system has been computed
    //and some data is stored in the following global variables:
    //
    //   SystemFormatter - positioning information for columns and 
    //          minimum size for each column for current system.
    //   m_uFreeSpace - free space available on this system
    //   m_nColumnsInSystem  - the number of measures that fit in this system
    //
    //Now we are going to compute new column sizes and store results
    //in SystemFormatter but changes nothing in the StaffObjs

    if (!fThisIsLastSystem || (fThisIsLastSystem && m_fJustifyFinalBarline))
        RedistributeFreeSpace(m_uFreeSpace, fThisIsLastSystem);

}

void lmFormatter5::AddInitialLineJoiningAllStavesInSystem()
{
    //TODO: In current code, the decision about joining staves depends only on first
    //instrument. This should be changed and the line should go from first visible
    //staff to last visible one.

    //TODO: For empty scores (no staffobj on any instrument) this initial line should
    //not be drawn

	//Add the shape for the initial barline that joins all staves in a system
    lmVStaff* pVStaff = m_pScore->GetFirstInstrument()->GetVStaff();
	if (m_pScore->GetOptionBool(_T("Staff.DrawLeftBarline")) && !pVStaff->HideStaffLines() )
	{
		lmLUnits uxPos = m_pCurrentBoxSystem->GetXLeft() +
						    (m_nCurSystem == 1 ? m_uFirstSystemIndent : m_uOtherSystemIndent);
		lmLUnits uLineThickness = lmToLogicalUnits(0.2, lmMILLIMETERS);        // thin line width will be 0.2 mm TODO user options
        lmShapeSimpleLine* pLine =
            new lmShapeSimpleLine(pVStaff, uxPos, m_pCurrentBoxSystem->GetYTopFirstStaff(),
						uxPos, m_pCurrentBoxSystem->GetYBottom(),
						uLineThickness, 0.0, *wxBLACK, _T("System joining line"),
						lm_eEdgeHorizontal);
	    m_pCurrentBoxSystem->AddShape(pLine, lm_eLayerBarlines);
	}
}

bool lmFormatter5::CreateColumnAndAddItToCurrentSystem()
{
    //Creates a column and adds it to current system, if enough space.
    //The column is sized and this space discunted from available line
    //space. Returns true if current system is completed,there is not enough space for
    //including this column in current system or because a newSystem tag is found.
    //If not enough space for adding the column, SystemCursor is repositined againg at 
    //start of this column and nothing is added to current system.

    //reposition paper vertically at the start of the system. It has been advanced
    //when sizing the previous column
    m_pPaper->SetCursorY( m_uStartOfCurrentSystem );

    #if defined(__WXDEBUG__)
    g_pLogger->LogTrace(_T("Formatter4.Step1"),
        _T("m_uMeasure=%d, Paper X = %.2f"), m_nRelColumn, m_pPaper->GetCursorX() );
    #endif

    //if start of system, set system length. This is needed because for
    //placing ties (and probably other objects) it is necessary to know
    //system box position/size.
    if (m_nRelColumn == 0)
        m_pCurrentBoxSystem->UpdateXRight( m_pScore->GetRightMarginXPos() );

    //size this column and create BoxSlice (and BoxSlice hierarchy) for
    //the measure being processed
    bool fNewSystemTagFound = false;
    fNewSystemTagFound = SizeBarColumn(m_nCurSystem, m_pCurrentBoxSystem, 
                                            (m_nCurSystem == 1 ? 
                                                        m_uFirstSystemIndent 
                                                        : m_uOtherSystemIndent) );

    #if defined(__WXDEBUG__)
    g_pLogger->LogTrace(_T("Formatter4.Step1"),
        _T("Col[%d].MinimumSize = %.2f"),
        m_nRelColumn, m_SysFormatters[m_nCurSystem-1]->GetMinimumSize(m_nRelColumn) );
    #endif

    //if this is the first column compute the space available in
    //this system. The method is a little tricky. The total space available
    //is (pScore->GetPageRightMargin() - pScore->GetCursorX()). But we have
    //to take into account the space that will be used by the prolog. As the
    //left position of the first column has taken all this into account,
    //it is posible to use that value by just doing:
    if (m_nRelColumn == 0)
    {
        m_uFreeSpace = m_pScore->GetRightMarginXPos() 
                       - m_pScore->GetSystemLeftSpace(m_nCurSystem - 1) 
                       - m_SysFormatters[m_nCurSystem-1]->GetStartPositionForColumn(m_nRelColumn);
    }

    #if defined(__WXDEBUG__)
    g_pLogger->LogTrace(_T("Formatter4.Step1"),
        _T("RelMeasure=%d, m_uFreeSpace = %.2f, PaperRightMarginXPos=%.2f, StartOfBar=%.2f"),
        m_nRelColumn, m_uFreeSpace, m_pScore->GetRightMarginXPos(),
        m_SysFormatters[m_nCurSystem-1]->GetStartPositionForColumn(m_nRelColumn) );
    g_pLogger->LogTrace(_T("Formatter4.Step1"),
        m_SysFormatters[m_nCurSystem-1]->DumpColumnData(m_nRelColumn) );
    #endif

    //check if there is enough space to add this column to current system
    if(m_uFreeSpace < m_SysFormatters[m_nCurSystem-1]->GetMinimumSize(m_nRelColumn))
	{
        //there is no enough space for this column.

        //restore cursors to re-process this column
        m_pSysCursor->GoBackPrevPosition();

        //if no column added to system, the line width is not enough for drawing
        //just one measure or no measures in score (i.e. no time signature).
        //We have to split the current column and reprocess it
        if (m_nColumnsInSystem == 0)
        {
            //determine break time to split this column
            SplitColumn(m_uFreeSpace);
        }

        //discard measurements for current column
        m_SysFormatters[m_nCurSystem-1]->DiscardMeasurementsForColumn(m_nRelColumn);
        m_pCurrentBoxSystem->DeleteLastSlice();

        //if at least one column in current system, the system is finished
        if (m_nColumnsInSystem > 0)
            return true;    //terminate system
    }
    else
    {
        //there is enough space for this column. Add it to system
        AddColumnToSystem();
    }

    return fNewSystemTagFound;
}

bool lmFormatter5::FillCurrentSystemWithColumns()
{
    //form and add columns until no more space in current system.
    //Returns true if this is the last system (there are no more objects to process)

    m_nRelColumn = 0;      //first column of this system
    bool fContinue = true;
    while (fContinue)
    {
        //each loop cycle is to process a column. The column is built and sized, and
        //its space discounted from available system space.

        bool fTerminateSystem = CreateColumnAndAddItToCurrentSystem();
        fContinue = m_pSysCursor->ThereAreObjects() && !fTerminateSystem;
    }
    return !m_pSysCursor->ThereAreObjects();
}

void lmFormatter5::SetCurrentSystemLenght(bool fThisIsLastSystem)
{
    //system must be truncated at final barline if requested. Otherwise it must go
    //to right margin. Set here the applicable lenght.

    if (fThisIsLastSystem && m_fStopStaffLinesAtFinalBarline)
    {
        //this is the last system and it has been requested to stop staff lines
        //in last measure. So, set final x so staff lines go to final bar line
        lmLUnits xFinalPos = 0.0f;
        lmLUnits yFinalPos = 0.0f;
        lmInstrument *pI;
        for (pI = m_pScore->GetFirstInstrument(); pI; pI=m_pScore->GetNextInstrument())
        {
            lmLUnits xPos, yPos;
            pI->GetVStaff()->GetBarlineOfLastNonEmptyMeasure(&xPos, &yPos);
            if (yPos > yFinalPos)
            {
                yFinalPos = yPos;
                xFinalPos = xPos;
            }
        }
        if (xFinalPos > 0.0f)
            m_pCurrentBoxSystem->UpdateXRight( xFinalPos - 1 );
        else
            m_pCurrentBoxSystem->UpdateXRight( m_pScore->GetRightMarginXPos() );
    }
    else
    {
        //staff lines go to the rigth margin
        m_pCurrentBoxSystem->UpdateXRight( m_pScore->GetRightMarginXPos() );
    }
}

void lmFormatter5::GetSystemHeightAndAdvancePaperCursor()
{
    //A system has been layouted. Update last system heigh record (without bottom space)
    //and advance paper cursor to next syystem position

    //AWARE: In GetSystemDistance() we are using m_nCurSystem instead of
    //m_nCurSystem-1. This is to get the system distance between this system
    //and next one.
    lmLUnits uSystemBottomSpace = m_pScore->GetSystemDistance(m_nCurSystem, false) / 2.0;
    m_pCurrentBoxSystem->SetBottomSpace(uSystemBottomSpace);
    m_uLastSystemHeight = m_pCurrentBoxSystem->GetBounds().GetHeight() 
                          + m_pCurrentBoxSystem->GetTopSpace();

    //advance paper in system bottom space
    m_pPaper->IncrementCursorY(uSystemBottomSpace);
}

void lmFormatter5::UpdateBoxSlicesSizes()
{
    //update lmBoxSlices with the final measures sizes, except for last
    //measure, as its length has been already set up

    lmLUnits xEnd = m_SysFormatters[m_nCurSystem-1]->GetStartPositionForColumn(0);
    for (int iRel=0; iRel < m_nColumnsInSystem; iRel++)
    {
        lmLUnits xStart = xEnd;
        xEnd = xStart + m_SysFormatters[m_nCurSystem-1]->GetMinimumSize(iRel);
        lmBoxSlice* pBoxSlice = m_pCurrentBoxSystem->GetSlice(iRel);
		pBoxSlice->UpdateXLeft(xStart);
        if (iRel < m_nColumnsInSystem)
			pBoxSlice->UpdateXRight(xEnd);
    }
}

bool lmFormatter5::RequestedToFillScoreWithEmptyStaves()
{
    return (!m_fStopStaffLinesAtFinalBarline
            && m_pScore->GetOptionBool(_T("Score.FillPageWithEmptyStaves")) );
}

void lmFormatter5::FillPageWithEmptyStaves()
{
    //First system has been always added before arriving here. Fill the remaining
    //page space with empty staves

    //advance vertically the previous system bottom space
    m_pPaper->IncrementCursorY( m_pScore->GetSystemDistance(m_nCurSystem, false) / 2.0 );

    while (true)      //loop is exited when reaching end of page
    {
        //Here Paper is positioned at the start of the new current system.

        bool fFirstSystemInPage = false;

        //TODO ************
        //  By using nSystemHeight we are assuming that next system height is going
        //  to be equal to last finished system. In this test it is necessary 
        //  to compute and use next system height
        lmLUnits nNextSystemHeight = m_uLastSystemHeight;
        lmLUnits yNew = m_pPaper->GetCursorY() + nNextSystemHeight;
        if ( yNew > m_pScore->GetMaximumY() )
            break;        //exit loop

        //create the system container
        m_uStartOfCurrentSystem = m_pPaper->GetCursorY();      //save start of system position
        m_pCurrentBoxSystem = 
            m_pCurrentBoxPage->AddSystem(m_nCurSystem, m_pPaper->GetCursorX(),
                                         m_uStartOfCurrentSystem, fFirstSystemInPage);
        m_pCurrentBoxSystem->SetFirstMeasure(m_nAbsColumn);
        m_pPaper->IncrementCursorX( m_pCurrentBoxSystem->GetLeftSpace() );
        m_pCurrentBoxSystem->SetIndent(((m_nCurSystem == 1) ? m_uFirstSystemIndent : m_uOtherSystemIndent ));


        m_nRelColumn = 0;          //first column of this system
        m_nAbsColumn++;
        m_uLastSystemHeight = AddEmptySystem(m_nCurSystem, m_pCurrentBoxSystem);     //Add the staff lines

        //staff lines go to the rigth margin
        m_pCurrentBoxSystem->UpdateXRight( m_pScore->GetRightMarginXPos() );

        // compute system bottom space
        //AWARE:
        //  In GetSystemDistance() we are using m_nCurSystem instead of m_nCurSystem-1.
        //  This is to get the system distance between this system and next one.
        lmLUnits uSystemBottomSpace = m_pScore->GetSystemDistance(m_nCurSystem, false) / 2.0;
        m_pCurrentBoxSystem->SetBottomSpace(uSystemBottomSpace);

        //advance paper in system bottom space
        m_pPaper->IncrementCursorY(uSystemBottomSpace);

        //increment loop information
        m_nCurSystem++;
    }
}

void lmFormatter5::Initializations()
{
    GetScoreRenderizationOptions();
    PrepareFontsThatMatchesStavesSizes();
    DecideSystemsIndentation();
    DecideSpaceBeforeProlog();
    CreateSystemCursor();

	m_nCurrentPageNumber = 0;
    m_nCurSystem = 0;
    m_uLastSystemHeight = 0.0f;
    m_nAbsColumn = 1;

    m_pBoxScore = new lmBoxScore(m_pScore);
}

//=========================================================================================
// methods coded only for Unit Tests
//=========================================================================================
#if defined(__WXDEBUG__)

int lmFormatter5::GetNumSystemFormatters() { return (int)m_SysFormatters.size(); }
int lmFormatter5::GetNumColumns(int iSys) { return m_SysFormatters[iSys]->GetNumColumns(); }
int lmFormatter5::GetNumLines(int iSys, int iCol) 
        { return m_SysFormatters[iSys]->GetNumLinesInColumn(iCol); }

lmSystemFormatter* lmFormatter5::GetSystemFormatter(int iSys)
{
    //iSys=[0..n-1]
    return m_SysFormatters[iSys];
}

#endif
