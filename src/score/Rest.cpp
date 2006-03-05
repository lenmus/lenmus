// RCS-ID: $Id: Rest.cpp,v 1.6 2006/02/23 19:23:54 cecilios Exp $
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
/*! @file Rest.cpp
    @brief Implementation file for class lmRest
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

#include "wx/image.h"
#include "Score.h"
#include "wx/debug.h"
#include "../ldp_parser/AuxString.h"


// chars for drawing rests (LeMus font)
//! @todo                                //larga
//! @todo                                //breve, cuadrada
#define CHAR_WHOLE_REST        _T("{")    //redonda
#define CHAR_HALF_REST        _T("z")    //blanca
#define CHAR_QUARTER_REST    _T("y")    //negra
#define CHAR_EIGHT_REST        _T("x")    //corchea
#define CHAR_16TH_REST        _T("w")    //semicorchea
#define CHAR_32ND_REST        _T("v")    //fusa
#define CHAR_64TH_REST        _T("u")    //semifusa
//! @todo                                //garrapatea
//! @todo                                //semigarrapatea


lmRest::lmRest(lmVStaff* pVStaff, ENoteType nNoteType, float rDuration, bool fDotted, bool fDoubleDotted,
        wxInt32 nStaff, lmContext* pContext, ETuplas nTupla,
        bool fBeamed, lmTBeamInfo BeamInfo[])
    : lmNoteRest(pVStaff, DEFINE_REST, nNoteType, rDuration, fDotted, fDoubleDotted, nStaff)
{

    m_yShift = 0;
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

// returns the y-axis offset from paper cursor position so that shape get correctly
// positioned over a five-lines staff (units: tenths of inter-line space)
lmTenths lmRest::GetGlyphOffset()
{
    lmTenths yOffset;
    switch(m_nNoteType)
    {
        case eWhole:    yOffset = -20;    break;
        case eHalf:        yOffset = -20;    break;
        case eQuarter:        yOffset = -15;    break;
        case eEighth:    yOffset = -20;    break;
        case e16th: yOffset = -15;    break;
        case e32th:        yOffset = -10;    break;
        case e64th:    yOffset = -25;    break;
        default:
            yOffset = 0;
            wxASSERT_MSG( false, _T("Invalid value for attribute m_nNoteType"));
    }

    return yOffset;

}

// returns the y-axis offset from bitmap rectangle to the selection rectangle origin
// (units: tenths of inter-line space)
lmTenths lmRest::GetSelRectShift()
{
    lmTenths yOffset;
    switch(m_nNoteType)
    {
        case eWhole:    yOffset = 30;    break;
        case eHalf:        yOffset = 35;    break;
        case eQuarter:        yOffset = 20;    break;
        case eEighth:    yOffset = 28;    break;
        case e16th: yOffset = 18;    break;
        case e32th:        yOffset = 10;    break;
        case e64th:    yOffset = 25;    break;
        default:
            yOffset = 0;
            wxASSERT_MSG( false, _T("Invalid value for attribute m_nNoteType"));
    }

    return yOffset;

}

// returns the height of the selection rectangle
// units: tenths (tenths of inter-line space)
lmTenths lmRest::GetSelRectHeight()
{
    lmTenths nHeight;
    switch(m_nNoteType)
    {
        case eWhole:    nHeight = 5;    break;
        case eHalf:        nHeight = 5;    break;
        case eQuarter:        nHeight = 30;    break;
        case eEighth:    nHeight = 20;    break;
        case e16th: nHeight = 30;    break;
        case e32th:        nHeight = 40;    break;
        case e64th:    nHeight = 50;    break;
        default:
            nHeight = 80;
            wxASSERT_MSG( false, _T("Invalid value for attribute m_nNoteType"));
    }

    return nHeight;

}

wxString lmRest::GetLenMusChar()
{
    // returns the character to use to print the rest (LenMus font)

    wxString sGlyph = CHAR_QUARTER_REST;
    switch (m_nNoteType) {
        case eWhole:    sGlyph = CHAR_WHOLE_REST;        break;
        case eHalf:        sGlyph = CHAR_HALF_REST;        break;
        case eQuarter:        sGlyph = CHAR_QUARTER_REST;        break;
        case eEighth:    sGlyph = CHAR_EIGHT_REST;        break;
        case e16th: sGlyph = CHAR_16TH_REST;        break;
        case e32th:        sGlyph = CHAR_32ND_REST;        break;
        case e64th:    sGlyph = CHAR_64TH_REST;        break;
        default:
            wxASSERT(false);
    }

    return sGlyph;
}

//====================================================================================================
// implementation of virtual methods defined in base abstract class lmNoteRest
//====================================================================================================

wxBitmap* lmRest::GetBitmap(double rScale)
{
    // Create the drag image.
    wxString sGlyph = GetLenMusChar();
    return PrepareBitMap(rScale, sGlyph);

}

void lmRest::DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC)
{
    /*
    This method is invoked by the base class (lmStaffObj). When reaching this point 
    paper cursor variable (m_paperPos) has been updated. This value must be used
    as the base for any measurement / drawing operation.

    DrawObject() method is responsible for:
    1. In DO_MEASURE phase (fMeasuring == true):
        - Compute the surrounding rectangle, the glyph position and other measurements
    2. In DO_DRAW phase (fMeasuring == false):
        - Render the object

    */


    // move to right staff
    lmMicrons nyTop = m_paperPos.y + GetStaffOffset() + 
                      m_pVStaff->TenthsToLogical(m_yShift, m_nStaffNum);
    lmMicrons nxLeft = m_paperPos.x;

    // prepare DC
    wxDC* pDC = pPaper->GetDC();
    wxASSERT(pDC);
    pDC->SetFont(*m_pFont);

    //if measurement phase and this is the first note/rest of a beam, measure beam
    if (fMeasuring && m_fBeamed && m_BeamInfo[0].Type == eBeamBegin) {
        m_pBeam->ComputeStems();
    }


    // Draw rest symbol
    //----------------------------------------------------------------------------------
