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
//
// Parameters
//  fNormalCmd
//      true  - Normal command. The command will be logged in the undo/redo chain and,
//              if applicable, the score will be saved for undo.
//      false - Hidden command. The command is executed but the command will not be
//              added to the command history and, if applicable, the score will not
//              be saved for undo. After command execution the screen is not updated.
//              This option is usefull for building commands by chaining other
//              commnads. The main command will be undoable and all atomic commands
//              will not. See, for example, lmCmdDeleteSelection.
//
// fDoLayout
//      true  - the score is relayouted and the screen updated
//      false - the score is not relayouted. Screen is updated just by rendering the
//              existing boxes/shapes. This option is useful for commands that do
//              not affect score layout. For instance, changing the color of an object.
//----------------------------------------------------------------------------------------

lmScoreCommand::lmScoreCommand(const wxString& sName, lmDocument *pDoc,
                               bool fNormalCmd, bool fDoLayout)
    : wxCommand(true, sName)
    , m_pDoc(pDoc)
    , m_fDocModified(false)
    , m_fUndoable(fNormalCmd)
    , m_fDoLayout(fDoLayout)
    , m_CursorState(pDoc->GetScore()->GetCursor()->GetState())
{
}

lmScoreCommand::~lmScoreCommand()
{
}

void lmScoreCommand::LogCommand()
{
    //reposition cursor and log command if undoable

    //reposition cursor
    RestoreCursor();

    if (!m_fUndoable)
        return;

    //get score and save source code
    m_sOldSource = m_pDoc->GetScore()->SourceLDP(true);     //true: export cursor
}

void lmScoreCommand::RestoreCursor()
{
    //reposition cursor at saved state

    m_pDoc->GetScore()->GetCursor()->SetState(&m_CursorState);
}

lmVStaff* lmScoreCommand::GetVStaff()
{
    return m_pDoc->GetScore()->GetCursor()->GetVStaff();
}

lmScoreObj* lmScoreCommand::GetScoreObj(long nID)
{
    return m_pDoc->GetScore()->GetScoreObj(nID);
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

bool lmScoreCommand::CommandDone(bool fCmdSuccess, int nUpdateHints)
{
    //common code after executing a command:
    //- save document current modification status flag, to restore it if command undo
    //- set document as 'modified'
    //- update the views with the changes
    //
    // Returns false to indicate that the action must not be added to the
    // command history.

    //if failure or hidden command, return 'false', meaning "Do not add this
    //command to command history, and do not update views"
    if (!fCmdSuccess || !m_fUndoable)
        return false;
    
    //success. mark document as 'modified'
	m_fDocModified = m_pDoc->IsModified();
	m_pDoc->Modify(true);

    //update views
    if (!m_fDoLayout)
        nUpdateHints |= lmHINT_NO_LAYOUT;
    m_pDoc->UpdateAllViews((wxView*)NULL, new lmUpdateHint(nUpdateHints));

    return true;    //success. Add command to command history
}

bool lmScoreCommand::CommandUndone(int nUpdateHints)
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
    m_pDoc->UpdateAllViews((wxView*)NULL, new lmUpdateHint(nUpdateHints));

	m_pDoc->Modify(m_fDocModified);     //restore modified state
    return true;
}



//----------------------------------------------------------------------------------------
// lmCmdDeleteSelection implementation
//----------------------------------------------------------------------------------------

lmCmdDeleteSelection::lmCmdDeleteSelection(bool fNormalCmd,
                                           const wxString& sName,
                                           lmDocument *pDoc,
                                           lmGMSelection* pSelection)
	: lmScoreCommand(sName, pDoc, fNormalCmd)
{
    //loop to create and save the commands to delete the selected objects
    //AWARE: commands are marked as lmCMD_HIDDEN so that they do not save the score
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
                    //    = new lmCmdDeleteTie(lmCMD_HIDDEN, tCursorState, sCmdName, pDoc,
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
                        = new lmCmdDeleteStaffObj(lmCMD_HIDDEN, sCmdName, pDoc,
                                             (lmStaffObj*)pGMO->GetScoreOwner() );
                    m_Commands.push_back( pCmd );
               }
                break;

            case eGMO_ShapeTuplet:
                {
                    //lmToDeleteSO* pSOData = new lmToDeleteSO;
                    //pSOData->nObjType = lm_eObjTuplet;
                    //pSOData->oIRef = (void*)( ((lmShapeTuplet*)pGMO)->GetScoreOwner() );   //start note

                    //m_Commands.push_back( pSOData );
                    //sCmdName = _T("Delete tuplet");
                    //lmScoreCommand* pCmd 
                    //    = new lmCmdDeleteStaffObj(lmCMD_HIDDEN, tCursorState, sCmdName, pDoc,
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
                    //    = new lmCmdDeleteStaffObj(lmCMD_HIDDEN, tCursorState, sCmdName, pDoc,
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
                    //    = new lmCmdDeleteStaffObj(lmCMD_HIDDEN, tCursorState, sCmdName, pDoc,
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
                    //    = new lmCmdDeleteStaffObj(lmCMD_HIDDEN, tCursorState, sCmdName, pDoc,
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
    //reposition cursor and log command if undoable
    LogCommand();

    //loop to issue delete commnads
    std::list<lmScoreCommand*>::iterator it;
    for (it = m_Commands.begin(); it != m_Commands.end(); ++it)
    {
        (*it)->Do();
    }

    return CommandDone(true);
}




