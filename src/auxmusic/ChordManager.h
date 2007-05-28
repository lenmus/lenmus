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

#ifndef __CHORDMANAGER_H__        //to avoid nested includes
#define __CHORDMANAGER_H__

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

    EChordType GetChordType() { return m_nType; }
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



//member variables

    EChordType      m_nType;
    EKeySignatures  m_nKey;
    int             m_ntMidi[lmNOTES_IN_CHORD];
    lmNoteBits      m_tBits[lmNOTES_IN_CHORD];
    int             m_nInversion;

};

#endif  // __CHORDMANAGER_H__

