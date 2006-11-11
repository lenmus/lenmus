//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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
/*! @file Interval.h
    @brief Header file for class lmInterval
    @ingroup auxmusic
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __INTERVAL_H__        //to avoid nested includes
#define __INTERVAL_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../score/score.h"
#include "../exercises/EarIntvalConstrains.h"
#include "Conversion.h"


typedef struct lmIntvBitsStruct {
    int nNum;              
    int nSemitones;
} lmIntvBits;


enum EIntervalType
{
    eti_Diminished = 0,
    eti_Minor,
    eti_Major,
    eti_Augmented,
    eti_Perfect,
    eti_DoubleAugmented,
    eti_DoubleDiminished
};

enum EIntervalDirection
{
    edi_Ascending = 0,
    edi_Descending,
    edi_Both
};

//global methods defined in this module 
extern wxString ComputeInterval(wxString sRootNote, wxString sIntvCode, 
                                EIntervalDirection nDirection,
                                EKeySignatures nKey = earmDo);
extern void ComputeInterval(lmNoteBits* pRoot, wxString sIntvCode,
                            EIntervalDirection nDirection,
                            lmNoteBits* pNewNote);
extern void AddSemitonesToNote(lmNoteBits* pRoot, wxString sIntvCode,
                               EKeySignatures nKey,
                               EIntervalDirection nDirection,
                               lmNoteBits* pNewNote);
extern wxString IntervalBitsToCode(lmIntvBits& tIntv);
extern bool IntervalCodeToBits(wxString sIntvCode, lmIntvBits* pBits);
extern wxString InvertInterval(wxString sIntvCode);
extern wxString AddIntervals(wxString sIntvCode1, wxString sIntvCode2);
extern wxString SubstractIntervals(wxString sIntvCode1, wxString sIntvCode2);



class lmInterval
{
public:
    //build from two notes
    lmInterval(lmNote* pNote1, lmNote* pNote2, EKeySignatures nKey = earmDo);
    //buid from constrains
    lmInterval(bool fDiatonic, int ntDiatMin, int ntDiatMax, bool sIntvPermitidos[],
             EIntervalDirection nDir, EKeySignatures nKey = earmDo);
    //destructor
    ~lmInterval() {};

    int GetNumSemitones() { return m_nSemi; }
    bool IsAscending() { return (m_ntMidi1 < m_ntMidi2); }

    // access to interval name and type:
    //      Name - a string with the interval full name, i.e.: "Major 3rd"
    //      Code - a string with interval code, i.e.: "M3"
    //      Type - enumeration: just type, no number, i.e.: eti_Major
    //      Num  - the number, i.e.: 3

    wxString GetIntervalName() { return m_sName; }
    int GetIntervalNum() { return m_nNumIntv; }
    EIntervalType GetIntervalType() { return m_nType; }
    wxString GetIntervalCode();

    //accsess to notes
    wxString GetPattern(int i)
        {
            wxASSERT(i == 0 || i == 1);
            return m_sPattern[i];
        }
    int GetMidiNote1() { return m_ntMidi1; }
    int GetMidiNote2() { return m_ntMidi2; }
    void GetNoteBits(int i, lmNoteBits* pNote);


private:
    void Analyze();
    void InitializeStrings();


        //member variables

    //data variables
    int               m_nSemi;
    EKeySignatures    m_nKey;            //key signature 
    int               m_ntMidi1;
    int               m_ntMidi2;
    int               m_ntDiat1;
    int               m_ntDiat2;

    // results of the analysis
    EIntervalType    m_nType;
    int              m_nNumIntv;
    wxString         m_sName;
    wxString         m_sPattern[2];  //without key accidentals, but with own accidentals

};

#endif  // __INTERVAL_H__

