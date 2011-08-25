//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2011 LenMus project
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

//#ifndef __LENMUS_COMPOSER_H__        //to avoid nested includes
//#define __LENMUS_COMPOSER_H__
//
//#include "vector"
//
//#include "Composer.h"
//
//
//namespace lenmus
//{
//
//// symbolic names for bool fUpStep
//#define lmDOWN      false
//#define lmUP        true
//
//class Composer
//{
//public:
//    Composer();
//    ~Composer();
//
//    lmScore* GenerateScore(lmScoreConstrains* pConstrains);
//
//private:
//    void GetNotesRange();
//
//    void AddSegment(wxString* pMeasure, lmSegmentEntry* pSegment, float rNoteTime);
//    wxString CreateNoteRest(int nNoteRestDuration, bool fNote);
//    wxString CreateNote(int nNoteDuration) { return CreateNoteRest(nNoteDuration, true); }
//    wxString CreateRest(int nRestDuration) { return CreateNoteRest(nRestDuration, false); }
//    wxString CreateLastMeasure(int nNumMeasure, lmETimeSignature nTimeSign, bool fOnlyQuarterNotes);
//
//    // pitch related methods
//    bool InstantiateNotes(lmScore* pScore, EKeySignature nKey);
//    void InstantiateNotesRandom(lmScore* pScore);
//    void GetRandomHarmony(int nFunctions, std::vector<long>& aFunction);
//    void FunctionToChordNotes(EKeySignature nKey, long nFunction, lmAPitch aNotes[4]);
//
//    void GenerateScale(EKeySignature nKey, lmAPitch aNotes[7]);
//    lmAPitch GenerateInChordList(EKeySignature nKey, long nFunction,
//                                    std::vector<lmAPitch>& aValidPitch);
//
//    int GetRootStep(const EKeySignature nKey) const;
//
//        // contour
//    void GenerateContour(int nNumPoints, std::vector<DiatonicPitch>& nContour);
//    void ComputeTriangle(bool fUp, int iStart, int nPoints, DiatonicPitch nLowPitch,
//                         DiatonicPitch nHighPitch, std::vector<DiatonicPitch>& aPoints);
//    void ComputeRamp(int iStart, int nPoints, DiatonicPitch nStartPitch,
//                     DiatonicPitch nEndPitch, std::vector<DiatonicPitch>& aPoints);
//    void ComputeArch(bool fUp, int iStart, int nPoints, DiatonicPitch nLowPitch,
//                     DiatonicPitch nHighPitch, std::vector<DiatonicPitch>& aPoints);
//
//        // pitch
//    lmAPitch NearestNoteOnChord(DiatonicPitch nPoint, lmNote* pNotePrev, lmNote* pNoteCur,
//                                            std::vector<lmAPitch>& aOnChordPitch);
//    lmAPitch RandomPitch();
//
//        // pitch for non-chord notes
//    void AssignNonChordNotes(int nNumNotes, lmNote* pOnChord1, lmNote* pOnChord2,
//                             lmNote* pNonChord[], lmAPitch aScale[7]);
//    void NeightboringNotes(int nNumNotes, lmNote* pOnChord1, lmNote* pOnChord2,
//                           lmNote* pNonChord[], lmAPitch aScale[7]);
//    void PassingNotes(bool fUp, int nNumNotes, lmNote* pOnChord1, lmNote* pOnChord2,
//                      lmNote* pNonChord[], lmAPitch aScale[7]);
//    void ThirdFifthNotes(bool fUp, int nNumNotes, lmNote* pOnChord1, lmNote* pOnChord2,
//                         lmNote* pNonChord[], lmAPitch aScale[7]);
//
//    lmAPitch MoveByChromaticStep(bool fUpStep, lmAPitch nPitch);
//    lmAPitch MoveByStep(bool fUpStep, lmAPitch nPitch, lmAPitch aScale[7]);
//
//    //debug
//    void InstantiateWithNote(lmScore* pScore, lmAPitch anPitch);
//
//
//
//    lmScore*        m_pScore;        //the generated score
//    lmETimeSignature      m_nTimeSign;
//    lmEClefType           m_nClef;
//    EKeySignature      m_nKey;
//    lmScoreConstrains*  m_pConstrains;
//
//    //variables to control note pitch generation
//    lmAPitch     m_nMinPitch, m_nMaxPitch;   // the valid range of notes to generate
//
//
//};
//
//
//}   //namespace lenmus
//
//#endif    // __LENMUS_COMPOSER_H__
