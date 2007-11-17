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
/*! @file SOControl.cpp
    @brief Implementation file for class lmSOControl
    @ingroup score_kernel
*/
/*! @class lmSOControl
    @ingroup score_kernel
    @brief This object represents a control operation (i.e. a timepos shift)

    This object represents a control operation, currently:
      - a timepos shift.
      - a 'new system' tag
*/

#ifdef __GNUG__
#pragma implementation "SOControl.h"
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

lmSOControl::lmSOControl(ESOCtrolType nType, lmVStaff* pVStaff, float rTimeShift)
    : lmStaffObj(pVStaff, eSFOT_Control, pVStaff, 1, lmNO_VISIBLE, lmNO_DRAGGABLE)
{
    wxASSERT(nType == lmTIME_SHIFT);
    m_nCtrolType = lmTIME_SHIFT;
    m_rTimeShift = rTimeShift;
}

lmSOControl::lmSOControl(ESOCtrolType nType, lmVStaff* pVStaff)
    : lmStaffObj(pVStaff, eSFOT_Control, pVStaff, 1, lmNO_VISIBLE, lmNO_DRAGGABLE)
{
    wxASSERT(nType == lmNEW_SYSTEM);
    m_nCtrolType = lmNEW_SYSTEM;
    m_rTimeShift = 0.0;
}

wxString lmSOControl::Dump()
{
    wxString sDump;
    if (m_nCtrolType == lmTIME_SHIFT) {
        wxString sType = _T("");
        if (m_rTimeShift < 0) {
            sType = _T("<backup>");
        }
        else {
            sType = _T("<forward>");
        }
        sDump = wxString::Format(
            _T("%d\tControl %s\tTimeShift=%.2f\n"),
            m_nId, sType.c_str(), m_rTimeShift);
    }
    else if (m_nCtrolType == lmNEW_SYSTEM) {
        sDump = wxString::Format(
            _T("%d\tControl <newSystem>\n"), m_nId);
    }
    else {
        sDump = wxString::Format(
            _T("%d\tControl <*** UNKNOWN ***>\n"), m_nId);
    }

    return sDump;
}

wxString lmSOControl::SourceXML()
{
    //! @todo generate MusicXML source
    return _T("TODO: lmSOControl source XML generation method");
}

wxString lmSOControl::SourceLDP(int nIndent)
{
    wxString sSource = _T("");
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));

    if (m_nCtrolType == lmTIME_SHIFT)
    {
        if (m_rTimeShift < 0) {
            sSource += _T("(goBack ");
        }
        else {
            sSource += _T("(goFwd ");
        }
        sSource += wxString::Format(_T("%d)\n"), (int)fabs(m_rTimeShift));
    }

    return sSource;
}
