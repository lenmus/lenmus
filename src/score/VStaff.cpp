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

//--------------------------------------------------------------------------------------------------
// class lmVStaff: A generalization of the 'staff' concept.
//
//    The object lmVStaff (Virtual staff) is a generalization of the concept 'staff': it is a staff
//    with an great number of lines and spaces (almost infinite) so to represent all posible pitches,
//    but whose visual renderization can be controlled so
//    that only the specified (needed) lines are rendered i.e: the standard five-lines staff, the piano
//    grand staff, the single line staff used for rithm or unpitched music, or any other design you
//    would like.
//
//    An lmVStaff can contain many 'staffs' (groups of lines rendered toghether), depending on the layout
//    choosen. For example:
//    - the standard five-lines staff is an lmVStaff with one five-lines staff.
//    - the grand staff is an lmVStaff with two five-lines staves.
//    - the single line unpitched staff is an VSatff with one sigle line staff.
//
//    The concept of 'staff' is not modelled into the program, its is just a term to be used only
//    in documentation, algoritms explanations, etc.
//
//    An lmVStaff is composed by objects such as notes, rests, barlines, keys, and all other
//    musical signs traditionally used for writing music.
//    All this objects than can appear in a staff will be modeled by an abstract class 'lmStaffObj'.
//    Therefore an lmVStaff is, roughfly, a collection of StaffObjs and some attributes.
//
//    Bars are not modelled by objects. They are just the collection of lmStaffObj found between
//    two lmStaffObj of type 'barline' (and between the start of the score and the first barline).
//    Nevertheless, tha concept of bar is very important at least in two situations:
//        1. In interpretation (playing back the score). For example:  play from bar #7
//        2. When rendering the score, as all the objects in a bar must be rendered together in
//           the same paper line.
//    Due to this, althoug the bar is not modelled as an object, there exits methods in the
//    VStaff object to deal with bars.
//
//-------------------------------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "VStaff.h"
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
#include "Notation.h"
#include "../app/global.h"
#include "MetronomeMark.h"
#include "../app/TheApp.h"		//to access g_rScreenDPI and g_rPixelsPerLU
#include "../graphic/GMObject.h"
#include "../graphic/ShapeStaff.h"
#include "../graphic/BoxSliceVStaff.h"
#include "../graphic/ShapeBarline.h"


//constructor
lmVStaff::lmVStaff(lmScore* pScore, lmInstrument* pInstr, bool fOverlayered)
    : lmScoreObj(pScore)
{
    //pScore is the lmScore to which this vstaff belongs.
    //Initially the lmVStaff will have only one standard five-lines staff. This can be
    //later modified invoking the 'AddStaff' method (REVIEW)

    m_pScore = pScore;
    m_pInstrument = pInstr;
    m_fOverlayered = fOverlayered;

    // default lmVStaff margins (logical units = tenths of mm)
    m_nHeight = 0;          //a value of 0 means 'compute it'
    m_leftMargin = 0;
    m_topMargin = 0;
    m_rightMargin = 0;
    m_bottomMargin = lmToLogicalUnits(1, lmCENTIMETERS);    // 1 cm

    //create one standard staff (five lines, 7.2 mm height)
    lmStaff* pStaff = new lmStaff(pScore);
    m_cStaves.Append(pStaff);

 //   //Add EOS control object to the StaffObjs collection
 //   //m_cStaffObjs.Store( new lmSOControl(lmEND_OF_STAFF, this) );
 //   //m_cStaffObjs.Store( new lmBarline(lm_eBarlineEOS, this, lmNO_VISIBLE) );
	//AddBarline(lm_eBarlineEOS, lmVISIBLE);

    //default value
    //TODO review this fixed space before the clef
    m_nSpaceBeforeClef = TenthsToLogical(10, 1);    // one line of first staff

    g_pLastNoteRest = (lmNoteRest*)NULL;

	//contexts
	m_pFirstContext = (lmContext*)NULL;
	m_pLastContext = (lmContext*)NULL;

}

lmVStaff::~lmVStaff()
{
    m_cStaves.DeleteContents(true);
    m_cStaves.Clear();

	//delete contexts
	lmContext* pCT = m_pFirstContext;
	while(pCT)
	{
		lmContext* pNext = pCT->GetNext();
		delete pCT;;
		pCT = pNext;
	}

}

lmLUnits lmVStaff::TenthsToLogical(lmTenths nTenths)
{
	return TenthsToLogical(nTenths, 1);
}

lmTenths lmVStaff::LogicalToTenths(lmLUnits uUnits)
{
	return LogicalToTenths(uUnits, 1);
}

lmStaff* lmVStaff::AddStaff(int nNumLines, lmLUnits nMicrons)
{
    lmStaff* pStaff = new lmStaff(m_pScore, nNumLines, nMicrons);
    m_cStaves.Append(pStaff);

    return pStaff;

}

lmStaff* lmVStaff::GetStaff(int nStaff)
{
    //iterate over the collection of Staves (lmStaff Objects) to locate staff nStaff
    int iS;
    StaffList::Node* pNode = m_cStaves.GetFirst();
    for (iS=1 ; iS != nStaff && pNode; iS++ ) {
        pNode = pNode->GetNext();    //get next lmStaff
    }
    wxASSERT(pNode);
    return (lmStaff *)pNode->GetData();

}

lmStaff* lmVStaff::GetFirstStaff()
{
    m_pStaffNode = m_cStaves.GetFirst();
    return (m_pStaffNode ? (lmStaff *)m_pStaffNode->GetData() : (lmStaff *)m_pStaffNode);
}

lmStaff* lmVStaff::GetNextStaff()
{
    wxASSERT(m_pStaffNode);
    m_pStaffNode = m_pStaffNode->GetNext();
    return (m_pStaffNode ? (lmStaff *)m_pStaffNode->GetData() : (lmStaff *)m_pStaffNode);
}

lmStaff* lmVStaff::GetLastStaff()
{
    wxASSERT(m_pStaffNode);
    m_pStaffNode = m_cStaves.GetLast();
    return (m_pStaffNode ? (lmStaff *)m_pStaffNode->GetData() : (lmStaff *)m_pStaffNode);
}

lmLUnits lmVStaff::TenthsToLogical(lmTenths nTenths, int nStaff)
{
    lmStaff* pStaff = GetStaff(nStaff);
    wxASSERT(pStaff);
    return pStaff->TenthsToLogical(nTenths);
}

