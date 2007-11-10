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

// @class lmBeam
//    lmBeam objects are auxiliary objects within lmNote objects to contain the information and
//    methods related to beaming: i.e. grouping the beams of several consecutive notes.
//
//    A beamed group can contain rests, but can not start or end with a rest.

// @todo   Beamed groups can not start nor end with a rest. I am not sure if this is
//		    has been systematically enforced; I belive that there is still code to
//		    support rests in the begining or in the end. This code has to be identified
//		    and removed.

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

}
lmBeam::~lmBeam()
{
    // notes will not be deleted when deleting the list, as they are part of a lmScore
    // and will be deleted there.
    m_cNotes.DeleteContents(false);
    m_cNotes.Clear();
}

void lmBeam::Include(lmNoteRest* pNR)
{
    m_cNotes.Append(pNR);
    // if it is not a rest but a note, update first and last note pointers
    if (!pNR->IsRest()) {
        if (!m_pFirstNote) m_pFirstNote = (lmNote*)pNR;
        m_pLastNote = (lmNote*)pNR;
    }
}

void lmBeam::Remove(lmNoteRest* pNR)
{
    m_cNotes.DeleteObject(pNR);
}

int lmBeam::NumNotes()
{
    //return the number of Notes that there are in the beam
    return m_cNotes.GetCount();
}

