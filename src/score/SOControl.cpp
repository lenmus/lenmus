// RCS-ID: $Id: SOControl.cpp,v 1.3 2006/02/23 19:24:42 cecilios Exp $
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
/*! @file SOControl.cpp
    @brief Implementation file for class lmSOControl
    @ingroup score_kernel
*/
/*! @class lmSOControl
    @ingroup score_kernel
    @brief This object represents a control operation (i.e. a timepos shift)

    This object represents a control operation, for now only a timepos shift.
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

#include "wx/image.h"
#include "Score.h"

//-------------------------------------------------------------------------------------------------
// lmSOControl object implementation
//-------------------------------------------------------------------------------------------------

//
//constructors and destructor
//

lmSOControl::lmSOControl(lmVStaff* pVStaff, float rTimeShift)
    : lmSimpleObj(eTPO_Control, pVStaff, 1, sbNO_VISIBLE, sbNO_DRAGGABLE)
{
    m_rTimeShift = rTimeShift;
}


wxString lmSOControl::Dump()
{
    wxString sType = _T("");
    if (m_rTimeShift < 0) {
        sType = _T("<backup>");
    }
    else {
        sType = _T("<forward>");
    }
    
    wxString sDump = wxString::Format(
        _T("%d\tControl %s\tTimeShift=%.2f\n"),
        m_nId, sType, m_rTimeShift);
    return sDump;
}

wxString lmSOControl::SourceXML()
{
    //! @todo generate MusicXML source
    return _T("TODO: lmSOControl source XML generation method");
}

wxString lmSOControl::SourceLDP()
{
    //! @todo generate SourceLDP source
    return _T("TODO: lmSOControl source LDP generation method");
}
