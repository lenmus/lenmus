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
#pragma implementation "BoxSystem.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

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

//access to colors
#include "../globals/Colors.h"
extern lmColors* g_pColors;


//-----------------------------------------------------------------------------------------
// Implementation of class lmBoxSystem: a system in the printed score. 
//-----------------------------------------------------------------------------------------

lmBoxSystem::lmBoxSystem(lmBoxPage* pParent, int nNumPage, int iSystem,
                         lmLUnits uxPos, lmLUnits uyPos, bool fFirstOfPage)
    : lmBox(pParent->GetScoreOwner(), eGMO_BoxSystem, _("system"))
{
    // iSystem: 0..n-1
    // (uxPos, uyPos): top left corner of limits rectangle
    // fFirstOfPage: this is the first system of a page

    m_nNumMeasures = 0;
    m_nNumPage = nNumPage;
    m_pBPage = pParent;
	m_pTopSpacer = NULL;

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

void lmBoxSystem::AddShape(lmShape* pShape, long nLayer)
{
	//override to avoid adding the staff to the shapes list
	if (pShape->GetType() == eGMO_ShapeStaff)
	{
		m_ShapeStaff.push_back( (lmShapeStaff*)pShape );
        pShape->SetOwnerBox(this);
	}

    lmBox::AddShape(pShape, nLayer);
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

	return m_ShapeStaff[0]->GetYTop();
}

lmShapeStaff* lmBoxSystem::FindStaffAtPosition(lmUPoint& uPoint)
{
	//is it any staff?
    for (int i=0; i < (int)m_ShapeStaff.size(); i++)
    {
        if (m_ShapeStaff[i]->BoundsContainsPoint(uPoint))
			return m_ShapeStaff[i];
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
        m_ShapeStaff[i]->SetXRight(xRight);
    }
}

wxString lmBoxSystem::Dump(int nIndent)
{
    //override to dump also the staff

	wxString sDump = _T("\n");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump += wxString::Format(_T("lmBoxSystem. %d measures starting at %d, "),
						m_nNumMeasures, m_nFirstMeasure);
	nIndent++;

	// dump the staff
    for (int i=0; i < (int)m_ShapeStaff.size(); i++)
        sDump += m_ShapeStaff[i]->Dump(nIndent);

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

