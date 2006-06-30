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

//Font LeMus : characters to draw clefs
#define CHAR_G_CLEF         _T("A")
#define CHAR_F_CLEF         _T("B")
#define CHAR_C_CLEF         _T("C")
#define CHAR_NO_CLEF        _T("G")


//-------------------------------------------------------------------------------------------------
// lmClef object implementation
//-------------------------------------------------------------------------------------------------

//
//constructors and destructor
//

lmClef::lmClef(EClefType nClefType, lmVStaff* pStaff, int nNumStaff, bool fVisible) :
    lmSimpleObj(eTPO_Clef, pStaff, nNumStaff, fVisible, sbDRAGGABLE)
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
    wxString sGlyph = GetLenMusChar();
    return PrepareBitMap(rScale, sGlyph);

}

//--------------------------------------------------------------------------------------
// get fixed measures and values that depend on key type
//--------------------------------------------------------------------------------------

// returns the y-axis offset from paper cursor position so that shape get correctly
// positioned over a five-lines staff (units: tenths of inter-line space)
lmTenths lmClef::GetGlyphOffset()
{
    lmTenths yOffset;
    switch(m_nClefType)
    {
        case eclvSol: yOffset = -20;    break;
        case eclvFa4: yOffset = -20;    break;
        case eclvFa3: yOffset = -10;    break;
        case eclvDo1: yOffset = -20;    break;
        case eclvDo2: yOffset = -30;    break;
        case eclvDo3: yOffset = -40;    break;
        case eclvDo4: yOffset = -50;    break;
        case eclvPercusion: yOffset =-19; break;
        default:
            yOffset = 0;
            wxASSERT_MSG( false, _T("Invalid value for attribute m_nClefType"));
    }

    return yOffset;

}

// returns the y-axis offset from bitmap rectangle to the selection rectangle origin
// (units: tenths of inter-line space)
lmTenths lmClef::GetSelRectShift()
{
    lmTenths yOffset;
    switch(m_nClefType)
    {
        case eclvSol: yOffset = 5; break;
        case eclvFa4: yOffset = 20; break;
        case eclvFa3: yOffset = 20;    break;
        case eclvDo1: yOffset = 40;    break;
        case eclvDo2: yOffset = 40;    break;
        case eclvDo3: yOffset = 40;    break;
        case eclvDo4: yOffset = 40;    break;
        case eclvPercusion: yOffset = 20; break;
        default:
            yOffset = 0;
            wxASSERT_MSG( false, _T("Invalid value for attribute m_nClefType"));
    }

    return yOffset;

}

// returns the height of the selection rectangle
// units: tenths (tenths of inter-line space)
lmTenths lmClef::GetSelRectHeight()
{
    lmTenths nHeight;
    switch(m_nClefType)
    {
        case eclvSol: nHeight = 73; break;
        case eclvFa4: nHeight = 35; break;
        case eclvFa3: nHeight = 35;    break;
        case eclvDo1: nHeight = 40;    break;
        case eclvDo2: nHeight = 40;    break;
        case eclvDo3: nHeight = 40;    break;
        case eclvDo4: nHeight = 40;    break;
        case eclvPercusion: nHeight = 35; break;
        default:
            nHeight = 100;
            wxASSERT_MSG( false, _T("Invalid value for attribute m_nClefType"));
    }

    return nHeight;

}

wxString lmClef::GetLenMusChar()
{
    // returns the character to use to print the clef (LenMus font)

    wxString sGlyph = CHAR_G_CLEF;
    switch(m_nClefType)
    {
        case eclvSol: sGlyph = CHAR_G_CLEF; break;
        case eclvFa4: sGlyph = CHAR_F_CLEF;    break;
        case eclvFa3: sGlyph = CHAR_F_CLEF; break;
        case eclvDo1: sGlyph = CHAR_C_CLEF; break;
        case eclvDo2: sGlyph = CHAR_C_CLEF; break;
        case eclvDo3: sGlyph = CHAR_C_CLEF; break;
        case eclvDo4: sGlyph = CHAR_C_CLEF; break;
        case eclvPercusion: sGlyph = CHAR_NO_CLEF; break;
        default:
            wxASSERT_MSG( false, _T("Invalid value for attribute m_nClefType"));
    }

    return sGlyph;
}


//-----------------------------------------------------------------------------------------
// implementation of virtual methods defined in base abstract class lmStaffObj
//-----------------------------------------------------------------------------------------

void lmClef::DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC)
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

    if (fMeasuring) {

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
        m_selRect.height = m_pVStaff->TenthsToLogical( GetSelRectHeight(), m_nStaffNum );
        m_selRect.x = m_glyphPos.x;
        m_selRect.y = m_glyphPos.y + m_pVStaff->TenthsToLogical( GetSelRectShift(), m_nStaffNum );

        // set total width (incremented in one line for after space)
        m_nWidth = nWidth + m_pVStaff->TenthsToLogical(10, m_nStaffNum);    //one line space
    }

}

// returns the width of the draw (logical units)
lmLUnits lmClef::DrawClef(bool fMeasuring, lmPaper* pPaper, wxColour colorC)
{    
    wxDC* pDC = pPaper->GetDC();
    wxASSERT(pDC);
    pDC->SetFont(*m_pFont);

    wxString sGlyph = GetLenMusChar();
    if (fMeasuring) {
        lmLUnits width, height;
        pDC->GetTextExtent(sGlyph, &width, &height);
        return width;
    } else {
        wxPoint pos = GetGlyphPosition();
        pDC->SetTextForeground(colorC);
        pDC->DrawText(sGlyph, pos.x, pos.y );
        return 0;
    }

}

lmLUnits lmClef::DrawAt(bool fMeasuring, wxDC* pDC, wxPoint pos, wxColour colorC)
{
    /*
    This method is, primarely, to be used when rendering the prolog
    Returns the width of the draw
    */

    if (fMeasuring) return m_nWidth;

    wxString sGlyph = GetLenMusChar();
    pDC->SetFont(*m_pFont);
    pDC->SetTextForeground(colorC);
    pDC->DrawText(sGlyph, pos.x, pos.y + m_pVStaff->TenthsToLogical( GetGlyphOffset(), m_nStaffNum ) );

    return m_nWidth;
}

void lmClef::MoveDragImage(lmPaper* pPaper, wxDragImage* pDragImage, wxPoint& offsetD, 
                         const wxPoint& pagePosL, const wxPoint& dragStartPosL, const wxPoint& canvasPosD)
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
    wxPoint ptNew = canvasPosD;
    ptNew.y = pPaper->LogicalToDeviceY(m_paperPos.y + m_glyphPos.y) + offsetD.y;
    pDragImage->Move(ptNew);

}

wxPoint lmClef::EndDrag(const wxPoint& pos)
{
    wxPoint oldPos(m_paperPos + m_glyphPos);

    //Only X pos. can be changed
    m_paperPos.x = pos.x - m_glyphPos.x;

    return wxPoint(oldPos);
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


