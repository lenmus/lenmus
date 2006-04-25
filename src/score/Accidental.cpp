// RCS-ID: $Id: Accidental.cpp,v 1.3 2006/02/23 19:22:56 cecilios Exp $
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

    // prepare glyph and measure it
    wxString sGlyphs = GetAccidentalGlyphs(m_nType);
    lmLUnits nWidth, nHeight;
    pDC->GetTextExtent(sGlyphs, &nWidth, &nHeight);

    // store glyphs position
    m_glyphPos.x = xPos;
    m_glyphPos.y = yPos - pVStaff->TenthsToLogical( 10, nStaffNum );

    // store selection rectangle position and size
    m_selRect.width = nWidth;
    m_selRect.height = (m_nType == eDoubleSharp ?
                            pVStaff->TenthsToLogical( 12, nStaffNum ) :
                            pVStaff->TenthsToLogical( 30, nStaffNum ) );
    m_selRect.x = m_glyphPos.x;
    m_selRect.y = m_glyphPos.y + pVStaff->TenthsToLogical( 45, nStaffNum );

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

//global methods related to accidentals

wxString GetAccidentalGlyphs(EAccidentals nType)
{
    wxString sGlyphs;
    switch(nType) {
        case eNatural:
            sGlyphs = _T("'");
            break;
        case eSharp:
            sGlyphs = _T("#");
            break;
        case eFlat:
            sGlyphs = _T("%");
            break;
        case eFlatFlat:
            sGlyphs = _T("&");
            break;
        case eDoubleSharp:
            sGlyphs = _T("$");
            break;
        case eNaturalFlat:
            sGlyphs = _T("'%");
            break;
        case eNaturalSharp:
            sGlyphs = _T("'#");
            break;
        case eSharpSharp:
            sGlyphs = _T("##");
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
    return sGlyphs;

}