lmTenths lmVStaff::LogicalToTenths(lmLUnits uUnits, int nStaff)
{
    lmStaff* pStaff = GetStaff(nStaff);
    wxASSERT(pStaff);
    return pStaff->LogicalToTenths(uUnits);
}

lmLUnits lmVStaff::GetStaffLineThick(int nStaff)
{
    lmStaff* pStaff = GetStaff(nStaff);
    wxASSERT(pStaff);
    return pStaff->GetLineThick();

}

//----------------------------------------------------------------------------------------
// contexts management
//----------------------------------------------------------------------------------------

//void lmVStaff::ResetContexts()
//{
//    // Verify if current context is just the key signature accidentals. If not,
//    // create a new context.
//    // This method is invoked after a barline to reset the context if it was modified
//    // by accidentals in notes
//
//    // iterate over the collection of Staves
//    wxStaffListNode* pNode = m_cStaves.GetFirst();
//    for (; pNode; pNode = pNode->GetNext() )
//    {
//        lmStaff* pStaff = (lmStaff *)pNode->GetData();
//
//        lmContext* pOldContext = pStaff->GetLastContext();
//        if (!pOldContext) return;
//        lmContext* pNewContext = new lmContext(pOldContext->GetClef(),
//                                               pOldContext->GeyKey(),
//                                               pOldContext->GetTime() );
//        bool fEqual = true;
//        for (int i=0; i < 7; i++) {
//            if (pOldContext->GetAccidentals(i) != pNewContext->GetAccidentals(i)) {
//                fEqual = false;
//                break;
//            }
//        }
//        if (fEqual) {
//            //current context is just the key signature accidentals. Continue using it.
//            delete pNewContext;
//        }
//        else {
//            //current contex has additional accidentals. Use the new clean one
//            pStaff->NewContext(pNewContext);
//        }
//    }
//
//}

void lmVStaff::OnContextUpdated(lmNote* pStartNote, int nStaff, int nStep,
                           int nNewAccidentals, lmContext* pCurrentContext)
{
    // Note pStartNote (whose diatonic name is nStep) has accidentals that must be 
	// propagated to the context and to the following notes until the end of the measure
	// or until a new accidental for the same step is found

    //create a new context by updating current one
    //lmStaff* pStaff = GetStaff(nStaff);
    //lmContext* pNewContext = pStaff->NewContext(pCurrentContext, nNewAccidentals, nStep);
    //pStaff->NewContext(pCurrentContext, nNewAccidentals, nStep);

    /*TODO
    For now, as we are not yet dealing with edition, it is not possible to
    insert notes in a score. Therefore, there are no notes after the one being
    processed (pStartNote). So the treatment to propagate accidentals until the
    start of the next measure is not yet implemented.
    */

    /*
    //propagate new context
    //define a forward iterator
    lmStaffObj* pSO = (lmStaffObj*) NULL;
    lmNoteRest* pNR = (lmNoteRest*)NULL;
    lmNote* pNote = (lmNote*)NULL;
    lmStaffObjIterator* pIter = m_cStaffObjs.CreateIterator(eTR_ByTime);
    pIter->MoveToObject(pStartNote);
    wxASSERT(pIter);
    pIter->MoveNext();

    while(!pIter->EndOfList())
    {
        pSO = pIter->GetCurrent();
        switch (pSO->GetClass()) {
            case eSFOT_NoteRest:
                if (pSO->GetStaffNum() == nStaff) {
                    pNR = (lmNoteRest*)pSO;
                    if (!pNR->IsRest()) {
                        pNote = (lmNote*)pSO;
                        if (pNote->OnContextUpdated(nStep, nNewAccidentals, pNewContext)) {
                            //if returns true, no context modification was necessary.
                            //So is not necessary to continue until the end of the measure
                            delete pIter;
                            return;
                        }
                    }
                }
                break;

            case eSFOT_Barline:
                //End of measure reached. End uptade process
                    delete pIter;
                    return;
                break;

            default:
                ;
        }
        pIter->MoveNext();
    }
    delete pIter;
    return;
    */

}

lmContext* lmVStaff::FindCurrentContext(lmContext* pContext, int nStaff)
{
	// Starting with received context look backward for the first context
	// applicable to staff nStaff and returns it.
	// AWARE: Only Clef, key signature and time signature are updated. To get
	//	applicable accidentals use NewUpdatedContext() instead.
	//find an applicable context to staff nStaff (1..n)

    lmContext* pCT = pContext;
	for (; pCT && !pCT->AppliesTo(nStaff); pCT = pCT->GetPrev()) {}
	return pCT;
}

lmContext* lmVStaff::GetCurrentContext(lmStaffObj* pSO)
{
	// Returns the context that is applicable to the received StaffObj.
	// AWARE: Only Clef, key signature and time signature are updated. To get
	//	applicable accidentals use NewUpdatedContext() instead.

    int nStaff = pSO->GetStaffNum();
    lmContext* pCT = (lmContext*)NULL;
	lmStaffObjIterator* pIter = m_cStaffObjs.CreateIteratorTo(pSO);
    while(!pIter->StartOfList())
	{
        pSO = pIter->GetCurrent();
		if (pSO->GetClass() == eSFOT_Clef) {
            pCT = ((lmClef*)pSO)->GetContext();
			break;
		}
		else if (pSO->GetClass() == eSFOT_KeySignature) {
            pCT = ((lmKeySignature*)pSO)->GetContext();
			break;
		}
		else if (pSO->GetClass() == eSFOT_TimeSignature) {
            pCT = ((lmTimeSignature*)pSO)->GetContext();
			break;
		}
		else if (pSO->GetClass() == eSFOT_Barline) {
            pCT = ((lmBarline*)pSO)->GetContext();
			break;
		}
		else
		    pIter->MovePrev();
    }
    delete pIter;

	if (!pCT) return pCT;

	//find an applicable context to staff nStaff (1..n)
	for (; pCT && !pCT->AppliesTo(nStaff); pCT = pCT->GetPrev()) {}
	return pCT;
}

int lmVStaff::GetUpdatedContextAccidentals(lmStaffObj* pThisSO, int nStep)
{
	//returns the context accidentals for this StaffObj, updated with all
	//accidentals introduced by previous notes.

	lmContext* pContext = NewUpdatedContext(pThisSO);
	if (pContext)
	{
		int nAcc = pContext->GetAccidentals(nStep);
		delete pContext;
		return nAcc;
	}
	else
		return 0;
}

