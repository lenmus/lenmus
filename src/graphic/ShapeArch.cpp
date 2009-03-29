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
#pragma implementation "ShapeArch.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "../score/Score.h"
#include "../app/ScoreCanvas.h"
#include "GMObject.h"
#include "ShapeNote.h"
#include "ShapeArch.h"
#include "BoxSystem.h"
#include "Handlers.h"
#include "BoxPage.h"


//========================================================================================
// lmShapeArch object implementation
//========================================================================================


lmShapeArch::lmShapeArch(lmScoreObj* pOwner, int nShapeIdx, lmUPoint uStart, lmUPoint uEnd,
                         bool fArchUnder, wxColour nColor, wxString sName,
                         bool fDraggable, bool fVisible)
    : lmSimpleShape(eGMO_ShapeArch, pOwner, nShapeIdx, sName, fDraggable, lmSELECTABLE, nColor,
                    fVisible)
      , m_fArchUnder(fArchUnder)
{
    m_uPoint[lmBEZIER_START] = uStart;
    m_uPoint[lmBEZIER_END] = uEnd;
    m_uPoint[lmBEZIER_CTROL1] = lmUPoint(0.0, 0.0);
    m_uPoint[lmBEZIER_CTROL2] = lmUPoint(0.0, 0.0);

    m_color = nColor;
    SetDefaultControlPoints();
    Create();
}

lmShapeArch::lmShapeArch(lmScoreObj* pOwner, int nShapeIdx, lmUPoint uStart, lmUPoint uEnd,
                         lmUPoint uCtrol1, lmUPoint uCtrol2, wxColour nColor,
                         wxString sName, bool fDraggable, bool fVisible)
    : lmSimpleShape(eGMO_ShapeArch, pOwner, nShapeIdx, sName, fDraggable, lmSELECTABLE, 
                    nColor, fVisible)
      , m_fArchUnder(uStart.y < uCtrol1.y)
{
    m_uPoint[lmBEZIER_START] = uStart;
    m_uPoint[lmBEZIER_END] = uEnd;
    m_uPoint[lmBEZIER_CTROL1] = uCtrol1;
    m_uPoint[lmBEZIER_CTROL2] = uCtrol2;

    m_color = nColor;
    Create();
}

lmShapeArch::lmShapeArch(lmScoreObj* pOwner, int nShapeIdx, bool fArchUnder,
                         wxColour nColor, wxString sName, bool fDraggable, bool fVisible)
    : lmSimpleShape(eGMO_ShapeArch, pOwner, nShapeIdx, sName, fDraggable, lmSELECTABLE,
                    nColor, fVisible)
      , m_fArchUnder(fArchUnder)
{
    m_color = nColor;
    Create();
}

void lmShapeArch::Create()
{
    //Create handlers
    for (int i=0; i < lmBEZIER_MAX; i++)
        m_pHandler[i] = new lmHandlerSquare(m_pOwner, this, i);
}

lmShapeArch::~lmShapeArch()
{
    //delete handlers
    for (int i=0; i < lmBEZIER_MAX; i++)
        if (m_pHandler[i]) delete m_pHandler[i];
}

wxString lmShapeArch::Dump(int nIndent)
{
    //TODO
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump += wxString::Format(_T("Idx: %d %s: start=(%.2f, %.2f), end=(%.2f, %.2f), ")
                _T("ctrol1=(%.2f, %.2f), ctrol2=(%.2f, %.2f), ")
                _T("Arch under note = %s, "),
                m_nOwnerIdx, m_sGMOName.c_str(), m_uPoint[lmBEZIER_START].x, m_uPoint[lmBEZIER_START].y, m_uPoint[lmBEZIER_END].x, m_uPoint[lmBEZIER_END].y,
                m_uPoint[lmBEZIER_CTROL1].x, m_uPoint[lmBEZIER_CTROL1].y, m_uPoint[lmBEZIER_CTROL2].x, m_uPoint[lmBEZIER_CTROL2].y,
                (m_fArchUnder ? _T("yes") : _T("no")) );
    sDump += DumpBounds();
    sDump += _T("\n");
	return sDump;
}

