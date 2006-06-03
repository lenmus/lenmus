//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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
/*! @file Beam.cpp
    @brief Implementation file for class lmBeam
    @ingroup score_kernel
*/
/*! @class lmBeam
    lmBeam objects are auxiliary objects within lmNote objects to contain the information and
    methods related to beaming: i.e. grouping the beams of several consecutive notes.

    A beamed group can contain rests, but can not start or end with a rest.

*/

/*! @todo   Beamed groups can not start nor end with a rest. I am not sure if this is
		    has been systematically enforced; I belive that there is still code to
		    support rests in the begining or in the end. This code has to be identified
		    and removed.
*/

#ifdef __GNUG__
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

#include "Score.h"

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
//!   This method is invoked during the measurement phase, from the first note of a beamed
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
    int nMaxPosOnStaff = 0;
    int nMinPosOnStaff = 99999;
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
        }
    }
    if (nMinPosOnStaff == 99999) nMinPosOnStaff = 0;
    m_nPosForRests = (nMaxPosOnStaff + nMinPosOnStaff)*5;        // average * 10 --> tenths
    
    
    //look for the stem direction of most notes. If one note has is stem direction
    // forced (by a slur, probably) forces the group stem in this direction

    bool fStemForced = false;    // assume no stem forced
    bool fStemMixed = false;    // assume all stems in the same direction
    int nStemDown = 0;            // number of noteheads with stem down
    int nNumNotes = 0;            // total number of notes            
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
    During measurement phase all computations, except final trimming of stem' lengths, is
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
    wxArrayInt yBase, yTop;
    yBase.SetCount(nNumNotes+1, 0);     // 1 based
    yTop.SetCount(nNumNotes+1, 0);      // 1 based

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
         lmLUnits dyStem = pNote->GetDefaultStemLength();
        if (pNote->IsInChord()) {
             pMinNote = (pNote->GetChord())->GetMinNote();
             pMaxNote = (pNote->GetChord())->GetMaxNote();
            if (pNote->StemGoesDown()) {
                yBase[i] = pMaxNote->GetStaffOffset() - pMaxNote->GetPitchShift();
                yTop[i] = pMinNote->GetStaffOffset() - pMinNote->GetPitchShift() + dyStem;
            } else {
                yBase[i] = pMinNote->GetStaffOffset() - pMinNote->GetPitchShift();
                yTop[i] = pMaxNote->GetStaffOffset() - pMaxNote->GetPitchShift() - dyStem;
            }
        } else {
            if (pNote->IsRest()) {
                yBase[i] = pNote->GetStaffOffset();
                yTop[i] = yBase[i];
            }
            else {
                yBase[i] = pNote->GetStaffOffset() - pNote->GetPitchShift();
                yTop[i] = yBase[i] + (pNote->StemGoesDown() ? dyStem : -dyStem);
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
    lmLUnits x1 = m_pFirstNote->GetXStem();
    lmLUnits Ax = m_pLastNote->GetXStem() - x1;
    lmNoteRest* pNR;
    int nMinStem;
    for(i=1, pNode = m_cNotes.GetFirst(); pNode; pNode=pNode->GetNext(), i++) 
    {
        pNR = (lmNoteRest*)pNode->GetData();
        if (!pNR->IsRest()) {
            pNote = (lmNote*)pNR;
            yTop[i] = yTop[1] + (Ay * (pNote->GetXStem() - x1)) / Ax;
            //save the shortest stem
            if (i==1)
                nMinStem = abs(yBase[1] - yTop[1]);
            else
                nMinStem = wxMin(nMinStem, abs(yBase[i] - yTop[i]));
        }
    }

    // If there is a note in the group out of the interval formed by the first note and the
    // last note, then stem could be too too short. For example, a group of three notes,
    // the first and the last ones D4 and the middle one G4; the beam is horizontal, nearly
    // the G4 line; so the midle notehead would be positioned just on the beam line.
    // So let's avoid these problems by adjusting the stems so that all stems have 
    // a minimum height

    lmLUnits dyStem = m_pFirstNote->GetDefaultStemLength();
    int dyMin = (2 * dyStem) / 3;
    bool fAdjust;

    // compare the shortest with this minimun required
    int yIncr;
    if (nMinStem < dyMin) {
        // a stem is smaller than dyMin. Increment all stems.
        yIncr = dyMin - nMinStem;
        fAdjust = true;
    }
    else if ( nMinStem > dyStem) {
        // all stems are greater than the standard size. Reduce them.
        yIncr = -(nMinStem - dyStem);
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
                yTop[i] += yIncr;
            } else {
                yTop[i] -= yIncr;
            }
            //wxLogMessage(_T("[lmBeam::TrimStems] after yTop[%d]=%d"), i, yTop[i]);
        }
    }

    // At this point stems' lengths are computed and adjusted.
    // Transfer the computed values to the notes
    for(i=1, pNode = m_cNotes.GetFirst(); pNode; pNode=pNode->GetNext(), i++) 
    {
        lmLUnits nLength = (yBase[i] > yTop[i] ? yBase[i] - yTop[i] : yTop[i] - yBase[i]);
        pNR = (lmNoteRest*)pNode->GetData();
        if (pNR->IsRest()) {
            ((lmRest*)pNR)->SetDisplacement(m_nPosForRests);
        }
        else {
            pNote = (lmNote*)pNR;
            pNote->SetStemLength(nLength);
        }
    }

}

