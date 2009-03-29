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
#pragma implementation "Shapes.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "Shapes.h"
#include "AggDrawer.h"
#include "../score/Glyph.h"      //access to glyphs table
#include "../score/Score.h"
#include "../score/VStaff.h"
#include "../score/Staff.h"
#include "../app/ScoreCanvas.h"




//========================================================================================
// lmShapeSimpleLine object implementation
//========================================================================================

lmShapeSimpleLine::lmShapeSimpleLine(lmScoreObj* pOwner, lmLUnits xStart, lmLUnits yStart,
                lmLUnits xEnd, lmLUnits yEnd, lmLUnits uWidth, lmLUnits uBoundsExtraWidth,
				wxColour nColor, wxString sName, lmELineEdges nEdge)
    : lmSimpleShape(eGMO_ShapeLine, pOwner, 0, sName)
{
	Create(xStart, yStart, xEnd, yEnd, uWidth, uBoundsExtraWidth, nColor, nEdge);
}

void lmShapeSimpleLine::Create(lmLUnits xStart, lmLUnits yStart,
						 lmLUnits xEnd, lmLUnits yEnd, lmLUnits uWidth,
						 lmLUnits uBoundsExtraWidth, wxColour nColor,
						 lmELineEdges nEdge)
{
    m_xStart = xStart;
    m_yStart = yStart;
    m_xEnd = xEnd;
    m_yEnd = yEnd;
    m_color = nColor;
    m_uWidth = uWidth;
	m_uBoundsExtraWidth = uBoundsExtraWidth;
	m_nEdge = nEdge;

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
	lmLUnits uWidthRect = (m_uWidth + uBoundsExtraWidth) / 2.0;
	if (xStart == xEnd)
	{
		//vertical line
		m_uBoundsTop.x = xStart - uWidthRect;
		m_uBoundsTop.y = yStart;
		m_uBoundsBottom.x = xEnd + uWidthRect;
		m_uBoundsBottom.y = yEnd;
	}
	else
	{
		//Horizontal line
		m_uBoundsTop.x = xStart;
		m_uBoundsTop.y = yStart - uWidthRect;
		m_uBoundsBottom.x = xEnd;
		m_uBoundsBottom.y = yEnd + uWidthRect;
	}

	NormaliceBoundsRectangle();

    // store selection rectangle position and size
	m_uSelRect = GetBounds();

}

void lmShapeSimpleLine::Render(lmPaper* pPaper, wxColour color)
{
    pPaper->SolidLine(m_xStart, m_yStart, m_xEnd, m_yEnd, m_uWidth, m_nEdge, color);
    lmSimpleShape::Render(pPaper, color);
}

wxString lmShapeSimpleLine::Dump(int nIndent)
{
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump += wxString::Format(_T("Idx: %d %s: start=(%.2f, %.2f), end=(%.2f, %.2f), line width=%.2f, "),
                m_nOwnerIdx, m_sGMOName.c_str(), m_xStart, m_yStart, m_xEnd, m_yEnd, m_uWidth );
    sDump += DumpBounds();
    sDump += _T("\n");

	return sDump;
}

void lmShapeSimpleLine::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
    m_xStart += xIncr;
    m_yStart += yIncr;
    m_xEnd += xIncr;
    m_yEnd += yIncr;

    ShiftBoundsAndSelRec(xIncr, yIncr);

	//if included in a composite shape update parent bounding and selection rectangles
	if (this->IsChildShape())
		((lmCompositeShape*)GetParentShape())->RecomputeBounds();
}



//========================================================================================
// lmShapeGlyph object implementation
//========================================================================================

