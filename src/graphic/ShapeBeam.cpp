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
#pragma implementation "ShapeBeam.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include <vector>

#include "GMObject.h"
#include "ShapeLine.h"
#include "ShapeNote.h"
#include "ShapeRest.h"
#include "ShapeBeam.h"
#include "../score/Score.h"
#include "../score/VStaff.h"
#include "../app/Preferences.h"


//-------------------------------------------------------------------------------------
// Implementation of lmShapeBeam


lmShapeBeam::lmShapeBeam(lmNoteRest* pOwner, bool fStemsDown, wxColour color)
	: lmCompositeShape(pOwner, 0, color, _T("Beam"), lmNO_DRAGGABLE, eGMO_ShapeBeam)
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

void lmShapeBeam::AddNoteRest(lmShapeStem* pStem, lmShape* pNoteRest,
							  lmTBeamInfo* pBeamInfo)
{
	//add the info. For rests pStem and pBeamInfo are NULL

	lmParentNote* pData = new lmParentNote;
	pData->pBeamInfo = pBeamInfo;
	pData->pShape = pNoteRest;
    if (pStem)
	    pData->nStem = Add(pStem);
    else
        pData->nStem = -1;
	m_cParentNotes.push_back(pData);

    //link note/rest to beam and, if it is a note, add stem info to note
    if (pStem)
    {
	    ((lmShapeNote*)pNoteRest)->SetBeamShape(this);
	    ((lmShapeNote*)pNoteRest)->SetStemShape(pStem);
    }
    else
	    ((lmShapeRest*)pNoteRest)->SetBeamShape(this);
}

