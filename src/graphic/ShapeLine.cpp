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
#pragma implementation "ShapeLine.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "Handlers.h"
#include "ShapeLine.h"
#include "AggDrawer.h"
#include "../score/Score.h"
#include "BoxPage.h"
#include "../app/ScoreCanvas.h"

//access to colors
#include "../globals/Colors.h"
extern lmColors* g_pColors;


//========================================================================================
// lmShapeLine object implementation
//========================================================================================

//Backwards compatibility contructor
lmShapeLine::lmShapeLine(lmScoreObj* pOwner, lmLUnits xStart, lmLUnits yStart,
                lmLUnits xEnd, lmLUnits yEnd, lmLUnits uWidth, lmLUnits uBoundsExtraWidth,
				wxColour nColor, wxString sName, lmELineEdges nEdge)
    : lmSimpleShape(eGMO_ShapeLine, pOwner, 0, sName)
{
	Create(xStart, yStart, xEnd, yEnd, uWidth, uBoundsExtraWidth, nColor, nEdge);
}

//new constructor
lmShapeLine::lmShapeLine(lmScoreObj* pOwner)
    : lmSimpleShape(eGMO_ShapeLine, pOwner, 0, _T("Line"))
{
	Create(0, 0, 0, 0, 0, 0, *wxBLACK, eEdgeNormal);
}

lmShapeLine::~lmShapeLine()
{
    if (m_pHandler[lmID_START]) delete m_pHandler[lmID_START];
    if (m_pHandler[lmID_END]) delete m_pHandler[lmID_END];
}

void lmShapeLine::Create(lmLUnits xStart, lmLUnits yStart,
						 lmLUnits xEnd, lmLUnits yEnd, lmLUnits uWidth,
						 lmLUnits uBoundsExtraWidth, wxColour nColor,
						 lmELineEdges nEdge)
{
    m_uPoint[lmID_START].x = xStart;
    m_uPoint[lmID_START].y = yStart;
    m_uPoint[lmID_END].x = xEnd;
    m_uPoint[lmID_END].y = yEnd;
    m_color = nColor;
    m_uWidth = uWidth;
	m_uBoundsExtraWidth = uBoundsExtraWidth;
	m_nEdge = nEdge;

    UpdateBounds();

    //Create line handlers
    m_pHandler[lmID_START] = new lmHandlerSquare(m_pOwner, this, lmID_START);
    m_pHandler[lmID_END] = new lmHandlerSquare(m_pOwner, this, lmID_END);
	//AddHandler(m_pHandler[lmID_START]);
	//AddHandler(m_pHandler[lmID_END]);
}

void lmShapeLine::UpdateBounds()
{
/*
	//TODO
    // if line is neither vertical nor horizontal, should we use a strait rectangle or a
    // leaned rectangle sorrounding the line?

    //width of rectangle = width of line + 2 pixels
    uWidth += 2.0 / g_r;

    //line angle
    double alpha = atan((yEnd - yStart) / (xEnd - xStart));

    //boundling rectangle
    {
    lmLUnits uIncrX = (lmLUnits)( (uWidth * sin(alpha)) / 2.0 );
    lmLUnits uIncrY = (lmLUnits)( (uWidth * cos(alpha)) / 2.0 );
    lmUPoint uPoints[] = {
        lmUPoint(xStart+uIncrX, yStart-uIncrY),
        lmUPoint(xStart-uIncrX, yStart+uIncrY),
        lmUPoint(xEnd-uIncrX, yEnd+uIncrY),
        lmUPoint(xEnd+uIncrX, yEnd-uIncrY)
    };
    SolidPolygon(4, uPoints, color);
*/

	//For now assume the line is either vertical or horizontal
	//TODO

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

void lmShapeLine::OnSelectionStatusChanged()
{
    //hide or restore visibility of main shape

    //SetVisible( !IsSelected() );
}

void lmShapeLine::Render(lmPaper* pPaper, wxColour color)
{
    //if selected, book to be rendered with handlers when posible
    if (IsSelected())
    {
        //book to be rendered with handlers
        GetOwnerBoxPage()->OnNeedToDrawHandlers(this);

         for (int i=0; i < lmNUM_HANDLERS; i++)
         {
            //save points and update handlers position
            m_uSavePoint[i] = m_uPoint[i];
            m_pHandler[i]->SetHandlerCenterPoint(m_uPoint[i].x, m_uPoint[i].y);
         }
    }
    else
    {
        //draw the line
        DrawLine(pPaper, color, false);        //false -> anti-aliased
        lmSimpleShape::Render(pPaper, color);
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
    for (int i=0; i < lmNUM_HANDLERS; i++)
    {
        m_pHandler[i]->Render(pPaper, colorC);
        GetOwnerBoxPage()->AddActiveHandler(m_pHandler[i]);
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
        pPaper->SolidLine(m_uPoint[lmID_START].x, m_uPoint[lmID_START].y,
                          m_uPoint[lmID_END].x, m_uPoint[lmID_END].y, m_uWidth, m_nEdge, colorC);

    UpdateBounds();
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

	return sDump;
}

void lmShapeLine::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
    m_uPoint[lmID_START].x += xIncr;
    m_uPoint[lmID_START].y += yIncr;
    m_uPoint[lmID_END].x += xIncr;
    m_uPoint[lmID_END].y += yIncr;

    ShiftBoundsAndSelRec(xIncr, yIncr);

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
    wxASSERT(nHandlerID >= 0 && nHandlerID < lmNUM_HANDLERS);
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
    lmUPoint uShifts[lmNUM_HANDLERS];
    for (int i=0; i < lmNUM_HANDLERS; i++)
        uShifts[i] = lmUPoint(0.0, 0.0);
    uShifts[nHandlerID] = uPos + m_pHandler[nHandlerID]->GetTopCenterDistance()
                         - m_uSavePoint[nHandlerID];

    //MoveObjectPoints() apply shifts computed from drag start points. As handlers and
    //shape points are already displaced, it is necesary to restore the original positions to
    //avoid double displacements.
    for (int i=0; i < lmNUM_HANDLERS; i++)
        m_uPoint[i] = m_uSavePoint[i];
    UpdateBounds();

    pCanvas->MoveObjectPoints(this, uShifts, lmNUM_HANDLERS, false);  //false-> do not update views
}

wxBitmap* lmShapeLine::OnBeginDrag(double rScale, wxDC* pDC)
{
    //save all points position
    for (int i=0; i < lmNUM_HANDLERS; i++)
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
    for (int i=0; i < lmNUM_HANDLERS; i++)
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
    for (int i=0; i < lmNUM_HANDLERS; i++)
        m_uPoint[i] = m_uSavePoint[i];
    UpdateBounds();

    //as this is an object defined by points, instead of MoveObject() command we have to issue
    //a MoveObjectPoints() command.
    lmUPoint uShifts[lmNUM_HANDLERS];
    for (int i=0; i < lmNUM_HANDLERS; i++)
        uShifts[i] = uShift;
    pCanvas->MoveObjectPoints(this, uShifts, lmNUM_HANDLERS, false);  //false-> do not update views
}

void lmShapeLine::MovePoints(int nNumPoints, int nShapeIdx, lmUPoint* pShifts,
                             bool fAddShifts)
{
    //Each time a commnad is issued to change the line, we will receive a call
    //back to update the shape

    for (int i=0; i < lmNUM_HANDLERS; i++)
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