//    if (fMeasuring) { m_xAnchor = CSng(xLeft)
//    xAncho = m_oPapel.PintarSilencio(fMeasuring, m_nNoteType, xLeft, yTop)
    wxString sGlyph = GetLenMusChar();
    if (fMeasuring) {

        // store position
        m_glyphPos.x = 0;
        m_glyphPos.y = nyTop - m_paperPos.y +
            m_pVStaff->TenthsToLogical( GetGlyphOffset(), m_nStaffNum );

        // store selection rectangle position and size
        lmMicrons nWidth, nHeight;
        pDC->GetTextExtent(sGlyph, &nWidth, &nHeight);
        m_selRect.width = nWidth;
        m_selRect.height = m_pVStaff->TenthsToLogical( GetSelRectHeight(), m_nStaffNum );
        m_selRect.x = m_glyphPos.x;
        m_selRect.y = m_glyphPos.y + m_pVStaff->TenthsToLogical( GetSelRectShift(), m_nStaffNum );

        // store total width
        lmMicrons afterSpace = m_pVStaff->TenthsToLogical(10, m_nStaffNum);    //one line space
        m_nWidth = nWidth + afterSpace;

    } else {
        // drawing phase: do the draw
        wxPoint pos = GetGlyphPosition();
        pDC->SetTextForeground((m_fSelected ? g_pColors->ScoreSelected() : colorC));
        pDC->DrawText(sGlyph, pos.x, pos.y );
    }
    nxLeft += m_selRect.width;

    //draw dots
    //------------------------------------------------------------
    if (m_fDotted || m_fDoubleDotted) {
        nxLeft += m_pVStaff->TenthsToLogical(4, m_nStaffNum);
        if (!fMeasuring) {
            //! @todo dot positioning. Shift seems to depend on rest type
            lmMicrons nDotRadius = m_pVStaff->TenthsToLogical(2, m_nStaffNum);
            lmMicrons yPos = m_glyphPos.y + m_paperPos.y; //nyTop + m_pVStaff->TenthsToLogical(15, m_nStaffNum);
            pDC->DrawCircle(nxLeft, yPos, nDotRadius);
        }
    }

    if (m_fDoubleDotted) {
        nxLeft += m_pVStaff->TenthsToLogical(4, m_nStaffNum);
        if (!fMeasuring) {
            //! @todo dot positioning and drawing
        }
    }

    // render associated notations ----------------------------------
    if (m_pNotations) {
        lmNoteRestObj* pNRO;
        wxAuxObjsListNode* pNode = m_pNotations->GetFirst();
        for (; pNode; pNode = pNode->GetNext() ) {
            pNRO = (lmNoteRestObj*)pNode->GetData();
            if (fMeasuring) {
                 lmMicrons xPos = 0;
                lmMicrons yPos = 0;
                switch(pNRO->GetSymbolType()) {
                    case eST_Fermata:
                        // set position (relative to paperPos)
                         xPos = m_selRect.x + m_selRect.width / 2;
                        yPos = nyTop - m_paperPos.y;
                        pNRO->SetSizePosition(pPaper, m_pVStaff, m_nStaffNum, xPos, yPos);
                        pNRO->UpdateMeasurements();
                        break;
                    default:
                        wxASSERT(false);
                }
            }
            else
                pNRO->Draw(DO_DRAW, pPaper, colorC);
        }
    }

    
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
wxString lmRest::SourceLDP()
{
    wxString sSource = _T("            (s ");    

    //! @todo Finish lmRest LDP Source code generation method

//    if (nCalderon == eC_ConCalderon) m_sFuente = m_sFuente & " c";
//    for (int i=1; i <= cAnotaciones.Count; i++) {
//        m_sFuente = m_sFuente & " " & cAnotaciones.Item(i);
//    }
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
// implementation of virtual methods defined in base class lmCompositeObj
//====================================================================================================

lmScoreObj* lmRest::FindSelectableObject(wxPoint& pt)
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

void lmRest::MoveDragImage(lmPaper* pPaper, wxDragImage* pDragImage, wxPoint& ptOffset, 
            const wxPoint& ptLog, const wxPoint& dragStartPosL, const wxPoint& ptPixels)
{
    lmScoreObj::MoveDragImage(pPaper, pDragImage, ptOffset, ptLog, dragStartPosL, ptPixels);
}

wxPoint lmRest::EndDrag(const wxPoint& pos)
{
    return lmScoreObj::EndDrag(pos);
}

//====================================================================================================
// implementation of virtual methods defined in base class lmScoreObj
//====================================================================================================

void lmRest::SetLeft(lmMicrons nLeft)
{
    lmScoreObj::SetLeft(nLeft);
}


