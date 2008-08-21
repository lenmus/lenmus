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
#pragma implementation "AuxObj.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Score.h"
#include "VStaff.h"
#include "../graphic/GMObject.h"
#include "../graphic/Shapes.h"
#include "../graphic/ShapeNote.h"


//-----------------------------------------------------------------------------------------
// lmAuxObj implementation
//-----------------------------------------------------------------------------------------

lmAuxObj::lmAuxObj(bool fIsDraggable) :
    lmComponentObj((lmComponentObj*)NULL, lm_eAuxObj, &g_tDefaultPos, fIsDraggable)
{
}

void lmAuxObj::Layout(lmBox* pBox, lmPaper* pPaper, wxColour colorC,
					  bool fHighlight)
{
	//assign AuxObj reference position: the parent's m_uComputedPos value
	SetReferencePos(pPaper);

	m_uComputedPos = ComputeObjectLocation(pPaper);
    LayoutObject(pBox, pPaper, m_uComputedPos, colorC);
}

lmLUnits lmAuxObj::TenthsToLogical(lmTenths nTenths)
{
	return ((lmStaffObj*)m_pParent)->TenthsToLogical(nTenths);
}

lmTenths lmAuxObj::LogicalToTenths(lmLUnits uUnits)
{
	return ((lmStaffObj*)m_pParent)->LogicalToTenths(uUnits);
}

void lmAuxObj::SetOwner(lmScoreObj* pOwner)
{
    m_pParent = pOwner;
}

wxFont* lmAuxObj::GetSuitableFont(lmPaper* pPaper)
{
    return m_pParent->GetSuitableFont(pPaper);
}

void lmAuxObj::OnParentComputedPositionShifted(lmLUnits uxShift, lmLUnits uyShift)
{
    //This method is invoked only from lmScoreObj::StoreOriginAndShiftShapes() to inform
    //about a change in the computed final position for parent ScoreObj. Therefore, it
    //is necessary to update this AuxObj reference pos.

	m_uComputedPos.x += uxShift;
	m_uComputedPos.y += uyShift;
    lmShape* pGMObj = GetShape();
	if (pGMObj)
    {
		////DBG--------------------------------------------------------------------------------
		//if (GetID()==4)
		//{
		//	lmUPoint uNewOrg = m_uComputedPos + m_uUserShift;
	    //  lmUPoint uShapePos = m_pGMObj->GetBounds().GetTopLeft();
		//	wxLogMessage(_T("[lmAuxObj::OnParentComputedPositionShifted] uxShift=%.2f, ShapeOrg=(%.2f, %.2f), ShapePos=(%.2f, %.2f), ComputedPos=(%.2f, %.2f), UserShift=(%.2f, %.2f), NewOrg=(%.2f, %.2f)"),
		//				uxShift,
		//				m_pGMObj->GetOrigin().x, m_pGMObj->GetOrigin().y,
		//				uShapePos.x, uShapePos.y,
		//				m_uComputedPos.x, m_uComputedPos.y, m_uUserShift.x, m_uUserShift.y,
		//				uNewOrg.x, uNewOrg.y );
		//}
		////END DBG----------------------------------------------------------------------------
        pGMObj->Shift(uxShift, uyShift);
        pGMObj->ApplyUserShift( this->GetUserShift() );
    }
}

void lmAuxObj::OnParentMoved(lmLUnits uxShift, lmLUnits uyShift)
{
	//TODO: specific flag to decouple from parent staffObj, so the user can 
	//control if the attached AuxObj will be moved with the parent or not

    lmShape* pGMObj = GetShape();
	if (pGMObj)
    {
        lmUPoint uUserShift = GetUserShift();
	    uUserShift.x += uxShift;
	    uUserShift.y += uyShift;
        this->SaveUserLocation(uUserShift.x, uUserShift.y, 0);
        pGMObj->ApplyUserShift(uUserShift);
    }

	////DBG--------------------------------------------------------------------------------
	//if (GetID()==4)	//if (((lmComponentObj*)m_pParent)->GetID()==3)
	//{
	//	lmUPoint uNewOrg = m_uComputedPos + m_uUserShift;
	//	wxLogMessage(_T("[lmAuxObj::OnParentMoved] Shift=(%.2f, %.2f), new UserShift=(%.2f, %.2f)"),
	//				xShift, yShift, m_uUserShift.x, m_uUserShift.y );
	//}
	////END DBG----------------------------------------------------------------------------
}

