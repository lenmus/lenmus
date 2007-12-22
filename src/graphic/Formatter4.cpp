//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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

// class lmFormatter4
//      A rendering algorithm based on containers and a table of times and positions


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
#endif

#include "wx/debug.h"
#include "wx/list.h"
#include "wx/numdlg.h"      // for ::wxGetNumberFromUser

#include "../score/Score.h"
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



lmFormatter4::lmFormatter4()
{
    // set debugging options
    m_fDebugMode = g_pLogger->IsAllowedTraceMask(_T("Formater4"));
    m_nTraceMeasure = 0;
    if (m_fDebugMode) {
        m_nTraceMeasure = ::wxGetNumberFromUser(
                                _T("Specify the measure to trace (0 for all measures)"),
                                _T("Measure: "),
                                _T("Debug Formatter4"),
                                0L);        // default value: all measures
    }

}

lmFormatter4::~lmFormatter4()
{
    //clear Timepos tables
    for (int i=1; i <= MAX_MEASURES_PER_SYSTEM; i++) {
        m_oTimepos[i].CleanTable();
    }
}

lmBoxScore* lmFormatter4::Layout(lmScore* pScore, lmPaper* pPaper)
{
    m_pScore = pScore;
    switch (pScore->GetRenderizationType())
    {
        case eRenderJustified:
        case eRenderSimple:
            return RenderJustified(pPaper);
        default:
            wxASSERT(false);
            return (lmBoxScore*)NULL;
    }
}

