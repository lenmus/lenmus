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

#ifndef __COMPOSERV5_H__        //to avoid nested includes
#define __COMPOSERV5_H__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "ComposerV5.cpp"
#endif

#include "Composer.h"

// For performance, instead of encoding notes in source LDP
// (i.e. "+c4") they will be encoded in longs:
// 'c'=0x0100 = 64, '+'= 0x00010000 = 256, '4' = 0x04 = 4 -->
//  '+c4' = 64+256+4 = 324 (0x00010104)
enum lmENote
{
    //octave: 8 bits (0x000000ff)
    lmOCTAVE_MASK        = 0x00000ff,   // to extract octave

    // diatonic note: 8 bits (0x0000ff00)
    lmC_NOTE            = 0x00000100,
    lmD_NOTE            = 0x00000200,
    lmE_NOTE            = 0x00000400,
    lmF_NOTE            = 0x00000800,
    lmG_NOTE            = 0x00001000,
    lmA_NOTE            = 0x00002000,
    lmB_NOTE            = 0x00004000,

    lmNOTE_MASK         = 0x0000ff00,   // to extract note

    //accidentals: 8 bits
    lmSHARP             = 0x00010000,   // +
    lmFLAT              = 0x00020000,   // -
    lmSHARP_SHARP       = 0x00040000,   // ++
    lmFLAT_FLAT         = 0x00080000,   // --
    lmDOUBLE_SHARP      = 0x00100000,   // x
    lmNATURAL           = 0x00200000,   // =

    lmACCIDENTAL_MASK   = 0x00ff0000,   // to extract accidentals

};

#define lmNoteData  long    //Type used for portability and legibility


class lmComposer5 : lmComposer
{
public:
    lmComposer5();
    ~lmComposer5();

    lmScore* GenerateScore(lmScoreConstrains* pConstrains);

private:
    void            ChooseRangeOfNotes();
    wxString        GenerateNewNote(bool fRepeat, bool fRootPitch);

    void AddSegment(wxString* pMeasure, lmSegmentEntry* pSegment, float rNoteTime);
    wxString CreateNoteRest(int nNoteRestDuration, bool fNote);
    wxString CreateNote(int nNoteDuration) { return CreateNoteRest(nNoteDuration, true); }
    wxString CreateRest(int nRestDuration) { return CreateNoteRest(nRestDuration, false); }
    wxString CreateLastMeasure(int nNumMeasure, ETimeSignature nTimeSign, bool fOnlyQuarterNotes);

    // pitch related methods
    wxString    InstantiateNotes(wxString sBeats, bool fRootPtich=false);
    lmPitch     RootNote(EKeySignatures nKey);
    void        GetRandomHarmony(int nFunctions, long* pFunction[]);
    void        FunctionToChordNotes(EKeySignatures nKey, long nFunction, lmNoteData* pNote[4]);




    ETimeSignature      m_nTimeSign;
    EClefType           m_nClef;
    EKeySignatures      m_nKey;
    lmScoreConstrains*  m_pConstrains;

    //variables to control note pitch generation
    int         m_nNumNotes;                // num of notes generated
    lmPitch     m_minPitch, m_maxPitch;     // range of notes to generate
    lmPitch     m_lastPitch;                // last note pitch
    bool        m_fTied;                    // next note tied to m_lastPitch


};



#endif    // __COMPOSERV5_H__



