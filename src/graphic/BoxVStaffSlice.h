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

#ifndef __LM_BOXVSTAFFSLICE_H__        //to avoid nested includes
#define __LM_BOXVSTAFFSLICE_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "BoxVStaffSlice.cpp"
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

class lmBoxInstrSlice;

//
// Class lmBoxVStaffSlice represents a part of the VStaff of an instrument. 
//

class lmBoxVStaffSlice : public lmBox
{
public:
    lmBoxVStaffSlice(lmBoxInstrSlice* pParent, int nFirstMeasure, int nLastMeasure,
                     int nVStaff, lmVStaff* pVStaff);
    ~lmBoxVStaffSlice();

    lmLUnits Render(lmPaper* pPaper, int nNumPage, int nSystem);

    //implementation of virtual methods from base class
    wxString Dump(int nIndent);


private:
    void RenderMeasure(int nMeasure, lmPaper* pPaper, int nNumPage);

    lmBoxInstrSlice*    m_pInstrSlice;      //paren instrumet slice
    int                 m_nFirstMeasure;    //number of first measure in this slice
    int                 m_nLastMeasure;     //number of last measure in this slice
    int                 m_nVStaff;
    lmVStaff*           m_pVStaff;          //VStaff to which this slice belongs

    //start and end points of the initial barline that joins all staves in a system
    lmLUnits    m_xLeftLine;
    lmLUnits    m_yTopLeftLine;
    lmLUnits    m_yBottomLeftLine;

};



#endif  // __LM_BOXVSTAFFSLICE_H__

