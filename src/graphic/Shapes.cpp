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


//========================================================================================
// lmShapeLine object implementation
//========================================================================================

lmShapeLine::lmShapeLine(lmObject* pOwner,
                lmLUnits xStart, lmLUnits yStart,
                lmLUnits xEnd, lmLUnits yEnd, lmLUnits uWidth, wxColour nColor)
    : lmSimpleShape(eGMO_ShapeLine, pOwner)
{
    m_xStart = xStart;
    m_yStart = yStart;
    m_xEnd = xEnd;
    m_yEnd = yEnd;
    m_color = nColor;
    m_uWidth = uWidth;
}

void lmShapeLine::Render(lmPaper* pPaper, wxColour color)
{
    lmUPoint uPos(0.0, 0.0);    //by-pass new version with absolute positions

    // start and end points
    lmLUnits x1 = uPos.x + m_xStart;
    lmLUnits y1 = uPos.y + m_yStart;
    lmLUnits x2 = uPos.x + m_xEnd;
    lmLUnits y2 = uPos.y + m_yEnd;

    pPaper->SolidLine(x1, y1, x2, y2, m_uWidth, eEdgeNormal, color);

    lmShape::RenderCommon(pPaper);

}

wxString lmShapeLine::Dump(int nIndent)
{
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump += wxString::Format(_T("lmShapeLine: start=(%.2f, %.2f), end=(%.2f, %.2f), line width=%.2f, "),
                m_xStart, m_yStart, m_xEnd, m_yEnd, m_uWidth );
    sDump += DumpBounds();
    sDump += _T("\n");
	return sDump;
}

void lmShapeLine::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
	//TODO
}




//========================================================================================
// lmShapeLin2 object implementation
//========================================================================================

lmShapeLin2::lmShapeLin2(lmObject* pOwner, lmLUnits xStart, lmLUnits yStart,
                lmLUnits xEnd, lmLUnits yEnd, lmLUnits uWidth, lmLUnits uBoundsExtraWidth,
				wxColour nColor, wxString sName, lmELineEdges nEdge)
    : lmSimpleShape(eGMO_ShapeLine, pOwner, sName)
{
	Create(xStart, yStart, xEnd, yEnd, uWidth, uBoundsExtraWidth, nColor, nEdge);
}

void lmShapeLin2::Create(lmLUnits xStart, lmLUnits yStart,
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

void lmShapeLin2::Render(lmPaper* pPaper, wxColour color)
{
    pPaper->SolidLine(m_xStart, m_yStart, m_xEnd, m_yEnd, m_uWidth, m_nEdge, color);
    lmShape::RenderCommon(pPaper);
}

wxString lmShapeLin2::Dump(int nIndent)
{
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump += wxString::Format(_T("%04d %s: start=(%.2f, %.2f), end=(%.2f, %.2f), line width=%.2f, "),
                m_nId, m_sShapeName, m_xStart, m_yStart, m_xEnd, m_yEnd, m_uWidth );
    sDump += DumpBounds();
    sDump += _T("\n");
	return sDump;
}

void lmShapeLin2::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
    m_xStart += xIncr;
    m_yStart += yIncr;
    m_xEnd += xIncr;
    m_yEnd += yIncr;

    ShiftBoundsAndSelRec(xIncr, yIncr);
}


//========================================================================================
// lmShapeGlyph object implementation
//========================================================================================

lmShapeGlyph::lmShapeGlyph(lmObject* pOwner, int nGlyph, wxFont* pFont)
    : lmSimpleShape(eGMO_ShapeGlyph, pOwner)
{
    m_nGlyph = nGlyph;
    m_pFont = pFont;

    //default values
    m_uShift.x = 0;
    m_uShift.y = 0;



}

