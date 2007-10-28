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

//
// Class lmBoxSlice represents a sytem measure
//


class lmBoxSlice : public lmBox
{
public:
    lmBoxSlice(lmBoxSystem* pParent, int nAbsMeasure, lmLUnits xStart=0, lmLUnits xEnd=0);
    ~lmBoxSlice();

    inline void UpdateSize(lmLUnits xStart, lmLUnits xEnd) {
            m_xStart = xStart;
            m_xEnd = xEnd;
        }

    lmBoxSliceInstr* AddInstrument(lmInstrument* pInstr);


	//render
	void DrawSelRectangle(lmPaper* pPaper);
    void Render(lmPaper* pPaper, lmUPoint uPos, wxColour color=*wxBLACK);

    //info
    inline int GetNumMeasure() const { return m_nAbsMeasure; }
    lmBoxSlice* FindMeasureAt(lmUPoint& pointL);

	//positioning and bounds
    void SetFinalX(lmLUnits xPos);

    //implementation of virtual methods from base class
    wxString Dump(int nIndent);



private:

    lmBoxSystem*    m_pBSystem;     //parent system box
    int             m_nAbsMeasure;     //number of this measure (absolute)

    std::vector<lmBoxSliceInstr*>   m_SliceInstr;   //collection of instr. slices

    //start and end positions
    lmLUnits    m_xStart;
    lmLUnits    m_xEnd;

};



#endif  // __LM_BOXSLICE_H__