wxString lmAuxObj::Dump()
{
	return lmComponentObj::Dump();
}

wxString lmAuxObj::SourceLDP(int nIndent)
{
	return lmComponentObj::SourceLDP(nIndent);
}

wxString lmAuxObj::SourceXML(int nIndent)
{
	return lmComponentObj::SourceXML(nIndent);
}



//========================================================================================
// lmFermata implementation
//========================================================================================

lmFermata::lmFermata(lmEPlacement nPlacement)
        : lmAuxObj(lmDRAGGABLE)
{
    m_nPlacement = nPlacement;
}

lmUPoint lmFermata::ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper)
{
	// if no location is specified in LDP source file, this method is invoked from
	// base class to ask derived object to compute a suitable position to
	// place itself.
	// uOrg is the assigned paper position for this object.

	lmUPoint uPos = uOrg;

	// compute y location
	bool fAbove = IsAbove();
	if (fAbove)
		uPos.y -= ((lmStaffObj*)m_pParent)->TenthsToLogical(70);
	else
		uPos.y -= ((lmStaffObj*)m_pParent)->TenthsToLogical(5);

    //create a temporal shape object to get its measurements
    int nGlyphIndex = (fAbove ? GLYPH_FERMATA_OVER : GLYPH_FERMATA_UNDER);
    lmShape* pPS = m_pParent->GetShape();
    lmShapeGlyph* pFS =
		new lmShapeGlyph(this, -1, nGlyphIndex, m_pParent->GetSuitableFont(pPaper),
						 pPaper, uPos, _T("Fermata"), lmDRAGGABLE);

	//center it on the owner
	lmLUnits uCenterPos;
	if (((lmStaffObj*)m_pParent)->GetClass() == eSFOT_NoteRest &&
		((lmNoteRest*)m_pParent)->IsNote() )
	{
		//it is a note. Center fermata on notehead shape
		lmShape* pNHS = ((lmShapeNote*)pPS)->GetNoteHead();
		uCenterPos = pNHS->GetXLeft() + pNHS->GetWidth() / 2.0;
	}
	else
	{
		//it is not a note. Center fermata on parent shape
		uCenterPos = pPS->GetXLeft() + pPS->GetWidth() / 2.0;
	}
    uPos.x += uCenterPos - (pFS->GetXLeft() + pFS->GetWidth() / 2.0);

	//avoid placing it over the note if surpasses the staff
	//TODO

	delete pFS;
	return uPos;

}

bool lmFermata::IsAbove()
{
	//returns true if fermata should be placed above parent object

    if (m_nPlacement == ep_Default) {
        if (((lmNoteRest*)m_pParent)->IsRest())
            return true;
        else {
            lmNote* pNote = (lmNote*)m_pParent;
            if (pNote->GetNoteType() <= eWhole || pNote->StemGoesDown())
                return true;
            else
                return false;
        }
    }
    else
        return (m_nPlacement == ep_Above);
}

lmLUnits lmFermata::LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)
{
    // This method is invoked by the base class (lmStaffObj). It is responsible for
    // creating the shape object and adding it to the graphical model.

    //create the shape object
    int nGlyphIndex = (IsAbove() ? GLYPH_FERMATA_OVER : GLYPH_FERMATA_UNDER);
    lmShapeGlyph* pShape =
		new lmShapeGlyph(this, 0, nGlyphIndex, m_pParent->GetSuitableFont(pPaper),
						 pPaper, uPos, _T("Fermata"), lmDRAGGABLE, colorC);
	pBox->AddShape(pShape);
    StoreShape(pShape);

	return pShape->GetWidth();
}

wxString lmFermata::SourceLDP(int nIndent)
{
    wxString sSource = _T("");
	sSource += _T(" (fermata");
    if (m_nPlacement == ep_Default)
        sSource += _T(")");
    else if (m_nPlacement == ep_Above)
        sSource += _T(" above");
    else
        sSource += _T(" below");

	//base class info
    sSource += lmAuxObj::SourceLDP(nIndent);

	//close element
	sSource += _T(")");
	return sSource;
}
wxString lmFermata::SourceXML(int nIndent)
{
	//TODO
    wxString sSource = _T("");
	sSource += _T("lmFermata");
    return sSource;
}

