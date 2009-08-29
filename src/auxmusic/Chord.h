//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
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

#define lmINVALID_CHORD_TYPE ect_Max

typedef struct lmChordInfoStruct {
    int nNumNotes;
    int nNumIntervals;
    int nNumInversions;
    // TODO: consider to improve the ellision with a generic mechanism
    //    bool fAllowedElidedIntervals[lmINTERVALS_IN_CHORD];
    //    bool fElidedIntervals[lmINTERVALS_IN_CHORD];
    //    now (jun-2009) only 5th ellided is allowed; then, those arrays are not necessary
    // TODO: confirm music theory: ellided INTERVALS instead of just NOTES
    int nFifthElided;
    bool fRootIsDuplicated;
    lmFIntval nIntervals[lmINTERVALS_IN_CHORD];
    void Initalize() //TODO: consider possible inmprovement: a constructor?
    {
        nNumIntervals = 0;
        nNumNotes = 0;
        nNumInversions = 0;
        fRootIsDuplicated = false;
        for (int i=0; i<lmINTERVALS_IN_CHORD; i++)
        {
          nIntervals[i] = lmNULL_FIntval;
        }
        nFifthElided = 0;  // Elision allowed only in second interval
    }
} lmChordInfo;

//TODO: This method was used mainly for debug. Consider to put it inside lmNote
extern wxString NoteId(lmNote &tNote);

// TODO: global methods. They could probably be placed inside a class...
extern  void CreateChordInfo(int numNotes, lmNote** inpChordNotes, lmChordInfo* outChordInfo);
extern void SortChordNotes( int numNotes, lmNote** inpChordNotes);
extern void GetIntervalsFromNotes(int numNotes, lmNote** inpChordNotes, lmChordInfo* outChordInfo);
extern lmEChordType GetChordTypeFromIntervals(lmChordInfo& chordInfo, bool fAllowFifthElided=false );
extern bool TryChordCreation(int numNotes, lmNote** inpChordNotes, lmChordInfo* outChordInfo, wxString &outStatusStr);
extern int DoInversionsToChord( lmChordInfo* pInOutChordInfo, int nNumTotalInv);



class lmChord
{
public:
    //default constructor
    lmChord();
    //build a chord from root note and type
    lmChord(wxString sRootNote, lmEChordType nChordType, int nInversion = 0,
                   lmEKeySignatures nKey = earmDo);
    //build a chord from the root note and the figured bass
    lmChord(wxString sRootNote, lmFiguredBass* pFigBass, lmEKeySignatures nKey = earmDo);
    //??
    lmChord(lmNote* pRootNote, lmChordInfo &chordInfo);
    //destructor
    ~lmChord();

    // creation
    void Create(wxString sRootNote, lmEChordType nChordType, int nInversion,
                lmEKeySignatures nKey);
    void Create(wxString sRootNote, wxString sIntervals, lmEKeySignatures nKey);
    void Create(lmNote* pRootNote, lmChordInfo* chordInfo);
    void Initialize();

    //access to intervals
    lmFIntval GetInterval(int i);

    // for debugging
    wxString ToString();

    lmEChordType GetChordType() { return m_nType; }
    wxString GetNameFull();
    wxString GetName() { return lmChordTypeToName( m_nType ); }
    int GetNumNotes();
    lmMPitch GetMidiNote(int i);
    wxString GetPattern(int i);
    int GetInversion() { return m_nInversion; }
    int GetElision() { return m_nElision; }
    int IsRootDuplicated() { return m_fRootIsDuplicated; }

    // access to note data
    int GetStep(int i) { return FPitch_Step(m_fpNote[i]); }
    int GetOctave(int i) { return FPitch_Octave(m_fpNote[i]); }
    int GetAccidentals(int i) { return FPitch_Accidentals(m_fpNote[i]); }

    bool IsCreated(){ return m_nType != lmINVALID_CHORD_TYPE; };

#ifdef __WXDEBUG__
    //debug methods
    void UnitTests();
    void DumpIntervals(wxString& sMsg, int nNumInvt, lmFIntval* pFI);
    void DumpIntervals(wxString& sMsg);
#else
    void DumpIntervals(wxString& sMsg, int nNumInvt, lmFIntval* pFI) {}
#endif

private:
    void DoCreateChord(lmFIntval nIntval[]);
    lmEChordType ComputeChordType(int nInversion);
    void GetChordIntervals(lmEChordType nType, int nInversion, lmFIntval* pFI);



//member variables

    lmEChordType        m_nType;
    lmEKeySignatures    m_nKey;
    int                 m_nInversion;
    int                 m_nNumNotes;                    //num notes in the chord
    lmFPitch            m_fpNote[lmNOTES_IN_CHORD];     //the chord notes
    int                 m_nElision; // TODO: consider to make an enum in ChordConstrains...
    bool                m_fRootIsDuplicated;

};


//-----------------------------------------------------------------------------------
// global methods related to chords
//-----------------------------------------------------------------------------------

#ifdef __WXDEBUG__

    //Unit tests
    extern bool lmChordUnitTests();
    extern bool lmChordFromFiguredBassUnitTest(wxString sRootNote, 
                                               lmEKeySignatures nKey);
#endif


#endif  // __LM_CHORD_H__

