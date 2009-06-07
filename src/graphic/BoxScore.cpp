//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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
    , m_pScore(pScore)
{
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

void lmBoxScore::RenderPage(int nPage, lmPaper* pPaper, wxWindow* pRenderWindow,
                            wxPoint& vOffset)
{
    // Render page nPage (1..n)
	// This method is invoked from lmGraphicManager::Render()
    
    wxASSERT(nPage > 0 && nPage <= (int)m_aPages.size());

    //if firts page render score titles
    if (nPage == 1)
	    RenderShapes(pPaper);

    //render the requested page
    m_aPages[nPage-1]->SetRenderWindow(pRenderWindow);
    m_aPages[nPage-1]->SetRenderWindowOffset(vOffset);
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

int lmBoxScore::GetNumSystems()
{
	//return total number of systems (1..n) in this score

	return m_aPages.back()->GetLastSystem();
}

lmBoxSystem* lmBoxScore::GetSystem(int nSystem)
{
	//return pointer to BoxSystem for system nSystem (1..n)

	//locate page
	bool fFound = false;
    int i;
    for (i=0; i < (int)m_aPages.size(); i++)
    {
        if (m_aPages[i]->GetFirstSystem() <= nSystem &&
			m_aPages[i]->GetLastSystem() >= nSystem)
		{
			fFound = true;
			break;
		}
    }

	wxASSERT(fFound);
	return m_aPages[i]->GetSystem(nSystem);
}

void lmBoxScore::PopulateLayers()
{
    std::vector<lmBoxPage*>::iterator it;
    for (it = m_aPages.begin(); it != m_aPages.end(); ++it)
        (*it)->PopulateLayers();
}

void lmBoxScore::AddToSelection(lmGMObject* pGMO)
{
    if (pGMO->IsSelectable())
        m_Selection.AddToSelection(pGMO);
}

void lmBoxScore::RemoveFromSelection(lmGMObject* pGMO)
{
    m_Selection.RemoveFromSelection(pGMO);
}

void lmBoxScore::ClearSelection()
{
    m_Selection.ClearSelection();
}

void lmBoxScore::AddToSelection(int nNumPage, lmLUnits uXMin, lmLUnits uXMax,
                                lmLUnits uYMin, lmLUnits uYMax)
{
    wxASSERT(nNumPage <= GetNumPages());

    lmBoxPage* pBPage = GetPage(nNumPage);
    pBPage->SelectGMObjects(true, uXMin, uXMax, uYMin, uYMax);
}

lmBoxPage* lmBoxScore::GetPage(int nPage) 
{ 
    if (nPage <= GetNumPages())
        return m_aPages[nPage - 1]; 
    else
        return (lmBoxPage*)NULL;
}

