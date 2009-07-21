//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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

// class lmSOControl
//    This object represents a control operation, currently:
//      - a 'new system' tag

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
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
#include "VStaff.h"

//-------------------------------------------------------------------------------------------------
// lmSOControl object implementation
//-------------------------------------------------------------------------------------------------

//
//constructors and destructor
//

lmSOControl::lmSOControl(ESOCtrolType nType, lmVStaff* pVStaff, long nID)
    : lmStaffObj(pVStaff, nID, eSFOT_Control, pVStaff, 1, lmVISIBLE, lmNO_DRAGGABLE)
    , m_nCtrolType(nType)
{
    wxASSERT(nType == lmNEW_SYSTEM);
}

wxString lmSOControl::Dump()
{
    wxString sDump;
	if (m_nCtrolType == lmNEW_SYSTEM) {
        sDump = wxString::Format(
            _T("%d\tControl <newSystem>\n"), m_nId);
    }
    else {
        sDump = wxString::Format(
            _T("%d\tControl <*** UNKNOWN ***>\n"), m_nId);
    }

    return sDump;
}

wxString lmSOControl::SourceXML(int nIndent)
{
    //TODO generate MusicXML source
    return _T("TODO: lmSOControl source XML generation method\n");
}

wxString lmSOControl::SourceLDP(int nIndent, bool fUndoData)
{
    wxString sSource = _T("");
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));

    switch(m_nCtrolType)
	{
		case lmNEW_SYSTEM:
            if (fUndoData)
                sSource += wxString::Format(_T("(newSystem#%d)\n"), GetID() );
            else
			    sSource += _T("(newSystem)\n");
			break;
		default:
			wxASSERT_MSG(false, _T("No treatment for this Ctrol type"));
    }

    return sSource;
}

lmLUnits lmSOControl::LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)
{
 //   //DBG ------------------------------------------------------------------------------
 //   //compute position
 //   lmLUnits uyStart = uPos.y - TenthsToLogical(10);
 //   lmLUnits uyEnd = uPos.y + TenthsToLogical(60);
 //   lmLUnits uWidth = TenthsToLogical(1);
 //   lmLUnits uBoundsExtraWidth = TenthsToLogical(2);

 //   //create the shape
 //   lmShapeLine* pShape = new lmShapeLine(this, uPos.x, uyStart, uPos.x, uyEnd,
 //                                       uWidth, uBoundsExtraWidth, lm_eLine_Solid,
 //                                       *wxGREEN, lm_eEdgeNormal);
	//pBox->AddShape(pShape);
 //   m_pGMObj = pShape;

 //   return 0.0;
 //   //END DBG --------------------------------------------------------------------------
	
	return 0.0f;

}

