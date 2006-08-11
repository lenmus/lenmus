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
/*! @file ChordManager.cpp
    @brief Implementation file for class lmChordManager
    @ingroup auxmusic
*/
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "ChordManager.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "ChordManager.h"
#include "Conversion.h"
#include "../ldp_parser/AuxString.h"
#include "../exercises/Generators.h"
#include "../score/KeySignature.h"

//access to error's logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;


typedef struct lmChordDataStruct {
    int nNumNotes;
    wxString sIntervals[3];
} lmChordData;

static wxString m_sChordName[ect_Max];
static m_fStringsInitialized = false;

//! @aware Array indexes are in correspondence with enum EChordType
// - intervals are from root note
//      number + type:   m=minor, M=major, p=perfect, a=augmented, d=diminished
static lmChordData tData[ect_Max] = { 
    { 3, _T("M3"), _T("p5"), _T("") },        //MT        - MajorTriad
    { 3, _T("m3"), _T("p5"), _T("") },        //mT        - MinorTriad
    { 3, _T("M3"), _T("a5"), _T("") },        //aT        - AugTriad
    { 3, _T("m3"), _T("d5"), _T("") },        //dT        - DimTriad
    { 3, _T("p4"), _T("p5"), _T("") },        //I,IV,V    - Suspended_4th
    { 3, _T("M2"), _T("p5"), _T("") },        //I,II,V    - Suspended_2nd
    { 4, _T("M3"), _T("p5"), _T("M7") },      //MT + M7   - MajorSeventh
    { 4, _T("M3"), _T("p5"), _T("m7") },      //MT + m7   - DominantSeventh
    { 4, _T("m3"), _T("p5"), _T("m7") },      //mT + m7   - MinorSeventh
    { 4, _T("m3"), _T("d5"), _T("d7") },      //dT + d7   - HalfDimSeventh
    { 4, _T("m3"), _T("d5"), _T("m7") },      //dT + m7   - HalfDimSeventh
    { 4, _T("M3"), _T("a5"), _T("M7") },      //aT + M7   - AugMajorSeventh
    { 4, _T("M3"), _T("a5"), _T("m7") },      //aT + m7   - AugSeventh
    { 4, _T("m3"), _T("p5"), _T("M7") },      //mT + M7   - MinorMajorSeventh
    { 4, _T("M3"), _T("p5"), _T("M6") },      //MT + M6   - MajorSixth
    { 4, _T("m3"), _T("p5"), _T("M6") },      //mT + M6   - MinorSixth
    { 3, _T("M3"), _T("a4"), _T("a6") },      //          - AugSixth
};


//-------------------------------------------------------------------------------------
// Implementation of lmChordManager class


