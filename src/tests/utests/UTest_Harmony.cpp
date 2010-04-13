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

#ifdef __WXDEBUG__

#include "wx/wxprec.h"

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


//@@ todo: continue with testing of 
//  lmChordIntervals, lmChord, lmFPitchChord, lmScoreChord

#endif  // __WXDEBUG__
