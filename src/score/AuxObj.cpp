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
#pragma implementation "AuxObj.h"
#endif

// For compilers that support precompilation, includes <wx/wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "Score.h"
#include "VStaff.h"
#include "../graphic/GMObject.h"
#include "../graphic/Shapes.h"
#include "../graphic/ShapeNote.h"
#include "../graphic/ShapeLine.h"
#include "../ldp_parser/AuxString.h"
#include "properties/DlgProperties.h"
#include "../app/ScoreCanvas.h"			//lmController

#include "lenmus_internal_model.h"
#include "lenmus_im_note.h"

using namespace lenmus;

extern bool g_fShowDirtyObjects;        //defined in TheApp.cpp


//========================================================================================
// lmAuxObj implementation
//========================================================================================

lmAuxObj::lmAuxObj(lmScoreObj* pOwner, long nID, lmEScoreObjType nType, bool fIsDraggable)
    : lmComponentObj(pOwner, nID, nType, fIsDraggable)
{
    SetLayer(lm_eLayerAuxObjs);
}

void lmAuxObj::Layout(lmBox* pBox, lmPaper* pPaper, bool fHighlight)
{
	//assign AuxObj reference position: the parent's m_uComputedPos value
	SetReferencePos(pPaper);

    m_uComputedPos = ComputeBestLocation(m_uPaperPos, pPaper);
    if (lmPRESERVE_SHAPES && !IsDirty())
    {
        //Not dirty: just add existing shapes to the Box
        lmShape* pOldShape = this->GetShape();      //TODO: Multi-shaped AuxObjs?
        pBox->AddShape(pOldShape, GetLayer());
        pOldShape->SetColour(*wxCYAN);//m_color);       //change its colour to new desired colour
    }
    else
    {
        LayoutObject(pBox, pPaper, m_uComputedPos, (g_fShowDirtyObjects && IsDirty() ? *wxRED : m_color));
    }
}

lmLUnits lmAuxObj::TenthsToLogical(lmTenths nTenths)
{
	return ((lmStaffObj*)m_pParent)->TenthsToLogical(nTenths);
}

lmTenths lmAuxObj::LogicalToTenths(lmLUnits uUnits)
{
	return ((lmStaffObj*)m_pParent)->LogicalToTenths(uUnits);
}

void lmAuxObj::SetOwner(lmScoreObj* pOwner)
{
    wxASSERT(!m_pParent);
    m_pParent = pOwner;
    GetScore()->AssignID(this);
}

wxFont* lmAuxObj::GetSuitableFont(lmPaper* pPaper)
{
    return m_pParent->GetSuitableFont(pPaper);
}

void lmAuxObj::OnParentComputedPositionShifted(lmLUnits uxShift, lmLUnits uyShift)
{
    //This method is invoked only from lmScoreObj::StoreOriginAndShiftShapes() to inform
    //about a change in the computed final position for parent ScoreObj. Therefore, it
    //is necessary to update this AuxObj reference pos.

	m_uComputedPos.x += uxShift;
	m_uComputedPos.y += uyShift;
    lmShape* pGMObj = GetShape();
	if (pGMObj)
    {
		////DBG--------------------------------------------------------------------------------
		//if (GetID()==63 || GetID()==64)
		//{
	    //  lmUPoint uShapePos = pGMObj->GetBounds().GetTopLeft();
		//	wxLogMessage(_T("[lmAuxObj::OnParentComputedPositionShifted] uxShift=%.2f, ShapeOrg=(%.2f, %.2f), ShapePos=(%.2f, %.2f), ComputedPos=(%.2f, %.2f)"),
		//				uxShift,
		//				pGMObj->GetObjectOrigin().x, pGMObj->GetObjectOrigin().y,
		//				uShapePos.x, uShapePos.y,
		//				m_uComputedPos.x, m_uComputedPos.y );
		//}
		////END DBG----------------------------------------------------------------------------
        pGMObj->Shift(uxShift, uyShift);
        pGMObj->ApplyUserShift( this->GetUserShift() );
    }
}

