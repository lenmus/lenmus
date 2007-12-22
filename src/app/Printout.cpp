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
#pragma implementation "Printout.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "ScoreView.h"
#include "Printout.h"

lmPrintout::lmPrintout(lmScoreView* pView, wxChar *title)
    : wxPrintout(title)
{
    wxASSERT(pView);
    m_pView = pView;
}

bool lmPrintout::OnPrintPage(int nPage)
{
    wxDC *pDC = GetDC();
    if (pDC) {
        m_pView->DrawPage(pDC, nPage, this);
        return true;
    }
    else
        return false;
}

bool lmPrintout::OnBeginDocument(int startPage, int endPage)
{
    if (!wxPrintout::OnBeginDocument(startPage, endPage))
        return false;

    return true;
}

void lmPrintout::GetPageInfo(int* pMinPage, int* pMaxPage, int* pSelPageFrom, int* pSelPageTo)
{
    m_pView->GetPageInfo(pMinPage, pMaxPage, pSelPageFrom, pSelPageTo);
    m_nMinPage = *pMinPage;
    m_nMaxPage = *pMaxPage;
}

bool lmPrintout::HasPage(int nPage)
{
    return (nPage > 0 && nPage <= m_nMaxPage);
}


/*
// Writes a header on a page. Margin units are in millimetres.
bool WritePageHeader(wxPrintout *printout, wxDC *dc, wxChar *text, float mmToLogical)
{
    //static wxFont *headerFont = (wxFont *) NULL;
    //if (!headerFont)
    //{
    //headerFont = wxTheFontList->FindOrCreateFont(16, wxSWISS, wxNORMAL, wxBOLD);
    //}
    //dc->SetFont(headerFont);

    int pageWidthMM, pageEighthMM;

    printout->GetPageSizeMM(&pageWidthMM, &pageEighthMM);

    int leftMargin = 10;
    int topMargin = 10;
    int rightMargin = 10;

    float leftMarginLogical = (float)(mmToLogical*leftMargin);
    float topMarginLogical = (float)(mmToLogical*topMargin);
    float rightMarginLogical = (float)(mmToLogical*(pageWidthMM - rightMargin));

    long xExtent, yExtent;
    dc->GetTextExtent(text, &xExtent, &yExtent);
    float xPos = (float)(((((pageWidthMM - leftMargin - rightMargin)/2.0)+leftMargin)*mmToLogical) - (xExtent/2.0));
    dc->DrawText(text, (long)xPos, (long)topMarginLogical);

    dc->SetPen(* wxBLACK_PEN);
    dc->DrawLine( (long)leftMarginLogical, (long)(topMarginLogical+yExtent),
        (long)rightMarginLogical, (long)topMarginLogical+yExtent );

    return true;
}
*/
