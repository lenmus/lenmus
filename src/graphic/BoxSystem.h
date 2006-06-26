//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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
/*! @file BoxSystem.h
    @brief Header file for class lmBoxSystem
    @ingroup graphic_management
*/
#ifndef __BOXSYSTEM_H__        //to avoid nested includes
#define __BOXSYSTEM_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../score/score.h"

/*
    Class lmBoxSystem represents a line of music in the printed score. It is made up from
    one or more instances of class lmBoxSysSlice.
*/

class lmBoxSystem
{
public:
    lmBoxSystem();
    ~lmBoxSystem();

    void SetNumMeasures(int nMeasures) { m_nNumMeasures = nMeasures; }
    int GetNumMeasures() { return m_nNumMeasures; }

    void SetFirstMeasure(int nMeasure) { m_nFirstMeasure = nMeasure; }

    void SetPositionY(lmLUnits nLUnits) { m_yPos = nLUnits; }
    lmLUnits GetPositionY() { return m_yPos; }

    void SetFinalX(lmLUnits nLUnits) { m_xFinal = nLUnits; }

    void Render(int nSystem, lmScore* pScore, lmPaper* pPaper);

private:
    void RenderMeasure(lmVStaff* pVStaff, int nMeasure, lmPaper* pPaper);

    int         m_nNumMeasures;     //number of measures that fit in this system
    int         m_nFirstMeasure;    //number of first measure
    lmLUnits    m_yPos;             // y pos to render first staff
    lmLUnits    m_xFinal;           // x pos for end of staff lines

};



#endif  // __BOXSYSTEM_H__

