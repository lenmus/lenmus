//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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
#pragma implementation "ShapeNote.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "GMObject.h"
#include "../score/Score.h"
#include "../score/VStaff.h"
#include "../app/ScoreCanvas.h"
#include "ShapeNote.h"
#include "ShapeLine.h"
#include "ShapeArch.h"


//-------------------------------------------------------------------------------------
// Implementation of lmShapeNote
//-------------------------------------------------------------------------------------

//temporary data to be used during dragging
static int m_nOldSteps;		//to clear leger lines while dragging
static lmLUnits m_uxOldPos;



lmShapeNote::lmShapeNote(lmNoteRest* pOwner, lmLUnits xLeft, lmLUnits yTop, wxColour color)
	: lmCompositeShape(pOwner, 0, color, _T("Note"), lmDRAGGABLE, eGMO_ShapeNote)
{
    m_uxLeft = xLeft;
    m_uyTop = yTop;
	m_color = color;

	//initializations
	m_nNoteHead = -1;		// -1 = no shape
	m_pBeamShape = (lmShapeBeam*)NULL;
	m_pStemShape = (lmShapeStem*)NULL;
    m_pTieShape[0] = (lmShapeTie*)NULL;
    m_pTieShape[1] = (lmShapeTie*)NULL;
}

lmShapeNote::~lmShapeNote()
{
    //TODO. If this note is deleted and it has attachements to any other note,
    //the common attached shapes must also be deleted. This problem was detected 
    //with ties, when the next note is in the next system. [000.00.error6]. But
    //it will happen with beams [000.00.error7] and possibly with other objects. 
    //BUG_BYPASS. Specific code to deal with ties
 //   lmNote* pNote = (lmNote*)m_pOwner;
 //   if (pNote->IsTiedToPrev())
 //   {
 //       //Delete the tie,
 //       //When this note is re-layouted, the tie will be created again.
	//    std::list<lmAttachPoint*>::iterator pItem;
	//    for (pItem = m_cAttachments.begin(); pItem != m_cAttachments.end(); pItem++)
	//    {
	//	    if ( (*pItem)->pShape->IsShapeTie() )
 //           {
 //               //get 
 //           }
 //       }
	//if (pItem != m_cAttachments.end())
	//	m_cAttachments.erase(pItem);
 //       void lmShape::Detach(lmShape* pShape)

 //   }

}

void lmShapeNote::AddStem(lmShapeStem* pShape)
{
	Add(pShape);
	m_pStemShape = pShape;
}

void lmShapeNote::AddNoteHead(lmShape* pShape)
{
	m_nNoteHead = Add(pShape);
}

void lmShapeNote::AddFlag(lmShape* pShape)
{
	Add(pShape);
}

void lmShapeNote::AddAccidental(lmShape* pShape)
{
	Add(pShape);
}

void lmShapeNote::AddNoteInBlock(lmShape* pShape)
{
	Add(pShape);
}

void lmShapeNote::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
	lmCompositeShape::Shift(xIncr, yIncr);

	m_uxLeft += xIncr;
    m_uyTop += yIncr;

	InformAttachedShapes(xIncr, yIncr, lmSHIFT_EVENT);

	//if included in a composite shape update parent bounding and selection rectangles
	if (this->IsChildShape())
		((lmCompositeShape*)GetParentShape())->RecomputeBounds();
}

lmShape* lmShapeNote::GetNoteHead()
{
	if (m_nNoteHead < 0)
		return (lmShape*)NULL;

	return GetShape(m_nNoteHead);
}

void lmShapeNote::SetStemLength(lmLUnits uLength)
{
	lmShapeStem* pStem = GetStem();
	if (!pStem) return;


	if (StemGoesDown())
	{
		//adjust bottom point
		pStem->SetLength(uLength, false);
	}
	else
	{
		//adjust top point
		pStem->SetLength(uLength, true);
	}

	RecomputeBounds();

}

lmLUnits lmShapeNote::GetStemThickness()
{
	lmShapeStem* pStem = GetStem();
	if (!pStem) return 0.0;

	return pStem->GetXRight() - pStem->GetXLeft();
}

bool lmShapeNote::StemGoesDown()
{
	return ((lmNote*)m_pOwner)->StemGoesDown();
}

void lmShapeNote::ApplyUserShiftsToTieShape()
{
    //This note is the end note of a tie. And the note has been moved, during layout,
    //to its final position. Then, this method is invoked to inform the tie, so that
    //it can to apply user shifts to bezier points

    if (m_pTieShape[0])
        m_pTieShape[0]->ApplyUserShifts();

    if (m_pTieShape[1])
        m_pTieShape[1]->ApplyUserShifts();
}

wxBitmap* lmShapeNote::OnBeginDrag(double rScale, wxDC* pDC)
{
	m_nOldSteps = 0;
	m_uxOldPos = -100000.0f;		//any absurd value
	return lmCompositeShape::OnBeginDrag(rScale, pDC);
}

