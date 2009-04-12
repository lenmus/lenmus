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
#pragma implementation "BoxPage.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "BoxScore.h"
#include "BoxPage.h"
#include "BoxSlice.h"
#include "BoxSystem.h"
#include "Handlers.h"


//access to colors
#include "../globals/Colors.h"
extern lmColors* g_pColors;

//-----------------------------------------------------------------------------------------

lmBoxPage::lmBoxPage(lmBoxScore* pParent, int nNumPage)
    : lmBox(pParent->GetScoreOwner(), eGMO_BoxPage, _("page"))
    , m_nNumPage(nNumPage)
    , m_nFirstSystem(0)
    , m_nLastSystem(0)
    , m_pBScore(pParent)
    , m_pRenderWindow(NULL)
{
    //create margin shapes
	lmScore* pScore = (lmScore*)m_pOwner;
	lmLUnits uxLeftMargin = pScore->GetLeftMarginXPos();
    lmLUnits uxRightMargin = pScore->GetRightMarginXPos();
	lmLUnits uyTopMargin = pScore->GetPageTopMargin();
    lmLUnits uyBottomMargin = pScore->GetMaximumY();
    lmLUnits uPageWidth = pScore->GetPaperSize().GetWidth();
    lmLUnits uPageHeight = pScore->GetPaperSize().GetHeight();
	
    m_pMarginShapes[0] =
        new lmShapeMargin(pScore, this, lmMARGIN_TOP, m_nNumPage, lmHORIZONTAL, 
                          uyTopMargin, uPageWidth);

    m_pMarginShapes[1] =
        new lmShapeMargin(pScore, this, lmMARGIN_BOTTOM, m_nNumPage, lmHORIZONTAL,
                          uyBottomMargin, uPageWidth);

    m_pMarginShapes[2] =
        new lmShapeMargin(pScore, this, lmMARGIN_LEFT, m_nNumPage, lmVERTICAL,
                          uxLeftMargin, uPageHeight);

    m_pMarginShapes[3] =
        new lmShapeMargin(pScore, this, lmMARGIN_RIGHT, m_nNumPage, lmVERTICAL,
                          uxRightMargin, uPageHeight);
}

lmBoxPage::~lmBoxPage()
{
    //delete all systems
    for (int i=0; i < (int)m_aSystems.size(); i++)
    {
        delete m_aSystems[i];
    }
    m_aSystems.clear();

    m_GMObjsWithHandlers.clear();

    //delete handlers
    for (int i=0; i<4; i++)
        delete m_pMarginShapes[i];
}

lmBoxSystem* lmBoxPage::AddSystem(int nSystem)
{
    //Update references
    if (m_nFirstSystem == 0) m_nFirstSystem = nSystem;
    m_nLastSystem = nSystem;

    //create the system
    lmBoxSystem* pSystem = new lmBoxSystem(this, m_nNumPage);
    m_aSystems.push_back(pSystem);
    return pSystem;
}

void lmBoxPage::Render(lmScore* pScore, lmPaper* pPaper)
{
    if (m_nLastSystem == 0) return;

    //clear lists with renderization information
    m_ActiveHandlers.clear();
    m_GMObjsWithHandlers.clear();

	//render score titles
	for (int i=0; i < (int)m_Shapes.size(); i++)
	{
		m_Shapes[i]->Render(pPaper);
	}

    //loop to render the systems in this page
	int iSystem = m_nFirstSystem;	//number of system in process
    for(int i=0; i < (int)m_aSystems.size(); iSystem++, i++)
    {
        m_aSystems[i]->Render(iSystem, pScore, pPaper);
    }

    //if requested, book to render page margins
    if (g_fShowMargins)
        this->OnNeedToDrawHandlers(this);
}

void lmBoxPage::RenderWithHandlers(lmPaper* pPaper)
{
    //render page with margins

    wxColour color = *wxGREEN;      //TODO User options
	//std::list<lmHandler*>::iterator it;
	//for (it = m_pHandlers->begin(); it != m_pHandlers->end(); ++it)
 //       (*it)->Render(pPaper, color);
    for (int i=0; i < 4; i++)
    {
        m_pMarginShapes[i]->Render(pPaper, color);
        AddActiveHandler( m_pMarginShapes[i] );
    }
}

void lmBoxPage::DrawAllHandlers(lmPaper* pPaper)
{
    //Inform all GMObjects that booked to draw handlers (during renderization) to do
    //it now

	std::vector<lmGMObject*>::iterator it;
	for (it = m_GMObjsWithHandlers.begin(); it != m_GMObjsWithHandlers.end(); ++it)
        (*it)->RenderWithHandlers(pPaper);
}

