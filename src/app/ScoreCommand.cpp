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
#pragma implementation "ScoreCommand.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../score/Score.h"
#include "ScoreCommand.h"
#include "ScoreDoc.h"
#include "TheApp.h"

//
// Implementation of the score commands class
//

lmScoreCommand::lmScoreCommand(const wxString& sName, lmScoreDocument *pDoc)
    : wxCommand(true, sName)
{
    m_pDoc = pDoc;
	m_fDocModified = false;
}

lmScoreCommand::~lmScoreCommand()
{
}

//----------------------------------------------------------------------------------------
// lmCmdSelectSingle implementation
//----------------------------------------------------------------------------------------

bool lmCmdSelectSingle::Do()
{
    return DoSelectObject();
}

bool lmCmdSelectSingle::Undo()
{
    return DoSelectObject();
}

bool lmCmdSelectSingle::DoSelectObject()
{
    wxASSERT( m_pGMO);

    //Toggle 'selected' and return 'true' to force redraw it
    bool fSelected = m_pGMO->IsSelected();
    m_pGMO->SetSelected(!fSelected);
    m_pDoc->UpdateAllViews();
    return true;

}



//----------------------------------------------------------------------------------------
// lmCmdDeleteObject implementation
//----------------------------------------------------------------------------------------

lmCmdDeleteObject::lmCmdDeleteObject(const wxString& name, lmScoreDocument *pDoc,
									 lmStaffObj* pCursorSO)
        : lmScoreCommand(name, pDoc)
{
	m_pCursorSO = pCursorSO;
}

bool lmCmdDeleteObject::Do()
{
	m_pCursorSO->GetVStaff()->DeleteObject(m_pCursorSO);
	m_fDocModified = m_pDoc->IsModified();
	m_pDoc->Modify(true);
    m_pDoc->UpdateAllViews();
    return true;
}

bool lmCmdDeleteObject::Undo()
{
	//TODO
    return true;
}



//----------------------------------------------------------------------------------------
// lmCmdMoveScoreObj implementation
//----------------------------------------------------------------------------------------

lmCmdMoveScoreObj::lmCmdMoveScoreObj(const wxString& sName, lmScoreDocument *pDoc,
									   lmScoreObj* pSO, const lmUPoint& uPos)
	: lmScoreCommand(sName, pDoc)
{
	lmUPoint uOrg = pSO->GetOrigin();
	m_tPos.x = uPos.x - uOrg.x;
	m_tPos.y = uPos.y - uOrg.y;
	m_tPos.xType = lmLOCATION_USER_RELATIVE;
	m_tPos.yType = lmLOCATION_USER_RELATIVE;
	m_tPos.xUnits = lmLUNITS;
	m_tPos.yUnits = lmLUNITS;

	m_pSO = pSO;
}

bool lmCmdMoveScoreObj::Do()
{
    wxASSERT_MSG( m_pSO, _T("[lmCmdMoveScoreObj::Do] No ScoreObj to move!"));

    m_tOldPos = m_pSO->SetUserLocation(m_tPos);
	m_fDocModified = m_pDoc->IsModified();
	m_pDoc->Modify(true);
    m_pDoc->UpdateAllViews();
    return true;

}

bool lmCmdMoveScoreObj::Undo()
{
    wxASSERT_MSG( m_pSO, _T("[lmCmdMoveScoreObj::Undo]: No ScoreObj to move!"));

    m_pSO->SetUserLocation(m_tOldPos);
	m_pDoc->Modify(m_fDocModified);
    m_pDoc->UpdateAllViews();
    return true;

}




//----------------------------------------------------------------------------------------
// lmCmdInsertBarline: Insert a clef at current cursor position
//----------------------------------------------------------------------------------------

lmCmdInsertBarline::lmCmdInsertBarline(const wxString& sName, lmScoreDocument *pDoc,
                                 lmStaffObj* pCursorSO, lmEBarline nType)
	: lmScoreCommand(sName, pDoc)
{
    m_nBarlineType = nType;
	m_pCursorSO = pCursorSO;
}

bool lmCmdInsertBarline::Do()
{
	m_pCursorSO->GetVStaff()->InsertBarline(m_pCursorSO, m_nBarlineType);
	m_fDocModified = m_pDoc->IsModified();
	m_pDoc->Modify(true);
    m_pDoc->UpdateAllViews();
    return true;
}

