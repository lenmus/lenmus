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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "BoxSliceInstr.h"
#endif

// For compilers that support precompilation, includes <wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "../score/Score.h"
#include "../score/Instrument.h"
#include "BoxScore.h"
#include "BoxPage.h"
#include "BoxSystem.h"
#include "BoxSlice.h"
#include "BoxSliceInstr.h"

//access to colors
#include "../globals/Colors.h"
extern lmColors* g_pColors;


//-----------------------------------------------------------------------------------------
// Implementation of lmBoxSliceInstr: a part (measure) of an instrument.
//


lmBoxSliceInstr::lmBoxSliceInstr(lmBoxSlice* pParent, lmInstrument* pInstr)
    : lmBox(pParent->GetScoreOwner(), eGMO_BoxSliceInstr, _T("SliceInstr"))
    , m_pSlice(pParent)
    , m_pInstr(pInstr)
{
}

lmBoxSliceInstr::~lmBoxSliceInstr()
{
}

int lmBoxSliceInstr::GetPageNumber() const
{ 
	return m_pSlice->GetPageNumber();
}

lmShapeStaff* lmBoxSliceInstr::GetStaffShape(int nStaff)
{ 
    //nStaff = 1..n

    wxASSERT(nStaff > 0 && nStaff <= m_pInstr->GetNumStaves());

    return GetOwnerSystem()->GetStaffShape(m_pInstr, nStaff);
}

lmShapeStaff* lmBoxSliceInstr::GetNearestStaff(lmUPoint& uPoint)
{
    //returns the nearest staff to point uPoint

    return GetOwnerSystem()->GetStaffShape(m_pInstr, uPoint);
}

void lmBoxSliceInstr::DrawTimeGrid(lmPaper* pPaper)
{ 
	//as painting uses XOR we need the complementary color
	wxColour color(192,192,192);    //TODO: User option
	wxColour colorC(255 - (int)color.Red(), 255 - (int)color.Green(), 255 - (int)color.Blue() );
	pPaper->SetLogicalFunction(wxXOR);

    //Draw the limits rectangle
    lmUPoint uTopLeft(m_uBoundsTop.x - m_uLeftSpace, m_uBoundsTop.y - m_uTopSpace);
    lmUSize uSize( GetWidth() + m_uLeftSpace + m_uRightSpace,
                   GetHeight() + m_uTopSpace + m_uBottomSpace );
    //wxLogMessage(_T("[lmBoxSliceInstr::DrawTimeGrid] rect=(%.2f, %.2f, %.2f, %.2f)"),
    //             uTopLeft.x, uTopLeft.y, uTopLeft.x+uSize.GetWidth(), uTopLeft.y+uSize.GetHeight() );
    pPaper->SketchRectangle(uTopLeft, uSize, colorC);

    //draw vertical lines for existing times
    ((lmBoxSlice*)GetParentBox())->DrawTimeLines( pPaper, colorC, uTopLeft.y,
                                                  uTopLeft.y + uSize.GetHeight() );
}

void lmBoxSliceInstr::DrawMeasureFrame(lmPaper* pPaper)
{ 
	//as painting uses XOR we need the complementary color
	wxColour color(255,0,0);    //TODO: User option
	wxColour colorC(255 - (int)color.Red(), 255 - (int)color.Green(), 255 - (int)color.Blue() );
	pPaper->SetLogicalFunction(wxXOR);

    //determine first and last staves
    lmShape* pFirstStaff = (lmShape*)GetStaffShape(1);
    lmShape* pLastStaff = (lmShape*)GetStaffShape( m_pInstr->GetNumStaves() );
    lmLUnits yTop = pFirstStaff->GetBounds().GetLeftTop().y;
    lmLUnits dyHalfLine = (pFirstStaff->GetBounds().GetLeftBottom().y - yTop) / 8.0;
    lmLUnits yBottom = pLastStaff->GetBounds().GetLeftBottom().y;
    yTop -= dyHalfLine;
    yBottom += dyHalfLine;

    //Draw the limits rectangle
    lmUPoint uTopLeft(m_uBoundsTop.x - m_uLeftSpace, yTop);
    lmUSize uSize( GetWidth() + m_uLeftSpace + m_uRightSpace, yBottom - yTop );
    pPaper->SketchRectangle(uTopLeft, uSize, colorC);
}



