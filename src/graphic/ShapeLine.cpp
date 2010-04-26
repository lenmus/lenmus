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
#pragma implementation "ShapeLine.h"
#endif

// For compilers that support precompilation, includes <wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "Handlers.h"
#include "ShapeLine.h"
#include "AggDrawer.h"
#include "BoxPage.h"
#include "BoxSystem.h"
#include "../score/Score.h"
#include "../score/Score.h"
#include "../score/FiguredBass.h"
#include "../app/ScoreCanvas.h"

//access to colors
#include "../globals/Colors.h"
extern lmColors* g_pColors;



//========================================================================================
// lmShapeLine object implementation
//========================================================================================

//Backwards compatibility contructor
lmShapeLine::lmShapeLine(lmScoreObj* pOwner, int nShapeIdx,
                         lmLUnits uxStart, lmLUnits uyStart,
                         lmLUnits uxEnd, lmLUnits uyEnd, lmLUnits uWidth,
                         lmLUnits uBoundsExtraWidth, lmELineStyle nStyle,
                         wxColour nColor, lmELineEdges nEdge, bool fDraggable,
                         bool fSelectable, bool fVisible, wxString sName)
    : lmSimpleShape(eGMO_ShapeLine, pOwner, nShapeIdx, sName, fDraggable,
                    fSelectable, nColor, fVisible)
    , m_nStyle(nStyle)
{
	Create(uxStart, uyStart, uxEnd, uyEnd, uWidth, uBoundsExtraWidth, nColor, nEdge,
           lm_eLineCap_None, lm_eLineCap_None);
}

//new constructor
lmShapeLine::lmShapeLine(lmScoreObj* pOwner)
    : lmSimpleShape(eGMO_ShapeLine, pOwner, 0, _T("Line"))
{
	Create(0, 0, 0, 0, 0, 0, *wxBLACK, lm_eEdgeNormal, lm_eLineCap_None, lm_eLineCap_None);
}

lmShapeLine::~lmShapeLine()
{
    if (m_pHandler[lmID_START]) delete m_pHandler[lmID_START];
    if (m_pHandler[lmID_END]) delete m_pHandler[lmID_END];
}

void lmShapeLine::Create(lmLUnits xStart, lmLUnits yStart,
						 lmLUnits xEnd, lmLUnits yEnd, lmLUnits uWidth,
						 lmLUnits uBoundsExtraWidth, wxColour nColor,
						 lmELineEdges nEdge, lmELineCap nStartCap, lmELineCap nEndCap)
{
    m_uPoint[lmID_START].x = xStart;
    m_uPoint[lmID_START].y = yStart;
    m_uPoint[lmID_END].x = xEnd;
    m_uPoint[lmID_END].y = yEnd;
    m_color = nColor;
    m_uWidth = uWidth;
	m_uBoundsExtraWidth = uBoundsExtraWidth;
	m_nEdge = nEdge;
    m_nStartCap = nStartCap;
    m_nEndCap = nEndCap;

    UpdateBounds();

    //Create line handlers
    for (int i=0; i < lmID_NUM_HANDLERS; i++)
    {
        m_pHandler[i] = new lmHandlerSquare(m_pOwner, this, i);
	    m_fIsControlled[i] = false;
	    m_fIsFixed[i] = false;
    }
}

void lmShapeLine::SetAsControlled(lmELinePoint nPointID)
{
    //The referenced point will be set as 'controlled', that is, it will be
    //controlled by program (another shape) and therefore no handler will be
    //created for it. Also, the shape can not be dragged by user, as one of its
    //points is fixed.

    wxASSERT(nPointID == lmLINE_START || nPointID == lmLINE_END);

    m_fIsControlled[nPointID] = true;
}

