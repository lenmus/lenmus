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

#ifndef __LM_BOXSCORE_H__        //to avoid nested includes
#define __LM_BOXSCORE_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "BoxScore.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../score/defs.h"
#include "GMObject.h"

class lmPaper;
class lmScore;
class lmStaffObj;
class lmBoxPage;
class lmBoxSystem;




// Class lmBoxScore is the main container for the renderized score. Only one instance
// of this class is created for a score. lmBoxScore contains one or more instances of class 
// lmBoxPage, which represent the pages of the score.

class lmBoxScore : public lmBox
{
public:
    lmBoxScore(lmScore* pScore);
    ~lmBoxScore();

    lmBoxPage* AddPage();
    void RenderPage(int nPage, lmPaper* pPaper); 

    inline lmBoxPage* GetCurrentPage() const { return m_aPages.back(); }
    lmBoxPage* GetPage(int nPage);
    int GetNumPages();
    bool FindSliceAtPosition(lmUPoint& pointL);

	//info
	int GetNumSystems();

	//access to objects
	lmBoxSystem* GetSystem(int nSystem);	//nSystem = 1..n

    //selected objects management
    inline lmGMSelection* GetSelection() { return &m_Selection; }
    //void AddToSelection(lmGMSelection* pSelection);
    void AddToSelection(lmGMObject* pGMO);
    void AddToSelection(int nNumPage, lmLUnits uXMin, lmLUnits uXMax,
                       lmLUnits uYMin, lmLUnits uYMax);
    //void RemoveFromSelection(lmGMSelection* pSelection);
    void RemoveFromSelection(lmGMObject* pGMO);
    inline int GetNumObjectsSelected() { return m_Selection.NumObjects(); }
    void ClearSelection();


    //implementation of virtual methods from base class
    wxString Dump(int nIndent);

	//owners and related
	lmBoxSystem* GetOwnerSystem() { return (lmBoxSystem*)NULL; }
    inline lmBoxScore* GetOwnerBoxScore() { return this; }


private:
    lmScore*        m_pScore;       //score to be rendered

    // a lmBoxScore is, mainly, a collection of lmBoxPages
	std::vector<lmBoxPage*>	   m_aPages;	//array of ptrs to pages that form this score

    //selected objects
    lmGMSelection   m_Selection;        //info about selected objects


};

#endif  // __LM_BOXSCORE_H__

