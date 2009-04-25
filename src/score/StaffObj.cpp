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

#include <math.h>
#include "wx/image.h"
#include "Score.h"
#include "Staff.h"
#include "VStaff.h"
#include "Notation.h"
#include "Context.h"
#include "ObjOptions.h"
#include "UndoRedo.h"
#include "Text.h"
#include "../app/ArtProvider.h"
#include "../graphic/GMObject.h"
#include "../graphic/Shapes.h"
#include "../app/ScoreCanvas.h"
#include "properties/DlgProperties.h"
#include "properties/GeneralProperties.h"

extern bool g_fShowDirtyObjects;        //defined in TheApp.cpp


//-------------------------------------------------------------------------------------
// lmScoreObj implementation
//-------------------------------------------------------------------------------------

lmScoreObj::lmScoreObj(lmScoreObj* pParent)
{
    m_pParent = pParent;
    m_pObjOptions = (lmObjOptions*)NULL;
    m_pAuxObjs = (lmAuxObjsCol*)NULL;

    // initializations: positioning related info
    m_uPaperPos.y = 0.0f,   m_uPaperPos.x = 0.0f;
    m_uComputedPos.x = 0.0f,   m_uComputedPos.y = 0.0f;
	SetDirty(true);                         //any new created object always needs re-layout!
    m_pShapesMngr = new lmShapesMngr();     //default behaviour: only one shape
}

lmScoreObj::~lmScoreObj()
{
    if (m_pObjOptions) delete m_pObjOptions;

    //delete the attached AuxObjs
    if (m_pAuxObjs)
    {
        for (int i=0; i < (int)m_pAuxObjs->size(); i++)
        {
            delete (*m_pAuxObjs)[i];
        }
        m_pAuxObjs->clear();
        delete m_pAuxObjs;
    }

    if (m_pShapesMngr) delete m_pShapesMngr;

}

