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
#include "../score/UndoRedo.h"
#include "../score/VStaff.h"
#include "ScoreCommand.h"
#include "ScoreDoc.h"
#include "TheApp.h"
#include "../graphic/GMObject.h"


//----------------------------------------------------------------------------------------
// lmScoreCommand abstract class implementation
//
// Do() method will return true to indicate that the action has taken place, false 
// otherwise. Returning false will indicate to the command processor that the action is 
// not undoable and should not be added to the command history.
//----------------------------------------------------------------------------------------

lmScoreCommand::lmScoreCommand(const wxString& sName, lmScoreDocument *pDoc,
                               lmVStaffCursor* pVCursor, bool fHistory)
    : wxCommand(true, sName)
{
    m_pDoc = pDoc;
	m_fDocModified = false;
    m_fHistory = fHistory;
    if (pVCursor)
        m_tCursorState = pVCursor->GetState();
    else
        m_tCursorState = g_tNoVCursorState;
}

lmScoreCommand::~lmScoreCommand()
{
}

bool lmScoreCommand::CommandDone(bool fScoreModified, int nOptions)
{
    //common code after executing a command: 
    //- save document current modification status flag, to restore it if command undo
    //- set document as 'modified'
    //- update the views with the changes
    //
    // Returns false to indicate that the action must not be added to the command history.

	m_fDocModified = m_pDoc->IsModified();
	m_pDoc->Modify(fScoreModified);
    m_pDoc->UpdateAllViews(fScoreModified, new lmUpdateHint(nOptions) );

    return m_fHistory;
}

bool lmScoreCommand::Undo()
{
    return UndoCommand();
}




//----------------------------------------------------------------------------------------
// lmCmdSelectSingle implementation
//----------------------------------------------------------------------------------------

bool lmCmdSelectSingle::Do()
{
    return DoSelectObject();
}

bool lmCmdSelectSingle::UndoCommand()
{
    return DoSelectObject();
}

bool lmCmdSelectSingle::DoSelectObject()
{

    wxASSERT( m_pGMO);

    //Toggle 'selected'
    bool fSelected = m_pGMO->IsSelected();
    m_pGMO->SetSelected(!fSelected);
    m_pDoc->UpdateAllViews(lmSCORE_NOT_MODIFIED, new lmUpdateHint(lmREDRAW));
    return false;       //do not add to Undo/Redo list
}




//----------------------------------------------------------------------------------------
// lmCmdSelectMultiple implementation
//----------------------------------------------------------------------------------------

lmCmdSelectMultiple::lmCmdSelectMultiple(const wxString& name, lmScoreDocument *pDoc,
                                         lmView* pView, lmGMSelection* pSelection,
                                         bool fSelect)
        : lmScoreCommand(name, pDoc, (lmVStaffCursor*)NULL )
{
    m_pSelection = pSelection;
    m_pView = pView;
    m_fSelect = fSelect;
}

bool lmCmdSelectMultiple::DoSelectUnselect()
{
    if (m_pSelection->NumObjects() < 1)
        return true;

    //Toggle 'selected'
    lmGMObject* pGMO = m_pSelection->GetFirst();
    while (pGMO)
    {
        if (pGMO->IsSelected() != m_fSelect)
            pGMO->SetSelected(m_fSelect);
        pGMO = m_pSelection->GetNext();
    }
    m_pDoc->UpdateAllViews(lmSCORE_NOT_MODIFIED, new lmUpdateHint(lmREDRAW));
    return false;       //do not add to Undo/Redo list
}



//----------------------------------------------------------------------------------------
// lmCmdDeleteObject implementation
//----------------------------------------------------------------------------------------

lmCmdDeleteObject::lmCmdDeleteObject(lmVStaffCursor* pVCursor, const wxString& name,
                                     lmScoreDocument *pDoc)
        : lmScoreCommand(name, pDoc, pVCursor)
{
    m_pVStaff = pVCursor->GetVStaff();
    m_pSO = pVCursor->GetStaffObj();
    m_fDeleteSO = false;                //m_pSO is still owned by the score
}

lmCmdDeleteObject::~lmCmdDeleteObject()
{
    if (m_fDeleteSO)
        delete m_pSO;       //delete frozen object
}

bool lmCmdDeleteObject::Do()
{
    //Proceed to delete the object
    lmUndoItem* pUndoItem = new lmUndoItem(&m_UndoLog);
    lmVStaffCmd* pVCmd = new lmVCmdDeleteObject(m_pVStaff, pUndoItem, m_pSO);

    if (pVCmd->Success())
    {
        m_fDeleteSO = true;                //m_pSO is no longer owned by the score
        m_UndoLog.LogCommand(pVCmd, pUndoItem);
	    return CommandDone(lmSCORE_MODIFIED);
    }
    else
    {
        m_fDeleteSO = false;
        delete pUndoItem;
        delete pVCmd;
        return false;
    }
}

