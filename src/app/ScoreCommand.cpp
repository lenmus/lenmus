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
#include "../graphic/ShapeArch.h"


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
// lmCmdDeleteSelection implementation
//----------------------------------------------------------------------------------------

enum        //type of object to delete
{
    //lm_eObjArch,
    //lm_eObjBeam,
    //lm_eObjBrace,
    //lm_eObjBracket,
    //lm_eObjGlyph,
    //lm_eObjInvisible,
    //lm_eObjLine,
    //lm_eObjStaff,
    lm_eObjStaffObj,
    //lm_eObjStem,
    //lm_eObjText,
    lm_eObjTie,
    lm_eObjTuplet,
};

lmCmdDeleteSelection::lmCmdDeleteSelection(lmVStaffCursor* pVCursor, const wxString& name,
                                           lmScoreDocument *pDoc, lmGMSelection* pSelection)
        : lmScoreCommand(name, pDoc, pVCursor)
{
    //loop to save objects to delete and its parameters
    wxString sCmdName;
    lmGMObject* pGMO = pSelection->GetFirst();
    while (pGMO)
    {
        switch(pGMO->GetType())
        {
            case eGMO_ShapeTie:
                {
                    lmDeletedSO* pSOData = new lmDeletedSO;
                    pSOData->nObjType = lm_eObjTie;
                    pSOData->pObj = (void*)NULL;
                    pSOData->fObjDeleted = false;
                    pSOData->pParm1 = (void*)( ((lmShapeTie*)pGMO)->GetEndNote() );     //end note
                    pSOData->pParm2 = (void*)NULL;

                    m_ScoreObjects.push_back( pSOData );
                    sCmdName = _T("Delete tie");
                }
                break;

            case eGMO_ShapeBarline:
            case eGMO_ShapeClef:
            case eGMO_ShapeNote:
            case eGMO_ShapeRest:
                {
                    lmDeletedSO* pSOData = new lmDeletedSO;
                    pSOData->nObjType = lm_eObjStaffObj;
                    pSOData->pObj = (void*)pGMO->GetScoreOwner();
                    pSOData->fObjDeleted = false;
                    pSOData->pParm1 = (void*)NULL;
                    pSOData->pParm2 = (void*)NULL;

                    m_ScoreObjects.push_back( pSOData );

                    //select command name
                    switch(pGMO->GetType())
                    {
                        case eGMO_ShapeBarline:
                            sCmdName = _T("Delete barline");    break;
                        case eGMO_ShapeClef:
                            sCmdName = _T("Delete clef");       break;
                        case eGMO_ShapeNote:
                            sCmdName = _T("Delete note");       break;
                        case eGMO_ShapeRest:
                            sCmdName = _T("Delete rest");       break;
                    }
               }
                break;

            case eGMO_ShapeTuplet:
                {
                    lmDeletedSO* pSOData = new lmDeletedSO;
                    pSOData->nObjType = lm_eObjTuplet;
                    pSOData->pObj = (void*)NULL;
                    pSOData->fObjDeleted = false;
                    pSOData->pParm1 = (void*)( ((lmShapeTuplet*)pGMO)->GetScoreOwner() );   //start note
                    pSOData->pParm2 = (void*)NULL;

                    m_ScoreObjects.push_back( pSOData );
                    sCmdName = _T("Delete tuplet");
                }
                break;

            //case eGMO_ShapeStaff:
            //case eGMO_ShapeArch:
            //case eGMO_ShapeBeam:
            //case eGMO_ShapeBrace:
            //case eGMO_ShapeBracket:
            //case eGMO_ShapeComposite:
            //case eGMO_ShapeGlyph:
            //case eGMO_ShapeInvisible:
            //case eGMO_ShapeLine:
            //case eGMO_ShapeMultiAttached:
            //case eGMO_ShapeStem:
            //case eGMO_ShapeText:
            //    break;

            default:
                wxMessageBox(
                    wxString::Format(_T("TODO: Code in lmCmdDeleteSelection to delete %s (type %d)"),
                    pGMO->GetName(), pGMO->GetType() )); 
        }
        pGMO = pSelection->GetNext();
    }

    //if only one object, change command name for better command identification
    if (pSelection->NumObjects() == 1)
        this->m_commandName = sCmdName;
}

