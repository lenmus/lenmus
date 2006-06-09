//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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
/*! @file Shape.cpp
    @brief Implementation file for class lmShape
    @ingroup graphic_management
*/
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

lmShapeObj::lmShapeObj(lmScoreObj* pOwner)
{
    m_pOwner = pOwner;

}

void lmShapeObj::SetSelRectangle(int x, int y, int nWidth, int nHeight)
{
    m_SelRect.width = nWidth;
    m_SelRect.height = nHeight;
    m_SelRect.x = x;
    m_SelRect.y = y;
}

void lmShapeObj::DrawSelRectangle(wxDC* pDC, wxPoint pos, wxColour colorC)
{
    wxPen oldPen = pDC->GetPen();
    wxPen pen(colorC, 1, wxSOLID);      //width = 1px
    pDC->SetPen(pen);
    pDC->SetBrush( *wxTRANSPARENT_BRUSH );
    pDC->DrawRectangle(m_SelRect.GetPosition()+pos, m_SelRect.GetSize());
    pDC->SetPen(oldPen);

}

//========================================================================================
// lmShapeSimple object implementation
//========================================================================================

lmShapeSimple::lmShapeSimple(lmScoreObj* pOwner) : lmShapeObj(pOwner)
{

}


wxRect lmShapeSimple::GetBoundsRect() const
{ 
    return m_BoundsRect;
}



//========================================================================================
// lmShapeComposite object implementation
//========================================================================================
lmShapeComposite::lmShapeComposite(lmScoreObj* pOwner) : lmShapeObj(pOwner)
{
    //default values
    m_fGrouped = false;

    m_SelRect = wxRect(0,0,0,0);

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

}

void lmShapeComposite::Render(wxDC* pDC, wxPoint pos, wxColour color)
{
    lmShapeObj* pShape;
    ShapesList::Node* pNode = m_Components.GetFirst();
    while (pNode) {
        pShape = (lmShapeObj*)pNode->GetData();
        pShape->Render(pDC, pos, color);
        pNode = pNode->GetNext();
    }
}



//========================================================================================
// lmShapeLine object implementation
//========================================================================================

lmShapeLine::lmShapeLine(lmScoreObj* pOwner, lmLUnits nLength, lmLUnits nWidth)
    : lmShapeSimple(pOwner)
{
    m_nLength = nLength;
    m_nWidth = nWidth;
}

void lmShapeLine::Render(wxDC* pDC, wxPoint pos, wxColour color)
{
    wxPen oldPen = pDC->GetPen();
    wxPen pen(color, m_nWidth, wxSOLID);
    pDC->SetPen(pen);

    // start and end points
    double x1 = pos.x;
    double y1 = pos.y;
    double x2 = x1 + (double)m_nLength;
    double y2 = y1 + (double)m_nLength;

    pDC->DrawLine(x1, y1, x2, y2);

    pDC->SetPen(oldPen);

}


//========================================================================================
// lmShapeGlyph object implementation
//========================================================================================

lmShapeGlyph::lmShapeGlyph(lmScoreObj* pOwner, int nGlyph, wxFont* pFont)
    : lmShapeSimple(pOwner)
{
    m_nGlyph = nGlyph;
    m_pFont = pFont;

    //default values
    m_shift.x = 0;
    m_shift.y = 0;



}

void lmShapeGlyph::Measure(wxDC* pDC, lmStaff* pStaff, wxPoint shift)
{
    // store positions
    m_shift.x = shift.x;
    m_shift.y = shift.y - pStaff->TenthsToLogical(aGlyphsInfo[m_nGlyph].GlyphOffset);

    // store boundling rectangle position and size
    lmLUnits nWidth, nHeight;
    wxString sGlyph( aGlyphsInfo[m_nGlyph].GlyphChar );
    pDC->GetTextExtent(sGlyph, &nWidth, &nHeight);
    m_BoundsRect.height = pStaff->TenthsToLogical(aGlyphsInfo[m_nGlyph].SelRectHeight);
    m_BoundsRect.width = nWidth;
    m_BoundsRect.x = m_shift.x;
    m_BoundsRect.y = m_shift.y + pStaff->TenthsToLogical(aGlyphsInfo[m_nGlyph].SelRectShift);

    // store selection rectangle position and size
    m_SelRect = m_BoundsRect;

}


void lmShapeGlyph::Render(wxDC* pDC, wxPoint pos, wxColour color)
{
    wxString sGlyph( aGlyphsInfo[m_nGlyph].GlyphChar );

    pDC->SetFont(*m_pFont);
    pDC->SetTextForeground(color);
    pDC->DrawText(sGlyph, pos.x + m_shift.x, pos.y + m_shift.y );

}

void lmShapeGlyph::SetFont(wxFont *pFont)
{
    m_pFont = pFont;
}

void lmShapeGlyph::SetShift(lmLUnits x, lmLUnits y)
{
    m_shift.x = x;
    m_shift.y = y;
}