void lmScoreObj::DefineAsMultiShaped()
{
    delete m_pShapesMngr;
    m_pShapesMngr = new lmMultiShapesMngr();
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


int lmScoreObj::AttachAuxObj(lmAuxObj* pAO, int nIndex)
{
    // Add an AuxObj to the collection of attached AuxObjs. Index is the position
	// that the addedAuxObj must occupy (0..n). If -1, it will be added at the end.

	//add the AuxObj
    if (!m_pAuxObjs)
	{
		wxASSERT(nIndex == -1 || nIndex == 0);
		m_pAuxObjs = new lmAuxObjsCol();
		m_pAuxObjs->push_back(pAO);
		nIndex = 0;
	}
	else if (nIndex == -1 || nIndex == (int)m_pAuxObjs->size())
	{
		m_pAuxObjs->push_back(pAO);
		nIndex = (int)m_pAuxObjs->size() - 1;
	}
	else
	{
		int iN;
		std::vector<lmAuxObj*>::iterator it;
		for (iN=0, it=m_pAuxObjs->begin(); it != m_pAuxObjs->end(); ++it, ++iN)
		{
			if (iN == nIndex)
			{
				//insert before current item
				m_pAuxObjs->insert(it, pAO);
				break;
			}
		}
	}

	//set owner and return index to attached object
    pAO->SetOwner(this);
	return nIndex;
}

int lmScoreObj::DetachAuxObj(lmAuxObj* pAO)
{
    //Remove object from collection. Return index to position it occupied

	int nIdx = 0;
    std::vector<lmAuxObj*>::iterator it;
    for (it = m_pAuxObjs->begin(); it != m_pAuxObjs->end(); ++it, ++nIdx)
	{
		if (*it == pAO) break;
	}
    wxASSERT(it != m_pAuxObjs->end());
    m_pAuxObjs->erase(it);
	return nIdx;
}

lmLocation lmScoreObj::SetUserLocation(lmTenths xPos, lmTenths yPos, int nShapeIdx)
{
    lmLocation tPos;
    tPos.xUnits = lmTENTHS;
    tPos.yUnits = lmTENTHS;
    tPos.x = xPos;
    tPos.y = yPos;

    return SetUserLocation(tPos, nShapeIdx);
}

lmLocation lmScoreObj::SetUserLocation(lmLocation tPos, int nShapeIdx)
{
    //Returns previous location
    //AWARE: Review method SetUserXLocation when doing changes in this one

	//convert location to logical units
	if (tPos.xUnits == lmTENTHS)
		tPos.x = TenthsToLogical(tPos.x);
	if (tPos.yUnits == lmTENTHS)
		tPos.y = TenthsToLogical(tPos.y);

    lmLocation tOldPos = {0.0f, 0.0f, lmLUNITS, lmLUNITS };
    lmGMObject* pGMObj = GetGraphicObject(nShapeIdx);
    if (pGMObj)
    {
		//interactive edition: user is moving an object
	    lmUPoint uShapePos = pGMObj->GetBounds().GetTopLeft();
        lmUPoint uShift(tPos.x - uShapePos.x, tPos.y - uShapePos.y);
        lmUPoint uUserShift = GetUserShift(nShapeIdx);
        tOldPos.x = uShapePos.x;
        tOldPos.y = uShapePos.y;
		uUserShift.x += uShift.x;
		uUserShift.y += uShift.y;
        this->SaveUserLocation(uUserShift.x, uUserShift.y, nShapeIdx);

		////DBG--------------------------------------------------------------------------------
		//if (GetScoreObjType()==lmSOT_ComponentObj) // && ((lmComponentObj*)this)->GetID()==4)
		//{
		//    lmUPoint uShapePos = pGMObj->GetBounds().GetTopLeft();
		//    lmUPoint uShapeOrg = pGMObj->GetOrigin();
		//	  wxLogMessage(_T("[lmScoreObj::SetUserLocation] UserPos=(%.2f, %.2f), ShapeOrg=(%.2f, %.2f), ShapePos=(%.2f, %.2f), old UserShift=(%.2f, %.2f), new UserShift=(%.2f, %.2f)"),
		//				tPos.x, tPos.y, uShapeOrg.x, uShapeOrg.y,
		//				uShapePos.x, uShapePos.y, uShift.x, uShift.y,
		//				m_uUserShift.x, m_uUserShift.y );
		//}
		////END DBG----------------------------------------------------------------------------

		//Move also attached AuxObjs to this ScoreObj
		if (m_pAuxObjs && IsMainShape(nShapeIdx))
		{
			for (int i=0; i < (int)m_pAuxObjs->size(); i++)
			{
				(*m_pAuxObjs)[i]->OnParentMoved(uShift.x, uShift.y);
			}
		}
    }
	else
	{
		//loading the score from a file. Not yet layouted
        this->SaveUserLocation(tPos.x, tPos.y, nShapeIdx);
	}

    //store new position and return previous one
	return tOldPos;
}

lmLUnits lmScoreObj::SetUserXLocation(lmLUnits uxPos, int nShapeIdx)
{
    //Returns previous y location.
    //AWARE: Review method SetUserLocation when doing changes in this one
    //This method is only used during interactive edition. Therefore, the code to be used
    //during file load has been deleted

    lmGMObject* pGMObj = GetGraphicObject(nShapeIdx);
    wxASSERT(pGMObj);

    lmUPoint uShapePos = pGMObj->GetBounds().GetTopLeft();
    lmLUnits uxShift = uxPos - uShapePos.x;
    lmLUnits uxUserShift = GetUserShift(nShapeIdx).x + uxShift;
    lmLUnits uxOldPos = uShapePos.x;
    this->SaveUserXLocation(uxUserShift, nShapeIdx);

	//Move also attached AuxObjs to this ScoreObj
	if (m_pAuxObjs && IsMainShape(nShapeIdx))
	{
		for (int i=0; i < (int)m_pAuxObjs->size(); i++)
		{
			(*m_pAuxObjs)[i]->OnParentMoved(uxShift, 0.0f);
		}
	}

    //return previous y position
	return uxOldPos;
}

lmUPoint lmScoreObj::SetUserLocation(lmUPoint uNewPos, int nShapeIdx)
{
    //Returns previous location.
    //This method is only used during interactive edition
    //AWARE: Review method SetUserLocation when doing changes in this one

    lmGMObject* pGMObj = GetGraphicObject(nShapeIdx);
    wxASSERT(pGMObj);

	//interactive edition: user is moving an object
	lmUPoint uShapePos = pGMObj->GetBounds().GetTopLeft();
    lmUPoint uShift(uNewPos - uShapePos);
    lmUPoint uUserShift = GetUserShift(nShapeIdx) + uShift;
    lmUPoint uOldPos = uShapePos;
    this->SaveUserLocation(uUserShift.x, uUserShift.y, nShapeIdx);

	//Move also attached AuxObjs to this ScoreObj
	if (m_pAuxObjs && IsMainShape(nShapeIdx))
	{
		for (int i=0; i < (int)m_pAuxObjs->size(); i++)
		{
			(*m_pAuxObjs)[i]->OnParentMoved(uShift.x, uShift.y);
		}
	}

    //return previous position
	return uOldPos;
}

void lmScoreObj::MoveObjectPoints(int nNumPoints, int nShapeIdx, lmUPoint* pShifts,
                                  bool fAddShifts)
{
    //This method is only used during interactive edition.
    //It receives a vector with the shifts for object points and a flag to signal
    //whether to add or to substract shifts.

    //wxASSERT( nNumPoints == (int)pNewPoints->size() );

    //lmGMObject* pGMObj = GetGraphicObject(nShapeIdx);
    //wxASSERT(pGMObj);

    //for(int i=0; i < nNumPoints; i++)
    //{
    //    if (fAddShifts)
    //    {
    //        m_tPoint[i].x += m_pParent->LogicalToTenths(pShifts->at(i).x);
    //        m_tPoint[i].y += m_pParent->LogicalToTenths(pShifts->at(i).y);
    //    }
    //    else
    //    {
    //        m_tPoint[i].x -= m_pParent->LogicalToTenths(pShifts->at(i).x);
    //        m_tPoint[i].y -= m_pParent->LogicalToTenths(pShifts->at(i).y);
    //    }
    //}

    ////inform the shape
    //lmShapeLine* pShape = (lmShapeLine*)GetShape(nShapeIdx);
    //if (pShape)
    //    pShape->UpdatePoints(nNumPoints, pShifts, fAddShifts);
}

void lmScoreObj::StoreOriginAndShiftShapes(lmLUnits uxShift, int nShapeIdx)
{
    //This method is invoked only from TimeposTable module, from methods 
    //lmTimeLine::ShiftEntries() and lmTimeLine::Reposition(), during auto-layout
    //computations.
    //By invoking this method, the auto-layout algorithm is informing about a change in
    //the computed final position for this ScoreObj.
    //Be aware of the fact that this method can be invoked several times for the
    //same ScoreObj, when the auto-layout algorithm refines the final position.

    lmGMObject* pGMObj = GetGraphicObject(nShapeIdx);
    if (pGMObj)
    {
		////DBG--------------------------------------------------------------------------------
		////if (GetScoreObjType()==lmSOT_ComponentObj && ((lmComponentObj*)this)->GetID()==1)
		//if (GetScoreObjType()==lmSOT_ComponentObj
  //          && ((lmComponentObj*)this)->GetType()==lm_eStaffObj
  //          && ((lmStaffObj*)this)->IsClef() )
		//{
		//	lmUPoint uNewOrg = m_uComputedPos + m_uUserShift;
		//	wxLogMessage(_T("[lmScoreObj::StoreOriginAndShiftShapes] uxShift=%.2f, ShapeIdx=%d"),
		//				uxShift, nShapeIdx );
		//}
		////END DBG----------------------------------------------------------------------------
        
        pGMObj->Shift(uxShift, 0.0f);
        pGMObj->ApplyUserShift( this->GetUserShift(nShapeIdx) );
    }

	// inform about the change to AuxObjs attached to this StaffObj
    if (m_pAuxObjs && this->IsMainShape(nShapeIdx))
    {
        for (int i=0; i < (int)m_pAuxObjs->size(); i++)
        {
            (*m_pAuxObjs)[i]->OnParentComputedPositionShifted(uxShift, 0.0f);
        }
    }

}

void lmScoreObj::PopupMenu(lmController* pCanvas, lmGMObject* pGMO, const lmDPoint& vPos)
{
	wxMenu* pMenu = pCanvas->GetContextualMenu();
	if (!pMenu)
		wxMessageBox(pGMO->Dump(0));
	else
	{
		CustomizeContextualMenu(pMenu, pGMO);
		pCanvas->ShowContextualMenu(this, pGMO, pMenu, vPos.x, vPos.y);
	}
}

void lmScoreObj::CustomizeContextualMenu(wxMenu* pMenu, lmGMObject* pGMO)
{
    // Add 'Attach text' item if it is a StaffObj or a AuxObj
    if (this->IsComponentObj())
    {
#if defined(__WXMSW__) || defined(__WXGTK__)

		wxMenuItem* pItem;
		wxSize nIconSize(16, 16);

        pMenu->AppendSeparator();

		pItem = new wxMenuItem(pMenu, lmPOPUP_AttachText, _("Attach text"));
		pItem->SetBitmap( wxArtProvider::GetBitmap(_T("tool_add_text"), wxART_TOOLBAR, nIconSize) );
		pMenu->Append(pItem);

#else
        pMenu->AppendSeparator();
        pMenu->Append(lmPOPUP_AttachText, _("Attach text"));

#endif
    }
}

void lmScoreObj::OnProperties(lmController* pController, lmGMObject* pGMO)
{
	if(!pGMO)
		wxMessageBox(_T("Nothing selected!"));
	else
	{
		lmDlgProperties dlg(pController);

		//give opportunity to derived classes to add specific panels
		OnEditProperties(&dlg);

		//prepare dialog and show it
		dlg.Layout();
		if (dlg.ShowModal() == wxID_OK)
            OnPropertiesChanged();      //inform object
	}
}

void lmScoreObj::OnEditProperties(lmDlgProperties* pDlg, const wxString& sTabName)
{
	pDlg->AddPanel( new lmGeneralProperties(pDlg->GetNotebook(), this, this->GetScore()),
				_("General"));
}

int lmScoreObj::GetPageNumber()
{
    //For visual highlight we need to know the page in wich the StaffObj to highlight
    //is located. To get it we are going to access this object main shape.
	//Returns the page number in whith the shape for this ScoreObj is rendered
	//if no shape returns 0

    lmShape* pGMObj = GetShape();
	if (!pGMObj) return 0;
	return pGMObj->GetPageNumber();
}

lmShape* lmScoreObj::GetShape(int nStaff)
{
    //Single shape ScoreObjs use a simple shape manager. Idx is ignored as there is
    //only one shape.
    //There are two behaviours for multi-shape ScoreObjs (clefs, time & key signatures). 
    //For clefs, there is a shape for each system. Real object corresponds to Idx=0
    //For key signatures there is a shape for each staff and system. Indexes are 
    //computed as nIdx = (nSystem -1) * numStaves + nStaff - 1
    //For time signatures there is a shape only for each staff in first system. Indexes
    //are computed as nIdx = nStaff - 1

    //This is the basic implementation, is valid for single shape ScoreObjs and for
    //time & key signatures. Therfore, it is overriden by Clefs

    wxASSERT(nStaff > 0);

    return (lmShape*)m_pShapesMngr->GetGraphicObject(nStaff - 1);
}

wxFont* lmScoreObj::GetSuitableFont(lmPaper* pPaper)
{
	//returns the font to use to render this ScoreObj
	return (wxFont*)NULL;
}

lmUPoint lmScoreObj::SetReferencePos(lmPaper* pPaper)
{
    m_uPaperPos.x = pPaper->GetCursorX();
    m_uPaperPos.y = pPaper->GetCursorY();
	return m_uPaperPos;
}

void lmScoreObj::SetReferencePos(lmUPoint& uPos)
{
    m_uPaperPos = uPos;
}

wxString lmScoreObj::Dump()
{
	wxString sDump = _T("");

    ////position info
    //sDump += wxString::Format(_T(", ComputedPos=(%.2f, %.2f), UserShift=(%.2f, %.2f)"),
    //                m_uComputedPos.x, m_uComputedPos.y, m_uUserShift.x, m_uUserShift.y );
	return sDump;
}

wxString lmScoreObj::SourceLDP(int nIndent)
{
	wxString sSource = _T("");

    //TODO: Code for multi-shaped objects

	//location
    lmUPoint uUserShift = this->GetUserShift(0);
    if (uUserShift.x != 0.0f)
		sSource += wxString::Format(_T(" dx:%s"),
					DoubleToStr((double)m_pParent->LogicalToTenths(uUserShift.x), 4).c_str() );

	if (uUserShift.y != 0.0f)
		sSource += wxString::Format(_T(" dy:%s"),
					DoubleToStr((double)m_pParent->LogicalToTenths(uUserShift.y), 4).c_str() );

	return sSource;
}

wxString lmScoreObj::SourceXML(int nIndent)
{
    //TODO: Code for SourceXML
	wxString sSource = _T("");
	return sSource;
}

void lmScoreObj::SetDirty(bool fValue, bool fPropagate)
{
    m_fDirty = fValue;

	// propagate to attached AuxObjs
    if (fPropagate && m_pAuxObjs)
    {
	    for (int i=0; i < (int)m_pAuxObjs->size(); i++)
	    {
		    (*m_pAuxObjs)[i]->SetDirty(fValue, fPropagate);
	    }
    }
}

void lmScoreObj::PrepareToCreateShapes() 
{ 
    m_pShapesMngr->Init( IsDirty() );
}

lmAuxObj* lmScoreObj::AttachTextBox(lmTPoint& ntBoxPos, lmTPoint& ntLinePos, wxString& sText,
                                    lmTextStyle* pTextStyle, wxSize size, wxColour nBgColor) 
{ 
    //wrapper method to encapsulate and simplify operations related to score creation by program.
    //This method creates and attaches a textbox

    lmScoreTextParagraph* pTextBox = new lmScoreTextParagraph(size.x, size.y, ntBoxPos);
    this->AttachAuxObj(pTextBox);
    lmBaseText* pBText = new lmBaseText(sText, pTextStyle);
    pTextBox->InsertTextUnit(pBText);
    pTextBox->SetBgColour(nBgColor);

    lmLocation tPos;
    tPos.x = ntLinePos.x;
    tPos.y = ntLinePos.y;
    pTextBox->AddAnchorLine(tPos, 1.0f, lm_eLine_Solid, lm_eEndLine_None, *wxBLACK);

    return pTextBox;
}



//-------------------------------------------------------------------------------------------------
// lmComponentObj implementation
//-------------------------------------------------------------------------------------------------

//implementation of the StaffObjs List
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(StaffObjsList);

//implementation of the AuxObjs List
WX_DEFINE_LIST(AuxObjsList);

static int m_IdCounter = 0;        //to assign unique IDs to ComponentObjs

lmComponentObj::lmComponentObj(lmScoreObj* pParent, lmEComponentObjType nType, bool fIsDraggable)
    : lmScoreObj(pParent)
{
    m_nId = m_IdCounter++;      // give it an ID
    m_nType = nType;            // save type

    // behaviour
    m_fIsDraggable = fIsDraggable;

    //other
    m_color = *wxBLACK;
}

lmComponentObj::~lmComponentObj()
{
}

lmUPoint lmComponentObj::ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper)
{
	// if no location is specified in LDP source file, this method is invoked from
	// base class to ask derived object to compute a suitable position to
	// place itself.
	// uOrg is the assigned paper position for this object.

	return uOrg;
}