void lmShapeArch::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
    for (int i=0; i < lmBEZIER_MAX; i++)
    {
        m_uPoint[i].x += xIncr;
        m_uPoint[i].y += yIncr;
    }

    ShiftBoundsAndSelRec(xIncr, yIncr);

	//if included in a composite shape update parent bounding and selection rectangles
	if (this->IsChildShape())
		((lmCompositeShape*)GetParentShape())->RecomputeBounds();
}

void lmShapeArch::SetStartPoint(lmLUnits xPos, lmLUnits yPos)
{
    m_uPoint[lmBEZIER_START].x = xPos;
    m_uPoint[lmBEZIER_START].y = yPos;
    SetDefaultControlPoints();
}

void lmShapeArch::SetEndPoint(lmLUnits xPos, lmLUnits yPos)
{
    m_uPoint[lmBEZIER_END].x = xPos;
    m_uPoint[lmBEZIER_END].y = yPos;
    SetDefaultControlPoints();
}

void lmShapeArch::SetCtrolPoint1(lmLUnits xPos, lmLUnits yPos)
{
    m_uPoint[lmBEZIER_CTROL1].x = xPos;
    m_uPoint[lmBEZIER_CTROL1].y = yPos;
}

void lmShapeArch::SetCtrolPoint2(lmLUnits xPos, lmLUnits yPos)
{
    m_uPoint[lmBEZIER_CTROL2].x = xPos;
    m_uPoint[lmBEZIER_CTROL2].y = yPos;
}

void lmShapeArch::Render(lmPaper* pPaper, wxColour colorC)
{
	if (!m_fVisible) return;

    //if selected, book to be rendered with handlers when posible
    if (IsSelected())
    {
        //book to be rendered with handlers
        GetOwnerBoxPage()->OnNeedToDrawHandlers(this);

         for (int i=0; i < lmBEZIER_MAX; i++)
         {
            //save points and update handlers position
            m_uSavePoint[i] = m_uPoint[i];
            m_pHandler[i]->SetHandlerCenterPoint(m_uPoint[i].x, m_uPoint[i].y);
         }
    }
    else
    {
        Draw(pPaper, colorC, false);        //false: anti-aliased
        lmSimpleShape::Render(pPaper, colorC);
    }
}

void lmShapeArch::Draw(lmPaper* pPaper, wxColour colorC, bool fSketch)
{
    lmLUnits uWidth = lmToLogicalUnits(0.2, lmMILLIMETERS);         // width = 0.2 mm

    //lmShapeArch is rendered as a cubic bezier curve. The number of points to draw is
    // variable, to suit a minimun resolution of 5 points / mm.

    // determine number of interpolation points to use
    int nNumPoints = (int)((m_uPoint[lmBEZIER_END].x - m_uPoint[lmBEZIER_START].x) / lmToLogicalUnits(0.2, lmMILLIMETERS) );
    if (nNumPoints < 5) nNumPoints = 5;

    // compute increment for mu variable
    double incr = 1.0 / (double)(nNumPoints-1);

    // start point
    double x1 = m_uPoint[lmBEZIER_START].x;
    double y1 = m_uPoint[lmBEZIER_START].y;

    //take the opportunity to compute bounds limits
    double xMin = x1 , yMin = y1;
    double xMax = x1 , yMax = y1;

    // loop to compute bezier curve points and draw segment lines
    int i;
    double mu, mum1, a, b, c, d;
    double x2, y2;
    for (i=1, mu = incr; i < nNumPoints-1; i++, mu += incr) {
        mum1 = 1 - mu;
        a = mum1 * mum1 * mum1;
        b = 3 * mu * mum1 * mum1;
        c = 3 * mu * mu * mum1;
        d = mu * mu * mu;

        // compute next point
        x2 = a * m_uPoint[lmBEZIER_START].x + b * m_uPoint[lmBEZIER_CTROL1].x 
             + c * m_uPoint[lmBEZIER_CTROL2].x + d * m_uPoint[lmBEZIER_END].x;
        y2 = a * m_uPoint[lmBEZIER_START].y + b * m_uPoint[lmBEZIER_CTROL1].y 
             + c * m_uPoint[lmBEZIER_CTROL2].y + d * m_uPoint[lmBEZIER_END].y;

        // draw segment line
        if (fSketch)
            pPaper->SketchLine(x1, y1, x2, y2, colorC);
        else
            pPaper->SolidLine(x1, y1, x2, y2, uWidth, eEdgeNormal, colorC);

        //update bounds
        xMin = wxMin(xMin, x2);
        yMin = wxMin(yMin, y2);
        xMax = wxMax(xMax, x2);
        yMax = wxMax(yMax, y2);

        // prepare for next point
        x1 = x2;
        y1 = y2;
    }

    //Update bounds rectangle
    SetXLeft(xMin);
    SetYTop(yMin);
    SetXRight(xMax);
    SetYBottom(yMax);

    //update selection rectangle
    m_uSelRect = GetBounds();
}

