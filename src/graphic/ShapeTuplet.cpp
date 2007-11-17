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
#pragma implementation "ShapeTuplet.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "GMObject.h"
#include "../score/Score.h"
#include "ShapeNote.h"
#include "ShapeTuplet.h"


//-------------------------------------------------------------------------------------
// Implementation of lmShapeTuplet


lmShapeTuplet::lmShapeTuplet(lmNoteRest* pStartNR, lmNoteRest* pEndNR, int nNumNotes,
							 bool fAbove, bool fShowNumber, wxString sNumber, wxFont* pFont,
							 wxColour color, lmETupletDesign nDesign)
	: lmSimpleShape(eGMO_ShapeTuplet, pStartNR, _T("Tuplet bracket"))
{
	m_pStartNR = pStartNR;
	m_pEndNR = pEndNR;
	m_nNumNotes = nNumNotes;
    m_sNumber = sNumber;
	m_color = color;
	m_fAbove = fAbove;
    m_fShowNumber = fShowNumber;
    m_nTupletDesign = nDesign;
	m_pFont = pFont;

	//compute positions and bounds
	OnAttachmentPointMoved(pStartNR->GetShap2(), eGMA_StartNote, 0.0, 0.0, lmMOVE_EVENT);
	OnAttachmentPointMoved(pEndNR->GetShap2(), eGMA_EndNote, 0.0, 0.0, lmMOVE_EVENT);

}

lmShapeTuplet::~lmShapeTuplet()
{
}

void lmShapeTuplet::OnAttachmentPointMoved(lmShape* pShape, lmEAttachType nTag,
										   lmLUnits ux, lmLUnits uy, lmEParentEvent nEvent)
{
	WXUNUSED(ux);
	WXUNUSED(uy);
	WXUNUSED(nEvent);

	//if intermediate note moved, nothing to do
	if (!(nTag == eGMA_StartNote || nTag == eGMA_EndNote)) return;

	//computhe half notehead width
	lmShape* pSNH = ((lmShapeNote*)pShape)->GetNoteHead();
	wxASSERT(pSNH);
	lmLUnits uHalfNH = (pSNH->GetXRight() - pSNH->GetXLeft()) / 2.0;

	if (nTag == eGMA_StartNote)
	{
		//start note moved. Recompute start of shape
		//Placed on center of notehead if above, or half notehead before if below
		if (m_fAbove)
		{
			m_uxStart = pSNH->GetXLeft() + uHalfNH;
			m_uyStart = pShape->GetYTop();
		}
		else
		{
			m_uxStart = pSNH->GetXLeft() - uHalfNH;
			m_uyStart = pShape->GetYBottom();
		}
		SetXLeft(m_uxStart);
		SetYTop(m_uyStart);
	}

	else if (nTag == eGMA_EndNote)
	{
		//end note moved. Recompute end of shape
		//Placed half notehead appart if above, or on center of notehead if below
		if (m_fAbove)
		{
			m_uxEnd = pSNH->GetXRight() + uHalfNH;
			m_uyEnd = pShape->GetYTop();
		}
		else
		{
			m_uxEnd = pSNH->GetXRight() - uHalfNH;
			m_uyEnd = pShape->GetYBottom();
		}
		SetXRight(m_uxEnd);
		SetYBottom(m_uyEnd);
	}

	NormaliceBoundsRectangle();

}

