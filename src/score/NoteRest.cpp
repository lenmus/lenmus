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
#pragma implementation "NoteRest.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#else
#include <wx/debug.h>
#endif

#include <math.h>

#include "Score.h"
#include "VStaff.h"
#include "../ldp_parser/AuxString.h"
#include "../sound/SoundManager.h"


//====================================================================================================
//Constructors and destructors
//====================================================================================================

lmNoteRest::lmNoteRest(lmVStaff* pVStaff, long nID, bool fIsRest, lmENoteType nNoteType,
                       float rDuration, int nNumDots, int nStaff, int nVoice, bool fVisible)
    : lmStaffObj(pVStaff, nID, (fIsRest ? lm_eSO_Rest : lm_eSO_Note), pVStaff, nStaff,
                 fVisible, lmDRAGGABLE)
    , m_nNoteType(nNoteType)
    , m_nNumDots(nNumDots)
    //, m_fIsRest(IsRest)
	, m_nVoice(nVoice)
    , m_pNotations((AuxObjsList*)NULL)
    , m_pLyrics((AuxObjsList*)NULL)
    , m_rDuration(rDuration)
    , m_pTuplet((lmTupletBracket*)NULL)
    , m_pBeam((lmBeam*)NULL)
{
    SetLayer(lm_eLayerNotes);
}

lmNoteRest::~lmNoteRest()
{
    //Remove the associated AuxObjs
    if (m_pNotations) {
        m_pNotations->DeleteContents(true);
        m_pNotations->Clear();
        delete m_pNotations;
        m_pNotations = (AuxObjsList*)NULL;
    }
    if (m_pLyrics) {
        m_pLyrics->DeleteContents(true);
        m_pLyrics->Clear();
        delete m_pLyrics;
        m_pLyrics = (AuxObjsList*)NULL;
    }

    //if note/rest to remove is in a tuplet, inform the tuplet
	if (m_pTuplet) {
        m_pTuplet->Remove(this);
		if (m_pTuplet->NumNotes() <= 1)
			delete m_pTuplet;
	}

    //if note/rest to remove is in a beam, inform the beam
	if (m_pBeam) {
        m_pBeam->Remove(this);
        DetachAuxObj(m_pBeam);
		if (m_pBeam->NumNotes() <= 1)
			delete m_pBeam;
	}
}

void lmNoteRest::CreateBeam(bool fBeamed, lmTBeamInfo BeamInfo[])
{
    //This method is used when loading a file.
    //Set up beaming information

	static lmBeam* pCurBeam = (lmBeam*)NULL;        // no beam open


    if (!fBeamed)
	{
        m_pBeam = (lmBeam*)NULL;
    }
    else
	{
        for (int i=0; i < 6; i++)
		{
            m_BeamInfo[i] = BeamInfo[i];
        }
        if (m_BeamInfo[0].Type == eBeamBegin)
		{
            m_pBeam = new lmBeam((lmNote*)this);
            pCurBeam = m_pBeam;
        }
		else 
		{
            m_pBeam = pCurBeam;
            if (!m_pBeam) {
                //TODO Show message. Error: ¡se pide finalizar un grupo que no ha sido abierto!
                fBeamed = false;
            }
			else
			{
                if (IsRest())
                    m_pBeam->Include(this);
                else {
                    if (!((lmNote*)this)->IsInChord()) m_pBeam->Include(this);
                }
                if (m_BeamInfo[0].Type == eBeamEnd) {
                        //m_pBeam->CreateShape();
                    //AWARE with this note/rest the beaming ends. But it it not yet posible to
                    //compute beaming information as there could be more notes to add in
                    //chord to this note. Due to this, the computation of stems has
                    //been delayed to the layout phase, when layouting the first note of
                    //the beam.
                    pCurBeam = (lmBeam*)NULL;        // no beam open
                }
            }
        }
    }
}