bool lmCmdInsertBarline::Undo()
{
    //TODO
	m_pDoc->Modify(m_fDocModified);
    m_pDoc->UpdateAllViews();
    return true;
}




//----------------------------------------------------------------------------------------
// lmCmdInsertClef: Insert a clef at current cursor position
//----------------------------------------------------------------------------------------

lmCmdInsertClef::lmCmdInsertClef(const wxString& sName, lmScoreDocument *pDoc,
                                 lmStaffObj* pCursorSO, lmEClefType nClefType)
	: lmScoreCommand(sName, pDoc)
{
    m_nClefType = nClefType;
	m_pCursorSO = pCursorSO;
}

bool lmCmdInsertClef::Do()
{
	m_pCursorSO->GetVStaff()->InsertClef(m_pCursorSO, m_nClefType);
	m_fDocModified = m_pDoc->IsModified();
	m_pDoc->Modify(true);
    m_pDoc->UpdateAllViews();
    return true;
}

bool lmCmdInsertClef::Undo()
{
    //TODO
	m_pDoc->Modify(m_fDocModified);
    m_pDoc->UpdateAllViews();
    return true;
}




//----------------------------------------------------------------------------------------
// lmCmdInsertNote: Insert a note at current cursor position
//----------------------------------------------------------------------------------------

lmCmdInsertNote::lmCmdInsertNote(const wxString& sName, lmScoreDocument *pDoc,
                                 lmStaffObj* pCursorSO, lmEPitchType nPitchType,
								 wxString sStep, wxString sOctave, 
								 lmENoteType nNoteType, float rDuration)
	: lmScoreCommand(sName, pDoc)
{
	m_nNoteType = nNoteType;
	m_nPitchType = nPitchType;
	m_sStep = sStep;
	m_sOctave = sOctave;
	m_rDuration = rDuration;
	m_pCursorSO = pCursorSO;
}

bool lmCmdInsertNote::Do()
{
	m_pCursorSO->GetVStaff()->InsertNote(m_pCursorSO, m_nPitchType, m_sStep, m_sOctave, 
										 m_nNoteType, m_rDuration);
	m_fDocModified = m_pDoc->IsModified();
	m_pDoc->Modify(true);
    m_pDoc->UpdateAllViews();
    return true;
}

bool lmCmdInsertNote::Undo()
{
    //TODO
	m_pDoc->Modify(m_fDocModified);
    m_pDoc->UpdateAllViews();
    return true;
}




//----------------------------------------------------------------------------------------
// lmCmdChangeNotePitch: Change pitch of note at current cursor position
//----------------------------------------------------------------------------------------

lmCmdChangeNotePitch::lmCmdChangeNotePitch(const wxString& sName, lmScoreDocument *pDoc,
                                 lmNote* pNote, int nSteps)
	: lmScoreCommand(sName, pDoc)
{
	m_nSteps = nSteps;
	m_pNote = pNote;
}

bool lmCmdChangeNotePitch::Do()
{
	m_pNote->ChangePitch(m_nSteps);
	m_fDocModified = m_pDoc->IsModified();
	m_pDoc->Modify(true);
    m_pDoc->UpdateAllViews();
    return true;
}

bool lmCmdChangeNotePitch::Undo()
{
	m_pNote->ChangePitch(-m_nSteps);
	m_pDoc->Modify(m_fDocModified);
    m_pDoc->UpdateAllViews();
    return true;
}




//----------------------------------------------------------------------------------------
// lmCmdChangeNoteAccidentals: Change accidentals of note at current cursor position
//----------------------------------------------------------------------------------------

lmCmdChangeNoteAccidentals::lmCmdChangeNoteAccidentals(const wxString& sName, lmScoreDocument *pDoc,
                                 lmNote* pNote, int nSteps)
	: lmScoreCommand(sName, pDoc)
{
	m_nSteps = nSteps;
	m_pNote = pNote;
}

bool lmCmdChangeNoteAccidentals::Do()
{
	m_pNote->ChangeAccidentals(m_nSteps);
	m_fDocModified = m_pDoc->IsModified();
	m_pDoc->Modify(true);
    m_pDoc->UpdateAllViews();
    return true;
}

bool lmCmdChangeNoteAccidentals::Undo()
{
	m_pNote->ChangeAccidentals(-m_nSteps);
	m_pDoc->Modify(m_fDocModified);
    m_pDoc->UpdateAllViews();
    return true;
}

