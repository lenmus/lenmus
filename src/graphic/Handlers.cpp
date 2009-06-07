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
#pragma implementation "Handlers.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "GMObject.h"
#include "../score/Score.h"
#include "../app/ScoreCanvas.h"
#include "ShapeLine.h"

#include "Handlers.h"


//-------------------------------------------------------------------------------------
// Implementation of lmHandler
//-------------------------------------------------------------------------------------


lmHandler::lmHandler(lmScoreObj* pOwner, lmGMObject* pOwnerGMO, long nHandlerID)
	: lmSimpleShape(eGMO_Handler, pOwner, 0, _T("Handler"), lmDRAGGABLE, lmNO_SELECTABLE)
      , m_pOwnerGMO(pOwnerGMO)
      , m_nHandlerID(nHandlerID)
{
}

lmUPoint lmHandler::OnDrag(lmPaper* pPaper, const lmUPoint& uPos)
{
	// The view informs that the user continues dragging. We receive the new desired
	// shape position and we must return the new allowed shape position.
	//
	// The default behaviour is to inform the controlled shape, and to return the 
    // received position, so the view redraws the drag image at that position. No action must be performed by the shape on 
	// the score and score objects.

    return m_pOwnerGMO->OnHandlerDrag(pPaper, uPos, m_nHandlerID);
}

void lmHandler::OnEndDrag(lmPaper* pPaper, lmController* pCanvas, const lmUPoint& uPos)
{
	// End drag. Receives the command processor associated to the view and the
	// final position of the object (logical units referred to page origin).
	// This method must validate/adjust final position and, if ok, it must 
	// send a move object command to the controller.

    m_pOwnerGMO->OnHandlerEndDrag(pCanvas, uPos, m_nHandlerID);
}

//-------------------------------------------------------------------------------------
// Implementation of lmHandlerLine
//-------------------------------------------------------------------------------------

lmHandlerLine::lmHandlerLine(lmScoreObj* pOwner, lmGMObject* pOwnerGMO,
                                 long nHandlerID)
	: lmHandler(pOwner, pOwnerGMO, nHandlerID)
{
}

//lmUPoint lmHandlerLine::GetHandlerCenterPoint()
//{
//    return lmUPoint(m_uTopLeft.x + m_uSide/2.0, m_uTopLeft.y + m_uSide/2.0);
//}

void lmHandlerLine::SetHandlerPoints(lmUPoint uStart, lmUPoint uEnd)
{
    SetHandlerPoints(uStart.x, uStart.y, uEnd.x, uEnd.y);
}

void lmHandlerLine::SetHandlerPoints(lmLUnits xStart, lmLUnits yStart,
						             lmLUnits xEnd, lmLUnits yEnd)
{
    m_xStart = xStart;
    m_yStart = yStart;
    m_xEnd = xEnd;
    m_yEnd = yEnd;

    OnPointsChanged();
}

void lmHandlerLine::OnPointsChanged()
{
    //Compute bounding rectangle and selection rectangle

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
	lmLUnits uWidthRect = m_pOwner->TenthsToLogical(4.0);   //TODO: Options?
	if (m_xStart == m_xEnd)
	{
		//vertical line
		m_uBoundsTop.x = m_xStart - uWidthRect;
		m_uBoundsTop.y = m_yStart;
		m_uBoundsBottom.x = m_xEnd + uWidthRect;
		m_uBoundsBottom.y = m_yEnd;
	}
	else
	{
		//Horizontal line
		m_uBoundsTop.x = m_xStart;
		m_uBoundsTop.y = m_yStart - uWidthRect;
		m_uBoundsBottom.x = m_xEnd;
		m_uBoundsBottom.y = m_yEnd + uWidthRect;
	}

	NormaliceBoundsRectangle();

    // store selection rectangle position and size
	m_uSelRect = GetBounds();
}

void lmHandlerLine::Render(lmPaper* pPaper, wxColour color)
{
    //render the handler

    pPaper->SketchLine(m_xStart, m_yStart, m_xEnd, m_yEnd, color, wxSOLID);
}

wxString lmHandlerLine::Dump(int nIndent)
{
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
    sDump += wxString::Format(_T("Idx: %d %s: HandlerID:%d"),
        m_nOwnerIdx, m_sGMOName.c_str(), m_nHandlerID );
    sDump += DumpBounds();
    sDump += _T("\n");
	return sDump;
}

void lmHandlerLine::OnMouseIn(wxWindow* pWindow, lmUPoint& uPoint)
{
    pWindow->SetCursor( wxCursor(wxCURSOR_SIZING) );
    m_pMouseCursorWindow = pWindow;
}



