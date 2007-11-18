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

#ifdef __GNUG__
#pragma implementation "TimeSignature.h"
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

//-------------------------------------------------------------------------------------------------
// lmTimeSignature object implementation
//-------------------------------------------------------------------------------------------------

//
//constructors and destructor
//

//constructors for type eTS_Normal
lmTimeSignature::lmTimeSignature(int nBeats, int nBeatType, lmVStaff* pVStaff, bool fVisible) :
    lmStaffObj(pVStaff, eSFOT_TimeSignature, pVStaff, 1, fVisible, lmDRAGGABLE)
{
    m_nType = eTS_Normal;
    m_nBeats = nBeats;
    m_nBeatType = nBeatType;
}

lmTimeSignature::lmTimeSignature(ETimeSignature nTimeSign, lmVStaff* pVStaff, bool fVisible) :
    lmStaffObj(pVStaff, eSFOT_TimeSignature, pVStaff, 1, fVisible, lmDRAGGABLE)
{
    m_nType = eTS_Normal;
    m_nBeats = GetNumUnitsFromTimeSignType(nTimeSign);
    m_nBeatType = GetBeatTypeFromTimeSignType(nTimeSign);
}

//constructor for types eTS_Common, eTS_Cut and eTS_SenzaMisura
lmTimeSignature::lmTimeSignature(ETimeSignatureType nType, lmVStaff* pVStaff, bool fVisible) :
    lmStaffObj(pVStaff, eSFOT_TimeSignature, pVStaff, 1, fVisible, lmDRAGGABLE)
{
    m_nType = nType;
    wxASSERT(false);    //TODO not yet implemented
}

//constructor for type eTS_SingleNumber
lmTimeSignature::lmTimeSignature(int nSingleNumber, lmVStaff* pVStaff, bool fVisible) :
    lmStaffObj(pVStaff, eSFOT_TimeSignature, pVStaff, 1, fVisible, lmDRAGGABLE)
{
    m_nType = eTS_SingleNumber;
    wxASSERT(false);    //TODO not yet implemented
}

//constructor for type eTS_Composite
lmTimeSignature::lmTimeSignature(int nNumBeats, int nBeats[], int nBeatType, lmVStaff* pVStaff,
                             bool fVisible) :
    lmStaffObj(pVStaff, eSFOT_TimeSignature, pVStaff, 1, fVisible, lmDRAGGABLE)
{
    m_nType = eTS_Composite;
    wxASSERT(false);    //TODO not yet implemented
}

//constructor for type eTS_Multiple
lmTimeSignature::lmTimeSignature(int nNumFractions, int nBeats[], int nBeatType[],
                             lmVStaff* pVStaff, bool fVisible) :
    lmStaffObj(pVStaff, eSFOT_TimeSignature, pVStaff, 1, fVisible, lmDRAGGABLE)
{
    m_nType = eTS_Multiple;
    wxASSERT(false);    //TODO not yet implemented
}

wxString lmTimeSignature::Dump()
{
    //TODO take into account TimeSignatures types other than eTS_Normal
    wxString sDump = _T("");

    switch (m_nType) {
        case eTS_Normal :
            sDump = wxString::Format(
                _T("%d\tTime Sign. %d/%d\tTimePos=%.2f\n"),
                m_nId, m_nBeats, m_nBeatType, m_rTimePos );
            break;
        default:
            sDump = wxString::Format(
                _T("%d\tTime Sign. Type=%d\tTimePos=%.2f\n"),
                m_nId, m_nType, m_rTimePos );
    }
    return sDump;
}

wxString lmTimeSignature::SourceLDP(int nIndent)
{
    wxString sSource = _T("");
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += wxString::Format(_T("(time %d %d"), m_nBeats, m_nBeatType);

    //visible?
    if (!m_fVisible) { sSource += _T(" noVisible"); }

    sSource += _T(")\n");
    return sSource;

}

wxString lmTimeSignature::SourceXML()
{
	//TODO
    return _T("");
}