void lmShapeLine::UpdateBounds()
{
	//For now assume the line is either vertical or horizontal

    // store boundling rectangle position and size
	lmLUnits uWidthRect = (m_uWidth + m_uBoundsExtraWidth) / 2.0;
	if (m_uPoint[lmID_START].x == m_uPoint[lmID_END].x)
	{
		//vertical line
		m_uBoundsTop.x = m_uPoint[lmID_START].x - uWidthRect;
		m_uBoundsTop.y = m_uPoint[lmID_START].y;
		m_uBoundsBottom.x = m_uPoint[lmID_END].x + uWidthRect;
		m_uBoundsBottom.y = m_uPoint[lmID_END].y;
	}
	else
	{
		//Horizontal line
		m_uBoundsTop.x = m_uPoint[lmID_START].x;
		m_uBoundsTop.y = m_uPoint[lmID_START].y - uWidthRect;
		m_uBoundsBottom.x = m_uPoint[lmID_END].x;
		m_uBoundsBottom.y = m_uPoint[lmID_END].y + uWidthRect;
	}

	NormaliceBoundsRectangle();

    // store selection rectangle position and size
	m_uSelRect = GetBounds();
}

void lmShapeLine::Render(lmPaper* pPaper, wxColour color)
{
    WXUNUSED(color);

    //if selected, book to be rendered with handlers when posible
    if (IsSelected())
    {
        //book to be rendered with handlers
        GetOwnerBoxPage()->OnNeedToDrawHandlers(this);

         for (int i=0; i < lmID_NUM_HANDLERS; i++)
         {
            //save points and update handlers position
            m_uSavePoint[i] = m_uPoint[i];
            m_pHandler[i]->SetHandlerCenterPoint(m_uPoint[i].x, m_uPoint[i].y);
         }
    }
    else
    {
        //draw the line
        DrawLine(pPaper, m_color, false);        //false -> anti-aliased
        lmSimpleShape::Render(pPaper, m_color);
    }
}

void lmShapeLine::RenderWithHandlers(lmPaper* pPaper)
{
    //render the line and its handlers

    //as painting uses XOR we need the complementary color
    wxColour color = *wxBLUE;      //TODO User options
    wxColour colorC = wxColour(255 - (int)color.Red(),
                               255 - (int)color.Green(),
                               255 - (int)color.Blue() );

    //prepare to render
    pPaper->SetLogicalFunction(wxXOR);

    //draw the handlers
    for (int i=0; i < lmID_NUM_HANDLERS; i++)
    {
        if (!m_fIsControlled[i])
        {
            m_pHandler[i]->Render(pPaper, colorC);
            GetOwnerBoxPage()->AddActiveHandler(m_pHandler[i]);
        }
    }

    //draw the line
    DrawLine(pPaper, colorC, true);        //true -> Sketch

    //terminate renderization
    pPaper->SetLogicalFunction(wxCOPY);
}

void lmShapeLine::DrawLine(lmPaper* pPaper, wxColour colorC, bool fSketch)
{
    //draw the line and update its bounds

    if (fSketch)
        pPaper->SketchLine(m_uPoint[lmID_START].x, m_uPoint[lmID_START].y,
                           m_uPoint[lmID_END].x, m_uPoint[lmID_END].y, colorC, wxSOLID);
    else
        //pPaper->SolidLine(m_uPoint[lmID_START].x, m_uPoint[lmID_START].y,
        //                  m_uPoint[lmID_END].x, m_uPoint[lmID_END].y, m_uWidth, m_nEdge, colorC);
        pPaper->DecoratedLine(m_uPoint[lmID_START], m_uPoint[lmID_END], m_uWidth, 
                              m_nStartCap, m_nEndCap, colorC);

    UpdateBounds();
}

bool lmShapeLine::HitTest(lmUPoint& uPoint)
{
    //point is not within the limits of this object selection rectangle
    if (!GetSelRectangle().Contains(uPoint))
        return false;

    //Find the distance from point to line
    lmLUnits uDistance = GetDistanceToLine(uPoint);

    lmLUnits uTolerance = m_uBoundsExtraWidth;

    //return true if click point is within tolerance margin 
    return (uDistance >= -uTolerance && uDistance <= uTolerance);
}

lmUPoint lmShapeLine::GetPointForHandler(long nHandlerID)
{
    return m_uPoint[nHandlerID];
}