lmChordManager::lmChordManager(wxString sRootNote, EChordType nChordType, 
                               int nInversion, EKeySignatures nKey)
{
    //parameter 'nInversion' is encoded as follows:
    //  0 - root position
    //  1 - 1st inversion
    //  2 - 2nd inversion
    //  and so on

    //save parameters
    m_nType = nChordType;
    m_nKey = nKey;
    m_nInversion = nInversion;

    if (NoteToBits(sRootNote, &m_tBits[0])) 
        wxASSERT(false);

    //get the intervals that form the chord
    int i;
    wxString sIntval[3], sNewIntv[3];
    sIntval[0] = tData[m_nType].sIntervals[0];
    sIntval[1] = tData[m_nType].sIntervals[1];
    sIntval[2] = tData[m_nType].sIntervals[2];

    //correction for inversions
    if (m_nInversion == 1)
    {
        sNewIntv[0] = SubstractIntervals( sIntval[1], sIntval[0] );

        if (sIntval[2] == _T("")) {
            sNewIntv[1] = InvertInterval( sIntval[0] );
            sNewIntv[2] = _T("");
        }
        else {
            sNewIntv[1] = SubstractIntervals( sIntval[2], sIntval[0] );
            sNewIntv[2] = InvertInterval( sIntval[0] );
        }
    }
    else if (m_nInversion == 2)
    {
        if (sIntval[2] == _T("")) {
            sNewIntv[0] = InvertInterval( sIntval[1] );
            sNewIntv[1] = InvertInterval( sIntval[0] );
            sNewIntv[2] = _T("");
        }
        else {
            sNewIntv[0] = SubstractIntervals( sIntval[2], sIntval[1] );
            sNewIntv[1] = InvertInterval( sIntval[1] );
            sNewIntv[2] = InvertInterval( sIntval[0] );
        }
    } 
    else if (m_nInversion == 3)
    {
        sNewIntv[0] = SubstractIntervals( _T("p8"), sIntval[2] );
        sNewIntv[1] = AddIntervals( sNewIntv[0], sIntval[0] );
        sNewIntv[2] = AddIntervals( sNewIntv[0], sIntval[1] );
    } 
    if (m_nInversion != 0) {
        sIntval[0] = sNewIntv[0];
        sIntval[1] = sNewIntv[1];
        sIntval[2] = sNewIntv[2];
    }

    //get notes that form the interval
    wxLogMessage(_T("[lmChordManager] Root note = %s, interval type=%s, inversion=%d"),
                  NoteBitsToName(m_tBits[0], m_nKey), GetName(), m_nInversion );
    for (i=1; i < tData[m_nType].nNumNotes; i++) {
        ComputeInterval(&m_tBits[0], sIntval[i-1], &m_tBits[i]);
        wxLogMessage(_T("[lmChordManager] Note %d = %s, (Bits: Step=%d, Octave=%d, Accidentals=%d, StepSemitones=%d), key=%d"),
                     i, NoteBitsToName(m_tBits[i],m_nKey),
                     m_tBits[i].nStep, m_tBits[i].nOctave, m_tBits[i].nAccidentals, m_tBits[i].nStepSemitones,
                     m_nKey );
    }

}

lmChordManager::~lmChordManager()
{
}

int lmChordManager::GetNumNotes()
{ 
    return tData[m_nType].nNumNotes;
}

int lmChordManager::GetMidiNote(int i)
{ 
    wxASSERT(i < GetNumNotes());
    return m_ntMidi[i];
}

int lmChordManager::GetMidiNote(int nMidiRoot, wxString sInterval)
{
    // Receives a Midi pitch and a string encoding the interval as follows:
    // - intval = number + type: 
    //      m=minor, M=major, p=perfect, a=augmented, 
    //      d=diminished, += double aug. -=double dim.
    //   examples:
    //      3M - major 3th
    //      +7 - double augmented 7th
    //
    // Returns the top midi pitch of the requested interval

    //  intval  semitones
    //  2m      1
    //  2M      2
    //  3m      3
    //  3M      4
    //  3a/4p   5
    //  4a/5d   6 
    //  5p      7
    //  5a/6m   8      
    //  6M      9
    //  6a/7m   10
    //  7M      11
    //  8p      12

    if (sInterval == _T("m2"))  return nMidiRoot + 1;
    if (sInterval == _T("M2"))  return nMidiRoot + 2;
    if (sInterval == _T("m3"))  return nMidiRoot + 3;
    if (sInterval == _T("M3"))  return nMidiRoot + 4;
    if (sInterval == _T("a3"))  return nMidiRoot + 5;
    if (sInterval == _T("p4"))  return nMidiRoot + 5;
    if (sInterval == _T("a4"))  return nMidiRoot + 6;
    if (sInterval == _T("d5"))  return nMidiRoot + 6;
    if (sInterval == _T("p5"))  return nMidiRoot + 7;
    if (sInterval == _T("a5"))  return nMidiRoot + 8;
    if (sInterval == _T("m6"))  return nMidiRoot + 8;
    if (sInterval == _T("M6"))  return nMidiRoot + 9;
    if (sInterval == _T("a6"))  return nMidiRoot + 10;
    if (sInterval == _T("m7"))  return nMidiRoot + 10;
    if (sInterval == _T("M7"))  return nMidiRoot + 11;
    if (sInterval == _T("p8"))  return nMidiRoot + 12;

    return 0;

}

