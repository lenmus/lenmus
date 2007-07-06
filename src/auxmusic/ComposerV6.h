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

#ifndef __LM_COMPOSERV6_H__        //to avoid nested includes
#define __LM_COMPOSERV6_H__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "ComposerV6.cpp"
#endif

#include "vector"

#include "Composer.h"



class lmComposer6 : lmComposer
{
public:
    lmComposer6();
    ~lmComposer6();

    lmScore* GenerateScore(lmScoreConstrains* pConstrains);

private:
    void GetNotesRange();

    void AddSegment(wxString* pMeasure, lmSegmentEntry* pSegment, float rNoteTime);
    wxString CreateNoteRest(int nNoteRestDuration, bool fNote);
    wxString CreateNote(int nNoteDuration) { return CreateNoteRest(nNoteDuration, true); }
    wxString CreateRest(int nRestDuration) { return CreateNoteRest(nRestDuration, false); }
    wxString CreateLastMeasure(int nNumMeasure, ETimeSignature nTimeSign, bool fOnlyQuarterNotes);

    // pitch related methods
    bool InstantiateNotes(lmScore* pScore, EKeySignatures nKey);
    void GetRandomHarmony(int nFunctions, std::vector<long>& aFunction);
    void FunctionToChordNotes(EKeySignatures nKey, long nFunction, lmNotePitch aNotes[4]);
    lmNotePitch GenerateOnBeatNote(lmNote* pNotePrev, lmNote* pNoteCur,
                                   std::vector<lmNotePitch>& aOnChordPitch,
                                   lmNotePitch nRootPitch);

    void GenerateScale(EKeySignatures nKey, lmNotePitch aNotes[7]);
    lmNotePitch GenerateOffBeatNote(lmNote* pNotePrev, lmNotePitch aScale[7]);
    lmNotePitch GenerateInChordList(EKeySignatures nKey, long nFunction,
                                    std::vector<lmNotePitch>& aValidPitch);

    lmPitch RootNote(EKeySignatures nKey);




    ETimeSignature      m_nTimeSign;
    EClefType           m_nClef;
    EKeySignatures      m_nKey;
    lmScoreConstrains*  m_pConstrains;

    //variables to control note pitch generation
    lmNotePitch     m_nMinPitch, m_nMaxPitch;   // the valid range of notes to generate


};



#endif    // __LM_COMPOSERV6_H__



