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
#pragma implementation "Context.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "Score.h"
#include "Context.h"

#if defined(_LM_DEBUG_)
static int m_nIdCounter = 0;
#endif

//access to global functions
extern void lmComputeAccidentals(lmEKeySignatures nKeySignature, int nAccidentals[]);

lmContext::lmContext(lmClef* pClef, lmKeySignature* pKey, lmTimeSignature* pTime,
              bool fClefInherited, bool fKeyInherited, bool fTimeInherited)
{
	//create a new context
    m_pClef = pClef;
    m_pKey = pKey;
    m_pTime = pTime;
    m_fModified = false;

    m_fClefInherited = fClefInherited;
    m_fKeyInherited = fKeyInherited;
    m_fTimeInherited = fTimeInherited;

    InitializeAccidentals();
	m_pPrev = (lmContext*) NULL;
	m_pNext = (lmContext*) NULL;

#if defined(_LM_DEBUG_)
    m_nId = ++m_nIdCounter;
#endif

}

lmContext::lmContext(lmContext* pContext)
{
    m_pClef = pContext->GetClef();
    m_pKey = pContext->GetKey();
    m_pTime = pContext->GetTime();
    m_fModified = false;

    CopyAccidentals(pContext);
	m_pPrev = (lmContext*) NULL;
	m_pNext = (lmContext*) NULL;

#if defined(_LM_DEBUG_)
    m_nId = ++m_nIdCounter;
#endif

}

lmEClefType lmContext::GetClefType() const
{
    if (m_pClef)
        return m_pClef->GetClefType();
    else
        return lmE_Undefined;
}

void lmContext::SetKey(lmKeySignature* pKey)
{
    m_pKey = pKey;
    InitializeAccidentals();
}

void lmContext::InitializeAccidentals()
{
    // initialize array: no accidentals
    for (int i=0; i < 7; i++) {
        m_nAccidentals[i] = 0;
    }
    if (!m_pKey) return;

    lmComputeAccidentals(m_pKey->GetKeyType(), m_nAccidentals);
}

void lmContext::CopyAccidentals(lmContext* pContext)
{
    int i;
    for (i=0; i < 7; i++) {
        m_nAccidentals[i] = pContext->GetAccidentals(i);
    }
}

wxString lmContext::DumpContext(int nIndent)
{
    wxString sDump = _T("");
#if defined(_LM_DEBUG_)
    sDump.append(nIndent * lmLDP_INDENT_STEP, _T(' '));
    sDump += wxString::Format(_T("Context %d: clef: %s, key: %s, time: %s, acc=%d,%d,%d,%d,%d,%d,%d\n"),
         m_nId,
        (m_pClef ? GetClefLDPNameFromType(m_pClef->GetClefType()).c_str() : _T("No")),
        (m_pKey ? GetKeyLDPNameFromType(m_pKey->GetKeyType()).c_str() : _T("No")),
        (m_pTime ? wxString::Format(_T("%d/%d"),
            m_pTime->GetNumBeats(), m_pTime->GetBeatType()).c_str() : _T("No")),
         m_nAccidentals[0], m_nAccidentals[1], m_nAccidentals[2], m_nAccidentals[3],
         m_nAccidentals[4], m_nAccidentals[5], m_nAccidentals[6] );
#endif
    return sDump;
}

void lmContext::PropagateValueWhileInherited(lmStaffObj* pSO)
{
    //update this and following contexts in the chain. If following context inherited a value
    //form removed context, we have to update these inherited values. An example: if we are
    //removing a clef and next context is created by a time signature, in this context
    //the clef was inherited.

    if (pSO->IsClef() && m_fClefInherited)
        m_pClef = (m_pPrev ? m_pPrev->GetClef() : (lmClef*)NULL);
    else if (pSO->IsTimeSignature() && m_fTimeInherited)
        m_pTime = (m_pPrev ? m_pPrev->GetTime() : (lmTimeSignature*)NULL);
    else if (pSO->IsKeySignature() && m_fKeyInherited)
        SetKey(m_pPrev ? m_pPrev->GetKey() : (lmKeySignature*)NULL);
    else
        return;     //no inherited

    //propagate to next one
    m_fModified = true;
    if (m_pNext)
        m_pNext->PropagateValueWhileInherited(pSO);
}

void lmContext::PropagateNewWhileInherited(lmStaffObj* pNewSO)
{
    //update this and following contexts in the chain with new inserted context.
    //If following context inherited a value it must be replaced by the value
    //from the new context.

    if (pNewSO->IsClef() && m_fClefInherited)
        m_pClef = (m_pPrev ? m_pPrev->GetClef() : (lmClef*)NULL);
    else if (pNewSO->IsTimeSignature() && m_fTimeInherited)
        m_pTime = (lmTimeSignature*)pNewSO;
    else if (pNewSO->IsKeySignature() && m_fKeyInherited)
        SetKey((lmKeySignature*)pNewSO);
    else
        return;     //no inherited

    //propagate to next one
    m_fModified = true;
    if (m_pNext)
        m_pNext->PropagateNewWhileInherited(pNewSO);
}
