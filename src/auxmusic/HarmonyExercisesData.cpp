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

#include "HarmonyExercisesData.h"

// TODO: consider a storage better than just global (extern) variables
//  data is exchange between lmTheoHarmonyCtrol and  lmHarmonyProcessor

// todo: improve with inversions: just calculate bass note from root note and inversions
//       consider: inversions might be an option of the exercise

// Exercise 1 checks:
//   bass note
// Exercise 2 checks:
//   soprano note
//   bass note
//    todo: consider no to chek the bass: let free the chord degree
int  nHarmonyExerciseChordsToCheck = 0;
lmFPitch nHE_BassNotesFPitch[nMAX_HARMONY_EXERCISE_CHORDS]; // check bass note in Excercise 1 and 2
lmFPitch nHE_SopranoNotesFPitch[nMAX_HARMONY_EXERCISE_CHORDS]; // check Soprano note in Excercise 2
lmEChordType nHE_ChordType[nMAX_HARMONY_EXERCISE_CHORDS];
int nHarmonyExcerciseType;

