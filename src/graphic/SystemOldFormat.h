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

#ifndef __LM_SYSTEMOLDFORMAT_H__        //to avoid nested includes
#define __LM_SYSTEMOLDFORMAT_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "SystemOldFormat.cpp"
#endif

#include <vector>
#include "../score/defs.h"

#include "SystemFormatter.h"

class lmStaffObj;
class lmStaff;
class lmVStaff;
class lmBoxSlice;

class lmSystemOldFormat;
class lmColumnOldFormat;
class lmColuOldStorage;
class lmLineOldFormat;
class lmLinOldTbl;
class lmLinOldEntry;

class lmCtiticalOldLine;
class lmOldBreaksTable;
class lmColuOldSplitter;
class lmLinOldResizer;

//some helper definitions
#define lmLineEntryOldItera		    std::vector<lmLinOldEntry*>::iterator
#define lmLinesOldItera             std::vector<lmLinOldTbl*>::iterator
#define lmLineFormattersOldItera    std::vector<lmLineOldFormat*>::iterator


//----------------------------------------------------------------------------------------
//lmLinOldEntry: an entry in lmLinOldTbl
//----------------------------------------------------------------------------------------

//entry types
enum lmEOldEnType
{
    lm_eOldAlpha = 1,       //start of line
    lm_eOldStaffObj,       //lmStaffObj
    lm_eOldOmega,          //end of line
};

class lmLinOldEntry
{
public:
    // constructor and destructor
    lmLinOldEntry(lmEOldEnType nType, lmStaffObj* pSO, lmShape* pShape, bool fProlog);
    lmLinOldEntry(lmLinOldEntry* pEntry);
    ~lmLinOldEntry() {}

	void AssignSpace(lmColumnOldFormat* pTT, float rFactor);
	void SetNoteRestSpace(lmColumnOldFormat* pTT, float rFactor);
	void Reposition(lmLUnits uxPos);
    inline lmLUnits GetTotalSize() { return m_uSize + m_uFixedSpace + m_uVariableSpace; }

    wxString Dump(int iEntry);
    static wxString DumpHeader();



    //member variables (one entry of the table)
    //----------------------------------------------------------------------------
    lmEOldEnType    m_nType;            //type of entry
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


//----------------------------------------------------------------------------------------
// lmLinOldTbl: An object to encapsulate the table for a line
//----------------------------------------------------------------------------------------

class lmLinOldTbl
{
protected:
	std::vector<lmLinOldEntry*>	m_LineEntries;	    //the entries that form this table
	int							m_nInstr;		    //instrument (0..n-1)
	int							m_nVoice;		    //voice (0=not yet defined)

public:
    lmLinOldTbl(int nInstr, int nVoice);
    ~lmLinOldTbl();

    //access to an item
    inline lmLinOldEntry* Front() { return m_LineEntries.front(); }
    inline lmLinOldEntry* Back() { return m_LineEntries.back(); }
    inline lmLinOldEntry* Item(int i) { return m_LineEntries[i]; }
    inline lmLinOldEntry* GetLastEntry() { return m_LineEntries.back(); }
    inline lmLinOldEntry* GetAlphaEntry() { return m_LineEntries.front(); }

    //iterator to an item
    inline lmLineEntryOldItera Begin() { return m_LineEntries.begin(); }
    inline lmLineEntryOldItera End() { return m_LineEntries.end(); }

    //table manipulation
    inline void Clear() { m_LineEntries.clear(); }
    inline void PushBack(lmLinOldEntry* pEntry) { m_LineEntries.push_back(pEntry); }
	lmLinOldEntry* AddEntry(lmEOldEnType nType, lmStaffObj* pSO, lmShape* pShape,
							 bool fProlog);
    void CreateAlphaEntry(lmLUnits uxStart, lmLUnits uSpace);

    //properties
    inline lmLUnits GetLineStartPosition() { return m_LineEntries.front()->m_xLeft; }
    lmLUnits GetLineWidth();
    lmLUnits GetMaxXFinal();
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


protected:
    lmLinOldEntry*  NewEntry(lmEOldEnType nType, lmStaffObj* pSO, lmShape* pShape,
							  bool fProlog, lmLUnits uSpace = 0.0f);

};


//----------------------------------------------------------------------------------------
//lmColuOldStorage: encapsulates the lines for a column
//----------------------------------------------------------------------------------------

class lmColuOldStorage
{
protected:
	std::vector<lmLinOldTbl*> m_Lines;	    //lines that form this column
	lmStaff* m_pStaff[lmMAX_STAFF];         //staves (nedeed to compute spacing)

public:
    lmColuOldStorage();
    ~lmColuOldStorage();