void lmShapeArch::RenderWithHandlers(lmPaper* pPaper)
{
    //render the arch and its handlers

    //as painting uses XOR we need the complementary color
    wxColour color = *wxBLUE;      //TODO User options
    wxColour colorC = wxColour(255 - (int)color.Red(),
                               255 - (int)color.Green(),
                               255 - (int)color.Blue() );

    //prepare to render
    pPaper->SetLogicalFunction(wxXOR);

    //draw the handlers
    for (int i=0; i < lmBEZIER_MAX; i++)
    {
        m_pHandler[i]->Render(pPaper, colorC);
        GetOwnerBoxPage()->AddActiveHandler(m_pHandler[i]);
    }

    //draw the arch
    Draw(pPaper, colorC, true);        //true: sketch

    //terminate renderization
    pPaper->SetLogicalFunction(wxCOPY);
}

void lmShapeArch::SetDefaultControlPoints()
{
    // compute the default control points for the arc
    m_uPoint[lmBEZIER_CTROL1].x = m_uPoint[lmBEZIER_START].x + (m_uPoint[lmBEZIER_END].x - m_uPoint[lmBEZIER_START].x) / 3;
    lmLUnits yDsplz = lmToLogicalUnits(2, lmMILLIMETERS);
    m_uPoint[lmBEZIER_CTROL1].y = m_uPoint[lmBEZIER_START].y + (m_fArchUnder ? yDsplz : -yDsplz);

    m_uPoint[lmBEZIER_CTROL2].x = m_uPoint[lmBEZIER_CTROL1].x + (m_uPoint[lmBEZIER_END].x - m_uPoint[lmBEZIER_START].x) / 3;
    m_uPoint[lmBEZIER_CTROL2].y = m_uPoint[lmBEZIER_END].y + (m_fArchUnder ? yDsplz : -yDsplz);
}

lmUPoint lmShapeArch::OnHandlerDrag(lmPaper* pPaper, const lmUPoint& uPos, long nHandlerID)
{
    //erase previous draw
    RenderWithHandlers(pPaper);

    //store new handler coordinates and update all
    wxASSERT(nHandlerID >= 0 && nHandlerID < lmBEZIER_MAX);
    m_pHandler[nHandlerID]->SetHandlerTopLeftPoint(uPos);
    m_uPoint[nHandlerID] = m_pHandler[nHandlerID]->GetHandlerCenterPoint();

    //draw at new position
    RenderWithHandlers(pPaper);

    return uPos;
}

void lmShapeArch::OnHandlerEndDrag(lmController* pCanvas, const lmUPoint& uPos, long nHandlerID)
{
	// End drag. Receives the command processor associated to the view and the
	// final position of the object (logical units referred to page origin).
	// This method must validate/adjust final position and, if ok, it must
	// send a move object command to the controller.

    //Compute shifts from start of drag points
    lmUPoint uShifts[lmBEZIER_MAX];
    for (int i=0; i < lmBEZIER_MAX; i++)
        uShifts[i] = lmUPoint(0.0, 0.0);
    uShifts[nHandlerID] = uPos + m_pHandler[nHandlerID]->GetTopCenterDistance()
                         - m_uSavePoint[nHandlerID];

    //MoveObjectPoints() apply shifts computed from drag start points. As handlers and
    //shape points are already displaced, it is necesary to restore the original positions to
    //avoid double displacements.
    for (int i=0; i < lmBEZIER_MAX; i++)
        m_uPoint[i] = m_uSavePoint[i];

    pCanvas->MoveObjectPoints(this, uShifts, lmBEZIER_MAX, false);  //false-> do not update views
}