lmContext* lmVStaff::NewUpdatedContext(lmStaffObj* pThisSO)
{
	//returns the applicable context for this StaffObj, updated with all
	//accidentals introduced by previous notes.
	//AWARE: context ownership is transferred to the caller.
	//       The returned context MUST BE deleted by the invoking method.

    int nStaff = pThisSO->GetStaffNum();
    lmContext* pCT = (lmContext*)NULL;
	lmStaffObjIterator* pIter = m_cStaffObjs.CreateIteratorTo(pThisSO);
    while(!pIter->StartOfList())
	{
        lmStaffObj* pSO = pIter->GetCurrent();
		if (pSO->GetClass() == eSFOT_Clef) {
            pCT = ((lmClef*)pSO)->GetContext();
			break;
		}
		else if (pSO->GetClass() == eSFOT_KeySignature) {
            pCT = ((lmKeySignature*)pSO)->GetContext();
			break;
		}
		else if (pSO->GetClass() == eSFOT_TimeSignature) {
            pCT = ((lmTimeSignature*)pSO)->GetContext();
			break;
		}
		else if (pSO->GetClass() == eSFOT_Barline) {
            pCT = ((lmBarline*)pSO)->GetContext();
			break;
		}
		else
		    pIter->MovePrev();
    }

	if (!pCT) {
		delete pIter;
		return pCT;
	}

	//Here pIter is pointing to the StaffObj pointing to the current context.
	//Now we have to go forward, updating accidentals until we reach pThisSO

	//find an applicable context to staff nStaff (1..n)
	for (; pCT && !pCT->AppliesTo(nStaff); pCT = pCT->GetPrev()) {}
	if (!pCT) {
		delete pIter;
		return pCT;
	}

	lmContext* pUpdated = new lmContext(pCT);

	//Now, update context with accidentals
    while(true)
	{
        lmStaffObj* pSO = pIter->GetCurrent();
		if (pSO == pThisSO) break;

		if (pSO->GetClass() == eSFOT_NoteRest && !((lmNote*)pSO)->IsRest())
		{
			//Note found. Update context
			lmAPitch apPitch = ((lmNote*)pSO)->GetAPitch();
			pUpdated->SetAccidental(apPitch.Step(), apPitch.Accidentals());
		}
		pIter->MoveNext();
	}
    delete pIter;

	return pUpdated;
}

lmContext* lmVStaff::NewUpdatedLastContext(int nStaff)
{
	//returns the last context for staff nStaff, updated with all
	//accidentals introduced by previous notes.
	//AWARE: context ownership is transferred to the caller.
	//       The returned context MUST BE deleted by the invoking method.

    //locate first context for this staff
    lmContext* pCT = (lmContext*)NULL;
    lmStaffObjIterator* pIter = m_cStaffObjs.CreateIterator(eTR_ByTime);
    pIter->MoveLast();
    while(!pIter->StartOfList())
	{
        lmStaffObj* pSO = pIter->GetCurrent();
		if (pSO->GetClass() == eSFOT_Clef) {
            pCT = ((lmClef*)pSO)->GetContext();
			break;
		}
		else if (pSO->GetClass() == eSFOT_KeySignature) {
            pCT = ((lmKeySignature*)pSO)->GetContext();
			break;
		}
		else if (pSO->GetClass() == eSFOT_TimeSignature) {
            pCT = ((lmTimeSignature*)pSO)->GetContext();
			break;
		}
		else if (pSO->GetClass() == eSFOT_Barline) {
            pCT = ((lmBarline*)pSO)->GetContext();
			break;
		}
		else
		    pIter->MovePrev();
    }

	if (!pCT) {
		delete pIter;
		return pCT;
	}

	//Here pIter is pointing to the StaffObj pointing to the current context.
	//Now we have to go forward, updating accidentals until we reach end of collection

	//find an applicable context to staff nStaff (1..n)
	for (; pCT && !pCT->AppliesTo(nStaff); pCT = pCT->GetPrev()) {}
	if (!pCT) {
		delete pIter;
		return pCT;
	}

	lmContext* pUpdated = new lmContext(pCT);

	//Now, update context with accidentals
    while(!pIter->EndOfList())
	{
        lmStaffObj* pSO = pIter->GetCurrent();
		if (pSO->GetClass() == eSFOT_NoteRest && !((lmNote*)pSO)->IsRest())
		{
			//Note found. Update context
			lmAPitch apPitch = ((lmNote*)pSO)->GetAPitch();
			pUpdated->SetAccidental(apPitch.Step(), apPitch.Accidentals());
		}
		pIter->MoveNext();
	}
    delete pIter;

	return pUpdated;
}


//---------------------------------------------------------------------------------------
// Methods for inserting StaffObjs
//---------------------------------------------------------------------------------------

lmClef* lmVStaff::InsertClef(lmStaffObj* pCursorSO, lmEClefType nClefType)
{
    int nStaff = pCursorSO->GetStaffNum();
    lmClef* pClef = new lmClef(nClefType, this, nStaff, lmVISIBLE);
    lmContext* pContext = NewContextAfter(pClef, GetCurrentContext(pCursorSO));
	pClef->SetContext(pContext);
    m_cStaffObjs.Insert(pClef, pCursorSO);
    return pClef;
}

lmBarline* lmVStaff::InsertBarline(lmStaffObj* pCursorSO, lmEBarline nType)
{
    lmBarline* pBarline = new lmBarline(nType, this, lmVISIBLE);
    m_cStaffObjs.Insert(pBarline, pCursorSO);
    pBarline->SetContext( pCursorSO->GetCurrentContext() );
	//TODO: it is necessary to update context in following notes, as the measure has
	//been splitted
    return pBarline;
}

lmNote* lmVStaff::InsertNote(lmStaffObj* pCursorSO, lmEPitchType nPitchType, wxString sStep,
							 wxString sOctave, lmENoteType nNoteType, float rDuration)
{
    int nStaff = pCursorSO->GetStaffNum();
    //lmStaff* pStaff = GetStaff(nStaff);
    //lmContext* pContext = pStaff->GetLastContext();

	//get the applicable context
	lmContext* pContext = NewUpdatedContext(pCursorSO);

    lmTBeamInfo BeamInfo[6];
    for (int i=0; i < 6; i++) {
        BeamInfo[i].Repeat = false;
        BeamInfo[i].Type = eBeamNone;
    }
	wxString sAccidentals = _T("");

    lmNote* pNt = new lmNote(this, nPitchType,
                        sStep, sOctave, sAccidentals, eNoAccidentals,
                        nNoteType, rDuration, false, false, nStaff, lmVISIBLE,
                        pContext, false, BeamInfo, false, false, lmSTEM_DEFAULT);

    m_cStaffObjs.Insert(pNt, pCursorSO);

	delete pContext;
    return pNt;
}

