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

// Encapsulation of the table and management algoritms to compute the positioning
// data for each lmStaffObj, when a bar column must be rendered.

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "TimeposTable.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <vector>
#include <list>
#include <algorithm>
#include <math.h>

#include "wx/debug.h"

#include "../score/Score.h"
#include "../score/Staff.h"
#include "../score/VStaff.h"
#include "../score/Notation.h"
#include "TimeposTable.h"

class lmTimeLine;
class lmBreaksTable;

#define lmDUMP_TABLES   1

//spacing function parameters
//-----------------------------------------------
    //TODO: User options
float				m_rDmin = 8.0f;				//Dmin: min. duration to consider
lmTenths			m_rMinSpace = 10.0f;		//Smin: space for Dmin

    //TODO: User options
lmTenths m_rSpaceAfterProlog = 25.0f;
lmTenths m_rSpaceAfterIntermediateClef = 20.0f;
lmTenths m_rMinSpaceBetweenNoteAndClef = 10.0f;




//=====================================================================================
//lmTimeposEntry definition: an entry of the lmTimePos table
//=====================================================================================

//entry types
enum eTimeposEntryType
{
    eAlfa = 1,              //start of voice
    eStaffobj,              //lmStaffObj inside bar
    eOmega,                 //end of voice
};

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
    lmTimeLine*     m_pOwner;           //owner of this entry
    eTimeposEntryType m_nType;          //type of entry
    lmStaffObj*     m_pSO;              //ptr to the StaffObj
    lmShape*        m_pShape;           //ptr to the shape
	bool			m_fProlog;          //this shape is a prolog object (clef, KS, TS at start of system)
    float           m_rTimePos;         //timepos for this pSO or -1 if not anchored in time
    lmLUnits        m_xInitialLeft;     //initial position of the left border of the object
    lmLUnits        m_xLeft;            //current position of the left border of the object
    lmLUnits        m_uxAnchor;         //position of the anchor line
    lmLUnits        m_xFinal;           //next position (right border position + trailing space)
    //to redistribute objects we need to know:
    lmLUnits        m_uSize;            //size of the shape (notehead, etc.)
    lmLUnits        m_uTotalSize;       //total occupied space (=shape size + spacing)
    lmLUnits        m_uFixedSpace;      //fixed space added after shape
    lmLUnits        m_uVariableSpace;   //any variable added space we can adjust

};



//=====================================================================================
//lmTimeLine definition: Helper class to contain a line
//=====================================================================================

class lmTimeLine
{
public:
    lmTimeLine(lmTimeposTable* pMngr, int nInstr, int nVoice, lmLUnits uxStart, lmLUnits uSpace);
    virtual ~lmTimeLine();

	lmTimeposEntry* AddEntry(eTimeposEntryType nType, lmStaffObj* pSO, lmShape* pShape,
							 bool fProlog);
	lmLUnits ShiftEntries(lmLUnits uNewBarSize, lmLUnits uNewStart);
    lmLUnits RepositionShapes(lmCriticalLine* pCriticalLine, lmLUnits uNewBarSize,
                              lmLUnits uNewStart, lmLUnits uOldBarSize);

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

    //break points
    void TerminateLineAfter(float rTime, lmLUnits uxFinal);
    void ComputeBreakPoints(lmBreaksTable* pBT);

	
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


//=====================================================================================
//lmCriticalLine definition: derived class to implement the critical line
//  Its enties are owned by the respective lines. DO NOT CHANGE THEM
//=====================================================================================

class lmCriticalLine : public lmTimeLine
{
public:
    lmCriticalLine(lmTimeposTable* pMngr) : lmTimeLine(pMngr) {}
    ~lmCriticalLine() {}

    void AddNonTimed(lmTimeLine* pLine, float rTime);
    void AddEntry(lmTimeposEntry* pEntry);
    void ComputeBeta(float rFactor);
    lmLUnits RedistributeSpace(lmLUnits uNewBarSize, lmLUnits uNewStart,
                               lmLUnits uOldBarSize);

    void InitializeToGetTimepos();
    lmLUnits GetTimepos(float rTime);
    lmLUnits GetPosForStaffobj(lmStaffObj* pSO, float rTime);
    wxString DumpPosTimes();


protected:

    //table of positions and timepos
    typedef struct lmPosTimeEntry_Struct
    {
        lmTimeposEntry* pTPE;
        float           rTimepos;
        lmLUnits        uxPos;
    }
    lmPosTimeEntry;

    std::vector<lmPosTimeEntry> m_PosTimes;

    //to traverse the table by timepos
    float   m_rLastTimepos;
    std::vector<lmPosTimeEntry>::iterator   m_it;   

};


//=====================================================================================
//lmBreaksTable definition: table to contain possible break points
//=====================================================================================

//an entry of the BreaksTable
typedef struct lmBreaksTimeEntry_Struct
{
    float       rTimepos;
    float       rPriority;
    lmLUnits    uxStart;
    lmLUnits    uxEnd;
    bool        fInBeam;
    lmLUnits    uxBeam;
}
lmBreaksTimeEntry;

//the breaks table
class lmBreaksTable
{
public:
    lmBreaksTable();
    ~lmBreaksTable();

    void AddEntry(float rTime, lmLUnits uxStart, lmLUnits uWidth, bool fInBeam,
                  lmLUnits uxBeam, float rPriority = 0.8f);
    void AddEntry(lmBreaksTimeEntry* pBTE);
    void SetPriority();
    void ChangePriority(int iEntry, float rMultiplier);
    wxString Dump();
    inline bool IsEmpty() { return m_BreaksTable.empty(); }

    //traversing the table
    lmBreaksTimeEntry* GetFirst();
    lmBreaksTimeEntry* GetNext();


private:

