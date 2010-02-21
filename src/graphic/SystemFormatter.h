//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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

#ifndef __LM_SYSTEMNEWFORMAT_H__        //to avoid nested includes
#define __LM_SYSTEMNEWFORMAT_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "SystemFormatter.cpp"
#endif

#include <vector>
#include "../score/defs.h"
#include "SystemFormatter.h"

class lmStaffObj;
class lmStaff;
class lmVStaff;
class lmBoxSlice;

class lmSystemFormatter;
class lmColumnFormatter;
class lmColumnStorage;
class lmLineTable;
class lmLineEntry;

class lmCriticalLine;
class lmBreaksTable;
class lmBreakPoints;
class lmColumnSplitter;
class lmLineResizer;
class lmLineSpacer;
class lmTimeGridLineExplorer;

//some helper definitions
#define lmLineEntryIterator		    std::vector<lmLineEntry*>::iterator
#define lmLinesIterator             std::vector<lmLineTable*>::iterator
#define lmLineSpacersIterator       std::vector<lmLineSpacer*>::iterator



//----------------------------------------------------------------------------------------
//lmBreaksTable definition: table to contain possible break points
//----------------------------------------------------------------------------------------

//an entry of the BreaksTable
typedef struct lmBeaksTimeEntry_Struct
{
    float       rTimepos;
    float       rPriority;
    lmLUnits    uxStart;
    lmLUnits    uxEnd;
    bool        fInBeam;
    lmLUnits    uxBeam;
}
lmBeaksTimeEntry;

//the breaks table
class lmBreaksTable
{
public:
    lmBreaksTable();
    ~lmBreaksTable();

    void AddEntry(float rTime, lmLUnits uxStart, lmLUnits uWidth, bool fInBeam,
                  lmLUnits uxBeam, float rPriority = 0.8f);
    void AddEntry(lmBeaksTimeEntry* pBTE);
    void SetPriority();
    void ChangePriority(int iEntry, float rMultiplier);
    wxString Dump();
    inline bool IsEmpty() { return m_BreaksTable.empty(); }

    //traversing the table
    lmBeaksTimeEntry* GetFirst();
    lmBeaksTimeEntry* GetNext();


private:

    std::list<lmBeaksTimeEntry*>               m_BreaksTable;      //the table
    std::list<lmBeaksTimeEntry*>::iterator     m_it;               //for GetFirst(), GetNext();


};



//----------------------------------------------------------------------------------------
//lmLineEntry: an entry in lmLineTable
//----------------------------------------------------------------------------------------

class lmLineEntry
{
public:
    // constructor and destructor
    lmLineEntry(lmStaffObj* pSO, lmShape* pShape, bool fProlog);
    ~lmLineEntry() {}

    void RepositionAt(lmLUnits uxNewXLeft);
	void AssignFixedAndVariableSpace(lmColumnFormatter* pTT, float rFactor);
    void MoveShape();

    //setters and getters
    inline lmLUnits GetTotalSize() { return m_uSize + m_uFixedSpace + m_uVariableSpace; }
    inline lmLUnits GetVariableSpace() { return m_uVariableSpace; }
    inline void SetVariableSpace(lmLUnits uSpace) { m_uVariableSpace = uSpace; }
    inline float GetDuration() { return m_pSO->GetTimePosIncrement(); }
    inline float GetTimepos() { return m_rTimePos; }
    inline lmLUnits GetPosition() { return m_xLeft; }
    inline void SetPosition(lmLUnits uPos) { m_xLeft = uPos; }
    inline lmLUnits GetShapeSize() { return m_uSize; }
    inline void MarkAsBarlineEntry() { m_fBarline = true; }
    inline lmLUnits GetAnchor() { return m_uxAnchor; }
    lmLUnits GetShiftToNoteRestCenter();

    //other
    inline bool IsBarlineEntry() { return m_fBarline; }

