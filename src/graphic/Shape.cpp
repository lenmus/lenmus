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
#pragma implementation "Shape.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "Shape.h"
#include "../score/Glyph.h"      //access to glyphs table
#include "../score/Score.h"



//implementation of the lmShapeObj List
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(ShapesList);


//========================================================================================
// lmShapeObj object implementation
//========================================================================================

lmShapeObj::lmShapeObj(lmObject* pOwner)
{
    m_pOwner = pOwner;
    m_SelRect = wxRect(0,0,0,0);
    m_BoundsRect = wxRect(0,0,0,0);
}

void lmShapeObj::SetSelRectangle(int x, int y, int nWidth, int nHeight)
{
    m_SelRect.width = nWidth;
    m_SelRect.height = nHeight;
    m_SelRect.x = x;
    m_SelRect.y = y;
}

void lmShapeObj::DrawSelRectangle(lmPaper* pPaper, lmUPoint pos, wxColour colorC)
{
    wxPoint pt = m_SelRect.GetPosition();
    lmUPoint uPoint((lmLUnits)pt.x, (lmLUnits)pt.y);
    pPaper->SketchRectangle(uPoint + pos, m_SelRect.GetSize(), colorC);
}

bool lmShapeObj::Collision(lmShapeObj* pShape)
{
    wxRect rect1 = GetBoundsRectangle();
    return rect1.Intersects( pShape->GetBoundsRectangle() );
}


//========================================================================================
// lmShapeSimple object implementation
//========================================================================================

lmShapeSimple::lmShapeSimple(lmObject* pOwner) : lmShapeObj(pOwner)
{

}



//========================================================================================
// lmShapeComposite object implementation
//========================================================================================
lmShapeComposite::lmShapeComposite(lmObject* pOwner) : lmShapeObj(pOwner)
{
    //default values
    m_fGrouped = false;


}

lmShapeComposite::~lmShapeComposite()
{
    m_Components.DeleteContents(true);
    m_Components.Clear();
}

void lmShapeComposite::Add(lmShapeObj* pShape)
{
    m_Components.Append(pShape);

    //compute new selection rectangle by union of individual selection rectangles
    m_SelRect.Union(pShape->GetSelRectangle());

    //! @todo add boundling rectangle to bounds rectangle list
    m_BoundsRect = m_SelRect;

}

void lmShapeComposite::Render(lmPaper* pPaper, lmUPoint pos, wxColour color)
{
    lmShapeObj* pShape;
    ShapesList::Node* pNode = m_Components.GetFirst();
    while (pNode) {
        pShape = (lmShapeObj*)pNode->GetData();
        pShape->Render(pPaper, pos, color);
        pNode = pNode->GetNext();
    }
}

void lmShapeComposite::Shift(lmLUnits xIncr)
{
    lmShapeObj* pShape;
    ShapesList::Node* pNode = m_Components.GetFirst();
    while (pNode) {
        pShape = (lmShapeObj*)pNode->GetData();
        pShape->Shift(xIncr);
        pNode = pNode->GetNext();
    }
}


//========================================================================================
// lmShapeLine object implementation
//========================================================================================

lmShapeLine::lmShapeLine(lmObject* pOwner,
                lmLUnits xStart, lmLUnits yStart,
                lmLUnits xEnd, lmLUnits yEnd, lmLUnits uWidth, wxColour nColor)
    : lmShapeSimple(pOwner)
{
    m_xStart = xStart;
    m_yStart = yStart;
    m_xEnd = xEnd;
    m_yEnd = yEnd;
    m_color = nColor;
    m_uWidth = uWidth;
}

void lmShapeLine::Render(lmPaper* pPaper, lmUPoint pos, wxColour color)
{
    // start and end points
    lmLUnits x1 = pos.x + m_xStart;
    lmLUnits y1 = pos.y + m_yStart;
    lmLUnits x2 = pos.x + m_xEnd;
    lmLUnits y2 = pos.y + m_yEnd;

    pPaper->SolidLine(x1, y1, x2, y2, m_uWidth, eEdgeNormal, color);

}

wxString lmShapeLine::Dump()
{
    return _T("LineShape");
}

void lmShapeLine::Shift(lmLUnits xIncr)
{
}