lmShapeGlyph::lmShapeGlyph(lmScoreObj* pOwner, int nShapeIdx, int nGlyph, 
                           lmPaper* pPaper, lmUPoint uPos, wxString sName, bool fDraggable,
                           wxColour color)
    : lmSimpleShape(eGMO_ShapeGlyph, pOwner, nShapeIdx, sName, fDraggable, lmSELECTABLE, color)
{
    m_nGlyph = nGlyph;

    // compute and store position
    m_uGlyphPos.x = uPos.x;
    m_uGlyphPos.y = uPos.y; // - pVStaff->TenthsToLogical(aGlyphsInfo[m_nGlyph].GlyphOffset, nStaff);

    // store boundling rectangle position and size
    wxASSERT(pOwner->IsComponentObj());
    wxString sGlyph( aGlyphsInfo[m_nGlyph].GlyphChar );
    lmComponentObj* pSO = ((lmComponentObj*)m_pOwner);
    lmStaff* pStaff = pSO->GetStaff();
    double rPointSize = pStaff->GetMusicFontSize();
    pPaper->FtSetFontSize(rPointSize);
    lmURect bbox = ((lmAggDrawer*)(pPaper->GetDrawer()))->FtGetGlyphBounds( (unsigned int)sGlyph.GetChar(0) );

	m_uBoundsTop.x = m_uGlyphPos.x + bbox.x;
	m_uBoundsTop.y = m_uGlyphPos.y + bbox.y + pSO->TenthsToLogical(60);
	m_uBoundsBottom.x = m_uBoundsTop.x + bbox.width;
	m_uBoundsBottom.y = m_uBoundsTop.y + bbox.height;

    // store selection rectangle position and size
	m_uSelRect = GetBounds();
}

double lmShapeGlyph::GetPointSize()
{
    lmComponentObj* pSO = ((lmComponentObj*)m_pOwner);
    return pSO->GetStaff()->GetMusicFontSize();
}

void lmShapeGlyph::Render(lmPaper* pPaper, wxColour color)
{
    wxString sGlyph( aGlyphsInfo[m_nGlyph].GlyphChar );

    pPaper->FtSetFontSize(GetPointSize());
    pPaper->SetTextForeground(color);
    lmComponentObj* pSO = ((lmComponentObj*)m_pOwner);
    pPaper->FtSetTextPosition(m_uGlyphPos.x, m_uGlyphPos.y + pSO->TenthsToLogical(60) );
    pPaper->FtDrawChar( (unsigned int)sGlyph.GetChar(0) );

    lmSimpleShape::Render(pPaper, color);
}

wxString lmShapeGlyph::Dump(int nIndent)
{
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump += wxString::Format(_T("Idx: %d %s: pos=(%.2f,%.2f), "),
        m_nOwnerIdx, m_sGMOName.c_str(), m_uGlyphPos.x, m_uGlyphPos.y);
    sDump += DumpBounds();
    sDump += _T("\n");

	return sDump;
}

void lmShapeGlyph::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
    m_uGlyphPos.x += xIncr;
    m_uGlyphPos.y += yIncr;

    ShiftBoundsAndSelRec(xIncr, yIncr);

	//if included in a composite shape update parent bounding and selection rectangles
	if (this->IsChildShape())
		((lmCompositeShape*)GetParentShape())->RecomputeBounds();
}

wxBitmap* lmShapeGlyph::OnBeginDrag(double rScale, wxDC* pDC)
{
	// A dragging operation is started. The view invokes this method to request the
	// bitmap to be used as drag image. No other action is required.

    return GetBitmapFromShape(rScale, g_pColors->ScoreSelected());
}

lmUPoint lmShapeGlyph::OnDrag(lmPaper* pPaper, const lmUPoint& uPos)
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

	return uPos;
}

void lmShapeGlyph::OnEndDrag(lmPaper* pPaper, lmController* pCanvas, const lmUPoint& uPos)
{
	// End drag. Receives the command processor associated to the view and the
	// final position of the object (logical units referred to page origin).
	// This method must validate/adjust final position and, if ok, it must
	// send a move object command to the controller.

	//send a move object command to the controller
	pCanvas->MoveObject(this, uPos);
}

lmUPoint lmShapeGlyph::GetObjectOrigin()
{
	//returns the origin of this shape
	return m_uBoundsTop;    //m_uGlyphPos;
}