//-------------------------------------------------------------------------------------------------
// lmStaffObj implementation
//-------------------------------------------------------------------------------------------------

lmStaffObj::lmStaffObj(lmScoreObj* pParent, EStaffObjType nType, lmVStaff* pStaff, int nStaff,
                   bool fVisible, bool fIsDraggable)
    : lmComponentObj(pParent, lm_eStaffObj, fIsDraggable)
    , m_fVisible(fVisible)
    , m_nClass(nType)
    , m_pVStaff(pStaff)
    , m_nStaffNum(pStaff ? nStaff : 0)
	, m_pPrevSO((lmStaffObj*) NULL)
	, m_pNextSO((lmStaffObj*) NULL)
{
    wxASSERT(nStaff > 0);
    SetLayer(lm_eLayerNotes);
}

lmStaffObj::~lmStaffObj()
{
}

lmScore* lmStaffObj::GetScore()
{ 
    return m_pVStaff->GetScore(); 
}

lmStaff* lmStaffObj::GetStaff() 
{ 
    return m_pVStaff->GetStaff(m_nStaffNum);
}

void lmStaffObj::Layout(lmBox* pBox, lmPaper* pPaper, bool fHighlight)
{
    PrepareToCreateShapes();

    //save current paper position in m_uPaperPos and returns it
	lmUPoint uOrg = SetReferencePos(pPaper);        //here uPos == m_uPaperPos

    //ask object to compute a suitable positio. Normally it returns m_uPaperPos
    m_uComputedPos = ComputeBestLocation(m_uPaperPos, pPaper);

    wxColour color = (g_fShowDirtyObjects && IsDirty() ? *wxRED : m_color);

	lmLUnits uWidth = 0;
    if (m_fVisible || IsMultishaped())
    {
        if (lmPRESERVE_SHAPES && !IsDirty() && GetShape(1) &&
            !IsClef() && !IsKeySignature() && !IsTimeSignature() )
        {
            //Not dirty: just add existing shapes to the Box
            lmShape* pOldShape = this->GetShape();
            pBox->AddShape(pOldShape, GetLayer());
            pOldShape->SetColour(*wxCYAN);//colorC);       //change its colour to new desired colour
        }
        else
        {
            //add shapes for this object
            uWidth = LayoutObject(pBox, pPaper, m_uComputedPos, color);
        }
    }
	else
	{
		//Create invisible shapes, to store the StaffObj position
        CreateInvisibleShape(pBox, uOrg, 0);
	}

	// layout AuxObjs attached to this StaffObj
    if (m_pAuxObjs)
    {
	    for (int i=0; i < (int)m_pAuxObjs->size(); i++)
	    {
		    //assign m_uComputedPos as paper pos. for this AuxObj
		    pPaper->SetCursorX(m_uComputedPos.x);
		    pPaper->SetCursorY(m_uComputedPos.y);

            if (!(*m_pAuxObjs)[i]->IsRelObj())
		        (*m_pAuxObjs)[i]->Layout(pBox, pPaper, fHighlight);
            else if ((lmNoteRest*)this == ((lmBinaryRelObj*)(*m_pAuxObjs)[i])->GetEndNoteRest())
		        (*m_pAuxObjs)[i]->Layout(pBox, pPaper, fHighlight);
	    }
    }

    // update paper cursor position
    pPaper->SetCursorX(uOrg.x + uWidth);
}

