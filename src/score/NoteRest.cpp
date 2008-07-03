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

//--------------------------------------------------------------------------------------------------
/*! @class lmNoteRest
    @ingroup score_kernel
    @brief Object lmNoteRest represents a note or a silence

    Si consideramos los silencios como un tipo de figura y los acordes como una generalización de
    una nota, obtenemos una entidad conceptual más general que engloba a todos ellos. Como en la
    notación músical tradicional no existe este concepto, le llamo "FigSil" (Figura/Silencio):
        - Un silencio es un lmNoteRest sin notas
        - Una nota es un lmNoteRest con sólo una nota
        - Un acorde es un lmNoteRest con varias notas

    Por cómo ha ido evolucionando el programa, no se ha definido un objeto abstracto (FigSil) y
    tres objetos concretos (Nota, Acorde y Silencio) sino que el objeto lmNoteRest modela todo.

    Se han definido los métodos .EsSilencio y .InChord para identificar, respectivamente, si
    una lmNoteRest modeliza un silencio y si una lmNoteRest es una nota que forma parte de un acorde. Si
    el resultado de ambas es False, indica que es una nota simple.

    Existe un objeto auxiliar (CAcorde) cuya función es englobar algunas funcionalidaes propias
    de los acordes. Pero el acorde no es más que un conjunto de CNotas, identificadas como
    que forman un acorde.

*/

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
#endif

#include "Score.h"
#include "UndoRedo.h"
#include "VStaff.h"

#include "wx/debug.h"
#include "../ldp_parser/AuxString.h"
#include <math.h>


//====================================================================================================
//Constructors and destructors
//====================================================================================================

lmNoteRest::lmNoteRest(lmVStaff* pVStaff, bool IsRest, lmENoteType nNoteType, float rDuration,
                   bool fDotted, bool fDoubleDotted, int nStaff, int nVoice, bool fVisible)
	: lmStaffObj(pVStaff, eSFOT_NoteRest, pVStaff, nStaff, fVisible, lmDRAGGABLE)
{
    // initialize all atributes
    m_fCalderon = false;
    m_nNoteType = nNoteType;
    m_fDotted = fDotted;
    m_fDoubleDotted = fDoubleDotted;
    m_fIsRest = IsRest;
	m_nVoice = nVoice;

    m_pNotations = (AuxObjsList*)NULL;
    m_pLyrics = (AuxObjsList*)NULL;

    m_rDuration = rDuration;
    m_pTuplet = (lmTupletBracket*)NULL;

    m_pBeam = (lmBeam*)NULL;
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
		if (m_pBeam->NumNotes() <= 1)
			delete m_pBeam;
	}
}

