//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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
#pragma implementation "MetronomeMark.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "Score.h"
#include "MetronomeMark.h"
#include "../graphic/GMObject.h"
#include "../graphic/Shapes.h"

// 'note_symbol = 80'
lmMetronomeMark::lmMetronomeMark(lmVStaff* pVStaff, lmENoteType nNoteType, int nDots,
                    int nTicksPerMinute, bool fParentheses, bool fVisible)
    : lmStaffObj(pVStaff, eSFOT_MetronomeMark, pVStaff, 1, fVisible, lmDRAGGABLE)
{
    m_nMarkType = eMMT_Note_Value;
    m_nLeftNoteType = nNoteType;
    m_nLeftDots = nDots;
    m_nTicksPerMinute = nTicksPerMinute;
    m_fParentheses = fParentheses;
}

// 'm.m. = 80'
lmMetronomeMark::lmMetronomeMark(lmVStaff* pVStaff, int nTicksPerMinute,
                    bool fParentheses, bool fVisible)
    : lmStaffObj(pVStaff, eSFOT_MetronomeMark, pVStaff, 1, fVisible, lmDRAGGABLE)
{
    m_nMarkType = eMMT_MM_Value;
    m_nTicksPerMinute = nTicksPerMinute;
    m_fParentheses = fParentheses;
}

// 'note_symbol = note_symbol'
lmMetronomeMark::lmMetronomeMark(lmVStaff* pVStaff,
                    lmENoteType nLeftNoteType, int nLeftDots,
                    lmENoteType nRightNoteType, int nRightDots,
                    bool fParentheses, bool fVisible)
    : lmStaffObj(pVStaff, eSFOT_MetronomeMark, pVStaff, 1, fVisible, lmDRAGGABLE)
{
    m_nMarkType = eMMT_Note_Note;
    m_nLeftNoteType = nLeftNoteType;
    m_nLeftDots = nLeftDots;
    m_nRightNoteType = nRightNoteType;
    m_nRightDots = nRightDots;
    m_nTicksPerMinute = 0;
    m_fParentheses = fParentheses;
}

lmMetronomeMark::~lmMetronomeMark()
{
}


//-----------------------------------------------------------------------------------------
// implementation of virtual methods defined in base abstract class lmStaffObj
//-----------------------------------------------------------------------------------------

lmUPoint lmMetronomeMark::ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper)
{
	// if no location is specified in LDP source file, this method is invoked from
	// base class to ask derived object to compute a suitable position to
	// place itself.
	// uOrg is the assigned paper position for this object.

	lmUPoint uPos = uOrg;
    uPos.y -= m_pVStaff->TenthsToLogical(50, m_nStaffNum);
	return uPos;
}

lmLUnits lmMetronomeMark::LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)
{
	//create the container shape and add it to the box
	lmCompositeShape* pShape = new lmCompositeShape(this, _("metronome mark"), lmDRAGGABLE);
	pBox->AddShape(pShape);
	m_pShape = pShape;

	wxFont* pFont = GetSuitableFont(pPaper);

    switch(m_nMarkType)
    {
        case eMMT_MM_Value:         // 'm.m. = 80'
			AddTextShape(pShape, pPaper, wxString::Format(_T("m.m. = %d"), m_nTicksPerMinute),
						uPos, colorC);
            break;

        case eMMT_Note_Note:        // 'note_symbol = note_symbol'
			uPos.x += AddSymbolShape(pShape, pPaper, SelectGlyph(m_nLeftNoteType, m_nLeftDots),
									 pFont, uPos, colorC);
			uPos.x += AddTextShape(pShape, pPaper, _T(" =  "), uPos, colorC);
			AddSymbolShape(pShape, pPaper, SelectGlyph(m_nRightNoteType, m_nRightDots),
				           pFont, uPos, colorC);
            break;

        case eMMT_Note_Value:       // 'note_symbol = 80'
            uPos.x += AddSymbolShape(pShape, pPaper, SelectGlyph(m_nLeftNoteType, m_nLeftDots),
									 pFont, uPos, colorC);
            AddTextShape(pShape, pPaper, wxString::Format(_T(" = %d"), m_nTicksPerMinute),
						 uPos, colorC);
            break;

        default:
            wxASSERT(false);
    }

	return pShape->GetWidth();
}

