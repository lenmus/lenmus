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
#include "Processor.h"

//access to logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;



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


IMPLEMENT_ABSTRACT_CLASS(lmScoreCommand, wxCommand)

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

void lmScoreCommand::PrepareForRedo()
{
    //move cursor to original position when the command was issued. This position
    //was saved in m_CursorState.
    //Then, save data for undoing the command.

    //reposition cursor
    RestoreCursor();

    if (!m_fUndoable)
        return;

    //get score and save source code
    m_sOldSource = m_pDoc->GetScore()->SourceLDP(true);     //true: export cursor
    LogForensicData();        //save data for forensic analysis if a crash
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
    pScore->ResetUndoMode();
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

    ////DBG ---------------------------------------------
    //wxLogMessage(_T("[lmScoreCommand::CommandDone] Dump of new score follows:"));
    //wxLogMessage( m_pDoc->GetScore()->Dump() );
    //wxLogMessage( m_pDoc->GetScore()->SourceLDP(true) );
    //wxMessageBox(_T("[lmScoreCommand::CommandDone] Dump completed."));
    ////END DBG -----------------------------------------

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

void lmScoreCommand::LogForensicData()
{
    //save data for forensic analysis if a crash

    g_pLogger->FlushForensicLog();
    g_pLogger->LogForensic(
        wxString::Format(_T("Command class: %s, Command name: '%s'"),
                         this->GetClassInfo()->GetClassName(),
                         this->GetName().c_str()
                         ));
    g_pLogger->LogScore(m_sOldSource);
}


//----------------------------------------------------------------------------------------
// lmCmdDeleteSelection implementation
//----------------------------------------------------------------------------------------

IMPLEMENT_CLASS(lmCmdDeleteSelection, lmScoreCommand)

lmCmdDeleteSelection::lmCmdDeleteSelection(bool fNormalCmd,
                                           const wxString& sName,
                                           lmDocument *pDoc,
                                           lmGMSelection* pSelection)
	: lmScoreCommand(sName, pDoc, fNormalCmd)
{
    //loop to get affected ScoreObjs and add their ID to the ignore set
    lmScoreCursor* pScoreCursor = m_pDoc->GetScore()->GetCursor();
    lmGMObject* pGMO = pSelection->GetFirst();
    while (pGMO)
    {
        lmScoreObj* pSCO = pGMO->GetScoreOwner();       //get affected ScoreObj
        //For secondary & prolog shapes the StaffObj is not deleted
        if (pGMO->IsMainShape())
        {
            bool fDelete = true;     //default: delete this ScoreObj
            if (pSCO->IsBarline() || pSCO->IsClef() || pSCO->IsKeySignature()
                || pSCO->IsTimeSignature())
            {
                //barlines, clefs, keys and time signatures will be deleted later.
                //Move cursor to point barline to delete and create command to delete barline
                fDelete = false;
                pScoreCursor->MoveCursorToObject((lmStaffObj*)pSCO);
                wxString sName = _T("Delete object");
                if (pSCO->IsBarline())
                    sName = _T("Delete barline");
                else if (pSCO->IsClef())
                    sName = _T("Delete clef");
                else if (pSCO->IsKeySignature())
                    sName = _T("Delete key signature");
                else if (pSCO->IsTimeSignature())
                    sName = _T("Delete time signature");
                lmScoreCommand* pCmd = new lmCmdDeleteStaffObj(lmCMD_HIDDEN, sName, pDoc,
                                                    (lmStaffObj*)pSCO);
                m_Commands.push_back( pCmd );
            }
            else if (pSCO->IsStaff() || pSCO->IsVStaff())
            {
                fDelete = false;    //staves and VStaves are never deleted
            }
            else if (pSCO->IsInstrument())
            {
                //braces/brakes owner is the instrument. Do not delete instrument
                if (pGMO->IsShapeBracket())
                    fDelete = false;
            }


            if (fDelete)
                m_IgnoreSet.insert(pSCO->GetID());
        }

        pGMO = pSelection->GetNext();
    }
}

lmCmdDeleteSelection::~lmCmdDeleteSelection()
{
    std::list<lmScoreCommand*>::iterator it;
    for (it = m_Commands.begin(); it != m_Commands.end(); ++it)
        delete *it;
}

bool lmCmdDeleteSelection::Do()
{
    //reposition cursor and save data for undoing the command
    PrepareForRedo();
    std::set<long>::iterator it;
    for (it = m_IgnoreSet.begin(); it != m_IgnoreSet.end(); ++it)
        g_pLogger->LogForensic(_T("IgnoreList: %d"), *it);

    //re-parse the source ignoring objects to delete
    if (m_IgnoreSet.size() > 0)
    {
        lmLDPParser parser;
        parser.SetIgnoreList(&m_IgnoreSet);

        lmScore* pScore = parser.ParseScoreFromText(m_sOldSource);
        if (!pScore)
        {
            wxASSERT(false);
            return false;
        }

        //ask document to replace current score by the new one, but do not update views
        pScore->ResetUndoMode();
        m_pDoc->ReplaceScore(pScore, false);        //false: do not update views
    }

    //now issue other delete commnads
    std::list<lmScoreCommand*>::iterator itC;
    for (itC = m_Commands.begin(); itC != m_Commands.end(); ++itC)
        (*itC)->Do();

    return CommandDone(true, lmHINT_NEW_SCORE);
}



//----------------------------------------------------------------------------------------
// lmCmdDeleteStaffObj implementation
//----------------------------------------------------------------------------------------

IMPLEMENT_CLASS(lmCmdDeleteStaffObj, lmScoreCommand)

lmCmdDeleteStaffObj::lmCmdDeleteStaffObj(bool fNormalCmd,
                                         const wxString& sName,
                                         lmDocument *pDoc, lmStaffObj* pSO, bool fAskUser)
	: lmScoreCommand(sName, pDoc, fNormalCmd)
    , m_nObjID(pSO->GetID())
    , m_nAction(0)              //0: Cancel operation
{
    //if there are notes affected by clef or key removal, get user desired behaviour
    if (pSO->IsClef())
    {
        if (fAskUser && pSO->GetVStaff()->CheckIfNotesAffectedByDeletingClef())
            m_nAction = pSO->GetVStaff()->AskUserAboutClef();
        else
            m_nAction = 1;      //keep pitch
    }
    else if (pSO->IsKeySignature())
    {
        if (fAskUser && pSO->GetVStaff()->CheckIfNotesAffectedByKey(true))    //true->skip this key
            m_nAction = pSO->GetVStaff()->AskUserAboutKey();
        else
            m_nAction = 2;      //do nothing. Pitch could change if affected by deleted accidentals
    }
}

bool lmCmdDeleteStaffObj::Do()
{
    //reposition cursor and save data for undoing the command
    PrepareForRedo();

    //Get pointer to object to delete
    lmStaffObj* pSO = (lmStaffObj*)GetScoreObj(m_nObjID);

    //Proceed to delete the object
    bool fError = false;
    if (pSO->IsClef())
        fError = GetVStaff()->Cmd_DeleteClef((lmClef*)pSO, m_nAction);
    else if (pSO->IsTimeSignature())
        fError = GetVStaff()->Cmd_DeleteTimeSignature((lmTimeSignature*)pSO);
    else if (pSO->IsKeySignature())
        fError = GetVStaff()->Cmd_DeleteKeySignature((lmKeySignature*)pSO, m_nAction);
    else
        fError = GetVStaff()->Cmd_DeleteStaffObj(pSO);

	return CommandDone(!fError);
}



//----------------------------------------------------------------------------------------
// lmCmdDeleteTie implementation
//----------------------------------------------------------------------------------------

IMPLEMENT_CLASS(lmCmdDeleteTie, lmScoreCommand)

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

IMPLEMENT_CLASS(lmCmdAddTie, lmScoreCommand)

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

IMPLEMENT_CLASS(lmCmdMoveObject, lmScoreCommand)

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

IMPLEMENT_CLASS(lmCmdInsertBarline, lmScoreCommand)

lmCmdInsertBarline::lmCmdInsertBarline(bool fNormalCmd,
                                       const wxString& sName, lmDocument *pDoc,
                                       lmEBarline nType)
	: lmScoreCommand(sName, pDoc, fNormalCmd)
    , m_nBarlineType(nType)
{
}

bool lmCmdInsertBarline::Do()
{
    //reposition cursor and save data for undoing the command
    PrepareForRedo();

    //insert the barline
    lmBarline* pBL = GetVStaff()->Cmd_InsertBarline(m_nBarlineType);

    return CommandDone(pBL != (lmBarline*)NULL);
}




//----------------------------------------------------------------------------------------
// lmCmdInsertClef: Insert a clef at current cursor position
//----------------------------------------------------------------------------------------

IMPLEMENT_CLASS(lmCmdInsertClef, lmScoreCommand)

lmCmdInsertClef::lmCmdInsertClef(bool fNormalCmd,
                                 const wxString& sName,
                                 lmDocument *pDoc, lmEClefType nClefType)
	: lmScoreCommand(sName, pDoc, fNormalCmd)
    , m_nClefType(nClefType)
{
}

bool lmCmdInsertClef::Do()
{
    //reposition cursor and save data for undoing the command
    PrepareForRedo();

    //insert the clef
    lmClef* pClef = GetVStaff()->Cmd_InsertClef(m_nClefType);
    return CommandDone(pClef != (lmClef*)NULL);
}



//----------------------------------------------------------------------------------------
// lmCmdInsertFiguredBass: Insert a figured bass symbol at current cursor timepos
//----------------------------------------------------------------------------------------

IMPLEMENT_CLASS(lmCmdInsertFiguredBass, lmScoreCommand)

lmCmdInsertFiguredBass::lmCmdInsertFiguredBass(bool fNormalCmd, lmDocument *pDoc,
                                               wxString& sFigBass)
	: lmScoreCommand(_("Insert figured bass"), pDoc, fNormalCmd)
    , m_fFirstTime(true)
    , m_FBData(sFigBass)
{
}

bool lmCmdInsertFiguredBass::Do()
{
    //reposition cursor and save data for undoing the command
    PrepareForRedo();

    //insert the figured bass
    lmFiguredBass* pFB = GetVStaff()->Cmd_InsertFiguredBass(&m_FBData);

    //if not redo (first time Do() is invoked), edit the inserted figured bass
    if (pFB && m_fFirstTime)
    {
        m_fFirstTime = false;
	    lmDlgProperties dlg((lmController*)NULL);
	    pFB->OnEditProperties(&dlg);
	    dlg.Layout();
	    if (dlg.ShowModal() == wxID_OK)
        {
            //save the new figured bass data, for redo
            m_FBData.CopyDataFrom( (lmFiguredBassData*)pFB );
        }
        else
        {
            //user has cancelled insertion. Remove object
            GetVStaff()->Cmd_DeleteStaffObj(pFB);
        }
    }

    //pFB->GetScore()->Dump(_T("dump.txt"));
    return CommandDone(pFB != (lmFiguredBass*)NULL);
}



//----------------------------------------------------------------------------------------
// lmCmdInsertFBLine: Insert a figured bass line at current cursor timepos
//----------------------------------------------------------------------------------------

IMPLEMENT_CLASS(lmCmdInsertFBLine, lmScoreCommand)

lmCmdInsertFBLine::lmCmdInsertFBLine(bool fNormalCmd, lmDocument *pDoc)
	: lmScoreCommand(_("Insert figured bass"), pDoc, fNormalCmd)
{
}

bool lmCmdInsertFBLine::Do()
{
    //reposition cursor and save data for undoing the command
    PrepareForRedo();

    //insert the figured bass line
    lmFiguredBassLine* pFBL = GetVStaff()->Cmd_InsertFBLine();

    //pFB->GetScore()->Dump(_T("dump.txt"));
    return CommandDone(pFBL != (lmFiguredBassLine*)NULL);
}



//----------------------------------------------------------------------------------------
// lmCmdInsertTimeSignature: Insert a time signature at current cursor position
//----------------------------------------------------------------------------------------

IMPLEMENT_CLASS(lmCmdInsertTimeSignature, lmScoreCommand)

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
    //reposition cursor and save data for undoing the command
    PrepareForRedo();

    //insert the barline
    lmTimeSignature* pTS = GetVStaff()
        ->Cmd_InsertTimeSignature(m_nBeats, m_nBeatType, m_fVisible);
    return CommandDone(pTS != (lmTimeSignature*)NULL);
}