lmShapeGlyph::lmShapeGlyph(lmStaffObj* pOwner, int nGlyph, wxFont* pFont, lmPaper* pPaper,
                           lmUPoint offset)
    : lmSimpleShape(eGMO_ShapeGlyph, pOwner)
{
    m_nGlyph = nGlyph;
    m_pFont = pFont;

    // compute and store position
    lmVStaff* pVStaff = pOwner->GetVStaff();
    int nStaff = pOwner->GetStaffNum();
    m_uShift.x = offset.x;
    m_uShift.y = offset.y - pVStaff->TenthsToLogical(aGlyphsInfo[m_nGlyph].GlyphOffset, nStaff);

    // store boundling rectangle position and size
    lmLUnits uWidth, uHeight;
    wxString sGlyph( aGlyphsInfo[m_nGlyph].GlyphChar );
    pPaper->SetFont(*m_pFont);
    pPaper->GetTextExtent(sGlyph, &uWidth, &uHeight);

	m_uBoundsTop.x = m_uShift.x;
	m_uBoundsTop.y = m_uShift.y + pVStaff->TenthsToLogical(aGlyphsInfo[m_nGlyph].SelRectShift, nStaff);
	m_uBoundsBottom.x = m_uBoundsTop.x + uWidth;
	m_uBoundsBottom.y = m_uBoundsTop.y + pVStaff->TenthsToLogical(aGlyphsInfo[m_nGlyph].SelRectHeight, nStaff);

    // store selection rectangle position and size
	m_uSelRect = GetBounds();

}

void lmShapeGlyph::Measure(lmPaper* pPaper, lmStaff* pStaff, lmUPoint offset)
{
    // compute and store position
    m_uShift.x = offset.x;
    m_uShift.y = offset.y - pStaff->TenthsToLogical(aGlyphsInfo[m_nGlyph].GlyphOffset);

    // store boundling rectangle position and size
    lmLUnits uWidth, uHeight;
    wxString sGlyph( aGlyphsInfo[m_nGlyph].GlyphChar );
    pPaper->SetFont(*m_pFont);
    pPaper->GetTextExtent(sGlyph, &uWidth, &uHeight);

	m_uBoundsTop.x = m_uShift.x;
	m_uBoundsTop.y = m_uShift.y + pStaff->TenthsToLogical(aGlyphsInfo[m_nGlyph].SelRectShift);
	m_uBoundsBottom.x = m_uBoundsTop.x + uWidth;
	m_uBoundsBottom.y = m_uBoundsTop.y + pStaff->TenthsToLogical(aGlyphsInfo[m_nGlyph].SelRectHeight);

    // store selection rectangle position and size
	m_uSelRect = GetBounds();

}


void lmShapeGlyph::Render(lmPaper* pPaper, wxColour color)
{
    lmUPoint uPos(0.0, 0.0);    //by-pass new version with absolute positions

    wxString sGlyph( aGlyphsInfo[m_nGlyph].GlyphChar );

    pPaper->SetFont(*m_pFont);
    pPaper->SetTextForeground(color);
    pPaper->DrawText(sGlyph, uPos.x + m_uShift.x, uPos.y + m_uShift.y);

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
	sDump += wxString::Format(_T("lmShapeGlyph: shift=(%d,%d), "),
        m_uShift.x, m_uShift.y);
    sDump += DumpBounds();
    sDump += _T("\n");
	return sDump;
}

void lmShapeGlyph::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
    m_uShift.x += xIncr;
    m_uShift.y += yIncr;

    ShiftBoundsAndSelRec(xIncr, yIncr);
}




//========================================================================================
// lmShapeGlyp2 object implementation
//========================================================================================

