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
#include "../score/VStaff.h"
#include "../score/properties/DlgProperties.h"
#include "ScoreCommand.h"
#include "ScoreDoc.h"
#include "TheApp.h"
#include "../graphic/GMObject.h"
#include "../graphic/ShapeArch.h"
#include "../graphic/ShapeBeam.h"
#include "../graphic/ShapeText.h"
#include "../app/Preferences.h"
#include "../ldp_parser/LDPParser.h"


//----------------------------------------------------------------------------------------
// lmScoreCommand abstract class implementation
//
// Do() method will return true to indicate that the action has taken place, false
// otherwise. Returning false will indicate to the command processor that the action is
// not undoable and should not be added to the command history.
//----------------------------------------------------------------------------------------

lmScoreCommand::lmScoreCommand(const wxString& sName, lmDocument *pDoc,
                                     bool fUndoable,
                                     int nOptions, bool fUpdateViews)
    : wxCommand(true, sName)
      , m_pDoc(pDoc)
	  , m_fDocModified(false)
      , m_fUndoable(fUndoable)
      , m_nOptions(nOptions)
      , m_fUpdateViews(fUpdateViews)
{
}

lmScoreCommand::~lmScoreCommand()
{
}

void lmScoreCommand::LogCommand()
{
    //log command if undoable

    if (!m_fUndoable)
        return;

    //get score and save source code
    m_sOldSource = m_pDoc->GetScore()->SourceLDP(true);     //true: export cursor
}

lmVStaff* lmScoreCommand::GetVStaff()
{
    return m_pDoc->GetScore()->GetCursor()->GetVStaff();
}

lmStaffObj* lmScoreCommand::GetStaffObj(lmIRef& nIRef)
{
    return m_pDoc->GetScore()->GetCursor()->GetReferredObject(nIRef);
}

bool lmScoreCommand::Undo()
{
    //Default implementation: Restore previous state from LDP source code
    //Returns true to indicate that the action has taken place, false otherwise.
    //Returning false will indicate to the command processor that the action is
    //not redoable and no change should be made to the command history.

    //recover old score and cursor state (it is saved in the score)
    lmLDPParser parser;
    lmScore* pScore = parser.ParseScoreFromText(m_sOldSource);
    if (!pScore)
    {
        wxASSERT(false);
        return false;
    }

    //ask document to replace current score by the old one
    m_pDoc->ReplaceScore(pScore);
    return true;        //undo action has taken place
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

    //update views
    m_pDoc->UpdateAllViews((wxView*)NULL, new lmUpdateHint(m_nOptions | nOptions) );

    return m_fUndoable;
}

bool lmScoreCommand::CommandUndone(int nOptions)
{
    //common code after executing an Undo operation:
    //- reset document to previous 'modified' state
    //- update the views with the changes
    //
    //Returns true to indicate that the action has taken place, false otherwise.
    //Returning false will indicate to the command processor that the action is
    //not redoable and no change should be made to the command history.


    //update views
	m_pDoc->Modify(true);           //set modified, so that the score is repainted
    m_pDoc->UpdateAllViews((wxView*)NULL, new lmUpdateHint(m_nOptions | nOptions));

	m_pDoc->Modify(m_fDocModified);     //restore modified state
    return true;
}



//----------------------------------------------------------------------------------------
// lmCmdDeleteSelection implementation
//----------------------------------------------------------------------------------------

