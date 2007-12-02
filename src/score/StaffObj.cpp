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

#ifdef __GNUG__
#pragma implementation "StaffObj.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/image.h"
#include "Score.h"
#include "ObjOptions.h"
#include "../graphic/GMObject.h"
#include "../graphic/Shapes.h"


//implementation of the StaffObjs List
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(StaffObjsList);

//implementation of the AuxObjs List
WX_DEFINE_LIST(AuxObjsList);

static int m_IdCounter = 0;        //to assign unique IDs to ComponentObjs

//-------------------------------------------------------------------------------------------------
// lmComponentObj implementation
//-------------------------------------------------------------------------------------------------
lmComponentObj::lmComponentObj(lmScoreObj* pParent, EScoreObjType nType, bool fIsDraggable) :
    lmScoreObj(pParent)
{
    m_nId = m_IdCounter++;        // give it an ID
    m_nType = nType;            // save type

    // behaviour
    m_fIsDraggable = fIsDraggable;

    // initializations: font related info
    m_pFont = (wxFont *)NULL;

    // initializations: positioning related info
    m_uPaperPos.y = 0;
    m_uPaperPos.x = 0;
    m_fFixedPos = false;
    m_nNumPage = 1;

    //transitional
    m_pShape2 = (lmShape*)NULL;

    //// GraphicObjs owned by this ComponentObj
    //m_pGraphObjs = (GraphicObjsList*)NULL;

}

lmComponentObj::~lmComponentObj()
{
    //if (m_pGraphObjs) {
    //    m_pGraphObjs->DeleteContents(true);
    //    m_pGraphObjs->Clear();
    //    delete m_pGraphObjs;
    //    m_pGraphObjs = (GraphicObjsList*)NULL;
    //}

}

void lmComponentObj::MoveTo(lmUPoint& uPt)
{
    m_uPaperPos.y = uPt.y;
    m_uPaperPos.x = uPt.x;
}

//// Management of GraphicObjs attached to this object
//
//void lmComponentObj::DoAddGraphicObj(lmComponentObj* pGO)
//{
//    wxASSERT(pGO->GetType() == eSCOT_GraphicObj);
//    if (!m_pGraphObjs) m_pGraphObjs = new GraphicObjsList();
//    m_pGraphObjs->Append((lmGraphicObj*)pGO);
//}
//
//void lmComponentObj::DoRemoveGraphicObj(lmComponentObj* pGO)
//{
//    wxASSERT(pGO->GetType() == eSCOT_GraphicObj);
//}

void lmComponentObj::ShiftObject(lmLUnits uLeft)
{ 
    // update shapes' positions when the object is moved

    if (m_pShape2) m_pShape2->Shift(uLeft, 0.0);    //(uLeft - m_uPaperPos.x, 0.0);
    //wxLogMessage(_T("[lmComponentObj::ShiftObject] shift=%.2f, ID=%d"), uLeft, GetID());
    //m_uPaperPos.x = uLeft;
}


//-------------------------------------------------------------------------------------------------
// lmStaffObj implementation
//-------------------------------------------------------------------------------------------------

lmStaffObj::lmStaffObj(lmScoreObj* pParent, EStaffObjType nType, lmVStaff* pStaff, int nStaff,
                   bool fVisible, bool fIsDraggable) :
    lmComponentObj(pParent, eSCOT_StaffObj, fIsDraggable)
{
    // store parameters
    m_fVisible = fVisible;
    m_nClass = nType;

    // initializations: staff ownership info
    m_pVStaff = pStaff;
    m_nStaffNum = (pStaff ? nStaff : 0);

}

lmStaffObj::~lmStaffObj()
{
    //delete the attached AuxObjs
    for (int i=0; i < (int)m_AuxObjs.size(); i++)
    {
        delete m_AuxObjs[i];
    }
    m_AuxObjs.clear();
}

lmUPoint lmStaffObj::GetReferencePos(lmPaper* pPaper)
{
    if (!m_fFixedPos) {
        m_uPaperPos.x = pPaper->GetCursorX();
        m_uPaperPos.y = pPaper->GetCursorY();
    } else {
        pPaper->SetCursorX(m_uPaperPos.x);
        pPaper->SetCursorY(m_uPaperPos.y);
    }
	return lmUPoint(pPaper->GetCursorX(), pPaper->GetCursorY());
}

void lmStaffObj::Layout(lmBox* pBox, lmPaper* pPaper, wxColour colorC, bool fHighlight)
{
	lmUPoint uOrg = GetReferencePos(pPaper);

	lmLUnits uWidth;
    if (m_fVisible) 
	{
		// set the font
		SetFont(pPaper);

		// ask derived object to layout itself
		uWidth = LayoutObject(pBox, pPaper, colorC);
	}
	else
	{
		//Create an invisible shape, to store the StaffObj position
		
		lmShapeInvisible* pShape = new lmShapeInvisible(this, uOrg);
		pBox->AddShape(pShape);
		m_pShape2 = pShape;
		uWidth = 0;
	}

	// layout AuxObjs attached to this StaffObj
	for (int i=0; i < (int)m_AuxObjs.size(); i++)
	{ 
		//restore this object paper pos.
		pPaper->SetCursorX(uOrg.x);
		pPaper->SetCursorY(uOrg.y);

		m_AuxObjs[i]->Layout(pBox, pPaper, colorC, fHighlight);
	}

    // update paper cursor position
    pPaper->SetCursorX(m_uPaperPos.x + uWidth);
    
}

