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
#pragma implementation "ShapeBeam.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "GMObject.h"
#include "../score/Score.h"
#include "ShapeNote.h"
#include "ShapeBeam.h"
#include <vector>


//-------------------------------------------------------------------------------------
// Implementation of lmShapeBeam


lmShapeBeam::lmShapeBeam(lmNoteRest* pOwner, bool fStemsDown, wxColour color)
	: lmCompositeShape(pOwner, _T("Beam"), lmNO_DRAGGABLE, eGMO_ShapeBeam)
{
	m_color = color;
	m_fStemsDown = fStemsDown;

	//initializations
	m_fLayoutPending = true;
}

lmShapeBeam::~lmShapeBeam()
{
	//delete notes info
	for(int i=0; i < (int)m_cParentNotes.size(); i++)
    {
		delete m_cParentNotes[i];
    }
}

void lmShapeBeam::AddNoteRest(lmShapeStem* pStem, lmShapeNote* pNote,
							  lmTBeamInfo* pBeamInfo)
{
	//add the info
	lmParentNote* pData = new lmParentNote;
	pData->pBeamInfo = pBeamInfo;
	pData->pShape = pNote;
	pData->nStem = Add(pStem);
	m_cParentNotes.push_back(pData);
	pNote->SetBeamShape(this);
	pNote->SetStemShape(pStem);
}

lmShapeStem* lmShapeBeam::GetStem(int iParentNote)
{
	wxASSERT(iParentNote < (int)m_cParentNotes.size());

	int iShape = m_cParentNotes[iParentNote]->nStem;
	return (lmShapeStem*)GetShape(iShape);
}

int lmShapeBeam::FindNoteShape(lmShapeNote* pShape)
{
    for(int iNote=0; iNote < (int)m_cParentNotes.size(); iNote++)
    {
        if (m_cParentNotes[iNote]->pShape->GetID() == pShape->GetID())
			return iNote;
	}
	return -1;	//not found
}

void lmShapeBeam::SetStemsDown(bool fValue)
{
	m_fStemsDown = fValue;
	m_fLayoutPending = true;
}

void lmShapeBeam::OnAttachmentPointMoved(lmShape* pShape, lmEAttachType nTag,
										 lmLUnits ux, lmLUnits uy, lmEParentEvent nEvent)
{
	m_fLayoutPending = true;

	//identify note moved and move its stem
	int i = FindNoteShape((lmShapeNote*)pShape);
	wxASSERT(i != -1);
	lmShapeStem* pStem = GetStem(i);
	wxASSERT(pStem);

	pStem->Shift(ux, uy);
}

