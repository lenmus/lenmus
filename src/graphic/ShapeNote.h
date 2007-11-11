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

#ifndef __LM_SHAPENOTE_H__        //to avoid nested includes
#define __LM_SHAPENOTE_H__

#ifdef __GNUG__
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

	//access to constituent shapes
	lmShape* GetNoteHead();
	lmShapeStem* GetStem();

	//access to info
	inline lmLUnits GetXEnd() const { return m_uxLeft + m_uWidth; }
	lmLUnits GetStemThickness();
	bool StemGoesDown();

	//re-layout
	void SetStemLength(lmLUnits uLength);


protected:
	//index to some important constituent shapes
	int		m_nNoteHead;
	int		m_nStem;

    //position
    lmLUnits    m_uxLeft;
    lmLUnits    m_uyTop;

	lmLUnits	m_uWidth;

    wxColour    m_color;


};

#endif    // __LM_SHAPENOTE_H__