lmCmdDeleteSelection::lmCmdDeleteSelection(bool fUndoable,
                                           const wxString& sName,
                                           lmDocument *pDoc,
                                           lmGMSelection* pSelection)
	: lmScoreCommand(sName, pDoc, fUndoable)
{
    //loop to create and save the commands to delete the selected objects
    //AWARE: commands are marked as lmNO_UNDO so that they do not save the score
    wxString sCmdName;
    lmGMObject* pGMO = pSelection->GetFirst();
    while (pGMO)
    {
        switch(pGMO->GetType())
        {
            case eGMO_ShapeTie:
                {
                    //sCmdName = _T("Delete tie");
                    //lmScoreCommand* pCmd 
                    //    = new lmCmdDeleteTie(lmNO_UNDO, tCursorState, sCmdName, pDoc,
                    //                         ((lmShapeTie*)pGMO)->GetEndNote() );
                    //m_Commands.push_back( pCmd );
                }
                break;

            case eGMO_ShapeBarline:
            case eGMO_ShapeClef:
            case eGMO_ShapeNote:
            case eGMO_ShapeRest:
                {
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
                        default:
                            wxASSERT(false);
                    }
                    lmScoreCommand* pCmd 
                        = new lmCmdDeleteStaffObj(lmNO_UNDO, sCmdName, pDoc,
                                             (lmStaffObj*)pGMO->GetScoreOwner() );
                    m_Commands.push_back( pCmd );
               }
                break;

            case eGMO_ShapeTuplet:
                {
                    //lmToDeleteSO* pSOData = new lmToDeleteSO;
                    //pSOData->nObjType = lm_eObjTuplet;
                    //pSOData->nIRef = (void*)( ((lmShapeTuplet*)pGMO)->GetScoreOwner() );   //start note

                    //m_Commands.push_back( pSOData );
                    //sCmdName = _T("Delete tuplet");
                    //lmScoreCommand* pCmd 
                    //    = new lmCmdDeleteStaffObj(lmNO_UNDO, tCursorState, sCmdName, pDoc,
                    //                         (lmStaffObj*)pGMO->GetScoreOwner() );
                    //m_Commands.push_back( pCmd );
                }
                break;

            case eGMO_ShapeBeam:
                {
                    //lmToDeleteSO* pSOData = new lmToDeleteSO;
                    //pSOData->nObjType = lm_eObjBeam;
                    //pSOData->pObj = (void*)NULL;
                    //pSOData->fObjDeleted = false;
                    //pSOData->pParm1 = (void*)( ((lmShapeBeam*)pGMO)->GetScoreOwner() );   //a note in the beam
                    //pSOData->pParm2 = (void*)NULL;

                    //m_Commands.push_back( pSOData );
                    //sCmdName = _T("Delete beam");
                    //lmScoreCommand* pCmd 
                    //    = new lmCmdDeleteStaffObj(lmNO_UNDO, tCursorState, sCmdName, pDoc,
                    //                         (lmStaffObj*)pGMO->GetScoreOwner() );
                    //m_Commands.push_back( pCmd );
                }
                break;

            case eGMO_ShapeText:
                {
                    //lmToDeleteSO* pSOData = new lmToDeleteSO;
                    //pSOData->nObjType = lm_eObjText;
                    //pSOData->pObj = (void*)NULL;
                    //pSOData->fObjDeleted = false;
                    //pSOData->pParm1 = (void*)( ((lmShapeText*)pGMO)->GetScoreOwner() );
                    //pSOData->pParm2 = (void*)NULL;

                    //m_Commands.push_back( pSOData );
                    //sCmdName = _T("Delete text");
                    //lmScoreCommand* pCmd 
                    //    = new lmCmdDeleteStaffObj(lmNO_UNDO, tCursorState, sCmdName, pDoc,
                    //                         (lmStaffObj*)pGMO->GetScoreOwner() );
                    //m_Commands.push_back( pCmd );
                }
                break;

			case eGMO_ShapeTextBlock:
                {
                    //lmToDeleteSO* pSOData = new lmToDeleteSO;
                    //pSOData->nObjType = lm_eObjText;
                    //pSOData->pObj = (void*)NULL;
                    //pSOData->fObjDeleted = false;
                    //pSOData->pParm1 = (void*)( ((lmShapeTitle*)pGMO)->GetScoreOwner() );
                    //pSOData->pParm2 = (void*)NULL;

                    //m_Commands.push_back( pSOData );
                    //sCmdName = _T("Delete text");
                    //lmScoreCommand* pCmd 
                    //    = new lmCmdDeleteStaffObj(lmNO_UNDO, tCursorState, sCmdName, pDoc,
                    //                         (lmStaffObj*)pGMO->GetScoreOwner() );
                    //m_Commands.push_back( pCmd );
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
                    pGMO->GetName().c_str(), pGMO->GetType() ));
        }
        wxLogMessage(_T("[lmCmdDeleteSelection::lmCmdDeleteSelection] %s"), sCmdName.c_str());
        pGMO = pSelection->GetNext();
    }

    //if only one object, change command name for better command identification
    if (pSelection->NumObjects() == 1)
        this->m_commandName = sCmdName;
}

lmCmdDeleteSelection::~lmCmdDeleteSelection()
{
    //delete stored data
    std::list<lmScoreCommand*>::iterator it;
    for (it = m_Commands.begin(); it != m_Commands.end(); ++it)
        delete *it;
    m_Commands.clear();
}

bool lmCmdDeleteSelection::Do()
{
    //log command if undoable
    LogCommand();

    //loop to issue delete commnads
    std::list<lmScoreCommand*>::iterator it;
    for (it = m_Commands.begin(); it != m_Commands.end(); ++it)
    {
        (*it)->Do();
    }

    return CommandDone(lmSCORE_MODIFIED);
}




//----------------------------------------------------------------------------------------
// lmCmdDeleteStaffObj implementation
//----------------------------------------------------------------------------------------

