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
static bool m_fStringsInitialized = false;

//! @aware Array indexes are in correspondence with enum EChordType
// - intervals are from root note
//      number + type:   m=minor, M=major, p=perfect, a=augmented, d=diminished
static lmChordData tData[ect_Max] = {
    { 3, { _T("M3"), _T("p5"), _T("") }},        //MT        - MajorTriad
    { 3, { _T("m3"), _T("p5"), _T("") }},        //mT        - MinorTriad
    { 3, { _T("M3"), _T("a5"), _T("") }},        //aT        - AugTriad
    { 3, { _T("m3"), _T("d5"), _T("") }},        //dT        - DimTriad
    { 3, { _T("p4"), _T("p5"), _T("") }},        //I,IV,V    - Suspended_4th
    { 3, { _T("M2"), _T("p5"), _T("") }},        //I,II,V    - Suspended_2nd
    { 4, { _T("M3"), _T("p5"), _T("M7") }},      //MT + M7   - MajorSeventh
    { 4, { _T("M3"), _T("p5"), _T("m7") }},      //MT + m7   - DominantSeventh
    { 4, { _T("m3"), _T("p5"), _T("m7") }},      //mT + m7   - MinorSeventh
    { 4, { _T("m3"), _T("d5"), _T("d7") }},      //dT + d7   - HalfDimSeventh
    { 4, { _T("m3"), _T("d5"), _T("m7") }},      //dT + m7   - HalfDimSeventh
    { 4, { _T("M3"), _T("a5"), _T("M7") }},      //aT + M7   - AugMajorSeventh
    { 4, { _T("M3"), _T("a5"), _T("m7") }},      //aT + m7   - AugSeventh
    { 4, { _T("m3"), _T("p5"), _T("M7") }},      //mT + M7   - MinorMajorSeventh
    { 4, { _T("M3"), _T("p5"), _T("M6") }},      //MT + M6   - MajorSixth
    { 4, { _T("m3"), _T("p5"), _T("M6") }},      //mT + M6   - MinorSixth
    { 3, { _T("M3"), _T("a4"), _T("a6") }},      //          - AugSixth
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

    if (lmConverter::NoteToBits(sRootNote, &m_tBits[0])) {
        wxLogMessage(_T("[lmChordManager::lmChordManager] Unexpected error in lmConverter::NoteToBits coversion. Note: '%s'"),
                sRootNote.c_str() );
        wxASSERT(false);
    }

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
    //wxLogMessage(_T("[lmChordManager] Root note = %s, interval type=%s, inversion=%d"),
    //              lmConverter::NoteBitsToName(m_tBits[0], m_nKey), GetName(), m_nInversion );
    for (i=1; i < tData[m_nType].nNumNotes; i++) {
        ComputeInterval(&m_tBits[0], sIntval[i-1], true, &m_tBits[i]);
        //wxLogMessage(_T("[lmChordManager] Note %d = %s, (Bits: Step=%d, Octave=%d, Accidentals=%d, StepSemitones=%d), key=%d"),
        //             i, lmConverter::NoteBitsToName(m_tBits[i],m_nKey),
        //             m_tBits[i].nStep, m_tBits[i].nOctave, m_tBits[i].nAccidentals, m_tBits[i].nStepSemitones,
        //             m_nKey );
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

wxString lmChordManager::GetPattern(int i)
{
    // Returns LDP pattern for note i (0 .. m_nNumNotes-1)
    wxASSERT( i < GetNumNotes());
    return lmConverter::NoteBitsToName(m_tBits[i], m_nKey);

}

wxString lmChordManager::GetNameFull()
{
    wxString sName = ChordTypeToName( m_nType );
    sName += _T(", ");
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


#ifdef __WXDEBUG__
void lmChordManager::UnitTests()
{
    int i, j;

    //lmConverter::NoteToBits and lmConverter::NoteBitsToName
    wxLogMessage(_T("[lmChordManager::UnitTests] Test of lmConverter::NoteToBits() method:"));
    wxString sNote[8] = { _T("a4"), _T("+a5"), _T("--b2"), _T("-a4"),
        _T("+e4"), _T("++f6"), _T("b1"), _T("xc4") };
    lmNoteBits tNote;
    for(i=0; i < 8; i++) {
        if (lmConverter::NoteToBits(sNote[i], &tNote))
            wxLogMessage(_T("Unexpected error in lmConverter::NoteToBits()"));
        else {
            wxLogMessage(_T("Note: '%s'. Bits: Step=%d, Octave=%d, Accidentals=%d, StepSemitones=%d --> '%s'"),
                sNote[i], tNote.nStep, tNote.nOctave, tNote.nAccidentals, tNote.nStepSemitones,
                lmConverter::NoteBitsToName(tNote, m_nKey) );
        }
    }

    //ComputeInterval(): interval computation
    wxString sIntv[8] = { _T("M3"), _T("m3"), _T("p8"), _T("p5"),
        _T("a5"), _T("d7"), _T("M6"), _T("M2") };
    for(i=0; i < 8; i++) {
        for (j=0; j < 8; j++) {
            wxString sNewNote = ComputeInterval(sNote[i], sIntv[j], true, m_nKey);
            wxLogMessage(_T("Note='%s' + Intv='%s' --> '%s'"),
                         sNote[i], sIntv[j], sNewNote );
            wxString sStartNote = ComputeInterval(sNewNote, sIntv[j], false, m_nKey);
            wxLogMessage(_T("Note='%s' - Intv='%s' --> '%s'"),
                         sNewNote, sIntv[j], sStartNote );
        }
    }

    //IntervalCodeToBits and IntervalBitsToCode
    wxLogMessage(_T("[lmChordManager::UnitTests] Test of IntervalCodeToBits() method:"));
    lmIntvBits tIntv;
    for(i=0; i < 8; i++) {
        if (IntervalCodeToBits(sIntv[i], &tIntv))
            wxLogMessage(_T("Unexpected error in IntervalCodeToBits()"));
        else {
            wxLogMessage(_T("Intv: '%s'. Bits: num=%d, Semitones=%d --> '%s'"),
                sIntv[i], tIntv.nNum,tIntv.nSemitones,
                IntervalBitsToCode(tIntv) );
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
#endif  // __WXDEBUG__

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
