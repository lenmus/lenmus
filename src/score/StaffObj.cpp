//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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
#include "ObjOptions.h"
#include "UndoRedo.h"
#include "../graphic/GMObject.h"
#include "../graphic/Shapes.h"
#include "../app/ScoreCanvas.h"


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
	m_tSrcPos = g_tDefaultPos;
    m_uComputedPos.x = 0.0f,   m_uComputedPos.y = 0.0f;
    m_uUserShift.x = 0.0f,   m_uUserShift.y = 0.0f;

    m_pShape = (lmShape*)NULL;
	m_fModified = false;
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


int lmScoreObj::AttachAuxObj(lmAuxObj* pAO)
{
    //return index to attached object

    if (!m_pAuxObjs) m_pAuxObjs = new lmAuxObjsCol();
    m_pAuxObjs->push_back(pAO);
    pAO->SetOwner(this);

	//return index to attached object
	return (int)m_pAuxObjs->size() - 1;
}

void lmScoreObj::DetachAuxObj(lmAuxObj* pAO)
{
    //TODO
}

lmLocation lmScoreObj::SetUserLocation(lmLocation tPos)
{
	//convert location to logical units
	if (tPos.xUnits == lmTENTHS)
		tPos.x = TenthsToLogical(tPos.x);
	if (tPos.yUnits == lmTENTHS)
		tPos.y = TenthsToLogical(tPos.y);

    if (m_pShape)
    {
		//interactive edition: user is moving an object
		lmUPoint uOldShapePos = m_pShape->GetBounds().GetTopLeft();
		m_uUserShift.x += tPos.x - uOldShapePos.x;
		m_uUserShift.y += tPos.y - uOldShapePos.y;
		//DBG--------------------------------------------------------------------------------
		if (GetScoreObjType()==lmSOT_ComponentObj && ((lmComponentObj*)this)->GetID()==4)
		{
			wxLogMessage(_T("[lmScoreObj::SetUserLocation] UserPos=(%.2f, %.2f), old ShapePos=(%.2f, %.2f), old UserShift=(%.2f, %.2f), new UserShift=(%.2f, %.2f)"),
						tPos.x, tPos.y, uOldShapePos.x, uOldShapePos.y,
						tPos.x - uOldShapePos.x, tPos.y - uOldShapePos.y,
						m_uUserShift.x, m_uUserShift.y );
		}
		//END DBG----------------------------------------------------------------------------

		//Move also attached AuxObjs to this ScoreObj
		if (m_pAuxObjs)
		{
			for (int i=0; i < (int)m_pAuxObjs->size(); i++)
			{
				(*m_pAuxObjs)[i]->OnParentMoved(tPos.x - uOldShapePos.x, tPos.y - uOldShapePos.y);
			}
		}
    }
	else
	{
		//loading the score from a file. Not yet layouted
		m_uUserShift.x = tPos.x;
		m_uUserShift.y = tPos.y;
	}

	m_tSrcPos = m_tPos;
	//m_tPos = tPos;
	return m_tSrcPos;
}

void lmScoreObj::ResetObjectLocation()
{
	m_tPos = m_tSrcPos;

 //   // X position
 //   wxString sType = _T("");
	//switch (m_tPos.xType) {
	//	case lmLOCATION_DEFAULT:		sType = _T("LOCATION_DEFAULT"); break;
	//	case lmLOCATION_COMPUTED:		sType = _T("LOCATION_COMPUTED"); break;
	//	case lmLOCATION_USER_RELATIVE:	sType = _T("LOCATION_USER_RELATIVE"); break;
	//	case lmLOCATION_USER_ABSOLUTE:	sType = _T("LOCATION_USER_ABSOLUTE"); break;
	//	default:
	//		sType = wxString::Format(_T("LOCATION %d"), m_tPos.xType);
	//}

	//wxString sSource = wxString::Format(_T("x: %.2f %.2f - "), m_tPos.x, m_tPos.xUnits);
	//sSource += sType;

	//// Y position
 //   sType = _T("");
	//switch (m_tPos.yType) {
	//	case lmLOCATION_DEFAULT:		sType = _T("LOCATION_DEFAULT"); break;
	//	case lmLOCATION_COMPUTED:		sType = _T("LOCATION_COMPUTED"); break;
	//	case lmLOCATION_USER_RELATIVE:	sType = _T("LOCATION_USER_RELATIVE"); break;
	//	case lmLOCATION_USER_ABSOLUTE:	sType = _T("LOCATION_USER_ABSOLUTE"); break;
	//	default:
	//		sType = wxString::Format(_T("LOCATION %d"), m_tPos.yType);
	//}

	//sSource += wxString::Format(_T("  /  y: %.2f %.2f - %s\n"), m_tPos.y, m_tPos.yUnits, sType.c_str());

	//wxLogMessage(sSource);
}