void lmAuxObj::OnParentMoved(lmLUnits uxShift, lmLUnits uyShift)
{
	//TODO: specific flag to decouple from parent staffObj, so the user can
	//control if the attached AuxObj will be moved with the parent or not

    lmShape* pGMObj = GetShape();
	if (pGMObj)
    {
        lmUPoint uUserShift = GetUserShift();
	    uUserShift.x += uxShift;
	    uUserShift.y += uyShift;
        this->SaveUserLocation(uUserShift.x, uUserShift.y, 0);
        pGMObj->ApplyUserShift(uUserShift);
    }

	////DBG--------------------------------------------------------------------------------
	//if (GetID()==4)	//if (((lmComponentObj*)m_pParent)->GetID()==3)
	//{
	//	lmUPoint uNewOrg = m_uComputedPos + m_uUserShift;
	//	wxLogMessage(_T("[lmAuxObj::OnParentMoved] Shift=(%.2f, %.2f), new UserShift=(%.2f, %.2f)"),
	//				xShift, yShift, m_uUserShift.x, m_uUserShift.y );
	//}
	////END DBG----------------------------------------------------------------------------
}

wxString lmAuxObj::Dump()
{
	return lmComponentObj::Dump();
}

wxString lmAuxObj::SourceLDP(int nIndent, bool fUndoData)
{
	return lmComponentObj::SourceLDP(nIndent, fUndoData);
}

wxString lmAuxObj::SourceXML(int nIndent)
{
	return lmComponentObj::SourceXML(nIndent);
}



//========================================================================================
// lmRelObj implementation
//========================================================================================

wxString lmRelObj::SourceLDP(int nIndent, bool fUndoData)
{
    WXUNUSED(nIndent);
    WXUNUSED(fUndoData);
	return wxEmptyString;
}



//========================================================================================
// lmBinaryRelObj implementation
//========================================================================================

lmBinaryRelObj::lmBinaryRelObj(lmScoreObj* pOwner, long nID, lmEScoreObjType nType,
                               lmNoteRest* pStartNR, lmNoteRest* pEndNR, bool fIsDraggable)
    : lmRelObj(pOwner, nID, nType, fIsDraggable)
    , m_pStartNR(pStartNR)
    , m_pEndNR(pEndNR)
{
}

lmBinaryRelObj::~lmBinaryRelObj()
{
    //AWARE: notes must not be deleted as they are part of a lmScore
    //and will be deleted there.

	//inform the notes
    if (m_pStartNR)
        m_pStartNR->OnRemovedFromRelationship(this);

    if (m_pEndNR)
        m_pEndNR->OnRemovedFromRelationship(this);
}

void lmBinaryRelObj::Remove(lmNoteRest* pNR)
{
    //remove note/rest.
	//AWARE: This method is always invoked by a NoteRest. Therefore it will
	//not inform back the NoteRest, as this is unnecessary and causes problems when
	//deleting the relationship object

    if (m_pStartNR == pNR)
        m_pStartNR = (lmNoteRest*)NULL;
    else if (m_pEndNR == pNR)
        m_pEndNR = (lmNoteRest*)NULL;
}



//========================================================================================
// lmMultiRelObj implementation
//========================================================================================

lmMultiRelObj::lmMultiRelObj(lmScoreObj* pOwner, long nID, lmEScoreObjType nType,
                             bool fIsDraggable)
    : lmRelObj(pOwner, nID, nType, fIsDraggable)
{
}

lmMultiRelObj::~lmMultiRelObj()
{
    //AWARE: notes must not be deleted as they are part of a lmScore
    //and will be deleted there.

	//the relationship is going to be removed. Inform notes
    std::list<lmNoteRest*>::iterator it;
    for(it=m_NoteRests.begin(); it != m_NoteRests.end(); ++it)
	{
        (*it)->OnRemovedFromRelationship(this);
	}
    m_NoteRests.clear();
}

void lmMultiRelObj::Remove(lmNoteRest* pNR)
{
    //remove note/rest.
	//AWARE: This method is always invoked by a NoteRest. Therefore it will
	//not inform back the NoteRest, as this is unnecessary and causes problems when
	//deleting the relationship object

    wxASSERT(NumNotes() > 0);

    std::list<lmNoteRest*>::iterator it;
    it = std::find(m_NoteRests.begin(), m_NoteRests.end(), pNR);
    m_NoteRests.erase(it);
    OnRelationshipModified();
}

void lmMultiRelObj::Include(lmNoteRest* pNR, int nIndex)
{
    // Add a note to the relation. nIndex is the position that the added note/rest
    // must occupy (0..n). If nIndex == -1, note/rest will be added at the end.

	//add the note/rest
	if (nIndex == -1 || nIndex == NumNotes())
		m_NoteRests.push_back(pNR);
	else
	{
		int iN;
		std::list<lmNoteRest*>::iterator it;
		for(iN=0, it=m_NoteRests.begin(); it != m_NoteRests.end(); ++it, iN++)
		{
			if (iN == nIndex)
			{
				//insert before current item
				m_NoteRests.insert(it, pNR);
				break;
			}
		}
	}
	//wxLogMessage(Dump());
	pNR->OnIncludedInRelationship(this);
    OnRelationshipModified();
}

