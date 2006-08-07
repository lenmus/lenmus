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

//access to error's logger
#include "../app/Logger.h"
extern lmLogger* g_pLogger;


typedef struct lmChordDataStruct {
    wxString sName;
    int nNumNotes;
    wxString sIntervals[3];
} lmChordData;

static wxString sChordName[16];
static fStringsInitialized = false;

//! @aware Array indexes are in correspondence with enum EChordType
// - intervals are from root note
//      number + type:   m=minor, M=major, p=perfect, a=augmented, d=diminished
static lmChordData tData[ect_Max] = { 
    { _T("MajorTriad"),         3, _T("3M"), _T("5p"), _T("") },        //MT
    { _T("MinorTriad"),         3, _T("3m"), _T("5p"), _T("") },        //mT
    { _T("AugTriad"),           3, _T("3M"), _T("5a"), _T("") },        //aT
    { _T("DimTriad"),           3, _T("3m"), _T("5d"), _T("") },        //dT
    { _T("Suspended_4th"),      3, _T("4p"), _T("5p"), _T("") },        //I,IV,V
    { _T("Suspended_2nd"),      3, _T("2M"), _T("5p"), _T("") },        //I,II,V
    { _T("MajorSeventh"),       4, _T("3M"), _T("5p"), _T("7M") },      //MT + M7
    { _T("DominantSeventh"),    4, _T("3M"), _T("5p"), _T("7m") },      //MT + m7
    { _T("MinorSeventh"),       4, _T("3m"), _T("5p"), _T("7m") },      //mT + m7
    { _T("DimSeventh"),         4, _T("3m"), _T("5d"), _T("7d") },      //dT + d7
    { _T("HalfDimSeventh"),     4, _T("3m"), _T("5d"), _T("7m") },      //dT + m7
    { _T("AugMajorSeventh"),    4, _T("3M"), _T("5a"), _T("7M") },      //aT + M7
    { _T("AugSeventh"),         4, _T("3M"), _T("5a"), _T("7m") },      //aT + m7
    { _T("MinorMajorSeventh"),  4, _T("3m"), _T("5p"), _T("7M") },      //mT + M7
    { _T("MajorSixth"),         4, _T("3M"), _T("5p"), _T("6M") },      //MT + M6
    { _T("MinorSixth"),         4, _T("3m"), _T("5p"), _T("6M") },      //mT + M6
    { _T("AugSixth"),           3, _T("3M"), _T("6a"), _T("") },        //M3 + a6
};


//-------------------------------------------------------------------------------------
// Implementation of lmChordManager class


lmChordManager::lmChordManager(wxString sRootNote, EChordType nChordType, EKeySignatures nKey)
{
    if (!fStringsInitialized) InitializeStrings();

    //save parameters
    m_nType = nChordType;

    if (NoteToBits(sRootNote, &m_tBits[0])) 
        wxASSERT(false);

    //get notes that form the interval
    int i;
    wxLogMessage(_T("[lmChordManager] Root note = %s, interval type=%s"),
                  NoteBitsToName(m_tBits[0]), GetName() );
    for (i=1; i < tData[m_nType].nNumNotes; i++) {
        ComputeInterval(&m_tBits[0], tData[m_nType].sIntervals[i-1], &m_tBits[i]);
        wxLogMessage(_T("[lmChordManager] Note %d = %s"), i, NoteBitsToName(m_tBits[i]) );
    }


/*
    //convert root note name to midi pitch
    lmPitch nPitch;
    EAccidentals nAccidentals;
    if (PitchNameToData(sRootNote, &nPitch, &nAccidentals)) 
        wxASSERT(false);
    lmConverter oConv;
    m_ntMidi[0] = oConv.PitchToMidiPitch(nPitch);
    switch (nAccidentals) {
        case eNoAccidentals:                        break;
        case eFlat:             m_ntMidi[0]--;      break;
        case eSharp:            m_ntMidi[0]++;      break;
        case eFlatFlat:         m_ntMidi[0]-=2;     break;
        case eSharpSharp:       m_ntMidi[0]+=2;     break; 
        case eDoubleSharp:      m_ntMidi[0]+=2;     break;
        default:
            ;
    }

    //generate midi pitch for chord notes
    for (i=1; i < tData[m_nType].nNumNotes; i++) {
        m_ntMidi[i] = GetNote(m_ntMidi[0], tData[m_nType].sIntervals[i]);
    }
*/

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

void lmChordManager::InitializeStrings()
{
    //language dependent strings. Can not be statically initiallized because
    //then they do not get translated
    sChordName[0] = _T("major");
    sChordName[1] = _("minor");
    sChordName[2] = _("augmented");
    sChordName[3] = _("diminished");
    sChordName[4] = _("major seventh");
    sChordName[5] = _("5th");
    sChordName[6] = _("6th");
    sChordName[7] = _("7th");
    sChordName[8] = _("octave");
    sChordName[9] = _("9th");
    sChordName[10] = _("10th");
    sChordName[11] = _("11th");
    sChordName[12] = _("12th");
    sChordName[13] = _("13th");
    sChordName[14] = _("14th");
    sChordName[15] = _("Two octaves");

    fStringsInitialized = true;
}

int lmChordManager::GetNote(int nMidiRoot, wxString sInterval)
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

    if (sInterval == _T("2m"))  return nMidiRoot + 1;
    if (sInterval == _T("2M"))  return nMidiRoot + 2;
    if (sInterval == _T("3m"))  return nMidiRoot + 3;
    if (sInterval == _T("3M"))  return nMidiRoot + 4;
    if (sInterval == _T("3a"))  return nMidiRoot + 5;
    if (sInterval == _T("4p"))  return nMidiRoot + 5;
    if (sInterval == _T("4a"))  return nMidiRoot + 6;
    if (sInterval == _T("5d"))  return nMidiRoot + 6;
    if (sInterval == _T("5p"))  return nMidiRoot + 7;
    if (sInterval == _T("5a"))  return nMidiRoot + 8;
    if (sInterval == _T("6m"))  return nMidiRoot + 8;
    if (sInterval == _T("6M"))  return nMidiRoot + 9;
    if (sInterval == _T("6a"))  return nMidiRoot + 10;
    if (sInterval == _T("7m"))  return nMidiRoot + 10;
    if (sInterval == _T("7M"))  return nMidiRoot + 11;
    if (sInterval == _T("8p"))  return nMidiRoot + 12;

    return 0;

}

