//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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
#pragma implementation "ShapeBracket.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "GMObject.h"
#include "ShapeBracket.h"
#include "../score/Score.h"


//-------------------------------------------------------------------------------------
// Implementation of lmShapeBracket


lmShapeBracket::lmShapeBracket(lmInstrument* pInstr, lmEBracketSymbol nSymbol,
                               lmLUnits xLeft, lmLUnits yTop,
                               lmLUnits xRight, lmLUnits yBottom,
                               wxColour color)
	: lmSimpleShape(eGMO_ShapeBracket, pInstr, _T("brace/bracket"))
{
    m_nSymbol = nSymbol;
    m_uxLeft = xLeft;
    m_uyTop = yTop;
    m_uxRight = xRight;
    m_uyBottom = yBottom;
	m_color = color;

	//set bounds
	SetXLeft(xLeft);
	SetYTop(yTop);
	SetXRight(xRight);
	SetYBottom(yBottom);
}

lmShapeBracket::~lmShapeBracket()
{
}

void lmShapeBracket::Render(lmPaper* pPaper, wxColour color)
{
    if (m_nSymbol == lm_eBracket)
    {
        lmLUnits w = m_uxRight - m_uxLeft;
        lmLUnits h = m_uyBottom - m_uyTop;
        lmLUnits x = m_uxLeft;
        lmLUnits y = m_uyTop;

        lmUPoint uPoints[] = {
            lmUPoint(x+w, y),
            lmUPoint(x+w/6, y+h/8),
            lmUPoint(x+w/6, y+h/5),
            lmUPoint(x+w*2/3, y+h*2/5),
            lmUPoint(x, y+h/2),
            lmUPoint(x+w*2/3, y+h*3/5),
            lmUPoint(x+w/6, y+h*4/5),
            lmUPoint(x+w/6, y+h*7/8),
            lmUPoint(x+w, y+h),
            lmUPoint(x+w/2, y+h*7/8),
            lmUPoint(x+w, y+h*3/5),
            lmUPoint(x, y+h/2),
            lmUPoint(x+w, y+h*2/5),
            lmUPoint(x+w/2, y+h*1/8)
        };
        pPaper->SolidPolygon(14, uPoints, color);
    }
    else if (m_nSymbol == lm_eBrace)
    {
        lmInstrument* pInstr = (lmInstrument*)m_pOwner;

        lmLUnits uThick = m_uxRight - m_uxLeft;
        lmLUnits uyDown = pInstr->TenthsToLogical(20.0f);
        lmLUnits uxDown = pInstr->TenthsToLogical(20.0f);

        lmUPoint uPoints[] = {
            lmUPoint(m_uxLeft, m_uyTop),
            lmUPoint(m_uxLeft, m_uyBottom),
            lmUPoint(m_uxLeft + uThick + uxDown, m_uyBottom + uyDown),
            lmUPoint(m_uxLeft + uThick, m_uyBottom),
            lmUPoint(m_uxLeft + uThick, m_uyTop),
            lmUPoint(m_uxLeft + uThick + uxDown, m_uyTop - uyDown)
        };
        pPaper->SolidPolygon(6, uPoints, color);
    }
    lmShape::RenderCommon(pPaper);
}

wxString lmShapeBracket::Dump(int nIndent)
{
	//TODO
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump += wxString::Format(_T("%04d %s: "),
		m_nId, m_sGMOName.c_str() );
    sDump += DumpBounds();
    sDump += _T("\n");
	return sDump;
}

void lmShapeBracket::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
    m_uxLeft += xIncr;
	m_uyTop += yIncr;
    m_uxRight += xIncr;
	m_uyBottom += yIncr;

    ShiftBoundsAndSelRec(xIncr, yIncr);

	//if included in a composite shape update parent bounding and selection rectangles
	if (this->IsChildShape())
		((lmCompositeShape*)GetParentShape())->RecomputeBounds();
}