//----------------------------------------------------------------------------------------
// lmCmdInsertKeySignature: Insert a key signature at current cursor position
//----------------------------------------------------------------------------------------

IMPLEMENT_CLASS(lmCmdInsertKeySignature, lmScoreCommand)

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
    //reposition cursor and save data for undoing the command
    PrepareForRedo();

    //insert the key
    lmKeySignature* pKey = GetVStaff()->Cmd_InsertKeySignature(m_nFifths, m_fMajor, m_fVisible);
    return CommandDone(pKey != (lmKeySignature*)NULL);
}



//----------------------------------------------------------------------------------------
// lmCmdInsertNote: Insert a note at current cursor position
//----------------------------------------------------------------------------------------

IMPLEMENT_CLASS(lmCmdInsertNote, lmScoreCommand)

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
    //reposition cursor and save data for undoing the command
    PrepareForRedo();

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

IMPLEMENT_CLASS(lmCmdInsertRest, lmScoreCommand)

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
    //reposition cursor and save data for undoing the command
    PrepareForRedo();

    //insert the rest
    bool fAutoBar = lmPgmOptions::GetInstance()->GetBoolValue(lm_DO_AUTOBAR);
    lmRest* pRest =  GetVStaff()->Cmd_InsertRest(m_nNoteType, m_rDuration,
                                                    m_nDots, m_nVoice, fAutoBar);

    return CommandDone(pRest != (lmRest*)NULL);
}