wxString lmFermata::Dump()
{
	wxString sDump = wxString::Format(_T("\n   %d\tFermata:"), GetID() );

	//base class info
	sDump += lmAuxObj::Dump();
	sDump += _T("\n");
	return sDump;
}


//========================================================================================
// lmLyric object implementation
//========================================================================================
//Global variables used as default initializators
lmFontInfo tLyricDefaultFont = { _T("Arial"), 8, lmTEXT_ITALIC };

lmLyric::lmLyric(wxString sText, ESyllabicTypes nSyllabic, int nNumLine, wxString sLanguage)
    : lmAuxObj(lmDRAGGABLE),
      lmBasicText(sText, sLanguage, &g_tDefaultPos, tLyricDefaultFont)
{
    m_nNumLine = nNumLine;
}

wxFont* lmLyric::GetSuitableFont(lmPaper* pPaper)
{
    //wxLogMessage(_T("[lmLyric::GetSuitableFont]: size=%d, name=%s"),
	//             m_nFontSize, m_sFontName );

    int nWeight = (m_fBold ? wxBOLD : wxNORMAL);
    int nStyle = (m_fItalic ? wxITALIC : wxNORMAL);
    wxFont* pFont = pPaper->GetFont(m_nFontSize, m_sFontName, wxDEFAULT, nStyle, nWeight, false);

    if (!pFont) {
        wxMessageBox(_("Sorry, an error has occurred while allocating the font."),
            _T("lmLyric::GetSuitableFont"), wxOK);
        ::wxExit();
    }
	return pFont;
}

lmUPoint lmLyric::ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper)
{
	// if no location is specified in LDP source file, this method is invoked from
	// base class to ask derived object to compute a suitable position to
	// place itself.
	// uOrg is the assigned paper position for this object.

	lmUPoint uPos = uOrg;
	//TODO
	return uPos;
}

lmLUnits lmLyric::LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)
{
    //wxASSERT(fMeasuring == DO_DRAW);    //measuring pahse is done in SetSizePosition()

    //pPaper->SetFont(*m_pFont);

    //lmUPoint uPos = GetGlyphPosition();
    //pPaper->SetTextForeground((m_fSelected ? g_pColors->ScoreSelected() : colorC));
    //pPaper->DrawText(m_sText, uPos.x, uPos.y );
	return 0;
}

void lmLyric::SetOwner(lmNoteRest* pOwner)
{
    //wxASSERT(pOwner);
    //m_pOwner = pOwner;
    //m_pVStaff = pOwner->GetVStaff();
    //m_nStaffNum = pOwner->GetStaffNum();
}

//void lmLyric::SetSizePosition(lmPaper* pPaper, lmVStaff* pVStaff, int nStaffNum,
//                        lmLUnits xPos, lmLUnits yPos)
//{
//    ///*
//    //This method does the measurement phase
//    //*/
//
//    //// save paper position and prepare font
//    //m_uPaperPos = m_pOwner->GetReferencePaperPos();
//    //SetFont(pPaper);
//
//    //// prepare DC
//    //pPaper->SetFont(*m_pFont);
//
//    //// prepare the text and measure it
//    //lmLUnits nWidth, nHeight;
//    //pPaper->GetTextExtent(m_sText, &nWidth, &nHeight);
//    //m_uWidth = nWidth;
//
//    //// store glyph position (relative to paper pos).
//    ////// Remember: XML positioning values origin is the left-hand side of the note
//    ////// or the musical position within the bar (x) and the top line of the staff (y)
//    ////m_uGlyphPos.x = m_pVStaff->TenthsToLogical(m_xRel, m_nStaffNum);
//    ////// as relative-y refers to the top line of the staff, so 5 lines must be
//    ////// substracted from yBase position
//    ////m_uGlyphPos.y = m_pVStaff->TenthsToLogical(m_yRel-50, m_nStaffNum);
//    ////if (m_fOverrideDefaultX) {
//    ////    m_uGlyphPos.x += m_pVStaff->TenthsToLogical(m_xDef, m_nStaffNum) - m_uPaperPos.x;
//    ////}
//    ////if (m_fOverrideDefaultY) {
//    ////    m_uGlyphPos.y += m_pVStaff->TenthsToLogical(m_yDef, m_nStaffNum) - m_uPaperPos.y;
//    ////}
//    //m_uGlyphPos.x = xPos;
//    //m_uGlyphPos.y = yPos + pVStaff->TenthsToLogical( 40, nStaffNum ) * m_nNumLine;
//
//    // // store selection rectangle (relative to m_uPaperPos). Coincides with glyph rectangle
//    //m_uSelRect.width = nWidth;
//    //m_uSelRect.height = nHeight;
//    //m_uSelRect.x = m_uGlyphPos.x;
//    //m_uSelRect.y = m_uGlyphPos.y;
//
//}
//
wxString lmLyric::SourceLDP(int nIndent)
{
	//TODO
    wxString sSource = _T("");
	sSource += _T("lmLyric");
    return sSource;
}
wxString lmLyric::SourceXML(int nIndent)
{
	//TODO
    wxString sSource = _T("");
	sSource += _T("lmLyric");
    return sSource;
}