lmShape* lmStaffObj::CreateInvisibleShape(lmBox* pBox, lmUPoint uPos, int nShapeIdx)
{
    //create an invisible shape

	lmShapeInvisible* pShape = new lmShapeInvisible(this, nShapeIdx, uPos, lmUSize(0.0, 0.0) );
	pBox->AddShape(pShape, GetLayer());
	StoreShape(pShape);
    return pShape;
}

wxFont* lmStaffObj::GetSuitableFont(lmPaper* pPaper)
{
	WXUNUSED(pPaper);
    wxASSERT(m_pVStaff);
    wxASSERT(m_nStaffNum > 0);
    lmStaff* pStaff = m_pVStaff->GetStaff(m_nStaffNum);
    return pStaff->GetFontDraw();
}

lmLUnits lmStaffObj::TenthsToLogical(lmTenths nTenths)
{
    return m_pVStaff->TenthsToLogical(nTenths, m_nStaffNum);
}

lmTenths lmStaffObj::LogicalToTenths(lmLUnits uUnits)
{
    return m_pVStaff->LogicalToTenths(uUnits, m_nStaffNum);
}

lmContext* lmStaffObj::GetCurrentContext(int nStaff)
{
	// Returns the context that is applicable to the this StaffObj.
	// AWARE: Only Clef, key signature and time signature are updated. To get
	//	applicable accidentals use NewUpdatedContext() instead.
	return m_pVStaff->GetCurrentContext(this, nStaff);
}

