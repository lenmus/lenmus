// RCS-ID: $Id: Context.cpp,v 1.3 2006/02/23 19:22:56 cecilios Exp $
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

    ComputeAccidentals(m_pKey->GetType(), m_nAccidentals);


    //// add accidentals implied by key signature
 //   switch (m_pKey->GetType()) {
    //    case earmDo:
    //    case earmLam:
 //           //no accidentals
    //        break;

 //       //Sharps ---------------------------------------
 //       case earmSol:
    //    case earmMim:
 //           m_nAccidentals[3] = 1;     //Fa #
    //        break;
 //       case earmRe:
    //    case earmSim:
 //           m_nAccidentals[3] = 1;     //Fa #
 //           m_nAccidentals[0] = 1;     //Do #
    //        break;
 //       case earmLa:
    //    case earmFasm:
 //           m_nAccidentals[3] = 1;     //Fa #
 //           m_nAccidentals[0] = 1;     //Do #
 //           m_nAccidentals[4] = 1;     //Sol #
 //            break;
 //      case earmMi:
    //    case earmDosm:
 //           m_nAccidentals[3] = 1;     //Fa #
 //           m_nAccidentals[0] = 1;     //Do #
 //           m_nAccidentals[4] = 1;     //Sol #
 //           m_nAccidentals[1] = 1;     //Re #
    //        break;
 //       case earmSi:
    //    case earmSolsm:
 //           m_nAccidentals[3] = 1;     //Fa #
 //           m_nAccidentals[0] = 1;     //Do #
 //           m_nAccidentals[4] = 1;     //Sol #
 //           m_nAccidentals[1] = 1;     //Re #
 //           m_nAccidentals[5] = 1;     //La #
    //        break;
 //       case earmFas:
    //    case earmResm:
 //           m_nAccidentals[3] = 1;     //Fa #
 //           m_nAccidentals[0] = 1;     //Do #
 //           m_nAccidentals[4] = 1;     //Sol #
 //           m_nAccidentals[1] = 1;     //Re #
 //           m_nAccidentals[5] = 1;     //La #
 //           m_nAccidentals[2] = 1;     //Mi #
    //        break;
 //       case earmDos:
    //    case earmLasm:
 //           m_nAccidentals[3] = 1;     //Fa #
 //           m_nAccidentals[0] = 1;     //Do #
 //           m_nAccidentals[4] = 1;     //Sol #
 //           m_nAccidentals[1] = 1;     //Re #
 //           m_nAccidentals[5] = 1;     //La #
 //           m_nAccidentals[2] = 1;     //Mi #
 //           m_nAccidentals[6] = 1;     //Si #
 //            break;

    //    //Flats -------------------------------------------
    //    case earmFa:
    //    case earmRem:
 //           m_nAccidentals[6] = -1;         //Si b
    //        break;
 //       case earmSib:
    //    case earmSolm:
 //           m_nAccidentals[6] = -1;         //Si b
 //           m_nAccidentals[2] = -1;         //Mi b
 //            break;
 //      case earmMib:
    //    case earmDom:
 //           m_nAccidentals[6] = -1;         //Si b
 //           m_nAccidentals[2] = -1;         //Mi b
 //           m_nAccidentals[5] = -1;         //La b
    //        break;
 //       case earmLab:
    //    case earmFam:
 //           m_nAccidentals[6] = -1;         //Si b
 //           m_nAccidentals[2] = -1;         //Mi b
 //           m_nAccidentals[5] = -1;         //La b
 //           m_nAccidentals[1] = -1;         //Re b
 //            break;
 //       case earmReb:
    //    case earmSibm:
 //           m_nAccidentals[6] = -1;         //Si b
 //           m_nAccidentals[2] = -1;         //Mi b
 //           m_nAccidentals[5] = -1;         //La b
 //           m_nAccidentals[1] = -1;         //Re b
 //           m_nAccidentals[4] = -1;         //Sol b
    //        break;
 //       case earmSolb:
    //    case earmMibm:
 //           m_nAccidentals[6] = -1;         //Si b
 //           m_nAccidentals[2] = -1;         //Mi b
 //           m_nAccidentals[5] = -1;         //La b
 //           m_nAccidentals[1] = -1;         //Re b
 //           m_nAccidentals[4] = -1;         //Sol b
 //           m_nAccidentals[0] = -1;         //Do b
    //        break;
 //       case earmDob:
    //    case earmLabm:
 //           m_nAccidentals[6] = -1;         //Si b
 //           m_nAccidentals[2] = -1;         //Mi b
 //           m_nAccidentals[5] = -1;         //La b
 //           m_nAccidentals[1] = -1;         //Re b
 //           m_nAccidentals[4] = -1;         //Sol b
 //           m_nAccidentals[0] = -1;         //Do b
 //           m_nAccidentals[3] = -1;         //Fa b
    //        break;
 //       default:
    //        wxASSERT(false);
 //   }

}