void lmVStaff::DeleteObject(lmStaffObj* pCursorSO)
{
    m_cStaffObjs.Delete(pCursorSO);
}

//---------------------------------------------------------------------------------------
// Methods for adding StaffObjs
//---------------------------------------------------------------------------------------

// adds a clef to the end of current StaffObjs collection
lmClef* lmVStaff::AddClef(lmEClefType nClefType, int nStaff, bool fVisible)
{
    wxASSERT(nStaff <= GetNumStaves());

    lmClef* pClef = new lmClef(nClefType, this, nStaff, fVisible);
    lmContext* pContext = NewContextAfter(pClef, m_pLastContext);
	pClef->SetContext(pContext);
    m_cStaffObjs.Store(pClef);
    return pClef;
}

// adds a spacer to the end of current StaffObjs collection
lmSpacer* lmVStaff::AddSpacer(lmTenths nWidth)
{
    lmSpacer* pSpacer = new lmSpacer(this, nWidth);
    m_cStaffObjs.Store(pSpacer);
    return pSpacer;

}

lmStaffObj* lmVStaff::AddAnchorObj()
{
    // adds an anchor StaffObj to the end of current StaffObjs collection

    if (IsGlobalStaff())
    {
        lmScoreAnchor* pAnchor = new lmScoreAnchor(this);
        m_cStaffObjs.Store(pAnchor);
        return pAnchor;
    }
    else
    {
        lmAnchor* pAnchor = new lmAnchor(this);
        m_cStaffObjs.Store(pAnchor);
        return pAnchor;
    }

}

// returns a pointer to the lmNote object just created
lmNote* lmVStaff::AddNote(lmEPitchType nPitchType,
                    wxString sStep, wxString sOctave, wxString sAlter,
                    lmEAccidentals nAccidentals,
                    lmENoteType nNoteType, float rDuration,
                    bool fDotted, bool fDoubleDotted,
                    int nStaff, bool fVisible,
                    bool fBeamed, lmTBeamInfo BeamInfo[],
                    bool fInChord,
                    bool fTie,
                    lmEStemType nStem)
{

    wxASSERT(nStaff <= GetNumStaves() );
    lmContext* pContext = NewUpdatedLastContext(nStaff);

    lmNote* pNt = new lmNote(this, nPitchType,
                        sStep, sOctave, sAlter, nAccidentals,
                        nNoteType, rDuration, fDotted, fDoubleDotted, nStaff, fVisible,
                        pContext, fBeamed, BeamInfo, fInChord, fTie, nStem);

    m_cStaffObjs.Store(pNt);

	delete pContext;
    return pNt;
}

// returns a pointer to the lmRest object just created
lmRest* lmVStaff::AddRest(lmENoteType nNoteType, float rDuration,
                      bool fDotted, bool fDoubleDotted,
                      int nStaff, bool fVisible,
                      bool fBeamed, lmTBeamInfo BeamInfo[])
{
    wxASSERT(nStaff <= GetNumStaves() );

    lmRest* pR = new lmRest(this, nNoteType, rDuration, fDotted, fDoubleDotted, nStaff,
                        fVisible, fBeamed, BeamInfo);

    m_cStaffObjs.Store(pR);
    return pR;

}

lmStaffObj* lmVStaff::AddText(wxString sText, lmEAlignment nAlign,
                            lmLocation* pPos, lmFontInfo tFontData, bool fHasWidth)
{
    lmScoreText* pText = new lmScoreText(sText, nAlign, *pPos, tFontData);

    // create an anchor object
    lmStaffObj* pAnchor;
    if (fHasWidth)
    {
        //attach it to a spacer
        pAnchor = this->AddSpacer( pText->GetShap2()->GetWidth() );
    }
    else
    {
        //No width. Attach it to an anchor
        pAnchor = AddAnchorObj();
    }
    pAnchor->AttachAuxObj(pText);

    return pAnchor;

}

lmMetronomeMark* lmVStaff::AddMetronomeMark(int nTicksPerMinute,
                        bool fParentheses, bool fVisible)
{
    lmMetronomeMark* pMM = new lmMetronomeMark(this, nTicksPerMinute,
                                               fParentheses, fVisible);
    m_cStaffObjs.Store(pMM);
    return pMM;

}

lmMetronomeMark* lmVStaff::AddMetronomeMark(lmENoteType nLeftNoteType, int nLeftDots,
                        lmENoteType nRightNoteType, int nRightDots,
                        bool fParentheses, bool fVisible)
{
    lmMetronomeMark* pMM = new lmMetronomeMark(this, nLeftNoteType, nLeftDots,
                                               nRightNoteType, nRightDots,
                                               fParentheses, fVisible);
    m_cStaffObjs.Store(pMM);
    return pMM;

}

lmMetronomeMark* lmVStaff::AddMetronomeMark(lmENoteType nLeftNoteType, int nLeftDots,
                        int nTicksPerMinute, bool fParentheses, bool fVisible)
{
    lmMetronomeMark* pMM = new lmMetronomeMark(this, nLeftNoteType, nLeftDots,
                                               nTicksPerMinute,
                                               fParentheses, fVisible);
    m_cStaffObjs.Store(pMM);
    return pMM;

}


//for types eTS_Common, eTS_Cut and eTS_SenzaMisura
lmTimeSignature* lmVStaff::AddTimeSignature(lmETimeSignatureType nType, bool fVisible)
{
    lmTimeSignature* pTS = new lmTimeSignature(nType, this, fVisible);
    return AddTimeSignature(pTS);
}

//for type eTS_SingleNumber
lmTimeSignature* lmVStaff::AddTimeSignature(int nSingleNumber, bool fVisible)
{
    lmTimeSignature* pTS = new lmTimeSignature(nSingleNumber, this, fVisible);
    return AddTimeSignature(pTS);
}