    //debug
    wxString Dump(int iEntry);
    static wxString DumpHeader();

protected:
    void SetNoteRestSpace(lmColumnFormatter* pTT, float rFactor);
    void AssignMinimumFixedSpace(lmColumnFormatter* pColFmt);
    void AssignVariableSpace(lmLUnits uIdeal);
    lmLUnits ComputeIdealDistance(lmColumnFormatter* pColFmt, float rFactor);
    lmLUnits ComputeIdealDistanceFixed(lmColumnFormatter* pColFmt);
    lmLUnits ComputeIdealDistanceProportional(lmColumnFormatter* pColFmt, float rFactor);


public:
//protected:
    //member variables (one entry of the table)
    //----------------------------------------------------------------------------
    bool            m_fBarline;         //last entry: barline or nothing
    lmStaffObj*     m_pSO;              //ptr to the StaffObj
    lmShape*        m_pShape;           //ptr to the shape
	bool			m_fProlog;          //this shape is a prolog object (clef, KS, TS at start of system)
    float           m_rTimePos;         //timepos for this pSO or -1 if not anchored in time
    lmLUnits        m_xLeft;            //current position of the left border of the object
    lmLUnits        m_uxAnchor;         //position of the anchor line
    lmLUnits        m_xFinal;           //next position (right border position + trailing space)
    //to redistribute objects we need to know:
    lmLUnits        m_uSize;            //size of the shape (notehead, etc.)
    lmLUnits        m_uFixedSpace;      //fixed space added after shape
    lmLUnits        m_uVariableSpace;   //any variable added space we can adjust

};


//----------------------------------------------------------------------------------------
// lmLineTable: an object to encapsulate positioning data for a line
//----------------------------------------------------------------------------------------

class lmLineTable
{
protected:
	std::vector<lmLineEntry*>	m_LineEntries;	    //the entries that form this table
	int			m_nInstr;		    //instrument (0..n-1)
	int			m_nVoice;		    //voice (0=not yet defined)
    lmLUnits    m_uxLineStart;      //initial position      
    lmLUnits    m_uInitialSpace;    //space at beginning

public:
    lmLineTable(int nInstr, int nVoice, lmLUnits uxStart, lmLUnits uSpace);
    ~lmLineTable();

    //access to an item
    inline lmLineEntry* Front() { return m_LineEntries.front(); }
    inline lmLineEntry* Back() { return m_LineEntries.back(); }
    inline lmLineEntry* Item(int i) { return m_LineEntries[i]; }
    inline lmLineEntry* GetLastEntry() { return m_LineEntries.back(); }

    //iterator to an item
    inline lmLineEntryIterator Begin() { return m_LineEntries.begin(); }
    inline lmLineEntryIterator End() { return m_LineEntries.end(); }

    //table manipulation
    inline void Clear() { m_LineEntries.clear(); }
    inline void PushBack(lmLineEntry* pEntry) { m_LineEntries.push_back(pEntry); }
	lmLineEntry* AddEntry(lmStaffObj* pSO, lmShape* pShape, bool fProlog);
	lmLineEntry* AddFinalEntry(lmStaffObj* pSO, lmShape* pShape);


    //properties
    inline lmLUnits GetLineStartPosition() { return m_uxLineStart; }
    inline lmLUnits GetSpaceAtBeginning() { return m_uInitialSpace; }
    lmLUnits GetLineWidth();
    inline size_t Size() { return m_LineEntries.size(); }
    inline bool IsLineForInstrument(int nInstr) { return m_nInstr == nInstr; }
    inline bool IsLineForVoice(int nVoice) { return m_nVoice == 0 || m_nVoice == nVoice; }
    inline bool IsVoiceNotYetDefined() { return m_nVoice == 0; }
    inline void SetVoice(int nVoice) { m_nVoice = nVoice; }
    inline int GetInstrument() { return m_nInstr; }
    inline int GetVoice() { return m_nVoice; }

