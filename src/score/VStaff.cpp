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

//--------------------------------------------------------------------------------------------------
// class lmVStaff: A generalization of the 'staff' concept.
//
//    The object lmVStaff (Virtual staff) is a generalization of the concept 'staff': it is a staff
//    with an great number of lines and spaces (almost infinite) so to represent all posible pitches,
//    but whose visual renderization can be controlled so
//    that only the specified (needed) lines are rendered i.e: the standard five-lines staff, the piano
//    grand staff, the single line staff used for rithm or unpitched music, or any other design you
//    would like.
//
//    An lmVStaff can contain many 'staffs' (groups of lines rendered toghether), depending on the layout
//    choosen. For example:
//    - the standard five-lines staff is an lmVStaff with one five-lines staff.
//    - the grand staff is an lmVStaff with two five-lines staves.
//    - the single line unpitched staff is an VSatff with one sigle line staff.
//
//    The concept of 'staff' is not modelled into the program, its is just a term to be used only
//    in documentation, algoritms explanations, etc.
//
//    An lmVStaff is composed by objects such as notes, rests, barlines, keys, and all other
//    musical signs traditionally used for writing music.
//    All this objects than can appear in a staff will be modeled by an abstract class 'lmStaffObj'.
//    Therefore an lmVStaff is, roughfly, a collection of StaffObjs and some attributes.
//
//    Bars are not modelled by objects. They are just the collection of lmStaffObj found between
//    two lmStaffObj of type 'barline' (and between the start of the score and the first barline).
//    Nevertheless, tha concept of bar is very important at least in two situations:
//        1. In interpretation (playing back the score). For example:  play from bar #7
//        2. When rendering the score, as all the objects in a bar must be rendered together in
//           the same paper line.
//    Due to this, althoug the bar is not modelled as an object, there exits methods in the
//    VStaff object to deal with bars.
//
//-------------------------------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "VStaff.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include <list>

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Score.h"
#include "Staff.h"
#include "VStaff.h"
#include "EditCmd.h"
#include "Context.h"
#include "UndoRedo.h"
#include "Notation.h"
#include "MetronomeMark.h"
#include "../sound/SoundManager.h"
#include "../app/global.h"
#include "../app/TheApp.h"		//to access g_rScreenDPI and g_rPixelsPerLU
#include "../app/Preferences.h"
#include "../graphic/GMObject.h"
#include "../graphic/ShapeStaff.h"
#include "../graphic/BoxSliceVStaff.h"
#include "../graphic/ShapeBarline.h"
#include "../widgets/MsgBox.h"


//for AddShitTimeTag methods
#define lmGO_FWD   true
#define lmGO_BACK  false

//constructor
lmVStaff::lmVStaff(lmScore* pScore, lmInstrument* pInstr)
    : lmScoreObj(pScore), m_cStaffObjs(this, 1)    // 1 = m_nNumStaves
{
    //pScore is the lmScore to which this vstaff belongs.
    //Initially the lmVStaff will have only one standard five-lines staff. This can be
    //later modified invoking the 'AddStaff' method (REVIEW)

    m_pScore = pScore;
    m_pInstrument = pInstr;

    // default lmVStaff margins (logical units = tenths of mm)
    m_nHeight = 0;          //a value of 0 means 'compute it'
    m_leftMargin = 0;
    m_topMargin = 0;
    m_rightMargin = 0;
    m_bottomMargin = lmToLogicalUnits(1, lmCENTIMETERS);    // 1 cm

    //initialize staves
    m_nNumStaves = 1;
    for (int i=0; i < lmMAX_STAFF; i++)
        m_cStaves[i] = (lmStaff*)NULL;

    //create one standard staff (five lines, 7.2 mm height)
    lmStaff* pStaff = new lmStaff(pScore);
    m_cStaves[0] = pStaff;

	//link cursor to collection
	m_VCursor.AttachToCollection(&m_cStaffObjs);

    //default value
    //TODO review this fixed space before the clef
    m_nSpaceBeforeClef = TenthsToLogical(10, 1);    // one line of first staff

    m_sErrorMsg = _T("");
}

lmVStaff::~lmVStaff()
{
    for (int i=0; i < lmMAX_STAFF; i++)
        if (m_cStaves[i]) delete m_cStaves[i];
}

lmLUnits lmVStaff::TenthsToLogical(lmTenths nTenths)
{
	return TenthsToLogical(nTenths, 1);
}

lmTenths lmVStaff::LogicalToTenths(lmLUnits uUnits)
{
	return LogicalToTenths(uUnits, 1);
}

lmStaff* lmVStaff::AddStaff(int nNumLines, lmLUnits nMicrons)
{
    wxASSERT(m_nNumStaves < lmMAX_STAFF);
    lmStaff* pStaff = new lmStaff(m_pScore, nNumLines, nMicrons);
    m_cStaves[m_nNumStaves] = pStaff;
    m_nNumStaves++;
    m_cStaffObjs.AddStaff();

    return pStaff;
}

lmStaff* lmVStaff::GetStaff(int nStaff)
{
    return m_cStaves[nStaff-1];
}

lmStaff* lmVStaff::GetFirstStaff()
{
    m_nCurStaff = 0;
    return m_cStaves[0];
}

lmStaff* lmVStaff::GetNextStaff()
{
    if (++m_nCurStaff < m_nNumStaves)
        return m_cStaves[m_nCurStaff];
    else
        return (lmStaff*)NULL;
}

lmStaff* lmVStaff::GetLastStaff()
{
    m_nCurStaff = m_nNumStaves-1;
    return m_cStaves[m_nCurStaff];
}

lmLUnits lmVStaff::TenthsToLogical(lmTenths nTenths, int nStaff)
{
    lmStaff* pStaff = GetStaff(nStaff);
    wxASSERT(pStaff);
    return pStaff->TenthsToLogical(nTenths);
}

lmTenths lmVStaff::LogicalToTenths(lmLUnits uUnits, int nStaff)
{
    lmStaff* pStaff = GetStaff(nStaff);
    wxASSERT(pStaff);
    return pStaff->LogicalToTenths(uUnits);
}

lmLUnits lmVStaff::GetStaffLineThick(int nStaff)
{
    lmStaff* pStaff = GetStaff(nStaff);
    wxASSERT(pStaff);
    return pStaff->GetLineThick();

}

//----------------------------------------------------------------------------------------
// contexts related
//----------------------------------------------------------------------------------------

void lmVStaff::OnContextUpdated(lmNote* pStartNote, int nStaff, int nStep,
                           int nNewAccidentals, lmContext* pCurrentContext)
{
    // Note pStartNote (whose diatonic name is nStep) has accidentals that must be
	// propagated to the context and to the following notes until the end of the measure
	// or until a new accidental for the same step is found
    
    lmTODO(_T("[lmVStaff::OnContextUpdated] All code in this method"));
}

int lmVStaff::GetUpdatedContextAccidentals(lmStaffObj* pThisSO, int nStep)
{
	//returns the current context, updated with the accidentals for step nStep,
    //applicable to StaffObj pThisSO.

	lmContext* pContext = m_cStaffObjs.NewUpdatedContext(pThisSO);
	if (pContext)
	{
		int nAcc = pContext->GetAccidentals(nStep);
		delete pContext;
		return nAcc;
	}
	else
		return 0;
}

lmTimeSignature* lmVStaff::GetApplicableTimeSignature()
{
    //returns the applicable time signature at current cursor position

    lmStaffObj* pSO = m_VCursor.GetStaffObj();
    if (pSO)
        return pSO->GetApplicableTimeSignature();
    else
    {
        pSO = m_VCursor.GetPreviousStaffobj();
        if (pSO)
            return pSO->GetApplicableTimeSignature();
        else
        {
            //empty segment?
            lmContext* pContext = m_cStaffObjs.GetStartOfSegmentContext(m_VCursor.GetSegment(),
                                                  m_VCursor.GetNumStaff() );
            if (pContext)
                return pContext->GetTime();
            else
                return (lmTimeSignature*)NULL;
        }
    }
}


//---------------------------------------------------------------------------------------
// Methods for inserting StaffObjs
//---------------------------------------------------------------------------------------

lmClef* lmVStaff::Cmd_InsertClef(lmUndoItem* pUndoItem, lmEClefType nClefType, int nStaff,
                                 bool fVisible)
{
    //if there are notes affected by new clef, get user desired behaviour
    int nAction = 1;        //0=Cancel operation, 1=keep pitch, 2=keep position
    if (CheckIfNotesAffectedByClef())
        nAction = AskUserAboutClef();

    if (nAction == 0)
        return (lmClef*)NULL;       //Cancel clef insertion

    bool fClefKeepPosition = (nAction == 2);

    //save answer for undo/redo
    lmUndoData* pUndoData = pUndoItem->GetUndoData();
    pUndoData->AddParam<bool>(fClefKeepPosition);

    //create the clef and prepare its insertion
    lmStaffObj* pCursorSO = m_VCursor.GetStaffObj();
    lmClef* pClef = new lmClef(nClefType, this, nStaff, fVisible);
    lmStaff* pStaff = GetStaff(nStaff);
    lmContext* pContext = (pCursorSO ? GetCurrentContext(pCursorSO) : GetLastContext(nStaff));
    pContext = pStaff->NewContextAfter(pClef, pContext);

	pClef->SetContext(pContext);

    //proceed to insert the clef
    m_cStaffObjs.Add(pClef, fClefKeepPosition);

    return pClef;
}

void lmVStaff::UndoCmd_InsertClef(lmUndoItem* pUndoItem, lmClef* pClef)
{
    //delete the requested object, and log info to undo history
    //Precondition: must be in this VStaff

    //recover user option about keeping pitch or position
    lmUndoData* pUndoData = pUndoItem->GetUndoData();
    bool fClefKeepPosition = pUndoData->GetParam<bool>();

    //remove the contexts created by the clef
	pClef->RemoveCreatedContexts();

    //now remove the clef from the staffobjs collection
    m_cStaffObjs.Delete(pClef, true, fClefKeepPosition);        //true->invoke destructor
}

lmTimeSignature* lmVStaff::Cmd_InsertTimeSignature(lmUndoItem* pUndoItem, int nBeats,
                                    int nBeatType, bool fVisible)
{
    //It must return NULL if not succedeed

    lmTimeSignature* pTS = new lmTimeSignature(nBeats, nBeatType, this, fVisible);
    if ( InsertKeyTimeSignature(pUndoItem, pTS) )
        return pTS;
    else
    {
        delete pTS;
        return (lmTimeSignature*)NULL;
    }
}

void lmVStaff::UndoCmd_InsertTimeSignature(lmUndoItem* pUndoItem, lmTimeSignature* pTS)
{
    //delete the requested object, and log info to undo history

    //remove the contexts created by the TS
	pTS->RemoveCreatedContexts();

    //now remove the TS from the staffobjs collection
    m_cStaffObjs.Delete(pTS, true);        //true->invoke destructor
}

lmKeySignature* lmVStaff::Cmd_InsertKeySignature(lmUndoItem* pUndoItem, int nFifths,
                                    bool fMajor, bool fVisible)
{
    //It must return NULL if not succedeed

    lmKeySignature* pKS = new lmKeySignature(nFifths, fMajor, this, fVisible);
    if ( InsertKeyTimeSignature(pUndoItem, pKS) )
        return pKS;
    else
    {
        delete pKS;
        return (lmKeySignature*)NULL;
    }
}

void lmVStaff::UndoCmd_InsertKeySignature(lmUndoItem* pUndoItem, lmKeySignature* pKS)
{
    //delete the requested object, and log info to undo history

    //remove the contexts created by the KS
	pKS->RemoveCreatedContexts();

    //now remove the KS from the staffobjs collection
    m_cStaffObjs.Delete(pKS, true);        //true->invoke destructor
}

