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
/*! @file Clef.cpp
    @brief Implementation file for class lmClef
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

#include "wx/image.h"
#include "Score.h"


//-------------------------------------------------------------------------------------------------
// lmClef object implementation
//-------------------------------------------------------------------------------------------------

//
//constructors and destructor
//

lmClef::lmClef(EClefType nClefType, lmVStaff* pStaff, int nNumStaff, bool fVisible) :
    lmSimpleObj(eTPO_Clef, pStaff, nNumStaff, fVisible, lmDRAGGABLE)
{
    m_nClefType = nClefType;
    m_fHidden = false;
}

// Create the drag image.
// Under wxGTK the DC logical function (ROP) is used by DrawText() but it is ignored by
// wxMSW. Thus, it is not posible to do dragging just by redrawing the lmStaffObj using ROPs.
// For portability it is necessary to implement dragging by means of bitmaps and wxDragImage
wxBitmap* lmClef::GetBitmap(double rScale)
{
    lmEGlyphIndex nGlyph = GetGlyphIndex();
    wxString sGlyph( aGlyphsInfo[nGlyph].GlyphChar );
    return PrepareBitMap(rScale, sGlyph);

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
        case eclvFa3: yOffset += 10;    break;
        case eclvFa5: yOffset -= 10;    break;
        case eclvDo1: yOffset += 20;    break;
        case eclvDo2: yOffset += 10;    break;
        case eclvDo4: yOffset -= 10;    break;
        case eclvDo5: yOffset -= 20;    break;
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
        case eclvSol: return GLYPH_G_CLEF;
        case eclvFa4: return GLYPH_F_CLEF;
        case eclvFa3: return GLYPH_F_CLEF;
        case eclvDo1: return GLYPH_C_CLEF;
        case eclvDo2: return GLYPH_C_CLEF;
        case eclvDo3: return GLYPH_C_CLEF;
        case eclvDo4: return GLYPH_C_CLEF;
        case eclvPercussion: return GLYPH_PERCUSSION_CLEF_BLOCK;
        default:
            wxASSERT_MSG( false, _T("Invalid value for attribute m_nClefType"));
            return GLYPH_G_CLEF;
    }

}



//-----------------------------------------------------------------------------------------
// implementation of virtual methods defined in base abstract class lmStaffObj
//-----------------------------------------------------------------------------------------

void lmClef::DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC, bool fHighlight)
{
    /*
    This method is invoked by the base class (lmStaffObj). When reaching this point 
    paper cursor variable (m_paperPos) has been updated. This value must be used
    as the base for any measurement / drawing operation.

    DrawObject() method is responsible for:
    1. In DO_MEASURE phase (fMeasuring == true):
        - Compute the surrounding rectangle, the glyph position and other measurements
    2. In DO_DRAW phase (fMeasuring == false):
        - Render the object

    */

    if (!fMeasuring && m_fHidden) return;

    lmEGlyphIndex nGlyph;
    if (fMeasuring)
    {
        nGlyph = GetGlyphIndex();

        // get the shift to the staff on which the clef must be drawn
        lmLUnits yShift = m_pVStaff->GetStaffOffset(m_nStaffNum);

        // store glyph position
        m_glyphPos.x = 0;
        m_glyphPos.y = yShift + m_pVStaff->TenthsToLogical( GetGlyphOffset(), m_nStaffNum );
    }

    lmLUnits nWidth = DrawClef(fMeasuring, pPaper,
        (m_fSelected ? g_pColors->ScoreSelected() : g_pColors->ScoreNormal() ));

    if (fMeasuring) {
        // store selection rectangle measures and position (relative to m_paperPos)
        m_selRect.width = nWidth;
        m_selRect.height = m_pVStaff->TenthsToLogical(
                            aGlyphsInfo[nGlyph].SelRectHeight, m_nStaffNum );
        m_selRect.x = m_glyphPos.x;
        m_selRect.y = m_glyphPos.y + m_pVStaff->TenthsToLogical(
                            aGlyphsInfo[nGlyph].SelRectShift, m_nStaffNum );

        // set total width (incremented in one line for after space)
        m_nWidth = nWidth + m_pVStaff->TenthsToLogical(10, m_nStaffNum);    //one line space
    }

}