//----------------------------------------------------------------------------------------
// lmCmdDeleteStaffObj implementation
//----------------------------------------------------------------------------------------

lmCmdDeleteStaffObj::lmCmdDeleteStaffObj(bool fNormalCmd, 
                                         const wxString& sName,
                                         lmDocument *pDoc, lmStaffObj* pSO)
	: lmScoreCommand(sName, pDoc, fNormalCmd)
    , m_nObjID(pSO->GetID())
{
}

bool lmCmdDeleteStaffObj::Do()
{
    //reposition cursor and log command if undoable
    LogCommand();

    //Get pointer to object to delete
    lmStaffObj* pSO = (lmStaffObj*)GetScoreObj(m_nObjID);

    //Proceed to delete the object
    bool fError = false;
    if (pSO->IsClef())
        fError = GetVStaff()->Cmd_DeleteClef((lmClef*)pSO);
    else if (pSO->IsTimeSignature())
        fError = GetVStaff()->Cmd_DeleteTimeSignature((lmTimeSignature*)pSO);
    else if (pSO->IsKeySignature())
        fError = GetVStaff()->Cmd_DeleteKeySignature((lmKeySignature*)pSO);
    else
        fError = GetVStaff()->Cmd_DeleteStaffObj(pSO);

	return CommandDone(!fError);
}



//----------------------------------------------------------------------------------------
// lmCmdDeleteTie implementation
//----------------------------------------------------------------------------------------

lmCmdDeleteTie::lmCmdDeleteTie(bool fNormalCmd,  
                               const wxString& sName, lmDocument *pDoc,
                               lmNote* pEndNote)
	: lmScoreCommand(sName, pDoc, fNormalCmd)
{
    m_nEndNoteID = pEndNote->GetID();
    m_nStartNoteID = pEndNote->GetTiedNotePrev()->GetID();
    lmTie* pTie = pEndNote->GetTiePrev();
    m_nTieID = pTie->GetID();
    m_Bezier[0] = pTie->GetBezier(0);
    m_Bezier[1] = pTie->GetBezier(1);
}

bool lmCmdDeleteTie::Do()
{
    //Direct command. NO UNDO LOG

    //reposition cursor
    RestoreCursor();

    //Get pointer to end note
    lmNote* pEndNote = (lmNote*)GetScoreObj(m_nEndNoteID);
    //remove the tie
    pEndNote->DeleteTiePrev();

    return CommandDone(true);
}

bool lmCmdDeleteTie::Undo()
{
    //Get pointers to start and end notes
    lmNote* pEndNote = (lmNote*)GetScoreObj(m_nEndNoteID);
    lmNote* pStartNote = (lmNote*)GetScoreObj(m_nStartNoteID);
    //re-create the tie
    pEndNote->CreateTie(pStartNote, pEndNote, m_nTieID);
    lmTie* pTie = pStartNote->GetTieNext();
    pTie->SetBezier(0, &m_Bezier[0]);
    pTie->SetBezier(1, &m_Bezier[1]);

    return CommandUndone();
}




//----------------------------------------------------------------------------------------
// lmCmdAddTie implementation
//----------------------------------------------------------------------------------------

lmCmdAddTie::lmCmdAddTie(bool fNormalCmd,  const wxString& sName,
                         lmDocument *pDoc, lmNote* pStartNote, lmNote* pEndNote)
	: lmScoreCommand(sName, pDoc, fNormalCmd)
{
    m_nStartNoteID = pStartNote->GetID();
    m_nEndNoteID = pEndNote->GetID();
}

bool lmCmdAddTie::Do()
{
    //Direct command. NO UNDO LOG

    //reposition cursor
    RestoreCursor();

    //Get pointers to start and end notes
    lmNote* pEndNote = (lmNote*)GetScoreObj(m_nEndNoteID);
    lmNote* pStartNote = (lmNote*)GetScoreObj(m_nStartNoteID);

    //add the tie
    pEndNote->CreateTie(pStartNote, pEndNote);

    return CommandDone(true);
}

bool lmCmdAddTie::Undo()
{
    //Get pointers to start and end notes
    lmNote* pEndNote = (lmNote*)GetScoreObj(m_nEndNoteID);
    lmNote* pStartNote = (lmNote*)GetScoreObj(m_nStartNoteID);

    //remove the tie
    pEndNote->DeleteTiePrev();

    return CommandUndone();
}



//----------------------------------------------------------------------------------------
// lmCmdMoveObject implementation
//----------------------------------------------------------------------------------------

