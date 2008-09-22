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

#ifndef __LM_COMPOSERV6_H__        //to avoid nested includes
#define __LM_COMPOSERV6_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ComposerV6.cpp"
#endif

#include "vector"

#include "Composer.h"

// symbolic names for bool fUpStep
#define lmDOWN      false
#define lmUP        true

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
    wxString CreateLastMeasure(int nNumMeasure, lmETimeSignature nTimeSign, bool fOnlyQuarterNotes);

    // pitch related methods
    bool InstantiateNotes(lmScore* pScore, lmEKeySignatures nKey);
    void GetRandomHarmony(int nFunctions, std::vector<long>& aFunction);
    void FunctionToChordNotes(lmEKeySignatures nKey, long nFunction, lmAPitch aNotes[4]);

    void GenerateScale(lmEKeySignatures nKey, lmAPitch aNotes[7]);
    lmAPitch GenerateInChordList(lmEKeySignatures nKey, long nFunction,
                                    std::vector<lmAPitch>& aValidPitch);

    int GetRootStep(const lmEKeySignatures nKey) const;

        // contour
    void GenerateContour(int nNumPoints, std::vector<lmDPitch>& nContour);
    void ComputeTriangle(bool fUp, int iStart, int nPoints, lmDPitch nLowPitch,
                         lmDPitch nHighPitch, std::vector<lmDPitch>& aPoints);
    void ComputeRamp(int iStart, int nPoints, lmDPitch nStartPitch,
                     lmDPitch nEndPitch, std::vector<lmDPitch>& aPoints);
    void ComputeArch(bool fUp, int iStart, int nPoints, lmDPitch nLowPitch,
                     lmDPitch nHighPitch, std::vector<lmDPitch>& aPoints);

        // pitch
    lmAPitch NearestNoteOnChord(lmDPitch nPoint, lmNote* pNotePrev, lmNote* pNoteCur,
                                            std::vector<lmAPitch>& aOnChordPitch);
        // pitch for non-chord notes
    void AssignNonChordNotes(int nNumNotes, lmNote* pOnChord1, lmNote* pOnChord2,
                             lmNote* pNonChord[], lmAPitch aScale[7]);
    void NeightboringNotes(int nNumNotes, lmNote* pOnChord1, lmNote* pOnChord2,
                           lmNote* pNonChord[], lmAPitch aScale[7]);
    void PassingNotes(bool fUp, int nNumNotes, lmNote* pOnChord1, lmNote* pOnChord2,
                      lmNote* pNonChord[], lmAPitch aScale[7]);
    void ThirdFifthNotes(bool fUp, int nNumNotes, lmNote* pOnChord1, lmNote* pOnChord2,
                         lmNote* pNonChord[], lmAPitch aScale[7]);

    lmAPitch MoveByChromaticStep(bool fUpStep, lmAPitch nPitch);
    lmAPitch MoveByStep(bool fUpStep, lmAPitch nPitch, lmAPitch aScale[7]);

    //debug
    void InstantiateWithNote(lmScore* pScore, lmAPitch anPitch);




    lmETimeSignature      m_nTimeSign;
    lmEClefType           m_nClef;
    lmEKeySignatures      m_nKey;
    lmScoreConstrains*  m_pConstrains;

    //variables to control note pitch generation
    lmAPitch     m_nMinPitch, m_nMaxPitch;   // the valid range of notes to generate


};



#endif    // __LM_COMPOSERV6_H__