//----------------------------------------------------------------------------------------
// lmCmdChangeNotePitch: Change pitch of note at current cursor position
//----------------------------------------------------------------------------------------

IMPLEMENT_CLASS(lmCmdChangeNotePitch, lmScoreCommand)

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

IMPLEMENT_CLASS(lmCmdChangeNoteAccidentals, lmScoreCommand)

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

IMPLEMENT_CLASS(lmCmdChangeNoteRestDots, lmScoreCommand)

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
    //reposition cursor and save data for undoing the command
    PrepareForRedo();

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

IMPLEMENT_CLASS(lmCmdDeleteTuplet, lmScoreCommand)

lmCmdDeleteTuplet::lmCmdDeleteTuplet(bool fNormalCmd,
                                     const wxString& sName, lmDocument *pDoc,
                                     lmNoteRest* pStartNR)
	: lmScoreCommand(sName, pDoc, fNormalCmd)
    , m_nStartID(pStartNR->GetID())
{
}

bool lmCmdDeleteTuplet::Do()
{
    //reposition cursor and save data for undoing the command
    PrepareForRedo();

    //Get pointer to object to delete
    lmNoteRest* pNR = (lmNoteRest*)GetScoreObj(m_nStartID);

    //Proceed to delete the tuplet
    bool fError = GetVStaff()->Cmd_DeleteTuplet(pNR);
    return CommandDone(!fError);
}



