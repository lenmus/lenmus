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

/*
EIntervalType
    eti_Diminished = 0,     d
    eti_Minor,              m
    eti_Major,              M
    eti_Augmented,          a
    eti_Perfect,            p
    eti_DoubleAugmented,    da -
    eti_DoubleDiminished    dd +

An interval is characterized by its number and its type

intval  semitones
2m      1
2M      2
3m      3
3M      4
3a/4p   5
4a/5d   6 
5p      7
5a/6m   8      
6M      9
6a/7m   10
7M      11
8p      12

*/

//-------------------------------------------------------------------------------------
// Implementation of lmChordManager class


lmChordManager::lmChordManager(wxString sRootNote, EChordType nChordType, EKeySignatures nKey)
{
    if (!fStringsInitialized) InitializeStrings();

    //save parameters
    m_nType = nChordType;

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
    int i;
    for (i=1; i < tData[m_nType].nNumNotes; i++) {
        m_ntMidi[i] = GetNote(m_ntMidi[0], tData[m_nType].sIntervals[i]);
    }

/*
    //create the score with the chord
    lmNote* pNote[lmNOTES_IN_CHORD];
    lmLDPParser parserLDP;
    lmLDPNode* pNode;
    lmVStaff* pVStaff;

    m_pChordScore = new lmScore();
    m_pChordScore->AddInstrument(1,0,0,_T(""));         //one vstaff, MIDI channel 0, MIDI instr 0
    pVStaff = m_pChordScore->GetVStaff(1, 1);           //get first vstaff of instr.1
    pVStaff->AddClef( eclvSol );
    pVStaff->AddKeySignature(nKey);
    pVStaff->AddTimeSignature(4 ,4, sbNO_VISIBLE );
    //pVStaff->AddEspacio 24
    for (i=1; i < tData[m_nType].nNumNotes; i++) {
        m_ntMidi[i] = GetNote(m_ntMidi[0], tData[m_nType].sIntervals[i]);
        pNode = parserLDP.ParseText( sPattern[i] );
        pNote[i] = parserLDP.AnalyzeNote(pNode, pVStaff);
    }
    pVStaff->AddBarline(etb_EndBarline);
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

wxString lmChordManager::GetName(wxString sRootNote, wxString sInterval)
{
    //Root note elements. i.e.: 'd4+' -> (d, 4, 1)
    wxString sStep;
    int nOctave;
    int nAccidentals;  // '--'=-1, '-'=-1, ''=0, '+'=+1, '++'=+2

    //interval elements. i.e.: '5a' -> (5, 8)
    int nInterval;
    int nSemitones;

    wxString sNewStep = (sStep + nInterval - 1) mod 8
    nNewOctave = (nNewStep > 8 ? ...
    int nStepIndex =        //c=0, d=2, e=4, f=5, g=7, a=9, b=11
    int nNewStepIndex =     //idem
    nNewAcc = (nStepIndex + nAccidentals + nSemitones - nNewStepIndex) mod 12;
}
