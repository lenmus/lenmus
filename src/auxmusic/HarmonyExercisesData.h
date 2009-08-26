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

//@@@@ TODO: Move these aux data to lmHarmonyConstrains ?

// Exercise 1 checks:
//   root note
//   chord type
// Root notes
const int nMAX_E1BCHORDS = 20;
extern int  nHarmonyExercise1ChordsToCheck;
extern lmFPitch nExerciseBassNotesFPitch[nMAX_E1BCHORDS];
extern lmFPitch nExercise2NotesFPitch[nMAX_E1BCHORDS];
extern lmEChordType nExercise1ChordType[nMAX_E1BCHORDS];
extern int nHarmonyExcerciseType;


#endif
