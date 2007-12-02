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
#include "../graphic/GMObject.h"
#include "../graphic/Shapes.h"
#include "../graphic/ShapeNote.h"


//-----------------------------------------------------------------------------------------
// lmAuxObj implementation
//-----------------------------------------------------------------------------------------

lmAuxObj::lmAuxObj(lmScoreObj* pParent, bool fIsDraggable) :
    lmComponentObj(pParent, eSCOT_AuxObj, fIsDraggable)
{
}

void lmAuxObj::Layout(lmBox* pBox, lmPaper* pPaper, wxColour colorC, bool fHighlight)
{
    // ask derived object to layout itself
    LayoutObject(pBox, pPaper, colorC);

}

lmUPoint lmAuxObj::GetReferencePos(lmPaper* pPaper)
{
	//TODO
	return lmUPoint(0.0, 0.0);
}

lmLUnits lmAuxObj::TenthsToLogical(lmTenths nTenths)
{
	return ((lmStaffObj*)m_pParent)->TenthsToLogical(nTenths);
}

lmTenths lmAuxObj::LogicalToTenths(lmLUnits uUnits)
{
	return ((lmStaffObj*)m_pParent)->LogicalToTenths(uUnits);
}


//========================================================================================
// lmFermata implementation
//========================================================================================

lmFermata::lmFermata(lmNoteRest* pOwner, lmEPlacement nPlacement)
        : lmAuxObj(pOwner, lmDRAGGABLE)
{
    if (nPlacement == ep_Default) {
        if (pOwner->IsRest())
            m_nPlacement = ep_Above;
        else {
            lmNote* pNote = (lmNote*)pOwner;
            if (pNote->GetNoteType() <= eWhole || pNote->StemGoesDown())
                m_nPlacement = ep_Above;
            else
                m_nPlacement = ep_Below;
        }
    }
    else
        m_nPlacement = nPlacement;
}

void lmFermata::SetSizePosition(lmPaper* pPaper, lmVStaff* pVStaff, int nStaffNum,
                             lmLUnits xPos, lmLUnits yPos)
{
    ///*
    //This method does the measurement phase
    //*/

    //m_uPaperPos = m_pOwner->GetOrigin();

    //// prepare DC
    //pPaper->SetFont( *(m_pOwner->GetFont()) );

    //// prepare glyph and measure it
    //bool fAboveNote = (m_nPlacement == ep_Above);
    //wxString sGlyph = (fAboveNote ? CHAR_FERMATA_OVER : CHAR_FERMATA_UNDER );
    //lmLUnits nWidth, nHeight;
    //pPaper->GetTextExtent(sGlyph, &nWidth, &nHeight);

    //// store glyph position
    //m_uGlyphPos.x = xPos - nWidth/2;
    //if (fAboveNote)
    //    m_uGlyphPos.y = yPos - pVStaff->TenthsToLogical( 70, nStaffNum );
    //else
    //    m_uGlyphPos.y = yPos - pVStaff->TenthsToLogical( 5, nStaffNum );


    //// store selection rectangle position and size
    //m_uSelRect.width = nWidth;
    //m_uSelRect.height = pVStaff->TenthsToLogical( 20, nStaffNum );
    //m_uSelRect.x = m_uGlyphPos.x;
    //m_uSelRect.y = m_uGlyphPos.y + pVStaff->TenthsToLogical( (fAboveNote ? 45 : 40), nStaffNum );

}