lmCmdMoveObject::lmCmdMoveObject(bool fNormalCmd, 
                                 const wxString& sName, lmDocument *pDoc,
								 lmGMObject* pGMO, const lmUPoint& uPos)
	: lmScoreCommand(sName, pDoc, fNormalCmd)
{
	m_tPos.x = uPos.x;
	m_tPos.y = uPos.y;
	m_tPos.xUnits = lmLUNITS;
	m_tPos.yUnits = lmLUNITS;

	lmScoreObj* pSO = pGMO->GetScoreOwner();
    m_nObjectID = pSO->GetID();
    m_nShapeIdx = pGMO->GetOwnerIDX();
    wxASSERT_MSG( pSO, _T("[lmCmdMoveObject::Do] No ScoreObj to move!"));
}

bool lmCmdMoveObject::Do()
{
    //Direct command. NO UNDO LOG

    //reposition cursor
    RestoreCursor();

    lmScoreObj* pSO = GetScoreObj(m_nObjectID);
    m_tOldPos = pSO->SetUserLocation(m_tPos, m_nShapeIdx);
    return CommandDone(true);
}

bool lmCmdMoveObject::Undo()
{
    //Direct command. NO UNDO LOG

    lmScoreObj* pSO = GetScoreObj(m_nObjectID);
    pSO->SetUserLocation(m_tOldPos, m_nShapeIdx);
    return CommandUndone(0);
}



//----------------------------------------------------------------------------------------
// lmCmdInsertBarline: Insert a barline at current cursor position
//----------------------------------------------------------------------------------------

lmCmdInsertBarline::lmCmdInsertBarline(bool fNormalCmd,
                                       const wxString& sName, lmDocument *pDoc,
                                       lmEBarline nType)
	: lmScoreCommand(sName, pDoc, fNormalCmd)
    , m_nBarlineType(nType)
{
}

bool lmCmdInsertBarline::Do()
{
    //reposition cursor and log command if undoable
    LogCommand();

    //insert the barline
    lmBarline* pBL = GetVStaff()->Cmd_InsertBarline(m_nBarlineType);

    return CommandDone(pBL != (lmBarline*)NULL);
}




//----------------------------------------------------------------------------------------
// lmCmdInsertClef: Insert a clef at current cursor position
//----------------------------------------------------------------------------------------

lmCmdInsertClef::lmCmdInsertClef(bool fNormalCmd,
                                 const wxString& sName,
                                 lmDocument *pDoc, lmEClefType nClefType)
	: lmScoreCommand(sName, pDoc, fNormalCmd)
    , m_nClefType(nClefType)
{
}

bool lmCmdInsertClef::Do()
{
    //reposition cursor and log command if undoable
    LogCommand();

    //insert the barline
    lmClef* pClef = GetVStaff()->Cmd_InsertClef(m_nClefType);
    return CommandDone(pClef != (lmClef*)NULL);
}




//----------------------------------------------------------------------------------------
// lmCmdInsertTimeSignature: Insert a time signature at current cursor position
//----------------------------------------------------------------------------------------

lmCmdInsertTimeSignature::lmCmdInsertTimeSignature(bool fNormalCmd,
                                                   const wxString& sName,
                                                   lmDocument *pDoc,  int nBeats,
                                                   int nBeatType, bool fVisible)
	: lmScoreCommand(sName, pDoc, fNormalCmd)
    , m_nBeats(nBeats)
    , m_nBeatType(nBeatType)
    , m_fVisible(fVisible)
{
}

bool lmCmdInsertTimeSignature::Do()
{
    //reposition cursor and log command if undoable
    LogCommand();

    //insert the barline
    lmTimeSignature* pTS = GetVStaff()
        ->Cmd_InsertTimeSignature(m_nBeats, m_nBeatType, m_fVisible);
    return CommandDone(pTS != (lmTimeSignature*)NULL);
}




//----------------------------------------------------------------------------------------
// lmCmdInsertKeySignature: Insert a key signature at current cursor position
//----------------------------------------------------------------------------------------

lmCmdInsertKeySignature::lmCmdInsertKeySignature(bool fNormalCmd,
                                                 const wxString& sName,
                             lmDocument *pDoc, int nFifths, bool fMajor,
                             bool fVisible)
	: lmScoreCommand(sName, pDoc, fNormalCmd)
    , m_nFifths(nFifths)
    , m_fMajor(fMajor)
    , m_fVisible(fVisible)
{
}

bool lmCmdInsertKeySignature::Do()
{
    //reposition cursor and log command if undoable
    LogCommand();

    //insert the key
    lmKeySignature* pKey = GetVStaff()->Cmd_InsertKeySignature(m_nFifths, m_fMajor, m_fVisible);
    return CommandDone(pKey != (lmKeySignature*)NULL);
}



//----------------------------------------------------------------------------------------
// lmCmdInsertNote: Insert a note at current cursor position
//----------------------------------------------------------------------------------------

