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
#pragma implementation "BoxSystem.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "BoxPage.h"
#include "BoxSystem.h"
#include "BoxSlice.h"
#include "BoxSliceInstr.h"
#include "BoxSliceVStaff.h"
#include "ShapeStaff.h"
#include "Handlers.h"

//access to colors
#include "../globals/Colors.h"
extern lmColors* g_pColors;


//-----------------------------------------------------------------------------------------
// Implementation of class lmBoxSystem: a system in the printed score. 
//-----------------------------------------------------------------------------------------

lmBoxSystem::lmBoxSystem(lmBoxPage* pParent, int nNumPage)
    : lmBox(pParent->GetScoreOwner(), eGMO_BoxSystem, _("system"))
{
    m_nNumMeasures = 0;
    m_nNumPage = nNumPage;
    m_pBPage = pParent;
	m_pTopSpacer = NULL;

	//add handler for top spacer. It will be positioned in lmBoxSystem::SetPosition() method
	//lmScore* pScore = (lmScore*)GetScoreOwner();
 //   lmLUnits uPageWidth = pScore->GetPaperSize().GetWidth();
	//lmLUnits uyTopMargin = 0.0f;
	//m_pTopSpacer = new lmShapeMargin((lmScore*)m_pOwner, lmMARGIN_TOP, lmHORIZONTAL, 
 //                                    uyTopMargin, uPageWidth);
	//AddHandler(m_pTopSpacer);
}

lmBoxSystem::~lmBoxSystem()
{
    //delete BoxSlices collection
    for (int i=0; i < (int)m_Slices.size(); i++)
    {
        delete m_Slices[i];
    }
    m_Slices.clear();

    //delete staff shapes
    for (int i=0; i < (int)m_ShapeStaff.size(); i++)
    {
        delete m_ShapeStaff[i];
    }
    m_ShapeStaff.clear();
}

void lmBoxSystem::SetPosition(lmLUnits xPos, lmLUnits yPos)
{ 
	m_xPos = xPos; 
	m_yPos = yPos; 

	//reposition the handlers
	if (m_pTopSpacer)
	{
		m_pTopSpacer->SetYBottom(yPos);
		m_pTopSpacer->SetYTop(yPos);
	}
}

void lmBoxSystem::AddShape(lmShape* pShape)
{
	//override to avoid adding the staff to the shapes list
	if (pShape->GetType() == eGMO_ShapeStaff)
	{
		m_ShapeStaff.push_back( (lmShapeStaff*)pShape );
        pShape->SetOwnerBox(this);
	}
	else
		lmBox::AddShape(pShape);

}


void lmBoxSystem::Render(int nSystem, lmScore* pScore, lmPaper* pPaper)
{
    //At this point paper position is not in the right place. Therefore, we move
    //to the start of system position.
    pPaper->SetCursorY( m_yPos );

    //render staff lines
    for (int i=0; i < (int)m_ShapeStaff.size(); i++)
    {
        m_ShapeStaff[i]->Render(pPaper);
    }

	//for each lmBoxSlice
    for (int i=0; i < (int)m_Slices.size(); i++)
    {
        m_Slices[i]->Render(pPaper, lmUPoint(m_xPos, m_yPos));
    }

	//render shapes
    for (int i=0; i < (int)m_Shapes.size(); i++)
    {
        m_Shapes[i]->Render(pPaper);    //, lmUPoint(m_xPos, m_yPos));
    }
}

void lmBoxSystem::SetNumMeasures(int nMeasures, lmScore* pScore)
{
	// This method is only invoked during layout phase, when the number of measures in the
	// system has been finally decided. We have to store this number, delete any addional
	// slices added during measurements, and propagate 'y' coordinates from first slice to
	// all others.

    m_nNumMeasures = nMeasures;

	//remove extra slices not needed
	for(int i=nMeasures; i < (int)m_Slices.size(); i++)
	{
		delete m_Slices.back();
		m_Slices.pop_back();
	}

	//propagate 'y' coordinates from first slice to all others
    for (int i=1; i < (int)m_Slices.size(); i++)
    {
        m_Slices[i]->CopyYBounds(m_Slices[0]);
    }

	//update system yBottom position by copying yBottom from first slice
    if (m_Slices.size() > 0)
	    SetYBottom(m_Slices[0]->GetYBottom());

}

lmLUnits lmBoxSystem::GetYTopFirstStaff()
{
	// Returns the Y top position of first staff

	return m_ShapeStaff[0]->GetYTop();
}

lmBoxSlice* lmBoxSystem::FindSliceAtPosition(lmUPoint& pointL)
{
    if (BoundsContainsPoint(pointL))
    {
        //identify the measure
        for (int iS=0; iS < (int)m_Slices.size(); iS++)
        {
            if (m_Slices[iS]->FindMeasureAt(pointL))
            {
                return m_Slices[iS];
            }
        }
        wxMessageBox( wxString::Format( _T("Page %d, measure not identified!!! Between measure %d and %d"),
            m_nNumPage, m_nFirstMeasure, m_nFirstMeasure+m_nNumMeasures-1) );
        return (lmBoxSlice*)NULL;
    }
    return (lmBoxSlice*)NULL;
}