lmCmdDeleteSelection::~lmCmdDeleteSelection()
{
    //delete frozen objects
    std::list<lmDeletedSO*>::iterator it;
    for (it = m_ScoreObjects.begin(); it != m_ScoreObjects.end(); ++it)
    {
        if ((*it)->fObjDeleted && (*it)->pObj)
        {
            switch((*it)->nObjType)
            {
                case lm_eObjTie:        delete (lmTie*)(*it)->pObj;             break;
                case lm_eObjTuplet:     delete (lmTupletBracket*)(*it)->pObj;   break;
                case lm_eObjStaffObj:   delete (lmStaffObj*)(*it)->pObj;        break;
                default:
                    wxASSERT(false);
            }
        }
        delete *it;
    }
    m_ScoreObjects.clear();
}

bool lmCmdDeleteSelection::Do()
{
    //loop to delete the objects
    bool fSuccess = true;
    bool fSkipCmd;
    lmVStaffCmd* pVCmd;
    std::list<lmDeletedSO*>::iterator it;
    for (it = m_ScoreObjects.begin(); it != m_ScoreObjects.end(); ++it)
    {
        fSkipCmd = false;
        lmUndoItem* pUndoItem = new lmUndoItem(&m_UndoLog);
        switch((*it)->nObjType)
        {
            case lm_eObjTie:
                {
                    lmNote* pEndNote = (lmNote*)(*it)->pParm1;
                    //if any of the owner notes is also in the selection, it could get deleted
                    //before the tie, causing automatically the removal of the tie. So let's
                    //check that the tie still exists
                    if (pEndNote->IsTiedToPrev())
                    {
                        lmVStaff* pVStaff = pEndNote->GetVStaff();      //affected VStaff
                        pVCmd = new lmVCmdDeleteTie(pVStaff, pUndoItem, pEndNote);
                        wxLogMessage(_T("[lmCmdDeleteSelection::Do] Deleting tie"));
                    }
                    else
                        fSkipCmd = true;
                }
                break;

            case lm_eObjStaffObj:
                {
                    lmStaffObj* pSO = (lmStaffObj*)(*it)->pObj;
                    lmVStaff* pVStaff = pSO->GetVStaff();      //affected VStaff
                    pVCmd = new lmVCmdDeleteStaffObj(pVStaff, pUndoItem, pSO);
                    wxLogMessage(_T("[lmCmdDeleteSelection::Do] Deleting staffobj"));
                }
                break;

            case lm_eObjTuplet:
                {
                    lmNote* pStartNote = (lmNote*)(*it)->pParm1;
                    //if the owner notes are also in the selection, they could get deleted
                    //before the tuplet, causing automatically the removal of the tuplet.
                    //So let's check that the tuplet still exists
                    if (pStartNote->IsInTuplet())
                    {
                        lmVStaff* pVStaff = pStartNote->GetVStaff();
                        pVCmd = new lmVCmdDeleteTuplet(pVStaff, pUndoItem, pStartNote);
                        wxLogMessage(_T("[lmCmdDeleteSelection::Do] Deleting tuplet"));
                    }
                    else
                        fSkipCmd = true;
                }
                break;

            //case lm_eObjArch:
            //case lm_eObjBeam:
            //case lm_eObjBrace:
            //case lm_eObjBracket:
            //case lm_eObjComposite:
            //case lm_eObjGlyph:
            //case lm_eObjInvisible:
            //case lm_eObjLine:
            //case lm_eObjStaff:
            //case lm_eObjStem:
            //case lm_eObjText:
            //    break;

            default:
                wxASSERT(false);
        }

        if (fSkipCmd)
            delete pUndoItem;
        else
        {
            if (pVCmd->Success())
            {
                (*it)->fObjDeleted = true;                //the Obj is no longer owned by the score
                m_UndoLog.LogCommand(pVCmd, pUndoItem);
            }
            else
            {
                fSuccess = false;
                delete pUndoItem;
                delete pVCmd;
            }
        }
    }

    //
    if (fSuccess)
   	    return CommandDone(lmSCORE_MODIFIED);
    else
    {
        //undo command for the deleted ScoreObjs
        UndoCommand();
        return false;
    }
}