wxString lmChordManager::NoteBitsToName(lmNoteBits& tBits, EKeySignatures nKey)
{
    static wxString m_sSteps = _T("cdefgab");

    // Get the accidentals implied by the key signature. 
    // Each element of the array refers to one note: 0=Do, 1=Re, 2=Mi, 3=Fa, ... , 6=Si
    // and its value can be one of: 0=no accidental, -1 = a flat, 1 = a sharp
    int nAccidentals[7];
    ComputeAccidentals(nKey, nAccidentals);

    //compute accidentals note accidentals
    wxString sResult = _T("");
    if (tBits.nAccidentals == 1)
        sResult = _T("+");
    else if (tBits.nAccidentals == 2)
        sResult = _T("x");
    else if (tBits.nAccidentals == -1)
        sResult = _T("-");
    else if (tBits.nAccidentals == -2)
        sResult = _T("--");

    //change note accidentals to take key into account
    if (nAccidentals[tBits.nStep] != 0) {
        if (tBits.nAccidentals == nAccidentals[tBits.nStep])
            sResult = _T("");   //replace note accidental by key accidental
        else if (tBits.nAccidentals == 0)
            sResult = _T("=");  //force a natural
        else
            ;   //leave note accidentals
    }

    // compute step letter
    sResult += m_sSteps.Mid(tBits.nStep, 1);

    // compute octave
    sResult += wxString::Format(_T("%d"), tBits.nOctave);

    return sResult;

}

wxString lmChordManager::ComputeInterval(wxString sRootNote, wxString sInterval)
{
    //Root note elements. i.e.: '+d4' -> (1, 4, 1)
    lmNoteBits tRoot;
    if (NoteToBits(sRootNote, &tRoot)) 
        wxASSERT(false);

    lmNoteBits tNew;
    ComputeInterval(&tRoot, sInterval, &tNew);

    return NoteBitsToName(tNew, m_nKey);

}

void lmChordManager::ComputeInterval(lmNoteBits* pRoot, wxString sInterval,
                                     lmNoteBits* pNewNote)
{
    //interval elements. i.e.: '5a' -> (5, 8)
    lmIntvBits tIntval;
    if (IntervalNameToBits(sInterval, &tIntval))
        wxASSERT(false);

    // compute new step
    int nNewStepFull = pRoot->nStep + tIntval.nNum - 1;
    pNewNote->nStep = nNewStepFull % 7;

    //compute octave increment
    int nIncrOctave = (pNewNote->nStep == nNewStepFull ? 0 : (nNewStepFull - pNewNote->nStep)/7 );
    pNewNote->nOctave = pRoot->nOctave + nIncrOctave;

    //compute new step semitones
    pNewNote->nStepSemitones = StepToSemitones(pNewNote->nStep);

    //compute new accidentals
    pNewNote->nAccidentals = (pRoot->nStepSemitones + tIntval.nSemitones - 
                         pNewNote->nStepSemitones) % 12 + pRoot->nAccidentals;

}