//for type eTS_Composite
lmTimeSignature* lmVStaff::AddTimeSignature(int nNumBeats, int nBeats[], int nBeatType,
                                        bool fVisible)
{
    lmTimeSignature* pTS = new lmTimeSignature(nNumBeats, nBeats, nBeatType, this, fVisible);
    return AddTimeSignature(pTS);
}

//for type eTS_Multiple
lmTimeSignature* lmVStaff::AddTimeSignature(int nNumFractions, int nBeats[], int nBeatType[],
                                        bool fVisible)
{
    lmTimeSignature* pTS = new lmTimeSignature(nNumFractions, nBeats, nBeatType, this, fVisible);
    return AddTimeSignature(pTS);
}

//for type eTS_Normal
lmTimeSignature* lmVStaff::AddTimeSignature(int nBeats, int nBeatType, bool fVisible)
{
    lmTimeSignature* pTS = new lmTimeSignature(nBeats, nBeatType, this, fVisible);
    return AddTimeSignature(pTS);
}

lmTimeSignature* lmVStaff::AddTimeSignature(lmETimeSignature nTimeSign, bool fVisible)
{
    lmTimeSignature* pTS = new lmTimeSignature(nTimeSign, this, fVisible);
    return AddTimeSignature(pTS);
}


//common code for all time signatures types
lmTimeSignature* lmVStaff::AddTimeSignature(lmTimeSignature* pTS)
{
	lmContext* pContext = NewContextAfter(pTS, m_pLastContext);
	pTS->SetContext(pContext);
    m_cStaffObjs.Store(pTS);
    return pTS;
}

lmKeySignature* lmVStaff::AddKeySignature(int nFifths, bool fMajor, bool fVisible)
{
    lmKeySignature* pKS = new lmKeySignature(nFifths, fMajor, this, fVisible);
	lmContext* pContext = NewContextAfter(pKS, m_pLastContext);
	pKS->SetContext(pContext);
    m_cStaffObjs.Store(pKS);
    return pKS;
}

lmKeySignature* lmVStaff::AddKeySignature(lmEKeySignatures nKeySignature, bool fVisible)
{
    int nFifths = KeySignatureToNumFifths(nKeySignature);
    bool fMajor = IsMajor(nKeySignature);
    return AddKeySignature(nFifths, fMajor, fVisible);
}

int lmVStaff::GetNumMeasures()
{
    return m_cStaffObjs.GetNumMeasures();
}

lmLUnits lmVStaff::LayoutStaffLines(lmBox* pBox, lmLUnits xFrom, lmLUnits xTo, lmLUnits yPos)
{
    //Computes all staff lines of this lmVStaff and creates the necessary shapes
	//to render them. Add this shapes to the received lmBox object.
    //Returns the Y coord. of last line (line 1, last staff)

    if (HideStaffLines()) return 0.0;

    //Set left position and lenght of lines, and save these values
    lmLUnits yCur = yPos + m_topMargin;
    m_yLinTop = yCur;              //save y coord. for first line start point

    //iterate over the collection of Staves (lmStaff Objects)
    StaffList::Node* pNode = m_cStaves.GetFirst();
    lmStaff* pStaff = (pNode ? (lmStaff *)pNode->GetData() : (lmStaff *)pNode);
    for ( ; pStaff; )
	{
        //draw one staff
		lmShapeStaff* pShape = 
				new lmShapeStaff(pStaff, pStaff->GetNumLines(), 
								 pStaff->GetLineThick(), pStaff->GetLineSpacing(),
								 xFrom, yCur, xTo, *wxBLACK );
		pBox->AddShape(pShape);
        yCur = pShape->GetYBottom() + pStaff->GetAfterSpace();
		m_yLinBottom = pShape->GetYBottom() - pStaff->GetLineThick();  

        //get next lmStaff
        pNode = pNode->GetNext();
        pStaff = (pNode ? (lmStaff *)pNode->GetData() : (lmStaff *)pNode);
    }
	return m_yLinBottom;
}

void lmVStaff::SetUpFonts(lmPaper* pPaper)
{
    //iterate over the collection of Staves (lmStaff Objects) to set up the fonts
    // to use on that staff
    StaffList::Node* pNode = m_cStaves.GetFirst();
    for ( ; pNode; ) {
        SetFont((lmStaff *)pNode->GetData(), pPaper);
        pNode = pNode->GetNext();
    }
}

void lmVStaff::SetFont(lmStaff* pStaff, lmPaper* pPaper)
{
    // Font "LeMus Notas" has been designed to draw on a staff whose interline
    // space is of 512 FUnits. This gives an optimal rendering on VGA displays (96 pixels per inch)
    // as staff lines are drawn on exact pixels, according to the following relationships:
    //       Let dyLines be the distance between lines (pixels), then
    //       Font size = 3 * dyLines   (points)
    //       Scale = 100 * dyLines / 8     (%)
    //
    // Given a zooming factor (as a percentage, i.e. zoom=250.0%) fontsize can be computed as
    //       i = Round((zoom*8) / 100)
    //       dyLines = i        (pixels)
    //       FontSize = 3*i        (points)
    //
    // As all scaling takes place in the DC it is not necessary to allocate fonts of
    // different size as all scaling takes place in the DC. Then:
    //       Let dyLines be the distance between lines (logical units), then
    //       Font size = 3 * dyLines   (logical points)

    lmLUnits dyLinesL = pStaff->GetLineSpacing();

    // the font for drawing is scaled by the DC.
    pStaff->SetFontDraw( pPaper->GetFont((int)(3.0 * dyLinesL * 96.0/g_rScreenDPI), _T("LenMus Basic") ) );        //logical points

    //wxLogMessage(_T("[lmVStaff::SetFont] dyLinesL=%d"), dyLinesL);

    //// the font for dragging is not scaled by the DC as all dragging operations takes
    //// place dealing with device units
    //int dyLinesD = pPaper->LogicalToDeviceY(100 * dyLinesL);
    //pStaff->SetFontDrag( pPaper->GetFont((3 * dyLinesD) / 100) );

}

lmLUnits lmVStaff::GetStaffOffset(int nStaff)
{
    //returns the Y offset to staff nStaff (1..n)
    wxASSERT(nStaff <= GetNumStaves() );
    lmLUnits yOffset = m_topMargin;

    // iterate over the collection of Staves (lmStaff Objects) to add up the
    // height and after space of all previous staves to the requested one
    lmStaff* pStaff;
    StaffList::Node* pNode = m_cStaves.GetFirst();
    for (int iS=1 ; iS < nStaff && pNode; iS++) {
        pStaff = (lmStaff *)pNode->GetData();
        yOffset += pStaff->GetHeight();
        yOffset += pStaff->GetAfterSpace();
        pNode = pNode->GetNext();
    }
    return yOffset;

}

