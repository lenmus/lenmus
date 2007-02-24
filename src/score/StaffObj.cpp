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
#include "ObjOptions.h"


//implementation of the StaffObjs List
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(StaffObjsList);

//implementation of the AuxObjs List
WX_DEFINE_LIST(AuxObjsList);

static int m_IdCounter = 0;        //to assign unique IDs to ScoreObjs

//-------------------------------------------------------------------------------------------------
// lmScoreObj implementation
//-------------------------------------------------------------------------------------------------
lmScoreObj::lmScoreObj(lmObject* pParent, EScoreObjType nType, bool fIsDraggable) :
    lmObject(pParent)
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
    m_nNumPage = 1;

    // initializations: selection related info
    m_fSelected = false;

    //transitional
    SetShapeRendered(false);
    m_pShape = (lmShapeObj*)NULL;

}

lmScoreObj::~lmScoreObj()
{
    if (m_pShape) delete m_pShape;
}

bool lmScoreObj::IsAtPoint(lmUPoint& pt)
{
    wxRect rect(GetSelRect());
    return rect.Contains(pt.x, pt.y);
}

void lmScoreObj::DrawSelRectangle(lmPaper* pPaper, wxColour colorC)
{
    if (IsShapeRendered()) {
        m_pShape->DrawSelRectangle(pPaper, m_paperPos, colorC);
    }
    else {
        wxPoint pt = GetSelRect().GetPosition();
        lmUPoint uPoint((lmLUnits)pt.x, (lmLUnits)pt.y);
        pPaper->SketchRectangle(uPoint, GetSelRect().GetSize(), *wxRED);
        //! @todo change *wxRED by colorC when no longer necesary to distinguise
        //!       between shape rendered and drawing rendered objects
    }
}



//======================================================================================
// methods only for daggable objects
//======================================================================================

void lmScoreObj::MoveTo(lmUPoint& pt)
{
    m_paperPos.y = pt.y;
    m_paperPos.x = pt.x;
}

void lmScoreObj::MoveDragImage(lmPaper* pPaper, wxDragImage* pDragImage, lmDPoint& offsetD, 
                const lmUPoint& pagePosL, const lmUPoint& dragStartPosL, const lmDPoint& canvasPosD)
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

//    lmUPoint nShiftVector = pagePosL - dragStartPosL;        // the displacement
//    // as m_glyphPos is fixed, the displacement must be translated to paperPos
//    lmUPoint newPaperPos = m_paperPos + nShiftVector;
//    // then the shape must be drawn at:
//    lmDPoint ptNewD;
//    ptNewD.x = pPaper->LogicalToDeviceX(newPaperPos.x + m_glyphPos.x) + offsetD.x;
//    ptNewD.y = pPaper->LogicalToDeviceY(newPaperPos.y + m_glyphPos.y) + offsetD.y;
//    pDragImage->Move(ptNewD);

}

lmUPoint lmScoreObj::EndDrag(const lmUPoint& pos)
{
    lmUPoint oldPos(m_paperPos + m_glyphPos);        // save current position for Undo command

    // move object to new position
    m_paperPos.x = pos.x - m_glyphPos.x;
    m_paperPos.y = pos.y - m_glyphPos.y;

    return oldPos;        //return old position
}