lmCmdDeleteStaffObj::lmCmdDeleteStaffObj(bool fUndoable, 
                                         const wxString& sName,
                                         lmDocument *pDoc, lmStaffObj* pSO)
	: lmScoreCommand(sName, pDoc, fUndoable)
{
    m_nIRef = pSO->GetIRef();
}

bool lmCmdDeleteStaffObj::Do()
{
    //log command if undoable
    LogCommand();

    //Get pointer to object to delete
    lmStaffObj* pSO = GetStaffObj(m_nIRef);

    //Proceed to delete the object
    bool fError = false;
    if (pSO->IsClef())
        fError = GetVStaff()->CmdNew_DeleteClef((lmClef*)pSO);
    else if (pSO->IsTimeSignature())
        fError = GetVStaff()->CmdNew_DeleteTimeSignature((lmTimeSignature*)pSO);
    else if (pSO->IsKeySignature())
        fError = GetVStaff()->CmdNew_DeleteKeySignature((lmKeySignature*)pSO);
    else
        fError = GetVStaff()->CmdNew_DeleteStaffObj(pSO);

    if (!fError)
	    return CommandDone(lmSCORE_MODIFIED);
    else
        return false;
}



//----------------------------------------------------------------------------------------
// lmCmdDeleteTie implementation
//----------------------------------------------------------------------------------------

lmCmdDeleteTie::lmCmdDeleteTie(bool fUndoable,  
                               const wxString& sName, lmDocument *pDoc,
                               lmNote* pEndNote)
	: lmScoreCommand(sName, pDoc, fUndoable)
{
    m_nEndNoteIRef = pEndNote->GetIRef();
    m_nStartNoteIRef = pEndNote->GetTiedNotePrev()->GetIRef();
}

bool lmCmdDeleteTie::Do()
{
    //AWARE: Direct command. No need to save the score

    //Get pointer to end note
    lmNote* pEndNote = (lmNote*)GetStaffObj(m_nEndNoteIRef);
    //remove the tie
    pEndNote->DeleteTiePrev();

    return CommandDone(lmSCORE_MODIFIED);
}

bool lmCmdDeleteTie::Undo()
{
    //Get pointers to start and end notes
    lmNote* pEndNote = (lmNote*)GetStaffObj(m_nEndNoteIRef);
    lmNote* pStartNote = (lmNote*)GetStaffObj(m_nStartNoteIRef);
    //re-create the tie
    pEndNote->CreateTie(pStartNote, pEndNote);

    return CommandUndone();
}




//----------------------------------------------------------------------------------------
// lmCmdAddTie implementation
//----------------------------------------------------------------------------------------

lmCmdAddTie::lmCmdAddTie(bool fUndoable,  const wxString& sName,
                         lmDocument *pDoc, lmNote* pStartNote, lmNote* pEndNote)
	: lmScoreCommand(sName, pDoc, fUndoable)
{
    m_nStartNoteIRef = pStartNote->GetIRef();
    m_nEndNoteIRef = pEndNote->GetIRef();
}

bool lmCmdAddTie::Do()
{
    //AWARE: Direct command. No need to save the score

    //Get pointers to start and end notes
    lmNote* pEndNote = (lmNote*)GetStaffObj(m_nEndNoteIRef);
    lmNote* pStartNote = (lmNote*)GetStaffObj(m_nStartNoteIRef);

    //add the tie
    pEndNote->CreateTie(pStartNote, pEndNote);

    return CommandDone(lmSCORE_MODIFIED);
}

bool lmCmdAddTie::Undo()
{
    //Get pointers to start and end notes
    lmNote* pEndNote = (lmNote*)GetStaffObj(m_nEndNoteIRef);
    lmNote* pStartNote = (lmNote*)GetStaffObj(m_nStartNoteIRef);

    //remove the tie
    pEndNote->DeleteTiePrev();

    return CommandUndone();
}



//----------------------------------------------------------------------------------------
// lmCmdMoveObject implementation
//----------------------------------------------------------------------------------------

lmCmdMoveObject::lmCmdMoveObject(bool fUndoable, 
                                 const wxString& sName, lmDocument *pDoc,
								 lmGMObject* pGMO, const lmUPoint& uPos,
                                 bool fUpdateViews)
	: lmScoreCommand(sName, pDoc, fUndoable)
{
	m_tPos.x = uPos.x;
	m_tPos.y = uPos.y;
	m_tPos.xUnits = lmLUNITS;
	m_tPos.yUnits = lmLUNITS;

	m_pSO = pGMO->GetScoreOwner();
    m_nShapeIdx = pGMO->GetOwnerIDX();
    wxASSERT_MSG( m_pSO, _T("[lmCmdMoveObject::Do] No ScoreObj to move!"));
}

