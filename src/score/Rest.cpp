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
#pragma implementation "Rest.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/image.h"
#include "Score.h"
#include "wx/debug.h"
#include "../ldp_parser/AuxString.h"

#include "Glyph.h"


lmRest::lmRest(lmVStaff* pVStaff, ENoteType nNoteType, float rDuration, bool fDotted, bool fDoubleDotted,
        int nStaff, bool fVisible, lmContext* pContext, bool fBeamed, lmTBeamInfo BeamInfo[])
    : lmNoteRest(pVStaff, DEFINE_REST, nNoteType, rDuration, fDotted, fDoubleDotted, 
                 nStaff, fVisible)
{

    CreateBeam(fBeamed, BeamInfo);
    g_pLastNoteRest = this;

}

lmRest::~lmRest()
{
    //remove the rest from the beam and if beam is empty delete the beam
    if (m_pBeam) {
        m_pBeam->Remove(this);
        if (m_pBeam->NumNotes() == 0) {
            delete m_pBeam;
            m_pBeam = (lmBeam*)NULL;
        }
    }

}


//--------------------------------------------------------------------------------------
// get glyph data to define character to use and selection rectangle 
//--------------------------------------------------------------------------------------

lmEGlyphIndex lmRest::GetGlyphIndex()
{
    // returns the index (over global glyphs table) to the character to use to print 
    // the rest (LenMus font)

    switch (m_nNoteType) {
        case eLonga:        return GLYPH_LONGA_REST;
        case eBreve:        return GLYPH_BREVE_REST;        
        case eWhole:        return GLYPH_WHOLE_REST;
        case eHalf:         return GLYPH_HALF_REST;        
        case eQuarter:      return GLYPH_QUARTER_REST;     
        case eEighth:       return GLYPH_EIGHTH_REST;       
        case e16th:         return GLYPH_16TH_REST;        
        case e32th:         return GLYPH_32ND_REST;        
        case e64th:         return GLYPH_64TH_REST;        
        case e128th:        return GLYPH_128TH_REST;
        case e256th:        return GLYPH_256TH_REST;
        default:
            wxASSERT(false);
            return GLYPH_QUARTER_REST;
    }

}

//====================================================================================================
// implementation of virtual methods defined in base abstract class lmNoteRest
//====================================================================================================

wxBitmap* lmRest::GetBitmap(double rScale)
{
    // Create the drag image.
    lmEGlyphIndex nGlyph = GetGlyphIndex();
    wxString sGlyph( aGlyphsInfo[nGlyph].GlyphChar );
    return PrepareBitMap(rScale, sGlyph);

}