bool lmChordManager::NoteToBits(wxString sNote, lmNoteBits* pBits)
{
    //Returns true if error
    //- sNote must be letter followed by a number (i.e.: "c4" ) optionally precedeed by
    //  accidentals (i.e.: "++c4")
    //- It is assumed that sNote is trimmed (no spaces before or after data)
    //  and lower case.
    
    //split the string: accidentals and name
    wxString sAccidentals;
    switch (sNote.Len()) {
        case 2: 
            sAccidentals = _T("");
            break;
        case 3:
            sAccidentals = sNote.Mid(0, 1);
            sNote = sNote.Mid(1, 2);
            break;
        case 4:
            sAccidentals = sNote.Mid(0, 2);
            sNote = sNote.Mid(2, 2);
            break;
        default:
            return true;   //error
    }

    //compute step
    int nStep = StepToInt( sNote.Left(1) );
    if (nStep == -1) return true;   //error
    pBits->nStep = nStep;

    //compute octave
    wxString sOctave = sNote.Mid(1, 1);
    long nOctave;
    if (!sOctave.ToLong(&nOctave)) return true;    //error
    pBits->nOctave = (int)nOctave;
    
    //compute accidentals
    int nAccidentals = AccidentalsToInt(sAccidentals);
    if (nAccidentals == -999) return true;  //error
    pBits->nAccidentals = nAccidentals;
    
    //compute step semitones
    pBits->nStepSemitones = StepToSemitones( pBits->nStep );

    return false;  //no error

}

int lmChordManager::StepToSemitones(int nStep)
{
    //Returns -1 if error
    // 'c'=0, 'd'=2, 'e'=4, 'f'=5, 'g'=7, 'a'=9, 'b'=11
    if (nStep == 0)  return 0;
    if (nStep == 1)  return 2;
    if (nStep == 2)  return 4;
    if (nStep == 3)  return 5;
    if (nStep == 4)  return 7;
    if (nStep == 5)  return 9;
    if (nStep == 6)  return 11;
    return -1;  //error
}

int lmChordManager::AccidentalsToInt(wxString sAccidentals)
{
    //Returns -999 if error
    // '--'=-1, '-'=-1, ''=0, '+'=+1, '++'=+2 'x'=2
    if (sAccidentals == _T(""))     return 0;
    if (sAccidentals == _T("-"))    return -1;
    if (sAccidentals == _T("--"))   return -2;
    if (sAccidentals == _T("+"))    return 1;
    if (sAccidentals == _T("++"))   return 2;
    if (sAccidentals == _T("x"))    return 2;
    return -999;
}

int lmChordManager::StepToInt(wxString sStep)
{
    //Returns -1 if error
    // 'c'=0, 'd'=1, 'e'=2, 'f'=3, 'g'=4, 'a'=5, 'b'=6
    if (sStep == _T("c")) return 0;
    if (sStep == _T("d")) return 1;
    if (sStep == _T("e")) return 2;
    if (sStep == _T("f")) return 3;
    if (sStep == _T("g")) return 4;
    if (sStep == _T("a")) return 5;
    if (sStep == _T("b")) return 6;
    return -1;
}