void lmStaffObj::ShiftObject(lmLUnits uLeft)
{ 
    // update this StaffObj shape position
    if (m_pShape2) m_pShape2->Shift(uLeft, 0.0);    //(uLeft - m_uPaperPos.x, 0.0);

    // shift also AuxObjs attached to this StaffObj
    for (int i=0; i < (int)m_AuxObjs.size(); i++)
    { 
        m_AuxObjs[i]->ShiftObject(uLeft);
    }

}


// default behaviour
void lmStaffObj::SetFont(lmPaper* pPaper)
{
    wxASSERT(m_pVStaff);
    wxASSERT(m_nStaffNum > 0);
    lmStaff* pStaff = m_pVStaff->GetStaff(m_nStaffNum);
    m_pFont = pStaff->GetFontDraw();
}

lmLUnits lmStaffObj::TenthsToLogical(lmTenths nTenths)
{ 
    return m_pVStaff->TenthsToLogical(nTenths, m_nStaffNum);
}

lmTenths lmStaffObj::LogicalToTenths(lmLUnits uUnits)
{ 
    return m_pVStaff->LogicalToTenths(uUnits, m_nStaffNum);
}

void lmStaffObj::AttachAuxObj(lmAuxObj* pAO)
{
    m_AuxObjs.push_back(pAO);
}

void lmStaffObj::DetachAuxObj(lmAuxObj* pAO)
{
    //TODO
}

wxString lmStaffObj::SourceLDP(int nIndent)
{
    // Generate source code for AuxObjs attached to this StaffObj
	wxString sSource = _T("");
    for (int i=0; i < (int)m_AuxObjs.size(); i++)
    { 
        sSource += m_AuxObjs[i]->SourceLDP(nIndent);
    }
	return sSource;
}

wxString lmStaffObj::SourceXML(int nIndent)
{
    // Generate source code for AuxObjs attached to this StaffObj
	wxString sSource = _T("");
    for (int i=0; i < (int)m_AuxObjs.size(); i++)
    { 
        sSource += m_AuxObjs[i]->SourceXML(nIndent);
    }
	return sSource;
}

wxString lmStaffObj::Dump()
{
    // Dump AuxObjs attached to this StaffObj
	wxString sSource = _T("");
    for (int i=0; i < (int)m_AuxObjs.size(); i++)
    { 
        sSource += m_AuxObjs[i]->Dump();
    }
	return sSource;
}

//-------------------------------------------------------------------------------------
// lmScoreObj implementation
//-------------------------------------------------------------------------------------
lmScoreObj::lmScoreObj(lmScoreObj* pParent)
{ 
    m_pParent = pParent;
    m_pObjOptions = (lmObjOptions*)NULL;
}

lmScoreObj::~lmScoreObj()
{ 
    if (m_pObjOptions) delete m_pObjOptions;

}

lmObjOptions* lmScoreObj::GetCurrentObjOptions()
{
    //recurse in the parents chain to find the first non-null CtxObject
    //and return it
    if (m_pObjOptions) return m_pObjOptions;
    if (m_pParent) return m_pParent->GetCurrentObjOptions();
    return (lmObjOptions*)NULL;
}


//Set value for option in this object context. If no context exist, create it

void lmScoreObj::SetOption(wxString sName, long nLongValue)
{
    if (!m_pObjOptions) m_pObjOptions = new lmObjOptions();
    m_pObjOptions->SetOption(sName, nLongValue);
}

void lmScoreObj::SetOption(wxString sName, wxString sStringValue)
{
    if (!m_pObjOptions) m_pObjOptions = new lmObjOptions();
    m_pObjOptions->SetOption(sName, sStringValue);
}

void lmScoreObj::SetOption(wxString sName, double nDoubleValue)
{
    if (!m_pObjOptions) m_pObjOptions = new lmObjOptions();
    m_pObjOptions->SetOption(sName, nDoubleValue);
}

void lmScoreObj::SetOption(wxString sName, bool fBoolValue)
{
    if (!m_pObjOptions) m_pObjOptions = new lmObjOptions();
    m_pObjOptions->SetOption(sName, fBoolValue);
}

//Look for the value of an option. A method for each supported data type.
//Recursive search throug the ObjOptions chain

long lmScoreObj::GetOptionLong(wxString sOptName)
{ 
    return GetCurrentObjOptions()->GetOptionLong(sOptName);
}

double lmScoreObj::GetOptionDouble(wxString sOptName) 
{ 
    return GetCurrentObjOptions()->GetOptionDouble(sOptName); 
}

bool lmScoreObj::GetOptionBool(wxString sOptName) 
{ 
    return GetCurrentObjOptions()->GetOptionBool(sOptName); 
}

wxString lmScoreObj::GetOptionString(wxString sOptName) 
{   
    return GetCurrentObjOptions()->GetOptionString(sOptName);
}