lmBeam* lmNoteRest::IncludeOnBeam(lmEBeamType nBeamType, lmBeam* pBeam)
{
    //this method is used in the score editor, when the beam has been created and this
    //note/rest must be included on it. Previously, any current beam has been removed.
    //pBeam is only valid when nBeamType is not eBeamBegin

    SetDirty(true);

    //set up basic beaming information
    for (int i=0; i < 6; i++) {
        m_BeamInfo[i].Repeat = false;
        m_BeamInfo[i].Type = eBeamNone;
    }
    m_BeamInfo[0].Type = nBeamType;         //eBeamBegin, eBeamContinue or eBeamEnd

    //save beam and include this note/rest on it
    if (nBeamType == eBeamBegin)
    {
        wxASSERT (this->IsNote());
        m_pBeam = new lmBeam((lmNote*)this);
    }
    else
    {
        m_pBeam = pBeam;
        m_pBeam->Include(this);
    }

    return m_pBeam;
}

void lmNoteRest::OnIncludedInRelationship(void* pRel, lmERelationshipClass nRelClass)
{	
	switch (nRelClass)
	{
		case lm_eBeamClass:
			m_pBeam = (lmBeam*)pRel;
			return;

		case lm_eTupletClass:
			m_pTuplet = (lmTupletBracket*)pRel;
			return;

		default:
			wxASSERT(false);
	}
}

void lmNoteRest::OnRemovedFromRelationship(void* pRel,
                                           lmERelationshipClass nRelClass)
{ 
	//AWARE: this method is invoked only when the relationship is being deleted and
	//this deletion is not requested by this note/rest. If this note/rest would like
	//to delete the relationship it MUST invoke Remove(this) before deleting the 
	//relationship object

    SetDirty(true);

	switch (nRelClass)
	{
		case lm_eTupletClass:
			m_pTuplet = (lmTupletBracket*)NULL;
			return;

		default:
			wxASSERT(false);
	}
}

void lmNoteRest::OnIncludedInRelationship(lmRelObj* pRel)
{
    SetDirty(true);

	if (pRel->IsBeam())
    {
        m_pBeam = (lmBeam*)pRel;
        AttachAuxObj(m_pBeam);
    }
    else
        wxASSERT(false);
}

void lmNoteRest::OnRemovedFromRelationship(lmRelObj* pRel)
{
	//AWARE: this method is invoked only when the relationship is being deleted and
	//this deletion is not requested by this note/rest. If this note/rest would like
	//to delete the relationship it MUST invoke Remove(this) before deleting the 
	//relationship object

    SetDirty(true);

	if (pRel->IsTie())
    {
        ((lmNote*)this)->OnRemovedFromRelationship(pRel);
    }
	else if (pRel->IsBeam())
    {
        DetachAuxObj(m_pBeam);
        m_pBeam = (lmBeam*)NULL;
    }
    else
        wxASSERT(false);
}

lmLUnits lmNoteRest::AddDotShape(lmCompositeShape* pCS, lmPaper* pPaper,
                                 lmLUnits xPos, lmLUnits yPos, wxColour colorC)
{
    //Creates a shape for a dot and adds it to the composite shape
    //returns the width of the dot

    yPos += m_pVStaff->TenthsToLogical(50, m_nStaffNum);
    yPos += m_pVStaff->TenthsToLogical(aGlyphsInfo[GLYPH_DOT].GlyphOffset, m_nStaffNum);
    lmShapeGlyph* pShape = new lmShapeGlyph(this, -1, GLYPH_DOT, pPaper, lmUPoint(xPos, yPos),
                                            _T("Dot"), lmDRAGGABLE, colorC);
	pCS->Add(pShape);
    return pShape->GetBounds().GetWidth();

}