bool lmCmdDeleteObject::UndoCommand()
{
    //undelete the object
    m_UndoLog.UndoAll();
    m_fDeleteSO = false;                //m_pSO is again owned by the score

    //set cursor
    m_pDoc->GetScore()->SetNewCursorState(&m_tCursorState);

	m_pDoc->Modify(m_fDocModified);
    m_pDoc->UpdateAllViews();
    return true;
}



//----------------------------------------------------------------------------------------
// lmCmdUserMoveScoreObj implementation
//----------------------------------------------------------------------------------------

lmCmdUserMoveScoreObj::lmCmdUserMoveScoreObj(const wxString& sName, lmScoreDocument *pDoc,
									         lmGMObject* pGMO, const lmUPoint& uPos)
	: lmScoreCommand(sName, pDoc, (lmVStaffCursor*)NULL )
{
	m_tPos.x = uPos.x;
	m_tPos.y = uPos.y;
    //wxLogMessage(_T("[lmCmdUserMoveScoreObj::lmCmdUserMoveScoreObj] User pos (%.2f, %.2f)"), uPos.x, uPos.y );
	m_tPos.xType = lmLOCATION_USER_RELATIVE;
	m_tPos.yType = lmLOCATION_USER_RELATIVE;
	m_tPos.xUnits = lmLUNITS;
	m_tPos.yUnits = lmLUNITS;

	m_pSO = pGMO->GetScoreOwner();
    m_nShapeIdx = pGMO->GetOwnerIDX();
}

bool lmCmdUserMoveScoreObj::Do()
{
    wxASSERT_MSG( m_pSO, _T("[lmCmdUserMoveScoreObj::Do] No ScoreObj to move!"));

    m_tOldPos = m_pSO->SetUserLocation(m_tPos, m_nShapeIdx);

	return CommandDone(lmSCORE_MODIFIED);  //, lmREDRAW);
}

bool lmCmdUserMoveScoreObj::UndoCommand()
{
    wxASSERT_MSG( m_pSO, _T("[lmCmdUserMoveScoreObj::Undo]: No ScoreObj to move!"));

    m_pSO->SetUserLocation(m_tOldPos, m_nShapeIdx);
	m_pDoc->Modify(m_fDocModified);
    m_pDoc->UpdateAllViews();
    return true;

}




//----------------------------------------------------------------------------------------
// lmCmdInsertBarline: Insert a barline at current cursor position
//----------------------------------------------------------------------------------------

lmCmdInsertBarline::lmCmdInsertBarline(lmVStaffCursor* pVCursor, const wxString& sName,
                                       lmScoreDocument *pDoc, lmEBarline nType)
	: lmScoreCommand(sName, pDoc, pVCursor)
{
    m_nBarlineType = nType;
}

bool lmCmdInsertBarline::Do()
{
    lmScoreCursor* pCursor = m_pDoc->GetScore()->SetNewCursorState(&m_tCursorState);
    lmVStaff* pVStaff = pCursor->GetVStaff();

    lmUndoItem* pUndoItem = new lmUndoItem(&m_UndoLog);
    lmVStaffCmd* pVCmd = new lmVCmdInsertBarline(pVStaff, pUndoItem, m_nBarlineType);

    if (pVCmd->Success())
    {
        m_UndoLog.LogCommand(pVCmd, pUndoItem);
	    return CommandDone(lmSCORE_MODIFIED);
    }
    else
    {
        delete pUndoItem;
        delete pVCmd;
        return false;
    }
}

bool lmCmdInsertBarline::UndoCommand()
{
    m_UndoLog.UndoAll();
    m_pDoc->GetScore()->SetNewCursorState(&m_tCursorState);

	m_pDoc->Modify(m_fDocModified);
    m_pDoc->UpdateAllViews();
    return true;
}




//----------------------------------------------------------------------------------------
// lmCmdInsertClef: Insert a clef at current cursor position
//----------------------------------------------------------------------------------------

lmCmdInsertClef::lmCmdInsertClef(lmVStaffCursor* pVCursor, const wxString& sName,
                                 lmScoreDocument *pDoc, lmEClefType nClefType,
                                 bool fHistory)
	: lmScoreCommand(sName, pDoc, pVCursor, fHistory)
{
    m_nClefType = nClefType;
}

