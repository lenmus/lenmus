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

#ifndef __LM_BRACEBRACKET_H__        //to avoid nested includes
#define __LM_BRACEBRACKET_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "InstrGroup.cpp"
#endif

#include <list>
#include "defs.h"

class lmShape;
class lmInstrument;
class lmPaper;
class lmScoreText;
class lmBox;


class lmInstrGroup
{
public:
    lmInstrGroup(lmEBracketSymbol nGrpSymbol, bool fJoinBarlines);
    ~lmInstrGroup();

    //   inline void Save(lmUndoData* pUndoData) {}
//   void CreateShape();
 //   lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, wxColour color);
    inline int NumInstruments() { return (int)m_Instruments.size(); }
    inline lmInstrument* GetFirstInstrument() { return m_Instruments.front(); }
    inline lmInstrument* GetLastInstrument() { return m_Instruments.back(); }
    void Include(lmInstrument* pInstr, int nIndex=-1);
    void Remove(lmInstrument* pInstr);
    void OnGroupModified();

    void MeasureNames(lmPaper* pPaper);
    void AddNameAndBracket(lmBox* pBox, lmPaper* pPaper, int nSystem,
                           lmLUnits xStaff, lmLUnits yTop, lmLUnits yBottom);

    lmLUnits GetIndentFirst() { return m_uIndentFirst; }
    lmLUnits GetIndentOther() { return m_uIndentOther; }

private:
    bool RenderBraket();
	void AddNameAbbrevShape(lmBox* pBox, lmPaper* pPaper, lmScoreText* pName,
                            lmLUnits xStaff, lmLUnits yTop, lmLUnits yBottom);


    bool                        m_fJoinBarlines;
	lmShape*	                m_pShape;
    std::list<lmInstrument*>    m_Instruments;      //list of instruments that form the group

    lmLUnits            m_uIndentFirst;     //indentation for first system
    lmLUnits            m_uIndentOther;     //indentation for other systems
    lmScoreText*        m_pName;            //group name
    lmScoreText*        m_pAbbreviation;    //group abbreviated name
    lmEBracketSymbol    m_nBracket;         //bracket symbol
    lmLUnits            m_uBracketWidth;    //to render the bracket
    lmLUnits            m_uBracketGap;      //to render the bracket
};

#endif    // __LM_BRACEBRACKET_H__

