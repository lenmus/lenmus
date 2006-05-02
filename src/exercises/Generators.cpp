// RCS-ID: $Id: Generators.cpp,v 1.6 2006/02/23 19:19:15 cecilios Exp $
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
/*! @file Generators.cpp
    @brief Implementation file for generator classes
    @ingroup generators
*/
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "Generators.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "Generators.h"

/*! @class lmRandomGenerator
    Random generators for various elementes: clefs, notes, keys, tiem signatures, etc.
    Generation methods accept as parameter a constrain object of proper class
*/
lmRandomGenerator::lmRandomGenerator()
{

}

//! Generates a random number in the closed interval [nMin, nMax].
int lmRandomGenerator::RandomNumber(int nMin, int nMax)
{
    // note that rand() returns an int in the range 0 to RAND_MAX (= 0x7fff)
    int nRange = nMax - nMin + 1;
    int nRnd = rand() % nRange;            // 0..nRange-1 = 0..(nMax-nMin+1)-1 = 0..(nMax-nMin)
    return nRnd + nMin;                    // nMin ... (nMax-nMin)+nMin = nMin...nMax

}

bool lmRandomGenerator::FlipCoin()
{
    return ((rand() & 0x01) == 0x01);     //true in odd number, false if even
}

//! Generates a random clef, choosen to satisfy the received constrains
EClefType lmRandomGenerator::GenerateClef(lmClefConstrain* pValidClefs)
{
    int nWatchDog = 0;
    int nClef = RandomNumber(lmMIN_CLEF, lmMAX_CLEF);
    while (!pValidClefs->IsValid((EClefType)nClef)) {
        nClef = RandomNumber(lmMIN_CLEF, lmMAX_CLEF);
        if (nWatchDog++ == 1000) {
            wxMessageBox(_("Program error: Loop detected in lmRandomGenerator::GenerateClef."));
            return lmMIN_CLEF;
        }
    }
    return (EClefType)nClef;
}

//! Generates a random key signature, choosen to satisfy the received constrains
EKeySignatures lmRandomGenerator::GenerateKey(lmKeyConstrains* pValidKeys)
{
    int nWatchDog = 0;
    int nKey = RandomNumber(lmMIN_KEY, lmMAX_KEY);
    while (!pValidKeys->IsValid((EKeySignatures)nKey)) {
        nKey = RandomNumber(lmMIN_KEY, lmMAX_KEY);
        if (nWatchDog++ == 1000) {
            wxMessageBox(_("Program error: Loop detected in lmRandomGenerator::GenerateKey."));
            return lmMIN_KEY;
        }
    }
    return (EKeySignatures)nKey;
}

EKeySignatures lmRandomGenerator::RandomKeySignature()
{
    return (EKeySignatures)RandomNumber(lmMIN_KEY, lmMAX_KEY);
}

//! Generates a random time signature, choosen to satisfy the received constrains
ETimeSignature lmRandomGenerator::GenerateTimeSign(lmTimeSignConstrains* pValidTimeSignatures)
{
    int nWatchDog = 0;
    int nKey = RandomNumber(lmMIN_TIME_SIGN, lmMAX_TIME_SIGN);
    while (!pValidTimeSignatures->IsValid((ETimeSignature)nKey)) {
        nKey = RandomNumber(lmMIN_TIME_SIGN, lmMAX_TIME_SIGN);
        if (nWatchDog++ == 1000) {
            wxMessageBox(_("Program error: Loop detected in lmRandomGenerator::GenerateTime."));
            return lmMIN_TIME_SIGN;
        }
    }
    return (ETimeSignature)nKey;
}

ETimeSignature lmRandomGenerator::RandomTimeSignature()
{
    return (ETimeSignature)RandomNumber(lmMIN_TIME_SIGN, lmMAX_TIME_SIGN);
}



/*! Generates a random pitch in the range nMinLine to nMinLine+nRange-1, both included.
    If fRest==true also pitch = 0 (rest) can be generated.
*/
lmPitch lmRandomGenerator::GenerateRandomPitch(int nMinLine, int nRange, bool fRests, 
                                     EClefType nClef)
{
    int nPitch;
    
    if (fRests) {
        //allow for generating rests
        nRange++;
        nPitch = ((nRange * rand())/RAND_MAX) + nMinLine - 1;
        if (nPitch < nMinLine) nPitch = 0;
    }
    else {
        //do not generate rests
        nPitch = ((nRange * rand())/RAND_MAX) + nMinLine;
    }

    //correct note pitch to suit key signature base line
    switch (nClef) {
        case eclvSol:        nPitch += 29;    break;
        case eclvFa4:        nPitch += 17;    break;
        case eclvFa3:        nPitch += 19;    break;
        case eclvDo1:        nPitch += 27;    break;
        case eclvDo2:        nPitch += 25;    break;
        case eclvDo3:        nPitch += 23;    break;
        case eclvDo4:        nPitch += 21;    break;
        case eclvPercusion:    nPitch = 34;    break;
    }
    
    return nPitch;
    
}

