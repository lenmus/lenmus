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
/*! @file ChordManager.h
    @brief Header file for class lmChordManager
    @ingroup auxmusic
*/

#ifndef __CHORDMANAGER_H__        //to avoid nested includes
#define __CHORDMANAGER_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../score/score.h"
#include "Interval.h"
#include "../exercises/Constrains.h"

typedef struct lmNoteBitsStruct {
    int nStep;              // 'c'=0, 'd'=1, 'e'=2, 'f'=3, 'g'=4, 'a'=5, 'b'=6
    int nOctave;            // 0..9
    int nAccidentals;       // '--'=-1, '-'=-1, ''=0, '+'=+1, '++'=+2
    int nStepSemitones;     // 'c'=0, 'd'=2, 'e'=4, 'f'=5, 'g'=7, 'a'=9, 'b'=11
} lmNoteBits;

typedef struct lmIntvBitsStruct {
    int nNum;              
    int nSemitones;
} lmIntvBits;




//declare global functions defined in this module
extern wxString ChordTypeToName(EChordType nChordType);
extern int NumNotesInChord(EChordType nChordType);


//a chord is a sequence of up 4 notes. Change this for more notes in chord
#define lmNOTES_IN_CHORD  4

class lmChordManager
{
public:
    //build a chord from root note and type
    lmChordManager(wxString sRootNote, EChordType nChordType, int nInversion = 0,
                   EKeySignatures nKey = earmDo);
    //destructor
    ~lmChordManager();

    EChordType GetType() { return m_nType; }
    wxString GetNameFull();
    wxString GetName() { return ChordTypeToName( m_nType ); }
    int GetNumNotes();
    int GetMidiNote(int i);
    wxString GetPattern(int i);

#ifdef _DEBUG
    void UnitTests();
#endif

private:
    int GetMidiNote(int nMidiRoot, wxString sInterval);
    wxString ComputeInterval(wxString sRootNote, wxString sInterval);
    void ComputeInterval(lmNoteBits* pRoot, wxString sInterval, lmNoteBits* pNewNote);

    bool NoteToBits(wxString sNote, lmNoteBits* pBits);
    int StepToSemitones(int nStep);
    int AccidentalsToInt(wxString sAccidentals);
    int StepToInt(wxString sStep);
    wxString NoteBitsToName(lmNoteBits& tBits, EKeySignatures nKey);
    wxString IntervalBitsToName(lmIntvBits& tIntv);
    bool IntervalNameToBits(wxString sInterval, lmIntvBits* pBits);
    wxString InvertInterval(wxString sInterval);
    wxString AddIntervals(wxString sInterval1, wxString sInterval2);
    wxString SubstractIntervals(wxString sInterval1, wxString sInterval2);


//member variables

    EChordType      m_nType;
    EKeySignatures  m_nKey;
    int             m_ntMidi[lmNOTES_IN_CHORD];
    lmNoteBits      m_tBits[lmNOTES_IN_CHORD];
    int             m_nInversion;

};

#endif  // __CHORDMANAGER_H__

