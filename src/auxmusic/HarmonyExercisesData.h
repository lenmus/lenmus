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

#ifndef __LM_HARMONYEXERCISESDATA_H__  
#define __LM_HARMONYEXERCISESDATA_H__

#include "../exercises/ChordConstrains.h"
#include "../score/Note.h"
#include "../score/FiguredBass.h"

const int nMAX_HARMONY_EXERCISE_CHORDS = 20;
const int nNUM_VOICES_IN_HARMONY_EXERCISE = 4;

extern int  nHarmonyExerciseChordsToCheck;
extern lmChord* pHE_Chords[nMAX_HARMONY_EXERCISE_CHORDS];
extern int nHarmonyExcerciseType;

// @@@ TODO: RETHINK: CONSIDER TO REMOVE REDUNDANT INFORMATION 
//   notes, chord type, inversions... are already in lmChord!
// Notes info
extern lmFPitch nHE_NotesFPitch[nNUM_VOICES_IN_HARMONY_EXERCISE][nMAX_HARMONY_EXERCISE_CHORDS];
extern wxString sHE_Notes[nNUM_VOICES_IN_HARMONY_EXERCISE][nMAX_HARMONY_EXERCISE_CHORDS];
extern wxString sHE_Pattern[nNUM_VOICES_IN_HARMONY_EXERCISE][nMAX_HARMONY_EXERCISE_CHORDS];
extern lmNote* pHE_Notes[nNUM_VOICES_IN_HARMONY_EXERCISE][nMAX_HARMONY_EXERCISE_CHORDS];
// Chords info
extern lmEChordType nHE_ChordType[nMAX_HARMONY_EXERCISE_CHORDS];
extern int gnHE_InversionsInChord[nMAX_HARMONY_EXERCISE_CHORDS];
extern lmFiguredBass* pHE_FiguredBass[nMAX_HARMONY_EXERCISE_CHORDS];
extern lmFiguredBass* pHE_UserFiguredBass[nMAX_HARMONY_EXERCISE_CHORDS];  // the introduced by the user
extern int gnHE_NumUserFiguredBass;


#endif