wxString lmVStaff::Dump()
{
    wxString sDump = _T("");
	sDump += m_cStaffObjs.DumpStaffObjs();
	sDump += m_cStaffObjs.DumpMeasuresData();
	sDump += m_cStaffObjs.DumpMeasures();
    return sDump;
}

wxString lmVStaff::SourceLDP(int nIndent)
{
	wxString sSource = _T("");
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += _T("(musicData\n");
    nIndent++;

    //iterate over the collection of StaffObjs
    lmStaffObj* pSO;
    lmStaffObjIterator* pIT = m_cStaffObjs.CreateIterator(eTR_AsStored);  //THINK: Should be eTR_ByTime?
    while(!pIT->EndOfList())
    {
        pSO = pIT->GetCurrent();
        sSource += pSO->SourceLDP(nIndent);
        pIT->MoveNext();
    }
    delete pIT;

    //close musicData
    nIndent--;
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sSource += _T(")\n");

    return sSource;

}

wxString lmVStaff::SourceXML(int nIndent)
{
//    <measure number="1">
//      <attributes>
//        <divisions>1</divisions>
//        <key>
//          <fifths>0</fifths>
//        </key>
//        <time>
//          <beats>4</beats>
//          <beat-type>4</beat-type>
//        </time>
//        <clef>
//          <sign>G</sign>
//          <line>2</line>
//        </clef>
//      </attributes>
//      <note>
//        <pitch>
//          <step>C</step>
//          <octave>4</octave>
//        </pitch>
//        <duration>4</duration>
//        <type>whole</type>
//      </note>
//    </measure>

	wxString sSource = _T("");
	bool fStartMeasure = true;
	bool fStartAttributes = true;
	int nMeasure = 0;

    //iterate over the collection of StaffObjs
    lmStaffObj* pSO;
    lmStaffObjIterator* pIT = m_cStaffObjs.CreateIterator(eTR_AsStored);  //THINK: Should be eTR_ByTime?
    while(!pIT->EndOfList())
    {
        pSO = pIT->GetCurrent();

		if (fStartMeasure)
		{
			nMeasure++;
			sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
			sSource += wxString::Format(_T("<measure number='%d'>\n"), nMeasure);
			fStartMeasure = false;
			nIndent++;
		}

		//check if this is the end of a measure
		if (pSO->GetClass() == eSFOT_Barline)
		{
			nIndent--;
			sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
			sSource += _T("</measure>\n");
			fStartMeasure = true;
		}

		//check if this is a clef, tiem signature or key signature
		else if (pSO->GetClass() == eSFOT_Clef ||
			pSO->GetClass() == eSFOT_KeySignature ||
			pSO->GetClass() == eSFOT_TimeSignature)
		{
			if (fStartAttributes)
			{
				sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
				sSource += _T("<attributes>\n");
				fStartAttributes = false;
				nIndent++;
			}
			//get xml source for this staffobj
			sSource += pSO->SourceXML(nIndent);
		}

		else
		{
			if (!fStartAttributes)
			{
				nIndent--;
				sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
				sSource += _T("</attributes>\n");
				fStartAttributes = true;
			}
			//get xml source for this staffobj
			sSource += pSO->SourceXML(nIndent);
		}

        pIT->MoveNext();
    }
    delete pIT;

    //close last measure
	if (!fStartMeasure)
	{
		nIndent--;
		sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
		sSource += _T("</measure>\n");
	}

    return sSource;

}

// the next two methods are mainly used for drawing the barlines. For that purpose it is necessary
// to know the y coordinate of the top most upper line of first staff and the bottom most lower
// line of the last staff.

//TODO: This methods must be moved to lmBoxSystem / lmBoxSlice

lmLUnits lmVStaff::GetYTop()
{
    return m_yLinTop;
}

lmLUnits lmVStaff::GetYBottom()
{
    return m_yLinBottom;
}

lmLUnits lmVStaff::GetVStaffHeight()
{
    if (m_nHeight == 0) {
        m_nHeight = m_topMargin + m_bottomMargin;
        // iterate over the collection of Staves (lmStaff Objects) to add up its
        // height and its after space
        lmStaff* pStaff;
        StaffList::Node* pNode = m_cStaves.GetFirst();
        for ( ; pNode; ) {
            pStaff = (lmStaff *)pNode->GetData();
            m_nHeight += pStaff->GetHeight();
            m_nHeight += pStaff->GetAfterSpace();
            pNode = pNode->GetNext();
        }
    }

    return m_nHeight;

}

lmBarline* lmVStaff::AddBarline(lmEBarline nType, bool fVisible)
{
    //create and save the barline
    lmBarline* pBarline = new lmBarline(nType, this, fVisible);
	pBarline->SetContext(m_pLastContext);
    m_cStaffObjs.Store(pBarline);
    return pBarline;
}

bool lmVStaff::GetXPosFinalBarline(lmLUnits* pPos)
{
    // returns true if a barline is found and in this case updates content
    // of variable pointed by pPos with the right x position of last barline
    // This method is only used by Formatter, in order to not justify the last system
    lmStaffObj* pSO = (lmStaffObj*) NULL;
    lmStaffObjIterator* pIter = m_cStaffObjs.CreateIterator(eTR_AsStored);
    pIter->MoveLast();
    while(!pIter->StartOfList())
    {
        pSO = pIter->GetCurrent();
        if (pSO->GetClass() == eSFOT_Barline) break;
        pIter->MovePrev();
    }
    delete pIter;

    //check that a barline is found. Otherwise no barlines in the score
    if (pSO->GetClass() == eSFOT_Barline) {
        lmShape* pShape = (lmShape*)pSO->GetShap2();
        if (!pShape) return false;
		*pPos = pShape->GetXRight();
        return true;
    }
    else
        return false;

}

