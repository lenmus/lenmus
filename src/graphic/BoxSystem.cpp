//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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

// For compilers that support precompilation, includes <wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "BoxScore.h"
#include "BoxPage.h"
#include "BoxSystem.h"
#include "BoxSlice.h"
#include "BoxSliceInstr.h"
#include "ShapeStaff.h"
#include "Handlers.h"
#include "../score/Instrument.h"

//access to colors
#include "../globals/Colors.h"
extern lmColors* g_pColors;


//-----------------------------------------------------------------------------------------
// Implementation of class lmBoxSystem: a system in the printed score. 
//-----------------------------------------------------------------------------------------

lmBoxSystem::lmBoxSystem(lmBoxPage* pParent, int nNumPage, int iSystem,
                         lmLUnits uxPos, lmLUnits uyPos, bool fFirstOfPage)
    : lmBox(pParent->GetScoreOwner(), eGMO_BoxSystem, _("system"))
    , m_nNumPage(nNumPage)
    , m_pBPage(pParent)
	, m_pTopSpacer(NULL)
{
    // iSystem: 0..n-1
    // (uxPos, uyPos): top left corner of limits rectangle
    // fFirstOfPage: this is the first system of a page

    //top border space
    lmScore* pScore = (lmScore*)pParent->GetBoxScore()->GetScoreOwner();
    lmLUnits uTopSpace = pScore->GetSystemDistance(iSystem, fFirstOfPage);
    if (!fFirstOfPage)
        uTopSpace /= 2.0;

    //set system dimensions and spacing
    SetLeftSpace(pScore->GetSystemLeftSpace(iSystem));
    SetPosition(uxPos, uyPos + uTopSpace);
    SetYTop( uyPos + uTopSpace );
    SetXLeft(uxPos);
    SetTopSpace(uTopSpace);
}