bool lmChordManager::IntervalNameToBits(wxString sInterval, lmIntvBits* pBits)
{
    //Restrictions: any interval , including greater that one octave, but limited to
    // d, m , p , M and a. That is, it is not allowed double augmented, double diminished, etc.

    //split interval number and type
    int i = 0;
    while (!(sInterval.Mid(i, 1)).IsNumber() ) {
        i++;
    }
    wxString sChar = sInterval.Mid(0, i);
    wxString sNum = sInterval.Mid(i);

    long nNum;
    sNum.ToLong(&nNum);
    pBits->nNum = (int)nNum;

    //reduce the interval
    int nOctaves = (nNum - 1) / 7;
    nNum = 1 + (nNum - 1) % 7;

    // compute semitones implied by interval type
    int nSemi;
    if (nNum == 1)      nSemi = -1; 
    else if (nNum == 2) nSemi = 0;
    else if (nNum == 3) nSemi = 2; 
    else if (nNum == 4) nSemi = 4;
    else if (nNum == 5) nSemi = 6;
    else if (nNum == 6) nSemi = 7;
    else if (nNum == 7) nSemi = 9;
    else
        wxASSERT(false);    //impossible

    if ( nNum == 1 || nNum == 4 || nNum == 5) {
        if (sChar == _T("d"))       nSemi += 0;
        else if (sChar == _T("p"))  nSemi += 1;
        else if (sChar == _T("a"))  nSemi += 2;
        else wxASSERT(false);
    }
    else {  // 2, 3, 6, 7
        if (sChar == _T("d"))       nSemi += 0;
        else if (sChar == _T("m"))  nSemi += 1;
        else if (sChar == _T("M"))  nSemi += 2;
        else if (sChar == _T("a"))  nSemi += 3;
        else wxASSERT(false);
    }
    pBits->nSemitones = nSemi + 12 * nOctaves;


    //compute semitones
    //  intval  semitones
    //  2d      0
    //  2m      1
    //  2M/3d   2
    //  3m/2a   3
    //  3M/4d   4
    //  4p/3a   5
    //  4a/5d   6 
    //  5p/6d   7
    //  6m/5a   8      
    //  6M/7d   9
    //  7m/6a   10
    //  7M/8d   11
    //  8p/7a   12

    //if      (sInterval == _T("d2")) pBits->nSemitones = 0;
    //else if (sInterval == _T("m2")) pBits->nSemitones = 1;
    //else if (sInterval == _T("M2")) pBits->nSemitones = 2;
    //else if (sInterval == _T("a2")) pBits->nSemitones = 3;
    //else if (sInterval == _T("d3")) pBits->nSemitones = 2;
    //else if (sInterval == _T("m3")) pBits->nSemitones = 3;
    //else if (sInterval == _T("M3")) pBits->nSemitones = 4;
    //else if (sInterval == _T("a3")) pBits->nSemitones = 5;
    //else if (sInterval == _T("d4")) pBits->nSemitones = 4;
    //else if (sInterval == _T("p4")) pBits->nSemitones = 5;
    //else if (sInterval == _T("a4")) pBits->nSemitones = 6;
    //else if (sInterval == _T("d5")) pBits->nSemitones = 6;
    //else if (sInterval == _T("p5")) pBits->nSemitones = 7;
    //else if (sInterval == _T("a5")) pBits->nSemitones = 8;
    //else if (sInterval == _T("d6")) pBits->nSemitones = 7;
    //else if (sInterval == _T("m6")) pBits->nSemitones = 8;
    //else if (sInterval == _T("M6")) pBits->nSemitones = 9;
    //else if (sInterval == _T("a6")) pBits->nSemitones = 10;
    //else if (sInterval == _T("d7")) pBits->nSemitones = 9;
    //else if (sInterval == _T("m7")) pBits->nSemitones = 10;
    //else if (sInterval == _T("M7")) pBits->nSemitones = 11;
    //else if (sInterval == _T("a7")) pBits->nSemitones = 12;
    //else if (sInterval == _T("d8")) pBits->nSemitones = 11;
    //else if (sInterval == _T("p8")) pBits->nSemitones = 12;
    //else return true; //error

    return false;

}

wxString lmChordManager::InvertInterval(wxString sInterval)
{
    wxASSERT(sInterval.Length() == 2);

    if      (sInterval == _T("d2")) return _T("a7");
    else if (sInterval == _T("m2")) return _T("M7");
    else if (sInterval == _T("M2")) return _T("m7");
    else if (sInterval == _T("a2")) return _T("d7");
    else if (sInterval == _T("d3")) return _T("a6");
    else if (sInterval == _T("m3")) return _T("M6");
    else if (sInterval == _T("M3")) return _T("m6");
    else if (sInterval == _T("a3")) return _T("d6");
    else if (sInterval == _T("d4")) return _T("a4");
    else if (sInterval == _T("p4")) return _T("p4");
    else if (sInterval == _T("a4")) return _T("d4");
    else if (sInterval == _T("d5")) return _T("a5");
    else if (sInterval == _T("p5")) return _T("p5");
    else if (sInterval == _T("a5")) return _T("d5");
    else if (sInterval == _T("d6")) return _T("a3");
    else if (sInterval == _T("m6")) return _T("M3");
    else if (sInterval == _T("M6")) return _T("m3");
    else if (sInterval == _T("a6")) return _T("d3");
    else if (sInterval == _T("d7")) return _T("a2");
    else if (sInterval == _T("m7")) return _T("M2");
    else if (sInterval == _T("M7")) return _T("m2");
    else if (sInterval == _T("a7")) return _T("d2");

    wxASSERT(false);
    return sInterval; //error

}

