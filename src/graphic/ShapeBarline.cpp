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
#pragma implementation "ShapeBarline.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "GMObject.h"
#include "../score/Score.h"

#include "ShapeBarline.h"


//-------------------------------------------------------------------------------------
// Implementation of lmShapeBarline


lmShapeBarline::lmShapeBarline(lmBarline* pBarline, lmEBarline nBarlineType,
                               lmLUnits xPos, lmLUnits yTop,
						       lmLUnits yBottom, lmLUnits uThinLineWidth,
                               lmLUnits uThickLineWidth, lmLUnits uSpacing,
                               lmLUnits uRadius, wxColour color)
	: lmSimpleShape(eGMO_ShapeBarline, pBarline, _T("Barline"))
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
        case etb_DoubleBarline:
            m_uWidth = m_uThinLineWidth + m_uSpacing + m_uThinLineWidth;
            break;

        case etb_EndRepetitionBarline:
            m_uWidth = m_uRadius+ m_uSpacing + m_uRadius + m_uThinLineWidth + m_uSpacing + m_uThickLineWidth;
            break;

        case etb_StartRepetitionBarline:
            m_uWidth = m_uThinLineWidth + m_uSpacing + m_uThickLineWidth +
                        m_uSpacing + m_uRadius + m_uRadius;
            break;

        case etb_DoubleRepetitionBarline:
            m_uWidth = m_uRadius + m_uSpacing + m_uRadius + m_uThinLineWidth + m_uSpacing +
                        m_uThinLineWidth + m_uSpacing + m_uRadius + m_uRadius;
            break;

        case etb_StartBarline:
            m_uWidth = m_uThinLineWidth + m_uSpacing + m_uThickLineWidth;
            break;

        case etb_EndBarline:
            m_uWidth = m_uThinLineWidth + m_uSpacing + m_uThickLineWidth;
            break;

        case etb_SimpleBarline:
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
        case etb_DoubleBarline:
            DrawThinLine(pPaper, uxPos, uyTop, uyBottom, m_color);
            uxPos += m_uThinLineWidth + m_uSpacing;
            DrawThinLine(pPaper, uxPos, uyTop, uyBottom, m_color);
            break;

        case etb_EndRepetitionBarline:
            uxPos += m_uRadius;
            DrawTwoDots(pPaper, uxPos, uyTop);
            uxPos += m_uSpacing + m_uRadius;
            DrawThinLine(pPaper, uxPos, uyTop, uyBottom, m_color);
            uxPos += m_uThinLineWidth + m_uSpacing;
            DrawThickLine(pPaper, uxPos, uyTop, m_uThickLineWidth, uyBottom-uyTop, m_color);
            break;

        case etb_StartRepetitionBarline:
            DrawThickLine(pPaper, uxPos, uyTop, m_uThickLineWidth, uyBottom-uyTop, m_color);
            uxPos += m_uThickLineWidth + m_uSpacing;
            DrawThinLine(pPaper, uxPos, uyTop, uyBottom, m_color);
            uxPos += m_uThinLineWidth + m_uSpacing + m_uRadius;
            DrawTwoDots(pPaper, uxPos, uyTop);
            break;

        case etb_DoubleRepetitionBarline:
            uxPos += m_uRadius;
            DrawTwoDots(pPaper, uxPos, uyTop);
            uxPos += m_uSpacing + m_uRadius;
            DrawThinLine(pPaper, uxPos, uyTop, uyBottom, m_color);
            uxPos += m_uThinLineWidth + m_uSpacing;
            DrawThinLine(pPaper, uxPos, uyTop, uyBottom, m_color);
            uxPos += m_uThinLineWidth + m_uSpacing + m_uRadius;
            DrawTwoDots(pPaper, uxPos, uyTop);
            break;

        case etb_StartBarline:
            DrawThickLine(pPaper, uxPos, uyTop, m_uThickLineWidth, uyBottom-uyTop, m_color);
            uxPos += m_uThickLineWidth + m_uSpacing;
            DrawThinLine(pPaper, uxPos, uyTop, uyBottom, m_color);
            break;

        case etb_EndBarline:
            DrawThinLine(pPaper, uxPos, uyTop, uyBottom, m_color);
            uxPos += m_uThinLineWidth + m_uSpacing;
            DrawThickLine(pPaper, uxPos, uyTop, m_uThickLineWidth, uyBottom-uyTop, m_color);
            break;

        case etb_SimpleBarline:
            DrawThinLine(pPaper, uxPos, uyTop, uyBottom, m_color);
            break;
    }

    lmShape::RenderCommon(pPaper);

}

wxString lmShapeBarline::Dump(int nIndent)
{
	//TODO
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump += wxString::Format(_T("%04d %s: xPos=%.2f, yTop=%.2f, yBot=%.2f, "),
		m_nId, m_sShapeName, m_uxPos, m_uyTop, m_uyBottom );
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
}

void lmShapeBarline::DrawThinLine(lmPaper* pPaper, lmLUnits uxPos, lmLUnits uyTop, lmLUnits uyBottom,
                             wxColour color)
{
    pPaper->SolidLine(uxPos + m_uThinLineWidth/2, uyTop,
                      uxPos + m_uThinLineWidth/2, uyBottom,
                      m_uThinLineWidth, eEdgeNormal, color);

}

void lmShapeBarline::DrawThickLine(lmPaper* pPaper, lmLUnits xLeft, lmLUnits uyTop, lmLUnits uWidth,
                              lmLUnits uHeight, wxColour color)
{
    pPaper->SolidLine(xLeft + uWidth/2, uyTop,
                      xLeft + uWidth/2, uyTop + uHeight,
                      uWidth, eEdgeNormal, color);
}

void lmShapeBarline::DrawTwoDots(lmPaper* pPaper, lmLUnits uxPos, lmLUnits uyPos)
{
    lmLUnits uShift1 = ((lmStaffObj*)m_pOwner)->TenthsToLogical(15);	// 1.5 lines
    lmLUnits uShift2 = ((lmStaffObj*)m_pOwner)->TenthsToLogical(25);    // 2.5 lines
    pPaper->SolidCircle(uxPos, uyPos + uShift1, m_uRadius);
    pPaper->SolidCircle(uxPos, uyPos + uShift2, m_uRadius);
}


