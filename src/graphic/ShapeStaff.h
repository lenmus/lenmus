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

#ifndef __LM_SHAPESTAFF_H__        //to avoid nested includes
#define __LM_SHAPESTAFF_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ShapeStaff.cpp"
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

class lmStaff;


class lmShapeStaff : public lmSimpleShape
{
public:
	lmShapeStaff(lmStaff* pOwner, int nNumLines, lmLUnits uLineWidth, lmLUnits uSpacing,
				 lmLUnits xLeft, lmLUnits yTop, lmLUnits xRight, wxColour color);

	~lmShapeStaff();

	//implementation of pure virtual methods in base class
    void Render(lmPaper* pPaper, wxColour color);
	void Render(lmPaper* pPaper) { Render(pPaper, m_color); }
    wxString Dump(int nIndent);
    void Shift(lmLUnits xIncr, lmLUnits yIncr);

protected:

	int			m_nNumLines;		//how many staff lines
	lmLUnits	m_uLineWidth;		//staff lines width
	lmLUnits	m_uSpacing;			//spacing between lines

};

#endif    // __LM_SHAPESTAFF_H__

