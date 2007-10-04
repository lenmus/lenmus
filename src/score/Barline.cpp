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

#ifdef __GNUG__
#pragma implementation "Barline.h"
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
static lmLUnits m_uThinLineWidth;
static lmLUnits m_uThickLineWidth;
static lmLUnits m_uSpacing;         // between lines and lines-dots
static lmLUnits m_uRadius;            // for dots

//
//constructors and destructor
//

lmBarline::lmBarline(EBarline nBarlineType, lmVStaff* pVStaff, bool fVisible) :
    lmStaffObj(pVStaff, eSFOT_Barline, pVStaff, 1, fVisible, lmDRAGGABLE)
{
    m_nBarlineType = nBarlineType;

    m_uThinLineWidth = m_pVStaff->TenthsToLogical(1.5, 1);  // thin line width
    m_uThickLineWidth = m_pVStaff->TenthsToLogical(6, 1);   // thick line width
    m_uSpacing = m_pVStaff->TenthsToLogical(4, 1);          // space between lines: 4 tenths
    m_uRadius = m_pVStaff->TenthsToLogical(2, 1);           // dots radius: 2 tenths

    //default location (auto-layout)
    m_xUserPosType = lmLOCATION_DEFAULT;
    m_uxUserPos = 0;

}

void lmBarline::AddContext(lmContext* pContext, int nStaff)
{
    wxASSERT(nStaff-1 == (int)m_aContexts.GetCount());
    m_aContexts.Add(pContext);
}

lmContext* lmBarline::GetContext(int nStaff)
{
    wxASSERT(nStaff <= (int)m_aContexts.GetCount());
    return m_aContexts.Item(nStaff-1);
}

// Create the drag image.
// Under wxGTK the DC logical function (ROP) is used by DrawText() but it is ignored by
// wxMSW. Thus, it is not posible to do dragging just by redrawing the lmStaffObj using ROPs.
// For portability it is necessary to implement dragging by means of bitmaps and wxDragImage
wxBitmap* lmBarline::GetBitmap(double rScale)
{
    //create a lmPaper associated to the screen DC for creating the drag bitmap
    lmPaper oPaper;

    // Get size of barline, in logical units
    wxScreenDC dc;
    dc.SetMapMode(lmDC_MODE);
    dc.SetUserScale(rScale, rScale);
    lmLUnits hL = m_pVStaff->GetYBottom() - m_pVStaff->GetYTop();
    //oPaper.SetDC(&dc);
    oPaper.SetDrawer(new lmDirectDrawer(&dc));
    lmLUnits wL = DrawBarline(DO_MEASURE, &oPaper, 0, 0, hL, g_pColors->ScoreSelected());

    // allocate a memory DC for drawing into a bitmap
    wxMemoryDC dc2;
    dc2.SetMapMode(lmDC_MODE);
    dc2.SetUserScale(rScale, rScale);

    // allocate the bitmap
    // convert size to pixels
    wxCoord wD = dc2.LogicalToDeviceXRel((int)wL),
            hD = dc2.LogicalToDeviceYRel((int)hL);
    // Add a couple of pixels to the bitmap for security
    wxBitmap bitmap((int)(wD+2), (int)(hD+2));
    dc2.SelectObject(bitmap);

    // draw onto the bitmap
    dc2.SetBackground(* wxWHITE_BRUSH);
    dc2.Clear();
    dc2.SetBackgroundMode(wxTRANSPARENT);
    //oPaper.SetDC(&dc2);
    oPaper.SetDrawer(new lmDirectDrawer(&dc2));
    DrawBarline(DO_DRAW, &oPaper, 0, 0, hL, g_pColors->ScoreSelected());

    dc2.SelectObject(wxNullBitmap);

    // Make the bitmap masked
    wxImage image = bitmap.ConvertToImage();
    image.SetMaskColour(255, 255, 255);
    wxBitmap* pBitmap = new wxBitmap(image);
    return pBitmap;

}

