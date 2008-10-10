//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#include "Score.h"
#include "Staff.h"
#include "VStaff.h"
#include "Context.h"
#include "../sound/SoundManager.h"


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
    Create();
}

lmTimeSignature::lmTimeSignature(lmETimeSignature nTimeSign, lmVStaff* pVStaff, bool fVisible) :
    lmStaffObj(pVStaff, eSFOT_TimeSignature, pVStaff, 1, fVisible, lmDRAGGABLE)
{
    m_nType = eTS_Normal;
    m_nBeats = GetNumUnitsFromTimeSignType(nTimeSign);
    m_nBeatType = GetBeatTypeFromTimeSignType(nTimeSign);
    Create();
}

//constructor for types eTS_Common, eTS_Cut and eTS_SenzaMisura
lmTimeSignature::lmTimeSignature(lmETimeSignatureType nType, lmVStaff* pVStaff, bool fVisible) :
    lmStaffObj(pVStaff, eSFOT_TimeSignature, pVStaff, 1, fVisible, lmDRAGGABLE)
{
    m_nType = nType;
    Create();
    wxASSERT(false);    //TODO not yet implemented
}

//constructor for type eTS_SingleNumber
lmTimeSignature::lmTimeSignature(int nSingleNumber, lmVStaff* pVStaff, bool fVisible) :
    lmStaffObj(pVStaff, eSFOT_TimeSignature, pVStaff, 1, fVisible, lmDRAGGABLE)
{
    m_nType = eTS_SingleNumber;
    Create();
    wxASSERT(false);    //TODO not yet implemented
}

//constructor for type eTS_Composite
lmTimeSignature::lmTimeSignature(int nNumBeats, int nBeats[], int nBeatType, lmVStaff* pVStaff,
                             bool fVisible) :
    lmStaffObj(pVStaff, eSFOT_TimeSignature, pVStaff, 1, fVisible, lmDRAGGABLE)
{
    m_nType = eTS_Composite;
    Create();
   wxASSERT(false);    //TODO not yet implemented
}

//constructor for type eTS_Multiple
lmTimeSignature::lmTimeSignature(int nNumFractions, int nBeats[], int nBeatType[],
                             lmVStaff* pVStaff, bool fVisible) :
    lmStaffObj(pVStaff, eSFOT_TimeSignature, pVStaff, 1, fVisible, lmDRAGGABLE)
{
    m_nType = eTS_Multiple;
    Create();
    wxASSERT(false);    //TODO not yet implemented
}

void lmTimeSignature::Create()
{
    DefineAsMultiShaped();      //define time signature as multi-shaped ScoreObj

    //contexts and shapes
	for (int i=0; i < lmMAX_STAFF; i++)
	{
        m_pContext[i] = (lmContext*)NULL;
	}
}

wxString lmTimeSignature::Dump()
{
    //TODO take into account TimeSignatures types other than eTS_Normal
    wxString sDump = _T("");

    switch (m_nType) {
        case eTS_Normal :
            sDump = wxString::Format(
                _T("%d\tTime Sign. %d/%d\tTimePos=%.2f"),
                m_nId, m_nBeats, m_nBeatType, m_rTimePos );
            break;
        default:
            sDump = wxString::Format(
                _T("%d\tTime Sign. Type=%d\tTimePos=%.2f"),
                m_nId, m_nType, m_rTimePos );
    }

    //base class
    sDump += lmStaffObj::Dump();
    sDump += _T("\n");

    //contexts
    int nIndent = 5;
    for (int i=0; i < lmMAX_STAFF; i++)
    {
        if (m_pContext[i])
            sDump += m_pContext[i]->DumpContext(nIndent);
        else
        {
            sDump.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
            sDump += _T("Context: NULL\n");
        }
    }
    sDump += _T("\n");

    return sDump;
}

wxString lmTimeSignature::SourceLDP(int nIndent)
{
    wxString sSource = _T("");
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += wxString::Format(_T("(time %d %d"), m_nBeats, m_nBeatType);

	//base class
	sSource += lmStaffObj::SourceLDP(nIndent);

    //close element
    sSource += _T(")\n");
	return sSource;
}

wxString lmTimeSignature::SourceXML(int nIndent)
{
	//TODO
	wxString sSource = _T("");
	sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
    sSource += _T("TODO: TimeSignature\n");

	return sSource;
}

lmUPoint lmTimeSignature::ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper)
{
	// if no location is specified in LDP source file, this method is invoked from
	// base class to ask derived object to compute a suitable position to
	// place itself.
	// uOrg is the assigned paper position for this object.

	lmUPoint uPos = uOrg;
	//TODO
	return uPos;
}

