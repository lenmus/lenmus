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

#ifndef __LM_AUXSTRING_H        //to avoid nested includes
#define __LM_AUXSTRING_H

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "AuxString.cpp"
#endif

#include "../score/Score.h"

extern bool PitchNameToData(wxString sPitch, int* pPitch, lmEAccidentals* pAccidentals);
extern bool StringToPitch(wxString sStep, wxString sOctave, int* pPitch);
extern int LetterToStep(wxString sStep);
extern int LetterToStep(wxChar cStep);
extern void LoadCboBoxWithNoteNames(wxComboBox* pCboBox, lmDPitch nSelNote);
extern void LoadCboBoxWithNoteNames(wxComboBox* pCboBox, wxString sNoteName);


// LDP related
extern bool LDPDataToPitch(wxString sPitch, lmEAccidentals* pAccidentals,
                           wxString* sStep, wxString* sOctave);
extern lmFPitch lmLDPDataToFPitch(wxString& sPitch);

extern int SrcSplitPattern(wxString sSource);
extern bool SrcIsRest(wxString sElement);

extern lmEClefType LDPNameToClef(wxString sClefName);
extern lmEKeySignatures LDPNameToKey(wxString sKeyName);

extern wxString LineStyleToLDP(lmELineStyle nStyle);
extern wxString LineCapToLDP(lmELineCap nLineCap);


// MusicXML related
extern bool XmlDataToClef(wxString sClefLine, lmEClefType* pClef);
extern bool XmlDataToBarStyle(wxString sBarStyle, lmEBarline* pType);

#endif    // __LM_AUXSTRING_H
