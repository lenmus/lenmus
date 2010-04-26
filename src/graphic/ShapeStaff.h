//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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

#ifndef __LM_SHAPESTAFF_H__        //to avoid nested includes
#define __LM_SHAPESTAFF_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ShapeStaff.cpp"
#endif

// For compilers that support precompilation, includes <wx/wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "../score/defs.h"      // lmLUnits
#include "../app/Paper.h"
#include "GMObject.h"

class lmStaff;


class lmShapeStaff : public lmSimpleShape
{
public:
	lmShapeStaff(lmStaff* pOwner, int nStaff, int nNumLines, lmLUnits uLineWidth,
				 lmLUnits uSpacing, lmLUnits xLeft, lmLUnits yTop, lmLUnits xRight,
				 wxColour color);

	~lmShapeStaff();

	//implementation of pure virtual methods in base class
    void Render(lmPaper* pPaper, wxColour color);
	void Render(lmPaper* pPaper) { Render(pPaper, m_color); }
    wxString Dump(int nIndent);
    void Shift(lmLUnits xIncr, lmLUnits yIncr);

	//ownership and related info
	inline int GetNumStaff() { return m_nStaff; }

    //adding notes/rest with mouse
    lmUPoint OnMouseStartMoving(lmPaper* pPaper, const lmUPoint& uPos);
    lmUPoint OnMouseMoving(lmPaper* pPaper, const lmUPoint& uPos);
	void OnMouseEndMoving(lmPaper* pPaper, lmUPoint uPagePos);


    //other
    int GetLineSpace(lmLUnits uyPos);
    lmVStaff* GetOwnerVStaff();

protected:
	int			m_nStaff;			//num of staff in the owner VStaff
	int			m_nNumLines;		//how many staff lines
	lmLUnits	m_uLineWidth;		//staff lines width
	lmLUnits	m_uSpacing;			//spacing between lines

    //temporary data to be used when mouse tool moving over the staff
    int         m_nOldSteps;		//to clear leger lines while dragging
    lmLUnits    m_uxOldPos;
    int         m_nPosOnStaff;		//line/space on staff on which this note is placed
};

#endif    // __LM_SHAPESTAFF_H__