void lmTimeSignature::LayoutObject(lmBox* pBox, lmPaper* pPaper, wxColour colorC)
{
    // This method is invoked by the base class (lmStaffObj). It is responsible for
    // creating the shape object and adding it to the graphical model. 
    // Paper cursor must be used as the base for positioning.


    //get the position on which the time signature must be drawn
    lmLUnits uxPosTop = pPaper->GetCursorX();
    lmLUnits uxPosBottom = pPaper->GetCursorX();
	lmLUnits uyPosTop = pPaper->GetCursorY() + m_pVStaff->GetStaffOffset(m_nStaffNum)
						//- m_pVStaff->TenthsToLogical(aGlyphsInfo[GLYPH_NUMBER_0].GlyphOffset, m_nStaffNum );
						- m_pVStaff->TenthsToLogical(20, m_nStaffNum);
    lmLUnits uyPosBottom = uyPosTop + m_pVStaff->TenthsToLogical(20, m_nStaffNum);

    //compute Beats and BeatsType positions so that one is centered on the other
    wxString sTopGlyphs = wxString::Format(_T("%d"), m_nBeats );
    wxString sBottomGlyphs = wxString::Format(_T("%d"), m_nBeatType );
    lmLUnits uWidth1, uHeight1, uWidth2, uHeight2;
    pPaper->GetTextExtent(sTopGlyphs, &uWidth1, &uHeight1);
    pPaper->GetTextExtent(sBottomGlyphs, &uWidth2, &uHeight2);

    if (uWidth2 > uWidth1) {
        //bottom number wider
        uxPosTop += (uWidth2 - uWidth1) / 2;
    }
    else {
        //bottom number wider
        uxPosBottom += (uWidth1 - uWidth2) / 2;
    }

	//create the shape object
    lmCompositeShape* pShape = new lmCompositeShape(this, _T("Time signature"));
	pBox->AddShape(pShape);
    m_pShape2 = pShape;

    //pShape->Add(new lmShapeTex2(this, sTopGlyphs, GetFont(), pPaper,
    //                            lmUPoint(uxPosTop, uyPosTop), 
				//				_T("Beats"), lmNO_DRAGGABLE) );

    //pShape->Add(new lmShapeTex2(this, sBottomGlyphs, GetFont(), pPaper,
    //                            lmUPoint(uxPosBottom, uyPosBottom), 
				//				_T("BeatType"), lmNO_DRAGGABLE) );

	//loop to create glyphs for the top number
	long nDigit;
	for (int i=0; i < (int)sTopGlyphs.length(); i++)
	{
		wxString sDigit = sTopGlyphs.substr(i, 1);
		sDigit.ToLong(&nDigit);
		int nGlyph = GLYPH_NUMBER_0 + (int)nDigit;
		lmLUnits uyPos = uyPosTop 
						 + m_pVStaff->TenthsToLogical(aGlyphsInfo[nGlyph].GlyphOffset, m_nStaffNum );
		pShape->Add(new lmShapeGlyp2(this, nGlyph, GetFont(), pPaper,
									 lmUPoint(uxPosTop, uyPos), 
									 _T("Beats"), lmNO_DRAGGABLE) );
		uxPosTop += m_pVStaff->TenthsToLogical(aGlyphsInfo[nGlyph].thWidth, m_nStaffNum );
	}

	//loop to create glyphs for the bottom number
	for (int i=0; i < (int)sBottomGlyphs.length(); i++)
	{
		wxString sDigit = sBottomGlyphs.substr(i, 1);
		sDigit.ToLong(&nDigit);
		int nGlyph = GLYPH_NUMBER_0 + (int)nDigit;
		lmLUnits uyPos = uyPosBottom 
						+ m_pVStaff->TenthsToLogical(aGlyphsInfo[nGlyph].GlyphOffset, m_nStaffNum );
		pShape->Add(new lmShapeGlyp2(this, nGlyph, GetFont(), pPaper,
									 lmUPoint(uxPosBottom, uyPos), 
									 _T("BeatType"), lmNO_DRAGGABLE) );
		uxPosBottom += m_pVStaff->TenthsToLogical(aGlyphsInfo[nGlyph].thWidth, m_nStaffNum );
	}
	// set total width (incremented in one line for after space)
	m_uWidth = pShape->GetWidth() + m_pVStaff->TenthsToLogical(10, m_nStaffNum);

}