wxString lmMultiRelObj::Dump()
{
	wxString sDump = _T("");
	std::list<lmNoteRest*>::iterator it;
	for(it=m_NoteRests.begin(); it != m_NoteRests.end(); ++it)
	{
		sDump += wxString::Format(_T("Note id = %d\n"), (*it)->GetID());
	}
	return sDump;
}

int lmMultiRelObj::GetNoteIndex(lmNoteRest* pNR)
{
	//returns the position in the notes list (0..n)

	wxASSERT(NumNotes() > 0);

	int iN;
    std::list<lmNoteRest*>::iterator it;
    for(iN=0, it=m_NoteRests.begin(); it != m_NoteRests.end(); ++it, iN++)
	{
		if (pNR == *it) return iN;
	}
    wxASSERT(false);	//note not found
	return 0;			//compiler happy
}

lmNoteRest* lmMultiRelObj::GetFirstNoteRest()
{
    m_it = m_NoteRests.begin();
    if (m_it == m_NoteRests.end())
        return (lmNoteRest*)NULL;
    else
        return *m_it;
}

lmNoteRest* lmMultiRelObj::GetNextNoteRest()
{
    //advance to next one
    ++m_it;
    if (m_it != m_NoteRests.end())
        return *m_it;

    //no more notes/rests
    return (lmNoteRest*)NULL;
}



//========================================================================================
// lmRelObX implementation
//========================================================================================

wxString lmRelObX::SourceLDP(int nIndent, bool fUndoData)
{
    WXUNUSED(nIndent);
    WXUNUSED(fUndoData);
	return wxEmptyString;
}



//========================================================================================
// lmBinaryRelObX implementation
//========================================================================================

lmBinaryRelObX::lmBinaryRelObX(lmScoreObj* pOwner, long nID, lmEScoreObjType nType,
                               lmStaffObj* pStartSO, lmStaffObj* pEndSO, bool fIsDraggable)
    : lmRelObX(pOwner, nID, nType, fIsDraggable)
    , m_pStartSO(pStartSO)
    , m_pEndSO(pEndSO)
{
}

lmBinaryRelObX::~lmBinaryRelObX()
{
    //AWARE: StaffObjs must not be deleted as they are part of a lmScore
    //and will be deleted there.

	//inform the StaffObjs
    if (m_pStartSO)
        m_pStartSO->OnRemovedFromRelation(this);

    if (m_pEndSO)
        m_pEndSO->OnRemovedFromRelation(this);
}

void lmBinaryRelObX::Remove(lmStaffObj* pSO)
{
    //remove StaffObj.
	//AWARE: This method is always invoked by a SO. Therefore it will
	//not inform back the SO, as this is unnecessary and causes problems when
	//deleting the relationship object

    if (m_pStartSO == pSO)
        m_pStartSO = (lmStaffObj*)NULL;
    else if (m_pEndSO == pSO)
        m_pEndSO = (lmStaffObj*)NULL;
}



//========================================================================================
// lmMultiRelObX implementation
//========================================================================================

lmMultiRelObX::lmMultiRelObX(lmScoreObj* pOwner, long nID, lmEScoreObjType nType,
                             bool fIsDraggable)
    : lmRelObX(pOwner, nID, nType, fIsDraggable)
{
}

lmMultiRelObX::~lmMultiRelObX()
{
    //AWARE: StaffObjs must not be deleted as they are part of a lmScore
    //and will be deleted there.

	//the relationship is going to be removed. Inform StaffObjs
    std::list<lmStaffObj*>::iterator it;
    for(it=m_relatedSO.begin(); it != m_relatedSO.end(); ++it)
	{
        (*it)->OnRemovedFromRelation(this);
	}
    m_relatedSO.clear();
}

void lmMultiRelObX::Remove(lmStaffObj* pSO)
{
    //remove StaffObj.
	//AWARE: This method is always invoked by a SO. Therefore it will
	//not inform back the SO, as this is unnecessary and causes problems when
	//deleting the relationship object

    wxASSERT(NumObjects() > 0);

    std::list<lmStaffObj*>::iterator it;
    it = std::find(m_relatedSO.begin(), m_relatedSO.end(), pSO);
    m_relatedSO.erase(it);
    OnRelationshipModified();
}