    std::list<lmBreaksTimeEntry*>               m_BreaksTable;      //the table
    std::list<lmBreaksTimeEntry*>::iterator     m_it;               //for GetFirst(), GetNext();


};


//=====================================================================================
//lmBreaksTable implementation
//=====================================================================================


lmBreaksTable::lmBreaksTable()
{
}

lmBreaksTable::~lmBreaksTable()
{
    std::list<lmBreaksTimeEntry*>::iterator it;
    for (it = m_BreaksTable.begin(); it != m_BreaksTable.end(); ++it)
        delete *it;
    m_BreaksTable.clear();
}

void lmBreaksTable::AddEntry(float rTime, lmLUnits uxStart, lmLUnits uWidth, bool fInBeam,
                             lmLUnits uxBeam, float rPriority)
{
    lmBreaksTimeEntry* pBTE = new lmBreaksTimeEntry;
    pBTE->rPriority = rPriority;
    pBTE->rTimepos = rTime;
    pBTE->uxStart = uxStart;
    pBTE->uxEnd = uxStart + uWidth;
    pBTE->fInBeam = fInBeam;
    pBTE->uxBeam = uxBeam;

    m_BreaksTable.push_back(pBTE);
}

void lmBreaksTable::AddEntry(lmBreaksTimeEntry* pBTE)
{
    AddEntry(pBTE->rTimepos, pBTE->uxStart, pBTE->uxEnd - pBTE->uxStart, pBTE->fInBeam,
             pBTE->uxBeam, pBTE->rPriority);
}

void lmBreaksTable::ChangePriority(int iEntry, float rMultiplier)
{
}

wxString lmBreaksTable::Dump()
{
    wxString sMsg = _T("Breaks table\n");
    sMsg += _T("===================================================================\n\n");

    if (m_BreaksTable.size() == 0)
    {
        sMsg += _T("The table is empty.");
        return sMsg;
    }

    //          +....... +....... +....... +....... +..... +....... 
    sMsg += _T("Piority  TimePos  xStart   xEnd     InBeam xBeam\n");
    sMsg += _T("-------- -------- -------- -------- ------ --------\n");
    std::list<lmBreaksTimeEntry*>::iterator it;
    for (it = m_BreaksTable.begin(); it != m_BreaksTable.end(); ++it)
    {
        sMsg += wxString::Format(_T("%8.2f %8.2f %8.2f %8.2f %6s %8.2f\n"),
                    (*it)->rPriority, (*it)->rTimepos, (*it)->uxStart, (*it)->uxEnd,
                    ((*it)->fInBeam ? _T("yes   ") : _T("no    ")), (*it)->uxBeam );
    }
    return sMsg;
}

lmBreaksTimeEntry* lmBreaksTable::GetFirst()
{
    m_it = m_BreaksTable.begin();
    if (m_it == m_BreaksTable.end())
        return (lmBreaksTimeEntry*)NULL;

    return *m_it;
}

lmBreaksTimeEntry* lmBreaksTable::GetNext()
{
    //advance to next one
    ++m_it;
    if (m_it != m_BreaksTable.end())
        return *m_it;

    //no more items
    return (lmBreaksTimeEntry*)NULL;
}






//=====================================================================================
//lmTimeposEntry implementation
//=====================================================================================

lmTimeposEntry::lmTimeposEntry(lmTimeLine* pOwner, eTimeposEntryType nType, lmStaffObj* pSO,
                               lmShape* pShape, bool fProlog)
{
    m_pOwner = pOwner;
    m_nType = nType;
    m_pSO = pSO;
    m_pShape = pShape;
	m_fProlog = fProlog;

    m_xFinal = 0.0f;

    if (pSO && (pSO->GetClass() == eSFOT_NoteRest || pSO->GetClass() == eSFOT_Barline))
        m_rTimePos = pSO->GetTimePos();
    else
        m_rTimePos = -1.0f;

    m_uSize = (pShape ? pShape->GetWidth() : 0.0f);
    m_xLeft = (pShape ? pShape->GetXLeft() : 0.0f);
    m_xInitialLeft = m_xLeft;

    if (pSO && pSO->GetClass() == eSFOT_NoteRest)
        m_uxAnchor = m_xLeft - pSO->GetAnchorPos();
    else
        m_uxAnchor = 0.0f;

    m_uFixedSpace = 0.0f;
    m_uVariableSpace = 0.0f;
}

lmTimeposEntry::lmTimeposEntry(lmTimeLine* pOwner, lmTimeposEntry* pEntry)
{
    m_pOwner = pOwner;
    m_nType = pEntry->m_nType;
    m_pSO = pEntry->m_pSO;
    m_pShape = pEntry->m_pShape;
	m_fProlog = pEntry->m_fProlog;
    m_rTimePos = pEntry->m_rTimePos;
    m_xInitialLeft = pEntry->m_xInitialLeft;
    m_xLeft = pEntry->m_xLeft;
    m_uxAnchor = pEntry->m_uxAnchor;
    m_xFinal = pEntry->m_xFinal;
    m_uSize = pEntry->m_uSize;
    m_uTotalSize = pEntry->m_uTotalSize;
    m_uFixedSpace = pEntry->m_uFixedSpace;
    m_uVariableSpace = pEntry->m_uVariableSpace;
}

void lmTimeposEntry::AssignSpace(lmTimeposTable* pTT, float rFactor)
{
	//assign spacing to this object

    switch (m_nType)
    {
        case eAlfa:
			//already assigned. It was assigned when creating the entry.
			break;

        case eOmega:
			if (m_pSO && m_pSO->IsBarline())    //if exists it must be a barline !!
                ;
            else
                m_uSize = 0.0f;
            break;

        default:
            //lmStaffObj entry
			if (!m_pSO->IsVisible())
			{
				m_uSize = 0.0f;
                m_uFixedSpace = 0.0f;
                m_uVariableSpace = 0.0f;
			}
			else
			{
				if (m_pSO->IsNoteRest())
				{
					SetNoteRestSpace(pTT, rFactor);
				}
				else if (m_pSO->IsClef())
				{
                    m_uFixedSpace = pTT->TenthsToLogical(10, 1);
				}
				else if (m_pSO->IsKeySignature())
				{
                    m_uFixedSpace = pTT->TenthsToLogical(10, 1);
				}
				else if (m_pSO->IsTimeSignature())
				{
                    m_uFixedSpace = pTT->TenthsToLogical(10, 1);
				}
				else if (m_pSO->IsNotation() &&
                         ((lmNotation*)m_pSO)->GetNotationType()==eNT_Spacer )
				{
                    ;
				}
				else
                    m_uSize = 0.0f;
			}
    }
}

void lmTimeposEntry::SetNoteRestSpace(lmTimeposTable* pTT, float rFactor)
{
	static const float rLog2 = 0.3010299956640f;		// log(2)

	int iStaff = m_pSO->GetStaffNum();
    lmLUnits uTotalSize;
    if (pTT->SpacingMethod() == esm_PropConstantFixed)
    {
        //proportional constant spacing.
        float rVar = log(((lmNoteRest*)m_pSO)->GetDuration() / m_rDmin) / rLog2;
	    //wxLogMessage(_T("[lmTimeposEntry::SetNoteRestSpace] duration=%s, rel.space=%.2f"),
	    //		pSO->GetLDPNoteType(), (1.0f + rVar) );

	    //spacing function
	    uTotalSize = pTT->TenthsToLogical(m_rMinSpace, iStaff);		//Space(Di) = Smin
	    if (rVar > 0.0f) {
		    rVar *= uTotalSize;         //rVar = Smin*log2(Di/Dmin)
            m_pOwner->m_rBeta += rVar;  //rBeta = SUM( Smin*log2(Di/Dmin) )
            rVar *= rFactor;            //rVar = Smin*[A*log2(Di/Dmin)]
		    uTotalSize += rVar;			//Space(Di) = Smin*[1 + A*log2(Di/Dmin)]
	    }

	    //if space is lower than object width force space to be a little more than width
        m_uFixedSpace = 0.0f;
        m_uVariableSpace = uTotalSize - m_uSize;
	    if (m_uVariableSpace < 0)
        {
            m_uVariableSpace = 0.0f;
            m_uFixedSpace = pTT->TenthsToLogical(5.0f, iStaff);
        }
    }
    else if (pTT->SpacingMethod() == esm_Fixed)
    {
        // fixed spacing
        uTotalSize = pTT->TenthsToLogical(pTT->FixedSpacingValue(), iStaff);
        m_uVariableSpace = 0.0f;
        m_uFixedSpace = uTotalSize - m_uSize;
	    if (m_uFixedSpace < 0)
        {
            m_uFixedSpace = pTT->TenthsToLogical(5.0f, iStaff);
        }
    }
    else
        wxASSERT(false);

}

void lmTimeposEntry::Reposition(lmLUnits uxPos)
{
	//reposition Shape
    //wxLogMessage(_T("Reposition: old xLeft=%.2f, new xLeft=%.2f"), m_xInitialLeft, uxPos);
    lmLUnits uShift = uxPos - m_xInitialLeft;
    if (m_pSO && m_pShape)
        m_pSO->StoreOriginAndShiftShapes( uShift, m_pShape->GetOwnerIDX() );

	//update entry data
	m_xLeft = uxPos;
	m_xInitialLeft = m_xLeft;
	m_xFinal = uxPos + GetTotalSize();
}

wxString lmTimeposEntry::DumpHeader()
{
    //         ...+  ..+   ...+ ..+   +  ..........+........+........+........+........+........+........+........+........+......+
    return _T("Item    Type      ID Prolog   TimePos    xInit  xAnchor    xLeft     Size    Space   xFinal  SpFixed   SpVar ShpIdx\n");
}

wxString lmTimeposEntry::Dump(int iEntry)
{
    wxString sMsg = wxString::Format(_T("%4d: "), iEntry);
    switch (m_nType)
    {
        case eAlfa:
            sMsg += _T("  Alfa              ");
            break;

        case eOmega:
            sMsg += _T("  Omega");
            if (m_pSO) {
                sMsg += wxString::Format(_T("%3d          "), m_pSO->GetClass() );
            } else {
                sMsg += _T("  -           ");
            }
            break;

        default:
            //lmStaffObj entry
			sMsg += wxString::Format(_T("  pSO %4d %3d   %s  "),
									m_pSO->GetClass(),
									m_pSO->GetID(),
									(m_fProlog ? _T("S") : _T(" ")) );
    }

    sMsg += wxString::Format(_T("%11.2f %8.2f %8.2f %8.2f %8.2f %8.2f %8.2f %8.2f %8.2f"),
                m_rTimePos, m_xInitialLeft, m_uxAnchor, m_xLeft,
		        m_uSize, GetTotalSize(), m_xFinal,
                m_uFixedSpace, m_uVariableSpace);

    if (m_pShape)
        sMsg += wxString::Format(_T("  %4d\n"), m_pShape->GetOwnerIDX());
    else
        sMsg += _T("    --\n");

    return sMsg;
}


//=====================================================================================
//lmTimeLine
//=====================================================================================

lmTimeLine::lmTimeLine(lmTimeposTable* pMngr, int nInstr, int nVoice, lmLUnits uxStart,
                       lmLUnits uSpace)
{
	m_pOwner = pMngr;
	m_nInstr = nInstr;
	m_nVoice = nVoice;
    NewEntry(eAlfa, (lmStaffObj*)NULL, (lmShape*)NULL, false, uSpace);
    m_aMainTable.back()->m_uxAnchor = 0.0f;
    m_aMainTable.back()->m_xLeft = uxStart;
    m_aMainTable.back()->m_xInitialLeft = uxStart;

    //other variables
    m_rBeta = 0.0f;
}

lmTimeLine::lmTimeLine(lmTimeposTable* pMngr)
{
    //constructor used only for lmCriticalLine

	m_pOwner = pMngr;
	m_nInstr = 0;
	m_nVoice = 0;
    m_rBeta = 0.0f;
}

lmTimeLine::~lmTimeLine()
{
    for (lmItEntries it = m_aMainTable.begin(); it != m_aMainTable.end(); ++it)
	{
		delete *it;
	}
	m_aMainTable.clear();
}

lmTimeposEntry* lmTimeLine::AddEntry(eTimeposEntryType nType, lmStaffObj* pSO, lmShape* pShape,
									 bool fProlog)
{
    return NewEntry(nType, pSO, pShape, fProlog);
}

lmTimeposEntry* lmTimeLine::NewEntry(eTimeposEntryType nType, lmStaffObj* pSO, lmShape* pShape,
									 bool fProlog, lmLUnits uSpace)
{
    lmTimeposEntry* pEntry = new lmTimeposEntry(this, nType, pSO, pShape, fProlog);
    m_aMainTable.push_back(pEntry);
    if (nType == eAlfa)
        pEntry->m_uFixedSpace = uSpace;
	return pEntry;
}

lmLUnits lmTimeLine::RepositionShapes(lmCriticalLine* pCriticalLine, lmLUnits uNewBarSize, 
                                      lmLUnits uNewStart, lmLUnits uOldBarSize)
{
    //shapes must be moved to their final positions.
    //Final positions for each timepos are in critical line

    lmLUnits uBarPosition = 0;
    lmTimeposEntry* pTPE = m_aMainTable.front();
    lmLUnits uOldStart = pTPE->m_xLeft;

    //all non-timed entries, at beginning, marked as fProlog must be only re-located
    lmLUnits uShiftReloc = uNewStart - uOldStart;
    lmItEntries it = m_aMainTable.begin();
    while (it != m_aMainTable.end() && (*it)->m_rTimePos < 0.0f)
    {
        if ((*it)->m_pShape)
        {
            if ((*it)->m_fProlog)
			    (*it)->m_pSO->StoreOriginAndShiftShapes( uShiftReloc, (*it)->m_pShape->GetOwnerIDX() );
            else
			    break;
        }
        ++it;
    } 

    if (it == m_aMainTable.end())
         return uBarPosition;
     
	//wxLogMessage(_T("[lmTimeLine::RepositionShapes] Reposition: uNewBarSize=%.2f  uNewStart=%.2f  uOldBarSize=%.2f"),
	//			 uNewBarSize, uNewStart, uOldBarSize );

    //we are at the start point for repositioning shapes
    pCriticalLine->InitializeToGetTimepos();
    float rCurTime = (*it)->m_rTimePos;
    lmLUnits uxCurPos;
    if (rCurTime != -1.00f)
        uxCurPos = pCriticalLine->GetTimepos(rCurTime);
    else
        uxCurPos = pCriticalLine->GetPosForStaffobj((*it)->m_pSO, rCurTime);


    for (; it != m_aMainTable.end(); ++it)
	{
        lmTimeposEntry* pTPE = *it;
        if (!IsEqualTime(pTPE->m_rTimePos, rCurTime) && !(pTPE->m_rTimePos < 0.0f))
        {
            //advance to next time
            rCurTime = pTPE->m_rTimePos;
            uxCurPos = pCriticalLine->GetTimepos(rCurTime);
        }
        else if (pTPE->m_rTimePos < 0.0f && pTPE->m_nType == eStaffobj)
        {
            if (pTPE->m_pSO->GetID() > 62)
                wxLogMessage(_T("DBG"));
            uxCurPos = pCriticalLine->GetPosForStaffobj(pTPE->m_pSO, rCurTime);
        }

        if (pTPE->m_nType == eStaffobj)
        {
            lmLUnits uShift = uxCurPos - pTPE->m_xLeft + pTPE->m_uxAnchor;
            //wxLogMessage(_T("[lmTimeLine::RepositionShapes] rCurTime=%.2f, uxCurPos=%.2f, m_xLeft=%.2f, uShift=%.2f"),
            //             rCurTime, uxCurPos, pTPE->m_xLeft, uShift);
            if (pTPE->m_pShape)
				pTPE->m_pSO->StoreOriginAndShiftShapes( uShift, pTPE->m_pShape->GetOwnerIDX() );
        }
        else if (pTPE->m_nType == eOmega)
        {
            //there might be no barline.
            if (pTPE->m_pSO)
            {
                uBarPosition = uNewStart + uNewBarSize - pTPE->m_uSize;
                lmLUnits uShiftBar = uBarPosition - pTPE->m_xLeft;
                if (pTPE->m_pShape)
				    pTPE->m_pSO->StoreOriginAndShiftShapes(uShiftBar, pTPE->m_pShape->GetOwnerIDX() );

                //wxLogMessage(_T("[lmTimeLine::RepositionShapes] Reposition bar: uBarPosition=%.2f, uShiftBar=%.2f"),
				//			uBarPosition, uShiftBar );
            }
        }
    }
    return uBarPosition;
}

lmLUnits lmTimeLine::ShiftEntries(lmLUnits uNewBarSize, lmLUnits uNewStart)
{
    lmLUnits uBarPosition = 0;
    lmTimeposEntry* pTPE = m_aMainTable.front();
    lmLUnits uShift = uNewStart - pTPE->m_xLeft;

	//wxLogMessage(_T("[lmTimeLine::ShiftEntries] Reposition: uNewBarSize=%.2f  uNewStart=%.2f  Shift=%.2f"),
	//			 uNewBarSize, uNewStart, uShift );

	//Update table and store the new x positions into the StaffObjs
    for (lmItEntries it = m_aMainTable.begin(); it != m_aMainTable.end(); ++it)
	{
        pTPE = *it;
        if (uShift != 0.0f && pTPE->m_nType == eStaffobj)
        {
            if (pTPE->m_pShape)
				pTPE->m_pSO->StoreOriginAndShiftShapes(uShift, pTPE->m_pShape->GetOwnerIDX() );
        }
        else if (pTPE->m_nType == eOmega)
        {
            //there might be no barline.
            if (pTPE->m_pSO)
            {
                uBarPosition = uNewStart + uNewBarSize - pTPE->m_uSize;
                lmLUnits uShiftBar = uBarPosition - pTPE->m_xLeft;
                if (pTPE->m_pShape)
				    pTPE->m_pSO->StoreOriginAndShiftShapes(uShiftBar, pTPE->m_pShape->GetOwnerIDX() );
				//pTPE->m_pSO->StoreOriginAndShiftShapes(uShiftBar);
            }
        }
    }
    return uBarPosition;

}

lmLUnits lmTimeLine::GetLineWidth()
{
	//Return the size of the measure represented by this line or 0 if
	//no omega entry.

	if (m_aMainTable.size() > 0 && m_aMainTable.back()->m_nType == eOmega)
		return m_aMainTable.back()->m_xLeft
			   + m_aMainTable.back()->GetTotalSize() - m_aMainTable.front()->m_xLeft;
	else
		return 0.0f;
}

void lmTimeLine::AssignSpace(float rFactor)
{
	//Explores all entries in this line and assign space to each object

	//Update table and store the new x positions into the StaffObjs
    for (lmItEntries it = m_aMainTable.begin(); it != m_aMainTable.end(); ++it)
	{
        (*it)->AssignSpace(m_pOwner, rFactor);
    }
}

lmLUnits lmTimeLine::GetMinPossiblePosForTime(float rTime)
{
	//When entering this method iterator m_it points to the next timed entry or
	//to end iterator.
	//If next entry time is rTime, returns its xLeft position. Else returns 0

	if (m_it != m_aMainTable.end() && IsEqualTime((*m_it)->m_rTimePos, rTime) )
		return (*m_it)->m_xLeft- (*m_it)->m_uxAnchor;
	else
		return 0.0f;
}

lmTimeposEntry* lmTimeLine::GetMinNoteRestForTime(float rTime)
{
    //Explore all timed staffobjs at time rTime and return the entry having the
    //note/rest of minimum duration. If several notes of this minimal duration,
    //retuns the one occupying more space (because of accidentals or other).
    //
	//When enetring this method iterator m_it points to the next timed entry or
	//to end iterator. This method doen't change m_it

    float rMinDuration = 100000.0f;     //any too big value
    lmLUnits uMaxSpace = 0.0f;
    lmTimeposEntry* pEntry = (lmTimeposEntry*)NULL;
	lmItEntries it = m_it;
	lmItEntries itEnd = m_aMainTable.end();
	while (it != itEnd && IsEqualTime((*it)->m_rTimePos, rTime) )
    {
        if ((*it)->m_pSO->IsNoteRest())
        {
            float rDuration = (*it)->m_pSO->GetTimePosIncrement();
            if (rDuration <= rMinDuration)
            {
                lmLUnits uSpace = (*it)->GetTotalSize();
                if (uSpace >= uMaxSpace)
                {
	                rMinDuration = rDuration;
                    uMaxSpace = uSpace;
                    pEntry = *it;
                }
            }
        }
        ++it;
    }
    return pEntry;
}

lmLUnits lmTimeLine::GetMinRequiredPosForTime(float rTime)
{
    //Explore all timed staffobjs at time rTime and return the maximum start xPos found
	//When enetring this method iterator m_it points to the next timed entry or
	//to end iterator. This method doen't change m_it

    lmLUnits uxPos = 0.0f;
	lmItEntries it = m_it;
	lmItEntries itEnd = m_aMainTable.end();
	while (it != itEnd && IsEqualTime((*it)->m_rTimePos, rTime) )
    {
	    uxPos = wxMax(uxPos, (*it)->m_xLeft);
        ++it;
    }
    return uxPos;
}

lmLUnits lmTimeLine::GetSpaceNonTimedForTime(float rTime)
{
    //returns the space required for non-timed objects placed at time rTime

    if (m_itStart == m_aMainTable.end() ||
        (m_itNote != m_aMainTable.end() && !IsEqualTime((*m_itNote)->m_rTimePos, rTime)) )
        return 0.0f;        //there are no objects
    else
    {
        lmItEntries it = m_itNote;      //to point to last non-timed object
        --it;
        return (*it)->m_xFinal - (*m_itStart)->m_xLeft;
    }
}

lmLUnits lmTimeLine::GetAnchorForTime(float rTime)
{
	//When enetring this method iterator m_it points to the next timed entry or
	//to end iterator.
	//If next entry time is rTime, returns its xAnchor position. Else returns 0

	if (m_it != m_aMainTable.end() && IsEqualTime((*m_it)->m_rTimePos, rTime) )
		return (*m_it)->m_uxAnchor;
	else
		return 0.0f;
}

lmLUnits lmTimeLine::IntitializeSpacingAlgorithm(float rFactor, bool fCreateCriticalLine)
{
    //First, default space is assigned to each staffobj. Then, iterator m_it is initialized and
    //all non-timed objects at timepos 0 are processed to determine the minimum start position
    //for the first note/rest, and this position is provisionally assigned to the first
    //note/rest.
    //This minimum tentativally position is returned

    //Assign space to all entries
    AssignSpace(rFactor);

	//Initialize iterator
	m_it = m_aMainTable.begin();
	m_uxCurPos = (*m_it)->m_xLeft + (*m_it)->GetTotalSize();    //alpha entry after space

    //here default space is assigned to each staffobj and m_uxCurPos contains the alpha entry
    //after space.
    //Now we are going to process all non-timed objects at timepos 0 to determine the
    //minimum start position for the first note/rest

    bool fAddSpace = true;
    if (m_it != m_aMainTable.end())
    {
	    m_itStart = ++m_it;

	    //iterate until we reach a timed object or end of line
	    //This loop will process non-note/rest objects (clefs, key sign, etc.)
	    bool fThereAreObjects = false;
        lmItEntries itEnd = m_aMainTable.end();
	    while (m_it != itEnd && (*m_it)->m_rTimePos < 0.0f)
        {
		    fThereAreObjects = true;

            //if the prolog (clef, time and key) is processed add some spacing before first
            //non-prolog object
            if (fAddSpace && (*m_it)->m_pSO &&
                    !((*m_it)->m_pSO->IsClef() ||
                      (*m_it)->m_pSO->IsKeySignature() ||
                      (*m_it)->m_pSO->IsTimeSignature()) )
            {
                fAddSpace = false;
		        m_uxCurPos += m_pOwner->TenthsToLogical(m_rSpaceAfterProlog, 1);
            }

		    //move the shape and update the entry data
            (*m_it)->Reposition(m_uxCurPos);
            //wxLogMessage(_T("[lmTimeLine::IntitializeSpacingAlgorithm] ID=%d, m_uxCurPos=%.2f"),
            //    ((*m_it)->m_pSO ? (*m_it)->m_pSO->GetID() : 0), m_uxCurPos);

		    //advance the width and spacing
		    m_uxCurPos += (*m_it)->GetTotalSize();

            ++m_it;
        }
    }

    //Here m_uxCurPos conatins the minimun feasible position for the first note/rest.
	//Tentatively, we are going to assign this position to the first note/rest
    m_itNote = m_it;
	if (m_it != m_aMainTable.end())
	{
		(*m_it)->m_xLeft = m_uxCurPos;
	}

	////DBG -----------------------------------------------------------------
	//if (m_it != m_aMainTable.end())
    //    wxLogMessage(_T("Not timed. More entries. Next at time=%.2f ID=%d"),
	//				 //(*m_it)->m_rTimePos, (*m_it)->m_pSO->GetID() );
    //else
    //    wxLogMessage(_T("Not timed. No more entries."));
	////END_DBG -------------------------------------------------------------

	//Return the position assigned to the first note/rest
    return m_uxCurPos;
}

float lmTimeLine::ProcessTimepos(float rTime, lmLUnits uxPos, float rFactor,
                                 bool fCreateCriticalLine, lmLUnits* pMaxPos)
{
	//Starting at current position, explores the line.
	//Set the position of all time positioned objects placed at time rTime, until
    //we reach a time greater that rTime. The position for this next rTime object is
    //also assigned tentatively.
	//If during this traversal we find not timed objects, they are ignored and when
	//reached next rTime we go back assigning positions to them.
	//Returns the next timepos in this line, or -1 if no more entries.
    //Updates *pMaxPos with the maximum xPos reached after adding its width to
    //current pos

	//When entering this method, iterator m_it must point to a timed object. If it is
	//placed at time rTime, process it and all the following having the same timepos,
	//until we find the next timepos or we arrive to end of line.
	//Not-timed objects are skipped
    *pMaxPos = 0.0f;
    const lmItEntries itEnd = m_aMainTable.end();
	lmItEntries itLastNotTimed = itEnd;
	while (m_it != itEnd &&
			(IsEqualTime((*m_it)->m_rTimePos, rTime) || (*m_it)->m_rTimePos < 0.0f) )
    {
		//if not-timed take note and continue
		if ((*m_it)->m_rTimePos == -1.0f)
			itLastNotTimed = m_it;

        //else if contains a StaffObj (in the omega entry there might be no barline)
		//assign it a final position
        else if ((*m_it)->m_pSO)
        {
			//move the shape and update the entry data
            (*m_it)->Reposition(uxPos + (*m_it)->m_uxAnchor);

			//compute new current position
			lmLUnits uxFinal = uxPos + (*m_it)->GetTotalSize();
			m_uxCurPos = wxMax(uxFinal, m_uxCurPos);
            uxFinal = uxPos + (*m_it)->m_uSize;
            *pMaxPos = wxMax(*pMaxPos, uxFinal);
        }
        m_it++;
    }

	//Here we have arrived to the next timepos or to the end of line.
    m_itStart = itLastNotTimed;
    m_itNote = m_it;
	if (m_it != m_aMainTable.end())
	{
		//next timepos. Assign position tentatively
		(*m_it)->m_xLeft = m_uxCurPos;

		//go back to assign positions to not-timed objects
		if (itLastNotTimed != m_aMainTable.end())
		{
            lmLUnits uxCurPos = m_uxCurPos;
            lmLUnits rSpaceAfterClef =
                m_pOwner->TenthsToLogical(m_rSpaceAfterIntermediateClef,
                                        (*m_it)->m_pSO->GetStaffNum() );
            if ((*itLastNotTimed)->m_pSO->IsClef())
                uxCurPos -= rSpaceAfterClef;

            lmItEntries itStart = m_it;
            lmItEntries it = m_it;
            it--;
            while (it != m_aMainTable.begin() && (*it)->m_rTimePos < 0.0f)
            {
                uxCurPos -= (*it)->GetTotalSize();
			    (*it)->Reposition(uxCurPos);
                it--;
            }
            //Here we have arrived to a note/rest.

            //save ptr to first non-timed object
            m_itStart = it;
            ++m_itStart;

            //Verify that note/rest left position is lower than the last assigned
            //position. Otherwise we have to shift the not-timed objects.
            if (it != m_aMainTable.begin())
            {
                if ((*it)->m_xLeft + (*it)->m_uSize >= uxCurPos)
                {
                    //Overlap. We have to shift the not-timed objects.
                    uxCurPos = (*it)->m_xLeft + (*it)->m_uSize +
                        m_pOwner->TenthsToLogical(m_rMinSpaceBetweenNoteAndClef,
                                                  (*it)->m_pSO->GetStaffNum() );
                    ++it;
                    while (it != itStart)
                    {
 			            (*it)->Reposition(uxCurPos);
                        uxCurPos += (*it)->GetTotalSize();
                        ++it;
                    }
		            //We have arrived to next timepos. Assign position tentatively
                    m_uxCurPos = uxCurPos;
                    if ((*itLastNotTimed)->m_pSO->IsClef())
                        m_uxCurPos += rSpaceAfterClef;
                    *pMaxPos = uxCurPos;
		            (*itStart)->m_xLeft = m_uxCurPos;
                }
            }

		}
	}
    else
    {
        //we have arrived to the end of line, but it could be an omega entry without barline.
        //In that case, it is necessary to update its information
		if (itLastNotTimed != m_aMainTable.end() &&
            (*itLastNotTimed)->m_nType == eOmega &&
            (*itLastNotTimed)->m_rTimePos < 0.0f )
		{
		    (*itLastNotTimed)->Reposition(m_uxCurPos);
        }
    }

	//done. Return the next timepos in this line, or -1 if no more entries.
	if (m_it != m_aMainTable.end()) {
      //  wxLogMessage(_T("Timed. More entries. Next at time=%.2f ID=%d"),
					 //(*m_it)->m_rTimePos, (*m_it)->m_pSO->GetID() );
        return (*m_it)->m_rTimePos;
    }
    else {
        //wxLogMessage(_T("Timed. No more entries"));
        return -1.0f;
    }

}

float lmTimeLine::ComputeRequiredSpacingFactor(lmLUnits uNewBarSize)
{
    //wxLogMessage(_T("[lmTimeLine::ComputeRequiredSpacingFactor] New measure size=%.2f, current size=%.2f"),
    //            uNewBarSize, this->GetLineWidth());

    lmLUnits uTotalSize = 0.0f;
    lmLUnits uTotalFixed = 0.0f;
    lmLUnits uTotalVariable = 0.0f;
    int nVarNotes = 0;  //num of note/rest with variable spacing

    lmItEntries it = m_aMainTable.begin();
    for (; it != m_aMainTable.end(); ++it)
    {
		uTotalSize += (*it)->m_uSize;
		uTotalFixed += (*it)->m_uFixedSpace;
		uTotalVariable += (*it)->m_uVariableSpace;
        if ((*it)->m_pSO && (*it)->m_pSO->IsNoteRest() && (*it)->m_uVariableSpace > 0.0f)
            nVarNotes++;
    }
    lmLUnits uOldBarSize = uTotalSize + uTotalFixed + uTotalVariable;
  //  wxLogMessage(_T("[lmTimeLine::ComputeRequiredSpacingFactor] Shapes=%.2f, fixed=%.2f, var=%.2f, total=%.2f, NumNotes=%d"),
		//uTotalSize, uTotalFixed, uTotalVariable, uOldBarSize, nVarNotes);

    //compute new required spacing factor A':
    //
    //                   MW’ - MW                MW' - MW
    //   A' = A + ----------------------- = A + ----------
    //            Smin.SUM(Log2(Di/Dmin))          rBeta
    //

    float rOldFactor = m_pOwner->SpacingFactor();
    float rNewFactor = rOldFactor + (m_rBeta > 0.0f ? (uNewBarSize - uOldBarSize)/m_rBeta : 0.0f);

  //  wxLogMessage(_T("[lmTimeLine::ComputeRequiredSpacingFactor] old factor=%.2f, new factor=%.2f"),
		//m_pOwner->SpacingFactor(), rNewFactor);
    return rNewFactor;
}

wxString lmTimeLine::DumpMainTable()
{
    wxString sMsg = wxString::Format(_T("TimeLine table dump. Instr=%d, voice=%d \n"),
									 m_nInstr, m_nVoice );
    sMsg += _T("===================================================================\n\n");

    if (m_aMainTable.size() == 0)
    {
        sMsg += _T("The table is empty.");
        return sMsg;
    }

    //headers
    sMsg += lmTimeposEntry::DumpHeader();

    //loop to dump table entries
    lmTimeposEntry* pTE;
    for (int i = 0; i < (int)m_aMainTable.size(); i++)
    {
        if (i % 4 == 0) {
            sMsg += wxT("----------------------------------------------------------------------------\n");
        }
        pTE = m_aMainTable[i];
        sMsg += pTE->Dump(i);
    }

    sMsg += _T("=== End of table ==================================================\n\n");
    return sMsg;

}

lmLUnits lmTimeLine::GetMaxXFinal()
{
    //returns the maximum x final of this line

    //now compute the maximum xFinal
    lmLUnits xFinal = 0;
    for (lmItEntries it = m_aMainTable.begin(); it != m_aMainTable.end(); ++it)
	{
		lmLUnits xPos = (*it)->m_xFinal;
        xFinal = wxMax(xPos, xFinal);
    }

    return xFinal;

}

void lmTimeLine::TerminateLineAfter(float rTime, lmLUnits uxFinal)
{
    //Current column has been splitted at time rTime. It is necessary to remove all entries
    //placed at timepos > rTime and make any necessary adjustments.
    //Also, all created shapes must be removed from its box containers and deleted

    //THIS METHOD IS NO LONGER USED. BUT IT WORKS.
    //Leaved just in case there is a need to use it again

    //wxLogMessage(_T("[lmTimeLine::TerminateTableAfter] Removing entries after time %.2f"), rTime);

    //remove all entries except Omega entry, if exists
    lmItEntries it;
    for (it = m_aMainTable.begin(); it != m_aMainTable.end(); ++it)
	{
		if (IsHigherTime((*it)->m_rTimePos, rTime))
            break;
	}
    wxASSERT(it != m_aMainTable.end());

    //here 'it' is pointing to firts entry to remove.
    //Proceed to remove entries, except the omega entry
    lmItEntries itStart = it;
    lmTimeposEntry* pOmega = (lmTimeposEntry*)NULL;
    for (; it != m_aMainTable.end(); ++it)
	{
        //if this is the omega entry, break loop
        if ((*it)->m_nType == eOmega)
        {
            pOmega = (*it);
            break;
        }

        //remove the shape, if there is one
        if ((*it)->m_pShape)
        {
            lmBox* pBox = (*it)->m_pShape->GetOwnerBox();
            wxASSERT(pBox);
            pBox->RemoveShape( (*it)->m_pShape );
            delete (*it)->m_pShape;
        }

        //remove this entry
		delete *it;
	}
    m_aMainTable.erase(itStart, m_aMainTable.end());

    //if Omega entry exists, adjusts values in this entry
    if (pOmega)
    {
        //re-insert omega entry
        m_aMainTable.push_back(pOmega);

        //adjust values
        pOmega->m_xInitialLeft = uxFinal;
        pOmega->m_xLeft = uxFinal;
        pOmega->m_xFinal = uxFinal;
    }
}

void lmTimeLine::ComputeBreakPoints(lmBreaksTable* pBT)
{
    //This method computes the break points for this line and adds them to received
    //break points table.
    //
    //Algorithm:
    //
    //In a first approach, add an entry for each timepos at which there is an object placed.
    //Assign priority 0.8 to all entries.
    //
    //Now lower or raise priority of some entries according to empiric rules:
    //  
    //  1. If there is a time signature, strongly penalize those timepos not in beat 
    //     position (priority *= 0.5)
    //    
    //  2. Do not split notes/rests. Penalize those entries occupied in some 
    //     line (priority *= 0.7).
    //
    //  3. Do not to break beams. Penalize those entries  in which, at some line, there
    //     is a beam (priority *=0.9).
    //
    //Finally, when all priorities have been computed, sort the table by priority (high to
    //low) and by space (max to min).
    //
    //In order to accelerate the computation of this table, LineTables must have all 
    //necesary data so that it doesn't become necessary to traverse the StaffObjs
    //colection again.

    //TODO: Add filters for priority
    const lmItEntries itEnd = m_aMainTable.end();
	lmItEntries it = m_aMainTable.begin();

    //skip initial non-timed entries
	for (it = m_aMainTable.begin(); it != itEnd && IsLowerTime((*it)->m_rTimePos, 0.0f); ++it);
    if (it == itEnd) return;

    //process current time
    float rTime = (*it)->m_rTimePos;
    lmLUnits uxStart = (*it)->m_xFinal;
    lmLUnits uxWidth = (*it)->m_uSize;
    lmLUnits uxBeam = 0.0f;
    bool fInBeam = false;
    lmStaffObj* pSO = (*it)->m_pSO;
    if (pSO && pSO->IsNoteRest() && ((lmNoteRest*)pSO)->IsBeamed())
    {
        fInBeam = true;
        lmStaffObj* pSOEnd = ((lmNoteRest*)pSO)->GetBeam()->GetEndNoteRest();
        lmShape* pShape = pSOEnd->GetShape();
        uxBeam = pShape->GetXLeft() + pShape->GetWidth();
    }

	while (it != itEnd)
    {
		if (IsEqualTime((*it)->m_rTimePos, rTime) || IsLowerTime((*it)->m_rTimePos, 0.0f))
        {
		    //skip any not-timed entry
            if (IsEqualTime((*it)->m_rTimePos, rTime))
            {
                uxWidth = wxMax(uxWidth, (*it)->m_uSize);
                lmStaffObj* pSO = (*it)->m_pSO;
                if (pSO && pSO->IsNoteRest() && ((lmNoteRest*)pSO)->IsBeamed())
                {
                    fInBeam = true;
                    lmStaffObj* pSOEnd = ((lmNoteRest*)pSO)->GetBeam()->GetEndNoteRest();
                    lmShape* pShape = pSOEnd->GetShape();
                    uxBeam = wxMax(uxBeam, pShape->GetXLeft() + pShape->GetWidth());
                }
            }
        }
        else
        {
            //new timepos. Add entry for previous timepos
            pBT->AddEntry(rTime, uxStart, uxWidth, fInBeam, uxBeam);

            //start collecting data for new timepos
            rTime = (*it)->m_rTimePos;
            uxStart = (*it)->m_xFinal;
            uxWidth = (*it)->m_uSize;
            lmStaffObj* pSO = (*it)->m_pSO;
            if (pSO && pSO->IsNoteRest() && ((lmNoteRest*)pSO)->IsBeamed())
            {
                fInBeam = true;
                lmStaffObj* pSOEnd = ((lmNoteRest*)pSO)->GetBeam()->GetEndNoteRest();
                lmShape* pShape = pSOEnd->GetShape();
                uxBeam = pShape->GetXLeft() + pShape->GetWidth();
            }
            else
            {
                uxBeam = 0.0f;
                fInBeam = false;
            }
       }
		++it;
    }

    pBT->AddEntry(rTime, uxStart, uxWidth, fInBeam, uxBeam);

    wxLogMessage( pBT->Dump() );
}




//=====================================================================================
//lmCriticalLine
//=====================================================================================

void lmCriticalLine::AddNonTimed(lmTimeLine* pLine, float rTime)
{
    //Add all non-timed objects in received line to this critical line

    if (!pLine) return;
	lmItEntries it = pLine->m_itStart;
	lmItEntries itEnd = pLine->m_aMainTable.end();
	while (it != pLine->m_itNote && it != itEnd)
    {
        m_aMainTable.push_back(*it);
        (*it)->Dump(0);
        ++it;
    }
}

void lmCriticalLine::AddEntry(lmTimeposEntry* pEntry)
{
    //Add the received timed entry to this critical line

    if (!pEntry) return;
    m_aMainTable.push_back(pEntry);
    pEntry->Dump(0);
}

void lmCriticalLine::ComputeBeta(float rFactor)
{
	//Explores all note/rest entries in this line and computes the beta factor

	static const float rLog2 = 0.3010299956640f;		// log(2)

    if (m_pOwner->SpacingMethod() != esm_PropConstantFixed) return;

    m_rBeta = 0.0f;
    for (lmItEntries it = m_aMainTable.begin(); it != m_aMainTable.end(); ++it)
	{
        if ((*it)->m_nType == eStaffobj &&
            (*it)->m_pSO->IsVisible() &&
            (*it)->m_pSO->IsNoteRest() )
        {
            float rVar = log(((lmNoteRest*)((*it)->m_pSO))->GetDuration() / m_rDmin) / rLog2;
	        int iStaff = (*it)->m_pSO->GetStaffNum();
	        lmLUnits uTotalSize = m_pOwner->TenthsToLogical(m_rMinSpace, iStaff);		//Space(Di) = Smin
            //wxLogMessage(_T("[lmCriticalLine::ComputeBeta] id=%d, rVar=%f, uTotalSize=%.2f"),
            //    (*it)->m_pSO->GetID(), rVar, uTotalSize);
            if (rVar > 0.0f)
            {
		        rVar *= uTotalSize;         //rVar = Smin*log2(Di/Dmin)
                m_rBeta += rVar;            //rBeta = SUM( Smin*log2(Di/Dmin) )
	        }
        }
    }
    //wxLogMessage(_T("[lmCriticalLine::ComputeBeta] factor=%f, Beta = %f"), rFactor, m_rBeta);
}

lmLUnits lmCriticalLine::RedistributeSpace(lmLUnits uNewBarSize, lmLUnits uNewStart,
                                           lmLUnits uOldBarSize)
{
    //Creates the Pos<->Time table (m_PosTimes)

#if lmDUMP_TABLES
    wxLogMessage(_T("[lmCriticalLine::RedistributeSpace] Before space redistribution. uNewBarSize=%.2f, uNewStart=%.2f, uOldBarSize=%.2f"),
                 uNewBarSize, uNewStart, uOldBarSize );
    wxLogMessage( DumpMainTable() );
#endif

    lmLUnits uBarPosition = 0.0f;
    lmLUnits uOldStart = m_aMainTable.front()->m_xLeft;

    //all non-timed entries at beginning are only re-located
    //all non-timed entries, at beginning, marked as fProlog must be only re-located
    lmLUnits uShiftReloc = uNewStart - uOldStart;
    lmItEntries it = m_aMainTable.begin();
    while (it != m_aMainTable.end() && (*it)->m_rTimePos < 0.0f)
    {
        if ((*it)->m_pShape)
        {
            if ((*it)->m_fProlog)
                (*it)->m_xLeft += uShiftReloc;
            else
			    break;
        }
        ++it;
    } 

    if (it == m_aMainTable.end())
         return uBarPosition;
     
    //first timed entry marks the start point for repositioning.
    //Compute proportion factor
    lmLUnits uStartPos = (*it)->m_xLeft - (*it)->m_uxAnchor;
    lmLUnits uDiscount = uStartPos - uOldStart;
    float rProp = (uNewBarSize-uDiscount) / (uOldBarSize-uDiscount);
    
	//Reposition the remainder entries
    for (; it != m_aMainTable.end(); ++it)
	{
        lmTimeposEntry* pTPE = *it;
        if (pTPE->m_nType == eStaffobj)
        {
            lmLUnits uOldPos = pTPE->m_xLeft - pTPE->m_uxAnchor;
            lmLUnits uShift = uDiscount + (uNewStart + (uOldPos - uStartPos) * rProp) - uOldPos;
            lmPosTimeEntry tPosTime = {pTPE, pTPE->m_rTimePos, pTPE->m_xLeft + uShift};  
            m_PosTimes.push_back(tPosTime);
        }
        else if (pTPE->m_nType == eOmega)
        {
            //there might be no barline.
            if (pTPE->m_pSO)
            {
                uBarPosition = uNewStart + uNewBarSize - pTPE->m_uSize;
                lmLUnits uShiftBar = uBarPosition - pTPE->m_xLeft;
                lmPosTimeEntry tPosTime = {pTPE, pTPE->m_rTimePos, pTPE->m_xLeft + uShiftBar};  
                m_PosTimes.push_back(tPosTime);
				//wxLogMessage(_T("[lmCriticalLine::RedistributeSpace] Reposition bar: uBarPosition=%.2f, uShiftBar=%.2f"),
				//			uBarPosition, uShiftBar );
            }
        }
    }

#if lmDUMP_TABLES
    wxLogMessage(_T("Critical line. After space redistribution"));
    wxLogMessage( DumpMainTable() );
    wxLogMessage( DumpPosTimes() );
#endif

    return uBarPosition;
}

void lmCriticalLine::InitializeToGetTimepos()
{
    m_rLastTimepos = -1.0f;
    m_it = m_PosTimes.begin();       
}

lmLUnits lmCriticalLine::GetPosForStaffobj(lmStaffObj* pSO, float rTime)
{
    //find position for non-timed object pSO. 
    //Iterator is pointing to non-timed object after time rTime

    //save current entry (first non-timed object after time rTime)
    std::vector<lmPosTimeEntry>::iterator itNonTimed = m_it;

    //find object pSO
    for (; m_it != m_PosTimes.end() && !IsHigherTime((*m_it).rTimepos, rTime); ++m_it)
	{
        if ((*m_it).pTPE->m_pSO == pSO)
            return (*m_it).uxPos;
    }

    //the object is not in critical line.
    //Return position for first non-timed object after current time
    wxASSERT(itNonTimed != m_PosTimes.end());
    return (*itNonTimed).uxPos;
}

lmLUnits lmCriticalLine::GetTimepos(float rTime)
{
    wxASSERT(IsHigherTime(rTime, m_rLastTimepos));

    for (; m_it != m_PosTimes.end(); ++m_it)
	{
        if (IsEqualTime((*m_it).rTimepos, rTime))
            return (*m_it).uxPos;
    }
    wxASSERT(false);
    return 0.0f;       //compiler happy
}

wxString lmCriticalLine::DumpPosTimes()
{
    //dump Pos-Times table

    wxString sMsg = _T("Pos-Times table\n");
    sMsg += _T("===============================\n\n");

    if (m_PosTimes.size() == 0)
    {
        sMsg += _T("The table is empty.");
        return sMsg;
    }

    //headers
    //           .......+ .......+   ...+ ..+   +  ..........+........+........+........+........+........+........+........+........+......+
    sMsg +=  _T(" TimePos    uxPos\n");

    //loop to dump entries
    int iE = 0;
    std::vector<lmPosTimeEntry>::iterator itDbg;
    for (itDbg = m_PosTimes.begin(); itDbg != m_PosTimes.end(); ++itDbg, ++iE)
	{
        sMsg += wxString::Format(_T("%8.2f %8.2f "), (*itDbg).rTimepos, (*itDbg).uxPos );
        sMsg += (*itDbg).pTPE->Dump(iE);
    }

    return sMsg;
}




//=====================================================================================
//lmTimeposTable
//=====================================================================================

lmTimeposTable::lmTimeposTable()
{
    for(int i=0; i < lmMAX_STAFF; i++)
        m_nCurVoice[i] = 0;
	m_pCurEntry = (lmTimeposEntry*)NULL;
    m_pCriticalLine = (lmCriticalLine*)NULL;
}

lmTimeposTable::~lmTimeposTable()
{
	CleanTable();
}

void lmTimeposTable::SetParameters(float rSpacingFactor, lmESpacingMethod nSpacingMethod,
                                   lmTenths nSpacingValue)
{
    m_rSpacingFactor = rSpacingFactor;
    m_nSpacingMethod = nSpacingMethod;
    m_rSpacingValue = nSpacingValue;
}

void lmTimeposTable::CleanTable()
{
    //This method must be invoked before using the table. Can be also invoked as
    //many times as desired to clean the table and reuse it.

    for(int i=0; i < lmMAX_STAFF; i++)
        m_nCurVoice[i] = 0;

	for (lmItTimeLine it=m_aLines.begin(); it != m_aLines.end(); ++it)
	{
		delete *it;
	}
	m_aLines.clear();

    if (m_pCriticalLine)
    {
        m_pCriticalLine->m_aMainTable.clear();      //to avoid deleting entries. They are owned by real lines!
        delete m_pCriticalLine;
        m_pCriticalLine = (lmCriticalLine*)NULL;
    }
}

void lmTimeposTable::StartLines(int nInstr, lmLUnits uxStart, lmVStaff* pVStaff,
                                lmLUnits uSpace)
{
	int nNumStaves = pVStaff->GetNumStaves();
    wxASSERT(nNumStaves < lmMAX_STAFF);

    for(int iS=0; iS < nNumStaves; iS++)
    {
        m_nCurVoice[iS] = iS+1;
		m_pStaff[iS] = pVStaff->GetStaff(iS+1);
        StartLine(nInstr, iS+1, uxStart, uSpace);
    }
}

void lmTimeposTable::StartLine(int nInstr, int nVoice, lmLUnits uxStart, lmLUnits uSpace)
{
    //Start a new line for instrument nInstr (0..n-1)
	//If this is the first line for instrument nInstr, all non-voiced StaffObj
	//will be assigned to this line, as well as the first voice encountered

    if (uxStart < 0.0f)
    {
        //Copy start position from first line
        uxStart = m_aLines.front()->m_aMainTable.front()->m_xLeft;
    }


	lmTimeLine* pLine = new lmTimeLine(this, nInstr, nVoice, uxStart, uSpace);
	m_aLines.push_back(pLine);
	m_pCurEntry = pLine->m_aMainTable.back();
	m_itCurLine = m_aLines.end();
	m_itCurLine--;

	//wxLogMessage(_T("[lmTimeposTable::StartLine] New line started for instr=%d, voice=%d"),
 //       nInstr, nVoice);
}

void lmTimeposTable::CloseLine(lmStaffObj* pSO, lmShape* pShape, lmLUnits xStart)
{
	//close current line.

    m_pCurEntry = (*m_itCurLine)->AddEntry(eOmega, pSO, pShape, false);
    m_pCurEntry->m_xLeft = xStart;
    m_pCurEntry->m_xInitialLeft = xStart;
}

lmTimeLine* lmTimeposTable::FindLine(int nInstr, int nVoice)
{
	//Find the line for this nInstr/nVoice

	//find instrument
	for (lmItTimeLine it=m_aLines.begin(); it != m_aLines.end(); ++it)
	{
		if ((*it)->m_nInstr == nInstr
			&& ((*it)->m_nVoice == 0 || (*it)->m_nVoice == nVoice ))
		{
			return *it;
		}
	}
	//instrument not found
	return (lmTimeLine*) NULL;
}

void lmTimeposTable::AddEntry(int nInstr, lmStaffObj* pSO, lmShape* pShape, bool fProlog,
                              int nStaff)
{
    //determine voice
    int nVoice;
	if (nStaff == 0)
	{
		if (pSO->GetClass() == eSFOT_NoteRest)
			nVoice = ((lmNoteRest*)pSO)->GetVoice();
		else
			nVoice = m_nCurVoice[ pSO->GetStaffNum() - 1 ];
	}
	else
		nVoice = m_nCurVoice[nStaff - 1];

	//create the entry
	AddEntry(nInstr, nVoice, pSO, pShape, fProlog);
}

void lmTimeposTable::AddEntry(int nInstr, int nVoice, lmStaffObj* pSO, lmShape* pShape,
                              bool fProlog)
{
	//Add an entry to current line for the specified nInstr/nVoice.

    //wxLogMessage(_T("[lmTimeposTable::AddEntry] instr=%d, voice=%d, SO type = %d, staff=%d"),
    //    nInstr, nVoice, pSO->GetClass(), pSO->GetStaffNum() );

	//find line
	lmItTimeLine itInstr = m_aLines.end();
	for (lmItTimeLine it=m_aLines.begin(); it != m_aLines.end(); ++it)
	{
		if ((*it)->m_nInstr == nInstr)
		{
			//first line for this instrument found, Save ptr to it
			if (itInstr == m_aLines.end()) itInstr = it;

			if ( (*it)->m_nVoice == 0 || (*it)->m_nVoice == nVoice )
			{	//voice found. Save current line and add entry
				m_itCurLine = it;
				m_pCurEntry = (*it)->AddEntry(eStaffobj, pSO, pShape, fProlog);
                //wxLogMessage(_T("\tEntry added to line for voice=%d"), (*it)->m_nVoice);

				//update voice
                if ((*it)->m_nVoice == 0 && nVoice != 0) {
					(*it)->m_nVoice = nVoice;
                    //m_nCurVoice = nVoice;
                }
				return;
			}
		}
	}
	//not found. Start new line
	wxASSERT(itInstr != m_aLines.end());	//a line for the instrument must exist
	StartLine(nInstr, nVoice);
	m_pCurEntry = (*m_itCurLine)->AddEntry(eStaffobj, pSO, pShape, fProlog);
}

//-------------------------------------------------------------------------------------
//  methods to access to info
//-------------------------------------------------------------------------------------

lmBarline* lmTimeposTable::GetBarline()
{
    //returns the barline object in the omega entry, if any

    //we know that the bar is in the critical line
    if (!m_pCriticalLine)
        return (lmBarline*)NULL;

    lmTimeposEntry* pEntry = m_pCriticalLine->m_aMainTable.back();    //the omega entry
    wxASSERT(pEntry->m_nType == eOmega);
    return (lmBarline*)pEntry->m_pSO;
}

//-------------------------------------------------------------------------------------
//  methods to compute results
//-------------------------------------------------------------------------------------

lmLUnits lmTimeposTable::GetGrossBarSize()
{
    //returns the maximum measure size of all lines

    //compute the maximum xFinal
    lmLUnits xFinal = 0;
    for (lmItTimeLine it = m_aLines.begin(); it != m_aLines.end(); ++it)
	{
		lmLUnits xPos = (*it)->GetMaxXFinal();
        xFinal = wxMax(xFinal, xPos);
    }

    return xFinal - GetStartOfBarPosition();
}

lmLUnits lmTimeposTable::GetStartOfBarPosition()
{
    //returns the x position for the start of the bar column

    //the bar starts at the xLeft of the alfa entry
    lmTimeposEntry* pEntry = m_aLines[0]->m_aMainTable.front();    //the alfa entry
    wxASSERT(pEntry->m_nType == eAlfa);
    return pEntry->m_xLeft;
}

void lmTimeposTable::EndOfData()
{
    //this method is invoked to inform that all data has been suplied. Therefore, we
    //can do any preparatory work, not to be repeated if re-spacing.

}

lmLUnits lmTimeposTable::DoSpacing(bool fTrace)
{
#if lmDUMP_TABLES
    wxLogMessage( DumpTimeposTable() );
#endif

    lmLUnits uSize = ComputeSpacing(m_rSpacingFactor);

#if lmDUMP_TABLES
    wxLogMessage( DumpTimeposTable() );
#endif

    return uSize;
}

lmLUnits lmTimeposTable::ComputeSpacing(float rFactor)
{
    //Spacing algorithm.
    //Computes spacing and, if not created, computes also the critical line.
    //Returns the resulting measure size

	//Initializations
    //In following loop, the minimum start position for the first note/rest in each line is
    //computed and we take the maximum of them.
    //Also, we determine in which line the non-timed objects take more space.
    bool fCreateCriticalLine = (m_pCriticalLine == (lmCriticalLine*)NULL );
    float rTime = 0.0f;
	lmLUnits uxPos = 0.0f;
    lmTimeLine* pLongestNonTimedLine = (lmTimeLine*)NULL;
    lmItTimeLine itEnd = m_aLines.end();
	for (lmItTimeLine it=m_aLines.begin(); it != itEnd; ++it)
	{
		lmLUnits uxStartPos = (*it)->IntitializeSpacingAlgorithm(rFactor, fCreateCriticalLine);
        if (uxPos < uxStartPos)
        {
            pLongestNonTimedLine = *it;
		    uxPos = uxStartPos;
        }
	}

    //Now we create the critical line (if requested) and enter a loop to process notes/rests
    //at rTime and advance to next rTime.
    //The loop is exited when no more timepos are found

    if (fCreateCriticalLine)
    {
        //create the line
        m_pCriticalLine = new lmCriticalLine(this);

        //add alpha entry copying it from first line
        m_pCriticalLine->AddEntry( m_aLines.front()->m_aMainTable.front() );
    }

	bool fContinue = true;
	while(fContinue)
    {
        //each loop cycle is to process a time pos. We start at rTime=0

        //Here the first note/rest in each line, at current timepos, has been positioned.
        //Proceed to determine which one is most at right and assign that position to
        //all notes/rest at current timepos.

        //wxLogMessage(_T("Dump for time %.2f"), rTime);
        //wxLogMessage(this->DumpTimeposTable());

		//Determine minimum common x position for timepos rTime.
        //If computing critical line, also minimum note/rest for timepos rTime is determined
        lmTimeposEntry* pShortestEntry = (lmTimeposEntry*)NULL;
        float rMinDuration = 100000.0f;       //any too big value
		for (lmItTimeLine it=m_aLines.begin(); it != m_aLines.end(); ++it)
		{
			lmLUnits uxMinPossiblePos = (*it)->GetMinPossiblePosForTime(rTime);     //returns m_xLeft for that line
			lmLUnits uxMinRequiredPos = (*it)->GetMinRequiredPosForTime(rTime);
            lmLUnits uxMinPos = wxMax(uxMinPossiblePos, uxMinRequiredPos);
            lmLUnits uxObjAnchor = (*it)->GetAnchorForTime(rTime);
            if (uxObjAnchor < 0.0f) uxMinPos -= uxObjAnchor;
			uxPos = wxMax(uxPos, uxMinPos);

            //If computing critical line, determine minimum note/rest for current timepos
            if (fCreateCriticalLine)
            {
                lmTimeposEntry* pEntry = (*it)->GetMinNoteRestForTime(rTime);
                if (pEntry)
                {
                    float rDuration = ((lmNoteRest*)(pEntry->m_pSO))->GetDuration();
                    if (rDuration < rMinDuration)
                    {
                        rMinDuration = rDuration;
                        pShortestEntry = pEntry;
                    }
                }
            }

		    //wxLogMessage(_T("[lmTimeposTable::ComputeSpacing] Computing minimum xPos for time %.2f, uxMinPossiblePos=%.2f, uxMinRequiredPos=%.2f, uxMinPos=%.2f, uxObjAnchor=%.2f, uxPos=%.2f"),
			//    rTime, uxMinPossiblePos, uxMinRequiredPos, uxMinPos, uxObjAnchor, uxPos);
		}
		//wxLogMessage(_T("[lmTimeposTable::ComputeSpacing] Setting timed objects at time %.2f, min xPos=%.2f"),
		//	rTime, uxPos);

        //Here uxPos contains the minimum xPos to align all notes/rest at current timepos.
        //If creating the critical line, also following variables are meaningful:
        //  pShortestEntry: points to entry for the minimum note/rest
        //  pLongestNonTimedLine: point to the line containing the longest non-timed objects
        //
        //Now we will add entries to critical line (if creation requested) and process
        //all timed objects, placed at rTime, to reposition them.

        //If computing critical line, add longest non-timed objects and shorter
        //note/rest found to the critical line.
        if (fCreateCriticalLine)
        {
            m_pCriticalLine->AddNonTimed( pLongestNonTimedLine, rTime );
            m_pCriticalLine->AddEntry( pShortestEntry );
        }

		//Process all timed objects placed at time rTime to reposition them and compute
        //next timepos start position. This is done in each line by method ProcessTimepos(),
        //which returns the next timepos in each line and updates uxObjMaxPos with the
        //start position for next timepos
		fContinue = false;
		float rNextTime = 10000.0f;		//any too big value
        lmLUnits uxMaxPos = 0.0f;
		for (lmItTimeLine it=m_aLines.begin(); it != m_aLines.end(); ++it)
		{
            lmLUnits uxObjMaxPos;
			float rNewTime = (*it)->ProcessTimepos(rTime, uxPos, rFactor,
                                                   fCreateCriticalLine, &uxObjMaxPos);
			if (IsHigherTime(rNewTime, 0.0f))
			{
				fContinue = true;
				rNextTime = wxMin(rNextTime, rNewTime);
			}
            uxMaxPos = wxMax(uxMaxPos, uxObjMaxPos);

		}
        uxPos = uxMaxPos;

        //If computing critical line, determine in which line the non-timed objects take
        //more space.
        if (fCreateCriticalLine)
        {
	        lmLUnits udxNonTimed = -1.0f;   //any negative value
            pLongestNonTimedLine = (lmTimeLine*)NULL;
		    for (lmItTimeLine it=m_aLines.begin(); it != m_aLines.end(); ++it)
		    {
		        lmLUnits uxNonTimedSpace = (*it)->GetSpaceNonTimedForTime(rNextTime);
                if (udxNonTimed < uxNonTimedSpace)
                {
                    pLongestNonTimedLine = *it;
		            udxNonTimed = uxNonTimedSpace;
                }
            }
        }
        //wxLogMessage( DumpTimeposTable() );

		//advance to next time
		rTime = rNextTime;
	}

	//Get measure size and, if computing critical line, determine longest omega entry
	lmLUnits uMeasureSize = 0.0f;
    lmTimeposEntry* pOmegaEntry = (lmTimeposEntry*)NULL;
    lmLUnits uOmegaSize = -10000.0f;        //any too low value
    int iLine=0;
	for (lmItTimeLine it=m_aLines.begin(); it != m_aLines.end(); ++it)
	{
		lmLUnits uSize = (*it)->GetLineWidth();
		uMeasureSize = wxMax(uMeasureSize, uSize);

        //wxLogMessage(_T("[lmTimeposTable::ComputeSpacing] Dump of line %d"), iLine++);
        //wxLogMessage( (*it)->DumpMainTable() );

        if (fCreateCriticalLine)
        {
            lmTimeposEntry* pOmega = (*it)->m_aMainTable.back();
            if (pOmega->m_nType == eOmega && uOmegaSize < pOmega->GetTotalSize())
            {
                uOmegaSize = pOmega->GetTotalSize();
                pOmegaEntry = pOmega;
            }
        }
	}

    //if computing critical line, add the omega entry and compute beta factor
    if (fCreateCriticalLine && pOmegaEntry)
    {
        m_pCriticalLine->AddEntry( pOmegaEntry );
        //m_pCriticalLine->ComputeBeta(rFactor);
    }


    ////DBG
    //if (m_pCriticalLine)
    //{
    //    wxLogMessage(_T("[lmTimeposTable::ComputeSpacing] Dump of critical line:"));
    //    wxLogMessage( m_pCriticalLine->DumpMainTable() );
    //}

    //wxLogMessage(_T("[lmTimeposTable::ComputeSpacing] measure size = %.2f"), uMeasureSize);
	return uMeasureSize;
}

lmLUnits lmTimeposTable::RedistributeSpace(lmLUnits uNewBarSize, lmLUnits uNewStart)
{
    //Shift the position of all StaffObjs by the amount given by the difference between current
    //bar start position and the new desired start position.
    //In addition, the position of the barline at the end of this bar is also shifted so that
    //the new width on the bar becomes nNewBarWidth.
    //
    //Parameters:
    //   nNewBarWidth - the new width that this bar will have.
    //   uNewStart - the new left position for the start of this bar
    //
    //Results and return value:
    //   The new positions are stored in the StaffObjs
    //   The start position for next measure is retuned.

    
    //Critical line contains all existing timepos in this column.
    //Compute new positions for each timepos
    lmLUnits uOldBarSize = this->GetGrossBarSize();
    m_pCriticalLine->RedistributeSpace(uNewBarSize, uNewStart, uOldBarSize);

    //transfer the new positions to all lines
	for (lmItTimeLine it=m_aLines.begin(); it != m_aLines.end(); ++it)
	{
		(*it)->RepositionShapes(m_pCriticalLine, uNewBarSize, uNewStart, uOldBarSize);
    }

    //wxLogMessage(_T("[lmTimeposTable::RedistributeSpace] uNewBarSize=%.2f, uNewStart=%.2f, uOldBarSize=%.2f"),
    //             uNewBarSize, uNewStart, uOldBarSize );

#if lmDUMP_TABLES
    wxLogMessage(_T("[lmTimeposTable::RedistributeSpace] After repositioning shapes"));
    wxLogMessage( DumpTimeposTable() );
#endif

    return uNewStart + uNewBarSize;
}

lmLUnits lmTimeposTable::TenthsToLogical(lmTenths rTenths, int nStaff)
{
    wxASSERT(nStaff > 0);

	return m_pStaff[nStaff-1]->TenthsToLogical(rTenths);
}

bool lmTimeposTable::GetOptimumBreakPoint(lmLUnits uAvailable, float* prTime,
                                          lmLUnits* puWidth)
{
    //returns true if no break point found (exceptional case). In all other cases
    //returns the information (rTime, xPos) for the first entry with space <= uAvailable
    //and with maximum priority

    lmBreaksTable* pBreaks = ComputeBreaksTable();

    wxLogMessage( DumpTimeposTable() );

    //select highest entry with space <= uAvailable
    lmBreaksTimeEntry* pBTE = pBreaks->GetFirst();
    lmBreaksTimeEntry* pSel = (lmBreaksTimeEntry*)NULL;
    while (pBTE && pBTE->uxEnd <= uAvailable)
    {
        pSel = pBTE;
        pBTE = pBreaks->GetNext();
    }
    if (!pSel)
        return true;        //big problem: no break points!

    wxLogMessage(_T("[lmTimeposTable::GetOptimumBreakPoint] uAvailable=%.2f, returned=%.2f, time=%.2f"),
                 uAvailable, pSel->uxEnd, pSel->rTimepos);

    //return information
	*prTime = pSel->rTimepos;
    *puWidth = pSel->uxEnd;

    //breaks table no longer needed. Delete it
    delete pBreaks;

    return false;       //no problems. There are break points
}

void lmTimeposTable::TerminateTableAfter(float rTime, lmLUnits uxFinal)
{
    //Current column has been splitted at time rTime. It is necessary to remove all entries
    //placed at timepos > rTime and make any necessary adjustments. Also, all created shapes
    //must be removed from its box containers and deleted

    //THIS METHOD IS NO LONGER USED. BUT IT WORKS.
    //Leaved just in case there is a need to use it again

    //wxLogMessage(_T("[lmTimeposTable::TerminateTableAfter] Removing entries after time %.2f"), rTime);

	for (lmItTimeLine it=m_aLines.begin(); it != m_aLines.end(); ++it)
	{
        (*it)->TerminateLineAfter(rTime, uxFinal);
    }

    //wxLogMessage(_T("[lmTimeposTable::TerminateTableAfter] After removal:"));
    //wxLogMessage( DumpTimeposTable() );
}

lmBreaksTable* lmTimeposTable::ComputeBreaksTable()
{
    //This method computes the BreaksTable. This is a table sumarizing break points
    //information, that is, suitable places through all staves and voices where it is
    //possible to break a system and start a new one. The best break locations are
    //usually are the bar lines common to all staves. But in certain rare cases (i.e.
    //scores without time signature or having instrumens not sharing a common
    //time signature, cases in which it is requested to render the score in very narrow
    //paper, etc.) it is necessary to split music in unnusual points.
    //Returns the breaks table. CALLER MUST DELETE the table when no longer needed

    //Step 1. Build a table for each line
    std::vector<lmBreaksTable*> partialTables;
	for (lmItTimeLine itTL = m_aLines.begin(); itTL != m_aLines.end(); ++itTL)
	{
        lmBreaksTable* pBT = new lmBreaksTable();
        (*itTL)->ComputeBreakPoints(pBT);
        partialTables.push_back(pBT);
    }


    //Step 2. Combine the partial tables
    lmBreaksTable* pTotalBT = new lmBreaksTable();
    std::vector<lmBreaksTable*>::iterator itBT;
    for (itBT = partialTables.begin(); itBT != partialTables.end(); ++itBT)
    {
        if (pTotalBT->IsEmpty())
        {
            //just copy entries
            lmBreaksTimeEntry* pEP = (*itBT)->GetFirst();       //pEP Entry from Partial list
            while (pEP)
            {
                pTotalBT->AddEntry(pEP);
                pEP = (*itBT)->GetNext();
            }
        }
        else
        {
            //merge current table with total table
            //lmBreaksTimeEntry* pEP = (*itBT)->GetFirst();       //pEP Entry from Partial list
            //while (pEP)
            //{
            //    pTotalBT->AddEntry(pEP);
            //    pEP = (*itBT)->GetNext();
            //}
        }
    }


    //Delete partial tables, no longer needed
    for (itBT = partialTables.begin(); itBT != partialTables.end(); ++itBT)
        delete *itBT;
    partialTables.clear();

    wxLogMessage(_T("Total Breaks Table:"));
    wxLogMessage( pTotalBT->Dump() );

    //Step 3. Sort breaks table by priority and final x position
    //TODO

    return pTotalBT;
}



//-------------------------------------------------------------------------------------
//  debug methods
//-------------------------------------------------------------------------------------

wxString lmTimeposTable::DumpTimeposTable()
{
    wxString sMsg = _T("Start of dump. Timepos Table\n");
	for (lmItTimeLine it = m_aLines.begin(); it != m_aLines.end(); ++it)
	{
        sMsg += (*it)->DumpMainTable();
    }
    return sMsg;
}

