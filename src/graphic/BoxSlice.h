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

#ifndef __LM_BOXSLICE_H__        //to avoid nested includes
#define __LM_BOXSLICE_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "BoxSlice.cpp"
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

#include "../score/defs.h"
#include "GMObject.h"

class lmBoxSystem;
class lmBoxSliceInstr;
class lmInstrument;
class lmBoxSystem;

//
// Class lmBoxSlice represents a sytem measure
//


class lmBoxSlice : public lmBox
{
public:
    lmBoxSlice(lmBoxSystem* pParent, int nAbsMeasure, int nNumInSystem,
			   lmLUnits xStart=0, lmLUnits xEnd=0);
    ~lmBoxSlice();

    inline void UpdateSize(lmLUnits xStart, lmLUnits xEnd) {
            m_xStart = xStart;
            m_xEnd = xEnd;
        }

	//render
	void DrawSelRectangle(lmPaper* pPaper);

    //info
    inline int GetNumMeasure() const { return m_nAbsMeasure; }

	//instrument slices
    lmBoxSliceInstr* AddInstrument(lmInstrument* pInstr);
	lmBoxSliceInstr* GetSliceInstr(int i) const { return (lmBoxSliceInstr*)m_Boxes[i]; }

    //implementation of virtual methods from base class
	int GetPageNumber() const;

	//owners and related
	lmBoxSystem* GetOwnerSystem() { return m_pBSystem; }
    lmBoxScore* GetOwnerBoxScore();
    lmBoxPage* GetOwnerBoxPage();

    //overrides
    void SetBottomSpace(lmLUnits uyValue);

    //table xPositions/timepos
    void ClearPosTimeTable();
    void AddPosTimeEntry(lmLUnits uxPos, float rTimepos, float rDuration, lmLUnits uxWidth);
    void ClosePosTimeTable();
    float GetTimeForPosition(lmLUnits uxPos);
    void DrawTimeLines(lmPaper* pPaper, wxColour color, lmLUnits uyTop,
                       lmLUnits uyBottom);

    #ifdef __WXDEBUG__
    void DumpPosTimeTable();
    #endif
    


private:
    bool IsInPosTimeTable(float rTimepos);
    void InsertInPosTimeTable(float rTimepos);


    lmBoxSystem*	m_pBSystem;			//parent system box
    int             m_nAbsMeasure;		//number of this measure (absolute, 1..n)
	int				m_nNumInSystem;		//number of slice for this system (0..n-1)

    //start and end positions
    lmLUnits    m_xStart;
    lmLUnits    m_xEnd;

    //table of positions and timepos
    typedef struct
    {
        float           rTimepos;
        float           rDuration;
        lmLUnits        uxPos;
    }
    lmPosTime;

    std::vector<lmPosTime*>     m_PosTimes;
};



#endif  // __LM_BOXSLICE_H__

