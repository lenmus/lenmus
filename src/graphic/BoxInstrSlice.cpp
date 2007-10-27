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
#pragma implementation "BoxInstrSlice.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "BoxInstrSlice.h"
#include "BoxVStaffSlice.h"

//access to colors
#include "../globals/Colors.h"
extern lmColors* g_pColors;

//
// Class lmBoxInstrSlice represents a part of an instrument.
//
// It is defined by the first and last measures contained in the slice.
// It is a container for positioning information
// It is used for:
//  - drawing the staff lines
//  - determine if a mouse click is on the staff
//

//-----------------------------------------------------------------------------------------

lmBoxInstrSlice::lmBoxInstrSlice(lmBoxSystem* pParent, int nFirstMeasure, int nLastMeasure,
                                 lmInstrument* pInstr, int nInstr)
    : lmBox(eGMO_BoxInstrSlice)
{
    m_pSystem = pParent;
    m_nFirstMeasure = nFirstMeasure;
    m_nLastMeasure = nLastMeasure;
    m_pInstr = pInstr;
    m_nNumInstr = nInstr;

    //build the collection of VStaff slices
    for (int iVStaff=1; iVStaff <= pInstr->GetNumStaves(); iVStaff++)
    {
        lmVStaff* pVStaff = pInstr->GetVStaff(iVStaff);
        m_VStaffSlices.push_back(
            new lmBoxVStaffSlice(this, m_nFirstMeasure, m_nLastMeasure, iVStaff, pVStaff) );

    }
}


lmBoxInstrSlice::~lmBoxInstrSlice()
{
    lmBoxVStaffSlice* pBVS;
    for (int i=0; i < (int)m_VStaffSlices.size(); i++)
    {
        pBVS = m_VStaffSlices[i];
        delete pBVS;
    }
    m_VStaffSlices.clear();
}


lmLUnits lmBoxInstrSlice::Render(lmPaper* pPaper, lmLUnits xStartPos, int nNumPage, int nSystem)
{
    pPaper->SetCursorX( xStartPos );    //align staves in system

    //draw instrument name or abbreviation
    if (nNumPage == 1 && nSystem == 1) {
        m_pInstr->DrawName(pPaper);
    }
    else {
        m_pInstr->DrawAbbreviation(pPaper);
    }

    //set start position for this instr slice. 'y' position is saved when
    //rendering the first VStaff of this instrument
    m_uBoundsTop.x = m_pSystem->GetPositionX() + m_pSystem->GetSystemIndent();

    //draw the VStaffs
    lmLUnits yBottomLeft;
    for (int i = 0; i < (int)m_VStaffSlices.size(); i++)
    {
        yBottomLeft = m_VStaffSlices[i]->Render(pPaper, nNumPage, nSystem);
    }

    //set bounds for this instr slice
    m_uBoundsBottom.x = m_pSystem->GetSystemFinalX();
    m_uBoundsBottom.y = yBottomLeft;

    return yBottomLeft;

}

lmGMObject* lmBoxInstrSlice::FindGMObjectAtPosition(lmUPoint& pointL)
{
    for (int i = 0; i < (int)m_VStaffSlices.size(); i++)
    {
        if (m_VStaffSlices[i]->ContainsPoint(pointL))
            return m_VStaffSlices[i];
    }
    return (lmGMObject*)NULL;
}

wxString lmBoxInstrSlice::Dump()
{
	//TODO
	return(_T("lmBoxInstrSlice\n"));
}