void lmShapeBeam::Render(lmPaper* pPaper, wxColour color)
{
	if (m_fLayoutPending)
	{
		m_fLayoutPending = false;
		AdjustStems();
	}

	//geometry values.
    //DOC: Beam spacing
    //
    //  according to http://www2.coloradocollege.edu/dept/mu/Musicpress/engraving.html
    //  distance between primary and secondary beams should be 1/4 space (2.5 tenths)
    //  But if I use 3 tenths (2.5 up rounding) beam spacing is practicaly
    //  invisible. In pictures displayed in the above mentioned www page, spacing
    //  is about 1/2 space, not 1/4 space. So I will use 5 tenths.
    //  So the number to put in next statement is 9:
    //      4 for beam thikness + 5 for beams spacing
    //
	//TODO: User options
	//TODO: Move to shape?
	lmLUnits uThickness = ((lmStaffObj*)m_pOwner)->TenthsToLogical(5.0);
    lmLUnits uBeamSpacing = ((lmStaffObj*)m_pOwner)->TenthsToLogical(9.0);


	lmLUnits uxStart=0, uxEnd=0, uyStart=0, uyEnd=0; // start and end points for a segment
    lmLUnits uxPrev=0, uyPrev=0, uxCur=0, uyCur=0;   // points for previous and current note
    lmLUnits uyShift = 0;                         // shift, to separate a beam line from the previous one
    lmShapeNote* pShapeNote = (lmShapeNote*)NULL;          // note being processed
    bool fStart = false, fEnd = false;      // we've got the start/end point.
    bool fForwardPending = false;           //finish a Forward hook in prev note
    lmShapeNote* pStartNote = (lmShapeNote*)NULL;      // origin and destination notes of a beam segment
	int iStartNote = -1;
    lmShapeNote* pEndNote = (lmShapeNote*)NULL;
	int iEndNote = -1;

    //TODO set BeamHookSize equal to notehead width and allow for customization.

    for (int iLevel=0; iLevel < 6; iLevel++)
	{
        fStart = false;
        fEnd = false;
        for(int iNote=0; iNote < (int)m_cParentNotes.size(); iNote++)
        {
            pShapeNote = (lmShapeNote*)m_cParentNotes[iNote]->pShape;
			lmShapeStem* pShapeStem = GetStem(iNote);
			wxASSERT(pShapeStem);

            //compute current position to optimize
            uxCur = pShapeStem->GetXLeft();
            uyCur = ComputeYPosOfSegment(pShapeStem, uyShift);

            //Let's check if we have to finish a forward hook in prev. note
            if (fForwardPending) {
                //TODO set forward hook equal to notehead width and allow for customization.
                uxEnd = uxPrev + (uxCur-uxPrev)/3;
                uyEnd = uyPrev + (uyCur-uyPrev)/3;
                DrawBeamSegment(pPaper, uxStart, uyStart, uxEnd, uyEnd, uThickness,
                        pStartNote, pEndNote, color);
                fForwardPending = false;
            }

            // now we can deal with current note 
			lmTBeamInfo tBeamInfo = *((m_cParentNotes[iNote]->pBeamInfo)+iLevel);
			lmEBeamType nType = tBeamInfo.Type;
            switch (nType) {
                case eBeamBegin:
                    //start of segment. Compute initial point
                    fStart = true;
                    uxStart = uxCur;
                    uyStart = uyCur;
                    pStartNote = pShapeNote;
					iStartNote = iNote;
                    break;

                case eBeamEnd:
                    // end of segment. Compute end point
                    fEnd = true;
                    uxEnd = uxCur;
                    uyEnd = uyCur;
                    pEndNote = pShapeNote;
					iEndNote = iNote;
                    break;

                case eBeamForward:
                    // start of segment. Mark that a forward hook is pending and
                    // compute initial point
                    fForwardPending = true;
                    uxStart = uxCur;
                    uyStart = uyCur;
                    pStartNote = pShapeNote;
					iStartNote = iNote;
                    break;

                case eBeamBackward:
                    // end of segment. compute start and end points
                    uxEnd = uxCur;
                    uyEnd = uyCur;
                    pEndNote = pShapeNote;
					iEndNote = iNote;

                    //TODO set backward hook equal to notehead width and allow for customization.
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
            if (fStart && fEnd)
			{
                //lmLUnits uStemWidth = pEndNote->GetStemThickness();
				pShapeStem = GetStem(iEndNote);
				wxASSERT(pShapeStem);
				lmLUnits uStemWidth = pShapeStem->GetXRight() - pShapeStem->GetXLeft();

                DrawBeamSegment(pPaper,  uxStart, uyStart, uxEnd + uStemWidth, uyEnd,
								uThickness, pStartNote, pEndNote, color);
                fStart = false;
                fEnd = false;
                pStartNote = (lmShapeNote*)NULL;
				iStartNote = -1;
                pEndNote = (lmShapeNote*)NULL;
				iEndNote = -1;
            }

            // save position of current note
            uxPrev = uxCur;
            uyPrev = uyCur;
        }

        // displace y coordinate for next beamline
        uyShift += (m_fStemsDown ? - uBeamSpacing : uBeamSpacing);
    }

	//render stems
	lmCompositeShape::Render(pPaper, color);

}

void lmShapeBeam::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
 //   m_uxStart += xIncr;
	//m_uyStart += yIncr;
 //   m_uxEnd += xIncr;
	//m_uyEnd += yIncr;

    ShiftBoundsAndSelRec(xIncr, yIncr);

	//if included in a composite shape update parent bounding and selection rectangles
	if (this->IsChildShape())
		((lmCompositeShape*)GetParentShape())->RecomputeBounds();
}

