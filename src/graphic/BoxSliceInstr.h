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

#ifndef __LM_BOXSLICEINSTR_H__        //to avoid nested includes
#define __LM_BOXSLICEINSTR_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "BoxSliceInstr.cpp"
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

#include "GMObject.h"
#include "BoxSystem.h"
#include "BoxSlice.h"


class lmInstrument;
class lmVStaff;
class lmPaper;

//
// Class lmBoxSliceInstr represents a part (measure) of an instrument. 
//

class lmBoxSliceInstr : public lmBox
{
public:
    lmBoxSliceInstr(lmBoxSlice* pParent, lmInstrument* pInstr);
    ~lmBoxSliceInstr();

    inline lmInstrument* GetInstrument() const { return m_pInstr; }
    inline int GetNumMeasure() const { return m_pSlice->GetNumMeasure(); }

    //implementation of virtual methods from base class
	int GetPageNumber() const;

	//owners and related
	lmBoxSystem* GetOwnerSystem() { return m_pSlice->GetOwnerSystem(); }
    inline lmBoxScore* GetOwnerBoxScore() { return m_pSlice->GetOwnerBoxScore(); }
    inline lmBoxPage* GetOwnerBoxPage() { return m_pSlice->GetOwnerBoxPage(); }

    //other
    lmShapeStaff* GetStaffShape(int nStaff);   //1..n
    lmShapeStaff* GetNearestStaff(lmUPoint& uPoint);
    void DrawTimeGrid(lmPaper* pPaper);
    void DrawMeasureFrame(lmPaper* pPaper);


private:

    lmBoxSlice*     m_pSlice;           //parent slice
    lmInstrument*   m_pInstr;           //instrument to which this slice belongs

};



#endif  // __LM_BOXSLICEINSTR_H__