bool lmCmdInsertClef::Do()
{
    lmScoreCursor* pCursor = m_pDoc->GetScore()->SetNewCursorState(&m_tCursorState);
    lmVStaff* pVStaff = pCursor->GetVStaff();

    lmUndoItem* pUndoItem = new lmUndoItem(&m_UndoLog);
    int nStaff = pCursor->GetCursorNumStaff();
    lmVStaffCmd* pVCmd = new lmVCmdInsertClef(pVStaff, pUndoItem, m_nClefType, nStaff);

    if (pVCmd->Success())
    {
        m_UndoLog.LogCommand(pVCmd, pUndoItem);
	    return CommandDone(lmSCORE_MODIFIED);
    }
    else
    {
        delete pUndoItem;
        delete pVCmd;
        return false;
    }
}

bool lmCmdInsertClef::UndoCommand()
{
    m_UndoLog.UndoAll();
    m_pDoc->GetScore()->SetNewCursorState(&m_tCursorState);

	m_pDoc->Modify(m_fDocModified);
    m_pDoc->UpdateAllViews();
    return true;
}




//----------------------------------------------------------------------------------------
// lmCmdInsertTimeSignature: Insert a time signature at current cursor position
//----------------------------------------------------------------------------------------

lmCmdInsertTimeSignature::lmCmdInsertTimeSignature(lmVStaffCursor* pVCursor, const wxString& sName,
                             lmScoreDocument *pDoc,  int nBeats, int nBeatType, 
                             bool fVisible, bool fHistory)
	: lmScoreCommand(sName, pDoc, pVCursor, fHistory)
{
    m_nBeats = nBeats;
    m_nBeatType = nBeatType;
    m_fVisible = fVisible;
}

bool lmCmdInsertTimeSignature::Do()
{
    lmScoreCursor* pCursor = m_pDoc->GetScore()->SetNewCursorState(&m_tCursorState);
    lmVStaff* pVStaff = pCursor->GetVStaff();

    lmUndoItem* pUndoItem = new lmUndoItem(&m_UndoLog);
    lmVStaffCmd* pVCmd = new lmVCmdInsertTimeSignature(pVStaff, pUndoItem, m_nBeats,
                                                       m_nBeatType, m_fVisible);

    if (pVCmd->Success())
    {
        m_UndoLog.LogCommand(pVCmd, pUndoItem);
	    return CommandDone(lmSCORE_MODIFIED);
    }
    else
    {
        delete pUndoItem;
        delete pVCmd;
        return false;
    }
}

bool lmCmdInsertTimeSignature::UndoCommand()
{
    m_UndoLog.UndoAll();
    m_pDoc->GetScore()->SetNewCursorState(&m_tCursorState);

	m_pDoc->Modify(m_fDocModified);
    m_pDoc->UpdateAllViews();
    return true;
}




//----------------------------------------------------------------------------------------
// lmCmdInsertKeySignature: Insert a key signature at current cursor position
//----------------------------------------------------------------------------------------

lmCmdInsertKeySignature::lmCmdInsertKeySignature(lmVStaffCursor* pVCursor, const wxString& sName,
                             lmScoreDocument *pDoc, int nFifths, bool fMajor, 
                             bool fVisible, bool fHistory)
	: lmScoreCommand(sName, pDoc, pVCursor, fHistory)
{
    m_nFifths = nFifths;
    m_fMajor = fMajor;
    m_fVisible = fVisible;
}

bool lmCmdInsertKeySignature::Do()
{
    lmScoreCursor* pCursor = m_pDoc->GetScore()->SetNewCursorState(&m_tCursorState);
    lmVStaff* pVStaff = pCursor->GetVStaff();

    lmUndoItem* pUndoItem = new lmUndoItem(&m_UndoLog);
    lmVStaffCmd* pVCmd = new lmVCmdInsertKeySignature(pVStaff, pUndoItem, m_nFifths,
                                                      m_fMajor, m_fVisible);

    if (pVCmd->Success())
    {
        m_UndoLog.LogCommand(pVCmd, pUndoItem);
	    return CommandDone(lmSCORE_MODIFIED);
    }
    else
    {
        delete pUndoItem;
        delete pVCmd;
        return false;
    }
}

bool lmCmdInsertKeySignature::UndoCommand()
{
    m_UndoLog.UndoAll();
    m_pDoc->GetScore()->SetNewCursorState(&m_tCursorState);

	m_pDoc->Modify(m_fDocModified);
    m_pDoc->UpdateAllViews();
    return true;
}




//----------------------------------------------------------------------------------------
// lmCmdInsertNote: Insert a note at current cursor position
//----------------------------------------------------------------------------------------