wxBitmap* lmShapeGlyph::GetBitmapFromShape(double rScale, wxColour colorF, wxColour colorB)
{
    //Returns a bitmap with the glyph. The bitmap is only the bounding box.
    //Ownership of bitmap is transferred to caller method. It must delete it.


    //allocate an empty drawer for measurements
    wxBitmap dummyBitmap(1, 1);
    lmAggDrawer* pDrawer = new lmAggDrawer(&dummyBitmap, rScale);
    
    // Get size of glyph, in logical units
    wxString sGlyph( aGlyphsInfo[m_nGlyph].GlyphChar );
    double rPointSize = GetPointSize();
    pDrawer->FtSetFontSize(rPointSize);
    wxRect vBox = pDrawer->FtGetGlyphBoundsInPixels( (unsigned int)sGlyph.GetChar(0) );

    //allocate a bitmap for the glyph
    wxBitmap bitmap(vBox.width + 1, vBox.height + 1);

     //use this bitmap as rendering buffer
    delete pDrawer;
    pDrawer = new lmAggDrawer(&bitmap, rScale);
    pDrawer->FtSetFontSize(rPointSize);

    //render the glyph
    pDrawer->SetTextForeground(colorF);
    pDrawer->SetTextBackground(colorB);
    pDrawer->Clear();
    pDrawer->FtSetTextPositionPixels(- vBox.x, - vBox.y);
    pDrawer->FtDrawChar( (unsigned int)sGlyph.GetChar(0) );

    //get the image buffer and create a bitmap from it
    wxImage& image = pDrawer->GetImageBuffer();

    // Make the bitmap masked
    image.SetMaskColour(colorB.Red(), colorB.Green(), colorB.Blue());
    wxBitmap* pBitmap = new wxBitmap(image);
    delete pDrawer;

    ////DBG -----------
    //wxString sFileName = _T("ShapeGlyp.bmp");
    //pBitmap->SaveFile(sFileName, wxBITMAP_TYPE_BMP);
    ////END DBG -------

    return pBitmap;
}

void lmShapeGlyph::RenderHighlighted(wxDC* pDC, wxColour colorC)
{
    //The DC is scaled and its origin is positioned according current scrolling and
    //page origin in the view

    //get the bitmap
    double rScaleX, rScaleY; 
    pDC->GetUserScale(&rScaleX, &rScaleY);
    wxBitmap* pBitmap = GetBitmapFromShape(rScaleX, colorC, *wxBLACK);

    //blend it with current displayed page
    lmPixels vxDest = pDC->LogicalToDeviceX(m_uBoundsTop.x), 
             vyDest = pDC->LogicalToDeviceY(m_uBoundsTop.y);
    const wxBitmap& bitmap = *pBitmap;

    ////DBG -----------
    //wxString sFileName = _T("draw.bmp");
    //pBitmap->SaveFile(sFileName, wxBITMAP_TYPE_BMP);
    ////END DBG -------

    pDC->SetUserScale(1.0, 1.0);
    pDC->SetMapMode(wxMM_TEXT);
	pDC->SetDeviceOrigin(0, 0);
    //pDC->SetLogicalFunction(wxINVERT);
    //pDC->DrawBitmap(bitmap, vxDest, vyDest, true);     //true -> transparent

    wxMemoryDC dc;
    dc.SetMapMode(wxMM_TEXT);
    dc.SelectObject(*pBitmap);
    pDC->Blit(vxDest, vyDest, pBitmap->GetWidth(), pBitmap->GetHeight(), &dc, 0, 0, wxXOR, true);


    //TODO: During playback the bitmap for the black notehead is constantly used. It could save
    //a lot of processing time if this particular bitmap is cached.

    delete pBitmap;
}



//========================================================================================
// lmShapeClef
//========================================================================================