bool lmCmdMoveObject::Do()
{
    //Direct command. NO UNDO LOG
    m_tOldPos = m_pSO->SetUserLocation(m_tPos, m_nShapeIdx);
    if (m_fUpdateViews)
	    return CommandDone(lmSCORE_MODIFIED);
    else
	    return CommandDone(lmSCORE_MODIFIED, lmDO_ONLY_REDRAW);
}

bool lmCmdMoveObject::Undo()
{
    //Direct command. NO UNDO LOG
    m_pSO->SetUserLocation(m_tOldPos, m_nShapeIdx);
    if (m_fUpdateViews)
	    return CommandDone(0);
    else
        return CommandUndone(lmDO_ONLY_REDRAW);
}



//----------------------------------------------------------------------------------------
// lmCmdInsertBarline: Insert a barline at current cursor position
//----------------------------------------------------------------------------------------

lmCmdInsertBarline::lmCmdInsertBarline(bool fUndoable,
                                       const wxString& sName, lmDocument *pDoc,
                                       lmEBarline nType)
	: lmScoreCommand(sName, pDoc, fUndoable)
    , m_nBarlineType(nType)
{
}

bool lmCmdInsertBarline::Do()
{
    //log command if undoable
    LogCommand();

    //insert the barline
    lmBarline* pBL = GetVStaff()->CmdNew_InsertBarline(m_nBarlineType);
    if (pBL)
	    return CommandDone(lmSCORE_MODIFIED);
    else
        return false;
}




//----------------------------------------------------------------------------------------
// lmCmdInsertClef: Insert a clef at current cursor position
//----------------------------------------------------------------------------------------

lmCmdInsertClef::lmCmdInsertClef(bool fUndoable,
                                 const wxString& sName,
                                 lmDocument *pDoc, lmEClefType nClefType)
	: lmScoreCommand(sName, pDoc, fUndoable)
    , m_nClefType(nClefType)
{
}

bool lmCmdInsertClef::Do()
{
    //log command if undoable
    LogCommand();

    //insert the barline
    lmClef* pClef = GetVStaff()->CmdNew_InsertClef(m_nClefType);
    if (pClef)
	    return CommandDone(lmSCORE_MODIFIED);
    else
        return false;
}




//----------------------------------------------------------------------------------------
// lmCmdInsertTimeSignature: Insert a time signature at current cursor position
//----------------------------------------------------------------------------------------

lmCmdInsertTimeSignature::lmCmdInsertTimeSignature(bool fUndoable,
                                                   const wxString& sName,
                                                   lmDocument *pDoc,  int nBeats,
                                                   int nBeatType, bool fVisible)
	: lmScoreCommand(sName, pDoc, fUndoable)
    , m_nBeats(nBeats)
    , m_nBeatType(nBeatType)
    , m_fVisible(fVisible)
{
}

bool lmCmdInsertTimeSignature::Do()
{
    //log command if undoable
    LogCommand();

    //insert the barline
    lmTimeSignature* pTS = GetVStaff()
        ->CmdNew_InsertTimeSignature(m_nBeats, m_nBeatType, m_fVisible);
    if (pTS)
	    return CommandDone(lmSCORE_MODIFIED);
    else
        return false;
}




//----------------------------------------------------------------------------------------
// lmCmdInsertKeySignature: Insert a key signature at current cursor position
//----------------------------------------------------------------------------------------

lmCmdInsertKeySignature::lmCmdInsertKeySignature(bool fUndoable,
                                                 const wxString& sName,
                             lmDocument *pDoc, int nFifths, bool fMajor,
                             bool fVisible)
	: lmScoreCommand(sName, pDoc, fUndoable)
    , m_nFifths(nFifths)
    , m_fMajor(fMajor)
    , m_fVisible(fVisible)
{
}

bool lmCmdInsertKeySignature::Do()
{
    //log command if undoable
    LogCommand();

    //insert the key
    lmKeySignature* pKey = GetVStaff()->CmdNew_InsertKeySignature(m_nFifths, m_fMajor, m_fVisible);
    if (pKey)
	    return CommandDone(lmSCORE_MODIFIED);
    else
        return false;
}



//----------------------------------------------------------------------------------------
// lmCmdInsertNote: Insert a note at current cursor position
//----------------------------------------------------------------------------------------

