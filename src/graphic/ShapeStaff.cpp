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
#pragma implementation "ShapeStaff.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "GMObject.h"
#include "../score/Score.h"
#include "../score/Staff.h"

#include "ShapeStaff.h"


//-------------------------------------------------------------------------------------
// Implementation of lmShapeStaff: an staff (usually 5 lines)


lmShapeStaff::lmShapeStaff(lmStaff* pStaff, int nStaff, int nNumLines, lmLUnits uLineWidth,
						   lmLUnits uSpacing, lmLUnits xLeft, lmLUnits yTop,
						   lmLUnits xRight, wxColour color)
	: lmSimpleShape(eGMO_ShapeStaff, pStaff, 0, _T("Staff"))
{
    wxASSERT(nStaff > 0);

	m_nStaff = nStaff;
	m_nNumLines = nNumLines;
	m_uLineWidth = uLineWidth;
	m_uSpacing = uSpacing;
	m_color = color;

	//bounds
	SetXLeft(xLeft);
	SetYTop(yTop);
	SetXRight(xRight);
	SetYBottom(yTop + (nNumLines - 1) * uSpacing + uLineWidth);
}

lmShapeStaff::~lmShapeStaff()
{
}

void lmShapeStaff::Render(lmPaper* pPaper, wxColour color)
{
    if (!m_fVisible) return;

    //update selection rectangle
    m_uSelRect = GetBounds();

    //draw the staff
	lmLUnits yPos = m_uBoundsTop.y;
    for (int iL=0; iL < m_nNumLines; iL++ )
	{
        pPaper->SolidLine(m_uBoundsTop.x, yPos, m_uBoundsBottom.x, yPos,
                          m_uLineWidth, eEdgeNormal, color);
        yPos += m_uSpacing;
    }
    lmSimpleShape::Render(pPaper,color);
}

wxString lmShapeStaff::Dump(int nIndent)
{
	//TODO
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump += wxString::Format(_T("%04d %s: "), m_nOwnerIdx, m_sGMOName.c_str());
    sDump += DumpBounds();
    sDump += _T("\n");
	return sDump;
}

void lmShapeStaff::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
	//TODO
    ShiftBoundsAndSelRec(xIncr, yIncr);

	//if included in a composite shape update parent bounding and selection rectangles
	if (this->IsChildShape())
		((lmCompositeShape*)GetParentShape())->RecomputeBounds();
}

