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
#pragma implementation "MetronomeMark.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "Score.h"
#include "MetronomeMark.h"
#include "../graphic/Shape.h"

// 'note_symbol = 80'
lmMetronomeMark::lmMetronomeMark(lmVStaff* pVStaff, ENoteType nNoteType, int nDots,
                    int nTicksPerMinute, bool fParentheses, bool fVisible)
    : lmStaffObj(pVStaff, eSFOT_MetronomeMark, pVStaff, 1, fVisible, lmDRAGGABLE)
{
    m_nMarkType = eMMT_Note_Value;
    m_nLeftNoteType = nNoteType;
    m_nLeftDots = nDots;
    m_nTicksPerMinute = nTicksPerMinute;
    m_fParentheses = fParentheses;
    wxString sText = wxString::Format(_T(" = %d"), m_nTicksPerMinute);
    m_pTextShape = new lmShapeText(this, sText, (wxFont*)NULL);

    m_pRightNoteShape = (lmShapeGlyph*)NULL;
    m_pLeftNoteShape = new lmShapeGlyph(this, SelectGlyph(nNoteType, nDots), m_pFont);

}

// 'm.m. = 80'
lmMetronomeMark::lmMetronomeMark(lmVStaff* pVStaff, int nTicksPerMinute,
                    bool fParentheses, bool fVisible)
    : lmStaffObj(pVStaff, eSFOT_MetronomeMark, pVStaff, 1, fVisible, lmDRAGGABLE)
{
    m_nMarkType = eMMT_MM_Value;
    m_nTicksPerMinute = nTicksPerMinute;
    m_fParentheses = fParentheses;
    wxString sText = wxString::Format(_T("m.m. = %d"), m_nTicksPerMinute);
    m_pTextShape = new lmShapeText(this, sText, (wxFont*)NULL);

    m_pLeftNoteShape = (lmShapeGlyph*)NULL;
    m_pRightNoteShape = (lmShapeGlyph*)NULL;
}

// 'note_symbol = note_symbol'
lmMetronomeMark::lmMetronomeMark(lmVStaff* pVStaff,
                    ENoteType nLeftNoteType, int nLeftDots,
                    ENoteType nRightNoteType, int nRightDots,
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
    m_pTextShape = new lmShapeText(this, _T(" =  "), (wxFont*)NULL);

    m_pLeftNoteShape = new lmShapeGlyph(this, SelectGlyph(nLeftNoteType, nLeftDots), m_pFont);
    m_pRightNoteShape = new lmShapeGlyph(this, SelectGlyph(nRightNoteType, nRightDots), m_pFont);
}

lmMetronomeMark::~lmMetronomeMark()
{
    if (m_pLeftNoteShape) delete m_pLeftNoteShape;
    if (m_pRightNoteShape) delete m_pRightNoteShape;
}


//-----------------------------------------------------------------------------------------
// implementation of virtual methods defined in base abstract class lmStaffObj
//-----------------------------------------------------------------------------------------

wxBitmap* lmMetronomeMark::GetBitmap(double rScale)
{
    //todo
    return (wxBitmap*)NULL; //PrepareBitMap(rScale, m_sText);
}

void lmMetronomeMark::DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC,
                                  bool fHighlight)
{
    lmLUnits yPos = pPaper->GetCursorY() - m_pVStaff->TenthsToLogical(50, m_nStaffNum);
    lmLUnits xPos = pPaper->GetCursorX();
    lmLUnits uWidth = DrawMetronomeMark(fMeasuring, pPaper, xPos, yPos, colorC);

    if (fMeasuring) {
        // store selection rectangle measures and position
        m_selRect.width = uWidth;
        m_selRect.height = m_pVStaff->TenthsToLogical(32, m_nStaffNum); //todo
        m_selRect.x = xPos - m_paperPos.x;        //relative to m_paperPos
        m_selRect.y = yPos - m_paperPos.y;;

        // set total width to zero: metronome marks does not consume staff space
        m_nWidth = 0;   // uWidth;

        // store glyph position (relative to paper pos).
        m_glyphPos.x = 0;
        m_glyphPos.y = pPaper->GetCursorY() - yPos;
    }

}