lmUPoint lmShapeNote::OnDrag(lmPaper* pPaper, const lmUPoint& uPos)
{
	// The view informs that the user continues dragging. We receive the new desired
	// shape position and we must return the new allowed shape position.
	//
	// The default behaviour is to return the received position, so the view redraws 
	// the drag image at that position. No action must be performed by the shape on 
	// the score and score objects.
	//
	// The received new desired shape position is in logical units and referred to page
	// origin. The returned new allowed shape position must also be in in logical units
	// and referred to page origin.

	if (g_fFreeMove) return uPos;

    // A note only can be moved in discrete vertical steps (staff lines/spaces)
    //return lmUPoint(uPos.x, GetYTop());	//only horizontal movement
    //return lmUPoint(uPos.x, uPos.y);		//free movement
    lmUPoint pos = uPos;
	int nSteps;
    pos.y = ((lmNote*)m_pOwner)->CheckNoteNewPosition(GetYTop(), uPos.y, &nSteps);

		//draw leger lines

	//as painting uses XOR we need the complementary color
	wxColour color = *wxBLUE;
	wxColour colorC(255 - (int)color.Red(), 255 - (int)color.Green(), 255 - (int)color.Blue() );
	pPaper->SetLogicalFunction(wxXOR);

	//wxLogMessage(_T("[lmShapeNote::OnDrag] OldSteps=%d, oldPos=%.2f, newSteps=%d, newPos=%.2f"),
	//	m_nOldSteps, m_uxOldPos, nSteps, uPos.x );
	//remove old ledger lines
	if (m_uxOldPos != -100000.0f)
		DrawLegerLines(m_nPosOnStaff + m_nOldSteps, m_uxOldPos, pPaper, colorC);

	//draw new ledger lines
	DrawLegerLines(m_nPosOnStaff + nSteps, uPos.x, pPaper, colorC);

	//save data for next time
	m_nOldSteps = nSteps;
	m_uxOldPos = uPos.x;

	return pos;
}

void lmShapeNote::OnEndDrag(lmPaper* pPaper, lmController* pCanvas, const lmUPoint& uPos)
{
	// End drag. Receives the command processor associated to the view and the
	// final position of the object (logical units referred to page origin).
	// This method must validate/adjust final position and, if ok, it must 
	// send a move object command to the controller.

	lmUPoint uFinalPos(uPos.x, uPos.y);
	int nSteps = 0;
	if (!g_fFreeMove)
	{
		//free movement not allowed. The note must be moved in discrete 
		//vertical steps (half lines)
		uFinalPos.y = ((lmNote*)m_pOwner)->CheckNoteNewPosition(GetYTop(), uPos.y, &nSteps);
	}

	//send a move note command to the controller
	pCanvas->MoveNote(this, uFinalPos, nSteps);
}

void lmShapeNote::Render(lmPaper* pPaper, wxColour color)
{
    //set selection rectangle as the notehead rectangle
	if (m_nNoteHead >= 0)
        m_uSelRect = GetNoteHead()->GetBounds();

    lmCompositeShape::Render(pPaper, color);
	DrawLegerLines(m_nPosOnStaff, GetXLeft(), pPaper, color);
}

void lmShapeNote::AddLegerLinesInfo(int nPosOnStaff, lmLUnits uyStaffTopLine)
{
	m_nPosOnStaff = nPosOnStaff;
	m_uyStaffTopLine = uyStaffTopLine;
}

void lmShapeNote::DrawLegerLines(int nPosOnStaff, lmLUnits uxLine, lmPaper* pPaper, wxColour color)
{
	//During note drag, it could be necessary to display new leger lines. This method
	//overlays to drag image the necesary leger lines

	//to draw leger lines we could use the same idea than for highlight: just invoke
	//render method of the appropiate shape. That means that:
	//1. the shape for leger lines
	//should not be a fixed shape for each line but a generic shape to draw all leger
	//lines of a note. The number of lines to draw should be dynamically computed by
	//shape render method
	//2. All notes must have a leger lines shape, even if no line is going to be drawn
	//
	//Therefore, it is note necessary to have leger lines shapes. They must be implicit
	//in the note shape rendering methods. So we only need to have the necessary information
	//in the note shape

    if (nPosOnStaff > 0 && nPosOnStaff < 12) return;

	lmVStaff* pVStaff = ((lmNote*)m_pOwner)->GetVStaff();
	int nStaff = ((lmNote*)m_pOwner)->GetStaffNum();
    lmLUnits uThick = pVStaff->GetStaffLineThick(nStaff);
    uxLine -= pVStaff->TenthsToLogical(4, nStaff);
    lmShape* pNoteHead = GetNoteHead();
    lmLUnits uLineLength = pNoteHead->GetWidth() + pVStaff->TenthsToLogical(8, nStaff);

	//force to paint lines of at least 1 px
	lmLUnits uOnePixel = pPaper->DeviceToLogicalY(1);
	uThick = uOnePixel;

    if (nPosOnStaff > 11)
	{
        // pos on staff > 11  ==> lines at top
        lmLUnits uDsplz = pVStaff->GetOptionLong(_T("Staff.UpperLegerLines.Displacement"));
        lmLUnits uyStart = m_uyStaffTopLine - pVStaff->TenthsToLogical(uDsplz, nStaff);
        for (int i=12; i <= nPosOnStaff; i++) {
            if (i % 2 == 0) {
                int nTenths = 5 * (i - 10);
                lmLUnits uyPos = uyStart - pVStaff->TenthsToLogical(nTenths, nStaff);
				//draw the line
				pPaper->SolidLine(uxLine, uyPos, uxLine + uLineLength, uyPos,
								  uThick, eEdgeNormal, color);
           }
        }

    }
	else
	{
        // nPosOnStaff < 1  ==>  lines at bottom
        for (int i=nPosOnStaff; i <= 0; i++) {
            if (i % 2 == 0)
			{
                int nTenths = 5 * (10 - i);
                lmLUnits uyPos = m_uyStaffTopLine + pVStaff->TenthsToLogical(nTenths, nStaff);
				//draw the line
				pPaper->SolidLine(uxLine, uyPos, uxLine + uLineLength, uyPos,
								  uThick, eEdgeNormal, color);
				//wxLogMessage(_T("[lmShapeNote::DrawLegerLines] Line from (%.2f, %.2f) to (%.2f, %.2f)"),
				//	uxLine, uyPos, uxLine + uLineLength, uyPos);

            }
        }
    }

}

