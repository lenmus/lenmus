// RCS-ID: $Id: Barline.cpp,v 1.3 2006/02/23 19:22:56 cecilios Exp $
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
/*! @file Barline.cpp
    @brief Implementation file for class lmBarline
    @ingroup score_kernel
*/
#ifdef __GNUG__
//  #pragma implementation
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

//-------------------------------------------------------------------------------------------------
// lmBarline object implementation
//-------------------------------------------------------------------------------------------------

//
//constructors and destructor
//

lmBarline::lmBarline(ETipoBarra nBarlineType, lmVStaff* pVStaff, bool fVisible) :
    lmSimpleObj(eTPO_Barline, pVStaff, 1, fVisible, sbDRAGGABLE)
{
    m_nBarlineType = nBarlineType;
}

// Create the drag image.
// Under wxGTK the DC logical function (ROP) is used by DrawText() but it is ignored by
// wxMSW. Thus, it is not posible to do dragging just by redrawing the lmStaffObj using ROPs.
// For portability it is necessary to implement dragging by means of bitmaps and wxDragImage
wxBitmap* lmBarline::GetBitmap(double rScale)
{
    // Get size of barline, in logical units
    wxScreenDC dc;
    dc.SetMapMode(lmDC_MODE);
    dc.SetUserScale(rScale, rScale);
    lmMicrons hL = m_pVStaff->GetYBottom() - m_pVStaff->GetYTop();
    lmMicrons wL = DrawBarline(DO_MEASURE, &dc, 0, 0, hL, g_pColors->ScoreSelected());

    // allocate a memory DC for drawing into a bitmap
    wxMemoryDC dc2;
    dc2.SetMapMode(lmDC_MODE);
    dc2.SetUserScale(rScale, rScale);

    // allocate the bitmap
    // convert size to pixels
    wxCoord wD = dc2.LogicalToDeviceXRel(wL),
            hD = dc2.LogicalToDeviceYRel(hL);
    // Add a couple of pixels to the bitmap for security
    wxBitmap bitmap((int)(wD+2), (int)(hD+2));
    dc2.SelectObject(bitmap);

    // draw onto the bitmap
    dc2.SetBackground(* wxWHITE_BRUSH);
    dc2.Clear();
    dc2.SetBackgroundMode(wxTRANSPARENT);
    DrawBarline(DO_DRAW, &dc2, 0, 0, hL, g_pColors->ScoreSelected());

    dc2.SelectObject(wxNullBitmap);

    // Make the bitmap masked
    wxImage image = bitmap.ConvertToImage();
    image.SetMaskColour(255, 255, 255);
    wxBitmap* pBitmap = new wxBitmap(image);
    return pBitmap;

}

void lmBarline::MoveDragImage(lmPaper* pPaper, wxDragImage* pDragImage, wxPoint& offsetD, 
        const wxPoint& pagePosL, const wxPoint& dragStartPosL, const wxPoint& canvasPosD)
{
    // A barline only can be moved horizonatlly
    wxPoint ptNew = canvasPosD;
    ptNew.y = pPaper->LogicalToDeviceY(m_paperPos.y + m_glyphPos.y) + offsetD.y;
    pDragImage->Move(ptNew);

}
    
wxPoint lmBarline::EndDrag(const wxPoint& pos)
{
    wxPoint oldPos(m_paperPos + m_glyphPos);

    //Only X pos. can be changed
    m_paperPos.x = pos.x - m_glyphPos.x;

    return wxPoint(oldPos);
}


//Property Get IPentObj_PositioningType() As EPositioningType
//    IPentObj_PositioningType = ePos_Serial
//End Property

wxString lmBarline::Dump()
{
    wxString sDump = wxString::Format(
        _T("%d\tBarline %s\tTimePos=%.2f\n"),
        m_nId, GetBarlineLDPNameFromType(m_nBarlineType), m_rTimePos );
    return sDump;
            
}

wxString lmBarline::SourceLDP()
{
    wxString sSource = _T("            (Barra ");
    sSource += GetBarlineLDPNameFromType(m_nBarlineType);
    if (!m_fVisible) { sSource += _T(" no_visible"); }
    sSource += _T(")\n");
    return sSource;
}

