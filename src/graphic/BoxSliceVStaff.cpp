//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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
#pragma implementation "BoxSliceVStaff.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "../score/Score.h"
#include "BoxScore.h"
#include "BoxPage.h"
#include "BoxSystem.h"
#include "BoxSlice.h"
#include "BoxSliceInstr.h"
#include "BoxSliceVStaff.h"

//access to colors
#include "../globals/Colors.h"
extern lmColors* g_pColors;

//-----------------------------------------------------------------------------------------
// Implementation of class lmBoxSliceVStaff: a part (measure) of a VStaff of an instrument.
//

lmBoxSliceVStaff::lmBoxSliceVStaff(lmBoxSliceInstr* pParent, lmVStaff* pVStaff, int nMeasure)
    : lmBox(pParent->GetScoreOwner(), eGMO_BoxSliceVStaff, _T("SliceVStaff"))
{
	m_nMeasure = nMeasure;
    m_pSliceInstr = pParent;
    m_pVStaff = pVStaff;
}


lmBoxSliceVStaff::~lmBoxSliceVStaff()
{
}

//void lmBoxSliceVStaff::Render(lmPaper* pPaper, lmUPoint uPos)
//{
//    //update selection rectangle
//    m_uSelRect = GetBounds();
//
//	//render shapes
//	RenderShapes(pPaper);
//
//    lmGMObject::Render(pPaper, *wxGREEN);
//}

lmBoxSystem* lmBoxSliceVStaff::GetOwnerSystem()
{
	return m_pSliceInstr->GetOwnerSystem();
}

void lmBoxSliceVStaff::UpdateXLeft(lmLUnits xLeft)
{
	// During layout there is a need to update initial computations about this
	// box slice position. This update must be propagated to all contained boxes

	SetXLeft(xLeft);
}

//void lmBoxSliceVStaff::UpdateXRight(lmLUnits xRight)
//{
//	// During layout there is a need to update initial computations about this
//	// box slice position. This update must be propagated to all contained boxes
//
//	SetXRight(xRight);
//}

void lmBoxSliceVStaff::CopyYBounds(lmBoxSliceVStaff* pBSV)
{
	//This method is only invoked during layout phase, when the number of measures in the
	//system has been finally decided. There is a need to copy 'y' coordinates from first
	//slice to all others. This method receives the first vstaff slice and must copy 'y'
	//coordinates from there

	SetYTop(pBSV->GetYTop());
	SetYBottom(pBSV->GetYBottom());
}

void lmBoxSliceVStaff::SelectGMObjects(bool fSelect, lmLUnits uXMin, lmLUnits uXMax,
                         lmLUnits uYMin, lmLUnits uYMax)
{
    //look up in this box
    lmBox::SelectGMObjects(fSelect, uXMin, uXMax, uYMin, uYMax);
}

int lmBoxSliceVStaff::GetPageNumber() const
{
	return m_pSliceInstr->GetPageNumber();
}

lmBoxScore* lmBoxSliceVStaff::GetOwnerBoxScore() 
{ 
    return m_pSliceInstr->GetOwnerBoxScore(); 
}

lmBoxPage* lmBoxSliceVStaff::GetOwnerBoxPage()
{ 
    return m_pSliceInstr->GetOwnerBoxPage(); 
}