wxString lmShapeLine::Dump(int nIndent)
{
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump += wxString::Format(_T("Idx: %d %s: start=(%.2f, %.2f), end=(%.2f, %.2f), line width=%.2f, "),
                m_nOwnerIdx, m_sGMOName.c_str(), m_uPoint[lmID_START].x, m_uPoint[lmID_START].y, m_uPoint[lmID_END].x, m_uPoint[lmID_END].y,
                m_uWidth );
    sDump += DumpBounds();
    sDump += _T("\n");

    //base class
    sDump += lmShape::Dump(nIndent);

	return sDump;
}

void lmShapeLine::Shift(lmLUnits uxIncr, lmLUnits uyIncr)
{
    m_uPoint[lmID_START].x += uxIncr;
    m_uPoint[lmID_START].y += uyIncr;
    m_uPoint[lmID_END].x += uxIncr;
    m_uPoint[lmID_END].y += uyIncr;

    ShiftBoundsAndSelRec(uxIncr, uyIncr);

	InformAttachedShapes(uxIncr, uyIncr, lmSHIFT_EVENT);

	//if included in a composite shape update parent bounding and selection rectangles
	if (this->IsChildShape())
		((lmCompositeShape*)GetParentShape())->RecomputeBounds();
}

lmUPoint lmShapeLine::OnHandlerDrag(lmPaper* pPaper, const lmUPoint& uPos,
                                    long nHandlerID)
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

    //erase previous draw
    RenderWithHandlers(pPaper);

    //store new handler coordinates and update all
    wxASSERT(nHandlerID >= 0 && nHandlerID < lmID_NUM_HANDLERS);
    m_pHandler[nHandlerID]->SetHandlerTopLeftPoint(uPos);
    m_uPoint[nHandlerID] = m_pHandler[nHandlerID]->GetHandlerCenterPoint();

    //draw at new position
    RenderWithHandlers(pPaper);

    return uPos;
}

void lmShapeLine::OnHandlerEndDrag(lmController* pCanvas, const lmUPoint& uPos,
                                   long nHandlerID)
{
	// End drag. Receives the command processor associated to the view and the
	// final position of the object (logical units referred to page origin).
	// This method must validate/adjust final position and, if ok, it must
	// send a move object command to the controller.

    //Compute shifts from start of drag points
    lmUPoint uShifts[lmID_NUM_HANDLERS];
    for (int i=0; i < lmID_NUM_HANDLERS; i++)
        uShifts[i] = lmUPoint(0.0, 0.0);
    uShifts[nHandlerID] = uPos + m_pHandler[nHandlerID]->GetTopCenterDistance()
                         - m_uSavePoint[nHandlerID];

    //MoveObjectPoints() apply shifts computed from drag start points. As handlers and
    //shape points are already displaced, it is necesary to restore the original positions to
    //avoid double displacements.
    for (int i=0; i < lmID_NUM_HANDLERS; i++)
        m_uPoint[i] = m_uSavePoint[i];
    UpdateBounds();

    pCanvas->MoveObjectPoints(this, uShifts, lmID_NUM_HANDLERS, false);  //false-> do not update views
}

wxBitmap* lmShapeLine::OnBeginDrag(double rScale, wxDC* pDC)
{
    //save all points position
    for (int i=0; i < lmID_NUM_HANDLERS; i++)
        m_uSavePoint[i] = m_uPoint[i];

    //No bitmap needed as we are going to re-draw the line as it is moved.
    return (wxBitmap*)NULL;
}

lmUPoint lmShapeLine::OnDrag(lmPaper* pPaper, const lmUPoint& uPos)
{
    //erase previous draw
    RenderWithHandlers(pPaper);

    //update all handler points and object points
    lmUPoint uShift(uPos - this->GetBounds().GetTopLeft());
    for (int i=0; i < lmID_NUM_HANDLERS; i++)
    {
        m_pHandler[i]->SetHandlerTopLeftPoint( uShift + m_pHandler[i]->GetBounds().GetLeftTop() );
        m_uPoint[i] = m_pHandler[i]->GetHandlerCenterPoint();
    }
    UpdateBounds();

    //draw at new position
    RenderWithHandlers(pPaper);

    return uPos;
}

