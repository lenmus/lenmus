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
#pragma implementation "ShapeArch.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "../score/Score.h"
#include "GMObject.h"
#include "ShapeNote.h"
#include "ShapeArch.h"


//========================================================================================
// lmShapeArch object implementation
//========================================================================================

lmShapeArch::lmShapeArch(lmScoreObj* pOwner, lmUPoint uStart, lmUPoint uEnd, bool fArchUnder,
                wxColour nColor, wxString sName)
    : lmSimpleShape(eGMO_ShapeArch, pOwner, sName)
{
    m_uStart = uStart;
    m_uEnd = uEnd;
    m_fArchUnder = fArchUnder;
    m_color = nColor;

    SetDefaultControlPoints();
    Create();
}

lmShapeArch::lmShapeArch(lmScoreObj* pOwner, lmUPoint uStart, lmUPoint uEnd,
                         lmUPoint uCtrol1, lmUPoint uCtrol2, wxColour nColor,
                         wxString sName)
    : lmSimpleShape(eGMO_ShapeArch, pOwner, sName)
{
    m_uStart = uStart;
    m_uEnd = uEnd;
    m_uCtrol1 = uCtrol1;
    m_uCtrol2 = uCtrol2;
    m_fArchUnder = (m_uStart.y < m_uCtrol1.y);
    m_color = nColor;

    Create();
}

lmShapeArch::lmShapeArch(lmScoreObj* pOwner, bool fArchUnder, wxColour nColor,
                         wxString sName)
    : lmSimpleShape(eGMO_ShapeArch, pOwner, sName)
{
    m_uStart = lmUPoint(0.0, 0.0);
    m_uEnd = lmUPoint(0.0, 0.0);
    m_uCtrol1 = lmUPoint(0.0, 0.0);
    m_uCtrol2 = lmUPoint(0.0, 0.0);
    m_fArchUnder = fArchUnder;
    m_color = nColor;

    Create();
}

void lmShapeArch::Create()
{
    // store boundling rectangle position and size
    //TODO
	//NormaliceBoundsRectangle();

 //   // store selection rectangle position and size
	//m_uSelRect = GetBounds();

}

wxString lmShapeArch::Dump(int nIndent)
{
    //TODO
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump += wxString::Format(_T("%04d %s: start=(%.2f, %.2f), end=(%.2f, %.2f), ")
                _T("ctrol1=(%.2f, %.2f), ctrol2=(%.2f, %.2f), ")
                _T("Arch under note = %s, "),
                m_nId, m_sGMOName, m_uStart.x, m_uStart.y, m_uEnd.x, m_uEnd.y,
                m_uCtrol1.x, m_uCtrol1.y, m_uCtrol2.x, m_uCtrol2.y,
                (m_fArchUnder ? _T("yes") : _T("no")) );
    sDump += DumpBounds();
    sDump += _T("\n");
	return sDump;
}

void lmShapeArch::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
    m_uStart.x += xIncr;
    m_uStart.y += yIncr;
    m_uEnd.x += xIncr;
    m_uEnd.y += yIncr;
    m_uCtrol1.x += xIncr;
    m_uCtrol1.y += yIncr;
    m_uCtrol2.x += xIncr;
    m_uCtrol2.y += yIncr;

    ShiftBoundsAndSelRec(xIncr, yIncr);
}

void lmShapeArch::SetStartPoint(lmLUnits xPos, lmLUnits yPos)
{
    m_uStart.x = xPos;
    m_uStart.y = yPos;
    SetDefaultControlPoints();
}

void lmShapeArch::SetEndPoint(lmLUnits xPos, lmLUnits yPos)
{
    m_uEnd.x = xPos;
    m_uEnd.y = yPos;
    SetDefaultControlPoints();
}

void lmShapeArch::SetCtrolPoint1(lmLUnits xPos, lmLUnits yPos)
{
    m_uCtrol1.x = xPos;
    m_uCtrol1.y = yPos;
}

void lmShapeArch::SetCtrolPoint2(lmLUnits xPos, lmLUnits yPos)
{
    m_uCtrol2.x = xPos;
    m_uCtrol2.y = yPos;
}