void lmRest::LayoutObject(lmBox* pBox, lmPaper* pPaper, wxColour colorC)
{
    ///*
    //This method is invoked by the base class (lmStaffObj). When reaching this point 
    //paper cursor variable (m_uPaperPos) has been updated. This value must be used
    //as the base for any measurement / drawing operation.

    //DrawObject() method is responsible for:
    //1. In DO_MEASURE phase (fMeasuring == true):
    //    - Compute the surrounding rectangle, the glyph position and other measurements
    //2. In DO_DRAW phase (fMeasuring == false):
    //    - Render the object

    //*/


    //// move to right staff
    //lmLUnits uyTop = m_uPaperPos.y + GetStaffOffset();
    //lmLUnits uxLeft = m_uPaperPos.x;

    //// prepare DC
    //pPaper->SetFont(*m_pFont);

    ////if measurement phase and this is the first note/rest of a beam, measure beam
    //if (fMeasuring && m_fBeamed && m_BeamInfo[0].Type == eBeamBegin) {
    //    m_pBeam->CreateShape();
    //}

    //// Draw rest symbol
    ////----------------------------------------------------------------------------------
    //lmEGlyphIndex nGlyph = GetGlyphIndex();
    //wxString sGlyph( aGlyphsInfo[nGlyph].GlyphChar );
    //if (fMeasuring) {

    //    // store position
    //    m_uGlyphPos.x = 0;
    //    m_uGlyphPos.y = uyTop - m_uPaperPos.y +
    //        m_pVStaff->TenthsToLogical( aGlyphsInfo[nGlyph].GlyphOffset, m_nStaffNum );

    //    // store selection rectangle position and size
    //    lmLUnits nWidth, nHeight;
    //    pPaper->GetTextExtent(sGlyph, &nWidth, &nHeight);
    //    m_uSelRect.width = nWidth;
    //    m_uSelRect.height = m_pVStaff->TenthsToLogical( aGlyphsInfo[nGlyph].SelRectHeight, m_nStaffNum );
    //    m_uSelRect.x = m_uGlyphPos.x;
    //    m_uSelRect.y = m_uGlyphPos.y + m_pVStaff->TenthsToLogical( aGlyphsInfo[nGlyph].SelRectShift, m_nStaffNum );

    //    // store total width
    //    lmLUnits afterSpace = m_pVStaff->TenthsToLogical(10, m_nStaffNum);    //one line space
    //    m_uWidth = nWidth + afterSpace;

    //} else {
    //    // drawing phase: do the draw
    //    lmUPoint uPos = GetGlyphPosition();
    //    pPaper->SetTextForeground((m_fSelected ? g_pColors->ScoreSelected() : colorC));
    //    pPaper->DrawText(sGlyph, uPos.x, uPos.y );
    //}
    //uxLeft += m_uSelRect.width;

    ////draw dots
    ////------------------------------------------------------------
    //if (m_fDotted || m_fDoubleDotted)
    //{
    //    lmLUnits nSpaceBeforeDot = m_pVStaff->TenthsToLogical(5, m_nStaffNum);
    //    uxLeft += nSpaceBeforeDot;      //! @todo user selectable

    //    lmLUnits nShift = aGlyphsInfo[nGlyph].SelRectShift + (aGlyphsInfo[nGlyph].SelRectHeight / 2);
    //    nShift = m_pVStaff->TenthsToLogical(nShift, m_nStaffNum);
    //    lmLUnits yPos = m_uGlyphPos.y + m_uPaperPos.y; // + nShift;

    //    uxLeft += DrawDot(fMeasuring, pPaper, uxLeft, yPos, colorC, true);
    //    if (m_fDoubleDotted) {
    //        uxLeft += nSpaceBeforeDot;
    //        uxLeft += DrawDot(fMeasuring, pPaper, uxLeft, yPos, colorC, true);
    //    }
    //}

    //// render associated notations ----------------------------------
    //if (m_pNotations) {
    //    lmNoteRestObj* pNRO;
    //    wxAuxObjsListNode* pNode = m_pNotations->GetFirst();
    //    for (; pNode; pNode = pNode->GetNext() ) {
    //        pNRO = (lmNoteRestObj*)pNode->GetData();
    //        if (fMeasuring) {
    //             lmLUnits xPos = 0;
    //            lmLUnits yPos = 0;
    //            switch(pNRO->GetSymbolType()) {
    //                case eST_Fermata:
    //                    // set position (relative to paperPos)
    //                    xPos = m_uSelRect.x + m_uSelRect.width / 2;
    //                    yPos = uyTop - m_uPaperPos.y;
    //                    pNRO->SetSizePosition(pPaper, m_pVStaff, m_nStaffNum, xPos, yPos);
    //                    pNRO->UpdateMeasurements();
    //                    break;
    //                default:
    //                    wxASSERT(false);
    //            }
    //        }
    //        else
    //            pNRO->Draw(DO_DRAW, pPaper, colorC);
    //    }
    //}

    
}

void lmRest::DoVerticalShift(lmTenths yShift)
{ 
    // rests inside a group of beamed notes need to be shifted to align with the noteheads.
    // This method is invoked *after* the measurement phase. Therefore we have to 
    // shift all already measured affected values

    // compute shift in logical units
    lmLUnits uShift = m_pVStaff->TenthsToLogical(yShift, m_nStaffNum);

    // apply shift to rest object
    m_uGlyphPos.y += uShift;
    m_uSelRect.y += uShift;

    // apply shift to associated notations
    // todo: there is a problem with following code: I need pointer pPaper
    //if (m_pNotations) {
    //    lmNoteRestObj* pNRO;
    //    wxAuxObjsListNode* pNode = m_pNotations->GetFirst();
    //    for (; pNode; pNode = pNode->GetNext() ) {
    //        pNRO = (lmNoteRestObj*)pNode->GetData();
    //        lmLUnits xPos = 0;
    //        lmLUnits yPos = 0;
    //        switch(pNRO->GetSymbolType()) {
    //            case eST_Fermata:
    //                // set position (relative to paperPos)
    //                xPos = m_uSelRect.x + m_uSelRect.width / 2;
    //                yPos = GetStaffOffset() + uShift;
    //                pNRO->SetSizePosition(m_pPaper, m_pVStaff, m_nStaffNum, xPos, yPos);
    //                pNRO->UpdateMeasurements();
    //                break;
    //            default:
    //                wxASSERT(false);
    //        }
    //    }
    //}

}