//-------------------------------------------------------------------------------------
// Implementation of lmHandlerSquare
//-------------------------------------------------------------------------------------

lmHandlerSquare::lmHandlerSquare(lmScoreObj* pOwner, lmGMObject* pOwnerGMO,
                                 long nHandlerID, wxStockCursor nCursorId)
	: lmHandler(pOwner, pOwnerGMO, nHandlerID)
    , m_cursor(wxCursor(nCursorId))
{
    m_uSide = m_pOwner->TenthsToLogical(10.0);
}

lmHandlerSquare::lmHandlerSquare(lmScoreObj* pOwner, lmGMObject* pOwnerGMO,
                                 long nHandlerID, wxCursor* pCursor)
	: lmHandler(pOwner, pOwnerGMO, nHandlerID)
    , m_cursor(*pCursor)
{
    m_uSide = m_pOwner->TenthsToLogical(10.0);
}

void lmHandlerSquare::SetHandlerCenterPoint(lmUPoint uPos)
{
    SetHandlerCenterPoint(uPos.x, uPos.y);
}

void lmHandlerSquare::SetHandlerCenterPoint(lmLUnits uxPos, lmLUnits uyPos)
{
    m_uTopLeft = lmUPoint(uxPos - m_uSide/2.0, uyPos - m_uSide/2.0);
    OnPointsChanged();
}

void lmHandlerSquare::OnPointsChanged()
{
    //Compute bounding rectangle and selection rectangle

	//set bounds
	SetXLeft(m_uTopLeft.x);
	SetYTop(m_uTopLeft.y);
	SetXRight(m_uTopLeft.x + m_uSide);
	SetYBottom(m_uTopLeft.y + m_uSide);

    // store selection rectangle position and size
	m_uSelRect = GetBounds();
}

void lmHandlerSquare::SetHandlerTopLeftPoint(lmUPoint uPos)
{ 
    m_uTopLeft = uPos;
    OnPointsChanged();
}

lmUPoint lmHandlerSquare::GetHandlerCenterPoint()
{
    return lmUPoint(m_uTopLeft.x + m_uSide/2.0, m_uTopLeft.y + m_uSide/2.0);
}

void lmHandlerSquare::Render(lmPaper* pPaper, wxColour color)
{
    //render the handler

    pPaper->SketchRectangle(m_uTopLeft, lmUSize(m_uSide, m_uSide), color);
    lmSimpleShape::Render(pPaper, color);
}

wxString lmHandlerSquare::Dump(int nIndent)
{
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
    sDump += wxString::Format(_T("Idx: %d %s: HandlerID:%d"),
		m_nOwnerIdx, m_sGMOName.c_str(), m_nHandlerID );
    sDump += DumpBounds();
    sDump += _T("\n");
	return sDump;
}

void lmHandlerSquare::OnMouseIn(wxWindow* pWindow, lmUPoint& uPoint)
{
    pWindow->SetCursor(m_cursor);
    m_pMouseCursorWindow = pWindow;
}



//-------------------------------------------------------------------------------------
// Implementation of lmShapeMargin
//-------------------------------------------------------------------------------------


lmShapeMargin::lmShapeMargin(lmScore* pScore, lmGMObject* pOwnerGMO, int nIdx,
                             int nPage, bool fVertical, lmLUnits uPos,
                             lmLUnits uLenght, wxColour color)
	: lmHandler(pScore, pOwnerGMO, (long)nIdx)
      , m_nIdx(nIdx)
	  , m_nPage(nPage)
      , m_fVertical(fVertical)
      , m_uPos(uPos)
      , m_uLenght(uLenght)
{
	m_color = color;

    //options?
    m_uThighness = pScore->TenthsToLogical(10.0f);   //handler thighness
    m_uWidth = pScore->TenthsToLogical(20.0f);       //handler width

	//set bounds
    lmLUnits uWidth = m_uThighness / 2.0f;
    if (fVertical)
    {
	    SetXLeft(m_uPos - uWidth);
	    SetYTop(0.0f);
	    SetXRight(m_uPos + uWidth);
	    SetYBottom(m_uLenght);
    }
    else
    {
	    SetXLeft(0.0f);
	    SetYTop(m_uPos - uWidth);
	    SetXRight(m_uLenght);
	    SetYBottom(m_uPos + uWidth);
    }

    // store selection rectangle position and size
	m_uSelRect = GetBounds();
}

lmShapeMargin::~lmShapeMargin()
{
}

