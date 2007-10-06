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

#include "../score/Score.h"

class lmBoxSystem;

//
// Class lmBoxSlice represents a sytem measure
//


class lmBoxSlice
{
public:
    lmBoxSlice(lmBoxSystem* pParent, int nMeasure, lmLUnits xStart, lmLUnits xEnd);
    ~lmBoxSlice();

    lmBoxSlice* FindMeasureAt(lmUPoint& pointL);

	//debug
	void DrawSelRectangle(lmPaper* pPaper);


private:

    lmBoxSystem*    m_pBSystem;     //parent system box
    int             m_nMeasure;     //number of this measure

    //start and end positions
    lmLUnits    m_xStart;
    lmLUnits    m_xEnd;

};



#endif  // __LM_BOXSLICE_H__

