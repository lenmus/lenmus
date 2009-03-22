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
#pragma implementation "Rest.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/debug.h"
#include "wx/image.h"

#include "Score.h"
#include "VStaff.h"
#include "../ldp_parser/AuxString.h"
#include "../graphic/Shapes.h"
#include "../graphic/ShapeRest.h"
#include "Glyph.h"


lmRest::lmRest(lmVStaff* pVStaff, lmENoteType nNoteType, float rDuration, int nNumDots,
        int nStaff, int nVoice, bool fVisible, bool fBeamed, lmTBeamInfo BeamInfo[])
    : lmNoteRest(pVStaff, lmDEFINE_REST, nNoteType, rDuration, nNumDots,
                 nStaff, nVoice, fVisible)
{
    CreateBeam(fBeamed, BeamInfo);
}

lmRest::~lmRest()
{
}


//--------------------------------------------------------------------------------------
// get glyph data to define character to use and selection rectangle
//--------------------------------------------------------------------------------------

lmEGlyphIndex lmRest::GetGlyphIndex()
{
    // returns the index (over global glyphs table) to the character to use to print
    // the rest (LenMus font)

    switch (m_nNoteType) {
        case eLonga:        return GLYPH_LONGA_REST;
        case eBreve:        return GLYPH_BREVE_REST;
        case eWhole:        return GLYPH_WHOLE_REST;
        case eHalf:         return GLYPH_HALF_REST;
        case eQuarter:      return GLYPH_QUARTER_REST;
        case eEighth:       return GLYPH_EIGHTH_REST;
        case e16th:         return GLYPH_16TH_REST;
        case e32th:         return GLYPH_32ND_REST;
        case e64th:         return GLYPH_64TH_REST;
        case e128th:        return GLYPH_128TH_REST;
        case e256th:        return GLYPH_256TH_REST;
        default:
            wxASSERT(false);
            return GLYPH_QUARTER_REST;
    }

}

//====================================================================================================
// implementation of virtual methods defined in base abstract class lmNoteRest
//====================================================================================================

lmLUnits lmRest::LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)
{
    // This method is invoked by the base class (lmStaffObj). It is responsible for
    // creating the shape object and adding it to the graphical model.
    // Paper cursor must be used as the base for positioning.

    if (lmPRESERVE_SHAPES && !IsDirty())
    {
        //Not dirty: just add existing shapes to the Box
        lmShape* pOldShape = this->GetShape();
        pBox->AddShape(pOldShape);
        pOldShape->SetColour(*wxCYAN);//colorC);       //change its colour to new desired colour
    }
    else
    {
        //Dirty: create new shapes for this object

        //get paper reference point
        lmUPoint uPaperPos(pPaper->GetCursorX(), pPaper->GetCursorY());

        // move to right staff
        lmLUnits uyTop = uPaperPos.y + GetStaffOffset();
        lmLUnits uxLeft = uPaperPos.x;

        //// prepare DC
        //pPaper->SetFont(*GetSuitableFont(pPaper));

        //create the container shape and add it to the box
        lmShapeRest* pRestShape = new lmShapeRest(this, colorC, lmDRAGGABLE, m_fVisible);
	    pBox->AddShape(pRestShape);
        StoreShape(pRestShape);

        // create shape for the rest symbol
        lmEGlyphIndex nGlyph = GetGlyphIndex();
        lmLUnits yPos = uyTop + m_pVStaff->TenthsToLogical( aGlyphsInfo[nGlyph].GlyphOffset , m_nStaffNum );
        lmShapeGlyph* pShape = new lmShapeGlyph(this, -1, nGlyph, pPaper, lmUPoint(uxLeft, yPos),
                                                _T("RestGlyph"), lmDRAGGABLE, colorC);
	    pRestShape->Add(pShape);
        uxLeft += pShape->GetWidth();

        //create shapes for dots if necessary
        //------------------------------------------------------------
        if (m_nNumDots > 0)
        {
            //TODO user selectable
            lmLUnits uSpaceBeforeDot = m_pVStaff->TenthsToLogical(5, m_nStaffNum);
            lmLUnits uyPos = yPos - m_pVStaff->TenthsToLogical(GetDotShift(), m_nStaffNum);
            for (int i = 0; i < m_nNumDots; i++)
            {
                uxLeft += uSpaceBeforeDot;
                uxLeft += AddDotShape(pRestShape, pPaper, uxLeft, uyPos, colorC);
            }
        }

	    // if rest in a beam, link the rest shape to the beam shape
	    if (m_pBeam)
		    m_pBeam->AddRestShape(pRestShape);
    }

	return GetShape()->GetWidth();
}

void lmRest::PlaybackHighlight(wxDC* pDC, wxColour colorC)
{
	//FIX_ME: there can be many views. Should only the active view be higlighted?
	//FIX_ME: m_pShape is only valid during layout. And there can be many views!!

	GetShape()->RenderHighlighted(pDC, colorC);
}

wxString lmRest::Dump()
{
    wxString sDump = wxString::Format(
        _T("%d\tRest\tType=%d, TimePos=%.2f, rDuration=%.2f, voice=%d "),
        m_nId, m_nNoteType, m_rTimePos, m_rDuration, m_nVoice );

	//base class info
	sDump += lmNoteRest::Dump();
    sDump += _T("\n");

    return sDump;
}

wxString lmRest::SourceLDP(int nIndent)
{
    wxString sSource = _T("");
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += _T("(r ");

    //duration
    sSource += GetLDPNoteType();
    for (int i=0; i < m_nNumDots; i++)
        sSource += _T(".");

	//base class
	sSource += lmNoteRest::SourceLDP(nIndent);

    //close element
    sSource += _T(")\n");
	return sSource;
}

wxString lmRest::SourceXML(int nIndent)
{
	wxString sSource = _T("");
	sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
    sSource += _T("TODO: lmRest XML Source code generation method\n");

	//base class
	sSource += lmNoteRest::SourceXML(nIndent);

    return sSource;
}

lmLUnits lmRest::GetDotShift()
{
    //returns needed shift (move upwards) to align it as required for each rest glyph

    switch (m_nNoteType) {
        case eLonga:        return 5.0f;         //half line
        case eBreve:        return 15.0f;
        case eWhole:        return 15.0f;
        case eHalf:         return 5.0f;
        case eQuarter:      return 5.0f;
        case eEighth:       return 5.0f;
        case e16th:         return 5.0f;
        case e32th:         return 5.0f;
        case e64th:         return 5.0f;
        case e128th:        return 5.0f;
        case e256th:        return 5.0f;
        default:
            wxLogMessage(_T("[lmRest::GetDotShift] Invalid value for m_nNoteType (%d)"), m_nNoteType);
            wxASSERT(false);
            return 5.0f;
    }
}
