// RCS-ID: $Id: StaffObj.cpp,v 1.3 2006/02/23 19:24:42 cecilios Exp $
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
/*! @file StaffObj.cpp    
    @brief Implementation file for classes lmScoreObj, lmAuxObj, lmStaffObj, lmSimpleObj and lmCompositeObj
    @ingroup score_kernel
*/
#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/image.h"
#include "Score.h"


//implementation of the StaffObjs List
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(StaffObjsList);

//implementation of the AuxObjs List
WX_DEFINE_LIST(AuxObjsList);

static wxInt32 m_IdCounter = 0;        //to assign unique IDs to ScoreObjs

//-------------------------------------------------------------------------------------------------
// lmScoreObj implementation
//-------------------------------------------------------------------------------------------------
lmScoreObj::lmScoreObj(EScoreObjType nType, bool fIsDraggable)
{
    m_nId = m_IdCounter++;        // give it an ID
    m_nType = nType;            // save type

    // behaviour
    m_fIsDraggable = fIsDraggable;

    // initializations: font related info
    m_glyphPos.x = 0;
    m_glyphPos.y = 0;
    m_pFont = (wxFont *)NULL;

    // initializations: positioning related info
    m_paperPos.y = 0;
    m_paperPos.x = 0;
    m_fFixedPos = false;

    // initializations: selection related info
    m_fSelected = false;


}

bool lmScoreObj::IsAtPoint(wxPoint& pt)
{
    wxRect rect(GetSelRect());
    return rect.Inside(pt.x, pt.y);
}

void lmScoreObj::DrawSelRectangle(lmPaper* pPaper, wxColour colorC)
{
    wxDC* pDC = pPaper->GetDC();
    pDC->SetPen( wxPen(colorC, 1, wxSOLID) );
    pDC->SetBrush( *wxTRANSPARENT_BRUSH );
    pDC->DrawRectangle(GetSelRect().GetPosition(), GetSelRect().GetSize());
}

//======================================================================================
// methods only for daggable objects
//======================================================================================

void lmScoreObj::MoveTo(wxPoint& pt)
{
    m_paperPos.y = pt.y;
    m_paperPos.x = pt.x;
}

void lmScoreObj::MoveDragImage(lmPaper* pPaper, wxDragImage* pDragImage, wxPoint& offsetD, 
                const wxPoint& pagePosL, const wxPoint& dragStartPosL, const wxPoint& canvasPosD)
{
    /*
     DragImage->Move() requires device units referred to canvas window. To compute the
     desired position the following parameters are received:
        canvasPosD:
            Current mouse position (device units referred to canvas window). If the 
            image movement is not constrained, this is the rigth value for DragImage->Move().
        offsetD:
            Offset to add when translating from logical units referred to page origin to 
            scrolled device units referred to canvas origin. It takes also into account the 
            offset introduced by the hotSpot point.
        pagePosL:
            Current mouse position (logical units referred to page origin).
        dragStartPosL:
            Mouse position (logical units) of the mouse point at which the dragging
            was started.
    */

    // this is the default behaviour: just move the image to the new position
    pDragImage->Move(canvasPosD);

    // and it is equivalent to work from logical units and doing all this:

//    wxPoint nShiftVector = pagePosL - dragStartPosL;        // the displacement
//    // as m_glyphPos is fixed, the displacement must be translated to paperPos
//    wxPoint newPaperPos = m_paperPos + nShiftVector;
//    // then the shape must be drawn at:
//    wxPoint ptNewD;
//    ptNewD.x = pPaper->LogicalToDeviceX(newPaperPos.x + m_glyphPos.x) + offsetD.x;
//    ptNewD.y = pPaper->LogicalToDeviceY(newPaperPos.y + m_glyphPos.y) + offsetD.y;
//    pDragImage->Move(ptNewD);

}

wxPoint lmScoreObj::EndDrag(const wxPoint& pos)
{
    wxPoint oldPos(m_paperPos + m_glyphPos);        // save current position for Undo command

    // move object to new position
    m_paperPos.x = pos.x - m_glyphPos.x;
    m_paperPos.y = pos.y - m_glyphPos.y;

    return oldPos;        //return old position
}

