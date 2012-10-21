//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2012 LenMus project
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

#ifndef __LENMUS_INTERVAL_H__        //to avoid nested includes
#define __LENMUS_INTERVAL_H__

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_ear_intervals_constrains.h"
#include "lenmus_conversion.h"

//lomse
#include <lomse_pitch.h>
#include <lomse_internal_model.h>
#include <lomse_im_note.h>
using namespace lomse;


namespace lenmus
{

//=======================================================================================


////forward declarations
//typedef struct lmIntvBitsStruct {
//    int nNum;
//    int nSemitones;
//} lmIntvBits;
//

enum EIntervalType
{
    k_diminished = 0,
    k_minor,
    k_major,
    k_augmented,
    k_perfect,
    k_double_augmented,
    k_double_diminished
};

//enum lmEIntervalDirection
//{
//    edi_Ascending = 0,
//    edi_Descending,
//    edi_Both
//};
//
////global methods defined in this module
//extern wxString ComputeInterval(wxString sRootNote, wxString sIntvCode,
//                                bool fAscending, EKeySignature nKey = k_key_C);
//extern void ComputeInterval(NoteBits* pRoot, wxString sIntvCode,
//                            bool fAscending, NoteBits* pNewNote);
////extern void AddSemitonesToNote(NoteBits* pRoot, wxString sIntvCode,
////                               EKeySignature nKey,
////                               lmEIntervalDirection nDirection,
////                               NoteBits* pNewNote);
//extern wxString IntervalBitsToCode(lmIntvBits& tIntv);
//extern bool IntervalCodeToBits(wxString sIntvCode, lmIntvBits* pBits);



//---------------------------------------------------------------------------------------
// Intervals in FPitch
//
// Intervals are just the difference between the two FPitches.
// Intervals can be added/substracted. For example: p5-3M = 3m (23-12=11)
// Intervals greater than one octave are computed by adding 40 (p8) for each octave.
// For example, a Major 10th is M3+p8 = 12+40 = 52

class FIntval
{
protected:
    int m_interval;

public:
    FIntval(const wxString& sName);
    FIntval(int intv, EIntervalType type);
    FIntval(int value) : m_interval(value) {}
    FIntval() : m_interval(-1) {}

    // operator to cast to an int
    operator const int() { return m_interval; }

    //get interval attributes
    wxString get_code();        //i.e. "m2", "p4", "M3"
    int get_number();           //i.e. 2, 4, 3
    wxString get_name();        //i.e. "Minor 2nd", "Perfect 4th", Major 3rd"
    EIntervalType get_type();   //i.e. k_minor, k_perfect, k_major
    int get_num_semitones();

    // comparison operators
    bool operator ==(FIntval intv) { return m_interval == int(intv); }
    bool operator !=(FIntval intv) { return m_interval != int(intv); }
    bool operator < (FIntval intv) { return m_interval < int(intv); }
    bool operator > (FIntval intv) { return m_interval > int(intv); }
    bool operator <= (FIntval intv) { return m_interval <= int(intv); }
    bool operator >= (FIntval intv) { return m_interval >= int(intv); }

    //operations
    FIntval operator -(FIntval intv) { return FIntval(m_interval - int(intv)); }
    FIntval operator +(FIntval intv) { return FIntval(m_interval + int(intv)); }
    FIntval operator -=(FIntval intv) {
        m_interval -= int(intv);
        return FIntval(m_interval);
    }
    FIntval operator +=(FIntval intv) {
        m_interval += int(intv);
        return FIntval(m_interval);
    }

protected:
    void initialize_strings();

};