wxString lmChordManager::AddIntervals(wxString sInterval1, wxString sInterval2)
{
    wxASSERT(sInterval1.Length() == 2 && sInterval2.Length() == 2);

    // get interval elements
    lmIntvBits tIntv1, tIntv2;
    if (IntervalNameToBits(sInterval1, &tIntv1))
        wxASSERT(false);
    if (IntervalNameToBits(sInterval2, &tIntv2))
        wxASSERT(false);
    
    //compute addition
    lmIntvBits tSum;
    tSum.nNum = tIntv1.nNum + tIntv2.nNum - 1;
    tSum.nSemitones = tIntv1.nSemitones + tIntv2.nSemitones;

    //rebuild interval name
    return IntervalBitsToName(tSum);
    
}

wxString lmChordManager::SubstractIntervals(wxString sInterval1, wxString sInterval2)
{
    wxASSERT(sInterval1.Length() == 2 && sInterval2.Length() == 2);

    //It is assumed that intv1 > int2

    // get interval elements
    lmIntvBits tIntv1, tIntv2;
    if (IntervalNameToBits(sInterval1, &tIntv1))
        wxASSERT(false);
    if (IntervalNameToBits(sInterval2, &tIntv2))
        wxASSERT(false);
    
    //compute difference
    lmIntvBits tDif;
    tDif.nNum = tIntv1.nNum - tIntv2.nNum + 1;
    tDif.nSemitones = tIntv1.nSemitones - tIntv2.nSemitones;

    //rebuild interval name
    return IntervalBitsToName(tDif);
    
}

wxString lmChordManager::IntervalBitsToName(lmIntvBits& tIntv)
{
    wxString sNormal = _T("dmMa");
    wxString sPerfect = _T("dpa");
    int nSemitones = tIntv.nSemitones % 12;
    int nNum = 1 + (tIntv.nNum - 1) % 7;
    wxString sResp;
    if (nNum == 1) 
        sResp = sPerfect.Mid(nSemitones + 1, 1);
    else if (nNum == 2) 
        sResp = sNormal.Mid(nSemitones, 1);
    else if (nNum == 3) 
        sResp = sNormal.Mid(nSemitones - 2, 1);
    else if (nNum == 4) 
        sResp = sPerfect.Mid(nSemitones - 4, 1);
    else if (nNum == 5) 
        sResp = sPerfect.Mid(nSemitones - 6, 1);
    else if (nNum == 6) 
        sResp = sNormal.Mid(nSemitones - 7, 1);
    else if (nNum == 7) 
        sResp = sNormal.Mid(nSemitones - 9, 1);
    else
        wxASSERT(false);    //impossible


    sResp += wxString::Format(_T("%d"), tIntv.nNum);
    return sResp;

}



/*
wxString lmChordManager::NoteBitsToName(lmNoteBits& tBits)
{
    wxString sResult;
    static wxString m_sSteps = _T("cdefgab");

    //compute accidentals
    if (tBits.nAccidentals == 1)
        sResult = _T("+");
    else if (tBits.nAccidentals == 2)
        sResult = _T("++");
    else if (tBits.nAccidentals == -1)
        sResult = _T("-");
    else if (tBits.nAccidentals == -2)
        sResult = _T("--");

    // compute step letter
    sResult += m_sSteps.Mid(tBits.nStep, 1);

    // compute octave
    sResult += wxString::Format(_T("%d"), tBits.nOctave);

    return sResult;

}
*/

wxString lmChordManager::GetPattern(int i)
{
    // Returns LDP pattern for note i (0 .. m_nNumNotes-1)
    wxASSERT( i < GetNumNotes());
    return NoteBitsToName(m_tBits[i], m_nKey);

}