bool lmCmdDeleteSelection::UndoCommand()
{
    //undelete the object
    m_UndoLog.UndoAll();

    //mark all objects as valid
    std::list<lmDeletedSO*>::iterator it;
    for (it = m_ScoreObjects.begin(); it != m_ScoreObjects.end(); ++it)
    {
        (*it)->fObjDeleted = false;      //the Obj is again owned by the score
    }

    //set cursor
    m_pDoc->GetScore()->SetNewCursorState(&m_tCursorState);

	m_pDoc->Modify(m_fDocModified);
    m_pDoc->UpdateAllViews();
    return true;
}




//----------------------------------------------------------------------------------------
// lmCmdDeleteStaffObj implementation
//----------------------------------------------------------------------------------------

lmCmdDeleteStaffObj::lmCmdDeleteStaffObj(lmVStaffCursor* pVCursor, const wxString& name,
                                     lmScoreDocument *pDoc, lmStaffObj* pSO)
        : lmScoreCommand(name, pDoc, pVCursor)
{
    m_pVStaff = pSO->GetVStaff();
    m_pSO = pSO;
    m_fDeleteSO = false;                //m_pSO is still owned by the score
}

lmCmdDeleteStaffObj::~lmCmdDeleteStaffObj()
{
    if (m_fDeleteSO)
        delete m_pSO;       //delete frozen object
}

