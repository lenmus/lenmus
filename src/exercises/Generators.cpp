//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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
#pragma implementation "Generators.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "Generators.h"
#include "../score/KeySignature.h"

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

//! Generates a random clef, choosen to satisfy the received constraints
lmEClefType lmRandomGenerator::GenerateClef(lmClefConstrain* pValidClefs)
{
    int nWatchDog = 0;
    int nClef = RandomNumber(lmMIN_CLEF, lmMAX_CLEF);
    while (!pValidClefs->IsValid((lmEClefType)nClef)) {
        nClef = RandomNumber(lmMIN_CLEF, lmMAX_CLEF);
        if (nWatchDog++ == 1000) {
            wxMessageBox(_("Program error: Loop detected in lmRandomGenerator::GenerateClef."));
            return lmMIN_CLEF;
        }
    }
    return (lmEClefType)nClef;
}

//! Generates a random key signature, choosen to satisfy the received constraints
lmEKeySignatures lmRandomGenerator::GenerateKey(lmKeyConstrains* pValidKeys)
{
    int nWatchDog = 0;
    int nKey = RandomNumber(lmMIN_KEY, lmMAX_KEY);
    while (!pValidKeys->IsValid((lmEKeySignatures)nKey)) {
        nKey = RandomNumber(lmMIN_KEY, lmMAX_KEY);
        if (nWatchDog++ == 1000) {
            wxMessageBox(_("Program error: Loop detected in lmRandomGenerator::GenerateKey."));
            return lmMIN_KEY;
        }
    }
    return (lmEKeySignatures)nKey;
}

lmEKeySignatures lmRandomGenerator::RandomKeySignature()
{
    return (lmEKeySignatures)RandomNumber(lmMIN_KEY, lmMAX_KEY);
}

//! Generates a random time signature, choosen to satisfy the received constraints
lmETimeSignature lmRandomGenerator::GenerateTimeSign(lmTimeSignConstrains* pValidTimeSignatures)
{
    int nWatchDog = 0;
    int nKey = RandomNumber(lmMIN_TIME_SIGN, lmMAX_TIME_SIGN);
    while (!pValidTimeSignatures->IsValid((lmETimeSignature)nKey)) {
        nKey = RandomNumber(lmMIN_TIME_SIGN, lmMAX_TIME_SIGN);
        if (nWatchDog++ == 1000) {
            wxMessageBox(_("Program error: Loop detected in lmRandomGenerator::GenerateTime."));
            return lmMIN_TIME_SIGN;
        }
    }
    return (lmETimeSignature)nKey;
}

lmETimeSignature lmRandomGenerator::RandomTimeSignature()
{
    return (lmETimeSignature)RandomNumber(lmMIN_TIME_SIGN, lmMAX_TIME_SIGN);
}

lmDPitch lmRandomGenerator::GenerateRandomDPitch(int nMinLine, int nRange, bool fRests,
                                     lmEClefType nClef)
{
    // Generates a random pitch in the range nMinLine to nMinLine+nRange-1,
    // both included.
    // If fRest==true also pitch = 0 (rest) can be generated.

    int nPitch;

    if (fRests)
    {
        //also generate rests
        nPitch = RandomNumber(0, nRange);
        nPitch = (nPitch == nRange ? 0 : nPitch + nMinLine);
    }
    else
    {
        //do not generate rests
        nPitch = RandomNumber(0, nRange-1) + nMinLine;
    }

    //correct note pitch to suit key signature base line
    switch (nClef)
    {
        case lmE_Sol:        nPitch += 29;    break;
        case lmE_Fa4:        nPitch += 17;    break;
        case lmE_Fa3:        nPitch += 19;    break;
        case lmE_Do1:        nPitch += 27;    break;
        case lmE_Do2:        nPitch += 25;    break;
        case lmE_Do3:        nPitch += 23;    break;
        case lmE_Do4:        nPitch += 21;    break;
        case lmE_Percussion:    nPitch = 34;    break;
        default:
            wxLogMessage(_T("[lmRandomGenerator::GenerateRandomDPitch] No treatment for clef %s"),
                        nClef);
            wxASSERT(false);
    }

    return nPitch;

}
wxString lmRandomGenerator::GenerateRandomRootNote(lmEClefType nClef,
                                                   lmEKeySignatures nKey,
                                                   bool fAllowAccidentals)
{
    // Get the index (0..6, 0=Do, 1=Re, 3=Mi, ... , 6=Si) to the root note for
    // the Key signature. For example, if nKeySignature is La sharp minor it returns
    // index = 5 (La)
    int nRoot = GetRootNoteIndex(nKey);
    wxString sNotes = _T("cdefgab");

    // Get the accidentals implied by the key signature.
    // Each element of the array refers to one note: 0=Do, 1=Re, 2=Mi, 3=Fa, ... , 6=Si
    // and its value can be one of: 0=no accidental, -1 = a flat, 1 = a sharp
    int nAccidentals[7];
    ComputeAccidentals(nKey, nAccidentals);

    wxString sAcc[5] = { _T("--"), _T("-"), _T(""), _T("+"), _T("x") };
    wxString sRootNote = sAcc[nAccidentals[nRoot]+ 2].c_str() +
                         sNotes.substr(nRoot, 1) +
                         (nRoot > 4 ? _T("3") : _T("4"));

    return sRootNote;
}