bool lmVStaff::InsertKeyTimeSignature(lmUndoItem* pUndoItem, lmStaffObj* pKTS)
{
    //This method implements the common part of methods Cmd_InsertKeySignature() and
    //Cmd_InsertTimeSignature. Object pKTS is the key/time signature to insert
    //It returns 'true' if succedeed

    wxASSERT(pKTS->IsKeySignature() || pKTS->IsTimeSignature());

    //save object pointed by cursor
    lmStaffObj* pSaveSO = m_VCursor.GetStaffObj();

    //Check that we are at start of measure. Otherwise, insert a double barline
    if (!IsEqualTime(m_VCursor.GetTimepos(), 0.0f))
    {
        //lmErrorBox oEB(_("Error: Key/time signature can only be inserted at start of measure"), _("Insertion will be cancelled."));
        //oEB.ShowModal();
        //return (lmTimeSignature*)NULL;

        //issue an 'insert barline' command
        lmUndoLog* pUndoLog = pUndoItem->GetUndoLog();
        lmUndoItem* pNewUndoItem = new lmUndoItem(pUndoLog);
        lmECmdInsertBarline* pECmd = 
            new lmECmdInsertBarline(this, pNewUndoItem, lm_eBarlineDouble);
        pUndoLog->LogCommand(pECmd, pNewUndoItem);
    }

    //Locate context insertion points for all staves
    //The key/time signature is for all staves. Therefore, we have to create a new context
    //in every staff, and chain it, at the right point, in the contexts chain. We know 
    //the key/time insertion point in first staff but we have to locate insertion points
    //for every staff
    std::list<lmVStaffCursor*> cAuxCursors;
    int nNumMeasure = m_VCursor.GetSegment();
    bool fIsTime = pKTS->IsTimeSignature();
    for (int iStaff=1; iStaff <= m_nNumStaves; iStaff++)
	{
        // Create an auxiliary cursor and position it at start of desired measure
        lmVStaffCursor* pAuxCursor = new lmVStaffCursor();
        pAuxCursor->AttachToCollection(&m_cStaffObjs);
        pAuxCursor->AdvanceToStartOfSegment(nNumMeasure, iStaff);

        // Advance aux cursor, if necessary, to skip clef and, if applicable, key signature.
        pAuxCursor->SkipClefKey(fIsTime);           //true -> skip key

        //Add a new context at found insertion point
        lmStaffObj* pCursorSO = pAuxCursor->GetStaffObj();
        lmContext* pContext = (pCursorSO ? GetCurrentContext(pCursorSO) : GetLastContext(iStaff));
        lmStaff* pStaff = GetStaff(iStaff);
        if (fIsTime)
            ((lmTimeSignature*)pKTS)->SetContext(iStaff, 
                                        pStaff->NewContextAfter((lmTimeSignature*)pKTS, pContext) );
        else
            ((lmKeySignature*)pKTS)->SetContext(iStaff,
	                                pStaff->NewContextAfter((lmKeySignature*)pKTS, pContext) );

        // save pointer to insertion point for this staff
        cAuxCursors.push_back(pAuxCursor);
    }

    // insert the key/time signature object in first staff
    lmVStaffCursor* pAuxCursor = cAuxCursors.front();
    pAuxCursor->AttachToCollection(&m_cStaffObjs, false);   //false: do not reset cursor
    m_cStaffObjs.Add(pKTS);

    //restore cursor and reposition it at time signature insertion point
    m_VCursor.AttachToCollection(&m_cStaffObjs, false);    //false-> do not reset it
    if (pSaveSO)
        m_VCursor.MoveCursorToObject(pSaveSO);

    //delete aux cursors
    std::list<lmVStaffCursor*>::iterator it;
    for (it = cAuxCursors.begin(); it != cAuxCursors.end(); ++it)
        delete *it;

    return true;    //success
}

lmBarline* lmVStaff::Cmd_InsertBarline(lmUndoItem* pUndoItem, lmEBarline nType, bool fVisible)
{

    //move cursor to start of current timepos
    //AWARE:
    //  Cursor might be pointing not to the first SO at current timepos. Therefore
    //  we have to move to the first SO at current timepos. Otherwise, the barline
    //  would be inserted between to objects at the same timepos!
    //  As an example, assume a piano grand staff with a C note on
    //  first staff and a G note on second staff. Also assume that cursor is pointing 
    //  to second staff, G note. As both notes C & G are at the same timepos, it would 
    //  be wrong to insert the barline before the G note. Therefore, it is necessary
    //  to find the first SO at current timepos (the C note in the example) and insert
    //  the barline there.
    m_VCursor.AdvanceToStartOfTimepos();

    //now, proceed to insert the barline
    lmBarline* pBarline = new lmBarline(nType, this, fVisible);
    m_cStaffObjs.Add(pBarline);

    return pBarline;
}

void lmVStaff::UndoCmd_InsertBarline(lmUndoItem* pUndoItem, lmBarline* pBarline)
{
    //delete the requested object, and log info to undo history

    //now remove the barline from the staffobjs collection
    m_cStaffObjs.Delete(pBarline, true);        //true->invoke destructor
}

lmNote* lmVStaff::Cmd_InsertNote(lmUndoItem* pUndoItem,
								 lmEPitchType nPitchType, int nStep, int nOctave,
                                 lmENoteType nNoteType, float rDuration, int nDots,
								 lmENoteHeads nNotehead, lmEAccidentals nAcc,
                                 int nVoice, lmNote* pBaseOfChord, bool fTiedPrev,
								 bool fAutoBar)
{
    int nStaff = m_VCursor.GetNumStaff();

	//get the applicable context
    lmStaffObj* pCursorSO = m_VCursor.GetStaffObj();
    lmContext* pContext;
    if (pCursorSO)
	    pContext = NewUpdatedContext(pCursorSO);    //TODO 1: pointed SO could be in different staff
    else
        pContext = NewUpdatedLastContext(nStaff);

    //if no Clef defined yet the context will be NULL
    if (!pContext || !pContext->GetClef())
    {
        wxString sQuestion = _("Error: No clef defined yet.");
        sQuestion += _T("\n\n");
        sQuestion += _("Would you like to have notes placed on the staff as if a G clef has been defined?");

        lmQuestionBox oQB(sQuestion, 2,     //msge, num buttons,
            _("Insert clef"), _("An invisible G clef will be inserted before the note."),
            _("Cancel"), _("The 'insert note' command will be cancelled.")
        );
        int nAnswer = oQB.ShowModal();

		if (nAnswer == 0)   //'Insert clef' button
		{
            //issue an 'insert clef' command
            lmUndoLog* pUndoLog = pUndoItem->GetUndoLog();
            lmUndoItem* pNewUndoItem = new lmUndoItem(pUndoLog);
            lmECmdInsertClef* pECmd = 
                new lmECmdInsertClef(this, pNewUndoItem, lmE_Sol, nStaff, lmNO_VISIBLE);
            pUndoLog->LogCommand(pECmd, pNewUndoItem);

			//re-compute context
			if (pCursorSO)
				pContext = NewUpdatedContext(pCursorSO);
			else
				pContext = NewUpdatedLastContext(nStaff);
			if (!pContext)
				return (lmNote*)NULL;
		}
		else
			return (lmNote*)NULL;
    }

    lmTBeamInfo BeamInfo[6];
    for (int i=0; i < 6; i++) {
        BeamInfo[i].Repeat = false;
        BeamInfo[i].Type = eBeamNone;
    }
	int nAccidentals = 0;

    lmNote* pNt = new lmNote(this, nPitchType,
                        nStep, nOctave, nAccidentals, nAcc,
                        nNoteType, rDuration, nDots, nStaff, nVoice, lmVISIBLE,
                        pContext, false, BeamInfo, pBaseOfChord, false, lmSTEM_DEFAULT);

    m_cStaffObjs.Add(pNt);

	delete pContext;

    //if requested to tie it with a previous note, try to do it
    if (fTiedPrev)
    {
        lmNote* pNtStart = FindPossibleStartOfTie(pNt);
        if (pNtStart)
        {
            //create the tie
            pNt->CreateTie(pNtStart, pNt);
        }
    }

    //if this note fills up a measure and AutoBar option is enabled, insert a simple barline
    if (fAutoBar)
        CheckAndDoAutoBar(pUndoItem, pNt);

    return pNt;
}

void lmVStaff::UndoCmd_InsertNote(lmUndoItem* pUndoItem, lmNote* pNote)
{
    //delete the requested object, and log info to undo history

    //remove the note from the staffobjs collection
    m_cStaffObjs.Delete(pNote, true);        //true->invoke destructor
}

lmRest* lmVStaff::Cmd_InsertRest(lmUndoItem* pUndoItem,
                                 lmENoteType nNoteType, float rDuration, int nDots,
                                 bool fAutoBar)
{
    int nStaff = m_VCursor.GetNumStaff();

    lmTBeamInfo BeamInfo[6];
    for (int i=0; i < 6; i++) {
        BeamInfo[i].Repeat = false;
        BeamInfo[i].Type = eBeamNone;
    }

	//TODO: For now, only auto-voice. It is necessary to get info from GUI about
	//user selected voice. Need to change this command parameter list to include voice
	int nVoice = 0;     //auto-voice

    lmRest* pRest = new lmRest(this, nNoteType, rDuration, nDots, nStaff, nVoice, lmVISIBLE,
                             false, BeamInfo);

    m_cStaffObjs.Add(pRest);

    //if this rest fills up a measure and AutoBar option is enabled, insert a simple barline
    if (fAutoBar)
        CheckAndDoAutoBar(pUndoItem, pRest);

    return pRest;
}

void lmVStaff::UndoCmd_InsertRest(lmUndoItem* pUndoItem, lmRest* pRest)
{
    //delete the requested object, and log info to undo history

    //now remove the rest from the staffobjs collection
    m_cStaffObjs.Delete(pRest, true);        //true->invoke destructor
}

void lmVStaff::CheckAndDoAutoBar(lmUndoItem* pUndoItem, lmNoteRest* pNR)
{
    //Check if note/rest pNR fills a measure. If it does, add a barline if necessary


    //get normal measure duration
    lmTimeSignature* pTime = GetApplicableTimeSignature();
    if (!pTime)
        return;         //no time signature. Do not add barlines
    //TODO: When no TS, we could try to insert hidden barlines to deal with no time
    //signature scores

    float rMeasure = pTime->GetMeasureDuration();

    //get time ocupied by all notes/rests until pNR included
    float rCurrent = pNR->GetTimePos() + pNR->GetTimePosIncrement();

    //check if measure is full
    //wxLogMessage(_T("[lmVStaff::CheckAndDoAutoBar] current=%.2f, maximum=%.2f"), rCurrent, rMeasure);
    if (IsLowerTime(rCurrent, rMeasure))
      return;         //measure is not full

    //Measure is full after note/rest pNR.
    //Add a barline if no barline in current segment or if more notes/rests after pNR

    bool fInsertBarline = false;        //assume no need to add barline
    lmBarline* pBL = GetBarlineOfMeasure( GetVCursor()->GetSegment() + 1 );
    if (!pBL)
    {
        //no barline. Add one
        fInsertBarline = true;
    }
    else
    {
        //check if more notes/rest with greater timepos in current measure
        lmSOIterator* pIT = m_cStaffObjs.CreateIteratorFrom(eTR_ByTime, pNR);
        pIT->MoveNext();        //skip pNR
        while(!pIT->EndOfMeasure())
        {
            lmStaffObj* pSO = pIT->GetCurrent();
			if (pSO->IsNoteRest())
            {
                //there are notes/rest after pNR.
                //If they have greater timepos insert a barline after pNR
                if (IsHigherTime(pSO->GetTimePos(), pNR->GetTimePos()) )
                    fInsertBarline = true;

                break;
            }
            pIT->MoveNext();
        }
        delete pIT;
    }
  
    //finally, insert the barline if necessary
    if (fInsertBarline)
    {
        //Issue an 'insert barline' command
        lmUndoLog* pUndoLog = pUndoItem->GetUndoLog();
        lmUndoItem* pNewUndoItem = new lmUndoItem(pUndoLog);
        lmECmdInsertBarline* pECmd =
            new lmECmdInsertBarline(this, pNewUndoItem, lm_eBarlineSimple, lmVISIBLE);
        pUndoLog->LogCommand(pECmd, pNewUndoItem);
    }
}

