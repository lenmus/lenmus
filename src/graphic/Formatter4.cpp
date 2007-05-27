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
#include "../score/Score.h"
#include "TimeposTable.h"
#include "Formatter4.h"
#include "BoxSystem.h"


lmFormatter4::lmFormatter4()
{
    m_fDebugMode = false;
}

lmFormatter4::~lmFormatter4()
{
    //wxLogMessage(_T("[lmFormatter4::~lmFormatter4] Deleting lmFormatter4 object"));

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
            return RenderJustified(pPaper);
        case eRenderSimple:
            return RenderMinimal(pPaper);
        default:
            wxASSERT(false);
            return (lmBoxScore*)NULL;
    }

}

lmBoxScore* lmFormatter4::RenderMinimal(lmPaper* pPaper)
{
    //drawing a score without bar justification and without breaking it into systems.
    //That is, it draws all the score in a single system without taking into consideration
    //paper length limitations.
    // This very simple renderer is usefull for simple scores and in some rare occations

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

    // write score titles
    m_pScore->WriteTitles(DO_MEASURE, pPaper);
    pPaper->RestartPageCursors();                                //restore page cursors are at top-left corner
    pPaper->IncrementCursorY(m_pScore->TopSystemDistance());    //advance to skip headers

    //prepare the only page
    lmBoxPage* pBoxPage = pBoxScore->GetCurrentPage();
    lmBoxSystem* pBoxSystem;

    //for each instrument
    lmLUnits nSpaceAfterBarline;
    int nAbsMeasure;
    for (pInstr = m_pScore->GetFirstInstrument(); pInstr; pInstr=m_pScore->GetNextInstrument())
    {
        //for each lmVStaff
        for (iVStaff=1; iVStaff <= pInstr->GetNumStaves(); iVStaff++)
        {
            pVStaff = pInstr->GetVStaff(iVStaff);
            nSpaceAfterBarline = pVStaff->TenthsToLogical(20, 1);

            pBoxSystem = pBoxPage->AddSystem(iVStaff);
            int ySystemPos = pPaper->GetCursorY();      //save the start of system position
            pBoxSystem->SetPosition(pPaper->GetCursorX(), ySystemPos);
            nAbsMeasure = 1;
            pBoxSystem->SetFirstMeasure(nAbsMeasure);

            //loop to process all StaffObjs in this VStaff
            lmStaffObj* pSO = (lmStaffObj*)NULL;
            lmStaffObjIterator* pIT = pVStaff->CreateIterator(eTR_AsStored);
            pIT->MoveFirst();
            while(!pIT->EndOfList())
            {
                pSO = pIT->GetCurrent();
                pSO->Draw(DO_MEASURE, pPaper);  //measure the staffobj

                if (pSO->GetClass() == eSFOT_Barline) {
                    if (pSO->IsVisible()) {
                        //add space after barline
                        pPaper->IncrementCursorX(nSpaceAfterBarline);
                    }
                    nAbsMeasure++;
                }

                pIT->MoveNext();
            }
            delete pIT;

            pBoxSystem->SetNumMeasures(--nAbsMeasure);
            pBoxSystem->SetFinalX( pPaper->GetCursorX() + nSpaceAfterBarline );
            pBoxSystem->SetIndent(0);

        }
    }

    return pBoxScore;

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

    // write score titles
    m_pScore->WriteTitles(DO_MEASURE, pPaper);
    pPaper->RestartPageCursors();                                //restore page cursors are at top-left corner
    pPaper->IncrementCursorY(m_pScore->TopSystemDistance());    //advance to skip headers

    int nSystem;        //number of system in process
    int nAbsMeasure;    //number of bar in process (absolute, counted from the start of the score)
    int nRelMeasure;        //number of bar in process (relative, counted from the start of current system)
    lmLUnits ySystemPos;    //paper y position at which current system starts
    /*
    The algorithm has two independent loops:

        1. Phase DO_MEASURE
            In the first one the number of measures that fits in the system is calculated, as
            well as their size and the position of all their StaffObjs.
            The number of measures per system is stored in .... and the positioning information is stored
            in each lmStaffObj.

        2. Phase DO_DRAW
            In the second loop the StaffObjs are just rendered using the information compiled
            in first loop.
    */

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
        if (nSystem != 1) {
            //Here Paper is positioned at the start of the new current system. No
            //computation for this is needed as staves' height and spacing have been
            //added to paper as renderization took place.
            //Lets' verify is there is space left in paper for next system
            //! @todo It is assumed that next system height is equal to previous one.
            //!     It is necesary to compute each system height
            lmLUnits yNew = pPaper->GetCursorY() + nSystemHeight;
            if (yNew > pPaper->GetMaximumY() ) {
                //wxLogMessage(_T("Page break needed. yCur=%d, yNew=%d, MaximumY=%d"), pPaper->GetCursorY(), yNew, pPaper->GetMaximumY());
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
        pBoxSystem->SetIndent(((nSystem == 1) ? nFirstSystemIndent : nOtherSystemIndent ));

        bool fNewSystem = false;
        nRelMeasure = 1;    // the first measure in current system
        while (nAbsMeasure <= nTotalMeasures)
        {
            //reposition paper vertically at the start of the system. It has been advanced
            //when sizing the previous measure column
            pPaper->SetCursorY( ySystemPos );

            //if start of system, add system indentation to paper x pos.
            if (nRelMeasure == 1) {
                pPaper->IncrementCursorX(
                    ((nSystem == 1) ? nFirstSystemIndent : nOtherSystemIndent ));
            }

            //size this measure column
            m_nMeasureSize[nRelMeasure] =
                SizeMeasureColumn(nAbsMeasure, nRelMeasure, nSystem, pPaper, &fNewSystem);

///*LogDbg*/        wxLogMessage(wxString::Format(_T("[lmFormatter4::RenderJustified]: ")
//                    _T("m_nMeasureSize[%d] = %d"), nRelMeasure, m_nMeasureSize[nRelMeasure] ));

            //if this is the first measure column compute the space available in
            //this system. The method is a little tricky. The total space available
            //is (pPaper->GetPageRightMargin() - pPaper->GetCursorX()). But we have
            //to take into account the space that will be used by the prolog. As the
            //left position of the first measure column has taken all this into account,
            //it is posible to use that value by just doing:
            if (nRelMeasure == 1) {
                m_nFreeSpace =
                    pPaper->GetRightMarginXPos() - m_oTimepos[nRelMeasure].GetStartOfBarPosition();
            }

///*LogDbg*/        wxLogMessage(wxString::Format(_T("[lmFormatter4::RenderJustified]: ")
//                    _T("m_nFreeSpace = %d, PageRightMargin=%d, StartOfBar=%d"),
//                    m_nFreeSpace, pPaper->GetPageRightMargin(), m_oTimepos[nRelMeasure].GetStartOfBarPosition() ));
///*LogDbg*/        wxLogMessage(m_oTimepos[nRelMeasure].DumpTimeposTable());

            //substract space ocupied by this measure from space available in the system
            if (m_nFreeSpace < m_nMeasureSize[nRelMeasure]) {
                //there is no enough space for this measure column.
                //exit the loop. The system is finished
                break;
            } else {
                //there is enough space for this measure column. Add it to current system and
                //discount the space that the measure will take
                m_nFreeSpace -= m_nMeasureSize[nRelMeasure];
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
        //   m_nFreeSpace - free space available on this system
        //   m_nMeasureSize[1..MaxBar] - stores the minimum size for each measure column for
        //           the current system.
        //   m_nMeasuresInSystem  - the number of measures that fit in this system
        //
        //Now we preceed to re-distribute the remaining free space across all measures, so that
        //the system is justified. This step only computes the new measure column sizes and stores
        //them in table m_nMeasureSize[1..MaxBar] but changes nothing in the StaffObjs
        //-------------------------------------------------------------------------------
        if (m_nMeasuresInSystem == 0) {
            //The line width is not enough for drawing just one bar!!!
            pPaper->RestartPageCursors();    //as cursors has been modified by measurements
            /*! @todo
                this is too simple as RenderMinimal only produces good rendering
                in simple short scores (no multi-line or multi-instrument)
            */
            wxLogMessage(_T("[lmFormatter4::RenderJustified] The line width is not enough for drawing just one bar!!!. RenderMinimal() used."));
            return RenderMinimal(pPaper);
        }

        //dbg --------------
        if (m_fDebugMode) {
            wxLogMessage(_T("Before distributing free space"));
            wxLogMessage(_T("***************************************\n"));
            for (int i = 1; i <= m_nMeasuresInSystem; i++) {
                wxLogMessage(wxString::Format(
                    _T("Bar column %d. Size = %d"), i, m_nMeasureSize[i]));
            }
        }
        //dbg ---------------

        if (fJustified) {
            //if requested to justify systems, divide up the remaining space
            //between all bars, except if this is the last bar and options flag
            //"StopStaffLinesAtFinalBar" is set.
            if (!(fThisIsLastSystem && fStopStaffLinesAtFinalBarline))
                RedistributeFreeSpace(m_nFreeSpace);
        }

        //dbg --------------
        if (m_fDebugMode) {
            wxLogMessage(_T("After distributing free space"));
            wxLogMessage(_T("***************************************\n"));
            for (int i = 1; i <= m_nMeasuresInSystem; i++) {
                wxLogMessage(wxString::Format(
                    _T("Bar column %d. Size = %d"), i, m_nMeasureSize[i]));
            }
        }
        //dbg --------------



        //-------------------------------------------------------------------------------
        //Step 3: Re-position StaffObjs.
        //-------------------------------------------------------------------------------
        //when reaching this point the table m_nMeasureSize[i] stores the final size that
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
            //GrabarTrace "RedistributeSpace: nNewSize = " & m_nMeasureSize(i) & ", newStart = " & xStartOfMeasure    //dbg
            xStartOfMeasure = m_oTimepos[i].RedistributeSpace(m_nMeasureSize[i], xStartOfMeasure);
        }

        //dbg ------------------------------------------------------------------------------
        if (m_fDebugMode) {
            wxLogMessage(_T("After repositioning objects"));
            wxLogMessage(_T("***************************************\n"));
            wxLogMessage( m_pScore->Dump() );
        }
        //dbg ------------------------------------------------------------------------------

        //Store information about this system
        pBoxSystem->SetNumMeasures(m_nMeasuresInSystem);
        if (fThisIsLastSystem && fStopStaffLinesAtFinalBarline) {
            //this is the last system and it has been requested to stop staff lines
            //in last measure. So, set final x so staff lines go to final bar line
            lmLUnits xPos;
            if (pVStaff->GetXPosFinalBarline(&xPos))
                pBoxSystem->SetFinalX( xPos - 1 );
            else
                pBoxSystem->SetFinalX( pPaper->GetRightMarginXPos() );
        }
        else {
            //staff lines go to the rigth margin
            pBoxSystem->SetFinalX( pPaper->GetRightMarginXPos() );
        }


        // compute system height
        if (nSystem == 1) {
            nSystemHeight = pPaper->GetCursorY() - ySystemPos;
            //wxLogMessage(_T("[lmFormatter4::RenderJustified] nSystemHeight = %d"),
            //    nSystemHeight );
        }


        //increment loop information
        iIni += m_nMeasuresInSystem;
        nAbsMeasure = iIni;
        nSystem++;

    }    //while (nAbsMeasure <= nTotalMeasures)

    return pBoxScore;

}

lmLUnits lmFormatter4::SizeMeasureColumn(int nAbsMeasure, int nRelMeasure, int nSystem,
                                        lmPaper* pPaper, bool* pNewSystem)
{
    /*
     For each instrument and staff it is computed how many measures could fit in the system.
     All measurements are stored in the global object m_oTimepos[nRelMeasure], so that other
     procedures can take decisions about the final number of measures to include and for
     repositioning the StaffObjs.

     Input parameters:
       nAbsMeasure - Measure number (absolute) to size
       nRelMeasure - Measure number (relative) to size
       nSystem - System number

     Returns:
       - The size of this measure column.
       - positioning information for this measure column is stored in m_oTimepos[nRelMeasure]
       - Updates flag pointed by pNewSystem and sets it to true if newSystem tag found
            in this measure
    */

    int iVStaff;
    lmInstrument *pInstr;
    lmVStaff *pVStaff;
    bool fNewSystem = false;

    // explore all instruments in the score
    lmLUnits xPaperPos, yPaperPos;
    lmLUnits xStartPos = pPaper->GetCursorX();      //save x pos to align staves in system
    for (pInstr = m_pScore->GetFirstInstrument(); pInstr; pInstr=m_pScore->GetNextInstrument())
    {
        //verify that program limits are observed
        if (pInstr->GetNumStaves() > MAX_STAVES_PER_SYSTEM) {
            //! @todo log and display message properly
            wxLogMessage(
                _("[lmFormatter4::RenderJustified]: Program limitation: Maximum number of staves per system has been exceeded."));
            wxASSERT(false);
        }

        pPaper->SetCursorX( xStartPos );    //align staves in system

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

            //The prolog must be rendered on each system, but the
            //matching StaffObjs only exist in the first system. Therefore:
            //1. in the first system the prolog is rendered as part as the normal lmStaffObj
            //   rendering process and the available space for measures is all the paper width.
            //2. but for the other systems we must force the rendering of the prolog because there
            //   are no StaffObjs representing the prolog.
            if (nSystem != 1 && nRelMeasure == 1) {
                pVStaff->DrawProlog(DO_MEASURE, nAbsMeasure, (nSystem == 1), pPaper);
            }

            fNewSystem |= SizeMeasure(pVStaff, nAbsMeasure, nRelMeasure, pPaper);

            //advance paper in height off this lmVStaff
            //@attention As advancing one staff has the effect of returning
            //x position to the left marging, all x position information stored
            //in m_timepos is relative to the start of the measure
            pVStaff->NewLine(pPaper);
            //! @todo add inter-staff space

        }    // next lmVStaff

    }    // next lmInstrument

    //all measures in measure column number nAbsMeasure have been sized. The information is stored in
    //object m_oTimepos[nRelMeasure]. Now proced to re-position the StaffObjs so that all StaffObjs
    //sounding at the same time will have the same x coordinate. The method .ArrageStaffobjsByTime
    //returns the measure column size
    *pNewSystem = fNewSystem;
    return m_oTimepos[nRelMeasure].ArrangeStaffobjsByTime(m_fDebugMode);      //true = debug on

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
    // - the table m_nMeasureSize() stores the minimum size for each measure column for the
    //   current system.
    // - nAvailable stores the free space remaining at the end of this system
    //
    //on exit:
    // - the values stored in table m_nMeasureSize() are modified to reflect the new size
    //   for the bar columns, so that the line get justified.

    //-------------------------------------------------------------------------------------

    if (nAvailable <= 0) return;       //no space to distribute

    lmLUnits nDif[MAX_MEASURES_PER_SYSTEM];

    //compute average measure column size
    lmLUnits nAverage = 0;
    for (int i = 1; i <= m_nMeasuresInSystem; i++) {
        nAverage += m_nMeasureSize[i];
    }
    nAverage /= m_nMeasuresInSystem;

    lmLUnits nMeanPrev = 0;
    lmLUnits nDifTotal = 0;
    while (nAvailable > 0 && nAverage != nMeanPrev) {
        //for each measure column, compute the diference between its size and the average size
        //sum up all the diferences in nDifTotal
        nDifTotal = 0;
        for (int i = 1; i <= m_nMeasuresInSystem; i++) {
            nDif[i] = nAverage - m_nMeasureSize[i];
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
            if (nDif[i] > 0) { m_nMeasureSize[i] += nDif[i]; }
        }
        nAvailable -= nDifTotal;

        //compute the new measure column size average
        nMeanPrev = nAverage;
        nAverage = 0;
        for (int i = 1; i <= m_nMeasuresInSystem; i++) {
            nAverage += m_nMeasureSize[i];
        }
        nAverage /= m_nMeasuresInSystem;
    }

    //divide up the remaining space between all bars
    if (nAvailable > 0) {
        nDifTotal = nAvailable / m_nMeasuresInSystem;
        for (int i = 1; i <= m_nMeasuresInSystem; i++) {
            m_nMeasureSize[i] += nDifTotal;
            nAvailable -= nDifTotal;
        }
        m_nMeasureSize[m_nMeasuresInSystem] += nAvailable;
    }

}


//=========================================================================================
// Methods to deal with measures
//=========================================================================================

bool lmFormatter4::SizeMeasure(lmVStaff* pVStaff, int nAbsMeasure, int nRelMeasure,
                             lmPaper* pPaper)
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

    //! @todo Review this commented code. Must review also DrawMeasure
    //      I think this code is only for positioning text relative to barline
    //StaffObjs could have positioning information relative to the start of barline position.
    //Therfore, it is necessary to store start of barline position so that relative positioned
    //StaffObjs can be correctly positioned.
    //The stored value is accesible by method //GetXInicioCompas//
//    pVStaff->SetXInicioCompas = pPaper->GetCursorX()

    //start new thread
    m_oTimepos[nRelMeasure].NewThread();
    lmLUnits xStart = pPaper->GetCursorX();
    m_oTimepos[nRelMeasure].SetCurXLeft( xStart );

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
            // todo: how do you know it is the previous barline?
            lmStaffObj* pSO = pIT->GetCurrent();
            if (pSO->IsVisible())
            {
                // the barline is visble. Add space
                // todo: review this fixed barline after space
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
                //start a new thread, returning x pos to the same x pos than the
                //previous thread
                m_oTimepos[nRelMeasure].NewThread();
                pPaper->SetCursorX(m_oTimepos[nRelMeasure].GetCurXLeft());
            }
            else if(lmNEW_SYSTEM == nCtrolType)
            {
                //new system tag found in this measure
                fNewSystem = true;
            }
           // else ignore it
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
                pNote = (lmNote*)pSO;        //@attention we do not know yet if it is a note or a rest,
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

            //measure lmStaffObj and store its final and anchor x positions
            pSO->Draw(DO_MEASURE, pPaper);
                //store its final and anchor x positions
            oTimepos.SetCurXFinal(pPaper->GetCursorX());
            oTimepos.SetCurXAnchor(oTimepos.GetCurXLeft() + pSO->GetAnchorPos());
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
            lmLUnits xPos = pBarline->GetLocationPos();

            //compute user required barline position
            if (pBarline->GetLocationType() == lmLOCATION_RELATIVE) {
                xFinalPos = xStart + xPos;
            }
            else {  //lmLOCATION_ABSOLUTE
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
        pSO->Draw(DO_MEASURE, pPaper);
    }
    else {
        // no barline at the end of the measure. Close thread
        m_oTimepos[nRelMeasure].CloseThread(pPaper->GetCursorX());
    }

    //now store final x position of this measure
    m_oTimepos[nRelMeasure].SetCurXFinal(pPaper->GetCursorX());

    return fNewSystem;
}

