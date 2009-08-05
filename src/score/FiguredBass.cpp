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
#pragma implementation "FiguredBass.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif


#include "Score.h"
#include "FiguredBass.h"
#include "VStaff.h"


#define lnNUM_INTV      12

//-----------------------------------------------------------------------------------------
// lmFiguredBass implementation
//-----------------------------------------------------------------------------------------

lmFiguredBass::lmFiguredBass(lmVStaff* pVStaff, long nID, wxString& sFiguredBass)
    : lmStaffObj(pVStaff, nID, eSFOT_FiguredBass, pVStaff, 1, lmVISIBLE, lmDRAGGABLE)
    , m_fStartOfLine(false)
    , m_fEndOfLine(false)
{
    SetLayer(lm_eLayerNotes);

    //initialize with 'Tasto solo'
    for (int i=0; i < lnNUM_INTV; i++)
        m_Intervals[i] = lm_eIM_NotPresent;

    //build from string
    if (sFiguredBass == _T(""))
    {
        //8 5 3
        m_Intervals[8] = lm_eIM_AsImplied;
        m_Intervals[5] = lm_eIM_AsImplied;
        m_Intervals[3] = lm_eIM_AsImplied;
    }
    else if (sFiguredBass == _T("0"))
    {
        //Tasto solo: bass alone, upper voices silent
    }
    else if (sFiguredBass == _T("s"))
    {
        //s               Upper voices stay stationary as bass moves
        //TODO
    }
    else if (sFiguredBass == _T("+"))
    {
        //8 5 +3
        m_Intervals[8] = lm_eIM_AsImplied;
        m_Intervals[5] = lm_eIM_AsImplied;
        m_Intervals[3] = lm_eIM_RaiseHalf;
    }
    else if (sFiguredBass == _T("-"))
    {
        //8 5 -3
        m_Intervals[8] = lm_eIM_AsImplied;
        m_Intervals[5] = lm_eIM_AsImplied;
        m_Intervals[3] = lm_eIM_LowerHalf;
    }
    else if (sFiguredBass == _T("="))
    {
        //8 5 =3
        m_Intervals[8] = lm_eIM_AsImplied;
        m_Intervals[5] = lm_eIM_AsImplied;
        m_Intervals[3] = lm_eIM_Natural;
    }
    else if (sFiguredBass == _T("2"))
    {
        //6 4 2
        m_Intervals[6] = lm_eIM_AsImplied;
        m_Intervals[4] = lm_eIM_AsImplied;
        m_Intervals[2] = lm_eIM_AsImplied;
    }
    else if (sFiguredBass == _T("+2"))
    {
        //6 4 +2
        m_Intervals[6] = lm_eIM_AsImplied;
        m_Intervals[4] = lm_eIM_AsImplied;
        m_Intervals[2] = lm_eIM_RaiseHalf;
    }
    else if (sFiguredBass == _T("-2"))
    {
        //6 4 -2
        m_Intervals[6] = lm_eIM_AsImplied;
        m_Intervals[4] = lm_eIM_AsImplied;
        m_Intervals[2] = lm_eIM_LowerHalf;
    }
    else if (sFiguredBass == _T("=2"))
    {
        //6 4 =2
        m_Intervals[6] = lm_eIM_AsImplied;
        m_Intervals[4] = lm_eIM_AsImplied;
        m_Intervals[2] = lm_eIM_Natural;
    }
    else if (sFiguredBass == _T("3"))
    {
        //8 5 3
        m_Intervals[8] = lm_eIM_AsImplied;
        m_Intervals[5] = lm_eIM_AsImplied;
        m_Intervals[3] = lm_eIM_AsImplied;
    }
    else if (sFiguredBass == _T("4"))
    {
        //8 5 4
        m_Intervals[8] = lm_eIM_AsImplied;
        m_Intervals[5] = lm_eIM_AsImplied;
        m_Intervals[4] = lm_eIM_AsImplied;
    }
    else if (sFiguredBass == _T("4 2"))
    {
        //6 4 2
        m_Intervals[6] = lm_eIM_AsImplied;
        m_Intervals[4] = lm_eIM_AsImplied;
        m_Intervals[2] = lm_eIM_AsImplied;
    }
    else if (sFiguredBass == _T("+4 2"))
    {
        //6 +4 2
        m_Intervals[6] = lm_eIM_AsImplied;
        m_Intervals[4] = lm_eIM_RaiseHalf;
        m_Intervals[2] = lm_eIM_AsImplied;
    }
    else if (sFiguredBass == _T("4 3"))
    {
        //6 4 3
        m_Intervals[6] = lm_eIM_AsImplied;
        m_Intervals[4] = lm_eIM_AsImplied;
        m_Intervals[3] = lm_eIM_AsImplied;
    }
    else if (sFiguredBass == _T("5"))
    {
        //8 5 3
        m_Intervals[8] = lm_eIM_AsImplied;
        m_Intervals[5] = lm_eIM_AsImplied;
        m_Intervals[3] = lm_eIM_AsImplied;
    }
    else if (sFiguredBass == _T("+5"))
    {
        //8 +5 3
        m_Intervals[8] = lm_eIM_AsImplied;
        m_Intervals[5] = lm_eIM_RaiseHalf;
        m_Intervals[3] = lm_eIM_AsImplied;
    }
    else if (sFiguredBass == _T("5/"))
    {
        //8 5dim 3
        m_Intervals[8] = lm_eIM_AsImplied;
        m_Intervals[5] = lm_eIM_Diminished;
        m_Intervals[3] = lm_eIM_AsImplied;
    }
    else if (sFiguredBass == _T("5 3"))
    {
        //8 5 3
        m_Intervals[8] = lm_eIM_AsImplied;
        m_Intervals[5] = lm_eIM_AsImplied;
        m_Intervals[3] = lm_eIM_AsImplied;
    }
    else if (sFiguredBass == _T("5 4"))
    {
        //8 5 4
        m_Intervals[8] = lm_eIM_AsImplied;
        m_Intervals[5] = lm_eIM_AsImplied;
        m_Intervals[4] = lm_eIM_AsImplied;
    }
    else if (sFiguredBass == _T("6"))
    {
        //8 6 3
        m_Intervals[8] = lm_eIM_AsImplied;
        m_Intervals[6] = lm_eIM_AsImplied;
        m_Intervals[3] = lm_eIM_AsImplied;
    }
    else if (sFiguredBass == _T("+6"))
    {
        //8 +6 3
        m_Intervals[8] = lm_eIM_AsImplied;
        m_Intervals[6] = lm_eIM_RaiseHalf;
        m_Intervals[3] = lm_eIM_AsImplied;
    }
    else if (sFiguredBass == _T("6 3"))
    {
        //8 6 3
        m_Intervals[8] = lm_eIM_AsImplied;
        m_Intervals[6] = lm_eIM_AsImplied;
        m_Intervals[3] = lm_eIM_AsImplied;
    }
    else if (sFiguredBass == _T("6 4"))
    {
        //8 6 4
        m_Intervals[8] = lm_eIM_AsImplied;
        m_Intervals[6] = lm_eIM_AsImplied;
        m_Intervals[3] = lm_eIM_AsImplied;
    }
    else if (sFiguredBass == _T("6 4 2"))
    {
        //6 4 2
        m_Intervals[6] = lm_eIM_AsImplied;
        m_Intervals[4] = lm_eIM_AsImplied;
        m_Intervals[2] = lm_eIM_AsImplied;
    }
    else if (sFiguredBass == _T("6 4 3"))
    {
        //6 4 3
        m_Intervals[6] = lm_eIM_AsImplied;
        m_Intervals[4] = lm_eIM_AsImplied;
        m_Intervals[3] = lm_eIM_AsImplied;
    }
    else if (sFiguredBass == _T("6 5"))
    {
        //6 5 3
        m_Intervals[6] = lm_eIM_AsImplied;
        m_Intervals[5] = lm_eIM_AsImplied;
        m_Intervals[3] = lm_eIM_AsImplied;
    }
    else if (sFiguredBass == _T("6 5 3"))
    {
        //6 5 3
        m_Intervals[6] = lm_eIM_AsImplied;
        m_Intervals[5] = lm_eIM_AsImplied;
        m_Intervals[3] = lm_eIM_AsImplied;
    }
    else if (sFiguredBass == _T("7"))
    {
        //7 5 3
        m_Intervals[7] = lm_eIM_AsImplied;
        m_Intervals[5] = lm_eIM_AsImplied;
        m_Intervals[3] = lm_eIM_AsImplied;
    }
    else if (sFiguredBass == _T("8"))
    {
        //8. Play the bass line alone in octaves
        m_Intervals[8] = lm_eIM_AsImplied;
    }
    else if (sFiguredBass == _T("9"))
    {
        //9 5 3
        m_Intervals[9] = lm_eIM_AsImplied;
        m_Intervals[5] = lm_eIM_AsImplied;
        m_Intervals[3] = lm_eIM_AsImplied;
    }
    else if (sFiguredBass == _T("10"))
    {
        //10 5 3
        m_Intervals[10] = lm_eIM_AsImplied;
        m_Intervals[5] = lm_eIM_AsImplied;
        m_Intervals[3] = lm_eIM_AsImplied;
    }
    else
    {
        wxLogMessage(_T("[lmFiguredBass::lmFiguredBass] Bad string '%s'"), sFiguredBass);
        wxASSERT(false);
    }

}

