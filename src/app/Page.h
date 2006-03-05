// RCS-ID: $Id: Page.h,v 1.3 2006/02/23 19:17:12 cecilios Exp $
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
/*! @file Page.h
    @brief Header file for class lmPage
    @ingroup app_gui
*/
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
    void SetTopMargin(lmMicrons nMicrons) { m_nTopMargin = nMicrons; }
    void SetBottomMargin(lmMicrons nMicrons) { m_nBottomMargin = nMicrons; }
    void SetLeftMargin(lmMicrons nMicrons) { m_nLeftMargin = nMicrons; }
    void SetRightMargin(lmMicrons nMicrons) { m_nRightMargin = nMicrons; }
    void SetBindingMargin(lmMicrons nMicrons) { m_nBindingMargin = nMicrons; }
    void SetPageSize(lmMicrons width, lmMicrons height);

    // Access

    lmMicrons TopMargin() { return m_nTopMargin; }
    lmMicrons BottomMargin() { return m_nBottomMargin; }
    lmMicrons LeftMargin() {
        return (m_nPageNum % 2) ? m_nLeftMargin + m_nBindingMargin : m_nLeftMargin ;
    }
    lmMicrons RightMargin() {
        return (m_nPageNum % 2) ? m_nRightMargin : m_nRightMargin + m_nBindingMargin ;
    }
    wxSize& PageSize() { return m_pageSize; }


private:
    // Mesaures: all in logical units
    lmMicrons        m_nLeftMargin;
    lmMicrons        m_nRightMargin;
    lmMicrons        m_nTopMargin;
    lmMicrons        m_nBottomMargin;
    lmMicrons        m_nBindingMargin;
    wxSize            m_pageSize;            // paper size
    lmMicrons        m_nPageNum;            // absolute num of page


};
    
#endif    // _PAGE_H