void lmShapeTuplet::Render(lmPaper* pPaper, wxColour color)
{
	//get staff, for scaling logical units
    lmVStaff* pVStaff = m_pStartNR->GetVStaff();
    int nStaff = m_pStartNR->GetStaffNum();
    lmStaff* pStaff = pVStaff->GetStaff(nStaff);

	lmLUnits BORDER_LENGHT = pStaff->TenthsToLogical(10.0);
    lmLUnits BRACKET_DISTANCE = pStaff->TenthsToLogical(10.0);

    //Prepare pen
    lmLUnits uOldThick = pPaper->GetLineWidth();
    wxColour oldColor = pPaper->GetLineColor();
    lmLUnits uThick = pStaff->TenthsToLogical(2.0);    //! @todo user options
    pPaper->SetPen(color, uThick);

	lmLUnits yLineStart;
    lmLUnits yLineEnd;
    lmLUnits yStartBorder;
    lmLUnits yEndBorder;

    if (m_fAbove) {
        yLineStart = m_uyStart - BRACKET_DISTANCE;
        yLineEnd = m_uyEnd - BRACKET_DISTANCE;
        yStartBorder = yLineStart + BORDER_LENGHT;
        yEndBorder = yLineEnd + BORDER_LENGHT;
    } else {
        yLineStart = m_uyStart + BRACKET_DISTANCE;
        yLineEnd = m_uyEnd + BRACKET_DISTANCE;
        yStartBorder = yLineStart - BORDER_LENGHT;
        yEndBorder = yLineEnd - BORDER_LENGHT;
    }

	//number placement
    lmLUnits NUMBER_DISTANCE = pStaff->TenthsToLogical(5.0);
    lmLUnits nNumberWidth=0, nNumberHeight=0;
    if (m_fShowNumber) {
        pPaper->SetFont(*m_pFont);
        pPaper->GetTextExtent(m_sNumber, &nNumberWidth, &nNumberHeight);
    }

    lmLUnits xNumber = (m_uxStart + m_uxEnd - nNumberWidth)/2;
    lmLUnits yNumber = (yStartBorder + yEndBorder) / 2.0;
	yNumber += (m_fAbove ? -nNumberHeight : 0.0);

    //---------------------------------------------
	//horizontal line shape
    if (m_fShowNumber)
	{
		//horizontal broken line shape
		float rTanAlpha = (yLineEnd - yLineStart) / (m_uxEnd - m_uxStart);
		lmLUnits uxGap = pStaff->TenthsToLogical(3.0);
		lmLUnits x1 = xNumber - uxGap;
		lmLUnits y1 = yLineStart + (x1 - m_uxStart) * rTanAlpha;
		lmLUnits x2 = xNumber + nNumberWidth + uxGap;
		lmLUnits y2 = yLineStart + (x2 - m_uxStart) * rTanAlpha;
		pPaper->SolidLine(m_uxStart, yLineStart, x1, y1, uThick, eEdgeVertical, color);
		pPaper->SolidLine(x2, y2, m_uxEnd, yLineEnd, uThick, eEdgeVertical, color);
	}
	else
	{
		//horizontal line shape
		pPaper->SolidLine(m_uxStart, yLineStart, m_uxEnd, yLineEnd, uThick, eEdgeVertical, color);
	}

	//vertical borders
    lmLUnits x1 = m_uxStart + uThick / 2;
    lmLUnits x2 = m_uxEnd - uThick / 2;
    pPaper->SolidLine(x1, yLineStart, x1, yStartBorder, uThick, eEdgeNormal, color);
    pPaper->SolidLine(x2, yLineEnd, x2, yEndBorder, uThick, eEdgeNormal, color);

    pPaper->SetPen(oldColor, uOldThick);

    //write the number
    if (m_fShowNumber) {
        pPaper->SetTextForeground(color);
        pPaper->DrawText(m_sNumber, xNumber, yNumber);
    }

    lmShape::RenderCommon(pPaper);

}

wxString lmShapeTuplet::Dump(int nIndent)
{
	//TODO
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump += wxString::Format(_T("%04d %s: "),
		m_nId, m_sShapeName );
    sDump += DumpBounds();
    sDump += _T("\n");
	return sDump;
}

void lmShapeTuplet::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
    m_uxStart += xIncr;
	m_uyStart += yIncr;
    m_uxEnd += xIncr;
	m_uyEnd += yIncr;

    ShiftBoundsAndSelRec(xIncr, yIncr);
}

