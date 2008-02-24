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


lmRest::lmRest(lmVStaff* pVStaff, lmENoteType nNoteType, float rDuration, bool fDotted, bool fDoubleDotted,
        int nStaff, int nVoice, bool fVisible, bool fBeamed, lmTBeamInfo BeamInfo[])
    : lmNoteRest(pVStaff, DEFINE_REST, nNoteType, rDuration, fDotted, fDoubleDotted,
                 nStaff, nVoice, fVisible)
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

lmUPoint lmRest::ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper)
{
	// if no location is specified in LDP source file, this method is invoked from
	// base class to ask derived object to compute a suitable position to
	// place itself.
	// uOrg is the assigned paper position for this object.

	lmUPoint uPos = uOrg;
	//TODO
	return uPos;
}

lmLUnits lmRest::LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)
{
    // This method is invoked by the base class (lmStaffObj). It is responsible for
    // creating the shape object and adding it to the graphical model.
    // Paper cursor must be used as the base for positioning.

    //get paper reference point
    lmUPoint uPaperPos(pPaper->GetCursorX(), pPaper->GetCursorY());

    // move to right staff
    lmLUnits uyTop = uPaperPos.y + GetStaffOffset();
    lmLUnits uxLeft = uPaperPos.x;

    // prepare DC
    pPaper->SetFont(*GetSuitableFont(pPaper));

    //create the container shape and add it to the box
    lmCompositeShape* pRestShape = new lmCompositeShape(this, _T("Rest"), lmDRAGGABLE);
	pBox->AddShape(pRestShape);
    m_pShape = pRestShape;

    ////if this is the first note/rest of a beam, create the beam
    ////AWARE This must be done before using stem information, as the beam could
    ////change stem direction if it is not determined for some/all the notes in the beam
    //if (m_fBeamed && m_BeamInfo[0].Type == eBeamBegin) {
    //    m_pBeam->CreateShape();
    //}

    // create shape for the rest symbol
    lmEGlyphIndex nGlyph = GetGlyphIndex();
    lmLUnits yPos = uyTop + m_pVStaff->TenthsToLogical( aGlyphsInfo[nGlyph].GlyphOffset , m_nStaffNum );
    lmShapeGlyph* pShape = new lmShapeGlyph(this, nGlyph, GetSuitableFont(pPaper), pPaper,
                                            lmUPoint(uxLeft, yPos), _T("Rest"));
	pRestShape->Add(pShape);
    uxLeft += pShape->GetWidth();

    //create shapes for dots if necessary
    //------------------------------------------------------------
    if (m_fDotted || m_fDoubleDotted)
	{
        //TODO user selectable
        lmLUnits uSpaceBeforeDot = m_pVStaff->TenthsToLogical(5, m_nStaffNum);
        uxLeft += uSpaceBeforeDot;
        lmLUnits uyPos = uyTop;
        uxLeft += AddDotShape(pRestShape, pPaper, uxLeft, uyPos, colorC);
        if (m_fDoubleDotted) {
            uxLeft += uSpaceBeforeDot;
            uxLeft += AddDotShape(pRestShape, pPaper, uxLeft, uyPos, colorC);
        }
    }

	return m_pShape->GetWidth();
}

void lmRest::DoVerticalShift(lmTenths yShift)
{
    // rests inside a group of beamed notes need to be shifted to align with the noteheads.
    // This method is invoked *after* the measurement phase. Therefore we have to
    // shift all already measured affected values

    //// compute shift in logical units
    //lmLUnits uShift = m_pVStaff->TenthsToLogical(yShift, m_nStaffNum);

    //// apply shift to rest object
    //m_uGlyphPos.y += uShift;
    //m_uSelRect.y += uShift;

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
        _T("%d\tRest\tType=%d, TimePos=%.2f, rDuration=%.2f, voice=%d "),
        m_nId, m_nNoteType, m_rTimePos, m_rDuration, m_nVoice );

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

    //Voice
    sSource += wxString::Format(_T(" v%d"), m_nVoice);

    //visible?
    if (!m_fVisible) { sSource += _T(" noVisible"); }

	//attached AuxObjs
	sSource += lmStaffObj::SourceLDP(nIndent+1);

    sSource += _T(")\n");
    return sSource;
}

wxString lmRest::SourceXML(int nIndent)
{
	wxString sSource = _T("");
	sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
    sSource += _T("TODO: lmRest XML Source code generation method\n");

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