void lmBarline::MoveDragImage(lmPaper* pPaper, wxDragImage* pDragImage, lmDPoint& offsetD,
        const lmUPoint& pagePosL, const lmUPoint& uDragStartPos, const lmDPoint& canvasPosD)
{
    // A barline only can be moved horizonatlly
    lmDPoint ptNew = canvasPosD;
    ptNew.y = pPaper->LogicalToDeviceY(m_uPaperPos.y + m_uGlyphPos.y) + offsetD.y;
    pDragImage->Move(ptNew);

}

lmUPoint lmBarline::EndDrag(const lmUPoint& uPos)
{
    lmUPoint oldPos(m_uPaperPos + m_uGlyphPos);

    //Only X uPos. can be changed
    m_uPaperPos.x = uPos.x - m_uGlyphPos.x;

    return lmUPoint(oldPos);
}

wxString lmBarline::Dump()
{
    wxString sDump = wxString::Format(
        _T("%d\tBarline %s\tTimePos=%.2f\n"),
        m_nId, GetBarlineLDPNameFromType(m_nBarlineType).c_str(), m_rTimePos );
    return sDump;

}

wxString lmBarline::SourceLDP()
{
    wxString sSource = _T("         (barline ");
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

void lmBarline::DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC, bool fHighlight)
{
    lmLUnits uyTop = m_pVStaff->GetYTop();
    lmLUnits uyBottom = m_pVStaff->GetYBottom();
    lmLUnits uxPos = pPaper->GetCursorX();
    lmLUnits uWidth = DrawBarline(fMeasuring, pPaper, uxPos, uyTop, uyBottom, colorC);

    if (fMeasuring) {
        // store selection rectangle measures and position
        m_uSelRect.width = uWidth;
        m_uSelRect.height = uyBottom - uyTop;
        m_uSelRect.x = uxPos - m_uPaperPos.x;        //relative to m_uPaperPos
        m_uSelRect.y = uyTop - m_uPaperPos.y;

        // set total width
        m_uWidth = uWidth;

        // store glyph position (relative to paper pos).
        m_uGlyphPos.x = 0;
        m_uGlyphPos.y = pPaper->GetCursorY() - uyTop;
    }

}

// returns the width of the barline (in logical units)
lmLUnits lmBarline::DrawBarline(bool fMeasuring, lmPaper* pPaper,
                                lmLUnits uxPos, lmLUnits uyTop,
                                lmLUnits uyBottom, wxColour colorC)
{
    switch(m_nBarlineType)
    {
        case etb_DoubleBarline:
            if (!fMeasuring) {
                DrawThinLine(pPaper, uxPos, uyTop, uyBottom, colorC);
                uxPos += m_uThinLineWidth + m_uSpacing;
                DrawThinLine(pPaper, uxPos, uyTop, uyBottom, colorC);
            }
            return m_uThinLineWidth + m_uSpacing + m_uThinLineWidth;
            break;

        case etb_EndRepetitionBarline:
            if (!fMeasuring) {
                uxPos += m_uRadius;
                DrawTwoDots(pPaper, uxPos, uyTop);
                uxPos += m_uSpacing + m_uRadius;
                DrawThinLine(pPaper, uxPos, uyTop, uyBottom, colorC);
                uxPos += m_uThinLineWidth + m_uSpacing;
                DrawThickLine(pPaper, uxPos, uyTop, m_uThickLineWidth, uyBottom-uyTop, colorC);
            }
            return m_uRadius+ m_uSpacing + m_uRadius + m_uThinLineWidth + m_uSpacing + m_uThickLineWidth;
            break;

        case etb_StartRepetitionBarline:
            if (!fMeasuring) {
                DrawThickLine(pPaper, uxPos, uyTop, m_uThickLineWidth, uyBottom-uyTop, colorC);
                uxPos += m_uThickLineWidth + m_uSpacing;
                DrawThinLine(pPaper, uxPos, uyTop, uyBottom, colorC);
                uxPos += m_uThinLineWidth + m_uSpacing + m_uRadius;
                DrawTwoDots(pPaper, uxPos, uyTop);
            }
            return m_uThinLineWidth + m_uSpacing + m_uThickLineWidth +
                        m_uSpacing + m_uRadius + m_uRadius;
            break;

        case etb_DoubleRepetitionBarline:
            if (!fMeasuring) {
                uxPos += m_uRadius;
                DrawTwoDots(pPaper, uxPos, uyTop);
                uxPos += m_uSpacing + m_uRadius;
                DrawThinLine(pPaper, uxPos, uyTop, uyBottom, colorC);
                uxPos += m_uThinLineWidth + m_uSpacing;
                DrawThinLine(pPaper, uxPos, uyTop, uyBottom, colorC);
                uxPos += m_uThinLineWidth + m_uSpacing + m_uRadius;
                DrawTwoDots(pPaper, uxPos, uyTop);
            }
            return m_uRadius + m_uSpacing + m_uRadius + m_uThinLineWidth + m_uSpacing +
                        m_uThinLineWidth + m_uSpacing + m_uRadius + m_uRadius;
            break;

        case etb_StartBarline:
            if (!fMeasuring) {
                DrawThickLine(pPaper, uxPos, uyTop, m_uThickLineWidth, uyBottom-uyTop, colorC);
                uxPos += m_uThickLineWidth + m_uSpacing;
                DrawThinLine(pPaper, uxPos, uyTop, uyBottom, colorC);
            }
            return m_uThinLineWidth + m_uSpacing + m_uThickLineWidth;
            break;

        case etb_EndBarline:
            if (!fMeasuring) {
                DrawThinLine(pPaper, uxPos, uyTop, uyBottom, colorC);
                uxPos += m_uThinLineWidth + m_uSpacing;
                DrawThickLine(pPaper, uxPos, uyTop, m_uThickLineWidth, uyBottom-uyTop, colorC);
            }
            return m_uThinLineWidth + m_uSpacing + m_uThickLineWidth;
            break;

        case etb_SimpleBarline:
            if (!fMeasuring) {
                DrawThinLine(pPaper, uxPos, uyTop, uyBottom, colorC);
            }
            return m_uThinLineWidth;
            break;

        default:
            wxASSERT(false);
            return 0;            // to keep compiler happy

    }
}

