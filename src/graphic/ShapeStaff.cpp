//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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
#include "ShapeNote.h"          //function lmDrawLegerLines()
#include "../score/Score.h"
#include "../score/VStaff.h"
#include "../score/Staff.h"

#include "ShapeStaff.h"



#define lmNO_LEDGER_LINES   -100000.0f

//-------------------------------------------------------------------------------------
// Implementation of lmShapeStaff: an staff (usually 5 lines)

lmShapeStaff::lmShapeStaff(lmStaff* pStaff, int nStaff, int nNumLines, lmLUnits uLineWidth,
						   lmLUnits uSpacing, lmLUnits xLeft, lmLUnits yTop,
						   lmLUnits xRight, wxColour color)
	: lmSimpleShape(eGMO_ShapeStaff, pStaff, 0, _T("Staff"))
	, m_nStaff(nStaff)
	, m_nNumLines(nNumLines)
	, m_uLineWidth(uLineWidth)
	, m_uSpacing(uSpacing)
    , m_nOldSteps(lmNO_LEDGER_LINES)
{
    wxASSERT(nStaff > 0);

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
                          m_uLineWidth, lm_eEdgeNormal, color);
        yPos += m_uSpacing;
    }
    lmSimpleShape::Render(pPaper,color);
}

wxString lmShapeStaff::Dump(int nIndent)
{
	//TODO
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump += wxString::Format(_T("Idx: %d %s: "), m_nOwnerIdx, m_sGMOName.c_str());
    sDump += DumpBounds();
    sDump += _T("\n");

    //base class
    sDump += lmShape::Dump(nIndent);

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

int lmShapeStaff::GetLineSpace(lmLUnits uyPos)
{
    //returns the position (line/space number) for the received point. Position is
    //referred to the first ledger line of the staff:
    //        0 - on first ledger line (C note in G clef)
    //        1 - on next space (D in G clef)
    //        2 - on first line (E not in G clef)
    //        3 - on first space
    //        4 - on second line
    //        5 - on second space
    //        etc.

	//The received position could be approximated (i.e. mouse position). So, first, we must
    //adjust position to the nearest valid line/half-line position

    //int nSteps;
    //lmCheckNoteNewPosition((lmStaff*)GetScoreOwner(), m_uBoundsTop.y, uyPos, &nSteps);
    //return nSteps + 10;

    //compute the number of steps (half lines) from line 5 (top staff line = step #10)
    lmStaff* pStaff = (lmStaff*)GetScoreOwner();
	lmLUnits uHalfLine = pStaff->TenthsToLogical(5.0f);
    float rStep = (m_uBoundsTop.y - uyPos)/uHalfLine;
    int nStep = (rStep > 0.0f ? (int)(rStep + 0.5f) : (int)(rStep - 0.5f) );
    //wxLogMessage(_T("[lmShapeStaff::GetLineSpace] uyPos=%.2f, uHalfLine=%.2f, m_uBoundsTop.y=%.2f, rStep=%.2f, nStep=%d"),
    //             uyPos, uHalfLine, m_uBoundsTop.y, rStep, nStep);
	return  10 + nStep;
    //AWARE: Note that y axis is reversed. Therefore we return 10 + steps instead
    // of 10 - steps. 
}

lmUPoint lmShapeStaff::OnMouseStartMoving(lmPaper* pPaper, const lmUPoint& uPos)
{
    m_uxOldPos = lmNO_LEDGER_LINES;

    //wxLogMessage(_T("[lmShapeStaff::OnMouseStartMoving] nStaff=%d"), m_nStaff);
    return OnMouseMoving(pPaper, uPos);
}

lmUPoint lmShapeStaff::OnMouseMoving(lmPaper* pPaper, const lmUPoint& uPos)
{
	//The user continues moving the mouse (tool: insert note) over a valid area for
    //inserting a note/rest in this staff. We receive the mouse position and we
    //must return the valid notehead position. We must also erase any previously drawn ledger
    //lines and draw new ones if necessary

    //A note only can be placed in discrete vertical steps (staff lines/spaces)
    lmUPoint pos = uPos;
	int nSteps;
    pos.y = lmCheckNoteNewPosition((lmStaff*)GetScoreOwner(), GetYTop(), uPos.y, &nSteps);

		//draw leger lines

    lmVStaff* pVStaff = (lmVStaff*)GetScoreOwner()->GetParentScoreObj();

	//as painting uses XOR we need the complementary color
	wxColour color = *wxBLUE;
	wxColour colorC(255 - (int)color.Red(), 255 - (int)color.Green(), 255 - (int)color.Blue() );
	pPaper->SetLogicalFunction(wxXOR);

    //wxLogMessage(_T("[lmShapeStaff::OnMouseMoving] VStaff=0x%x, nStaff=%d, OldSteps=%d, oldPos=%.2f, newSteps=%d, newPos=%.2f"),
    //             pVStaff, m_nStaff, m_nOldSteps, m_uxOldPos, nSteps, uPos.x );

	//remove old ledger lines
    lmLUnits uLineLength = 2.5f * m_uSpacing;
	if (m_uxOldPos != -100000.0f)
        lmDrawLegerLines(m_nOldSteps+10, m_uxOldPos, pVStaff, m_nStaff,
                         uLineLength, m_uBoundsTop.y, pPaper, colorC);

	//draw new ledger lines and save data for erasing them the next time
	m_uxOldPos = uPos.x - m_uSpacing;
    lmDrawLegerLines(nSteps+10, m_uxOldPos, pVStaff, m_nStaff,
                        uLineLength, m_uBoundsTop.y, pPaper, colorC);
	m_nOldSteps = nSteps;

	return pos;
}

void lmShapeStaff::OnMouseEndMoving(lmPaper* pPaper, lmUPoint uPos)
{
	// End drag. Receives the final position of the object (logical units referred
    // to page origin).
	// This method must remove any XOR painted lines.

	//remove old ledger lines
	if (m_uxOldPos != lmNO_LEDGER_LINES)
    {
	    //as painting uses XOR we need the complementary color
	    wxColour color = *wxBLUE;
	    wxColour colorC(255 - (int)color.Red(), 255 - (int)color.Green(), 255 - (int)color.Blue() );
	    pPaper->SetLogicalFunction(wxXOR);

        lmLUnits uLineLength = 2.5f * m_uSpacing;
        lmVStaff* pVStaff = (lmVStaff*)GetScoreOwner()->GetParentScoreObj();

 	    //wxLogMessage(_T("[lmShapeStaff::OnMouseEndMoving] VStaff=0x%x, nStaff=%d, OldSteps=%d, oldPos=%.2f"),
		    //         pVStaff, m_nStaff, m_nOldSteps, m_uxOldPos);

        lmDrawLegerLines(m_nOldSteps+10, m_uxOldPos, pVStaff, m_nStaff,
                        uLineLength, m_uBoundsTop.y, pPaper, colorC);
    }
    //else
	   // wxLogMessage(_T("[lmShapeStaff::OnMouseEndMoving] No ledger lines to remove") );

}

lmVStaff* lmShapeStaff::GetOwnerVStaff()
{
    return ((lmStaff*)m_pOwner)->GetOwnerVStaff();
}
