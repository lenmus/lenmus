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
#pragma implementation "Clef.h"
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


//-------------------------------------------------------------------------------------------------
// lmClef object implementation
//-------------------------------------------------------------------------------------------------

//
//constructors and destructor
//

lmClef::lmClef(lmEClefType nClefType, lmVStaff* pStaff, int nNumStaff, bool fVisible,
			   wxColour colorC) :
    lmStaffObj(pStaff, eSFOT_Clef, pStaff, nNumStaff, fVisible, lmDRAGGABLE)
{
    m_nClefType = nClefType;
    m_fHidden = false;
	m_color = colorC;
}

//--------------------------------------------------------------------------------------
// get fixed measures and values that depend on key type
//--------------------------------------------------------------------------------------

// returns the y-axis offset from paper cursor position so that shape get correctly
// positioned over a five-lines staff (units: tenths of inter-line space)
lmTenths lmClef::GetGlyphOffset()
{
    lmEGlyphIndex nGlyph = GetGlyphIndex();
    lmTenths yOffset = aGlyphsInfo[nGlyph].GlyphOffset;

    //add offset to move the clef up/down the required lines
    switch(m_nClefType)
    {
        case lmE_Fa3: yOffset += 10;    break;
        case lmE_Fa5: yOffset -= 10;    break;
        case lmE_Do1: yOffset += 20;    break;
        case lmE_Do2: yOffset += 10;    break;
        case lmE_Do4: yOffset -= 10;    break;
        case lmE_Do5: yOffset -= 20;    break;
        default:
            ;
    }

    return yOffset;

}

lmEGlyphIndex lmClef::GetGlyphIndex()
{
    // returns the index (over global glyphs table) to the character to use to print
    // the clef (LenMus font)

    switch (m_nClefType) {
        case lmE_Sol: return GLYPH_G_CLEF;
        case lmE_Fa4: return GLYPH_F_CLEF;
        case lmE_Fa3: return GLYPH_F_CLEF;
        case lmE_Do1: return GLYPH_C_CLEF;
        case lmE_Do2: return GLYPH_C_CLEF;
        case lmE_Do3: return GLYPH_C_CLEF;
        case lmE_Do4: return GLYPH_C_CLEF;
        case lmE_Percussion: return GLYPH_PERCUSSION_CLEF_BLOCK;
        default:
            wxASSERT_MSG( false, _T("Invalid value for attribute m_nClefType"));
            return GLYPH_G_CLEF;
    }

}



//-----------------------------------------------------------------------------------------
// implementation of virtual methods defined in base abstract class lmStaffObj
//-----------------------------------------------------------------------------------------

lmLUnits lmClef::ComputeXLocation(lmPaper* pPaper)
{
	return pPaper->GetCursorX();
}

lmLUnits lmClef::ComputeYLocation(lmPaper* pPaper)
{
	// get the shift to the staff on which the clef must be drawn
	return pPaper->GetCursorY() + m_pVStaff->GetStaffOffset(m_nStaffNum) +
				m_pVStaff->TenthsToLogical( GetGlyphOffset(), m_nStaffNum );
}

lmLUnits lmClef::LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)
{
    // This method is invoked by the base class (lmStaffObj). It is responsible for
    // creating the shape object and adding it to the graphical model. 
    // Paper cursor must be used as the base for positioning.

    //create the shape object
    lmShapeClef* pShape = new lmShapeClef(this, GetGlyphIndex(), GetSuitableFont(pPaper),
										  pPaper, uPos, _T("Clef"), lmDRAGGABLE, m_color);
	pBox->AddShape(pShape);
    m_pShape2 = pShape;

	// set total width (incremented in one line for after space)
	lmLUnits nWidth = pShape->GetWidth();
	return nWidth + m_pVStaff->TenthsToLogical(10, m_nStaffNum);    //one line space

}

lmLUnits lmClef::AddShape(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos,
					  wxColour colorC)
{
    // This method is, primarely, to be used when rendering the prolog
    // Returns the width of the draw


    // get the shift to the staff on which the clef must be drawn
	lmLUnits yPos = uPos.y;	// + m_pVStaff->GetStaffOffset(m_nStaffNum);
    yPos += m_pVStaff->TenthsToLogical( GetGlyphOffset(), m_nStaffNum );

    //create the shape object
    //lmShapeGlyph* pShape = new lmShapeGlyph(this, GetGlyphIndex(), GetSuitableFont(pPaper), pPaper,
    //                                        lmUPoint(uPos.x, yPos), _T("Clef"));
    lmShapeClef* pShape = new lmShapeClef(this, GetGlyphIndex(), GetSuitableFont(pPaper),
										  pPaper, lmUPoint(uPos.x, yPos), 
										  _T("Clef"), lmDRAGGABLE);
	pBox->AddShape(pShape);
    return pShape->GetWidth();
}

wxString lmClef::Dump()
{
    wxString sDump = wxString::Format(
        _T("%d\tClef %s\tTimePos=%.2f\n"),
        m_nId, GetClefLDPNameFromType(m_nClefType).c_str(), m_rTimePos );
    return sDump;

}

wxString lmClef::SourceLDP(int nIndent)
{
    wxString sSource = _T("");
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += _T("(clef ");
    sSource += GetClefLDPNameFromType(m_nClefType);

    //staff num
    if (m_pVStaff->GetNumStaves() > 1) {
        sSource += wxString::Format(_T(" p%d"), m_nStaffNum);
    }
    
    //visible?
    if (!m_fVisible) { sSource += _T(" noVisible"); }

    //location
    sSource += SourceLDP_Location(m_uPaperPos);

	//attached AuxObjs
	sSource += lmStaffObj::SourceLDP(nIndent+1);

    sSource += _T(")\n");
    return sSource;
}

wxString lmClef::SourceXML(int nIndent)
{
	wxString sSource = _T("");
	sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
    sSource += _T("TODO: lmClef XML Source code generation method\n");
    return sSource;
}

//
// global functions related to clefs
//------------------------------------------------------------------------------------------

wxString GetClefLDPNameFromType(lmEClefType nType)
{
    //AWARE: indexes in correspondence with enum lmEClefType
    static wxString sName[] = {
        _T("Undefined"),
        _T("G"),
        _T("F"),
        _T("F3"),
        _T("C1"),
        _T("C2"),
        _T("C3"),
        _T("C4"),
        _T("percussion"),
        _T("C5"),
        _T("F5"),
        _T("G1"),
    };

    //TODO: Not yet included in LDP
    //lmE_8Sol,       //8 above
    //lmE_Sol8,       //8 below
    //lmE_8Fa,        //8 above
    //lmE_Fa8,        //8 below

    wxASSERT(nType <= lmE_Sol1);
    return sName[nType];

}


