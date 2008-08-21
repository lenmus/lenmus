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

#ifndef __LM_PAGE_H__        //to avoid nested includes
#define __LM_PAGE_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Page.cpp"
#endif

#include "../score/defs.h"


class lmPageInfo
{
public:
    //constructor: all data in milimeters
    lmPageInfo(int nLeftMargin = 20, int nRightMargin = 15, int nTopMargin = 20,
               int nBottomMargin = 20, int nBindingMargin = 0,
               wxSize nPageSize = wxSize(210, 297), bool fPortrait = true );            
    ~lmPageInfo() {}

    //change settings
    inline void SetTopMargin(lmLUnits uValue) { m_uTopMargin = uValue; }
    inline void SetBottomMargin(lmLUnits uValue) { m_uBottomMargin = uValue; }
    inline void SetLeftMargin(lmLUnits uValue) { m_uLeftMargin = uValue; }
    inline void SetRightMargin(lmLUnits uValue) { m_uRightMargin = uValue; }
    inline void SetBindingMargin(lmLUnits uValue) { m_uBindingMargin = uValue; }
    void SetPageSize(lmLUnits width, lmLUnits height);
    inline void SetOrientation(bool fPortrait) { m_fPortrait = fPortrait; }
    inline void SetNewSection(bool fNewSection) { m_fNewSection = fNewSection; }

    // Access

    lmLUnits TopMargin() { return m_uTopMargin; }
    lmLUnits BottomMargin() { return m_uBottomMargin; }
    lmLUnits LeftMargin(int nNumPage) {
        return (nNumPage % 2) ? m_uLeftMargin + m_uBindingMargin : m_uLeftMargin ;
    }
    lmLUnits RightMargin(int nNumPage) {
        return (nNumPage % 2) ? m_uRightMargin : m_uRightMargin + m_uBindingMargin ;
    }
    inline lmLUnits PageWidth() { return (m_fPortrait ? m_uPageSize.Width() : m_uPageSize.Height()); }
    inline lmLUnits PageHeight() { return (m_fPortrait ? m_uPageSize.Height() : m_uPageSize.Width()); }
    inline lmLUnits GetUsableHeight() { return m_uPageSize.GetHeight() - m_uTopMargin - m_uBottomMargin; }



private:
    //margins, all in logical units
    lmLUnits        m_uLeftMargin;
    lmLUnits        m_uRightMargin;
    lmLUnits        m_uTopMargin;
    lmLUnits        m_uBottomMargin;
    lmLUnits        m_uBindingMargin;

    //paper size, in logical units
    lmUSize         m_uPageSize; 
    bool            m_fPortrait;
    bool            m_fNewSection;
};

#endif    // __LM_PAGE_H__