wxString lmChordManager::GetNameFull()
{ 
    wxString sName = ChordTypeToName( m_nType );
    sName += ", ";
    if (m_nInversion == 0)
        sName += _("root position");
    else if (m_nInversion == 1)
        sName += _("1st inversion");
    else if (m_nInversion == 2)
        sName += _("2nd inversion");
    else if (m_nInversion == 3)
        sName += _("3rd inversion");
    else
        wxASSERT(false);    //impossible

    return sName;

}


#ifdef _DEBUG
void lmChordManager::UnitTests()
{
    int i, j;

    //NoteToBits and NoteBitsToName
    wxLogMessage(_T("[lmChordManager::UnitTests] Test of NoteToBits() method:"));
    wxString sNote[8] = { _T("c4"), _T("+a5"), _T("--b2"), _T("-a4"),
        _T("+e4"), _T("++f6"), _T("b1"), _T("xc4") };
    lmNoteBits tNote;
    for(i=0; i < 8; i++) {
        if (NoteToBits(sNote[i], &tNote)) 
            wxLogMessage(_T("Unexpected error in NoteToBits()"));
        else {
            wxLogMessage(_T("Note: '%s'. Bits: Step=%d, Octave=%d, Accidentals=%d, StepSemitones=%d --> '%s'"),
                sNote[i], tNote.nStep, tNote.nOctave, tNote.nAccidentals, tNote.nStepSemitones,
                NoteBitsToName(tNote, m_nKey) );
        }
    }

    //ComputeInterval(): interval computation
    wxString sIntv[8] = { _T("M3"), _T("m3"), _T("p8"), _T("p5"),
        _T("a5"), _T("d7"), _T("M6"), _T("M2") };
    for(i=0; i < 8; i++) {
        for (j=0; j < 8; j++) {
            wxString sNewNote = ComputeInterval(sNote[i], sIntv[j]);
            wxLogMessage(_T("Note='%s' + Intv='%s' --> '%s'"),
                         sNote[i], sIntv[j], sNewNote );
        }
    }

    //IntervalNameToBits and IntervalBitsToName
    wxLogMessage(_T("[lmChordManager::UnitTests] Test of IntervalNameToBits() method:"));
    lmIntvBits tIntv;
    for(i=0; i < 8; i++) {
        if (IntervalNameToBits(sIntv[i], &tIntv)) 
            wxLogMessage(_T("Unexpected error in IntervalNameToBits()"));
        else {
            wxLogMessage(_T("Intv: '%s'. Bits: num=%d, Semitones=%d --> '%s'"),
                sIntv[i], tIntv.nNum,tIntv.nSemitones,
                IntervalBitsToName(tIntv) );
        }
    }

    ////SubstractIntervals
    //wxLogMessage(_T("[lmChordManager::UnitTests] Test of SubstractIntervals() method:"));
    //wxString sIntv1[8] = { _T("p5"), _T("p5"), _T("M7"), _T("M6"), _T("m6"), _T("M7"), _T("M6"), _T("p4") };
    //wxString sIntv2[8] = { _T("M3"), _T("m3"), _T("p5"), _T("p5"), _T("a5"), _T("M3"), _T("m3"), _T("M2") };
    //for(i=0; i < 8; i++) {
    //    wxLogMessage(_T("Intv1='%s', intv2='%s' --> dif='%s'"),
    //        sIntv1[i], sIntv2[i], SubstractIntervals(sIntv1[i], sIntv2[i]) );
    //}

    //AddIntervals
    wxLogMessage(_T("[lmChordManager::UnitTests] Test of AddIntervals() method:"));
    wxString sIntv1[8] = { _T("p5"), _T("p5"), _T("M6"), _T("M3"), _T("M3"), _T("M6"), _T("d4"), _T("p8") };
    wxString sIntv2[8] = { _T("M3"), _T("m3"), _T("m2"), _T("m3"), _T("M3"), _T("M3"), _T("m7"), _T("p8") };
    for(i=0; i < 8; i++) {
        wxLogMessage(_T("Intv1='%s', intv2='%s' --> sum='%s'"),
            sIntv1[i], sIntv2[i], AddIntervals(sIntv1[i], sIntv2[i]) );
    }

}
#endif  // _DEBUG

