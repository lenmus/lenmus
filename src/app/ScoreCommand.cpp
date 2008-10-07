//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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
#include "../score/EditCmd.h"
#include "../score/properties/DlgProperties.h"
#include "ScoreCommand.h"
#include "ScoreDoc.h"
#include "TheApp.h"
#include "../graphic/GMObject.h"
#include "../graphic/ShapeArch.h"
#include "../graphic/ShapeBeam.h"
#include "../graphic/ShapeText.h"


//----------------------------------------------------------------------------------------
// lmScoreCommand abstract class implementation
//
// Do() method will return true to indicate that the action has taken place, false 
// otherwise. Returning false will indicate to the command processor that the action is 
// not undoable and should not be added to the command history.
//----------------------------------------------------------------------------------------

lmScoreCommand::lmScoreCommand(const wxString& sName, lmScoreDocument *pDoc,
                               lmVStaffCursor* pVCursor, bool fHistory, int nOptions)
    : wxCommand(true, sName)
{
    m_pDoc = pDoc;
	m_fDocModified = false;
    m_fHistory = fHistory;
    m_nOptions = nOptions;
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
    m_pDoc->UpdateAllViews(fScoreModified, new lmUpdateHint(m_nOptions | nOptions) );

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
    lm_eObjBeam,
    //lm_eObjBrace,
    //lm_eObjBracket,
    //lm_eObjGlyph,
    //lm_eObjInvisible,
    //lm_eObjLine,
    //lm_eObjStaff,
    lm_eObjStaffObj,
    //lm_eObjStem,
    lm_eObjText,
	lm_eObjTextBlock,
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

            case eGMO_ShapeBeam:
                {
                    lmDeletedSO* pSOData = new lmDeletedSO;
                    pSOData->nObjType = lm_eObjBeam;
                    pSOData->pObj = (void*)NULL;
                    pSOData->fObjDeleted = false;
                    pSOData->pParm1 = (void*)( ((lmShapeBeam*)pGMO)->GetScoreOwner() );   //a note in the beam
                    pSOData->pParm2 = (void*)NULL;

                    m_ScoreObjects.push_back( pSOData );
                    sCmdName = _T("Delete beam");
                }
                break;

            case eGMO_ShapeText:
                {
                    lmDeletedSO* pSOData = new lmDeletedSO;
                    pSOData->nObjType = lm_eObjText;
                    pSOData->pObj = (void*)NULL;
                    pSOData->fObjDeleted = false;
                    pSOData->pParm1 = (void*)( ((lmShapeText*)pGMO)->GetScoreOwner() );
                    pSOData->pParm2 = (void*)NULL;

                    m_ScoreObjects.push_back( pSOData );
                    sCmdName = _T("Delete text");
                }
                break;

			case eGMO_ShapeTextBlock:
                {
                    lmDeletedSO* pSOData = new lmDeletedSO;
                    pSOData->nObjType = lm_eObjText;
                    pSOData->pObj = (void*)NULL;
                    pSOData->fObjDeleted = false;
                    pSOData->pParm1 = (void*)( ((lmShapeTextBlock*)pGMO)->GetScoreOwner() );
                    pSOData->pParm2 = (void*)NULL;

                    m_ScoreObjects.push_back( pSOData );
                    sCmdName = _T("Delete text");
                }
                break;

            //case eGMO_ShapeStaff:
            //case eGMO_ShapeArch:
            //case eGMO_ShapeBrace:
            //case eGMO_ShapeBracket:
            //case eGMO_ShapeComposite:
            //case eGMO_ShapeGlyph:
            //case eGMO_ShapeInvisible:
            //case eGMO_ShapeLine:
            //case eGMO_ShapeMultiAttached:
            //case eGMO_ShapeStem:
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
                case lm_eObjBeam:       delete (lmStaffObj*)(*it)->pObj;        break;
				case lm_eObjText:		delete (lmTextItem*)(*it)->pObj;        break;
				case lm_eObjTextBlock:	delete (lmTextItem*)(*it)->pObj;        break;
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
    lmEditCmd* pECmd;
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
                        pECmd = new lmECmdDeleteTie(pVStaff, pUndoItem, pEndNote);
                        //wxLogMessage(_T("[lmCmdDeleteSelection::Do] Deleting tie"));
                    }
                    else
                        fSkipCmd = true;
                }
                break;

            case lm_eObjBeam:
                {
                    lmNote* pNote = (lmNote*)(*it)->pParm1;
                    //if the owner notes are also in the selection, they could get deleted
                    //before the beam, causing automatically the removal of the beam.
                    //So let's check that the beam still exists
                    if (pNote->IsBeamed())
                    {
                        lmVStaff* pVStaff = pNote->GetVStaff();
                        pECmd = new lmECmdDeleteBeam(pVStaff, pUndoItem, pNote);
                        //wxLogMessage(_T("[lmCmdDeleteSelection::Do] Deleting beam"));
                    }
                    else
                        fSkipCmd = true;
                }
                break;

            case lm_eObjStaffObj:
                {
                    lmStaffObj* pSO = (lmStaffObj*)(*it)->pObj;
                    lmVStaff* pVStaff = pSO->GetVStaff();      //affected VStaff
                    if (pSO->IsClef())
                        pECmd = new lmECmdDeleteClef(pVStaff, pUndoItem, (lmClef*)pSO);
                    else if (pSO->IsTimeSignature())
                        pECmd = new lmECmdDeleteTimeSignature(pVStaff, pUndoItem,
                                                              (lmTimeSignature*)pSO);
                    else if (pSO->IsKeySignature())
                        pECmd = new lmECmdDeleteKeySignature(pVStaff, pUndoItem,
                                                             (lmKeySignature*)pSO);
                    else
                        pECmd = new lmECmdDeleteStaffObj(pVStaff, pUndoItem, pSO);
                    //wxLogMessage(_T("[lmCmdDeleteSelection::Do] Deleting staffobj"));
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
                        pECmd = new lmECmdDeleteTuplet(pVStaff, pUndoItem, pStartNote);
                        //wxLogMessage(_T("[lmCmdDeleteSelection::Do] Deleting tuplet"));
                    }
                    else
                        fSkipCmd = true;
                }
                break;

            case lm_eObjText:
			case lm_eObjTextBlock:
                {
                    lmScoreText* pText = (lmScoreText*)(*it)->pParm1;
					lmComponentObj* pAnchor = (lmComponentObj*)pText->GetParentScoreObj();
					pECmd = new lmEDeleteText(pText, pAnchor, pUndoItem);
					//wxLogMessage(_T("[lmCmdDeleteSelection::Do] Deleting tuplet. %s"),
					//	(pECmd->Success() ? _T("Success") : _T("Fail")) );
				}
                break;

			//case lm_eObjArch:
            //case lm_eObjBrace:
            //case lm_eObjBracket:
            //case lm_eObjComposite:
            //case lm_eObjGlyph:
            //case lm_eObjInvisible:
            //case lm_eObjLine:
            //case lm_eObjStaff:
            //case lm_eObjStem:
            //    break;

            default:
                wxASSERT(false);
        }

        if (fSkipCmd)
            delete pUndoItem;
        else
        {
            if (pECmd->Success())
            {
                (*it)->fObjDeleted = true;                //the Obj is no longer owned by the score
                m_UndoLog.LogCommand(pECmd, pUndoItem);
            }
            else
            {
                fSuccess = false;
                delete pUndoItem;
                delete pECmd;
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
    lmEditCmd* pECmd;
    if (m_pSO->IsClef())
        pECmd = new lmECmdDeleteClef(m_pVStaff, pUndoItem, (lmClef*)m_pSO);
    else if (m_pSO->IsTimeSignature())
        pECmd = new lmECmdDeleteTimeSignature(m_pVStaff, pUndoItem,
                                                (lmTimeSignature*)m_pSO);
    else if (m_pSO->IsKeySignature())
        pECmd = new lmECmdDeleteKeySignature(m_pVStaff, pUndoItem,
                                                (lmKeySignature*)m_pSO);
    else
        pECmd = new lmECmdDeleteStaffObj(m_pVStaff, pUndoItem, m_pSO);

    if (pECmd->Success())
    {
        m_fDeleteSO = true;                //m_pSO is no longer owned by the score
        m_UndoLog.LogCommand(pECmd, pUndoItem);
	    return CommandDone(lmSCORE_MODIFIED);
    }
    else
    {
        m_fDeleteSO = false;
        delete pUndoItem;
        delete pECmd;
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
    lmEditCmd* pECmd = new lmECmdDeleteTie(m_pEndNote->GetVStaff(), pUndoItem, m_pEndNote);

    if (pECmd->Success())
    {
        m_UndoLog.LogCommand(pECmd, pUndoItem);
	    return CommandDone(lmSCORE_MODIFIED);
    }
    else
    {
        delete pUndoItem;
        delete pECmd;
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
    lmEditCmd* pECmd = new lmECmdAddTie(m_pEndNote->GetVStaff(), pUndoItem,
                                          m_pStartNote, m_pEndNote);

    if (pECmd->Success())
    {
        m_UndoLog.LogCommand(pECmd, pUndoItem);
	    return CommandDone(lmSCORE_MODIFIED);
    }
    else
    {
        delete pUndoItem;
        delete pECmd;
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
    //Direct command. NO UNDO LOG

    wxASSERT_MSG( m_pSO, _T("[lmCmdUserMoveScoreObj::Do] No ScoreObj to move!"));

    m_tOldPos = m_pSO->SetUserLocation(m_tPos, m_nShapeIdx);

	return CommandDone(lmSCORE_MODIFIED);  //, lmDO_ONLY_REDRAW);
}

bool lmCmdUserMoveScoreObj::UndoCommand()
{
    //Direct command. NO UNDO LOG

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
    lmEditCmd* pECmd = new lmECmdInsertBarline(pVStaff, pUndoItem, m_nBarlineType);

    if (pECmd->Success())
    {
        m_UndoLog.LogCommand(pECmd, pUndoItem);
	    return CommandDone(lmSCORE_MODIFIED);
    }
    else
    {
        delete pUndoItem;
        delete pECmd;
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
    lmEditCmd* pECmd = new lmECmdInsertClef(pVStaff, pUndoItem, m_nClefType, nStaff);

    if (pECmd->Success())
    {
        m_UndoLog.LogCommand(pECmd, pUndoItem);
	    return CommandDone(lmSCORE_MODIFIED);
    }
    else
    {
        delete pUndoItem;
        delete pECmd;
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
    lmEditCmd* pECmd = new lmECmdInsertTimeSignature(pVStaff, pUndoItem, m_nBeats,
                                                       m_nBeatType, m_fVisible);

    if (pECmd->Success())
    {
        m_UndoLog.LogCommand(pECmd, pUndoItem);
	    return CommandDone(lmSCORE_MODIFIED);
    }
    else
    {
        delete pUndoItem;
        delete pECmd;
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
    lmEditCmd* pECmd = new lmECmdInsertKeySignature(pVStaff, pUndoItem, m_nFifths,
                                                      m_fMajor, m_fVisible);

    if (pECmd->Success())
    {
        m_UndoLog.LogCommand(pECmd, pUndoItem);
	    return CommandDone(lmSCORE_MODIFIED);
    }
    else
    {
        delete pUndoItem;
        delete pECmd;
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
                                 int nVoice, lmNote* pBaseOfChord, bool fTiedPrev)
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
	m_nVoice = nVoice;
	m_pBaseOfChord = pBaseOfChord;
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

    lmEditCmd* pECmd = new lmECmdInsertNote(m_pVStaff, pUndoItem, m_nPitchType, m_nStep,
                                             m_nOctave, m_nNoteType, m_rDuration, m_nDots, 
                                             m_nNotehead, m_nAcc, m_nVoice, m_pBaseOfChord,
											 m_fTiedPrev);

    if (pECmd->Success())
    {
        m_UndoLog.LogCommand(pECmd, pUndoItem);
	    return CommandDone(lmSCORE_MODIFIED);
    }
    else
    {
        delete pUndoItem;
        delete pECmd;
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
                                 lmScoreDocument *pDoc, lmENoteType nNoteType,
                                 float rDuration, int nDots, int nVoice)
	: lmScoreCommand(sName, pDoc, pVCursor)
{
	m_nNoteType = nNoteType;
    m_nDots = nDots;
	m_rDuration = rDuration;
    m_nVoice = nVoice;
}

lmCmdInsertRest::~lmCmdInsertRest()
{
}

bool lmCmdInsertRest::Do()
{
    lmScoreCursor* pCursor = m_pDoc->GetScore()->SetNewCursorState(&m_tCursorState);
    m_pVStaff = pCursor->GetVStaff();

    lmUndoItem* pUndoItem = new lmUndoItem(&m_UndoLog);

    lmEditCmd* pECmd = new lmECmdInsertRest(m_pVStaff, pUndoItem, m_nNoteType,
                                            m_rDuration, m_nDots, m_nVoice);

    if (pECmd->Success())
    {
        m_UndoLog.LogCommand(pECmd, pUndoItem);
	    return CommandDone(lmSCORE_MODIFIED);
    }
    else
    {
        delete pUndoItem;
        delete pECmd;
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
    //Direct command. NO UNDO LOG

	m_pNote->ChangePitch(m_nSteps);

	return CommandDone(lmSCORE_MODIFIED);
}

bool lmCmdChangeNotePitch::UndoCommand()
{
    //Direct command. NO UNDO LOG

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
    lmEditCmd* pECmd;
    std::list<lmNoteRest*>::iterator it;
    for (it = m_NoteRests.begin(); it != m_NoteRests.end(); ++it)
    {
        lmUndoItem* pUndoItem = new lmUndoItem(&m_UndoLog);
        lmVStaff* pVStaff = (*it)->GetVStaff();      //affected VStaff
        pECmd = new lmECmdChangeDots(pVStaff, pUndoItem, *it, m_nDots);

        if (pECmd->Success())
            m_UndoLog.LogCommand(pECmd, pUndoItem);     //save command in the undo log
        else
        {
            fSuccess = false;
            delete pUndoItem;
            delete pECmd;
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
    lmEditCmd* pECmd = new lmECmdDeleteTuplet(pVStaff, pUndoItem, m_pStartNR);

    if (pECmd->Success())
    {
        m_UndoLog.LogCommand(pECmd, pUndoItem);
	    return CommandDone(lmSCORE_MODIFIED);
    }
    else
    {
        delete pUndoItem;
        delete pECmd;
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
    lmEditCmd* pECmd = 
        new lmECmdAddTuplet(pVStaff, pUndoItem, m_NotesRests,  m_fShowNumber, m_nNumber,
                            m_fBracket, m_nAbove, m_nActual, m_nNormal);

    if (pECmd->Success())
    {
        m_UndoLog.LogCommand(pECmd, pUndoItem);
	    return CommandDone(lmSCORE_MODIFIED);
    }
    else
    {
        delete pUndoItem;
        delete pECmd;
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
    lmEditCmd* pECmd = new lmECmdBreakBeam(m_pBeforeNR->GetVStaff(), pUndoItem, m_pBeforeNR);

    if (pECmd->Success())
    {
        m_UndoLog.LogCommand(pECmd, pUndoItem);
	    return CommandDone(lmSCORE_MODIFIED);
    }
    else
    {
        delete pUndoItem;
        delete pECmd;
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




//----------------------------------------------------------------------------------------
// lmCmdJoinBeam implementation
//----------------------------------------------------------------------------------------

lmCmdJoinBeam::lmCmdJoinBeam(lmVStaffCursor* pVCursor, const wxString& name,
                                     lmScoreDocument *pDoc, lmGMSelection* pSelection)
        : lmScoreCommand(name, pDoc, pVCursor)
{
    //loop to save the note/rests to beam
    lmGMObject* pGMO = pSelection->GetFirst();
    while (pGMO)
    {
        if (pGMO->GetType() == eGMO_ShapeNote || pGMO->GetType() == eGMO_ShapeRest)
        {
            lmNoteRest* pNR = (lmNoteRest*)pGMO->GetScoreOwner();
            //exclude notes in chord
            if (pNR->IsRest() || (pNR->IsNote() && 
                                  (!((lmNote*)pNR)->IsInChord() ||
                                   ((lmNote*)pNR)->IsBaseOfChord() )) )
                m_NotesRests.push_back(pNR);
        }
        pGMO = pSelection->GetNext();
    }
}

lmCmdJoinBeam::~lmCmdJoinBeam()
{
}

bool lmCmdJoinBeam::Do()
{
    //Proceed to create the beam
    lmUndoItem* pUndoItem = new lmUndoItem(&m_UndoLog);
    lmVStaff* pVStaff = m_NotesRests.front()->GetVStaff();
    lmEditCmd* pECmd = new lmECmdJoinBeam(pVStaff, pUndoItem, m_NotesRests);

    if (pECmd->Success())
    {
        m_UndoLog.LogCommand(pECmd, pUndoItem);
	    return CommandDone(lmSCORE_MODIFIED);
    }
    else
    {
        delete pUndoItem;
        delete pECmd;
        return false;
    }
}

bool lmCmdJoinBeam::UndoCommand()
{
    //undelete the object

    m_UndoLog.UndoAll();
    m_pDoc->GetScore()->SetNewCursorState(&m_tCursorState);

	m_pDoc->Modify(m_fDocModified);
    m_pDoc->UpdateAllViews();
    return true;
}




//----------------------------------------------------------------------------------------
// lmCmdChangeText: Change ScoreText properties
//----------------------------------------------------------------------------------------

lmCmdChangeText::lmCmdChangeText(lmVStaffCursor* pVCursor, const wxString& name,
                                 lmScoreDocument *pDoc, lmScoreText* pST,
                                 wxString& sText, lmEHAlign nHAlign, 
                                 lmLocation tPos, lmTextStyle* pStyle,
                                 int nHintOptions)
	: lmScoreCommand(name, pDoc, pVCursor, true, nHintOptions)
{
    m_pST = pST;
    m_sText = sText;
    m_nHAlign = nHAlign; 
    m_tPos = tPos;
    m_pStyle = pStyle;
}

lmCmdChangeText::~lmCmdChangeText()
{
}

bool lmCmdChangeText::Do()
{
    lmUndoItem* pUndoItem = new lmUndoItem(&m_UndoLog);
    lmEditCmd* pECmd = new lmECmdChangeText(m_pST, pUndoItem, m_sText, m_nHAlign,
                                            m_tPos, m_pStyle);

    if (pECmd->Success())
    {
        m_UndoLog.LogCommand(pECmd, pUndoItem);
	    return CommandDone(lmSCORE_MODIFIED);
    }
    else
    {
        delete pUndoItem;
        delete pECmd;
        return false;
    }
}

bool lmCmdChangeText::UndoCommand()
{
    m_UndoLog.UndoAll();
    m_pDoc->GetScore()->SetNewCursorState(&m_tCursorState);

	m_pDoc->Modify(m_fDocModified);
    m_pDoc->UpdateAllViews();
    return true;
}



//----------------------------------------------------------------------------------------
// lmCmdChangePageMargin implementation
//----------------------------------------------------------------------------------------

lmCmdChangePageMargin::lmCmdChangePageMargin(const wxString& name, lmScoreDocument *pDoc,
                                             lmGMObject* pGMO, int nIdx, int nPage,
											 lmLUnits uPos)
	: lmScoreCommand(name, pDoc, (lmVStaffCursor*)NULL )
{
	m_nIdx = nIdx;
	m_uNewPos = uPos;
	m_nPage = nPage;

    //save current position
    m_pScore = pDoc->GetScore();
    switch(m_nIdx)
    {
        case lmMARGIN_TOP:
            m_uOldPos = m_pScore->GetPageTopMargin(nPage);
            break;

        case lmMARGIN_BOTTOM:
            m_uOldPos = m_pScore->GetMaximumY(nPage);
            break;

        case lmMARGIN_LEFT:
            m_uOldPos = m_pScore->GetLeftMarginXPos(nPage);
            break;

        case lmMARGIN_RIGHT:
            m_uOldPos = m_pScore->GetRightMarginXPos(nPage);
            break;

        default:
            wxASSERT(false);
    }

}

bool lmCmdChangePageMargin::Do()
{
    //Direct command. NO UNDO LOG

    ChangeMargin(m_uNewPos);
	return CommandDone(lmSCORE_MODIFIED);  //, lmDO_ONLY_REDRAW);
}

bool lmCmdChangePageMargin::UndoCommand()
{
    //Direct command. NO UNDO LOG

    ChangeMargin(m_uOldPos);
	m_pDoc->Modify(m_fDocModified);
    m_pDoc->UpdateAllViews();
    return true;
}

void lmCmdChangePageMargin::ChangeMargin(lmLUnits uPos)
{
    lmUSize size = m_pScore->GetPaperSize(m_nPage);

    switch(m_nIdx)
    {
        case lmMARGIN_TOP:
            m_pScore->SetPageTopMargin(uPos, m_nPage);
            break;

        case lmMARGIN_BOTTOM:
            m_pScore->SetPageBottomMargin(size.Height() - uPos, m_nPage);
            break;

        case lmMARGIN_LEFT:
            m_pScore->SetPageLeftMargin(uPos, m_nPage);
            break;

        case lmMARGIN_RIGHT:
            m_pScore->SetPageRightMargin(size.Width() - uPos, m_nPage);
            break;

        default:
            wxASSERT(false);
    }
}



//----------------------------------------------------------------------------------------
// lmCmdAttachNewText implementation
//----------------------------------------------------------------------------------------

lmCmdAttachNewText::lmCmdAttachNewText(const wxString& name, lmScoreDocument *pDoc,
                                       lmComponentObj* pAnchor)
	: lmScoreCommand(name, pDoc, (lmVStaffCursor*)NULL )
{
	m_pAnchor = pAnchor;
    m_fDeleteText = false;

    //Create the text
    lmTextStyle* pStyle = pAnchor->GetScore()->GetStyleInfo(_("Normal text"));
    wxASSERT(pStyle);

    //create the text object
    wxString sText = _T("");
    m_pNewText = new lmTextItem(sText, lmHALIGN_DEFAULT, pStyle);

	//This is dirty: To use OnEditProperties() the text must be on the score. so I will
	//attach it provisionally to the score
	pAnchor->GetScore()->AttachAuxObj(m_pNewText);

    //show dialog to create the text
	lmDlgProperties dlg((lmController*)NULL);
	m_pNewText->OnEditProperties(&dlg);
	dlg.Layout();
	dlg.ShowModal();

	//dettach the text from the score
	pAnchor->GetScore()->DetachAuxObj(m_pNewText);
}

lmCmdAttachNewText::~lmCmdAttachNewText()
{
	if (m_pNewText && m_fDeleteText)
        delete m_pNewText;
}

bool lmCmdAttachNewText::Do()
{
    //Direct command. NO UNDO LOG

    m_pAnchor->AttachAuxObj(m_pNewText);
    m_fDeleteText = false;
	return CommandDone(lmSCORE_MODIFIED);  //, lmDO_ONLY_REDRAW);
}

bool lmCmdAttachNewText::UndoCommand()
{
    //Direct command. NO UNDO LOG

    m_pAnchor->DetachAuxObj(m_pNewText);
    m_fDeleteText = true;
	m_pDoc->Modify(m_fDocModified);
    m_pDoc->UpdateAllViews();
    return true;
}



//----------------------------------------------------------------------------------------
// lmCmdAddNewTitle implementation
//----------------------------------------------------------------------------------------

lmCmdAddNewTitle::lmCmdAddNewTitle(lmScoreDocument *pDoc)
	: lmScoreCommand(_("add title"), pDoc, (lmVStaffCursor*)NULL )
{
    m_fDeleteTitle = false;

    //Create the text
    lmTextStyle* pStyle = pDoc->GetScore()->GetStyleInfo(_("Title"));
    wxASSERT(pStyle);

    //create the text object
    wxString sTitle = _T("");
    m_pNewTitle = new lmTextBlock(sTitle, lmBLOCK_ALIGN_BOTH, lmHALIGN_DEFAULT,
								  lmVALIGN_DEFAULT, pStyle);

	//This is dirty: To use OnEditProperties() the text must be on the score. so I will
	//attach it provisionally to the score
	pDoc->GetScore()->AttachAuxObj(m_pNewTitle);

    //show dialog to create the text
	lmDlgProperties dlg((lmController*)NULL);
	m_pNewTitle->OnEditProperties(&dlg);
	dlg.Layout();
	if (dlg.ShowModal() == wxID_OK)
        pDoc->GetScore()->OnPropertiesChanged();

	//dettach the text from the score
	pDoc->GetScore()->DetachAuxObj(m_pNewTitle);
}

lmCmdAddNewTitle::~lmCmdAddNewTitle()
{
	if (m_pNewTitle && m_fDeleteTitle)
        delete m_pNewTitle;
}

bool lmCmdAddNewTitle::Do()
{
    //Direct command. NO UNDO LOG

    if (m_pNewTitle->GetText() != _T(""))
    {
		m_pDoc->GetScore()->AttachAuxObj(m_pNewTitle);
		m_fDeleteTitle = false;
		return CommandDone(lmSCORE_MODIFIED); 
    }
    else
    {
        m_fDeleteTitle = true;
        return false;
    }
}

bool lmCmdAddNewTitle::UndoCommand()
{
    //Direct command. NO UNDO LOG

    m_pDoc->GetScore()->DetachAuxObj(m_pNewTitle);
    m_fDeleteTitle = true;
	m_pDoc->Modify(m_fDocModified);
    m_pDoc->UpdateAllViews();
    return true;
}



//----------------------------------------------------------------------------------------
// lmCmdChangeBarline implementation
//----------------------------------------------------------------------------------------

lmCmdChangeBarline::lmCmdChangeBarline(lmScoreDocument *pDoc, lmBarline* pBL,
									   lmEBarline nType, bool fVisible)
	: lmScoreCommand(_("change barline"), pDoc, (lmVStaffCursor*)NULL )
{
    m_pBL = pBL;
    m_nType = nType;
	m_fVisible = fVisible;
    m_nOldType = m_pBL->GetBarlineType();
	m_fOldVisible = m_pBL->IsVisible();
}

lmCmdChangeBarline::~lmCmdChangeBarline()
{
}

bool lmCmdChangeBarline::Do()
{
    //Direct command. NO UNDO LOG

    m_pBL->SetBarlineType(m_nType);
    m_pBL->SetVisible(m_fVisible);
	return CommandDone(lmSCORE_MODIFIED);
}

bool lmCmdChangeBarline::UndoCommand()
{
    //Direct command. NO UNDO LOG

    m_pBL->SetBarlineType(m_nOldType);
    m_pBL->SetVisible(m_fOldVisible);
	m_pDoc->Modify(m_fDocModified);
    m_pDoc->UpdateAllViews();
    return true;
}



//----------------------------------------------------------------------------------------
// lmCmdChangeMidiSettings implementation
//----------------------------------------------------------------------------------------

lmCmdChangeMidiSettings::lmCmdChangeMidiSettings(lmScoreDocument *pDoc,
                                                 lmInstrument* pInstr,
                                                 int nMidiChannel,
                                                 int nMidiInstr)
	: lmScoreCommand(_("change MIDI settings"), pDoc, (lmVStaffCursor*)NULL )
{
    m_pInstr = pInstr;
    m_nMidiChannel = nMidiChannel;
    m_nMidiInstr = nMidiInstr;
    m_nOldMidiChannel = pInstr->GetMIDIChannel();
    m_nOldMidiInstr = pInstr->GetMIDIInstrument();
}

lmCmdChangeMidiSettings::~lmCmdChangeMidiSettings()
{
}

bool lmCmdChangeMidiSettings::Do()
{
    //Direct command. NO UNDO LOG

    m_pInstr->SetMIDIChannel(m_nMidiChannel);
    m_pInstr->SetMIDIInstrument(m_nMidiInstr);
	return CommandDone(lmSCORE_MODIFIED, lmDO_ONLY_REDRAW);
}

bool lmCmdChangeMidiSettings::UndoCommand()
{
    //Direct command. NO UNDO LOG

    m_pInstr->SetMIDIChannel(m_nOldMidiChannel);
    m_pInstr->SetMIDIInstrument(m_nOldMidiInstr);

	m_pDoc->Modify(m_fDocModified);
    m_pDoc->UpdateAllViews();
    return true;
}



//----------------------------------------------------------------------------------------
// lmCmdMoveNote implementation
//----------------------------------------------------------------------------------------

lmCmdMoveNote::lmCmdMoveNote(lmScoreDocument *pDoc, lmNote* pNote, const lmUPoint& uPos,
							 int nSteps)
	: lmScoreCommand(_("move note"), pDoc, (lmVStaffCursor*)NULL )
{
	m_tPos.x = uPos.x;
	m_tPos.xType = lmLOCATION_USER_RELATIVE;
	m_tPos.xUnits = lmLUNITS;
	m_tPos.y = uPos.y;	//(g_fFreeMove ? uPos.y : pNote->GetUserShift().y);
	m_tPos.yType = lmLOCATION_USER_RELATIVE;
	m_tPos.yUnits = lmLUNITS;

	m_pNote = pNote;
    m_nSteps = nSteps;
}

bool lmCmdMoveNote::Do()
{
    //m_tOldPos = m_pNote->SetUserLocation(m_tPos);
	m_pNote->ChangePitch(m_nSteps);

	return CommandDone(lmSCORE_MODIFIED);
}

bool lmCmdMoveNote::UndoCommand()
{
    //Direct command. NO UNDO LOG

	//m_pNote->SetUserLocation(m_tOldPos);
	m_pNote->ChangePitch(-m_nSteps);

	m_pDoc->Modify(m_fDocModified);
    m_pDoc->UpdateAllViews();
    return true;

}