void lmMultiRelObX::Include(lmStaffObj* pSO, int nIndex)
{
    // Add a note to the relation. nIndex is the position that the added StaffObj
    // must occupy (0..n). If nIndex == -1, StaffObj will be added at the end.

	//add the StaffObj
	if (nIndex == -1 || nIndex == NumObjects())
		m_relatedSO.push_back(pSO);
	else
	{
		int iN;
		std::list<lmStaffObj*>::iterator it;
		for(iN=0, it=m_relatedSO.begin(); it != m_relatedSO.end(); ++it, iN++)
		{
			if (iN == nIndex)
			{
				//insert before current item
				m_relatedSO.insert(it, pSO);
				break;
			}
		}
	}
	//wxLogMessage(Dump());
	pSO->OnAddedToRelation(this);
    OnRelationshipModified();
}

wxString lmMultiRelObX::Dump()
{
	wxString sDump = _T("");
	std::list<lmStaffObj*>::iterator it;
	for(it=m_relatedSO.begin(); it != m_relatedSO.end(); ++it)
	{
		sDump += wxString::Format(_T("Note id = %d\n"), (*it)->GetID());
	}
	return sDump;
}

int lmMultiRelObX::GetSOIndex(lmStaffObj* pSO)
{
	//returns the position in the StaffObjs list (0..n)

	wxASSERT(NumObjects() > 0);

	int iN;
    std::list<lmStaffObj*>::iterator it;
    for(iN=0, it=m_relatedSO.begin(); it != m_relatedSO.end(); ++it, iN++)
	{
		if (pSO == *it) return iN;
	}
    wxASSERT(false);	//note not found
	return 0;			//compiler happy
}

lmStaffObj* lmMultiRelObX::GetFirstSO()
{
    m_it = m_relatedSO.begin();
    if (m_it == m_relatedSO.end())
        return (lmStaffObj*)NULL;
    else
        return *m_it;
}

lmStaffObj* lmMultiRelObX::GetNextSO()
{
    //advance to next one
    ++m_it;
    if (m_it != m_relatedSO.end())
        return *m_it;

    //no more StaffObjs/rests
    return (lmStaffObj*)NULL;
}



//========================================================================================
// lmFermata implementation
//========================================================================================

lmFermata::lmFermata(lmScoreObj* pOwner, long nID, lmEPlacement nPlacement)
        : lmAuxObj(pOwner, nID, lm_eSO_Fermata, lmDRAGGABLE)
{
    m_nPlacement = nPlacement;
}

lmUPoint lmFermata::ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper)
{
	// if no location is specified in LDP source file, this method is invoked from
	// base class to ask derived object to compute a suitable position to
	// place itself.
	// uOrg is the assigned paper position for this object.

	lmUPoint uPos = uOrg;

	// compute y location
	bool fAbove = IsAbove();
	if (fAbove)
		uPos.y -= ((lmStaffObj*)m_pParent)->TenthsToLogical(70);
	else
		uPos.y -= ((lmStaffObj*)m_pParent)->TenthsToLogical(5);

    //create a temporal shape object to get its measurements
    int nGlyphIndex = (fAbove ? GLYPH_FERMATA_OVER : GLYPH_FERMATA_UNDER);
    lmShape* pPS = m_pParent->GetShape();
    lmShapeGlyph* pFS =
		new lmShapeGlyph(this, -1, nGlyphIndex, pPaper, uPos, _T("Fermata"), lmDRAGGABLE);

	//center it on the owner
	lmLUnits uCenterPos;
	if (m_pParent->IsNote() )
	{
		//it is a note. Center fermata on notehead shape
		lmShape* pNHS = ((lmShapeNote*)pPS)->GetNoteHead();
		uCenterPos = pNHS->GetXLeft() + pNHS->GetWidth() / 2.0;
	}
	else
	{
		//it is not a note. Center fermata on parent shape
		uCenterPos = pPS->GetXLeft() + pPS->GetWidth() / 2.0;
	}
    uPos.x += uCenterPos - (pFS->GetXLeft() + pFS->GetWidth() / 2.0);

	//avoid placing it over the note if surpasses the staff
	//TODO

	delete pFS;
	return uPos;

}