lmShapeGlyp2::lmShapeGlyp2(lmObject* pOwner, int nGlyph, wxFont* pFont, lmPaper* pPaper,
                           lmUPoint uPos, wxString sName, bool fDraggable)
    : lmSimpleShape(eGMO_ShapeGlyph, pOwner, sName, fDraggable)
{
    m_nGlyph = nGlyph;
    m_pFont = pFont;

    // compute and store position
    lmVStaff* pVStaff = ((lmStaffObj*)pOwner)->GetVStaff();
    int nStaff = ((lmStaffObj*)pOwner)->GetStaffNum();
    m_uGlyphPos.x = uPos.x;
    m_uGlyphPos.y = uPos.y; // - pVStaff->TenthsToLogical(aGlyphsInfo[m_nGlyph].GlyphOffset, nStaff);

    // store boundling rectangle position and size
    lmLUnits uWidth, uHeight;
    wxString sGlyph( aGlyphsInfo[m_nGlyph].GlyphChar );
    pPaper->SetFont(*m_pFont);
    pPaper->GetTextExtent(sGlyph, &uWidth, &uHeight);

	m_uBoundsTop.x = m_uGlyphPos.x;
	m_uBoundsTop.y = m_uGlyphPos.y + pVStaff->TenthsToLogical(aGlyphsInfo[m_nGlyph].SelRectShift, nStaff);
	m_uBoundsBottom.x = m_uBoundsTop.x + uWidth;
	m_uBoundsBottom.y = m_uBoundsTop.y + pVStaff->TenthsToLogical(aGlyphsInfo[m_nGlyph].SelRectHeight, nStaff);

    // store selection rectangle position and size
	m_uSelRect = GetBounds();

}

void lmShapeGlyp2::Render(lmPaper* pPaper, wxColour color)
{
    wxString sGlyph( aGlyphsInfo[m_nGlyph].GlyphChar );

    pPaper->SetFont(*m_pFont);
    pPaper->SetTextForeground(color);
    pPaper->DrawText(sGlyph, m_uGlyphPos.x, m_uGlyphPos.y);

    lmShape::RenderCommon(pPaper);

}

void lmShapeGlyp2::SetFont(wxFont *pFont)
{
    m_pFont = pFont;
}

wxString lmShapeGlyp2::Dump(int nIndent)
{
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump += wxString::Format(_T("%04d %s: pos=(%.2f,%.2f), "),
        m_nId, m_sShapeName, m_uGlyphPos.x, m_uGlyphPos.y);
    sDump += DumpBounds();
    sDump += _T("\n");
	return sDump;
}

void lmShapeGlyp2::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
    m_uGlyphPos.x += xIncr;
    m_uGlyphPos.y += yIncr;

    ShiftBoundsAndSelRec(xIncr, yIncr);
}

wxBitmap* lmShapeGlyp2::OnBeginDrag(double rScale)
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
    wxCoord wD = dc2.LogicalToDeviceXRel(wText),
            hD = dc2.LogicalToDeviceYRel(hText);
    // GetTextExtent has not enough precision. Add a couple of pixels for security
    wxBitmap bitmap((int)(wD+2), (int)(hD+2));
    dc2.SelectObject(bitmap);

    // draw onto the bitmap
    dc2.SetBackground(* wxWHITE_BRUSH);
    dc2.Clear();
    dc2.SetBackgroundMode(wxTRANSPARENT);
    dc2.SetTextForeground(g_pColors->ScoreSelected());
    dc2.DrawText(sGlyph, 0, 0);

    dc2.SelectObject(wxNullBitmap);

    // Make the bitmap masked
    wxImage image = bitmap.ConvertToImage();
    image.SetMaskColour(255, 255, 255);
    wxBitmap* pBitmap = new wxBitmap(image);

    return pBitmap;
}

lmUPoint lmShapeGlyp2::OnDrag(lmPaper* pPaper, const lmUPoint& uPos)
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

lmUPoint lmShapeGlyp2::GetObjectOrigin()
{
	//returns the origin of this shape
	return m_uGlyphPos;
}


//========================================================================================
// lmShapeText object implementation
//========================================================================================

lmShapeText::lmShapeText(lmObject* pOwner, wxString sText, wxFont* pFont)
    : lmSimpleShape(eGMO_ShapeText, pOwner)
{
    m_sText = sText;
    m_pFont = pFont;

    //default values
    m_uShift.x = 0;
    m_uShift.y = 0;



}