void lmVStaff::Cmd_DeleteStaffObj(lmUndoItem* pUndoItem, lmStaffObj* pSO)
{
    //delete the requested object, and log info to undo history
    wxASSERT(pUndoItem);

    //AWARE: Logged actions must be logged in the required order for re-construction.
    //History works as a FIFO stack: first one logged will be the first one to be recovered

    //save positioning information
    m_cStaffObjs.LogObjectToDeletePosition(pUndoItem->GetUndoData(), pSO);

    //Save info to re-create the object
    pSO->Freeze(pUndoItem->GetUndoData());

    //if object to remove is a clef, key or time signature, the contexts they created
    //have to be removed
	pSO->RemoveCreatedContexts();

    //Delete the object
    m_cStaffObjs.Delete(pSO, false);    //false = do not delete object, only remove it from collection
}

void lmVStaff::UndoCmd_DeleteStaffObj(lmUndoItem* pUndoItem, lmStaffObj* pSO)
{
    //un-delete the object, according to info in history

    //recover positioning info
    lmStaffObj* pBeforeSO = pUndoItem->GetUndoData()->GetParam<lmStaffObj*>();

    //unfreeze restored object
    pSO->UnFreeze(pUndoItem->GetUndoData());

	//re-insert the deleted object
    m_cStaffObjs.Insert(pSO, pBeforeSO);
	//wxLogMessage(m_cStaffObjs.Dump());
}

int lmVStaff::AskUserAboutClef()
{
    //When a clef is inserted/deleted or changed it might be necessary to update
    //following note pitches. If this is the case, this method ask user what to do:
    //maintain pitch->move notes, or change pitch->do not reposition notes. 
    //User might also choose to cancel the deletion.
    //Returns:
    //  0=Cancel operation, 1=keep pitch, 2=keep position

    lmPgmOptions* pPgmOpt = lmPgmOptions::GetInstance();
    long nOptValue = pPgmOpt->GetLongValue(lm_DO_CLEF_CHANGE);  //0=ask, 1=keep pitch, 2=keep position
    if (nOptValue == 0)
    {
        wxString sQuestion = _("Notes after the clef will be affected by this action.");
        sQuestion += _T("\n\n");
        sQuestion +=
_("Would you like to keep notes' pitch and, therefore, to change \
notes' positions on the staff? or, would you prefer to keep notes \
placed on their current staff positions? (implies pitch change)");

        lmQuestionBox oQB(sQuestion, 3,     //msge, num buttons,
            //labels (2 per button: button text + explanation)
            _("Keep position"), _("Change notes' pitch and keep their current staff position."),
            _("Keep pitch"), _("Keep pitch and move notes to new staff positions."),
            _("Cancel"), _("The insert, delete or change clef command will be cancelled.") 
        );
        int nAnswer = oQB.ShowModal();

		if (nAnswer == 0)       //'Keep position' button
            return 2;
        else if (nAnswer == 1)  //'Keep pitch' button
            return 1;
        else
            return 0;       //Cancel operation
    }
    else
         return (int)nOptValue;
}

void lmVStaff::Cmd_DeleteClef(lmUndoItem* pUndoItem, lmClef* pClef)
{
    //if there are notes affected by deleting clef, get user desired behaviour
    int nAction = 1;        //0=Cancel operation, 1=keep pitch, 2=keep position
    //TODO: determine if it is necessary to ask user. For now, always ask
    //if (CheckIfNotesAffectedByClef())
        nAction = AskUserAboutClef();

    if (nAction == 0)
        return;       //Cancel clef insertion

    bool fClefKeepPosition = (nAction == 2);

    //save answer for undo/redo
    lmUndoData* pUndoData = pUndoItem->GetUndoData();
    pUndoData->AddParam<bool>(fClefKeepPosition);

    //remove the contexts created by the clef
	pClef->RemoveCreatedContexts();

    //now remove the clef from the staffobjs collection
    m_cStaffObjs.Delete(pClef, false, fClefKeepPosition);        //false->do not invoke destructor
}

void lmVStaff::UndoCmd_DeleteClef(lmUndoItem* pUndoItem, lmClef* pClef)
{
    //recover user option about keeping pitch or position
    lmUndoData* pUndoData = pUndoItem->GetUndoData();
    bool fClefKeepPosition = pUndoData->GetParam<bool>();

    //prepare clef insertion
    lmStaffObj* pCursorSO = m_VCursor.GetStaffObj();
    int nStaff = pClef->GetStaffNum();
    lmStaff* pStaff = GetStaff(nStaff);
    lmContext* pContext = (pCursorSO ? GetCurrentContext(pCursorSO) : GetLastContext(nStaff));
    pContext = pStaff->NewContextAfter(pClef, pContext);

	pClef->SetContext(pContext);

    //proceed to insert the clef
    m_cStaffObjs.Add(pClef, fClefKeepPosition);
}

void lmVStaff::Cmd_DeleteKeySignature(lmUndoItem* pUndoItem, lmKeySignature* pKS)
{
    //TODO: As user about inserting accidentals in following notes

    //remove the contexts created by the KS
	pKS->RemoveCreatedContexts();

    //now remove the KS from the staffobjs collection
    m_cStaffObjs.Delete(pKS, false);        //false->do not invoke destructor
}

void lmVStaff::UndoCmd_DeleteKeySignature(lmUndoItem* pUndoItem, lmKeySignature* pKS)
{
    InsertKeyTimeSignature(pUndoItem, pKS);
}

void lmVStaff::Cmd_DeleteTimeSignature(lmUndoItem* pUndoItem, lmTimeSignature* pTS)
{
    //TODO: As user about anything?

 //   //delete the requested object, and log info to undo history
 //   wxASSERT(pUndoItem);

 //   //AWARE: Logged actions must be logged in the required order for re-construction.
 //   //History works as a FIFO stack: first one logged will be the first one to be recovered

 //   //save positioning information
 //   m_cStaffObjs.LogObjectToDeletePosition(pUndoItem->GetUndoData(), pSO);

 //   //Save info to re-create the object
 //   pSO->Freeze(pUndoItem->GetUndoData());

 //   //if object to remove is a clef, key or time signature, the contexts they created
 //   //have to be removed
	//pSO->RemoveCreatedContexts();

 //   //Delete the object
 //   m_cStaffObjs.Delete(pSO, false);    //false = do not delete object, only remove it from collection

    //remove the contexts created by the TS
	pTS->RemoveCreatedContexts();

    //now remove the TS from the staffobjs collection
    m_cStaffObjs.Delete(pTS, false);        //false->do not invoke destructor
}

void lmVStaff::UndoCmd_DeleteTimeSignature(lmUndoItem* pUndoItem, lmTimeSignature* pTS)
{
 //   //un-delete the object, according to info in history

 //   //recover positioning info
 //   lmStaffObj* pBeforeSO = pUndoItem->GetUndoData()->GetParam<lmStaffObj*>();

 //   //unfreeze restored object
 //   pSO->UnFreeze(pUndoItem->GetUndoData());

	////re-insert the deleted object
 //   m_cStaffObjs.Insert(pSO, pBeforeSO);

	//wxLogMessage(m_cStaffObjs.Dump());
    InsertKeyTimeSignature(pUndoItem, pTS);
}

void lmVStaff::Cmd_DeleteTie(lmUndoItem* pUndoItem, lmNote* pEndNote)
{
    //delete the requested tie, and log info to undo history
    wxASSERT(pUndoItem);

    //AWARE: Logged actions must be logged in the required order for re-construction.
    //History works as a FIFO stack: first one logged will be the first one to be recovered

    //save start note
    lmUndoData* pUndoData = pUndoItem->GetUndoData();
    lmNote* pStartNote = pEndNote->GetTiedNotePrev();
    pUndoData->AddParam<lmNote*>( pStartNote );

    //remove tie
    pEndNote->DeleteTiePrev();
}

void lmVStaff::UndoCmd_DeleteTie(lmUndoItem* pUndoItem, lmNote* pEndNote)
{
    //un-delete the tie, according to info in history

    //recover start note
    lmUndoData* pUndoData = pUndoItem->GetUndoData();
    lmNote* pStartNote = pUndoItem->GetUndoData()->GetParam<lmNote*>();

    //re-create the tie
    pEndNote->CreateTie(pStartNote, pEndNote);
}

void lmVStaff::Cmd_AddTie(lmUndoItem* pUndoItem, lmNote* pStartNote, lmNote* pEndNote)
{
    //add a tie
    pEndNote->CreateTie(pStartNote, pEndNote);
}

void lmVStaff::UndoCmd_AddTie(lmUndoItem* pUndoItem, lmNote* pStartNote, lmNote* pEndNote)
{
    //remove tie
    pEndNote->DeleteTiePrev();
}

void lmVStaff::Cmd_AddTuplet(lmUndoItem* pUndoItem, std::vector<lmNoteRest*>& notes,
                             bool fShowNumber, int nNumber, bool fBracket,
                             lmEPlacement nAbove, int nActual, int nNormal)
{
    // add a tuplet

    WXUNUSED(pUndoItem);

    //create the tuplet object
    lmTupletBracket* pTuplet = 
        new lmTupletBracket(fShowNumber, nNumber, fBracket, nAbove, nActual, nNormal);

    //include the tuplet in the notes and adjusts notes duration
    float rFactor = (float)nNormal / (float)nActual;
    std::vector<lmNoteRest*>::iterator it;
    for (it=notes.begin(); it != notes.end(); ++it)
    {
        //include the note in the tuplet
        pTuplet->Include(*it);

        //adjust note/rest duration and recompute current measure duration
        float rOldDuration = (*it)->GetDuration();
        float rNewDuration = rOldDuration * rFactor;
        (*it)->SetDuration(rNewDuration);
        m_cStaffObjs.RecomputeSegmentDuration(*it, rNewDuration - rOldDuration);
    }
}

void lmVStaff::UndoCmd_AddTuplet(lmUndoItem* pUndoItem, lmNoteRest* pStartNR)
{
    WXUNUSED(pUndoItem);

    //get the tuplet
    lmTupletBracket* pTuplet = pStartNR->GetTuplet();

    //re-adjust notes/rests duration
    float rFactor = (float)pTuplet->GetActualNotes() / (float)pTuplet->GetNormalNotes();
    lmNoteRest* pNR = pTuplet->GetFirstNoteRest();
    while (pNR)
    {
        //adjust note/rest duration and recompute current measure duration
        float rOldDuration = pNR->GetDuration();
        float rNewDuration = rOldDuration * rFactor;
        pNR->SetDuration(rNewDuration);
        m_cStaffObjs.RecomputeSegmentDuration(pNR, rNewDuration - rOldDuration);

        //proceed with next note/rest
        pNR = pTuplet->GetNextNoteRest();
    }

    //remove the tuplet
    pNR = pTuplet->GetFirstNoteRest();
    while (pNR)
    {
        pNR->OnRemovedFromTuplet();
        pNR = pTuplet->GetNextNoteRest();
    }
    delete pTuplet;
}

