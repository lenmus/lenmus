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
#pragma implementation "Staff.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Staff.h"
#include "Score.h"
#include "VStaff.h"
#include "Context.h"
#include "../app/global.h"

// A lmStaff is a collection of consecutive lines and spaces.

//constructor
lmStaff::lmStaff(lmVStaff* pVStaff, long nID, int nNumLines, lmLUnits uSpacing,
                 lmLUnits uDistance, lmLUnits uLineThickness)
    : lmScoreObj(pVStaff, nID, lm_eSO_Staff)
        //lines
    , m_nNumLines(nNumLines)
	    //contexts
	, m_pFirstContext((lmContext*)NULL)
	, m_pLastContext((lmContext*)NULL)
        //default clef and key signature
    , m_nDefaultClefType(lmE_Sol)       //G in 2nd line
    , m_nDefaultKeyType(earmDo)         //C major
{
    //line distance: default 1.8 mm -> lmStaff height = 7.2 mm    //TODO user option
    m_uSpacing = (uSpacing == 0.0f ? lmToLogicalUnits(1.8, lmMILLIMETERS) : uSpacing);

    //line thickness: default: 0.15 millimeters     //TODO user option
   m_uLineThickness = (uLineThickness == 0.0f ?
                       lmToLogicalUnits(0.15, lmMILLIMETERS) : uLineThickness);

    //top margin: default: 10 millimeters     //TODO user option
   m_uStaffDistance = (uDistance == 0.0f ?
                       lmToLogicalUnits(10, lmMILLIMETERS) : uDistance);
}

lmStaff::~lmStaff()
{
	//delete contexts
	lmContext* pCT = m_pFirstContext;
	while(pCT)
	{
		lmContext* pNext = pCT->GetNext();
		delete pCT;;
		pCT = pNext;
	}

}

int lmStaff::GetNumberOfInstrument()
{
    lmInstrument* pInstr = ((lmVStaff*)m_pParent)->GetOwnerInstrument();
    return ((lmVStaff*)m_pParent)->GetScore()->GetNumberOfInstrument(pInstr);
}

int lmStaff::GetNumberOfStaff()
{
    return ((lmVStaff*)m_pParent)->GetNumberOfStaff(this);
}

lmLUnits lmStaff::GetHeight()
{
    // returns the height (in logical units) of the staff without margins, that is, the
    // distance between first and last line
    return (m_nNumLines - 1) * m_uSpacing;
}
void lmStaff::SetDefaultClef(lmEClefType nType) 
{ 
    if (nType == lmE_Undefined)
        m_nDefaultClefType = lmE_Sol;     //G in 2nd line
    else
        m_nDefaultClefType = nType; 
}

void lmStaff::SetDefaultKey(lmEKeySignatures nType) 
{ 
    m_nDefaultKeyType = nType; 
}



//----------------------------------------------------------------------------------------
// debug
//----------------------------------------------------------------------------------------
//#if defined(__WXDEBUG__)

wxString lmStaff::DumpContextsChain()
{
    wxString sDump = wxString::Format(_T("\nStaff. Contexts: First: %d, Last: %d, Default clef = %s\n"),
        (m_pFirstContext ? m_pFirstContext->GetContextId() : 0),
        (m_pLastContext ? m_pLastContext->GetContextId() : 0),
        GetClefLDPNameFromType(m_nDefaultClefType).c_str() );

    sDump += _T("Fwd chain: ");
    lmContext* pContext = m_pFirstContext;
    while (pContext)
    {
        sDump += wxString::Format(_T("%d, "), pContext->GetContextId() );
        pContext = pContext->GetNext();
    }
    sDump += _T("\n");

    sDump += _T("Bwd chain: ");
    pContext = m_pLastContext;
    while (pContext)
    {
        sDump += wxString::Format(_T("%d, "), pContext->GetContextId() );
        pContext = pContext->GetPrev();
    }
    sDump += _T("\n\n");

    return sDump;
}

//#endif

wxString lmStaff::Dump()
{
    wxString sDump = _T("Staff\n");
    return sDump;
}


//----------------------------------------------------------------------------------------
// contexts management
//----------------------------------------------------------------------------------------

lmContext* lmStaff::NewContextAfter(lmClef* pNewClef, lmContext* pPrevContext)
{
    //get current values
    lmKeySignature* pKey = (lmKeySignature*)NULL;
    lmTimeSignature* pTime = (lmTimeSignature*)NULL;
    lmClef* pPrevClef = (lmClef*)NULL;
    if (pPrevContext)
	{
        pKey = pPrevContext->GetKey();
        pTime = pPrevContext->GetTime();
        pPrevClef = pPrevContext->GetClef();
    }

	//create the new context
	lmContext* pNewContext = new lmContext(pNewClef, pKey, pTime, false, true, true);
	if (pPrevContext)
        pNewContext->CopyAccidentals(pPrevContext);

	//chain it in the list and update following contexts in the context chain
    lmContext* pNextContext = (pPrevContext ? pPrevContext->GetNext() : m_pFirstContext);
    InsertContextAfter(pNewContext, pPrevContext, pNextContext, pNewClef);

    //FIX_ME: default clef should be replaced only if there are no notes before the
    //clef.
    ////if this is the first clef, save it as default clef
    //if (!pPrevClef)
    //    m_nDefaultClefType = pNewClef->GetClefType();

    //wxLogMessage(this->DumpContextsChain());

	return pNewContext;
}

