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
class lmInstrument;

//
// Class lmBoxSystem represents a line of music in the printed score. 
//

class lmBoxSystem : public lmBox
{
public:
    lmBoxSystem(lmBoxPage* pParent, int nNumPage, int iSystem,
                lmLUnits uxPos, lmLUnits uyPos, bool fFirstOfPage);
    ~lmBoxSystem();

	int GetSystemNumber();

    void DeleteLastSlice();
    int GetNumMeasures() { return m_nNumMeasures; }

    void SetFirstMeasure(int nAbsMeasure) { m_nFirstMeasure = nAbsMeasure; }
    lmBoxSlice* AddSlice(int nAbsMeasure, lmLUnits xStart=0, lmLUnits xEnd=0);
    inline lmBoxSlice* GetSlice(int nRelMeasure) const { return (lmBoxSlice*)m_Boxes[nRelMeasure - 1]; }
	inline int GetNumSlices() const { return (int)m_Boxes.size(); }

    //positioning
    void SetPosition(lmLUnits xPos, lmLUnits yPos);
    inline lmLUnits GetPositionX() const { return m_xPos; }
    inline lmLUnits GetPositionY() const { return m_yPos; }
    inline void SetIndent(lmLUnits xDsplz) { m_nIndent = xDsplz; }
    inline lmLUnits GetSystemIndent() const { return m_nIndent; }
    inline lmLUnits GetSystemFinalX() const { return m_uBoundsBottom.x; }

	//miscellaneous info 
	lmLUnits GetYTopFirstStaff();
    lmShapeStaff* GetStaffShape(int nRelStaff); 
    lmShapeStaff* GetStaffShape(lmInstrument* pInstr, int nStaff); 

    //pointing at
	lmShapeStaff* FindStaffAtPosition(lmUPoint& uPoint);
	int GetNumMeasureAt(lmLUnits uxPos);

	//access to objects
	lmBoxSlice* FindBoxSliceAt(lmLUnits uxPos);

    //implementation of virtual methods from base class
    wxString Dump(int nIndent);
	int GetPageNumber() const;

	//overrides
    void UpdateXRight(lmLUnits xPos);
    void SetBottomSpace(lmLUnits uyValue);

	//owners and related
	lmBoxSystem* GetOwnerSystem() { return this; }
	lmBoxScore* GetBoxScore();
    inline lmBoxPage* GetOwnerBoxPage() { return m_pBPage; }
    lmBoxScore* GetOwnerBoxScore();

    //Staff shapes
	void AddStaffShape(lmShapeStaff* ShapeStaff, lmInstrument* pInstr, int nStaff);


private:
    void ClearStaffShapesTable();

    lmBoxPage*  m_pBPage;           //parent page
    int         m_nNumMeasures;     //number of measures that fit in this system
    int         m_nFirstMeasure;    //number of first measure
    lmLUnits    m_xPos, m_yPos;     //system position: pos to render first staff
    lmLUnits    m_nIndent;          //indentation for this system
    int         m_nNumPage;         //page number (1..n) on which this system is included
	lmShapeMargin*	m_pTopSpacer;	


    //table of staff shapes
    //staff shapes for the system are stored in this list. It also stores additional
    //information to help in locating a particulat staff shape.
    typedef struct
    {
        lmShapeStaff*   pShape;
        lmInstrument*   pInstr;
        int             nStaff;     //1..n, relative to instrument
    }
    lmShapeStaffData;

	std::vector<lmShapeStaffData*>  m_ShapeStaff;		//the table


};



#endif  // __LM_BOXSYSTEM_H__

