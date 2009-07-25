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

// class lmBeam
//    lmBeam objects are auxiliary objects within lmNote objects to contain the information and
//    methods related to beaming: i.e. grouping the beams of several consecutive notes.
//
//    A beamed group can contain rests, but can not start or end with a rest.

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "Beam.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <list>
#include <vector>
#include <algorithm>

#include "Score.h"
#include "../graphic/ShapeBeam.h"
#include "../graphic/Shapes.h"
#include "../graphic/ShapeNote.h"

lmBeam::lmBeam(lmNote* pNote, long nID)
    : lmMultiRelObj(pNote, nID, eAXOT_Beam, lmNO_DRAGGABLE)
{
	m_pBeamShape = (lmShapeBeam*)NULL;
    m_fNeedsSetUp = true;
    Include(pNote);
}

lmBeam::~lmBeam()
{
}

void lmBeam::AddNoteAndStem(lmShapeStem* pStemShape, lmShapeNote* pNoteShape, lmTBeamInfo* pBeamInfo)
{
	m_pBeamShape->AddNoteRest(pStemShape, pNoteShape, pBeamInfo);

	//attach the beam to the note
	pNoteShape->Attach(m_pBeamShape);
}

void lmBeam::AddRestShape(lmShape* pRestShape)
{
	m_pBeamShape->AddNoteRest((lmShapeStem*)NULL, pRestShape, (lmTBeamInfo*)NULL);

	//attach the beam to the rest
	pRestShape->Attach(m_pBeamShape);
}

void lmBeam::CreateShape()
{
	// This method is invoked during the layout phase, from the first note of a beamed
	// group.
	// We create the beam shape, so that stem shapes can be added by the notes.
	// Also, we precompute some rendering information: stems length, m_fStemsDown
	// and position for rests included in the beamed group (m_nPosForRests).
	//
	// In chords, if the stem goes up only the highest pitch note should be used for computations.
	// But if the stem goes down it must be the lowest pitch note.
	// Be aware that for chords only the base note is included in the beam.


	// BUG_BYPASS:  -------------------------------------------------------------------
	// There is a bug in Composer5 and it some times generate scores
    // ending with a start of group. As this start is in the last note of the score,
    // the group has only one note.
    //
    if (m_NoteRests.size() == 1) {
        wxLogMessage(_T("*** ERROR *** Beam with just one note!"));
        return;
    }
    // End of BUG_BYPASS   ------------------------------------------------------------

    //create the beam container shape
    m_pBeamShape = new lmShapeBeam(m_NoteRests.front());

    // look for the highest and lowest pitch notes so that we can properly position posible
    // rests along the group
    // I am going to place the rest in the average position of all noteheads.
    int nMaxPosOnStaff = 0;
    int nMinPosOnStaff = 99999;
    m_nPosForRests = 0;
    int nNumNotes = 0;
    int i;
    std::list<lmNoteRest*>::iterator it;
    for(i=0, it=m_NoteRests.begin(); it != m_NoteRests.end(); ++it, i++)
	{
        if ((*it)->IsNote())      //ignore rests
        {
		    lmNote* pNote = (lmNote*)(*it);
            if (pNote->IsInChord()) {
                //Is in chord. So this is the base note
                nMaxPosOnStaff = wxMax(nMaxPosOnStaff, ((pNote->GetChord())->GetMaxNote())->GetPosOnStaff());
                nMinPosOnStaff = wxMin(nMinPosOnStaff, ((pNote->GetChord())->GetMinNote())->GetPosOnStaff());
            } else {
                //is not part of a chord
                nMaxPosOnStaff = wxMax(nMaxPosOnStaff, pNote->GetPosOnStaff());
                nMinPosOnStaff = wxMin(nMinPosOnStaff, pNote->GetPosOnStaff());
            }
            m_nPosForRests += pNote->GetPosOnStaff();
            nNumNotes++;
            //wxLogMessage(_T("[Beam::CreateShape] NotePos = %d"), pNote->GetPosOnStaff());
        }
    }
    if (nMinPosOnStaff == 99999) nMinPosOnStaff = 0;

    // Now lets compute the average
    m_nPosForRests = (m_nPosForRests * 5) / nNumNotes;
    // Here m_nPosForRests is the position (line/space) in which to place the rest.
    // We have computed the average noteheads position (m_nPosForRests / nNumNotes) to
    // get the following values
    //        0 - on first ledger line (C note in G clef)
    //        1 - on next space (D in G clef)
    //        2 - on first line (E not in G clef)
    //        3 - on first space
    //        4 - on second line
    //        5 - on second space
    //       etc.
    // To convert to tenths it is necesary to multiply by 10/2 = 5.

    // As rests are normally positioned on 3rd space (pos 35), the shift to apply is
    m_nPosForRests = 35 - m_nPosForRests;

    //look for the stem direction of most notes. If one note has is stem direction
    // forced (by a slur, probably) forces the group stem in this direction

    bool fStemForced = false;    // assume no stem forced
    bool fStemMixed = false;    // assume all stems in the same direction
    int nStemDown = 0;            // number of noteheads with stem down
    nNumNotes = 0;            // total number of notes
    m_fStemsDown = false;        // stems up by default

    for(it=m_NoteRests.begin(); it != m_NoteRests.end(); ++it)
	{
        if ((*it)->IsNote())      //ignore rests
        {
		    lmNote* pNote = (lmNote*)(*it);
            //count number of notes with stem down
            nNumNotes++;
            if (pNote->StemGoesDown()) nStemDown++;

            if (pNote->GetStemType() != lmSTEM_DEFAULT) {
                fStemForced = true;
                m_fStemsDown = pNote->StemGoesDown();
            }
        }
    }

    if (!fStemForced) {
        if (nStemDown >= (nNumNotes + 1) / 2 )
            m_fStemsDown = true;
        fStemMixed = false;
    }
    else {
        if (nStemDown !=0 && nStemDown != nNumNotes)
            fStemMixed = true;
    }

    //correct beam position (and reverse stems direction) if first note of beamed group is
    //tied to a previous note and the stems' directions are not forced
    if (!fStemForced && m_NoteRests.front()->IsNote())
    {
        lmNote* pFirst = (lmNote*)m_NoteRests.front();
        if (pFirst->IsTiedToPrev())
            m_fStemsDown = pFirst->GetTiedNotePrev()->StemGoesDown();
    }

    //the beam line position is going to be established by the first and last notes' stems.
    //therefore, if stems are not prefixed, let's update stem directions of notes,
    //so that following computations take the right stem directions
    if (!fStemForced) {
        for(it=m_NoteRests.begin(); it != m_NoteRests.end(); ++it)
	    {
            if ((*it)->IsNote()) {
                ((lmNote*)(*it))->SetStemDirection(m_fStemsDown);
            }
        }
    }

}

