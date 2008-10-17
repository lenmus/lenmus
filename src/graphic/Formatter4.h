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

#ifndef __LM_FORMATTER4_H__        //to avoid nested includes
#define __LM_FORMATTER4_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Formatter4.cpp"
#endif

#include <vector>

//constants to define some tables' size
//! @limit a system can not have more than 20 measures
//TODO Review all code to avoid limits: dynamic tables
#define MAX_MEASURES_PER_SYSTEM        20    //max number of measures in a system

#include "../score/Score.h"
#include "TimeposTable.h"

class lmBoxScore;
class lmBoxSystem;
class lmBoxSliceVStaff;
class lmSystemCursor;

class lmFormatter4
{
public:
    lmFormatter4();
    ~lmFormatter4();

    //measure phase
    lmBoxScore* LayoutScore(lmScore* pScore, lmPaper* pPaper); 


private:
    lmLUnits SizeMeasureColumn(int nSystem, lmBoxSystem* pBoxSystem, bool* pNewSystem,
                               lmLUnits nSystemIndent);
    void AddEmptySystem(int nSystem, lmBoxSystem* pBoxSystem);
    void RedistributeFreeSpace(lmLUnits nAvailable, bool fLastSystem);
    bool SizeMeasure(lmBoxSliceVStaff* pBSV, lmVStaff* pVStaff, int nInstr);
    lmLUnits ComputeSystemHeight();
    void SplitMeasureColumn(lmLUnits uAvailable);
	void AddProlog(lmBoxSliceVStaff* pBSV, bool fDrawTimekey, lmVStaff* pVStaff, int nInstr);
	void AddKey(lmKeySignature* pKey, lmBox* pBox, lmVStaff* pVStaff, int nInstr);
	void AddTime(lmTimeSignature* pTime, lmBox* pBox, lmVStaff* pVStaff, int nInstr);
    void AddColumnToSystem();


        // member variables

    lmScore*        m_pScore;        //the score to be rendered

    //auxiliary data for computing and justifying systems.
    lmTimeposTable  m_oTimepos[MAX_MEASURES_PER_SYSTEM+1];      //timepos table for each column for current system
    lmLUnits        m_uMeasureSize[MAX_MEASURES_PER_SYSTEM+1];  //minimum size for each column for current system
    lmLUnits        m_uFreeSpace;                               //free space available on current system
    int             m_nColumnsInSystem;                         //the number of columns in current system

    // renderization options and parameters
    float               m_rSpacingFactor;           //for proportional spacing of notes
    lmESpacingMethod    m_nSpacingMethod;           //fixed, proportional, etc.
    lmTenths            m_nSpacingValue;            //spacing for 'fixed' method

    // variables for debugging
    bool            m_fDebugMode;           //debug on/off
    long            m_nTraceMeasure;        //measure to trace. 0 = all

    //for rendering the prolog
	lmLUnits	    m_uSpaceBeforeProlog;   //space between start of system and clef

    //new global vars
    lmPaper*        m_pPaper;
    int             m_nColumn;      //number of column in process, relative to current system
    int             m_nAbsColumn;   //number of column in process, absolute 1..n
    lmSystemCursor* m_pSysCursor;

};


#endif    // __LM_FORMATTER4_H__