//----------------------------------------------------------------------------------------
// lmCmdAddTuplet implementation: Add a tuplet to notes in current selection
//----------------------------------------------------------------------------------------

IMPLEMENT_CLASS(lmCmdAddTuplet, lmScoreCommand)

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
    //reposition cursor and save data for undoing the command
    PrepareForRedo();

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

IMPLEMENT_CLASS(lmCmdBreakBeam, lmScoreCommand)

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
    //reposition cursor and save data for undoing the command
    PrepareForRedo();

    //Get pointer to note and issue the command
    GetVStaff()->Cmd_BreakBeam( (lmNoteRest*)GetScoreObj(m_nBeforeNR) );

    return CommandDone(true);
}



//----------------------------------------------------------------------------------------
// lmCmdJoinBeam implementation
//----------------------------------------------------------------------------------------

IMPLEMENT_CLASS(lmCmdJoinBeam, lmScoreCommand)

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
    //reposition cursor and save data for undoing the command
    PrepareForRedo();

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

IMPLEMENT_CLASS(lmCmdChangeText, lmScoreCommand)

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

IMPLEMENT_CLASS(lmCmdChangePageMargin, lmScoreCommand)

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

IMPLEMENT_CLASS(lmCmdAttachText, lmScoreCommand)

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
    //reposition cursor and save data for undoing the command
    PrepareForRedo();

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

IMPLEMENT_CLASS(lmCmdAddTitle, lmScoreCommand)

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
    //reposition cursor and save data for undoing the command
    PrepareForRedo();

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

IMPLEMENT_CLASS(lmCmdChangeBarline, lmScoreCommand)

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
// lmCmdChangeFiguredBass implementation
//----------------------------------------------------------------------------------------

IMPLEMENT_CLASS(lmCmdChangeFiguredBass, lmScoreCommand)

