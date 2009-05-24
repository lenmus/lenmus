//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation,
//    either version 3 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program. If not, see <http://www.gnu.org/licenses/>.
//
//    For any comment, suggestion or feature request, please contact the manager of
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "ShapeBarline.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "GMObject.h"
#include "../score/Score.h"
#include "../score/VStaff.h"

#include "ShapeBarline.h"


//-------------------------------------------------------------------------------------
// Implementation of lmShapeBarline


lmShapeBarline::lmShapeBarline(lmBarline* pBarline, lmEBarline nBarlineType,
                               lmLUnits xPos, lmLUnits yTop,
						       lmLUnits yBottom, lmLUnits uThinLineWidth,
                               lmLUnits uThickLineWidth, lmLUnits uSpacing,
                               lmLUnits uRadius, wxColour color)
	: lmSimpleShape(eGMO_ShapeBarline, pBarline, 0, _T("Barline"), lmDRAGGABLE, lmSELECTABLE)
{
    m_nBarlineType = nBarlineType;
    m_uxPos = xPos;
    m_uyTop = yTop;
    m_uyBottom = yBottom;
    m_uThinLineWidth = uThinLineWidth;
    m_uThickLineWidth = uThickLineWidth;
    m_uSpacing = uSpacing;
    m_uRadius = uRadius;
	m_color = color;

    //compute width
    switch(m_nBarlineType)
    {
        case lm_eBarlineDouble:
            m_uWidth = m_uThinLineWidth + m_uSpacing + m_uThinLineWidth;
            break;

        case lm_eBarlineEndRepetition:
            m_uWidth = m_uRadius+ m_uSpacing + m_uRadius + m_uThinLineWidth + m_uSpacing + m_uThickLineWidth;
            break;

        case lm_eBarlineStartRepetition:
            m_uWidth = m_uThinLineWidth + m_uSpacing + m_uThickLineWidth +
                        m_uSpacing + m_uRadius + m_uRadius;
            break;

        case lm_eBarlineDoubleRepetition:
            m_uWidth = m_uRadius + m_uSpacing + m_uRadius + m_uThinLineWidth + m_uSpacing +
                        m_uThinLineWidth + m_uSpacing + m_uRadius + m_uRadius;
            break;

        case lm_eBarlineStart:
            m_uWidth = m_uThinLineWidth + m_uSpacing + m_uThickLineWidth;
            break;

        case lm_eBarlineEnd:
            m_uWidth = m_uThinLineWidth + m_uSpacing + m_uThickLineWidth;
            break;

        case lm_eBarlineSimple:
            m_uWidth = m_uThinLineWidth;
            break;

        default:
            wxASSERT(false);
            m_uWidth = 0;

    }

	//set bounds
	SetXLeft(xPos);
	SetYTop(yTop);
	SetXRight(xPos + m_uWidth);
	SetYBottom(yBottom);

    // store selection rectangle position and size
	m_uSelRect = GetBounds();

    //force selection rectangle to have at least a width of half line (5 tenths)
    lmTenths nWidth = pBarline->GetVStaff()->LogicalToTenths(m_uSelRect.GetWidth());
    if (nWidth < 5.0f)
    {
        lmLUnits uWidth = pBarline->GetVStaff()->TenthsToLogical(5.0f);
        m_uSelRect.SetX(m_uSelRect.x - uWidth/2.0f);
        m_uSelRect.SetWidth(uWidth);
    }
}

lmShapeBarline::~lmShapeBarline()
{
}

void lmShapeBarline::Render(lmPaper* pPaper, wxColour color)
{
    lmLUnits uxPos = m_uxPos;
    lmLUnits uyTop = m_uyTop;
    lmLUnits uyBottom = m_uyBottom;

    switch(m_nBarlineType)
    {
        case lm_eBarlineDouble:
            DrawThinLine(pPaper, uxPos, uyTop, uyBottom, color);
            uxPos += m_uThinLineWidth + m_uSpacing;
            DrawThinLine(pPaper, uxPos, uyTop, uyBottom, color);
            break;

        case lm_eBarlineEndRepetition:
            uxPos += m_uRadius;
            DrawTwoDots(pPaper, uxPos, uyTop);
            uxPos += m_uSpacing + m_uRadius;
            DrawThinLine(pPaper, uxPos, uyTop, uyBottom, color);
            uxPos += m_uThinLineWidth + m_uSpacing;
            DrawThickLine(pPaper, uxPos, uyTop, m_uThickLineWidth, uyBottom-uyTop, color);
            break;

        case lm_eBarlineStartRepetition:
            DrawThickLine(pPaper, uxPos, uyTop, m_uThickLineWidth, uyBottom-uyTop, color);
            uxPos += m_uThickLineWidth + m_uSpacing;
            DrawThinLine(pPaper, uxPos, uyTop, uyBottom, color);
            uxPos += m_uThinLineWidth + m_uSpacing + m_uRadius;
            DrawTwoDots(pPaper, uxPos, uyTop);
            break;

        case lm_eBarlineDoubleRepetition:
            uxPos += m_uRadius;
            DrawTwoDots(pPaper, uxPos, uyTop);
            uxPos += m_uSpacing + m_uRadius;
            DrawThinLine(pPaper, uxPos, uyTop, uyBottom, color);
            uxPos += m_uThinLineWidth + m_uSpacing;
            DrawThinLine(pPaper, uxPos, uyTop, uyBottom, color);
            uxPos += m_uThinLineWidth + m_uSpacing + m_uRadius;
            DrawTwoDots(pPaper, uxPos, uyTop);
            break;

        case lm_eBarlineStart:
            DrawThickLine(pPaper, uxPos, uyTop, m_uThickLineWidth, uyBottom-uyTop, color);
            uxPos += m_uThickLineWidth + m_uSpacing;
            DrawThinLine(pPaper, uxPos, uyTop, uyBottom, color);
            break;

        case lm_eBarlineEnd:
            DrawThinLine(pPaper, uxPos, uyTop, uyBottom, color);
            uxPos += m_uThinLineWidth + m_uSpacing;
            DrawThickLine(pPaper, uxPos, uyTop, m_uThickLineWidth, uyBottom-uyTop, color);
            break;

        case lm_eBarlineSimple:
            DrawThinLine(pPaper, uxPos, uyTop, uyBottom, color);
            break;
    }

    lmSimpleShape::Render(pPaper, color);
}