wxString lmNoteRest::Dump()
{
    //Voice
    wxString sDump = wxString::Format(_T(", voice=%d"), m_nVoice);

	//beam
    if (m_pBeam) {
        sDump += wxString::Format(_T(", Beamed: BeamTypes(%d"), m_BeamInfo[0].Type);
        for (int i=1; i < 6; i++) {
            sDump += wxString::Format(_T(",%d"), m_BeamInfo[i].Type);
        }
        sDump += _T(")");
    }

	//tuplet
	if (m_pTuplet) {
        if (m_pTuplet->GetEndNoteRest() == this) {
            sDump += _T(", End of tuplet");
        }
        else if (m_pTuplet->GetStartNoteRest() == this)
            sDump += _T(", Start of tuplet");
        else
            sDump += _T(", In tuplet");
    }

    //// Dump associated lyrics
    //if (m_pLyrics) {
    //    lmLyric* pLyric;
    //    wxAuxObjsListNode* pNode = m_pLyrics->GetFirst();
    //    for (; pNode; pNode = pNode->GetNext() ) {
    //        pLyric = (lmLyric*)pNode->GetData();
    //        sDump += pLyric->Dump();
    //    }
    //}

    //base class information
	sDump += lmStaffObj::Dump();
    sDump += _T("\n");

    return sDump;

}

wxString lmNoteRest::SourceLDP(int nIndent, bool fUndoData)
{
    wxString sSource = _T("");

    //tuplets
    if (m_pTuplet)
    {
        if ((lmNoteRest*)this == m_pTuplet->GetStartNoteRest())
            sSource += m_pTuplet->SourceLDP();
        else if((lmNoteRest*)this == m_pTuplet->GetEndNoteRest())
            sSource += _T(" t-");
    }

    //Voice
    sSource += wxString::Format(_T(" v%d"), m_nVoice);

	//base class
	wxString sBase = lmStaffObj::SourceLDP(nIndent, fUndoData);
    if (sBase != _T(""))
    {
        sSource += sBase;
        sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    }

    return sSource;
}

wxString lmNoteRest::SourceXML(int nIndent)
{
    //TODO
    wxString sSource = _T("");

	//base class
	wxString sBase = lmStaffObj::SourceXML(nIndent);
    if (sBase != _T(""))
        sSource += sBase;

    return sSource;
}


//====================================================================================================
// methods related to associated AuxObjs management
//====================================================================================================
lmFermata* lmNoteRest::AddFermata(const lmEPlacement nPlacement, long nID)
{
    SetDirty(true);

    if (!m_pNotations) m_pNotations = new AuxObjsList();

    lmFermata* pFermata = new lmFermata(this, nID, nPlacement);
	AttachAuxObj(pFermata);
    return pFermata;
}

void lmNoteRest::AddLyric(lmLyric* pLyric)
{
    SetDirty(true);

    if (!m_pLyrics) m_pLyrics = new AuxObjsList();

    pLyric->SetOwner(this);
    m_pLyrics->Append(pLyric);

}