void lmVStaff::Cmd_DeleteTuplet(lmUndoItem* pUndoItem, lmNoteRest* pStartNR)
{
    //delete the tuplet, adjust duration of affected notes/rests, and log info
    //to undo history

    //get the tuplet
    lmTupletBracket* pTuplet = pStartNR->GetTuplet();
    if (!pTuplet) return;       //nothing to do

    //save data for undoing the command
    //AWARE: Logged actions must be logged in the required order for re-construction.
    //History works as a FIFO stack: first one logged will be the first one to be recovered
    wxASSERT(pUndoItem);
    lmUndoData* pUndoData = pUndoItem->GetUndoData();

        //save number of note/rests in the tuplet
    pUndoData->AddParam<int>( pTuplet->NumNotes() );

    //save pointers to the note/rests and adjust notes/rests duration
    float rFactor = (float)pTuplet->GetActualNotes() / (float)pTuplet->GetNormalNotes();
    lmNoteRest* pNR = pTuplet->GetFirstNoteRest();
    while (pNR)
    {
        //save pointer
        pUndoData->AddParam<lmNoteRest*>( pNR );

        //adjust notes/rests duration and recompute current measure duration
        float rOldDuration = pNR->GetDuration();
        float rNewDuration = rOldDuration * rFactor;
        pNR->SetDuration(rNewDuration);
        m_cStaffObjs.RecomputeSegmentDuration(pNR, rNewDuration - rOldDuration);

        //proceed with next note/rest
        pNR = pTuplet->GetNextNoteRest();
    }

    //save tuplet info:
    pTuplet->Save(pUndoData);

    //remove the tuplet
    pNR = pTuplet->GetFirstNoteRest();
    while (pNR)
    {
        pNR->OnRemovedFromTuplet();
        pNR = pTuplet->GetNextNoteRest();
    }
    delete pTuplet;
}

void lmVStaff::UndoCmd_DeleteTuplet(lmUndoItem* pUndoItem, lmNoteRest* pStartNR)
{
    //un-delete the tuplet, according to info in history

    //recover number of note/rests in the tuplet
    lmUndoData* pUndoData = pUndoItem->GetUndoData();
    int nNumNotes = pUndoData->GetParam<int>();

    //recover pointers to the note/rests
    std::vector<lmNoteRest*> notes;
    for (int i=0; i < nNumNotes; i++)
        notes.push_back( pUndoData->GetParam<lmNoteRest*>() );

    //recover tuplet info and re-create the tuplet object
    lmTupletBracket* pTuplet = new lmTupletBracket(pStartNR, pUndoData);

    //add the tuplet to the notes and adjusts their duration
    float rFactor = (float)pTuplet->GetNormalNotes() / (float)pTuplet->GetActualNotes();
    std::vector<lmNoteRest*>::iterator it;
    bool fFirst = true;
    for (it=notes.begin(); it != notes.end(); ++it)
    {
        //skip first note, as it is automatically included at tuplet constructor
        if (!fFirst)
            pTuplet->Include(*it);

        //adjust note/rest duration and recompute current measure duration
        float rOldDuration = (*it)->GetDuration();
        float rNewDuration = rOldDuration * rFactor;
        (*it)->SetDuration(rNewDuration);
        m_cStaffObjs.RecomputeSegmentDuration(*it, rNewDuration - rOldDuration);

        fFirst = false;
    }
}

void lmVStaff::Cmd_BreakBeam(lmUndoItem* pUndoItem, lmNoteRest* pBeforeNR)
{
    //break the beamed group before note/rest pBeforeNR. 

    //it is previously verified that pBeforeNR is beamed and it is not the first one
    //of the beam

    //get the beam
    lmBeam* pBeam = pBeforeNR->GetBeam();

    //find this note/rest and the previous one and count notes before break point
    int nNotesBefore = 0;
    lmNoteRest* pNR = pBeam->GetFirstNoteRest();
    lmNoteRest* pPrevNR = (lmNoteRest*)NULL;
    while (pNR && pNR != pBeforeNR)
    {
        pPrevNR = pNR;
        ++nNotesBefore;
        pNR = pBeam->GetNextNoteRest();
    }
    wxASSERT(pNR);          //pBeforeNR must be found!!
    wxASSERT(pPrevNR);      //pBeforeNR is not the first one in the beam

    //save data for undoing the command: all note/rests before break point
    //AWARE: Logged actions must be logged in the required order for re-construction.
    //History works as a FIFO stack: first one logged will be the first one to be recovered
    wxASSERT(pUndoItem);
    lmUndoData* pUndoData = pUndoItem->GetUndoData();

    //save number of note/rests before the break point
    pUndoData->AddParam<int>( nNotesBefore );

    //save pointers to the note/rests before break point
    std::vector<lmNoteRest*> notes;
    pNR = pBeam->GetFirstNoteRest();
    while (pNR && pNR != pBeforeNR)
    {
        //save pointer in undo log and also copy it in local variable
        pUndoData->AddParam<lmNoteRest*>( pNR );
        notes.push_back( pNR );
        pNR = pBeam->GetNextNoteRest();
    }

    int nNotesAfter = pBeam->NumNotes() - nNotesBefore;

    //four cases:
    //  a) two single notes         (nNotesBefore == 1 && nNotesAfter == 1)
    //  b) single note + new beam   (nNotesBefore == 1 && nNotesAfter > 1)
    //  c) new beam + new beam      (nNotesBefore > 1 && nNotesAfter > 1)
    //  d) new beam + single note   (nNotesBefore > 1 && nNotesAfter == 1)

    //Case a) two single notes 
    if (nNotesBefore == 1 && nNotesAfter == 1)
    {
        //just remove the beam
	    pPrevNR->OnRemovedFromBeam();
	    pBeforeNR->OnRemovedFromBeam();
        delete pBeam;
    }
    //cae b) single note + new beam
    else if (nNotesBefore == 1 && nNotesAfter > 1)
    {
        //remove first note from beam
        pBeam->Remove(pPrevNR);
	    pPrevNR->OnRemovedFromBeam();
        pBeam->AutoSetUp();
    }
    //case c) new beam + new beam
    else if (nNotesBefore > 1 && nNotesAfter > 1)
    {
        //split the beam. Create a new beam for first group and keep the existing one
        //for the second group
        std::vector<lmNoteRest*>::iterator it = notes.begin();

        pBeam->Remove(*it);
	    (*it)->OnRemovedFromBeam();
        lmBeam* pBeam1 = new lmBeam((lmNote*)(*it));

        ++it;
        while (it != notes.end())
        {
            pBeam->Remove(*it);
	        (*it)->OnRemovedFromBeam();
            pBeam1->Include((*it));
            ++it;
        }

        pBeam->AutoSetUp();
        pBeam1->AutoSetUp();
    }
    //case d) new beam + single note
    else if (nNotesBefore > 1 && nNotesAfter == 1)
    {
        //remove last note from beam
        pBeam->Remove(pBeforeNR);
	    pBeforeNR->OnRemovedFromBeam();
        pBeam->AutoSetUp();
    }
    else
        wxASSERT(false);
}

void lmVStaff::UndoCmd_BreakBeam(lmUndoItem* pUndoItem, lmNoteRest* pBeforeNR)
{
    //re-create the beamed group that was broken before note/rest pBeforeNR. 

    //recover the notes before pBeforeNR

    //recover number of note/rests
    lmUndoData* pUndoData = pUndoItem->GetUndoData();
    int nNotesBefore = pUndoData->GetParam<int>();

    //recover pointers to the note/rests
    std::vector<lmNoteRest*> notes;
    for (int i=0; i < nNotesBefore; i++)
        notes.push_back( pUndoData->GetParam<lmNoteRest*>() );

    //count notes after break point
    int nNotesAfter = (pBeforeNR->IsBeamed() ? pBeforeNR->GetBeam()->NumNotes() : 1);

    //re-create the beam. Four cases:
    //  a) join two single notes    (nNotesBefore == 1 && nNotesAfter == 1)
    //  b) join single note + beam  (nNotesBefore == 1 && nNotesAfter > 1)
    //  c) join two beams           (nNotesBefore > 1 && nNotesAfter > 1)
    //  d) join beam + single note  (nNotesBefore > 1 && nNotesAfter == 1)

    if (nNotesBefore == 1 && nNotesAfter == 1)
    {
        //case a) join two single notes
        lmBeam* pNewBeam = new lmBeam( (lmNote*)notes.front() );
        pNewBeam->Include(pBeforeNR);
    }
    else if (nNotesBefore == 1 && nNotesAfter > 1)
    {
        //case b) join single note + beam
        lmBeam* pBeam = pBeforeNR->GetBeam();
	    pBeam->Include(notes.front(), 0);
    }
    else if (nNotesBefore > 1 && nNotesAfter > 1)
    {
        //case c) join two beams
        //To keep note/rest order we must remove note/rests from the second beam and add them
        //to the firts one.
        lmBeam* pBeam1 = notes.front()->GetBeam();
        lmBeam* pBeam2 = pBeforeNR->GetBeam();
        lmNoteRest* pNR = pBeam2->GetFirstNoteRest();
        while (pNR)
        {
            pBeam2->Remove(pNR);
	        pNR->OnRemovedFromBeam();
            pBeam1->Include(pNR);
            //AWARE:  As we have removed the firts note, GetNextNoteRest() will fail because
            //the internal iterator is now invalid. Moreover, the next note is now the first one.
            pNR = pBeam2->GetFirstNoteRest();
        }
        delete pBeam2;
    }
    else if (nNotesBefore > 1 && nNotesAfter == 1)
    {
        //case d) join beam + single note
        lmBeam* pBeam = notes.front()->GetBeam();
	    pBeam->Include(pBeforeNR);
    }
    else
        wxASSERT(false);
}

void lmVStaff::Cmd_JoinBeam(lmUndoItem* pUndoItem, std::vector<lmNoteRest*>& notes)
{
    //depending on received note/rests beam status, either:
    // - create a beamed group with the received notes,
    // - join two or more beamed groups
    // - or add a note to a beamed group

    //preconditions:
    // - if not beamed, first note/rest must be a note
    // - note/rests to beam must be eighths or shorter ones


    //list of involved note/rests with its beam status information
    std::list<lmBeamNoteInfo*> oInvolvedNR;
    int nBeamIdx = 1;

    //create beam and add first note
    std::vector<lmNoteRest*>::iterator it = notes.begin();
    lmBeam* pNewBeam;
    if ((*it)->IsBeamed())
    {
        //First note is beamed. Add notes to this beam
        pNewBeam = (*it)->GetBeam();

        //save all notes in this beam
        lmNoteRest* pNR = pNewBeam->GetFirstNoteRest();
        while (pNR)
        {
            SaveBeamNoteInfo(pNR, oInvolvedNR, nBeamIdx);
            pNR = pNewBeam->GetNextNoteRest();
        }
        nBeamIdx++;
    }
    else
    {
        //save this note/rest info
        SaveBeamNoteInfo(*it, oInvolvedNR, 0);     //0 -> not beamed

        //Create a new beam and add all notes to it
        pNewBeam = new lmBeam((lmNote*)(*it));
    }

    //here target beam is initialized and first note/rest is included. Add remaining
    //notes to it
    for (++it; it != notes.end(); ++it)
    {
        if ((*it)->IsBeamed())
        {
            if ((*it)->GetBeam() == pNewBeam)
                ;   //nothing to do. Note already in target beam
            else
            {
                //remove this and following notes from current beam and add them to target beam
                lmBeam* pOldBeam = (*it)->GetBeam();

                //find this note
                lmNoteRest* pNR = pOldBeam->GetFirstNoteRest();
                while (pNR && pNR != *it)
                    pNR = pOldBeam->GetNextNoteRest();

                //copy the list of remaining notes in the beam, clear beam info from
                //notes and add them to target beam
                std::vector<lmNoteRest*> oldNotes;
                while(pNR)
                {
                    SaveBeamNoteInfo(pNR, oInvolvedNR, nBeamIdx);
	                pNR->OnRemovedFromBeam();
                    pNewBeam->Include(pNR);
                    oldNotes.push_back(pNR);
                    pNR = pOldBeam->GetNextNoteRest();
                }
                
                //remove notes form old beam
                std::vector<lmNoteRest*>::iterator itON;
                for(itON = oldNotes.begin(); itON != oldNotes.end(); ++itON)
                    pOldBeam->Remove(*itON);

                //delete old beam if empty. Else reorganize it
                if (pOldBeam->NumNotes() <= 1)
			        delete pOldBeam;
                else
                    pOldBeam->AutoSetUp();

                nBeamIdx++;
            }
        }
        else
        {
            //add the note to target beam
            SaveBeamNoteInfo(*it, oInvolvedNR, 0);     //0 -> not beamed
            pNewBeam->Include(*it);
        }
    }

    //reorganize new beam
    pNewBeam->AutoSetUp();


    //save data for undoing the command
    //AWARE: Logged actions must be logged in the required order for re-construction.
    //History works as a FIFO stack: first one logged will be the first one to be recovered
    wxASSERT(pUndoItem);
    lmUndoData* pUndoData = pUndoItem->GetUndoData();
    LogBeamData(pUndoData, oInvolvedNR);
}