void lmVStaff::NewLine(lmPaper* pPaper)
{
    //move x cursor to the left and advance y cursor the space
    //height of all stafves of this lmVStaff
    pPaper->NewLine(GetVStaffHeight());

}
void lmVStaff::AddPrologShapes(lmBoxSliceVStaff* pBSV, int nMeasure, bool fDrawTimekey,
							   lmPaper* pPaper)
{
    // The prolog (clef and key signature) must be rendered on each system,
    // but the matching StaffObjs only exist in the first system. Therefore, in the
    // normal staffobj rendering process, the prolog would be rendered only in
    // the first system.
    // So, for the other systems it is necessary to force the rendering
    // of the prolog because there are no StaffObjs representing it.
    // This method does it.
    //
    // To know what clef, key and time signature to draw we take this information from the
    // context associated to first note of the measure on each staff. If there are no notes,
    // the context is taken from the barline. If, finally, no context is found, no prolog
    // is drawn.

    lmLUnits nPrologWidth = 0;
    lmClef* pClef = (lmClef*)NULL;
    lmEClefType nClef = lmE_Undefined;
    lmKeySignature* pKey = (lmKeySignature*)NULL;
    lmTimeSignature* pTime = (lmTimeSignature*)NULL;

    //AWARE when this method is invoked the paper position must be at the left marging,
    //at the start of a new system.
    lmLUnits xStartPos = pPaper->GetCursorX() + m_nSpaceBeforeClef;         //Save x to align all clefs
    lmLUnits yStartPos = pPaper->GetCursorY();

    //iterate over the collection of lmStaff objects to draw current cleft and key signature

    wxStaffListNode* pNode = m_cStaves.GetFirst();
    lmStaff* pStaff = (lmStaff*)NULL;
    lmLUnits yOffset = m_topMargin;
    lmLUnits xPos=0;
    lmLUnits nWidth=0;

    lmContext* pContext = (lmContext*)NULL;
    lmStaffObj* pSO = (lmStaffObj*) NULL;
    lmNoteRest* pNR = (lmNoteRest*)NULL;
    lmNote* pNote = (lmNote*)NULL;
    for (int nStaff=1; pNode; pNode = pNode->GetNext(), nStaff++)
    {
        pStaff = (lmStaff *)pNode->GetData();
        xPos = xStartPos;

        //locate first context for this staff
        pContext = (lmContext*)NULL;
        lmStaffObjIterator* pIter = m_cStaffObjs.CreateIterator(eTR_ByTime);
        pIter->AdvanceToMeasure(nMeasure);
        while(!pIter->EndOfList())
		{
            pSO = pIter->GetCurrent();
            if (pSO->GetClass() == eSFOT_NoteRest) {
                pNR = (lmNoteRest*)pSO;
                if (!pNR->IsRest() && pNR->GetStaffNum() == nStaff)
				{
                    //OK. Note fount. Take context
                    pNote = (lmNote*)pSO;
                    pContext = pNote->GetCurrentContext();
                    break;
                }
            }
            else if (pSO->GetClass() == eSFOT_Barline)
			{
				//end of measure reached. Take content
                lmBarline* pBar = (lmBarline*)pSO;
                pContext = FindCurrentContext(pBar->GetContext(), nStaff);
                break;
            }
            pIter->MoveNext();
        }
        delete pIter;

        if (pContext) {
            pClef = pContext->GetClef();
            pKey = pContext->GeyKey();
            pTime = pContext->GetTime();

            //render clef
            if (pClef) {
                nClef = pClef->GetClefType();
				if (pClef->IsVisible()) {
					lmUPoint uPos = lmUPoint(xPos, yStartPos+yOffset);        //absolute position
					nWidth = pClef->AddShape(pBSV, pPaper, uPos);
					xPos += nWidth;
				}
            }

            //render key signature
            if (pKey && pKey->IsVisible()) {
                wxASSERT(nClef != lmE_Undefined);
                lmUPoint uPos = lmUPoint(xPos, yStartPos+yOffset);        //absolute position
                nWidth = pKey->AddShape(pBSV, pPaper, uPos, nClef, nStaff);
                xPos += nWidth;
            }

        }

        //compute prolog width
        nPrologWidth = wxMax(nPrologWidth, xPos - xStartPos);

        //compute vertical displacement for next staff
        yOffset += pStaff->GetHeight();
        yOffset += pStaff->GetAfterSpace();

    }

    // update paper cursor position
    pPaper->SetCursorX(xStartPos + nPrologWidth);

}

lmSoundManager* lmVStaff::ComputeMidiEvents(int nChannel)
{
    /*
    nChannel is the MIDI channel to use for all events of this lmVStaff.
    Returns the lmSoundManager object. It is not retained by the lmVStaff, so it is caller
    responsibility to delete it when no longer needed.
    */

    //TODO review this commented code
//    Dim nMetrica As lmETimeSignature, nDurCompas As Long, nTiempoIni As Long
//
//    nMetrica = this.MetricaInicial
//    nDurCompas = GetDuracionMetrica(nMetrica)
//    nTiempoIni = nDurCompas - this.DuracionCompas(1)
//
//    wxASSERT(nTiempoIni >= 0        //El compas tiene más notas de las que caben
//
//    //Si el primer compas no es anacrusa, retrasa todo un compas para que
//    //se marque un compas completo antes de comenzar
//    if (nTiempoIni = 0 { nTiempoIni = nDurCompas

    //Create lmSoundManager and initialize MIDI events table
    lmSoundManager* pSM = new lmSoundManager();
    //TODO review next line
//    pSM->Inicializar GetStaffsCompas(nMetrica), nTiempoIni, nDurCompas, this.NumCompases

    //iterate over the collection to create the MIDI events
    float rMeasureStartTime = 0;
    int nMeasure = 1;        //to count measures (1 based, normal musicians way)
    pSM->StoreMeasureStartTime(nMeasure, rMeasureStartTime);

    //iterate over the collection to create the MIDI events
    lmStaffObj* pSO;
    lmNoteRest* pNR;
    lmTimeSignature* pTS;
    lmStaffObjIterator* pIter = m_cStaffObjs.CreateIterator(eTR_ByTime);
    while(!pIter->EndOfList()) {
        pSO = pIter->GetCurrent();
        if (pSO->GetClass() == eSFOT_NoteRest) {
            pNR = (lmNoteRest*)pSO;
            pNR->AddMidiEvents(pSM, rMeasureStartTime, nChannel, nMeasure);
        }
        else if (pSO->GetClass() == eSFOT_Barline) {
            rMeasureStartTime += pSO->GetTimePos();        //add measure duration
            nMeasure++;
            pSM->StoreMeasureStartTime(nMeasure, rMeasureStartTime);
        }
        else if (pSO->GetClass() == eSFOT_TimeSignature) {
            //add a RhythmChange event to set up tempo (num beats, duration of a beat)
            pTS = (lmTimeSignature*)pSO;
            pTS->AddMidiEvent(pSM, rMeasureStartTime, nMeasure);
        }
        pIter->MoveNext();
    }
    delete pIter;

    return pSM;

}

