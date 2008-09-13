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

#ifndef __LM_BOXSYSTEM_H__        //to avoid nested includes
#define __LM_BOXSYSTEM_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "BoxSystem.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "vector"

#include "../score/Score.h"
#include "GMObject.h"

class lmBoxScore;
class lmBoxSlice;
class lmBoxPage;
class lmShapeStaff;
class lmShapeMargin;

//
// Class lmBoxSystem represents a line of music in the printed score. 
//

class lmBoxSystem : public lmBox
{
public:
    lmBoxSystem(lmBoxPage* pParent, int nNumPage);
    ~lmBoxSystem();

	int GetSystemNumber();

    void SetNumMeasures(int nMeasures, lmScore* pScore);
    int GetNumMeasures() { return m_nNumMeasures; }

    void SetFirstMeasure(int nAbsMeasure) { m_nFirstMeasure = nAbsMeasure; }
    lmBoxSlice* AddSlice(int nAbsMeasure, lmLUnits xStart=0, lmLUnits xEnd=0);
    inline lmBoxSlice* GetSlice(int nRelMeasure) const { return m_Slices[nRelMeasure - 1]; }
	inline int GetNumSlices() const { return (int)m_Slices.size(); }

    //positioning
    void UpdateXRight(lmLUnits xPos);
    void SetPosition(lmLUnits xPos, lmLUnits yPos);
    inline lmLUnits GetPositionX() const { return m_xPos; }
    inline lmLUnits GetPositionY() const { return m_yPos; }
    inline void SetIndent(lmLUnits xDsplz) { m_nIndent = xDsplz; }
    inline lmLUnits GetSystemIndent() const { return m_nIndent; }
    inline lmLUnits GetSystemFinalX() const { return m_uBoundsBottom.x; }

	//miscellaneous info 
	lmLUnits GetYTopFirstStaff();
    inline lmShapeStaff* GetStaffShape(int nStaff) { return m_ShapeStaff[nStaff - 1]; }

    //pointing at
    lmBoxSlice* FindSliceAtPosition(lmUPoint& pointL);
    lmGMObject* FindSelectableObjectAtPos(lmUPoint& pointL);
	lmShapeStaff* FindStaffAtPosition(lmUPoint& pointL);
    //void AddToSelection(lmGMSelection* pSelection, lmLUnits uXMin, lmLUnits uXMax,
    //                   lmLUnits uYMin, lmLUnits uYMax);
	int GetNumMeasureAt(lmLUnits uxPos);

    //selection
    void SelectGMObjects(bool fSelect, lmLUnits uXMin, lmLUnits uXMax,
                         lmLUnits uYMin, lmLUnits uYMax);

    //rendering
    void Render(int nSystem, lmScore* pScore, lmPaper* pPaper);

	//access to objects
	lmBoxSlice* GetSliceAt(lmLUnits xPos);

    //implementation of virtual methods from base class
    wxString Dump(int nIndent);
	int GetPageNumber() const;

	//overrides
	void AddShape(lmShape* pShape);

	//owners and related
	lmBoxSystem* GetOwnerSystem() { return this; }
	lmBoxScore* GetBoxScore();
    inline lmBoxPage* GetBoxPage() const { return m_pBPage; }
    lmBoxScore* GetOwnerBoxScore();


private:

    lmBoxPage*  m_pBPage;           //parent page
    int         m_nNumMeasures;     //number of measures that fit in this system
    int         m_nFirstMeasure;    //number of first measure
    lmLUnits    m_xPos, m_yPos;     //system position: pos to render first staff
    lmLUnits    m_nIndent;          //indentation for this system
    int         m_nNumPage;         //page number (1..n) on which this system is included
	lmShapeMargin*	m_pTopSpacer;	

    std::vector<lmBoxSlice*>        m_Slices;   //collection of slices in this system

	std::vector<lmShapeStaff*>		m_ShapeStaff;		//list of staff shapes, only in first vstaff.


};



#endif  // __LM_BOXSYSTEM_H__

