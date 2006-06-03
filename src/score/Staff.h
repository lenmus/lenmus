//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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
/*! @file Staff.h
    @brief Header file for class lmStaff
    @ingroup score_kernel
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __STAFFT_H__        //to avoid nested includes
#define __STAFFT_H__

#include "Score.h"

class lmStaff
{
public:
    //ctor and dtor
    lmStaff(lmScore* pScore, wxInt32 nNumLines=5, lmLUnits nMicrons=0);
    ~lmStaff();

    // margins
    lmLUnits GetAfterSpace() { return m_afterSpace; }
    lmLUnits GetLeftMargin() { return m_leftMargin; }
    lmLUnits GetRightMargin() { return m_rightMargin; }
    void SetAfterSpace(lmLUnits nLogicalUnits) { m_afterSpace = nLogicalUnits; }
    void SetLeftMargin(lmLUnits nLogicalUnits) { m_leftMargin = nLogicalUnits; }
    void SetRightMargin(lmLUnits nLogicalUnits) { m_rightMargin = nLogicalUnits; }
    void SetMargins(lmLUnits nLeft, lmLUnits nRight, lmLUnits nAfter) {
                m_afterSpace = nAfter;
                m_leftMargin = nLeft;
                m_rightMargin = nRight;
    }

    // sizes
    wxInt32 GetHeight();

    lmLUnits GetLineSpacing() { return m_spacing; }
    lmLUnits GetLineThick() { return m_lineThick; }
    wxInt32 GetNumLines() { return m_numLines; }

    lmLUnits TenthsToLogical(wxInt32 nTenths) { return (m_spacing * nTenths)/10; }
    wxFont* GetFontDraw() { return m_pFontDraw; }
    void SetFontDraw(wxFont* pFont) { m_pFontDraw = pFont; }

    //access to current clef. Only used by key signatures for renderization
    lmClef* GetCurrentClef() { return GetLastClef(); }

    //context management
    lmContext* NewContext(lmClef* pClef);
    lmContext* NewContext(lmKeySignature* pKey);
    lmContext* NewContext(lmTimeSignature* pNewTime);
    lmContext* NewContext(lmContext* pNewContext);
    lmContext* NewContext(lmContext* pCurrentContext, int nNewAccidentals, int nStep);
    lmClef* GetLastClef();
    lmKeySignature* GetLastKey();
    lmTimeSignature* GetLastTime();
    lmContext* GetLastContext();

private:
    lmLUnits    m_lineThick;        // in logical units
    wxInt32     m_numLines;
    lmLUnits    m_spacing;          // in logical units (thousandths of a mm.,microns)
    wxFont*     m_pFontDraw;        // font to use for drawing on this staff

    lmLUnits    m_leftMargin;       // lmStaff margins (logical units))
    lmLUnits    m_rightMargin;
    lmLUnits    m_afterSpace;

    // List of contexts
    ContextList        m_cContext;

};

// declare a list of StaffObjs class
#include "wx/list.h"
WX_DECLARE_LIST(lmStaff, StaffList);


#endif    // __STAFFT_H__
