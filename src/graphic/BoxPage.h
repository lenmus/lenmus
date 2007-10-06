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

#ifndef __BOXPAGE_H__        //to avoid nested includes
#define __BOXPAGE_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "BoxPage.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "BoxScore.h"
#include "BoxSystem.h"

// to manage systems let's define an array to contain pointers to systems
#include "wx/dynarray.h"
WX_DEFINE_ARRAY(lmBoxSystem*, ArrayBoxSystems);

class lmBoxScore;
class lmBoxSlice;

class lmBoxPage
{
public:
    lmBoxPage(lmBoxScore* pParent, int nNumPage);
    ~lmBoxPage();
    inline int GetFirstSystem() const { return m_nFirstSystem; }
    inline int GetLastSystem() const { return m_nLastSystem; }

    lmBoxSlice* FindStaffAtPosition(lmUPoint& pointL);
    lmBoxSystem* AddSystem(int nSystem);
    void Render(lmScore* pScore, lmPaper* pPaper);


private:
    lmBoxScore*     m_pBScore;      //parent score box
    int             m_nNumPage;     //this page number (1..n)
    int             m_nFirstSystem;
    int             m_nLastSystem;

    // a lmBoxPage is, mainly, a collection of lmBoxSystems
    ArrayBoxSystems  m_aSystems;       //array of ptrs to systems that form this page

    //page origin
    lmUPoint    m_pageOrgL;


};



#endif  // __BOXPAGE_H__

