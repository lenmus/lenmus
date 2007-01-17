//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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

#ifdef __GNUG__
// #pragma interface
#endif

#ifndef _PAGE_H        //to avoid nested includes
#define _PAGE_H

#include "../score/defs.h"


class lmPage
{
public:
    lmPage();
    ~lmPage() {}
    void SetTopMargin(lmLUnits nMicrons) { m_nTopMargin = nMicrons; }
    void SetBottomMargin(lmLUnits nMicrons) { m_nBottomMargin = nMicrons; }
    void SetLeftMargin(lmLUnits nMicrons) { m_nLeftMargin = nMicrons; }
    void SetRightMargin(lmLUnits nMicrons) { m_nRightMargin = nMicrons; }
    void SetBindingMargin(lmLUnits nMicrons) { m_nBindingMargin = nMicrons; }
    void SetPageSize(lmLUnits width, lmLUnits height);

    // Access

    lmLUnits TopMargin() { return m_nTopMargin; }
    lmLUnits BottomMargin() { return m_nBottomMargin; }
    lmLUnits LeftMargin() {
        return (m_nPageNum % 2) ? m_nLeftMargin + m_nBindingMargin : m_nLeftMargin ;
    }
    lmLUnits RightMargin() {
        return (m_nPageNum % 2) ? m_nRightMargin : m_nRightMargin + m_nBindingMargin ;
    }
    wxSize& PageSize() { return m_pageSize; }
    lmLUnits GetUsableHeight() { return m_pageSize.GetHeight() - m_nTopMargin - m_nBottomMargin; }



private:
    // Mesaures: all in logical units
    lmLUnits        m_nLeftMargin;
    lmLUnits        m_nRightMargin;
    lmLUnits        m_nTopMargin;
    lmLUnits        m_nBottomMargin;
    lmLUnits        m_nBindingMargin;

    wxSize          m_pageSize;            // paper size
    int             m_nPageNum;            // absolute num of page


};
    
#endif    // _PAGE_H
