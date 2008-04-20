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
#pragma implementation "Context.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "Score.h"
#include "Context.h"


//access to global functions
extern void ComputeAccidentals(lmEKeySignatures nKeySignature, int nAccidentals[]);

lmContext::lmContext(lmClef* pClef, lmKeySignature* pKey, lmTimeSignature* pTime,
              bool fClefInherited, bool fKeyInherited, bool fTimeInherited)
{
	//create a new context
    m_pClef = pClef;
    m_pKey = pKey;
    m_pTime = pTime;

    m_fClefInherited = fClefInherited;
    m_fKeyInherited = fKeyInherited;
    m_fTimeInherited = fTimeInherited;

    InitializeAccidentals();
	m_pPrev = (lmContext*) NULL;
	m_pNext = (lmContext*) NULL;
}

lmContext::lmContext(lmContext* pContext)
{
    m_pClef = pContext->GetClef();
    m_pKey = pContext->GetKey();
    m_pTime = pContext->GetTime();

    CopyAccidentals(pContext);
	m_pPrev = (lmContext*) NULL;
	m_pNext = (lmContext*) NULL;
}

void lmContext::InitializeAccidentals()
{
    // initialize array: no accidentals
    for (int i=0; i < 7; i++) {
        m_nAccidentals[i] = 0;
    }
    if (!m_pKey) return;

    ComputeAccidentals(m_pKey->GetKeyType(), m_nAccidentals);

}

void lmContext::CopyAccidentals(lmContext* pContext)
{
    int i;
    for (i=0; i < 7; i++) {
        m_nAccidentals[i] = pContext->GetAccidentals(i);
    }

}

wxString lmContext::Dump()
{
    wxString sDump = wxString::Format(_T("Context: clef: %s, key: %s, time: %s\n"),
        (m_pClef ? GetClefLDPNameFromType(m_pClef->GetClefType()).c_str() : _T("No")),
        (m_pKey ? GetKeyLDPNameFromType(m_pKey->GetKeyType()).c_str() : _T("No")),
        (m_pTime ? wxString::Format(_T("%d/%d"),
            m_pTime->GetNumBeats(), m_pTime->GetBeatType()).c_str() : _T("No")) );
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
        m_pKey = (m_pPrev ? m_pPrev->GetKey() : (lmKeySignature*)NULL);
    else
        return;     //no inherited

    //propagate to next one
    if (m_pNext)
        m_pNext->PropagateValueWhileInherited(pSO);
}
