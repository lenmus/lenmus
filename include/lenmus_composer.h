//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2012 LenMus project
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

#ifndef __LENMUS_COMPOSER_H__        //to avoid nested includes
#define __LENMUS_COMPOSER_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_scores_constrains.h"

//lomse
#include <lomse_internal_model.h>
#include <lomse_document.h>
#include <lomse_pitch.h>
using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>

//other
#include "vector"


namespace lenmus
{

//forward declarations
class ScoreConstrains;

// symbolic names for bool fUpStep
#define k_down      false
#define k_up        true

//---------------------------------------------------------------------------------------
class Composer
{
public:
    Composer();
    ~Composer();

    ImoScore* GenerateScore(ScoreConstrains* pConstrains, Document* pDoc);



private:

    //Some helper static methods to deal with enum ETimeSignature
    static int get_metronome_pulses_for(ETimeSignature nTimeSign);
    static int get_beats_for(ETimeSignature nTimeSign);
    static int get_beat_type_for(ETimeSignature nTimeSign);
    static float get_beat_duration_for(ETimeSignature nTimeSign);
    static float get_measure_duration_for(ETimeSignature nTimeSign);

    void GetNotesRange();

    string CreateNoteRest(int nNoteRestDuration, bool fNote);
    string CreateNote(int nNoteDuration) { return CreateNoteRest(nNoteDuration, true); }
    string CreateRest(int nRestDuration) { return CreateNoteRest(nRestDuration, false); }
    string CreateLastMeasure(int nNumMeasure, ETimeSignature nTimeSign,
                             bool fOnlyQuarterNotes);

    // pitch related methods
    bool InstantiateNotes(ImoScore* pScore, EKeySignature nKey);
    void InstantiateNotesRandom(ImoScore* pScore);
    void GetRandomHarmony(int nFunctions, std::vector<long>& aFunction);
    void FunctionToChordNotes(EKeySignature nKey, long nFunction, FPitch aNotes[4]);
    FPitch NearestNoteOnChord(DiatonicPitch nPoint, ImoNote* pNotePrev, ImoNote* pNoteCur,
                              std::vector<FPitch>& aOnChordPitch);
    FPitch RandomPitch();
    void set_pitch(ImoNote* pNote, FPitch fp);

    void GenerateScale(EKeySignature nKey, FPitch notes[7]);
    FPitch GenerateInChordList(EKeySignature nKey, long nFunction,
                               std::vector<FPitch>& aValidPitch);

        // contour
    void GenerateContour(int nNumPoints, std::vector<DiatonicPitch>& nContour);
    void ComputeTriangle(bool fUp, int iStart, int nPoints, DiatonicPitch nLowPitch,
                         DiatonicPitch nHighPitch, std::vector<DiatonicPitch>& aPoints);
    void ComputeRamp(int iStart, int nPoints, DiatonicPitch nStartPitch,
                     DiatonicPitch nEndPitch, std::vector<DiatonicPitch>& aPoints);
    void ComputeArch(bool fUp, int iStart, int nPoints, DiatonicPitch nLowPitch,
                     DiatonicPitch nHighPitch, std::vector<DiatonicPitch>& aPoints);


        // pitch for non-chord notes
    void AssignNonChordNotes(int nNumNotes, ImoNote* pOnChord1, ImoNote* pOnChord2,
                             ImoNote* pNonChord[], FPitch aScale[7]);
    void NeightboringNotes(int nNumNotes, ImoNote* pOnChord1, ImoNote* pOnChord2,
                           ImoNote* pNonChord[], FPitch aScale[7]);
    void PassingNotes(bool fUp, int nNumNotes, ImoNote* pOnChord1, ImoNote* pOnChord2,
                      ImoNote* pNonChord[], FPitch aScale[7]);
    void ThirdFifthNotes(bool fUp, int nNumNotes, ImoNote* pOnChord1, ImoNote* pOnChord2,
                         ImoNote* pNonChord[], FPitch aScale[7]);

    FPitch MoveByChromaticStep(bool fUpStep, FPitch nPitch);
    FPitch MoveByStep(bool fUpStep, FPitch nPitch, FPitch aScale[7]);

    //debug
    void InstantiateWithNote(ImoScore* pScore, FPitch fp);


    ETimeSignature      m_nTimeSign;
    EClefExercise       m_nClef;
    EKeySignature       m_nKey;
    ScoreConstrains*    m_pConstrains;

    //variables to control note pitch generation
    FPitch      m_fpMinPitch, m_fpMaxPitch;   // the valid range of notes to generate

};


}   //namespace lenmus

#endif    // __LENMUS_COMPOSER_H__