void lmShapeMargin::Render(lmPaper* pPaper, wxColour color)
{
    // Render will draw nothing unless flag g_fShowMargins is true

    if (!g_fShowMargins) return;


    //render the margin
    //as painting uses XOR we need the complementary color
    wxColour colorC = 
        wxColour(255 - (int)color.Red(), 255 - (int)color.Green(), 255 - (int)color.Blue() );

    pPaper->SetLogicalFunction(wxXOR);

    DrawLine(pPaper, colorC);
    DrawHandlers(pPaper, colorC);

    pPaper->SetLogicalFunction(wxCOPY);

    lmSimpleShape::Render(pPaper, color);
}

wxString lmShapeMargin::Dump(int nIndent)
{
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump += wxString::Format(_T("Idx: %d %s: uPos=%.2f, uLenght=%.2f, fVertical=%s, "),
		m_nOwnerIdx, m_sGMOName.c_str(), m_uPos, m_uLenght, 
        (m_fVertical ? _T("yes") : _T("no")) );
    sDump += DumpBounds();
    sDump += _T("\n");
	return sDump;
}

void lmShapeMargin::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
    if (m_fVertical)
    {
        m_uPos += xIncr;
        ShiftBoundsAndSelRec(xIncr, 0.0f);
    }
    else
    {
        m_uPos += yIncr;
        ShiftBoundsAndSelRec(0.0f, yIncr);
    }
}

void lmShapeMargin::DrawLine(lmPaper* pPaper, wxColour color)
{
    if (m_fVertical)
        pPaper->SketchLine(m_uPos, 0.0f, m_uPos, m_uLenght, color, wxSHORT_DASH);
    else
        pPaper->SketchLine(0.0f, m_uPos, m_uLenght, m_uPos, color, wxSHORT_DASH);
}

void lmShapeMargin::DrawHandlers(lmPaper* pPaper, wxColour color)
{
    if (m_fVertical)
    {
        lmUSize size(m_uThighness, m_uWidth);
        lmUPoint uLeft(m_uPos - m_uThighness / 2.0f, 0.0f);
        pPaper->SketchRectangle(uLeft, size, color);
        lmUPoint uRight(m_uPos - m_uThighness / 2.0f, m_uLenght - m_uWidth);
        pPaper->SketchRectangle(uRight, size, color);
    }
    else
    {
        lmUSize size(m_uWidth, m_uThighness);
        lmUPoint uTop(0.0f, m_uPos - m_uThighness / 2.0f);
        pPaper->SketchRectangle(uTop, size, color);
        lmUPoint uBottom(m_uLenght - m_uWidth, m_uPos - m_uThighness / 2.0f);
        pPaper->SketchRectangle(uBottom, size, color);
    }
}

void lmShapeMargin::OnMouseIn(wxWindow* pWindow, lmUPoint& uPoint)
{
    if (m_fVertical)
	    pWindow->SetCursor( wxCursor(wxCURSOR_SIZEWE) );
    else
	    pWindow->SetCursor( wxCursor(wxCURSOR_SIZENS) );

    m_pMouseCursorWindow = pWindow;
}

lmUPoint lmShapeMargin::OnDrag(lmPaper* pPaper, const lmUPoint& uPos)
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

    // A margin line only can be moved in vertical or horizontal

    //limit margins movement to have at least 30% of page size for rendering the score
    lmScore* pScore = (lmScore*)m_pOwner;
    lmUPoint pos = uPos;
    pos = pScore->CheckHandlerNewPosition(this, m_nIdx, m_nPage, pos);


    //received paper is a DirectDrawer DC

    //erase previous draw
    Render(pPaper, *wxGREEN);

    //store new coordinates
    if (m_fVertical)
        m_uPos = pos.x;
    else
        m_uPos = pos.y;

    //draw at new position
    Render(pPaper, *wxGREEN);

    return pos;
}

void lmShapeMargin::OnEndDrag(lmPaper* pPaper, lmController* pCanvas, const lmUPoint& uPos)
{
	// End drag. Receives the command processor associated to the view and the
	// final position of the object (logical units referred to page origin).
	// This method must validate/adjust final position and, if ok, it must 
	// send a move object command to the controller.


    lmScore* pScore = (lmScore*)m_pOwner;
    lmUPoint pos = uPos;
    pos = pScore->CheckHandlerNewPosition(this, m_nIdx, m_nPage, pos);

    //save new position and send the command to change margin position
    if (m_fVertical)
        m_uPos = pos.x;
    else
        m_uPos = pos.y;

    pCanvas->ChangePageMargin(this, m_nIdx, m_nPage, m_uPos);
}
