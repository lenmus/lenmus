//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

//-------------------------------------------------------------------------------------
// This class encapsulates the algorithms for layouting a score:
//  - The line breaking algorithm.
//  - The spacing algorithm.
//  - The page filling algorithm.
//
// This class is only used in lmScore, and the only method used is LayoutScore() that
// is invoked from lmScore::Layout().
// lmFormatter could be part of lmScore but I prefer to keep this isolated, to have
// the main layouting algorithm isolated.
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
#include "TimeposTable.h"
#include "Formatter4.h"
#include "BoxScore.h"
#include "BoxPage.h"
#include "BoxSystem.h"
#include "BoxSlice.h"
#include "BoxSliceInstr.h"
#include "BoxSliceVStaff.h"

//access to logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;

//-----------------------------------------------------------------------------------------
// Helper class: To determine "possible break locations" we have to traverse all staves in
// in parallel. This class keeps information about the current traversing position:
// num. of segment and position in the segment
//-----------------------------------------------------------------------------------------

class lmSystemCursor
{
public:
    lmSystemCursor(lmScore* pScore);
    ~lmSystemCursor();

    bool ThereAreObjects() { return (m_nAbsMeasure <= m_nTotalMeasures); }

    //locate context for first note in this staff
	lmContext* GetStartOfSegmentContext(lmVStaff* pVStaff, int nStaff);

    //returns current absolute measure numeber (1..n) for VStaff
    int GetNumMeasure(lmVStaff* pVStaff) { return m_nAbsMeasure; }

    //methods related to AbsMeasure (to be replaced)
    inline void SetAbsMeasure(int nValue) { m_nAbsMeasure = nValue; }
    inline int GetAbsMeasure() { return m_nAbsMeasure; }
    inline int AdvanceNextBar() { return ++m_nAbsMeasure; };

    //methods related to nTotalMeasures (to be replaced)
    inline int GetTotalMeasures() { return m_nTotalMeasures; }

    //access to cursor for instrument nInstr (0..n-1)
    inline lmSOIterator* GetIterator(int nInstr) { return m_Iterators[nInstr]; }

private:

    lmScore*    m_pScore;
    int m_nAbsMeasure;      //number of bar in process (absolute, counted from the start of the score)
    int m_nTotalMeasures;   //num measures in the score

    std::vector<lmSOIterator*> m_Iterators;

};

lmSystemCursor::lmSystemCursor(lmScore* pScore)
{
    m_pScore = pScore;
    m_nAbsMeasure = 1;
    m_nTotalMeasures = ((m_pScore->GetFirstInstrument())->GetVStaff())->GetNumMeasures();

    //create iterators and point to start of each instrument
    lmInstrument* pInstr = m_pScore->GetFirstInstrument();
    for (; pInstr; pInstr=m_pScore->GetNextInstrument())
    {
        lmSOIterator* pIT = pInstr->GetVStaff()->CreateIterator(eTR_ByTime);
        pIT->AdvanceToMeasure(1);
        m_Iterators.push_back(pIT);
    }
}

lmSystemCursor::~lmSystemCursor()
{
    std::vector<lmSOIterator*>::iterator it;
    for (it = m_Iterators.begin(); it != m_Iterators.end(); ++it)
        delete *it;

    m_Iterators.clear();
}

lmContext* lmSystemCursor::GetStartOfSegmentContext(lmVStaff* pVStaff, int nStaff)
{
    //locate context for first note in this staff
	return pVStaff->GetStartOfSegmentContext(m_nAbsMeasure, nStaff);
}




//-----------------------------------------------------------------------------------------
// lmFormatter4 implementation
//-----------------------------------------------------------------------------------------