bool lmFermata::IsAbove()
{
	//returns true if fermata should be placed above parent object

    if (m_nPlacement == ep_Default) {
        if (((lmNoteRest*)m_pParent)->IsRest())
            return true;
        else {
            lmNote* pNote = (lmNote*)m_pParent;
            if (pNote->GetNoteType() <= ImoNoteRest::k_whole || pNote->StemGoesDown())
                return true;
            else
                return false;
        }
    }
    else
        return (m_nPlacement == ep_Above);
}

lmLUnits lmFermata::LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)
{
    // This method is invoked by the base class (lmStaffObj). It is responsible for
    // creating the shape object and adding it to the graphical model.

    //create the shape object
    int nGlyphIndex = (IsAbove() ? GLYPH_FERMATA_OVER : GLYPH_FERMATA_UNDER);
    lmShapeGlyph* pShape =
		new lmShapeGlyph(this, 0, nGlyphIndex, pPaper, uPos, _T("Fermata"), lmDRAGGABLE, colorC);
	pBox->AddShape(pShape, GetLayer());
    StoreShape(pShape);

	return pShape->GetWidth();
}

wxString lmFermata::SourceLDP(int nIndent, bool fUndoData)
{
    wxString sSource = _T("");
    if (fUndoData)
        sSource += wxString::Format(_T(" (fermata#%d"), GetID() );
    else
	    sSource += _T(" (fermata");

    //placement
    if (m_nPlacement == ep_Default)
        sSource += _T(")");
    else if (m_nPlacement == ep_Above)
        sSource += _T(" above");
    else
        sSource += _T(" below");

	//base class info
    sSource += lmAuxObj::SourceLDP(nIndent, fUndoData);

	//close element
	sSource += _T(")");
	return sSource;
}
wxString lmFermata::SourceXML(int nIndent)
{
	//TODO
    wxString sSource = _T("");
	sSource += _T("lmFermata");
    return sSource;
}

wxString lmFermata::Dump()
{
	wxString sDump = wxString::Format(_T("\n   %d\tFermata:"), GetID() );

	//base class info
	sDump += lmAuxObj::Dump();
	sDump += _T("\n");
	return sDump;
}


//========================================================================================
// lmLyric object implementation
//========================================================================================

lmLyric::lmLyric(lmScoreObj* pOwner, wxString& sText, lmTextStyle* pStyle, ESyllabicTypes nSyllabic,
                 int nNumLine, wxString sLanguage)
    : lmAuxObj(pOwner, lmNEW_ID, lm_eSO_Lyric, lmDRAGGABLE),
      lmBasicText(sText, g_tDefaultPos, pStyle, sLanguage)
{
    m_nNumLine = nNumLine;
}

wxFont* lmLyric::GetSuitableFont(lmPaper* pPaper)
{
    //wxLogMessage(_T("[lmLyric::GetSuitableFont]: size=%d, name=%s"),
	//             m_nFontSize, m_sFontName );

    int nWeight = m_pStyle->tFont.nFontWeight;
    int nStyle = m_pStyle->tFont.nFontStyle;
    wxFont* pFont = pPaper->GetFont((int)PointsToLUnits(m_pStyle->tFont.nFontSize),
                                    m_pStyle->tFont.sFontName, wxDEFAULT, nStyle,
                                    nWeight, false);
    if (!pFont) {
        wxMessageBox(_("Sorry, an error has occurred while allocating the font."),
            _T("lmLyric::GetSuitableFont"), wxOK);
        ::wxExit();
    }
	return pFont;
}

lmUPoint lmLyric::ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper)
{
	// if no location is specified in LDP source file, this method is invoked from
	// base class to ask derived object to compute a suitable position to
	// place itself.
	// uOrg is the assigned paper position for this object.

	lmUPoint uPos = uOrg;
	//TODO
	return uPos;
}

lmLUnits lmLyric::LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)
{
    //wxASSERT(fMeasuring == DO_DRAW);    //measuring pahse is done in SetSizePosition()

    //pPaper->SetFont(*m_pFont);

    //lmUPoint uPos = GetGlyphPosition();
    //pPaper->SetTextForeground((m_fSelected ? g_pColors->ScoreSelected() : colorC));
    //pPaper->DrawText(m_sText, uPos.x, uPos.y );
	return 0;
}

//void lmLyric::SetOwner(lmNoteRest* pOwner)
//{
//    //wxASSERT(pOwner);
//    //m_pOwner = pOwner;
//    //m_pVStaff = pOwner->GetVStaff();
//    //m_nStaffNum = pOwner->GetStaffNum();
//}

