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
#pragma implementation "BoxPage.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "BoxScore.h"
#include "BoxPage.h"
#include "BoxSlice.h"
#include "BoxInstrSlice.h"
#include "BoxSystem.h"


//access to colors
#include "../globals/Colors.h"
extern lmColors* g_pColors;

//-----------------------------------------------------------------------------------------

lmBoxPage::lmBoxPage(lmBoxScore* pParent, int nNumPage)
    : lmBox(eGMO_BoxPage)
{
    m_nNumPage = nNumPage;
    m_nFirstSystem = 0;
    m_nLastSystem = 0;
    m_pBScore = pParent;

}

lmBoxPage::~lmBoxPage()
{
    //delete all systems
    WX_CLEAR_ARRAY(m_aSystems);
}

lmBoxSystem* lmBoxPage::AddSystem(int nSystem)
{
    //Update references
    if (m_nFirstSystem == 0) m_nFirstSystem = nSystem;
    m_nLastSystem = nSystem;

    //create the system
    lmBoxSystem* pSystem = new lmBoxSystem(this, m_nNumPage);
    m_aSystems.Add(pSystem);
    return pSystem;

}

void lmBoxPage::Render(lmScore* pScore, lmPaper* pPaper)
{
    if (m_nLastSystem == 0) return;

    int iSystem;                //number of system in process
    int i;
    lmBoxSystem* pBoxSystem;
    //loop to render the systems in this page
    for(i=0, iSystem = m_nFirstSystem; iSystem <= m_nLastSystem; iSystem++, i++)
    {
        pBoxSystem = m_aSystems.Item(i);
        pBoxSystem->Render(iSystem, pScore, pPaper);
    }

}

lmBoxSlice* lmBoxPage::FindSliceAtPosition(lmUPoint& pointL)
{
    //loop to look up in the systems
    int iSystem;                //number of system in process
    int i;
    lmBoxSystem* pBoxSystem;
    //loop to render the systems in this page
    for(i=0, iSystem = m_nFirstSystem; iSystem <= m_nLastSystem; iSystem++, i++)
    {
        pBoxSystem = m_aSystems.Item(i);
        lmBoxSlice* pBSlice = pBoxSystem->FindSliceAtPosition(pointL);
        if (pBSlice)
			return pBSlice;    //found
    }
    return (lmBoxSlice*)NULL;;
}

lmBoxInstrSlice* lmBoxPage::FindInstrSliceAtPosition(lmUPoint& pointL)
{
    //loop to look up in the systems
    int iSystem;                //number of system in process
    int i;
    lmBoxSystem* pBoxSystem;
    //loop to render the systems in this page
    for(i=0, iSystem = m_nFirstSystem; iSystem <= m_nLastSystem; iSystem++, i++)
    {
        pBoxSystem = m_aSystems.Item(i);
        lmBoxInstrSlice* pBInstrSlice = pBoxSystem->FindInstrSliceAtPosition(pointL);
        if (pBInstrSlice)
			return pBInstrSlice;    //found
    }
    return (lmBoxInstrSlice*)NULL;;
}

lmGMObject* lmBoxPage::FindGMObjectAtPosition(lmUPoint& pointL)
{
    //locate an slice
    lmBoxSlice* pSlice = FindSliceAtPosition(pointL);
    if (!pSlice) return this;

    //locate InstrSlice
    lmBoxInstrSlice* pIS = FindInstrSliceAtPosition(pointL);
    if (!pIS) return pSlice;

    //locate VStaffSlice
    lmGMObject* pGMO = pIS->FindGMObjectAtPosition(pointL);
    if (!pGMO) return pSlice;

    return (pGMO);
}

