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
#pragma implementation "GraphObj.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "GraphObj.h"
#include "Score.h"



//implementation of the lmGraphObj List
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(ShapesList);


//========================================================================================
// lmGraphObj object implementation
//========================================================================================

lmGraphObj::lmGraphObj(lmScoreObj* pOwner)
{
    m_pOwner = pOwner;
    m_SelRect = wxRect(0,0,0,0);
    m_BoundsRect = wxRect(0,0,0,0);
}

void lmGraphObj::SetSelRectangle(int x, int y, int nWidth, int nHeight)
{
    m_SelRect.width = nWidth;
    m_SelRect.height = nHeight;
    m_SelRect.x = x;
    m_SelRect.y = y;
}

void lmGraphObj::DrawSelRectangle(lmPaper* pPaper, lmUPoint pos, wxColour colorC)
{
    wxPoint pt = m_SelRect.GetPosition();
    lmUPoint uPoint((lmLUnits)pt.x, (lmLUnits)pt.y);
    pPaper->SketchRectangle(uPoint + pos, m_SelRect.GetSize(), colorC);
}

bool lmGraphObj::Collision(lmGraphObj* pShape)
{
    wxRect rect1 = GetBoundsRectangle();
    return rect1.Intersects( pShape->GetBoundsRectangle() );
}


//========================================================================================
// lmGraphSimpleObj object implementation
//========================================================================================

lmGraphSimpleObj::lmGraphSimpleObj(lmScoreObj* pOwner) : lmGraphObj(pOwner)
{

}



//========================================================================================
// lmGraphCompositeObj object implementation
//========================================================================================
lmGraphCompositeObj::lmGraphCompositeObj(lmScoreObj* pOwner) : lmGraphObj(pOwner)
{
    //default values
    m_fGrouped = false;


}

lmGraphCompositeObj::~lmGraphCompositeObj()
{
    m_Components.DeleteContents(true);
    m_Components.Clear();
}

void lmGraphCompositeObj::Add(lmGraphObj* pShape)
{
    m_Components.Append(pShape);

    //compute new selection rectangle by union of individual selection rectangles
    m_SelRect.Union(pShape->GetSelRectangle());

    //! @todo add boundling rectangle to bounds rectangle list
    m_BoundsRect = m_SelRect;

}

void lmGraphCompositeObj::Render(lmPaper* pPaper, lmUPoint pos, wxColour color)
{
    lmGraphObj* pShape;
    ShapesList::Node* pNode = m_Components.GetFirst();
    while (pNode) {
        pShape = (lmGraphObj*)pNode->GetData();
        pShape->Render(pPaper, pos, color);
        pNode = pNode->GetNext();
    }
}

void lmGraphCompositeObj::Shift(lmLUnits xIncr)
{
    lmGraphObj* pShape;
    ShapesList::Node* pNode = m_Components.GetFirst();
    while (pNode) {
        pShape = (lmGraphObj*)pNode->GetData();
        pShape->Shift(xIncr);
        pNode = pNode->GetNext();
    }
}


//========================================================================================
// lmGraphLine object implementation
//========================================================================================

lmGraphLine::lmGraphLine(lmScoreObj* pOwner, lmLUnits uLength, lmLUnits uWidth)
    : lmGraphSimpleObj(pOwner)
{
    m_uLength = uLength;
    m_uWidth = uWidth;
}

void lmGraphLine::Render(lmPaper* pPaper, lmUPoint pos, wxColour color)
{
    // start and end points
    lmLUnits x1 = pos.x;
    lmLUnits y1 = pos.y;
    lmLUnits x2 = x1 + m_uLength;
    lmLUnits y2 = y1 + m_uLength;

    pPaper->SolidLine(x1, y1, x2, y2, m_uWidth, eEdgeNormal, color);

}

wxString lmGraphLine::Dump()
{
    return _T("LineShape");
}

void lmGraphLine::Shift(lmLUnits xIncr)
{
}