lmCmdInsertNote::lmCmdInsertNote(bool fNormalCmd,
                                 const wxString& sName,
                                 lmDocument *pDoc,
                                 lmEPitchType nPitchType,
								 int nStep, int nOctave,
								 lmENoteType nNoteType, float rDuration, int nDots,
								 lmENoteHeads nNotehead, lmEAccidentals nAcc,
                                 int nVoice, lmNote* pBaseOfChord, bool fTiedPrev,
                                 lmEStemType nStem)
	: lmScoreCommand(sName, pDoc, fNormalCmd)
	, m_nNoteType(nNoteType)
	, m_nPitchType(nPitchType)
    , m_nStem(nStem)
	, m_nStep(nStep)
	, m_nOctave(nOctave)
    , m_nDots(nDots)
	, m_rDuration(rDuration)
	, m_nNotehead(nNotehead)
	, m_nAcc(nAcc)
	, m_nVoice(nVoice)
	, m_nBaseOfChordID(lmNULL_ID)
    , m_fTiedPrev(fTiedPrev)
{
    if (pBaseOfChord)
        m_nBaseOfChordID = pBaseOfChord->GetID();
}

lmCmdInsertNote::~lmCmdInsertNote()
{
}

bool lmCmdInsertNote::Do()
{
    //reposition cursor and log command if undoable
    LogCommand();

    //insert the note
    bool fAutoBar = lmPgmOptions::GetInstance()->GetBoolValue(lm_DO_AUTOBAR);
    lmNote* pBaseOfChord = (lmNote*)GetScoreObj(m_nBaseOfChordID);
    lmNote* pNewNote = 
            GetVStaff()->Cmd_InsertNote(m_nPitchType, m_nStep, m_nOctave, m_nNoteType,
                                           m_rDuration, m_nDots, m_nNotehead, m_nAcc, 
                                           m_nVoice, pBaseOfChord, m_fTiedPrev,
                                           m_nStem, fAutoBar);

    return CommandDone(pNewNote != (lmNote*)NULL);
}



//----------------------------------------------------------------------------------------
// lmCmdInsertRest: Insert a rest at current cursor position
//----------------------------------------------------------------------------------------

lmCmdInsertRest::lmCmdInsertRest(bool fNormalCmd,
                                 const wxString& sName,
                                 lmDocument *pDoc, lmENoteType nNoteType,
                                 float rDuration, int nDots, int nVoice)
	: lmScoreCommand(sName, pDoc, fNormalCmd)
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
    //reposition cursor and log command if undoable
    LogCommand();

    //insert the rest
    bool fAutoBar = lmPgmOptions::GetInstance()->GetBoolValue(lm_DO_AUTOBAR);
    lmRest* pRest =  GetVStaff()->Cmd_InsertRest(m_nNoteType, m_rDuration,
                                                    m_nDots, m_nVoice, fAutoBar);

    return CommandDone(pRest != (lmRest*)NULL);
}



//----------------------------------------------------------------------------------------
// lmCmdChangeNotePitch: Change pitch of note at current cursor position
//----------------------------------------------------------------------------------------

lmCmdChangeNotePitch::lmCmdChangeNotePitch(bool fNormalCmd,
                                           const wxString& sName, lmDocument *pDoc,
                                           lmNote* pNote, int nSteps)
	: lmScoreCommand(sName, pDoc, fNormalCmd)
	, m_nSteps(nSteps)
	, m_nNoteID(pNote->GetID())
{
}

bool lmCmdChangeNotePitch::Do()
{
    //Direct command. NO UNDO LOG

    //reposition cursor
    RestoreCursor();

	((lmNote*)GetScoreObj(m_nNoteID))->ChangePitch(m_nSteps);

	return CommandDone(true);
}

bool lmCmdChangeNotePitch::Undo()
{
    //Direct command. NO UNDO LOG

	((lmNote*)GetScoreObj(m_nNoteID))->ChangePitch(-m_nSteps);
    return CommandUndone();
}



//----------------------------------------------------------------------------------------
// lmCmdChangeNoteAccidentals: Change accidentals of notes in current selection
//----------------------------------------------------------------------------------------

