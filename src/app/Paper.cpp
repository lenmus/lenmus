//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation,
//    either version 3 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program. If not, see <http://www.gnu.org/licenses/>.

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

#include "Paper.h"


// global data structures for printing. Defined in TheApp.cpp
#include "wx/cmndata.h"
extern wxPrintData* g_pPrintData;
extern wxPageSetupData* g_pPaperSetupData;


lmPaper::lmPaper()
{
    m_uxCursor = 0.0f;
    m_uyCursor = 0.0f;
    m_pDrawer = (lmDrawer*) NULL;
}

lmPaper::~lmPaper()
{
    if (m_pDrawer)
        delete m_pDrawer;
}

void lmPaper::SetDrawer(lmDrawer* pDrawer)
{
    if (m_pDrawer)
        delete m_pDrawer;

    m_pDrawer = pDrawer;
}

bool lmPaper::IsDirectDrawer()
{
    return m_pDrawer && m_pDrawer->IsDirectDrawer();
}

wxFont* lmPaper::GetFont(int nPointSize, wxString sFontName,
                       int nFamily, int nStyle, int nWeight, bool fUnderline)
{
    return m_fontManager.GetFont(nPointSize, sFontName, nFamily, nStyle, nWeight, fUnderline);
}

