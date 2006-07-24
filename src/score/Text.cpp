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
/*! @file Text.cpp
    @brief Implementation file for classes lmBasicText and lmScoreText
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
#include "Text.h"

//Aux. function to convert font pointsize to lmLUnits
int PointsToLUnits(lmLUnits nPoints)
{
    //One point equals 1/72 of an inch
    //One inch equals 2.54 cm = 25.4 mm
    //then 1 pt = 25.4/72 mm
    return lmToLogicalUnits(nPoints * 25.4 / 72.0, lmMILLIMETERS);
}

//Global variables used as default initializators
lmFontInfo tBasicTextDefaultFont = { _T("Arial"), 12, lmTEXT_NORMAL };



//==========================================================================================
// lmBasicText implementation
//==========================================================================================

lmBasicText::lmBasicText(wxString sText, wxString sLanguage,
                   lmLocation* pPos, lmFontInfo tFontData) 
{
    m_sText = sText;
    m_sLanguage = sLanguage;

    // font data
    m_sFontName = tFontData.sFontName;
    m_nFontSize = PointsToLUnits(tFontData.nFontSize);
    m_fBold = (tFontData.nStyle == lmTEXT_BOLD || tFontData.nStyle == lmTEXT_ITALIC_BOLD);
    m_fItalic = (tFontData.nStyle == lmTEXT_ITALIC || tFontData.nStyle == lmTEXT_ITALIC_BOLD);

    // position data
    m_tPos = *pPos;

}

//==========================================================================================
// lmScoreText implementation
//==========================================================================================

lmScoreText::lmScoreText(lmScore* pScore, wxString sTitle, lmEAlignment nAlign,
               lmLocation tPos, lmFontInfo tFont) :
    lmSimpleObj(eTPO_Text, (lmVStaff*)NULL, 0, true, sbDRAGGABLE)
{
    m_pScore = pScore;
    m_sText = sTitle;
    m_sFontName = tFont.sFontName;
    m_nFontSize = PointsToLUnits(tFont.nFontSize);
    m_fBold = (tFont.nStyle == lmTEXT_BOLD || tFont.nStyle == lmTEXT_ITALIC_BOLD);
    m_fItalic = (tFont.nStyle == lmTEXT_ITALIC || tFont.nStyle == lmTEXT_ITALIC_BOLD);
    m_tPos = tPos;
    m_nAlignment = nAlign;

}




//-----------------------------------------------------------------------------------------
// implementation of virtual methods defined in base abstract class lmStaffObj
//-----------------------------------------------------------------------------------------

wxBitmap* lmScoreText::GetBitmap(double rScale)
{
    return PrepareBitMap(rScale, m_sText);
}

void lmScoreText::DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC)
{
    wxDC* pDC = pPaper->GetDC();
    wxASSERT(pDC);
    pDC->SetFont(*m_pFont);

    if (fMeasuring) {
        lmLUnits nWidth, nHeight;
        pDC->GetTextExtent(m_sText, &nWidth, &nHeight);
        wxLogMessage(_T("[lmScoreText::DrawObject] text='%s'. width=%d"), m_sText, nWidth);

         // store selection rectangle (relative to m_paperPos)
        m_selRect.width = nWidth;
        m_selRect.height = nHeight;
        m_selRect.x = 0;    //remember: relative to m_paperPos
        m_selRect.y = 0;

        // set total width
        m_nWidth = nWidth;

        // store glyph position (relative to paper pos). 
        m_glyphPos.x = 0;
        m_glyphPos.y = 0;

    }
    else {
        wxPoint pos = GetGlyphPosition();
        pDC->SetTextForeground((m_fSelected ? g_pColors->ScoreSelected() : colorC));
        pDC->DrawText(m_sText, pos.x, pos.y );
    }

}

wxString lmScoreText::Dump()
{
    wxString sDump = wxString::Format(
        _T("%d\tText %s\tfixed=%s paperPos=(%d, %d)\n"),
        m_nId, m_sText.Left(15), (m_fFixedPos ? _T("yes") : _T("no")),
        m_paperPos.x, m_paperPos.y);
    return sDump;
            
}

wxString lmScoreText::SourceLDP()
{
    wxString sSource = _T("(text ");
    sSource += m_sText;
    sSource += _T(")");
    return sSource;

}

wxString lmScoreText::SourceXML()
{
    //! @todo all
    wxString sSource = _T("TODO: lmScoreText XML Source code generation methods");
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

void lmScoreText::SetFont(lmPaper* pPaper)
{
    //wxLogMessage(wxString::Format(
    //    _T("[lmScoreText::SetFont]: size=%d, name=%s"), m_nFontSize, m_sFontName));

    int nWeight = (m_fBold ? wxBOLD : wxNORMAL);
    int nStyle = (m_fItalic ? wxITALIC : wxNORMAL);
    m_pFont = pPaper->GetFont(m_nFontSize, m_sFontName, wxDEFAULT, nStyle, nWeight, false);

    if (!m_pFont) {
        wxMessageBox(_("Sorry, an error has occurred while allocating the font."),
            _T("lmScoreText::SetFont"), wxOK);
        ::wxExit();
    }
}