lmUPoint lmFiguredBass::ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper)
{
	// if no location is specified in LDP source file, this method is invoked from
	// base class to ask derived object to compute a suitable position to
	// place itself.
	// uOrg is the assigned paper position for this object.

	lmUPoint uPos = uOrg;
	return uPos;
}

lmLUnits lmFiguredBass::LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)
{
    lmLUnits uWidth = m_pVStaff->TenthsToLogical(10, m_nStaffNum);
    lmShape* pShape;

    //TODO
    //For now draw a green rectangle to show its position
    lmLUnits uyStart = uPos.y + m_pParent->TenthsToLogical(5);
    lmLUnits uyEnd = uPos.y + m_pParent->TenthsToLogical(35);
    lmLUnits uLineWidth = m_pParent->TenthsToLogical(1);

    //create the shape
    pShape = new lmShapeRectangle(this, uPos.x, uyStart, uPos.x + uWidth, uyEnd, uLineWidth,
                                  *wxGREEN, _T("FiguredBass"), lmDRAGGABLE,
                                  lmSELECTABLE, lmVISIBLE);

	pBox->AddShape(pShape, GetLayer());
    StoreShape(pShape);

    // set total width
    return uWidth;
}

wxString lmFiguredBass::Dump()
{
    wxString sDump = wxString::Format(
        _T("%d\tFiguredBass\tTimePos=%.2f"),
        m_nId, m_rTimePos );
    sDump += lmStaffObj::Dump();
    sDump += _T("\n");
    return sDump;
}

wxString lmFiguredBass::SourceLDP(int nIndent, bool fUndoData)
{
    wxString sSource = _T("");
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    if (fUndoData)
        sSource += wxString::Format(_T("(figuredBass#%d"), GetID());
    else
        sSource += _T("(figuredBass");

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

wxString lmFiguredBass::SourceXML(int nIndent)
{
    //TODO
    wxString sSource = _T("");
    return sSource;

}