    //other
    void ClearDirtyFlags();

    //Debug and Unit Tests
#if defined(__WXDEBUG__)

    //Unit Tests
    inline int GetNumObjectsInLine() { return (int)m_LineEntries.size(); }

#endif
    //debug
    wxString DumpMainTable();

};


class lmDirtyFlagsCleaner
{
protected:
    lmColumnStorage*    m_pColStorage;

public: 
    lmDirtyFlagsCleaner(lmColumnStorage* pColStorage);
    
    void ClearDirtyFlags();
};


//----------------------------------------------------------------------------------------
//lmColumnStorage: encapsulates the lines for a column
//----------------------------------------------------------------------------------------

class lmColumnStorage
{
protected:
	std::vector<lmLineTable*> m_Lines;	    //lines that form this column
	lmStaff* m_pStaff[lmMAX_STAFF];         //staves (nedeed to compute spacing)

public:
    lmColumnStorage();
    ~lmColumnStorage();

    void Initialize();

    //access to an item
    lmLineTable* Front() { return m_Lines.front(); }

    //iterator
    inline lmLinesIterator Begin() { return m_Lines.begin(); }
    inline lmLinesIterator End() { return m_Lines.end(); }
    inline lmLinesIterator GetLastLine() {
	    lmLinesIterator it = m_Lines.end();
	    return --it;
    }
    lmLinesIterator FindLineForInstrAndVoice(int nInstr, int nVoice);


    //storage manipulation
    lmLineTable* OpenNewLine(int nInstr, int nVoice, lmLUnits uxStart, lmLUnits uSpace);
    inline void SaveStaffPointer(int iStaff, lmStaff* pStaff) { m_pStaff[iStaff] = pStaff; }

    //properties
    inline size_t Size() { return m_Lines.size(); }
    inline bool IsEndOfTable(lmLinesIterator it) { return it == m_Lines.end(); }

    //access to column measurements
    lmLUnits GetColumnWitdh();
    lmLUnits GetStartOfBarPosition();

    //units conversion
    lmLUnits TenthsToLogical(lmTenths rTenths, int nStaff);

    //debug
    wxString DumpColumnStorage();

    //Public methods coded only for Unit Tests
#if defined(__WXDEBUG__)

    inline int GetNumObjectsInLine(int iLine) { return (int)m_Lines[iLine]->Size(); }

#endif


    //other methods
    void ClearDirtyFlags();

protected:
    void DeleteLines();

};


//----------------------------------------------------------------------------------------
//lmLinesBuilder: receives information about objects in a column, organizes this
//info into lines and stores them in the received column storage
//----------------------------------------------------------------------------------------

class lmLinesBuilder
{
protected:
    lmColumnStorage*    m_pColStorage;              //music lines for this column
    int					m_nStaffVoice[lmMAX_STAFF];	//voice assigned to each staff
	lmLineEntry*        m_pCurEntry;				//ptr to last added entry
	lmLinesIterator		m_itCurLine;				//point to the pos table for current line

public:
    lmLinesBuilder(lmColumnStorage* pStorage);
    ~lmLinesBuilder();

    void Initialize();

    //methods to build the lines
    void StarMeasurementsForInstrument(int nInstr, lmLUnits uxStart, lmVStaff* pVStaff, lmLUnits uSpace);
    void CloseLine(lmStaffObj* pSO, lmShape* pShape, lmLUnits xStart);
    void IncludeObject(int nInstr, lmStaffObj* pSO, lmShape* pShape, bool fProlog, int nStaff=0);
    void EndOfData();        //inform that all data has been suplied

private:
    void ResetDefaultStaffVoices();
    int DecideVoiceToUse(lmStaffObj* pSO, int nStaff);
    void StartLine(int nInstr, int nVoice=0, lmLUnits uxStart = -1.0f, lmLUnits uSpace = 0.0f);
    void CreateLinesForEachStaff(int nInstr, lmLUnits uxStart, lmVStaff* pVStaff,
                                 lmLUnits uSpace);
};


