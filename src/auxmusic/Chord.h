//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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

#ifndef __LM_CHORD_H__        //to avoid nested includes
#define __LM_CHORD_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Chord.cpp"
#endif

// For compilers that support precompilation, includes <wx/wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "../score/Score.h"
#include "../score/FiguredBass.h"
#include "Interval.h"
#include "../exercises/ChordConstrains.h"
#include "Conversion.h"

class lmFiguredBass;



//declare global functions defined in this module
extern wxString lmChordTypeToName(lmEChordType nChordType);
extern int lmNumNotesInChord(lmEChordType nChordType);
extern lmEChordType lmChordShortNameToType(wxString sName);

//todo: improve: constant?  better place?
#define lmINVALID_CHORD_TYPE ect_Max



// lmChordIntervals: A generic chord (a list of intervals)
//-------------------------------------------------------------------------------------------
class lmChordIntervals
{
public:
    lmChordIntervals(int nNumIntv, lmFIntval* pFI);
    lmChordIntervals(lmEChordType nChordType, int nInversion);
    lmChordIntervals(wxString sIntervals);
    lmChordIntervals(int nNumNotes, wxString* pNotes);
    lmChordIntervals(int nNumNotes, lmFPitch fNotes[], int nUseless); // int nUseless: just to distinguish from  constructor (int nNumIntv, lmFIntval*)
    lmChordIntervals(int nNumNotes, lmNote** pNotes);
    lmChordIntervals(int nStep, lmEKeySignatures nKey, int nNumIntervals, int nInversion);
    
    ~lmChordIntervals();

    //accessors
    inline int GetNumIntervals() { return m_nNumIntv; }
    inline lmFIntval GetInterval(int i) { return m_nIntervals[i]; }

    //operations
    void DoInversion();
    void SortIntervals();
    void Normalize();

    bool IsEqualTo(lmChordIntervals* tOther);

   //debug
    wxString DumpIntervals();
    wxString ToString();


protected:
    lmFIntval       m_nIntervals[lmINTERVALS_IN_CHORD];
    int             m_nNumIntv;
};


// lmChord: A real chord (root note + intervals)
//-------------------------------------------------------------------------------------------
// Aware: fundamental information in a chord is only:
//   BASS note CONTEXT-INDEPENDENT (key independent)
//      e.g.: lmFPitch OCTAVE-INDEPENDENT (0..39)
//       or   STEP + accidentals
//   INTERVALS
//   ELISIONS (if allowed)
// Directly derived from the above are:
//   TYPE
//   INVERSIONS
//   ROOT NOTE
//    (remember: root note == bass note only if no inversions)
class lmChord : public lmChordIntervals
{
public:
        //build a chord from root note and type
    lmChord(wxString sRootNote, lmEChordType nChordType, int nInversion = 0,
            lmEKeySignatures nKey = earmDo);
        //build a chord from the root note and the figured bass
    lmChord(wxString sRootNote, lmFiguredBass* pFigBass, lmEKeySignatures nKey = earmDo);
    // Todo: (Carlos april 2010) consider to replace lmFiguredBass by wxString in
    //         the above constructor.
    //        The reason: lmChord is a low-level, context-independent, score-independent chord
    //         lmFiguredBass is score-dependent. We could use the abstracted figured bass:
    //            the figured bass STRING.
    //        For the same reason, consider to remove the constructors based on lmNote and lmFPitch below
        //build a chord from a list of notes in LDP source code
    lmChord(int nNumNotes, wxString* pNotes, lmEKeySignatures nKey = earmDo);
        //build a chord from a list of score note pointers
    lmChord(int nNumNotes, lmNote** pNotes, lmEKeySignatures nKey = earmDo);  
    lmChord(int nNumNotes, lmFPitch fNotes[], lmEKeySignatures nKey);
        //build a chord from a list of intervals (as strings)
    lmChord(wxString sRootNote, wxString sIntervals, lmEKeySignatures nKey);
        // build a chord from "essential" information
    lmChord(int nDegree, lmEKeySignatures nKey, int nNumIntervals, int nNumInversions, int octave);

    //destructor
    virtual ~lmChord();

    //access to intervals
    inline int GetNumIntervals() { return m_nNumIntv; }
    lmFIntval GetInterval(int i);   //1..n

    //access to notes
    inline int GetNumNotes() { return m_nNumIntv+1; }
    lmFPitch GetNote(int i);        //0..n-1
    wxString GetPattern(int i);     //0..n-1
    lmMPitch GetMidiNote(int i);    //0..n-1
    inline int GetStep(int i) { return FPitch_Step(GetNote(i)); }
    inline int GetOctave(int i) { return FPitch_Octave(GetNote(i)); }
    inline int GetAccidentals(int i) { return FPitch_Accidentals(GetNote(i)); }

    //chord info
    lmEChordType GetChordType();
    wxString GetNameFull();
    inline wxString GetName() { return lmChordTypeToName( GetChordType() ); }
    int GetInversion();
    inline int GetElision() { return m_nElision; }
// todo: move this to a derived class with actual notes   inline int IsRootDuplicated() { return m_fRootIsDuplicated; }
    inline bool IsStandardChord() { return GetChordType() != lmINVALID_CHORD_TYPE; };
    // A note is valid in a chord if it can be derived from de root note plus any of the intervals, i.e:
    //    note + any interval + N octaves
    int IsValidChordNote(lmFPitch fNote);

    lmFPitch GetNormalizedBass() { return m_fpRootNote % lm_p8;} // key-independent bass information
    lmFPitch GetNormalizedRoot(); // key independent root note, calculated from bass and inversions
    lmStepType GetChordDegree();

    bool IsEqualTo(lmChord* tOther);

    // for debugging
    wxString ToString();


private:
    void ComputeTypeAndInversion();

        //member variables

    //  TODO: simplify this class, moving calculated and accesory data to derived classes
    lmEChordType        m_nType;        // aware: do not use directly!. Always call GetChordType()
    lmEKeySignatures    m_nKey;         // TODO: not essential information; move it to a derived class
    int                 m_nInversion;   //  aware: do not use directly!. Always call GetInversion()   
    int                 m_nElision;     // TODO: consider to make an enum in ChordConstrains...
//    bool                m_fRootIsDuplicated; // TODO: not essential information; move it to a derived class
    lmFPitch            m_fpRootNote;   // TODO: it should be called m_fpNormalizedBass. And make it % lm_p8

};



//lmChordDB: A singleton containing a data base with chords info
//--------------------------------------------------------------------------------------------

class lmChordDBEntry;       //an entry of the DB table

class lmChordsDB
{
public:
    ~lmChordsDB();
    
    static lmChordsDB* GetInstance();
    static void DeleteInstance();

    lmChordDBEntry* Find(lmChordIntervals* pChordIntv);
    void DumpChords();


protected:
    lmChordsDB();
    void BuildDatabase();

    static lmChordsDB*              m_pInstance;    //the only instance of this class
    std::vector<lmChordDBEntry*>    m_ChordsDB;     //The Chords DB Table
};


//-----------------------------------------------------------------------------------
// global methods related to chords
//-----------------------------------------------------------------------------------

#ifdef _LM_DEBUG_

    //Unit tests
    extern bool lmChordUnitTests();
    extern bool lmChordFromFiguredBassUnitTest();
#endif


#endif  // __LM_CHORD_H__

