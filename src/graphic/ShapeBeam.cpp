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


//-------------------------------------------------------------------------------------
// Implementation of lmShapeBeam


lmShapeBeam::lmShapeBeam(lmNoteRest* pOwner, bool fStemsDown, int nNumNotes, wxColour color)
	: lmCompositeShape(pOwner, _T("Beam"), eGMO_ShapeBeam)
{
	m_color = color;
	m_fStemsDown = fStemsDown;
	m_nNumNotes = nNumNotes;
}

lmShapeBeam::~lmShapeBeam()
{
}

void lmShapeBeam::AddNoteRest(lmShapeNote* pShape, lmTBeamInfo* pBeamInfo[])
{
	//add the info
	lmParentNote* pData = new lmParentNote;
	pData->pBeamInfo = pBeamInfo;
	pData->pShape = pShape;
	m_cParentNotes.push_back(pData);

	//if finished recompute layout
	if (m_nNumNotes <= (int)m_cParentNotes.size())
	{
		m_nNumNotes = (int)m_cParentNotes.size();
		//Relayout();
	}

}

void lmShapeBeam::OnAttachmentPointMoved(lmShape* pShape, lmEAttachType nTag)
{
	////if intermediate note moved, nothing to do
	//if (!(nTag == eGMA_StartNote || nTag == eGMA_EndNote)) return;

	////computhe half notehead width
	//lmShape* pSNH = ((lmShapeNote*)pShape)->GetNoteHead();
	//wxASSERT(pSNH);
	//lmLUnits uHalfNH = (pSNH->GetXRight() - pSNH->GetXLeft()) / 2.0;

	//if (nTag == eGMA_StartNote)
	//{
	//	//start note moved. Recompute start of shape
	//	//Placed on center of notehead if above, or half notehead before if below
	//	if (m_fAbove)
	//	{
	//		m_uxStart = pSNH->GetXLeft() + uHalfNH;
	//		m_uyStart = pShape->GetYTop();
	//	}
	//	else
	//	{
	//		m_uxStart = pSNH->GetXLeft() - uHalfNH;
	//		m_uyStart = pShape->GetYBottom();
	//	}
	//	SetXLeft(m_uxStart);
	//	SetYTop(m_uyStart);
	//}

	//else if (nTag == eGMA_EndNote)
	//{
	//	//end note moved. Recompute end of shape
	//	//Placed half notehead appart if above, or on center of notehead if below
	//	if (m_fAbove)
	//	{
	//		m_uxEnd = pSNH->GetXRight() + uHalfNH;
	//		m_uyEnd = pShape->GetYTop();
	//	}
	//	else
	//	{
	//		m_uxEnd = pSNH->GetXRight() - uHalfNH;
	//		m_uyEnd = pShape->GetYBottom();
	//	}
	//	SetXRight(m_uxEnd);
	//	SetYBottom(m_uyEnd);
	//}

	//NormaliceBoundsRectangle();

}

void lmShapeBeam::Render(lmPaper* pPaper, lmUPoint uPos, wxColour color)
{
 //   WXUNUSED(uPos);

	////get staff, for scaling logical units
 //   lmVStaff* pVStaff = m_pStartNR->GetVStaff();
 //   int nStaff = m_pStartNR->GetStaffNum();
 //   lmStaff* pStaff = pVStaff->GetStaff(nStaff);

 //   //Prepare pen
 //   lmLUnits uThick = pStaff->TenthsToLogical(2.0);    //! @todo user options
 //   pPaper->SetPen(color, uThick);

 //   lmShape::RenderCommon(pPaper);

}

void lmShapeBeam::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
 //   m_uxStart += xIncr;
	//m_uyStart += yIncr;
 //   m_uxEnd += xIncr;
	//m_uyEnd += yIncr;

    ShiftBoundsAndSelRec(xIncr, yIncr);
}

