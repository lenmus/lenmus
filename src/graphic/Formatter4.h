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

#ifndef __LM_FORMATTER4_H__        //to avoid nested includes
#define __LM_FORMATTER4_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Formatter4.cpp"
#endif

//constants to define some tables' size
//! @limit a system can not have more than 30 staves
//! @limit a system can not have more than 20 measures
//TODO Review all code to avoid limits: dynamic tables
#define MAX_STAVES_PER_SYSTEM        30    //max number of staves in a system
#define MAX_MEASURES_PER_SYSTEM        20    //max number of measures in a system

#include "../score/Score.h"
#include "TimeposTable.h"

class lmBoxScore;
class lmBoxSystem;
class lmBoxSliceVStaff;

class lmFormatter4
{
public:
    lmFormatter4();
    ~lmFormatter4();

    //measure phase
    lmBoxScore* Layout(lmScore* pScore, lmPaper* pPaper); 


private:
    lmBoxScore* RenderJustified(lmPaper* pPaper);

    lmLUnits SizeMeasureColumn(int nAbsMeasure, int nRelMeasure, int nSystem, lmBoxSystem* pBoxSystem,
                               lmPaper* pPaper, bool* pNewSystem);
    void AddEmptyMeasureColumn(int nAbsMeasure, int nRelMeasure, int nSystem,
                               lmBoxSystem* pBoxSystem, lmPaper* pPaper);
    void RedistributeFreeSpace(lmLUnits nAvailable);
    void DrawMeasure(lmVStaff* pVStaff, int iMeasure, lmPaper* pPaper);
    bool SizeMeasure(lmBoxSliceVStaff* pBSV, lmVStaff* pVStaff, int nAbsMeasure,
					 int nRelMeasure, int nInstr, lmPaper* pPaper);
    lmLUnits ComputeSystemHeight(lmPaper* pPaper);
	void ResetLocation(int nAbsMeasure);
    bool SplitMeasureColumn();
	void AddProlog(lmBoxSliceVStaff* pBSV, int nAbsMeasure, int nRelMeasure, bool fDrawTimekey,
				   lmVStaff* pVStaff, int nInstr, lmPaper* pPaper);
	void AddKey(lmKeySignature* pKey, lmBox* pBox, lmPaper* pPaper, lmVStaff* pVStaff,
				int nInstr, int nRelMeasure);
	void AddTime(lmTimeSignature* pTime, lmBox* pBox, lmPaper* pPaper, lmVStaff* pVStaff,
				 int nInstr, int nRelMeasure);


        // member variables

    lmScore*        m_pScore;        //the score to be rendered

    //auxiliary data for computing and justifying systems.
    lmTimeposTable  m_oTimepos[MAX_MEASURES_PER_SYSTEM+1];      //timepos table for current measure column
    lmLUnits        m_uFreeSpace;                               //free space available on current system
    lmLUnits        m_uMeasureSize[MAX_STAVES_PER_SYSTEM+1];    //size of all measure columns of current system
    int             m_nMeasuresInSystem;                        //the number of measures in current system

    // renderization options and parameters
    float               m_rSpacingFactor;           //for proportional spacing of notes
    lmESpacingMethod    m_nSpacingMethod;           //fixed, proportional, etc.
    lmTenths            m_nSpacingValue;            //spacing for 'fixed' method

    // variables for debugging
    bool        m_fDebugMode;           //debug on/off
    long        m_nTraceMeasure;        //measure to trace. 0 = all

    //for rendering the prolog
	lmLUnits	m_uSpaceBeforeProlog;		//space between start of system and clef

};


#endif    // __LM_FORMATTER4_H__



