//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

class lmInstrument;
class lmVStaff;
class lmPaper;
class lmBoxSystem;
class lmBoxSliceVStaff;

//
// Class lmBoxSliceInstr represents a part (measure) of an instrument. 
//

class lmBoxSliceInstr : public lmBox
{
public:
    lmBoxSliceInstr(lmBoxSlice* pParent, lmInstrument* pInstr);
    ~lmBoxSliceInstr();

	//vstaff slices
    lmBoxSliceVStaff* AddVStaff(lmVStaff* pVStaff);
	lmBoxSliceVStaff* GetSliceVStaff(int i) const { return m_SlicesVStaff[i]; }

	//drawing
    void Render(lmPaper* pPaper, lmUPoint uPos);

    inline lmInstrument* GetInstrument() const { return m_pInstr; }
    inline int GetNumMeasure() const { return m_pSlice->GetNumMeasure(); }
    lmGMObject* FindGMObjectAtPosition(lmUPoint& pointL);

	//positioning and bounds
    void UpdateXLeft(lmLUnits xLeft);
    void UpdateXRight(lmLUnits xRight);
	//void SystemXRightUpdated(lmLUnits xRight);
    void CopyYBounds(lmBoxSliceInstr* pBSI);

    //implementation of virtual methods from base class
    wxString Dump(int nIndent);
	int GetPageNumber() const;

	//owners and related
	lmBoxSystem* GetOwnerSystem() { return m_pSlice->GetOwnerSystem(); }



private:

    lmBoxSlice*     m_pSlice;           //parent slice
    lmInstrument*   m_pInstr;           //instrument to which this slice belongs

    std::vector<lmBoxSliceVStaff*>  m_SlicesVStaff;     //collection of VStaff slices

};



#endif  // __LM_BOXSLICEINSTR_H__

