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


//implementation of the StaffObjs List
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(StaffObjsList);

//implementation of the AuxObjs List
WX_DEFINE_LIST(AuxObjsList);

static int m_IdCounter = 0;        //to assign unique IDs to ScoreObjs

//-------------------------------------------------------------------------------------------------
// lmScoreObj implementation
//-------------------------------------------------------------------------------------------------
lmScoreObj::lmScoreObj(lmObject* pParent, EScoreObjType nType, bool fIsDraggable) :
    lmObject(pParent)
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

    //// GraphicObjs owned by this ScoreObj
    //m_pGraphObjs = (GraphicObjsList*)NULL;

}

lmScoreObj::~lmScoreObj()
{
    //if (m_pGraphObjs) {
    //    m_pGraphObjs->DeleteContents(true);
    //    m_pGraphObjs->Clear();
    //    delete m_pGraphObjs;
    //    m_pGraphObjs = (GraphicObjsList*)NULL;
    //}

}

void lmScoreObj::MoveTo(lmUPoint& uPt)
{
    m_uPaperPos.y = uPt.y;
    m_uPaperPos.x = uPt.x;
}

//// Management of GraphicObjs attached to this object
//
//void lmScoreObj::DoAddGraphicObj(lmScoreObj* pGO)
//{
//    wxASSERT(pGO->GetType() == eSCOT_GraphicObj);
//    if (!m_pGraphObjs) m_pGraphObjs = new GraphicObjsList();
//    m_pGraphObjs->Append((lmGraphicObj*)pGO);
//}
//
//void lmScoreObj::DoRemoveGraphicObj(lmScoreObj* pGO)
//{
//    wxASSERT(pGO->GetType() == eSCOT_GraphicObj);
//}

void lmScoreObj::ShiftObject(lmLUnits uLeft)
{ 
    // update shapes' positions when the object is moved

    if (m_pShape2) m_pShape2->Shift(uLeft, 0.0);    //(uLeft - m_uPaperPos.x, 0.0);
    //wxLogMessage(_T("[lmScoreObj::ShiftObject] shift=%.2f, ID=%d"), uLeft, GetID());
    //m_uPaperPos.x = uLeft;
}


//-------------------------------------------------------------------------------------------------
// lmStaffObj implementation
//-------------------------------------------------------------------------------------------------

lmStaffObj::lmStaffObj(lmObject* pParent, EStaffObjType nType, lmVStaff* pStaff, int nStaff,
                   bool fVisible, bool fIsDraggable) :
    lmScoreObj(pParent, eSCOT_StaffObj, fIsDraggable)
{
    // store parameters
    m_fVisible = fVisible;
    m_nClass = nType;

    //default values
    m_uWidth = 0;

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

void lmStaffObj::Layout(lmBox* pBox, lmPaper* pPaper, wxColour colorC, bool fHighlight)
{
    if (!m_fVisible) return;
    
    if (!m_fFixedPos) {
        m_uPaperPos.x = pPaper->GetCursorX();
        m_uPaperPos.y = pPaper->GetCursorY();
    } else {
        pPaper->SetCursorX(m_uPaperPos.x);
        pPaper->SetCursorY(m_uPaperPos.y);
    }

    // set the font
    SetFont(pPaper);

    // ask derived object to layout itself
    LayoutObject(pBox, pPaper, colorC);

    // layout AuxObjs attached to this StaffObj
    for (int i=0; i < (int)m_AuxObjs.size(); i++)
    { 
        m_AuxObjs[i]->Layout(pBox, pPaper, colorC, fHighlight);
    }

    //// Layout GraphicObjs owned by this StaffObj
    //if (m_pGraphObjs)
    //{
    //    lmGraphicObj* pGO;
    //    wxGraphicObjsListNode* pNode = m_pGraphObjs->GetFirst();
    //    for (; pNode; pNode = pNode->GetNext() ) {
    //        pGO = (lmGraphicObj*)pNode->GetData();
    //        pGO->Layout(pBox, pPaper, colorC, fHighlight);
    //    }
    //}

    // update paper cursor position
    pPaper->SetCursorX(m_uPaperPos.x + m_uWidth);
    
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

//void lmStaffObj::AddGraphicObj(lmGraphicObj* pGO) 
//{ 
//    DoAddGraphicObj(pGO);
//}
//
//void lmStaffObj::RemoveGraphicObj(lmGraphicObj* pGO)
//{ 
//    DoRemoveGraphicObj(pGO);
//}

void lmStaffObj::AttachAuxObj(lmAuxObj* pAO)
{
    m_AuxObjs.push_back(pAO);
}

void lmStaffObj::DetachAuxObj(lmAuxObj* pAO)
{
    //TODO
}


//-------------------------------------------------------------------------------------
// lmObject implementation
//-------------------------------------------------------------------------------------
lmObject::lmObject(lmObject* pParent)
{ 
    m_pParent = pParent;
    m_pObjOptions = (lmObjOptions*)NULL;
}

lmObject::~lmObject()
{ 
    if (m_pObjOptions) delete m_pObjOptions;

}

lmObjOptions* lmObject::GetCurrentObjOptions()
{
    //recurse in the parents chain to find the first non-null CtxObject
    //and return it
    if (m_pObjOptions) return m_pObjOptions;
    if (m_pParent) return m_pParent->GetCurrentObjOptions();
    return (lmObjOptions*)NULL;
}


//Set value for option in this object context. If no context exist, create it

void lmObject::SetOption(wxString sName, long nLongValue)
{
    if (!m_pObjOptions) m_pObjOptions = new lmObjOptions();
    m_pObjOptions->SetOption(sName, nLongValue);
}

void lmObject::SetOption(wxString sName, wxString sStringValue)
{
    if (!m_pObjOptions) m_pObjOptions = new lmObjOptions();
    m_pObjOptions->SetOption(sName, sStringValue);
}

void lmObject::SetOption(wxString sName, double nDoubleValue)
{
    if (!m_pObjOptions) m_pObjOptions = new lmObjOptions();
    m_pObjOptions->SetOption(sName, nDoubleValue);
}

void lmObject::SetOption(wxString sName, bool fBoolValue)
{
    if (!m_pObjOptions) m_pObjOptions = new lmObjOptions();
    m_pObjOptions->SetOption(sName, fBoolValue);
}

//Look for the value of an option. A method for each supported data type.
//Recursive search throug the ObjOptions chain

long lmObject::GetOptionLong(wxString sOptName)
{ 
    return GetCurrentObjOptions()->GetOptionLong(sOptName);
}

double lmObject::GetOptionDouble(wxString sOptName) 
{ 
    return GetCurrentObjOptions()->GetOptionDouble(sOptName); 
}

bool lmObject::GetOptionBool(wxString sOptName) 
{ 
    return GetCurrentObjOptions()->GetOptionBool(sOptName); 
}

wxString lmObject::GetOptionString(wxString sOptName) 
{   
    return GetCurrentObjOptions()->GetOptionString(sOptName);
}


