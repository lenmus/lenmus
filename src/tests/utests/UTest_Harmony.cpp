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

#ifdef _LM_DEBUG_

#include <wx/wxprec.h>

//classes related to these tests
#include <UnitTest++.h>
#include "../../auxmusic/Harmony.h"

using namespace UnitTest;

// Integration tests: test with fixture for

// Unit tests: test without fixture

TEST(HarmonicDirection)
{
    //  Harmonic direction of an interval: 
    //   descending (-1) when the interval is negative, ascending (1) when positive, linear when 0
    CHECK( GetHarmonicDirection(-1) == -1);
    CHECK( GetHarmonicDirection(-66000) == -1);
    CHECK( GetHarmonicDirection(1) == 1);
    CHECK( GetHarmonicDirection(67000) == 1);
    CHECK( GetHarmonicDirection(0) == -0);
}

TEST(HarmonicMovementType)
{
    //  Harmonic motion of 2 voices: calculated from the harmonic direction of each voice
    //    parallel when both voices have the same direction
    //    contrary: both voices have opposite direction
    //    oblique: one direction is linear and the other is not
//    GetHarmonicMovementType( lmFPitch fVoice10, lmFPitch fVoice11, lmFPitch fVoice20, lmFPitch fVoice21)
    CHECK( GetHarmonicMovementType( 10, 20, 5, 6) == lm_eDirectMovement);
    CHECK( GetHarmonicMovementType( 15, 14, 30, 10) == lm_eDirectMovement);
    CHECK( GetHarmonicMovementType( 10, 20, 6, 5) == lm_eContraryMovement);
    CHECK( GetHarmonicMovementType( 15, 14, 10, 30) == lm_eContraryMovement);
    CHECK( GetHarmonicMovementType( 12, 12, 5, 6) == lm_eObliqueMovement);
    CHECK( GetHarmonicMovementType( 23, 23, 6, 5) == lm_eObliqueMovement);
}

TEST(FPitchInterval)
{
//  Get interval in FPitch from:
//   chord degree (root note step)
//   key signature
//   interval index (1=3rd, 2=5th, etc)
//lmFIntval FPitchInterval(int nRootStep, lmEKeySignatures nKey, int nInterval)
    CHECK(FPitchInterval(lmSTEP_C, earmDo, 1) == lm_M3);
    CHECK(FPitchInterval(lmSTEP_C, earmDo, 2) == lm_p5);
    CHECK(FPitchInterval(lmSTEP_A, earmLam, 1) == lm_m3);
    CHECK(FPitchInterval(lmSTEP_A, earmLam, 2) == lm_p5);
}

struct SortChordNotesFixture
{
bool CheckOrdered(int nNumNotes, lmFPitch fInpChordNotes[])
{
    for (int i = 1; i < nNumNotes && fInpChordNotes[i]; i++)
        for (int j = 0; j < i; j++)
            if (fInpChordNotes[j] > fInpChordNotes[i])
                return false;
    return true;
}
};

TEST_FIXTURE(SortChordNotesFixture, SortChordNotes)
{
    const int nNumNotes = 5;
    lmFPitch fInpChordNotes1[nNumNotes] = { 39, 30, 20, 11, 0};
    SortChordNotes(nNumNotes, fInpChordNotes1);
    CHECK ( CheckOrdered(nNumNotes, fInpChordNotes1) );
    lmFPitch fInpChordNotes2[nNumNotes] = { 0, 30, 1, 11, 2};
    SortChordNotes(nNumNotes, fInpChordNotes2);
    CHECK ( CheckOrdered(nNumNotes, fInpChordNotes2) );
    lmFPitch fInpChordNotes3[nNumNotes] = { 5, 1, 3, 11, 39};
    SortChordNotes(nNumNotes, fInpChordNotes3);
    CHECK ( CheckOrdered(nNumNotes, fInpChordNotes3) );
    lmFPitch fInpChordNotes4[nNumNotes] = { 0, 1, 3, 11, 39};
    SortChordNotes(nNumNotes, fInpChordNotes4);
    CHECK ( CheckOrdered(nNumNotes, fInpChordNotes4) );
}

