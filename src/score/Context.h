//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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
/*! @file Context.h
    @brief Header file for class lmContext
    @ingroup score_kernel
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __CONTEXT_H__        //to avoid nested includes
#define __CONTEXT_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "score.h"


class lmContext
{
public:
    lmContext(lmClef* pClef, lmKeySignature* pKey, lmTimeSignature* pTime);
    ~lmContext() {}

    void SetAccidental(int i, int nValue) { m_nAccidentals[i] = nValue; }
    int GetAccidentals(int i) { return m_nAccidentals[i]; }

    lmClef*            GetClef() { return m_pClef; }
    lmKeySignature*    GeyKey() { return m_pKey; }
    lmTimeSignature*    GetTime() { return m_pTime; }

    //management of references counter
    void StopUsing() { m_nNumReferences--; }
    void StartUsing() { m_nNumReferences++; }


private:
    void InitializeAccidentals();

        // member variables

    //information about current clef, key and time signature
    lmClef*            m_pClef;
    lmKeySignature*    m_pKey;
    lmTimeSignature*    m_pTime;

    //the next array keeps information about the accidentals applicable to each
    //note. Each element refers to one note: 0=Do, 1=Re, 2=Mi, 3=Fa, ... , 6=Si
    int        m_nAccidentals[7];

    //counter of references to this context (to delete it when not used)
    int        m_nNumReferences;

};


// declare a list of lmContext objects class
#include "wx/list.h"
WX_DECLARE_LIST(lmContext, ContextList);

// this defines the type ArrayOfContexts as an array of lmContext pointers
WX_DEFINE_ARRAY(lmContext*, ArrayOfContexts);


#endif  // __CONTEXT_H__