void lmVStaff::UndoCmd_JoinBeam(lmUndoItem* pUndoItem)
{
    //Restore beam status for a collection of logged note/rests
    //AWARE: this method is used also for cmd UndoCmd_DeleteBeam


    //recover number of involved note/rests and data about them
    lmUndoData* pUndoData = pUndoItem->GetUndoData();
    int nNotes;
    std::list<lmBeamNoteInfo> notes;
    GetLoggedBeamData(pUndoData, &nNotes, notes);


        // Now we have all necessary information. Proceed to restore previous beam state

    //all note/rests either are in a single beam group or aren't beamed. If they are
    //in a beam remove noteRests from it an delete this beam
    std::list<lmBeamNoteInfo>::iterator it;
    lmBeam* pBeam = (lmBeam*)NULL;
    if (notes.front().pNR->IsBeamed())
    {
        //remove noe/rest from current beam
        pBeam = notes.front().pNR->GetBeam();
        for(it = notes.begin(); it != notes.end(); ++it)
        {
            pBeam->Remove((*it).pNR);
            (*it).pNR->OnRemovedFromBeam();
        }

        //delete the old beam
        wxASSERT(pBeam->NumNotes() == 0);
	    delete pBeam;
        pBeam = (lmBeam*)NULL;
    }

    //re-create each removed beam, and restore notes beam info
    int nCurBeam = 0;
    for(it = notes.begin(); it != notes.end(); ++it)
    {
        lmNoteRest* pNR = (*it).pNR;

        if ((*it).nBeamRef > 0)
        {
            if ((*it).nBeamRef > nCurBeam)
            {
                //create a new beam and add the note to it
                wxASSERT(pNR->IsNote());
                pBeam = new lmBeam((lmNote*)pNR);
                ++nCurBeam;
            }
            else
            {
                //add note/rest to current beam
                pBeam->Include(pNR);
            }
        }

        //restore note/rest beaming info
        for(int i=0; i < 6; i++)
            pNR->SetBeamInfo(i, (*it).tBeamInfo[i]);
    }
}

void lmVStaff::SaveBeamNoteInfo(lmNoteRest* pNR, std::list<lmBeamNoteInfo*>& oListNR, int nBeamIdx)
{
    lmBeamNoteInfo* pInvNR = new lmBeamNoteInfo;
    pInvNR->pNR = pNR;
    lmTBeamInfo* pBI = pNR->GetBeamInfo();
    for(int i=0; i < 6; i++)
        pInvNR->tBeamInfo[i] = *pBI;
    pInvNR->nBeamRef = nBeamIdx;
    oListNR.push_back(pInvNR);
}

void lmVStaff::LogBeamData(lmUndoData* pUndoData, std::list<lmBeamNoteInfo*>& oListNR)
{
    //save number of involved note/rests
    pUndoData->AddParam<int>( int(oListNR.size()) );

    //save data about the involved note/rests and clear the list
    std::list<lmBeamNoteInfo*>::iterator it;
    for (it = oListNR.begin(); it != oListNR.end(); ++it)
    {
        pUndoData->AddParam<lmBeamNoteInfo>( **it );
        delete *it;
    }
}

void lmVStaff::GetLoggedBeamData(lmUndoData* pUndoData, int* pNumNotes,
                                 std::list<lmBeamNoteInfo>& oListNR)
{
    //recover number of involved note/rests
    int nNotes = pUndoData->GetParam<int>();
    *pNumNotes = nNotes;

    //recover data about the involved note/rests
    for (int i=0; i < nNotes; i++)
        oListNR.push_back( pUndoData->GetParam<lmBeamNoteInfo>() );
}

void lmVStaff::Cmd_DeleteBeam(lmUndoItem* pUndoItem, lmNoteRest* pBeamedNR)
{
    //removes the beam associated to received note/rest

    wxASSERT(pBeamedNR->IsBeamed());

    //list of involved note/rests with its beam status information
    std::list<lmBeamNoteInfo*> oBeamNR;

    //save beam notes and status
    lmBeam* pBeam = pBeamedNR->GetBeam();
    lmNoteRest* pNR = pBeam->GetFirstNoteRest();
    while (pNR)
    {
        SaveBeamNoteInfo(pNR, oBeamNR, 1);      //beam 1, the only one we are removing
        pNR = pBeam->GetNextNoteRest();
    }

    //remove note/rests from beam
    std::list<lmBeamNoteInfo*>::iterator it;
    for(it = oBeamNR.begin(); it != oBeamNR.end(); ++it)
    {
        pBeam->Remove((*it)->pNR);
        (*it)->pNR->OnRemovedFromBeam();
    }

    //delete the beam
    wxASSERT(pBeam->NumNotes() == 0);
	delete pBeam;

    //save data for undoing the command
    //AWARE: Logged actions must be logged in the required order for re-construction.
    //History works as a FIFO stack: first one logged will be the first one to be recovered
    wxASSERT(pUndoItem);
    lmUndoData* pUndoData = pUndoItem->GetUndoData();
    LogBeamData(pUndoData, oBeamNR);
}

void lmVStaff::UndoCmd_DeleteBeam(lmUndoItem* pUndoItem)
{
    UndoCmd_JoinBeam(pUndoItem);
}

void lmVStaff::Cmd_ChangeDots(lmUndoItem* pUndoItem, lmNoteRest* pNR, int nDots)
{
    //Change the number of dots of NoteRests pNR. As a consecuence, the measure duration
    //must be reviewed, as in could change


    //AWARE: Logged actions must be logged in the required order for re-construction.
    //History works as a FIFO stack: first one logged will be the first one to be recovered

    //save current note/rest dots
    wxASSERT(pUndoItem);
    pUndoItem->GetUndoData()->AddParam<int>( pNR->GetNumDots() );

    //change dots and compute timepos increment/decrement
    float rOldDuration = pNR->GetDuration();
    pNR->ChangeDots(nDots);

    //recompute current measure duration
    m_cStaffObjs.RecomputeSegmentDuration(pNR, pNR->GetDuration() - rOldDuration);
}

void lmVStaff::UndoCmd_ChangeDots(lmUndoItem* pUndoItem, lmNoteRest* pNR)
{
    //Restore original dots


    //recover original dots
    wxASSERT(pUndoItem);
    int nDots = pUndoItem->GetUndoData()->GetParam<int>();

    //restore dots and compute timepos increment/decrement
    float rOldDuration = pNR->GetDuration();
    pNR->ChangeDots(nDots);

    //recompute current measure duration
    m_cStaffObjs.RecomputeSegmentDuration(pNR, pNR->GetDuration() - rOldDuration);
}

//---------------------------------------------------------------------------------------
// Methods for adding StaffObjs
//---------------------------------------------------------------------------------------

// adds a clef to the end of current StaffObjs collection
lmClef* lmVStaff::AddClef(lmEClefType nClefType, int nStaff, bool fVisible)
{
    wxASSERT(nStaff <= GetNumStaves());

    lmClef* pClef = new lmClef(nClefType, this, nStaff, fVisible);
    lmStaff* pStaff = GetStaff(nStaff);
    lmContext* pContext = pStaff->NewContextAfter(pClef, GetLastContext(nStaff));
	pClef->SetContext(pContext);
    m_cStaffObjs.Add(pClef);
    return pClef;
}

// adds a spacer to the end of current StaffObjs collection
lmSpacer* lmVStaff::AddSpacer(lmTenths nWidth)
{
    lmSpacer* pSpacer = new lmSpacer(this, nWidth);
    m_cStaffObjs.Add(pSpacer);
    return pSpacer;

}

lmStaffObj* lmVStaff::AddAnchorObj()
{
    // adds an anchor StaffObj to the end of current StaffObjs collection

    lmStaffObj* pAnchor;
    if (IsGlobalStaff())
		pAnchor = new lmScoreAnchor(this);
    else
        pAnchor = new lmAnchor(this);

    m_cStaffObjs.Add(pAnchor);
    return pAnchor;
}

// returns a pointer to the lmNote object just created
lmNote* lmVStaff::AddNote(lmEPitchType nPitchType,
                    int nStep, int nOctave, int nAlter,
                    lmEAccidentals nAccidentals,
                    lmENoteType nNoteType, float rDuration, int nDots,
                    int nStaff, int nVoice, bool fVisible,
                    bool fBeamed, lmTBeamInfo BeamInfo[],
                    lmNote* pBaseOfChord,
                    bool fTie,
                    lmEStemType nStem)
{

    wxASSERT(nStaff <= GetNumStaves() );
    lmContext* pContext = NewUpdatedLastContext(nStaff);

    lmNote* pNt = new lmNote(this, nPitchType,
                        nStep, nOctave, nAlter, nAccidentals,
                        nNoteType, rDuration, nDots, nStaff, nVoice,
						fVisible, pContext, fBeamed, BeamInfo, pBaseOfChord, fTie, nStem);

    m_cStaffObjs.Add(pNt);

	delete pContext;
    return pNt;
}

// returns a pointer to the lmRest object just created
lmRest* lmVStaff::AddRest(lmENoteType nNoteType, float rDuration, int nDots,
                      int nStaff, int nVoice, bool fVisible,
                      bool fBeamed, lmTBeamInfo BeamInfo[])
{
    wxASSERT(nStaff <= GetNumStaves() );

    lmRest* pR = new lmRest(this, nNoteType, rDuration, nDots, nStaff,
							nVoice, fVisible, fBeamed, BeamInfo);

    m_cStaffObjs.Add(pR);
    return pR;

}

lmTextItem* lmVStaff::AddText(wxString& sText, lmEHAlign nHAlign, lmFontInfo& tFontData,
                              bool fHasWidth)
{
    lmTextStyle* pTS = m_pScore->GetStyleName(tFontData);
    wxASSERT(pTS);
    return AddText(sText, nHAlign, pTS, fHasWidth);
}

lmTextItem* lmVStaff::AddText(wxString& sText, lmEHAlign nHAlign, lmTextStyle* pStyle,
                              bool fHasWidth)
{
    lmTextItem* pText = new lmTextItem(sText, nHAlign, pStyle);

    // create an anchor object
    lmStaffObj* pAnchor;
    if (fHasWidth)
    {
        //attach it to a spacer
        pAnchor = this->AddSpacer( pText->GetShape()->GetWidth() );
    }
    else
    {
        //No width. Attach it to an anchor
        pAnchor = AddAnchorObj();
    }
    pAnchor->AttachAuxObj(pText);

    return pText;
}

lmMetronomeMark* lmVStaff::AddMetronomeMark(int nTicksPerMinute,
                        bool fParentheses, bool fVisible)
{
    lmMetronomeMark* pMM = new lmMetronomeMark(this, nTicksPerMinute,
                                               fParentheses, fVisible);
    m_cStaffObjs.Add(pMM);
    return pMM;

}

