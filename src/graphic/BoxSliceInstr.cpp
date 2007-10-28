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
    : lmBox(eGMO_BoxSliceInstr)
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

lmBoxSliceVStaff* lmBoxSliceInstr::AddVStaff(lmVStaff* pVStaff)
{
    lmBoxSliceVStaff* pBSV = new lmBoxSliceVStaff(this, pVStaff);
    m_SlicesVStaff.push_back(pBSV);
    return pBSV;
}

void lmBoxSliceInstr::Render(lmPaper* pPaper, lmUPoint uPos, wxColour color)
{
    for (int i=0; i < (int)m_SlicesVStaff.size(); i++)
    {
        m_SlicesVStaff[i]->Render(pPaper, uPos, color);
    }
}

void lmBoxSliceInstr::SetFinalX(lmLUnits xPos)
{ 
	SetXRight(xPos);

	//propagate change
    for (int i=0; i < (int)m_SlicesVStaff.size(); i++)
    {
        m_SlicesVStaff[i]->SetFinalX(xPos);
    }
}

wxString lmBoxSliceInstr::Dump(int nIndent)
{
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump.append(_T("lmBoxSliceInstr\n"));

    //loop to dump the systems in this page
	nIndent++;
    for (int i=0; i < (int)m_SlicesVStaff.size(); i++)
    {
        sDump += m_SlicesVStaff[i]->Dump(nIndent);
    }

	return sDump;
}
