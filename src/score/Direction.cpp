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


lmWordsDirection::lmWordsDirection(lmVStaff* pVStaff, wxString sText, lmEAlignment nAlign,
                   lmLocation* pPos, lmFontInfo oFontData, bool fHasWidth) 
    : lmSimpleObj(eTPO_WordsDirection, pVStaff, 1, true, lmDRAGGABLE),
      lmBasicText(sText, _T(""), pPos, oFontData)
{
    m_fHasWidth = fHasWidth;
    m_nAlign = nAlign;
}


//-----------------------------------------------------------------------------------------
// implementation of virtual methods defined in base abstract class lmStaffObj
//-----------------------------------------------------------------------------------------

wxBitmap* lmWordsDirection::GetBitmap(double rScale)
{
    return PrepareBitMap(rScale, m_sText);
}

void lmWordsDirection::DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC,
                                  bool fHighlight)
{
    pPaper->SetFont(*m_pFont);

    if (fMeasuring) {
        lmLUnits nWidth, nHeight;
        pPaper->GetTextExtent(m_sText, &nWidth, &nHeight);

        // set total width
        m_nWidth = nWidth;

        //compute paper x shift to align text
        lmLUnits xPaperShift;
        if (m_nAlign == lmALIGN_CENTER) {
            xPaperShift = - nWidth/2;
        }
        else if (m_nAlign == lmALIGN_RIGHT) {   
            xPaperShift = - nWidth;
        }
        else {   
            xPaperShift = 0;
        }

        // store glyph position. Take into account that it is relative to paper pos.
        if (m_tPos.xType == lmLOCATION_RELATIVE) 
            m_glyphPos.x = m_pVStaff->TenthsToLogical(m_tPos.x, m_nStaffNum) + xPaperShift;
        else if (m_tPos.xType == lmLOCATION_ABSOLUTE)
            m_glyphPos.x = m_pVStaff->TenthsToLogical(m_tPos.x, m_nStaffNum) - m_paperPos.x  + xPaperShift;
        else
            m_glyphPos.x = xPaperShift;

        //method DC::DrawText position text with reference to its upper left
        //corner but lenmus anchor point is lower left corner. Therefore, it
        //is necessary to shift text up by text height
        if (m_tPos.yType == lmLOCATION_RELATIVE) 
            m_glyphPos.y = m_pVStaff->TenthsToLogical(m_tPos.y, m_nStaffNum) - nHeight;
        else if (m_tPos.yType == lmLOCATION_ABSOLUTE)
            m_glyphPos.y = m_pVStaff->TenthsToLogical(m_tPos.y, m_nStaffNum) - m_paperPos.y - nHeight;
        else
            m_glyphPos.y = - nHeight;

         // store selection rectangle (relative to m_paperPos). Coincides with glyph rectangle
        m_selRect.width = nWidth;
        m_selRect.height = nHeight;
        m_selRect.x = m_glyphPos.x;
        m_selRect.y = m_glyphPos.y;

        if (!m_fHasWidth) m_nWidth=0;

    }
    else {
        lmUPoint pos = GetGlyphPosition();
        pPaper->SetTextForeground((m_fSelected ? g_pColors->ScoreSelected() : colorC));
        pPaper->DrawText(m_sText, pos.x, pos.y );
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


