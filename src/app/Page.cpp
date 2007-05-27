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
#pragma implementation "Page.h"
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
    m_nLeftMargin = lmToLogicalUnits(20, lmMILLIMETERS);    // 20 mm
    m_nRightMargin = lmToLogicalUnits(15, lmMILLIMETERS);   // 15 mm
    m_nTopMargin = lmToLogicalUnits(20, lmMILLIMETERS);     // 20 mm
    m_nBottomMargin = lmToLogicalUnits(20, lmMILLIMETERS);  // 20 mm
    m_nBindingMargin = 0;    // no binding margin

    m_pageSize.SetHeight(lmToLogicalUnits(297, lmMILLIMETERS));        //DIN A4: 210.0 x 297.0 mm
    m_pageSize.SetWidth(lmToLogicalUnits(210, lmMILLIMETERS));
    m_nPageNum = 1;

}

void lmPage::SetPageSize(lmLUnits width, lmLUnits height)
{
    m_pageSize.SetWidth(width);
    m_pageSize.SetHeight(height);
}


