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
/*! @file Accidental.cpp
    @brief Implementation file for class lmAccidental
    @ingroup score_kernel
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

#include "Score.h"


//========================================================================================
// lmAccidental object implementation
//========================================================================================

lmAccidental::lmAccidental(lmNoteRest* pOwner, EAccidentals nType) :
    lmNoteRestObj(eST_Accidental, pOwner)
{
    m_nType = nType;
}

void lmAccidental::SetSizePosition(lmPaper* pPaper, lmVStaff* pVStaff, wxInt32 nStaffNum,
                             lmLUnits xPos, lmLUnits yPos)
{
    /*
    This method does the measurement phase
    */
    m_paperPos = m_pOwner->GetOrigin();

    // prepare DC
    wxDC* pDC = pPaper->GetDC();
    pDC->SetFont( *(m_pOwner->GetFont()) );

    // prepare glyphs and measure them
    lmLUnits nOffset, nWidth, nHeight, nShift, nNotUsed;
    wxString sGlyphs = GetAccidentalGlyphs(m_nType, &nOffset, &nWidth, &nHeight, &nShift,
                                           pVStaff, nStaffNum );
    pDC->GetTextExtent(sGlyphs, &nWidth, &nNotUsed);

    // store glyphs position
    m_glyphPos.x = xPos;
    m_glyphPos.y = yPos + nOffset;

    // store selection rectangle position and size
    m_selRect.width = nWidth;
    m_selRect.height = nHeight;
    m_selRect.x = m_glyphPos.x;
    m_selRect.y = m_glyphPos.y + nShift;

    #define ACCIDENTALS_AFTERSPACE  7      //in tenths   @todo user options
    m_nAfterSpace = pVStaff->TenthsToLogical(ACCIDENTALS_AFTERSPACE, nStaffNum);

}

void lmAccidental::DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC)
{
    // prepare DC
    wxDC* pDC = pPaper->GetDC();
    wxASSERT(pDC);
    pDC->SetFont( *(m_pOwner->GetFont()) );

    wxString sGlyphs = GetAccidentalGlyphs(m_nType);
    wxPoint pos = GetGlyphPosition();
    pDC->SetTextForeground((m_fSelected ? g_pColors->ScoreSelected() : colorC));
    pDC->DrawText(sGlyphs, pos.x, pos.y );

}

lmLUnits lmAccidental::GetWidth()
{
    return GetSelRect().width + m_nAfterSpace;

}




//global methods related to accidentals

wxString GetAccidentalGlyphs(EAccidentals nType,
                             lmLUnits* pOffset,
                             lmLUnits* pWidth, lmLUnits* pHeight, 
                             lmLUnits* pShift,
                             lmVStaff* pVStaff, wxInt32 nStaff)
{
    wxString sGlyphs;
    int nGlyph[2] = { -1, -1};
    switch(nType) {
        case eNatural:
            nGlyph[0] = GLYPH_NATURAL_ACCIDENTAL;
            break;
        case eSharp:
            nGlyph[0] = GLYPH_SHARP_ACCIDENTAL;
            break;
        case eFlat:
            nGlyph[0] = GLYPH_FLAT_ACCIDENTAL;
            break;
        case eFlatFlat:
            nGlyph[0] = GLYPH_DOUBLE_FLAT_ACCIDENTAL;
            break;
        case eDoubleSharp:
            nGlyph[0] = GLYPH_DOUBLE_SHARP_ACCIDENTAL;
            break;
        case eNaturalFlat:
            nGlyph[0] = GLYPH_NATURAL_ACCIDENTAL;
            nGlyph[1] = GLYPH_FLAT_ACCIDENTAL;
            break;
        case eNaturalSharp:
            nGlyph[0] = GLYPH_NATURAL_ACCIDENTAL;
            nGlyph[1] = GLYPH_SHARP_ACCIDENTAL;
            break;
        case eSharpSharp:
            nGlyph[0] = GLYPH_SHARP_ACCIDENTAL;
            nGlyph[1] = GLYPH_SHARP_ACCIDENTAL;
            break;
        case eQuarterFlat:
            wxASSERT(false);    //! @todo Not implemented
            break;
        case eQuarterSharp:
            wxASSERT(false);    //! @todo Not implemented
            break;
        case eThreeQuartersFlat:
            wxASSERT(false);    //! @todo Not implemented
            break;
        case eThreeQuartersSharp:
            wxASSERT(false);    //! @todo Not implemented
            break;
        default:
            wxASSERT(false);
    }

    sGlyphs = aGlyphsInfo[nGlyph[0]].GlyphChar;
    if (pVStaff) {
        *pOffset = (lmLUnits)pVStaff->TenthsToLogical( aGlyphsInfo[nGlyph[0]].GlyphOffset , nStaff );
        *pHeight = (lmLUnits)pVStaff->TenthsToLogical( aGlyphsInfo[nGlyph[0]].SelRectHeight , nStaff );
        *pShift = (lmLUnits)pVStaff->TenthsToLogical( aGlyphsInfo[nGlyph[0]].SelRectShift , nStaff );
    }

    if (nGlyph[1] != -1) {
        sGlyphs += aGlyphsInfo[nGlyph[1]].GlyphChar;
        if (pVStaff) {
            lmLUnits nShift2 = (lmLUnits)pVStaff->TenthsToLogical( aGlyphsInfo[nGlyph[1]].SelRectShift , nStaff );
            lmLUnits nHeight2 = (lmLUnits)pVStaff->TenthsToLogical( aGlyphsInfo[nGlyph[1]].SelRectHeight , nStaff );
            *pHeight = wxMax(*pHeight+*pShift, nHeight2+nShift2) - wxMin(*pShift, nShift2);
            *pShift = wxMin(*pShift, nShift2);
       }
    }

    return sGlyphs;

}