void lmShapeArch::Render(lmPaper* pPaper, wxColour colorC)
{
    //wxLogMessage(_T("[lmShapeArch::Render]"));
    lmLUnits uWidth = lmToLogicalUnits(0.2, lmMILLIMETERS);         // width = 0.2 mm

    //lmShapeArch is rendered as a cubic bezier curve. The number of points to draw is
    // variable, to suit a minimun resolution of 5 points / mm.

    // determine number of interpolation points to use
    int nNumPoints = (int)((m_uEnd.x - m_uStart.x) / lmToLogicalUnits(0.2, lmMILLIMETERS) );
    if (nNumPoints < 5) nNumPoints = 5;

    // compute increment for mu variable
    double incr = 1.0 / (double)(nNumPoints-1);

    // start point
    double x1 = m_uStart.x;
    double y1 = m_uStart.y;

    //take the opportunity to compute bounds limits
    double xMin = x1 , yMin = y1;
    double xMax = x1 , yMax = y1;

    // loop to compute bezier curve points and draw segment lines
    int i;
    double mu, mum1, a, b, c, d;
    double x2, y2;
    for (i=1, mu = incr; i < nNumPoints-1; i++, mu += incr) {
        mum1 = 1 - mu;
        a = mum1 * mum1 * mum1;
        b = 3 * mu * mum1 * mum1;
        c = 3 * mu * mu * mum1;
        d = mu * mu * mu;

        // compute next point
        x2 = a * m_uStart.x + b * m_uCtrol1.x + c * m_uCtrol2.x + d * m_uEnd.x;
        y2 = a * m_uStart.y + b * m_uCtrol1.y + c * m_uCtrol2.y + d * m_uEnd.y;

        // draw segment line
        pPaper->SolidLine(x1, y1, x2, y2, uWidth, eEdgeNormal, colorC);

        //update bounds
        xMin = wxMin(xMin, x2);
        yMin = wxMin(yMin, y2);
        xMax = wxMax(xMax, x2);
        yMax = wxMax(yMax, y2);

        // prepare for next point
        x1 = x2;
        y1 = y2;
    }

    //Update bounds rectangle
    SetXLeft(xMin);
    SetYTop(yMin);
    SetXRight(xMax);
    SetYBottom(yMax);

    RenderCommon(pPaper);

}

void lmShapeArch::CubicBezier(double* x, double* y, int nNumPoints)
{
    // Four control points Bezier interpolation
    // Computes nNumPoints between start and end of curve points

    double mu, mum1, a, b, c, d;
    double incr = 1.0 / (double)(nNumPoints-1);
    int i;

    *x = m_uStart.x;
    *(x + nNumPoints-1) = m_uEnd.x;
    *y = m_uStart.y;
    *(y + nNumPoints-1) = m_uEnd.y;

    for (i=1, mu = incr; i < nNumPoints-1; i++, mu += incr) {
        mum1 = 1 - mu;
        a = mum1 * mum1 * mum1;
        b = 3 * mu * mum1 * mum1;
        c = 3 * mu * mu * mum1;
        d = mu * mu * mu;

        *(x+i) = a * m_uStart.x + b * m_uCtrol1.x + c * m_uCtrol2.x + d * m_uEnd.x;
        *(y+i) = a * m_uStart.y + b * m_uCtrol1.y + c * m_uCtrol2.y + d * m_uEnd.y;
    }

}

void lmShapeArch::SetDefaultControlPoints()
{
    // compute the default control points for the arc
    m_uCtrol1.x = m_uStart.x + (m_uEnd.x - m_uStart.x) / 3;
    lmLUnits yDsplz = lmToLogicalUnits(2, lmMILLIMETERS);
    m_uCtrol1.y = m_uStart.y + (m_fArchUnder ? yDsplz : -yDsplz);

    m_uCtrol2.x = m_uCtrol1.x + (m_uEnd.x - m_uStart.x) / 3;
    m_uCtrol2.y = m_uEnd.y + (m_fArchUnder ? yDsplz : -yDsplz);

}




//========================================================================================
// lmShapeTie implementation
//========================================================================================


lmShapeTie::lmShapeTie(lmNote* pOwner, lmShapeNote* pShapeStart, lmShapeNote* pShapeEnd,
                       bool fTieUnderNote, wxColour color)
    : lmShapeArch(pOwner, fTieUnderNote, color, _T("Tie"))
{
    //store parameters
    m_fTieUnderNote = fTieUnderNote;

    //compute the arch
    OnAttachmentPointMoved(pShapeStart, eGMA_StartNote, 0.0, 0.0, lmSHIFT_EVENT);
    OnAttachmentPointMoved(pShapeEnd, eGMA_EndNote, 0.0, 0.0, lmSHIFT_EVENT);

}

