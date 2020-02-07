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

#ifndef __LENMUS_SCALE_H__        //to avoid nested includes
#define __LENMUS_SCALE_H__

//lenmus
#include "lenmus_standard_header.h"

//wxString
#include <wx/wxprec.h>
#include <wx/wx.h>

//lomse
#include <lomse_pitch.h>
using namespace lomse;


namespace lenmus
{

//---------------------------------------------------------------------------------------
// enum to classify scales
//
//    Major scales            Type
//    ---------------------   ----
//    Natural                 I
//    Harmonic                II
//    Type III                III
//    Mixolydian              IV
//
//    Minor scales            Type
//    ---------------------   ----
//    Natural
//    Melodic
//    Dorian
//    Harmonic
//
//    Medievals modes         Mode    Gregorian modes
//    ---------------------   ----    --------------------------
//    Protus      auténtico   I       Dorico          Dorian (Re)
//                plagal      II      Hipodórico
//    Deuterus    auténtico   III     Frigio          Phrygian (Mi)
//                plagal      IV      Hipofrigio
//    Tritus      auténtico   V       Lidio           Lydian (Fa)
//                plagal      VI      Hipolidio
//    Tetrardus   auténtico   VII     Mixolidio       Mixolydian (Sol)
//                plagal      VIII    Hipomixolidio
//
//    Modes introduced in 1547:
//
//                auténtico   IX      Eolio           Aeolian (La = minor natural)
//                plagal      X       Hipoeolio
//                auténtico   XI      Jónico          Ionian (Do = major natural)
//                plagal      XII     Hipojónico
//
//    Later introduced modes (rarely used):
//
//                            XIII    Locrio          Locrian (Si)
//                            XIV     Hipolocrio
//
enum EScaleType
{
    // Major scales
    est_MajorNatural = 0,
    est_MajorTypeII,
    est_MajorTypeIII,
    est_MajorTypeIV,
    est_LastMajor = est_MajorTypeIV,

    // Minor scales
    est_MinorNatural,
    est_MinorDorian,
    est_MinorHarmonic,
    est_MinorMelodic,
    est_LastMinor = est_MinorMelodic,

    // From here, scales without mode
    est_EndOfModalScales = est_LastMinor,

    // Gregorian modes
    est_GreekIonian,
    est_GreekDorian,
    est_GreekPhrygian,
    est_GreekLydian,
    est_GreekMixolydian,
    est_GreekAeolian,
    est_GreekLocrian,
    est_LastGreek = est_GreekLocrian,

    // Other scales
    est_PentatonicMinor,
    est_PentatonicMajor,
    est_Blues,
        //Start of non-tonal scales
    est_StartNonTonal,
    est_WholeTones = est_StartNonTonal,
    est_Chromatic,
    est_LastOther = est_Chromatic,

    //last element, to signal End Of Table
    est_Max
};


//---------------------------------------------------------------------------------------
//a scale is a sequence of up 13 notes (12 chromatic notes plus repetition of first one).

#define     k_notes_in_scale    13     //Change this for more notes in a scale

class Scale
{
protected:
    EScaleType    m_nType;
    EKeySignature   m_nKey;
    FPitch          m_fpNote[k_notes_in_scale];     //the scale

public:
    //build a scale from root note and type
    Scale(FPitch fpRootNote, EScaleType nScaleType, EKeySignature nKey = k_key_C);
    ~Scale();

    inline EScaleType get_scale_type() { return m_nType; }
    inline wxString get_name() { return type_to_name( m_nType ); }
    int get_num_notes();
    string rel_ldp_name_for_note(int i);
    string abs_ldp_name_for_note(int i);
    inline FPitch get_note(int i) { return m_fpNote[i]; }

    //static methods
    static wxString type_to_name(EScaleType nType);
    static bool is_major(EScaleType nType);
    static bool is_minor(EScaleType nType);
    static bool is_gregorian(EScaleType nType);
    static bool is_tonal(EScaleType nScaleType) { return nScaleType < est_StartNonTonal; }
    static EScaleType short_name_to_type(const wxString& sName);

};


}   //namespace lenmus

#endif  // __LENMUS_SCALE_H__