//========================================================================================
// lmShapeGlyph object implementation
//========================================================================================

lmShapeGlyph::lmShapeGlyph(lmObject* pOwner, int nGlyph, wxFont* pFont)
    : lmShapeSimple(pOwner)
{
    m_nGlyph = nGlyph;
    m_pFont = pFont;

    //default values
    m_shift.x = 0;
    m_shift.y = 0;



}

void lmShapeGlyph::Measure(lmPaper* pPaper, lmStaff* pStaff, lmUPoint offset)
{
    // compute and store position
    m_shift.x = offset.x;
    m_shift.y = offset.y - pStaff->TenthsToLogical(aGlyphsInfo[m_nGlyph].GlyphOffset);

    // store boundling rectangle position and size
    lmLUnits nWidth, nHeight;
    wxString sGlyph( aGlyphsInfo[m_nGlyph].GlyphChar );
    pPaper->SetFont(*m_pFont);
    pPaper->GetTextExtent(sGlyph, &nWidth, &nHeight);
    m_BoundsRect.height = pStaff->TenthsToLogical(aGlyphsInfo[m_nGlyph].SelRectHeight);
    m_BoundsRect.width = nWidth;
    m_BoundsRect.x = m_shift.x;
    m_BoundsRect.y = m_shift.y + pStaff->TenthsToLogical(aGlyphsInfo[m_nGlyph].SelRectShift);

    // store selection rectangle position and size
    m_SelRect = m_BoundsRect;

}


void lmShapeGlyph::Render(lmPaper* pPaper, lmUPoint pos, wxColour color)
{
    wxString sGlyph( aGlyphsInfo[m_nGlyph].GlyphChar );

    pPaper->SetFont(*m_pFont);
    pPaper->SetTextForeground(color);
    pPaper->DrawText(sGlyph, pos.x + m_shift.x, pos.y + m_shift.y);

}

void lmShapeGlyph::SetFont(wxFont *pFont)
{
    m_pFont = pFont;
}

wxString lmShapeGlyph::Dump()
{
    return wxString::Format(_T("GlyphShape: shift=(%d,%d)"),
        m_shift.x, m_shift.y);
}

void lmShapeGlyph::Shift(lmLUnits xIncr)
{
    m_shift.x += xIncr;
    m_SelRect.x += xIncr;
    m_BoundsRect.x += xIncr;
}



//========================================================================================
// lmShapeText object implementation
//========================================================================================

lmShapeText::lmShapeText(lmObject* pOwner, wxString sText, wxFont* pFont)
    : lmShapeSimple(pOwner)
{
    m_sText = sText;
    m_pFont = pFont;

    //default values
    m_shift.x = 0;
    m_shift.y = 0;



}

void lmShapeText::Measure(lmPaper* pPaper, lmStaff* pStaff, lmUPoint offset)
{
    // compute and store position
    m_shift.x = offset.x;
    m_shift.y = offset.y;

    // store boundling rectangle position and size
    lmLUnits uWidth, uHeight;
    pPaper->SetFont(*m_pFont);
    pPaper->GetTextExtent(m_sText, &uWidth, &uHeight);
    m_BoundsRect.height = uHeight;
    m_BoundsRect.width = uWidth;
    m_BoundsRect.x = m_shift.x;
    m_BoundsRect.y = m_shift.y;

    // store selection rectangle position and size
    m_SelRect = m_BoundsRect;

}


void lmShapeText::Render(lmPaper* pPaper, lmUPoint pos, wxColour color)
{
    pPaper->SetFont(*m_pFont);
    pPaper->SetTextForeground(color);
    pPaper->DrawText(m_sText, pos.x + m_shift.x, pos.y + m_shift.y);
}

void lmShapeText::SetFont(wxFont *pFont)
{
    m_pFont = pFont;
}

wxString lmShapeText::Dump()
{
    return wxString::Format(_T("TextShape: shift=(%d,%d), text=%s"),
        m_shift.x, m_shift.y, m_sText.c_str() );
}

void lmShapeText::Shift(lmLUnits xIncr)
{
    m_shift.x += xIncr;
    m_SelRect.x += xIncr;
    m_BoundsRect.x += xIncr;
}