lmLUnits lmBeam::LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos,
                              wxColour color)
{
    // This method is only called from lmNote::LayoutObject(), in particular from the last
    // note of a group of beamed notes. The purpose of this method is to add the beam shape
	// to graphical model

	//BUG_BYPASS: ------------------------------------------------------------------
	// There is a bug in Composer5 and it some times generate scores
    // ending with a start of group. As this start is in the last note of the score,
    // the group has only one note.
    //
    if (m_NoteRests.size() == 1) {
        wxLogMessage(_T("*** ERROR *** Group with just one note!"));
        return 0;
    }
    // End of BUG_BYPASS -----------------------------------------------------------

    //add the beam shape to graphical model
    m_pBeamShape->SetStemsDown(m_fStemsDown);
    pBox->AddShape(m_pBeamShape, m_NoteRests.front()->GetLayer());

	return m_pBeamShape->GetWidth();
}

void lmBeam::OnRelationshipModified()
{
    //mark that it is necessary to invoke AutoSetUp() when
    //rendering the first note in the beam.

    m_fNeedsSetUp = true;
}

void lmBeam::AutoSetUp()
{
    //This method determines the beam type for each note, based on time signature and note
    //types.

    m_fNeedsSetUp = false;
	if (NumNotes() < 2)
        return;

    //Filter out any rest in the beam
	std::vector<lmNote*> cNotes;
    std::list<lmNoteRest*>::iterator it;
    for (it = m_NoteRests.begin(); it != m_NoteRests.end(); ++it)
    {
        if ((*it)->IsNote())
        {
            //it is a note. Add to notes collection
            cNotes.push_back((lmNote*)(*it));
        }
    }

    //define pointers to current note in process, and previous and next notes
    lmNote* pPrevNote = (lmNote*)NULL;
    lmNote* pCurNote = cNotes[0];
    lmNote* pNextNote = cNotes[1];

    //Let's determine the maximum beam level for this first notes triad
    int nLevelPrev = -1;
    int nLevelCur = GetBeamingLevel(pCurNote);
    int nLevelNext = GetBeamingLevel(pNextNote);

    //determine current note position in beam: first, middle or end note
    enum {
        lmFirstNote = 0,
        lmMiddleNote = 1,
        lmLastNote = 2
    };
    int nNotePos = lmFirstNote;

    //loop to assign beam information to current note
    int iN=0;
    while (iN < (int)cNotes.size())
    {
        //At this point, current note is classified as First, Middle or Last note in beam.
        //Also, pointers not Prev, Cur and Next notes are set, as well as the maximum beam
        //level for current notes triad

        //Now compute beam types for each level
        for (int iL=0; iL < 6; iL++)
        {
            if (iL > nLevelCur)
                pCurNote->SetBeamType(iL, eBeamNone);

            else if (nNotePos == lmFirstNote)
            {
                //a) Case First note:
	            // 2.1) CurLevel > Level(i+1)   -->		Forward hook
	            // 2.2) other cases             -->		Begin

                if (iL > nLevelNext)
                    pCurNote->SetBeamType(iL, eBeamForward);    //2.1
                else
                    pCurNote->SetBeamType(iL, eBeamBegin);      //2.2
            }

            else if (nNotePos == lmMiddleNote)
            {
                //b) Case Intermediate note:
	            //   2.1) CurLevel < Level(i)
	            //     2.1a) CurLevel > Level(i+1)		-->		End
	            //     2.1b) else						-->		Continue
                //
	            //   2.2) CurLevel > Level(i-1)
		        //     2.2a) CurLevel > Level(i+1)		-->		Hook (fwd or bwd, depending on beat)
		        //     2.2b) else						-->		Begin
                //
	            //   2.3) else [CurLevel <= Level(i-1)]
		        //     2.3a) CurLevel > Level(i+1)		-->		End
		        //     2.3b) else						-->		Continue

                if (iL > nLevelCur)     //2.1) CurLevel < Level(i)
                {
                    if (iL < nLevelNext)
                        pCurNote->SetBeamType(iL, eBeamEnd);        //2.1a
                    else
                        pCurNote->SetBeamType(iL, eBeamContinue);   //2.1b
                }
                else if (iL > nLevelPrev)       //2.2) CurLevel > Level(i-1)
                {
                    if (iL > nLevelNext)        //2.2a
                    {
                        //hook. Backward/Forward, depends on position in beat or on values
                        //of previous beams
                        int i;
                        for (i=0; i < iL; i++)
                        {
                            if (pCurNote->GetBeamType(i) == eBeamBegin ||
                                pCurNote->GetBeamType(i) == eBeamForward)
                            {
                                pCurNote->SetBeamType(iL, eBeamForward);
                                break;
                            }
                            else if (pCurNote->GetBeamType(i) == eBeamEnd ||
                                     pCurNote->GetBeamType(i) == eBeamBackward)
                            {
                                pCurNote->SetBeamType(iL, eBeamBackward);
                                break;
                            }
                        }
                        if (i == iL)
                        {
                            //no possible to take decision based on higher level beam values
                            //Determine it based on position in beat

                            int nPos = pCurNote->GetPositionInBeat();
                            if (nPos == lmUNKNOWN_BEAT)
                                //Unknownn time signature. Cannot determine type of hook. Use backward
                                pCurNote->SetBeamType(iL, eBeamBackward);
                            else if (nPos >= 0)
                                //on-beat note
                                pCurNote->SetBeamType(iL, eBeamForward);
                            else
                                //off-beat note
                                pCurNote->SetBeamType(iL, eBeamBackward);
                        }
                    }
                    else
                        pCurNote->SetBeamType(iL, eBeamBegin);      //2.2b
                }

                else   //   2.3) else [CurLevel <= Level(i-1)]
                {
                    if (iL > nLevelNext)
                        pCurNote->SetBeamType(iL, eBeamEnd);        //2.3a
                    else
                        pCurNote->SetBeamType(iL, eBeamContinue);   //2.3b
                }
            }

            else
            {
                //c) Case Final note:
	            //   2.1) CurLevel <= Level(i-1)    -->		End
	            //   2.2) else						-->		Backward hook
                if (iL <= nLevelPrev)
                    pCurNote->SetBeamType(iL, eBeamEnd);        //2.1
                else
                    pCurNote->SetBeamType(iL, eBeamBackward);   //2.2
            }
        }

        //Curren note done. Take next note, determine its position in beam and adjust
        //pointers to notes triad
        iN++;
        if (iN == (int)cNotes.size() - 1)
        {
            //last note
            nNotePos = lmLastNote;
            pPrevNote = pCurNote;
            pCurNote = cNotes[iN];
            pNextNote = (lmNote*)NULL;
        }
        else if (iN < (int)cNotes.size())
        {
            //middle note
            nNotePos = lmMiddleNote;
            pPrevNote = pCurNote;
            pCurNote = cNotes[iN];
            pNextNote = cNotes[iN+1];
        }
        else
            break;

        //determine the maximum beam level for the new notes triad
        nLevelPrev = nLevelCur;
        nLevelCur = GetBeamingLevel(pCurNote);
        nLevelNext = (pNextNote ? GetBeamingLevel(pNextNote) : -1);
    }
}

