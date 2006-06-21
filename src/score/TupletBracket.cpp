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
/*! @file TupletBracket.cpp
    @brief Implementation file for class lmTupletBracket
    @ingroup score_kernel
*/
#ifdef __GNUG__
// #pragma implementation
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


//---------------------------------------------------------
//   lmTupletBracket implementation
//---------------------------------------------------------

lmTupletBracket::lmTupletBracket(bool fShowNumber, int nNumber, bool fBracket, bool fAbove)
    : lmSimpleObj(eTPO_TupletBracket)
{
    m_fShowNumber = fShowNumber;
    m_nTupletNumber = nNumber;
    m_fBracket = fBracket;
    m_fAbove = fAbove;

    //! @todo Allow user to change this values
    m_sFontName = _T("Arial");
    m_nFontSize = PointsToLUnits(8);
    m_fBold = false;
    m_fItalic = true;

}

lmTupletBracket::~lmTupletBracket()
{
    //notes/rests will not be deleted when deleting the list, as they are part of a lmScore 
    //and will be deleted there.
    m_cNotes.DeleteContents(false);
    m_cNotes.Clear();
}

//void lmTupletBracket::SetStartPoint(lmLUnits xPos, lmLUnits yPos, lmLUnits xPaperLeft)
//{
//    m_xStart = xPos;
//    m_yStart = yPos;
//    m_xPaperLeft = xPaperLeft;
//
//}
//
//void lmTupletBracket::SetEndPoint(lmLUnits xPos, lmLUnits yPos, lmLUnits xPaperRight)
//{
//    m_xEnd = xPos;
//    m_yEnd = yPos;
//    m_xPaperRight = xPaperRight;
//
//    // with the end positioning points we have all data needed to update measurements
//    UpdateMeasurements();
//}
//
//void lmTupletBracket::UpdateMeasurements()
//{
//    /*
//    the position of one of the owner notes has changed. Update bracket position and size
//    */
//    wxPoint startOffset = (GetStartNote())->GetOrigin();
//    wxPoint endOffset = (GetEndNote())->GetOrigin();
//
//    m_paperPos = startOffset;
//    SetSelRectangle(m_xStart, m_yStart,
//                    m_xEnd + endOffset.x - startOffset.x - m_xStart,
//                    m_yEnd + endOffset.y - startOffset.y - m_yStart + 2000);
//
//}

void lmTupletBracket::AutoPosition()
{
    //! @todo
}
wxString lmTupletBracket::Dump()
{
    wxString sDump = wxString::Format(
        _T("\t-->lmTupletBracket:\ttupletNumber=%d, showNumber=%s, showBracket=%s, above=%s\n"),
        m_nTupletNumber,
        (m_fShowNumber ? _T("yes") : _T("no")),
        (m_fBracket ? _T("yes") : _T("no")),
        (m_fAbove ? _T("yes") : _T("no")) );
    return sDump;

}

wxString lmTupletBracket::SourceLDP()
{
    //! @todo all
    wxString sSource = _T("TODO: lmTupletBracket LDP Source code generation methods");
    return sSource;
}

wxString lmTupletBracket::SourceXML()
{
    //! @todo all
    wxString sSource = _T("TODO: lmTupletBracket XML Source code generation methods");
    return sSource;
}