void lmShapeLine::OnEndDrag(lmPaper* pPaper, lmController* pCanvas, const lmUPoint& uPos)
{
    //erase previous draw
    RenderWithHandlers(pPaper);

    //compute shift from start of drag point
    lmUPoint uShift = uPos - m_uSavePoint[0];

    //restore shape position to that of start of drag start so that MoveObject() or 
    //MoveObjectPoints() commands can apply shifts from original points.
    for (int i=0; i < lmID_NUM_HANDLERS; i++)
        m_uPoint[i] = m_uSavePoint[i];
    UpdateBounds();

    //as this is an object defined by points, instead of MoveObject() command we have to issue
    //a MoveObjectPoints() command.
    lmUPoint uShifts[lmID_NUM_HANDLERS];
    for (int i=0; i < lmID_NUM_HANDLERS; i++)
        uShifts[i] = uShift;
    pCanvas->MoveObjectPoints(this, uShifts, lmID_NUM_HANDLERS, false);  //false-> do not update views
}

void lmShapeLine::MovePoints(int nNumPoints, int nShapeIdx, lmUPoint* pShifts,
                             bool fAddShifts)
{
    //Each time a command is issued to change the line, we will receive a call
    //back to update the shape

    for (int i=0; i < lmID_NUM_HANDLERS; i++)
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
    UpdateBounds();
}


//------------------------------------------------------------------------------------------
//Some vector computations
//------------------------------------------------------------------------------------------

lmLUnits lmShapeLine::GetDistanceToLine(lmUPoint uPoint)
{
    //define line vector
    lmUVector uLineVector;
    uLineVector.x = m_uPoint[lmID_END].x - m_uPoint[lmID_START].x;
    uLineVector.y = m_uPoint[lmID_END].y - m_uPoint[lmID_START].y;

    //vector for line through the point
    lmUVector uPointVector;
    uPointVector.x = uPoint.x - m_uPoint[lmID_START].x;
    uPointVector.y = uPoint.y - m_uPoint[lmID_START].y;

    //get its projection on the line.
    //  uProjectionVector = uLineVector * factor;
    //  factor = (uPointVector * uLineVector) / (|uLineVector|^2);
    lmUVector uProjectionVector;
    lmLUnits uFactor = VectorDotProduct(uPointVector, uLineVector) / VectorDotProduct(uLineVector, uLineVector);
    uProjectionVector.x = uLineVector.x * uFactor;
    uProjectionVector.y = uLineVector.y * uFactor;

    //get normal vector: uNormalVector = uPointVector - uProjectionVector
    lmUVector uNormalVector;
    SubtractVectors(uPointVector, uProjectionVector, uNormalVector);

    //return its magnitude
    return VectorMagnitude(uNormalVector);
}

lmLUnits lmShapeLine::VectorDotProduct(lmUVector& v0, lmUVector& v1)
{
    return v0.x * v1.x + v0.y * v1.y;
}

void lmShapeLine::SubtractVectors(lmUVector& v0, lmUVector& v1, lmUVector& v)
{
    v.x = v0.x - v1.x;
    v.y = v0.y - v1.y;
}

lmLUnits lmShapeLine::VectorMagnitude(lmUVector& v)
{
    return sqrt(v.x * v.x + v.y * v.y);
}



//-------------------------------------------------------------------------------
//class lmShapeFBLine implementation
//-------------------------------------------------------------------------------

lmShapeFBLine::lmShapeFBLine(lmScoreObj* pOwner, int nShapeIdx,
                             lmFiguredBass* pEndFB,
                             lmLUnits uxStart, lmLUnits uyStart,    //user shift
                             lmLUnits uxEnd, lmLUnits uyEnd,        //user shift
                             lmTenths tWidth,
                             lmShapeFiguredBass* pShapeStartFB,
                             lmShapeFiguredBass* pShapeEndFB,
                             wxColour nColor, bool fVisible)
    : lmShapeLine(pOwner, nShapeIdx, uxStart, uyStart, uxEnd, uyEnd, 
                  pOwner->TenthsToLogical(tWidth),
                  pOwner->TenthsToLogical(tWidth + 1.0f),       //BoundsExtraWidth
                  lm_eLine_Solid, nColor, lm_eEdgeNormal, lmDRAGGABLE,
                  lmSELECTABLE, fVisible, _T("FB Line"))
    , m_pEndFB(pEndFB)
    , m_pBrotherLine((lmShapeFBLine*)NULL)
{
    m_nType = eGMO_ShapeFBLine;
    
    //save user shifts
    m_uUserShifts[0] = lmUPoint(uxStart, uyStart);
    m_uUserShifts[1] = lmUPoint(uxEnd, uyEnd);

    //compute the default line
    OnAttachmentPointMoved(pShapeStartFB, lm_eGMA_StartObj, 0.0, 0.0, lmSHIFT_EVENT);
    OnAttachmentPointMoved(pShapeEndFB, lm_eGMA_EndObj, 0.0, 0.0, lmSHIFT_EVENT);
    m_fUserShiftsApplied = false;
}