//@ TODO: USE THIS FIXTURE (or improved) to tests lmChord with several input data
/*    improve with: notes, chord degree?...
struct lmChordFixture
{
    struct lmChordFixtureDataStruct
    {
        wxString            sFigBass;   //figured bass string
        lmEKeySignatures    nKey;       //key signature
        wxString            sRootNote;  //root note
        lmEChordType        nChordType; //test result, to validate test
        int                 nInversion; //test result, to validate test
        wxString            sIntervals; //test result, to validate test
    };

    static struct lmChordFixtureDataStruct lmChordFixturetTestData[];

};

struct lmChordFixtureDataStruct lmChordFixture::lmChordFixturetTestData[] =
{
        //Minor scale
        //fig
        //bass       key      root note  chord type        inversion     Intvals
        { _T("#"),   earmLam, _T("a3"),  ect_MajorTriad,      0, _T("M3,p5") },
        { _T("b"),   earmLam, _T("a3"),  ect_Max,             0, _T("d3,p5") },
        { _T("="),   earmLam, _T("a3"),  ect_MinorTriad,      0, _T("m3,p5") },
        { _T("2"),   earmLam, _T("a3"),  ect_HalfDimSeventh,  3, _T("M2,p4,m6") },
        { _T("#2"),  earmLam, _T("a3"),  ect_Max,             0, _T("a2,p4,m6") },
        { _T("b2"),  earmLam, _T("a3"),  ect_MajorSeventh,    3, _T("m2,p4,m6") },
        { _T("=2"),  earmLam, _T("a3"),  ect_HalfDimSeventh,  3, _T("M2,p4,m6") },
        { _T("2+"),  earmLam, _T("a3"),  ect_Max,             0, _T("a2,p4,m6") },
        { _T("2 3"), earmLam, _T("a3"),  ect_Max,             0, _T("M2,m3") },
        { _T("3"),   earmLam, _T("a3"),  ect_MinorTriad,      0, _T("m3,p5") },
        { _T("4"),   earmLam, _T("a3"),  ect_Suspended_4th,   0, _T("p4,p5") },
        { _T("4 2"), earmLam, _T("a3"),  ect_HalfDimSeventh,  3, _T("M2,p4,m6") },
        { _T("4+ 2"), earmLam, _T("a3"), ect_AugSixth,        1, _T("M2,a4,m6") },
        { _T("4 3"), earmLam, _T("a3"),  ect_MinorSeventh,    2, _T("m3,p4,m6") },
        { _T("5"),   earmLam, _T("a3"),  ect_MinorTriad,      0, _T("m3,p5") },
        { _T("5 #"), earmLam, _T("a3"),  ect_MajorTriad,      0, _T("M3,p5") },
        { _T("5 b"), earmLam, _T("a3"),  ect_Max,             0, _T("d3,p5") },
        { _T("5+"),  earmLam, _T("a3"),  ect_Max,             0, _T("m3,a5") },
        { _T("5/"),  earmLam, _T("a3"),  ect_Max,             0, _T("m3,a5") },
        { _T("5 3"), earmLam, _T("a3"),  ect_MinorTriad,      0, _T("m3,p5") },
        { _T("5 4"), earmLam, _T("a3"),  ect_Suspended_4th,   0, _T("p4,p5") },
        { _T("6"),   earmLam, _T("a3"),  ect_MajorTriad,      1, _T("m3,m6") },
        { _T("6 #"), earmLam, _T("a3"),  ect_AugTriad,        1, _T("M3,m6") },
        { _T("6 b"), earmLam, _T("a3"),  ect_Max,             0, _T("d3,m6") },
        { _T("6\\"),  earmLam, _T("a3"),  ect_Max,             0, _T("m3,d6") },
        { _T("6 3"), earmLam, _T("a3"),  ect_MajorTriad,      1, _T("m3,m6") },
        { _T("6 #3"), earmLam, _T("a3"), ect_AugTriad,        1, _T("M3,m6") },
        { _T("6 b3"), earmLam, _T("a3"), ect_Max,             0, _T("d3,m6") },
        { _T("6 4"), earmLam, _T("a3"),  ect_MinorTriad,      2, _T("p4,m6") },
        { _T("6 4 2"), earmLam, _T("a3"),ect_HalfDimSeventh,  3, _T("M2,p4,m6") },
        { _T("6 4 3"), earmLam, _T("a3"),ect_MinorSeventh,    2, _T("m3,p4,m6") },
        { _T("6 5"), earmLam, _T("a3"),  ect_MajorSeventh,    1, _T("m3,p5,m6") },
        { _T("6 5 3"), earmLam, _T("a3"),ect_MajorSeventh,    1, _T("m3,p5,m6") },
        { _T("7"),   earmLam, _T("a3"),  ect_MinorSeventh,    0, _T("m3,p5,m7") },
        { _T("7 4 2"), earmLam, _T("a3"), ect_Max,            0, _T("M2,p4,m7") },
        { _T("8"),   earmLam, _T("a3"),  ect_Max,             0, _T("p8") },
        { _T("9"),   earmLam, _T("a3"),  ect_Max,             0, _T("m3,p5,M9") },
        { _T("10"),  earmLam, _T("a3"),  ect_Max,             0, _T("m3,p5,m10") },

        // Mayor scale
        { _T("#"),   earmDo, _T("c4"), ect_Max,              0, _T("a3,p5") },
        { _T("b"),   earmDo, _T("c4"), ect_MinorTriad,       0, _T("m3,p5") },
        { _T("="),   earmDo, _T("c4"), ect_MajorTriad,       0, _T("M3,p5") },
        { _T("2"),   earmDo, _T("c4"), ect_MinorSeventh,     3, _T("M2,p4,M6") },
        { _T("#2"),  earmDo, _T("c4"), ect_Max,              0, _T("a2,p4,M6") },
        { _T("b2"),  earmDo, _T("c4"), ect_AugMajorSeventh,  3, _T("m2,p4,M6") },
        { _T("=2"),  earmDo, _T("c4"), ect_MinorSeventh,     3, _T("M2,p4,M6") },
        { _T("2+"),  earmDo, _T("c4"), ect_Max,              0, _T("a2,p4,M6") },
        { _T("2 3"), earmDo, _T("c4"), ect_Max,              0, _T("M2,M3") },
        { _T("3"),   earmDo, _T("c4"), ect_MajorTriad,       0, _T("M3,p5") },
        { _T("4"),   earmDo, _T("c4"), ect_Suspended_4th,    0, _T("p4,p5") },
        { _T("4 2"), earmDo, _T("c4"), ect_MinorSeventh,     3, _T("M2,p4,M6") },
        { _T("4+ 2"), earmDo, _T("c4"), ect_DominantSeventh, 3, _T("M2,a4,M6") },
        { _T("4 3"), earmDo, _T("c4"), ect_MajorSeventh,     2, _T("M3,p4,M6") },
        { _T("5"),   earmDo, _T("c4"), ect_MajorTriad,       0, _T("M3,p5") },
        { _T("5 #"), earmDo, _T("c4"), ect_Max,              0, _T("a3,p5") },
        { _T("5 b"), earmDo, _T("c4"), ect_MinorTriad,       0, _T("m3,p5") },
        { _T("5+"),  earmDo, _T("c4"), ect_AugTriad,         0, _T("M3,a5") },
        { _T("5/"),  earmDo, _T("c4"), ect_AugTriad,         0, _T("M3,a5") },
        { _T("5 3"), earmDo, _T("c4"), ect_MajorTriad,       0, _T("M3,p5") },
        { _T("5 4"), earmDo, _T("c4"), ect_Suspended_4th,    0, _T("p4,p5") },
        { _T("6"),   earmDo, _T("c4"), ect_MinorTriad,       1, _T("M3,M6") },
        { _T("6 #"), earmDo, _T("c4"), ect_Max,              0, _T("a3,M6") },
        { _T("6 b"), earmDo, _T("c4"), ect_DimTriad,         1, _T("m3,M6") },
        { _T("6\\"),  earmDo, _T("c4"), ect_AugTriad,         1, _T("M3,m6") },
        { _T("6 3"), earmDo, _T("c4"), ect_MinorTriad,       1, _T("M3,M6") },
        { _T("6 #3"), earmDo, _T("c4"), ect_Max,             0, _T("a3,M6") },
        { _T("6 b3"), earmDo, _T("c4"), ect_DimTriad,        1, _T("m3,M6") },
        { _T("6 4"), earmDo, _T("c4"), ect_MajorTriad,       2, _T("p4,M6") },
        { _T("6 4 2"), earmDo, _T("c4"), ect_MinorSeventh,   3, _T("M2,p4,M6") },
        { _T("6 4 3"), earmDo, _T("c4"), ect_MajorSeventh,   2, _T("M3,p4,M6") },
        { _T("6 5"), earmDo, _T("c4"), ect_MinorSeventh,     1, _T("M3,p5,M6") },
        { _T("6 5 3"), earmDo, _T("c4"), ect_MinorSeventh,   1, _T("M3,p5,M6") },
        { _T("7"),   earmDo, _T("c4"), ect_MajorSeventh,     0, _T("M3,p5,M7") },
        { _T("7 4 2"), earmDo, _T("c4"), ect_Max,            0, _T("M2,p4,M7") },
        { _T("8"),   earmDo, _T("c4"), ect_Max,              0, _T("p8") },
        { _T("9"),   earmDo, _T("c4"), ect_Max,              0, _T("M3,p5,M9") },
        { _T("10"),  earmDo, _T("c4"), ect_Max,              0, _T("M3,p5,M10") },
};
*/