lmMetronomeMark* lmVStaff::AddMetronomeMark(lmENoteType nLeftNoteType, int nLeftDots,
                        lmENoteType nRightNoteType, int nRightDots,
                        bool fParentheses, bool fVisible)
{
    lmMetronomeMark* pMM = new lmMetronomeMark(this, nLeftNoteType, nLeftDots,
                                               nRightNoteType, nRightDots,
                                               fParentheses, fVisible);
    m_cStaffObjs.Add(pMM);
    return pMM;

}

lmMetronomeMark* lmVStaff::AddMetronomeMark(lmENoteType nLeftNoteType, int nLeftDots,
                        int nTicksPerMinute, bool fParentheses, bool fVisible)
{
    lmMetronomeMark* pMM = new lmMetronomeMark(this, nLeftNoteType, nLeftDots,
                                               nTicksPerMinute,
                                               fParentheses, fVisible);
    m_cStaffObjs.Add(pMM);
    return pMM;

}


//for types eTS_Common, eTS_Cut and eTS_SenzaMisura
lmTimeSignature* lmVStaff::AddTimeSignature(lmETimeSignatureType nType, bool fVisible)
{
    lmTimeSignature* pTS = new lmTimeSignature(nType, this, fVisible);
    return AddTimeSignature(pTS);
}

//for type eTS_SingleNumber
lmTimeSignature* lmVStaff::AddTimeSignature(int nSingleNumber, bool fVisible)
{
    lmTimeSignature* pTS = new lmTimeSignature(nSingleNumber, this, fVisible);
    return AddTimeSignature(pTS);
}

//for type eTS_Composite
lmTimeSignature* lmVStaff::AddTimeSignature(int nNumBeats, int nBeats[], int nBeatType,
                                        bool fVisible)
{
    lmTimeSignature* pTS = new lmTimeSignature(nNumBeats, nBeats, nBeatType, this, fVisible);
    return AddTimeSignature(pTS);
}

//for type eTS_Multiple
lmTimeSignature* lmVStaff::AddTimeSignature(int nNumFractions, int nBeats[], int nBeatType[],
                                        bool fVisible)
{
    lmTimeSignature* pTS = new lmTimeSignature(nNumFractions, nBeats, nBeatType, this, fVisible);
    return AddTimeSignature(pTS);
}

//for type eTS_Normal
lmTimeSignature* lmVStaff::AddTimeSignature(int nBeats, int nBeatType, bool fVisible)
{
    lmTimeSignature* pTS = new lmTimeSignature(nBeats, nBeatType, this, fVisible);
    return AddTimeSignature(pTS);
}

lmTimeSignature* lmVStaff::AddTimeSignature(lmETimeSignature nTimeSign, bool fVisible)
{
    lmTimeSignature* pTS = new lmTimeSignature(nTimeSign, this, fVisible);
    return AddTimeSignature(pTS);
}


//common code for all time signatures types
lmTimeSignature* lmVStaff::AddTimeSignature(lmTimeSignature* pTS)
{
    //iterate over the collection of Staves to add a new context
    for (int nStaff=1; nStaff <= m_nNumStaves; nStaff++)
	{
        lmStaff* pStaff = GetStaff(nStaff);
	    lmContext* pContext = pStaff->NewContextAfter(pTS, GetLastContext(nStaff));
        pTS->SetContext(nStaff, pContext);
    }
    m_cStaffObjs.Add(pTS);
    return pTS;
}

lmKeySignature* lmVStaff::AddKeySignature(int nFifths, bool fMajor, bool fVisible)
{
    lmKeySignature* pKS = new lmKeySignature(nFifths, fMajor, this, fVisible);

    //iterate over the collection of Staves to add a new context
    for (int nStaff=1; nStaff <= m_nNumStaves; nStaff++)
	{
        lmStaff* pStaff = GetStaff(nStaff);
	    lmContext* pContext = pStaff->NewContextAfter(pKS, GetLastContext(nStaff));
        pKS->SetContext(nStaff, pContext);
    }
    m_cStaffObjs.Add(pKS);
    return pKS;
}

lmKeySignature* lmVStaff::AddKeySignature(lmEKeySignatures nKeySignature, bool fVisible)
{
    int nFifths = KeySignatureToNumFifths(nKeySignature);
    bool fMajor = IsMajor(nKeySignature);
    return AddKeySignature(nFifths, fMajor, fVisible);
}

int lmVStaff::GetNumMeasures()
{
    return m_cStaffObjs.GetNumMeasures();
}

lmLUnits lmVStaff::LayoutStaffLines(lmBox* pBox, lmLUnits xFrom, lmLUnits xTo, lmLUnits yPos)
{
    //Computes all staff lines of this lmVStaff and creates the necessary shapes
	//to render them. Add this shapes to the received lmBox object.
    //Returns the Y coord. of last line (line 1, last staff)

    bool fVisible = !HideStaffLines();

    //Set left position and lenght of lines, and save these values
    lmLUnits yCur = yPos + m_topMargin;
    m_yLinTop = yCur;              //save y coord. for first line start point

    //iterate over the collection of Staves (lmStaff Objects)
    for (int nStaff=1; nStaff <= m_nNumStaves; nStaff++)
	{
        lmStaff* pStaff = GetStaff(nStaff);
        //draw one staff
		lmShapeStaff* pShape =
				new lmShapeStaff(pStaff, nStaff, pStaff->GetNumLines(),
								 pStaff->GetLineThick(), pStaff->GetLineSpacing(),
								 xFrom, yCur, xTo, *wxBLACK );
		pBox->AddShape(pShape);
        yCur = pShape->GetYBottom() + pStaff->GetAfterSpace();
		m_yLinBottom = pShape->GetYBottom() - pStaff->GetLineThick();
        pShape->SetVisible(fVisible);
    }
	return m_yLinBottom;
}

void lmVStaff::SetUpFonts(lmPaper* pPaper)
{
    //iterate over the collection of Staves (lmStaff Objects) to set up the fonts
    // to use on that staff
    for (int nStaff=1; nStaff <= m_nNumStaves; nStaff++)
	{
        SetFont(GetStaff(nStaff), pPaper);
    }
}

void lmVStaff::SetFont(lmStaff* pStaff, lmPaper* pPaper)
{
    // Font "LeMus Notas" has been designed to draw on a staff whose interline
    // space is of 512 FUnits. This gives an optimal rendering on VGA displays (96 pixels per inch)
    // as staff lines are drawn on exact pixels, according to the following relationships:
    //       Let dyLines be the distance between lines (pixels), then
    //       Font size = 3 * dyLines   (points)
    //       Scale = 100 * dyLines / 8     (%)
    //
    // Given a zooming factor (as a percentage, i.e. zoom=250.0%) fontsize can be computed as
    //       i = Round((zoom*8) / 100)
    //       dyLines = i        (pixels)
    //       FontSize = 3*i        (points)
    //
    // As all scaling takes place in the DC it is not necessary to allocate fonts of
    // different size as all scaling takes place in the DC. Then:
    //       Let dyLines be the distance between lines (logical units), then
    //       Font size = 3 * dyLines   (logical points)

    static lmLUnits uPrevSpacing = 0.0f;
    static float rScale = 1.0f;

    lmLUnits uLineSpacing = pStaff->GetLineSpacing();
    if (uPrevSpacing != uLineSpacing)
    {
        //the font size is choosen so that the height of the C clef music symbol (it has the
        //same height than the staff height) matches the staff size
        uPrevSpacing = uLineSpacing;
        wxFont* pFont = pPaper->GetFont((int)(3.0f * uLineSpacing), _T("LenMus Basic") );
        pPaper->SetFont(*pFont);
        wxString sGlyph( aGlyphsInfo[GLYPH_C_CLEF].GlyphChar );
        lmLUnits uWidth, uHeight;
        pPaper->GetTextExtent(sGlyph, &uWidth, &uHeight);
        rScale = 2.0f * (4.0f * uLineSpacing) / uHeight;
        //wxLogMessage(_T("[lmVStaff::SetFont] Staff height = %.2f, uHeight = %.2f, scale = %.4f, g_rScreenDPI=%.2f"),
        //    (4.0f * uLineSpacing), uHeight, rScale, g_rScreenDPI);
    }

    // the font for drawing will be scaled by the DC.
    pStaff->SetFontDraw( pPaper->GetFont((int)(3.0 * uLineSpacing * rScale), _T("LenMus Basic") ));        //logical points

    //// the font for dragging is not scaled by the DC as all dragging operations takes
    //// place dealing with device units
    //int dyLinesD = pPaper->LogicalToDeviceY(100 * dyLinesL);
    //pStaff->SetFontDrag( pPaper->GetFont((3 * dyLinesD) / 100) );
}

lmLUnits lmVStaff::GetStaffOffset(int nStaff)
{
    //returns the Y offset to staff nStaff (1..n)
    wxASSERT(nStaff <= m_nNumStaves );
    lmLUnits yOffset = m_topMargin;

    // iterate over the collection of Staves (lmStaff Objects) to add up the
    // height and after space of all previous staves to the requested one
    for (int iS=1; iS < nStaff; iS++)
	{
        lmStaff* pStaff = GetStaff(iS);
        yOffset += pStaff->GetHeight();
        yOffset += pStaff->GetAfterSpace();
    }
    return yOffset;

}

wxString lmVStaff::Dump()
{
    return m_cStaffObjs.Dump();
}

wxString lmVStaff::SourceLDP(int nIndent)
{
	wxString sSource = _T("");
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += _T("(musicData\n");
    nIndent++;

    //iterate over the collection of StaffObjs, ordered by voice.
    //Measures must be processed one by one
    for (int nMeasure=1; nMeasure <= m_cStaffObjs.GetNumMeasures(); nMeasure++)
    {
        //add comment to separate measures
        sSource += _T("\n");
        sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
        sSource += wxString::Format(_T("//Measure %d\n"), nMeasure);

        int nNumVoices = m_cStaffObjs.GetNumVoicesInMeasure(nMeasure);
        int nVoice = 1;
        while (!m_cStaffObjs.IsVoiceUsedInMeasure(nVoice, nMeasure) &&
               nVoice <= lmMAX_VOICE)
        {
            nVoice++;
        }
        int nVoicesProcessed = 1;   //voice 0 is automatically processed
		lmBarline* pBL = (lmBarline*)NULL;
        bool fGoBack = false;
		float rTime = 0.0f;
        while (true)
        {
            lmSOIterator* pIT = m_cStaffObjs.CreateIterator(eTR_ByTime, nVoice);
            pIT->AdvanceToMeasure(nMeasure);
            while(!pIT->EndOfMeasure())
            {
                lmStaffObj* pSO = pIT->GetCurrent();
                //voice 0 staffobjs go with first voice if more than one voice
				if (!pSO->IsBarline())
				{
					if (nVoicesProcessed == 1)
					{
						if (!pSO->IsNoteRest() || ((lmNoteRest*)pSO)->GetVoice() == nVoice)
						{
							LDP_AddShitTimeTagIfNeeded(sSource, nIndent, lmGO_FWD, rTime, pSO);
							sSource += pSO->SourceLDP(nIndent);
							rTime = LDP_AdvanceTimeCounter(pSO);
						}
					}
					else
						if (pSO->IsNoteRest() && ((lmNoteRest*)pSO)->GetVoice() == nVoice)
						{
							LDP_AddShitTimeTagIfNeeded(sSource, nIndent, lmGO_FWD, rTime, pSO);
							sSource += pSO->SourceLDP(nIndent);
							rTime = LDP_AdvanceTimeCounter(pSO);
						}
				}
				else
					pBL = (lmBarline*)pSO;

                pIT->MoveNext();
            }
            delete pIT;

            //check if more voices
            if (++nVoicesProcessed >= nNumVoices) break;
            nVoice++;
            while (!m_cStaffObjs.IsVoiceUsedInMeasure(nVoice, nMeasure) &&
                nVoice <= lmMAX_VOICE)
            {
                nVoice++;
            }
            wxASSERT(nVoice <= lmMAX_VOICE);

            //there are more voices. Add (goBak) tag
            fGoBack = true;
            sSource += _T("\n");
            sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
            sSource += _T("(goBack start)\n");
            rTime = 0.0f;
        }

        //if goBack added, add a goFwd to ensure that we are at end of measure
        if (fGoBack)
        {
            sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
            sSource += _T("(goFwd end)\n");
        }

		//add barline, if present
		if (pBL)
			sSource += pBL->SourceLDP(nIndent);

    }

    //close musicData
    nIndent--;
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += _T(")\n");

    return sSource;

}