void lmShapeBeam::AdjustStems()
{
//	// In this method the lenght of note stems in a beamed group is adjusted.
//	// It is necessary to adjust stems whenever the x position of a note in the 
//	// beam changes.
//	
//	
//	//  BUG_BYPASS: There is a bug in Composer5 and it some times generate scores
//    //    ending with a start of group. As this start is in the last note of the score,
//    //    the group has only one note.
//    //
//    if (m_cNotes.GetCount()==1) {
//        wxLogMessage(_T("*** ERROR *** Group with just one note!"));
//        return;
//    }
//    // End of BUG_BYPASS
//
//
//    // At this point all stems have the standard size and the stem start point (the point
//    // nearest to the notehead) is computed. Now we are goin to compute the end point
//    // for each stem.
//    // As we are interested in the stems' length, not in the exact coordinates, instead
//    // of using the real start coordinates, we are going to compute an arbitrary start
//    // coordinate relative to zero. This has the advantage, over using the real coordinates,
//    // that all stems will be in the same system, and this is better when we have to split
//    // the beam into two systems.
//    // The computed start and end positions for each stem will be stored in the auxiliary
//    // arrays yBase and yTop, respectively.
//
//    int nNumNotes = m_cNotes.GetCount();
//    std::vector<float>yBase(nNumNotes+1);   // 1 based
//    std::vector<float>yTop(nNumNotes+1);    // 1 based
//
//    wxNoteRestsListNode *pNode;
//    lmNote* pMaxNote;
//    lmNote* pMinNote;
//    lmNote* pNote;
//    int i = 1;
//
//#if 1
//    //code using an arbitrary start coordinate relative to zero
//    for(pNode = m_cNotes.GetFirst(); pNode; pNode=pNode->GetNext(), i++)
//    {
//        pNote = (lmNote*)pNode->GetData();
//         lmLUnits udyStem = pNote->GetDefaultStemLength();
//        if (pNote->IsInChord()) {
//             pMinNote = (pNote->GetChord())->GetMinNote();
//             pMaxNote = (pNote->GetChord())->GetMaxNote();
//            if (pNote->StemGoesDown()) {
//                yBase[i] = pMaxNote->GetStaffOffset() - pMaxNote->GetPitchShift();
//                yTop[i] = pMinNote->GetStaffOffset() - pMinNote->GetPitchShift() + udyStem;
//            } else {
//                yBase[i] = pMinNote->GetStaffOffset() - pMinNote->GetPitchShift();
//                yTop[i] = pMaxNote->GetStaffOffset() - pMaxNote->GetPitchShift() - udyStem;
//            }
//        } else {
//            if (pNote->IsRest()) {
//                yBase[i] = pNote->GetStaffOffset();
//                yTop[i] = yBase[i];
//            }
//            else {
//                yBase[i] = pNote->GetStaffOffset() - pNote->GetPitchShift();
//                yTop[i] = yBase[i] + (pNote->StemGoesDown() ? udyStem : -udyStem);
//            }
//        }
//    }
//
//#else
//    //  code using the real start coordinate
//    for(pNode = m_cNotes.GetFirst(); pNode; pNode=pNode->GetNext(), i++)
//    {
//        pNote = (lmNote*)pNode->GetData();
//        lmLUnits dyStem = pNote->GetDefaultStemLength();
//        if (pNote->IsInChord()) {
//            pMinNote = (pNote->GetChord())->GetMinNote();
//            pMaxNote = (pNote->GetChord())->GetMaxNote();
//            if (pNote->StemGoesDown()) {
//                yBase[i] = pMaxNote->GetYStem();
//                yTop[i] = pMinNote->GetYStem() + dyStem;
//            } else {
//                yBase[i] = pMinNote->GetYStem();
//                yTop[i] = pMaxNote->GetYStem() - dyStem;
//            }
//        } else {
//            if (pNote->IsRest()) {
//                yBase[i] = pNote->GetYStem();
//                yTop[i] = yBase[i];
//            }
//            else {
//                yBase[i] = pNote->GetYStem();
//                yTop[i] = yBase[i] + (pNote->StemGoesDown() ? dyStem : -dyStem);
//            }
//        }
//    }
//#endif
//
//
//    // lmBeam line position is established by the first and last notes' stems. Now
//    // let's adjust the intermediate notes' stem lengths to end up in the beam line.
//    // This is just a proportional share based on line slope:
//    // If (x1,y1) and (xn,yn) are, respectively, the position of first and last notes of
//    // the group, the y position of an intermediate note i can be computed as:
//    //     Ay = yn-y1
//    //     Ax = xn-x1
//    //                Ay
//    //     yi = y1 + ---- (xi-x1)
//    //                Ax
//    // The x position of the stem has beeen computed in Note object during the
//    // measurement phase.
//    //
//    // The loop is also used to look for the shortest stem
//
//    lmLUnits Ay = yTop[nNumNotes] - yTop[1];
//    lmLUnits x1 = m_pFirstNote->GetXStemLeft();
//    lmLUnits Ax = m_pLastNote->GetXStemLeft() - x1;
//    lmNoteRest* pNR;
//    lmLUnits uMinStem;
//    for(i=1, pNode = m_cNotes.GetFirst(); pNode; pNode=pNode->GetNext(), i++)
//    {
//        pNR = (lmNoteRest*)pNode->GetData();
//        if (!pNR->IsRest()) {
//            pNote = (lmNote*)pNR;
//            yTop[i] = yTop[1] + (Ay * (pNote->GetXStemLeft() - x1)) / Ax;
//            //save the shortest stem
//            if (i==1)
//                uMinStem = fabs(yBase[1] - yTop[1]);
//            else
//                uMinStem = wxMin(uMinStem, fabs(yBase[i] - yTop[i]));
//        }
//    }
//
//    // If there is a note in the group out of the interval formed by the first note and the
//    // last note, then stem could be too too short. For example, a group of three notes,
//    // the first and the last ones D4 and the middle one G4; the beam is horizontal, nearly
//    // the G4 line; so the midle notehead would be positioned just on the beam line.
//    // So let's avoid these problems by adjusting the stems so that all stems have
//    // a minimum height
//
//    lmLUnits dyStem = m_pFirstNote->GetDefaultStemLength();
//    lmLUnits dyMin = (2 * dyStem) / 3;
//    bool fAdjust;
//
//    // compare the shortest with this minimun required
//    lmLUnits uyIncr;
//    if (uMinStem < dyMin) {
//        // a stem is smaller than dyMin. Increment all stems.
//        uyIncr = dyMin - uMinStem;
//        fAdjust = true;
//    }
//    else if (uMinStem > dyStem) {
//        // all stems are greater than the standard size. Reduce them.
//        uyIncr = -(uMinStem - dyStem);
//        fAdjust = true;
//    }
//    else {
//        fAdjust = false;
//    }
//    //wxLogMessage(_T("[lmBeam::TrimStems] dyMin=%d, nMinStem=%d, dyStem=%d, yIncr=%d"),
//    //    dyMin, nMinStem, dyStem, yIncr);
//
//    if (fAdjust) {
//        for (i = 1; i <= nNumNotes; i++) {
//            //wxLogMessage(_T("[lmBeam::TrimStems] before yTop[%d]=%d"), i, yTop[i]);
//           if (yBase[i] < yTop[i]) {
//                yTop[i] += uyIncr;
//            } else {
//                yTop[i] -= uyIncr;
//            }
//            //wxLogMessage(_T("[lmBeam::TrimStems] after yTop[%d]=%d"), i, yTop[i]);
//        }
//    }
//
//    // At this point stems' lengths are computed and adjusted.
//    // Transfer the computed values to the notes
//    for(i=1, pNode = m_cNotes.GetFirst(); pNode; pNode=pNode->GetNext(), i++)
//    {
//        lmLUnits uLength = (yBase[i] > yTop[i] ? yBase[i] - yTop[i] : yTop[i] - yBase[i]);
//        pNR = (lmNoteRest*)pNode->GetData();
//        if (pNR->IsRest()) {
//            ((lmRest*)pNR)->DoVerticalShift(m_nPosForRests);
//        }
//        else {
//            pNote = (lmNote*)pNR;
//            pNote->SetStemLength(uLength);
//        }
//    }

}


