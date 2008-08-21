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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
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


lmPageInfo::lmPageInfo(int nLeftMargin, int nRightMargin, int nTopMargin,
                       int nBottomMargin, int nBindingMargin, wxSize nPageSize,
                       bool fPortrait)          
{
    //constructor: all data in milimeters
    //default paper size: DIN A4 (210.0 x 297.0 mm)

    m_uLeftMargin = lmToLogicalUnits(nLeftMargin, lmMILLIMETERS);
    m_uRightMargin = lmToLogicalUnits(nRightMargin, lmMILLIMETERS);
    m_uTopMargin = lmToLogicalUnits(nTopMargin, lmMILLIMETERS);
    m_uBottomMargin = lmToLogicalUnits(nBottomMargin, lmMILLIMETERS);
    m_uBindingMargin = lmToLogicalUnits(nBindingMargin, lmMILLIMETERS);

    m_uPageSize.SetHeight(lmToLogicalUnits(nPageSize.GetHeight(), lmMILLIMETERS));        
    m_uPageSize.SetWidth(lmToLogicalUnits(nPageSize.GetWidth(), lmMILLIMETERS));

    m_fPortrait = fPortrait;
    m_fNewSection = true;
}

void lmPageInfo::SetPageSize(lmLUnits uWidth, lmLUnits uHeight)
{
    m_uPageSize.SetWidth(uWidth);
    m_uPageSize.SetHeight(uHeight);
}