//----------------------------------------------------------------------------------------
//lmColumnFormatter: column layout algorithm
//  - explores all lines, by time pos, aligning objects
//----------------------------------------------------------------------------------------

class lmColumnFormatter
{
protected:
    lmColumnStorage*            m_pColStorage;  //music lines for this column
    std::vector<lmLineSpacer*>  m_LineSpacers;  //one spacer for each line

    lmLUnits            m_uMinColumnSize;           //minimum size for this column

    //formatter parameters
    float               m_rSpacingFactor;           //for proportional spacing of notes
    lmESpacingMethod    m_nSpacingMethod;           //fixed, proportional, etc.
    lmTenths            m_rSpacingValue;            //spacing for 'fixed' method

public:
    lmColumnFormatter(lmColumnStorage* pStorage, float rSpacingFactor,
                      lmESpacingMethod nSpacingMethod, lmTenths nSpacingValue);
    ~lmColumnFormatter();

    inline void Initialize() {}

    //methods to compute results
    void DoSpacing(bool fTrace = false);
    inline void IncrementColumnSize(lmLUnits uIncr) { m_uMinColumnSize += uIncr; }

    //access to info
    lmBarline* GetBarline();
    inline lmLUnits GetMinimumSize() { return m_uMinColumnSize; }

    //methods for spacing
	lmLUnits TenthsToLogical(lmTenths rTenths, int nStaff); 
    inline bool IsProportionalSpacing() { return m_nSpacingMethod == esm_PropConstantFixed; }
    inline bool IsFixedSpacing() { return m_nSpacingMethod == esm_Fixed; }
    inline lmTenths GetFixedSpacingValue() const { return m_rSpacingValue; }

    //Public methods coded only for Unit Tests
#if defined(__WXDEBUG__)

    inline int GetNumLines() { return (int)m_pColStorage->Size(); }

#endif



private:
    lmLUnits ComputeSpacing();
    void DeleteLineSpacers();
    inline bool ThereAreObjects() { return m_fThereAreObjects; }

    //variables and methods for column traversal ---------------------------------
    bool        m_fThereAreObjects;
    float       m_rCurrentTime;         //current tiempos being aligned
    lmLUnits    m_rCurrentPos;          //xPos to start placing objects

    void CreateLineSpacers();
    void ProcessNonTimedAtProlog();
    void ProcessTimedAtCurrentTimepos();
    void ProcessNonTimedAtCurrentTimepos();
    //----------------------------------------------------------------------------

};


//----------------------------------------------------------------------------------------
//lmBreakPoints: 
//  encloses the algorithm to determine optimum break points to split a column
//----------------------------------------------------------------------------------------

class lmBreakPoints
{
protected:
    lmColumnStorage*    m_pColStorage;      //the column to split
    lmBreaksTable*      m_pPossibleBreaks;
    lmBeaksTimeEntry*   m_pOptimumEntry;

public:
    lmBreakPoints(lmColumnStorage* pColStorage);
    ~lmBreakPoints();

    bool FindOptimunBreakPointForSpace(lmLUnits uAvailable);
    float GetOptimumTimeForFoundBreakPoint();
    lmLUnits GetOptimumPosForFoundBreakPoint();

protected:
    void ComputeBreaksTable();
    void DeleteBreaksTable();

};



//----------------------------------------------------------------------------------------
// lmSystemFormatter: orchestrates the layout of a system
//----------------------------------------------------------------------------------------

class lmSystemFormatter
{
protected:
    std::vector<lmColumnFormatter*> m_ColFormatters;    //formmater for each column
    std::vector<lmColumnStorage*> m_ColStorage;         //data for each column
    std::vector<lmLinesBuilder*> m_LinesBuilder;        //lines builder for each column