lmFormatter4::lmFormatter4()
{
    //initializations
    m_pSysCursor = (lmSystemCursor*)NULL;

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

lmFormatter4::~lmFormatter4()
{
}

lmBoxScore* lmFormatter4::LayoutScore(lmScore* pScore, lmPaper* pPaper)
{
    //Render a score justifying measures so that they fit exactly in page width.
    //This method encapsulates the line breaking algorithm and the spacing algorithm.
    //Page filling is not yet implemented.
    //This method is only invoked from lmScore::Layout()

    m_pScore = pScore;

    //verify that there is a score
    if (!m_pScore || m_pScore->GetNumInstruments() == 0) return (lmBoxScore*)NULL;

    ////DBG -------------------------------------------------------------------------
	//m_pScore->Dump(_T("dump.txt"));
	////END DBG ---------------------------------------------------------------------

    lmLUnits xStartOfMeasure;

    // get options for renderization
    bool fStopStaffLinesAtFinalBarline = m_pScore->GetOptionBool(_T("StaffLines.StopAtFinalBarline"));
    m_rSpacingFactor = (float) m_pScore->GetOptionDouble(_T("Render.SpacingFactor"));
    m_nSpacingMethod = (lmESpacingMethod) m_pScore->GetOptionLong(_T("Render.SpacingMethod"));
    m_nSpacingValue = (lmTenths) m_pScore->GetOptionLong(_T("Render.SpacingValue"));

    //create the root of the graphical model: BoxScore object
    lmBoxScore* pBoxScore = new lmBoxScore(m_pScore);

    //create first page
	int nNumPage = 0;
	m_pScore->SetPageInfo(++nNumPage);
    lmBoxPage* pBoxPage = pBoxScore->AddPage();

    //ensure that page cursors are at top-left corner
    pPaper->SetCursor(m_pScore->GetPageLeftMargin(), m_pScore->GetPageTopMargin());

    //for each staff size, setup fonts of right point size for that staff size
    lmInstrument *pInstr;
    lmVStaff *pVStaff;
    for (pInstr = m_pScore->GetFirstInstrument(); pInstr; pInstr=m_pScore->GetNextInstrument())
    {
        pVStaff = pInstr->GetVStaff();
        pVStaff->SetUpFonts(pPaper);
    }

	//Set up some spacing values, based on first instrument, first staff
	lmTenths rSpaceBeforeProlog = 7.5f;			//TODO: User options
	pInstr = m_pScore->GetFirstInstrument();
	pVStaff = pInstr->GetVStaff();
	m_uSpaceBeforeProlog = pVStaff->TenthsToLogical(rSpaceBeforeProlog, 1);


    // write score titles. Titles will be attached to current page (the fisr one)
    m_pScore->LayoutAttachedObjects(pBoxScore->GetCurrentPage(), pPaper);
    pPaper->SetCursor(m_pScore->GetPageLeftMargin(), m_pScore->GetPageTopMargin());
    pPaper->IncrementCursorY(m_pScore->TopSystemDistance());    //advance to skip headers

    int nSystem;        //number of system in process
    lmLUnits ySystemPos;    //paper y position at which current system starts


    // The algorithm has a main loop to process measures columns:
    //		The number of measures that fits in each system is finded out,
	//		as well as their size and the position of all their StaffObjs.
    //		The number of measures per system is stored in .... , the shapes are created
	//		and the positioning information is stored in the shapes.
    //-------------------------------------------------------------------------------------

    //compute systems indentation
    lmLUnits nFirstSystemIndent = 0;
    lmLUnits nOtherSystemIndent = 0;
    lmInstrument *pI;
    for (pI = m_pScore->GetFirstInstrument(); pI; pI=m_pScore->GetNextInstrument())
    {
        pI->MeasureNames(pPaper);
        nFirstSystemIndent = wxMax(nFirstSystemIndent, pI->GetIndentFirst());
        nOtherSystemIndent = wxMax(nOtherSystemIndent, pI->GetIndentOther());
    }


    //First loop: splitting the score into systems, and do all positioning and spacing.
    //Each loop cycle computes and justifies one system
    //----------------------------------------------------------------------------------

    //create new cursor and initialize it
    if (m_pSysCursor)
        delete m_pSysCursor;
    m_pSysCursor = new lmSystemCursor(m_pScore);   
    int iIni = m_pSysCursor->GetAbsMeasure();   //iIni = Measure in which the system starts

    //int nTotalMeasures = m_pSysCursor->GetTotalMeasures;
        //! @limit It is assumed that all staves have the same number of measures

    nSystem = 1;
    pBoxPage = pBoxScore->GetCurrentPage();
    lmBoxSystem* pBoxSystem;

    lmLUnits nSystemHeight;

    while (m_pSysCursor->ThereAreObjects())
    {
        m_nColumnsInSystem = 0;

        //set up tables for storing StaffObjs positioning information
        for (int i=1; i <= MAX_MEASURES_PER_SYSTEM; i++) {
            m_oTimepos[i].CleanTable();
            m_oTimepos[i].SetParameters(m_rSpacingFactor, m_nSpacingMethod, m_nSpacingValue);
        }

        //-------------------------------------------------------------------------------
        //Step 1: Form and size a column.
        //-------------------------------------------------------------------------------
        //inner loop: each loop cycle corresponds to processing a column
        //The column is sized and this space discunted from available line space.
        //The loop is exited when there is not enough space for including in this system
        //the column just computed or when a newSystem tag is found
        //-------------------------------------------------------------------------------

        //if this is not the first system advance vertically the previous system height
        if (nSystem != 1)
		{
            //Here Paper is positioned at the start of the new current system. No
            //computation for this is needed as staves' height and spacing have been
            //added to paper as renderization took place.
            //Lets' verify is there is space left in paper for next system
            //TODO It is assumed that next system height is equal to previous one.
            //     It is necesary to compute each system height
            lmLUnits yNew = pPaper->GetCursorY() + nSystemHeight;
            if (yNew > m_pScore->GetMaximumY() )
            {
                //wxLogMessage(_T("Page break needed. yCur=%.2f, yNew=%.2f, MaximumY=%.2f"), pPaper->GetCursorY(), yNew, m_pScore->GetMaximumY());
                //ensure that page cursors are at top-left corner
                pPaper->SetCursor(m_pScore->GetPageLeftMargin(), m_pScore->GetPageTopMargin());
                //start a new page
				m_pScore->SetPageInfo(++nNumPage);
                pBoxPage = pBoxScore->AddPage();
            }
       }

       //create the system container
        pBoxSystem = pBoxPage->AddSystem(nSystem);
        ySystemPos = pPaper->GetCursorY();  //save the start of system position
        pBoxSystem->SetPosition(pPaper->GetCursorX(), ySystemPos);
        pBoxSystem->SetYTop( ySystemPos );
        pBoxSystem->SetXLeft( pPaper->GetCursorX() );
        pBoxSystem->SetFirstMeasure(m_pSysCursor->GetAbsMeasure());

        bool fNewSystem = false;
        m_fFirstMeasureInSystem = true;
        m_nColumn = 1;      //first column of this system
        m_nAbsColumn = 1;
        while (m_pSysCursor->ThereAreObjects())
        {
            //reposition paper vertically at the start of the system. It has been advanced
            //when sizing the previous measure column
            pPaper->SetCursorY( ySystemPos );


            #if defined(__WXDEBUG__)
            g_pLogger->LogTrace(_T("Formatter4.Step1"),
                _T("m_uMeasure=%d, Paper X = %.2f"), m_nColumn, pPaper->GetCursorX() );
            #endif

			//if start of system, set initial system length
            //TODO: Is this needed?
            if (m_fFirstMeasureInSystem)
			{
				pBoxSystem->UpdateXRight( m_pScore->GetRightMarginXPos() );
            }

            //size this measure column create BoxSlice (and BoxSlice hierarchy) for
            //the measure being processed
            m_uMeasureSize[m_nColumn] =
                SizeMeasureColumn(nSystem, pBoxSystem, pPaper, &fNewSystem,
                                  (nSystem == 1 ? nFirstSystemIndent : nOtherSystemIndent) );

            #if defined(__WXDEBUG__)
            g_pLogger->LogTrace(_T("Formatter4.Step1"),
                _T("m_uMeasureSize[%d] = %.2f"), m_nColumn, m_uMeasureSize[m_nColumn] );
            #endif

            //if this is the first measure column compute the space available in
            //this system. The method is a little tricky. The total space available
            //is (pScore->GetPageRightMargin() - pScore->GetCursorX()). But we have
            //to take into account the space that will be used by the prolog. As the
            //left position of the first measure column has taken all this into account,
            //it is posible to use that value by just doing:
            if (m_fFirstMeasureInSystem) {
                m_uFreeSpace =
                    m_pScore->GetRightMarginXPos() - m_oTimepos[m_nColumn].GetStartOfBarPosition();
            }

            #if defined(__WXDEBUG__)
            g_pLogger->LogTrace(_T("Formatter4.Step1"),
                _T("RelMeasure=%d, m_uFreeSpace = %.2f, PaperRightMarginXPos=%.2f, StartOfBar=%.2f"),
                m_nColumn, m_uFreeSpace, m_pScore->GetRightMarginXPos(),
                m_oTimepos[m_nColumn].GetStartOfBarPosition() );
            g_pLogger->LogTrace(_T("Formatter4.Step1"),
                m_oTimepos[m_nColumn].DumpTimeposTable());
            #endif

            //substract space ocupied by this measure from space available in the system
            if (m_uFreeSpace < m_uMeasureSize[m_nColumn])
			{
                //there is no enough space for this measure column.

				//for this measure ScoreObjs, remove 'position computed' mark
                ResetLocation();

                //exit the loop. The system is finished
                break;
            } else {
                //there is enough space for this measure column. Add it to current system and
                //discount the space that the measure will take
                m_uFreeSpace -= m_uMeasureSize[m_nColumn];
                m_nColumnsInSystem++;
            }

            //if newSystem tag found force to finish current system
            if (fNewSystem) break;

            //advance to next bar
            m_pSysCursor->AdvanceNextBar();
            m_nColumn++;
            m_nAbsColumn++;
            m_fFirstMeasureInSystem = false;

        }    //end of loop to process a measure column

        //helper flag to signal if current system is the last one.
        //HERE-TODO: To determine if this is last system we should check if there are
        //more objects. Otherwise we have reached end of score
        bool fThisIsLastSystem =
                (iIni + m_nColumnsInSystem > m_pSysCursor->GetTotalMeasures());

        //-------------------------------------------------------------------------------
        //Step 2: Justify measures (distribute remainnig space across all measures)
        //-------------------------------------------------------------------------------
        //At this point the number of measures to include in current system has been computed
        //and some data is stored in the following global variables:
        //
        //   m_oTimepos[1..MaxBar] - positioning information for measure columns
        //   m_uFreeSpace - free space available on this system
        //   m_uMeasureSize[1..MaxBar] - stores the minimum size for each measure column for
        //           the current system.
        //   m_nColumnsInSystem  - the number of measures that fit in this system
        //
        //Now we proceed to re-distribute the remaining free space across all measures, so that
        //the system is justified. This step only computes the new measure column sizes and stores
        //them in table m_uMeasureSize[1..MaxBar] but changes nothing in the StaffObjs
        //-------------------------------------------------------------------------------
        if (m_nColumnsInSystem == 0)
        {
            //The line width is not enough for drawing just one bar!!!

            //We have to split the measure
            if (SplitMeasureColumn())
            {
                //it has been requested to abort layout. Return an empty score
			    if (pBoxScore) delete pBoxScore;
                pBoxScore = new lmBoxScore(m_pScore);
                m_rSpacingFactor *= 0.7f;
                //return LayoutScore(pPaper);
	            return pBoxScore;
            }
            //As a consequence of splitting the measure, the score will have one more measure.
            //In order to take this into account incrementing measure
            //count, rendering twice this measure, or ...?

        }

        //dbg --------------
        if (m_fDebugMode) {
            wxLogMessage(_T("Before distributing free space"));
            wxLogMessage(_T("***************************************\n"));
            for (int i = 1; i <= m_nColumnsInSystem; i++) {
                wxLogMessage(wxString::Format(
                    _T("Bar column %d. Size = %.2f"), i, m_uMeasureSize[i]));
            }
        }
        //dbg ---------------

        //To justify systems, divide up the remaining space
        //between all bars, except if this is the last bar and options flag
        //"StopStaffLinesAtFinalBar" is set or no barline.
        if (!(fThisIsLastSystem && fStopStaffLinesAtFinalBarline))
            RedistributeFreeSpace(m_uFreeSpace, fThisIsLastSystem);

        //dbg --------------
        if (m_fDebugMode) {
            wxLogMessage(_T("After distributing free space"));
            wxLogMessage(_T("***************************************\n"));
            for (int i = 1; i <= m_nColumnsInSystem; i++) {
                wxLogMessage(wxString::Format(
                    _T("Bar column %d. Size = %.2f"), i, m_uMeasureSize[i]));
            }
        }
        //dbg --------------


        //-------------------------------------------------------------------------------
        //Step 3: Re-position StaffObjs.
        //-------------------------------------------------------------------------------
        //when reaching this point the table m_uMeasureSize[i] stores the final size that
        //must have each measure column of this system.
        //Now proceed to change StaffObjs locations so that they are evenly distributed across
        //the the bar.

        //dbg ------------------------------------------------------------------------------
        if (m_fDebugMode) {
            wxLogMessage(_T("Before repositioning objects"));
            wxLogMessage(_T("***************************************\n"));
            wxLogMessage( m_pScore->Dump() );
        }
        //dbg ------------------------------------------------------------------------------

        xStartOfMeasure = m_oTimepos[1].GetStartOfBarPosition();
        for (int i=1; i <= m_nColumnsInSystem; i++) {
            //GrabarTrace "RedistributeSpace: nNewSize = " & m_uMeasureSize(i) & ", newStart = " & xStartOfMeasure    //dbg
            xStartOfMeasure = m_oTimepos[i].RedistributeSpace(m_uMeasureSize[i], xStartOfMeasure);
        }

        //dbg ------------------------------------------------------------------------------
        if (m_fDebugMode) {
            wxLogMessage(_T("After repositioning objects"));
            wxLogMessage(_T("***************************************\n"));
            wxLogMessage( m_pScore->Dump() );
        }
        //dbg ------------------------------------------------------------------------------



        //-------------------------------------------------------------------------------
        //Step 4: Do final layout of aux objects
		//
		//During layout phase it is not possible to set some measurements that depend on
		//the final x position of two or more notes. For example:
		//	- Beams: it is not posible to adjust the lenght of note stems in a beamed
		//		group until the x position of the notes is finally established.
		//	- Ties: start and end points can not be fixed until their parent notes are
		//		fixed.
        //-------------------------------------------------------------------------------
        //when reaching this point all StaffObjs locations are their final locations.

        //Update information about this system
        pBoxSystem->SetNumMeasures(m_nColumnsInSystem, m_pScore);
        if (fThisIsLastSystem && fStopStaffLinesAtFinalBarline)
        {
            //this is the last system and it has been requested to stop staff lines
            //in last measure. So, set final x so staff lines go to final bar line
            lmLUnits xPos;
            if (pVStaff->GetBarlineOfLastNonEmptyMeasure(&xPos))
                pBoxSystem->UpdateXRight( xPos - 1 );
            else
                pBoxSystem->UpdateXRight( m_pScore->GetRightMarginXPos() );
        }
        else
        {
            //staff lines go to the rigth margin
            pBoxSystem->UpdateXRight( m_pScore->GetRightMarginXPos() );
        }

	    //Add the shape for the initial barline that joins all staves in a system
	    if (m_pScore->GetOptionBool(_T("Staff.DrawLeftBarline")) && !pVStaff->HideStaffLines() )
	    {
			lmLUnits uxPos = pBoxSystem->GetXLeft() +
						     (nSystem == 1 ? nFirstSystemIndent : nOtherSystemIndent);
		    lmLUnits uLineThickness = lmToLogicalUnits(0.2, lmMILLIMETERS);        // thin line width will be 0.2 mm TODO user options
            lmShapeLine* pLine =
                new lmShapeLine(pVStaff, uxPos, pBoxSystem->GetYTopFirstStaff(),
						    uxPos, pBoxSystem->GetYBottom(),
						    uLineThickness, 0.0, *wxBLACK, _T("System joining line"),
							eEdgeHorizontal);
	        pBoxSystem->AddShape(pLine);
	    }

        //update lmBoxSlices with the final measures sizes, except for last
        //measure, as its length has been already set up
		lmLUnits xEnd = m_oTimepos[1].GetStartOfBarPosition();
        for (int iRel=1; iRel <= m_nColumnsInSystem; iRel++)
        {
            lmLUnits xStart = xEnd;
            xEnd = xStart + m_uMeasureSize[iRel];
            lmBoxSlice* pBoxSlice = pBoxSystem->GetSlice(iRel);
			pBoxSlice->UpdateXLeft(xStart);
            if (iRel < m_nColumnsInSystem)
			    pBoxSlice->UpdateXRight(xEnd);
        }

        // compute system height
        if (nSystem == 1) {
            nSystemHeight = pPaper->GetCursorY() - ySystemPos;
            //wxLogMessage(_T("[lmFormatter4::LayoutScore] nSystemHeight = %.2f"),
            //    nSystemHeight );
        }


        //increment loop information
        iIni += m_nColumnsInSystem;
        m_pSysCursor->SetAbsMeasure(iIni);
        //HERE-TODO
        //I should store Iterators for each column. Then, at this point, I will update
        //last iterator
        nSystem++;

    }    //while (ThereAreObjects())


    //Here the score is prepared. Last staff lines are finished at the right margin or
    //at the final bar, depending on flag fStopStaffLinesAtFinalBarline.
    //If this flag is false and option 'Score.FillPageWithEmptyStaves' is true it means
    //that the user has requested to fill the remaining page space with empty systems.
    //Let's proceed to do it.
    bool fFillPageWithEmptyStaves = m_pScore->GetOptionBool(_T("Score.FillPageWithEmptyStaves"));
    if (!fStopStaffLinesAtFinalBarline && fFillPageWithEmptyStaves)
    {
        //fill the remaining page space with empty staves
        while (true)      //loop is exited when reaching end of page
        {
            //if this is not the first system advance vertically the previous system height
            if (nSystem != 1) {
                //Here Paper is positioned at the start of the new current system.
                //Lets' verify is there is space left in paper for next system
                //TODO It is assumed that next system height is equal to previous one.
                //!     It is necesary to compute each system height
                lmLUnits yNew = pPaper->GetCursorY() + nSystemHeight;
                if (yNew > m_pScore->GetMaximumY() )
                    break;        //exit loop
            }

            //create the system container
            pBoxSystem = pBoxPage->AddSystem(nSystem);
            ySystemPos = pPaper->GetCursorY();  //save the start of system position
            pBoxSystem->SetPosition(pPaper->GetCursorX(), ySystemPos);
            pBoxSystem->SetFirstMeasure( m_pSysCursor->GetAbsMeasure() );
            pBoxSystem->SetIndent(((nSystem == 1) ? nFirstSystemIndent : nOtherSystemIndent ));

            m_nColumn = 1;          //first column of this system
            m_nAbsColumn++;
            m_fFirstMeasureInSystem = true;
            AddEmptyMeasureColumn(nSystem, pBoxSystem, pPaper);

            //Store information about this system
            pBoxSystem->SetNumMeasures(0, m_pScore);
            //staff lines go to the rigth margin
            pBoxSystem->UpdateXRight( m_pScore->GetRightMarginXPos() );

            // compute system height
            if (nSystem == 1) {
                nSystemHeight = ComputeSystemHeight(pPaper);
            }

            //advance paper
            pPaper->SetCursorY( pPaper->GetCursorY() + nSystemHeight );

            nSystem++;
        }
    }

    return pBoxScore;

}

lmLUnits lmFormatter4::ComputeSystemHeight(lmPaper* pPaper)
{
    //compute the height of the system
    lmInstrument *pInstr;
    lmVStaff *pVStaff;

    // save paper position to restore it at the end
    lmLUnits xPaperStart = pPaper->GetCursorX();
    lmLUnits yPaperStart = pPaper->GetCursorY();

    // explore all instruments in the score
    lmLUnits yPaperPos;
    for (pInstr = m_pScore->GetFirstInstrument(); pInstr; pInstr=m_pScore->GetNextInstrument())
    {
        pVStaff = pInstr->GetVStaff();

        //save this VStaff paper position
        yPaperPos = pPaper->GetCursorY();

        //advance paper in height off this lmVStaff
        pVStaff->NewLine(pPaper);
        //TODO add inter-staff space

    }    // next lmInstrument

    lmLUnits nSystemHeight = pPaper->GetCursorY() - yPaperStart;

    //restore paper position
    pPaper->SetCursorX( xPaperStart );
    pPaper->SetCursorY( yPaperStart );

    return nSystemHeight;

}

bool lmFormatter4::SplitMeasureColumn()
{
    //We have measured a column and it doesn't fit in a line. Data about this measure column
    //is stored in the following global variables:
    //
    //   m_oTimepos[1] - positioning information
    //   m_uFreeSpace - free space available on this system
    //   m_uMeasureSize[1] - the minimum size for this measure column
    //
    //This method must split the measure by inserting a splitBar object.
    //As a consequence of splitting the measure, the score will have one more measure. This
    //has to be taken into account by formatter algorithm, either by incrementing measure
    //count, rendering twice this measure, or ...?
    //The previously mentioned global variables must be updated to keep only the data for the
    //measure part to be rendered in current system.
    //Variable  m_nColumnsInSystem must be set to one.

    //dbg --------------
    wxLogMessage(_T("[lmFormatter4::SplitMeasureColumn]. Dump of relevant info"));
    wxLogMessage(_T("*********************************************************\n"));
    wxLogMessage(wxString::Format(_T("Measure width= %.2f, free space=%.2f, m_rSpacingFactor=%f"),
                 m_uMeasureSize[1], m_uFreeSpace, m_rSpacingFactor));
    m_oTimepos[1].DumpTimeposTable();
    //dbg ---------------

    wxString sMsg = _("Program failure: not enough space for drawing just one bar.");
    sMsg += _T("\n\n");
    sMsg += _("Posible causes:");
    sMsg += _T("\n");
    sMsg += _("- No time signature specified.");
    sMsg += _T("\n");
    sMsg += _("- Paper width is not enough for drawing just one bar.");

    ::wxLogFatalError(sMsg);
    return true;        //abort rederization
}

lmLUnits lmFormatter4::SizeMeasureColumn(int nSystem,
                                         lmBoxSystem* pBoxSystem, lmPaper* pPaper,
                                         bool* pNewSystem, lmLUnits nSystemIndent)
{
    // For each instrument and staff it is computed the size of this measure.
    // Also the hierarchy of BoxSlice objects is created for this measure.
    //
    // All measurements are stored in the global object m_oTimepos[m_nColumn], so that other
    // procedures can take decisions about the final number of measures to include in a system
    // and for repositioning the StaffObjs.
    //
    // Input parameters:
    //   nSystem - System number
    //   nSystemIndent - indentation for first measure
    //
    // Returns:
    //   - The size of this measure column.
    //   - positioning information for this measure column is stored in m_oTimepos[m_nColumn]
    //   - Updates flag pointed by pNewSystem and sets it to true if newSystem tag found
    //        in this measure
    //

    lmInstrument* pInstr;
    bool fNewSystem = false;

    wxLogMessage(_T("[lmFormatter4::SizeMeasureColumn] m_nAbsColumn=%d, xPaper=%.2f "),
                    m_nAbsColumn, pPaper->GetCursorX() );

    //determine xLeft position for this measure:
    //      if fisrt measure: xPaper + system indent
    //      for other measures it doesnt'n matter: use xPaper
    lmLUnits xStartPos = pPaper->GetCursorX() + (m_fFirstMeasureInSystem ? nSystemIndent : 0.0f);
    lmLUnits yPaperPos = pPaper->GetCursorY();

    // create an empty BoxSlice to contain this measure column
    lmBoxSlice* pBoxSlice = pBoxSystem->AddSlice(m_nAbsColumn);    
	pBoxSlice->SetYTop( yPaperPos );
	pBoxSlice->SetXLeft( xStartPos );

    // explore all instruments in the score
	lmLUnits yBottomLeft = 0;
	int nInstr = 0;
    for (pInstr = m_pScore->GetFirstInstrument(); pInstr; pInstr=m_pScore->GetNextInstrument(), nInstr++)
    {
        //verify that program limits are observed
        if (pInstr->GetNumStaves() > MAX_STAVES_PER_SYSTEM) {
            //TODO log and display message properly
            wxMessageBox(
                _("Program limitation: Maximum number of staves per system has been exceeded."));
            wxASSERT(false);
        }

        pPaper->SetCursorX( xStartPos );    //to align start of all staves in this system
        yPaperPos = pPaper->GetCursorY();

        // create an empty BoxSliceInstr to contain this instrument slice
        lmBoxSliceInstr* pBSI = pBoxSlice->AddInstrument(pInstr);
		pBSI->SetYTop( yPaperPos );
		pBSI->SetXLeft( pBoxSlice->GetXLeft() );

        //explore all its VStaff to size the measure column m_nAbsColumn.
        //All collected information is stored in m_oTimepos[m_nColumn]
        lmVStaff* pVStaff = pInstr->GetVStaff();

        // create the BoxSliceVStaff
        lmBoxSliceVStaff* pBSV = pBSI->AddVStaff(pVStaff, m_pSysCursor->GetNumMeasure(pVStaff));

        // if first measure in system add the ShapeStaff
		if (m_fFirstMeasureInSystem)
		{
			// Final xPos is yet unknown, so I use zero.
			// It will be updated when the system is completed
			yBottomLeft = pVStaff->LayoutStaffLines(pBoxSystem, xStartPos, 0.0, yPaperPos);
		}

		//update bounds of VStaff slice
		pBSV->SetXLeft(xStartPos);
		pBSV->SetYTop(yPaperPos);
		pBSV->SetYBottom(yBottomLeft);

        fNewSystem |= SizeMeasure(pBSV, pVStaff, nInstr, pPaper);

        //update bounds of boxes
		pBSI->SetYBottom(yBottomLeft);
		pBoxSlice->SetYBottom(yBottomLeft);

        // if first measure in system add instrument name and bracket/brace.
        // Instrument is also responsible for managing group name and bracket/brace layout
		if (m_fFirstMeasureInSystem)
            pInstr->AddNameAndBracket(pBoxSystem, pBSI, pPaper, nSystem);

        //advance paper in height off this lmVStaff
        //AWARE As advancing one staff has the effect of returning
        //x position to the left marging, all x position information stored
        //in m_timepos is relative to the start of the measure
        pVStaff->NewLine(pPaper);
        //TODO add inter-staff space

    }    // next lmInstrument

    //all measures in measure column number m_nAbsColumn have been sized. The information is stored in
    //object m_oTimepos[m_nColumn]. Now proced to re-position the StaffObjs so that all StaffObjs
    //sounding at the same time will have the same x coordinate. The method .ArrageStaffobjsByTime
    //returns the measure column size
    *pNewSystem = fNewSystem;
    bool fTrace =  m_fDebugMode && (m_nTraceMeasure == 0  || m_nTraceMeasure == m_nAbsColumn);

    m_oTimepos[m_nColumn].EndOfData();        //inform that all data has been suplied
    return m_oTimepos[m_nColumn].DoSpacing(fTrace);

}


void lmFormatter4::AddEmptyMeasureColumn(int nSystem, lmBoxSystem* pBoxSystem, lmPaper* pPaper)
{

    lmInstrument* pInstr;
    lmVStaff* pVStaff;
    //bool fNewSystem = false;

    lmLUnits xPaperPos, yPaperPos;
    lmLUnits xStartPos = pPaper->GetCursorX();      //save x pos to align staves in system

    // create an empty BoxSlice to contain this measure column
    lmBoxSlice* pBoxSlice = pBoxSystem->AddSlice(m_nAbsColumn);
	bool fUpdateSlice = true;

	//initial conditions
	bool fFirstStaffInSystem = true;

    // explore all instruments in the score
	lmLUnits yBottomLeft = 0;
    for (pInstr = m_pScore->GetFirstInstrument(); pInstr; pInstr=m_pScore->GetNextInstrument())
    {
        pPaper->SetCursorX( xStartPos );    //align start of staves in this system

        // create an empty BoxSliceInstr to contain this instrument slice
        lmBoxSliceInstr* pBSI = pBoxSlice->AddInstrument(pInstr);
		bool fUpdateInstr = true;

        //For current instrument, explore its VStaff to size the measure
        //column m_nAbsColumn. All collected information is stored in m_oTimepos[m_nColumn]
        pVStaff = pInstr->GetVStaff();

        //save this VStaff paper position
        xPaperPos = pPaper->GetCursorX();
        yPaperPos = pPaper->GetCursorY();

        // create the BoxSliceVStaff
        lmBoxSliceVStaff* pBSV = pBSI->AddVStaff(pVStaff, m_pSysCursor->GetNumMeasure(pVStaff));
        // if first measure in system add the ShapeStaff
		if (m_fFirstMeasureInSystem)
		{
			// Final xPos is yet unknown, so I use zero.
			// It will be updated when the system is completed
			yBottomLeft = pVStaff->LayoutStaffLines(pBoxSystem, xPaperPos, 0.0, yPaperPos);
		}

		//save start position of this system, slice, instrument and vstaff
		if (fFirstStaffInSystem)
		{
			//start of system
			fFirstStaffInSystem = false;
			pBoxSystem->SetXLeft(xPaperPos);
			pBoxSystem->SetYTop(yPaperPos);
		}
		if (fUpdateSlice)
		{
			//start of slice
			pBoxSlice->SetXLeft(xPaperPos);
			pBoxSlice->SetYTop(yPaperPos);
			fUpdateSlice = false;
		}
		if (fUpdateInstr)
		{
			//start of instrument
			pBSI->SetXLeft(xPaperPos);
			pBSI->SetYTop(yPaperPos);
			fUpdateInstr = false;
		}
		//start of VStaff slice
		pBSV->SetXLeft(xPaperPos);
		pBSV->SetYTop(yPaperPos);
		pBSV->SetYBottom(yBottomLeft);

        //The prolog must be rendered on each system, but the
        //matching StaffObjs only exist in the first system. Therefore:
        //1. in the first system the prolog is rendered as part as the normal lmStaffObj
        //   rendering process and the available space for measures is all the paper width.
        //2. but for the other systems we must force the rendering of the prolog because there
        //   are no StaffObjs representing the prolog.
        if (nSystem != 1 && m_fFirstMeasureInSystem)
		{
			pPaper->SetCursorX(xPaperPos);
        }

        //fNewSystem |= SizeMeasure(pBSV, pVStaff, pPaper);

        //advance paper in height off this lmVStaff
        //AWARE As advancing one staff has the effect of returning
        //x position to the left marging, all x position information stored
        //in m_timepos is relative to the start of the measure
        pVStaff->NewLine(pPaper);
        //TODO add inter-staff space

        ///*** Update measures of this BoxSliceVStaff

        ///*** Update measures of this BoxSliceInstr
		pBSI->SetYBottom(yBottomLeft);
		pBoxSlice->SetYBottom(yBottomLeft);

  //      // if first measure in system add instrument names and brackets/bracets
		//if (m_fFirstMeasureInSystem)
		//{
		//	if (nSystem == 1)
		//		pInstr->AddNameShape(pBSI, pPaper);
		//	else
		//		pInstr->AddAbbreviationShape(pBSI, pPaper);
		//}

    }    // next lmInstrument

}

void lmFormatter4::ResetLocation()
{
  //  // explore all instruments in the score
  //  lmInstrument* pInstr;
  //  for (pInstr = m_pScore->GetFirstInstrument(); pInstr; pInstr=m_pScore->GetNextInstrument())
  //  {
  //      //For current instrument, explore its VStaff
  //      lmVStaff* pVStaff = pInstr->GetVStaff();

		////loop to process all StaffObjs in this measure
		//lmSOIterator* pIT = pVStaff->CreateIterator(eTR_AsStored);
		//pIT->AdvanceToMeasure( m_pSysCursor->GetAbsMeasure() );
		//while(!pIT->EndOfMeasure())
		//{
		//	lmStaffObj* pSO = pIT->GetCurrent();
		//	pSO->ResetObjectLocation();
		//	pIT->MoveNext();
		//}
		//delete pIT;
  //  }
}


void lmFormatter4::RedistributeFreeSpace(lmLUnits nAvailable, bool fLastSystem)
{
    //Step 3: Justify measures (distribute remainnig space across all measures)
    //-------------------------------------------------------------------------------
    //In summary, the algoritm computes the average size of all measure columns and
    //increases the size of all measure columns that are narrower than the average size,
    //so that their size is grown to the average size. The process of computing the new
    //average and enlarging the narrower than average measures is repeated until all
    //space is distributed.
    //The system is not justified if this is the last system and there is no barline
    //in the last measure.
    //
    //on entering in this function:
    // - the table m_uMeasureSize() stores the minimum size for each measure column for
    //   the current system.
    // - nAvailable stores the free space remaining at the end of this system
    //
    //on exit:
    // - the values stored in table m_uMeasureSize() are modified to reflect the new size
    //   for the bar columns, so that the line get justified.
    //
    //-------------------------------------------------------------------------------------

    if (nAvailable <= 0) return;       //no space to distribute

    //The system must not be justified if this is the last system and there is no barline
    //in the last measure. Check this.
    if (fLastSystem)
    {
        lmBarline* pBar = m_oTimepos[m_nColumnsInSystem].GetBarline();
        if (!pBar)
            return;     //no need to justify
    }

    //Check for program limits
    lmLUnits nDif[MAX_MEASURES_PER_SYSTEM];
    if (m_nColumnsInSystem == MAX_MEASURES_PER_SYSTEM)
    {
        wxMessageBox( wxString::Format(_T("Program limit reached: no more than %d measures per system"),
                        MAX_MEASURES_PER_SYSTEM ));
        return;
    }

    //compute average measure column size
    lmLUnits nAverage = 0;
    for (int i = 1; i <= m_nColumnsInSystem; i++) {
        nAverage += m_uMeasureSize[i];
    }
    nAverage /= m_nColumnsInSystem;

    lmLUnits nMeanPrev = 0;
    lmLUnits nDifTotal = 0;
    while (nAvailable > 0 && nAverage != nMeanPrev) {
        //for each measure column, compute the diference between its size and the average size
        //sum up all the diferences in nDifTotal
        nDifTotal = 0;
        for (int i = 1; i <= m_nColumnsInSystem; i++) {
            nDif[i] = nAverage - m_uMeasureSize[i];
            if (nDif[i] > 0) { nDifTotal += nDif[i]; }
        }

        //if the sum of all diferences is greater than the space to distribute
        //reduce the differences
        while (nDifTotal > nAvailable) {
            nDifTotal = 0;
            for (int i = 1; i <= m_nColumnsInSystem; i++) {
                if (nDif[i] > 0) {
                    nDif[i]--;
                    nDifTotal += nDif[i];
                }
            }
        }

        //The size of all measure columns whose size is lower than the average
        //is going to be increased by the amount stated in the differences table
        for (int i = 1; i <= m_nColumnsInSystem; i++) {
            if (nDif[i] > 0) { m_uMeasureSize[i] += nDif[i]; }
        }
        nAvailable -= nDifTotal;

        //compute the new measure column size average
        nMeanPrev = nAverage;
        nAverage = 0;
        for (int i = 1; i <= m_nColumnsInSystem; i++) {
            nAverage += m_uMeasureSize[i];
        }
        nAverage /= m_nColumnsInSystem;
    }

    //divide up the remaining space between all bars
    if (nAvailable > 0) {
        nDifTotal = nAvailable / m_nColumnsInSystem;
        for (int i = 1; i <= m_nColumnsInSystem; i++) {
            m_uMeasureSize[i] += nDifTotal;
            nAvailable -= nDifTotal;
        }
        m_uMeasureSize[m_nColumnsInSystem] += nAvailable;
    }

}


//=========================================================================================
// Methods to deal with measures
//=========================================================================================
#define lmNEW 1

bool lmFormatter4::SizeMeasure(lmBoxSliceVStaff* pBSV, lmVStaff* pVStaff, 
							   int nInstr, lmPaper* pPaper)
{
    // Compute the width of the requested measure of the lmVStaff
    // Input variables:
    //   pVStaff - lmVStaff to process
	//   nInstr - instrument number 0..n
    // Results:
    //   all measurements are stored in global variable  m_oTimepos[m_nColumn]

    //   return bool: true if newSystem tag found in this measure

    //BUG_BYPASS:
    //These algorithm was designed for scores in which all staves have the same number
    //of measures. This is the normal case when loading a complete score, for eBooks.
    //But in score edition, the opposite is the normal case. For example,
    //assume a choir SATB four staves score. You start adding notes and measures in the first
    //staff and all other staves are empty. They could be synchronized by adding measures with
    //rests, but this is not yet implemented. Also, it is not clear if that is going to be 
    //the solution. For now, next 'if' sentence works.
    if(m_pSysCursor->GetAbsMeasure() > pVStaff->GetNumMeasures())
        return false;       //this instrument has less measures than maximum

    wxLogMessage(_T("[lmFormatter4::SizeMeasure] m_nAbsColumn=%d"), m_nAbsColumn);

    //add some space at start of measure, if necessary
    lmLUnits uSpaceAfterStart = 0.0f;
    if (m_fFirstMeasureInSystem)
    {
        //if first measure of system, add some space before prolog
        uSpaceAfterStart = m_uSpaceBeforeProlog;
    }
    else
    {
        //Not first measure of system. Get the previous barline and add some space if
        //the previous barline is visible.
        lmBarline* pBar = pVStaff->GetBarlineOfMeasure(m_pSysCursor->GetAbsMeasure()-1, NULL);
        if (pBar)
        {
            if (pBar->IsVisible())
                uSpaceAfterStart = pVStaff->TenthsToLogical(20.0f);    // TODO: user options
        }
    }

    //start a voice for each staff
    lmLUnits uxStart = pPaper->GetCursorX();
    m_oTimepos[m_nColumn].StartLines(nInstr, uxStart, pVStaff, uSpaceAfterStart);

    //The prolog (clef and key signature) must be rendered on each system, but the
    //matching StaffObjs only exist in the first system. In the first system the prolog
	//is rendered as part as the normal lmStaffObj rendering process, so there is nothig
	//special to do to render the prolog But for the other systems we must force the
	//rendering of the prolog because there are no StaffObjs representing the prolog.
    if (m_pSysCursor->GetAbsMeasure() != 1 && m_fFirstMeasureInSystem)
	{
		AddProlog(pBSV, false, pVStaff, nInstr, pPaper);
	}

    //loop to process all StaffObjs in this measure
	//int nCurVoice = 0;
    //bool fNoteRestFound = false;
    bool fNewSystem = false;                //newSystem tag found
    lmStaffObj* pSO = (lmStaffObj*)NULL;
#if lmNEW
    lmSOIterator* pIT = m_pSysCursor->GetIterator(nInstr);
    while(!pIT->EndOfList())
#else
    lmSOIterator* pIT = pVStaff->CreateIterator(eTR_ByTime);
    pIT->AdvanceToMeasure( m_pSysCursor->GetAbsMeasure() );
    while(!pIT->EndOfMeasure())
#endif
    {
        pSO = pIT->GetCurrent();
        EStaffObjType nType = pSO->GetClass();

        if (nType == eSFOT_Barline) break;         //End of measure: exit loop.

        else if (nType == eSFOT_Control)
        {
            ESOCtrolType nCtrolType = ((lmSOControl*)pSO)->GetCtrolType();
            if(lmNEW_SYSTEM == nCtrolType)
            {
                //new system tag found in this measure
                fNewSystem = true;
            }
			else {
				wxLogMessage(_T("lmFormatter4::SizeMeasure] Bad SOControl type"));
				wxASSERT(false);
			}
        }

        else if (nType == eSFOT_KeySignature)
		{
			pPaper->SetCursorX(uxStart);
			AddKey((lmKeySignature*)pSO, pBSV, pPaper, pVStaff, nInstr);
        }

        else if (nType == eSFOT_TimeSignature)
		{
			pPaper->SetCursorX(uxStart);
			AddTime((lmTimeSignature*)pSO, pBSV, pPaper, pVStaff, nInstr);
		}

		else
		{
			//create this lmStaffObj shape and add to table
			pPaper->SetCursorX(uxStart);
			pSO->Layout(pBSV, pPaper);
			lmShape* pShape = pSO->GetShape();
			m_oTimepos[m_nColumn].AddEntry(nInstr, pSO, pShape, false);
        }

        pIT->MoveNext();
    }
#if !lmNEW
    delete pIT;
#endif

    //The barline lmStaffObj is not included in the loop as it might not exist in the last
    //bar of a score. In theses cases, the loop is exited because the end of the score is
    //reached. In any case we have to close the line
    if (pSO && pSO->GetClass() == eSFOT_Barline)
    {
#if lmNEW
        pIT->MoveNext();    //leave cursor pointing to next measure
#endif
		//lmLUnits uPos = pPaper->GetCursorX();
        pPaper->SetCursorX(uxStart);
        pSO->Layout(pBSV, pPaper);
        lmShape* pShape = pSO->GetShape();
        m_oTimepos[m_nColumn].CloseLine(pSO, pShape, uxStart);
    }
    else
	{
        // no barline at the end of the measure.
        m_oTimepos[m_nColumn].CloseLine((lmStaffObj*)NULL, (lmShape*)NULL, uxStart);
    }

    return fNewSystem;
}

void lmFormatter4::AddProlog(lmBoxSliceVStaff* pBSV, bool fDrawTimekey, lmVStaff* pVStaff,
                             int nInstr, lmPaper* pPaper)
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
    lmLUnits xStartPos = pPaper->GetCursorX();      //Save x to align all clefs
    lmLUnits yStartPos = pPaper->GetCursorY();

    //iterate over the collection of lmStaff objects to draw current clef and key signature

    lmStaff* pStaff = pVStaff->GetFirstStaff();
    lmLUnits uyOffset = pVStaff->GetTopMargin();
    lmLUnits xPos = 0.0f;

    lmContext* pContext = (lmContext*)NULL;
    for (int nStaff=1; nStaff <= pVStaff->GetNumStaves(); pStaff = pVStaff->GetNextStaff(), nStaff++)
    {
        xPos = xStartPos;

        //locate context for first note in this staff
		//pContext = pVStaff->GetStartOfSegmentContext(m_pSysCursor->GetAbsMeasure(), nStaff);
        pContext = m_pSysCursor->GetStartOfSegmentContext(pVStaff, nStaff);

        if (pContext) {
            pClef = pContext->GetClef();
            pKey = pContext->GetKey();
            pTime = pContext->GetTime();

            //render clef
            if (pClef) {
                nClef = pClef->GetClefType();
				if (pClef->IsVisible())
                {
					lmUPoint uPos = lmUPoint(xPos, yStartPos+uyOffset);        //absolute position
					lmShape* pShape = pClef->AddShape(pBSV, pPaper, uPos);
					xPos += pShape->GetWidth();
					m_oTimepos[m_nColumn].AddEntry(nInstr, pClef, pShape, true);
				}
            }

            //render key signature
            if (pKey && pKey->IsVisible())
            {
                lmUPoint uPos = lmUPoint(xPos, yStartPos+uyOffset);        //absolute position
                lmShape* pShape = pKey->CreateShape(pBSV, pPaper, uPos, nClef, pStaff);
				xPos += pShape->GetWidth();
				m_oTimepos[m_nColumn].AddEntry(nInstr, pKey, pShape, true, nStaff);
            }

        }

        //compute prolog width
        uPrologWidth = wxMax(uPrologWidth, xPos - xStartPos);

        //compute vertical displacement for next staff
        uyOffset += pStaff->GetHeight();
        uyOffset += pStaff->GetAfterSpace();

    }

    // update paper cursor position
    pPaper->SetCursorX(xStartPos + uPrologWidth);

}