lmContext* lmStaffObj::NewUpdatedContext()
{
	//returns the applicable context for this StaffObj, updated with all
	//accidentals introduced by previous notes
	return m_pVStaff->NewUpdatedContext(this->GetStaffNum(), this);
}

lmEClefType lmStaffObj::GetApplicableClefType()
{
    //returns the clef applicable to this staffobj, if any

    lmContext* pContext = GetCurrentContext();
    if (pContext)
        return pContext->GetClefType();
    else
        return lmE_Undefined;
}

lmClef* lmStaffObj::GetApplicableClef()
{
    //returns the clef applicable to this staffobj, if any

    lmContext* pContext = GetCurrentContext();
    if (pContext)
        return pContext->GetClef();
    else
        return (lmClef*)NULL;
}

lmTimeSignature* lmStaffObj::GetApplicableTimeSignature()
{
    //returns the TS applicable to this staffobj, if any

    lmContext* pContext = GetCurrentContext();
    if (pContext)
        return pContext->GetTime();
    else
        return (lmTimeSignature*)NULL;
}

lmKeySignature* lmStaffObj::GetApplicableKeySignature()
{
    //returns the Key Signature applicable to this staffobj, if any

    lmContext* pContext = GetCurrentContext();
    if (pContext)
        return pContext->GetKey();
    else
        return (lmKeySignature*)NULL;
}