//!
//!   This method is invoked during the layout phase, from the first note of a beamed
//!   group. Here we precompute some rendering information: stems length, m_fStemsDown
//!   and position for rests included in the beamed group (m_nPosForRests).
//!
//!   In chords, if the stem goes up only the highest pitch note should be used for computations.
//!   But if the stem goes down it must be the lowest pitch note.
//!   Be aware that for chords only the base note is included in the beam.
//!
//!   The exact length of stems can not be computed here because it is not yet known the
//!   exact x positions of noteheads. So this final trimming is delayed to drawing phase and
//!   is performed in method TrimStems()
//!
void lmBeam::ComputeStemsDirection()
{
    //  BUG_BYPASS: There is a bug in Composer5 and it some times generate scores
    //    ending with a start of group. As this start is in the last note of the score,
    //    the group has only one note.
    //
    if (m_cNotes.GetCount()==1) {
        wxLogMessage(_T("*** ERROR *** Group with just one note!"));
        return;
    }
    // End of BUG_BYPASS

    lmNote* pNote;
    wxNoteRestsListNode *pNode;

    // look for the highest and lowest pitch notes so that we can properly position posible
    // rests along the group
    // I am going to place the rest in the average position of all noteheads.
    int nMaxPosOnStaff = 0;
    int nMinPosOnStaff = 99999;
    m_nPosForRests = 0;
    int nNumNotes = 0;
    for(pNode = m_cNotes.GetFirst(); pNode; pNode=pNode->GetNext())
    {
        pNote = (lmNote*)pNode->GetData();
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
            //wxLogMessage(_T("[Beam::ComputeStemsDirection] NotePos = %d"), pNote->GetPosOnStaff());
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

    for(pNode = m_cNotes.GetFirst(); pNode; pNode=pNode->GetNext())
    {
        pNote = (lmNote*)pNode->GetData();
        if (!pNote->IsRest()) {     //ignore rests
            //count number of notes with stem down
            nNumNotes++;
            if (pNote->StemGoesDown()) nStemDown++;

            if (pNote->GetStemType() != eDefaultStem) {
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
        lmNoteRest* pNR;
        for(pNode = m_cNotes.GetFirst(); pNode; pNode=pNode->GetNext()) {
            pNR = (lmNoteRest*)pNode->GetData();
            if (!pNR->IsRest()) {
                pNote = (lmNote*)pNR;
                pNote->SetStemDirection(m_fStemsDown);
            }
        }
    }

}

/*! In this method the lenght of note stems in a beamed group is adjusted.
    During layout phase all computations, except final trimming of stem' lengths, is
    done. Final trimming of stems' length is delayed to MakeUp phase because it is not
    posible to adjust lentghts until the x position of the notes is finally established, and
    this takes place AFTER the measurement phase, once the lines are justified.
*/
void lmBeam::TrimStems()
{
    //  BUG_BYPASS: There is a bug in Composer5 and it some times generate scores
    //    ending with a start of group. As this start is in the last note of the score,
    //    the group has only one note.
    //
    if (m_cNotes.GetCount()==1) {
        wxLogMessage(_T("*** ERROR *** Group with just one note!"));
        return;
    }
    // End of BUG_BYPASS


    // At this point all stems have the standard size and the stem start point (the point
    // nearest to the notehead) is computed. Now we are goin to compute the end point
    // for each stem.
    // As we are interested in the stems' length, not in the exact coordinates, instead
    // of using the real start coordinates, we are going to compute an arbitrary start
    // coordinate relative to zero. This has the advantage, over using the real coordinates,
    // that all stems will be in the same system, and this is better when we have to split
    // the beam into two systems.
    // The computed start and end positions for each stem will be stored in the auxiliary
    // arrays yBase and yTop, respectively.

    int nNumNotes = m_cNotes.GetCount();
    std::vector<float>yBase(nNumNotes+1);   // 1 based
    std::vector<float>yTop(nNumNotes+1);    // 1 based

    wxNoteRestsListNode *pNode;
    lmNote* pMaxNote;
    lmNote* pMinNote;
    lmNote* pNote;
    int i = 1;

#if 1
    //code using an arbitrary start coordinate relative to zero
    for(pNode = m_cNotes.GetFirst(); pNode; pNode=pNode->GetNext(), i++)
    {
        pNote = (lmNote*)pNode->GetData();
         lmLUnits udyStem = pNote->GetDefaultStemLength();
        if (pNote->IsInChord()) {
             pMinNote = (pNote->GetChord())->GetMinNote();
             pMaxNote = (pNote->GetChord())->GetMaxNote();
            if (pNote->StemGoesDown()) {
                yBase[i] = pMaxNote->GetStaffOffset() - pMaxNote->GetPitchShift();
                yTop[i] = pMinNote->GetStaffOffset() - pMinNote->GetPitchShift() + udyStem;
            } else {
                yBase[i] = pMinNote->GetStaffOffset() - pMinNote->GetPitchShift();
                yTop[i] = pMaxNote->GetStaffOffset() - pMaxNote->GetPitchShift() - udyStem;
            }
        } else {
            if (pNote->IsRest()) {
                yBase[i] = pNote->GetStaffOffset();
                yTop[i] = yBase[i];
            }
            else {
                yBase[i] = pNote->GetStaffOffset() - pNote->GetPitchShift();
                yTop[i] = yBase[i] + (pNote->StemGoesDown() ? udyStem : -udyStem);
            }
        }
    }

#else
    //  code using the real start coordinate
    for(pNode = m_cNotes.GetFirst(); pNode; pNode=pNode->GetNext(), i++)
    {
        pNote = (lmNote*)pNode->GetData();
        lmLUnits dyStem = pNote->GetDefaultStemLength();
        if (pNote->IsInChord()) {
            pMinNote = (pNote->GetChord())->GetMinNote();
            pMaxNote = (pNote->GetChord())->GetMaxNote();
            if (pNote->StemGoesDown()) {
                yBase[i] = pMaxNote->GetYStem();
                yTop[i] = pMinNote->GetYStem() + dyStem;
            } else {
                yBase[i] = pMinNote->GetYStem();
                yTop[i] = pMaxNote->GetYStem() - dyStem;
            }
        } else {
            if (pNote->IsRest()) {
                yBase[i] = pNote->GetYStem();
                yTop[i] = yBase[i];
            }
            else {
                yBase[i] = pNote->GetYStem();
                yTop[i] = yBase[i] + (pNote->StemGoesDown() ? dyStem : -dyStem);
            }
        }
    }
#endif


    // lmBeam line position is established by the first and last notes' stems. Now
    // let's adjust the intermediate notes' stem lengths to end up in the beam line.
    // This is just a proportional share based on line slope:
    // If (x1,y1) and (xn,yn) are, respectively, the position of first and last notes of
    // the group, the y position of an intermediate note i can be computed as:
    //     Ay = yn-y1
    //     Ax = xn-x1
    //                Ay
    //     yi = y1 + ---- (xi-x1)
    //                Ax
    // The x position of the stem has beeen computed in Note object during the
    // measurement phase.
    //
    // The loop is also used to look for the shortest stem

    lmLUnits Ay = yTop[nNumNotes] - yTop[1];
    lmLUnits x1 = m_pFirstNote->GetXStemLeft();
    lmLUnits Ax = m_pLastNote->GetXStemLeft() - x1;
    lmNoteRest* pNR;
    lmLUnits uMinStem;
    for(i=1, pNode = m_cNotes.GetFirst(); pNode; pNode=pNode->GetNext(), i++)
    {
        pNR = (lmNoteRest*)pNode->GetData();
        if (!pNR->IsRest()) {
            pNote = (lmNote*)pNR;
            yTop[i] = yTop[1] + (Ay * (pNote->GetXStemLeft() - x1)) / Ax;
            //save the shortest stem
            if (i==1)
                uMinStem = fabs(yBase[1] - yTop[1]);
            else
                uMinStem = wxMin(uMinStem, fabs(yBase[i] - yTop[i]));
        }
    }

    // If there is a note in the group out of the interval formed by the first note and the
    // last note, then stem could be too too short. For example, a group of three notes,
    // the first and the last ones D4 and the middle one G4; the beam is horizontal, nearly
    // the G4 line; so the midle notehead would be positioned just on the beam line.
    // So let's avoid these problems by adjusting the stems so that all stems have
    // a minimum height

    lmLUnits dyStem = m_pFirstNote->GetDefaultStemLength();
    lmLUnits dyMin = (2 * dyStem) / 3;
    bool fAdjust;

    // compare the shortest with this minimun required
    lmLUnits uyIncr;
    if (uMinStem < dyMin) {
        // a stem is smaller than dyMin. Increment all stems.
        uyIncr = dyMin - uMinStem;
        fAdjust = true;
    }
    else if (uMinStem > dyStem) {
        // all stems are greater than the standard size. Reduce them.
        uyIncr = -(uMinStem - dyStem);
        fAdjust = true;
    }
    else {
        fAdjust = false;
    }
    //wxLogMessage(_T("[lmBeam::TrimStems] dyMin=%d, nMinStem=%d, dyStem=%d, yIncr=%d"),
    //    dyMin, nMinStem, dyStem, yIncr);

    if (fAdjust) {
        for (i = 1; i <= nNumNotes; i++) {
            //wxLogMessage(_T("[lmBeam::TrimStems] before yTop[%d]=%d"), i, yTop[i]);
           if (yBase[i] < yTop[i]) {
                yTop[i] += uyIncr;
            } else {
                yTop[i] -= uyIncr;
            }
            //wxLogMessage(_T("[lmBeam::TrimStems] after yTop[%d]=%d"), i, yTop[i]);
        }
    }

    // At this point stems' lengths are computed and adjusted.
    // Transfer the computed values to the notes
    for(i=1, pNode = m_cNotes.GetFirst(); pNode; pNode=pNode->GetNext(), i++)
    {
        lmLUnits uLength = (yBase[i] > yTop[i] ? yBase[i] - yTop[i] : yTop[i] - yBase[i]);
        pNR = (lmNoteRest*)pNode->GetData();
        if (pNR->IsRest()) {
            ((lmRest*)pNR)->DoVerticalShift(m_nPosForRests);
        }
        else {
            pNote = (lmNote*)pNR;
            pNote->SetStemLength(uLength);
        }
    }

}

void lmBeam::LayoutObject(lmBox* pBox, lmPaper* pPaper, wxColour color)
{
    //
    // This method is only called from lmNote::LayoutObject(), in particular from the last
    // note of a group of beamed notes. The purpose of this method is to create the shapes
    // for all the beam lines of the group, and to add them to the received composite shape.
    //

    //create the beam container shape
    lmShapeBeam* pBeamShape = new lmShapeBeam(m_pFirstNote, m_fStemsDown, NumNotes());
    pBox->AddShape(pBeamShape);

	//add the beamed notes
    wxNoteRestsListNode *pNode;
	int i = 1;
    for(pNode = m_cNotes.GetFirst(); pNode; pNode=pNode->GetNext(), i++)
    {
		lmNoteRest* pNote = pNode->GetData();
		lmShape* pNoteShape = pNote->GetShap2();
		pBeamShape->AddNoteRest(pNoteShape, pNote->GetBeamInfo());

		//attach the beam to the note
		pNoteShape->Attach(this, (i==1 ? eGMA_StartNote : 
									(i==NumNotes() ? eGMA_EndNote : eGMA_MiddleNote)));
	}


	////get staff, for scaling logical units
 //   lmVStaff* pVStaff = m_pFirstNote->GetVStaff();
 //   int nStaff = m_pFirstNote->GetStaffNum();
 //   lmStaff* pStaff = pVStaff->GetStaff(nStaff);

	////geometry values.
 //   //DOC: Beam spacing
 //   //
 //   //  according to http://www2.coloradocollege.edu/dept/mu/Musicpress/engraving.html
 //   //  distance between primary and secondary beams should be 1/4 space (2.5 tenths)
 //   //  But if I use 3 tenths (2.5 up rounding) beam spacing is practicaly
 //   //  invisible. In pictures displayed in the above mentioned www page, spacing
 //   //  is about 1/2 space, not 1/4 space. So I will use 5 tenths.
 //   //  So the number to put in next statement is 9:
 //   //      4 for beam thikness + 5 for beams spacing
 //   //
	////TODO: User options
	////TODO: Move to shape?
	//lmLUnits uThickness = pStaff->TenthsToLogical(5.0);
 //   lmLUnits uBeamSpacing = pStaff->TenthsToLogical(9.0);

 //   //create the beam container shape
 //   lmCompositeShape* pBeamShape = new lmCompositeShape(m_pFirstNote, _T("Beam"));
 //   pBox->AddShape(pBeamShape);
	//
	//lmLUnits uxStart=0, uxEnd=0, uyStart=0, uyEnd=0; // start and end points for a segment
 //   lmLUnits uxPrev=0, uyPrev=0, uxCur=0, uyCur=0;   // points for previous and current note
 //   lmLUnits uyShift = 0;                         // shift, to separate a beam line from the previous one
 //   lmNote* pNote = (lmNote*)NULL;          // note being processed
 //   bool fStemDown = false;
 //   wxNoteRestsListNode *pNode;
 //   bool fStart = false, fEnd = false;      // we've got the start/end point.
 //   bool fForwardPending = false;           //finish a Forward hook in prev note
 //   lmNote* pStartNote = (lmNote*)NULL;      // origin and destination notes of a beam segment
 //   lmNote* pEndNote = (lmNote*)NULL;

 //   //! @todo set BeamHookSize equal to notehead width and allow for customization.

 //   for (int iLevel=0; iLevel < 6; iLevel++) {
 //       fStart = false;
 //       fEnd = false;
 //       for(pNode = m_cNotes.GetFirst(); pNode; pNode=pNode->GetNext())
 //       {
 //           pNote = (lmNote*)pNode->GetData();
 //           fStemDown = pNote->StemGoesDown();

 //           //compute current position to optimize
 //           uxCur = pNote->GetXStemLeft();
 //           uyCur = ComputeYPosOfSegment(pNote, fStemDown, uyShift);

 //           //Let's check if we have to finish a forward hook in prev. note
 //           if (fForwardPending) {
 //               //! @todo set forward hook equal to notehead width and allow for customization.
 //               uxEnd = uxPrev + (uxCur-uxPrev)/3;
 //               uyEnd = uyPrev + (uyCur-uyPrev)/3;
 //               DrawBeamSegment(pPaper, fStemDown, uxStart, uyStart, uxEnd, uyEnd, uThickness,
 //                       pStartNote, pEndNote, color);
 //               fForwardPending = false;
 //           }

 //           // now we can deal with current note
 //           switch (pNote->GetBeamType(iLevel)) {
 //               case eBeamBegin:
 //                   //start of segment. Compute initial point
 //                   fStart = true;
 //                   uxStart = uxCur;
 //                   uyStart = uyCur;
 //                   pStartNote = pNote;
 //                   break;

 //               case eBeamEnd:
 //                   // end of segment. Compute end point
 //                   fEnd = true;
 //                   uxEnd = uxCur;
 //                   uyEnd = uyCur;
 //                   pEndNote = pNote;
 //                   break;

 //               case eBeamForward:
 //                   // start of segment. Mark that a forward hook is pending and
 //                   // compute initial point
 //                   fForwardPending = true;
 //                   uxStart = uxCur;
 //                   uyStart = uyCur;
 //                   pStartNote = pNote;
 //                   break;

 //               case eBeamBackward:
 //                   // end of segment. compute start and end points
 //                   uxEnd = uxCur;
 //                   uyEnd = uyCur;
 //                   pEndNote = pNote;
 //                   //! @todo set backward hook equal to notehead width and allow for customization.
 //                   uxStart = uxPrev + (2*(uxCur-uxPrev))/3;
 //                   uyStart = uyPrev + (2*(uyCur-uyPrev))/3;
 //                   fStart = true;      //mark 'segment ready to be drawn'
 //                   fEnd = true;
 //                  break;

 //               case eBeamContinue:
 //               case eBeamNone:
 //                   // nothing to do.
 //                   break;

 //               default:
 //                   wxASSERT(false);
 //           }

 //           // if we have data to draw a segment, draw it
 //           if (fStart && fEnd) {
 //               lmLUnits uStemWidth = pEndNote->GetStemThickness();
 //               //DrawBeamSegment(pPaper, fStemDown,
 //               //    uxStart, uyStart, uxEnd + uStemWidth, uyEnd, uThickness,
 //               //    pStartNote, pEndNote, color);
 //               AddBeamSegmentShape(pBeamShape, pPaper, fStemDown,
 //                   uxStart, uyStart, uxEnd + uStemWidth, uyEnd, uThickness,
 //                   pStartNote, pEndNote, color);
 //               fStart = false;
 //               fEnd = false;
 //               pStartNote = (lmNote*)NULL;
 //               pEndNote = (lmNote*)NULL;
 //           }

 //           // save position of current note
 //           uxPrev = uxCur;
 //           uyPrev = uyCur;
 //       }

 //       // displace y coordinate for next beamline
 //       uyShift += (fStemDown ? - uBeamSpacing : uBeamSpacing);
 //   }

}

void lmBeam::OnNoteMoved(lmNoteRest* pNR, lmLUnits uLeft)
{
}

void lmBeam::DrawBeamLines(lmPaper* pPaper, lmLUnits uThickness, lmLUnits uBeamSpacing,
                           wxColour color)
{
    //
    // This method is only called from lmNote::DrawObject(), in particular from the last note
    // of a group of beamed notes. The purpose of this method is to draw all the beam lines
    // of the group.
    //

    lmLUnits uxStart=0, uxEnd=0, uyStart=0, uyEnd=0; // start and end points for a segment
    lmLUnits uxPrev=0, uyPrev=0, uxCur=0, uyCur=0;   // points for previous and current note
    lmLUnits uyShift = 0;                         // shift, to separate a beam line from the previous one
    lmNote* pNote = (lmNote*)NULL;          // note being processed
    bool fStemDown = false;
    wxNoteRestsListNode *pNode;
    bool fStart = false, fEnd = false;      // we've got the start/end point.
    bool fForwardPending = false;           //finish a Forward hook in prev note
    lmNote* pStartNote = (lmNote*)NULL;      // origin and destination notes of a beam segment
    lmNote* pEndNote = (lmNote*)NULL;

    //! @todo set BeamHookSize equal to notehead width and allow for customization.

    for (int iLevel=0; iLevel < 6; iLevel++) {
        fStart = false;
        fEnd = false;
        for(pNode = m_cNotes.GetFirst(); pNode; pNode=pNode->GetNext())
        {
            pNote = (lmNote*)pNode->GetData();
            fStemDown = pNote->StemGoesDown();

            //compute current position to optimize
            uxCur = pNote->GetXStemLeft();
            uyCur = ComputeYPosOfSegment(pNote, fStemDown, uyShift);

            //Let's check if we have to finish a forward hook in prev. note
            if (fForwardPending) {
                //! @todo set forward hook equal to notehead width and allow for customization.
                uxEnd = uxPrev + (uxCur-uxPrev)/3;
                uyEnd = uyPrev + (uyCur-uyPrev)/3;
                DrawBeamSegment(pPaper, fStemDown, uxStart, uyStart, uxEnd, uyEnd, uThickness,
                        pStartNote, pEndNote, color);
                fForwardPending = false;
            }

            // now we can deal with current note
            switch (pNote->GetBeamType(iLevel)) {
                case eBeamBegin:
                    //start of segment. Compute initial point
                    fStart = true;
                    uxStart = uxCur;
                    uyStart = uyCur;
                    pStartNote = pNote;
                    break;

                case eBeamEnd:
                    // end of segment. Compute end point
                    fEnd = true;
                    uxEnd = uxCur;
                    uyEnd = uyCur;
                    pEndNote = pNote;
                    break;

                case eBeamForward:
                    // start of segment. Mark that a forward hook is pending and
                    // compute initial point
                    fForwardPending = true;
                    uxStart = uxCur;
                    uyStart = uyCur;
                    pStartNote = pNote;
                    break;

                case eBeamBackward:
                    // end of segment. compute start and end points
                    uxEnd = uxCur;
                    uyEnd = uyCur;
                    pEndNote = pNote;
                    //! @todo set backward hook equal to notehead width and allow for customization.
                    uxStart = uxPrev + (2*(uxCur-uxPrev))/3;
                    uyStart = uyPrev + (2*(uyCur-uyPrev))/3;
                    fStart = true;      //mark 'segment ready to be drawn'
                    fEnd = true;
                   break;

                case eBeamContinue:
                case eBeamNone:
                    // nothing to do.
                    break;

                default:
                    wxASSERT(false);
            }

            // if we have data to draw a segment, draw it
            if (fStart && fEnd) {
                lmLUnits uStemWidth = pEndNote->GetStemThickness();
                DrawBeamSegment(pPaper, fStemDown,
                    uxStart, uyStart, uxEnd + uStemWidth, uyEnd, uThickness,
                    pStartNote, pEndNote, color);
                fStart = false;
                fEnd = false;
                pStartNote = (lmNote*)NULL;
                pEndNote = (lmNote*)NULL;
            }

            // save position of current note
            uxPrev = uxCur;
            uyPrev = uyCur;
        }

        // displace y coordinate for next beamline
        uyShift += (fStemDown ? - uBeamSpacing : uBeamSpacing);
    }

}

lmLUnits lmBeam::ComputeYPosOfSegment(lmNote* pNote, bool fStemDown, lmLUnits uyShift)
{
    lmLUnits uyPos;
    if (pNote->IsInChord()) {
        if (fStemDown) {
            lmNote* pMinNote = (pNote->GetChord())->GetMinNote();
            uyPos = pMinNote->GetYStem() + pNote->GetStemLength();
        }
        else {
            lmNote* pMaxNote = (pNote->GetChord())->GetMaxNote();
            uyPos = pMaxNote->GetYStem() - pNote->GetStemLength();
            //wxLogMessage(_T("[lmBeam::ComputeYPosOfSegment] uyPos=%.2f, yStem=%.2f, stemLength=%.2f"),
            //    uyPos, pMaxNote->GetYStem(), pNote->GetStemLength());
        }
    }
    else {
        uyPos = pNote->GetFinalYStem();
    }
    uyPos += uyShift;

    return uyPos;

}

void lmBeam::DrawBeamSegment(lmPaper* pPaper, bool fStemDown,
                             lmLUnits uxStart, lmLUnits uyStart,
                             lmLUnits uxEnd, lmLUnits uyEnd, lmLUnits uThickness,
                             lmNote* pStartNote, lmNote* pEndNote,
                             wxColour color)
{
    //check to see if the beam segment has to be splitted in two systems
    if (pStartNote && pEndNote) {
        lmUPoint paperPosStart = pStartNote->GetOrigin();
        lmUPoint paperPosEnd = pEndNote->GetOrigin();
        if (paperPosEnd.y != paperPosStart.y) {
            //if start note paperPos Y is not the same than end note paperPos Y the notes are
            //in different systems. Therefore, the beam must be splitted. Let's do it
            wxLogMessage(_T("***** BEAM SPLIT ***"));
            //TODO
            //lmLUnits xLeft = pPaper->GetLeftMarginXPos();
            //lmLUnits xRight = pPaper->GetRightMarginXPos();
            return; //to avoid rendering bad lines across the score. It is less noticeable
        }
    }

    //draw the segment
    pPaper->SolidLine(uxStart, uyStart, uxEnd, uyEnd, uThickness, eEdgeVertical, color);

    //wxLogMessage(_T("[lmBeam::DrawBeamSegment] uxStart=%d, uyStart=%d, uxEnd=%d, uyEnd=%d, uThickness=%d, yStartIncr=%d, yEndIncr=%d, fStemDown=%s"),
    //    uxStart, uyStart, uxEnd, uyEnd, uThickness, yStartIncr, yEndIncr, (fStemDown ? _T("down") : _T("up")) );

}

void lmBeam::AddBeamSegmentShape(lmCompositeShape* pCS, lmPaper* pPaper, bool fStemDown,
                                 lmLUnits uxStart, lmLUnits uyStart,
                                 lmLUnits uxEnd, lmLUnits uyEnd, lmLUnits uThickness,
                                 lmNote* pStartNote, lmNote* pEndNote, wxColour color)
{
    //check to see if the beam segment has to be splitted in two systems
    if (pStartNote && pEndNote) {
        lmUPoint paperPosStart = pStartNote->GetOrigin();
        lmUPoint paperPosEnd = pEndNote->GetOrigin();
        if (paperPosEnd.y != paperPosStart.y) {
            //if start note paperPos Y is not the same than end note paperPos Y the notes are
            //in different systems. Therefore, the beam must be splitted. Let's do it
            wxLogMessage(_T("***** BEAM SPLIT ***"));
            //TODO
            //lmLUnits xLeft = pPaper->GetLeftMarginXPos();
            //lmLUnits xRight = pPaper->GetRightMarginXPos();
            return; //to avoid rendering bad lines across the score. It is less noticeable
        }
    }

    //create the shape for the segment
    //pPaper->SolidLine(uxStart, uyStart, uxEnd, uyEnd, uThickness, eEdgeVertical, color);
    lmShapeLin2* pLine = 
        new lmShapeLin2(pCS->Owner(), uxStart, uyStart, uxEnd, uyEnd, uThickness, 0.0,
                        color, _T("Stem"), eEdgeVertical);
	pCS->Add(pLine);

    //wxLogMessage(_T("[lmBeam::DrawBeamSegment] uxStart=%d, uyStart=%d, uxEnd=%d, uyEnd=%d, uThickness=%d, yStartIncr=%d, yEndIncr=%d, fStemDown=%s"),
    //    uxStart, uyStart, uxEnd, uyEnd, uThickness, yStartIncr, yEndIncr, (fStemDown ? _T("down") : _T("up")) );

}