lmLUnits lmTimeSignature::LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)
{
    // This method is invoked by the base class (lmStaffObj). It is responsible for
    // creating the shape object and adding it to the graphical model. 


    //get the position on which the time signature must be drawn
    lmLUnits uxPosTop = uPos.x;
    lmLUnits uxPosBottom = uPos.x;
	lmLUnits uyPosTop = uPos.y + m_pVStaff->GetStaffOffset(m_nStaffNum)
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

    //Time signature is common to all lmVStaff staves of the instrument, but the lmStaffObj
    //representing it is only present in the first staff. Therefore, for renderization, it
    //is necessary to repeat the shape in each staff of the instrument
    //So in the following loop we add a time signature shape for each VStaff of the
    //instrument
    lmLUnits yOffset = 0;
    lmStaff* pStaff = m_pVStaff->GetFirstStaff();
    for (int nStaff=1; pStaff; pStaff = m_pVStaff->GetNextStaff(), nStaff++)
    {
        // Add the shape for time signature
        CreateShape(nStaff-1, pBox, pPaper, colorC, sTopGlyphs, uxPosTop, uyPosTop + yOffset,
					sBottomGlyphs, uxPosBottom, uyPosBottom + yOffset);

        //compute vertical displacement for next staff
        yOffset += pStaff->GetHeight();
        yOffset += pStaff->GetAfterSpace();
    }

	// set total width (incremented in one line for after space)
	return GetShape(1)->GetWidth() + m_pVStaff->TenthsToLogical(10, m_nStaffNum);

}

lmShape* lmTimeSignature::CreateShape(int nShapeIdx, lmBox* pBox, lmPaper* pPaper,
                                      wxColour colorC,
                                      wxString& sTopGlyphs,
                                      lmLUnits uxPosTop, lmLUnits uyPosTop,
                                      wxString& sBottomGlyphs,
                                      lmLUnits uxPosBottom, lmLUnits uyPosBottom)
{
    if (!m_fVisible)
        return CreateInvisibleShape(pBox, lmUPoint(uxPosTop, uyPosTop), nShapeIdx);


	//create the shape object
    lmCompositeShape* pShape = new lmCompositeShape(this, nShapeIdx, _T("Time signature"),
                                                    lmDRAGGABLE);
	pBox->AddShape(pShape);
    StoreShape(pShape);

	//loop to create glyphs for the top number
	long nDigit;
	for (int i=0; i < (int)sTopGlyphs.length(); i++)
	{
		wxString sDigit = sTopGlyphs.substr(i, 1);
		sDigit.ToLong(&nDigit);
		int nGlyph = GLYPH_NUMBER_0 + (int)nDigit;
		lmLUnits uyPos = uyPosTop 
						 + m_pVStaff->TenthsToLogical(aGlyphsInfo[nGlyph].GlyphOffset, m_nStaffNum );
        pShape->Add(new lmShapeGlyph(this, -1, nGlyph, GetSuitableFont(pPaper), pPaper,
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
		pShape->Add(new lmShapeGlyph(this, -1, nGlyph, GetSuitableFont(pPaper), pPaper,
									 lmUPoint(uxPosBottom, uyPos), 
									 _T("BeatType"), lmNO_DRAGGABLE) );
		uxPosBottom += m_pVStaff->TenthsToLogical(aGlyphsInfo[nGlyph].thWidth, m_nStaffNum );
	}

    return pShape;
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

void lmTimeSignature::StoreOriginAndShiftShapes(lmLUnits uxShift, int nShapeIdx)
{
 //   //This method is invoked only from TimeposTable module, from methods 
 //   //lmTimeLine::ShiftEntries() and lmTimeLine::Reposition(), during auto-layout
 //   //computations.
 //   //By invoking this method, the auto-layout algorithm is informing about a change in
 //   //the computed final position for this ScoreObj.
 //   //Take into account that this method can be invoked several times for the
 //   //same ScoreObj, when the auto-layout algorithm refines the final position.

    lmStaffObj::StoreOriginAndShiftShapes(uxShift, nShapeIdx);

}

void lmTimeSignature::RemoveCreatedContexts()
{
    //delete contexts
	for (int iS=0; iS < m_pVStaff->GetNumStaves(); iS++)
	{
        if (m_pContext[iS])
        {
            lmStaff* pStaff = m_pVStaff->GetStaff(iS+1);
            pStaff->RemoveContext(m_pContext[iS], this);
            delete m_pContext[iS];
            m_pContext[iS] = (lmContext*)NULL;
        }
	}
}

float lmTimeSignature::GetMeasureDuration()
{ 
    return m_nBeats * GetBeatDuration(m_nBeatType);
}




//----------------------------------------------------------------------------------------
// global functions related to TimeSignatures
//----------------------------------------------------------------------------------------

//! returns the numerator of time signature fraction
int GetNumUnitsFromTimeSignType(lmETimeSignature nTimeSign)
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

int GetNumBeatsFromTimeSignType(lmETimeSignature nTimeSign)
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

int GetBeatTypeFromTimeSignType(lmETimeSignature nTimeSign)
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
float GetBeatDuration(lmETimeSignature nTimeSign)
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
            return pow(2, (10 - eEighth));
        case 16:
            return pow(2, (10 - e16th));
        default:
            wxASSERT(false);
            return 0;     //compiler happy
    }
}

//! Returns the required duration for a measure in the received time signature
float GetMeasureDuration(lmETimeSignature nTimeSign)
{
    float rNumBeats = (float)GetNumBeatsFromTimeSignType(nTimeSign);
    return rNumBeats * GetBeatDuration(nTimeSign);
}

//bool IsBinaryTimeSignature(lmETimeSignature nTimeSign)
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
    // Some times it is necessary to know if a note/rest sounds in beat part.
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