//----------------------------------------------------------------------------------------
//global functions
//----------------------------------------------------------------------------------------

wxString ChordTypeToName(EChordType nType)
{
    wxASSERT(nType < ect_Max);

    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    if (!m_fStringsInitialized) {
        // Triads
        m_sChordName[ect_MajorTriad] = _("Major triad");
        m_sChordName[ect_MinorTriad] = _("Minor triad");
        m_sChordName[ect_AugTriad] = _("Augmented triad");
        m_sChordName[ect_DimTriad] = _("Diminished triad");
        m_sChordName[ect_Suspended_4th] = _("Suspended triad (4th)");
        m_sChordName[ect_Suspended_2nd] = _("Suspended triad (2nd)");

        // Seventh chords
        m_sChordName[ect_MajorSeventh] = _("Major 7th");
        m_sChordName[ect_DominantSeventh] = _("Dominant 7th");
        m_sChordName[ect_MinorSeventh] = _("Minor 7th");
        m_sChordName[ect_DimSeventh] = _("Diminished 7th");
        m_sChordName[ect_HalfDimSeventh] = _("Half diminished 7th");
        m_sChordName[ect_AugMajorSeventh] = _("Augmented major 7th");
        m_sChordName[ect_AugSeventh] = _("Augmented 7th");
        m_sChordName[ect_MinorMajorSeventh] = _("Minor major 7th");

        // Sixth chords
        m_sChordName[ect_MajorSixth] = _("Major 6th");
        m_sChordName[ect_MinorSixth] = _("Minor 6th");
        m_sChordName[ect_AugSixth] = _("Augmented 6th");

        m_fStringsInitialized = true;
    }
    
    return m_sChordName[nType];
    
}

int NumNotesInChord(EChordType nChordType)
{
    wxASSERT(nChordType < ect_Max);
    return tData[nChordType].nNumNotes;

}

EChordType ChordShortNameToType(wxString sName)
{
    // returns -1 if error
    //
    // m-minor, M-major, a-augmented, d-diminished, s-suspended
    // T-triad, dom-dominant, hd-half diminished
    //
    // triads: mT, MT, aT, dT, s4, s2
    // sevenths: m7, M7, a7, d7, mM7, aM7 dom7, hd7
    // sixths: m6, M6, a6

            // Triads
    if      (sName == _T("MT")) return ect_MajorTriad;
    else if (sName == _T("mT")) return ect_MinorTriad;
    else if (sName == _T("aT")) return ect_AugTriad;
    else if (sName == _T("dT")) return ect_DimTriad;
    else if (sName == _T("s4")) return ect_Suspended_4th;
    else if (sName == _T("s2")) return ect_Suspended_2nd;

        // Seventh chords
    else if (sName == _T("M7")) return ect_MajorSeventh;
    else if (sName == _T("dom7")) return ect_DominantSeventh;
    else if (sName == _T("m7")) return ect_MinorSeventh;
    else if (sName == _T("d7")) return ect_DimSeventh;
    else if (sName == _T("hd7")) return ect_HalfDimSeventh;
    else if (sName == _T("aM7")) return ect_AugMajorSeventh;
    else if (sName == _T("a7")) return ect_AugSeventh;
    else if (sName == _T("mM7")) return ect_MinorMajorSeventh;

        // Sixth chords
    else if (sName == _T("M6")) return ect_MajorSixth;
    else if (sName == _T("m6")) return ect_MinorSixth;
    else if (sName == _T("a6")) return ect_AugSixth;

    return (EChordType)-1;  //error

}