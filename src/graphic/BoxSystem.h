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

#ifndef __BOXSYSTEM_H__        //to avoid nested includes
#define __BOXSYSTEM_H__

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

//
// Class lmBoxSystem represents a line of music in the printed score. 
//

class lmBoxInstrSlice;

class lmBoxSystem
{
public:
    lmBoxSystem(int nNumPage);
    ~lmBoxSystem();

    void SetNumMeasures(int nMeasures, lmScore* pScore);
    int GetNumMeasures() { return m_nNumMeasures; }

    void SetFirstMeasure(int nMeasure) { m_nFirstMeasure = nMeasure; }

    void SetPosition(lmLUnits xPos, lmLUnits yPos) { m_xPos = xPos; m_yPos = yPos; }
    inline lmLUnits GetPositionX() const { return m_xPos; }
    inline lmLUnits GetPositionY() const { return m_yPos; }

    inline void SetFinalX(lmLUnits nLUnits) { m_xFinal = nLUnits; }
    inline void SetIndent(lmLUnits nLUnits) { m_nIndent = nLUnits; }
    inline lmLUnits GetSystemIndent() const { return m_nIndent; }
    inline lmLUnits GetSystemFinalX() const { return m_xFinal; }

    inline void SetXLeft(lmLUnits xLeft) { m_xLeftLine = xLeft; }
    inline void SetYTopLeft(lmLUnits yTop) { m_yTopLeftLine = yTop; }
    inline void SetYBottomLeft(lmLUnits yBottom) { m_yBottomLeftLine = yBottom; }

    bool FindStaffAtPosition(lmUPoint& pointL);

    void Render(int nSystem, lmScore* pScore, lmPaper* pPaper);

private:
    void RenderMeasure(lmVStaff* pVStaff, int nMeasure, lmPaper* pPaper);

    int         m_nNumMeasures;     //number of measures that fit in this system
    int         m_nFirstMeasure;    //number of first measure
    lmLUnits    m_xPos, m_yPos;     //system position: pos to render first staff
    lmLUnits    m_xFinal;           //x pos for end of staff lines
    lmLUnits    m_nIndent;          //indentation for this system
    int         m_nNumPage;         //page number (1..n) on which this system is included

    std::vector<lmUPoint>   m_posStartStaff;    //staff positions: top left corner
    std::vector<lmUPoint>   m_posEndStaff;      //staff positions: bottom right corner
    std::vector<lmBoxInstrSlice*> m_InstrSlices; //collection of Instr slices in this system

    //start and end points of the initial barline that joins all staves in a system
    //Relative to this page, that is, origin is at this page start
    lmLUnits    m_xLeftLine;
    lmLUnits    m_yTopLeftLine;
    lmLUnits    m_yBottomLeftLine;

};



#endif  // __BOXSYSTEM_H__