    //layout options
    float               m_rSpacingFactor;           //for proportional spacing of notes
    lmESpacingMethod    m_nSpacingMethod;           //fixed, proportional, etc.
    lmTenths            m_rSpacingValue;            //space for 'fixed' method


public:
    lmSystemFormatter(float rSpacingFactor, lmESpacingMethod nSpacingMethod,
                      lmTenths rSpacingValue);
    ~lmSystemFormatter();

        //Collecting measurements

    //caller informs that all data for this system has been suplied
    void EndOfSystemMeasurements();             

    //caller ask to prepare to receive data for a instrument in column iCol [0..n-1]
    void StarBarMeasurements(int iCol, int nInstr, lmLUnits uxStart, lmVStaff* pVStaff,
                              lmLUnits uSpace);

    //caller sends data about one staffobj in column iCol [0..n-1]
    void IncludeObject(int iCol, int nInstr, lmStaffObj* pSO, lmShape* pShape,
                       bool fProlog, int nStaff=0);

    //caller sends lasts object to store in column iCol [0..n-1]. 
    void IncludeBarlineAndTerminateBarMeasurements(int iCol, lmStaffObj* pSO, lmShape* pShape, lmLUnits xStart);

    //caller informs that there are no barline and no more objects in column iCol [0..n-1]. 
    void TerminateBarMeasurementsWithoutBarline(int iCol, lmLUnits xStart);

    //caller request to ignore measurements for column iCol [0..n-1]
    void DiscardMeasurementsForColumn(int iCol);

        // Processing
    void DoColumnSpacing(int iCol, bool fTrace = false);
    lmLUnits RedistributeSpace(int iCol, lmLUnits uNewStart, lmBoxSlice* pBSlice);

        //Operations

    void IncrementColumnSize(int iCol, lmLUnits uIncr);

        //Access to information
    lmLUnits GetStartPositionForColumn(int iCol);

    lmLUnits GetMinimumSize(int iCol);
    bool GetOptimumBreakPoint(int iCol, lmLUnits uAvailable, float* prTime,
                              lmLUnits* puWidth);
    lmBarline* GetColumnBarline(int iCol);

    //methods for debugging
    wxString DumpColumnData(int iCol);

    //other methods
    void ClearDirtyFlags(int iCol);


    //Public methods coded only for Unit Tests
#if defined(__WXDEBUG__)

    inline int GetNumColumns() { return (int)m_ColFormatters.size(); }
    inline int GetNumLinesInColumn(int iCol) { return m_ColFormatters[iCol]->GetNumLines(); }
    int GetNumObjectsInColumnLine(int iCol, int iLine);     //iCol, iLine = [0..n-1]

#endif

            // Specific methods of this class

    //coded only for Unit Tests
#if defined(__WXDEBUG__)

    inline lmColumnStorage* GetColumnData(int iCol) { return m_ColStorage[iCol]; }

#endif
};


//------------------------------------------------------------------------------------
//lmColumnSplitter:
//  Algorithm to determine optimum break points to split a column 
//------------------------------------------------------------------------------------

class lmColumnSplitter
{
protected:
    lmLineTable*               m_pLineTable;       //the line to process

public:
    lmColumnSplitter(lmLineTable* pLineTable);
    ~lmColumnSplitter();

    void ComputeBreakPoints(lmBreaksTable* pBT);

};


//----------------------------------------------------------------------------------------
//lmLineResizer: encapsulates the methods to recompute shapes positions so that the line
//will have the desired width, and to move the shapes to those positions
//----------------------------------------------------------------------------------------

class lmLineResizer
{
protected:
    lmLineTable*    m_pTable;           //table for the line to resize
    lmLUnits        m_uOldBarSize;
    lmLUnits        m_uNewBarSize;
    lmLUnits        m_uNewStart;
    lmLineEntryIterator     m_itCurrent;

public:
    lmLineResizer(lmLineTable* pTable, lmLUnits uOldBarSize, lmLUnits uNewBarSize);
    ~lmLineResizer();