void lmNoteRest::CreateBeam(bool fBeamed, lmTBeamInfo BeamInfo[])
{
    //This method is used when loading a file.
    //Set up beaming information

    if (!fBeamed) {
        m_pBeam = (lmBeam*)NULL;
    }
    else {
        for (int i=0; i < 6; i++) {
            m_BeamInfo[i] = BeamInfo[i];
        }
        if (m_BeamInfo[0].Type == eBeamBegin) {
            m_pBeam = new lmBeam((lmNote*)this);
            g_pCurBeam = m_pBeam;
        } else {
            m_pBeam = g_pCurBeam;
            if (!m_pBeam) {
                //TODO Show message. Error: ¡se pide finalizar un grupo que no ha sido abierto!
                fBeamed = false;
            } else {
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
                    g_pCurBeam = (lmBeam*)NULL;        // no beam open
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

    //set up basic beaming information
    for (int i=0; i < 6; i++) {
        m_BeamInfo[i].Repeat = false;
        m_BeamInfo[i].Type = eBeamNone;
    }
    m_BeamInfo[0].Type = nBeamType;         //eBeamBegin, eBeamContinue or eBeamEnd

    //save beam and include tis note/rest on it
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

void lmNoteRest::Freeze(lmUndoData* pUndoData)
{
    //save info about relations and invalidate ptrs.

    //if note/rest is in a beam, remove it from the beam
    m_pBeam = FreezeRelationship<lmBeam>(m_pBeam, pUndoData);

    //if note/rest is in a tuplet, remove it from the tuplet
    m_pTuplet = FreezeRelationship<lmTupletBracket>(m_pTuplet, pUndoData);

    //invalidate other pointers
    //m_pNotations = (AuxObjsList*)NULL;
    //m_pLyrics = (AuxObjsList*)NULL;

    //invalidate other pointers
    //NONE

    //TODO
    //lmStaffObj::Freeze(pUndoData);
}

void lmNoteRest::UnFreeze(lmUndoData* pUndoData)
{
    //restore pointers

    //restore beam
    m_pBeam = UnFreezeRelationship<lmBeam>(pUndoData);

    //restore tuplet
    m_pTuplet = UnFreezeRelationship<lmTupletBracket>(pUndoData);

    //TODO
    //lmStaffObj::UnFreeze(pUndoData);
}

template <class T>
T* lmNoteRest::FreezeRelationship(T* pRel, lmUndoData* pUndoData)
{
    //if note/rest is in the relation, remove it from the relation
    pUndoData->AddParam<bool>(pRel != (T*)NULL);
    if (pRel != (T*)NULL)
    {
		//save position in relation of note to be removed
		pUndoData->AddParam<int>(pRel->GetNoteIndex(this));

        pRel->Remove(this);

		//save num note/rests, and first note/rest in beam, for recovering relation pointer
        pUndoData->AddParam<int>( pRel->NumNotes() );
		pUndoData->AddParam<lmNoteRest*>( pRel->GetStartNoteRest() );

        //if only one note remaining, delete the relation
        if (pRel->NumNotes() == 1)
        {
            pRel->Save(pUndoData);
			delete pRel;	//this notifies all notes
        }

        pRel = (T*)NULL;
    }
    return pRel;
}

template <class T>
T* lmNoteRest::UnFreezeRelationship(lmUndoData* pUndoData)
{
    T* pRel =(T*)NULL;
    bool fInRel = pUndoData->GetParam<bool>();
    if (fInRel)
    {
		int nIndex = pUndoData->GetParam<int>();
		int nNumNotes = pUndoData->GetParam<int>();
		lmNoteRest* pFirstNR = pUndoData->GetParam<lmNoteRest*>();
		if (nNumNotes == 1)
		{
			//re-build the relation
            pRel = new T(pFirstNR, pUndoData);
		}
		else
		{
			//use existing pointer
			pRel = pFirstNR->GetRelationship<T>();
		}
		pRel->Include(this, nIndex);
    }
    return pRel;
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

void lmNoteRest::OnRemovedFromRelationship(void* pRel, lmERelationshipClass nRelClass)
{ 
	//AWARE: this method is invoked only when the relationship is being deleted and
	//this deletion is not requested by this note/rest. If this note/rest would like
	//to delete the relationship it MUST invoke Remove(this) before deleting the 
	//relationship object

	switch (nRelClass)
	{
		case lm_eBeamClass:
			m_pBeam = (lmBeam*)NULL;
			return;

		case lm_eTupletClass:
			m_pTuplet = (lmTupletBracket*)NULL;
			return;

		default:
			wxASSERT(false);
	}
}

//template <> void lmNoteRest::OnRemovedFromRelationship(lmMultipleRelationship<lmNoteRest>* pRel)
//{ 
//	if (lmBeam* pBeam = dynamic_cast<lmBeam*>(pRel))
//		m_pBeam = (lmBeam*)NULL;
//	else if (lmTupletBracket* pTuplet = dynamic_cast<lmTupletBracket*>(pRel))
//		m_pTuplet = (lmTupletBracket*)NULL;
//}

//----------------------------------------------------------------------------------------
//template specializations
//----------------------------------------------------------------------------------------

//for beams
template <> lmBeam* lmNoteRest::GetRelationship<lmBeam>() { return m_pBeam; }

//for tuplets
template <> lmTupletBracket* lmNoteRest::GetRelationship<lmTupletBracket>() { return m_pTuplet; }



lmLUnits lmNoteRest::DrawDot(bool fMeasuring, lmPaper* pPaper,
                             lmLUnits xPos, lmLUnits yPos,
                             wxColour colorC, bool fUseFont)
{
    //lmLUnits halfLine = m_pVStaff->TenthsToLogical(5, m_nStaffNum);
    yPos += m_pVStaff->TenthsToLogical(50, m_nStaffNum);

    if (fUseFont) {
        //Draw dot by using the font glyph
        wxString sGlyph( aGlyphsInfo[GLYPH_DOT].GlyphChar );
        yPos += m_pVStaff->TenthsToLogical(aGlyphsInfo[GLYPH_DOT].GlyphOffset, m_nStaffNum);
        if (!fMeasuring) {
            pPaper->SetTextForeground(colorC);
            pPaper->DrawText(sGlyph, xPos, yPos);
        }
        lmLUnits nWidth, nHeight;
        pPaper->GetTextExtent(sGlyph, &nWidth, &nHeight);
        return nWidth;
    }
    else {
        //Direct draw
        lmLUnits uDotRadius = m_pVStaff->TenthsToLogical(22, m_nStaffNum) / 10;
        if (!fMeasuring) pPaper->SolidCircle(xPos, yPos, uDotRadius);
        return 2*uDotRadius;
    }

}

lmLUnits lmNoteRest::AddDotShape(lmCompositeShape* pCS, lmPaper* pPaper,
                                 lmLUnits xPos, lmLUnits yPos, wxColour colorC)
{
    //Creates a shape for a dot and adds it to the composite shape
    //returns the width of the dot

    yPos += m_pVStaff->TenthsToLogical(50, m_nStaffNum);
    yPos += m_pVStaff->TenthsToLogical(aGlyphsInfo[GLYPH_DOT].GlyphOffset, m_nStaffNum);
    lmShapeGlyph* pShape = new lmShapeGlyph(this, -1, GLYPH_DOT, GetSuitableFont(pPaper), pPaper,
                                            lmUPoint(xPos, yPos), _T("Dot"));
	pCS->Add(pShape);
    return pShape->GetBounds().GetWidth();

}

wxString lmNoteRest::Dump()
{
    wxString sDump;

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
        if ((m_pTuplet->GetEndNoteRest())->GetID() == m_nId) {
            sDump += _T(", End of tuplet");
        }
        else if ((m_pTuplet->GetStartNoteRest())->GetID() == m_nId)
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

	////positioning information
	//lmURect rect = GetSelRect();
	//sDump += wxString::Format(_T("\n                    SelRect=(%.2f, %.2f, %.2f, %.2f)"),
	//	rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom() );

    //base class information
	sDump += lmStaffObj::Dump();
    sDump += _T("\n");

    return sDump;

}

wxString lmNoteRest::SourceLDP(int nIndent)
{
    wxString sSource = _T("");

	//start or end of group
    if (m_pBeam) {
        if (m_BeamInfo[0].Type == eBeamBegin) {
            sSource += _T(" g+");
        }
        else if (m_BeamInfo[0].Type == eBeamEnd) {
            sSource += _T(" g-");
        }
    }

    //tuplets
    if (m_pTuplet)
    {
        if ((lmNoteRest*)this == m_pTuplet->GetStartNoteRest())
            sSource += m_pTuplet->SourceLDP();
        else if((lmNoteRest*)this == m_pTuplet->GetEndNoteRest())
            sSource += _T(" t-");
    }

    //staff num
    if (m_pVStaff->GetNumStaves() > 1)
        sSource += wxString::Format(_T(" p%d"), m_nStaffNum);

    //Voice
    sSource += wxString::Format(_T(" v%d"), m_nVoice);

	//base class
	sSource += lmStaffObj::SourceLDP(nIndent);

    return sSource;
}

wxString lmNoteRest::SourceXML(int nIndent)
{
    wxString sSource = _T("");
    return sSource;
}


//====================================================================================================
// methods related to associated AuxObjs management
//====================================================================================================
lmFermata* lmNoteRest::AddFermata(const lmEPlacement nPlacement)
{
    if (!m_pNotations) m_pNotations = new AuxObjsList();

    lmFermata* pFermata = new lmFermata(nPlacement);
	AttachAuxObj(pFermata);
    return pFermata;
}

void lmNoteRest::AddLyric(lmLyric* pLyric)
{
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


/*! Receives a string (sNoteType) with the LDP letter for the type of note and, optionally,
    dots ".". It is assumed the source is normalized (no spaces, lower case)
    @returns the enum value that corresponds to this note type, or -1 if error
*/
int LDPNoteTypeToEnumNoteType(const wxString& sNoteType)
{
    //OBSOLETE
    wxASSERT(false);
    return -1;

    //wxChar cNoteType = sNoteType.GetChar(0);
    //switch (cNoteType) {
    //    case _T('l'):
    //        return eLonga;
    //    case _T('d'):
    //        return  eBreve;
    //    case _T('r'):
    //        return  eWhole;
    //    case _T('b'):
    //        return  eHalf;
    //    case _T('n'):
    //        return  eQuarter;
    //    case _T('c'):
    //        return  eEighth;
    //    case _T('s'):
    //        return  e16th;
    //    case _T('f'):
    //        return  e32th;
    //    case _T('m'):
    //        return  e64th;
    //    case _T('g'):
    //        return  e128th;
    //    case _T('p'):
    //        return  e256th;
    //    default:
    //        return  -1;     //error
    //}
}

/*! Receives a string (sNoteType) with the LDP letter for the type of note and, optionally,
    dots ".". It is assumed the source is normalized (no spaces, lower case)
    @returns the duration or -1.0 if error
*/
float LDPNoteTypeToDuration(const wxString& sNoteType)
{
    //OBSOLETE
    wxASSERT(false);
    return 0;

    //int nNoteType = LDPNoteTypeToEnumNoteType(sNoteType);
    //if (nNoteType == -1) return -1.0;    //error

    ////analyze dots
    //bool fDotted=false, fDoubleDotted=false;
    //if (sNoteType.length() > 1) {
    //    if (sNoteType.substr(1) == _T("..") ) {
    //        fDoubleDotted = true;
    //    } else if (sNoteType.substr(1) ==  _T(".") ) {
    //        fDotted = true;
    //    } else {
    //        return -1.0;    //error
    //    }
    //}

    //return NoteTypeToDuration((lmENoteType)nNoteType, fDotted, fDoubleDotted);
}

float NoteTypeToDuration(lmENoteType nNoteType, bool fDotted, bool fDoubleDotted)
{
    //compute duration without modifiers
    float rDuration = pow(2, (10 - nNoteType));

   //take dots into account
    if (fDotted) { rDuration *= 1.5; }
    if (fDoubleDotted) { rDuration *= 1.75; }

    return rDuration;

}

