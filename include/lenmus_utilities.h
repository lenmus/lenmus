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

#ifndef __LENMUS_UTILITIES_H__        //to avoid nested includes
#define __LENMUS_UTILITIES_H__

//lenmus
#include "lenmus_standard_header.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>

//lomse
#include <lomse_pitch.h>
using namespace lomse;


namespace lenmus
{


//extern bool PitchNameToData(wxString sPitch, int* pPitch, lmEAccidentals* pAccidentals);
//extern bool StringToPitch(wxString sStep, wxString sOctave, int* pPitch);
//extern int LetterToStep(wxString sStep);
//extern int LetterToStep(wxChar cStep);
extern void load_combobox_with_note_names(wxComboBox* pCboBox, DiatonicPitch nSelNote);
extern void load_combobox_with_note_names(wxComboBox* pCboBox, wxString sNoteName);
//extern void lmLoadChoiceWithNoteNames(wxChoice* pChoice, wxString sNoteName);
extern const wxString& get_key_signature_name(EKeySignature nKeySignature);
//
//
//// LDP related
//extern bool LDPDataToPitch(wxString sPitch, lmEAccidentals* pAccidentals,
//                           wxString* sStep, wxString* sOctave);
//extern FPitch lmLDPDataToFPitch(wxString& sPitch);

extern int split_ldp_pattern(const wxString& sSource);
extern bool ldp_pattern_is_rest(const wxString& sElement);

//extern wxString LineStyleToLDP(lmELineStyle nStyle);
//extern wxString LineCapToLDP(lmELineCap nLineCap);
//extern wxString lmTPointToLDP(lmTPoint& tPoint, const wxString& sName=wxEmptyString,
//                              bool fEmptyIfZero=true);
//extern wxString lmColorToLDP(const wxColour& nColor, bool fEmptyIfEqual=true,
//                             const wxColour& nRefColor=*wxBLACK);
//extern wxString lmFloatToLDP(float rValue, const wxString& sName,
//                             bool fEmptyIfEqual=false, float rRefValue=0.0f);
//
//
//// MusicXML related
//extern bool XmlDataToClef(wxString sClefLine, lmEClefType* pClef);
//extern bool XmlDataToBarStyle(wxString sBarStyle, lmEBarline* pType);


}  //namespace lenmus

#endif    // __LENMUS_UTILITIES_H__
