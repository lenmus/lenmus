//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2018 LenMus project
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
//---------------------------------------------------------------------------------------

#ifndef __LENMUS_CHORD_H__        //to avoid nested includes
#define __LENMUS_CHORD_H__

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>

//lenmus
#include "lenmus_standard_header.h"

//lomse
#include <lomse_pitch.h>
#include <lomse_internal_model.h>
#include <lomse_im_note.h>
#include <lomse_interval.h>
using namespace lomse;


namespace lenmus
{

//forward declarations
class lmFiguredBass;
class ImoNote;

typedef int StepType;

//---------------------------------------------------------------------------------------
enum EChordType
{
    ect_undefined = -1,

    // Triads
    ect_MajorTriad = 0,     // 3M,5p perfect major
    ect_FirstTriad = ect_MajorTriad,
    ect_MinorTriad,         // 3m,5p perfect minor
    ect_AugTriad,           // 3M,5A 5th augmented
    ect_DimTriad,           // 3m,5d 5th diminished
    ect_Suspended_4th,      // p4,p5
    ect_Suspended_2nd,      // m4,p5
    ect_LastTriad = ect_Suspended_2nd,

    // Seventh chords
    ect_MajorSeventh,
    ect_FirstSeventh = ect_MajorSeventh,
    ect_DominantSeventh,
    ect_MinorSeventh,
    ect_DimSeventh,
    ect_HalfDimSeventh,
    ect_AugMajorSeventh,
    ect_AugSeventh,
    ect_MinorMajorSeventh,
    ect_LastSeventh = ect_MinorMajorSeventh,

    // Sixth chords
    ect_MajorSixth,
    ect_FirstSixth = ect_MajorSixth,
    ect_MinorSixth,
    ect_AugSixth,
    ect_LastSixth = ect_AugSixth,

    ect_MaxInExercises,    //<-- AWARE: Last value for exercises

    //Ninths
    ect_DominantNinth = ect_MaxInExercises,          //dominant-seventh + major ninth
    ect_FirstNinth = ect_DominantNinth,
    ect_MajorNinth,             //major-seventh + major ninth
    ect_MinorNinth,             //minor-seventh + major ninth
    ect_LastNinth = ect_MinorNinth,

    //11ths
    ect_Dominant_11th,          //dominantNinth + perfect 11th
    ect_First_11th = ect_Dominant_11th,
    ect_Major_11th,             //majorNinth + perfect 11th
    ect_Minor_11th,             //minorNinth + perfect 11th
    ect_Last_11th = ect_Minor_11th,

    //13ths
    ect_Dominant_13th,          //dominant_11th + major 13th
    ect_First_13th = ect_Dominant_13th,
    ect_Major_13th,             //major_11th + major 13th
    ect_Minor_13th,             //minor_11th + major 13th
    ect_Last_13th = ect_Minor_13th,

    //Other
    //ect_PowerChord,             //perfect fifth, (octave)
    //ect_FirstOther = ect_PowerChord,
    ect_TristanChord,           //augmented fourth, augmented sixth, augmented second
    //ect_LastOther = ect_TristanChord,

    //last element, to signal End Of Table
    ect_Max,
    ect_invalid,
};


//a chord is a sequence of up to 7 notes. Change this for more notes in chord
const int k_notes_in_chord = 7;
const int k_intervals_in_chord = k_notes_in_chord - 1;


//-------------------------------------------------------------------------------------------
// ChordIntervals: A generic chord (a list of intervals)
class ChordIntervals
{
protected:
    FIntval     m_nIntervals[k_intervals_in_chord];
    int         m_nNumIntv;

public:
    ChordIntervals(EChordType nChordType, int nInversion);
    ChordIntervals(int nNumIntv, FIntval* pFI);
    ChordIntervals(wxString sIntervals);
    ////TODO 5.0
    ChordIntervals(int numNotes, string notes[]);
    //ChordIntervals(int nNumNotes, FPitch fNotes[], int nUseless); // int nUseless: just to distinguish from  constructor (int nNumIntv, FIntval*)
    //ChordIntervals(int nNumNotes, ImoNote** pNotes);
    //ChordIntervals(int nStep, EKeySignature nKey, int nNumIntervals, int nInversion);

    ~ChordIntervals();

    //accessors
    inline int GetNumIntervals() { return m_nNumIntv; }
    inline FIntval GetInterval(int i) { return m_nIntervals[i]; }

    //operations
    void DoInversion();
    void SortIntervals();
    void Normalize();

    bool IsEqualTo(ChordIntervals* tOther);