wxString lmRest::Dump()
{
    wxString sDump = wxString::Format(
        _T("%d\tRest\tType=%d, TimePos=%.2f, rDuration=%.2f "),
        m_nId, m_nNoteType, m_rTimePos, m_rDuration );

    if (m_fBeamed) {
        sDump += wxString::Format(_T(", Beamed: BeamTypes(%d"), m_BeamInfo[0].Type);
        for (int i=1; i < 6; i++) {
            sDump += wxString::Format(_T(",%d"), m_BeamInfo[i].Type);
        }
        sDump += _T(")");
    }
    sDump += _T("\n");
                      
    return sDump;
    
}
wxString lmRest::SourceLDP(int nIndent)
{
    wxString sSource = _T("");
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += _T("(r ");    

    //duration
    sSource += GetLDPNoteType();
    if (m_fDotted) sSource += _T(".");
    if (m_fDoubleDotted) sSource += _T(".");

    //staff num
    if (m_pVStaff->GetNumStaves() > 1) {
        sSource += wxString::Format(_T(" p%d"), m_nStaffNum);
    }

    //visible?
    if (!m_fVisible) { sSource += _T(" noVisible"); }

    sSource += _T(")\n");
    return sSource;
}

wxString lmRest::SourceXML()
{
    wxString sSource = _T("TODO: lmRest XML Source code generation methods");
    return sSource;
//    sPitch = GetNombreSajon(m_nPitch)
//    
//    sFuente = "            <note>" & sCrLf
//    sFuente = sFuente & "                <rest>" & sCrLf
//    sFuente = sFuente & "                    <display-step>" & Left$(sPitch, 1) & "</display-step>" & sCrLf
//    sFuente = sFuente & "                    <display-octave>" & Mid$(sPitch, 2) & "</display-octave>" & sCrLf
//    sFuente = sFuente & "                </rest>" & sCrLf
//    sFuente = sFuente & "                <duration>2</duration>" & sCrLf
//    sFuente = sFuente & "                <voice>1</voice>" & sCrLf
//    sFuente = sFuente & "                <type>quarter</type>" & sCrLf
//    sFuente = sFuente & "                <stem>up</stem>" & sCrLf
//    sFuente = sFuente & "                <notations>" & sCrLf
//    sFuente = sFuente & "                    <slur type=""start"" number=""1""/>" & sCrLf
//    sFuente = sFuente & "                </notations>" & sCrLf
//    sFuente = sFuente & "            </note>" & sCrLf
//    FuenteXML = sFuente
}


//====================================================================================================
// implementation of virtual methods defined in base class lmStaffObj
//====================================================================================================

lmScoreObj* lmRest::FindSelectableObject(lmUPoint& pt)
{
    //THINK: Rests aren't really composite obj
    if (IsAtPoint(pt)) return this;

    // try with associated AuxObjs
    if (m_pNotations) {
        lmNoteRestObj* pNRO;
        wxAuxObjsListNode* pNode = m_pNotations->GetFirst();
        for (; pNode; pNode = pNode->GetNext() ) {
            pNRO = (lmNoteRestObj*)pNode->GetData();
            if (pNRO->IsAtPoint(pt)) return pNRO;
        }
    }

    // Not found
    return (lmScoreObj*)NULL;    //none found

}

//====================================================================================================
// implementation of virtual methods defined in base class lmStaffObj
//====================================================================================================

void lmRest::OnDrag(lmPaper* pPaper, wxDragImage* pDragImage, lmDPoint& ptOffset, 
            const lmUPoint& ptLog, const lmUPoint& uDragStartPos, const lmDPoint& ptPixels)
{
    lmScoreObj::OnDrag(pPaper, pDragImage, ptOffset, ptLog, uDragStartPos, ptPixels);
}

lmUPoint lmRest::EndDrag(const lmUPoint& uPos)
{
    return lmScoreObj::EndDrag(uPos);
}

