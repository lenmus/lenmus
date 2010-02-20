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
#include "SystemFormatter.h"        //for lmTimeGridTable

//access to colors
#include "../globals/Colors.h"
extern lmColors* g_pColors;

//
// Class lmBoxSlice represents a sytem measure
//-----------------------------------------------------------------------------------------

lmBoxSlice::lmBoxSlice(lmBoxSystem* pParent, int nAbsMeasure, int nNumInSystem,
					   lmLUnits xStart, lmLUnits xEnd)
    : lmBox(pParent->GetScoreOwner(), eGMO_BoxSlice, _T("slice"))
    , m_pBSystem(pParent)
    , m_nAbsMeasure(nAbsMeasure)
	, m_nNumInSystem(nNumInSystem)
    , m_xStart(xStart)
    , m_xEnd(xEnd)
    , m_pGridTable((lmTimeGridTable*)NULL)
{
}

lmBoxSlice::~lmBoxSlice()
{
    if (m_pGridTable)
        delete m_pGridTable;
}

lmBoxSliceInstr* lmBoxSlice::AddInstrument(lmInstrument* pInstr)
{
    lmBoxSliceInstr* pBSI = new lmBoxSliceInstr(this, pInstr);
    AddBox(pBSI);
    return pBSI;
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

void lmBoxSlice::SetBottomSpace(lmLUnits uyValue) 
{ 
    //overrided. To propagate bottom space to last instrument

    m_uBottomSpace = uyValue;
    m_Boxes.back()->SetBottomSpace(uyValue);
}

float lmBoxSlice::GetGridTimeForPosition(lmLUnits uxPos)
{
    if (m_pGridTable)
        return m_pGridTable->GetTimeForPosititon(uxPos);
    else
        return 0.0f;
}

void lmBoxSlice::DrawTimeLines(lmPaper* pPaper, wxColour color, lmLUnits uyTop,
                               lmLUnits uyBottom)
{
    //Draw lines for available times in posTimes table. Last timepos corresponds to
    //barline and is not drawed.
    //Paper is already set in XOR mode

    for (int i=0; i < m_pGridTable->GetSize()-1; ++i)
    {
        lmLUnits uxPos = m_pGridTable->GetXPos(i);
        pPaper->SketchLine(uxPos, uyTop, uxPos, uyBottom, color);
    }
}


