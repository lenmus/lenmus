//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

lmGMObject* lmBoxSlice::FindGMObjectAtPosition(lmUPoint& pointL)
{
	//wxLogMessage(_T("[lmBoxSlice::FindShapeAtPosition] GMO %s - %d"), m_sGMOName, m_nId); 
    //look in shapes collection
    lmShape* pShape = FindShapeAtPosition(pointL);
    if (pShape) return pShape;

    //loop to look up in the instrument slices
    std::vector<lmBoxSliceInstr*>::iterator it;
	for(it = m_SliceInstr.begin(); it != m_SliceInstr.end(); ++it)
    {
        lmGMObject* pGMO = (*it)->FindGMObjectAtPosition(pointL);
        if (pGMO)
			return pGMO;    //found
    }

    // no object found. Verify if the point is in this slice
    if (ContainsPoint(pointL)) 
        return this;
    else
        return (lmGMObject*)NULL;

}

void lmBoxSlice::AddToSelection(lmGMSelection* pSelection, lmLUnits uXMin, lmLUnits uXMax,
                              lmLUnits uYMin, lmLUnits uYMax)
{
    AddShapesToSelection(pSelection, uXMin, uXMax, uYMin, uYMax);

    //loop to look up in the intrument slices
    std::vector<lmBoxSliceInstr*>::iterator it;
	for(it = m_SliceInstr.begin(); it != m_SliceInstr.end(); ++it)
    {
        (*it)->AddToSelection(pSelection, uXMin, uXMax, uYMin, uYMax);
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

void lmBoxSlice::Render(lmPaper* pPaper, lmUPoint uPos)
{
    for (int i=0; i < (int)m_SliceInstr.size(); i++)
    {
        m_SliceInstr[i]->Render(pPaper, uPos);
    }
}

void lmBoxSlice::UpdateXLeft(lmLUnits xLeft)
{
	// During layout there is a need to update initial computations about this
	// box slice position. This update must be propagated to all contained boxes

	SetXLeft(xLeft);

	//propagate change
    for (int i=0; i < (int)m_SliceInstr.size(); i++)
    {
        m_SliceInstr[i]->UpdateXLeft(xLeft);
    }
}

void lmBoxSlice::UpdateXRight(lmLUnits xRight)
{
	// During layout there is a need to update initial computations about this
	// box slice position. This update must be propagated to all contained boxes

	SetXRight(xRight);

	//propagate change
    for (int i=0; i < (int)m_SliceInstr.size(); i++)
    {
        m_SliceInstr[i]->UpdateXRight(xRight);
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
    for (int i=0; i < (int)m_SliceInstr.size(); i++)
    {
        m_SliceInstr[i]->CopyYBounds(pSlice->GetSliceInstr(i));
    }
}

wxString lmBoxSlice::Dump(int nIndent)
{
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump += wxString::Format(_T("lmBoxSlice. measure %d, "),
						m_nAbsMeasure);
    sDump += DumpBounds();
    sDump += _T("\n");

    //loop to dump the systems in this page
	nIndent++;
    for (int i=0; i < (int)m_SliceInstr.size(); i++)
    {
        sDump += m_SliceInstr[i]->Dump(nIndent);
    }

	return sDump;
}

int lmBoxSlice::GetPageNumber() const
{ 
	return m_pBSystem->GetPageNumber(); 
}

bool lmBoxSlice::ContainsXPos(lmLUnits uxPos)
{
	//return true if position uxPos is within the limits of this Slice

	return (uxPos >= GetXLeft() && uxPos <= GetXRight());
}