lmGMObject* lmBoxSystem::FindSelectableObjectAtPos(lmUPoint& pointL)
{
    //look in shapes collection
    lmShape* pShape = FindShapeAtPosition(pointL);
    if (pShape) return pShape;

    std::vector<lmBoxSlice*>::iterator it;
	for(it = m_Slices.begin(); it != m_Slices.end(); ++it)
    {
        lmGMObject* pGMO = (*it)->FindSelectableObjectAtPos(pointL);
        if (pGMO)
			return pGMO;    //found
    }

	//AWARE: It is useless to check here if it is an staff, as in that case
	//as the staff area is inside an SliceVStaff, this method will finish in
	//previous 'for' loop, returning an SliceVStaff

    // no object found. Verify if the point is in this object
    if (IsSelectable() && SelRectContainsPoint(pointL))
        return this;
    else
        return (lmGMObject*)NULL;

}

//void lmBoxSystem::AddToSelection(lmGMSelection* pSelection, lmLUnits uXMin, lmLUnits uXMax,
//                              lmLUnits uYMin, lmLUnits uYMax)
//{
//    AddShapesToSelection(pSelection, uXMin, uXMax, uYMin, uYMax);
//
//    //loop to look up in the slices
//    std::vector<lmBoxSlice*>::iterator it;
//	for(it = m_Slices.begin(); it != m_Slices.end(); ++it)
//    {
//        (*it)->AddToSelection(pSelection, uXMin, uXMax, uYMin, uYMax);
//    }
//}

void lmBoxSystem::SelectGMObjects(bool fSelect, lmLUnits uXMin, lmLUnits uXMax,
                         lmLUnits uYMin, lmLUnits uYMax)
{
    //look up in this box
    lmBox::SelectGMObjects(fSelect, uXMin, uXMax, uYMin, uYMax);

    //loop to look up in the slices
    std::vector<lmBoxSlice*>::iterator it;
	for(it = m_Slices.begin(); it != m_Slices.end(); ++it)
    {
        (*it)->SelectGMObjects(fSelect, uXMin, uXMax, uYMin, uYMax);
    }
}

lmShapeStaff* lmBoxSystem::FindStaffAtPosition(lmUPoint& pointL)
{
	//is it any staff?
    for (int i=0; i < (int)m_ShapeStaff.size(); i++)
    {
        if (m_ShapeStaff[i]->BoundsContainsPoint(pointL))
			return m_ShapeStaff[i];
    }
	return (lmShapeStaff*)NULL;
}

lmBoxSlice* lmBoxSystem::AddSlice(int nAbsMeasure, lmLUnits xStart, lmLUnits xEnd)
{
    lmBoxSlice* pBSlice = new lmBoxSlice(this, nAbsMeasure, (int)m_Slices.size(),
										 xStart, xEnd);
    m_Slices.push_back(pBSlice);
    return pBSlice;
}

void lmBoxSystem::UpdateXRight(lmLUnits xPos)
{ 
	SetXRight(xPos);

	//propagate change to last slice of this system
	if (m_Slices.size() > 0)
		m_Slices.back()->UpdateXRight(xPos);

	//update the ShapeStaff final position
    for (int i=0; i < (int)m_ShapeStaff.size(); i++)
    {
        m_ShapeStaff[i]->SetXRight(xPos);
    }
}

wxString lmBoxSystem::Dump(int nIndent)
{
	wxString sDump = _T("\n");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump += wxString::Format(_T("lmBoxSystem. %d measures starting at %d, "),
						m_nNumMeasures, m_nFirstMeasure);
    sDump += DumpBounds();
    sDump += _T("\n");

	nIndent++;

	// dump the staff
    for (int i=0; i < (int)m_ShapeStaff.size(); i++)
    {
        sDump += m_ShapeStaff[i]->Dump(nIndent);
    }

	//dump the shapes in this system
    for (int i=0; i < (int)m_Shapes.size(); i++)
    {
        sDump += m_Shapes[i]->Dump(nIndent);
    }

    //loop to dump the slices in this system
    for (int i=0; i < (int)m_Slices.size(); i++)
    {
        sDump += m_Slices[i]->Dump(nIndent);
    }

	return sDump;
}

int lmBoxSystem::GetPageNumber() const
{ 
	return m_pBPage->GetPageNumber();
}

int lmBoxSystem::GetSystemNumber()
{
	//return number of this system (1..n)

	return m_pBPage->GetSystemNumber(this);
}

lmBoxScore* lmBoxSystem::GetBoxScore()
{
	//return owner BoxScore

	return m_pBPage->GetBoxScore();
}

lmBoxSlice* lmBoxSystem::GetSliceAt(lmLUnits xPos)
{
	//return slice located at xPos

    //locate the BoxSlice
	for(int i=0; i < (int)m_Slices.size(); i++)
    {
        if (m_Slices[i]->ContainsXPos(xPos))
			return m_Slices[i];		//found
    }
	return (lmBoxSlice*)NULL;
}

int lmBoxSystem::GetNumMeasureAt(lmLUnits uxPos)
{
	lmBoxSlice* pSlice = GetSliceAt(uxPos);
	if (!pSlice)
		return 0;
	else
		return pSlice->GetNumMeasure();
}

lmBoxScore* lmBoxSystem::GetOwnerBoxScore() 
{ 
    return m_pBPage->GetOwnerBoxScore(); 
}