lmShapeTie::~lmShapeTie()
{
}

void lmShapeTie::Render(lmPaper* pPaper, wxColour color)
{
    lmShapeArch::Render(pPaper, color);
}

void lmShapeTie::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
}

void lmShapeTie::OnAttachmentPointMoved(lmShape* pShape, lmEAttachType nTag,
								lmLUnits ux, lmLUnits uy, lmEParentEvent nEvent)
{
    //start or end note moved. Recompute start/end of tie and, if necessary, split
    //the tie.

	//WXUNUSED(ux);
	//WXUNUSED(uy);
	//WXUNUSED(nEvent);

	//get notehead shape
	lmShape* pSNH = ((lmShapeNote*)pShape)->GetNoteHead();
	wxASSERT(pSNH);

	//Compute new attachment point 
	lmLUnits uHalfNH = (pSNH->GetXRight() - pSNH->GetXLeft()) / 2.0;
    lmLUnits uHeightNH = pSNH->GetYBottom() - pSNH->GetYTop();
    lmLUnits uxPos = pSNH->GetXLeft() + uHalfNH;
    lmLUnits uyPos = ((lmStaffObj*)m_pOwner)->TenthsToLogical(5.0);
    uyPos = (m_fTieUnderNote ?
            pSNH->GetYTop() + uHeightNH + uyPos : pSNH->GetYTop() - uyPos );

    //update arch start/end points
	if (nTag == eGMA_StartNote)
        SetStartPoint(uxPos, uyPos);
	else if (nTag == eGMA_EndNote)
        SetEndPoint(uxPos, uyPos);


	//NormaliceBoundsRectangle();

//    // check if the tie have to be splitted
//    lmLUnits xStart, yStart;
//    lmUPoint paperPosStart = m_pStartNote->GetOrigin();
//    if (paperPosEnd.y != paperPosStart.y) {
//        //if start note paperPos Y is not the same than end note paperPos Y the notes are
//        //in different systems. Therefore, the tie must be splitted. Let's do it
//        m_pExtraArc = new lmArch();
//
//        //recompute the end of the first arc
//        xEnd = m_xPaperRight;
//        yEnd = m_mainArc.GetStartPosY();
//        m_mainArc.SetEndPoint(xEnd, yEnd);
//
//        //compute the start of the second arc
//        xStart = m_xPaperLeft;
//        yStart = yPos + paperPosEnd.y;
//        m_pExtraArc->SetStartPoint(xStart, yStart);
//
//        //comute the end of the second arc
//        xEnd = xPos + paperPosEnd.x;
//        yEnd = yStart;
//        m_pExtraArc->SetEndPoint(xEnd, yEnd);
//    }
//
//
//    // compute the default control points for the first arc
//    xStart = m_mainArc.GetStartPosX();
//    yStart = m_mainArc.GetStartPosY();
//    xEnd = m_mainArc.GetEndPosX();
//    yEnd = m_mainArc.GetEndPosY();
//
//    lmLUnits xCtrol = xStart + (xEnd - xStart) / 3;
//    lmLUnits yDsplz = lmToLogicalUnits(2, lmMILLIMETERS);
//    lmLUnits yCtrol = yStart + (m_fTieUnderNote ? yDsplz : -yDsplz);
//    m_mainArc.SetCtrolPoint1(xCtrol, yCtrol);
//
//    xCtrol += (xEnd - xStart) / 3;
//    yCtrol = yEnd + (m_fTieUnderNote ? yDsplz : -yDsplz);
//    m_mainArc.SetCtrolPoint2(xCtrol, yCtrol);
//
//
//    // compute the default control points for the second arc, if it exists
//    if (m_pExtraArc) {
//        xStart = m_pExtraArc->GetStartPosX();
//        yStart = m_pExtraArc->GetStartPosY();
//        xEnd = m_pExtraArc->GetEndPosX();
//        yEnd = m_pExtraArc->GetEndPosY();
//
//        lmLUnits xCtrol = xStart + (xEnd - xStart) / 3;
//        lmLUnits yCtrol = yStart + (m_fTieUnderNote ? yDsplz : -yDsplz);
//        m_pExtraArc->SetCtrolPoint1(xCtrol, yCtrol);
//
//        xCtrol += (xEnd - xStart) / 3;
//        yCtrol = yEnd + (m_fTieUnderNote ? yDsplz : -yDsplz);
//        m_pExtraArc->SetCtrolPoint2(xCtrol, yCtrol);
//    }

}