lmShapeStem* lmShapeBeam::GetStem(int iParentNote)
{
	wxASSERT(iParentNote < (int)m_cParentNotes.size());

	int iShape = m_cParentNotes[iParentNote]->nStem;
    return (iShape != -1 ? (lmShapeStem*)GetShape(iShape) : (lmShapeStem*)NULL);
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

void lmShapeBeam::AdjustStemsIfNeeded()
{
    //The beam and the stems are rendered *after* noteheads and rests are rendered.
    //Therefore, when rendering the beam there is no option to adjust rests positions
    //to fit gracefuly inside the beamed group. 
    //This method is invoked when going to renter a rest that is included in the beam.
    //Its purpose is to adjust stems, if necessary, to adjust all rests' positions.

	if (m_fLayoutPending)
	{
		m_fLayoutPending = false;
		AdjustStems();
	}
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
    //AWARE:
    //The beam and the stems are rendered *after* noteheads and rests are rendered.
    //Therefore, at this point it is not feasible to move rests. Stem leght is adjusted
    //but only changes the stem itself.

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
    //
    lmPgmOptions* pPgmOpt = lmPgmOptions::GetInstance();
    lmTenths nOptValue = pPgmOpt->GetFloatValue(lm_EO_BEAM_THICKNESS);
	lmLUnits uThickness = ((lmStaffObj*)m_pOwner)->TenthsToLogical(nOptValue);
    nOptValue = pPgmOpt->GetFloatValue(lm_EO_BEAM_SPACING);
    lmLUnits uBeamSpacing = ((lmStaffObj*)m_pOwner)->TenthsToLogical(nOptValue) + uThickness;


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

    //TODO allow for customization.
    lmLUnits uBeamHookSize = ((lmStaffObj*)m_pOwner)->TenthsToLogical(11.0f);

    //clear bounds. They will be recomputed as we draw the beam segments
    m_uBoundsTop.x = 100000000.0f;      //any too big value
    m_uBoundsTop.y = 100000000.0f;      //any too big value
    m_uBoundsBottom.x = -100000000.0f;      //any too low value
    m_uBoundsBottom.y = -100000000.0f;      //any too low value

    //draw beam segments
    for (int iLevel=0; iLevel < 6; iLevel++)
	{
        fStart = false;
        fEnd = false;
        for(int iNote=0; iNote < (int)m_cParentNotes.size(); iNote++)
        {
			lmShapeStem* pShapeStem = GetStem(iNote);
            bool fIsNote = (pShapeStem != (lmShapeStem*)NULL);
            if (fIsNote)
            {
                pShapeNote = (lmShapeNote*)m_cParentNotes[iNote]->pShape;

                //compute current position to optimize
                uxCur = pShapeStem->GetXLeft();
                uyCur = pShapeStem->GetYEndStem() + uyShift;

                //Let's check if we have to finish a forward hook in prev. note
                if (fForwardPending)
                {
                    uxEnd = uxPrev + uBeamHookSize;
                    uyEnd = uyPrev + uBeamHookSize*(uyCur-uyPrev)/(uxCur-uxPrev);
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
                        uxStart = uxCur - uBeamHookSize;
                        uyStart = uyPrev + (uxCur-uxPrev-uBeamHookSize)*(uyCur-uyPrev)/(uxCur-uxPrev);
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
        }

        // displace y coordinate for next beamline
        uyShift += (m_fStemsDown ? - uBeamSpacing : uBeamSpacing);
    }

    //update selection rectangle
    m_uSelRect = GetBounds();

	//render stems
	lmCompositeShape::Render(pPaper, color);

}

void lmShapeBeam::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
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
		lmShapeStem* pShapeStem = GetStem(i);

		if (pShapeStem)
		{
			//it is a note
		    lmShapeNote* pShapeNote = (lmShapeNote*)m_cParentNotes[i]->pShape;
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
		//else
		    //it is a rest. Nothing to do
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
		lmShapeStem* pShapeStem = GetStem(i);
        if (pShapeStem)
		{
            yEnd[i] = yEnd[0] + (Ay * (pShapeStem->GetXLeft() - x1)) / Ax;

            //compute stem length. For chords we have to substract the stem segment joining
            //all chord notes. This extra lenght is zero for notes not in chord
            lmLUnits uStemLength = fabs(yStart[i] - yEnd[i]) - pShapeStem->GetExtraLenght();

            //save the shortest stem
            if (i==0)
                uMinStem = uStemLength;
            else
                uMinStem = wxMin(uMinStem, uStemLength);
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
    if (uMinStem < dyMin)
    {
        // a stem is smaller than dyMin. Increment all stems.
        uyIncr = dyMin - uMinStem;
        fAdjust = true;
    }
    else if (uMinStem > dyStem)
    {
        // all stems are greater than the standard size. Reduce them.
        //I'm not sure if this case is passible. But it is simple to deal with it
        uyIncr = -(uMinStem - dyStem);
        fAdjust = true;
    }
    else
    {
        fAdjust = false;
    }
    //wxLogMessage(_T("[lmShapeBeam::AdjustStems] dyMin=%d, nMinStem=%d, dyStem=%d, yIncr=%d"),
    //    dyMin, nMinStem, dyStem, yIncr);

    if (fAdjust)
    {
        for (int i = 0; i < nNumNotes; i++)
        {
            //wxLogMessage(_T("[lmShapeBeam::AdjustStems] before yEnd[%d]=%d"), i, yEnd[i]);
            if (yStart[i] < yEnd[i])
                yEnd[i] += uyIncr;
             else
                yEnd[i] -= uyIncr;
            //wxLogMessage(_T("[lmShapeBeam::AdjustStems] after yEnd[%d]=%d"), i, yEnd[i]);
        }
    }

    // At this point stems' lengths are computed and adjusted.
    // Transfer the computed values to the stem shape or shift the rest shape
    for(int i=0; i < nNumNotes; i++)
    {
		lmShapeStem* pShapeStem = GetStem(i);
        if (pShapeStem)
        {
			SetStemLength(pShapeStem, fabs(yEnd[i] - yStart[i]));
        }
        else
        {
		    //It is a rest. Shift rest to fit gracefuly inside the beamed group
		    lmShape* pShapeRest = m_cParentNotes[i]->pShape;
            lmLUnits uRestCenter = pShapeRest->GetYTop() + pShapeRest->GetHeight() / 2.0f;

            //compute center of first and last notes
            lmLUnits uMiddle = yStart[0] + (yStart[n] - yStart[0]) / 2.0f;

            //get shift
            lmLUnits uyShift = uMiddle - uRestCenter;

            //TODO: ensure that rest doesn't collide with beam line
            lmTODO(_T("[lmShapeBeam::AdjustStems] TODO: Ensure that rest doesn't collide with beam line"));

            //shift rest
            pShapeRest->Shift(0.0f, uyShift);
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

    //update bounds
    m_uBoundsTop.x = wxMin(m_uBoundsTop.x, wxMin(uxStart, uxEnd));
    m_uBoundsTop.y = wxMin(m_uBoundsTop.y, wxMin(uyStart, uyEnd) - uThickness);
    m_uBoundsBottom.x = wxMax(m_uBoundsBottom.x, wxMax(uxStart, uxEnd));
    m_uBoundsBottom.y = wxMax(m_uBoundsBottom.y, wxMax(uyStart, uyEnd) + uThickness);

    //wxLogMessage(_T("[lmShapeBeam::DrawBeamSegment] uxStart=%d, uyStart=%d, uxEnd=%d, uyEnd=%d, uThickness=%d, yStartIncr=%d, yEndIncr=%d, m_fStemsDown=%s"),
    //    uxStart, uyStart, uxEnd, uyEnd, uThickness, yStartIncr, yEndIncr, (m_fStemsDown ? _T("down") : _T("up")) );

}

bool lmShapeBeam::BoundsContainsPoint(lmUPoint& pointL)
{
    //check if point is in beam segments
    if (lmGMObject::BoundsContainsPoint(pointL))
        return true;

    //check if point is in any of the stems
    return lmCompositeShape::BoundsContainsPoint(pointL);
}

bool lmShapeBeam::HitTest(lmUPoint& pointL)
{
    //check if point is in beam segments
    if (lmGMObject::HitTest(pointL))
        return true;

    //check if point is in any of the stems
    return lmCompositeShape::HitTest(pointL);
}

