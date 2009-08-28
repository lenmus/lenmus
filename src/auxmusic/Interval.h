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

#ifndef __LM_INTERVAL_H__        //to avoid nested includes
#define __LM_INTERVAL_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Interval.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../score/Score.h"
#include "../exercises/EarIntvalConstrains.h"
#include "Conversion.h"


typedef struct lmIntvBitsStruct {
    int nNum;
    int nSemitones;
} lmIntvBits;


enum lmEIntervalType
{
    eti_Diminished = 0,
    eti_Minor,
    eti_Major,
    eti_Augmented,
    eti_Perfect,
    eti_DoubleAugmented,
    eti_DoubleDiminished
};

enum lmEIntervalDirection
{
    edi_Ascending = 0,
    edi_Descending,
    edi_Both
};

//global methods defined in this module
extern wxString ComputeInterval(wxString sRootNote, wxString sIntvCode,
                                bool fAscending, lmEKeySignatures nKey = earmDo);
extern void ComputeInterval(lmNoteBits* pRoot, wxString sIntvCode,
                            bool fAscending, lmNoteBits* pNewNote);
//extern void AddSemitonesToNote(lmNoteBits* pRoot, wxString sIntvCode,
//                               lmEKeySignatures nKey,
//                               lmEIntervalDirection nDirection,
//                               lmNoteBits* pNewNote);
extern wxString IntervalBitsToCode(lmIntvBits& tIntv);
extern bool IntervalCodeToBits(wxString sIntvCode, lmIntvBits* pBits);



// Intervals in FPitch

// Intervals are just the difference between the two FPitches.
// Intervals can be added/substracted. For example: p5-3M = 3m (23-12=11)
// Intervals greater than one octave are computed by adding 40 (p8) for each octave.
// For example, a Major 10th is M3+p8 = 12+40 = 52


    //unison
#define lm_p1   0
#define lm_a1   1
#define lm_da1  2
    //second
#define lm_dd2  3
#define lm_d2   4 
#define lm_m2   5 
#define lm_M2   6 
#define lm_a2   7
#define lm_da2  8
    //third
#define lm_dd3  9
#define lm_d3   10 
#define lm_m3   11 
#define lm_M3   12 
#define lm_a3   13
#define lm_da3  14
    //fourth
#define lm_dd4  15
#define lm_d4   16 
#define lm_p4   17 
#define lm_a4   18
#define lm_da4  19
    //fifth
#define lm_dd5  21
#define lm_d5   22 
#define lm_p5   23 
#define lm_a5   24
#define lm_da5  25
    //sixth
#define lm_dd6  26
#define lm_d6   27 
#define lm_m6   28 
#define lm_M6   29 
#define lm_a6   30
#define lm_da6  31
    //seventh
#define lm_dd7  32
#define lm_d7   33 
#define lm_m7   34 
#define lm_M7   35 
#define lm_a7   36
#define lm_da7  37
    //octave
#define lm_dd8  38
#define lm_d8   39 
#define lm_p8   40

// Lets define a 'NULL interval'. It is usefull to signal special situations such as
// 'no interval defined', 'end of list', etc.
#define lmNULL_FIntval  -1

//lmFIntval 'constructors'
extern lmFIntval FIntval(wxString& sName);
extern lmFIntval FIntval_FromType(int nIntv, lmEIntervalType nType);

//lmFIntval 'methods'
extern int FIntval_GetNumber(lmFIntval fi);             //get interval number. i.e. 1, 4
extern wxString FIntval_GetIntvCode(lmFIntval fi);      //get code i.e. "m2", "p4", "da3"
extern wxString FIntval_GetName(lmFIntval fi);          //get name. i.e. "Major 3rd"
extern lmEIntervalType FIntval_GetType(lmFIntval fi);   //get type



class lmInterval
{
public:
    //buid from constraints
    lmInterval(bool fDiatonic, int ntDiatMin, int ntDiatMax, bool fAllowedIntervals[],
             bool fAscending, lmEKeySignatures nKey = earmDo, int nMidiStartNote=0);
    //destructor
    ~lmInterval() {};

    int GetNumSemitones() { return m_nSemi; }
    bool IsAscending() { return (m_MPitch1 < m_MPitch2); }

    // access to interval name and type:
    //      Name - a string with the interval full name, i.e.: "Major 3rd"
    //      Code - a string with interval code, i.e.: "M3"
    //      Type - enumeration: just type, no number, i.e.: eti_Major
    //      Num  - the number, i.e.: 3

    wxString GetIntervalName() { return m_sName; }
    int GetIntervalNum() { return m_nNumIntv; }
    lmEIntervalType GetIntervalType() { return m_nType; }
    wxString GetIntervalCode();

    //accsess to notes
    wxString GetPattern(int i)
        {
            wxASSERT(i == 0 || i == 1);
            return m_sPattern[i];
        }
    int GetMidiNote1() { return m_MPitch1; }
    int GetMidiNote2() { return m_MPitch2; }
    void GetNoteBits(int i, lmNoteBits* pNote);


private:
    void Analyze();
    void InitializeStrings();


        //member variables

    //data variables
    int               m_nSemi;
    lmEKeySignatures    m_nKey;            //key signature
    lmMPitch          m_MPitch1;
    lmMPitch          m_MPitch2;
    lmDPitch          m_DPitch1;
    lmDPitch          m_DPitch2;

    // results of the analysis
    lmEIntervalType    m_nType;
    int              m_nNumIntv;
    wxString         m_sName;
    wxString         m_sPattern[2];  //without key accidentals, but with own accidentals

};

#endif  // __LM_INTERVAL_H__