lmLUnits lmMetronomeMark::AddTextShape(lmCompositeShape* pShape, lmPaper* pPaper,
									   wxString sText, lmUPoint uPos, wxColour colorC)
{
   // returns the width of the text shape

    int nWeight = wxNORMAL;
    int nStyle = wxNORMAL;
    int nFontSize = PointsToLUnits(8);
    wxFont* pFont = pPaper->GetFont(nFontSize, _T("Times New Roman"), wxDEFAULT, nStyle, nWeight, false);
    if (!pFont) {
        wxMessageBox(_("Sorry, an error has occurred while allocating the font."),
            _T("lmMetronomeMark::AddTextShape"), wxOK);
        ::wxExit();
    }
    uPos.y += m_pVStaff->TenthsToLogical(10, m_nStaffNum);

	//create the shape
    lmShapeText* pTS =
		new lmShapeText(this, sText, pFont, pPaper, uPos, _T("equal sign"), lmDRAGGABLE,
						colorC);

	//add the shape to the composite parent shape
	pShape->Add(pTS);

	return pTS->GetWidth();
}

lmLUnits lmMetronomeMark::AddSymbolShape(lmCompositeShape* pShape, lmPaper* pPaper, lmEGlyphIndex nGlyph,
									     wxFont* pFont, lmUPoint uPos, wxColour colorC)
{
    // returns the width of the symbol shape

    uPos.y -= m_pVStaff->TenthsToLogical(35, m_nStaffNum);

	//create the shape
    lmShapeGlyph* pSG = 
		new lmShapeGlyph(this, nGlyph, pFont, pPaper, uPos, _T("metronome mark symbol"),
				         lmDRAGGABLE, colorC);

	//add the shape to the composite parent shape
	pShape->Add(pSG);

	return pSG->GetWidth();
}

lmEGlyphIndex lmMetronomeMark::SelectGlyph(lmENoteType nNoteType, int nDots)
{
    lmEGlyphIndex nGlyph = GLYPH_SMALL_QUARTER_NOTE;
    switch (nNoteType)
	{
        case eQuarter:
            if (nDots == 0)
                nGlyph = GLYPH_SMALL_QUARTER_NOTE;
            else
                nGlyph = GLYPH_SMALL_QUARTER_NOTE_DOTTED;
            break;
        case eEighth:
            if (nDots == 0)
                nGlyph = GLYPH_SMALL_EIGHTH_NOTE;
            else
                nGlyph = GLYPH_SMALL_EIGHTH_NOTE_DOTTED;
            break;
        default:
            wxASSERT(false);
    }
    return nGlyph;

}

wxString lmMetronomeMark::Dump()
{
    wxString sDump = wxString::Format(
        _T("%d\tMetronome %d\tTicksPerMinute=%d, fParentheses=%s,")
        _T(" TimePos=%.2f\n"),
        m_nId, m_nMarkType, m_nTicksPerMinute, (m_fParentheses ? _T("yes") : _T("no")),
        m_rTimePos );
    return sDump;
}

wxString lmMetronomeMark::SourceLDP(int nIndent)
{
    wxString sSource = _T("");
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += _T("(metronome ");

    switch (m_nMarkType)
    {
        case eMMT_MM_Value:
            sSource += wxString::Format(_T(" %d"), m_nTicksPerMinute);
            break;
        case eMMT_Note_Note:
            sSource += GetLDPNote(m_nLeftNoteType, m_nLeftDots);
            sSource += GetLDPNote(m_nRightNoteType, m_nRightDots);
            break;
        case eMMT_Note_Value:
            sSource += GetLDPNote(m_nLeftNoteType, m_nLeftDots);
            sSource += wxString::Format(_T(" %d"), m_nTicksPerMinute);
            break;
        default:
            wxASSERT(false);
    }

    if (m_fParentheses) sSource += _T(" parentheses");
    if (!m_fVisible) sSource += _T(" noVisible");

	//location
    sSource += SourceLDP_Location( ((lmStaffObj*)m_pParent)->GetReferencePaperPos() );

	//close element
    sSource += _T(")\n");
    return sSource;
}

wxString lmMetronomeMark::SourceXML(int nIndent)
{
    //TODO all
	wxString sSource = _T("");
	sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
    sSource += _T("TODO: lmMetronomeMark XML Source code generation method\n");

    return sSource;
}

wxString lmMetronomeMark::GetLDPNote(lmENoteType nNoteType, int nDots)
{
    // returns LDP data (English) starting with an space

    wxString sNote = _T(" ");
    switch (nNoteType)
    {
        case eWhole:
            sNote += _T("w");
            break;
        case eHalf:
            sNote += _T("h");
            break;
        case eQuarter:
            sNote += _T("q");
            break;
        case eEighth:
            sNote += _T("e");
            break;
        case e16th:
            sNote += _T("s");
            break;
        case e32th:
            sNote += _T("t");
            break;
        case e64th:
            sNote += _T("i");
            break;
        case e128th:
            sNote += _T("o");
            break;
        case e256th:
            sNote += _T("f");
            break;
        default:
            wxLogMessage(_T("[lmMetronomeMark::GetLDPNote] Invalid NoteType %d "), nNoteType);
            wxASSERT(false);
    }

    for(int i=0; i < nDots; i++) sNote += _T(".");

    return sNote;
}
