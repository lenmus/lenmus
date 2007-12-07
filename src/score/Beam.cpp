//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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

#include "vector"
#include "Score.h"
#include "../graphic/ShapeBeam.h"
#include "../graphic/Shapes.h"
#include "../graphic/ShapeNote.h"

lmBeam::lmBeam(lmNoteRest* pNotePrev)
{
    //
    // m_pNotePrev is the note preceding the beamed group. It is necessary to have access to
    // it because if this note is tied to the first one of the group, the stems og the group
    // are forced to go in the same direction than this previous note.
    //
    m_pNotePrev = pNotePrev;

    m_pFirstNote = (lmNote*)NULL;
    m_pLastNote = (lmNote*)NULL;
	m_pBeamShape = (lmShapeBeam*)NULL;

}
lmBeam::~lmBeam()
{
    // notes will not be deleted when deleting the list, as they are part of a lmScore
    // and will be deleted there.
    //m_cNotes.clear();
}

void lmBeam::Include(lmNoteRest* pNR)
{
	m_cNotes.push_back(pNR);
    // if it is not a rest but a note, update first and last note pointers
    if (!pNR->IsRest()) {
        if (!m_pFirstNote) m_pFirstNote = (lmNote*)pNR;
        m_pLastNote = (lmNote*)pNR;
    }
}


int lmBeam::FindNote(lmNoteRest* pNR)
{
    //find a note/rest
    for (int i=0; i < (int)m_cNotes.size(); i++)
    {
        if (m_cNotes[i]->GetID() == pNR->GetID())
			return i;
	}
	return -1;
}


void lmBeam::Remove(lmNoteRest* pNR)
{
    //find note/rest to remove
	int i = FindNote(pNR);

	//if found, remove note
   if (i != -1)
		m_cNotes.erase(m_cNotes.begin()+i);

}

void lmBeam::AddNoteAndStem(lmShapeStem* pStem, lmShapeNote* pNote, lmTBeamInfo* pBeamInfo)
{
	m_pBeamShape->AddNoteRest(pStem, pNote, pBeamInfo);

	//attach the beam to the note
	pNote->Attach(m_pBeamShape);
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
    if (m_cNotes.size() == 1) {
        wxLogMessage(_T("*** ERROR *** Group with just one note!"));
        return;
    }
    // End of BUG_BYPASS   ------------------------------------------------------------

    //create the beam container shape
    m_pBeamShape = new lmShapeBeam(m_pFirstNote);

    // look for the highest and lowest pitch notes so that we can properly position posible
    // rests along the group
    // I am going to place the rest in the average position of all noteheads.
    int nMaxPosOnStaff = 0;
    int nMinPosOnStaff = 99999;
    m_nPosForRests = 0;
    int nNumNotes = 0;
    for (int i=0; i < (int)m_cNotes.size(); i++)
    {
		lmNote* pNote = (lmNote*)m_cNotes[i];
        if (!pNote->IsRest()) {     //ignore rests
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

    for (int i=0; i < (int)m_cNotes.size(); i++)
    {
		lmNote* pNote = (lmNote*)m_cNotes[i];
        if (!pNote->IsRest()) {     //ignore rests
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
    if (!fStemForced && m_pNotePrev && !m_pNotePrev->IsRest()) {
        lmNote* pN = (lmNote*)m_pNotePrev;
        if (pN->IsTiedToNext()) m_fStemsDown = pN->StemGoesDown();
    }

    //the beam line position is going to be established by the first and last notes' stems.
    //therefore, if stems are not prefixed, let's update stem directions of notes,
    //so that following computations take the right stem directions
    if (!fStemForced) {
		for (int i=0; i < (int)m_cNotes.size(); i++)
		{
			lmNoteRest* pNR = m_cNotes[i];;
            if (!pNR->IsRest()) {
                ((lmNote*)pNR)->SetStemDirection(m_fStemsDown);
            }
        }
    }

}

lmLUnits lmBeam::LayoutObject(lmBox* pBox, lmPaper* pPaper, wxColour color)
{
    // This method is only called from lmNote::LayoutObject(), in particular from the last
    // note of a group of beamed notes. The purpose of this method is to add the beam shape
	// to graphic model


	//BUG_BYPASS: ------------------------------------------------------------------
	// There is a bug in Composer5 and it some times generate scores
    // ending with a start of group. As this start is in the last note of the score,
    // the group has only one note.
    //
    if (m_cNotes.size() == 1) {
        wxLogMessage(_T("*** ERROR *** Group with just one note!"));
        return 0;
    }
    // End of BUG_BYPASS -----------------------------------------------------------

    //add the beam shape to graphic model
    m_pBeamShape->SetStemsDown(m_fStemsDown);
    pBox->AddShape(m_pBeamShape);

	return m_pBeamShape->GetWidth();

}