// returns the width of the draw (logical units)
lmLUnits lmClef::DrawClef(bool fMeasuring, lmPaper* pPaper, wxColour colorC)
{    
    pPaper->SetFont(*m_pFont);

    lmEGlyphIndex nGlyph = GetGlyphIndex();
    wxString sGlyph( aGlyphsInfo[nGlyph].GlyphChar );
    if (fMeasuring) {
        lmLUnits width, height;
        pPaper->GetTextExtent(sGlyph, &width, &height);
        return width;
    } else {
        //wxLogMessage(_T("[lmClef::DrawClef]"));
        lmUPoint pos = GetGlyphPosition();
        pPaper->SetTextForeground(colorC);
        pPaper->DrawText(sGlyph, pos.x, pos.y );
        return 0;
    }

}

lmLUnits lmClef::DrawAt(bool fMeasuring, lmPaper* pPaper, lmUPoint pos, wxColour colorC)
{
    // This method is, primarely, to be used when rendering the prolog
    // Returns the width of the draw

    if (fMeasuring) return m_nWidth;

    lmEGlyphIndex nGlyph = GetGlyphIndex();
    wxString sGlyph( aGlyphsInfo[nGlyph].GlyphChar );
    pPaper->SetFont(*m_pFont);
    pPaper->SetTextForeground(colorC);
    pPaper->DrawText(sGlyph, pos.x, pos.y + m_pVStaff->TenthsToLogical( GetGlyphOffset(), m_nStaffNum ) );

    return m_nWidth;
}

void lmClef::MoveDragImage(lmPaper* pPaper, wxDragImage* pDragImage, lmDPoint& offsetD, 
                         const lmUPoint& pagePosL, const lmUPoint& dragStartPosL, const lmDPoint& canvasPosD)
{
    // DragImage->Move() requires device units referred to canvas window. To compute the
    // desired position the following coordinates are received:
    // - canvasPosD - current mouse position (device units referred to canvas window). If the 
    //        image movement is not constrained, this is the rigth value for DragImage->Move(). See
    //        default method in StaffObj.h
    // - offsetD - offset to add when translating from logical units referred to page origin to 
    //        scrolled device units referred to canvas origin. It takes also into account the 
    //        offset introduced by the hotSpot point.
    // - pagePosL - current mouse position (logical units referred to page origin).

    //pDragImage->Move(canvasPosD);

    // A clef only can be moved horizonatlly
    lmDPoint ptNew = canvasPosD;
    ptNew.y = pPaper->LogicalToDeviceY(m_paperPos.y + m_glyphPos.y) + offsetD.y;
    pDragImage->Move(ptNew);

}

lmUPoint lmClef::EndDrag(const lmUPoint& pos)
{
    lmUPoint oldPos(m_paperPos + m_glyphPos);

    //Only X pos. can be changed
    m_paperPos.x = pos.x - m_glyphPos.x;

    return lmUPoint(oldPos);
}

wxString lmClef::Dump()
{
    wxString sDump = wxString::Format(
        _T("%d\tClef %s\tTimePos=%.2f\n"),
        m_nId, GetClefLDPNameFromType(m_nClefType), m_rTimePos );
    return sDump;
            
}

wxString lmClef::SourceLDP()
{
    wxString sSource = _T("         (clef ");
    sSource += GetClefLDPNameFromType(m_nClefType);

    //staff num
    if (m_pVStaff->GetNumStaves() > 1) {
        sSource += wxString::Format(_T(" p%d"), m_nStaffNum);
    }

    if (!m_fVisible) { sSource += _T(" no_visible"); }
    sSource += _T(")\n");
    return sSource;
}

wxString lmClef::SourceXML()
{
    wxString sSource = _T("TODO: lmClef XML Source code generation methods");
    return sSource;
}

//
// global functions related to clefs
//------------------------------------------------------------------------------------------

wxString GetClefLDPNameFromType(EClefType nType)
{
    static bool fNamesLoaded = false;
    static wxString sName[9];
    
    wxASSERT(nType < 9);
    if (!fNamesLoaded) {
        sName[0] = _T("error");
        sName[1] = _T("Sol");
        sName[2] = _T("Fa4");
        sName[3] = _T("Fa3");
        sName[4] = _T("Do1");
        sName[5] = _T("Do2");
        sName[6] = _T("Do3");
        sName[7] = _T("Do4");
        sName[8] = _T("Sin clave");
        fNamesLoaded = true;
    }
    
    return sName[nType];
    
}


