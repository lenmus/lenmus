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
#pragma implementation "BoxSliceInstr.h"
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
// Implementation of lmBoxSliceInstr: a part (measure) of an instrument.
//


lmBoxSliceInstr::lmBoxSliceInstr(lmBoxSlice* pParent, lmInstrument* pInstr)
    : lmBox(pParent->GetScoreOwner(), eGMO_BoxSliceInstr, _T("SliceInstr"))
{
    m_pSlice = pParent;
    m_pInstr = pInstr;
}

lmBoxSliceInstr::~lmBoxSliceInstr()
{
}

lmBoxSliceVStaff* lmBoxSliceInstr::AddVStaff(lmVStaff* pVStaff, int nMeasure)
{
    lmBoxSliceVStaff* pBSV = new lmBoxSliceVStaff(this, pVStaff, nMeasure);
    AddBox(pBSV);
    return pBSV;
}

//void lmBoxSliceInstr::UpdateXLeft(lmLUnits xLeft)
//{
//	// During layout there is a need to update initial computations about this
//	// box slice position. This update must be propagated to all contained boxes
//
//    lmLUnits uIncr = GetXLeft() - xLeft;
//	SetXLeft(xLeft);
//    m_uLimitsTop.x += uIncr;
//
//	//propagate change
//    for (int i=0; i < (int)m_Boxes.size(); i++)
//    {
//        ((lmBoxSliceVStaff*)m_Boxes[i])->UpdateXLeft(xLeft);
//    }
//}

void lmBoxSliceInstr::CopyYBounds(lmBoxSliceInstr* pBSI)
{
	//This method is only invoked during layout phase, when the number of measures in the
	//system has been finally decided. There is a need to copy 'y' coordinates from first
	//slice to all others. This method receives the first instr.slice and must copy 'y'
	//coordinates from there

	SetYTop(pBSI->GetYTop());
	SetYBottom(pBSI->GetYBottom());

	//propagate request
    for (int i=0; i < (int)m_Boxes.size(); i++)
    {
        ((lmBoxSliceVStaff*)m_Boxes[i])->CopyYBounds(pBSI->GetSliceVStaff(i));
    }
}

void lmBoxSliceInstr::SelectGMObjects(bool fSelect, lmLUnits uXMin, lmLUnits uXMax,
                         lmLUnits uYMin, lmLUnits uYMax)
{
    //look up in this box
    lmBox::SelectGMObjects(fSelect, uXMin, uXMax, uYMin, uYMax);

    //loop to look up in the VStaff slices
    std::vector<lmBox*>::iterator it;
	for(it = m_Boxes.begin(); it != m_Boxes.end(); ++it)
    {
        ((lmBoxSliceVStaff*)(*it))->SelectGMObjects(fSelect, uXMin, uXMax, uYMin, uYMax);
    }
}

int lmBoxSliceInstr::GetPageNumber() const
{ 
	return m_pSlice->GetPageNumber();
}
