// RCS-ID: $Id: Page.cpp,v 1.3 2006/02/23 19:17:12 cecilios Exp $
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
/*! @file Page.cpp
    @brief Implementation file for class lmPage
    @ingroup app_gui
*/
/*! @class lmPage
    @ingroup app_gui
    @brief A lmPage object defines the page layout 
*/
#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Page.h"


lmPage::lmPage()
{
    // default values
    // TODO: Load default values from user defined templates
    m_nLeftMargin = 20000;    // 20 mm
    m_nRightMargin = 15000;    // 15 mm
    m_nTopMargin = 20000;        // 20 mm
    m_nBottomMargin = 20000;    // 20 mm
    m_nBindingMargin = 0;    // no binding margin

    m_pageSize.SetHeight(297000);        //DIN A4: 210.0 x 297.0 mm
    m_pageSize.SetWidth(210000);
    m_nPageNum = 1;

}

void lmPage::SetPageSize(lmMicrons width, lmMicrons height)
{
    m_pageSize.SetWidth(width);
    m_pageSize.SetHeight(height);
}


