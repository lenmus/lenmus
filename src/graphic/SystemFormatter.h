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

#ifndef __LM_SYSTEMFORMATTER_H__        //to avoid nested includes
#define __LM_SYSTEMFORMATTER_H__

#include "../score/defs.h"

class lmStaffObj;
class lmStaff;
class lmVStaff;
class lmBoxSlice;


//----------------------------------------------------------------------------------------
// lmSystemFormatter: Orchestrates the layout of a system [Abstract]
//----------------------------------------------------------------------------------------

class lmSystemFormatter
{
protected:
    //layout options
    float               m_rSpacingFactor;           //for proportional spacing of notes
    lmESpacingMethod    m_nSpacingMethod;           //fixed, proportional, etc.
    lmTenths            m_rSpacingValue;            //space for 'fixed' method


public:
    lmSystemFormatter(float rSpacingFactor, lmESpacingMethod nSpacingMethod,
                      lmTenths nSpacingValue)
        : m_rSpacingFactor(rSpacingFactor)
        , m_nSpacingMethod(nSpacingMethod)
        , m_rSpacingValue(nSpacingValue)
    {
    }

    virtual ~lmSystemFormatter() {}

        //Collecting measurements

    //caller informs that all data for this system has been suplied
    virtual void EndOfSystemMeasurements()=0;             

    //caller ask to prepare to receive data for a instrument in column iCol [0..n-1]
    virtual void StarBarMeasurements(int iCol, int nInstr, lmLUnits uxStart,
                                     lmVStaff* pVStaff, lmLUnits uSpace)=0;

    //caller sends data about one staffobj in column iCol [0..n-1]
    virtual void IncludeObject(int iCol, int nInstr, lmStaffObj* pSO, lmShape* pShape,
                               bool fProlog, int nStaff=0)=0;

    //caller sends lasts object to store in column iCol [0..n-1]. 
    virtual void IncludeBarlineAndTerminateBarMeasurements(int iCol, lmStaffObj* pSO,
                                                   lmShape* pShape, lmLUnits xStart)=0;

    //caller informs that there are no barline and no more objects in column iCol [0..n-1]. 
    virtual void TerminateBarMeasurementsWithoutBarline(int iCol, lmLUnits xStart)=0;

    //caller request to ignore measurements for column iCol [0..n-1]
    virtual void DiscardMeasurementsForColumn(int iCol)=0;

        // Processing
    virtual void DoColumnSpacing(int iCol, bool fTrace = false)=0;
    virtual lmLUnits RedistributeSpace(int iCol, lmLUnits uNewStart, lmBoxSlice* pBSlice)=0;

        //Operations

    virtual void IncrementColumnSize(int iCol, lmLUnits uIncr)=0;

        //Access to information
    virtual lmLUnits GetStartPositionForColumn(int iCol)=0;
    virtual lmLUnits GetMinimumSize(int iCol)=0;
    virtual bool GetOptimumBreakPoint(int iCol, lmLUnits uAvailable, float* prTime,
                              lmLUnits* puWidth)=0;
    virtual lmBarline* GetColumnBarline(int iCol)=0;

    //methods for debugging
    virtual wxString DumpColumnData(int iCol)=0;

    //other methods
    virtual void ClearDirtyFlags(int iCol)=0;


    //Public methods coded only for Unit Tests
#if defined(__WXDEBUG__)

    virtual int GetNumColumns()=0;
    virtual int GetNumLinesInColumn(int iCol)=0;    //iCol = [0..n-1]
    virtual int GetNumObjectsInColumnLine(int iCol, int iLine)=0;     //iCol, iLine = [0..n-1]

#endif

};

#endif    // __LM_SYSTEMFORMATTER_H__