    //unison
#define lm_p1   FIntval(0)
#define lm_a1   FIntval(1)
#define lm_da1  FIntval(2)
    //second
#define lm_dd2  FIntval(3)
#define lm_d2   FIntval(4)
#define lm_m2   FIntval(5)
#define lm_M2   FIntval(6)
#define lm_a2   FIntval(7)
#define lm_da2  FIntval(8)
    //third
#define lm_dd3  FIntval(9)
#define lm_d3   FIntval(10)
#define lm_m3   FIntval(11)
#define lm_M3   FIntval(12)
#define lm_a3   FIntval(13)
#define lm_da3  FIntval(14)
    //fourth
#define lm_dd4  FIntval(15)
#define lm_d4   FIntval(16)
#define lm_p4   FIntval(17)
#define lm_a4   FIntval(18)
#define lm_da4  FIntval(19)
    //fifth
#define lm_dd5  FIntval(21)
#define lm_d5   FIntval(22)
#define lm_p5   FIntval(23)
#define lm_a5   FIntval(24)
#define lm_da5  FIntval(25)
    //sixth
#define lm_dd6  FIntval(26)
#define lm_d6   FIntval(27)
#define lm_m6   FIntval(28)
#define lm_M6   FIntval(29)
#define lm_a6   FIntval(30)
#define lm_da6  FIntval(31)
    //seventh
#define lm_dd7  FIntval(32)
#define lm_d7   FIntval(33)
#define lm_m7   FIntval(34)
#define lm_M7   FIntval(35)
#define lm_a7   FIntval(36)
#define lm_da7  FIntval(37)
    //octave
#define lm_dd8  FIntval(38)
#define lm_d8   FIntval(39)
#define lm_p8   FIntval(40)
#define lm_a8   FIntval(41)
#define lm_da8  FIntval(42)
    //9th
#define lm_dd9  FIntval(43)
#define lm_d9   FIntval(44)
#define lm_m9   FIntval(45)
#define lm_M9   FIntval(46)
#define lm_a9   FIntval(47)
#define lm_da9  FIntval(48)
    //10th
#define lm_dd10 FIntval(49)
#define lm_d10  FIntval(50)
#define lm_m10  FIntval(51)
#define lm_M10  FIntval(52)
#define lm_a10  FIntval(53)
#define lm_da10 FIntval(54)
    //11h
#define lm_dd11 FIntval(55)
#define lm_d11  FIntval(56)
#define lm_p11  FIntval(57)
#define lm_a11  FIntval(58)
#define lm_da11 FIntval(59)
    //12h
#define lm_dd12 FIntval(61)
#define lm_d12  FIntval(62)
#define lm_p12  FIntval(63)
#define lm_a12  FIntval(64)
#define lm_da12 FIntval(65)
    //13h
#define lm_dd13 FIntval(66)
#define lm_d13  FIntval(67)
#define lm_m13  FIntval(68)
#define lm_M13  FIntval(69)
#define lm_a13  FIntval(70)
#define lm_da13 FIntval(71)
    //14h
#define lm_dd14 FIntval(72)
#define lm_d14  FIntval(73)
#define lm_m14  FIntval(74)
#define lm_M14  FIntval(75)
#define lm_a14  FIntval(76)
#define lm_da14 FIntval(77)
    //two octaves
#define lm_dd15 FIntval(78)
#define lm_d15  FIntval(79)
#define lm_p15  FIntval(80)

// Define a 'NULL interval'. It is usefull to signal special situations such as
// 'no interval defined', 'end of list', etc.
#define lmNULL_FIntval  FIntval(-1)


//---------------------------------------------------------------------------------------
// Interval class is a totally defined interval: two notes
// It is oriented to aural training exercises, that require special constrains.
class Interval
{
protected:
    FPitch m_fp[2];
    EKeySignature m_nKey;

public:
    Interval(bool fDiatonic, DiatonicPitch dpMin, DiatonicPitch dpMax,
             bool fAllowedIntervals[], bool fAscending,
             EKeySignature nKey, FPitch fpStartNote=k_undefined_fpitch);

    ~Interval() {};

    int get_num_semitones();
    FIntval get_interval() { return FIntval( abs((int)m_fp[0] - (int)m_fp[1]) ); }
    inline FPitch get_pitch(int iNote) { return m_fp[iNote]; }
    wxString get_interval_name();

};


}   //namespace lenmus

#endif  // __LENMUS_INTERVAL_H__
