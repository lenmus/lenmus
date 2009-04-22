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

#ifndef __LM_BOXSLICEVSTAFF_H__        //to avoid nested includes
#define __LM_BOXSLICEVSTAFF_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "BoxSliceVStaff.cpp"
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

class lmVStaff;
class lmBoxSliceInstr;
class lmBoxSliceVStaff;
class lmBoxSystem;

//
// Class lmBoxSliceVStaff represents a part of the VStaff of an instrument. 
//

class lmBoxSliceVStaff : public lmBox
{
public:
    lmBoxSliceVStaff(lmBoxSliceInstr* pParent, lmVStaff* pVStaff, int nMeasure);
    ~lmBoxSliceVStaff();

    //renderization
    void Render(lmPaper* pPaper, lmUPoint uPos);

	//positioning and bounds
	void UpdateXLeft(lmLUnits xLeft);
    //void UpdateXRight(lmLUnits xRight);
	void CopyYBounds(lmBoxSliceVStaff* pBSV);

    //info
    //lmGMObject* FindObjectAtPos(lmUPoint& pointL, bool fSelectable);
    //void AddToSelection(lmGMSelection* pSelection, lmLUnits uXMin, lmLUnits uXMax,
    //                   lmLUnits uYMin, lmLUnits uYMax);

    //selection
    void SelectGMObjects(bool fSelect, lmLUnits uXMin, lmLUnits uXMax,
                         lmLUnits uYMin, lmLUnits uYMax);

    //implementation of virtual methods from base class
	int GetPageNumber() const;

	//owners and related
	lmBoxSystem* GetOwnerSystem();
	inline lmVStaff* GetCreatorVStaff() { return m_pVStaff; }
	inline int GetNumMeasure() { return m_nMeasure; }
    lmBoxScore* GetOwnerBoxScore();
    lmBoxPage* GetOwnerBoxPage();


private:
    lmBoxSliceInstr*    m_pSliceInstr;      //parent instrumet slice
    lmVStaff*           m_pVStaff;          //VStaff to which this slice belongs
	int					m_nMeasure;			//Originator measure (1..n)

};



#endif  // __LM_BOXSLICEVSTAFF_H__

