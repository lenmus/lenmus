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
        m_uSize = 0;
        m_xLeft = 0;
        m_xInitialLeft = 0;
        m_uxAnchor = 0;
        m_xRight = 0;
        m_xFinal = 0;
    }
    ~lmTimeposEntry() {}

	void ShiftEntry(lmLUnits uShift)
	{
		m_xFinal += uShift;
		m_uxAnchor += uShift;
        m_xLeft += uShift;
        m_xRight += uShift;
	}

    //member variables (one entry of the table)
    //----------------------------------------------------------------------------
    eTimeposEntryType m_nType;      //type of entry
    lmStaffObj*     m_pSO;          //ptr to the lmStaffObj
    float           m_rTimePos;     //timepos for this pSO or -1 if not anchored in time
    lmLUnits        m_uSize;        //size of this lmStaffObj
    lmLUnits        m_xInitialLeft; //initial position of the left border of the object
    lmLUnits        m_xLeft;        //current position of the left border of the object
    lmLUnits        m_uxAnchor;      //position of the anchor line
    lmLUnits        m_xRight;       //position of the right border
    lmLUnits        m_xFinal;       //next position (right border position + trailing space)

};


// Definition of an entry of the auxiliary table for ordering by time
class lmTimeauxEntry
{
public:
    // constructor and destructor
    lmTimeauxEntry(int item, int line, float timePos) {
        nLine = line;
        nItem = item;
        rTimePos = timePos;
        uShift = 0;
    }
    ~lmTimeauxEntry() {}

    //member variables (one entry of the table)
    int         nLine;          //line to which this element belongs
    int         nItem;          //index to entry in line MainTable
    float       rTimePos;       //timepos for this entry
    lmLUnits    uShift;         //x position shift to apply to this entry

};

//Helper class to contain a line
class lmTimeLine
{
public:
    lmTimeLine(int nInstr, int nVoice, lmLUnits uxStart);
    ~lmTimeLine();

	lmTimeposEntry* AddEntry(eTimeposEntryType nType, float rTimePos, lmStaffObj* pSO);
	lmLUnits ShiftEntries(lmLUnits uNewBarSize, lmLUnits uNewStart);

    void SetCurXLeft(lmLUnits uValue);
	lmLUnits GetMaxXFinal();
	inline lmLUnits GetXStartLine() { return m_aMainTable[0]->m_xLeft; }	//xLeft of alpha entry

    //methods for debugging
    wxString DumpMainTable();

	
//private:
    lmTimeposEntry*  NewEntry(eTimeposEntryType nType, float rTimePos, lmStaffObj* pSO);
	#define lmItEntries		std::vector<lmTimeposEntry*>::iterator

	int								m_nInstr;		//instrument (0..n-1)
	int								m_nVoice;		//voice (0=not yet defined)
	std::vector<lmTimeposEntry*>	m_aMainTable;	//The main table

};


//Finally let's define the class that implements the tables and the algoritms
class lmTimeposTable
{
public:
    lmTimeposTable();
    ~lmTimeposTable();

    void    StartLine(int nInstr, lmLUnits uxStart, int nVoice=0);
    void    CloseLine(lmLUnits uxLeft, lmLUnits uxFinal, lmStaffObj* pSO);
    void    AddEntry(int nInstr, int nVoice, float rTimePos, lmStaffObj* pSO);
    void    CleanTable();

    // access to the last added entry
    void        SetCurXLeft(lmLUnits uValue);
    lmLUnits    GetCurXLeft();
	void        SetCurXFinal(lmLUnits uValue);
    void        SetCurXAnchor(lmLUnits uValue);

	//access to the last entry for a voice
	lmLUnits	GetCurPaperPosX(int nInstr, int nVoice);


    //methods to compute results
    lmLUnits    GetStartOfBarPosition();
    lmLUnits    GetGrossBarSize();
    lmLUnits    ArrangeStaffobjsByTime(bool fTrace = false);
    lmLUnits    RedistributeSpace(lmLUnits uNewBarSize, lmLUnits uNewStart);

    //methods for debugging
    wxString DumpTimeposTable();
    wxString DumpTimeauxTable();


private:
    void    AddTimeAuxEntry(int nLine, int nItem, lmTimeposEntry* pTPE);
	lmTimeLine* FindLine(int nInstr, int nVoice);

	#define lmItTimeLine	std::vector<lmTimeLine*>::iterator


private:
    int								m_nCurVoice;	//num of current voice
    std::vector<lmTimeauxEntry*>	m_aTimeAux;		//auxiliary table for ordering by time
	std::vector<lmTimeLine*>		m_aLines;		//the music lines
	lmTimeposEntry*					m_pCurEntry;	//ptr to last added entry
	lmItTimeLine					m_itCurLine;	//iter pointing to current line

};

#endif    // __LM_TIMEPOSTABLE_H__

