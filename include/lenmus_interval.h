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

//#ifndef __LENMUS_INTERVAL_H__        //to avoid nested includes
//#define __LENMUS_INTERVAL_H__
//
//// For compilers that support precompilation, includes <wx/wx.h>.
//#include <wx/wxprec.h>
//#include <wx/wx.h>
//
//#include "lenmus_ear_intervals_constrains.h"
//#include "lenmus_conversion.h"
//
////lomse
//#include "lomse_pitch.h"
//#include "lomse_internal_model.h"
//#include "lomse_im_note.h"
//using namespace lomse;
//
//
//namespace lenmus
//{
//
////=======================================================================================
//
//
////forward declarations
//typedef struct lmIntvBitsStruct {
//    int nNum;
//    int nSemitones;
//} lmIntvBits;
//
//
//enum lmEIntervalType
//{
//    eti_Diminished = 0,
//    eti_Minor,
//    eti_Major,
//    eti_Augmented,
//    eti_Perfect,
//    eti_DoubleAugmented,
//    eti_DoubleDiminished
//};
//
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
//
//
//
//// Intervals in FPitch
//
//// Intervals are just the difference between the two FPitches.
//// Intervals can be added/substracted. For example: p5-3M = 3m (23-12=11)
//// Intervals greater than one octave are computed by adding 40 (p8) for each octave.
//// For example, a Major 10th is M3+p8 = 12+40 = 52
//
//    //unison
//#define lm_p1   0
//#define lm_a1   1
//#define lm_da1  2
//    //second
//#define lm_dd2  3
//#define lm_d2   4
//#define lm_m2   5
//#define lm_M2   6
//#define lm_a2   7
//#define lm_da2  8
//    //third
//#define lm_dd3  9
//#define lm_d3   10
//#define lm_m3   11
//#define lm_M3   12
//#define lm_a3   13
//#define lm_da3  14
//    //fourth
//#define lm_dd4  15
//#define lm_d4   16
//#define lm_p4   17
//#define lm_a4   18
//#define lm_da4  19
//    //fifth
//#define lm_dd5  21
//#define lm_d5   22
//#define lm_p5   23
//#define lm_a5   24
//#define lm_da5  25
//    //sixth
//#define lm_dd6  26
//#define lm_d6   27
//#define lm_m6   28
//#define lm_M6   29
//#define lm_a6   30
//#define lm_da6  31
//    //seventh
//#define lm_dd7  32
//#define lm_d7   33
//#define lm_m7   34
//#define lm_M7   35
//#define lm_a7   36
//#define lm_da7  37
//    //octave
//#define lm_dd8  38
//#define lm_d8   39
//#define lm_p8   40
//    //other intervals used in chord definitions
//#define lm_M9   (lm_M2+40)
//#define lm_p11  (lm_p4+40)
//#define lm_M13  (lm_M6+40)
//
//// Define a 'NULL interval'. It is usefull to signal special situations such as
//// 'no interval defined', 'end of list', etc.
//#define lmNULL_FIntval  -1
//
////FIntval 'constructors'
//extern FIntval FIntval(wxString& sName);
//extern FIntval FIntval_FromType(int nIntv, lmEIntervalType nType);
//
////FIntval 'methods'
//extern int FIntval_GetNumber(FIntval fi);             //get interval number. i.e. 1, 4
//extern wxString FIntval_GetIntvCode(FIntval fi);      //get code i.e. "m2", "p4", "da3"
//extern wxString FIntval_GetName(FIntval fi);          //get name. i.e. "Major 3rd"
//extern lmEIntervalType FIntval_GetType(FIntval fi);   //get type
//
//
//
////---------------------------------------------------------------------------------------
//class Interval
//{
//public:
//    //buid from constraints
//    Interval(bool fDiatonic, int ntDiatMin, int ntDiatMax, bool fAllowedIntervals[],
//             bool fAscending, EKeySignature nKey = k_key_C, int nMidiStartNote=0);
//    //destructor
//    ~Interval() {};
//
//    int GetNumSemitones() { return m_nSemi; }
//    bool IsAscending() { return (m_MPitch1 < m_MPitch2); }
//
//    // access to interval name and type:
//    //      Name - a string with the interval full name, i.e.: "Major 3rd"
//    //      Code - a string with interval code, i.e.: "M3"
//    //      Type - enumeration: just type, no number, i.e.: eti_Major
//    //      Num  - the number, i.e.: 3
//
//    wxString GetIntervalName() { return m_sName; }
//    int GetIntervalNum() { return m_nNumIntv; }
//    lmEIntervalType GetIntervalType() { return m_nType; }
//    wxString GetIntervalCode();
//
//    //accsess to notes
//    wxString GetPattern(int i)
//        {
//            wxASSERT(i == 0 || i == 1);
//            return m_sPattern[i];
//        }
//    int GetMidiNote1() { return m_MPitch1; }
//    int GetMidiNote2() { return m_MPitch2; }
//    void GetNoteBits(int i, NoteBits* pNote);
//
//
//private:
//    void Analyze();
//    void InitializeStrings();
//
//
//        //member variables
//
//    //data variables
//    int               m_nSemi;
//    EKeySignature    m_nKey;            //key signature
//    MidiPitch          m_MPitch1;
//    MidiPitch          m_MPitch2;
//    DiatonicPitch          m_DPitch1;
//    DiatonicPitch          m_DPitch2;
//
//    // results of the analysis
//    lmEIntervalType    m_nType;
//    int              m_nNumIntv;
//    wxString         m_sName;
//    wxString         m_sPattern[2];  //without key accidentals, but with own accidentals
//
//};
//
//
//}   //namespace lenmus
//
//#endif  // __LENMUS_INTERVAL_H__
