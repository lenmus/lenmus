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

#ifdef __GNUG__
#pragma interface "AuxString.cpp"
#endif

#ifndef __AUXSTRING_H        //to avoid nested includes
#define __AUXSTRING_H

#include "../score/Score.h"

extern bool PitchNameToData(wxString sPitch, int* pPitch, EAccidentals* pAccidentals);
extern bool StringToPitch(wxString sStep, wxString sOctave, int* pPitch);
extern int LetterToStep(wxString sStep);
extern void LoadCboBoxWithNoteNames(wxComboBox* pCboBox, lmPitch nSelNote);
extern void LoadCboBoxWithNoteNames(wxComboBox* pCboBox, wxString sNoteName);


// LDP related
extern bool LDPDataToPitch(wxString sPitch, EAccidentals* pAccidentals,
                           wxString* sStep, wxString* sOctave);

extern float SrcGetPatternDuracion(wxString sPattern);
extern float SrcGetElementDuracion(wxString sElement);
extern int SrcSplitPattern(wxString sSource);
extern bool SrcIsRest(wxString sElement);


extern EClefType LDPNameToClef(wxString sClefName);
extern EKeySignatures LDPInternalNameToKey(wxString sKeyName);



// MusicXML related
extern bool XmlDataToClef(wxString sClefLine, EClefType* pClef);
extern bool XmlDataToBarStyle(wxString sBarStyle, EBarline* pType);

#endif    // __AUXSTRING_H
