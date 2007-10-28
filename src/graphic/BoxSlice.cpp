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

lmBoxSlice::lmBoxSlice(lmBoxSystem* pParent, int nAbsMeasure, lmLUnits xStart, lmLUnits xEnd)
    : lmBox(eGMO_BoxSlice)
{
    m_pBSystem = pParent;
    m_nAbsMeasure = nAbsMeasure;
    m_xStart = xStart;
    m_xEnd = xEnd;

}


lmBoxSlice::~lmBoxSlice()
{
    //delete instrument slices
    for (int i=0; i < (int)m_SliceInstr.size(); i++)
    {
        //lmBoxSliceInstr* pBSI = m_SliceInstr[i];
        //delete pBSI;
        delete m_SliceInstr[i];
    }
    m_SliceInstr.clear();
}

lmBoxSliceInstr* lmBoxSlice::AddInstrument(lmInstrument* pInstr)
{
    lmBoxSliceInstr* pBSI = new lmBoxSliceInstr(this, pInstr);
    m_SliceInstr.push_back(pBSI);
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

void lmBoxSlice::DrawSelRectangle(lmPaper* pPaper)
{
	//draw system border in red
	m_pBSystem->DrawBoundsRectangle(pPaper, *wxRED);

    //draw a border around slice region in cyan
	lmLUnits yTop = m_pBSystem->GetYTop();
    lmLUnits yBottom = m_pBSystem->GetYBottom();

    pPaper->SketchRectangle(lmUPoint(m_xStart, yTop),
                            lmUSize(m_xEnd - m_xStart, yBottom - yTop),
                            *wxCYAN);

}

void lmBoxSlice::Render(lmPaper* pPaper, lmUPoint uPos, wxColour color)
{
    for (int i=0; i < (int)m_SliceInstr.size(); i++)
    {
        m_SliceInstr[i]->Render(pPaper, uPos, color);
    }
}

void lmBoxSlice::SetFinalX(lmLUnits xPos)
{ 
	SetXRight(xPos);

	//propagate change
    for (int i=0; i < (int)m_SliceInstr.size(); i++)
    {
        m_SliceInstr[i]->SetFinalX(xPos);
    }
}

wxString lmBoxSlice::Dump(int nIndent)
{
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump += wxString::Format(_T("lmBoxSlice. measure %d\n"),
						m_nAbsMeasure);

    //loop to dump the systems in this page
	nIndent++;
    for (int i=0; i < (int)m_SliceInstr.size(); i++)
    {
        sDump += m_SliceInstr[i]->Dump(nIndent);
    }

	return sDump;
}

