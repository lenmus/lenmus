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
#pragma implementation "ShapeNote.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "GMObject.h"
#include "../score/Score.h"

#include "ShapeNote.h"


//-------------------------------------------------------------------------------------
// Implementation of lmShapeNote: an staff (usually 5 lines)


lmShapeNote::lmShapeNote(lmNoteRest* pOwner, lmLUnits xLeft, lmLUnits yTop, wxColour color)
	: lmCompositeShape(pOwner, _T("Note"), eGMO_ShapeNote)
{
    m_uxLeft = xLeft;
    m_uyTop = yTop;
	m_color = color;

	//initializations
	m_nNoteHead = -1;		// -1 = no shape
	m_nStem = -1;

}

lmShapeNote::~lmShapeNote()
{
}

void lmShapeNote::AddStem(lmShape* pShape)
{
	m_nStem = Add(pShape);
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

	InformAttachedShapes();
}

lmShape* lmShapeNote::GetNoteHead()
{
	if (m_nNoteHead < 0)
		return (lmShape*)NULL;

	return GetShape(m_nNoteHead);
}

lmShape* lmShapeNote::GetStem()
{
	if (m_nStem < 0)
		return (lmShape*)NULL;

	return GetShape(m_nStem);
}