void lmBeam::DrawBeamLines(wxDC* pDC, lmLUnits nThickness, lmLUnits nBeamSpacing)
{
    //
    // This method is only called from lmNote::DrawObject(), in particular from the last note
    // of a group of beamed notes. The purpose of this method is to draw all the beam lines
    // of the group.
    //

    int xStart=0, xEnd=0, yStart=0, yEnd=0; // start and end points for a segment
    int xPrev=0, yPrev=0, xCur=0, yCur=0;   // points for previous and current note
    int yShift = 0;                         // shift, to separate a beam line from the previous one
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
            xCur = pNote->GetXStem();
            yCur = ComputeYPosOfSegment(pNote, fStemDown, yShift);

            //Let's check if we have to finish a forward hook in prev. note
            if (fForwardPending) {
                //! @todo set forward hook equal to notehead width and allow for customization.
                xEnd = xPrev + (xCur-xPrev)/3;
                yEnd = yPrev + (yCur-yPrev)/3;
                DrawBeamSegment(pDC, fStemDown, xStart, yStart, xEnd, yEnd, nThickness,
                        pStartNote, pEndNote);
                fForwardPending = false;
            }

            // now we can deal with current note
            switch (pNote->GetBeamType(iLevel)) {
                case eBeamBegin:
                    //start of segment. Compute initial point
                    fStart = true;
                    xStart = xCur; 
                    yStart = yCur;
                    pStartNote = pNote;
                    break;

                case eBeamEnd:
                    // end of segment. Compute end point
                    fEnd = true;
                    xEnd = xCur;
                    yEnd = yCur;
                    pEndNote = pNote;
                    break;

                case eBeamForward:
                    // start of segment. Mark that a forward hook is pending and
                    // compute initial point
                    fForwardPending = true;
                    xStart = xCur;
                    yStart = yCur;
                    pStartNote = pNote;
                    break;

                case eBeamBackward:
                    // end of segment. compute start and end points
                    xEnd = xCur;
                    yEnd = yCur;
                    pEndNote = pNote;
                    //! @todo set backward hook equal to notehead width and allow for customization.
                    xStart = xPrev + (2*(xCur-xPrev))/3;
                    yStart = yPrev + (2*(yCur-yPrev))/3;
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
                DrawBeamSegment(pDC, fStemDown, xStart, yStart, xEnd, yEnd, nThickness,
                    pStartNote, pEndNote);
                fStart = false;
                fEnd = false;
                pStartNote = (lmNote*)NULL;
                pEndNote = (lmNote*)NULL;
            }

            // save position of current note 
            xPrev = xCur;
            yPrev = yCur;
        }

        // displace y coordinate for next beamline
        yShift += (fStemDown ? - nBeamSpacing : nBeamSpacing);
    }
            
}

int lmBeam::ComputeYPosOfSegment(lmNote* pNote, bool fStemDown, int yShift)
{
    int yPos;
    if (pNote->IsInChord()) {
        if (fStemDown) {
            lmNote* pMinNote = (pNote->GetChord())->GetMinNote();
            yPos = pMinNote->GetYStem() + pNote->GetStemLength();
        }
        else {
            lmNote* pMaxNote = (pNote->GetChord())->GetMaxNote();
            yPos = pMaxNote->GetYStem() - pNote->GetStemLength();
        }
    }
    else {
        yPos = pNote->GetFinalYStem();
    }
    yPos += yShift;

    return yPos;

}

void lmBeam::DrawBeamSegment(wxDC* pDC, bool fStemDown, int xStart, int yStart,
                             int xEnd, int yEnd, lmLUnits nThickness,
                             lmNote* pStartNote, lmNote* pEndNote)
{
    //check to see if the beam segment has to be splitted in two systems
    if (pStartNote && pEndNote) {
        wxPoint paperPosStart = pStartNote->GetOrigin();
        wxPoint paperPosEnd = pEndNote->GetOrigin();
        if (paperPosEnd.y != paperPosStart.y) {
            //if start note paperPos Y is not the same than end note paperPos Y the notes are
            //in different systems. Therefore, the beam must be splitted. Let's do it
            wxLogMessage(_T("***** BEAM SPLIT ***"));
            return; //to avoid rendering bad lines across the score. It is less noticeable
            // pPaper must be a parameter of this method. It is necessary to replace pDC
            //lmLUnits xLeft = pPaper->GetLeftMarginXPos();
            //lmLUnits xRight = pPaper->GetRightMarginXPos();
        }
    }

    int yStartIncr=0, yEndIncr=0;

    //take thickness into account
    if (fStemDown) {
        yStartIncr = yStart + nThickness;
        yEndIncr = yEnd + nThickness;
    } else {
        yStartIncr = yStart - nThickness;
        yEndIncr = yEnd - nThickness;
    }
    //draw the segment
    wxPoint points[] = {
        wxPoint(xStart, yStart),
        wxPoint(xStart, yStartIncr),
        wxPoint(xEnd, yEndIncr),
        wxPoint(xEnd, yEnd)
    };
    pDC->DrawPolygon(4, points);

    //wxLogMessage(_T("[lmBeam::DrawBeamSegment] xStart=%d, yStart=%d, xEnd=%d, yEnd=%d, nThickness=%d, yStartIncr=%d, yEndIncr=%d, fStemDown=%s"),
    //    xStart, yStart, xEnd, yEnd, nThickness, yStartIncr, yEndIncr, (fStemDown ? _T("down") : _T("up")) );

}