void lmTupletBracket::DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC)
{
    /*
    TupletBrackets never appear in a lmVStaff. They are always part
    of a lmNoteRest. Therefore, drawing is not controlled by the standard DrawObject() mechaninsm
    but is always controlled by the two owner NotesRests.

    It is too early to define an standard behaviour for AuxObjs. In general:
      -    m_paperPos and m_selRect must be always updated as it is being used for selection.
        This implies that positioning and size information of AuxObjs associated to an
        lmStaffObj (i.e. Ties, TupletBrackets) must be updated when the owner lmStaffObj is 
        updated (i.e. the formatting and justification process).

    For lmTupletBracket and Ties:
      -    the operations to be performed during the measurement phase are done via
        invocation of the methods SetStartPoint() and  SetEndPoint().
      -    the drawing phase is done by invoking Draw()
    */

    if (fMeasuring) return;


    lmNoteRest* pStartNR = GetStartNote();
    lmNoteRest* pEndNR = GetEndNote();
    //! @todo For now I am only dealing with notes
    lmNote* pStartNote = (lmNote*)pStartNR;
    lmNote* pEndNote = (lmNote*)pEndNR;

    //Prepare DC
    wxDC* pDC = pPaper->GetDC();
    wxPen oldPen = pDC->GetPen();
    wxPen pen((m_fSelected ? g_pColors->ScoreSelected() : colorC),
              lmToLogicalUnits(0.2, lmMILLIMETERS),         // width = 0.2 mm
              wxSOLID);
    pDC->SetPen(pen);

    //Mesure number
    lmLUnits nNumberWidth=0, nNumberHeight=0;
    wxString sNumber = _T("");
    if (m_fShowNumber) {
        sNumber = wxString::Format(_T("%d"), m_nTupletNumber);
        pDC->SetFont(*m_pFont);
        pDC->GetTextExtent(sNumber, &nNumberWidth, &nNumberHeight);
    }


    //compute bracket position
    //------------------------------------------------
    lmLUnits BORDER_LENGHT = lmToLogicalUnits(1, lmMILLIMETERS);  // 1 mm
    lmLUnits BRACKET_DISTANCE = lmToLogicalUnits(3, lmMILLIMETERS);  // 3 mm
    lmLUnits NUMBER_DISTANCE = lmToLogicalUnits(1, lmMILLIMETERS);  // 1 mm

    lmLUnits xStart = pStartNote->GetBoundsLeft();
    lmLUnits yStart = (m_fAbove ? pStartNote->GetBoundsTop() : pStartNote->GetBoundsBottom() );
    lmLUnits xEnd = pEndNote->GetBoundsRight();
    lmLUnits yEnd = (m_fAbove ? pEndNote->GetBoundsTop() : pEndNote->GetBoundsBottom() );
    lmLUnits yLineStart, yLineEnd, yStartBorder, yEndBorder, yNumber;

    ////DEBUG: Draw the boundling rectangle of start and end notes ------------------
    //pDC->SetPen( wxPen(*wxRED, 1, wxSOLID) );
    //pDC->SetBrush( *wxTRANSPARENT_BRUSH );
    //lmLUnits xLeft = pStartNote->GetBoundsLeft();
    //lmLUnits xRight = pStartNote->GetBoundsRight();
    //lmLUnits yTop = pStartNote->GetBoundsTop();
    //lmLUnits yBottom = pStartNote->GetBoundsBottom();
    //pDC->DrawRectangle(xLeft, yTop, xRight-xLeft, yBottom-yTop);
    //xLeft = pEndNote->GetBoundsLeft();
    //xRight = pEndNote->GetBoundsRight();
    //yTop = pEndNote->GetBoundsTop();
    //yBottom = pEndNote->GetBoundsBottom();
    //pDC->DrawRectangle(xLeft, yTop, xRight-xLeft, yBottom-yTop);
    ////DEBUG END -------------------------------------------------------------------

    if (m_fAbove) {
        yLineStart = yStart - BRACKET_DISTANCE;
        yLineEnd = yEnd - BRACKET_DISTANCE;
        yStartBorder = yLineStart + BORDER_LENGHT;        
        yEndBorder = yLineEnd + BORDER_LENGHT;        
        yNumber = yStartBorder - NUMBER_DISTANCE - nNumberHeight;
    } else {
        yLineStart = yStart + BRACKET_DISTANCE;
        yLineEnd = yEnd + BRACKET_DISTANCE;
        yStartBorder = yLineStart - BORDER_LENGHT;        
        yEndBorder = yLineEnd - BORDER_LENGHT;        
        yNumber = yStartBorder + NUMBER_DISTANCE;
    }
    lmLUnits xNumber = (xStart + xEnd - nNumberWidth)/2;
    

    //draw bracket
    //---------------------------------------------
    //horizontal line
    pDC->DrawLine(xStart, yLineStart, xEnd, yLineEnd);
    
    //vertical borders
    pDC->DrawLine(xStart, yLineStart, xStart, yStartBorder);
    pDC->DrawLine(xEnd, yLineEnd, xEnd, yEndBorder);
    
    //write the number
    if (m_fShowNumber) {
        pDC->SetTextForeground((m_fSelected ? g_pColors->ScoreSelected() : colorC));
        pDC->DrawText(sNumber, xNumber, yNumber);
    }
    pDC->SetPen(oldPen);

    
}