void lmShapeText::Measure(lmPaper* pPaper, lmStaff* pStaff, lmUPoint offset)
{
    // compute and store position
    m_uShift.x = offset.x;
    m_uShift.y = offset.y;

    // store boundling rectangle position and size
    lmLUnits uWidth, uHeight;
    pPaper->SetFont(*m_pFont);
    pPaper->GetTextExtent(m_sText, &uWidth, &uHeight);

    m_uBoundsTop.x = m_uShift.x;
    m_uBoundsTop.y = m_uShift.y;
    m_uBoundsBottom.x = m_uBoundsTop.x + uWidth;
    m_uBoundsBottom.y = m_uBoundsTop.y + uHeight;

    // store selection rectangle position and size
	m_uSelRect = GetBounds();

}


void lmShapeText::Render(lmPaper* pPaper, wxColour color)
{
    pPaper->SetFont(*m_pFont);
    pPaper->SetTextForeground(color);
    pPaper->DrawText(m_sText, m_uShift.x, m_uShift.y);

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
    sDump += wxString::Format(_T("TextShape: shift=(%d,%d), text=%s, "),
        m_uShift.x, m_uShift.y, m_sText.c_str() );
    sDump += DumpBounds();
    sDump += _T("\n");
	return sDump;
}

void lmShapeText::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
    m_uShift.x += xIncr;
    m_uSelRect.x += xIncr;
	m_uBoundsTop.x += xIncr;
	m_uBoundsBottom.x += xIncr;
}




//========================================================================================
// lmShapeTex2 object implementation
//========================================================================================

lmShapeTex2::lmShapeTex2(lmObject* pOwner, wxString sText, wxFont* pFont, lmPaper* pPaper,
						 lmUPoint offset, wxString sName, bool fDraggable)
    : lmSimpleShape(eGMO_ShapeText, pOwner, sName, fDraggable)
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


void lmShapeTex2::Render(lmPaper* pPaper, wxColour color)
{
    pPaper->SetFont(*m_pFont);
    pPaper->SetTextForeground(color);
    pPaper->DrawText(m_sText, m_uPos.x, m_uPos.y);

    lmShape::RenderCommon(pPaper);
}

void lmShapeTex2::SetFont(wxFont *pFont)
{
    m_pFont = pFont;
}

wxString lmShapeTex2::Dump(int nIndent)
{
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
    sDump += wxString::Format(_T("TextShape: pos=(%.2f,%.2f), text=%s, "),
        m_uPos.x, m_uPos.y, m_sText.c_str() );
    sDump += DumpBounds();
    sDump += _T("\n");
	return sDump;
}

void lmShapeTex2::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
    m_uPos.x += xIncr;
    m_uPos.y += yIncr;

    ShiftBoundsAndSelRec(xIncr, yIncr);
}




//========================================================================================
// lmShapeStem object implementation: a vertical line
//========================================================================================

lmShapeStem::lmShapeStem(lmObject* pOwner, lmLUnits xStart, lmLUnits yStart,
						 lmLUnits xEnd, lmLUnits yEnd, bool fStemDown,
						 lmLUnits uWidth, wxColour nColor)
	: lmShapeLin2(pOwner, xStart, yStart, xEnd, yEnd, uWidth, 0.0, nColor,
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
    return lmUPoint(uPos.x, m_uGlyphPos.y);

}

void lmShapeClef::OnEndDrag(wxCommandProcessor* pCP, const lmUPoint& uPos)
{
	// End drag. Receives the command processor associated to the view and the
	// final position of the object (logical units referred to page origin).

	if (g_fFreeMove)
		Shift(uPos.x - m_uGlyphPos.x, uPos.y - m_uGlyphPos.y);
	else
	{
		//only x position can be changed
		Shift(uPos.x - m_uGlyphPos.x, 0.0);
	}


}