wxString lmStaffObj::SourceLDP(int nIndent)
{
	wxString sSource = _T("");

    //Anchor notations doesn't have a source LDP element. Therefore, only attached AuxObjs must
    //be generated
    if (!IsNotation() || !( ((lmNotation*)this)->IsAnchor() || ((lmNotation*)this)->IsScoreAnchor() ))
    {
        //staff num
        if (m_pVStaff->GetNumStaves() > 1
            && !IsKeySignature()            //KS, TS & barlines are common to all staves.
            && !IsTimeSignature()
            && !IsBarline() )
        {
            sSource += wxString::Format(_T(" p%d"), m_nStaffNum);
        }
        
        //visible?
        if (!m_fVisible) { sSource += _T(" noVisible"); }
    }

    // Generate source code for AuxObjs attached to this StaffObj
    if (m_pAuxObjs)
    {
		nIndent++;
        for (int i=0; i < (int)m_pAuxObjs->size(); i++)
        {
            if ( (*m_pAuxObjs)[i]->IsRelObj() )
            {
                lmRelObj* pRO = (lmRelObj*)(*m_pAuxObjs)[i];
                if ( pRO->GetStartNoteRest() == (lmNoteRest*)this )
                    sSource += pRO->SourceLDP_First(nIndent);
                else if ( pRO->GetEndNoteRest() == (lmNoteRest*)this )
                    sSource += pRO->SourceLDP_Last(nIndent);
                else
                    sSource += pRO->SourceLDP_Middle(nIndent);
            }
            else
                sSource += (*m_pAuxObjs)[i]->SourceLDP(nIndent);
        }
		nIndent--;
    }

    //base class info
    sSource += lmScoreObj::SourceLDP(nIndent);
	return sSource;
}

