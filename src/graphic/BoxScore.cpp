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
#pragma implementation "BoxScore.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "../score/Score.h"
#include "BoxScore.h"
#include "BoxPage.h"

//-----------------------------------------------------------------------------------------

lmBoxScore::lmBoxScore(lmScore* pScore) 
    : lmBox(pScore, eGMO_BoxScore, _("score"))
{
    m_pScore = pScore;

    //initializations
    m_pCursorSO = (lmStaffObj*)NULL;

    //Initially the BoxScore will have one page.
    AddPage();

}

lmBoxScore::~lmBoxScore()
{
    //delete all pages
    for (int i=0; i < (int)m_aPages.size(); i++)
    {
        delete m_aPages[i];
    }
    m_aPages.clear();
}

void lmBoxScore::RenderPage(int nPage, lmPaper* pPaper)
{
    // Render page nPage (1..n)
	// This method is invoked from lmGraphicManager::Render()
    
    wxASSERT(nPage > 0 && nPage <= (int)m_aPages.size());

    if (nPage == 1) {
		//render score titles
		for (int i=0; i < (int)m_Shapes.size(); i++)
		{
			m_Shapes[i]->Render(pPaper);
		}
    }

    //render the requested page
    m_aPages[nPage-1]->Render(m_pScore, pPaper);

}

lmBoxPage* lmBoxScore::AddPage()
{
    lmBoxPage* pPage = new lmBoxPage(this, (int)m_aPages.size()+1);
    m_aPages.push_back(pPage);
    return pPage;

}

int lmBoxScore::GetNumPages()
{
    return (int)m_aPages.size();
}

bool lmBoxScore::FindSliceAtPosition(lmUPoint& pointL)
{
    //loop to look up in the pages
    for (int i=0; i < (int)m_aPages.size(); i++)
    {
        if (m_aPages[i]->FindSliceAtPosition(pointL))
            return true;    //found
    }
    return false;
}

wxString lmBoxScore::Dump(int nIndent)
{
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump += wxString::Format(_T("lmBoxScore. ID %d\n"), GetID());

    //loop to dump the pages in this score
	nIndent++;
    for (int i=0; i < (int)m_aPages.size(); i++)
    {
        sDump += m_aPages[i]->Dump(nIndent);
    }

	return sDump;
}