wxString lmShapeBarline::Dump(int nIndent)
{
	//TODO
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump += wxString::Format(_T("Idx: %d %s: xPos=%.2f, yTop=%.2f, yBot=%.2f, "),
		m_nOwnerIdx, m_sGMOName.c_str(), m_uxPos, m_uyTop, m_uyBottom );
    sDump += DumpBounds();
    sDump += _T("\n");
	return sDump;
}

void lmShapeBarline::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
    m_uxPos += xIncr;
    m_uyTop += yIncr;
    m_uyBottom += yIncr;

    ShiftBoundsAndSelRec(xIncr, yIncr);

	//if included in a composite shape update parent bounding and selection rectangles
	if (this->IsChildShape())
		((lmCompositeShape*)GetParentShape())->RecomputeBounds();
}

void lmShapeBarline::DrawThinLine(lmPaper* pPaper, lmLUnits uxPos, lmLUnits uyTop, lmLUnits uyBottom,
                             wxColour color)
{
    pPaper->SolidLine(uxPos + m_uThinLineWidth/2, uyTop,
                      uxPos + m_uThinLineWidth/2, uyBottom,
                      m_uThinLineWidth, lm_eEdgeNormal, color);

}

void lmShapeBarline::DrawThickLine(lmPaper* pPaper, lmLUnits xLeft, lmLUnits uyTop, lmLUnits uWidth,
                              lmLUnits uHeight, wxColour color)
{
    pPaper->SolidLine(xLeft + uWidth/2, uyTop,
                      xLeft + uWidth/2, uyTop + uHeight,
                      uWidth, lm_eEdgeNormal, color);
}

void lmShapeBarline::DrawTwoDots(lmPaper* pPaper, lmLUnits uxPos, lmLUnits uyPos)
{
    lmLUnits uShift1 = ((lmStaffObj*)m_pOwner)->TenthsToLogical(15);	// 1.5 lines
    lmLUnits uShift2 = ((lmStaffObj*)m_pOwner)->TenthsToLogical(25);    // 2.5 lines
    pPaper->SolidCircle(uxPos, uyPos + uShift1, m_uRadius);
    pPaper->SolidCircle(uxPos, uyPos + uShift2, m_uRadius);
}

wxBitmap* lmShapeBarline::OnBeginDrag(double rScale, wxDC* pDC)
{
	// A dragging operation is started. The view invokes this method to request the
	// bitmap to be used as drag image. No other action is required.
	// If no bitmap is returned drag is cancelled.
	//
	// So this method returns the bitmap to use with the drag image.


    // allocate the bitmap
    // convert size to pixels
    int wD = (int)pDC->LogicalToDeviceXRel((wxCoord)m_uWidth);
    int hD = (int)pDC->LogicalToDeviceYRel((wxCoord)(m_uyBottom - m_uyTop));
    wxBitmap bitmap(wD+2, hD+2);

    // allocate a memory DC for drawing into a bitmap
    wxMemoryDC dc2;
    dc2.SelectObject(bitmap);
    dc2.SetMapMode(lmDC_MODE);
    dc2.SetUserScale(rScale, rScale);

    // draw onto the bitmap
    dc2.SetBackground(*wxRED_BRUSH);	//*wxWHITE_BRUSH);
    dc2.Clear();
    dc2.SetBackgroundMode(wxTRANSPARENT);
    dc2.SetTextForeground(g_pColors->ScoreSelected());
    //dc2.DrawText(sGlyph, 0, 0);

    dc2.SelectObject(wxNullBitmap);

    // Make the bitmap masked
    wxImage image = bitmap.ConvertToImage();
    image.SetMaskColour(255, 255, 255);
    wxBitmap* pBitmap = new wxBitmap(image);

    ////DBG -----------
    //wxString sFileName = _T("ShapeGlyp2.bmp");
    //pBitmap->SaveFile(sFileName, wxBITMAP_TYPE_BMP);
    ////END DBG -------

    return pBitmap;
}

