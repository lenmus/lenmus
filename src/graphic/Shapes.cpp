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

void lmShapeLine::Render(lmPaper* pPaper, lmUPoint uPos, wxColour color)
{
    // start and end points
    lmLUnits x1 = uPos.x + m_xStart;
    lmLUnits y1 = uPos.y + m_yStart;
    lmLUnits x2 = uPos.x + m_xEnd;
    lmLUnits y2 = uPos.y + m_yEnd;

    pPaper->SolidLine(x1, y1, x2, y2, m_uWidth, eEdgeNormal, color);

}

wxString lmShapeLine::Dump()
{
    return _T("LineShape\n");
}

void lmShapeLine::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
	//TODO
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


void lmShapeGlyph::Render(lmPaper* pPaper, lmUPoint uPos, wxColour color)
{
    wxString sGlyph( aGlyphsInfo[m_nGlyph].GlyphChar );

    pPaper->SetFont(*m_pFont);
    pPaper->SetTextForeground(color);
    pPaper->DrawText(sGlyph, uPos.x + m_uShift.x, uPos.y + m_uShift.y);

}

void lmShapeGlyph::SetFont(wxFont *pFont)
{
    m_pFont = pFont;
}

wxString lmShapeGlyph::Dump()
{
    return wxString::Format(_T("GlyphShape: shift=(%d,%d)\n"),
        m_uShift.x, m_uShift.y);
}

void lmShapeGlyph::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
    m_uShift.x += xIncr;
    m_uShift.y += yIncr;

    m_uSelRect.x += xIncr;		//AWARE: As it is a rectangle, changing its origin does not
    m_uSelRect.y += yIncr;		//       change its width/height

	m_uBoundsTop.x += xIncr;
	m_uBoundsBottom.x += xIncr;
	m_uBoundsTop.y += yIncr;
	m_uBoundsBottom.y += yIncr;

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


void lmShapeText::Render(lmPaper* pPaper, lmUPoint uPos, wxColour color)
{
    pPaper->SetFont(*m_pFont);
    pPaper->SetTextForeground(color);
    pPaper->DrawText(m_sText, uPos.x + m_uShift.x, uPos.y + m_uShift.y);
}

void lmShapeText::SetFont(wxFont *pFont)
{
    m_pFont = pFont;
}

wxString lmShapeText::Dump()
{
    return wxString::Format(_T("TextShape: shift=(%d,%d), text=%s\n"),
        m_uShift.x, m_uShift.y, m_sText.c_str() );
}

void lmShapeText::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
    m_uShift.x += xIncr;
    m_uSelRect.x += xIncr;
	m_uBoundsTop.x += xIncr;
	m_uBoundsBottom.x += xIncr;
}