TEST(Intervals)
{
    const int nNumIntv = 5;
    const int i1 = 1;
    const int i2 = 12;
    const int i3 = 31;
    lmFIntval pFI[nNumIntv] = { lm_p8, i3+lm_p8,  i3, i2+(lm_p8*2), i1 };
    lmChordIntervals  ci(nNumIntv, pFI);
    // first check raw intervals
    CHECK( ci.GetNumIntervals() == nNumIntv  );
    CHECK( ci.GetInterval(4) == pFI[4]  );
    // then normalize: reduce octave, remove duplicated and sort
    ci.Normalize();
    int i = ci.GetNumIntervals();
    CHECK(3 == ci.GetNumIntervals());
    i = ci.GetInterval(0);
    CHECK(i1 == ci.GetInterval(0) );
    i = ci.GetInterval(1);
    CHECK(i2 == ci.GetInterval(1) );
    i = ci.GetInterval(2);
    CHECK(i3 == ci.GetInterval(2) );

}

TEST(Chords)
{
    // Test data set. TODO: make a fixture with several data sets for different chords; including inversions
    //   MajorTriad chord
    wxString sRootNote = _T("c4");
    wxString psNotes[] = { _T("c4"), _T("e4"), _T("g4")};
    const int nNumNotes = 3;
    wxString sIntervals = _T("M3,p5");
    const int nNumIntervals = nNumNotes  - 1;
    lmEKeySignatures nKey = earmDo;
    int nNumInversions = 0;
    lmEChordType nChordType = ect_MajorTriad;
    int nDegree = lmSTEP_C; // root C4, no inversion : degree I
    int nOctave = 4;  // C4: octave 4
    // notes for lmFPitchChord
    lmFPitch fNotes[] = { FPitch(psNotes[0]),  FPitch(psNotes[1]), FPitch(psNotes[2])};
    // With lmFPitchChord the notes can follow the chord rules:
    //  notes can be duplicated
    //  a note can be increased any number of octaves
    //  but the notes must be ordered
    //   (todo: consider to allow unordered notes in lmFPitchChord constructor; use SortChordNotes)
    lmFPitch fNotes2[] =
    // AWARE: remember: the bass note is ABSOLUTE (octave dependent);
    //         the rest are RELATIVE (octave independent)
    //  Therefore if two chords are equivalent, both must have the same absolute bass note
    //  Therefore, the bass note must be in the first position, while the rest can be unordered
    { FPitch(psNotes[0]),  FPitch(psNotes[2])+(lm_p8*4), FPitch(psNotes[1])+(lm_p8*3), FPitch(psNotes[2])};


    // Check constructors

    //build a chord from root note and type
 // lmChord(wxString sRootNote, lmEChordType nChordType, int nInversion = 0, lmEKeySignatures nKey = earmDo);
    lmChord C1(sRootNote, nChordType, nNumInversions, nKey);
    //build a chord from the root note and the figured bass
    // todo:  this constructor is already tested in lmChordFromFiguredBassUnitTest. 
    //          transform these tests to Unittest++ TEST
    //      lmChord(wxString sRootNote, lmFiguredBass* pFigBass, lmEKeySignatures nKey = earmDo);
    //build a chord from a list of notes in LDP source code
 // lmChord(int nNumNotes, wxString* pNotes, lmEKeySignatures nKey = earmDo);
    lmChord C2(nNumNotes, psNotes, nKey);
    //build a chord from a list of intervals (as strings)
 // lmChord(wxString sRootNote, wxString sIntervals, lmEKeySignatures nKey);
    lmChord C3(sRootNote, sIntervals, nKey);
    // build a chord from "essential" information
//  lmChord(int nDegree, lmEKeySignatures nKey, int nNumIntervals, int nNumInversions, int octave);
    lmChord C4(nDegree, nKey, nNumIntervals, nNumInversions, nOctave);

    wxString sC1 = C1.ToString(); // for debug
    wxString sC2 = C2.ToString();
    wxString sC3 = C3.ToString();
    wxString sC4 = C4.ToString();

    CHECK( C1.IsEqualTo(&C2) );
    CHECK( C2.IsEqualTo(&C3) );
    CHECK( C3.IsEqualTo(&C4) );
    CHECK( C4.IsEqualTo(&C1) );

    CHECK(sC4.compare(sC3) == 0);
    CHECK(sC3.compare(sC2) == 0);
    CHECK(sC2.compare(sC1) == 0);
    CHECK(sC1.compare(sC4) == 0);

//    lmFPitchChord(int nNumNotes, lmFPitch fNotes[], lmEKeySignatures nKey = earmDo);  
    lmFPitchChord C5(nNumNotes, fNotes, nKey);
    const int nNumfNotes2 = sizeof(fNotes2) / sizeof(fNotes2[0]);
    // todo: remove: do not required anymore SortChordNotes(nNumfNotes2, fNotes2);
    lmFPitchChord C6(nNumfNotes2, fNotes2, nKey);


    CHECK( C5.lmChord::IsEqualTo(&C2) );
    CHECK( C6.lmChord::IsEqualTo(&C3) );
    CHECK( C6.IsEqualTo(&C5) );

    // aware: lmFPitchChord is a chord with absolute notes
    //          therefore, two lmFPitchChord can have different notes but the same chord type (lmChord)
    //           in this case, both are "equal" when compared using IsEqualTo()
    //              but different when comparing the ToString()
    //             conclusion: in this case, compare the ToString() of the parent class lmChord
    wxString sC5= C5.lmChord::ToString();
    wxString sC6= C6.lmChord::ToString();
    CHECK(sC5.compare(sC1) == 0);
    CHECK(sC6.compare(sC2) == 0);
    CHECK(sC5.compare(sC6) == 0);
}

// TODO: possible tests
//  with score
//   lmScoreChord
//   AnalyzeHarmonicProgression
//     TODO: to test AnalyzeHarmonicProgression is required some refactoring
//         ProcessScore must be divided: separate into a method the part 
//          that obtains a list of chords (lmScoreChord) from a score 
//         Fixture: method to get a score from a file
//         Test: 
//           Load test file and get the score
//           Get the chords from the score
//           Compare chords with expected
//
//       int nNumHarmonyErrors = AnalyzeHarmonicProgression(&tChordDescriptor[0], nNumChords, pChordErrorBox);



#endif  // _LM_DEBUG_