// returns the width of the draw (logical units)
lmLUnits lmTimeSignature::DrawTimeSignature(bool fMeasuring, lmPaper* pPaper, wxColour colorC)
{
    pPaper->SetFont(*m_pFont);

    wxString sTopGlyphs = wxString::Format(_T("%d"), m_nBeats );
    wxString sBottomGlyphs = wxString::Format(_T("%d"), m_nBeatType );

    if (fMeasuring) {
        lmLUnits nWidth1, nHeight1, nWidth2, nHeight2;
        pPaper->GetTextExtent(sTopGlyphs, &nWidth1, &nHeight1);
        pPaper->GetTextExtent(sBottomGlyphs, &nWidth2, &nHeight2);

        // store selection rectangle measures and position (relative to m_uPaperPos)
        m_uSelRect.width = wxMax(nWidth1, nWidth2);
        m_uSelRect.height = m_pVStaff->TenthsToLogical( 40, m_nStaffNum );
        m_uSelRect.x = m_uGlyphPos.x;
        m_uSelRect.y = m_uGlyphPos.y + m_pVStaff->TenthsToLogical( 40, m_nStaffNum );

        //compute Beats and BeatsType positions so that one is centered on the other
        if (nWidth2 > nWidth1) {
            //bottom number wider
            m_xPosTop = (nWidth2 - nWidth1) / 2;
            m_xPosBottom = 0;
        }
        else {
            //bottom number wider
            m_xPosTop = 0;
            m_xPosBottom = (nWidth1 - nWidth2) / 2;
        }

        // set total width (incremented in one line for after space)
        m_uWidth = m_uSelRect.width + m_pVStaff->TenthsToLogical(10, m_nStaffNum);    //one line space
        return m_uWidth;
    }
    else {
        //Time signature is common to all lmVStaff staves, but it is only present, as lmStaffObj, in
        //the first staff. Therefore, for renderization, it is necessary to repeat it for
        //each staff
        //wxLogMessage(_T("[lmTimeSignature::DrawTimeSignature]"));
        pPaper->SetTextForeground(colorC);
        lmLUnits yOffset = 0;
        lmStaff* pStaff = m_pVStaff->GetFirstStaff();
        for (int nStaff=1; pStaff; pStaff = m_pVStaff->GetNextStaff(), nStaff++) {
            // Draw the time signature
            lmUPoint uPos = GetGlyphPosition();
            pPaper->DrawText(sTopGlyphs, uPos.x + m_xPosTop, uPos.y + yOffset );
            pPaper->DrawText(sBottomGlyphs, uPos.x + m_xPosBottom,
                            uPos.y + yOffset + m_pVStaff->TenthsToLogical( 20, nStaff ) );

            //compute vertical displacement for next staff
            yOffset += pStaff->GetHeight();
            yOffset += pStaff->GetAfterSpace();
        }
        return 0;
    }

}

void lmTimeSignature::AddMidiEvent(lmSoundManager* pSM, float rMeasureStartTime, int nMeasure)
{
    //  Add a MIDI event of type RhythmChange
    //TODO Deal with non-standard time signatures

    float rTime = m_rTimePos + rMeasureStartTime;

    //transform beat type into duration in LDP notes duration units
    int nBeatDuration = (int)GetBeatDuration(m_nBeatType);

    //add the RhythmChange event
    pSM->StoreEvent( rTime, eSET_RhythmChange, 0, m_nBeats, 0, nBeatDuration, this, nMeasure);

}

//TODO Code all following methods needed for dragging and selection rectangle

wxBitmap* lmTimeSignature::GetBitmap(double rScale)
{
    //TODO
    return (wxBitmap*)NULL;
}

void lmTimeSignature::OnDrag(lmPaper* pPaper, wxDragImage* pDragImage, lmDPoint& ptOffset,
                        const lmUPoint& ptLog, const lmUPoint& uDragStartPos, const lmDPoint& ptPixels)
{
}

lmUPoint lmTimeSignature::EndDrag(const lmUPoint& uPos)
{
    //TODO
    return lmUPoint(0,0);
}

