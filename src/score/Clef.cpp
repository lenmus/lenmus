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
#include "VStaff.h"
#include "Staff.h"
#include "Context.h"


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
    m_pContext = (lmContext*)NULL;
    
    DefineAsMultiShaped();      //define clef as multi-shaped ScoreObj
}

lmClef::~lmClef()
{
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

lmUPoint lmClef::ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper)
{
	// if no location is specified in LDP source file, this method is invoked from
	// base class to ask derived object to compute a suitable position to
	// place itself.
	// uOrg is the assigned paper position for this object.

	lmUPoint uPos = uOrg;

	// get the shift to the staff on which the clef must be drawn
	uPos.y += m_pVStaff->GetStaffOffset(m_nStaffNum);

	return uPos;
}

lmLUnits lmClef::LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)
{
    // This method is invoked by the base class (lmStaffObj). It is responsible for
    // creating the shape object and adding it to the graphical model. 
    // Paper cursor must be used as the base for positioning.

    //create the shape object
    lmShape* pShape = CreateShape(pBox, pPaper, uPos, m_color);

	// set total width (incremented in one line for after space)
	lmLUnits nWidth = pShape->GetWidth();
	return nWidth + m_pVStaff->TenthsToLogical(10, m_nStaffNum);    //one line space
}

lmShape* lmClef::CreateShape(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)
{
    int nIdx = NewShapeIndex();
    if (!m_fVisible)
        return CreateInvisibleShape(pBox, uPos, nIdx);

    // get the shift to the staff on which the clef must be drawn
	lmLUnits yPos = uPos.y;
    yPos += m_pVStaff->TenthsToLogical( GetGlyphOffset(), m_nStaffNum );

    //create the shape object
    lmShapeClef* pShape = new lmShapeClef(this, nIdx, GetGlyphIndex(), GetSuitableFont(pPaper),
										  pPaper, lmUPoint(uPos.x, yPos), 
										  _T("Clef"), lmDRAGGABLE, colorC);
    StoreShape(pShape);
	pBox->AddShape(pShape);
    return pShape;
}

wxString lmClef::Dump()
{
    wxString sDump = wxString::Format(
        _T("%d\tClef %s\tTimePos=%.2f"),
        m_nId, GetClefLDPNameFromType(m_nClefType).c_str(), m_rTimePos);

    //base class
    sDump += lmStaffObj::Dump();
    sDump += _T("\n");

    //contexts
    int nIndent = 5;
    if (m_pContext)
        sDump += m_pContext->DumpContext(nIndent);
    else
    {
        sDump.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
        sDump += _T("Context: NULL\n");
    }
    sDump += _T("\n");

    return sDump;
}

wxString lmClef::SourceLDP(int nIndent)
{
    wxString sSource = _T("");
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += _T("(clef ");
    sSource += GetClefLDPNameFromType(m_nClefType);

	//base class
	sSource += lmStaffObj::SourceLDP(nIndent);

    //close element
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

void lmClef::RemoveCreatedContexts()
{
    lmStaff* pStaff = m_pVStaff->GetStaff( m_nStaffNum );
    pStaff->RemoveContext(m_pContext, this);
    delete m_pContext;
    m_pContext = (lmContext*)NULL;
}


//------------------------------------------------------------------------------------------
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
        _T("G+8va"),    //8 above
        _T("G-8va"),    //8 below
        _T("F+8va"),    //8 above
        _T("F-8va"),    //8 below
        _T("G+15ma"),   //15 above
        _T("G-15ma"),   //15 below
        _T("F+15ma"),   //15 above
        _T("F-15ma"),   //15 below
    };

    //TODO: Not yet included in LDP

    wxASSERT(nType <= lmE_Sol1);
    return sName[nType];

}