lmLUnits lmFermata::LayoutObject(lmBox* pBox, lmPaper* pPaper, wxColour colorC)
{
    // This method is invoked by the base class (lmStaffObj). It is responsible for
    // creating the shape object and adding it to the graphical model. 
    // Paper cursor must be used as the base for positioning.

	lmUPoint uPos(pPaper->GetCursorX(), pPaper->GetCursorY());
    bool fAboveNote = (m_nPlacement == ep_Above);

	//TODO
	////if no positioning information do our best!
	if (true)
	{
		if (fAboveNote)
		    uPos.y -= ((lmStaffObj*)m_pParent)->TenthsToLogical(70);
		else
		    uPos.y -= ((lmStaffObj*)m_pParent)->TenthsToLogical(5);
	}
	//else
	//{
	//	// just position fermata on specified pos.
	//}

    //create the shape object
    int nGlyphIndex = (fAboveNote ? GLYPH_FERMATA_OVER : GLYPH_FERMATA_UNDER);
    lmShapeGlyph* pShape = 
		new lmShapeGlyph(this, nGlyphIndex, ((lmStaffObj*)m_pParent)->GetFont(), pPaper, uPos,
						_T("Fermata"), lmDRAGGABLE, colorC);
	pBox->AddShape(pShape);
    m_pShape2 = pShape;

	//if position isn't fixed, shift the shape to center it on the owner and
	//avoid placing it over the note if surpasses the staff
	//TODO
	if (true)
	{
		lmShape* pPS = ((lmStaffObj*)m_pParent)->GetShap2();
		lmLUnits uCenterPos;
		if (((lmStaffObj*)m_pParent)->GetClass() == eSFOT_NoteRest &&
			!((lmNoteRest*)m_pParent)->IsRest() )
		{
			//it is a note. Center fermata on notehead shape
			lmShape* pNHS = ((lmShapeNote*)pPS)->GetNoteHead();
			uCenterPos = pNHS->GetXLeft() + pNHS->GetWidth() / 2.0;
		}
		else
		{
			//it is not a note. Center fermata on StaffObj shape
			uCenterPos = pPS->GetXLeft() + pPS->GetWidth() / 2.0;
		}
        lmLUnits uxShift = uCenterPos - (pShape->GetXLeft() + pShape->GetWidth() / 2.0);
		pShape->Shift(uxShift, 0.0);
	}

	return pShape->GetWidth();
}

wxString lmFermata::SourceLDP(int nIndent)
{
	//TODO
    wxString sSource = _T("");
	sSource += _T("lmFermata");
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
	//TODO
    wxString sDump = _T("lmFermata");
    return sDump;

}


//========================================================================================
// lmLyric object implementation
//========================================================================================
//Global variables used as default initializators
lmFontInfo tLyricDefaultFont = { _T("Arial"), 8, lmTEXT_ITALIC };
lmLocation g_tDefaultPos = {0,0,lmLOCATION_RELATIVE,lmLOCATION_RELATIVE,lmTENTHS,lmTENTHS};

lmLyric::lmLyric(lmNoteRest* pOwner, wxString sText, ESyllabicTypes nSyllabic,
            int nNumLine, wxString sLanguage )
    : lmAuxObj(pOwner, lmDRAGGABLE),
      lmBasicText(sText, sLanguage, &g_tDefaultPos, tLyricDefaultFont)
{
    m_nNumLine = nNumLine;
}

void lmLyric::SetFont(lmPaper* pPaper)
{
    //wxLogMessage(wxString::Format(
    //    _T("[lmLyric::SetFont]: size=%d, name=%s"), m_nFontSize, m_sFontName));

    int nWeight = (m_fBold ? wxBOLD : wxNORMAL);
    int nStyle = (m_fItalic ? wxITALIC : wxNORMAL);
    m_pFont = pPaper->GetFont(m_nFontSize, m_sFontName, wxDEFAULT, nStyle, nWeight, false);

    if (!m_pFont) {
        wxMessageBox(_("Sorry, an error has occurred while allocating the font."),
            _T("lmLyric::SetFont"), wxOK);
        ::wxExit();
    }
}

lmLUnits lmLyric::LayoutObject(lmBox* pBox, lmPaper* pPaper, wxColour colorC)
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

