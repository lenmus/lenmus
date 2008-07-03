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
#include "Staff.h"
#include "VStaff.h"
#include "../graphic/Shapes.h"


//========================================================================================
// lmAccidental object implementation
//========================================================================================

lmAccidental::lmAccidental(lmNote* pOwner, lmEAccidentals nType)
{
	m_pOwner = pOwner;
    m_nType = nType;
	m_pStaff = pOwner->GetVStaff()->GetStaff(pOwner->GetStaffNum());
	//AWARE: Althoug shape pointer is initialized to NULL never assume that there is
	//a shape if not NULL, as the shape is deleted in the graphical model.
	m_pShape = (lmShape*)NULL;

	//set up the after space
    #define ACCIDENTALS_AFTERSPACE  3      //in tenths   TODO user options
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
        case lm_eSharp:
            nGlyph[0] = GLYPH_SHARP_ACCIDENTAL;
            break;
        case lm_eFlat:
            nGlyph[0] = GLYPH_FLAT_ACCIDENTAL;
            break;
        case lm_eFlatFlat:
            nGlyph[0] = GLYPH_DOUBLE_FLAT_ACCIDENTAL;
            break;
        case lm_eDoubleSharp:
            nGlyph[0] = GLYPH_DOUBLE_SHARP_ACCIDENTAL;
            break;
        case lm_eNaturalFlat:
            nGlyph[0] = GLYPH_NATURAL_ACCIDENTAL;
            nGlyph[1] = GLYPH_FLAT_ACCIDENTAL;
            break;
        case lm_eNaturalSharp:
            nGlyph[0] = GLYPH_NATURAL_ACCIDENTAL;
            nGlyph[1] = GLYPH_SHARP_ACCIDENTAL;
            break;
        case lm_eSharpSharp:
            nGlyph[0] = GLYPH_SHARP_ACCIDENTAL;
            nGlyph[1] = GLYPH_SHARP_ACCIDENTAL;
            break;
        case lm_eQuarterFlat:
            wxASSERT(false);    //TODO Not implemented
            break;
        case lm_eQuarterSharp:
            wxASSERT(false);    //TODO Not implemented
            break;
        case lm_eThreeQuartersFlat:
            wxASSERT(false);    //TODO Not implemented
            break;
        case lm_eThreeQuartersSharp:
            wxASSERT(false);    //TODO Not implemented
            break;
        default:
            wxASSERT(false);
    }

    wxFont* pFont = m_pOwner->GetSuitableFont(pPaper);

	//if two shapes, create a composite shape, else it is enough with one shape
	if (nGlyph[1] != -1)
	{
		//two shapes
        lmLUnits yPos = uyPos - m_pStaff->TenthsToLogical(aGlyphsInfo[nGlyph[0]].GlyphOffset);
		m_pShape = new lmCompositeShape(m_pOwner, 0, _T("Note accidentals"));
		lmShapeGlyph* pSh1 = new lmShapeGlyph(m_pOwner, -1, nGlyph[0], pFont, pPaper,
										      lmUPoint(uxPos, yPos), _T("Accidental"));
		((lmCompositeShape*)m_pShape)->Add(pSh1);
        lmLUnits uWidth = pSh1->GetWidth();
        yPos = uyPos - m_pStaff->TenthsToLogical(aGlyphsInfo[nGlyph[1]].GlyphOffset);
        ((lmCompositeShape*)m_pShape)->Add(
				new lmShapeGlyph(m_pOwner, -1, nGlyph[1], pFont, pPaper,
								lmUPoint(uxPos+uWidth, yPos), _T("Accidental")) );
	}
	else
	{
        lmLUnits yPos = uyPos - m_pStaff->TenthsToLogical(aGlyphsInfo[nGlyph[0]].GlyphOffset);
        m_pShape = new lmShapeGlyph(m_pOwner, 0, nGlyph[0], pFont, pPaper,
									lmUPoint(uxPos, yPos), _T("Accidental"));
	}

}

wxString lmAccidental::GetLDPEncoding()
{
    switch(m_nType) {
        case eNatural:		return _T("");
        case lm_eSharp:		return _T("+");
        case lm_eFlat:			return _T("-");
        case lm_eFlatFlat:		return _T("--");
        case lm_eDoubleSharp:	return _T("x");
        case lm_eNaturalFlat:	return _T("=-");
        case lm_eNaturalSharp:	return _T("=+");
        case lm_eSharpSharp:	return _T("++");

        case lm_eQuarterFlat:
            wxASSERT(false);    //TODO Not implemented
            break;
        case lm_eQuarterSharp:
            wxASSERT(false);    //TODO Not implemented
            break;
        case lm_eThreeQuartersFlat:
            wxASSERT(false);    //TODO Not implemented
            break;
        case lm_eThreeQuartersSharp:
            wxASSERT(false);    //TODO Not implemented
            break;
        default:
            wxASSERT(false);
    }
	return _T("");
}