   //debug
    wxString DumpIntervals();
    wxString ToString();
    string intervals_to_string();


};


// Chord: A real chord (root note + intervals)
//-------------------------------------------------------------------------------------------
// Aware: fundamental information in a chord is only:
//   BASS note CONTEXT-INDEPENDENT (key independent)
//      e.g.: FPitch OCTAVE-INDEPENDENT (0..39)
//       or   STEP + accidentals
//   INTERVALS
//   ELISIONS (if allowed)
// Directly derived from the above are:
//   TYPE
//   INVERSIONS
//   ROOT NOTE
//    (remember: root note == bass note only if no inversions)
class Chord : public ChordIntervals
{
private:
    //  TODO: simplify this class, moving calculated and accesory data to derived classes
    EChordType      m_nType;        // aware: do not use directly!. Always call get_chord_type()
    EKeySignature   m_nKey;         // TODO: not essential information; move it to a derived class
    int             m_nInversion;   //  aware: do not use directly!. Always call GetInversion()
    int             m_nElision;     // TODO: consider to make an enum in ChordConstrains...
//    bool                m_fRootIsDuplicated; // TODO: not essential information; move it to a derived class
    FPitch          m_fpRootNote;   // TODO: it should be called m_fpNormalizedBass. And make it % k_interval_p8

public:
    /** Creates a chord from its type, the root note, the desired inversion, and the
        key signature.
        Parameter 'nInversion' values: 0 (root position), 1 (1st inversion),
            2 (2nd inversion), and so on
    */
    Chord(FPitch fpRootNote, EChordType nChordType, int nInversion = 0,
          EKeySignature nKey = k_key_C);

    ///Build a chord from a list of intervals (as strings)
    Chord(FPitch fpRootNote, wxString sIntervals, EKeySignature nKey);

    ////TODO 5.0
    //    //build a chord from the root note and the figured bass
    //Chord(FPitch fpRootNote, lmFiguredBass* pFigBass, EKeySignature nKey = k_key_C);
    //// Todo: (Carlos april 2010) consider to replace lmFiguredBass by wxString in
    ////         the above constructor.
    ////        The reason: Chord is a low-level, context-independent, score-independent chord
    ////         lmFiguredBass is score-dependent. We could use the abstracted figured bass:
    ////            the figured bass STRING.
    ////        For the same reason, consider to remove the constructors based on ImoNote and FPitch below

        //build a chord from a list of notes in LDP source code
    Chord(int numNotes, string notes[], EKeySignature nKey = k_key_C);
    //    //build a chord from a list of score note pointers
    //Chord(int nNumNotes, ImoNote** pNotes, EKeySignature nKey = k_key_C);
    //Chord(int nNumNotes, FPitch fNotes[], EKeySignature nKey);
    //    // build a chord from "essential" information
    //Chord(int nDegree, EKeySignature nKey, int nNumIntervals, int nNumInversions, int octave);

    //destructor
    virtual ~Chord();

    //access to intervals
    inline int GetNumIntervals() { return m_nNumIntv; }
    FIntval GetInterval(int i);   //1..n

    //access to notes
    inline int get_num_notes() { return m_nNumIntv+1; }
    FPitch get_note(int i);        //0..n-1
    string GetPattern(int i);     //0..n-1
    MidiPitch get_midi_note(int i);    //0..n-1
    inline int get_step(int i) { return get_note(i).step(); }
    inline int get_octave(int i) { return get_note(i).octave(); }
    inline int get_num_accidentals(int i) { return get_note(i).num_accidentals(); }

    //chord info
    EChordType get_chord_type();
    wxString get_name_and_inversion();
    inline wxString get_name() { return Chord::type_to_name( get_chord_type() ); }
    int GetInversion();
    inline int GetElision() { return m_nElision; }
// todo: move this to a derived class with actual notes   inline int IsRootDuplicated() { return m_fRootIsDuplicated; }
    inline bool IsStandardChord() { return get_chord_type() != ect_invalid; };
    // A note is valid in a chord if it can be derived from de root note plus any of the intervals, i.e:
    //    note + any interval + N octaves
    int IsValidChordNote(FPitch fNote);

    FPitch GetNormalizedBass() { return m_fpRootNote % k_interval_p8;} // key-independent bass information
    FPitch GetNormalizedRoot(); // key independent root note, calculated from bass and inversions
    StepType GetChordDegree();

    bool IsEqualTo(Chord* tOther);

    //global functions
    static wxString type_to_name(EChordType nChordType);
    static int num_notes(EChordType nChordType);
    static EChordType short_name_to_type(wxString sName);

    ////TODO 5.0
    //// for debugging
    //wxString ToString();
    string note_steps_to_string();


private:
    void ComputeTypeAndInversion();

};


//--------------------------------------------------------------------------------------------
//ChordsDB: A singleton containing a data base with chords info

class ChordDBEntry;       //an entry of the DB table

class ChordsDB
{
public:
    ~ChordsDB();

    static ChordsDB* GetInstance();
    static void DeleteInstance();

    ChordDBEntry* Find(ChordIntervals* pChordIntv);
    void DumpChords();


protected:
    ChordsDB();
    void BuildDatabase();

    static ChordsDB*              m_pInstance;    //the only instance of this class
    std::vector<ChordDBEntry*>    m_ChordsDB;     //The Chords DB Table
};


////TODO 5.0
////-----------------------------------------------------------------------------------
//// global methods related to chords
////-----------------------------------------------------------------------------------
//
//#if (LENMUS_DEBUG_BUILD == 1)
//
//    //Unit tests
//    extern bool lmChordUnitTests();
//    extern bool lmChordFromFiguredBassUnitTest();
//#endif


}   //namespace lenmus

#endif  // __LENMUS_CHORD_H__