wxString lmStaffObj::SourceXML(int nIndent)
{
    // Generate source code for AuxObjs attached to this StaffObj
	wxString sSource = _T("");
    if (m_pAuxObjs)
    {
        for (int i=0; i < (int)m_pAuxObjs->size(); i++)
        {
            sSource += (*m_pAuxObjs)[i]->SourceXML(nIndent);
        }
    }
	return sSource;
}

wxString lmStaffObj::Dump()
{
    //staff
    wxString sDump = wxString::Format(_T(", staff=%d"), m_nStaffNum);

    //base class info
	sDump += lmScoreObj::Dump();

    // Dump AuxObjs attached to this StaffObj
    if (m_pAuxObjs)
    {
        for (int i=0; i < (int)m_pAuxObjs->size(); i++)
        {
            sDump += (*m_pAuxObjs)[i]->Dump();
        }
    }
	return sDump;
}




//-------------------------------------------------------------------------------------------------
// lmShapesMngr implementation
//-------------------------------------------------------------------------------------------------

lmShapesMngr::lmShapesMngr()
{
    m_uUserShift = lmUPoint(0.0f, 0.0f);
    m_pGMObj = (lmShape*)NULL;
}

lmShapesMngr::~lmShapesMngr()
{
    if (lmPRESERVE_SHAPES && m_pGMObj)
        delete m_pGMObj;
}

void lmShapesMngr::StoreShape(lmGMObject* pGMObj)
{ 
    if (lmPRESERVE_SHAPES && m_pGMObj) delete m_pGMObj;
    m_pGMObj = pGMObj;
}

lmGMObject* lmShapesMngr::GetGraphicObject(int nShapeIdx)
{
    //default implementation assumes that an ScoreObj only has
    //one associated grapic object, ant it is pointed by m_pGMObj

    WXUNUSED(nShapeIdx);
    return m_pGMObj;
}

void lmShapesMngr::SaveUserLocation(lmLUnits xPos, lmLUnits yPos, int nShapeIdx)
{
    //default implementation for virtual method.
    //It assumes that ScoreObj only has a shape. 

    WXUNUSED(nShapeIdx);
	m_uUserShift.x = xPos;
	m_uUserShift.y = yPos;
}

void lmShapesMngr::SaveUserXLocation(lmLUnits xPos, int nShapeIdx)
{
    //default implementation for virtual method.
    //It assumes that ScoreObj only has a shape. 

    WXUNUSED(nShapeIdx);
	m_uUserShift.x = xPos;
}


lmUPoint lmShapesMngr::GetUserShift(int nShapeIdx)
{
    //default implementation for virtual method.
    //It assumes that ScoreObj only has a shape. 

    WXUNUSED(nShapeIdx);
    return m_uUserShift;
}





//-------------------------------------------------------------------------------------------------
// lmMultiShapesMngr implementation
//-------------------------------------------------------------------------------------------------

lmMultiShapesMngr::lmMultiShapesMngr()
{
    Init(false);
}

lmMultiShapesMngr::~lmMultiShapesMngr()
{
    std::vector<lmShapeInfo*>::iterator it = m_ShapesInfo.begin();
    while (it != m_ShapesInfo.end())
    {
        if (lmPRESERVE_SHAPES)
            delete (*it)->pGMObj;
        delete *it;
        ++it;
    }
    m_ShapesInfo.clear();
}