lmNote* lmVStaff::FindPossibleStartOfTie(lmAPitch anPitch)
{
    //
    // This method is invoked from lmNote constructor to find if the note being created
    // (the "target note") is tied to a previous one ("the candidate" one).
    // This method explores backwards to try to find a note that can be tied with the received
    // as parameter (the "target note").
    //
    // Algorithm:
    // Find the first previous note of the same pitch, in this measure or
    // in the previous one
    //


    //define a backwards iterator
    bool fInPrevMeasure = false;
    lmStaffObj* pSO = (lmStaffObj*) NULL;
    lmNoteRest* pNR = (lmNoteRest*)NULL;
    lmNote* pNote = (lmNote*)NULL;
    lmStaffObjIterator* pIter = m_cStaffObjs.CreateIterator(eTR_AsStored);
    pIter->MoveLast();
    while(!pIter->StartOfList())
    {
        pSO = pIter->GetCurrent();
        switch (pSO->GetClass()) {
            case eSFOT_NoteRest:
                pNR = (lmNoteRest*)pSO;
                if (!pNR->IsRest()) {
                    pNote = (lmNote*)pSO;
                    if (pNote->CanBeTied(anPitch)) {
                        delete pIter;
                        return pNote;    // candidate found
                    }
                }
                break;

            case eSFOT_Barline:
                if (fInPrevMeasure) {
                    delete pIter;
                    return (lmNote*)NULL;        // no suitable note found
                }
                fInPrevMeasure = true;
                break;

            default:
                ;
        }
        pIter->MovePrev();
    }
    delete pIter;
    return (lmNote*)NULL;        //no suitable note found

}

void lmVStaff::ShiftTime(float rTimeShift)
{
    /*
    Shifts the time counter and inserts a control lmStaffObj to signal the shift event
    This is necessary for rendering (to start a new thread) and for exporting the score:
     - in LDP, to create an element AVANCE/RETROCESO
     - in MusicXML: to create an element FORWARD/BACKWARD

    */

    //shift time counters
    m_cStaffObjs.ShiftTime(rTimeShift);

    //Insert a control object to signal the shift event so that we can start a
    //new thread at rendering
    lmSOControl* pControl = new lmSOControl(lmTIME_SHIFT, this, rTimeShift);
    m_cStaffObjs.Store(pControl);

}

lmSOControl* lmVStaff::AddNewSystem()
{
    /*
    Inserts a control lmStaffObj to signal a new system
    */

    //Insert the control object
    lmSOControl* pControl = new lmSOControl(lmNEW_SYSTEM, this);
    m_cStaffObjs.Store(pControl);
    return pControl;

}

lmStaffObjIterator* lmVStaff::CreateIterator(ETraversingOrder nOrder)
{
    return m_cStaffObjs.CreateIterator(nOrder);
}


//----------------------------------------------------------------------------------------
// context management
//----------------------------------------------------------------------------------------

lmContext* lmVStaff::NewContextAfter(lmClef* pNewClef, lmContext* pPrevContext)
{
    //get current values
    lmKeySignature* pKey = (lmKeySignature*)NULL;
    lmTimeSignature* pTime = (lmTimeSignature*)NULL;
    if (pPrevContext)
	{
        pKey = pPrevContext->GeyKey();
        pTime = pPrevContext->GetTime();
    }

	//create the new context
	int nStaff = pNewClef->GetStaffNum();
	lmContext* pNewContext = new lmContext(pNewClef, pKey, pTime, nStaff);
	if (pPrevContext) pNewContext->CopyAccidentals(pPrevContext);

	//chain it in the list
    InsertContextAfter(pNewContext, pPrevContext);
	return pNewContext;
}

lmContext* lmVStaff::NewContextAfter(lmKeySignature* pNewKey, lmContext* pPrevContext)
{
    //get current values
    lmClef* pClef = (lmClef*)NULL;
    lmTimeSignature* pTime = (lmTimeSignature*)NULL;
    if (pPrevContext)
	{
        pClef = pPrevContext->GetClef();
        pTime = pPrevContext->GetTime();
    }

	//create the new context
    lmContext* pNewContext = new lmContext(pClef, pNewKey, pTime, 0);

	//chain it in the list
    InsertContextAfter(pNewContext, pPrevContext);
	return pNewContext;
}

lmContext* lmVStaff::NewContextAfter(lmTimeSignature* pNewTime, lmContext* pPrevContext)
{
    //get current values
    lmClef* pClef = (lmClef*)NULL;
    lmKeySignature* pKey = (lmKeySignature*)NULL;
    if (pPrevContext)
	{
        pClef = pPrevContext->GetClef();
        pKey = pPrevContext->GeyKey();
    }

	//create the new context
    lmContext* pNewContext = new lmContext(pClef, pKey, pNewTime, 0);
    if (pPrevContext) pNewContext->CopyAccidentals(pPrevContext);

	//chain it in the list
    InsertContextAfter(pNewContext, pPrevContext);
	return pNewContext;
}

void lmVStaff::InsertContextAfter(lmContext* pNew, lmContext* pPrev)
{
	if (pPrev)
	{
		//this is not the first context. Chain it after pPrevContext
		lmContext* pNext = pPrev->GetNext();
		pNew->SetPrev(pPrev);
		pNew->SetNext(pNext);

		//update old links in prev and next nodes
		pPrev->SetNext(pNew);
		if (pNext) pNext->SetPrev(pNew);

		//update ptr to last node
		if(pPrev == m_pLastContext)
			m_pLastContext = pNew;	
	}
	else
	{
		//this is the first context. Insert it in front of list
		//update ptrs to first and last nodes
		m_pFirstContext = pNew;
		m_pLastContext = pNew;
	}
}

lmContext* lmVStaff::GetLastContext(int nStaff)
{
	//returns the last context applicable to staff nStaff (1..n)
	wxASSERT(nStaff > 0);
	lmContext* pCT = m_pLastContext;
	for (; pCT && !pCT->AppliesTo(nStaff); pCT = pCT->GetPrev()) {}
	return pCT;
}