wxBitmap* lmScoreObj::PrepareBitMap(double rScale, const wxString sGlyph)
{
    // Get size of glyph, in logical units
    lmMicrons wL, hL;
    wxScreenDC dc;
    dc.SetMapMode(lmDC_MODE);
    dc.SetUserScale(rScale, rScale);
    dc.SetFont(*m_pFont);
    dc.GetTextExtent(sGlyph, &wL, &hL);
    dc.SetFont(wxNullFont);

    // allocate a memory DC for drawing into a bitmap
    wxMemoryDC dc2;
    dc2.SetMapMode(lmDC_MODE);
    dc2.SetUserScale(rScale, rScale);
    dc2.SetFont(*m_pFont);

    // allocate the bitmap
    // convert size to pixels
    wxCoord wD = dc2.LogicalToDeviceXRel(wL),
            hD = dc2.LogicalToDeviceYRel(hL);
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



//-------------------------------------------------------------------------------------------------
// lmAuxObj implementation
//-------------------------------------------------------------------------------------------------
lmAuxObj::lmAuxObj(EScoreObjType nType, bool fIsDraggable) :
    lmScoreObj(nType, fIsDraggable)
{
}
void lmAuxObj::Draw(bool fMeasuring, lmPaper* pPaper, wxColour colorC)
{
    wxASSERT(fMeasuring == DO_DRAW);    //For AuxObjs measuring phase is done by specific methods

    //restore paper pos.
    pPaper->SetCursorX(m_paperPos.x);
    pPaper->SetCursorY(m_paperPos.y);

    // ask derived object to draw itself
    DrawObject(fMeasuring, pPaper, colorC);

    // draw selection rectangle
    if (gfDrawSelRec) DrawSelRectangle(pPaper, g_pColors->ScoreSelected());

}


//-------------------------------------------------------------------------------------------------
// lmStaffObj implementation
//-------------------------------------------------------------------------------------------------

lmStaffObj::lmStaffObj(EScoreObjType nType, lmVStaff* pStaff, wxInt32 nStaff,
                   bool fVisible, bool fIsDraggable) :
    lmScoreObj(nType, fIsDraggable)
{
    // store parameters
    m_fVisible = fVisible;

    //default values
    m_nWidth = 0;

    // initializations: staff ownership info
    m_pVStaff = pStaff;
    m_nStaffNum = (pStaff ? nStaff : 0);

}

lmStaffObj::~lmStaffObj()
{
}

void lmStaffObj::Draw(bool fMeasuring, lmPaper* pPaper, wxColour colorC)
{
    if (!m_fVisible) return;
    
    if (fMeasuring && !m_fFixedPos) {
        m_paperPos.x = pPaper->GetCursorX();
        m_paperPos.y = pPaper->GetCursorY();
    } else {
        pPaper->SetCursorX(m_paperPos.x);
        pPaper->SetCursorY(m_paperPos.y);
    }

    // set the font
    if (fMeasuring) SetFont(pPaper);

    // ask derived object to draw itself
    DrawObject(fMeasuring, pPaper, colorC);

    // update paper cursor position
    pPaper->SetCursorX(m_paperPos.x + m_nWidth);
    
    // draw selection rectangle
    if (gfDrawSelRec && !fMeasuring) DrawSelRectangle(pPaper, g_pColors->ScoreSelected());
            
}

// default behaviour
void lmStaffObj::SetFont(lmPaper* pPaper)
{
    wxASSERT(m_pVStaff);
    wxASSERT(m_nStaffNum > 0);
    lmStaff* pStaff = m_pVStaff->GetStaff(m_nStaffNum);
    m_pFont = pStaff->GetFontDraw();
}





//-------------------------------------------------------------------------------------------------
// lmSimpleObj implementation
//-------------------------------------------------------------------------------------------------
lmSimpleObj::lmSimpleObj(EScoreObjType nType, lmVStaff* pStaff, wxInt32 nStaff,
             bool fVisible, bool fIsDraggable)
    : lmStaffObj(nType, pStaff, nStaff, fVisible, fIsDraggable)
{
}

//-------------------------------------------------------------------------------------------------
// lmCompositeObj implementation
//-------------------------------------------------------------------------------------------------
lmCompositeObj::lmCompositeObj(EScoreObjType nType, lmVStaff* pStaff, wxInt32 nStaff,
             bool fVisible, bool fIsDraggable)
    : lmStaffObj(nType, pStaff, nStaff, fVisible, fIsDraggable)
{
}