//void lmLyric::SetSizePosition(lmPaper* pPaper, lmVStaff* pVStaff, int nStaffNum,
//                        lmLUnits xPos, lmLUnits yPos)
//{
//    ///*
//    //This method does the measurement phase
//    //*/
//
//    //// save paper position and prepare font
//    //m_uPaperPos = m_pOwner->GetReferencePaperPos();
//    //SetFont(pPaper);
//
//    //// prepare DC
//    //pPaper->SetFont(*m_pFont);
//
//    //// prepare the text and measure it
//    //lmLUnits nWidth, nHeight;
//    //pPaper->GetTextExtent(m_sText, &nWidth, &nHeight);
//    //m_uWidth = nWidth;
//
//    //// store glyph position (relative to paper pos).
//    ////// Remember: XML positioning values origin is the left-hand side of the note
//    ////// or the musical position within the bar (x) and the top line of the staff (y)
//    ////m_uGlyphPos.x = m_pVStaff->TenthsToLogical(m_xRel, m_nStaffNum);
//    ////// as relative-y refers to the top line of the staff, so 5 lines must be
//    ////// substracted from yBase position
//    ////m_uGlyphPos.y = m_pVStaff->TenthsToLogical(m_yRel-50, m_nStaffNum);
//    ////if (m_fOverrideDefaultX) {
//    ////    m_uGlyphPos.x += m_pVStaff->TenthsToLogical(m_xDef, m_nStaffNum) - m_uPaperPos.x;
//    ////}
//    ////if (m_fOverrideDefaultY) {
//    ////    m_uGlyphPos.y += m_pVStaff->TenthsToLogical(m_yDef, m_nStaffNum) - m_uPaperPos.y;
//    ////}
//    //m_uGlyphPos.x = xPos;
//    //m_uGlyphPos.y = yPos + pVStaff->TenthsToLogical( 40, nStaffNum ) * m_nNumLine;
//
//    // // store selection rectangle (relative to m_uPaperPos). Coincides with glyph rectangle
//    //m_uSelRect.width = nWidth;
//    //m_uSelRect.height = nHeight;
//    //m_uSelRect.x = m_uGlyphPos.x;
//    //m_uSelRect.y = m_uGlyphPos.y;
//
//}
//
wxString lmLyric::SourceLDP(int nIndent, bool fUndoData)
{
	//TODO
    wxString sSource = _T("");
    if (fUndoData)
        sSource += wxString::Format(_T(" (lmLyric#%d"), GetID() );
    else
	    sSource += _T(" (lmLyric");
    return sSource;
}
wxString lmLyric::SourceXML(int nIndent)
{
	//TODO
    wxString sSource = _T("");
	sSource += _T("lmLyric");
    return sSource;
}

wxString lmLyric::Dump()
{
	//TODO
    wxString sDump = wxString::Format(
        _T("\t-->lmLyric\t%s\tnumLine=%d\n"),
        m_sText.c_str(), m_nNumLine);
    return sDump;

}



//========================================================================================
// lmScoreLineProperties: helper class to edit lmScoreLine
//========================================================================================

class lmScoreLineProperties : public lmPropertiesPage
{
public:
	lmScoreLineProperties(lmDlgProperties* parent, lmScoreLine* pLine);
	~lmScoreLineProperties();

    //implementation of pure virtual methods in base class
    void OnAcceptChanges(lmController* pController, bool fCurrentPage);

    // event handlers

protected:
    void CreateControls();

    //controls
	wxStaticText*		m_pTxtBarline;
	wxBitmapComboBox*	m_pBarlinesList;

    //other variables
    lmScoreLine*			m_pLine;


    DECLARE_EVENT_TABLE()
};


//--------------------------------------------------------------------------------------
/// Implementation of lmScoreLineProperties
//--------------------------------------------------------------------------------------


enum {
    lmID_SCORELINE = 2600,
};


BEGIN_EVENT_TABLE(lmScoreLineProperties, lmPropertiesPage)

END_EVENT_TABLE()

//static lmBarlinesDBEntry tBarlinesDB[lm_eMaxBarline+1];

