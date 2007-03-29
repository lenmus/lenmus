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
/*! @file Context.cpp
    @brief Implementation file for class lmContext
    @ingroup score_kernel
*/
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "Conversion.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "Score.h"
#include "Context.h"

//implementation of the Context objects List
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(ContextList);

//access to global functions
extern void ComputeAccidentals(EKeySignatures nKeySignature, int    nAccidentals[]);

lmContext::lmContext(lmClef* pClef, lmKeySignature* pKey, lmTimeSignature* pTime)
{
    m_pClef = pClef;
    m_pKey = pKey;
    m_pTime = pTime;

    InitializeAccidentals();

    m_nNumReferences = 0;

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
