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

#ifndef __LM_SHAPENOTE_H__        //to avoid nested includes
#define __LM_SHAPENOTE_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ShapeNote.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../score/defs.h"      // lmLUnits
#include "../app/Paper.h"
#include "GMObject.h"

class lmNoteRest;
class lmController;
class lmShapeBeam;
class lmShapeStem;


class lmShapeNote : public lmCompositeShape
{
public:
    lmShapeNote(lmNoteRest* pOwner, lmLUnits xPos, lmLUnits yTop, wxColour color);

	~lmShapeNote();

	//overrides of virtual methods in base class
	void Shift(lmLUnits xIncr, lmLUnits yIncr);

	//specific methods
	void AddStem(lmShapeStem* pShape);
	void AddNoteHead(lmShape* pShape);
	void AddFlag(lmShape* pShape);
	void AddAccidental(lmShape* pShape);
	void AddNoteInBlock(lmShape* pShape);

	//info about related shapes
	inline void SetBeamShape(lmShapeBeam* pBeamShape) { m_pBeamShape = pBeamShape; }
	inline lmShapeBeam* GetBeamShape() const { return m_pBeamShape; }
	inline void SetStemShape(lmShapeStem* pStemShape) { m_pStemShape = pStemShape; }

	//access to constituent shapes
	lmShape* GetNoteHead();
	inline lmShapeStem* GetStem() const { return m_pStemShape; }

	//access to info
	inline lmLUnits GetXEnd() const { return m_uxLeft + m_uWidth; }
	lmLUnits GetStemThickness();
	bool StemGoesDown();

	//re-layout
	void SetStemLength(lmLUnits uLength);

	//dragging
    wxBitmap* OnBeginDrag(double rScale, wxDC* pDC);
	lmUPoint OnDrag(lmPaper* pPaper, const lmUPoint& uPos);
	void OnEndDrag(lmController* pCanvas, const lmUPoint& uPos);



protected:
	//index to some important constituent shapes
	int		m_nNoteHead;

    //position
    lmLUnits    m_uxLeft;
    lmLUnits    m_uyTop;

	lmLUnits	m_uWidth;

	//related shapes
	lmShapeBeam*	m_pBeamShape;
	lmShapeStem*	m_pStemShape;


};

#endif    // __LM_SHAPENOTE_H__