lmCmdChangeNoteAccidentals::lmCmdChangeNoteAccidentals(
                                        bool fNormalCmd,
                                        const wxString& sName, lmDocument *pDoc,
                                        lmGMSelection* pSelection, int nAcc)
	: lmScoreCommand(sName, pDoc, fNormalCmd)
{
	m_nAcc = nAcc;

    //loop to save notes to modify
    lmGMObject* pGMO = pSelection->GetFirst();
    while (pGMO)
    {
        if (pGMO->GetType() == eGMO_ShapeNote)
        {
            lmCmdNoteData* pData = new lmCmdNoteData;
            lmNote* pNote = (lmNote*)pGMO->GetScoreOwner();
            pData->nNoteID = pNote->GetID();
            pData->nAcc = pNote->GetAPitch().Accidentals();

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
    //Direct command. NO UNDO LOG

    //reposition cursor
    RestoreCursor();

    std::list<lmCmdNoteData*>::iterator it;
    for (it = m_Notes.begin(); it != m_Notes.end(); ++it)
    {
        ((lmNote*)GetScoreObj((*it)->nNoteID))->ChangeAccidentals(m_nAcc);
    }

	return CommandDone(true);
}

bool lmCmdChangeNoteAccidentals::Undo()
{
    //Direct command. NO UNDO LOG

    std::list<lmCmdNoteData*>::iterator it;
    for (it = m_Notes.begin(); it != m_Notes.end(); ++it)
    {
        ((lmNote*)GetScoreObj((*it)->nNoteID))->ChangeAccidentals( (*it)->nAcc );
    }

    return CommandUndone();
}



//----------------------------------------------------------------------------------------
// lmCmdChangeNoteRestDots: Change dots of notes in current selection
//----------------------------------------------------------------------------------------

lmCmdChangeNoteRestDots::lmCmdChangeNoteRestDots(bool fNormalCmd,
                                                 const wxString& sName,
                                                 lmDocument *pDoc,
                                                 lmGMSelection* pSelection,
                                                 int nDots)
	: lmScoreCommand(sName, pDoc, fNormalCmd)
	, m_nDots(nDots)
{
    //loop to save data about note/rests to modify
    lmGMObject* pGMO = pSelection->GetFirst();
    while (pGMO)
    {
        if (pGMO->GetType() == eGMO_ShapeNote || pGMO->GetType() == eGMO_ShapeRest)
        {
            lmNoteRest* pNR = (lmNoteRest*)pGMO->GetScoreOwner();
            m_NoteRests.push_back( pNR->GetID() );
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
    //reposition cursor and log command if undoable
    LogCommand();

    //loop to change dots
    std::list<long>::iterator it;
    for (it = m_NoteRests.begin(); it != m_NoteRests.end(); ++it)
    {
        //Get pointer to object to delete
        lmNoteRest* pNR = (lmNoteRest*)GetScoreObj(*it);
        GetVStaff()->Cmd_ChangeDots(pNR, m_nDots);
    }

    return CommandDone(true);
}



//----------------------------------------------------------------------------------------
// lmCmdDeleteTuplet implementation
//----------------------------------------------------------------------------------------

lmCmdDeleteTuplet::lmCmdDeleteTuplet(bool fNormalCmd,
                                     const wxString& sName, lmDocument *pDoc,
                                     lmNoteRest* pStartNR)
	: lmScoreCommand(sName, pDoc, fNormalCmd)
    , m_nStartID(pStartNR->GetID())
{
}

bool lmCmdDeleteTuplet::Do()
{
    //reposition cursor and log command if undoable
    LogCommand();

    //Get pointer to object to delete
    lmNoteRest* pNR = (lmNoteRest*)GetScoreObj(m_nStartID);

    //Proceed to delete the tuplet
    bool fError = GetVStaff()->Cmd_DeleteTuplet(pNR);
    return CommandDone(!fError);
}



//----------------------------------------------------------------------------------------
// lmCmdAddTuplet implementation: Add a tuplet to notes in current selection
//----------------------------------------------------------------------------------------

lmCmdAddTuplet::lmCmdAddTuplet(bool fNormalCmd,
                               const wxString& sName, lmDocument *pDoc, lmGMSelection* pSelection,
                               bool fShowNumber, int nNumber, bool fBracket,
                               lmEPlacement nAbove, int nActual, int nNormal)
	: lmScoreCommand(sName, pDoc, fNormalCmd)
    , m_fShowNumber(fShowNumber)
    , m_nNumber(nNumber)
    , m_fBracket(fBracket)
    , m_nAbove(nAbove)
    , m_nActual(nActual)
    , m_nNormal(nNormal)
{
    //loop to save the ID for the notes/rests to include in the tuplet
    lmGMObject* pGMO = pSelection->GetFirst();
    while (pGMO)
    {
        if (pGMO->GetType() == eGMO_ShapeNote || pGMO->GetType() == eGMO_ShapeRest)
        {
            m_NotesRests.push_back( ((lmNoteRest*)pGMO->GetScoreOwner())->GetID() );
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
    //reposition cursor and log command if undoable
    LogCommand();

    //Get pointers to notes/rest to include in tuplet
    std::vector<lmNoteRest*> notes;
    std::list<long>::iterator it;
    for (it = m_NotesRests.begin(); it != m_NotesRests.end(); ++it)
    {
        notes.push_back((lmNoteRest*)GetScoreObj(*it));
    }

    //Proceed to add the tuplet
    bool fError = GetVStaff()->Cmd_AddTuplet(notes, m_fShowNumber, m_nNumber, m_fBracket,
                                                m_nAbove, m_nActual, m_nNormal);
    notes.clear();

    return CommandDone(!fError);
}



//----------------------------------------------------------------------------------------
// lmCmdBreakBeam implementation
//----------------------------------------------------------------------------------------

lmCmdBreakBeam::lmCmdBreakBeam(bool fNormalCmd,
                               const wxString& sName, lmDocument *pDoc, lmNoteRest* pBeforeNR)
	: lmScoreCommand(sName, pDoc, fNormalCmd)
    , m_nBeforeNR(pBeforeNR->GetID())
{
}

lmCmdBreakBeam::~lmCmdBreakBeam()
{
}

bool lmCmdBreakBeam::Do()
{
    //reposition cursor and log command if undoable
    LogCommand();

    //Get pointer to note and issue the command
    GetVStaff()->Cmd_BreakBeam( (lmNoteRest*)GetScoreObj(m_nBeforeNR) );

    return CommandDone(true);
}



//----------------------------------------------------------------------------------------
// lmCmdJoinBeam implementation
//----------------------------------------------------------------------------------------

lmCmdJoinBeam::lmCmdJoinBeam(bool fNormalCmd,
                             const wxString& sName, lmDocument *pDoc,
                             lmGMSelection* pSelection)
	: lmScoreCommand(sName, pDoc, fNormalCmd)
{
    //loop to save the IDs of the notes/rests to beam
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
                m_NotesRests.push_back(pNR->GetID());
        }
        pGMO = pSelection->GetNext();
    }
}

bool lmCmdJoinBeam::Do()
{
    //reposition cursor and log command if undoable
    LogCommand();

    //Get pointers to note/rests
    std::vector<lmNoteRest*> notes;
    std::vector<long>::iterator it;
    for (it = m_NotesRests.begin(); it != m_NotesRests.end(); ++it)
    {
        notes.push_back( (lmNoteRest*)GetScoreObj(*it) );
    }

    //issue the command
    GetVStaff()->Cmd_JoinBeam(notes);

    return CommandDone(true);
}



//----------------------------------------------------------------------------------------
// lmCmdChangeText: Change ScoreText properties
//----------------------------------------------------------------------------------------

lmCmdChangeText::lmCmdChangeText(bool fNormalCmd,
                                 const wxString& sName, lmDocument *pDoc, lmScoreText* pST,
                                 wxString& sText, lmEHAlign nHAlign, lmLocation tPos,
                                 lmTextStyle* pStyle, int nHintOptions)
	: lmScoreCommand(sName, pDoc, fNormalCmd)
    , m_nTextID(pST->GetID())
    //save new values
    , m_sText(sText)
    , m_nHAlign(nHAlign)
    , m_tPos(tPos)
    , m_Style(*pStyle)
    //get and save current values
    , m_sOldText(pST->GetText())
    , m_nOldHAlign(pST->GetAlignment())
    , m_tOldPos(pST->GetLocation())
    , m_OldStyle(*(pST->GetStyle()))
{
}

bool lmCmdChangeText::Do()
{
    //Direct command. NO UNDO LOG

    //reposition cursor
    RestoreCursor();

    lmScoreText* pST = (lmScoreText*)GetScoreObj(m_nTextID);
    lmTextStyle* pStyle = m_pDoc->GetScore()
            ->AddStyle(m_Style.sName, m_Style.tFont, m_Style.nColor);
    pST->Cmd_ChangeText(m_sText, m_nHAlign, m_tPos, pStyle);

    return CommandDone(true);
}

bool lmCmdChangeText::Undo()
{
    //Direct command. NO UNDO LOG

    lmScoreText* pST = (lmScoreText*)GetScoreObj(m_nTextID);
    lmTextStyle* pOldStyle = m_pDoc->GetScore()
            ->AddStyle(m_OldStyle.sName, m_OldStyle.tFont, m_OldStyle.nColor);
    pST->Cmd_ChangeText(m_sOldText, m_nOldHAlign, m_tOldPos, pOldStyle);

    return CommandUndone();
}



//----------------------------------------------------------------------------------------
// lmCmdChangePageMargin implementation
//----------------------------------------------------------------------------------------

lmCmdChangePageMargin::lmCmdChangePageMargin(bool fNormalCmd,
                                             const wxString& sName, lmDocument *pDoc,
                                             lmGMObject* pGMO, int nIdx, int nPage,
											 lmLUnits uPos)
	: lmScoreCommand(sName, pDoc, fNormalCmd)
	, m_nIdx(nIdx)
	, m_uNewPos(uPos)
	, m_nPage(nPage)
{
    //save current position
    lmScore* pScore = m_pDoc->GetScore();
    switch(m_nIdx)
    {
        case lmMARGIN_TOP:
            m_uOldPos = pScore->GetPageTopMargin(nPage);
            break;

        case lmMARGIN_BOTTOM:
            m_uOldPos = pScore->GetMaximumY(nPage);
            break;

        case lmMARGIN_LEFT:
            m_uOldPos = pScore->GetLeftMarginXPos(nPage);
            break;

        case lmMARGIN_RIGHT:
            m_uOldPos = pScore->GetRightMarginXPos(nPage);
            break;

        default:
            wxASSERT(false);
    }

}

bool lmCmdChangePageMargin::Do()
{
    //Direct command. NO UNDO LOG

    //reposition cursor
    RestoreCursor();

    ChangeMargin(m_uNewPos);
	return CommandDone(true);
}

bool lmCmdChangePageMargin::Undo()
{
    //Direct command. NO UNDO LOG

    ChangeMargin(m_uOldPos);
    return CommandUndone();
}

void lmCmdChangePageMargin::ChangeMargin(lmLUnits uPos)
{
    lmScore* pScore = m_pDoc->GetScore();
    lmUSize size = pScore->GetPaperSize(m_nPage);

    switch(m_nIdx)
    {
        case lmMARGIN_TOP:
            pScore->SetPageTopMargin(uPos, m_nPage);
            break;

        case lmMARGIN_BOTTOM:
            pScore->SetPageBottomMargin(size.Height() - uPos, m_nPage);
            break;

        case lmMARGIN_LEFT:
            pScore->SetPageLeftMargin(uPos, m_nPage);
            break;

        case lmMARGIN_RIGHT:
            pScore->SetPageRightMargin(size.Width() - uPos, m_nPage);
            break;

        default:
            wxASSERT(false);
    }
}



//----------------------------------------------------------------------------------------
// lmCmdAttachText implementation
//----------------------------------------------------------------------------------------

lmCmdAttachText::lmCmdAttachText(bool fNormalCmd, lmDocument *pDoc, wxString& sText,
                                 lmTextStyle* pStyle, lmEHAlign nAlign,
                                 lmComponentObj* pAnchor)
	: lmScoreCommand(_("attach text"), pDoc, fNormalCmd)
	, m_nAnchorID(pAnchor->GetID())
    , m_Style(*pStyle)
    , m_nAlign(nAlign)
    , m_sText(sText)
    , m_nTextID(lmNEW_ID)
{
}

bool lmCmdAttachText::Do()
{
    //reposition cursor and log command if undoable
    LogCommand();

    lmTextStyle* pStyle = m_pDoc->GetScore()
            ->AddStyle(m_Style.sName, m_Style.tFont, m_Style.nColor);
    lmScoreObj* pAnchor = GetScoreObj(m_nAnchorID);
    lmTextItem* pNewText = new lmTextItem(pAnchor, m_nTextID, m_sText, m_nAlign, pStyle);
    pAnchor->AttachAuxObj(pNewText);
    m_nTextID = pNewText->GetID();
    
    //AWARE.
    //It is necessary to ensure that if the command is redone, the text always
    //get assigned the same ID, as the text could be referenced in a subsequent 
    //redo commnad.
    //In normal commands, the same ID is assigned automatically. But in this 
    //command, the text has been previously created in lmScoreCanvas::AttachNewText()
    //and then deleted, and this ID number n is discarded. Therefore, the first time
    //the command is executed the text will get ID = n+1. But in a redo operation, 
    //the text will not be created and deleted in lmScoreCanvas::AttachNewText(), but
    //only here. Therefore, the ID that will be assigned in the redo operation will
    //be the initially deleted ID (n), instead of the correct one (n+1).
    //And this will invalidate any redo reference.
    //To avoid this problem, m_nTextID is initialized with lmNEW_ID so the first
    //time the command is executed, a new ID (n+1) is assigned and saved. And this
    //saved value is used in any subsequent redo operation, ensuring that
    //the text receives the right ID (n+1).

	return CommandDone(true);
}



//----------------------------------------------------------------------------------------
// lmCmdAddTitle implementation
//----------------------------------------------------------------------------------------

lmCmdAddTitle::lmCmdAddTitle(bool fNormalCmd, lmDocument *pDoc, wxString& sText,
                             lmTextStyle* pStyle, lmEHAlign nAlign)
	: lmScoreCommand(_("add title"), pDoc, fNormalCmd)
    , m_Style(*pStyle)
    , m_nAlign(nAlign)
    , m_sText(sText)
    , m_nTitleID(lmNEW_ID)
{
}

bool lmCmdAddTitle::Do()
{
    //reposition cursor and log command if undoable
    LogCommand();

	lmScore* pScore = m_pDoc->GetScore();
    lmTextStyle* pStyle = pScore->AddStyle(m_Style.sName, m_Style.tFont,
                                           m_Style.nColor);
    lmScoreTitle* pNewTitle 
        = new lmScoreTitle(pScore, m_nTitleID, m_sText, lmBLOCK_ALIGN_BOTH,
                           lmHALIGN_DEFAULT, lmVALIGN_DEFAULT, pStyle);
	pScore->AttachAuxObj(pNewTitle);
    m_nTitleID = pNewTitle->GetID();

    //AWARE: See, in lmCmdAttachText command, an explanation for saving m_nTitleID

	return CommandDone(true);
}



//----------------------------------------------------------------------------------------
// lmCmdChangeBarline implementation
//----------------------------------------------------------------------------------------

lmCmdChangeBarline::lmCmdChangeBarline(bool fNormalCmd, lmDocument *pDoc, lmBarline* pBL,
									   lmEBarline nType, bool fVisible)
	: lmScoreCommand(_("change barline"), pDoc, fNormalCmd)
    , m_nBarlineID(pBL->GetID())
    , m_nType(nType)
	, m_fVisible(fVisible)
    , m_nOldType(pBL->GetBarlineType())
	, m_fOldVisible(pBL->IsVisible())
{
}

lmCmdChangeBarline::~lmCmdChangeBarline()
{
}

bool lmCmdChangeBarline::Do()
{
    //Direct command. NO UNDO LOG

    //reposition cursor
    RestoreCursor();

    lmBarline* pBL = (lmBarline*)GetScoreObj(m_nBarlineID);
    pBL->SetBarlineType(m_nType);
    pBL->SetVisible(m_fVisible);
	return CommandDone(true);
}

bool lmCmdChangeBarline::Undo()
{
    //Direct command. NO UNDO LOG

    lmBarline* pBL = (lmBarline*)GetScoreObj(m_nBarlineID);
    pBL->SetBarlineType(m_nOldType);
    pBL->SetVisible(m_fOldVisible);
    return CommandUndone();
}



//----------------------------------------------------------------------------------------
// lmCmdChangeMidiSettings implementation
//----------------------------------------------------------------------------------------

lmCmdChangeMidiSettings::lmCmdChangeMidiSettings(bool fNormalCmd, lmDocument *pDoc,
                                                 lmInstrument* pInstr,
                                                 int nMidiChannel,
                                                 int nMidiInstr)
	: lmScoreCommand(_("change MIDI settings"), pDoc, fNormalCmd)
    , m_nInstrID(pInstr->GetID())
    , m_nMidiChannel(nMidiChannel)
    , m_nMidiInstr(nMidiInstr)
    , m_nOldMidiChannel(pInstr->GetMIDIChannel())
    , m_nOldMidiInstr(pInstr->GetMIDIInstrument())
{
}

bool lmCmdChangeMidiSettings::Do()
{
    //Direct command. NO UNDO LOG

    //reposition cursor
    RestoreCursor();

    lmInstrument* pInstr = (lmInstrument*)GetScoreObj(m_nInstrID);
    pInstr->SetMIDIChannel(m_nMidiChannel);
    pInstr->SetMIDIInstrument(m_nMidiInstr);
	return CommandDone(true, lmHINT_NO_LAYOUT);
}

bool lmCmdChangeMidiSettings::Undo()
{
    //Direct command. NO UNDO LOG

    lmInstrument* pInstr = (lmInstrument*)GetScoreObj(m_nInstrID);
    pInstr->SetMIDIChannel(m_nOldMidiChannel);
    pInstr->SetMIDIInstrument(m_nOldMidiInstr);
    return CommandUndone(lmHINT_NO_LAYOUT);
}



//----------------------------------------------------------------------------------------
// lmCmdMoveNote implementation
//----------------------------------------------------------------------------------------

lmCmdMoveNote::lmCmdMoveNote(bool fNormalCmd, lmDocument *pDoc, lmNote* pNote,
                             const lmUPoint& uPos, int nSteps)
	: lmScoreCommand(_("move note"), pDoc, fNormalCmd)
	, m_uxPos(uPos.x)
	, m_nNoteID(pNote->GetID())
    , m_nSteps(nSteps)
{
}

bool lmCmdMoveNote::Do()
{
    //Direct command. NO UNDO LOG

    //reposition cursor
    RestoreCursor();

    lmNote* pNote = (lmNote*)GetScoreObj(m_nNoteID);
	pNote->ChangePitch(m_nSteps);
    m_uxOldPos = pNote->SetUserXLocation(m_uxPos);

	return CommandDone(true);
}

bool lmCmdMoveNote::Undo()
{
    //Direct command. NO UNDO LOG

    lmNote* pNote = (lmNote*)GetScoreObj(m_nNoteID);
	pNote->SetUserXLocation(m_uxOldPos);
	pNote->ChangePitch(-m_nSteps);
    return CommandUndone();
}



//----------------------------------------------------------------------------------------
// lmCmdMoveObjectPoints implementation
//----------------------------------------------------------------------------------------

lmCmdMoveObjectPoints::lmCmdMoveObjectPoints(bool fNormalCmd, const wxString& name,
                               lmDocument *pDoc, lmGMObject* pGMO,
                               lmUPoint uShift[], int nNumPoints, bool fDoLayout)
	: lmScoreCommand(name, pDoc, fNormalCmd, fDoLayout)
    , m_nNumPoints(nNumPoints)
	, m_nObjID(pGMO->GetScoreOwner()->GetID())
    , m_nShapeIdx(pGMO->GetOwnerIDX())
{
    wxASSERT(nNumPoints > 0);

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

    //reposition cursor
    RestoreCursor();

    lmScoreObj* pSCO = GetScoreObj(m_nObjID);
    pSCO->MoveObjectPoints(m_nNumPoints, m_nShapeIdx, m_pShifts, true);  //true->add shifts
	return CommandDone(true, lmHINT_NO_LAYOUT);
}

bool lmCmdMoveObjectPoints::Undo()
{
    //Direct command. NO UNDO LOG

    lmScoreObj* pSCO = GetScoreObj(m_nObjID);
    pSCO->MoveObjectPoints(m_nNumPoints, m_nShapeIdx, m_pShifts, false);  //false->substract shifts
    return CommandUndone(lmHINT_NO_LAYOUT);
}