wxString lmLyric::Dump()
{
	//TODO
    wxString sDump = wxString::Format(
        _T("\t-->lmLyric\t%s\tnumLine=%d\n"),
        m_sText.c_str(), m_nNumLine);
    return sDump;

}

//========================================================================================
// lmScoreLine object implementation
//========================================================================================

lmScoreLine::lmScoreLine(lmTenths xStart, lmTenths yStart,
                         lmTenths xEnd, lmTenths yEnd,
                         lmTenths tWidth, wxColour nColor)
    : lmAuxObj(lmDRAGGABLE)
{
    m_txStart = xStart;
    m_tyStart = yStart;
    m_txEnd = xEnd;
    m_tyEnd = yEnd;
    m_tWidth = tWidth;
	m_nColor = nColor;
}

wxString lmScoreLine::SourceLDP(int nIndent)
{
    wxString sSource = _T("");
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
	sSource += wxString::Format(_T("(graphic line %d %d %d %d)\n"),
					(int)(m_txStart + 0.5), (int)(m_tyStart + 0.5),
					(int)(m_txEnd + 0.5), (int)(m_tyEnd + 0.5) );
    return sSource;
}

wxString lmScoreLine::SourceXML(int nIndent)
{
	//TODO
    wxString sSource = _T("");
	sSource += _T("lmScoreLine");
    return sSource;
}

wxString lmScoreLine::Dump()
{
	//TODO
    wxString sDump = wxString::Format(
        _T("\t-->lmScoreLine\tstart=(%.2f, %.2f), end=(%.2f, %.2f), width=%.2f\n"),
            m_txStart, m_tyStart, m_txEnd, m_tyEnd, m_tWidth);
    return sDump;

}

lmUPoint lmScoreLine::ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper)
{
	// if no location is specified in LDP source file, this method is invoked from
	// base class to ask derived object to compute a suitable position to
	// place itself.
	// uOrg is the assigned paper position for this object.

	lmUPoint uPos = uOrg;
	//TODO
	return uPos;
}

lmLUnits lmScoreLine::LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)
{
    WXUNUSED(colorC);

    //compute position
    lmLUnits uxStart = m_pParent->TenthsToLogical(m_txStart) + pPaper->GetCursorX();
    lmLUnits uyStart = m_pParent->TenthsToLogical(m_tyStart) + pPaper->GetCursorY();
    lmLUnits uxEnd = m_pParent->TenthsToLogical(m_txEnd) + pPaper->GetCursorX();
    lmLUnits uyEnd = m_pParent->TenthsToLogical(m_tyEnd) + pPaper->GetCursorY();
    lmLUnits uWidth = m_pParent->TenthsToLogical(m_tWidth);
    lmLUnits uBoundsExtraWidth = 0.0;

    //create the shape
    lmShapeLine* pShape = new lmShapeLine(this, uxStart, uyStart, uxEnd, uyEnd,
                                          uWidth, uBoundsExtraWidth, m_nColor,
                                          _T("GraphLine"), eEdgeNormal);
	pBox->AddShape(pShape);
    StoreShape(pShape);
    return pShape->GetBounds().GetWidth();

}