lmContext* lmStaff::NewContextAfter(lmKeySignature* pNewKey, lmContext* pPrevContext)
{
    //get current values
    lmClef* pClef = (lmClef*)NULL;
    lmTimeSignature* pTime = (lmTimeSignature*)NULL;
    if (pPrevContext)
	{
        pClef = pPrevContext->GetClef();
        pTime = pPrevContext->GetTime();
    }

	//create the new context. Do not copy prev accidentals, as we are introducing a 
    //new key
    lmContext* pNewContext = new lmContext(pClef, pNewKey, pTime, true, false, true);

	//chain it in the list and update following contexts in the context chain
    lmContext* pNextContext = (pPrevContext ? pPrevContext->GetNext() : m_pFirstContext);
    InsertContextAfter(pNewContext, pPrevContext, pNextContext, pNewKey);

	return pNewContext;
}

lmContext* lmStaff::NewContextAfter(lmTimeSignature* pNewTime, lmContext* pPrevContext)
{
    //get current values
    lmClef* pClef = (lmClef*)NULL;
    lmKeySignature* pKey = (lmKeySignature*)NULL;
    if (pPrevContext)
	{
        pClef = pPrevContext->GetClef();
        pKey = pPrevContext->GetKey();
    }

	//create the new context
    lmContext* pNewContext = new lmContext(pClef, pKey, pNewTime, true, true, false);
	if (pPrevContext)
        pNewContext->CopyAccidentals(pPrevContext);

	//chain it in the list and update following contexts in the context chain
    lmContext* pNextContext = (pPrevContext ? pPrevContext->GetNext() : m_pFirstContext);
    InsertContextAfter(pNewContext, pPrevContext, pNextContext, pNewTime);

	return pNewContext;
}

void lmStaff::InsertContextAfter(lmContext* pNew, lmContext* pPrev, lmContext* pNext,
                                 lmStaffObj* pSO)
{
	//Chain new context
	pNew->SetPrev(pPrev);
	pNew->SetNext(pNext);

    //update links in prev and next nodes
	if (pPrev)
		pPrev->SetNext(pNew);
    if (pNext) 
        pNext->SetPrev(pNew);

    //update ptrs to first and last nodes
	if(!pPrev)
		m_pFirstContext = pNew;
    if (!pNext)
		m_pLastContext = pNew;

    //update this and following contexts in the chain with new inserted context.
    //If following context inherited a value it must be replaced by the value
    //from the new context.
    if (pNext)
        pNext->PropagateNewWhileInherited(pSO);
}

void lmStaff::RemoveContext(lmContext* pContext, lmStaffObj* pSO)
{
    //remove context from the list

    //un-chain the context to remove
	lmContext* pPrev = pContext->GetPrev();
	lmContext* pNext = pContext->GetNext();
	if (pPrev)
		pPrev->SetNext(pNext);
    if (pNext)
        pNext->SetPrev(pPrev);

    //update pointers to first and last contexts, if necessary
    if (pContext == m_pLastContext)
        m_pLastContext = pPrev;

    if (pContext == m_pFirstContext)
        m_pFirstContext = pNext;

    //update following contexts in the context chain. If following context inherited a value
    //form removed context, we have to update these inherited values. An example: if we are 
    //removing a clef and next context is created by a time signature, in this context 
    //the clef was inherited.
    if (pNext)
        pNext->PropagateValueWhileInherited(pSO);
}

wxString lmStaff::SourceLDP(int nIndent, bool fUndoData)
{
    wxString sSource = _T("");
    sSource.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    if (fUndoData)
        sSource += wxString::Format(_T("(staff#%d %d"), GetID(), GetNumberOfStaff());
    else
        sSource += wxString::Format(_T("(staff %d"), GetNumberOfStaff());

    //attributes
    sSource += _T(" (staffType regular)");
    sSource += wxString::Format(_T("(staffLines %d)"), m_nNumLines);
    sSource += wxString::Format(_T("(staffSpacing %s)"),
                                DoubleToStr((double)m_uSpacing, 2).c_str() );
    sSource += wxString::Format(_T("(staffDistance %s)"),
                                DoubleToStr((double)m_uStaffDistance, 2).c_str() );
    sSource += wxString::Format(_T("(lineThickness %s)"),
                                DoubleToStr((double)m_uLineThickness, 2).c_str() );

    //base class
    //AWARE: commented out because base class only adds location, and this
    //elements does not apply to staff
	//sSource += lmScoreObj::SourceLDP(nIndent, fUndoData);

    //close element
    sSource += _T(")\n");
	return sSource;
}

wxString lmStaff::SourceXML(int nIndent)
{
	wxString sSource = _T("");
	sSource.append(nIndent * lmXML_INDENT_STEP, _T(' '));
    sSource += _T("TODO: lmStaff XML Source code generation method\n");
    return sSource;
}



//------------------------------------------------------------------------------------------
// lmRefLine class implementation: A reference to line up lyrics, figured bass, other 
//------------------------------------------------------------------------------------------

lmRefLine::lmRefLine(lmVStaff* pVStaff)
{
}

lmRefLine::~lmRefLine()
{
}