int lmTupletBracket::NumNotes()
{
    //return the number of notes/rests that are grouped by this bracket
    return (int)m_cNotes.GetCount();
}

void lmTupletBracket::Include(lmNoteRest* pNR)
{
    m_cNotes.Append(pNR);
}

void lmTupletBracket::Remove(lmNoteRest* pNR)
{
    m_cNotes.DeleteObject(pNR);
}

lmNoteRest* lmTupletBracket::GetStartNote() 
{
    wxNoteRestsListNode *pNode = m_cNotes.GetFirst();
    return (lmNoteRest*)pNode->GetData();
}

lmNoteRest* lmTupletBracket::GetEndNote()
{
    wxNoteRestsListNode *pNode = m_cNotes.GetLast();
    return (lmNoteRest*)pNode->GetData();
}

void lmTupletBracket::SetFont(lmPaper* pPaper)
{
    //wxLogMessage(wxString::Format(
    //    _T("[lmTupletBracket::SetFont]: size=%d, name=%s"), m_nFontSize, m_sFontName));

    int nWeight = (m_fBold ? wxBOLD : wxNORMAL);
    int nStyle = (m_fItalic ? wxITALIC : wxNORMAL);
    m_pFont = pPaper->GetFont(m_nFontSize, m_sFontName, wxDEFAULT, nStyle, nWeight, false);

    if (!m_pFont) {
        wxMessageBox(_("Sorry, an error has occurred while allocating the font."),
            _T("lmTupletBracket::SetFont"), wxOK);
        ::wxExit();
    }
}