lmBoxSystem::~lmBoxSystem()
{
    ClearStaffShapesTable();
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

void lmBoxSystem::AddStaffShape(lmShapeStaff* pShapeStaff, lmInstrument* pInstr,
                                int nStaff)
{
	//add the shape for staff nStaff (1..n) to the staff shapes list and
    //to the shapes list

    lmShapeStaffData* pData = new lmShapeStaffData;
    pData->nStaff = nStaff;
    pData->pInstr = pInstr;
    pData->pShape = pShapeStaff;

	m_ShapeStaff.push_back(pData);
    pShapeStaff->SetOwnerBox(this);

    lmBox::AddShape(pShapeStaff, lm_eLayerStaff);
}

void lmBoxSystem::DeleteLastSlice()
{
    //This method is used during layout phase, to delete a column when finally it is decided not
    //to include it in current system

    //delete last slice
	delete m_Boxes.back();
	m_Boxes.pop_back();
}

lmLUnits lmBoxSystem::GetYTopFirstStaff()
{
	// Returns the Y top position of first staff

	return m_ShapeStaff[0]->pShape->GetYTop();
}

lmShapeStaff* lmBoxSystem::FindStaffAtPosition(lmUPoint& uPoint)
{
	//is it any staff?
    for (int i=0; i < (int)m_ShapeStaff.size(); i++)
    {
        if (m_ShapeStaff[i]->pShape->BoundsContainsPoint(uPoint))
			return m_ShapeStaff[i]->pShape;
    }
	return (lmShapeStaff*)NULL;
}

lmBoxSlice* lmBoxSystem::AddSlice(int nAbsMeasure, lmLUnits xStart, lmLUnits xEnd)
{
    lmBoxSlice* pBSlice = new lmBoxSlice(this, nAbsMeasure, (int)m_Boxes.size(),
										 xStart, xEnd);
    AddBox(pBSlice);
    return pBSlice;
}

void lmBoxSystem::UpdateXRight(lmLUnits xRight)
{ 
    //override to update only last slice of this system and the ShapeStaff final position 

    SetXRight(xRight);

	//propagate change to last slice of this system
	if (m_Boxes.size() > 0)
		((lmBoxSlice*)m_Boxes.back())->UpdateXRight(xRight);

	//update the ShapeStaff final position
    for (int i=0; i < (int)m_ShapeStaff.size(); i++)
    {
        m_ShapeStaff[i]->pShape->SetXRight(xRight);
    }
}

wxString lmBoxSystem::Dump(int nIndent)
{
    //override to dump also the staff

	wxString sDump = _T("\n");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump += wxString::Format(_T("lmBoxSystem. %d measures starting at %d, "),
						GetNumMeasures(), m_nFirstMeasure);
	nIndent++;

	// dump the staff
    for (int i=0; i < (int)m_ShapeStaff.size(); i++)
        sDump += m_ShapeStaff[i]->pShape->Dump(nIndent);

	//base class
    sDump += lmBox::Dump(nIndent);
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

lmBoxSlice* lmBoxSystem::FindBoxSliceAt(lmLUnits uxPos)
{
	//return slice located at uxPos

	return (lmBoxSlice*)FindChildBoxAt(uxPos);
}

int lmBoxSystem::GetNumMeasureAt(lmLUnits uxPos)
{
	lmBoxSlice* pSlice = FindBoxSliceAt(uxPos);
	if (!pSlice)
		return 0;
	else
		return pSlice->GetNumMeasure();
}

lmBoxScore* lmBoxSystem::GetOwnerBoxScore() 
{ 
    return m_pBPage->GetOwnerBoxScore(); 
}

void lmBoxSystem::SetBottomSpace(lmLUnits uyValue) 
{ 
    //overrided. To propagate bottom space to slice boxes

    m_uBottomSpace = uyValue;

	//propagate change
    std::vector<lmBox*>::iterator itB;
	for (itB=m_Boxes.begin(); itB != m_Boxes.end(); ++itB)
        (*itB)->SetBottomSpace(uyValue);
}

lmShapeStaff* lmBoxSystem::GetStaffShape(int nRelStaff) 
{ 
	//returns the shape for staff nRelStaff (1..n). nRelStaff is the staff number
    //relative to total staves in system

    wxASSERT(nRelStaff > 0);
    return m_ShapeStaff[nRelStaff - 1]->pShape;
}

lmShapeStaff* lmBoxSystem::GetStaffShape(lmInstrument* pInstr, int nStaff) 
{ 
	//returns the shape for staff nStaff (1..n) in instrument pInstr.
    //That is, nStaff is relative to the number of staves in the instrument, not
    //to the total number of staves in the system

    wxASSERT(nStaff > 0  && nStaff <= pInstr->GetNumStaves());

    std::vector<lmShapeStaffData*>::iterator it;
    for (it=m_ShapeStaff.begin(); it != m_ShapeStaff.end(); ++it)
    {
        if ((*it)->pInstr == pInstr && (*it)->nStaff == nStaff)
            return (*it)->pShape;
    }
    wxASSERT(false);    //impossible. It should have found the shape!        
    return (lmShapeStaff*)NULL;
}

lmShapeStaff* lmBoxSystem::GetStaffShape(lmInstrument* pInstr, lmUPoint uPoint)
{
	//For instrument nInstr, returns the nearest staff to point. That is, the staff
    //belongs to instrument nInstr.

    lmLUnits uDistance = 10000000000.0f;                    //any impossible big value
    lmShapeStaff* pShapeStaff = (lmShapeStaff*)NULL;        //nearest staff
    std::vector<lmShapeStaffData*>::iterator it;
    for (it=m_ShapeStaff.begin(); it != m_ShapeStaff.end(); ++it)
    {
        if ((*it)->pInstr == pInstr)
        {
            lmURect uBounds = (*it)->pShape->GetBounds();
            lmLUnits uThisDistance = wxMin(abs(uPoint.y - uBounds.GetLeftTop().y),
                                           abs(uPoint.y - uBounds.GetBottomLeft().y) );
            if (uDistance > uThisDistance)
            {
                uDistance = uThisDistance;
                pShapeStaff = (*it)->pShape;
            }
        }
        else if (pShapeStaff)
            return pShapeStaff;
    }
    wxASSERT(pShapeStaff);    //It should have found a shape!        
    return pShapeStaff;
}

void lmBoxSystem::ClearStaffShapesTable()
{
    std::vector<lmShapeStaffData*>::iterator it;
    for (it=m_ShapeStaff.begin(); it != m_ShapeStaff.end(); ++it)
        delete *it;

    m_ShapeStaff.clear();
}

int lmBoxSystem::GetNumMeasures() 
{ 
    return (int)m_Boxes.size(); 
}