    //access to an item
    lmLinOldTbl* Front() { return m_Lines.front(); }

    //iterator
    inline lmLinesOldItera Begin() { return m_Lines.begin(); }
    inline lmLinesOldItera End() { return m_Lines.end(); }
    inline lmLinesOldItera GetLastLine() {
	    lmLinesOldItera it = m_Lines.end();
	    return --it;
    }
    lmLinesOldItera FindLineForInstrAndVoice(int nInstr, int nVoice);


    //storage manipulation
    lmLinOldTbl* OpenNewLine(int nInstr, int nVoice, lmLUnits uxStart, lmLUnits uSpace);
    inline void SaveStaffPointer(int iStaff, lmStaff* pStaff) { m_pStaff[iStaff] = pStaff; }

    //properties
    inline size_t Size() { return m_Lines.size(); }
    inline bool IsEndOfTable(lmLinesOldItera it) { return it == m_Lines.end(); }

    //access to column measurements
    lmLUnits GetGrossBarSize();
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
//lmLinOldBuilder: receives information about objects in a column, organizes this
//info into lines and stores them in the received column storage
//----------------------------------------------------------------------------------------

class lmLinOldBuilder
{
protected:
    lmColuOldStorage*    m_pColStorage;              //music lines for this column
    int					m_nStaffVoice[lmMAX_STAFF];	//voice assigned to each staff
	lmLinOldEntry*     m_pCurEntry;				//ptr to last added entry
	lmLinesOldItera		m_itCurLine;				//point to the pos table for current line

public:
    lmLinOldBuilder(lmColuOldStorage* pStorage);
    ~lmLinOldBuilder();

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
//lmColumnOldFormat: column layout algorithm
//----------------------------------------------------------------------------------------

class lmColumnOldFormat
{
protected:
    lmColuOldStorage*                m_pColStorage;  //music lines for this column
    std::vector<lmLineOldFormat*>   m_LineFormatters;   //a layoutter for each line

    lmLUnits            m_uMinColumnSize;           //minimum size for this column

    lmCtiticalOldLine*     m_pCriticalLine;			//the critical line

    //formatter parameters
    float               m_rSpacingFactor;           //for proportional spacing of notes
    lmESpacingMethod    m_nSpacingMethod;           //fixed, proportional, etc.
    lmTenths            m_rSpacingValue;            //spacing for 'fixed' method


public:
    lmColumnOldFormat(lmColuOldStorage* pStorage, float rSpacingFactor,
                      lmESpacingMethod nSpacingMethod, lmTenths nSpacingValue);
    ~lmColumnOldFormat();

    void Initialize();

    //methods to compute results
    void DoSpacing(bool fTrace = false);
    lmLUnits RedistributeSpace(lmLUnits uNewStart, lmBoxSlice* pBSlice);
    inline void IncrementColumnSize(lmLUnits uIncr) { m_uMinColumnSize += uIncr; }

    //break points computation and related
    bool GetOptimumBreakPoint(lmLUnits uAvailable, float* prTime, lmLUnits* puWidth);

    //access to info
    lmBarline* GetBarline();
    inline lmLUnits GetMinimumSize() { return m_uMinColumnSize; }

    //methods for spacing
	lmLUnits TenthsToLogical(lmTenths rTenths, int nStaff); 
    inline lmESpacingMethod SpacingMethod() const { return m_nSpacingMethod; }
    inline lmTenths FixedSpacingValue() const { return m_rSpacingValue; }
	inline float SpacingFactor() const { return m_rSpacingFactor; }

    //Public methods coded only for Unit Tests
#if defined(__WXDEBUG__)

    inline int GetNumLines() { return (int)m_pColStorage->Size(); }

#endif



private:
    lmOldBreaksTable* ComputeBreaksTable();
    lmLUnits ComputeSpacing(float rFactor);

    //column spacing variables and methods
    bool m_fCreateCriticalLine;
    float m_rCurTime;                       //current tiempos we are processing
    float m_rNextTime;                      //next timepos to process
    lmLUnits m_uxNextTimeStartPosition;     //minimum xPos to start placing objects at next timepos
    lmLineOldFormat* m_pLongestNonTimedLineFmt;
    lmLinOldEntry* m_pShortestEntry;
    float m_rShortestObjectDuration;

