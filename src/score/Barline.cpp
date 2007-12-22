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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "Barline.h"
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
#include "../graphic/ShapeBarline.h"

//-------------------------------------------------------------------------------------------------
// lmBarline object implementation
//-------------------------------------------------------------------------------------------------
static lmLUnits m_uThinLineWidth;
static lmLUnits m_uThickLineWidth;
static lmLUnits m_uSpacing;         // between lines and lines-dots
static lmLUnits m_uRadius;            // for dots

//
//constructors and destructor
//

lmBarline::lmBarline(lmEBarline nBarlineType, lmVStaff* pVStaff, bool fVisible) :
    lmStaffObj(pVStaff, eSFOT_Barline, pVStaff, 1, fVisible, lmDRAGGABLE)
{
    m_nBarlineType = nBarlineType;

    m_uThinLineWidth = m_pVStaff->TenthsToLogical(1.5, 1);  // thin line width
    m_uThickLineWidth = m_pVStaff->TenthsToLogical(6, 1);   // thick line width
    m_uSpacing = m_pVStaff->TenthsToLogical(4, 1);          // space between lines: 4 tenths
    m_uRadius = m_pVStaff->TenthsToLogical(2, 1);           // dots radius: 2 tenths

    //default location (auto-layout)
    m_xUserPosType = lmLOCATION_DEFAULT;
    m_uxUserPos = 0;

}

void lmBarline::AddContext(lmContext* pContext, int nStaff)
{
    wxASSERT(nStaff-1 == (int)m_aContexts.GetCount());
    m_aContexts.Add(pContext);
}

lmContext* lmBarline::GetContext(int nStaff)
{
    wxASSERT(nStaff <= (int)m_aContexts.GetCount());
    return m_aContexts.Item(nStaff-1);
}

wxString lmBarline::Dump()
{
    wxString sDump = wxString::Format(
        _T("%d\tBarline %s\tTimePos=%.2f\n"),
        m_nId, GetBarlineLDPNameFromType(m_nBarlineType).c_str(), m_rTimePos );
    return sDump;

}

wxString lmBarline::SourceLDP(int nIndent)
{
    wxString sSource = _T("");
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += _T("(barline ");
    sSource += GetBarlineLDPNameFromType(m_nBarlineType);

    //visible?
    if (!m_fVisible) { sSource += _T(" noVisible"); }

	//attached AuxObjs
	sSource += lmStaffObj::SourceLDP(nIndent+1);

    //location
    sSource += SourceLDP_Location(m_uPaperPos);

    sSource += _T(")\n\n");
    return sSource;
}

wxString lmBarline::SourceXML(int nIndent)
{
	wxString sSource = _T("");
	sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
    sSource += _T("TODO: lmBarline XML Source code generation method\n");
    return sSource;
}

lmUPoint lmBarline::ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper)
{
	// if no location is specified in LDP source file, this method is invoked from
	// base class to ask derived object to compute a suitable position to
	// place itself.
	// uOrg is the assigned paper position for this object.

	lmUPoint uPos = uOrg;
	//TODO
	return uPos;
}

lmLUnits lmBarline::LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)
{
    // This method is invoked by the base class (lmStaffObj). It is responsible for
    // creating the shape object and adding it to the graphical model. 
    // Paper cursor must be used as the base for positioning.

    lmLUnits uyTop = m_pVStaff->GetYTop();
    lmLUnits uyBottom = m_pVStaff->GetYBottom();

    //create the shape
    lmShapeBarline* pShape = 
        new lmShapeBarline(this, m_nBarlineType, uPos.x, uyTop, uyBottom, m_uThinLineWidth,
                           m_uThickLineWidth, m_uSpacing, m_uRadius, colorC);
	pBox->AddShape(pShape);
    m_pShape = pShape;
    return pShape->GetBounds().GetWidth();

}

//void lmBarline::SetLocation(lmLUnits uxPos, lmELocationType nType)
//{
//    m_uxUserPos = uxPos;
//    m_xUserPosType = nType;
//}
//

//-------------------------------------------------------------------------------------------------
// global functions related to barlines
//-------------------------------------------------------------------------------------------------

wxString GetBarlineLDPNameFromType(lmEBarline nBarlineType)
{
    switch(nBarlineType)
    {
        case etb_EndRepetitionBarline:
            return _T("endRepetition");
        case etb_StartRepetitionBarline:
            return _T("startRepetition");
        case etb_EndBarline:
            return _T("end");
        case etb_DoubleBarline:
            return _T("double");
        case etb_SimpleBarline:
            return _T("simple");
        case etb_StartBarline:
            return _T("start");
        case etb_DoubleRepetitionBarline:
            return _T("doubleRepetition");
        default:
            wxASSERT(false);
            return _T("");        //let's keep the compiler happy
    }

}