int lmBeam::GetBeamingLevel(lmNote* pNote)
{
    switch(pNote->GetNoteType()) {
        case eEighth:
            return 0;
        case e16th:
            return 1;
        case e32th:
            return 2;
        case e64th:
            return 3;
        case e128th:
            return 4;
        case e256th:
            return 5;
        default:
            return -1; //Error: Requesting beaming a note longer than eight
    }
}

wxString lmBeam::SourceLDP_First(int nIndent, bool fUndoData, lmNoteRest* pNR)
{
    return SourceLDP(nIndent, fUndoData, pNR);
}

wxString lmBeam::SourceLDP_Middle(int nIndent, bool fUndoData, lmNoteRest* pNR)
{
    return SourceLDP(nIndent, fUndoData, pNR);
}

wxString lmBeam::SourceLDP_Last(int nIndent, bool fUndoData, lmNoteRest* pNR)
{
    return SourceLDP(nIndent, fUndoData, pNR);
}

wxString lmBeam::SourceLDP(int nIndent, bool fUndoData, lmNoteRest* pNR)
{
    wxString sSource = _T("");
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    if (fUndoData)
        sSource += wxString::Format(_T("(beam#%d %d"), GetID(), GetID() );
    else
        sSource += wxString::Format(_T("(beam %d"), GetID());

    //beam segments info
    for (int i=0; i < 6; ++i)
    {
        lmEBeamType nType = pNR->GetBeamType(i);
        if (nType == eBeamNone)
            break;
        sSource += _T(" ");
        sSource += GetLDPBeamNameFromType(nType);
    }

    sSource += _T(")\n");
    return sSource;
}