lmBoxSlice* lmBoxPage::FindSliceAtPosition(lmUPoint& pointL)
{
    //loop to look up in the systems

    for(int i=0; i < (int)m_aSystems.size(); i++)
    {
        lmBoxSlice* pBSlice = m_aSystems[i]->FindSliceAtPosition(pointL);
        if (pBSlice)
			return pBSlice;    //found
    }
    return (lmBoxSlice*)NULL;;
}

//void lmBoxPage::AddToSelection(lmGMSelection* pSelection, lmLUnits uXMin, lmLUnits uXMax,
//                              lmLUnits uYMin, lmLUnits uYMax)
//{
//    AddShapesToSelection(pSelection, uXMin, uXMax, uYMin, uYMax);
//
//    //loop to look up in the systems
//    std::vector<lmBoxSystem*>::iterator it;
//	for(it = m_aSystems.begin(); it != m_aSystems.end(); ++it)
//    {
//        (*it)->AddToSelection(pSelection, uXMin, uXMax, uYMin, uYMax);
//    }
//}

void lmBoxPage::SelectGMObjects(bool fSelect, lmLUnits uXMin, lmLUnits uXMax,
                         lmLUnits uYMin, lmLUnits uYMax)
{
    //look up in this box
    lmBox::SelectGMObjects(fSelect, uXMin, uXMax, uYMin, uYMax);

    //loop to look up in the systems
    std::vector<lmBoxSystem*>::iterator it;
	for(it = m_aSystems.begin(); it != m_aSystems.end(); ++it)
    {
        (*it)->SelectGMObjects(fSelect, uXMin, uXMax, uYMin, uYMax);
    }
}

lmGMObject* lmBoxPage::FindObjectAtPos(lmUPoint& pointL, bool fSelectable)
{
	//wxLogMessage(_T("[lmBoxPage::FindShapeAtPosition] GMO %s - %d"), m_sGMOName, m_nId); 

    //look up in active handlers
	std::list<lmHandler*>::iterator it;
	for (it = m_ActiveHandlers.begin(); it != m_ActiveHandlers.end(); ++it)
    {
		if ((*it)->BoundsContainsPoint(pointL))
            return *it;
    }

    //look in shapes collection
    lmShape* pShape = FindShapeAtPosition(pointL, fSelectable);
    if (pShape) return pShape;

    //loop to look up in the systems
	for(int i=0; i < (int)m_aSystems.size(); i++)
    {
        lmGMObject* pGMO = m_aSystems[i]->FindObjectAtPos(pointL, fSelectable);
        if (pGMO)
			return pGMO;		//Object found
    }

    // no object found.
    return (lmGMObject*)NULL;
}

void lmBoxPage::AddActiveHandler(lmHandler* pHandler)
{
    m_ActiveHandlers.push_back(pHandler);
}

void lmBoxPage::OnNeedToDrawHandlers(lmGMObject* pGMO)
{
    //This method is invoked by objects contained in this BoxPage. 
    //Handlers are not rendered during shapes renderization. Instead, if
    //during renderization and object has the need to draw handlers (for instance,
    //if the object is selected) if MUST inform its parent BoxPage by invoking this
    //method. BoxPage mantains a list of objects having requested to draw handlers
    //and, when appropriate, it will invoke method DrawHandlers for those objects, 
    //so that they can do it.

    m_GMObjsWithHandlers.push_back(pGMO);
}

wxString lmBoxPage::Dump(int nIndent)
{
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump += wxString::Format(_T("lmBoxPage %d (systems %d to %d), "),
						m_nNumPage, m_nFirstSystem, m_nLastSystem);
    sDump += DumpBounds();
    sDump += _T("\n");

    //loop to dump the systems in this page
	nIndent++;
	for(int i=0; i < (int)m_aSystems.size(); i++)
    {
        sDump += m_aSystems[i]->Dump(nIndent);
    }

	return sDump;
}

int lmBoxPage::GetSystemNumber(lmBoxSystem* pSystem)
{
	//returns the number of the system (1..n) received as parameter
	//precondition: the system must be in this page

    //loop to look up in the systems collection
	for(int i=0; i < (int)m_aSystems.size(); i++)
    {
        if (m_aSystems[i] == pSystem)
			return i + m_nFirstSystem;		//system found
    }
	wxASSERT(false);	//system not in this page
	return 0;			//keep compiler happy
}

lmBoxSystem* lmBoxPage::GetSystem(int nSystem)
{
	//return pointer to BoxSystem for system nSystem (1..n)
    //loop to look up in the systems collection

	int i = nSystem - m_nFirstSystem;
	if (i < 0)
		return (lmBoxSystem*)NULL;		//the system is not in this page
	else
		return m_aSystems[i];
}


