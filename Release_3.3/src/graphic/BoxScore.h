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
/*! @file BoxScore.h
    @brief Header file for class lmBoxScore
    @ingroup graphic_management
*/
#ifndef __BOXSCORE_H__        //to avoid nested includes
#define __BOXSCORE_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "BoxPage.h"
#include "../score/score.h"

// to manage pages let's define an array to contain pointers to pages
#include "wx/dynarray.h"
class lmBoxPage;
WX_DEFINE_ARRAY(lmBoxPage*, ArrayBoxPages);

/*
    Class lmBoxScore is the main container for the renderized score. Only one instance
    of this class is created for a score. lmBoxScore contains one or more instances of class 
    lmBoxPage, which represent the pages of the score.
*/
class lmBoxScore
{
public:
    lmBoxScore(lmScore* pScore);
    ~lmBoxScore();

    lmBoxPage* AddPage();

    void Render(lmPaper* pPaper);                   // do the drawing phase
    void RenderPage(int nPage, lmPaper* pPaper);        // do the drawing phase

    lmBoxPage* GetCurrentPage() { return m_aPages.Item( m_aPages.GetCount() - 1); }
    int GetNumPages();


private:
    lmScore*        m_pScore;       //score to be rendered

    // a lmBoxScore is, mainly, a collection of lmBoxPages
    ArrayBoxPages  m_aPages;       //array of ptrs to pages that form this score


};

#endif  // __BOXSCORE_H__