wxString lmBeam::SourceXML_First(int nIndent, lmNoteRest* pNR)
{
    return SourceXML(nIndent, pNR);
}

wxString lmBeam::SourceXML_Middle(int nIndent, lmNoteRest* pNR)
{
    return SourceXML(nIndent, pNR);
}

wxString lmBeam::SourceXML_Last(int nIndent, lmNoteRest* pNR)
{
    return SourceXML(nIndent, pNR);
}

wxString lmBeam::SourceXML(int nIndent, lmNoteRest* pNR)
{
	wxString sSource = _T("");
    for (int i=0; i < 6; ++i)
    {
        lmEBeamType nType = pNR->GetBeamType(i);
        if (nType == eBeamNone)
            break;
	    sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
        sSource += wxString::Format(_T("<beam %d beam-level=\"%d\" \""), GetID(), i+1);
        sSource += GetXMLBeamNameFromType(nType);
        sSource += _T("\">\n");
    }

    return sSource;
}


//==================================================================================
// Global functions related to beams
//==================================================================================

wxString& GetLDPBeamNameFromType(lmEBeamType nType)
{
    //AWARE: indexes in correspondence with enum lmEBeamType
    static wxString sName[] = {
        _T("none"),         //eBeamNone
        _T("begin"),        //eBeamBegin
        _T("continue"),     //eBeamContinue
        _T("end"),          //eBeamEnd
        _T("forward"),      //eBeamForward
        _T("backward"),     //eBeamBackward
    };
    
    wxASSERT(nType >= eBeamNone && nType <= eBeamBackward);
    return sName[nType];
}

wxString& GetXMLBeamNameFromType(lmEBeamType nType)
{
    //AWARE: indexes in correspondence with enum lmEBeamType
    static wxString sName[] = {
        _T("none"),             //eBeamNone
        _T("begin"),            //eBeamBegin
        _T("continue"),         //eBeamContinue
        _T("end"),              //eBeamEnd
        _T("forward hook"),     //eBeamForward
        _T("backward hook"),    //eBeamBackward
    };
    
    wxASSERT(nType >= eBeamNone && nType <= eBeamBackward);
    return sName[nType];
}