wxBitmap* lmShapeArch::OnBeginDrag(double rScale, wxDC* pDC)
{
    //save all points position
    for (int i=0; i < lmBEZIER_MAX; i++)
        m_uSavePoint[i] = m_uPoint[i];

    //No bitmap needed as we are going to re-draw the line as it is moved.
    return (wxBitmap*)NULL;
}

lmUPoint lmShapeArch::OnDrag(lmPaper* pPaper, const lmUPoint& uPos)
{
    //erase previous draw
    RenderWithHandlers(pPaper);

    //update all handler points and object points
    lmUPoint uShift(uPos - this->GetBounds().GetTopLeft());
    for (int i=0; i < lmBEZIER_MAX; i++)
    {
        m_pHandler[i]->SetHandlerTopLeftPoint( uShift + m_pHandler[i]->GetBounds().GetLeftTop() );
        m_uPoint[i] = m_pHandler[i]->GetHandlerCenterPoint();
    }

    //draw at new position
    RenderWithHandlers(pPaper);

    return uPos;
}

void lmShapeArch::OnEndDrag(lmPaper* pPaper, lmController* pCanvas, const lmUPoint& uPos)
{
    //erase previous draw
    RenderWithHandlers(pPaper);

    //compute shift from start of drag point
    lmUPoint uShift = uPos - m_uSavePoint[0];

    //restore shape position to that of start of drag start so that MoveObject() or 
    //MoveObjectPoints() commands can apply shifts from original points.
    for (int i=0; i < lmBEZIER_MAX; i++)
        m_uPoint[i] = m_uSavePoint[i];

    //as this is an object defined by points, instead of MoveObject() command we have to issue
    //a MoveObjectPoints() command.
    lmUPoint uShifts[lmBEZIER_MAX];
    for (int i=0; i < lmBEZIER_MAX; i++)
        uShifts[i] = uShift;
    pCanvas->MoveObjectPoints(this, uShifts, lmBEZIER_MAX, false);  //false-> do not update views
}

void lmShapeArch::MovePoints(int nNumPoints, int nShapeIdx, lmUPoint* pShifts,
                             bool fAddShifts)
{
    //Each time a commnad is issued to change the object, we will receive a call
    //back to update the shape

    for (int i=0; i < lmBEZIER_MAX; i++)
    {
        if (fAddShifts)
        {
            m_uPoint[i].x += (*(pShifts+i)).x;
            m_uPoint[i].y += (*(pShifts+i)).y;
        }
        else
        {
            m_uPoint[i].x -= (*(pShifts+i)).x;
            m_uPoint[i].y -= (*(pShifts+i)).y;
        }

        m_pHandler[i]->SetHandlerCenterPoint(m_uPoint[i]);
    }
}



//========================================================================================
// lmShapeTie implementation
//========================================================================================


lmShapeTie::lmShapeTie(lmTie* pOwner, int nShapeIdx, lmNote* pEndNote, lmUPoint* pPoints,
                       lmShapeNote* pShapeStart, lmShapeNote* pShapeEnd,
                       bool fTieUnderNote, wxColour color, bool fVisible)
    : lmShapeArch(pOwner, nShapeIdx, fTieUnderNote, color, _T("Tie"), lmDRAGGABLE, fVisible)
    , m_pEndNote(pEndNote)
    , m_fTieUnderNote(fTieUnderNote)
	, m_pBrotherTie((lmShapeTie*)NULL)
{
    m_nType = eGMO_ShapeTie;
    
    //save user shifts
    for (int i=0; i < 4; i++)
        m_uUserShifts[i] = *(pPoints+i);

    //compute the default arch
    OnAttachmentPointMoved(pShapeStart, eGMA_StartNote, 0.0, 0.0, lmSHIFT_EVENT);
    OnAttachmentPointMoved(pShapeEnd, eGMA_EndNote, 0.0, 0.0, lmSHIFT_EVENT);
    m_fUserShiftsApplied = false;
}

lmShapeTie::~lmShapeTie()
{
}

