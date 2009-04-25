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
#pragma implementation "BoxSlice.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "../score/Score.h"
#include "BoxSlice.h"
#include "BoxSystem.h"
#include "BoxSliceInstr.h"

//access to colors
#include "../globals/Colors.h"
extern lmColors* g_pColors;

//
// Class lmBoxSlice represents a sytem measure
//

//-----------------------------------------------------------------------------------------

lmBoxSlice::lmBoxSlice(lmBoxSystem* pParent, int nAbsMeasure, int nNumInSystem,
					   lmLUnits xStart, lmLUnits xEnd)
    : lmBox(pParent->GetScoreOwner(), eGMO_BoxSlice, _T("slice"))
{
    m_pBSystem = pParent;
    m_nAbsMeasure = nAbsMeasure;
	m_nNumInSystem = nNumInSystem;
    m_xStart = xStart;
    m_xEnd = xEnd;
}

lmBoxSlice::~lmBoxSlice()
{
}

lmBoxSliceInstr* lmBoxSlice::AddInstrument(lmInstrument* pInstr)
{
    lmBoxSliceInstr* pBSI = new lmBoxSliceInstr(this, pInstr);
    AddBox(pBSI);
    return pBSI;
}

lmBoxSlice* lmBoxSlice::FindMeasureAt(lmUPoint& pointL)
{
    if (m_xStart <= pointL.x && m_xEnd >= pointL.x)
    {
        //is in this measure
        return this;
    }
    return (lmBoxSlice*)NULL;
}

void lmBoxSlice::SelectGMObjects(bool fSelect, lmLUnits uXMin, lmLUnits uXMax,
                         lmLUnits uYMin, lmLUnits uYMax)
{
    //look up in this box
    lmBox::SelectGMObjects(fSelect, uXMin, uXMax, uYMin, uYMax);

    //loop to look up in the intrument slices
    std::vector<lmBox*>::iterator it;
	for(it = m_Boxes.begin(); it != m_Boxes.end(); ++it)
    {
        ((lmBoxSliceInstr*)(*it))->SelectGMObjects(fSelect, uXMin, uXMax, uYMin, uYMax);
    }
}

void lmBoxSlice::DrawSelRectangle(lmPaper* pPaper)
{
	//draw system border in red
	m_pBSystem->DrawBounds(pPaper, *wxRED);

    //draw a border around slice region in cyan
	lmLUnits yTop = m_pBSystem->GetYTop();
    lmLUnits yBottom = m_pBSystem->GetYBottom();

    pPaper->SketchRectangle(lmUPoint(m_xStart, yTop),
                            lmUSize(m_xEnd - m_xStart, yBottom - yTop),
                            *wxCYAN);

}

void lmBoxSlice::UpdateXLeft(lmLUnits xLeft)
{
	// During layout there is a need to update initial computations about this
	// box slice position. This update must be propagated to all contained boxes

	SetXLeft(xLeft);

	//propagate change
    for (int i=0; i < (int)m_Boxes.size(); i++)
    {
        ((lmBoxSliceInstr*)m_Boxes[i])->UpdateXLeft(xLeft);
    }
}

void lmBoxSlice::CopyYBounds(lmBoxSlice* pSlice)
{
	//This method is only invoked during layout phase, when the number of measures in the
	//system has been finally decided. There is a need to copy 'y' coordinates from first
	//slice to all others. This method receives the first slice and must copy 'y' coordinates
	//from there

	SetYTop(pSlice->GetYTop());
	SetYBottom(pSlice->GetYBottom());

	//propagate request
    for (int i=0; i < (int)m_Boxes.size(); i++)
    {
        ((lmBoxSliceInstr*)m_Boxes[i])->CopyYBounds(pSlice->GetSliceInstr(i));
    }
}

int lmBoxSlice::GetPageNumber() const
{ 
	return m_pBSystem->GetPageNumber(); 
}

lmBoxScore* lmBoxSlice::GetOwnerBoxScore() 
{ 
    return m_pBSystem->GetOwnerBoxScore(); 
}

lmBoxPage* lmBoxSlice::GetOwnerBoxPage()
{ 
    return m_pBSystem->GetOwnerBoxPage(); 
}