lmScoreLineProperties::lmScoreLineProperties(lmDlgProperties* parent, lmScoreLine* pLine)
    : lmPropertiesPage(parent)
    , m_pLine(pLine)
{

    ////To avoid having to translate again barline names, we are going to load them
    ////by using global function GetBarlineName()
    //int i;
    //for (i = 0; i < lm_eMaxBarline; i++)
    //{
    //    tBarlinesDB[i].nBarlineType = (lmEBarline)i;
    //    tBarlinesDB[i].sBarlineName = GetBarlineName((lmEBarline)i);
    //}
    ////End of table item
    //tBarlinesDB[i].nBarlineType = (lmEBarline)-1;
    //tBarlinesDB[i].sBarlineName = _T("");

    CreateControls();
	//LoadBarlinesBitmapComboBox(m_pBarlinesList, tBarlinesDB);
	//SelectBarlineBitmapComboBox(m_pBarlinesList, m_pLine->GetBarlineType() );
}

void lmScoreLineProperties::CreateControls()
{
	wxBoxSizer* pMainSizer;
	pMainSizer = new wxBoxSizer( wxVERTICAL );

	m_pTxtBarline = new wxStaticText( this, wxID_ANY, wxT("Line type"), wxDefaultPosition, wxDefaultSize, 0 );
	m_pTxtBarline->Wrap( -1 );
	m_pTxtBarline->SetFont( wxFont( 8, 74, 90, 90, false, wxT("Tahoma") ) );

	pMainSizer->Add( m_pTxtBarline, 0, wxALL, 5 );

	//wxArrayString m_pBarlinesListChoices;
 //   m_pBarlinesList = new wxBitmapComboBox();
 //   m_pBarlinesList->Create(this, lmID_SCORELINE, wxEmptyString, wxDefaultPosition, wxSize(135, 72),
	//						0, NULL, wxCB_READONLY);
	//pMainSizer->Add( m_pBarlinesList, 0, wxALL, 5 );

	this->SetSizer( pMainSizer );
	this->Layout();
}

lmScoreLineProperties::~lmScoreLineProperties()
{
}

void lmScoreLineProperties::OnAcceptChanges(lmController* pController, bool fCurrentPage)
{
	//int iB = m_pBarlinesList->GetSelection();
 //   lmEBarline nType = tBarlinesDB[iB].nBarlineType;
	//if (nType == m_pLine->GetBarlineType())
	//	return;		//nothing to change

 //   if (pController)
 //   {
 //       //Editing and existing object. Do changes by issuing edit commands
 //       pController->ChangeLine(m_pLine, nType, m_pLine->IsVisible());
 //   }
 // //  else
 // //  {
 // //      //Direct creation. Modify text object directly
 // //      m_pParentText->SetText( m_pTxtCtrl->GetValue() );
 // //      m_pParentText->SetStyle(pStyle);
	//	//m_pParentText->SetAlignment(m_nHAlign);
 // //  }
}

//========================================================================================
// lmScoreLine object implementation
//========================================================================================

lmScoreLine::lmScoreLine(lmScoreObj* pOwner, long nID, lmTenths xStart, lmTenths yStart,
                         lmTenths xEnd, lmTenths yEnd, lmTenths nWidth,
                         lmELineCap nStartCap, lmELineCap nEndCap, lmELineStyle nStyle,
                         wxColour nColor)
    : lmAuxObj(pOwner, nID, lm_eSO_Line, lmDRAGGABLE)
    , m_txStart(xStart)
    , m_tyStart(yStart)
    , m_txEnd(xEnd)
    , m_tyEnd(yEnd)
    , m_tWidth(nWidth)
	, m_nColor(nColor)
	, m_nEdge(lm_eEdgeNormal)
    , m_nStyle(nStyle)
    , m_nStartCap(nStartCap)
    , m_nEndCap(nEndCap)
{
}

wxString lmScoreLine::SourceLDP(int nIndent, bool fUndoData)
{
    wxString sSource = _T("");
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    if (fUndoData)
        sSource += wxString::Format(_T("(line#%d "), GetID() );
    else
        sSource += _T("(line ");

    //location
    sSource += _T("(startPoint dx:");
	sSource += DoubleToStr((double)m_txStart, 4);
    sSource += _T(" dy:");
	sSource += DoubleToStr((double)m_tyStart, 4);
    sSource += _T(")(endPoint dx:");
	sSource += DoubleToStr((double)m_txEnd, 4);
    sSource += _T(" dy:");
	sSource += DoubleToStr((double)m_tyEnd, 4);
    sSource += _T(")");

    //width and color
    sSource += _T("(width ");
	sSource += DoubleToStr((double)m_tWidth, 4);
    sSource += _T(")");
    sSource += lmColorToLDP(m_nColor, false);   //false=always generate source

    //line style and caps
    sSource += _T("(lineStyle ") + LineStyleToLDP(m_nStyle);
    sSource += _T(")");
    sSource += _T("(lineCapStart ");
    sSource += LineCapToLDP(m_nStartCap);
    sSource += _T(")");
    sSource += _T("(lineCapEnd ");
    sSource += LineCapToLDP(m_nEndCap);
    sSource += _T(")");

	//base class info
    sSource += lmAuxObj::SourceLDP(nIndent, fUndoData);

    //close element
    sSource += _T(")\n");
    return sSource;
}