    void RepositionShapes(lmLUnits uNewStart);
    void InformAttachedObjs();

protected:
    void MovePrologShapes();
    void ReassignPositionToAllOtherObjects();

};


//----------------------------------------------------------------------------------------
//lmLineSpacer:
//  encapsulates the algorithm to assign spaces and positions to a single line
//----------------------------------------------------------------------------------------

class lmLineSpacer
{
private:
    lmLineTable*            m_pTable;           //the line to assign space
    float                   m_rFactor;          //spacing factor
    lmColumnFormatter*      m_pColFmt;          //for tenths/logical conversion
    lmLineEntryIterator     m_itCur;            //current entry
    float                   m_rCurTime;         //current time
	lmLUnits                m_uxCurPos;         //current xPos at start of current time
    lmLUnits                m_uxRemovable;      //space that can be removed if required
    lmLineEntryIterator     m_itNonTimedAtCurPos;
    lmLUnits                m_uxNotTimedFinalPos;

public:
    lmLineSpacer(lmLineTable* pLineTable, lmColumnFormatter* pColFmt, float rFactor);
    ~lmLineSpacer();

    void InitializeForTraversing();
    lmLUnits ProcessNonTimedAtProlog(lmLUnits uSpaceAfterProlog);
    lmLUnits ProcessNonTimedAtCurrentTimepos(lmLUnits uxPos);
    lmLUnits ProcessTimedAtCurrentTimepos(lmLUnits uxPos);
	inline bool CurrentTimeIs(float rTime) { return m_rCurTime == rTime; }
    inline bool ThereAreTimedObjs() {
        return (m_itCur != m_pTable->End() && IsEqualTime((*m_itCur)->GetTimepos(), m_rCurTime));
    }

    inline bool ThereAreMoreObjects() { return (m_itCur != m_pTable->End()); }
    float GetNextAvailableTime();

protected:
    lmLUnits ComputeShiftToAvoidOverlapWithPrevious();
    void DragAnyPreviousCleftToPlaceItNearThisNote();

    inline bool IsNonTimedObject(lmLineEntryIterator it) {
        return (it != m_pTable->End() && (*it)->GetTimepos() < 0.0f);
    }
    inline bool IsTimedObject(lmLineEntryIterator it) {
        return (it != m_pTable->End() && (*it)->GetTimepos() >= 0.0f);
    }
    inline bool CurrentObjectIsNonTimed() { return IsNonTimedObject(m_itCur); }


    //variables and methods used only to position non-timed objects ------
    // create helper object?
    lmLUnits m_uxMaxOcuppiedSpace;
    lmLUnits m_uxMinOcuppiedSpace;

    void ComputeMaxAndMinOcuppiedSpace();
    void PositionNonTimed();
    void ShiftNonTimed(lmLUnits uxShift);

    void PositionUsingMaxSpaceWithShift(lmLUnits uShift);
    void PositionUsingMinSpaceWithShift(lmLUnits uShift);
    //--------------------------------------------------------------------

};


//----------------------------------------------------------------------------------------
//lmLayoutOptions:
//  encapsulates all constants and user defined values used for layoutting a score
//----------------------------------------------------------------------------------------

//class lmLayoutOptions
//{
//public:
//
//
//protected:
//
//    //spacing function parameters
//    float lmDMIN = 8.0f;				//Dmin: min. duration to consider
//    lmTenths lmMIN_SPACE = 10.0f;		//Smin: space for Dmin

    //space
//    lmTenths lmSPACE_AFTER_PROLOG = 25.0f;
//    lmTenths lmSPACE_AFTER_INTERMEDIATE_CLEF = 20.0f;
//    lmTenths lmMIN_SPACE_BETWEEN_NOTE_AND_CLEF = 10.0f;
//
//};