wxString lmBarline::SourceXML()
{
    wxString sSource = _T("TODO: lmBarline XML Source code generation methods");
    return sSource;
}

//Property Let IPentObj_Left(nLeft As Long)
//    Dim nAncho As Long
//    nAncho = m_rRight - m_rLeft
//    m_rLeft = nLeft
//    m_rRight = nLeft + nAncho
//    
//End Property

void lmBarline::DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC)
{
    lmMicrons yTop = m_pVStaff->GetYTop();
    lmMicrons yBottom = m_pVStaff->GetYBottom();
    
    wxDC* pDC = pPaper->GetDC();
    wxASSERT(pDC);
    lmMicrons xPos = pPaper->GetCursorX();
    lmMicrons nWidth = DrawBarline(fMeasuring, pDC, xPos, yTop, yBottom, colorC);

    if (fMeasuring) {
        // store selection rectangle measures and position
        m_selRect.width = nWidth;
        m_selRect.height = yBottom - yTop;
        m_selRect.x = xPos - m_paperPos.x;        //relative to m_paperPos
        m_selRect.y = yTop - m_paperPos.y;;

        // set total width
        m_nWidth = nWidth;

        // store glyph position (relative to paper pos). 
        m_glyphPos.x = 0;
        m_glyphPos.y = pPaper->GetCursorY() - yTop;
    }

}

// returns the width of the barline (in microns)
lmMicrons lmBarline::DrawBarline(bool fMeasuring, wxDC* pDC, lmMicrons xPos, lmMicrons yTop,
                               lmMicrons yBottom, wxColour colorC)
{
#define THIN_LINE_WIDTH        200        // thin line width will be 0.2 mm (200 microns)
#define THICK_LINE_WIDTH    1000    // thick line width will be 1.0 mm (1000 microns)

    int nSpacing = m_pVStaff->TenthsToLogical(4, 1);    //space between lines: 4 tenths
    if (!fMeasuring) {
        wxPen pen(colorC, THIN_LINE_WIDTH, wxSOLID);
        wxBrush brush(colorC, wxSOLID);
        pDC->SetPen(pen);
        pDC->SetBrush(brush);
    }

    switch(m_nBarlineType)
    {
        case etbBarraDoble:
            if (!fMeasuring) {
                DrawThinLine(pDC, xPos, yTop, yBottom);
                xPos += THIN_LINE_WIDTH + nSpacing;
                DrawThinLine(pDC, xPos, yTop, yBottom);
            }
            return (lmMicrons)(nSpacing + THIN_LINE_WIDTH + THIN_LINE_WIDTH);
            break;
            
        case etbBarraFinRepeticion:
            if (!fMeasuring) {
                DrawTwoDots(pDC, xPos, yTop);
                xPos += nSpacing;
                DrawThinLine(pDC, xPos, yTop, yBottom);
                xPos += THIN_LINE_WIDTH + nSpacing;
                DrawThickLine(pDC, xPos, yTop, THICK_LINE_WIDTH, yBottom-yTop);
            }
            return (lmMicrons)(nSpacing + nSpacing + THIN_LINE_WIDTH + THICK_LINE_WIDTH);
            break;

        case etbBarraInicioRepeticion:
            if (!fMeasuring) {
                DrawThickLine(pDC, xPos, yTop, THICK_LINE_WIDTH, yBottom-yTop);
                xPos += THICK_LINE_WIDTH + nSpacing;
                DrawThinLine(pDC, xPos, yTop, yBottom);
                xPos += nSpacing/2;
                DrawTwoDots(pDC, xPos, yTop);
            }
            return (lmMicrons)(nSpacing + nSpacing + THIN_LINE_WIDTH + THICK_LINE_WIDTH);
            break;
            
        case etbDobleRepeticion:
            if (!fMeasuring) {
                DrawTwoDots(pDC, xPos, yTop);
                xPos += nSpacing;
                DrawThinLine(pDC, xPos, yTop, yBottom);
                xPos += THIN_LINE_WIDTH + nSpacing;
                DrawThinLine(pDC, xPos, yTop, yBottom);
                xPos += THIN_LINE_WIDTH + nSpacing/2;
                DrawTwoDots(pDC, xPos, yTop);
            }
            return (lmMicrons)(nSpacing + nSpacing + THIN_LINE_WIDTH + THIN_LINE_WIDTH);
            break;
        
        case etbBarraInicial:
            if (!fMeasuring) {
                DrawThickLine(pDC, xPos, yTop, THICK_LINE_WIDTH, yBottom-yTop);
                xPos += THICK_LINE_WIDTH + nSpacing;
                DrawThinLine(pDC, xPos, yTop, yBottom);
            }
            return (lmMicrons)(nSpacing + THIN_LINE_WIDTH + THICK_LINE_WIDTH);
            break;
            
        case etbBarraFinal:
            if (!fMeasuring) {
                DrawThinLine(pDC, xPos, yTop, yBottom);
                xPos += THIN_LINE_WIDTH + nSpacing;
                DrawThickLine(pDC, xPos, yTop, THICK_LINE_WIDTH, yBottom-yTop);
            }
            return (lmMicrons)(nSpacing + THIN_LINE_WIDTH + THICK_LINE_WIDTH);
            break;

        case etbBarraNormal:
            if (!fMeasuring) {
                DrawThinLine(pDC, xPos, yTop, yBottom);
            }
            return THIN_LINE_WIDTH;
            break;

        default:
            wxASSERT(false);
            return 0;            // to keep compiler happy
            
    }
}

