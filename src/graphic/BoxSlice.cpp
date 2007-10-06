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
#pragma implementation "BoxSlice.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "BoxSlice.h"
#include "BoxSystem.h"

//access to colors
#include "../globals/Colors.h"
extern lmColors* g_pColors;

//
// Class lmBoxSlice represents a sytem measure
//

//-----------------------------------------------------------------------------------------

lmBoxSlice::lmBoxSlice(lmBoxSystem* pParent, int nMeasure, lmLUnits xStart, lmLUnits xEnd)
{
    m_pBSystem = pParent;
    m_nMeasure = nMeasure;
    m_xStart = xStart;
    m_xEnd = xEnd;

}


lmBoxSlice::~lmBoxSlice()
{
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

void lmBoxSlice::DrawSelRectangle(lmPaper* pPaper)
{
	//draw system border in red
	m_pBSystem->DrawSelRectangle(pPaper);

    //draw a border around slice region in cyan
	lmLUnits yTop = m_pBSystem->GetYTopLeft();
    lmLUnits yBottom = m_pBSystem->GetYBottomLeft();

    pPaper->SketchRectangle(lmUPoint(m_xStart, yTop),
                            lmUSize(m_xEnd - m_xStart, yBottom - yTop),
                            *wxCYAN);

}
