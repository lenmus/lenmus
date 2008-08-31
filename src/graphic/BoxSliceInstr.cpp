//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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
    //delete VStaffs
    for (int i=0; i < (int)m_SlicesVStaff.size(); i++)
    {
        delete m_SlicesVStaff[i];
    }
    m_SlicesVStaff.clear();
}

lmBoxSliceVStaff* lmBoxSliceInstr::AddVStaff(lmVStaff* pVStaff, int nMeasure)
{
    lmBoxSliceVStaff* pBSV = new lmBoxSliceVStaff(this, pVStaff, nMeasure);
    m_SlicesVStaff.push_back(pBSV);
    return pBSV;
}

void lmBoxSliceInstr::Render(lmPaper* pPaper, lmUPoint uPos)
{
	//render instrument names, bracet/bracket
	for (int i=0; i < (int)m_Shapes.size(); i++)
	{
		m_Shapes[i]->Render(pPaper);
	}

    for (int i=0; i < (int)m_SlicesVStaff.size(); i++)
    {
        m_SlicesVStaff[i]->Render(pPaper, uPos);
    }

    //this->DrawBounds(pPaper, *wxGREEN);
}

void lmBoxSliceInstr::UpdateXLeft(lmLUnits xLeft)
{
	// During layout there is a need to update initial computations about this
	// box slice position. This update must be propagated to all contained boxes

	SetXLeft(xLeft);

	//propagate change
    for (int i=0; i < (int)m_SlicesVStaff.size(); i++)
    {
        m_SlicesVStaff[i]->UpdateXLeft(xLeft);
    }
}

void lmBoxSliceInstr::UpdateXRight(lmLUnits xRight)
{
	// During layout there is a need to update initial computations about this
	// box slice position. This update must be propagated to all contained boxes

	SetXRight(xRight);

	//propagate change
    for (int i=0; i < (int)m_SlicesVStaff.size(); i++)
    {
        m_SlicesVStaff[i]->UpdateXRight(xRight);
    }
}

void lmBoxSliceInstr::CopyYBounds(lmBoxSliceInstr* pBSI)
{
	//This method is only invoked during layout phase, when the number of measures in the
	//system has been finally decided. There is a need to copy 'y' coordinates from first
	//slice to all others. This method receives the first instr.slice and must copy 'y'
	//coordinates from there

	SetYTop(pBSI->GetYTop());
	SetYBottom(pBSI->GetYBottom());

	//propagate request
    for (int i=0; i < (int)m_SlicesVStaff.size(); i++)
    {
        m_SlicesVStaff[i]->CopyYBounds(pBSI->GetSliceVStaff(i));
    }
}

wxString lmBoxSliceInstr::Dump(int nIndent)
{
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump.append(_T("lmBoxSliceInstr "));
    sDump += DumpBounds();
    sDump += _T("\n");

    //loop to dump the systems in this page
	nIndent++;
    for (int i=0; i < (int)m_SlicesVStaff.size(); i++)
    {
        sDump += m_SlicesVStaff[i]->Dump(nIndent);
    }

	return sDump;
}

lmGMObject* lmBoxSliceInstr::FindGMObjectAtPosition(lmUPoint& pointL)
{
	//wxLogMessage(_T("[lmBoxSliceInstr::FindShapeAtPosition] GMO %s - %d"), m_sGMOName, m_nId); 
    //look in shapes collection
    lmShape* pShape = FindShapeAtPosition(pointL);
    if (pShape) return pShape;

    //loop to look up in the VStaff slices
    std::vector<lmBoxSliceVStaff*>::iterator it;
	for(it = m_SlicesVStaff.begin(); it != m_SlicesVStaff.end(); ++it)
    {
        lmGMObject* pGMO = (*it)->FindGMObjectAtPosition(pointL);
        if (pGMO)
			return pGMO;    //found
    }

    // no object found. Verify if the point is in this object
    if (BoundsContainsPoint(pointL)) 
        return this;
    else
        return (lmGMObject*)NULL;

}

//void lmBoxSliceInstr::AddToSelection(lmGMSelection* pSelection, lmLUnits uXMin, lmLUnits uXMax,
//                              lmLUnits uYMin, lmLUnits uYMax)
//{
//    AddShapesToSelection(pSelection, uXMin, uXMax, uYMin, uYMax);
//
//    //loop to look up in the VStaff slices
//    std::vector<lmBoxSliceVStaff*>::iterator it;
//	for(it = m_SlicesVStaff.begin(); it != m_SlicesVStaff.end(); ++it)
//    {
//        (*it)->AddToSelection(pSelection, uXMin, uXMax, uYMin, uYMax);
//    }
//}

void lmBoxSliceInstr::SelectGMObjects(bool fSelect, lmLUnits uXMin, lmLUnits uXMax,
                         lmLUnits uYMin, lmLUnits uYMax)
{
    //look up in this box
    lmBox::SelectGMObjects(fSelect, uXMin, uXMax, uYMin, uYMax);

    //loop to look up in the VStaff slices
    std::vector<lmBoxSliceVStaff*>::iterator it;
	for(it = m_SlicesVStaff.begin(); it != m_SlicesVStaff.end(); ++it)
    {
        (*it)->SelectGMObjects(fSelect, uXMin, uXMax, uYMin, uYMax);
    }
}

int lmBoxSliceInstr::GetPageNumber() const
{ 
	return m_pSlice->GetPageNumber();
}
