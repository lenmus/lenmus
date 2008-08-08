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


//entry types
enum eTimeposEntryType
{
    eAlfa = 1,              //start of voice
    eStaffobj,              //lmStaffObj inside bar
    eOmega,                 //end of voice
};

class lmTimeposTable;
class lmTimeLine;

// Definition of an entry of the timepos table
class lmTimeposEntry
{
public:
    // constructor and destructor
    lmTimeposEntry(lmTimeLine* pOwner, eTimeposEntryType nType, lmStaffObj* pSO,
                   lmShape* pShape, bool fProlog);
    lmTimeposEntry(lmTimeLine* pOwner, lmTimeposEntry* pEntry);
    ~lmTimeposEntry() {}

	void AssignSpace(lmTimeposTable* pTT, float rFactor);
	void SetNoteRestSpace(lmTimeposTable* pTT, float rFactor);
	void Reposition(lmLUnits uxPos);
    inline lmLUnits GetTotalSize() { return m_uSize + m_uFixedSpace + m_uVariableSpace; }

    wxString Dump(int iEntry);
    static wxString DumpHeader();



    //member variables (one entry of the table)
    //----------------------------------------------------------------------------
    lmTimeLine*     m_pOwner;       //owner of this entry
    eTimeposEntryType m_nType;      //type of entry
    lmStaffObj*     m_pSO;          //ptr to the StaffObj
    lmShape*        m_pShape;       //ptr to the shape
	bool			m_fProlog;
    float           m_rTimePos;     //timepos for this pSO or -1 if not anchored in time
    lmLUnits        m_xInitialLeft; //initial position of the left border of the object
    lmLUnits        m_xLeft;        //current position of the left border of the object
    lmLUnits        m_uxAnchor;     //position of the anchor line
    lmLUnits        m_xFinal;       //next position (right border position + trailing space)
    //to redistribute objects we need to know:
    lmLUnits        m_uSize;            //size of the shape (notehead, etc.)
    lmLUnits        m_uTotalSize;       //total occupied space (=shape size + spacing)
    lmLUnits        m_uFixedSpace;      //fixed space added after shape
    lmLUnits        m_uVariableSpace;   //any variable added space we can adjust

};


//Helper class to contain a line
class lmTimeposTable;

class lmTimeLine
{
public:
    lmTimeLine(lmTimeposTable* pMngr, int nInstr, int nVoice, lmLUnits uxStart, lmLUnits uSpace);
    virtual ~lmTimeLine();

	lmTimeposEntry* AddEntry(eTimeposEntryType nType, lmStaffObj* pSO, lmShape* pShape,
							 bool fProlog);
	lmLUnits ShiftEntries(lmLUnits uNewBarSize, lmLUnits uNewStart);
    lmLUnits RepositionShapes(lmLUnits uNewBarSize, lmLUnits uNewStart, lmLUnits uOldBarSize);

    lmLUnits GetMaxXFinal();
	inline lmLUnits GetXStartLine() { return m_aMainTable[0]->m_xLeft; }	//xLeft of alpha entry

    //methods for debugging
    wxString DumpMainTable();

	//spacing algorithm
	lmLUnits IntitializeSpacingAlgorithm(float rFactor, bool fCreateCriticalLine);
	float ProcessTimepos(float rTime, lmLUnits uxPos, float rFactor,
                         bool fCreateCriticalLine, lmLUnits* pMaxPos);
	lmLUnits GetMinPossiblePosForTime(float rTime);
    lmLUnits GetMinRequiredPosForTime(float rTime);
    lmLUnits GetSpaceNonTimedForTime(float rTime);
    lmLUnits GetAnchorForTime(float rTime);
	lmLUnits GetLineWidth();
    lmTimeposEntry* GetMinNoteRestForTime(float rTime);
    void AssignSpace(float rFactor);
    float ComputeRequiredSpacingFactor(lmLUnits uNewBarSize);
	
//private:
    lmTimeposEntry*  NewEntry(eTimeposEntryType nType, lmStaffObj* pSO, lmShape* pShape,
							  bool fProlog, lmLUnits uSpace = 0.0f);

	#define lmItEntries		std::vector<lmTimeposEntry*>::iterator

	lmTimeposTable*					m_pOwner;		//the owner of this line
	int								m_nInstr;		//instrument (0..n-1)
	int								m_nVoice;		//voice (0=not yet defined)
	std::vector<lmTimeposEntry*>	m_aMainTable;	//The main table

	//temporary data for ComputeSpacing() and related methods
	lmItEntries			m_it;           //point to current entry 
	lmItEntries			m_itNote;       //point to first note/rest at rTime.
	lmItEntries			m_itStart;      //point to start of rTime (possibly, a non-timed object)
	lmLUnits			m_uxCurPos;

    //data to recompute spacing factor
    float               m_rBeta;

protected:
    lmTimeLine(lmTimeposTable* pMngr);      //constructor for lmCriticalLine


};


//derived class to implement the critical line
class lmCriticalLine : public lmTimeLine
{
public:
    lmCriticalLine(lmTimeposTable* pMngr) : lmTimeLine(pMngr) {}
    ~lmCriticalLine() {}

    void AddNonTimed(lmTimeLine* pLine, float rTime);
    void AddTimed(lmTimeposEntry* pEntry);
    void ComputeBeta(float rFactor);
};


//Finally let's define the class that implements the tables and the algoritms
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

