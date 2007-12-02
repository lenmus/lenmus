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

#ifdef __GNUG__
#pragma implementation "Accidental.h"
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

lmAccidental::lmAccidental(lmNote* pOwner, EAccidentals nType)
{
	m_pOwner = pOwner;
    m_nType = nType;
	m_pStaff = pOwner->GetVStaff()->GetStaff(pOwner->GetStaffNum());
	//AWARE: Althoug shape pointer is initialized to NULL never assume that there is
	//a shape if not NULL, as the shape is deleted in the graphic model.
	m_pShape = (lmShape*)NULL;

	//set up the after space
    #define ACCIDENTALS_AFTERSPACE  3      //in tenths   @todo user options
    m_uAfterSpace = m_pStaff->TenthsToLogical(ACCIDENTALS_AFTERSPACE);
}

lmAccidental::~lmAccidental()
{
}

void lmAccidental::Layout(lmPaper* pPaper, lmLUnits uxPos, lmLUnits uyPos)
{
	CreateShapes(pPaper, uxPos, uyPos);
	m_uxPos = uxPos;
	m_uyPos = uyPos;

}

void lmAccidental::MoveTo(lmLUnits uxPos, lmLUnits uyPos)
{
	wxASSERT(m_pShape);
	//Shift shapes to new position
	m_pShape->Shift(m_uxPos - uxPos, m_uyPos - uyPos);
}

lmLUnits lmAccidental::GetWidth()
{
    return GetShape()->GetWidth() + m_uAfterSpace;

}

void lmAccidental::CreateShapes(lmPaper* pPaper, lmLUnits uxPos, lmLUnits uyPos)
{
    wxString sGlyphs;
    int nGlyph[2] = { -1, -1};
    switch(m_nType) {
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
            wxASSERT(false);    //TODO Not implemented
            break;
        case eQuarterSharp:
            wxASSERT(false);    //TODO Not implemented
            break;
        case eThreeQuartersFlat:
            wxASSERT(false);    //TODO Not implemented
            break;
        case eThreeQuartersSharp:
            wxASSERT(false);    //TODO Not implemented
            break;
        default:
            wxASSERT(false);
    }

    wxFont* pFont = m_pOwner->GetFont();

	//if two shapes, create a composite shape, else it is enough with one shape
	if (nGlyph[1] != -1)
	{
		//two shapes
        lmLUnits yPos = uyPos - m_pStaff->TenthsToLogical(aGlyphsInfo[nGlyph[0]].GlyphOffset);
		m_pShape = new lmCompositeShape(m_pOwner, _T("Note accidentals"));
		lmShapeGlyph* pSh1 = new lmShapeGlyph(m_pOwner, nGlyph[0], pFont, pPaper,
										      lmUPoint(uxPos, yPos), _T("Accidental"));
		((lmCompositeShape*)m_pShape)->Add(pSh1);
        lmLUnits uWidth = pSh1->GetWidth();
        yPos = uyPos - m_pStaff->TenthsToLogical(aGlyphsInfo[nGlyph[1]].GlyphOffset);
        ((lmCompositeShape*)m_pShape)->Add(
				new lmShapeGlyph(m_pOwner, nGlyph[1], pFont, pPaper,
								lmUPoint(uxPos+uWidth, yPos), _T("Accidental")) );
	}
	else
	{
        lmLUnits yPos = uyPos - m_pStaff->TenthsToLogical(aGlyphsInfo[nGlyph[0]].GlyphOffset);
        m_pShape = new lmShapeGlyph(m_pOwner, nGlyph[0], pFont, pPaper,
									lmUPoint(uxPos, yPos), _T("Accidental"));
	}

}

wxString lmAccidental::GetLDPEncoding()
{
    switch(m_nType) {
        case eNatural:		return _T("");
        case eSharp:		return _T("+");
        case eFlat:			return _T("-");
        case eFlatFlat:		return _T("--");
        case eDoubleSharp:	return _T("x");
        case eNaturalFlat:	return _T("=-");
        case eNaturalSharp:	return _T("=+");
        case eSharpSharp:	return _T("++");

        case eQuarterFlat:
            wxASSERT(false);    //TODO Not implemented
            break;
        case eQuarterSharp:
            wxASSERT(false);    //TODO Not implemented
            break;
        case eThreeQuartersFlat:
            wxASSERT(false);    //TODO Not implemented
            break;
        case eThreeQuartersSharp:
            wxASSERT(false);    //TODO Not implemented
            break;
        default:
            wxASSERT(false);
    }
	return _T("");
}