lmBoxScore* lmFormatter4::RenderJustified(lmPaper* pPaper)
{

    bool fJustified = true;     // right justify measures

    //---------------------------------------------------------------------------------------
    //Render a score justifying measures so that they fit exactly in the width of the staff
    //---------------------------------------------------------------------------------------

    int nTotalMeasures;
    int iIni;
    lmLUnits xStartOfMeasure;

    //verify that there is a score
    if (!m_pScore || m_pScore->GetNumInstruments() == 0) return (lmBoxScore*)NULL;

    // get options for renderization
    bool fStopStaffLinesAtFinalBarline = m_pScore->GetOptionBool(_T("StaffLines.StopAtFinalBarline"));
    m_rSpacingFactor = m_pScore->GetOptionDouble(_T("Render.SpacingFactor"));
    m_nSpacingMethod = (lmESpacingMethod) m_pScore->GetOptionLong(_T("Render.SpacingMethod"));
    m_nSpacingValue = (lmTenths) m_pScore->GetOptionLong(_T("Render.SpacingValue"));

    lmBoxScore* pBoxScore = new lmBoxScore(m_pScore);
    pPaper->RestartPageCursors();    //ensure that page cursors are at top-left corner

    //for each staff size, setup fonts of right point size for that staff size
    int iVStaff;
    lmInstrument *pInstr;
    lmVStaff *pVStaff;
    for (pInstr = m_pScore->GetFirstInstrument(); pInstr; pInstr=m_pScore->GetNextInstrument())
    {
        for (iVStaff=1; iVStaff <= pInstr->GetNumStaves(); iVStaff++) {
            pVStaff = pInstr->GetVStaff(iVStaff);
            pVStaff->SetUpFonts(pPaper);
        }
    }

    // write score titles. Titles will be attached to current page (the fisr one)
    m_pScore->LayoutTitles(pBoxScore->GetCurrentPage(), pPaper);
    pPaper->RestartPageCursors();                                //restore page cursors are at top-left corner
    pPaper->IncrementCursorY(m_pScore->TopSystemDistance());    //advance to skip headers

    int nSystem;        //number of system in process
    int nAbsMeasure;    //number of bar in process (absolute, counted from the start of the score)
    int nRelMeasure;        //number of bar in process (relative, counted from the start of current system)
    lmLUnits ySystemPos;    //paper y position at which current system starts
    

    // The algorithm has two independent loops:
	//
    //	1. Layout phase
    //		In the first one the number of measures that fits in the system is finded out,
	//		as well as their size and the position of all their StaffObjs.
    //		The number of measures per system is stored in .... , the shapes are created 
	//		and the positioning information is stored in the shapes.
	//
    //	2. Reder phase
    //		In the second loop the StaffObjs are just rendered using the information compiled
    //		in first loop.
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
    iIni = 1;                //iIni = Measure in which the system starts
    nAbsMeasure = 1;
    nTotalMeasures = ((m_pScore->GetFirstInstrument())->GetVStaff(1))->GetNumMeasures();    //num measures in the score
        //! @limit It is assumed that all staves have the same number of measures
    nSystem = 1;
    lmBoxPage* pBoxPage = pBoxScore->GetCurrentPage();
    lmBoxSystem* pBoxSystem;

    lmLUnits nSystemHeight;

    while (nAbsMeasure <= nTotalMeasures)
    {
        m_nMeasuresInSystem = 0;

        //set up tables for storing StaffObjs positioning information
        for (int i=1; i <= MAX_MEASURES_PER_SYSTEM; i++) {
            m_oTimepos[i].CleanTable();
        }

        //-------------------------------------------------------------------------------
        //Step 1: Form and size a measure column.
        //-------------------------------------------------------------------------------
        //inner loop: each loop cycle corresponds to processing a measure column
        //The measure column is sized and this space discunted from available line space.
        //The loop is exited when there is not enough space for including in this system
        //the measure column just computed or when a newSystem tag is found
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
            if (yNew > pPaper->GetMaximumY() ) {
                //wxLogMessage(_T("Page break needed. yCur=%.2f, yNew=%.2f, MaximumY=%.2f"), pPaper->GetCursorY(), yNew, pPaper->GetMaximumY());
                pPaper->RestartPageCursors();       //restore page cursors are at top-left corner
                //start a new page
                pBoxPage = pBoxScore->AddPage();
            }
        }

        //create the system container
        pBoxSystem = pBoxPage->AddSystem(nSystem);
        ySystemPos = pPaper->GetCursorY();  //save the start of system position
        pBoxSystem->SetPosition(pPaper->GetCursorX(), ySystemPos);
        pBoxSystem->SetFirstMeasure(nAbsMeasure);

        bool fNewSystem = false;
        nRelMeasure = 1;    // the first measure in current system
        while (nAbsMeasure <= nTotalMeasures)
        {
            //reposition paper vertically at the start of the system. It has been advanced
            //when sizing the previous measure column
            pPaper->SetCursorY( ySystemPos );

			//if start of system, set initial spacing and size
            if (nRelMeasure == 1)
			{
				pBoxSystem->SetIndent(((nSystem == 1) ? nFirstSystemIndent : nOtherSystemIndent ));
				pPaper->IncrementCursorX(
					((nSystem == 1) ? nFirstSystemIndent : nOtherSystemIndent ));
				pBoxSystem->UpdateXRight( pPaper->GetRightMarginXPos() );
            }

            //size this measure column create BoxSlice (and BoxSlice hierarchy) for 
            //the measure being processed
            m_uMeasureSize[nRelMeasure] =
                SizeMeasureColumn(nAbsMeasure, nRelMeasure, nSystem, pBoxSystem, pPaper, &fNewSystem);

            #if defined(__WXDEBUG__)
            g_pLogger->LogTrace(_T("Formatter4.Step1"),
                _T("m_uMeasureSize[%d] = %.2f"), nRelMeasure, m_uMeasureSize[nRelMeasure] );
            #endif

            //if this is the first measure column compute the space available in
            //this system. The method is a little tricky. The total space available
            //is (pPaper->GetPageRightMargin() - pPaper->GetCursorX()). But we have
            //to take into account the space that will be used by the prolog. As the
            //left position of the first measure column has taken all this into account,
            //it is posible to use that value by just doing:
            if (nRelMeasure == 1) {
                m_uFreeSpace =
                    pPaper->GetRightMarginXPos() - m_oTimepos[nRelMeasure].GetStartOfBarPosition();
            }

            #if defined(__WXDEBUG__)
            g_pLogger->LogTrace(_T("Formatter4.Step1"),
                _T("RelMeasure=%d, m_uFreeSpace = %.2f, PaperRightMarginXPos=%.2f, StartOfBar=%.2f"),
                nRelMeasure, m_uFreeSpace, pPaper->GetRightMarginXPos(), 
                m_oTimepos[nRelMeasure].GetStartOfBarPosition() );
            g_pLogger->LogTrace(_T("Formatter4.Step1"),
                m_oTimepos[nRelMeasure].DumpTimeposTable());
            #endif

            //substract space ocupied by this measure from space available in the system
            if (m_uFreeSpace < m_uMeasureSize[nRelMeasure])
			{
                //there is no enough space for this measure column.

				//for this measure ScoreObjs, remove 'position computed' mark
                ResetLocation(nAbsMeasure);

                //exit the loop. The system is finished
                break;
            } else {
                //there is enough space for this measure column. Add it to current system and
                //discount the space that the measure will take
                m_uFreeSpace -= m_uMeasureSize[nRelMeasure];
                m_nMeasuresInSystem++;
            }

            //if newSystem tag found force to finish current system
            if (fNewSystem) break;

            //advance to next bar
            nAbsMeasure++;
            nRelMeasure++;

        }    //end of loop to process a measure column

        //helper flag to signal if current system is the last one.
        bool fThisIsLastSystem =
                (iIni + m_nMeasuresInSystem > nTotalMeasures);

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
        //   m_nMeasuresInSystem  - the number of measures that fit in this system
        //
        //Now we proceed to re-distribute the remaining free space across all measures, so that
        //the system is justified. This step only computes the new measure column sizes and stores
        //them in table m_uMeasureSize[1..MaxBar] but changes nothing in the StaffObjs
        //-------------------------------------------------------------------------------
        if (m_nMeasuresInSystem == 0)
        {
            //The line width is not enough for drawing just one bar!!!
            //We have to split the measure
            if (SplitMeasureColumn())
            {
                //it has been requested to abort layout. Return an empty score
			    if (pBoxScore) delete pBoxScore;
                pBoxScore = new lmBoxScore(m_pScore);
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
            for (int i = 1; i <= m_nMeasuresInSystem; i++) {
                wxLogMessage(wxString::Format(
                    _T("Bar column %d. Size = %.2f"), i, m_uMeasureSize[i]));
            }
        }
        //dbg ---------------

        if (fJustified) {
            //if requested to justify systems, divide up the remaining space
            //between all bars, except if this is the last bar and options flag
            //"StopStaffLinesAtFinalBar" is set.
            if (!(fThisIsLastSystem && fStopStaffLinesAtFinalBarline))
                RedistributeFreeSpace(m_uFreeSpace);
        }

        //dbg --------------
        if (m_fDebugMode) {
            wxLogMessage(_T("After distributing free space"));
            wxLogMessage(_T("***************************************\n"));
            for (int i = 1; i <= m_nMeasuresInSystem; i++) {
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
        for (int i=1; i <= m_nMeasuresInSystem; i++) {
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
        pBoxSystem->SetNumMeasures(m_nMeasuresInSystem, m_pScore);
        if (fThisIsLastSystem && fStopStaffLinesAtFinalBarline) {
            //this is the last system and it has been requested to stop staff lines
            //in last measure. So, set final x so staff lines go to final bar line
            lmLUnits xPos;
            if (pVStaff->GetXPosFinalBarline(&xPos))
                pBoxSystem->UpdateXRight( xPos - 1 );
            else
                pBoxSystem->UpdateXRight( pPaper->GetRightMarginXPos() );
        }
        else {
            //staff lines go to the rigth margin
            pBoxSystem->UpdateXRight( pPaper->GetRightMarginXPos() );
        }

	    //Add the shape for the initial barline that joins all staves in a system
	    if (m_pScore->GetOptionBool(_T("Staff.DrawLeftBarline")) && !pVStaff->HideStaffLines() )
	    {
			//GetSliceVStaff(int i)
			lmLUnits uxPos = pBoxSystem->GetXLeft() + 
						     (nSystem == 1 ? nFirstSystemIndent : nOtherSystemIndent);
		    lmLUnits uLineThickness = lmToLogicalUnits(0.2, lmMILLIMETERS);        // thin line width will be 0.2 mm @todo user options
            lmShapeLine* pLine = 
                new lmShapeLine(pVStaff, uxPos, pBoxSystem->GetYTopFirstStaff(),
						    uxPos, pBoxSystem->GetYBottom(),
						    uLineThickness, 0.0, *wxBLACK, _T("System joining line"),
							eEdgeHorizontal);
	        pBoxSystem->AddShape(pLine);
	    }

        //update lmBoxSlices with the final measures sizes
		lmLUnits xEnd = m_oTimepos[1].GetStartOfBarPosition();
        for (int iRel=1; iRel <= m_nMeasuresInSystem; iRel++)
        {
            lmLUnits xStart = xEnd;
            xEnd = xStart + m_uMeasureSize[iRel];
            lmBoxSlice* pBoxSlice = pBoxSystem->GetSlice(iRel);
			pBoxSlice->UpdateXLeft(xStart);
			pBoxSlice->UpdateXRight(xEnd);
        }

        // compute system height
        if (nSystem == 1) {
            nSystemHeight = pPaper->GetCursorY() - ySystemPos;
            //wxLogMessage(_T("[lmFormatter4::RenderJustified] nSystemHeight = %.2f"),
            //    nSystemHeight );
        }


        //increment loop information
        iIni += m_nMeasuresInSystem;
        nAbsMeasure = iIni;
        nSystem++;

    }    //while (nAbsMeasure <= nTotalMeasures)

    //Here the score is prepared. Last staff lines are finished at the right margin or
    //at the final bar, depending on flag fStopStaffLinesAtFinalBarline.
    //If this flag is false and option 'Score.FillPageWithEmptyStaves' is true it means
    //that the user has requested to fill the remaining page space with empty staves.
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
                if (yNew > pPaper->GetMaximumY() )
                    break;        //exit loop
            }

            //create the system container
            pBoxSystem = pBoxPage->AddSystem(nSystem);
            ySystemPos = pPaper->GetCursorY();  //save the start of system position
            pBoxSystem->SetPosition(pPaper->GetCursorX(), ySystemPos);
            pBoxSystem->SetFirstMeasure(nAbsMeasure);
            pBoxSystem->SetIndent(((nSystem == 1) ? nFirstSystemIndent : nOtherSystemIndent ));

            nRelMeasure = 1;    // the first measure in current system
            AddEmptyMeasureColumn(nAbsMeasure, nRelMeasure, nSystem, pBoxSystem, pPaper);

            //Store information about this system
            pBoxSystem->SetNumMeasures(0, m_pScore);
            //staff lines go to the rigth margin
            pBoxSystem->UpdateXRight( pPaper->GetRightMarginXPos() );

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
    int iVStaff;
    lmInstrument *pInstr;
    lmVStaff *pVStaff;

    // save paper position to restore it at the end
    lmLUnits xPaperStart = pPaper->GetCursorX();
    lmLUnits yPaperStart = pPaper->GetCursorY();

    // explore all instruments in the score
    lmLUnits yPaperPos;
    for (pInstr = m_pScore->GetFirstInstrument(); pInstr; pInstr=m_pScore->GetNextInstrument())
    {
        //loop for current instrument, explore all its staves
        for (iVStaff=1; iVStaff <= pInstr->GetNumStaves(); iVStaff++)
        {
            pVStaff = pInstr->GetVStaff(iVStaff);

            //if it is not first VStaff, set paper position for this VStaff
            if (iVStaff != 1) {
                if (pVStaff->IsOverlayered()) {
                    //overlayered: restore paper position to previous VStaff position
                    pPaper->SetCursorY( yPaperPos );
                }
            }

            //save this VStaff paper position
            yPaperPos = pPaper->GetCursorY();

            //advance paper in height off this lmVStaff
            pVStaff->NewLine(pPaper);
            //TODO add inter-staff space

        }    // next lmVStaff
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
    //Variable  m_nMeasuresInSystem must be set to one.

    //dbg --------------
    wxLogMessage(_T("[lmFormatter4::SplitMeasureColumn]. Dump of relevant info"));
    wxLogMessage(_T("*********************************************************\n"));
    wxLogMessage(wxString::Format(_T("Measure width= %.2f, free space=%.2f"),
                 m_uMeasureSize[1], m_uFreeSpace));
    m_oTimepos[1].DumpTimeposTable();
    //dbg ---------------

    //TODO
    wxLogMessage(_T("[lmFormatter4::RenderJustified] The line width is not enough for drawing just one bar!!!."));
    return true;        //abort rederization

}

lmLUnits lmFormatter4::SizeMeasureColumn(int nAbsMeasure, int nRelMeasure, int nSystem,
                                         lmBoxSystem* pBoxSystem, lmPaper* pPaper, bool* pNewSystem)
{
    // For each instrument and staff it is computed the size of this measure.
    // Also the hierarchy of BoxSlice objects is created for this measure.
    //
    // All measurements are stored in the global object m_oTimepos[nRelMeasure], so that other
    // procedures can take decisions about the final number of measures to include in a system
    // and for repositioning the StaffObjs.
    //
    // Input parameters:
    //   nAbsMeasure - Measure number (absolute) to size
    //   nRelMeasure - Measure number (relative) to size
    //   nSystem - System number
    //
    // Returns:
    //   - The size of this measure column.
    //   - positioning information for this measure column is stored in m_oTimepos[nRelMeasure]
    //   - Updates flag pointed by pNewSystem and sets it to true if newSystem tag found
    //        in this measure
    //

    int iVStaff;
    lmInstrument* pInstr;
    lmVStaff* pVStaff;
    bool fNewSystem = false;

    lmLUnits xPaperPos, yPaperPos;
    lmLUnits xStartPos = pPaper->GetCursorX();      //save x pos to align staves in system

    // create an empty BoxSlice to contain this measure column
    lmBoxSlice* pBoxSlice = pBoxSystem->AddSlice(nAbsMeasure);
	bool fUpdateSlice = true;

	//initial conditions
	bool fFirstStaffInSystem = true;

    // explore all instruments in the score
	lmLUnits yBottomLeft = 0;
    for (pInstr = m_pScore->GetFirstInstrument(); pInstr; pInstr=m_pScore->GetNextInstrument())
    {
        //verify that program limits are observed
        if (pInstr->GetNumStaves() > MAX_STAVES_PER_SYSTEM) {
            //TODO log and display message properly
            wxLogMessage(
                _("[lmFormatter4::RenderJustified]: Program limitation: Maximum number of staves per system has been exceeded."));
            wxASSERT(false);
        }

        pPaper->SetCursorX( xStartPos );    //align start of staves in this system

        // create an empty BoxSliceInstr to contain this instrument slice
        lmBoxSliceInstr* pBSI = pBoxSlice->AddInstrument(pInstr);
		bool fUpdateInstr = true;

        //loop. For current instrument, explore all its staves to size the measure
        //column nAbsMeasure. All collected information is stored in m_oTimepos[nRelMeasure]
        for (iVStaff=1; iVStaff <= pInstr->GetNumStaves(); iVStaff++)
        {
            pVStaff = pInstr->GetVStaff(iVStaff);

            //if it is not first VStaff, set paper position for this VStaff
            if (iVStaff != 1) {
                if (pVStaff->IsOverlayered()) {
                    //overlayered: restore paper position to previous VStaff position
                    pPaper->SetCursorX( xPaperPos );
                    pPaper->SetCursorY( yPaperPos );
                }
            }

            //save this VStaff paper position
            xPaperPos = pPaper->GetCursorX();
            yPaperPos = pPaper->GetCursorY();

            // create the BoxSliceVStaff
            lmBoxSliceVStaff* pBSV = pBSI->AddVStaff(pVStaff);
            // if first measure in system add the ShapeStaff
			if (nRelMeasure == 1)
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
            if (nSystem != 1 && nRelMeasure == 1)
			{
				pPaper->SetCursorX(xPaperPos);
				pVStaff->AddPrologShapes(pBSV, nAbsMeasure, (nSystem == 1), pPaper);
            }

            fNewSystem |= SizeMeasure(pBSV, pVStaff, nAbsMeasure, nRelMeasure, pPaper);

            //advance paper in height off this lmVStaff
            //AWARE As advancing one staff has the effect of returning
            //x position to the left marging, all x position information stored
            //in m_timepos is relative to the start of the measure
            pVStaff->NewLine(pPaper);
            //TODO add inter-staff space

            ///*** Update measures of this BoxVStaffSlice

        }    // next lmVStaff

        ///*** Update measures of this BoxInstrSlice
		pBSI->SetYBottom(yBottomLeft);
		pBoxSlice->SetYBottom(yBottomLeft);

        // if first measure in system add instrument names and brackets/bracets
		if (nRelMeasure == 1)
		{
			if (nSystem == 1)
				pInstr->AddNameShape(pBSI, pPaper);
			else
				pInstr->AddAbbreviationShape(pBSI, pPaper);
		}

    }    // next lmInstrument

    //all measures in measure column number nAbsMeasure have been sized. The information is stored in
    //object m_oTimepos[nRelMeasure]. Now proced to re-position the StaffObjs so that all StaffObjs
    //sounding at the same time will have the same x coordinate. The method .ArrageStaffobjsByTime
    //returns the measure column size
    *pNewSystem = fNewSystem;
    bool fTrace =  m_fDebugMode && (m_nTraceMeasure == 0 || m_nTraceMeasure == nAbsMeasure);
    return m_oTimepos[nRelMeasure].ArrangeStaffobjsByTime(fTrace);      //true = debug on

}


void lmFormatter4::AddEmptyMeasureColumn(int nAbsMeasure, int nRelMeasure, int nSystem,
                                         lmBoxSystem* pBoxSystem, lmPaper* pPaper)
{

    int iVStaff;
    lmInstrument* pInstr;
    lmVStaff* pVStaff;
    bool fNewSystem = false;

    lmLUnits xPaperPos, yPaperPos;
    lmLUnits xStartPos = pPaper->GetCursorX();      //save x pos to align staves in system

    // create an empty BoxSlice to contain this measure column
    lmBoxSlice* pBoxSlice = pBoxSystem->AddSlice(nAbsMeasure);
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

        //loop. For current instrument, explore all its staves to size the measure
        //column nAbsMeasure. All collected information is stored in m_oTimepos[nRelMeasure]
        for (iVStaff=1; iVStaff <= pInstr->GetNumStaves(); iVStaff++)
        {
            pVStaff = pInstr->GetVStaff(iVStaff);

            //if it is not first VStaff, set paper position for this VStaff
            if (iVStaff != 1) {
                if (pVStaff->IsOverlayered()) {
                    //overlayered: restore paper position to previous VStaff position
                    pPaper->SetCursorX( xPaperPos );
                    pPaper->SetCursorY( yPaperPos );
                }
            }

            //save this VStaff paper position
            xPaperPos = pPaper->GetCursorX();
            yPaperPos = pPaper->GetCursorY();

            // create the BoxSliceVStaff
            lmBoxSliceVStaff* pBSV = pBSI->AddVStaff(pVStaff);
            // if first measure in system add the ShapeStaff
			if (nRelMeasure == 1)
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
            if (nSystem != 1 && nRelMeasure == 1)
			{
				pPaper->SetCursorX(xPaperPos);
				pVStaff->AddPrologShapes(pBSV, nAbsMeasure, (nSystem == 1), pPaper);
            }

            //fNewSystem |= SizeMeasure(pBSV, pVStaff, nAbsMeasure, nRelMeasure, pPaper);

            //advance paper in height off this lmVStaff
            //AWARE As advancing one staff has the effect of returning
            //x position to the left marging, all x position information stored
            //in m_timepos is relative to the start of the measure
            pVStaff->NewLine(pPaper);
            //TODO add inter-staff space

            ///*** Update measures of this BoxVStaffSlice

        }    // next lmVStaff

        ///*** Update measures of this BoxInstrSlice
		pBSI->SetYBottom(yBottomLeft);
		pBoxSlice->SetYBottom(yBottomLeft);

        // if first measure in system add instrument names and brackets/bracets
		if (nRelMeasure == 1)
		{
			if (nSystem == 1)
				pInstr->AddNameShape(pBSI, pPaper);
			else
				pInstr->AddAbbreviationShape(pBSI, pPaper);
		}

    }    // next lmInstrument

}

void lmFormatter4::ResetLocation(int nAbsMeasure)
{
    // explore all instruments in the score
    lmInstrument* pInstr;
    for (pInstr = m_pScore->GetFirstInstrument(); pInstr; pInstr=m_pScore->GetNextInstrument())
    {
        //loop. For current instrument, explore all its staves
        for (int iVStaff=1; iVStaff <= pInstr->GetNumStaves(); iVStaff++)
        {
            lmVStaff* pVStaff = pInstr->GetVStaff(iVStaff);

			//loop to process all StaffObjs in this measure
			lmStaffObjIterator* pIT = pVStaff->CreateIterator(eTR_AsStored);
			pIT->AdvanceToMeasure(nAbsMeasure);
			while(!pIT->EndOfList())
			{
				lmStaffObj* pSO = pIT->GetCurrent();
				pSO->ResetObjectLocation();
				if (pSO->GetType() == eSFOT_Barline) break;	//End of measure: exit loop.
				pIT->MoveNext();
			}
			delete pIT;
		}
    }
}


void lmFormatter4::RedistributeFreeSpace(lmLUnits nAvailable)
{
    //Step 3: Justify measures (distribute remainnig space across all measures)
    //-------------------------------------------------------------------------------
    //In summary, the algoritm computes the average size of all measure columns and
    //increases the size of all measure columns that are narrower than the average size,
    //so that their size is grown to the average size. The process of computing the new
    //average and enlarging the narrower than average measures is repeated until all
    //space is distributed.
    //
    //on entering in this function:
    // - the table m_uMeasureSize() stores the minimum size for each measure column for the
    //   current system.
    // - nAvailable stores the free space remaining at the end of this system
    //
    //on exit:
    // - the values stored in table m_uMeasureSize() are modified to reflect the new size
    //   for the bar columns, so that the line get justified.

    //-------------------------------------------------------------------------------------

    if (nAvailable <= 0) return;       //no space to distribute

    lmLUnits nDif[MAX_MEASURES_PER_SYSTEM];

    //compute average measure column size
    lmLUnits nAverage = 0;
    for (int i = 1; i <= m_nMeasuresInSystem; i++) {
        nAverage += m_uMeasureSize[i];
    }
    nAverage /= m_nMeasuresInSystem;

    lmLUnits nMeanPrev = 0;
    lmLUnits nDifTotal = 0;
    while (nAvailable > 0 && nAverage != nMeanPrev) {
        //for each measure column, compute the diference between its size and the average size
        //sum up all the diferences in nDifTotal
        nDifTotal = 0;
        for (int i = 1; i <= m_nMeasuresInSystem; i++) {
            nDif[i] = nAverage - m_uMeasureSize[i];
            if (nDif[i] > 0) { nDifTotal += nDif[i]; }
        }

        //if the sum of all diferences is greater than the space to distribute
        //reduce the differences
        while (nDifTotal > nAvailable) {
            nDifTotal = 0;
            for (int i = 1; i <= m_nMeasuresInSystem; i++) {
                if (nDif[i] > 0) {
                    nDif[i]--;
                    nDifTotal += nDif[i];
                }
            }
        }

        //The size of all measure columns whose size is lower than the average
        //is going to be increased by the amount stated in the differences table
        for (int i = 1; i <= m_nMeasuresInSystem; i++) {
            if (nDif[i] > 0) { m_uMeasureSize[i] += nDif[i]; }
        }
        nAvailable -= nDifTotal;

        //compute the new measure column size average
        nMeanPrev = nAverage;
        nAverage = 0;
        for (int i = 1; i <= m_nMeasuresInSystem; i++) {
            nAverage += m_uMeasureSize[i];
        }
        nAverage /= m_nMeasuresInSystem;
    }

    //divide up the remaining space between all bars
    if (nAvailable > 0) {
        nDifTotal = nAvailable / m_nMeasuresInSystem;
        for (int i = 1; i <= m_nMeasuresInSystem; i++) {
            m_uMeasureSize[i] += nDifTotal;
            nAvailable -= nDifTotal;
        }
        m_uMeasureSize[m_nMeasuresInSystem] += nAvailable;
    }

}


//=========================================================================================
// Methods to deal with measures
//=========================================================================================

bool lmFormatter4::SizeMeasure(lmBoxSliceVStaff* pBSV, lmVStaff* pVStaff, int nAbsMeasure,
							   int nRelMeasure, lmPaper* pPaper)
{
    // Compute the width of the requested measure of the lmVStaff
    // Input variables:
    //   pVStaff - lmVStaff to process
    //   nAbsMeasure - number of measure to size (absolute, from the start of the score)
    //   nRelMeasure - number of this measure (relative, referred to current system)
    // Results:
    //   all measurements are stored in global variable  m_oTimepos[nRelMeasure]

    //   return bool: true if newSystem tag found in this measure

    wxASSERT(nAbsMeasure <= pVStaff->GetNumMeasures());

    //TODO Review this commented code. Must review also DrawMeasure
    //      I think this code is only for positioning text relative to barline
    //StaffObjs could have positioning information relative to the start of barline position.
    //Therfore, it is necessary to store start of barline position so that relative positioned
    //StaffObjs can be correctly positioned.
    //The stored value is accesible by method //GetXInicioCompas//
//    pVStaff->SetXInicioCompas = pPaper->GetCursorX()

    //start new thread
    lmLUnits xStart = pPaper->GetCursorX();
    m_oTimepos[nRelMeasure].NewThread();
    m_oTimepos[nRelMeasure].SetCurXLeft(xStart);

    //if this is not the first measure of the score advance (horizontally) a space to leave a gap
    //between the previous barline (or the prolog, if first measure in system) and the first note
    if (nAbsMeasure != 1)
    {
        // If previous barline is not visible, do not add space

        // Get the previous barline
        lmStaffObjIterator* pIT = pVStaff->CreateIterator(eTR_AsStored);
        pIT->AdvanceToMeasure(nAbsMeasure);
        pIT->BackToItemOfType(eSFOT_Barline);
        if (!pIT->EndOfList())
        {
            // a barline found
            //TODO: how do you know it is the previous barline?
            lmStaffObj* pSO = pIT->GetCurrent();
            if (pSO->IsVisible())
            {
                // the barline is visble. Add space
                //TODO: review this fixed barline after space
                lmLUnits nSpaceAfterBarline = pVStaff->TenthsToLogical(20, 1);    // two lines
                pPaper->IncrementCursorX(nSpaceAfterBarline);       //space after barline
           }

        }
        delete pIT;
    }

    lmNote* pNote = (lmNote*)NULL;
    lmNoteRest* pNoteRest = (lmNoteRest*)NULL;
    lmClef* pClef = (lmClef*)NULL;
    //lmKeySignature* pKey = (lmKeySignature*)NULL;
    bool fPreviousWasClef = false;      //the previous lmStaffObj was a clef
    lmLUnits nClefXPos=0;               //x left position of previous clef
    int nClefStaffNum=0;                //number of staff in which the previous clef was located
    lmLUnits xChordPos=0;               //position of base note of a chord

    //loop to process all StaffObjs in this measure
    bool fNoteRestFound = false;
    bool fNewSystem = false;                //newSystem tag found
    EStaffObjType nType;                    //type of score obj being processed
    lmStaffObj* pSO = (lmStaffObj*)NULL;
    lmStaffObjIterator* pIT = pVStaff->CreateIterator(eTR_AsStored);
    pIT->AdvanceToMeasure(nAbsMeasure);
    while(!pIT->EndOfList())
    {
        pSO = pIT->GetCurrent();
        nType = pSO->GetClass();

        if (nType == eSFOT_Barline) break;         //End of measure: exit loop.

        if (nType == eSFOT_Control)
        {
            lmSOControl* pSOCtrol = (lmSOControl*)pSO;
            ESOCtrolType nCtrolType = pSOCtrol->GetCtrolType();
            if (lmTIME_SHIFT == nCtrolType)
            {
                //start a new thread, setting x pos to the same x pos than the
                //previous thread
                m_oTimepos[nRelMeasure].NewThread();
                pPaper->SetCursorX(m_oTimepos[nRelMeasure].GetCurXLeft());
            }
            else if(lmNEW_SYSTEM == nCtrolType)
            {
                //new system tag found in this measure
                fNewSystem = true;
            }
            else    //layout it
            pSO->Layout(pBSV, pPaper);
        }
        else {
            //collect data about the lmStaffObj
            lmTimeposTable& oTimepos = m_oTimepos[nRelMeasure];
            if (nType == eSFOT_NoteRest) {
                oTimepos.AddEntry(pSO->GetTimePos(), pSO);
            } else {
                oTimepos.AddEntry (-1, pSO);
            }

            //if this lmStaffObj is a lmNoteRest that is part of a chord its
            //anchor x position must be the same than that of the base note
            if (nType == eSFOT_NoteRest) {
                fNoteRestFound = true;
                fPreviousWasClef = false;            //this lmStaffObj is not a clef
                pNoteRest = (lmNoteRest*)pSO;
                pNote = (lmNote*)pSO;        //AWARE we do not know yet if it is a note or a rest,
                                        //but I force the casting to simplify next if statement
                if (!pNoteRest->IsRest() && pNote->IsInChord())
                {
                    //this note is part of a chord
                    if (pNote->IsBaseOfChord()) {
                        //it is the base note. Save x position. All other notes in the
                        // chord must be assigned the same x position
                        xChordPos = pPaper->GetCursorX();    //save this position
                        oTimepos.SetCurXLeft(xChordPos);    //and store it as the x position for this lmStaffObj
                    } else {
                        //All other notes in the chord must have the same x position
                        //than the base note.
                        pPaper->SetCursorX(xChordPos);        //Restore paper to note base x postion
                        oTimepos.SetCurXLeft(xChordPos);    //and store this as the x position for this lmStaffObj
                    }
                } else
                {
                    //Is a rest or is a lmNote not in chord. Store its x position
                    oTimepos.SetCurXLeft(pPaper->GetCursorX());
                }

            } else {
                //it is not a lmNoteRest.

                //if it is a clef or a key hide/unhide it in prologs
                bool fHide = (nAbsMeasure != 1 && nRelMeasure == 1 && !fNoteRestFound);
                if (nType == eSFOT_Clef) {
                    ((lmClef*)pSO)->Hide(fHide);
                }
                if (nType == eSFOT_KeySignature) {
                    ((lmKeySignature*)pSO)->Hide(fHide);
                }

                //if this is a key on the first measure of a system, add space before clef
                if (nType == eSFOT_Clef && nRelMeasure == 1) {
                    pPaper->IncrementCursorX( pVStaff->GetSpaceBeforeClef() );
                }

                //Store current x position for this lmStaffObj.
                if (pSO->GetClass() == eSFOT_Clef) {
                    //if previous lmStaffObj was also a cleft and this new is in a
                    //different staff than the previous one, the left position of this new
                    //cleft must be the same than that of the previous clef so that
                    //both clefs are aligned.
                    pClef = (lmClef*)pSO;
                    int nStaff = pClef->GetStaffNum();
                     if (fPreviousWasClef && (nStaff != nClefStaffNum)) {
                        pPaper->SetCursorX(nClefXPos);  //paper back to aling both clefs
                    }
                }

                oTimepos.SetCurXLeft(pPaper->GetCursorX());

                //if it is a clef save xLeft position just in case the next
                //lmStaffObj is also the clef for other staff
                fPreviousWasClef = (pSO->GetClass() == eSFOT_Clef);
                if (fPreviousWasClef) {
                    pClef = (lmClef*)pSO;
                    nClefXPos = oTimepos.GetCurXLeft();
                    nClefStaffNum = pClef->GetStaffNum();
                }
            }

            //layout this lmStaffObj and store its final and anchor x positions
            pSO->Layout(pBSV, pPaper);
                //store its final and anchor x positions
            oTimepos.SetCurXFinal(pPaper->GetCursorX());
            oTimepos.SetCurXAnchor(pSO->GetAnchorPos());
            //oTimepos.SetCurXAnchor(oTimepos.GetCurXLeft() + pSO->GetAnchorPos());
            //wxLogMessage(_T("[lmFormatter4::SizeMeasure] anchor pos = %.2f, ID=%d"), pSO->GetAnchorPos(), pSO->GetID() ); 
            // add after space
            if (pSO->GetClass() == eSFOT_NoteRest) {
                lmTenths rSpace;
                if (m_nSpacingMethod == esm_PropConstantFixed) {
                    //proportional constant spacing.
                    pNoteRest = (lmNoteRest*)pSO;
                    rSpace = pNoteRest->GetDuration()* m_rSpacingFactor;
                }
                else if (m_nSpacingMethod == esm_Fixed) {
                    // fixed spacing
                    rSpace = m_nSpacingValue;
                }
                else
                    wxASSERT(false);

                lmLUnits uSpace = pVStaff->TenthsToLogical(rSpace, pSO->GetStaffNum());
                pPaper->IncrementCursorX( uSpace );
            }

        }

        pIT->MoveNext();
    }
    delete pIT;

    //The barline lmStaffObj is not included in the loop as it might not exist in the last
    //bar of a score. In theses cases, the loop is exited because the end of the score is
    //reached.

    //if the barline exists and is visible we have to advance paper x cursor
    //in barline width and store it in the Omega entry
    if (pSO->GetClass() == eSFOT_Barline)
    {
        //reposition paper if user location for barline is defined
        lmBarline* pBarline = (lmBarline*)pSO;
        if (pBarline->GetLocationType() != lmLOCATION_DEFAULT)
        {
            lmLUnits xEnd = pPaper->GetCursorX();
            //wxLogMessage(_T("[lmFormatter4::SizeMeasure] xStart=%.2f, xEnd=%.2f"), xStart, xEnd);
            lmLUnits xFinalPos = 0;
            lmLUnits xPos = (pBarline->GetOrigin()).x;  //pBarline->GetLocationPos();

            //compute user required barline position
            if (pBarline->GetLocationType() == lmLOCATION_USER_RELATIVE) {
                xFinalPos = xStart + xPos;
            }
            else {  //lmLOCATION_USER_ABSOLUTE
                xFinalPos = xPos + pPaper->GetLeftMarginXPos();
            }
            xEnd = wxMax(xEnd, xFinalPos);
            //wxLogMessage(_T("[lmFormatter4::SizeMeasure] xFinalPos=%.2f, new xEnd=%.2f"),
            //    xFinalPos, xEnd);
            pPaper->SetCursorX(xEnd);
        }

        // end up current thread
        m_oTimepos[nRelMeasure].CloseThread(pPaper->GetCursorX());

        //Now add the barline
        m_oTimepos[nRelMeasure].AddBarline(pSO);
        pSO->Layout(pBSV, pPaper);
    }
    else {
        // no barline at the end of the measure. Close thread
        m_oTimepos[nRelMeasure].CloseThread(pPaper->GetCursorX());
    }

    //now store final x position of this measure
    m_oTimepos[nRelMeasure].SetCurXFinal(pPaper->GetCursorX());

    return fNewSystem;
}