void lmFormatter4::AddKey(lmKeySignature* pKey, lmBox* pBox, lmPaper* pPaper,
						  lmVStaff* pVStaff, int nInstr)
{
    // This method is responsible for creating the key signature shapes for 
    // all staves of this instrument. And also, of adding them to the graphical 
    // model and to the Timepos table

    //create the shapes
    pKey->Layout(pBox, pPaper);

	//add the shapes to the timepos table
	lmShape* pMainShape = ((lmStaffObj*)pKey)->GetShape();          //cast forced because otherwise the compiler complains
    for (int nStaff=1; nStaff <= pVStaff->GetNumStaves(); nStaff++)
    {
        lmShape* pShape = pKey->GetShape(nStaff);
		m_oTimepos[m_nColumn].AddEntry(nInstr, pKey, pShape,
										 (pShape != pMainShape), nStaff);
    }

}


void lmFormatter4::AddTime(lmTimeSignature* pTime, lmBox* pBox, lmPaper* pPaper,
						   lmVStaff* pVStaff, int nInstr)
{
    // This method is responsible for creating the time signature shapes for 
    // all staves of this instrument. And also, of adding them to the graphical 
    // model and to the Timepos table

    //create the shapes
    pTime->Layout(pBox, pPaper);

	//add the shapes to the timepos table
	lmShape* pMainShape = ((lmStaffObj*)pTime)->GetShape();          //cast forced because otherwise the compiler complains
    for (int nStaff=1; nStaff <= pVStaff->GetNumStaves(); nStaff++)
    {
        lmShape* pShape = pTime->GetShape(nStaff);
		m_oTimepos[m_nColumn].AddEntry(nInstr, pTime, pShape,
										 (pShape != pMainShape), nStaff);
    }

}