lmCmdInsertNote::lmCmdInsertNote(bool fUndoable,
                                 const wxString& sName,
                                 lmDocument *pDoc,
                                 lmEPitchType nPitchType,
								 int nStep, int nOctave,
								 lmENoteType nNoteType, float rDuration, int nDots,
								 lmENoteHeads nNotehead, lmEAccidentals nAcc,
                                 int nVoice, lmNote* pBaseOfChord, bool fTiedPrev)
	: lmScoreCommand(sName, pDoc, fUndoable)
	, m_nNoteType(nNoteType)
	, m_nPitchType(nPitchType)
	, m_nStep(nStep)
	, m_nOctave(nOctave)
    , m_nDots(nDots)
	, m_rDuration(rDuration)
	, m_nNotehead(nNotehead)
	, m_nAcc(nAcc)
	, m_nVoice(nVoice)
	, m_pBaseOfChord(pBaseOfChord)
    , m_fTiedPrev(fTiedPrev)
{
}

lmCmdInsertNote::~lmCmdInsertNote()
{
}

bool lmCmdInsertNote::Do()
{
    //log command if undoable
    LogCommand();

    //insert the note
    bool fAutoBar = lmPgmOptions::GetInstance()->GetBoolValue(lm_DO_AUTOBAR);
    lmNote* pNewNote = 
            GetVStaff()->CmdNew_InsertNote(m_nPitchType, m_nStep, m_nOctave, m_nNoteType,
                                           m_rDuration, m_nDots, m_nNotehead, m_nAcc, 
                                           m_nVoice, m_pBaseOfChord, m_fTiedPrev,
                                           fAutoBar);

    if (pNewNote)
	    return CommandDone(lmSCORE_MODIFIED);
    else
        return false;
}



//----------------------------------------------------------------------------------------
// lmCmdInsertRest: Insert a rest at current cursor position
//----------------------------------------------------------------------------------------

lmCmdInsertRest::lmCmdInsertRest(bool fUndoable,
                                 const wxString& sName,
                                 lmDocument *pDoc, lmENoteType nNoteType,
                                 float rDuration, int nDots, int nVoice)
	: lmScoreCommand(sName, pDoc, fUndoable)
	, m_nNoteType(nNoteType)
    , m_nDots(nDots)
	, m_rDuration(rDuration)
	, m_nVoice(nVoice)
{
}

lmCmdInsertRest::~lmCmdInsertRest()
{
}

bool lmCmdInsertRest::Do()
{
    //log command if undoable
    LogCommand();

    //insert the rest
    bool fAutoBar = lmPgmOptions::GetInstance()->GetBoolValue(lm_DO_AUTOBAR);
    lmRest* pRest =  GetVStaff()->CmdNew_InsertRest(m_nNoteType, m_rDuration,
                                                    m_nDots, m_nVoice, fAutoBar);

    if (pRest)
	    return CommandDone(lmSCORE_MODIFIED);
    else
        return false;
}



//----------------------------------------------------------------------------------------
// lmCmdChangeNotePitch: Change pitch of note at current cursor position
//----------------------------------------------------------------------------------------

lmCmdChangeNotePitch::lmCmdChangeNotePitch(bool fUndoable,
                                           const wxString& sName, lmDocument *pDoc,
                                           lmNote* pNote, int nSteps)
	: lmScoreCommand(sName, pDoc, fUndoable)
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

bool lmCmdChangeNotePitch::Undo()
{
    //Direct command. NO UNDO LOG

	m_pNote->ChangePitch(-m_nSteps);
    return CommandUndone();
}



//----------------------------------------------------------------------------------------
// lmCmdChangeNoteAccidentals: Change accidentals of notes in current selection
//----------------------------------------------------------------------------------------

lmCmdChangeNoteAccidentals::lmCmdChangeNoteAccidentals(
                                        bool fUndoable,
                                        const wxString& sName, lmDocument *pDoc,
                                        lmGMSelection* pSelection, int nAcc)
	: lmScoreCommand(sName, pDoc, fUndoable)
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

bool lmCmdChangeNoteAccidentals::Undo()
{
    //AWARE: Not using UndoLog. Direct execution of command

    std::list<lmCmdNoteData*>::iterator it;
    for (it = m_Notes.begin(); it != m_Notes.end(); ++it)
    {
        (*it)->pNote->ChangeAccidentals( (*it)->nAcc );
    }

    return CommandUndone();
}



//----------------------------------------------------------------------------------------
// lmCmdChangeNoteRestDots: Change dots of notes in current selection
//----------------------------------------------------------------------------------------

