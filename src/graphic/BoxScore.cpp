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
/*! @file BoxScore.cpp
    @brief Implementation file for class lmBoxScore
    @ingroup graphic_management
*/
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "BoxScore.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "BoxScore.h"

//-----------------------------------------------------------------------------------------

lmBoxScore::lmBoxScore(lmScore* pScore)
{
    m_pScore = pScore;

    //Initially the BoxScore will have one page.
    AddPage();

}

lmBoxScore::~lmBoxScore()
{
    //delete all pages
    WX_CLEAR_ARRAY(m_aPages);
}

void lmBoxScore::Render(lmPaper* pPaper)
{
    // write score titles
    m_pScore->WriteTitles(DO_DRAW, pPaper);
    pPaper->RestartPageCursors();                //restore page cursors are at top-left corner

    //loop to render pages
    int iPage;
    lmBoxPage* pBoxPage;
    for(iPage=0; iPage < (int)m_aPages.GetCount(); iPage++) {
        pBoxPage = m_aPages.Item(iPage);
        if (iPage != 0) pPaper->NewPage();
        pBoxPage->Render(m_pScore, pPaper);
    }

}

lmBoxPage* lmBoxScore::AddPage()
{
    lmBoxPage* pPage = new lmBoxPage();
    m_aPages.Add(pPage);
    return pPage;

}
