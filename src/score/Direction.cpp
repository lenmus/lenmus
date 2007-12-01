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

#ifdef __GNUG__
#pragma implementation "Direction.h"
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
    : lmStaffObj(pVStaff, eSFOT_WordsDirection, pVStaff, 1, true, lmDRAGGABLE),
      lmBasicText(sText, _T(""), pPos, oFontData)
{
    m_fHasWidth = fHasWidth;
    m_nAlign = nAlign;
}


//-----------------------------------------------------------------------------------------
// implementation of virtual methods defined in base abstract class lmStaffObj
//-----------------------------------------------------------------------------------------

void lmWordsDirection::LayoutObject(lmBox* pBox, lmPaper* pPaper, wxColour colorC)
{
    pPaper->SetFont(*m_pFont);

    lmLUnits uWidth, uHeight;
    pPaper->GetTextExtent(m_sText, &uWidth, &uHeight);

    //compute paper x shift to align text
    lmLUnits xPaperShift;
    if (m_nAlign == lmALIGN_CENTER) {
        xPaperShift = - uWidth/2;
    }
    else if (m_nAlign == lmALIGN_RIGHT) {
        xPaperShift = - uWidth;
    }
    else {
        xPaperShift = 0;
    }

    // Compute X position. Take into account if it is relative or absolute to paper pos.
    lmLUnits uxPos = m_pVStaff->TenthsToLogical(m_tPos.x, m_nStaffNum)
                     + xPaperShift;
    if (m_tPos.xType == lmLOCATION_RELATIVE)
        uxPos += pPaper->GetCursorX();
    else if (m_tPos.xType == lmLOCATION_ABSOLUTE)
        ;
    else
        uxPos = xPaperShift;

    //Compute Y position. 
    //method DC::DrawText position text with reference to its upper left
    //corner but lenmus anchor point is lower left corner. Therefore, it
    //is necessary to shift text up by text height
    lmLUnits uyPos = m_pVStaff->TenthsToLogical(m_tPos.y, m_nStaffNum) - uHeight;
    if (m_tPos.yType == lmLOCATION_RELATIVE)
        uyPos += pPaper->GetCursorY();
    else if (m_tPos.yType == lmLOCATION_ABSOLUTE)
        ;
    else
        uyPos = - uHeight;

    // create the shape
    lmShapeTex2* pShape = 
        new lmShapeTex2(this, m_sText, m_pFont, pPaper,
                        lmUPoint(uxPos, uyPos), _T("WordsDirection"), lmDRAGGABLE, colorC);
	pBox->AddShape(pShape);
    m_pShape2 = pShape;

	// set total width
    if (!m_fHasWidth)
        m_uWidth=0;
    else
        m_uWidth = pShape->GetWidth();

}

wxString lmWordsDirection::Dump()
{
    wxString sDump = wxString::Format(
        _T("%d\tText %s\tTimePos=%.2f, fixed=%s paperPos=(%d, %d)\n"),
        m_nId, m_sText.Left(15).c_str(), m_rTimePos, (m_fFixedPos ? _T("yes") : _T("no")),
        m_uPaperPos.x, m_uPaperPos.y);
    return sDump;

}

wxString lmWordsDirection::SourceLDP(int nIndent)
{
    wxString sSource = _T("");
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += _T("(text \"");
    sSource += m_sText;
    sSource += _T("\")\n");
    return sSource;

}

wxString lmWordsDirection::SourceXML(int nIndent)
{
    //TODO all
	wxString sSource = _T("");
	sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
    sSource += _T("TODO: lmWordsDirection XML Source code generation method\n");
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