lmShapeClef::lmShapeClef(lmScoreObj* pOwner, int nShapeIdx, int nGlyph, lmPaper* pPaper,
                         lmUPoint offset, bool fSmallClef, wxString sName,
				         bool fDraggable, wxColour color) 
	: lmShapeGlyph(pOwner, nShapeIdx, nGlyph, pPaper, offset, sName, fDraggable, color)
    , m_fSmallClef(fSmallClef)
{
    m_nType = eGMO_ShapeClef;
}

double lmShapeClef::GetPointSize()
{
    lmStaffObj* pSO = ((lmStaffObj*)m_pOwner);
    lmStaff* pStaff = pSO->GetVStaff()->GetStaff(pSO->GetStaffNum());
    return (m_fSmallClef ? pStaff->GetMusicFontSize() * 0.8 : pStaff->GetMusicFontSize());
}

lmUPoint lmShapeClef::OnDrag(lmPaper* pPaper, const lmUPoint& uPos)
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

    // A clef only can be moved horizonatlly
    return lmUPoint(uPos.x, GetYTop());

}

void lmShapeClef::OnEndDrag(lmPaper* pPaper, lmController* pCanvas, const lmUPoint& uPos)
{
	// End drag. Receives the command processor associated to the view and the
	// final position of the object (logical units referred to page origin).
	// This method must validate/adjust final position and, if ok, it must
	// send a move object command to the controller.

	lmUPoint uFinalPos(uPos.x, uPos.y);
	if (!g_fFreeMove)
	{
		//free movement not allowed. Only x position can be changed
		uFinalPos.y = GetYTop();
	}

	//send a move object command to the controller
	pCanvas->MoveObject(this, uFinalPos);

}


//========================================================================================
// lmShapeInvisible
//========================================================================================

lmShapeInvisible::lmShapeInvisible(lmScoreObj* pOwner, int nShapeIdx, lmUPoint uPos, lmUSize uSize,
                                   wxString sName)
	: lmSimpleShape(eGMO_ShapeInvisible, pOwner, nShapeIdx, sName, lmNO_DRAGGABLE, lmNO_SELECTABLE)
{
    m_uBoundsTop.x = uPos.x;
    m_uBoundsTop.y = uPos.y;
    m_uBoundsBottom.x = uPos.x + uSize.x;
    m_uBoundsBottom.y = uPos.y + uSize.y;
}

wxString lmShapeInvisible::Dump(int nIndent)
{
	//TODO
	return _T("lmShapeInvisible\n");
}

void lmShapeInvisible::Render(lmPaper* pPaper, wxColour color)
{
    //if (g_fDrawInvisible)       //TODO
    {
    }
}


//========================================================================================
// lmShapeRectangle: a rectangle with optional rounded corners
//========================================================================================

lmShapeRectangle::lmShapeRectangle(lmScoreObj* pOwner, lmLUnits xLeft, lmLUnits yTop,
                                   lmLUnits xRight, lmLUnits yBottom, lmLUnits uWidth,
                                   wxColour color, wxString sName,
				                   bool fDraggable, bool fSelectable, 
                                   bool fVisible)
    : lmSimpleShape(eGMO_ShapeRectangle, pOwner, 0, sName, fDraggable, fSelectable, 
                    color, fVisible)
{
    m_uCornerRadius = 0.0f;
    m_xLeft = xLeft;
    m_yTop = yTop;
    m_xRight = xRight;
    m_yBottom = yBottom;
    m_uWidth = uWidth;

    // store boundling rectangle position and size
    lmLUnits uWidthRect = m_uWidth / 2.0;
    
    m_uBoundsTop.x = xLeft - uWidthRect;
    m_uBoundsTop.y = yTop - uWidthRect;
    m_uBoundsBottom.x = xRight + uWidthRect;
    m_uBoundsBottom.y = yBottom + uWidthRect;

    NormaliceBoundsRectangle();

    // store selection rectangle position and size
    m_uSelRect = GetBounds();
}