// returns the width of the metronome mark (in logical units)
lmLUnits lmMetronomeMark::DrawMetronomeMark(bool fMeasuring, lmPaper* pPaper,
                                lmLUnits xPos, lmLUnits yPos, wxColour colorC)
{
    lmLUnits xStart = xPos;
    switch(m_nMarkType)
    {
        case eMMT_MM_Value:         // 'm.m. = 80'
            return DrawText(fMeasuring, pPaper, xPos, yPos, colorC);
            break;

        case eMMT_Note_Note:        // 'note_symbol = note_symbol'
            xPos += DrawSymbol(fMeasuring, pPaper, m_pLeftNoteShape, xPos, yPos, colorC);
            xPos += DrawText(fMeasuring, pPaper, xPos, yPos, colorC);
            xPos += DrawSymbol(fMeasuring, pPaper, m_pRightNoteShape, xPos, yPos, colorC);
            return xPos - xStart;
            break;

        case eMMT_Note_Value:       // 'note_symbol = 80'
            xPos += DrawSymbol(fMeasuring, pPaper, m_pLeftNoteShape, xPos, yPos, colorC);
            xPos += DrawText(fMeasuring, pPaper, xPos, yPos, colorC);
            return xPos - xStart;
            break;

        default:
            wxASSERT(false);
            return 0;            // to keep compiler happy

    }
}

lmLUnits lmMetronomeMark::DrawText(bool fMeasuring, lmPaper* pPaper,
                               lmLUnits xPos, lmLUnits yPos, wxColour colorC)
{
    // returns the width of the text (in logical units)

    if (fMeasuring) {
        int nWeight = wxNORMAL;
        int nStyle = wxNORMAL;
        int nFontSize = PointsToLUnits(8);
        wxFont* pFont = pPaper->GetFont(nFontSize, _T("Times New Roman"), wxDEFAULT, nStyle, nWeight, false);
        if (!pFont) {
            wxMessageBox(_("Sorry, an error has occurred while allocating the font."),
                _T("lmScoreText::SetFont"), wxOK);
            ::wxExit();
        }
        m_pTextShape->SetFont(pFont);
        lmUPoint offset(xPos - m_paperPos.x,
                        yPos - m_paperPos.y + m_pVStaff->TenthsToLogical(10, m_nStaffNum));
        m_pTextShape->Measure(pPaper, m_pVStaff->GetStaff(m_nStaffNum), offset);
    }
    else {
        m_pTextShape->Render(pPaper, m_paperPos, colorC);
    }
    return m_pTextShape->GetWidth();

}

lmLUnits lmMetronomeMark::DrawSymbol(bool fMeasuring, lmPaper* pPaper, lmShapeGlyph* pShape,
                                     lmLUnits xPos, lmLUnits yPos, wxColour colorC)
{
    // returns the width of the note (in logical units)

    wxASSERT(pShape);
    if (fMeasuring) {
        lmUPoint offset(xPos - m_paperPos.x, yPos - m_paperPos.y - m_pVStaff->TenthsToLogical(35, m_nStaffNum));
        pShape->SetFont(m_pFont);
        pShape->Measure(pPaper, m_pVStaff->GetStaff(m_nStaffNum), offset);
    }
    else {
        pShape->Render(pPaper, m_paperPos, colorC);
    }
    return pShape->GetWidth();
}

lmEGlyphIndex lmMetronomeMark::SelectGlyph(ENoteType nNoteType, int nDots)
{
    lmEGlyphIndex nGlyph = GLYPH_SMALL_QUARTER_NOTE;
    switch (nNoteType) {
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
        _T(" TimePos=%.2f, fixed=%s\n"),
        m_nId, m_nMarkType, m_nTicksPerMinute, (m_fParentheses ? _T("yes") : _T("no")),
        m_rTimePos, (m_fFixedPos ? _T("yes") : _T("no")) );
    return sDump;
}

wxString lmMetronomeMark::SourceLDP()
{
    wxString sSource = _T("         (metronome");

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
    sSource += _T(")\n");
    return sSource;
}

wxString lmMetronomeMark::SourceXML()
{
    //! @todo all
    wxString sSource = _T("TODO: lmMetronomeMark XML Source code generation methods");
    return sSource;
}

wxString lmMetronomeMark::GetLDPNote(ENoteType nNoteType, int nDots)
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
    }

    for(int i=0; i < nDots; i++) sNote += _T(".");

    return sNote;
}
