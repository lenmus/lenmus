//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "Shapes.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "Shapes.h"
#include "../score/Glyph.h"      //access to glyphs table
#include "../score/Score.h"
#include "../app/ScoreCanvas.h"


//========================================================================================
// lmShapeLine object implementation
//========================================================================================

lmShapeLine::lmShapeLine(lmScoreObj* pOwner, lmLUnits xStart, lmLUnits yStart,
                lmLUnits xEnd, lmLUnits yEnd, lmLUnits uWidth, lmLUnits uBoundsExtraWidth,
				wxColour nColor, wxString sName, lmELineEdges nEdge)
    : lmSimpleShape(eGMO_ShapeLine, pOwner, sName)
{
	Create(xStart, yStart, xEnd, yEnd, uWidth, uBoundsExtraWidth, nColor, nEdge);
}

void lmShapeLine::Create(lmLUnits xStart, lmLUnits yStart,
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

void lmShapeLine::Render(lmPaper* pPaper, wxColour color)
{
    pPaper->SolidLine(m_xStart, m_yStart, m_xEnd, m_yEnd, m_uWidth, m_nEdge, color);
    lmShape::RenderCommon(pPaper);
}

wxString lmShapeLine::Dump(int nIndent)
{
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump += wxString::Format(_T("%04d %s: start=(%.2f, %.2f), end=(%.2f, %.2f), line width=%.2f, "),
                m_nId, m_sGMOName.c_str(), m_xStart, m_yStart, m_xEnd, m_yEnd, m_uWidth );
    sDump += DumpBounds();
    sDump += _T("\n");

	return sDump;
}

void lmShapeLine::Shift(lmLUnits xIncr, lmLUnits yIncr)
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

lmShapeGlyph::lmShapeGlyph(lmScoreObj* pOwner, int nGlyph, wxFont* pFont, lmPaper* pPaper,
                           lmUPoint uPos, wxString sName, bool fDraggable, wxColour color)
    : lmSimpleShape(eGMO_ShapeGlyph, pOwner, sName, fDraggable, color)
{
    m_nGlyph = nGlyph;
    m_pFont = pFont;

    // compute and store position
    m_uGlyphPos.x = uPos.x;
    m_uGlyphPos.y = uPos.y; // - pVStaff->TenthsToLogical(aGlyphsInfo[m_nGlyph].GlyphOffset, nStaff);

    // store boundling rectangle position and size
    lmLUnits uWidth, uHeight;
    wxString sGlyph( aGlyphsInfo[m_nGlyph].GlyphChar );
    pPaper->SetFont(*m_pFont);
    pPaper->GetTextExtent(sGlyph, &uWidth, &uHeight);

	m_uBoundsTop.x = m_uGlyphPos.x;
	m_uBoundsTop.y = m_uGlyphPos.y + ((lmStaffObj*)m_pOwner)->TenthsToLogical(aGlyphsInfo[m_nGlyph].SelRectShift);
	m_uBoundsBottom.x = m_uBoundsTop.x + uWidth;
	m_uBoundsBottom.y = m_uBoundsTop.y + ((lmStaffObj*)m_pOwner)->TenthsToLogical(aGlyphsInfo[m_nGlyph].SelRectHeight);

    // store selection rectangle position and size
	m_uSelRect = GetBounds();
}

void lmShapeGlyph::Render(lmPaper* pPaper, wxColour color)
{
    wxString sGlyph( aGlyphsInfo[m_nGlyph].GlyphChar );

    pPaper->SetFont(*m_pFont);
    pPaper->SetTextForeground(color);
    pPaper->DrawText(sGlyph, m_uGlyphPos.x, m_uGlyphPos.y);

    lmShape::RenderCommon(pPaper);
}

void lmShapeGlyph::SetFont(wxFont *pFont)
{
    m_pFont = pFont;
}

wxString lmShapeGlyph::Dump(int nIndent)
{
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump += wxString::Format(_T("%04d %s: pos=(%.2f,%.2f), "),
        m_nId, m_sGMOName.c_str(), m_uGlyphPos.x, m_uGlyphPos.y);
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

wxBitmap* lmShapeGlyph::OnBeginDrag(double rScale)
{
	// A dragging operation is started. The view invokes this method to request the
	// bitmap to be used as drag image. No other action is required.
	// If no bitmap is returned drag is cancelled.
	//
	// So this method returns the bitmap to use with the drag image.

    wxString sGlyph( aGlyphsInfo[m_nGlyph].GlyphChar );

	// Get size of glyph, in logical units
    wxCoord wText, hText;
    wxScreenDC dc;
    dc.SetMapMode(lmDC_MODE);
    dc.SetUserScale(rScale, rScale);
    dc.SetFont(*m_pFont);
    dc.GetTextExtent(sGlyph, &wText, &hText);
    dc.SetFont(wxNullFont);

    // allocate a memory DC for drawing into a bitmap
    wxMemoryDC dc2;
    dc2.SetMapMode(lmDC_MODE);
    dc2.SetUserScale(rScale, rScale);
    dc2.SetFont(*m_pFont);

    // allocate the bitmap
    // convert size to pixels
    int wD = (int)dc2.LogicalToDeviceXRel(wText);
    int hD = (int)dc2.LogicalToDeviceYRel(hText);
    wxBitmap bitmap(wD+2, hD+2);
    dc2.SelectObject(bitmap);

    // draw onto the bitmap
    dc2.SetBackground(* wxWHITE_BRUSH);
    dc2.Clear();
    dc2.SetBackgroundMode(wxTRANSPARENT);
    dc2.SetTextForeground(g_pColors->ScoreSelected());
    dc2.DrawText(sGlyph, 0, 0);

    dc2.SelectObject(wxNullBitmap);

    //cut out the image, to discard the outside out of the bounding box
    lmPixels vxLeft = dc2.LogicalToDeviceYRel((wxCoord)(GetXLeft() - m_uGlyphPos.x));
    lmPixels vyTop = dc2.LogicalToDeviceYRel((wxCoord)(GetYTop() - m_uGlyphPos.y));
    lmPixels vWidth = wxMin(bitmap.GetWidth() - vxLeft,
                            dc2.LogicalToDeviceXRel((wxCoord)GetWidth()) );
    lmPixels vHeight = wxMin(bitmap.GetHeight() - vyTop,
                             dc2.LogicalToDeviceYRel((wxCoord)GetHeight()) );
    const wxRect rect(vxLeft, vyTop, vWidth, vHeight);
    //wxLogMessage(_T("[lmShapeGlyph::OnBeginDrag] bitmap size w=%d, h=%d. Cut x=%d, y=%d, w=%d, h=%d"),
    //    bitmap.GetWidth(), bitmap.GetHeight(), vxLeft, vyTop, vWidth, vHeight);
    wxBitmap oShapeBitmap = bitmap.GetSubBitmap(rect);
    wxASSERT(oShapeBitmap.IsOk());

    // Make the bitmap masked
    wxImage image = oShapeBitmap.ConvertToImage();
    image.SetMaskColour(255, 255, 255);
    wxBitmap* pBitmap = new wxBitmap(image);

    ////DBG -----------
    //wxString sFileName = _T("ShapeGlyp2.bmp");
    //pBitmap->SaveFile(sFileName, wxBITMAP_TYPE_BMP);
    ////END DBG -------

    return pBitmap;
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

void lmShapeGlyph::OnEndDrag(lmController* pCanvas, const lmUPoint& uPos)
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


//========================================================================================
// lmShapeText object implementation
//========================================================================================

lmShapeText::lmShapeText(lmScoreObj* pOwner, wxString sText, wxFont* pFont, lmPaper* pPaper,
						 lmUPoint offset, wxString sName, bool fDraggable, wxColour color)
    : lmSimpleShape(eGMO_ShapeText, pOwner, sName, fDraggable, color)
{
    m_sText = sText;
    m_pFont = pFont;

    // compute and store position
    m_uPos.x = offset.x;
    m_uPos.y = offset.y;

    // store boundling rectangle position and size
    lmLUnits uWidth, uHeight;
    pPaper->SetFont(*m_pFont);
    pPaper->GetTextExtent(m_sText, &uWidth, &uHeight);

    m_uBoundsTop.x = offset.x;
    m_uBoundsTop.y = offset.y;
    m_uBoundsBottom.x = m_uBoundsTop.x + uWidth;
    m_uBoundsBottom.y = m_uBoundsTop.y + uHeight;

    // store selection rectangle position and size
	m_uSelRect = GetBounds();

}


void lmShapeText::Render(lmPaper* pPaper, wxColour color)
{
    pPaper->SetFont(*m_pFont);
    pPaper->SetTextForeground(color);
    pPaper->DrawText(m_sText, m_uPos.x, m_uPos.y);

    lmShape::RenderCommon(pPaper);
}

void lmShapeText::SetFont(wxFont *pFont)
{
    m_pFont = pFont;
}

wxString lmShapeText::Dump(int nIndent)
{
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
    sDump += wxString::Format(_T("TextShape: pos=(%.2f,%.2f), text=%s, "),
        m_uPos.x, m_uPos.y, m_sText.c_str() );
    sDump += DumpBounds();
    sDump += _T("\n");

	return sDump;
}

void lmShapeText::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
    m_uPos.x += xIncr;
    m_uPos.y += yIncr;

    ShiftBoundsAndSelRec(xIncr, yIncr);

	//if included in a composite shape update parent bounding and selection rectangles
	if (this->IsChildShape())
		((lmCompositeShape*)GetParentShape())->RecomputeBounds();
}

wxBitmap* lmShapeText::OnBeginDrag(double rScale)
{
	// A dragging operation is started. The view invokes this method to request the
	// bitmap to be used as drag image. No other action is required.
	// If no bitmap is returned drag is cancelled.
	//
	// So this method returns the bitmap to use with the drag image.


	// Get size of text, in logical units
    wxCoord wText, hText;
    wxScreenDC dc;
    dc.SetMapMode(lmDC_MODE);
    dc.SetUserScale(rScale, rScale);
    dc.SetFont(*m_pFont);
    dc.GetTextExtent(m_sText, &wText, &hText);
    dc.SetFont(wxNullFont);

    // allocate a memory DC for drawing into a bitmap
    wxMemoryDC dc2;
    dc2.SetMapMode(lmDC_MODE);
    dc2.SetUserScale(rScale, rScale);
    dc2.SetFont(*m_pFont);

    // allocate the bitmap
    // convert size to pixels
    int wD = (int)dc2.LogicalToDeviceXRel(wText);
    int hD = (int)dc2.LogicalToDeviceYRel(hText);
    wxBitmap bitmap(wD+2, hD+2);
    dc2.SelectObject(bitmap);

    // draw onto the bitmap
    dc2.SetBackground(* wxWHITE_BRUSH);
    dc2.Clear();
    dc2.SetBackgroundMode(wxTRANSPARENT);
    dc2.SetTextForeground(g_pColors->ScoreSelected());
    dc2.DrawText(m_sText, 0, 0);


    dc2.SelectObject(wxNullBitmap);

 //   //cut out the image, to discard the outside out of the bounding box
 //   lmPixels vxLeft = dc2.LogicalToDeviceYRel(GetXLeft() - m_uGlyphPos.x);
 //   lmPixels vyTop = dc2.LogicalToDeviceYRel(GetYTop() - m_uGlyphPos.y);
 //   lmPixels vWidth = wxMin(bitmap.GetWidth() - vxLeft,
 //                           dc2.LogicalToDeviceXRel(GetWidth()) );
 //   lmPixels vHeight = wxMin(bitmap.GetHeight() - vyTop,
 //                            dc2.LogicalToDeviceYRel(GetHeight()) );
 //   const wxRect rect(vxLeft, vyTop, vWidth, vHeight);
 //   //wxLogMessage(_T("[lmShapeGlyph::OnBeginDrag] bitmap size w=%d, h=%d. Cut x=%d, y=%d, w=%d, h=%d"),
 //   //    bitmap.GetWidth(), bitmap.GetHeight(), vxLeft, vyTop, vWidth, vHeight);
 //   wxBitmap oShapeBitmap = bitmap.GetSubBitmap(rect);
 //   wxASSERT(oShapeBitmap.IsOk());

    // Make the bitmap masked
    //wxImage image = oShapeBitmap.ConvertToImage();
    wxImage image = bitmap.ConvertToImage();
    image.SetMaskColour(255, 255, 255);
    wxBitmap* pBitmap = new wxBitmap(image);

 //   ////DBG -----------
 //   //wxString sFileName = _T("ShapeGlyp2.bmp");
 //   //pBitmap->SaveFile(sFileName, wxBITMAP_TYPE_BMP);
 //   ////END DBG -------

    return pBitmap;

}




//========================================================================================
// lmShapeStem object implementation: a vertical line
//========================================================================================

lmShapeStem::lmShapeStem(lmScoreObj* pOwner, lmLUnits xPos, lmLUnits yStart,
						 lmLUnits yEnd, bool fStemDown,
						 lmLUnits uWidth, wxColour nColor)
	: lmShapeLine(pOwner, xPos, yStart, xPos, yEnd, uWidth, 0.0, nColor,
				  _T("Stem"), eEdgeHorizontal)
{
	m_fStemDown = fStemDown;
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


//========================================================================================
// lmShapeClef
//========================================================================================

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

void lmShapeClef::OnEndDrag(lmController* pCanvas, const lmUPoint& uPos)
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

lmShapeInvisible::lmShapeInvisible(lmScoreObj* pOwner, lmUPoint uPos, lmUSize uSize,
                                   wxString sName)
	: lmSimpleShape(eGMO_ShapeInvisible, pOwner, sName, lmNO_DRAGGABLE)
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

