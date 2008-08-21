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

#ifndef __LM_CHORDMANAGER_H__        //to avoid nested includes
#define __LM_CHORDMANAGER_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ChordManager.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../score/Score.h"
#include "Interval.h"
#include "../exercises/ChordConstrains.h"
#include "Conversion.h"


//declare global functions defined in this module
extern wxString ChordTypeToName(EChordType nChordType);
extern int NumNotesInChord(EChordType nChordType);
extern EChordType ChordShortNameToType(wxString sName);


//a chord is a sequence of up 6 notes. Change this for more notes in chord
#define lmNOTES_IN_CHORD  6

class lmChordManager
{
public:
    //default constructor
    lmChordManager();
    //build a chord from root note and type
    lmChordManager(wxString sRootNote, EChordType nChordType, int nInversion = 0,
                   lmEKeySignatures nKey = earmDo);
    //destructor
    ~lmChordManager();

    //creation
    void Create(wxString sRootNote, EChordType nChordType, int nInversion,
                lmEKeySignatures nKey);
    void Create(wxString sRootNote, wxString sIntervals, lmEKeySignatures nKey);
    void Create(lmFPitch fpRootNote, int nNumNotes, lmFIntval nIntervals[], lmEKeySignatures nKey);

    EChordType GetChordType() { return m_nType; }
    wxString GetNameFull();
    wxString GetName() { return ChordTypeToName( m_nType ); }
    int GetNumNotes();
    lmMPitch GetMidiNote(int i);
    wxString GetPattern(int i);

    // access to note data
    int GetStep(int i) { return FPitch_Step(m_fpNote[i]); }
    int GetOctave(int i) { return FPitch_Octave(m_fpNote[i]); }
    int GetAccidentals(int i) { return FPitch_Accidentals(m_fpNote[i]); }

#ifdef __WXDEBUG__
    void UnitTests();
#endif

private:
    void DoCreateChord(lmFIntval nIntval[]);



//member variables

    EChordType      m_nType;
    lmEKeySignatures  m_nKey;
    int             m_nInversion;
    int             m_nNumNotes;                    //num notes in the chord
    lmFPitch        m_fpNote[lmNOTES_IN_CHORD];     //the chord notes

};

#endif  // __LM_CHORDMANAGER_H__