wxString lmChordManager::ComputeInterval(wxString sRootNote, wxString sInterval)
{
    //Root note elements. i.e.: '+d4' -> (1, 4, 1)
    lmNoteBits tRoot;
    if (NoteToBits(sRootNote, &tRoot)) 
        wxASSERT(false);

    lmNoteBits tNew;
    ComputeInterval(&tRoot, sInterval, &tNew);

    return NoteBitsToName(tNew);

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
    pNewNote->nAccidentals = (pRoot->nStepSemitones + pRoot->nAccidentals + tIntval.nSemitones - 
                         pNewNote->nStepSemitones) % 12;

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
    if (sInterval.Length() != 2) return true;   //error

    // compute interval number
    wxString sNum = sInterval.Left(1);
    long nNum;
    if (!sNum.ToLong(&nNum)) return true;    //error
    pBits->nNum = (int)nNum;
    
    //compute semitones
    //  intval  semitones
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

    if      (sInterval == _T("2m")) pBits->nSemitones = 1;
    else if (sInterval == _T("2M")) pBits->nSemitones = 2;
    else if (sInterval == _T("2a")) pBits->nSemitones = 3;
    else if (sInterval == _T("3d")) pBits->nSemitones = 2;
    else if (sInterval == _T("3m")) pBits->nSemitones = 3;
    else if (sInterval == _T("3M")) pBits->nSemitones = 4;
    else if (sInterval == _T("3a")) pBits->nSemitones = 5;
    else if (sInterval == _T("4d")) pBits->nSemitones = 4;
    else if (sInterval == _T("4p")) pBits->nSemitones = 5;
    else if (sInterval == _T("4a")) pBits->nSemitones = 6;
    else if (sInterval == _T("5d")) pBits->nSemitones = 6;
    else if (sInterval == _T("5p")) pBits->nSemitones = 7;
    else if (sInterval == _T("5a")) pBits->nSemitones = 8;
    else if (sInterval == _T("6d")) pBits->nSemitones = 7;
    else if (sInterval == _T("6m")) pBits->nSemitones = 8;
    else if (sInterval == _T("6M")) pBits->nSemitones = 9;
    else if (sInterval == _T("6a")) pBits->nSemitones = 10;
    else if (sInterval == _T("7d")) pBits->nSemitones = 9;
    else if (sInterval == _T("7m")) pBits->nSemitones = 10;
    else if (sInterval == _T("7M")) pBits->nSemitones = 11;
    else if (sInterval == _T("7a")) pBits->nSemitones = 12;
    else if (sInterval == _T("8d")) pBits->nSemitones = 11;
    else if (sInterval == _T("8p")) pBits->nSemitones = 12;
    else return true; //error

    return false;

}

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

wxString lmChordManager::GetPattern(int i)
{
    // Returns LDP pattern for note i (0 .. m_nNumNotes-1)
    wxASSERT( i < GetNumNotes());
    return NoteBitsToName(m_tBits[i]);

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
                NoteBitsToName(tNote) );
        }
    }

    //ComputeInterval(): interval computation
    wxString sIntv[8] = { _T("3M"), _T("3m"), _T("8p"), _T("5p"),
        _T("5a"), _T("7d"), _T("6M"), _T("2M") };
    for(i=0; i < 8; i++) {
        for (j=0; j < 8; j++) {
            wxString sNewNote = ComputeInterval(sNote[i], sIntv[j]);
            wxLogMessage(_T("Note='%s' + Intv='%s' --> '%s'"),
                         sNote[i], sIntv[j], sNewNote );
        }
    }

}
#endif  // _DEBUG

//----------------------------------------------------------------------------------------
//global functions
//----------------------------------------------------------------------------------------

wxString ChordTypeToName(EChordType nType)
{
    wxASSERT(nType < ect_Max);

    static bool fNamesLoaded = false;
    static wxString sName[ect_Max];
    
    if (!fNamesLoaded) {
        // Triads
        sName[0] = _("Major triad");
        sName[1] = _("Minor triad");
        sName[2] = _("Augmented triad");
        sName[3] = _("Diminished triad");
        sName[4] = _("Suspended triad (4th)");
        sName[5] = _("Suspended triad (2nd)");

        // Seventh chords
        sName[6] = _("Major 7th");
        sName[7] = _("Dominant 7th");
        sName[8] = _("Minor 7th");
        sName[9] = _("Diminished 7th");
        sName[10] = _("Half diminished 7th");
        sName[11] = _("Augmented major 7th");
        sName[12] = _("Augmented 7th");
        sName[13] = _("Minor major 7th");

        // Sixth chords
        sName[14] = _("Major 6th");
        sName[15] = _("Minor 6th");
        sName[16] = _("Augmented 6th");

        fNamesLoaded = true;
    }
    
    return sName[nType];
    
}