void lmVStaff::LDP_AddShitTimeTagIfNeeded(wxString& sSource, int nIndent, bool fFwd,
									      float rTime, lmStaffObj* pSO)
{
	if (!IsEqualTime(rTime, pSO->GetTimePos()))
	{
        //wxLogMessage(_T("[lmVStaff::LDP_AddShitTimeTagIfNeeded] Different times. Current: %.2f Needed: %.2f"),
        //             rTime, pSO->GetTimePos() );
		sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
		if (fFwd)
			sSource += _T("(goFwd ");
		else
			sSource += _T("(goBack ");
		sSource += wxString::Format(_T("%.0f)\n"), pSO->GetTimePos());
	}
}

float lmVStaff::LDP_AdvanceTimeCounter(lmStaffObj* pSO)
{
	float rTime = pSO->GetTimePos();

	//advance counter unless current object is a note in chord and is not the
	//last note in the chord
	bool fAdvance = true;
	if (pSO->IsNoteRest() && ((lmNote*)pSO)->IsNote())
	{
		lmNote* pNote = (lmNote*)pSO;
		if (pNote->IsInChord() && !pNote->GetChord()->IsLastNoteOfChord(pNote))
			fAdvance = false;
	}

	if (fAdvance)
		return rTime + pSO->GetTimePosIncrement();
	else
		return rTime;
}

void lmVStaff::XML_AddShitTimeTagIfNeeded(wxString& sSource, int nIndent, bool fFwd,
										  float rTime, lmStaffObj* pSO)
{
	if (!IsEqualTime(rTime, pSO->GetTimePos()))
        XML_AddShitTimeTag(sSource, nIndent, fFwd, pSO->GetTimePos());
}

void lmVStaff::XML_AddShitTimeTag(wxString& sSource, int nIndent, bool fFwd, float rTime)
{
	sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
	if (fFwd)
		sSource += _T("<forward>\n");
	else
		sSource += _T("<backup>\n");

	//duration
	nIndent++;
	sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
	sSource += wxString::Format(_T("<duration>%.0f"), rTime);
	sSource += _T("</duration>\n");

	//close tag
	nIndent--;
	sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
	if (fFwd)
		sSource += _T("</forward>\n");
	else
		sSource += _T("</backup>\n");
}

wxString lmVStaff::SourceXML(int nIndent)
{
	wxString sSource = _T("");

    //iterate over the collection of StaffObjs, ordered by voice.
    //Measures must be processed one by one
    for (int nMeasure=1; nMeasure <= m_cStaffObjs.GetNumMeasures(); nMeasure++)
    {
        //start measure
		sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
		sSource += wxString::Format(_T("<measure number='%d'>\n"), nMeasure);
		nIndent++;

		bool fStartAttributes = true;
        int nNumVoices = m_cStaffObjs.GetNumVoicesInMeasure(nMeasure);
        int nVoice = 1;
        while (!m_cStaffObjs.IsVoiceUsedInMeasure(nVoice, nMeasure) &&
               nVoice <= lmMAX_VOICE)
        {
            nVoice++;
        }
        int nVoicesProcessed = 1;   //voice 0 is automatically processed
		lmBarline* pBL = (lmBarline*)NULL;
        bool fGoBack = false;
		float rTime = 0.0f;
        while (true)
        {
            lmSOIterator* pIT = m_cStaffObjs.CreateIterator(eTR_ByTime, nVoice);
            pIT->AdvanceToMeasure(nMeasure);
            while(!pIT->EndOfMeasure())
            {
                lmStaffObj* pSO = pIT->GetCurrent();
                //voice 0 staffobjs go with first voice if more than one voice
				if (!pSO->IsBarline())
				{
					if (nVoicesProcessed == 1)
					{
						if (!pSO->IsNoteRest() || ((lmNoteRest*)pSO)->GetVoice() == nVoice)
						{
							//check if this is a clef, time signature or key signature
							if (pSO->IsClef() || pSO->IsKeySignature() ||
								pSO->IsTimeSignature())
							{
								if (fStartAttributes)
								{
									//start <attributes> tag
									sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
									sSource += _T("<attributes>\n");
									fStartAttributes = false;
									nIndent++;
								}
							}
							else
							{
								if (!fStartAttributes)
								{
									//close <attributes> tag
									nIndent--;
									sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
									sSource += _T("</attributes>\n");
									fStartAttributes = true;
								}
							}

							//add xml source for this staffobj
							XML_AddShitTimeTagIfNeeded(sSource, nIndent, lmGO_FWD, rTime, pSO);
							sSource += pSO->SourceXML(nIndent);
							rTime = pSO->GetTimePos() + pSO->GetTimePosIncrement();
						}
					}
					else
					{
						if (pSO->IsNoteRest() && ((lmNoteRest*)pSO)->GetVoice() == nVoice)
						{
							if (!fStartAttributes)
							{
								nIndent--;
								sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
								sSource += _T("</attributes>\n");
								fStartAttributes = true;
							}
							//add xml source for this staffobj
							XML_AddShitTimeTagIfNeeded(sSource, nIndent, lmGO_FWD, rTime, pSO);
							sSource += pSO->SourceXML(nIndent);
							rTime = pSO->GetTimePos() + pSO->GetTimePosIncrement();
						}
					}
				}
				else
					pBL = (lmBarline*)pSO;

                pIT->MoveNext();
            }
            delete pIT;

            //check if more voices
            if (++nVoicesProcessed >= nNumVoices) break;
            nVoice++;
            while (!m_cStaffObjs.IsVoiceUsedInMeasure(nVoice, nMeasure) &&
                nVoice <= lmMAX_VOICE)
            {
                nVoice++;
            }
            wxASSERT(nVoice <= lmMAX_VOICE);

            //there are more voices. Add <backup> tag
            fGoBack = true;
            XML_AddShitTimeTag(sSource, nIndent, lmGO_BACK, rTime);
            rTime = 0.0f;
        }

        //if goBack added, add a goFwd to ensure that we are at end of measure
        if (fGoBack)
        {
            rTime = m_cStaffObjs.GetMeasureDuration(nMeasure) - rTime;
            XML_AddShitTimeTag(sSource, nIndent, lmGO_FWD, rTime);
        }

		//add barline, if present
		if (pBL)
			sSource += pBL->SourceXML(nIndent);

		//close measure
		nIndent--;
		sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
		sSource += _T("</measure>\n");
    }

    return sSource;
}

// the next two methods are mainly used for drawing the barlines. For that purpose it is necessary
// to know the y coordinate of the top most upper line of first staff and the bottom most lower
// line of the last staff.

//TODO: These methods must be moved to lmBoxSystem / lmBoxSlice

lmLUnits lmVStaff::GetYTop()
{
    return m_yLinTop;
}

lmLUnits lmVStaff::GetYBottom()
{
    return m_yLinBottom;
}

lmLUnits lmVStaff::GetVStaffHeight()
{
    if (m_nHeight == 0) {
        m_nHeight = m_topMargin + m_bottomMargin;
        // iterate over the collection of Staves (lmStaff Objects) to add up its
        // height and its after space
        for (int nStaff=1; nStaff <= m_nNumStaves; nStaff++)
        {
            lmStaff* pStaff = GetStaff(nStaff);
            m_nHeight += pStaff->GetHeight();
            m_nHeight += pStaff->GetAfterSpace();
        }
    }

    return m_nHeight;

}

lmBarline* lmVStaff::AddBarline(lmEBarline nType, bool fVisible)
{
    //create and save the barline
    lmBarline* pBarline = new lmBarline(nType, this, fVisible);
    m_cStaffObjs.Add(pBarline);
    return pBarline;
}

//bool lmVStaff::GetXPosFinalBarline(lmLUnits* pPos)
//{
//    // returns true if a barline is found and in this case updates content
//    // of variable pointed by pPos with the right x position of last barline
//    // This method is only used by Formatter, in order to not justify the last system
//    lmStaffObj* pSO = (lmStaffObj*) NULL;
//    lmSOIterator* pIter = m_cStaffObjs.CreateIterator(eTR_ByTime);
//    pIter->MoveLast();
//    while(true)
//    {
//        pSO = pIter->GetCurrent();
//        if (pSO->GetClass() == eSFOT_Barline) break;
//		if(pIter->StartOfList()) break;
//        pIter->MovePrev();
//    }
//    delete pIter;
//
//    //check that a barline is found. Otherwise no barlines in the score
//    if (pSO->GetClass() == eSFOT_Barline) {
//        lmShape* pShape = (lmShape*)pSO->GetShape();
//        if (!pShape) return false;
//		*pPos = pShape->GetXRight();
//        return true;
//    }
//    else
//        return false;
//
//}

lmBarline* lmVStaff::GetBarlineOfMeasure(int nMeasure, lmLUnits* pPos)
{
    // returns the barline for measure nMeasure (1..n) and, if found, updates content
    // of variable pointed by pPos with the X right position of this barline.
    // If no barline is found for requested measure, returns NULL and pos is not updated.

    //get the barline
    lmBarline* pBarline = m_cStaffObjs.GetBarlineOfMeasure(nMeasure);

    //if a barline is found update position
    if (pBarline && pPos)
    {
        lmShape* pShape = (lmShape*)pBarline->GetShape();
		*pPos = pShape->GetXRight();
    }

    return pBarline;
}

lmBarline* lmVStaff::GetBarlineOfLastNonEmptyMeasure(lmLUnits* pPos)
{
    // returns the barline for last non-empty measure. If found, updates content
    // of variable pointed by pPos with the X right position of this barline.
    // If no barline is found for requested measure, returns NULL and pOs is not updated.
    // This method is only used by Formatter, in order to not justify the last system

    //get the barline
    lmBarline* pBarline = m_cStaffObjs.GetBarlineOfLastNonEmptyMeasure();

    //if a barline is found update position
    if (pBarline)
    {
        lmShape* pShape = (lmShape*)pBarline->GetShape();
		*pPos = pShape->GetXRight();
    }

    return pBarline;
}

void lmVStaff::NewLine(lmPaper* pPaper)
{
    //move x cursor to the left and advance y cursor the space
    //height of all staves of this lmVStaff
    pPaper->SetCursor( m_pScore->GetPageLeftMargin(),
                       pPaper->GetCursorY() + GetVStaffHeight() );
}

