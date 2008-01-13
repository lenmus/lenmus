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

#ifndef __LM_STAFF_H__        //to avoid nested includes
#define __LM_STAFF_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Staff.cpp"
#endif

#include <list>
#include "defs.h"

class lmScore;
class lmContext;

class lmStaff : public lmScoreObj
{
public:
    //ctor and dtor
    lmStaff(lmScore* pScore, int nNumLines=5, lmLUnits nMicrons=0);
    ~lmStaff();

	//---- virtual methods of base class -------------------------

	//units conversion
    inline lmLUnits TenthsToLogical(lmTenths rTenths) { return (m_uSpacing * rTenths)/10.0; }
	inline lmTenths LogicalToTenths(lmLUnits uUnits) { return (uUnits * 10.0) / m_uSpacing; }
	inline lmEScoreObjType GetScoreObjType() { return lmSOT_Staff; }

	//---- specific methods of this class ------------------------

	// margins
    lmLUnits GetAfterSpace() { return m_uAfterSpace; }
    lmLUnits GetLeftMargin() { return m_uLeftMargin; }
    lmLUnits GetRightMargin() { return m_uRightMargin; }
    void SetAfterSpace(lmLUnits nLogicalUnits) { m_uAfterSpace = nLogicalUnits; }
    void SetLeftMargin(lmLUnits nLogicalUnits) { m_uLeftMargin = nLogicalUnits; }
    void SetRightMargin(lmLUnits nLogicalUnits) { m_uRightMargin = nLogicalUnits; }
    void SetMargins(lmLUnits nLeft, lmLUnits nRight, lmLUnits nAfter) {
                m_uAfterSpace = nAfter;
                m_uLeftMargin = nLeft;
                m_uRightMargin = nRight;
    }

    // sizes
    lmLUnits GetHeight();

    lmLUnits GetLineSpacing() { return m_uSpacing; }
    lmLUnits GetLineThick() { return m_uLineThickness; }
    int GetNumLines() { return m_nNumLines; }

    wxFont* GetFontDraw() { return m_pFontDraw; }
    void SetFontDraw(wxFont* pFont) { m_pFontDraw = pFont; }

    //context management
    lmContext* NewContext(lmContext* pCurrentContext, int nNewAccidentals, int nStep);

    wxString Dump();

private:
    lmLUnits    m_uLineThickness;	// in logical units
    int         m_nNumLines;
    lmLUnits    m_uSpacing;			// in logical units (thousandths of a mm.,microns)
    wxFont*     m_pFontDraw;        // font to use for drawing on this staff

    lmLUnits    m_uLeftMargin;       // lmStaff margins (logical units))
    lmLUnits    m_uRightMargin;
    lmLUnits    m_uAfterSpace;


	//cursor (for edition). Points to current insertion position for this staff



};

// declare a list of StaffObjs class
#include "wx/list.h"
WX_DECLARE_LIST(lmStaff, StaffList);


#endif    // __LM_STAFF_H__
