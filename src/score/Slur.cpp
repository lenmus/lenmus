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
/*! @file Slur.cpp
    @brief Implementation file for classes lmArch and lmTie
    @ingroup score_kernel
*/
#ifdef __GNUG__
#pragma implementation "Slur.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Score.h"


//---------------------------------------------------------
//   lmArch
//---------------------------------------------------------

lmArch::lmArch()
{
    //mode       = 0;
    //_upMode    = 0;
    //up         = true;
}

void lmArch::SetStartPoint(lmLUnits xPos, lmLUnits yPos)
{
    m_xStart = xPos;
    m_yStart = yPos;
}

void lmArch::SetEndPoint(lmLUnits xPos, lmLUnits yPos)
{
    m_xEnd = xPos;
    m_yEnd = yPos;

}

void lmArch::SetCtrolPoint1(lmLUnits xPos, lmLUnits yPos)
{
    m_xCtrol1 = xPos;
    m_yCtrol1 = yPos;
}

void lmArch::SetCtrolPoint2(lmLUnits xPos, lmLUnits yPos)
{
    m_xCtrol2 = xPos;
    m_yCtrol2 = yPos;
}

void lmArch::Draw(lmPaper* pPaper, wxColour colorC)
{
    //wxLogMessage(_T("[lmArch::Draw]"));
    lmLUnits uWidth = lmToLogicalUnits(0.2, lmMILLIMETERS);         // width = 0.2 mm

    //lmArch is rendered as a cubic bezier curve. The number of points to draw is
    // variable, to suit a minimun resolution of 5 points / mm. 

    // determine number of interpolation points to use
    int nNumPoints = (m_xEnd - m_xStart) / lmToLogicalUnits(0.2, lmMILLIMETERS);
    if (nNumPoints < 5) nNumPoints = 5;

    // compute increment for mu variable
    double incr = 1.0 / (double)(nNumPoints-1);

    // start point
    double x1 = m_xStart;
    double y1 = m_yStart;

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
        x2 = a * m_xStart + b * m_xCtrol1 + c * m_xCtrol2 + d * m_xEnd;
        y2 = a * m_yStart + b * m_yCtrol1 + c * m_yCtrol2 + d * m_yEnd;

        // draw segment line
        pPaper->SolidLine(x1, y1, x2, y2, uWidth, eEdgeNormal, colorC);

        // prepare for next point
        x1 = x2;
        y1 = y2;
    }

}

void lmArch::CubicBezier(double* x, double* y, int nNumPoints)
{
    /*
    Four control point Bezier interpolation
    Cumputes nNumPoints interpolated between start and end of curve points
    */
    double mu, mum1, a, b, c, d;
    double incr = 1.0 / (double)(nNumPoints-1);
    int i;

    *x = m_xStart;
    *(x + nNumPoints-1) = m_xEnd;
    *y = m_yStart;
    *(y + nNumPoints-1) = m_yEnd;

    for (i=1, mu = incr; i < nNumPoints-1; i++, mu += incr) {
        mum1 = 1 - mu;
        a = mum1 * mum1 * mum1;
        b = 3 * mu * mum1 * mum1;
        c = 3 * mu * mu * mum1;
        d = mu * mu * mu;

        *(x+i) = a * m_xStart + b * m_xCtrol1 + c * m_xCtrol2 + d * m_xEnd;
        *(y+i) = a * m_yStart + b * m_yCtrol1 + c * m_yCtrol2 + d * m_yEnd;
    }

}


//---------------------------------------------------------
//   lmTie
//---------------------------------------------------------

lmTie::lmTie(lmNote* pStartNote, lmNote* pEndNote) : lmAuxObj(eTPO_Tie)
{
    m_pStartNote = pStartNote;
    m_pEndNote   = pEndNote;
    m_fTieUnderNote = false;
    m_pExtraArc = (lmArch*)NULL;    //default: tie is not splitted
}

lmTie::~lmTie()
{
    if (m_pExtraArc) delete m_pExtraArc;
}

void lmTie::Remove(lmNote* pNote)
{
    if (m_pStartNote == pNote) {
        m_pStartNote = (lmNote*)NULL;
        m_pEndNote->RemoveTie(this);
        m_pEndNote = (lmNote*)NULL;
    }
    else if (m_pEndNote == pNote) {
        m_pEndNote = (lmNote*)NULL;
        m_pStartNote->RemoveTie(this);
        m_pStartNote = (lmNote*)NULL;
    }
}

void lmTie::SetStartPoint(lmLUnits xPos, lmLUnits yPos, lmLUnits xPaperRight, bool fUnderNote)
{
    m_fTieUnderNote = fUnderNote;

    lmUPoint paperPos = m_pStartNote->GetOrigin();
    m_mainArc.SetStartPoint(xPos + paperPos.x, yPos + paperPos.y);
    m_xPaperRight = xPaperRight;

}