lmLUnits lmTimeSignature::DrawAt(bool fMeasuring, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)
{
    //TODO
    return 0;
}

lmTenths lmTimeSignature::GetSelRectHeight()
{
    //TODO
    return 0;
}

lmTenths lmTimeSignature::GetSelRectShift()
{
    //TODO
    return 0;
}

lmTenths lmTimeSignature::GetGlyphOffset()
{
    //TODO
    return 0;
}

wxString lmTimeSignature::GetLenMusChar()
{
    //TODO
    return _T("4/4");
}


//----------------------------------------------------------------------------------------
// global functions related to TimeSignatures
//----------------------------------------------------------------------------------------

//! returns the numerator of time signature fraction
int GetNumUnitsFromTimeSignType(ETimeSignature nTimeSign)
{
    switch (nTimeSign) {
        case emtr24:
            return 2;
        case emtr34:
            return 3;
        case emtr44:
            return 4;
        case emtr28:
            return 2;
        case emtr38:
            return 3;
        case emtr22:
            return 2;
        case emtr32:
            return 3;
        case emtr68:
            return 6;
        case emtr98:
            return 9;
        case emtr128:
            return 12;
        default:
            wxASSERT(false);
            return 4;
    }
}

int GetNumBeatsFromTimeSignType(ETimeSignature nTimeSign)
{
    switch (nTimeSign) {
        case emtr24:
            return 2;
        case emtr34:
            return 3;
        case emtr44:
            return 4;
        case emtr28:
            return 1;
        case emtr38:
            return 1;
        case emtr22:
            return 2;
        case emtr32:
            return 3;
        case emtr68:
            return 2;
        case emtr98:
            return 3;
        case emtr128:
            return 4;
        default:
            wxASSERT(false);
            return 4;
    }
}

int GetBeatTypeFromTimeSignType(ETimeSignature nTimeSign)
{
    switch (nTimeSign) {
        case emtr24:
        case emtr34:
        case emtr44:
            return 4;

        case emtr28:
        case emtr38:
        case emtr68:
        case emtr98:
        case emtr128:
            return 8;

        case emtr22:
        case emtr32:
            return 2;

        default:
            wxASSERT(false);
            return 4;
    }
}

//! returns beat duration (in LDP notes duration units)
float GetBeatDuration(ETimeSignature nTimeSign)
{
    int nBeatType = GetBeatTypeFromTimeSignType(nTimeSign);
    return GetBeatDuration(nBeatType);
}

//! returns beat duration (in LDP notes duration units)
float GetBeatDuration(int nBeatType)
{
    switch(nBeatType) {
        case 1:
            return pow(2, (10 - eWhole));
        case 2:
            return pow(2, (10 - eHalf));
        case 4:
            return pow(2, (10 - eQuarter));
        case 8:
            return (1.5 * pow(2, (10 - eQuarter)) );
        case 16:
            return pow(2, (10 - eEighth));
        default:
            wxASSERT(false);
            return 0;     //compiler happy
    }
}

//! Returns the required duration for a measure in the received time signature
float GetMeasureDuration(ETimeSignature nTimeSign)
{
    float rNumBeats = (float)GetNumBeatsFromTimeSignType(nTimeSign);
    return rNumBeats * GetBeatDuration(nTimeSign);
}

//bool IsBinaryTimeSignature(ETimeSignature nTimeSign)
//{
//    switch (nTimeSign) {
//        case emtr24:
//        case emtr34:
//        case emtr44:
//        case emtr28:
//        case emtr22:
//        case emtr32:
//            return true;
//        default:
//            return false;
//    }
//
//}

int AssignVolume(float rTimePos, int nBeats, int nBeatType)
{
    //Volume should depend on beak (strong, medium, weak) on which a note
    //is placed. This method receives the time for a note and the current time signature
    //and return the volume to assign to it

    int nPos = GetNoteBeatPosition(rTimePos, nBeats, nBeatType);

    int nVolume = 60;       // volume for off-beat notes

    if (nPos == 0)
        //on-beat notes on first beat
        nVolume = 85;
    else if (nPos > 0)
        //on-beat notes on other beats
        nVolume = 75;
    else
        // off-beat notes
        nVolume = 60;

    return nVolume;

}


