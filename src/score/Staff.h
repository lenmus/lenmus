// RCS-ID: $Id: Staff.h,v 1.3 2006/02/23 19:24:42 cecilios Exp $
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
    lmStaff(lmScore* pScore, wxInt32 nNumLines=5, lmMicrons nMicrons=1800);
    ~lmStaff();

    // margins
    lmMicrons GetAfterSpace() { return m_afterSpace; }
    lmMicrons GetLeftMargin() { return m_leftMargin; }
    lmMicrons GetRightMargin() { return m_rightMargin; }
    void SetAfterSpace(lmMicrons nLogicalUnits) { m_afterSpace = nLogicalUnits; }
    void SetLeftMargin(lmMicrons nLogicalUnits) { m_leftMargin = nLogicalUnits; }
    void SetRightMargin(lmMicrons nLogicalUnits) { m_rightMargin = nLogicalUnits; }
    void SetMargins(lmMicrons nLeft, lmMicrons nRight, lmMicrons nAfter) {
                m_afterSpace = nAfter;
                m_leftMargin = nLeft;
                m_rightMargin = nRight;
    }

    // sizes
    wxInt32 GetHeight();

    lmMicrons GetLineSpacing() { return m_spacing; }
    lmMicrons GetLineThick() { return m_lineThick; }
    wxInt32 GetNumLines() { return m_numLines; }

    lmMicrons TenthsToLogical(wxInt32 nTenths) { return (m_spacing * nTenths)/10; }
    wxFont* GetFontDraw() { return m_pFontDraw; }
    void SetFontDraw(wxFont* pFont) { m_pFontDraw = pFont; }

    //access to current clef and keys. Only valid during renderization process
    void SetCurrentClef(lmClef* pClef) { m_pCurClef = pClef; }
    void SetCurrentKey(lmKeySignature* pKS) { m_pCurKey = pKS; }
    lmClef* GetCurrentClef() { return m_pCurClef; }
    lmKeySignature* GetCurrentKey() { return m_pCurKey; };

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
    lmMicrons    m_lineThick;        // in logical units
    wxInt32        m_numLines;
    lmMicrons    m_spacing;            // in logical units (thousandths of a mm.,microns)
    wxFont*        m_pFontDraw;        // font to use for drawing on this staff

    lmMicrons    m_leftMargin;        // lmStaff margins (logical units))
    lmMicrons    m_rightMargin;
    lmMicrons    m_afterSpace;

    // These variables are used to track current clef, tonal key and time key while processing
    // the score, either when rendering it, when exporting the score or when building from file
    // reading/importing
    lmClef*            m_pCurClef;            // current clef
    lmKeySignature*    m_pCurKey;            // current key signature

    // List of contexts
    ContextList        m_cContext;

                                    

};

// declare a list of StaffObjs class
#include "wx/list.h"
WX_DECLARE_LIST(lmStaff, StaffList);


#endif    // __STAFFT_H__