void lmTie::SetEndPoint(lmLUnits xPos, lmLUnits yPos, lmLUnits xPaperLeft)
{
    lmUPoint paperPosEnd = m_pEndNote->GetOrigin();
    lmLUnits xEnd = xPos + paperPosEnd.x;
    lmLUnits yEnd = yPos + paperPosEnd.y;
    m_mainArc.SetEndPoint(xEnd, yEnd);
    m_xPaperLeft = xPaperLeft;

    // check if the tie have to be splitted
    lmLUnits xStart, yStart;
    lmUPoint paperPosStart = m_pStartNote->GetOrigin();
    if (paperPosEnd.y != paperPosStart.y) {
        //if start note paperPos Y is not the same than end note paperPos Y the notes are
        //in different systems. Therefore, the tie must be splitted. Let's do it
        m_pExtraArc = new lmArch();

        //recompute the end of the first arc
        xEnd = m_xPaperRight;
        yEnd = m_mainArc.GetStartPosY();
        m_mainArc.SetEndPoint(xEnd, yEnd);

        //compute the start of the second arc
        xStart = m_xPaperLeft;
        yStart = yPos + paperPosEnd.y;
        m_pExtraArc->SetStartPoint(xStart, yStart);

        //comute the end of the second arc
        xEnd = xPos + paperPosEnd.x;
        yEnd = yStart;
        m_pExtraArc->SetEndPoint(xEnd, yEnd);
    }


    // compute the default control points for the first arc
    xStart = m_mainArc.GetStartPosX();
    yStart = m_mainArc.GetStartPosY();
    xEnd = m_mainArc.GetEndPosX();
    yEnd = m_mainArc.GetEndPosY();

    lmLUnits xCtrol = xStart + (xEnd - xStart) / 3;
    lmLUnits yDsplz = lmToLogicalUnits(2, lmMILLIMETERS);
    lmLUnits yCtrol = yStart + (m_fTieUnderNote ? yDsplz : -yDsplz);
    m_mainArc.SetCtrolPoint1(xCtrol, yCtrol);
    
    xCtrol += (xEnd - xStart) / 3;
    yCtrol = yEnd + (m_fTieUnderNote ? yDsplz : -yDsplz);
    m_mainArc.SetCtrolPoint2(xCtrol, yCtrol);


    // compute the default control points for the second arc, if it exists
    if (m_pExtraArc) {
        xStart = m_pExtraArc->GetStartPosX();
        yStart = m_pExtraArc->GetStartPosY();
        xEnd = m_pExtraArc->GetEndPosX();
        yEnd = m_pExtraArc->GetEndPosY();

        lmLUnits xCtrol = xStart + (xEnd - xStart) / 3;
        lmLUnits yCtrol = yStart + (m_fTieUnderNote ? yDsplz : -yDsplz);
        m_pExtraArc->SetCtrolPoint1(xCtrol, yCtrol);
        
        xCtrol += (xEnd - xStart) / 3;
        yCtrol = yEnd + (m_fTieUnderNote ? yDsplz : -yDsplz);
        m_pExtraArc->SetCtrolPoint2(xCtrol, yCtrol);
    }


    // with the tie end positioning points we have all data needed to update measurements
    UpdateMeasurements();
}

void lmTie::UpdateMeasurements()
{
    /*
    the position of one of the owner notes has changed. Update tie position and size
    */
    lmUPoint startOffset = m_pStartNote->GetOrigin();

    //! @todo Adjust selRect to sourround control points
    m_paperPos = startOffset;
    SetSelRectangle(m_mainArc.GetStartPosX() - m_paperPos.x,
                    m_mainArc.GetStartPosY() - m_paperPos.y,
                    m_mainArc.GetEndPosX() - m_mainArc.GetStartPosX(),
                    m_mainArc.GetEndPosY() - m_mainArc.GetStartPosY() + (m_fTieUnderNote ? 2000 : -2000));

}

void lmTie::DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC)
{
    /*
    Ties never appear in a lmVStaff. They are always part
    of a Note. Therefore, drawing is not controlled by the standard DrawObject() mechaninsm
    but is always controlled by the two owners Notes.

    It is too early to define an standard behaviour. In general:
      -    m_paperPos and m_selRect must be always updated as it is being used for selection.
        This implies that positioning and size information of AuxObjs associated to an
        lmStaffObj (i.e. Ties) must be updated when the owner lmStaffObj is updated (i.e. the
        formatting and justification process).

    For Ties:
      -    the operations to be performed during the measurement phase are done via
        invocation of the methods SetStartPoint() and  SetEndPoint().
      -    the drawing phase is done by invoking Draw()
    */

    if (fMeasuring) return;

    m_mainArc.Draw(pPaper, (m_fSelected ? g_pColors->ScoreSelected() : colorC));
    if (m_pExtraArc) m_pExtraArc->Draw(pPaper, (m_fSelected ? g_pColors->ScoreSelected() : colorC));

}
