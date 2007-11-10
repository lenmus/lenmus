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

#ifndef __LM_SHAPEBEAM_H__        //to avoid nested includes
#define __LM_SHAPEBEAM_H__

#ifdef __GNUG__
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


class lmShapeBeam : public lmCompositeShape
{
public:
    lmShapeBeam(lmNoteRest* pOwner, bool fStemsDown, int nNumNotes, wxColour color = *wxBLACK);
	~lmShapeBeam();

	//creation
	void AddNoteRest(lmShapeNote* pShape, lmTBeamInfo* pBeamInfo[6]);

	//implementation of pure virtual methods in base class
    void Render(lmPaper* pPaper, lmUPoint uPos, wxColour color=*wxBLACK);
    void Shift(lmLUnits xIncr, lmLUnits yIncr);

	//layout changes
	void OnAttachmentPointMoved(lmShape* pShape, lmEAttachType nTag);


protected:
	void AdjustStems();

    //attributes for a beam
	bool		m_fStemsDown;
	int			m_nNumNotes;
    wxColour	m_color;

	typedef struct lmParentNote_Struct {
		lmShape*		pShape;
		lmTBeamInfo*	pBeamInfo[6];
	} lmParentNote;

	//list of notes in this beam
	std::vector<lmParentNote*>	m_cParentNotes;

};

#endif    // __LM_SHAPEBEAM_H__

