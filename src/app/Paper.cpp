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
/*! @file Paper.cpp
    @brief Implementation file for class lmPaper
    @ingroup app_gui
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

#include "scoreview.h"
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
    m_xCursor = 0;
    m_yCursor = 0;
    m_pDrawer = (lmDrawer*) NULL;
}

lmPaper::~lmPaper()
{
    if (m_pDrawer) delete m_pDrawer;
}

void lmPaper::SetDrawer(lmDrawer* pDrawer)
{
    if (m_pDrawer) delete m_pDrawer;
    m_pDrawer = pDrawer;
}

// returns paper size in logical units
wxSize& lmPaper::GetPaperSize()
{
    return m_Page.PageSize();
    //wxSize sz = g_pPaperSetupData->GetPaperSize();
    //return wxSize(10*sz.GetWidth(), 10*sz.GetHeight());
}


void lmPaper::RestartPageCursors()
{
    m_xCursor = GetPageLeftMargin();
    m_yCursor = GetPageTopMargin();

}

void lmPaper::NewLine(lmLUnits nSpace)
{
    m_yCursor += nSpace;
    m_xCursor = GetPageLeftMargin();
    
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

