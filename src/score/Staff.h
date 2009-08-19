//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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

#ifndef __LM_STAFF_H__        //to avoid nested includes
#define __LM_STAFF_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Staff.cpp"
#endif

#include <list>
#include "defs.h"

class lmScore;
class lmContext;

#include "StaffObj.h"

class lmStaff : public lmScoreObj
{
public:
    //ctor and dtor
    lmStaff(lmVStaff* pVStaff, long nID=lmNEW_ID, int nNumLines=5, lmLUnits uUnits=0);
    ~lmStaff();

	//---- virtual methods of base class -------------------------

	//units conversion
    inline lmLUnits TenthsToLogical(lmTenths rTenths) { return (m_uSpacing * rTenths)/10.0; }
	inline lmTenths LogicalToTenths(lmLUnits uUnits) { return (uUnits * 10.0) / m_uSpacing; }
    inline lmScore* GetScore() { return m_pParent->GetScore(); }

	//---- specific methods of this class ------------------------

    //reference info
    int GetNumberOfInstrument();
    int GetNumberOfStaff();

	//margins
    inline lmLUnits GetStaffDistance() { return m_uStaffDistance; }
    inline void SetStaffDistance(lmLUnits uSpace) { m_uStaffDistance = uSpace; }

    //sizes
    lmLUnits GetHeight();

    inline lmLUnits GetLineSpacing() { return m_uSpacing; }
    inline lmLUnits GetLineThick() { return m_uLineThickness; }
    inline int GetNumLines() { return m_nNumLines; }

    //font to draw music symbols on this staff
    inline wxFont* GetFontDraw() { return m_pFontDraw; }
    inline void SetFontDraw(wxFont* pFont) { m_pFontDraw = pFont; }
    inline void SetMusicFontSize(double rPointSize) { m_rFontPoints = rPointSize; }
    inline double GetMusicFontSize() { return m_rFontPoints; }

    //context management
    lmContext* NewContextAfter(lmClef* pClef, lmContext* pPrevContext);
    lmContext* NewContextAfter(lmKeySignature* pKey, lmContext* pPrevContext);
    lmContext* NewContextAfter(lmTimeSignature* pNewTime, lmContext* pPrevContext);
    inline lmContext* GetLastContext() { return m_pLastContext; }
    void RemoveContext(lmContext* pContext, lmStaffObj* pSO = (lmStaffObj*)NULL);
    inline lmEClefType GetPreviousFirstClefType() const { return m_nPreviousFirstClef; }

    //debug
    wxString Dump();
//#if defined(__WXDEBUG__)
    wxString DumpContextsChain();
//#endif

    //source code export/import
    wxString SourceLDP(int nIndent, bool fUndoData);
    wxString SourceXML(int nIndent);


private:
	//contexts management
	void InsertContextAfter(lmContext* pNew, lmContext* pPrev, lmContext* pNext,
                            lmStaffObj* pSO);

    //lines
    lmLUnits    m_uLineThickness;
    int         m_nNumLines;
    lmLUnits    m_uSpacing;			//between lines

    //margins
    lmLUnits    m_uStaffDistance;   //vertical distance from the bottom line of the previous
                                    //staff. This value is ignored for the first staff in a system.

    //info about font to use to draw music glyphs 
    wxFont*     m_pFontDraw;        //font to use for drawing on this staff
    double      m_rFontPoints;      //font size


	//Contexts are organized as a double linked list. First and last nodes:
	lmContext*		m_pFirstContext;
	lmContext*		m_pLastContext;
    lmEClefType     m_nPreviousFirstClef;   //to know previous clef when clef is deleted

};


//------------------------------------------------------------------------------------------
// lmRefLine class: a reference to line up lyrics, figured bass, other 
//------------------------------------------------------------------------------------------

class lmRefLine
{
    lmRefLine(lmVStaff* pVStaff);
    ~lmRefLine();

};


#endif    // __LM_STAFF_H__
