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
// Implementation of class lmBoxSliceStaff: a part (measure) of a staff of an instrument.
//

lmBoxSliceStaff::lmBoxSliceStaff(lmBoxSliceInstr* pParent, lmVStaff* pVStaff, int nMeasure)
    : lmBox(pParent->GetScoreOwner(), eGMO_BoxSliceVStaff, _T("SliceVStaff"))
{
	m_nMeasure = nMeasure;
    m_pSliceInstr = pParent;
    m_pVStaff = pVStaff;
}


lmBoxSliceStaff::~lmBoxSliceStaff()
{
}

lmBoxSystem* lmBoxSliceStaff::GetOwnerSystem()
{
	return m_pSliceInstr->GetOwnerSystem();
}

int lmBoxSliceStaff::GetPageNumber() const
{
	return m_pSliceInstr->GetPageNumber();
}

lmBoxScore* lmBoxSliceStaff::GetOwnerBoxScore() 
{ 
    return m_pSliceInstr->GetOwnerBoxScore(); 
}

lmBoxPage* lmBoxSliceStaff::GetOwnerBoxPage()
{ 
    return m_pSliceInstr->GetOwnerBoxPage(); 
}

