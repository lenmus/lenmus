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
#include "../graphic/GMObject.h"
#include "../graphic/ShapeNote.h"
#include "../graphic/ShapeArch.h"


//---------------------------------------------------------
//   lmTie
//---------------------------------------------------------

lmTie::lmTie(lmNote* pStartNote, lmNote* pEndNote)
{
    m_pStartNote = pStartNote;
    m_pEndNote   = pEndNote;
}

lmTie::~lmTie()
{
}

lmShape* lmTie::LayoutObject(lmBox* pBox, lmPaper* pPaper, wxColour color)
{
    //prepare information
    lmShapeNote* pShapeStart = (lmShapeNote*)m_pStartNote->GetShap2();
    lmShapeNote* pShapeEnd = (lmShapeNote*)m_pEndNote->GetShap2();
    bool fTieUnderNote = !m_pStartNote->StemGoesDown();

	//create the shape
    lmShapeTie* pShape = new lmShapeTie(m_pStartNote, pShapeStart, pShapeEnd,
                                        fTieUnderNote, color);
	pBox->AddShape(pShape);

	//attach the tie to start and end notes
	pShapeStart->Attach(pShape, eGMA_StartNote);
	pShapeEnd->Attach(pShape, eGMA_EndNote);

	return pShape;
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

void lmTie::PropagateNotePitchChange(lmNote* pNote, int nStep, int nOctave, int nAlter, bool fForward)
{
    if (pNote == m_pStartNote && fForward) {
        // propagate forwards
        m_pEndNote->PropagateNotePitchChange(nStep, nOctave, nAlter, lmFORWARDS);
    }
    else if (pNote == m_pEndNote && !fForward) {
        // propagate backwards
        m_pStartNote->PropagateNotePitchChange(nStep, nOctave, nAlter, lmBACKWARDS);
    }
    //other cases are for notes whose pitch is already changed 
}

//void lmTie::SetStartPoint(lmLUnits xPos, lmLUnits yPos, lmLUnits xPaperRight, bool fUnderNote)
//{
//    m_fTieUnderNote = fUnderNote;
//
//    lmUPoint paperPos = m_pStartNote->GetOrigin();
//    m_mainArc.SetStartPoint(xPos + paperPos.x, yPos + paperPos.y);
//    m_xPaperRight = xPaperRight;
//
//}
//
//void lmTie::SetEndPoint(lmLUnits xPos, lmLUnits yPos, lmLUnits xPaperLeft)
//{
//    lmUPoint paperPosEnd = m_pEndNote->GetOrigin();
//    lmLUnits xEnd = xPos + paperPosEnd.x;
//    lmLUnits yEnd = yPos + paperPosEnd.y;
//    m_mainArc.SetEndPoint(xEnd, yEnd);
//    m_xPaperLeft = xPaperLeft;
//
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
//
//
//    // with the tie end positioning points we have all data needed to update measurements
//    UpdateMeasurements();
//}
//
//void lmTie::UpdateMeasurements()
//{
//    /*
//    the position of one of the owner notes has changed. Update tie position and size
//    */
//    lmUPoint startOffset = m_pStartNote->GetOrigin();
//
//    //TODO Adjust selRect to sourround control points
//    m_uPaperPos = startOffset;
//    SetSelRectangle(m_mainArc.GetStartPosX() - m_uPaperPos.x,
//                    m_mainArc.GetStartPosY() - m_uPaperPos.y,
//                    m_mainArc.GetEndPosX() - m_mainArc.GetStartPosX(),
//                    m_mainArc.GetEndPosY() - m_mainArc.GetStartPosY() + (m_fTieUnderNote ? 2000 : -2000));
//
//}

