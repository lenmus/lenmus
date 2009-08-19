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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "Spacer.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif


#include "Score.h"
#include "Spacer.h"
#include "VStaff.h"
#include "../graphic/ShapeLine.h"


//-----------------------------------------------------------------------------------------
// lmSpacer implementation
//-----------------------------------------------------------------------------------------

lmSpacer::lmSpacer(lmVStaff* pVStaff, long nID, lmTenths nWidth, int nStaff)
    : lmStaffObj(pVStaff, nID, lm_eSO_Spacer, pVStaff, nStaff, lmVISIBLE, lmDRAGGABLE)
    , m_nSpacerWidth(nWidth)
{
    wxASSERT(nStaff > 0);
    SetLayer(lm_eLayerNotes);
}

lmUPoint lmSpacer::ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper)
{
	// if no location is specified in LDP source file, this method is invoked from
	// base class to ask derived object to compute a suitable position to
	// place itself.
	// uOrg is the assigned paper position for this object.

	lmUPoint uPos = uOrg;
	return uPos;
}

lmLUnits lmSpacer::LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)
{
    lmLUnits uWidth = m_pVStaff->TenthsToLogical(m_nSpacerWidth, m_nStaffNum);
    lmShape* pShape;
    if (g_fDrawAnchors)
    {
        //Draw a red rectangle to show spacer position
        lmLUnits uyStart = uPos.y + m_pParent->TenthsToLogical(5);
        lmLUnits uyEnd = uPos.y + m_pParent->TenthsToLogical(35);
        lmLUnits uLineWidth = m_pParent->TenthsToLogical(1);

        //create the shape
        pShape = new lmShapeRectangle(this, uPos.x, uyStart, uPos.x + uWidth, uyEnd, uLineWidth,
                                      *wxRED, _T("Spacer"), lmDRAGGABLE, lmSELECTABLE, lmVISIBLE);
    }
    else
        pShape = new lmShapeInvisible(this, 0, uPos, lmUSize(uWidth, 0.0), _T("spacer"));

	pBox->AddShape(pShape, GetLayer());
    StoreShape(pShape);

    // set total width
    return uWidth;
}

wxString lmSpacer::Dump()
{
    wxString sDump = wxString::Format(
        _T("%d\tSpacer %.2f\tTimePos=%.2f"),
        m_nId, m_nSpacerWidth, m_rTimePos );
    sDump += lmStaffObj::Dump();
    sDump += _T("\n");
    return sDump;
}

wxString lmSpacer::SourceLDP(int nIndent, bool fUndoData)
{
    wxString sSource = _T("");
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    if (fUndoData)
        sSource += wxString::Format(_T("(spacer#%d %.0f"), GetID(), m_nSpacerWidth);
    else
        sSource += wxString::Format(_T("(spacer %.0f"), m_nSpacerWidth);

	//base class
	wxString sBase = lmStaffObj::SourceLDP(nIndent, fUndoData);
    if (sBase != _T(""))
    {
        sSource += sBase;
        sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    }

    //close element
    sSource += _T(")\n");
	return sSource;
}

wxString lmSpacer::SourceXML(int nIndent)
{
    // THINK:
    // MusicXML doesn't have the spacer concept. Instead, each element have positioning
    // information. To covert to MusicXML it is enough to save positioning information.

    wxString sSource = _T("");
    return sSource;

}


//-----------------------------------------------------------------------------------------
// lmScoreAnchor implementation
//-----------------------------------------------------------------------------------------

lmScoreAnchor::lmScoreAnchor(lmVStaff* pVStaff, long nID, int nStaff)
    : lmStaffObj(pVStaff, nID, lm_eSO_ScoreAnchor, pVStaff, nStaff, lmVISIBLE, lmNO_DRAGGABLE)
{
    wxASSERT(nStaff > 0);
    SetLayer(lm_eLayerNotes);
}

lmUPoint lmScoreAnchor::ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper)
{
	// if no location is specified in LDP source file, this method is invoked from
	// base class to ask derived object to compute a suitable position to
	// place itself.
	// uOrg is the assigned paper position for this object.

	lmUPoint uPos = uOrg;
	//TODO
	return uPos;
}

lmLUnits lmScoreAnchor::LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)
{
    //force paper pos. to page origin
    lmScore* pScore = GetScore();
    pPaper->SetCursor(pScore->GetPageLeftMargin(), pScore->GetPageTopMargin());

    // set total width
    return 0;
}

wxString lmScoreAnchor::Dump()
{
    wxString sDump = wxString::Format(
        _T("%d\tScoreAnchor\tTimePos=%.2f\n"),
        m_nId, m_rTimePos );
    sDump += lmStaffObj::Dump();
    sDump += _T("\n");
    return sDump;
}

wxString lmScoreAnchor::SourceLDP(int nIndent, bool fUndoData)
{
    return lmStaffObj::SourceLDP(nIndent, fUndoData);
}

wxString lmScoreAnchor::SourceXML(int nIndent)
{
    // TODO
    wxString sSource = _T("");
    return sSource;

}