wxString lmScoreLine::SourceXML(int nIndent)
{
	//TODO
    wxString sSource = _T("");
	sSource += _T("lmScoreLine");
    return sSource;
}

wxString lmScoreLine::Dump()
{
	//TODO
    wxString sDump = wxString::Format(
        _T("\t-->lmScoreLine\tstart=(%.2f, %.2f), end=(%.2f, %.2f), width=%.2f\n"),
            m_txStart, m_tyStart, m_txEnd, m_tyEnd, m_tWidth);
    return sDump;

}

lmUPoint lmScoreLine::ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper)
{
	// if no location is specified in LDP source file, this method is invoked from
	// base class to ask derived object to compute a suitable position to
	// place itself.
	// uOrg is the assigned paper position for this object.

	lmUPoint uPos = uOrg;
	return uPos;
}

lmLUnits lmScoreLine::LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)
{
    WXUNUSED(colorC);

    //compute position
    lmLUnits uxStart = m_pParent->TenthsToLogical(m_txStart) + pPaper->GetCursorX();
    lmLUnits uyStart = m_pParent->TenthsToLogical(m_tyStart) + pPaper->GetCursorY();
    lmLUnits uxEnd = m_pParent->TenthsToLogical(m_txEnd) + pPaper->GetCursorX();
    lmLUnits uyEnd = m_pParent->TenthsToLogical(m_tyEnd) + pPaper->GetCursorY();
    lmLUnits uWidth = m_pParent->TenthsToLogical(m_tWidth);
    lmLUnits uBoundsExtraWidth = m_pParent->TenthsToLogical(2);  //TODO user option?

    //create the shape
    lmShapeLine* pShape = new lmShapeLine(this, 0, uxStart, uyStart, uxEnd, uyEnd,
                                          uWidth, uBoundsExtraWidth, m_nStyle,
                                          m_nColor, m_nEdge, lmDRAGGABLE,
                                          lmSELECTABLE, lmVISIBLE, _T("GraphLine"));
    pShape->SetHeadType(m_nStartCap);
    pShape->SetTailType(m_nEndCap);
	pBox->AddShape(pShape, GetLayer());
    StoreShape(pShape);
    return pShape->GetBounds().GetWidth();

}

void lmScoreLine::MoveObjectPoints(int nNumPoints, int nShapeIdx, lmUPoint* pShifts,
                                   bool fAddShifts)
{
    //This method is only used during interactive edition.
    //It receives a vector with the shifts for object points and a flag to signal
    //whether to add or to substract shifts.

    wxASSERT(nNumPoints == 2);

    if (fAddShifts)
    {
        m_txStart += m_pParent->LogicalToTenths((*(pShifts)).x);
        m_tyStart += m_pParent->LogicalToTenths((*(pShifts)).y);
        m_txEnd += m_pParent->LogicalToTenths((*(pShifts+1)).x);
        m_tyEnd += m_pParent->LogicalToTenths((*(pShifts+1)).y);
    }
    else
    {
        m_txStart -= m_pParent->LogicalToTenths((*(pShifts)).x);
        m_tyStart -= m_pParent->LogicalToTenths((*(pShifts)).y);
        m_txEnd -= m_pParent->LogicalToTenths((*(pShifts+1)).x);
        m_tyEnd -= m_pParent->LogicalToTenths((*(pShifts+1)).y);
    }

    //inform the shape
    lmShapeLine* pShape = (lmShapeLine*)GetGraphicObject(nShapeIdx);
    wxASSERT(pShape);
    pShape->MovePoints(nNumPoints, nShapeIdx, pShifts, fAddShifts);
}

void lmScoreLine::OnEditProperties(lmDlgProperties* pDlg, const wxString& sTabName)
{
	//invoked to add specific panels to the dialog

    WXUNUSED(sTabName)

	pDlg->AddPanel( new lmScoreLineProperties(pDlg, this), _("Line"));

	//change dialog title
	pDlg->SetTitle(_("Line properties"));
}