lmCmdChangeNoteRestDots::lmCmdChangeNoteRestDots(bool fUndoable,
                                                 const wxString& sName,
                                                 lmDocument *pDoc,
                                                 lmGMSelection* pSelection,
                                                 int nDots)
	: lmScoreCommand(sName, pDoc, fUndoable)
{
	m_nDots = nDots;

    //loop to save data about note/rests to modify
    lmGMObject* pGMO = pSelection->GetFirst();
    while (pGMO)
    {
        if (pGMO->GetType() == eGMO_ShapeNote || pGMO->GetType() == eGMO_ShapeRest)
        {
            lmNoteRest* pNR = (lmNoteRest*)pGMO->GetScoreOwner();
            m_NoteRests.push_back( pNR->GetIRef() );
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
    //log command if undoable
    LogCommand();

    //loop to change dots
    std::list<lmIRef>::iterator it;
    for (it = m_NoteRests.begin(); it != m_NoteRests.end(); ++it)
    {
        //Get pointer to object to delete
        lmNoteRest* pNR = (lmNoteRest*)GetStaffObj(*it);
        GetVStaff()->CmdNew_ChangeDots(pNR, m_nDots);
    }

    return CommandDone(lmSCORE_MODIFIED);
}



//----------------------------------------------------------------------------------------
// lmCmdDeleteTuplet implementation
//----------------------------------------------------------------------------------------

lmCmdDeleteTuplet::lmCmdDeleteTuplet(bool fUndoable,
                                     const wxString& sName, lmDocument *pDoc,
                                     lmNoteRest* pStartNR)
	: lmScoreCommand(sName, pDoc, fUndoable)
{
    m_nStartIRef = pStartNR->GetIRef();
}

bool lmCmdDeleteTuplet::Do()
{
    //log command if undoable
    LogCommand();

    //Get pointer to object to delete
    lmNoteRest* pNR = (lmNoteRest*)GetStaffObj(m_nStartIRef);

    //Proceed to delete the tuplet
    if (!GetVStaff()->CmdNew_DeleteTuplet(pNR))
	    return CommandDone(lmSCORE_MODIFIED);
    else
        return false;
}



//----------------------------------------------------------------------------------------
// lmCmdAddTuplet implementation: Add a tuplet to notes in current selection
//----------------------------------------------------------------------------------------

lmCmdAddTuplet::lmCmdAddTuplet(bool fUndoable,
                               const wxString& sName, lmDocument *pDoc, lmGMSelection* pSelection,
                               bool fShowNumber, int nNumber, bool fBracket,
                               lmEPlacement nAbove, int nActual, int nNormal)
	: lmScoreCommand(sName, pDoc, fUndoable)
{
    m_fShowNumber = fShowNumber;
    m_nNumber = nNumber;
    m_fBracket = fBracket;
    m_nAbove = nAbove;
    m_nActual = nActual;
    m_nNormal = nNormal;

    //loop to save the IRef for the notes/rests to include in the tuplet
    lmGMObject* pGMO = pSelection->GetFirst();
    while (pGMO)
    {
        if (pGMO->GetType() == eGMO_ShapeNote || pGMO->GetType() == eGMO_ShapeRest)
        {
            m_NotesRests.push_back( ((lmNoteRest*)pGMO->GetScoreOwner())->GetIRef() );
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
    //log command if undoable
    LogCommand();

    //Get pointers to notes/rest to include in tuplet
    std::vector<lmNoteRest*> notes;
    std::list<lmIRef>::iterator it;
    for (it = m_NotesRests.begin(); it != m_NotesRests.end(); ++it)
    {
        notes.push_back((lmNoteRest*)GetStaffObj(*it));
    }

    //Proceed to add the tuplet
    bool fError = GetVStaff()->CmdNew_AddTuplet(notes, m_fShowNumber, m_nNumber, m_fBracket,
                                                m_nAbove, m_nActual, m_nNormal);
    notes.clear();

    if (!fError)
	    return CommandDone(lmSCORE_MODIFIED);
    else
        return false;
}



//----------------------------------------------------------------------------------------
// lmCmdBreakBeam implementation
//----------------------------------------------------------------------------------------

lmCmdBreakBeam::lmCmdBreakBeam(bool fUndoable,
                               const wxString& sName, lmDocument *pDoc, lmNoteRest* pBeforeNR)
	: lmScoreCommand(sName, pDoc, fUndoable)
{
    m_nBeforeNR = pBeforeNR->GetIRef();
}

lmCmdBreakBeam::~lmCmdBreakBeam()
{
}

bool lmCmdBreakBeam::Do()
{
    //log command if undoable
    LogCommand();

    //Get pointer to note and issue the command
    GetVStaff()->CmdNew_BreakBeam( (lmNoteRest*)GetStaffObj(m_nBeforeNR) );

    return CommandDone(lmSCORE_MODIFIED);
}



//----------------------------------------------------------------------------------------
// lmCmdJoinBeam implementation
//----------------------------------------------------------------------------------------

lmCmdJoinBeam::lmCmdJoinBeam(bool fUndoable,
                             const wxString& sName, lmDocument *pDoc,
                             lmGMSelection* pSelection)
	: lmScoreCommand(sName, pDoc, fUndoable)
{
    //loop to save the IRef of the note/rests to beam
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
                m_NotesRests.push_back(pNR->GetIRef());
        }
        pGMO = pSelection->GetNext();
    }
}

bool lmCmdJoinBeam::Do()
{
    //log command if undoable
    LogCommand();

    //Get pointers to note/rests
    std::vector<lmNoteRest*> notes;
    std::vector<lmIRef>::iterator it;
    for (it = m_NotesRests.begin(); it != m_NotesRests.end(); ++it)
    {
        notes.push_back( (lmNoteRest*)GetStaffObj(*it) );
    }

    //issue the command
    GetVStaff()->CmdNew_JoinBeam(notes);

    return CommandDone(lmSCORE_MODIFIED);
}



//----------------------------------------------------------------------------------------
// lmCmdChangeText: Change ScoreText properties
//----------------------------------------------------------------------------------------

lmCmdChangeText::lmCmdChangeText(bool fUndoable,
                                 const wxString& sName, lmDocument *pDoc, lmScoreText* pST,
                                 wxString& sText, lmEHAlign nHAlign, lmLocation tPos,
                                 lmTextStyle* pStyle, int nHintOptions)
	: lmScoreCommand(sName, pDoc, fUndoable)
{
    //save text auxobj
    m_pST = pST;

    //save new values
    m_sText = sText;
    m_nHAlign = nHAlign;
    m_tPos = tPos;
    m_pStyle = pStyle;

    //get and save current values
    m_sOldText = pST->GetText();
    m_nOldHAlign = pST->GetAlignment();
    m_tOldPos = pST->GetLocation();
    m_pOldStyle = pST->GetStyle();
}

bool lmCmdChangeText::Do()
{
    //AWARE: Not using UndoLog. Direct execution of command

    //issue the command
    m_pST->Cmd_ChangeText(m_sText, m_nHAlign, m_tPos, m_pStyle);

    return CommandDone(lmSCORE_MODIFIED);
}

bool lmCmdChangeText::Undo()
{
    //AWARE: Not using UndoLog. Direct execution of command

    //issue the command
    m_pST->Cmd_ChangeText(m_sOldText, m_nOldHAlign, m_tOldPos, m_pOldStyle);

    return CommandUndone();
}



//----------------------------------------------------------------------------------------
// lmCmdChangePageMargin implementation
//----------------------------------------------------------------------------------------

lmCmdChangePageMargin::lmCmdChangePageMargin(bool fUndoable,
                                             const wxString& sName, lmDocument *pDoc,
                                             lmGMObject* pGMO, int nIdx, int nPage,
											 lmLUnits uPos)
	: lmScoreCommand(sName, pDoc, fUndoable)
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

bool lmCmdChangePageMargin::Undo()
{
    //Direct command. NO UNDO LOG

    ChangeMargin(m_uOldPos);
    return CommandUndone();
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

lmCmdAttachNewText::lmCmdAttachNewText(bool fUndoable,
                                       const wxString& sName, lmDocument *pDoc,
                                       lmComponentObj* pAnchor)
	: lmScoreCommand(sName, pDoc, fUndoable)
{
	m_pAnchor = pAnchor;
    m_fDeleteText = false;

    //Create the text
    lmTextStyle* pStyle = pAnchor->GetScore()->GetStyleInfo(_("Normal text"));
    wxASSERT(pStyle);

    //create the text object and attach it to to score
	//TODO: This is dirty. To use OnEditProperties() the text must be on the score. so I will
	//attach it provisionally to the score
    wxString sText = _T("");
    m_pNewText = new lmTextItem(pAnchor->GetScore(), 0L, sText, lmHALIGN_DEFAULT, pStyle);
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

bool lmCmdAttachNewText::Undo()
{
    //Direct command. NO UNDO LOG

    m_pAnchor->DetachAuxObj(m_pNewText);
    m_fDeleteText = true;
    return CommandUndone();
}



//----------------------------------------------------------------------------------------
// lmCmdAddNewTitle implementation
//----------------------------------------------------------------------------------------

lmCmdAddNewTitle::lmCmdAddNewTitle(bool fUndoable, lmDocument *pDoc)
	: lmScoreCommand(_("add title"), pDoc, fUndoable)
{
    m_fDeleteTitle = false;

    //Create the text
    lmTextStyle* pStyle = pDoc->GetScore()->GetStyleInfo(_("Title"));
    wxASSERT(pStyle);

    //create the text object
	//This is dirty: To use OnEditProperties() the text must be on the score. so I will
	//attach it provisionally to the score
    wxString sTitle = _T("");
    m_pNewTitle = new lmScoreTitle(pDoc->GetScore(), 0L, sTitle, lmBLOCK_ALIGN_BOTH,
                                   lmHALIGN_DEFAULT, lmVALIGN_DEFAULT, pStyle);
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

bool lmCmdAddNewTitle::Undo()
{
    //Direct command. NO UNDO LOG

    m_pDoc->GetScore()->DetachAuxObj(m_pNewTitle);
    m_fDeleteTitle = true;
    return CommandUndone();
}



//----------------------------------------------------------------------------------------
// lmCmdChangeBarline implementation
//----------------------------------------------------------------------------------------

lmCmdChangeBarline::lmCmdChangeBarline(bool fUndoable, lmDocument *pDoc, lmBarline* pBL,
									   lmEBarline nType, bool fVisible)
	: lmScoreCommand(_("change barline"), pDoc, fUndoable)
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

bool lmCmdChangeBarline::Undo()
{
    //Direct command. NO UNDO LOG

    m_pBL->SetBarlineType(m_nOldType);
    m_pBL->SetVisible(m_fOldVisible);
    return CommandUndone();
}



//----------------------------------------------------------------------------------------
// lmCmdChangeMidiSettings implementation
//----------------------------------------------------------------------------------------

lmCmdChangeMidiSettings::lmCmdChangeMidiSettings(bool fUndoable, lmDocument *pDoc,
                                                 lmInstrument* pInstr,
                                                 int nMidiChannel,
                                                 int nMidiInstr)
	: lmScoreCommand(_("change MIDI settings"), pDoc, fUndoable)
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

bool lmCmdChangeMidiSettings::Undo()
{
    //Direct command. NO UNDO LOG

    m_pInstr->SetMIDIChannel(m_nOldMidiChannel);
    m_pInstr->SetMIDIInstrument(m_nOldMidiInstr);
    return CommandUndone();
}



//----------------------------------------------------------------------------------------
// lmCmdMoveNote implementation
//----------------------------------------------------------------------------------------

lmCmdMoveNote::lmCmdMoveNote(bool fUndoable, lmDocument *pDoc, lmNote* pNote,
                             const lmUPoint& uPos, int nSteps)
	: lmScoreCommand(_("move note"), pDoc, fUndoable)
{
	m_uxPos = uPos.x;	//(g_fFreeMove ? uPos.y : pNote->GetUserShift().y);
	m_pNote = pNote;
    m_nSteps = nSteps;
}

bool lmCmdMoveNote::Do()
{
	m_pNote->ChangePitch(m_nSteps);
    m_uxOldPos = m_pNote->SetUserXLocation(m_uxPos);

	return CommandDone(lmSCORE_MODIFIED);
}

bool lmCmdMoveNote::Undo()
{
    //Direct command. NO UNDO LOG

	m_pNote->SetUserXLocation(m_uxOldPos);
	m_pNote->ChangePitch(-m_nSteps);
    return CommandUndone();
}



//----------------------------------------------------------------------------------------
// lmCmdMoveObjectPoints implementation
//----------------------------------------------------------------------------------------

lmCmdMoveObjectPoints::lmCmdMoveObjectPoints(bool fUndoable, const wxString& name,
                               lmDocument *pDoc, lmGMObject* pGMO,
                               lmUPoint uShift[], int nNumPoints, bool fUpdateViews)
	: lmScoreCommand(name, pDoc, fUndoable, 0, fUpdateViews)
      , m_nNumPoints(nNumPoints)
{
    wxASSERT(nNumPoints > 0);

    //get additional info
	m_pSCO = pGMO->GetScoreOwner();
    m_nShapeIdx = pGMO->GetOwnerIDX();

    //allocate a vector to save shifts
    m_pShifts = new lmUPoint[m_nNumPoints];
    for(int i=0; i < m_nNumPoints; i++)
        *(m_pShifts+i) = uShift[i];
}

lmCmdMoveObjectPoints::~lmCmdMoveObjectPoints()
{
    delete[] m_pShifts;
}

bool lmCmdMoveObjectPoints::Do()
{
    //Direct command. NO UNDO LOG
    m_pSCO->MoveObjectPoints(m_nNumPoints, m_nShapeIdx, m_pShifts, true);  //true->add shifts
	return CommandDone(lmSCORE_MODIFIED, lmDO_ONLY_REDRAW);
}

bool lmCmdMoveObjectPoints::Undo()
{
    //Direct command. NO UNDO LOG
    m_pSCO->MoveObjectPoints(m_nNumPoints, m_nShapeIdx, m_pShifts, false);  //false->substract shifts
    return CommandUndone(lmDO_ONLY_REDRAW);
}