    void InitializeCriticalLine();
    void AddShorterTimedObjectToCriticalLine(lmLinOldEntry* pShortestEntry);
    void DetermineLineWithLongestNonTimedObjects();
    void SelectTimedObjIfItHasMinimalDuration(lmLinOldEntry* pEntry);
    bool AlignObjectsAtCurrentTimeAndGetNextTime(lmLUnits uxPos, float rFactor);



};


//----------------------------------------------------------------------------------------
// lmSystemOldFormat: orchestrates the layout of a system
//----------------------------------------------------------------------------------------

class lmSystemOldFormat : public lmSystemFormatter
{
protected:
    std::vector<lmColumnOldFormat*> m_ColFormatters;    //formmater for each column
    std::vector<lmColuOldStorage*> m_ColStorage;         //data for each column
    std::vector<lmLinOldBuilder*> m_LinesBuilder;        //lines builder for each column

    ////layout options
    //float               m_rSpacingFactor;           //for proportional spacing of notes
    //lmESpacingMethod    m_nSpacingMethod;           //fixed, proportional, etc.
    //lmTenths            m_rSpacingValue;            //space for 'fixed' method


public:
    lmSystemOldFormat(float rSpacingFactor, lmESpacingMethod nSpacingMethod,
                      lmTenths nSpacingValue);
    ~lmSystemOldFormat();

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

};


//------------------------------------------------------------------------------------
//lmColuOldSplitter:
//  Algorithm to determine optimum break points to split a column 
//------------------------------------------------------------------------------------

class lmColuOldSplitter
{
protected:
    lmLinOldTbl*               m_pLineTable;       //the line to process

public:
    lmColuOldSplitter(lmLinOldTbl* pLineTable);
    ~lmColuOldSplitter();

    void ComputeBreakPoints(lmOldBreaksTable* pBT);

};


//------------------------------------------------------------------------------------
// An object to encapsulate the algoritms to traverse a line by timepos, decomposing
// the line in timed and not timed objects. As it traverses the line it assigns space
// and collects information.
// Collaborates with lmColumnOldFormat to layout a column
//------------------------------------------------------------------------------------

class lmLineOldFormat
{
protected:
    lmLinOldTbl*            m_pTable;           //the positions table to traverse
    lmLineEntryOldItera     m_itCur;            //current entry
    float                   m_rCurTime;         //current time
	lmLUnits                m_uxCurPos;         //current xPos at start of current time
    lmLineEntryOldItera     m_itFirstTimed;     //points to first timed entry (i.e. note/rest)
                                                //and current time. EndOfCollection if no object
    lmLineEntryOldItera     m_itFirstObj;       //points to first entry for current time. It
                                                //will be the first non-timed object, if it 
                                                //exists, (i.e. clef) or will coincide with
                                                //m_itFirstTimed.

public:
    lmLineOldFormat(lmLinOldTbl* pTable);
    ~lmLineOldFormat();

    //layoutting while traversing the table
	lmLUnits IntitializeSpacingAlgorithm(lmLUnits uSpaceAfterProlog);
    float AssignPositionToObjectsAtTime(float rTime, lmLUnits uxPos, float rFactor, 
                                        lmTenths rSpaceAfterClef,
                                        lmTenths rMinSpaceBetweenNoteAndClef,
                                        lmColumnOldFormat* pColFmt,
                                        lmLUnits* pMaxPos);
    lmLinOldEntry* GetMinAlignedObjForTime(float rTime);
    void FromCurrentPosAddNonTimedToLine(lmLinOldTbl* pLineTable);
    void AddLongestNonTimedObjectsToCriticalLine(lmCtiticalOldLine* pCriticalLine,
                                                 float rTime);
    lmLUnits GetSpaceNonTimedForTime(float rTime);
    lmLUnits GetAnchorForTime(float rTime);
    void AssignSpace(lmColumnOldFormat* pColFmt, float rFactor);
    lmLUnits GetMinFeasiblePosForTime(float rTime);
    inline void SetCurrentTime(float rTime) { m_rCurTime = rTime; }

protected:
    void FindFirstTimed(lmLUnits uSpaceAfterProlog);
    void StartTraversing(lmLUnits uSpaceAfterProlog);


};


//----------------------------------------------------------------------------------------
//lmLinOldResizer: encapsulates the methods to recompute shapes positions so that the line
//will have the desired width, and to move the shapes to those positions
//----------------------------------------------------------------------------------------

class lmLinOldResizer
{
private:
    lmLinOldTbl*   m_pLineTable;   //table for the line to resize

public:
    lmLinOldResizer(lmLinOldTbl* pLineTable);
    ~lmLinOldResizer();

    lmLUnits RepositionShapes(lmCtiticalOldLine* pCriticalLine, lmLUnits uNewBarSize,
                              lmLUnits uNewStart, lmLUnits uOldBarSize);
    void InformAttachedObjs();

};


#endif    // __LM_SYSTEMOLDFORMAT_H__