void lmBarline::DrawThinLine(wxDC* pDC, lmMicrons xPos, lmMicrons yTop, lmMicrons yBottom)
{
    /*
    The DC must have pen and brush properly set
    */
    pDC->DrawLine((wxCoord)xPos, (wxCoord)yTop, (wxCoord)xPos, (wxCoord)yBottom);

}


void lmBarline::DrawThickLine(wxDC* pDC, lmMicrons xLeft, lmMicrons yTop, lmMicrons nWidth,
                            lmMicrons nHeight)
{
    /*
    We can not use DrawLine because with DrawLine function we get end of lines rounded
    instead of flat ends. Therefore, we are going to draw thick lines as rectangles.

    The DC must have pen and brush properly set
    */
    pDC->DrawRectangle((wxCoord)xLeft, (wxCoord)yTop, (wxCoord)nWidth, (wxCoord)nHeight);

}

void lmBarline::DrawTwoDots(wxDC* pDC, lmMicrons xPos, lmMicrons yPos)
{            
    lmMicrons radius = m_pVStaff->TenthsToLogical(2, 1);    // 2 tenths
    lmMicrons shift1 = m_pVStaff->TenthsToLogical(15, 1) + radius;    // 1.5 lines
    lmMicrons shift2 = m_pVStaff->TenthsToLogical(25, 1) + radius;    // 2.5 lines
    pDC->DrawCircle((wxCoord)xPos, (wxCoord)(yPos + shift1), (wxCoord)radius);
    pDC->DrawCircle((wxCoord)xPos, (wxCoord)(yPos + shift2), (wxCoord)radius);
    
}

//-------------------------------------------------------------------------------------------------
// global functions related to barlines
//-------------------------------------------------------------------------------------------------

wxString GetBarlineLDPNameFromType(ETipoBarra nBarlineType)
{
    switch(nBarlineType)
    {
        case etbBarraFinRepeticion:
            return _T("FinRepeticion");
        case etbBarraInicioRepeticion:
            return _T("InicioRepeticion");
        case etbBarraFinal:
            return _T("Final");
        case etbBarraDoble:
            return _T("Doble");
        case etbBarraNormal:
            return _T("Simple");
        case etbBarraInicial:
            return _T("Inicial");
        case etbDobleRepeticion:
            return _T("DobleRepeticion");
        default:
            wxASSERT(false);
            return _T("");        //let's keep the compiler happy
    }
    
}