wxBitmap* lmScoreObj::PrepareBitMap(double rScale, const wxString sGlyph)
{
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



//-------------------------------------------------------------------------------------------------
// lmAuxObj implementation
//-------------------------------------------------------------------------------------------------
lmAuxObj::lmAuxObj(lmObject* pParent, EScoreObjType nType, bool fIsDraggable) :
    lmScoreObj(pParent, nType, fIsDraggable)
{
}
void lmAuxObj::Draw(bool fMeasuring, lmPaper* pPaper, wxColour colorC, bool fHighlight)
{
    wxASSERT(fMeasuring == DO_DRAW);    //For AuxObjs measuring phase is done by specific methods

    //restore paper pos.
    pPaper->SetCursorX(m_paperPos.x);
    pPaper->SetCursorY(m_paperPos.y);

    // ask derived object to draw itself
    DrawObject(fMeasuring, pPaper, colorC, fHighlight);

    // draw selection rectangle
    if (gfDrawSelRec) DrawSelRectangle(pPaper, g_pColors->ScoreSelected());

}


//-------------------------------------------------------------------------------------------------
// lmStaffObj implementation
//-------------------------------------------------------------------------------------------------

lmStaffObj::lmStaffObj(lmObject* pParent, EScoreObjType nType, lmVStaff* pStaff, int nStaff,
                   bool fVisible, bool fIsDraggable) :
    lmScoreObj(pParent, nType, fIsDraggable)
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

void lmStaffObj::Draw(bool fMeasuring, lmPaper* pPaper, wxColour colorC, bool fHighlight)
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

    if (IsShapeRendered()) {
        if (fMeasuring) {
            // ask derived object to measure itself
            DrawObject(fMeasuring, pPaper, colorC, fHighlight);
        }
        else {
            m_pShape->Render(pPaper, m_paperPos, colorC);
        }
    }
    else {
        // ask derived object to draw itself
        DrawObject(fMeasuring, pPaper, colorC, fHighlight);
    }

    // update paper cursor position
    pPaper->SetCursorX(m_paperPos.x + m_nWidth);
    
    // draw selection rectangle
    if (gfDrawSelRec && !fMeasuring)
        DrawSelRectangle(pPaper, g_pColors->ScoreSelected());
            
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
lmSimpleObj::lmSimpleObj(lmObject* pParent, EScoreObjType nType, lmVStaff* pStaff, int nStaff,
             bool fVisible, bool fIsDraggable)
    : lmStaffObj(pParent, nType, pStaff, nStaff, fVisible, fIsDraggable)
{
}

//-------------------------------------------------------------------------------------------------
// lmCompositeObj implementation
//-------------------------------------------------------------------------------------------------
lmCompositeObj::lmCompositeObj(lmObject* pParent, EScoreObjType nType, lmVStaff* pStaff, int nStaff,
             bool fVisible, bool fIsDraggable)
    : lmStaffObj(pParent, nType, pStaff, nStaff, fVisible, fIsDraggable)
{
}

//-------------------------------------------------------------------------------------
// lmObject implementation
//-------------------------------------------------------------------------------------
lmObject::lmObject(lmObject* pParent)
{ 
    m_pParent = pParent;
    m_pObjOptions = (lmObjOptions*)NULL;
}

lmObject::~lmObject()
{ 
    if (m_pObjOptions) delete m_pObjOptions;
}

lmObjOptions* lmObject::GetCurrentObjOptions()
{
    //recurse in the parents chain to find the first non-null CtxObject
    //and return it
    if (m_pObjOptions) return m_pObjOptions;
    if (m_pParent) return m_pParent->GetCurrentObjOptions();
    return (lmObjOptions*)NULL;
}


//Set value for option in this object context. If no context exist, create it

void lmObject::SetOption(wxString sName, long nLongValue)
{
    if (!m_pObjOptions) m_pObjOptions = new lmObjOptions();
    m_pObjOptions->SetOption(sName, nLongValue);
}

void lmObject::SetOption(wxString sName, wxString sStringValue)
{
    if (!m_pObjOptions) m_pObjOptions = new lmObjOptions();
    m_pObjOptions->SetOption(sName, sStringValue);
}

void lmObject::SetOption(wxString sName, double nDoubleValue)
{
    if (!m_pObjOptions) m_pObjOptions = new lmObjOptions();
    m_pObjOptions->SetOption(sName, nDoubleValue);
}

void lmObject::SetOption(wxString sName, bool fBoolValue)
{
    if (!m_pObjOptions) m_pObjOptions = new lmObjOptions();
    m_pObjOptions->SetOption(sName, fBoolValue);
}

//Look for the value of an option. A method for each supported data type.
//Recursive search throug the ObjOptions chain

long lmObject::GetOptionLong(wxString sOptName)
{ 
    return GetCurrentObjOptions()->GetOptionLong(sOptName);
}

double lmObject::GetOptionDouble(wxString sOptName) 
{ 
    return GetCurrentObjOptions()->GetOptionDouble(sOptName); 
}

bool lmObject::GetOptionBool(wxString sOptName) 
{ 
    return GetCurrentObjOptions()->GetOptionBool(sOptName); 
}

wxString lmObject::GetOptionString(wxString sOptName) 
{   
    return GetCurrentObjOptions()->GetOptionString(sOptName);
}


