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
#pragma implementation "GraphicObj.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "defs.h"
#include "GraphicObj.h"
#include "Score.h"
#include "../graphic/GMObject.h"


//implementation of the lmGraphicObj List
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(GraphicObjsList);


//========================================================================================
// lmGraphicObj object implementation
//========================================================================================

lmGraphicObj::lmGraphicObj(lmScoreObj* pOwner, bool fIsDraggable)
    : lmScoreObj(pOwner, eSCOT_GraphicObj, fIsDraggable) 
{
}

void lmGraphicObj::Layout(lmBox* pBox, lmPaper* pPaper, wxColour colorC, bool fHighlight)
{
	WXUNUSED(pBox);
	Draw(DO_MEASURE, pPaper, colorC, fHighlight);
}

void lmGraphicObj::Draw(bool fMeasuring, lmPaper* pPaper, wxColour colorC, bool fHighlight)
{
    wxASSERT(fMeasuring == DO_DRAW);    //For lmGraphicObj there is no measuring phase

    //get parent paper pos.
    wxRealPoint rOrg = ((lmScoreObj*)m_pParent)->GetOrigin();
    pPaper->SetCursorX(rOrg.x);
    pPaper->SetCursorY(rOrg.y);

    // ask derived object to draw itself
    if (IsShapeRendered()) {
        m_pShape->Render(pPaper, rOrg, colorC);
    }
    else {
        DrawObject(fMeasuring, pPaper, colorC, fHighlight);
    }

    // draw selection rectangle
    if (gfDrawSelRec) DrawSelRectangle(pPaper, g_pColors->ScoreSelected());
            
}

void lmGraphicObj::DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC, bool fHighlight)
{
    wxASSERT(fMeasuring == DO_DRAW);    //For lmGraphicObj there is no measuring phase
    wxASSERT(false);    //all GraphicObjs are shape rendered. So it should not arrive here.

}


//========================================================================================
// lmGOLine object implementation
//========================================================================================

lmGOLine::lmGOLine(lmScoreObj* pOwner,
                   lmTenths xStart, lmTenths yStart, 
                   lmTenths xEnd, lmTenths yEnd,
                   lmTenths nWidth, wxColour nColor)
    : lmGraphicObj(pOwner, lmDRAGGABLE) 
{
    // convert data to logical units
    lmLUnits x1, x2, y1, y2, uWidth;
    if (pOwner->GetType() == eSCOT_StaffObj) {
        lmStaffObj* pSO = (lmStaffObj*)pOwner;
        x1 = pSO->TenthsToLogical(xStart);
        y1 = pSO->TenthsToLogical(yStart);
        x2 = pSO->TenthsToLogical(xEnd);
        y2 = pSO->TenthsToLogical(yEnd);
        uWidth = pSO->TenthsToLogical(nWidth);
    }
    else {
        //@todo
        // Owner is an StaffObj or an AuxObj. For StaffObj we can convert to logical units
        // but, how to do it for AuxObjs?
        wxASSERT(false);
    }

    SetShape( new lmShapeLine(this, x1, y1, x2, y2, uWidth, nColor) );
    SetShapeRendered(true);         //transitional

}

wxString lmGOLine::Dump()
{
    return _T("GraphLine");
}

wxBitmap* lmGOLine::GetBitmap(double rScale)
{
    return (wxBitmap*)NULL;
}