int GetNoteBeatPosition(float rTimePos, int nBeats, int nBeatType)
{
    // Some times it is necessary to know the type of beak (strong, medium, weak, off-beat)
    // at which a note or rest is positioned.
    // This function receives the time for a note/rest and the current time signature
    //  and return the type of beat

    // coumpute beat duration
    int nBeatDuration;
    switch (nBeatType) {
        case 1: nBeatDuration = (int)eWholeDuration; break;
        case 2: nBeatDuration = (int)eHalfDuration; break;
        case 4: nBeatDuration = (int)eQuarterDuration; break;
        case 8: nBeatDuration = 3* (int)eEighthDuration; break;
        case 16: nBeatDuration = (int)e16thDuration; break;
        default:
            wxLogMessage(_T("[GetPositionBeatType] BeatType %d unknown."), nBeatType);
            wxASSERT(false);
    }

    // compute relative position of this note/rest with reference to the beat
    int nBeatNum = (int)rTimePos / nBeatDuration;               //number of beat
    float rBeatShift = fabs(rTimePos - (float)(nBeatDuration * nBeatNum));

    if (rBeatShift < 1.0)
        //on-beat
        return nBeatNum;
    else
        // off-beat
        return lmOFF_BEAT;

}

int GetChordPosition(float rTimePos, float rDuration, int nBeats, int nBeatType) 
{
    // Some times it is necessary to know a note/rest sounds in beat part.
    // This method receives the time for a note/rest and the current time signature
    // and returns the beat number if the note sounds in beat part (starts in beat or 
    // is sounding at beat time, or returns -1 (lmNON_CHORD_NOTE) if non-chord note

    // coumpute beat duration
    int nBeatDuration;
    switch (nBeatType) {
        case 1: nBeatDuration = (int)eWholeDuration; break;
        case 2: nBeatDuration = (int)eHalfDuration; break;
        case 4: nBeatDuration = (int)eQuarterDuration; break;
        case 8: nBeatDuration = 3* (int)eEighthDuration; break;
        case 16: nBeatDuration = (int)e16thDuration; break;
        default:
            wxLogMessage(_T("[GetPositionBeatType] BeatType %d unknown."), nBeatType);
            wxASSERT(false);
    }

    // compute start and end time
    // Duration: quarter = 64, eighth = 32
    // Example. 3/4 time, quarter note starting on third beat:
    //      rTimePos= 128
    //      nStartBeat = 128 / 64 = 2
    //      rStartShift = 128 - 2*64 = 0  --> starts on beat
    //
    // Example. 3/4 time, off-baet quarter note starting before third beat (rTimePos=96)
    //      rTimePos = 64+32= 96
    //      nStartBeat = 96/64 = 1
    //      rStartShift = 96 - 1*64 = 32 ==> Doesn't start on beat
    //      rEndTimePos = 96+64 = 160
    //      nEndBeat = 160/64 = 2
    //      rEndShift = 160 - 64*2 = 160-128 = 32

    // compute relative position of this note/rest with reference to the beat
    int nStartBeat = (int)rTimePos / nBeatDuration;               //start beat number (minus 1)
    float rStartShift = fabs(rTimePos - (float)(nBeatDuration * nStartBeat));

    float rEndTimePos = rTimePos + rDuration;
    int nEndBeat = (int)rEndTimePos /nBeatDuration;
    float rEndShift = fabs(rEndTimePos - (float)(nBeatDuration * nEndBeat));

    // note is on chord if it starts on beat or start point on beat N and end point on beat N+1.
    // 1.0 is the duration of a 256th note. I use 1.0 instead of e256thDuration to avoid
    // conversions
    if (rStartShift < 1.0 )
        return nStartBeat;
    else if (nStartBeat != nEndBeat && rEndShift > 1.0)
        return nStartBeat+1;    //AWARE: The note might last for many beats. So nEndBeat is
                                // not the right answer.
    else
        return lmNON_CHORD_NOTE;

}