lmCmdInsertNote::lmCmdInsertNote(lmVStaffCursor* pVCursor, const wxString& sName,
                                 lmScoreDocument *pDoc,
                                 lmEPitchType nPitchType,
								 int nStep, int nOctave,
								 lmENoteType nNoteType, float rDuration, int nDots,
								 lmENoteHeads nNotehead, lmEAccidentals nAcc)
	: lmScoreCommand(sName, pDoc, pVCursor)
{
	m_nNoteType = nNoteType;
	m_nPitchType = nPitchType;
	m_nStep = nStep;
	m_nOctave = nOctave;
    m_nDots = nDots;
	m_rDuration = rDuration;
	m_nNotehead = nNotehead;
	m_nAcc = nAcc;
}

lmCmdInsertNote::~lmCmdInsertNote()
{
}

bool lmCmdInsertNote::Do()
{
    lmScoreCursor* pCursor = m_pDoc->GetScore()->SetNewCursorState(&m_tCursorState);
    m_pVStaff = pCursor->GetVStaff();

    lmUndoItem* pUndoItem = new lmUndoItem(&m_UndoLog);

    lmVStaffCmd* pVCmd = new lmVCmdInsertNote(m_pVStaff, pUndoItem, m_nPitchType, m_nStep,
                                             m_nOctave, m_nNoteType, m_rDuration, m_nDots, 
                                             m_nNotehead, m_nAcc);

    if (pVCmd->Success())
    {
        m_UndoLog.LogCommand(pVCmd, pUndoItem);
	    return CommandDone(lmSCORE_MODIFIED);
    }
    else
    {
        delete pUndoItem;
        delete pVCmd;
        return false;
    }

}

bool lmCmdInsertNote::UndoCommand()
{
    m_UndoLog.UndoAll();
    m_pDoc->GetScore()->SetNewCursorState(&m_tCursorState);

	m_pDoc->Modify(m_fDocModified);
    m_pDoc->UpdateAllViews();
    return true;
}




//----------------------------------------------------------------------------------------
// lmCmdInsertRest: Insert a rest at current cursor position
//----------------------------------------------------------------------------------------

lmCmdInsertRest::lmCmdInsertRest(lmVStaffCursor* pVCursor, const wxString& sName,
                                 lmScoreDocument *pDoc,
								 lmENoteType nNoteType, float rDuration, int nDots)
	: lmScoreCommand(sName, pDoc, pVCursor)
{
	m_nNoteType = nNoteType;
    m_nDots = nDots;
	m_rDuration = rDuration;
}

lmCmdInsertRest::~lmCmdInsertRest()
{
}

bool lmCmdInsertRest::Do()
{
    lmScoreCursor* pCursor = m_pDoc->GetScore()->SetNewCursorState(&m_tCursorState);
    m_pVStaff = pCursor->GetVStaff();

    lmUndoItem* pUndoItem = new lmUndoItem(&m_UndoLog);

    lmVStaffCmd* pVCmd = new lmVCmdInsertRest(m_pVStaff, pUndoItem, m_nNoteType,
                                              m_rDuration, m_nDots);

    if (pVCmd->Success())
    {
        m_UndoLog.LogCommand(pVCmd, pUndoItem);
	    return CommandDone(lmSCORE_MODIFIED);
    }
    else
    {
        delete pUndoItem;
        delete pVCmd;
        return false;
    }

}

bool lmCmdInsertRest::UndoCommand()
{
    m_UndoLog.UndoAll();
    m_pDoc->GetScore()->SetNewCursorState(&m_tCursorState);

	m_pDoc->Modify(m_fDocModified);
    m_pDoc->UpdateAllViews();
    return true;
}




//----------------------------------------------------------------------------------------
// lmCmdChangeNotePitch: Change pitch of note at current cursor position
//----------------------------------------------------------------------------------------

lmCmdChangeNotePitch::lmCmdChangeNotePitch(const wxString& sName, lmScoreDocument *pDoc,
                                 lmNote* pNote, int nSteps)
	: lmScoreCommand(sName, pDoc, (lmVStaffCursor*)NULL )
{
	m_nSteps = nSteps;
	m_pNote = pNote;
}

bool lmCmdChangeNotePitch::Do()
{
	m_pNote->ChangePitch(m_nSteps);

	return CommandDone(lmSCORE_MODIFIED);
}

bool lmCmdChangeNotePitch::UndoCommand()
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
	: lmScoreCommand(sName, pDoc, (lmVStaffCursor*)NULL )
{
	m_nSteps = nSteps;
	m_pNote = pNote;
}

bool lmCmdChangeNoteAccidentals::Do()
{
	m_pNote->ChangeAccidentals(m_nSteps);
	return CommandDone(lmSCORE_MODIFIED);
}

bool lmCmdChangeNoteAccidentals::UndoCommand()
{
	m_pNote->ChangeAccidentals(-m_nSteps);
	m_pDoc->Modify(m_fDocModified);
    m_pDoc->UpdateAllViews();
    return true;
}