//----------------------------------------------------------------------------------------
//lmTimeGridTable:
//  A table with occupied times and durations, and connecting time with position
//----------------------------------------------------------------------------------------

//an item in the positions and times table
typedef struct
{
    float           rTimepos;
    float           rDuration;
    lmLUnits        uxPos;
}
lmPosTimeItem;

//the table
class lmTimeGridTable
{
protected:
    lmColumnStorage*            m_pColStorage;


    std::vector<lmPosTimeItem> m_PosTimes;         //the table

public:
    lmTimeGridTable(lmColumnStorage* pColStorage);
    ~lmTimeGridTable();

    inline int GetSize() { return (int)m_PosTimes.size(); }

    //access to an entry values
    inline float GetTimepos(int iItem) { return m_PosTimes[iItem].rTimepos; }
    inline float GetDuration(int iItem) { return m_PosTimes[iItem].rDuration; }
    inline lmLUnits GetXPos(int iItem) { return m_PosTimes[iItem].uxPos; }

    //access by position
    float GetTimeForPosititon(lmLUnits uxPos);

    //debug
    wxString Dump();

protected:
    //variables and methods for column traversal
    std::vector<lmTimeGridLineExplorer*> m_LineExplorers;
    float       m_rCurrentTime;
    float       m_rMinDuration;
    lmLUnits    m_uCurPos;
    bool        m_fTimedObjectsFound;

    inline bool TimedObjectsFound() { return m_fTimedObjectsFound; }
    bool ThereAreObjects();
    void CreateLineExplorers();
    void DeleteLineExplorers();
    void SkipNonTimedAtCurrentTimepos();
    void FindShortestNoteRestAtCurrentTimepos();
    void CreateTableEntry();
    void GetCurrentTime();
    void InterpolateMissingTimes();

};

// helper class to interpolate missing entries
//--------------------------------------------
class lmTimeInserter
{
protected:
    std::vector<lmPosTimeItem>&     m_PosTimes;

    std::vector<lmPosTimeItem>::iterator  m_itInsertionPoint;
    float       m_rTimeBeforeInsertionPoint;
    lmLUnits    m_uPositionBeforeInsertionPoint;

public:
    lmTimeInserter(std::vector<lmPosTimeItem>& oPosTimes);
    void InterpolateMissingTimes();

protected:
    bool IsTimeInTable(float rTimepos);
    void FindInsertionPoint(float rTimepos);
    void InsertTimeInterpolatingPosition(float rTimepos);

};


// helper class to encapsulate the line traversal algorithm
// for creating the time-pos table
//----------------------------------------------------------
class lmTimeGridLineExplorer
{
private:
    lmLineTable*            m_pTable;           //the line to assign space
    lmLineEntryIterator     m_itCur;            //current entry
    float                   m_rCurTime;
	lmLUnits                m_uCurPos;
    lmLUnits                m_uShiftToNoteRestCenter;
    float                   m_rMinDuration;

public:
    lmTimeGridLineExplorer(lmLineTable* pLineTable);
    ~lmTimeGridLineExplorer();

    bool SkipNonTimedAtCurrentTimepos();
    bool FindShortestNoteRestAtCurrentTimepos();
    inline bool ThereAreObjects() { return (m_itCur != m_pTable->End()); }
    float GetCurrentTime();
    float GetDurationForFoundEntry();
    lmLUnits GetPositionForFoundEntry();

protected:
    inline bool CurrentObjectIsNonTimed() {
        return (m_itCur != m_pTable->End() && (*m_itCur)->GetTimepos() < 0.0f);
    }
    inline bool CurrentObjectIsTimed() {
        return (m_itCur != m_pTable->End() && (*m_itCur)->GetTimepos() >= 0.0f);
    }
};

#endif    // __LM_SYSTEMNEWFORMAT_H__