void lmShapeRectangle::Render(lmPaper* pPaper, wxColour color)
{
    lmELineEdges nEdge = eEdgeNormal;

    //top side
    pPaper->SolidLine(m_xLeft, m_yTop, m_xRight, m_yTop, m_uWidth, nEdge, color);
    //right side
    pPaper->SolidLine(m_xRight, m_yTop, m_xRight, m_yBottom, m_uWidth, nEdge, color);
    //bottom side
    pPaper->SolidLine(m_xLeft, m_yBottom, m_xRight, m_yBottom, m_uWidth, nEdge, color);
    //left side
    pPaper->SolidLine(m_xLeft, m_yTop, m_xLeft, m_yBottom, m_uWidth, nEdge, color);

    lmSimpleShape::Render(pPaper, color);
}

void lmShapeRectangle::SetCornerRadius(lmLUnits uRadius)
{
    m_uCornerRadius = uRadius;
}

wxString lmShapeRectangle::Dump(int nIndent)
{
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump += wxString::Format(_T("Idx: %d %s: left-top=(%.2f, %.2f), right-bottom=(%.2f, %.2f), line width=%.2f, "),
                m_nOwnerIdx, m_sGMOName.c_str(), m_xLeft, m_yTop, m_xRight, m_yBottom, m_uWidth );
    sDump += DumpBounds();
    sDump += _T("\n");

	return sDump;
}

void lmShapeRectangle::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
    m_xLeft += xIncr;
    m_yTop += yIncr;
    m_xRight += xIncr;
    m_yBottom += yIncr;

    ShiftBoundsAndSelRec(xIncr, yIncr);

	//if included in a composite shape update parent bounding and selection rectangles
	if (this->IsChildShape())
		((lmCompositeShape*)GetParentShape())->RecomputeBounds();
}



//========================================================================================
// lmShapeStem object implementation: a vertical line
//========================================================================================

lmShapeStem::lmShapeStem(lmScoreObj* pOwner, lmLUnits xPos, lmLUnits yStart,
                         lmLUnits uExtraLength, lmLUnits yEnd, bool fStemDown,
                         lmLUnits uWidth, wxColour nColor)
	: lmShapeSimpleLine(pOwner, xPos, yStart, xPos, yEnd, uWidth, 0.0, nColor,
				  _T("Stem"), eEdgeHorizontal)
{
    m_nType = eGMO_ShapeStem;
	m_fStemDown = fStemDown;
    m_uExtraLength = uExtraLength;
}

void lmShapeStem::SetLength(lmLUnits uLenght, bool fModifyTop)
{
	if (fModifyTop)
	{
		if (m_yStart < m_yEnd)
			m_yStart = m_yEnd - uLenght;
		else
			m_yEnd = m_yStart - uLenght;
	}
	else
	{
		if (m_yStart < m_yEnd)
			m_yEnd = m_yStart + uLenght;
		else
			m_yStart = m_yEnd + uLenght;
	}

	//re-create the shape
	Create(m_xStart, m_yStart, m_xEnd, m_yEnd, m_uWidth, m_uBoundsExtraWidth,
		   m_color, m_nEdge);

}

void lmShapeStem::Adjust(lmLUnits xPos, lmLUnits yStart, lmLUnits yEnd, bool fStemDown)
{
	m_fStemDown = fStemDown;
	//re-create the shape
	Create(xPos, yStart, xPos, yEnd, m_uWidth, m_uBoundsExtraWidth,
		   m_color, m_nEdge);
}

lmLUnits lmShapeStem::GetYStartStem()
{
	//Start of stem is the nearest position to the notehead

	return (m_fStemDown ? GetYTop() : GetYBottom());
}

lmLUnits lmShapeStem::GetYEndStem()
{
	//End of stem is the farthest position from the notehead

	return (m_fStemDown ? GetYBottom() : GetYTop());
}

lmLUnits lmShapeStem::GetXCenterStem()
{
	//returns the stem x position. This position is in the middle of the line width
	return m_xStart;
}