void lmLyric::SetSizePosition(lmPaper* pPaper, lmVStaff* pVStaff, int nStaffNum,
                        lmLUnits xPos, lmLUnits yPos)
{
    ///*
    //This method does the measurement phase
    //*/

    //// save paper position and prepare font
    //m_uPaperPos = m_pOwner->GetOrigin();
    //SetFont(pPaper);

    //// prepare DC
    //pPaper->SetFont(*m_pFont);

    //// prepare the text and measure it
    //lmLUnits nWidth, nHeight;
    //pPaper->GetTextExtent(m_sText, &nWidth, &nHeight);
    //m_uWidth = nWidth;

    //// store glyph position (relative to paper pos).
    ////// Remember: XML positioning values origin is the left-hand side of the note
    ////// or the musical position within the bar (x) and the top line of the staff (y)
    ////m_uGlyphPos.x = m_pVStaff->TenthsToLogical(m_xRel, m_nStaffNum);
    ////// as relative-y refers to the top line of the staff, so 5 lines must be
    ////// substracted from yBase position
    ////m_uGlyphPos.y = m_pVStaff->TenthsToLogical(m_yRel-50, m_nStaffNum);
    ////if (m_fOverrideDefaultX) {
    ////    m_uGlyphPos.x += m_pVStaff->TenthsToLogical(m_xDef, m_nStaffNum) - m_uPaperPos.x;
    ////}
    ////if (m_fOverrideDefaultY) {
    ////    m_uGlyphPos.y += m_pVStaff->TenthsToLogical(m_yDef, m_nStaffNum) - m_uPaperPos.y;
    ////}
    //m_uGlyphPos.x = xPos;
    //m_uGlyphPos.y = yPos + pVStaff->TenthsToLogical( 40, nStaffNum ) * m_nNumLine;

    // // store selection rectangle (relative to m_uPaperPos). Coincides with glyph rectangle
    //m_uSelRect.width = nWidth;
    //m_uSelRect.height = nHeight;
    //m_uSelRect.x = m_uGlyphPos.x;
    //m_uSelRect.y = m_uGlyphPos.y;

}

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
        _T("\t-->lmLyric\t%s\tnumLine=%d, paperPos=(%d, %d)\n"),
        m_sText.c_str(), m_nNumLine, m_uPaperPos.x, m_uPaperPos.y);
    return sDump;

}

//========================================================================================
// lmScoreLine object implementation
//========================================================================================

lmScoreLine::lmScoreLine(lmStaffObj* pOwner,
                   lmTenths xStart, lmTenths yStart, 
                   lmTenths xEnd, lmTenths yEnd,
                   lmTenths nWidth, wxColour nColor)
    : lmAuxObj(pOwner, lmDRAGGABLE) 
{
    // convert data to logical units
    lmStaffObj* pSO = (lmStaffObj*)pOwner;
    m_uxStart = pSO->TenthsToLogical(xStart);
    m_uyStart = pSO->TenthsToLogical(yStart);
    m_uxEnd = pSO->TenthsToLogical(xEnd);
    m_uyEnd = pSO->TenthsToLogical(yEnd);
    m_uWidth = pSO->TenthsToLogical(nWidth);

	m_nColor = nColor;

}

wxString lmScoreLine::SourceLDP(int nIndent)
{
    wxString sSource = _T("");
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
	lmStaffObj* pSO = (lmStaffObj*)m_pParent;
	sSource += wxString::Format(_T("(graphic line %d %d %d %d)\n"),
					(int)(pSO->LogicalToTenths(m_uxStart) + 0.5),
					(int)(pSO->LogicalToTenths(m_uyStart) + 0.5),
					(int)(pSO->LogicalToTenths(m_uxEnd) + 0.5),
					(int)(pSO->LogicalToTenths(m_uyEnd) + 0.5) );
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
            m_uxStart, m_uyStart, m_uxEnd, m_uyEnd, m_uWidth);
    return sDump;

}

lmLUnits lmScoreLine::LayoutObject(lmBox* pBox, lmPaper* pPaper, wxColour colorC)
{
    WXUNUSED(colorC);

    //compute position
    lmLUnits uxStart = m_uxStart + pPaper->GetCursorX();
    lmLUnits uyStart = m_uyStart + pPaper->GetCursorY();
    lmLUnits uxEnd = m_uxEnd + pPaper->GetCursorX();
    lmLUnits uyEnd = m_uyEnd + pPaper->GetCursorY();
    lmLUnits uBoundsExtraWidth = 0.0;

    //create the shape
    lmShapeLine* pShape = new lmShapeLine(this, uxStart, uyStart, uxEnd, uyEnd,
                                          m_uWidth, uBoundsExtraWidth, m_nColor,
                                          _T("GraphLine"), eEdgeNormal);
	pBox->AddShape(pShape);
    m_pShape2 = pShape;
    return pShape->GetBounds().GetWidth();

}