lmShapeFBLine::~lmShapeFBLine()
{
}

void lmShapeFBLine::OnAttachmentPointMoved(lmShape* pSFB, lmEAttachType nTag,
								           lmLUnits uxShift, lmLUnits uyShift,
                                           lmEParentEvent nEvent)
{
    //start or end figured bass object moved. Recompute start/end of line and,
    //if necessary, split the line

	WXUNUSED(uxShift);
	WXUNUSED(uyShift);
	WXUNUSED(nEvent);

	//Compute new attachment point and update line start/end point. FB line is
    //placed 20 tenths appart from the FB number, and 10 tenths down
    lmUPoint uPos;
    uPos.y = pSFB->GetYTop() + ((lmStaffObj*)m_pOwner)->TenthsToLogical(10.0);
	if (nTag == lm_eGMA_StartObj)
    {
        uPos.x = pSFB->GetXRight() + ((lmStaffObj*)m_pOwner)->TenthsToLogical(20.0);
        SetStartPoint(uPos);
    }
	else if (nTag == lm_eGMA_EndObj)
    {
        uPos.x = pSFB->GetXLeft() - ((lmStaffObj*)m_pOwner)->TenthsToLogical(20.0);
        SetEndPoint(uPos);
    }

    // check if the line have to be splitted
	if (!m_pBrotherLine) return;		//creating the line. No information yet

    lmUPoint paperPosEnd = GetEndFB()->GetReferencePaperPos();
    lmUPoint paperPosStart = m_pBrotherLine->GetEndFB()->GetReferencePaperPos();
    if (paperPosEnd.y != paperPosStart.y)
	{
        //if start FB paperPos Y is not the same than end FB paperPos Y the
		//FBs are in different systems. Therefore, the line must be splitted.
		//To do it:
		//	- detach the two intermediate points.
		//	- make both shapes visible.
		//
		// As there is no controller object to perform these actions, the first line
		// detecting the need must co-ordinate the necessary actions.

		//determine which line is the first one
		lmShapeFBLine* pFirstLine = this;		//assume this is the first one
		lmShapeFBLine* pSecondLine = m_pBrotherLine;
		if (paperPosStart.y > paperPosEnd.y)
		{
			//wrong assumption. Reverse asignment
			pFirstLine = m_pBrotherLine;
			pSecondLine = this;
		}

        //first line end point is right paper margin
		lmBoxSystem* pSystem = this->GetOwnerSystem();
		lmUPoint uEnd;
		uEnd.x = pSystem->GetSystemFinalX();
		uEnd.y = pFirstLine->GetStartPosY();
		pFirstLine->SetEndPoint(uEnd);
		pFirstLine->SetVisible(true);

		//second line start point is begining of system
		lmUPoint uStart;
		uStart.x = pSystem->GetPositionX();
		uStart.y = pSecondLine->GetEndPosY();
		pSecondLine->SetStartPoint(uStart);
		pSecondLine->SetVisible(true);
	}
}

//lmFiguredBass* lmShapeFBLine::GetStartFB()
//{
//    //the owner of a FB line is always the end note. Therefore, to get the start
//    //FB let's access the end FB
//    return m_pEndFB->GetTiedNotePrev();
//}

lmFiguredBass* lmShapeFBLine::GetEndFB()
{
    //the owner of a FB line is always the end note
    return m_pEndFB;
}