void lmShapeBeam::AdjustStems()
{
	// In this method the lenght of note stems in a beamed group is adjusted.
	// It is necessary to adjust stems whenever the x position of a note in the 
	// beam changes.
	
    // At this point all stems have the standard size and the stem start and end points
    // are computed (start point = nearest to notehead). In the following loop we 
	// retrieve the start and end 'y' coordinates for each stem, 
	// and we store them in the auxiliary arrays yStart and yEnd, respectively.

    int nNumNotes = (int)m_cParentNotes.size();
    std::vector<lmLUnits> yStart(nNumNotes);
    std::vector<lmLUnits> yEnd(nNumNotes);

	int n = nNumNotes -1;	// index to last element
    lmLUnits x1, xn;		// x position of first and last stems, respectively

    for(int i=0; i < nNumNotes; i++)
    {
		lmShapeNote* pShapeNote = (lmShapeNote*)m_cParentNotes[i]->pShape;
		lmShapeStem* pShapeStem = GetStem(i);

		if (pShapeStem)
		{
			//it is a note
			if (i == 0)  x1 = pShapeStem->GetXLeft();
			else if (i == n)  xn = pShapeStem->GetXLeft();

			if (pShapeNote->StemGoesDown()) {
				yStart[i] = pShapeStem->GetYTop();
				yEnd[i] = pShapeStem->GetYBottom();
			}
			else {
				yStart[i] = pShapeStem->GetYBottom();
				yEnd[i] = pShapeStem->GetYTop();
			}
		}
		else
		{
			//assume it is a rest
			//TODO
		}
    }

	// In following loop we compute each stem and update their final position. 
    // lmShapeBeam line position is established by the first and last notes' stems. Now
    // let's adjust the intermediate notes' stem lengths to end up in the beam line.
    // This is just a proportional share based on line slope:
    // If (x1,y1) and (xn,yn) are, respectively, the position of first and last notes of
    // the group, the y position of an intermediate note i can be computed as:
    //     Ay = yn-y1
    //     Ax = xn-x1
    //                Ay
    //     yi = y1 + ---- (xi-x1)
    //                Ax
    //
    // The loop is also used to look for the shortest stem

    lmLUnits Ay = yEnd[n] - yEnd[0];
    lmLUnits Ax = xn - x1;
    lmLUnits uMinStem;
    for(int i=0; i < nNumNotes; i++)
    {
		lmShapeNote* pShapeNote = (lmShapeNote*)m_cParentNotes[i]->pShape;
		lmShapeStem* pShapeStem = GetStem(i);

        if (pShapeStem)
		{
            yEnd[i] = yEnd[0] + (Ay * (pShapeStem->GetXLeft() - x1)) / Ax;
            //save the shortest stem
            if (i==0)
                uMinStem = fabs(yStart[0] - yEnd[0]);
            else
                uMinStem = wxMin(uMinStem, fabs(yStart[i] - yEnd[i]));
        }
    }

    // If there is a note in the group out of the interval formed by the first note and the
    // last note, then stem could be too short. For example, a group of three notes,
    // the first and the last ones D4 and the middle one G4; the beam is horizontal, nearly
    // the G4 line; so the midle notehead would be positioned just on the beam line.
    // So let's avoid these problems by adjusting the stems so that all stems have
    // a minimum height

    lmLUnits dyStem = ((lmNote*)m_pOwner)->GetDefaultStemLength();
    lmLUnits dyMin = (2 * dyStem) / 3;
    bool fAdjust;

    // compare the shortest with this minimun required
    lmLUnits uyIncr;
    if (uMinStem < dyMin) {
        // a stem is smaller than dyMin. Increment all stems.
        uyIncr = dyMin - uMinStem;
        fAdjust = true;
    }
  //  else if (uMinStem > dyStem) {
  //      // all stems are greater than the standard size. Reduce them.
		////AWARE. For chords the size must be measured from highest/lowest pitch note
        //// but this is not now possible as the only stem we have is the one for the
        //// chord. All others were deleted during layout phase. Moreover. this trimming
        //// is no longer necessary as stem length is properly computed in the note layout
        //// method.
  //      uyIncr = -(uMinStem - dyStem);
  //      fAdjust = true;
  //  }
    else {
        fAdjust = false;
    }
    //wxLogMessage(_T("[lmShapeBeam::AdjustStems] dyMin=%d, nMinStem=%d, dyStem=%d, yIncr=%d"),
    //    dyMin, nMinStem, dyStem, yIncr);

    if (fAdjust) {
        for (i = 0; i < nNumNotes; i++) {
            //wxLogMessage(_T("[lmShapeBeam::AdjustStems] before yEnd[%d]=%d"), i, yEnd[i]);
           if (yStart[i] < yEnd[i]) {
                yEnd[i] += uyIncr;
            } else {
                yEnd[i] -= uyIncr;
            }
            //wxLogMessage(_T("[lmShapeBeam::AdjustStems] after yEnd[%d]=%d"), i, yEnd[i]);
        }
    }

    // At this point stems' lengths are computed and adjusted.
    // Transfer the computed values to the note shapes
    for(int i=0; i < nNumNotes; i++)
    {
		lmShapeNote* pShapeNote = (lmShapeNote*)m_cParentNotes[i]->pShape;
		lmShapeStem* pShapeStem = GetStem(i);

        lmLUnits uLength = fabs(yEnd[i] - yStart[i]);
		//TODO: treatment for rests
        if (pShapeStem) {
			SetStemLength(pShapeStem, uLength);
        }
        else {
        //    ((lmRest*)pNR)->DoVerticalShift(m_nPosForRests);
        }
    }
	RecomputeBounds();

}