void lmBarline::DrawThinLine(lmPaper* pPaper, lmLUnits uxPos, lmLUnits uyTop, lmLUnits uyBottom,
                             wxColour color)
{
    pPaper->SolidLine(uxPos + m_uThinLineWidth/2, uyTop,
                      uxPos + m_uThinLineWidth/2, uyBottom,
                      m_uThinLineWidth, eEdgeNormal, color);

}


void lmBarline::DrawThickLine(lmPaper* pPaper, lmLUnits xLeft, lmLUnits uyTop, lmLUnits uWidth,
                              lmLUnits uHeight, wxColour color)
{
    pPaper->SolidLine(xLeft + uWidth/2, uyTop,
                      xLeft + uWidth/2, uyTop + uHeight,
                      uWidth, eEdgeNormal, color);
}

void lmBarline::DrawTwoDots(lmPaper* pPaper, lmLUnits uxPos, lmLUnits uyPos)
{
    lmLUnits uShift1 = m_pVStaff->TenthsToLogical(15, 1);    // 1.5 lines
    lmLUnits uShift2 = m_pVStaff->TenthsToLogical(25, 1);    // 2.5 lines
    pPaper->SolidCircle(uxPos, uyPos + uShift1, m_uRadius);
    pPaper->SolidCircle(uxPos, uyPos + uShift2, m_uRadius);
}

void lmBarline::SetLocation(lmLUnits uxPos, lmELocationType nType)
{
    m_uxUserPos = uxPos;
    m_xUserPosType = nType;
}


//-------------------------------------------------------------------------------------------------
// global functions related to barlines
//-------------------------------------------------------------------------------------------------

wxString GetBarlineLDPNameFromType(EBarline nBarlineType)
{
    switch(nBarlineType)
    {
        case etb_EndRepetitionBarline:
            return _T("FinRepeticion");
        case etb_StartRepetitionBarline:
            return _T("InicioRepeticion");
        case etb_EndBarline:
            return _T("Final");
        case etb_DoubleBarline:
            return _T("Doble");
        case etb_SimpleBarline:
            return _T("Simple");
        case etb_StartBarline:
            return _T("Inicial");
        case etb_DoubleRepetitionBarline:
            return _T("DobleRepeticion");
        default:
            wxASSERT(false);
            return _T("");        //let's keep the compiler happy
    }

}
