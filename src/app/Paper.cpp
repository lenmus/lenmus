//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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
#pragma implementation "Paper.h"
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
#include "../score/Score.h"
#include "Page.h"
#include "FontManager.h"
#include "Paper.h"
#include "../graphic/GraphicManager.h"


// global data structures for printing. Defined in TheApp.cpp
#include "wx/cmndata.h"
extern wxPrintData* g_pPrintData;
extern wxPageSetupData* g_pPaperSetupData;


lmPaper::lmPaper()
{
    m_uxCursor = 0.0f;
    m_uyCursor = 0.0f;
    m_pDrawer = (lmDrawer*) NULL;
    m_pPageInfo = &m_DefaultPage;
    m_nNumPage = 1;
    m_fUseDefault = false;
}

lmPaper::~lmPaper()
{
    if (m_pDrawer) delete m_pDrawer;
}

void lmPaper::ForceDefaultPageInfo(bool fValue)
{
    if (fValue)
        m_pPageInfo = &m_DefaultPage;
    m_fUseDefault = fValue;
}

void lmPaper::SetPageInfo(lmPageInfo* pPageInfo, int nNumPage)
{
    //if paper not forced to use the default settings, sets the current page 
    //number and page info

    if (m_fUseDefault) return;

    m_pPageInfo = pPageInfo;
    m_nNumPage = nNumPage;
}

void lmPaper::SetDrawer(lmDrawer* pDrawer)
{
    if (m_pDrawer) delete m_pDrawer;
    m_pDrawer = pDrawer;
}

void lmPaper::RestartPageCursors()
{
    m_uxCursor = GetPageLeftMargin();
    m_uyCursor = GetPageTopMargin();

}

void lmPaper::NewLine(lmLUnits nSpace)
{
    m_uyCursor += nSpace;
    m_uxCursor = GetPageLeftMargin();

}

lmLUnits lmPaper::GetRightMarginXPos()
{
    return GetPaperSize().GetWidth() - GetPageRightMargin();
}

lmLUnits lmPaper::GetLeftMarginXPos()
{
    return GetPageLeftMargin();
}

wxFont* lmPaper::GetFont(int nPointSize, wxString sFontName,
                       int nFamily, int nStyle, int nWeight, bool fUnderline)
{
    return m_fontManager.GetFont(nPointSize, sFontName, nFamily, nStyle, nWeight, fUnderline);
}