void lmMultiShapesMngr::Init(bool fDeleteShapes)
{
    // AWARE. VERY IMPORTANT:
    // Entries to shapes vector are also added when invoking SaveUserLocation().
    // During LDP parsing, if a user position is specified, method [created object]->SaveUserLocation()
    // is invoked and it, in turn, invokes lmShapesMngr::SaveUserLocation(). But at this point the
    // shape is not yet created. Therefore, empty entries are added to the shapes table.
    // This method (Init) is invoked from lmScoreObj::Layout, when preparing to create the shapes.
    // Therefore, here we can not clear the vector, as it might contain valid information stored
    // in uUserShift. 
    // This method must JUST remove the unused shapes/shapes pointers and reset indexes counter.

    m_nNextIdx = 0;

    if (lmPRESERVE_SHAPES && !fDeleteShapes) return;

    //delete shapes and remove pointers to them
    std::vector<lmShapeInfo*>::iterator it = m_ShapesInfo.begin();
    while (it != m_ShapesInfo.end())
    {
        if (lmPRESERVE_SHAPES)
            delete (*it)->pGMObj;
        (*it)->pGMObj = (lmGMObject*)NULL;
        ++it;
    }
}

void lmMultiShapesMngr::StoreShape(lmGMObject* pGMObj)
{
    int nIdx = pGMObj->GetOwnerIDX();

    if (nIdx == (int)m_ShapesInfo.size())
    {
        lmShapeInfo* pShapeInfo = new lmShapeInfo;
        pShapeInfo->pGMObj = pGMObj;
        pShapeInfo->uUserShift = lmUPoint(0.0f, 0.0f);
        m_ShapesInfo.push_back(pShapeInfo);
    }
    else
    {
        lmGMObject* pShape = m_ShapesInfo[nIdx]->pGMObj;
        //wxASSERT(!lmPRESERVE_SHAPES && pShape == (lmGMObject*)NULL);
        if (lmPRESERVE_SHAPES && pShape) delete pShape;
        m_ShapesInfo[nIdx]->pGMObj = pGMObj;
    }
}

lmGMObject* lmMultiShapesMngr::GetGraphicObject(int nShapeIdx)
{
    //For prolog shapes, when object is not dirty, it might happen that a new sytem is added to
    //the score. When trying to reuse the shapes, they will not exists for the new system. Therefore,
    //if ndx >= vector.size it must return NULL and this will force to recreate it.

    if (nShapeIdx >= (int)m_ShapesInfo.size())
        return (lmGMObject*)NULL;
    else
        return m_ShapesInfo[nShapeIdx]->pGMObj;
}

void lmMultiShapesMngr::SaveUserLocation(lmLUnits xPos, lmLUnits yPos, int nShapeIdx)
{
    //if necessary, create empty shapes info entries
    int nToAdd = nShapeIdx - (int)m_ShapesInfo.size() + 1;
    for (int i=0; i < nToAdd; ++i)
    {
        lmShapeInfo* pShapeInfo = new lmShapeInfo;
        pShapeInfo->pGMObj = (lmGMObject*)NULL;
        pShapeInfo->uUserShift = lmUPoint(0.0f, 0.0f);
        m_ShapesInfo.push_back(pShapeInfo);
    }

    //save new user position
    m_ShapesInfo[nShapeIdx]->uUserShift = lmUPoint(xPos, yPos);
}

void lmMultiShapesMngr::SaveUserXLocation(lmLUnits xPos, int nShapeIdx)
{
    //if necessary, create empty shapes info entries
    int nToAdd = nShapeIdx - (int)m_ShapesInfo.size() + 1;
    for (int i=0; i < nToAdd; ++i)
    {
        lmShapeInfo* pShapeInfo = new lmShapeInfo;
        pShapeInfo->pGMObj = (lmGMObject*)NULL;
        pShapeInfo->uUserShift = lmUPoint(0.0f, 0.0f);
        m_ShapesInfo.push_back(pShapeInfo);
    }

    //save new user position
    m_ShapesInfo[nShapeIdx]->uUserShift.x = xPos;
}

lmUPoint lmMultiShapesMngr::GetUserShift(int nShapeIdx)
{
    wxASSERT(nShapeIdx < (int)m_ShapesInfo.size());
    return m_ShapesInfo[nShapeIdx]->uUserShift;
}