bool lmCmdDeleteStaffObj::Do()
{
    //Proceed to delete the object
    lmUndoItem* pUndoItem = new lmUndoItem(&m_UndoLog);
    lmVStaffCmd* pVCmd = new lmVCmdDeleteStaffObj(m_pVStaff, pUndoItem, m_pSO);

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

bool lmCmdDeleteStaffObj::UndoCommand()
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
// lmCmdDeleteTie implementation
//----------------------------------------------------------------------------------------

lmCmdDeleteTie::lmCmdDeleteTie(const wxString& name, lmScoreDocument *pDoc,
                               lmNote* pEndNote)
        : lmScoreCommand(name, pDoc, (lmVStaffCursor*)NULL)
{
    m_pEndNote = pEndNote;
}

lmCmdDeleteTie::~lmCmdDeleteTie()
{
}

bool lmCmdDeleteTie::Do()
{
    //Proceed to delete the tie
    lmUndoItem* pUndoItem = new lmUndoItem(&m_UndoLog);
    lmVStaffCmd* pVCmd = new lmVCmdDeleteTie(m_pEndNote->GetVStaff(), pUndoItem, m_pEndNote);

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

bool lmCmdDeleteTie::UndoCommand()
{
    //undelete the tie
    m_UndoLog.UndoAll();

	m_pDoc->Modify(m_fDocModified);
    m_pDoc->UpdateAllViews();
    return true;
}




//----------------------------------------------------------------------------------------
// lmCmdAddTie implementation
//----------------------------------------------------------------------------------------

lmCmdAddTie::lmCmdAddTie(const wxString& name, lmScoreDocument *pDoc,
                         lmNote* pStartNote, lmNote* pEndNote)
        : lmScoreCommand(name, pDoc, (lmVStaffCursor*)NULL)
{
    m_pStartNote = pStartNote;
    m_pEndNote = pEndNote;
}

lmCmdAddTie::~lmCmdAddTie()
{
}

bool lmCmdAddTie::Do()
{
    //Proceed to add a tie
    lmUndoItem* pUndoItem = new lmUndoItem(&m_UndoLog);
    lmVStaffCmd* pVCmd = new lmVCmdAddTie(m_pEndNote->GetVStaff(), pUndoItem,
                                          m_pStartNote, m_pEndNote);

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

bool lmCmdAddTie::UndoCommand()
{
    //undelete the tie
    m_UndoLog.UndoAll();

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
								 lmENoteHeads nNotehead, lmEAccidentals nAcc,
                                 bool fTiedPrev)
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
    m_fTiedPrev = fTiedPrev;
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
                                             m_nNotehead, m_nAcc, m_fTiedPrev);

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
// lmCmdChangeNoteAccidentals: Change accidentals of notes in current selection
//----------------------------------------------------------------------------------------

lmCmdChangeNoteAccidentals::lmCmdChangeNoteAccidentals(
                                        lmVStaffCursor* pVCursor,
                                        const wxString& name, lmScoreDocument *pDoc,
                                        lmGMSelection* pSelection, int nAcc)
	: lmScoreCommand(name, pDoc, pVCursor)
{
	m_nAcc = nAcc;

    //loop to save notes to modify
    lmGMObject* pGMO = pSelection->GetFirst();
    while (pGMO)
    {
        if (pGMO->GetType() == eGMO_ShapeNote)
        {
            lmCmdNoteData* pData = new lmCmdNoteData;
            pData->pNote = (lmNote*)pGMO->GetScoreOwner();
            pData->nAcc = pData->pNote->GetAPitch().Accidentals();

            m_Notes.push_back( pData );
        }
        pGMO = pSelection->GetNext();
    }
}

lmCmdChangeNoteAccidentals::~lmCmdChangeNoteAccidentals()
{
    //delete selection data
    std::list<lmCmdNoteData*>::iterator it;
    for (it = m_Notes.begin(); it != m_Notes.end(); ++it)
    {
        delete *it;
    }
    m_Notes.clear();
}

bool lmCmdChangeNoteAccidentals::Do()
{
    //AWARE: Not using UndoLog. Direct execution of command

    std::list<lmCmdNoteData*>::iterator it;
    for (it = m_Notes.begin(); it != m_Notes.end(); ++it)
    {
        (*it)->pNote->ChangeAccidentals(m_nAcc);
    }

	return CommandDone(lmSCORE_MODIFIED);
}

bool lmCmdChangeNoteAccidentals::UndoCommand()
{
    //AWARE: Not using UndoLog. Direct execution of command

    std::list<lmCmdNoteData*>::iterator it;
    for (it = m_Notes.begin(); it != m_Notes.end(); ++it)
    {
        (*it)->pNote->ChangeAccidentals( (*it)->nAcc );
    }

    //set cursor
    m_pDoc->GetScore()->SetNewCursorState(&m_tCursorState);

	m_pDoc->Modify(m_fDocModified);
    m_pDoc->UpdateAllViews();
    return true;
}




//----------------------------------------------------------------------------------------
// lmCmdChangeNoteRestDots: Change dots of notes in current selection
//----------------------------------------------------------------------------------------

lmCmdChangeNoteRestDots::lmCmdChangeNoteRestDots(lmVStaffCursor* pVCursor,
                                                 const wxString& name, lmScoreDocument *pDoc,
                                                 lmGMSelection* pSelection, int nDots)
	: lmScoreCommand(name, pDoc, pVCursor)
{
	m_nDots = nDots;

    //loop to save note/rests to modify
    lmGMObject* pGMO = pSelection->GetFirst();
    while (pGMO)
    {
        if (pGMO->GetType() == eGMO_ShapeNote || pGMO->GetType() == eGMO_ShapeRest)
        {
            m_NoteRests.push_back( (lmNoteRest*)pGMO->GetScoreOwner() );
        }
        pGMO = pSelection->GetNext();
    }
}

lmCmdChangeNoteRestDots::~lmCmdChangeNoteRestDots()
{
    //delete selection data
    m_NoteRests.clear();
}

bool lmCmdChangeNoteRestDots::Do()
{
    //loop to change dots
    bool fSuccess = true;
    lmVStaffCmd* pVCmd;
    std::list<lmNoteRest*>::iterator it;
    for (it = m_NoteRests.begin(); it != m_NoteRests.end(); ++it)
    {
        lmUndoItem* pUndoItem = new lmUndoItem(&m_UndoLog);
        lmVStaff* pVStaff = (*it)->GetVStaff();      //affected VStaff
        pVCmd = new lmVCmdChangeDots(pVStaff, pUndoItem, *it, m_nDots);

        if (pVCmd->Success())
            m_UndoLog.LogCommand(pVCmd, pUndoItem);     //save command in the undo log
        else
        {
            fSuccess = false;
            delete pUndoItem;
            delete pVCmd;
        }
    }

    // return result
    if (fSuccess)
   	    return CommandDone(lmSCORE_MODIFIED);
    else
    {
        //undo command for the modified note/rests
        UndoCommand();
        return false;
    }
}

bool lmCmdChangeNoteRestDots::UndoCommand()
{
    m_UndoLog.UndoAll();
    m_pDoc->GetScore()->SetNewCursorState(&m_tCursorState);

	m_pDoc->Modify(m_fDocModified);
    m_pDoc->UpdateAllViews();
    return true;
}




//----------------------------------------------------------------------------------------
// lmCmdDeleteTuplet implementation
//----------------------------------------------------------------------------------------

lmCmdDeleteTuplet::lmCmdDeleteTuplet(const wxString& name, lmScoreDocument *pDoc,
                                     lmNoteRest* pStartNR)
        : lmScoreCommand(name, pDoc, (lmVStaffCursor*)NULL)
{
    m_pStartNR = pStartNR;
}

lmCmdDeleteTuplet::~lmCmdDeleteTuplet()
{
}

bool lmCmdDeleteTuplet::Do()
{
    //Proceed to delete the tuplet
    lmUndoItem* pUndoItem = new lmUndoItem(&m_UndoLog);
    lmVStaff* pVStaff = m_pStartNR->GetVStaff();
    lmVStaffCmd* pVCmd = new lmVCmdDeleteTuplet(pVStaff, pUndoItem, m_pStartNR);

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

bool lmCmdDeleteTuplet::UndoCommand()
{
    //undelete the tuplet
    m_UndoLog.UndoAll();

	m_pDoc->Modify(m_fDocModified);
    m_pDoc->UpdateAllViews();
    return true;
}




//----------------------------------------------------------------------------------------
// lmCmdAddTuplet implementation: Add a tuplet to notes in current selection
//----------------------------------------------------------------------------------------

lmCmdAddTuplet::lmCmdAddTuplet(lmVStaffCursor* pVCursor, const wxString& name,
                               lmScoreDocument *pDoc, lmGMSelection* pSelection,
                               bool fShowNumber, int nNumber, bool fBracket,
                               lmEPlacement nAbove, int nActual, int nNormal)
	: lmScoreCommand(name, pDoc, pVCursor)
{
    m_fShowNumber = fShowNumber;
    m_nNumber = nNumber;
    m_fBracket = fBracket;
    m_nAbove = nAbove;
    m_nActual = nActual;
    m_nNormal = nNormal;

    //loop to save note/rests to form the tuplet
    lmGMObject* pGMO = pSelection->GetFirst();
    while (pGMO)
    {
        if (pGMO->GetType() == eGMO_ShapeNote || pGMO->GetType() == eGMO_ShapeRest)
        {
            m_NotesRests.push_back( (lmNoteRest*)pGMO->GetScoreOwner() );
        }
        pGMO = pSelection->GetNext();
    }
}

lmCmdAddTuplet::~lmCmdAddTuplet()
{
    //delete selection data
    m_NotesRests.clear();
}

bool lmCmdAddTuplet::Do()
{
    //Proceed to create the tuplet
    lmUndoItem* pUndoItem = new lmUndoItem(&m_UndoLog);
    lmVStaff* pVStaff = m_NotesRests.front()->GetVStaff();
    lmVStaffCmd* pVCmd = 
        new lmVCmdAddTuplet(pVStaff, pUndoItem, m_NotesRests,  m_fShowNumber, m_nNumber,
                            m_fBracket, m_nAbove, m_nActual, m_nNormal);

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

bool lmCmdAddTuplet::UndoCommand()
{
    m_UndoLog.UndoAll();
    m_pDoc->GetScore()->SetNewCursorState(&m_tCursorState);

	m_pDoc->Modify(m_fDocModified);
    m_pDoc->UpdateAllViews();
    return true;
}




//----------------------------------------------------------------------------------------
// lmCmdBreakBeam implementation
//----------------------------------------------------------------------------------------

lmCmdBreakBeam::lmCmdBreakBeam(lmVStaffCursor* pVCursor, const wxString& name,
                                     lmScoreDocument *pDoc, lmNoteRest* pBeforeNR)
        : lmScoreCommand(name, pDoc, pVCursor)
{
    m_pBeforeNR = pBeforeNR;
}

lmCmdBreakBeam::~lmCmdBreakBeam()
{
}

bool lmCmdBreakBeam::Do()
{
    //Proceed to delete the object
    lmUndoItem* pUndoItem = new lmUndoItem(&m_UndoLog);
    lmVStaffCmd* pVCmd = new lmVCmdBreakBeam(m_pBeforeNR->GetVStaff(), pUndoItem, m_pBeforeNR);

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

bool lmCmdBreakBeam::UndoCommand()
{
    //undelete the object

    m_UndoLog.UndoAll();
    m_pDoc->GetScore()->SetNewCursorState(&m_tCursorState);

	m_pDoc->Modify(m_fDocModified);
    m_pDoc->UpdateAllViews();
    return true;
}

