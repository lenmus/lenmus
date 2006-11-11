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
/*! @file NoteRestObj.cpp
    @brief Implementation file for class lmNoteRestObj
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


const wxString CHAR_FERMATA_OVER = _T(";");
const wxString CHAR_FERMATA_UNDER = _T("<");

lmNoteRestObj::lmNoteRestObj(ESymbolType nType, lmNoteRest* pOwner)
    : lmAuxObj(eTPO_Symbol, sbDRAGGABLE)
{
    m_nSymbolType = nType;
    m_pOwner = pOwner;

}
    
void lmNoteRestObj::UpdateMeasurements()
{
    m_paperPos = m_pOwner->GetOrigin();
}


//========================================================================================
// lmFermata object implementation
//========================================================================================

lmFermata::lmFermata(lmNoteRest* pOwner, bool fOverNote) : lmNoteRestObj(eST_Fermata, pOwner)
{
    m_fOverNote = fOverNote;
}

void lmFermata::SetSizePosition(lmPaper* pPaper, lmVStaff* pVStaff, int nStaffNum,
                             lmLUnits xPos, lmLUnits yPos)
{
    /*
    This method does the measurement phase
    */

    m_paperPos = m_pOwner->GetOrigin();

    // prepare DC
    pPaper->SetFont( *(m_pOwner->GetFont()) );

    // prepare glyph and measure it
    wxString sGlyph = (m_fOverNote ? CHAR_FERMATA_OVER : CHAR_FERMATA_UNDER );
    lmLUnits nWidth, nHeight;
    pPaper->GetTextExtent(sGlyph, &nWidth, &nHeight);

    // store glyph position
    m_glyphPos.x = xPos - nWidth/2;
    if (m_fOverNote)
        m_glyphPos.y = yPos - pVStaff->TenthsToLogical( 70, nStaffNum );
    else
        m_glyphPos.y = yPos - pVStaff->TenthsToLogical( 5, nStaffNum );


    // store selection rectangle position and size
    m_selRect.width = nWidth;
    m_selRect.height = pVStaff->TenthsToLogical( 20, nStaffNum );
    m_selRect.x = m_glyphPos.x;
    m_selRect.y = m_glyphPos.y + pVStaff->TenthsToLogical( (m_fOverNote ? 45 : 40), nStaffNum );

}

void lmFermata::DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC, bool fHighlight)
{
    //nxLeft es la coordenada x del centro del calderon
    
    // prepare DC
    pPaper->SetFont( *(m_pOwner->GetFont()) );

    wxString sGlyph = (m_fOverNote ? CHAR_FERMATA_OVER : CHAR_FERMATA_UNDER );
    lmUPoint pos = GetGlyphPosition();
    pPaper->SetTextForeground((m_fSelected ? g_pColors->ScoreSelected() : colorC));
    pPaper->DrawText(sGlyph, pos.x, pos.y );

}


//========================================================================================
// lmLyric object implementation
//========================================================================================
//Global variables used as default initializators
lmFontInfo tLyricDefaultFont = { _T("Arial"), 8, lmTEXT_ITALIC };
lmLocation g_tDefaultPos = {0,0,lmLOCATION_RELATIVE,lmLOCATION_RELATIVE,lmTENTHS,lmTENTHS};

lmLyric::lmLyric(lmNoteRest* pOwner, wxString sText, ESyllabicTypes nSyllabic,
            int nNumLine, wxString sLanguage )
    : lmNoteRestObj(eST_Lyric, pOwner),
      lmBasicText(sText, sLanguage, &g_tDefaultPos, tLyricDefaultFont)
{
    m_nNumLine = nNumLine;
}

void lmLyric::SetFont(lmPaper* pPaper)
{
    //wxLogMessage(wxString::Format(
    //    _T("[lmLyric::SetFont]: size=%d, name=%s"), m_nFontSize, m_sFontName));

    int nWeight = (m_fBold ? wxBOLD : wxNORMAL);
    int nStyle = (m_fItalic ? wxITALIC : wxNORMAL);
    m_pFont = pPaper->GetFont(m_nFontSize, m_sFontName, wxDEFAULT, nStyle, nWeight, false);

    if (!m_pFont) {
        wxMessageBox(_("Sorry, an error has occurred while allocating the font."),
            _T("lmLyric::SetFont"), wxOK);
        ::wxExit();
    }
}

void lmLyric::DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC, bool fHighlight)
{
    wxASSERT(fMeasuring == DO_DRAW);    //measuring pahse is done in SetSizePosition()

    pPaper->SetFont(*m_pFont);

    lmUPoint pos = GetGlyphPosition();
    pPaper->SetTextForeground((m_fSelected ? g_pColors->ScoreSelected() : colorC));
    pPaper->DrawText(m_sText, pos.x, pos.y );

}

void lmLyric::SetOwner(lmNoteRest* pOwner)
{
    wxASSERT(pOwner);
    m_pOwner = pOwner;
    m_pVStaff = pOwner->GetVStaff();
    m_nStaffNum = pOwner->GetStaffNum();
}

void lmLyric::SetSizePosition(lmPaper* pPaper, lmVStaff* pVStaff, int nStaffNum,
                        lmLUnits xPos, lmLUnits yPos)
{
    /*
    This method does the measurement phase
    */

    // save paper position and prepare font
    m_paperPos = m_pOwner->GetOrigin();
    SetFont(pPaper);

    // prepare DC
    pPaper->SetFont(*m_pFont);

    // prepare the text and measure it
    lmLUnits nWidth, nHeight;
    pPaper->GetTextExtent(m_sText, &nWidth, &nHeight);
    m_nWidth = nWidth;

    // store glyph position (relative to paper pos).
    //// Remember: XML positioning values origin is the left-hand side of the note 
    //// or the musical position within the bar (x) and the top line of the staff (y)
    //m_glyphPos.x = m_pVStaff->TenthsToLogical(m_xRel, m_nStaffNum);
    //// as relative-y refers to the top line of the staff, so 5 lines must be 
    //// substracted from yBase position
    //m_glyphPos.y = m_pVStaff->TenthsToLogical(m_yRel-50, m_nStaffNum);
    //if (m_fOverrideDefaultX) {
    //    m_glyphPos.x += m_pVStaff->TenthsToLogical(m_xDef, m_nStaffNum) - m_paperPos.x;
    //}
    //if (m_fOverrideDefaultY) {
    //    m_glyphPos.y += m_pVStaff->TenthsToLogical(m_yDef, m_nStaffNum) - m_paperPos.y;
    //}
    m_glyphPos.x = xPos;
    m_glyphPos.y = yPos + pVStaff->TenthsToLogical( 40, nStaffNum ) * m_nNumLine;

     // store selection rectangle (relative to m_paperPos). Coincides with glyph rectangle
    m_selRect.width = nWidth;
    m_selRect.height = nHeight;
    m_selRect.x = m_glyphPos.x;
    m_selRect.y = m_glyphPos.y;

}

wxString lmLyric::Dump()
{
    wxString sDump = wxString::Format(
        _T("\t-->lmLyric\t%s\tnumLine=%d, paperPos=(%d, %d)\n"),
        m_sText, m_nNumLine, m_paperPos.x, m_paperPos.y);
    return sDump;

}