lmCmdChangeFiguredBass::lmCmdChangeFiguredBass(bool fNormalCmd, lmDocument *pDoc,
                                               lmFiguredBass* pFB, wxString& sFigBass)
	: lmScoreCommand(_("Change figured bass"), pDoc, fNormalCmd)
    , m_nFigBasID(pFB->GetID())
    , m_sFigBass(sFigBass)
{
    m_FBData.CopyDataFrom( (lmFiguredBassData*)pFB );
}

bool lmCmdChangeFiguredBass::Do()
{
    //Direct command. NO UNDO LOG

    //reposition cursor
    RestoreCursor();

    lmFiguredBass* pFB = (lmFiguredBass*)GetScoreObj(m_nFigBasID);
    pFB->SetDataFromString(m_sFigBass);
	return CommandDone(true);
}

bool lmCmdChangeFiguredBass::Undo()
{
    //Direct command. NO UNDO LOG

    lmFiguredBassData* pFBData = (lmFiguredBassData*)GetScoreObj(m_nFigBasID);
    pFBData->CopyDataFrom(&m_FBData);
    return CommandUndone();
}


//----------------------------------------------------------------------------------------
// lmCmdChangeMidiSettings implementation
//----------------------------------------------------------------------------------------

IMPLEMENT_CLASS(lmCmdChangeMidiSettings, lmScoreCommand)

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

IMPLEMENT_CLASS(lmCmdMoveNote, lmScoreCommand)

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

IMPLEMENT_CLASS(lmCmdMoveObjectPoints, lmScoreCommand)

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



//----------------------------------------------------------------------------------------
// lmCmdScoreProcessor implementation
//----------------------------------------------------------------------------------------

IMPLEMENT_CLASS(lmCmdScoreProcessor, lmScoreCommand)

lmCmdScoreProcessor::lmCmdScoreProcessor(bool fNormalCmd, lmDocument *pDoc,
                                         lmScoreProcessor* pProc)
	: lmScoreCommand(_("Score processor"), pDoc, fNormalCmd)
{
    //copy score processor and inform that a copy is going to be used
    m_pProc = pProc;
    lmProcessorMngr::GetInstance()->IncrementReference(pProc);

    //get process options
    m_pOpt = pProc->GetProcessOptions();
}

lmCmdScoreProcessor::~lmCmdScoreProcessor()
{
    lmProcessorMngr::GetInstance()->DeleteScoreProcessor(m_pProc);
}

bool lmCmdScoreProcessor::Do()
{
    //reposition cursor and save data for undoing the command
    PrepareForRedo();

	lmScore* pScore = m_pDoc->GetScore();
    bool fOK = m_pProc->ProcessScore(pScore, m_pOpt);

	return CommandDone(fOK);
}



//----------------------------------------------------------------------------------------
// lmCmdToggleNoteStem: Toggle stem of notes in current selection
//----------------------------------------------------------------------------------------

IMPLEMENT_CLASS(lmCmdToggleNoteStem, lmScoreCommand)

lmCmdToggleNoteStem::lmCmdToggleNoteStem(bool fNormalCmd, lmDocument *pDoc,
                                         lmGMSelection* pSelection)
	: lmScoreCommand(_("Toggle stem"), pDoc, fNormalCmd)
{
    //loop to save data about note/rests to modify
    lmGMObject* pGMO = pSelection->GetFirst();
    while (pGMO)
    {
        if (pGMO->GetType() == eGMO_ShapeNote)
        {
            lmNote* pNote = (lmNote*)pGMO->GetScoreOwner();
            m_Notes.push_back( pNote->GetID() );
        }
        pGMO = pSelection->GetNext();
    }
}

lmCmdToggleNoteStem::~lmCmdToggleNoteStem()
{
    //delete selection data
    m_Notes.clear();
}

bool lmCmdToggleNoteStem::Do()
{
    //Direct command. NO UNDO LOG

    //loop to toggle stems
    std::list<long>::iterator it;
    for (it = m_Notes.begin(); it != m_Notes.end(); ++it)
    {
        ((lmNote*)GetScoreObj(*it))->ToggleStem();
    }

    return CommandDone(true);
}

bool lmCmdToggleNoteStem::Undo()
{
    //Direct command. NO UNDO LOG

    //loop to toggle stems
    std::list<long>::iterator it;
    for (it = m_Notes.begin(); it != m_Notes.end(); ++it)
    {
        ((lmNote*)GetScoreObj(*it))->ToggleStem();
    }

    return CommandUndone();
}