void lmShapeTie::Render(lmPaper* pPaper, wxColour color)
{
    lmShapeArch::Render(pPaper, color);
}

void lmShapeTie::DrawControlPoints(lmPaper* pPaper)
{
    //DBG
    DrawBounds(pPaper, *wxGREEN);
}

void lmShapeTie::OnAttachmentPointMoved(lmShape* pShape, lmEAttachType nTag,
								lmLUnits uxShift, lmLUnits uyShift, lmEParentEvent nEvent)
{
    //start or end note moved. Recompute start/end of tie and, if necessary, split
    //the tie.

	//WXUNUSED(uxShift);
	//WXUNUSED(uyShift);
	//WXUNUSED(nEvent);

	//get notehead shape
	lmShape* pSNH = ((lmShapeNote*)pShape)->GetNoteHead();
	wxASSERT(pSNH);

	//Compute new attachment point
	lmLUnits uHalfNH = (pSNH->GetXRight() - pSNH->GetXLeft()) / 2.0;
    lmLUnits uHeightNH = pSNH->GetYBottom() - pSNH->GetYTop();
    lmLUnits uxPos = pSNH->GetXLeft() + uHalfNH;
    lmLUnits uyPos = ((lmStaffObj*)m_pOwner)->TenthsToLogical(5.0);
    uyPos = (m_fTieUnderNote ?
            pSNH->GetYTop() + uHeightNH + uyPos : pSNH->GetYTop() - uyPos );

    //update arch start/end points
	if (nTag == eGMA_StartNote)
        SetStartPoint(uxPos, uyPos);
	else if (nTag == eGMA_EndNote)
        SetEndPoint(uxPos, uyPos);

    // check if the tie have to be splitted
	if (!m_pBrotherTie) return;		//creating the tie. No information yet

    lmUPoint paperPosEnd = GetEndNote()->GetReferencePaperPos();
    lmUPoint paperPosStart = m_pBrotherTie->GetEndNote()->GetReferencePaperPos();
    if (paperPosEnd.y != paperPosStart.y)
	{
        //if start note paperPos Y is not the same than end note paperPos Y the
		//notes are in different systems. Therefore, the tie must be splitted.
		//To do it:
		//	- detach the two intermediate points.
		//	- make both shapes visible.
		//
		// As there is no controller object to perform these actions, the first tie
		// detecting the need must co-ordinate the necessary actions.

		//determine which tie is the first one
		lmShapeTie* pFirstTie = this;		//assume this is the first one
		lmShapeTie* pSecondTie = m_pBrotherTie;
		if (paperPosStart.y > paperPosEnd.y)
		{
			//wrong assumption. Reverse asignment
			pFirstTie = m_pBrotherTie;
			pSecondTie = this;
		}

        //first tie end point is right paper margin
		lmBoxSystem* pSystem = this->GetOwnerSystem();
		lmUPoint uEnd;
		uEnd.x = pSystem->GetSystemFinalX();
		uEnd.y = pFirstTie->GetStartPosY();
		pFirstTie->SetEndPoint(uEnd.x, uEnd.y);
		pFirstTie->SetVisible(true);

		//second tie start point is begining of system
		lmUPoint uStart;
		uStart.x = pSystem->GetPositionX();
		uStart.y = pSecondTie->GetEndPosY();
		pSecondTie->SetStartPoint(uStart.x, uStart.y);
		pSecondTie->SetVisible(true);
	}
}

lmNote* lmShapeTie::GetStartNote()
{
    //the owner of a tie is always the end note
    return m_pEndNote->GetTiedNotePrev();
}

lmNote* lmShapeTie::GetEndNote()
{
    //the owner of a tie is always the end note
    return m_pEndNote;
}

void lmShapeTie::ApplyUserShifts()
{
    //Start and end notes are now at their final positions. Therefore, default bezier curve is computed.
    //This method is then invoked to apply user shifts to bezier arch.

    if (!m_fUserShiftsApplied)
    {
        //transfer bezier data
        for (int i=0; i < 4; i++)
        {
            m_uPoint[i].x += m_uUserShifts[i].x;
            m_uPoint[i].y += m_uUserShifts[i].y;
        }
        m_fUserShiftsApplied = true;
    }
}

