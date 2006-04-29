//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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
/*! @file BoxPage.cpp
    @brief Implementation file for class lmBoxPage
    @ingroup graphic_management
*/
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "BoxPage.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "BoxPage.h"

//access to colors
#include "../globals/Colors.h"
extern lmColors* g_pColors;

//-----------------------------------------------------------------------------------------

lmBoxPage::lmBoxPage()
{
    m_nFirstSystem = 0;
    m_nLastSystem = 0;

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
    lmBoxSystem* pSystem = new lmBoxSystem();
    m_aSystems.Add(pSystem);
    return pSystem;

}

void lmBoxPage::Render(lmScore* pScore, lmPaper* pPaper)
{
    int iSystem;                //number of system in process
    int i;
    lmBoxSystem* pBoxSystem;
    //loop to render the systems in this page
    for(i=0, iSystem = m_nFirstSystem; iSystem <= m_nLastSystem; iSystem++, i++) {
        pBoxSystem = m_aSystems.Item(i);
        pBoxSystem->Render(iSystem, pScore, pPaper);
    }

}


