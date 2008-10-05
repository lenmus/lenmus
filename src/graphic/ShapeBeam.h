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

#ifndef __LM_SHAPEBEAM_H__        //to avoid nested includes
#define __LM_SHAPEBEAM_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ShapeBeam.cpp"
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
class lmShapeStem;
class lmShapeNote;
struct lmTBeamInfo;


class lmShapeBeam : public lmCompositeShape
{
public:
    lmShapeBeam(lmNoteRest* pOwner, bool fStemsDown=true, wxColour color = *wxBLACK);
	~lmShapeBeam();

	//creation
	void AddNoteRest(lmShapeStem* pStem, lmShape* pNoteRest, lmTBeamInfo* pBeamInfo);
	void SetStemsDown(bool fValue);

	//implementation of pure virtual methods in base class
    void Render(lmPaper* pPaper, wxColour color);
    void Shift(lmLUnits xIncr, lmLUnits yIncr);

    //overrides
    bool BoundsContainsPoint(lmUPoint& pointL);
    bool SelRectContainsPoint(lmUPoint& pointL);

	//layout changes
    void AdjustStemsIfNeeded();
	void OnAttachmentPointMoved(lmShape* pShape, lmEAttachType nTag,
								lmLUnits ux, lmLUnits uy, lmEParentEvent nEvent);


protected:
	void AdjustStems();
	lmShapeStem* GetStem(int iParentNote);
	int FindNoteShape(lmShapeNote* pShape);
	void SetStemLength(lmShapeStem* pStem, lmLUnits uLength);
	void DrawBeamSegment(lmPaper* pPaper, 
                         lmLUnits uxStart, lmLUnits uyStart,
                         lmLUnits uxEnd, lmLUnits uyEnd, lmLUnits uThickness,
                         lmShapeNote* pStartNote, lmShapeNote* pEndNote,
                         wxColour color);

    //attributes for a beam
	bool		m_fStemsDown;

	//other
	bool		m_fLayoutPending;		//to optimize re-layouts

	typedef struct lmParentNote_Struct {
		lmShape*		pShape;			//ptr. to parent ShapeNote or Rest
		int				nStem;			//index to shapes collection. -1 if no stem (rests)
		lmTBeamInfo*	pBeamInfo;		//beaming info from parent note
	} lmParentNote;

	//list of notes in this beam
	std::vector<lmParentNote*>	m_cParentNotes;

};

#endif    // __LM_SHAPEBEAM_H__

