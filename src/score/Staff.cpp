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

#include "Score.h"
#include "../app/global.h"

// A lmStaff is a collection of consecutive lines and spaces.

//constructor
lmStaff::lmStaff(lmScore* pScore, int nNumLines, lmLUnits uUnits)
    : lmScoreObj(pScore)
{
    m_nNumLines = nNumLines;
    m_uLineThickness = lmToLogicalUnits(0.15, lmMILLIMETERS);     //TODO user option
    if (uUnits == 0)
        m_uSpacing = lmToLogicalUnits(1.8, lmMILLIMETERS);   //Default 1.8 mm -> lmStaff height = 7.2 mm
    else
        m_uSpacing = uUnits;        // in logical units

    // margins
    m_uAfterSpace = lmToLogicalUnits(10, lmMILLIMETERS);    // 10 mm
    m_uLeftMargin = 0;
    m_uRightMargin = 0;

	//contexts
	m_pFirstContext = (lmContext*)NULL;
	m_pLastContext = (lmContext*)NULL;
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

lmLUnits lmStaff::GetHeight()
{
    // returns the height (in logical units) of the staff without margins, that is, the
    // distance between first and last line
    return (m_nNumLines - 1) * m_uSpacing;

}

wxString lmStaff::Dump()
{
    wxString sDump = _T("Staff\n");
    return sDump;
}

//----------------------------------------------------------------------------------------
// context management
// TODO: when inserting a context, is it necessary to update anything? 
//----------------------------------------------------------------------------------------

lmContext* lmStaff::NewContextAfter(lmClef* pNewClef, lmContext* pPrevContext)
{
    if (!pPrevContext) pPrevContext = m_pLastContext;

    //get current values
    lmKeySignature* pKey = (lmKeySignature*)NULL;
    lmTimeSignature* pTime = (lmTimeSignature*)NULL;
    if (pPrevContext)
	{
        pKey = pPrevContext->GetKey();
        pTime = pPrevContext->GetTime();
    }

	//create the new context
	lmContext* pNewContext = new lmContext(pNewClef, pKey, pTime, false, true, true);
	if (pPrevContext) pNewContext->CopyAccidentals(pPrevContext);

	//chain it in the list
    InsertContextAfter(pNewContext, pPrevContext);
	return pNewContext;
}

lmContext* lmStaff::NewContextAfter(lmKeySignature* pNewKey, lmContext* pPrevContext)
{
    if (!pPrevContext) pPrevContext = m_pLastContext;

    //get current values
    lmClef* pClef = (lmClef*)NULL;
    lmTimeSignature* pTime = (lmTimeSignature*)NULL;
    if (pPrevContext)
	{
        pClef = pPrevContext->GetClef();
        pTime = pPrevContext->GetTime();
    }

	//create the new context
    lmContext* pNewContext = new lmContext(pClef, pNewKey, pTime, true, false, true);

	//chain it in the list
    InsertContextAfter(pNewContext, pPrevContext);
	return pNewContext;
}

lmContext* lmStaff::NewContextAfter(lmTimeSignature* pNewTime, lmContext* pPrevContext)
{
    if (!pPrevContext) pPrevContext = m_pLastContext;

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
    if (pPrevContext) pNewContext->CopyAccidentals(pPrevContext);

	//chain it in the list
    InsertContextAfter(pNewContext, pPrevContext);
	return pNewContext;
}

void lmStaff::InsertContextAfter(lmContext* pNew, lmContext* pPrev)
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