//void lmVStaff::AddPrologShapes(lmBoxSliceVStaff* pBSV, int nMeasure, bool fDrawTimekey,
//							   lmPaper* pPaper)
//{
//    // The prolog (clef and key signature) must be rendered on each system,
//    // but the matching StaffObjs only exist in the first system. Therefore, in the
//    // normal staffobj rendering process, the prolog would be rendered only in
//    // the first system.
//    // So, for the other systems it is necessary to force the rendering
//    // of the prolog because there are no StaffObjs representing it.
//    // This method does it.
//    //
//    // To know what clef, key and time signature to draw we take this information from the
//    // context associated to first note of the measure on each staff. If there are no notes,
//    // the context is taken from the barline. If, finally, no context is found, no prolog
//    // is drawn.
//
//    lmLUnits nPrologWidth = 0;
//    lmClef* pClef = (lmClef*)NULL;
//    lmEClefType nClef = lmE_Undefined;
//    lmKeySignature* pKey = (lmKeySignature*)NULL;
//    lmTimeSignature* pTime = (lmTimeSignature*)NULL;
//
//    //AWARE when this method is invoked the paper position must be at the left marging,
//    //at the start of a new system.
//    lmLUnits xStartPos = pPaper->GetCursorX() + m_nSpaceBeforeClef;         //Save x to align all clefs
//    lmLUnits yStartPos = pPaper->GetCursorY();
//
//    //iterate over the collection of lmStaff objects to draw current clef and key signature
//
//    wxStaffListNode* pNode = m_cStaves.GetFirst();
//    lmStaff* pStaff = (lmStaff*)NULL;
//    lmLUnits yOffset = m_topMargin;
//    lmLUnits xPos=0;
//
//    lmContext* pContext = (lmContext*)NULL;
//    lmStaffObj* pSO = (lmStaffObj*) NULL;
//    lmNoteRest* pNR = (lmNoteRest*)NULL;
//    lmNote* pNote = (lmNote*)NULL;
//    for (int nStaff=1; pNode; pNode = pNode->GetNext(), nStaff++)
//    {
//        pStaff = (lmStaff *)pNode->GetData();
//        xPos = xStartPos;
//
//        //locate first context for this staff
//        pContext = (lmContext*)NULL;
//        lmSOIterator* pIter = m_cStaffObjs.CreateIterator(eTR_ByTime);
//        pIter->AdvanceToMeasure(nMeasure);
//        while(!pIter->EndOfList())
//		{
//            pSO = pIter->GetCurrent();
//            if (pSO->GetClass() == eSFOT_NoteRest) {
//                pNR = (lmNoteRest*)pSO;
//                if (!pNR->IsRest() && pNR->GetStaffNum() == nStaff)
//				{
//                    //OK. Note fount. Take context
//                    pNote = (lmNote*)pSO;
//                    pContext = pNote->GetCurrentContext();
//                    break;
//                }
//            }
//            else if (pSO->GetClass() == eSFOT_Barline)
//			{
//				//end of measure reached. Take content
//                lmBarline* pBar = (lmBarline*)pSO;
//                pContext = pBar->GetContext(nStaff);
//                break;
//            }
//            pIter->MoveNext();
//        }
//        delete pIter;
//
//        if (pContext) {
//            pClef = pContext->GetClef();
//            pKey = pContext->GetKey();
//            pTime = pContext->GetTime();
//
//            //render clef
//            if (pClef) {
//                nClef = pClef->GetClefType();
//				if (pClef->IsVisible()) {
//					lmUPoint uPos = lmUPoint(xPos, yStartPos+yOffset);        //absolute position
//					lmShape* pShape = pClef->AddShape(pBSV, pPaper, uPos);
//					xPos += pShape->GetWidth();
//				}
//            }
//
//            //render key signature
//            if (pKey && pKey->IsVisible()) {
//                wxASSERT(nClef != lmE_Undefined);
//                lmUPoint uPos = lmUPoint(xPos, yStartPos+yOffset);        //absolute position
//                lmShape* pShape = pKey->AddShape(pBSV, pPaper, uPos, nClef, nStaff);
//                xPos += pShape->GetWidth();
//            }
//
//        }
//
//        //compute prolog width
//        nPrologWidth = wxMax(nPrologWidth, xPos - xStartPos);
//
//        //compute vertical displacement for next staff
//        yOffset += pStaff->GetHeight();
//        yOffset += pStaff->GetAfterSpace();
//
//    }
//
//    // update paper cursor position
//    pPaper->SetCursorX(xStartPos + nPrologWidth);
//
//}

lmSoundManager* lmVStaff::ComputeMidiEvents(int nChannel)
{
    //nChannel is the MIDI channel to use for all events of this lmVStaff.
    //Returns the lmSoundManager object. It is not retained by the lmVStaff, so 
    //it is caller responsibility to delete it when no longer needed.


    //TODO review this commented code
//    Dim nMetrica As lmETimeSignature, nDurCompas As Long, nTiempoIni As Long
//
//    nMetrica = this.MetricaInicial
//    nDurCompas = GetDuracionMetrica(nMetrica)
//    nTiempoIni = nDurCompas - this.DuracionCompas(1)
//
//    wxASSERT(nTiempoIni >= 0        //El compas tiene más notas de las que caben
//
//    //Si el primer compas no es anacrusa, retrasa todo un compas para que
//    //se marque un compas completo antes de comenzar
//    if (nTiempoIni = 0 { nTiempoIni = nDurCompas

    //Create lmSoundManager and initialize MIDI events table
    lmSoundManager* pSM = new lmSoundManager(m_pScore);
    //TODO review next line
//    pSM->Inicializar GetStaffsCompas(nMetrica), nTiempoIni, nDurCompas, this.NumCompases

    //iterate over the collection to create the MIDI events
    float rMeasureStartTime = 0;
    int nMeasure = 1;        //to count measures (1 based, normal musicians way)

    //iterate over the collection to create the MIDI events
    lmStaffObj* pSO;
    lmNoteRest* pNR;
    lmTimeSignature* pTS;
    lmSOIterator* pIter = m_cStaffObjs.CreateIterator(eTR_ByTime);
    while(!pIter->EndOfList())
    {
        pSO = pIter->GetCurrent();
        if (pSO->GetClass() == eSFOT_NoteRest)
        {
            pNR = (lmNoteRest*)pSO;
            pNR->AddMidiEvents(pSM, rMeasureStartTime, nChannel, nMeasure);
        }
        else if (pSO->GetClass() == eSFOT_Barline)
        {
            rMeasureStartTime += pSO->GetTimePos();        //add measure duration
            nMeasure++;
        }
        else if (pSO->GetClass() == eSFOT_TimeSignature)
        {
            //add a RhythmChange event to set up tempo (num beats, duration of a beat)
            pTS = (lmTimeSignature*)pSO;
            pTS->AddMidiEvent(pSM, rMeasureStartTime, nMeasure);
        }
        pIter->MoveNext();
    }
    delete pIter;

    return pSM;
}

lmNote* lmVStaff::FindPossibleStartOfTie(lmNote* pEndNote, bool fNotAdded)
{
    // This method explores backwards to try to find a note ("the candidate note") that
    // can be tied (as start of tie) with pEndNote.
    // Flag 'fNotAdded' signals than the end note is not yet included in the StaffObjs
    // collection. Therefore, in this case search must start from the end
    //
    // Algorithm:
    // Find the first previous note of the same pitch and voice, and verify that 
    // distance (in timepos) is equal to candidate note duration.
    // The search will fail as soon as we find a rest or a note with different pitch.

    //get target pitch and voice
    lmAPitch anPitch = pEndNote->GetAPitch();
    int nVoice = pEndNote->GetVoice();

    //define a backwards iterator
    lmSOIterator* pIter;
    if (fNotAdded) 
    {
        pIter = m_cStaffObjs.CreateIterator(eTR_ByTime);
        pIter->MoveLast();
    }
    else
    {
        pIter = m_cStaffObjs.CreateIteratorTo(eTR_ByTime, pEndNote);
        pIter->MovePrev();
    }

    //do search
    while(!pIter->EndOfList() && ! pIter->StartOfList())
    {
        lmStaffObj* pSO = pIter->GetCurrent();
        if (pSO->IsNoteRest() && ((lmNoteRest*)pSO)->GetVoice() == nVoice)
        {
            if (((lmNoteRest*)pSO)->IsNote())
            {
                if (((lmNote*)pSO)->CanBeTied(anPitch))
                {
                    delete pIter;
                    return (lmNote*)pSO;    // candidate found
                }
                else
                {
                    // a note in the same voice with different pitch found. Imposible to tie
                    delete pIter;
                    return (lmNote*)NULL;   // no suitable note found
                }
            }
            else
            {
                // a rest in the same voice found. Imposible to tie
                delete pIter;
                return (lmNote*)NULL;   // no suitable note found
            }
        }

		if(pIter->StartOfList()) break;
        pIter->MovePrev();
    }
    delete pIter;
    return (lmNote*)NULL;        //no suitable note found
}

lmNote* lmVStaff::FindPossibleEndOfTie(lmNote* pStartNote)
{
    // This method explores forwards to try to find a note ("the candidate note") that
    // can be tied (as end of tie) with pStartNote.
    //
    // Algorithm:
    // Find the first comming note of the same pitch and voice, and verify that 
    // distance (in timepos) is equal to start note duration.
    // The search will fail as soon as we find a rest or a note with different pitch.

    //get target pitch and voice
    lmAPitch anPitch = pStartNote->GetAPitch();
    int nVoice = pStartNote->GetVoice();

    //define a forwards iterator
    lmSOIterator* pIter = m_cStaffObjs.CreateIteratorTo(eTR_ByTime, pStartNote);
    if (!pIter->EndOfList())
        pIter->MoveNext();

    //do search
    while(!pIter->EndOfList())
    {
        lmStaffObj* pSO = pIter->GetCurrent();
        if (pSO->IsNoteRest() && ((lmNoteRest*)pSO)->GetVoice() == nVoice)
        {
            if (((lmNoteRest*)pSO)->IsNote())
            {
                if (((lmNote*)pSO)->CanBeTied(anPitch))
                {
                    delete pIter;
                    return (lmNote*)pSO;    // candidate found
                }
                else
                {
                    // a note in the same voice with different pitch found. Imposible to tie
                    delete pIter;
                    return (lmNote*)NULL;   // no suitable note found
                }
            }
            else
            {
                // a rest in the same voice found. Imposible to tie
                delete pIter;
                return (lmNote*)NULL;   // no suitable note found
            }
        }

        pIter->MoveNext();
    }
    delete pIter;
    return (lmNote*)NULL;        //no suitable note found
}

bool lmVStaff::CheckIfNotesAffectedByClef()
{
    //AWARE:
    //  This method is used when a clef is going to be inserted, to verify if the new
    //  clef affects any subsequent note.
    //  
    //  This method returns true if, starting from current position, no note
    //  is found or a clef is found before finding a note.


    //define iterator from current cursor position
    lmSOIterator* pIter = m_cStaffObjs.CreateIteratorFrom(eTR_ByTime, &m_VCursor);
    while(!pIter->EndOfList())
    {
        lmStaffObj* pSO = pIter->GetCurrent();
        if (pSO->IsClef())
            break;              //clef found before finding a note. No notes affected
        else if (pSO->IsNoteRest())
        {
            if ( ((lmNoteRest*)pSO)->IsNote() )
            {
                //note found
                delete pIter;
                return true;
            }
        }
        pIter->MoveNext();
    }

    //clef found before finding a note or no note found
    delete pIter;
    return false;
}

bool lmVStaff::ShiftTime(float rTimeShift)
{
    //Shifts the time counter.
    //Returns true if error

    return m_cStaffObjs.ShiftTime(rTimeShift);
}

float lmVStaff::GetCurrentMesureDuration()
{
    return m_cStaffObjs.GetMeasureDuration(m_VCursor.GetSegment()+1);
}

lmSOControl* lmVStaff::AddNewSystem()
{
    //Inserts a control lmStaffObj to signal a new system

    //Insert the control object
    lmSOControl* pControl = new lmSOControl(lmNEW_SYSTEM, this);
    m_cStaffObjs.Add(pControl);
    return pControl;
}

lmSOIterator* lmVStaff::CreateIterator(ETraversingOrder nOrder)
{
    return m_cStaffObjs.CreateIterator(nOrder);
}