void lmShapeBeam::SetStemLength(lmShapeStem* pStem, lmLUnits uLength)
{
	if (pStem->StemDown())
	{
		//adjust bottom point
		pStem->SetLength(uLength, false);
	}
	else
	{
		//adjust top point
		pStem->SetLength(uLength, true);
	}
}

void lmShapeBeam::DrawBeamSegment(lmPaper* pPaper, 
                             lmLUnits uxStart, lmLUnits uyStart,
                             lmLUnits uxEnd, lmLUnits uyEnd, lmLUnits uThickness,
                             lmShapeNote* pStartNote, lmShapeNote* pEndNote,
                             wxColour color)
{
    //check to see if the beam segment has to be splitted in two systems
    //if (pStartNote && pEndNote) {
    //    lmUPoint paperPosStart = pStartNote->GetReferencePaperPos();
    //    lmUPoint paperPosEnd = pEndNote->GetReferencePaperPos();
    //    if (paperPosEnd.y != paperPosStart.y) {
    //        //if start note paperPos Y is not the same than end note paperPos Y the notes are
    //        //in different systems. Therefore, the beam must be splitted. Let's do it
    //        wxLogMessage(_T("***** BEAM SPLIT ***"));
    //        //TODO
    //        //lmLUnits xLeft = pPaper->GetLeftMarginXPos();
    //        //lmLUnits xRight = pPaper->GetRightMarginXPos();
    //        return; //to avoid rendering bad lines across the score. It is less noticeable
    //    }
    //}

    //draw the segment
    pPaper->SolidLine(uxStart, uyStart, uxEnd, uyEnd, uThickness, eEdgeVertical, color);

    //wxLogMessage(_T("[lmShapeBeam::DrawBeamSegment] uxStart=%d, uyStart=%d, uxEnd=%d, uyEnd=%d, uThickness=%d, yStartIncr=%d, yEndIncr=%d, m_fStemsDown=%s"),
    //    uxStart, uyStart, uxEnd, uyEnd, uThickness, yStartIncr, yEndIncr, (m_fStemsDown ? _T("down") : _T("up")) );

}

lmLUnits lmShapeBeam::ComputeYPosOfSegment(lmShapeStem* pShapeStem, lmLUnits uyShift)
{
    return pShapeStem->GetYEndStem() + uyShift;

 //   lmLUnits uyPos;
 //   lmNote* pNote = (lmNote*)m_pOwner;
	//if ( pNote->IsInChord() )
	//{
	//    if (m_fStemsDown)
	//	{
 //           //PROBLEM: During layout only the stem in base note exists. All other stem shapes
 //           //are deleted.
 //           //lmNote* pMinNote = (pNote->GetChord())->GetMinNote();
 //           //uyPos = pMinNote->GetYStartStem() + pNote->GetStemLength();
 //           lmNote* pMinNote = (pNote->GetChord())->GetMinNote();
 //           lmShapeStem* pStem = ((lmShapeNote*)pMinNote->GetShap2())->GetStem();
 //           uyPos = pStem->GetYEndStem();
 //       }
 //       else
	//	{
 //   //        lmNote* pMaxNote = (pNote->GetChord())->GetMaxNote();
 //   //        uyPos = pMaxNote->GetYStartStem() - pNote->GetStemLength();
 //           //wxLogMessage(_T("[lmShapeBeam::ComputeYPosOfSegment] uyPos=%.2f, yStem=%.2f, stemLength=%.2f"),
 //           //    uyPos, pMaxNote->GetYStartStem(), pNote->GetStemLength());
 //           lmNote* pMaxNote = (pNote->GetChord())->GetMaxNote();
 //           lmShapeStem* pStem = ((lmShapeNote*)pMaxNote->GetShap2())->GetStem();
 //           uyPos = pStem->GetYEndStem();
 //       }
 //   }
 //   else 
	//{
	//	wxASSERT(pShapeStem);
	//	if (m_fStemsDown)
	//		uyPos = pShapeStem->GetYBottom();
	//	else
	//		uyPos = pShapeStem->GetYTop();
 //   }
 //   uyPos += uyShift;

 //   return uyPos;

}

