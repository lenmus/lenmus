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


// Definition of an entry of the timepos table
class lmTimeposEntry
{
public:
    // constructor and destructor
    lmTimeposEntry(eTimeposEntryType nType, lmStaffObj* pSO, float rTimePos)
	{
        m_nType = nType;
        m_pSO = pSO;
        m_rTimePos = rTimePos;
        m_uSize = 0.0f;
        m_uSpace = 0.0f;
        m_xLeft = 0.0f;
        m_xInitialLeft = 0.0f;
        m_uxAnchor = 0.0f;
        m_xFinal = 0.0f;
    }
    ~lmTimeposEntry() {}

	void Reposition(lmLUnits uxPos)
	{
		//reposition Shape
        m_pSO->StoreOriginAndShiftShapes(uxPos);

		//update entry data
		m_xLeft = uxPos;
		m_uxAnchor = uxPos;
		m_xInitialLeft = uxPos;
		m_xFinal = uxPos + m_uSpace;
	}

    //member variables (one entry of the table)
    //----------------------------------------------------------------------------
    eTimeposEntryType m_nType;      //type of entry
    lmStaffObj*     m_pSO;          //ptr to the StaffObj
    float           m_rTimePos;     //timepos for this pSO or -1 if not anchored in time
    lmLUnits        m_uSize;        //size of this StaffObj
    lmLUnits        m_uSpace;       //space after this StaffObj
    lmLUnits        m_xInitialLeft; //initial position of the left border of the object
    lmLUnits        m_xLeft;        //current position of the left border of the object
    lmLUnits        m_uxAnchor;     //position of the anchor line
    lmLUnits        m_xFinal;       //next position (right border position + trailing space)

};


//Helper class to contain a line
class lmTimeposTable;

class lmTimeLine
{
public:
    lmTimeLine(lmTimeposTable* pMngr, int nInstr, int nVoice);
    ~lmTimeLine();

	lmTimeposEntry* AddEntry(eTimeposEntryType nType, float rTimePos, lmStaffObj* pSO);
	lmLUnits ShiftEntries(lmLUnits uNewBarSize, lmLUnits uNewStart);

	void SetCurXLeft(lmLUnits uValue);
	lmLUnits GetMaxXFinal();
	inline lmLUnits GetXStartLine() { return m_aMainTable[0]->m_xLeft; }	//xLeft of alpha entry

    //methods for debugging
    wxString DumpMainTable();

	//spacing algorithm
	lmLUnits IntitializeSpacingAlgorithm();
	float SetTimedObjects(float rTime, lmLUnits uxPos, float rFactor);
	//lmLUnits SetInitialNotTimedObjects(lmLUnits uxPos);
	lmLUnits SetNoteRestSpace(float rFactor, float rDmin, lmNoteRest* pSO, lmLUnits uxWidth);
	lmLUnits GetPosForTime(float rTime);
	lmLUnits GetLineWidth();

	
//private:
    lmTimeposEntry*  NewEntry(eTimeposEntryType nType, float rTimePos, lmStaffObj* pSO);
	#define lmItEntries		std::vector<lmTimeposEntry*>::iterator

	lmTimeposTable*					m_pOwner;		//the owner of this line
	int								m_nInstr;		//instrument (0..n-1)
	int								m_nVoice;		//voice (0=not yet defined)
	std::vector<lmTimeposEntry*>	m_aMainTable;	//The main table

	//temporary data for SetTimedObjects() method
	lmItEntries			m_it;
	lmLUnits			m_uxCurPos;



};


//Finally let's define the class that implements the tables and the algoritms
class lmTimeposTable
{
public:
    lmTimeposTable();
    ~lmTimeposTable();

    void StartLines(int nInstr, lmVStaff* pVStaff);
    void CloseLine(lmStaffObj* pSO);
    void AddEntry(int nInstr, int nVoice, float rTimePos, lmStaffObj* pSO);
    void CleanTable();

    // access to the last added entry
    void SetCurXLeft(lmLUnits uValue);
    lmLUnits GetCurXLeft();
	void SetCurXFinal(lmLUnits uValue);
    void SetCurXAnchor(lmLUnits uValue);
	void SetSpace(lmLUnits uValue);
	void SetWidth(lmLUnits uValue);

	//access to the last entry for a voice
	lmLUnits GetCurPaperPosX(int nInstr, int nVoice);


    //methods to compute results
    lmLUnits GetStartOfBarPosition();
    lmLUnits GetGrossBarSize();
    lmLUnits DoSpacingFixed(lmTenths nSpacingValue, bool fTrace = false);
    lmLUnits DoSpacingProportional(double nSpacingFactor, bool fTrace = false);
    lmLUnits RedistributeSpace(lmLUnits uNewBarSize, lmLUnits uNewStart);

    //methods for spacing
    lmLUnits ComputeSpacing(float rFactor);
	lmLUnits TenthsToLogical(lmTenths rTenths, int nStaff); 


    //methods for debugging
    wxString DumpTimeposTable();


private:
    void StartLine(int nInstr, int nVoice=0);
	lmTimeLine* FindLine(int nInstr, int nVoice);

	#define lmItTimeLine	std::vector<lmTimeLine*>::iterator


private:
    enum {
        lmMAX_STAFF=4,		//LIMIT: max. staves in an instrument
    };
    int					m_nCurVoice[lmMAX_STAFF];	//num of current voice for each staff
	lmStaff*			m_pStaff[lmMAX_STAFF];		//staff (used to compute spacing)
	lmTimeposEntry*		m_pCurEntry;				//ptr to last added entry
	lmItTimeLine		m_itCurLine;				//iter pointing to current line
	std::vector<lmTimeLine*>	m_aLines;			//the music lines


};

#endif    // __LM_TIMEPOSTABLE_H__

