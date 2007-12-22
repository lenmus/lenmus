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

#ifndef __CONVERSION_H__        //to avoid nested includes
#define __CONVERSION_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Conversion.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

typedef struct lmNoteBitsStruct {
    int nStep;              // 'c'=0, 'd'=1, 'e'=2, 'f'=3, 'g'=4, 'a'=5, 'b'=6
    int nOctave;            // 0..9
    int nAccidentals;       // '--'=-1, '-'=-1, ''=0, '+'=+1, '++'=+2
    int nStepSemitones;     // 'c'=0, 'd'=2, 'e'=4, 'f'=5, 'g'=7, 'a'=9, 'b'=11
} lmNoteBits;

class lmConverter
{
public:
    lmConverter();

    //note parts
    static bool         NoteToBits(wxString sNote, lmNoteBits* pBits);
    static wxString     NoteBitsToName(lmNoteBits& tBits, lmEKeySignatures nKey);
    static int          StepToSemitones(int nStep);
    static int          AccidentalsToInt(wxString sAccidentals);
    static int          StepToInt(wxString sStep);
    static bool         DPitchToBits(lmDPitch nPitch, lmNoteBits* pBits);
};

#endif  // __CONVERSION_H__