void lmNoteRest::AddMidiEvents(lmSoundManager* pSM, float rMeasureStartTime, int nChannel,
                             int nMeasure)
{
    // Coumpute MIDI events for this lmNoteRest and add them to the lmSoundManager
    // object received as parameter.

    //Generate Note ON event
    float rTime = m_rTimePos + rMeasureStartTime;
    if (IsRest()) {
        //Generate only event for visual highlight
        pSM->StoreEvent( rTime, eSET_VisualON, nChannel, 0, 0, 0, this, nMeasure);
    }
    else {
        //It is a note. Generate Note On event
        lmNote* pN = (lmNote*)this;
        if (!pN->IsTiedToPrev()) {
            //It is not tied to the previous one. Generate NoteOn event to start the sound and
            //highlight the note
            pN->ComputeVolume();
            pSM->StoreEvent(rTime, eSET_NoteON, nChannel, pN->GetMPitch(),
                            pN->GetVolume(), pN->GetStep(), this, nMeasure);
        }
        else {
            //This note is tied to the previous one. Generate only a VisualOn event as the
            //sound is already started by the previous note.
            pSM->StoreEvent(rTime, eSET_VisualON, nChannel, pN->GetMPitch(), 0, pN->GetStep(),
                            this, nMeasure);
        }
    }

    //generate NoteOff event
    rTime += GetDuration();
    if (IsRest()) {
        //Is a rest. Genera only a VisualOff event
        pSM->StoreEvent(rTime, eSET_VisualOFF, nChannel, 0, 0, 0, this, nMeasure);
    }
    else {
        //It is a note
        lmNote* pN = (lmNote*)this;
        if (!pN->IsTiedToNext()) {
            //It is not tied to next note. Generate NoteOff event to stop the sound and
            //un-highlight the note
            pSM->StoreEvent(rTime, eSET_NoteOFF, nChannel, pN->GetMPitch(), 0, pN->GetStep(),
                            this, nMeasure);
        }
        else {
            //This note is tied to the next one. Generate only a VisualOff event so that
            //the note will be un-highlighted but the sound will not be stopped.
            pSM->StoreEvent(rTime, eSET_VisualOFF, nChannel, pN->GetMPitch(), 0, pN->GetStep(),
                            this, nMeasure);
        }
    }

}

wxString lmNoteRest::GetLDPNoteType()
{
    switch(m_nNoteType) {
        case eLonga:
            return _T("l");
        case eBreve:
            return _T("d");
        case eWhole:
            return _T("w");
        case eHalf:
            return _T("h");
        case eQuarter:
            return _T("q");
        case eEighth:
            return _T("e");
        case e16th:
            return _T("s");
        case e32th:
            return _T("t");
        case e64th:
            return _T("i");
        case e128th:
            return _T("o");
        case e256th:
            return _T("f");
        default:
            wxASSERT(false);
            return _T("");        //compiler happy
    }

}

lmLUnits lmNoteRest::GetStaffOffset() const 
{ 
    return m_pVStaff->GetStaffOffset(m_nStaffNum); 
}

void lmNoteRest::ChangeDots(int nDots)
{
    //Note duration could be altered by modifiers (i.e. a tuplet). Instead of investigating
    //this, I just compute the current time modification factor
    float rFactor = m_rDuration / NoteTypeToDuration(m_nNoteType, m_nNumDots);

    //Now, compute the new duration with new dots
    m_nNumDots = nDots;
    m_rDuration = rFactor * NoteTypeToDuration(m_nNoteType, m_nNumDots);

    SetDirty(true);
}





//==========================================================================================
// Global functions related to NoteRests
//==========================================================================================

float NoteTypeToDuration(lmENoteType nNoteType, int nDots)
{
    //compute duration without modifiers
    float rDuration = pow(2.0f, (10 - nNoteType));

    //take dots into account
    switch (nDots)
    {
        case 0:                             break;
        case 1: rDuration *= 1.5f;          break;
        case 2: rDuration *= 1.75f;         break;
        case 3: rDuration *= 1.875f;        break;
        case 4: rDuration *= 1.9375f;       break;
        case 5: rDuration *= 1.96875f;      break;
        case 6: rDuration *= 1.984375f;     break;
        case 7: rDuration *= 1.9921875f;    break;
        case 8: rDuration *= 1.99609375f;   break;
        case 9: rDuration *= 1.998046875f;  break;
        default:
            wxLogMessage(_T("[NoteTypeToDuration] Program limit: do you really need more than nine dots?"));
    }

    return rDuration;

}

float NoteTypeToDuration(lmENoteType nNoteType, bool fDotted, bool fDoubleDotted)
{
    //compute duration without modifiers
    float rDuration = pow(2.0f, (10 - nNoteType));

   //take dots into account
    if (fDotted) { rDuration *= 1.5; }
    if (fDoubleDotted) { rDuration *= 1.75; }

    return rDuration;

}

