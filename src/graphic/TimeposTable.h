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

#ifndef __LM_TIMEPOSTABLE_H__        //to avoid nested includes
#define __LM_TIMEPOSTABLE_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "TimeposTable.cpp"
#endif

#include <vector>
#include "../score/defs.h"

class lmStaffObj;
class lmStaff;
class lmVStaff;

class lmTimeLine;
class lmTimeposEntry;
class lmCriticalLine;
class lmBreaksTable;

//define the class that implements the main table and the algoritms
class lmTimeposTable
{
public:
    lmTimeposTable();
    ~lmTimeposTable();

    void SetParameters(float rSpacingFactor, lmESpacingMethod nSpacingMethod,
                       lmTenths nSpacingValue);
    void StartLines(int nInstr, lmLUnits uxStart, lmVStaff* pVStaff, lmLUnits uSpace);
    void CloseLine(lmStaffObj* pSO, lmShape* pShape, lmLUnits xStart);
    void AddEntry(int nInstr, lmStaffObj* pSO, lmShape* pShape, bool fProlog, int nStaff=0);
    void CleanTable();
    void EndOfData();        //inform that all data has been suplied

    //methods to compute results
    void ComputeCriticalLine();
    lmLUnits GetStartOfBarPosition();
    lmLUnits GetGrossBarSize();
    lmLUnits DoSpacing(bool fTrace = false);
    lmLUnits RedistributeSpace(lmLUnits uNewBarSize, lmLUnits uNewStart);

    //break points computation and related
    bool GetOptimumBreakPoint(lmLUnits uAvailable, float* prTime, lmLUnits* puWidth);
    void TerminateTableAfter(float rTime, lmLUnits uxFinal);

    //access to info
    lmBarline* GetBarline();

    //methods for spacing
    lmLUnits ComputeSpacing(float rFactor);
	lmLUnits TenthsToLogical(lmTenths rTenths, int nStaff); 
    inline lmESpacingMethod SpacingMethod() const { return m_nSpacingMethod; }
    inline lmTenths FixedSpacingValue() const { return m_rSpacingValue; }
	inline float SpacingFactor() const { return m_rSpacingFactor; }

    //methods for debugging
    wxString DumpTimeposTable();


private:
	void AddEntry(int nInstr, int nVoice, lmStaffObj* pSO, lmShape* pShape, bool fProlog);
    void StartLine(int nInstr, int nVoice=0, lmLUnits uxStart = -1.0f, lmLUnits uSpace = 0.0f);
	lmTimeLine* FindLine(int nInstr, int nVoice);
    lmBreaksTable* ComputeBreaksTable();


	#define lmItTimeLine	std::vector<lmTimeLine*>::iterator


private:
    int					m_nCurVoice[lmMAX_STAFF];	//num of current voice for each staff
	lmStaff*			m_pStaff[lmMAX_STAFF];		//staff (used to compute spacing)
	lmTimeposEntry*		m_pCurEntry;				//ptr to last added entry
	lmItTimeLine		m_itCurLine;				//iter pointing to current line
	std::vector<lmTimeLine*>	m_aLines;			//the music lines
    lmCriticalLine*     m_pCriticalLine;			//the critical line

    //formatter parameters
    float               m_rSpacingFactor;           //for proportional spacing of notes
    lmESpacingMethod    m_nSpacingMethod;           //fixed, proportional, etc.
    lmTenths            m_rSpacingValue;            //spacing for 'fixed' method

};


#endif    // __LM_TIMEPOSTABLE_H__

