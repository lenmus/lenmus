//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2014 LenMus project
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
const bool k_down = false;
const bool k_up = true;

//---------------------------------------------------------------------------------------
class Composer
{
protected:
    Document*           m_pDoc;
    ADocument           m_doc;
    int                 m_midiVoice;        //midiVoice: 0..255
    ETimeSignature      m_nTimeSign;
    EClef               m_nClef;
    EKeySignature       m_nKey;
    ScoreConstrains*    m_pConstrains;

    //variables to control note pitch generation
    FPitch      m_fpMinPitch, m_fpMaxPitch;   // the valid range of notes to generate

public:
    Composer(ADocument doc);
    ~Composer();

    //settings
    inline void midi_instrument(int midiVoice) { m_midiVoice = midiVoice; }

    //score generation
    ImoScore* generate_score(ScoreConstrains* pConstrains);

    //access to details about composed score
    inline EClef get_score_clef() { return m_nClef; }
    inline EKeySignature get_score_key_signature() { return m_nKey; }
    inline ETimeSignature get_score_time_signature() { return m_nTimeSign; }


private:
    void GetNotesRange();

    wxString CreateNoteRest(int nNoteRestDuration, bool fNote, bool fCompound, bool fFinal);
    wxString CreateNote(int nNoteDuration, bool fCompound, bool fFinal) {
        return CreateNoteRest(nNoteDuration, true, fCompound, fFinal);
    }
    wxString CreateRest(int nRestDuration, bool fCompound, bool fFinal) {
        return CreateNoteRest(nRestDuration, false, fCompound, fFinal);
    }
    wxString CreateLastMeasure(int nNumMeasure, ETimeSignature nTimeSign,
                               bool fOnlyQuarterNotes, TimeUnits rPickupDuration = 0.0);
    wxString CreateAnacruxMeasure(int nNumMeasure, ETimeSignature nTimeSign,
                                  TimeUnits rPickupDuration);

    // pitch related methods
    bool InstantiateNotes(ImoScore* pScore, EKeySignature nKey, int numMeasures);
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


};


}   //namespace lenmus

#endif    // __LENMUS_COMPOSER_H__
