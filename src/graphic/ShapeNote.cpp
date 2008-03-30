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
#pragma implementation "ShapeNote.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "GMObject.h"
#include "../score/Score.h"
#include "../app/ScoreCanvas.h"

#include "ShapeNote.h"


//-------------------------------------------------------------------------------------
// Implementation of lmShapeNote: an staff (usually 5 lines)


lmShapeNote::lmShapeNote(lmNoteRest* pOwner, lmLUnits xLeft, lmLUnits yTop, wxColour color)
	: lmCompositeShape(pOwner, _T("Note"), lmDRAGGABLE, eGMO_ShapeNote)
{
    m_uxLeft = xLeft;
    m_uyTop = yTop;
	m_color = color;

	//initializations
	m_nNoteHead = -1;		// -1 = no shape
	m_pBeamShape = (lmShapeBeam*)NULL;
	m_pStemShape = (lmShapeStem*)NULL;

}

lmShapeNote::~lmShapeNote()
{
}

void lmShapeNote::AddStem(lmShapeStem* pShape)
{
	Add(pShape);
	m_pStemShape = pShape;
}

void lmShapeNote::AddNoteHead(lmShape* pShape)
{
	m_nNoteHead = Add(pShape);
}

void lmShapeNote::AddFlag(lmShape* pShape)
{
	Add(pShape);
}

void lmShapeNote::AddAccidental(lmShape* pShape)
{
	Add(pShape);
}

void lmShapeNote::AddNoteInBlock(lmShape* pShape)
{
	Add(pShape);
}

void lmShapeNote::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
	lmCompositeShape::Shift(xIncr, yIncr);

	m_uxLeft += xIncr;
    m_uyTop += yIncr;

	InformAttachedShapes(xIncr, yIncr, lmSHIFT_EVENT);

	//if included in a composite shape update parent bounding and selection rectangles
	if (this->IsChildShape())
		((lmCompositeShape*)GetParentShape())->RecomputeBounds();
}

lmShape* lmShapeNote::GetNoteHead()
{
	if (m_nNoteHead < 0)
		return (lmShape*)NULL;

	return GetShape(m_nNoteHead);
}

void lmShapeNote::SetStemLength(lmLUnits uLength)
{
	lmShapeStem* pStem = GetStem();
	if (!pStem) return;


	if (StemGoesDown())
	{
		//adjust bottom point
		pStem->SetLength(uLength, false);
	}
	else
	{
		//adjust top point
		pStem->SetLength(uLength, true);
	}

	RecomputeBounds();

}

lmLUnits lmShapeNote::GetStemThickness()
{
	lmShapeStem* pStem = GetStem();
	if (!pStem) return 0.0;

	return pStem->GetXRight() - pStem->GetXLeft();
}

bool lmShapeNote::StemGoesDown()
{
	return ((lmNote*)m_pOwner)->StemGoesDown();
}

wxBitmap* lmShapeNote::OnBeginDrag(double rScale)
{
	return lmCompositeShape::OnBeginDrag(rScale);
}

lmUPoint lmShapeNote::OnDrag(lmPaper* pPaper, const lmUPoint& uPos)
{
	// The view informs that the user continues dragging. We receive the new desired
	// shape position and we must return the new allowed shape position.
	//
	// The default behaviour is to return the received position, so the view redraws 
	// the drag image at that position. No action must be performed by the shape on 
	// the score and score objects.
	//
	// The received new desired shape position is in logical units and referred to page
	// origin. The returned new allowed shape position must also be in in logical units
	// and referred to page origin.

	if (g_fFreeMove) return uPos;

    // A note only can be moved in discrete vertical steps (staff lines/spaces)
    //return lmUPoint(uPos.x, GetYTop());
    return lmUPoint(uPos.x, uPos.y);

}

void lmShapeNote::OnEndDrag(lmController* pCanvas, const lmUPoint& uPos)
{
	// End drag. Receives the command processor associated to the view and the
	// final position of the object (logical units referred to page origin).
	// This method must validate/adjust final position and, if ok, it must 
	// send a move object command to the controller.

	lmUPoint uFinalPos(uPos.x, uPos.y);
	if (!g_fFreeMove)
	{
		//free movement not allowed. Must end on a staff line/space
		//TODO: constrain movement to lines/spaces and change pitch
		uFinalPos.y = GetYTop();
	}

	////correct glyph displacement
	//uFinalPos.x += m_uGlyphPos.x - GetXLeft();
	//uFinalPos.y += m_uGlyphPos.y - GetYTop();

	//send a move object command to the controller
	pCanvas->MoveObject(this, uFinalPos);

}