void lmScoreObj::StoreOriginAndShiftShapes(lmLUnits uxShift)
{
    //The ScoreObj position is being computed in auto-layout procedure and the auto-layout
    //algorithm is invoking this method to inform about the computed final position for 
    //this ScoreObj. Take into account that this method can be invoked several times for the
    //same ScoreBoj, as the auto-layout algorithm refines the final position.
    //In this method, we can choose either to move the shape to the requested position or
    //to any other (i.e. the one requested by the user), and that has no influence on the
    //auto-layout computations.
    //This method is invoked only from TimeposTable module, from methods 
    //lmTimeLine::ShiftEntries() and lmTimeLine::Reposition()

	m_uComputedPos.x += uxShift;
    if (m_pShape)
    {
		//DBG--------------------------------------------------------------------------------
		if (GetScoreObjType()==lmSOT_ComponentObj && ((lmComponentObj*)this)->GetID()==3)
		{
			lmUPoint uNewOrg = m_uComputedPos + m_uUserShift;
			wxLogMessage(_T("[lmScoreObj::StoreOriginAndShiftShapes] uxShift=%.2f, ShapeOrg=(%.2f, %.2f), ComputedPos=(%.2f, %.2f), UserShift=(%.2f, %.2f), NewOrg=(%.2f, %.2f)"),
						uxShift,
						m_pShape->GetOrigin().x, m_pShape->GetOrigin().y,
						m_uComputedPos.x, m_uComputedPos.y, m_uUserShift.x, m_uUserShift.y,
						uNewOrg.x, uNewOrg.y );
		}
		//END DBG----------------------------------------------------------------------------
        m_pShape->ShiftOrigin(m_uComputedPos + m_uUserShift);
    }

	// shift also AuxObjs attached to this StaffObj
    if (m_pAuxObjs)
    {
        for (int i=0; i < (int)m_pAuxObjs->size(); i++)
        {
            (*m_pAuxObjs)[i]->StoreOriginAndShiftShapes(uxShift);
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
    //pMenu->Append(Menu_Help_About, _T("&About"));
    //pMenu->Append(Menu_Popup_Submenu, _T("&Submenu"), CreateDummyMenu(NULL));
    //pMenu->Append(Menu_Popup_ToBeDeleted, _T("To be &deleted"));
    //pMenu->AppendCheckItem(Menu_Popup_ToBeChecked, _T("To be &checked"));
    //pMenu->Append(Menu_Popup_ToBeGreyed, _T("To be &greyed"),
    //            _T("This menu item should be initially greyed out"));
    //pMenu->AppendSeparator();
    //pMenu->Append(Menu_File_Quit, _T("E&xit"));

    //pMenu->Delete(Menu_Popup_ToBeDeleted);
    //pMenu->Check(Menu_Popup_ToBeChecked, true);
    //pMenu->Enable(Menu_Popup_ToBeGreyed, false);

}

void lmScoreObj::OnProperties(lmGMObject* pGMO)
{
	//TODO: FIX_ME: pGMO is uselless here because whe the pop-up menu is removed
	//the score could be re-layouted and the pGMO become invalid
	//wxMessageBox(_T("Properties of this"));
	if(!GetShap2())
		wxMessageBox(_T("Nothing selected!"));
	else
		wxMessageBox(GetShap2()->Dump(0));
}

int lmScoreObj::GetPageNumber()
{
    //For visual highlight we need to know the page in wich the StaffObj to highlight
    //is located. To get it we are going to access this object main shape.
	//Returns the page number in whith the shape for this ScoreObj is rendered
	//if no shape returns 0

	if (!m_pShape) return 0;
	return m_pShape->GetPageNumber();
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

void lmScoreObj::RecordHistory(lmUndoData* pUndoData) 
{
	if (!m_fModified) return;

	//once the changes are logged, consolidate new state
	AcceptChanges();
	m_fModified = false;
}

void lmScoreObj::AcceptChanges() 
{
	m_fModified = false;
}

wxString lmScoreObj::Dump()
{
	wxString sDump = _T("");

    //position info
    sDump += wxString::Format(_T(", ComputedPos=(%.2f, %.2f), UserShift=(%.2f, %.2f)"),
                    m_uComputedPos.x, m_uComputedPos.y, m_uUserShift.x, m_uUserShift.y );
	return sDump;
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

lmComponentObj::lmComponentObj(lmScoreObj* pParent, EScoreObjType nType, lmLocation* pPos,
                               bool fIsDraggable)
    : lmScoreObj(pParent)
{
    m_nId = m_IdCounter++;      // give it an ID
    m_nType = nType;            // save type
    m_tPos = *pPos;

    // behaviour
    m_fIsDraggable = fIsDraggable;
}

lmComponentObj::~lmComponentObj()
{
}

wxString lmComponentObj::SourceLDP_Location(lmUPoint uPaperPos)
{
    wxString sSource = _T("");

    // X position
    wxString sPosX = _T("");
    if (m_tPos.xType != lmLOCATION_DEFAULT && m_tPos.xType != lmLOCATION_COMPUTED)
    {
        //value
        if (m_tPos.xType == lmLOCATION_USER_RELATIVE)
		{
			if (m_tPos.xUnits == lmLUNITS)
				sPosX = wxString::Format(_T("dx:%s"),
							DoubleToStr((double)LogicalToTenths(m_tPos.x), 4).c_str() );
			else if (m_tPos.xUnits == lmTENTHS)
				sPosX = wxString::Format(_T("dx:%s"), DoubleToStr((double)m_tPos.x, 4).c_str() );
		}
        else
		{
			//absolute. Convert to relative
			if (m_tPos.xUnits == lmLUNITS)
				sPosX = wxString::Format(_T("dx:%s"),
							DoubleToStr((double)LogicalToTenths(m_tPos.x - uPaperPos.x), 4).c_str() );
			else if (m_tPos.xUnits == lmTENTHS)
				sPosX = wxString::Format(_T("dx:%.4f"),
							DoubleToStr((double)(m_tPos.x - LogicalToTenths(uPaperPos.x)), 4).c_str() );
		}

        //units
        wxString sUnits = _T("");
        if (m_tPos.xUnits != lmTENTHS)
        {
        }
        sPosX += sUnits;
    }

    if (sPosX != _T(""))
    {
        sSource += _T(" ");
        sSource += sPosX;
        sSource += _T("");
    }

    // Y position
    wxString sPosY = _T("");
    if (m_tPos.yType != lmLOCATION_DEFAULT && m_tPos.yType != lmLOCATION_COMPUTED)
    {
        //value
        if (m_tPos.yType == lmLOCATION_USER_RELATIVE)
		{
			if (m_tPos.xUnits == lmLUNITS)
				sPosY = wxString::Format(_T("dy:%s"),
								DoubleToStr((double)LogicalToTenths(m_tPos.y), 4).c_str() );
			else if (m_tPos.xUnits == lmTENTHS)
				sPosY = wxString::Format(_T("dy:%s"), DoubleToStr((double)m_tPos.y, 4).c_str() );
		}
        else
		{
			//absolute. Convert to relative
			if (m_tPos.yUnits == lmLUNITS)
				sPosY = wxString::Format(_T("dy:%s"),
							DoubleToStr((double)LogicalToTenths(m_tPos.y - uPaperPos.y), 4).c_str() );
			else if (m_tPos.yUnits == lmTENTHS)
				sPosY = wxString::Format(_T("dy:%s"),
							DoubleToStr((double)(m_tPos.y - LogicalToTenths(uPaperPos.y)), 4).c_str() );
		}

        //units
        wxString sUnits = _T("");
        if (m_tPos.yUnits != lmTENTHS)
        {
        }
        sPosY += sUnits;
    }

    if (sPosY != _T(""))
    {
        sSource += _T(" ");
        sSource += sPosY;
        sSource += _T("");
    }

    return sSource;
}

lmUPoint lmComponentObj::ComputeObjectLocation(lmPaper* pPaper)
{
	lmUPoint uPos = GetReferencePaperPos();

#if 1
	return ComputeBestLocation(uPos, pPaper);

#else

	//if default location, ask derived object to compute the best position for itself
    if (m_tPos.xType == lmLOCATION_DEFAULT || m_tPos.yType == lmLOCATION_DEFAULT)
		uPos = ComputeBestLocation(uPos, pPaper);
	else if (m_tPos.xType == lmLOCATION_COMPUTED || m_tPos.yType == lmLOCATION_COMPUTED)
	{
		m_tPos = m_tSrcPos;
		uPos = ComputeBestLocation(uPos, pPaper);
	}


    if (m_tPos.xType == lmLOCATION_DEFAULT)
	{
		//use the computed best location
		m_tPos.x = uPos.x;
		m_tPos.xType = lmLOCATION_COMPUTED;
		m_tPos.xUnits = lmLUNITS;
    }

	else if (m_tPos.xType == lmLOCATION_COMPUTED)
	{
		//the default position was computed in a previous invocation. Use it
		//The computed location is always absolute, in tenths
		uPos.x = m_tPos.x;
    }

	else if (m_tPos.xType == lmLOCATION_USER_ABSOLUTE)
	{
		//the position was fixed by user (either in source file or by dragging object)
		//Use it
		if (m_tPos.xUnits == lmLUNITS)
			uPos.x = m_tPos.x;
		else if (m_tPos.xUnits == lmTENTHS)
			uPos.x = TenthsToLogical( m_tPos.x );
	}

	else if (m_tPos.xType == lmLOCATION_USER_RELATIVE)
	{
		//the position was fixed by user (either in source file or by dragging object)
		//Use it
		if (m_tPos.xUnits == lmLUNITS)
			uPos.x += m_tPos.x;
		else if (m_tPos.xUnits == lmTENTHS)
			uPos.x += TenthsToLogical( m_tPos.x );
	}
	else
		wxASSERT(false);


    if (m_tPos.yType == lmLOCATION_DEFAULT)
	{
		//use the computed best location
		m_tPos.y = uPos.y;
		m_tPos.yType = lmLOCATION_COMPUTED;
		m_tPos.yUnits = lmLUNITS;
    }

	else if (m_tPos.yType == lmLOCATION_COMPUTED)
	{
		//the position was computed in a previous invocation or was fixed by user.
		//Use it
		//The computed location is always absolute, in tenths
		uPos.y = m_tPos.y;
    }

	else if (m_tPos.yType == lmLOCATION_USER_ABSOLUTE)
	{
		//the position was fixed by user (either in source file or by dragging object)
		//Use it
		if (m_tPos.yUnits == lmLUNITS)
			uPos.y = m_tPos.y;
		else if (m_tPos.yUnits == lmTENTHS)
			uPos.y = TenthsToLogical( m_tPos.y );
	}

	else if (m_tPos.yType == lmLOCATION_USER_RELATIVE)
	{
		//the position was fixed by user (either in source file or by dragging object)
		//Use it
		if (m_tPos.yUnits == lmLUNITS)
			uPos.y += m_tPos.y;
		else if (m_tPos.xUnits == lmTENTHS)
			uPos.y += TenthsToLogical( m_tPos.y );
	}
	else
		wxASSERT(false);

	return uPos;
#endif

}




//-------------------------------------------------------------------------------------------------
// lmStaffObj implementation
//-------------------------------------------------------------------------------------------------

lmStaffObj::lmStaffObj(lmScoreObj* pParent, EStaffObjType nType, lmVStaff* pStaff, int nStaff,
                   bool fVisible, bool fIsDraggable) :
    lmComponentObj(pParent, eSCOT_StaffObj, &g_tDefaultPos, fIsDraggable)
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
}

void lmStaffObj::Layout(lmBox* pBox, lmPaper* pPaper, wxColour colorC, bool fHighlight)
{
	lmUPoint uOrg = SetReferencePos(pPaper);
	lmUPoint uPos = ComputeObjectLocation(pPaper);			// compute location
    m_uComputedPos = uPos;

	lmLUnits uWidth;
    if (m_fVisible)
	{
		//SetFont(pPaper);										// set the font
		uWidth = LayoutObject(pBox, pPaper, uPos, colorC);		// layout derived object
	}
	else
	{
		//Create an invisible shape, to store the StaffObj position
		lmShapeInvisible* pShape = new lmShapeInvisible(this, uOrg, lmUSize(0.0, 0.0) );
		pBox->AddShape(pShape);
		m_pShape = pShape;
		uWidth = 0;
	}

	//if user defined position shift the shape
	if (m_pShape && m_uUserShift.x != 0.0f || m_uUserShift.y != 0.0f)
		m_pShape->Shift(m_uUserShift.x, m_uUserShift.y);

	// layout AuxObjs attached to this StaffObj
    if (m_pAuxObjs)
    {
	    for (int i=0; i < (int)m_pAuxObjs->size(); i++)
	    {
		    //restore this object paper pos.
		    pPaper->SetCursorX(uPos.x);
		    pPaper->SetCursorY(uPos.y);

		    (*m_pAuxObjs)[i]->Layout(pBox, pPaper, colorC, fHighlight);
	    }
    }

    // update paper cursor position
    pPaper->SetCursorX(uOrg.x + uWidth);

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

lmContext* lmStaffObj::GetCurrentContext()
{
	// Returns the context that is applicable to the this StaffObj.
	// AWARE: Only Clef, key signature and time signature are updated. To get
	//	applicable accidentals use NewUpdatedContext() instead.
	return m_pVStaff->GetCurrentContext(this);
}

lmContext* lmStaffObj::NewUpdatedContext()
{
	//returns the applicable context for this StaffObj, updated with all
	//accidentals introduced by previous notes
	return m_pVStaff->NewUpdatedContext(this);
}

wxString lmStaffObj::SourceLDP(int nIndent)
{
	wxString sSource = _T("");

    //visible?
    if (!m_fVisible) { sSource += _T(" noVisible"); }

    // Generate source code for AuxObjs attached to this StaffObj
    if (m_pAuxObjs)
    {
		nIndent++;
        for (int i=0; i < (int)m_pAuxObjs->size(); i++)
        {
            sSource += (*m_pAuxObjs)[i]->SourceLDP(nIndent);
        }
		nIndent--;
    }

    //location
    sSource += SourceLDP_Location();


    sSource += _T(")\n");
	return sSource;
}

wxString lmStaffObj::SourceLDP_Location()
{
	wxString sSource = _T("");

	//location
    if (m_uUserShift.x != 0.0f)
		sSource += wxString::Format(_T(" dx:%s"),
					DoubleToStr((double)LogicalToTenths(m_uUserShift.x), 4).c_str() );

	if (m_uUserShift.y != 0.0f)
		sSource += wxString::Format(_T(" dy:%s"),
					DoubleToStr((double)LogicalToTenths(m_uUserShift.y), 4).c_str() );

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
	wxString sSource = _T("");

    //base class info
	sSource += lmScoreObj::Dump();

    // Dump AuxObjs attached to this StaffObj
    if (m_pAuxObjs)
    {
        for (int i=0; i < (int)m_pAuxObjs->size(); i++)
        {
            sSource += (*m_pAuxObjs)[i]->Dump();
        }
    }
	return sSource;
}

void lmStaffObj::CursorHighlight(lmPaper* pPaper, int nStaff, bool fHighlight)
{
    if (fHighlight)
    {
        GetShap2()->Render(pPaper, g_pColors->CursorColor());
    }
    else
    {
        //IMPROVE
        // If we paint in black it remains a coloured aureole around
        // the note. By painting it first in white the size of the aureole
        // is smaller but still visible. A posible better solution is to
        // modify Render method to accept an additional parameter: a flag
        // to signal that XOR draw mode in colour followed by a normal
        // draw in BLACK must be done.

        GetShap2()->Render(pPaper, *wxWHITE);
        GetShap2()->Render(pPaper, g_pColors->ScoreNormal());
    }
}