void lmTupletBracket::ComputePosition()
{
    /*
    During measurement phase method ComputePosition() is invoked to precompute positioning
    information based on grouped notes positions and stems lengths and directions.

    This method also takes care of all actions to be performed during the measurement phase,
    such as updating the selection rectangle.
    */

    //lmNoteRest* pNR;
    //wxNoteRestsListNode *pNode;

    //// look for the highest and lowest pitch notes so that we can properly position posible
    //// rests along the group
    //int nMaxPosOnStaff = 0;
    //int nMinPosOnStaff = 99999;
    //for(pNode = m_cNotes.GetFirst(); pNode; pNode=pNode->GetNext()) 
    //{
    //    pNR = (lmNoteRest*)pNode->GetData();
 //       if (!pNR->IsRest()) {     //ignore rests
 //           if (pNR->IsInChord()) {
 //               //Is in chord. So must be the base note
 //               nMaxPosOnStaff = wxMax(nMaxPosOnStaff, ((pNR->GetChord())->GetMaxNote())->GetPosOnStaff());
 //               nMinPosOnStaff = wxMin(nMinPosOnStaff, ((pNR->GetChord())->GetMinNote())->GetPosOnStaff());
 //           } else {
 //               //is not part of a chord
 //               nMaxPosOnStaff = wxMax(nMaxPosOnStaff, pNR->GetPosOnStaff());
 //               nMinPosOnStaff = wxMin(nMinPosOnStaff, pNR->GetPosOnStaff());
 //           }
 //       }
 //   }
 //   if (nMinPosOnStaff == 99999) nMinPosOnStaff = 0;
 //   m_nPosForRests = (nMaxPosOnStaff + nMinPosOnStaff) / 2;
    
    
//    //look for the stem direction of most notes. If one note has is stem direction
//    // forced (by a slur, probably) forces the group stem in this direction
//
//    bool fStemForced = false;        // assume no stem forced
//    int nAboveCentre = 0;            // number of noteheads drawn above the staff centre
//    int nNumNotes = 0;
//    m_fStemsDown = false;            // stems up by default
//
//    for(pNode = m_cNotes.GetFirst(); pNode; pNode=pNode->GetNext()) 
//    {
//        pNR = (lmNoteRest*)pNode->GetData();
//        if (!pNR->IsRest()) {     //ignore rests
//            if (pNR->IsInChord()) {
//                //is the base note of a chord. Get chord stem direction info from the base note
//                nNumNotes++;
//                if (pNR->StemGoesDown()) nAboveCentre++;
//            } else {
//                //Note is not in chord. Compute its stem direction 
//                nNumNotes++;
//                if (pNR->GetPosOnStaff() > 5) nAboveCentre++;
//            }
//
//            if (pNR->GetStemType() != eDefaultStem) {
//                fStemForced = true;
//                m_fStemsDown = pNR->StemGoesDown();
//                break;
//            }
//        }
//    }
//    
//    if (!fStemForced) {
//        if (nAboveCentre >= (nNumNotes + 1) / 2 ) 
//            m_fStemsDown = true;
//    }
//    
//    // At this point the stem direction is computed. Now we proceed to compute
//    // the stem size for each note in the beaming, so that all stem final poins get aligned.
//
//    //Absolute positioning information is irrelevant as we are computing stem's length.
//    //Therefore to simplify let's use an arbitrary value of 100 for paper Y cursor.
//    int yDo = 100;
//    nNumNotes = m_cNotes.GetCount();
//    //LIMIT: No more than 99 notes in a tuplet
//    int yBase[100], yTop[100];
//    if (nNumNotes > 99) {
//        //! @todo how to show the messege to the user and how to stop the program
//        wxLogMessage(_("[lmTupletBracket::ComputePosition] Program limit: more than 99 notes in a tuplet"));
//        wxASSERT(false);
//    }
//
//    lmNoteRest* pChordNote;
//    int i = 1;
//    for(pNode = m_cNotes.GetFirst(); pNode; pNode=pNode->GetNext(), i++) 
//    {
//        pNR = (lmNoteRest*)pNode->GetData();
//         lmLUnits dyStem = pNR->GetDefaultStemLength();
//        if (pNR->IsInChord()) {
//            if (m_fStemsDown) {
//                //use chord minimum pich for the computation
//                 pChordNote = (pNR->GetChord())->GetMinNote();
//                yBase[i] = yDo - pChordNote->GetPitchShift();
//                yTop[i] = yBase[i] + dyStem;
//            } else {
//                //use chord maximum pich for the computation
//                 pChordNote = (pNR->GetChord())->GetMaxNote();
//                yBase[i] = yDo - pChordNote->GetPitchShift();
//                yTop[i] = yBase[i] - dyStem;
//            }
//        } else {
//            yBase[i] = yDo - pNR->GetPitchShift();
//            yTop[i] = yBase[i] + (m_fStemsDown ? dyStem : -dyStem);
//        }
//    }
//    
//    //trim stem length of intermediate notes
//    lmLUnits dyA = (yTop[nNumNotes] - yTop[1]) / (nNumNotes - 1);
//    for (i = 2; i < nNumNotes; i++) {
//        yTop[i] = yTop[i - 1] + dyA;
//    }
//    
////    //ajusta altura para que haya un mínimo y no se sobrepase dyPlica
////    Dim fAjustar As Boolean, dyMinimo As Single
////    dyMinimo = 3 * dyLinea
////    dyMin = Abs(yBase(2) - yTop(2))
////    for (i = 3 To nNumNotes - 1
////        dyMin = MinSingle(dyMin, Abs(yBase[i] - yTop[i]))
////    }   //   i
////
////    if (dyMin < dyMinimo) {
////        dyMin = dyMinimo - dyMin
////        fAjustar = True
////    } else if ( dyMin > dyPlica) {
////        dyMin = -(dyPlica - dyMinimo)
////        fAjustar = True
////    } else {
////        fAjustar = false
////    }
////
////    if (fAjustar) {
////        for (i = 1 To nNumNotes
////            if (m_fStemsDown) {
////                yTop[i] = yTop[i] + dyMin
////            } else {
////                yTop[i] = yTop[i] - dyMin
////            }
////        }   //   i
////    }
//    
//    // At this point stems' lengths are computed.
//    // Transfer the computed values to the notes
//    for(i=1, pNode = m_cNotes.GetFirst(); pNode; pNode=pNode->GetNext(), i++) 
//    {
//        pNR = (lmNoteRest*)pNode->GetData();
//        lmLUnits nLength = (yBase[i] > yTop[i] ? yBase[i] - yTop[i] : yTop[i] - yBase[i]);
//        if (pNR->IsRest())
//            ;
//        //! @todo lmRest
////            pNR.PosSilencio = m_nPosForRests
//        else {
//            pNR->SetStemLength(nLength);
//            pNR->SetStemDirection(m_fStemsDown);
//        }
//    }
    
}

