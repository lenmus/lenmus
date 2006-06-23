// RCS-ID: $Id: Direction.cpp,v 1.3 2006/02/23 19:22:56 cecilios Exp $
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
/*! @file Direction.cpp
    @brief Implementation file for class lmWordsDirection
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

#include "Direction.h"


lmWordsDirection::lmWordsDirection(lmVStaff* pVStaff, wxString sText, wxString sLanguage,
                   lmXMLPosition oPos, lmFontInfo oFontData) 
    : lmSimpleObj(eTPO_WordsDirection, pVStaff, 1, true, sbDRAGGABLE),
      lmBasicText(sText, sLanguage, oPos, oFontData)
{
}


//-----------------------------------------------------------------------------------------
// implementation of virtual methods defined in base abstract class lmStaffObj
//-----------------------------------------------------------------------------------------

wxBitmap* lmWordsDirection::GetBitmap(double rScale)
{
    return PrepareBitMap(rScale, m_sText);
}

void lmWordsDirection::DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC)
{
    wxDC* pDC = pPaper->GetDC();
    wxASSERT(pDC);
    pDC->SetFont(*m_pFont);

    if (fMeasuring) {
        lmLUnits nWidth, nHeight;
        pDC->GetTextExtent(m_sText, &nWidth, &nHeight);

        // set total width
        m_nWidth = nWidth;

        // store glyph position (relative to paper pos).
        // Remember: XML positioning values origin is the left-hand side of the note 
        // or the musical position within the bar (x) and the top line of the staff (y)
        m_glyphPos.x = m_pVStaff->TenthsToLogical(m_xRel, m_nStaffNum);
        // as relative-y refers to the top line of the staff, so 5 lines must be 
        // substracted from yBase position
        m_glyphPos.y = m_pVStaff->TenthsToLogical(m_yRel-50, m_nStaffNum);
        if (m_fOverrideDefaultX) {
            m_glyphPos.x += m_pVStaff->TenthsToLogical(m_xDef, m_nStaffNum) - m_paperPos.x;
        }
        if (m_fOverrideDefaultY) {
            m_glyphPos.y += m_pVStaff->TenthsToLogical(m_yDef, m_nStaffNum) - m_paperPos.y;
        }

         // store selection rectangle (relative to m_paperPos). Coincides with glyph rectangle
        m_selRect.width = nWidth;
        m_selRect.height = nHeight;
        m_selRect.x = m_glyphPos.x;
        m_selRect.y = m_glyphPos.y;

    }
    else {
        wxPoint pos = GetGlyphPosition();
        pDC->SetTextForeground((m_fSelected ? g_pColors->ScoreSelected() : colorC));
        pDC->DrawText(m_sText, pos.x, pos.y );
    }

}

wxString lmWordsDirection::Dump()
{
    wxString sDump = wxString::Format(
        _T("%d\tText %s\tTimePos=%.2f, fixed=%s paperPos=(%d, %d)\n"),
        m_nId, m_sText.Left(15), m_rTimePos, (m_fFixedPos ? _T("yes") : _T("no")),
        m_paperPos.x, m_paperPos.y);
    return sDump;
            
}

wxString lmWordsDirection::SourceLDP()
{
    wxString sSource = _T("         (texto ");
    sSource += m_sText;
    sSource += _T(")\n");
    return sSource;

}

wxString lmWordsDirection::SourceXML()
{
    //! @todo all
    wxString sSource = _T("TODO: lmWordsDirection XML Source code generation methods");
    return sSource;

////    <direction placement="above">
////      <direction-type>
////        <words xml:lang="la" relative-y="5" relative-x="-5">Angelus
//// dicit:</words>
////      </direction-type>
////    </direction>
//    
//    sFuente = "<direction placement=""?"">" & sCrLf & _
//                "  <direction-type>" & sCrLf & _
//                "    <words xml:lang=""??"" relative-y=""??"" relative-x=""??"">" & _
//                    m_sTexto & "</words>" & sCrLf & _
//                "  </direction-type>" & sCrLf & _
//                "<direction>"
//    
}

void lmWordsDirection::SetFont(lmPaper* pPaper)
{
    //wxLogMessage(wxString::Format(
    //    _T("[lmWordsDirection::SetFont]: size=%d, name=%s"), m_nFontSize, m_sFontName));

    int nWeight = (m_fBold ? wxBOLD : wxNORMAL);
    int nStyle = (m_fItalic ? wxITALIC : wxNORMAL);
    m_pFont = pPaper->GetFont(m_nFontSize, m_sFontName, wxDEFAULT, nStyle, nWeight, false);

    if (!m_pFont) {
        wxMessageBox(_("Sorry, an error has occurred while allocating the font."),
            _T("lmWordsDirection::SetFont"), wxOK);
        ::wxExit();
    }
}


