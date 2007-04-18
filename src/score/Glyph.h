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
/*! @file Glyph.h
    @brief Header file for class lmGlyph
    @ingroup score_kernel
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __GLYPH_H__        //to avoid nested includes
#define __GLYPH_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

//---------------------------------------------------------
//   Glyphs info
//---------------------------------------------------------

// Definition of an entry of the Glyphs table
struct lmGlyph
{
    // all measurements in tenths
    wxChar      GlyphChar;
    int         GlyphOffset;
    int         SelRectShift;
    int         SelRectHeight;
    int         Top;
    int         Bottom;
    wxString    sName;

    lmGlyph(const wxChar g, int yo, int ys, int h, int t, int b, const wxString n)
        : GlyphChar(g), GlyphOffset(yo), SelRectShift(ys), SelRectHeight(h), 
          Top(t), Bottom(b), sName(n)
        {
        }

};

//indexes for the table
enum lmEGlyphIndex {
    // notes and noteheads
    GLYPH_LONGA_NOTE = 0,   //longa
    GLYPH_BREVE_NOTE,       //breve, cuadrada
    GLYPH_WHOLE_NOTE, 
    GLYPH_NOTEHEAD_HALF,    //half, blanca 
    GLYPH_NOTEHEAD_QUARTER, //quarter, negra 
    GLYPH_NOTEHEAD_CROSS,   //cross, aspa
    GLYPH_EIGHTH_NOTE_DOWN, //eighth, corchea 
    GLYPH_EIGHTH_NOTE_UP, 
    GLYPH_16TH_NOTE_DOWN,   //16th, semicorchea
    GLYPH_16TH_NOTE_UP, 
    GLYPH_32ND_NOTE_DOWN,   //32nd, fusa 
    GLYPH_32ND_NOTE_UP,
    GLYPH_64TH_NOTE_DOWN,   //64th, semifusa 
    GLYPH_64TH_NOTE_UP,
    GLYPH_128TH_NOTE_DOWN,  //128th garrapatea
    GLYPH_128TH_NOTE_UP,
    GLYPH_256TH_NOTE_DOWN,  //256th semigarrapatea 
    GLYPH_256TH_NOTE_UP,

    // rests
    GLYPH_LONGA_REST,       //longa
    GLYPH_BREVE_REST,       //breve, cuadrada
    GLYPH_WHOLE_REST,       //whole, redonda
    GLYPH_HALF_REST,        //half, blanca
    GLYPH_QUARTER_REST,     //quarter, negra
    GLYPH_EIGHTH_REST,      //eighth, corchea
    GLYPH_16TH_REST,        //16th, semicorchea
    GLYPH_32ND_REST,        //32nd, fusa
    GLYPH_64TH_REST,        //64th, semifusa
    GLYPH_128TH_REST,       //128th, garrapatea
    GLYPH_256TH_REST,       //256th, semigarrapatea

    //note flags
    GLYPH_EIGHTH_FLAG_DOWN,     //eighth, corchea 
    GLYPH_16TH_FLAG_DOWN,       //16th, semicorchea
    GLYPH_32ND_FLAG_DOWN,       //32nd, fusa 
    GLYPH_64TH_FLAG_DOWN,       //64th, semifusa 
    GLYPH_128TH_FLAG_DOWN,      //128th, garrapatea
    GLYPH_256TH_FLAG_DOWN,      //256th, semigarrapatea
    GLYPH_EIGHTH_FLAG_UP, 
    GLYPH_16TH_FLAG_UP, 
    GLYPH_32ND_FLAG_UP,
    GLYPH_64TH_FLAG_UP,
    GLYPH_128TH_FLAG_UP,
    GLYPH_256TH_FLAG_UP,

    //accidentals
    GLYPH_NATURAL_ACCIDENTAL,
    GLYPH_SHARP_ACCIDENTAL,
    GLYPH_FLAT_ACCIDENTAL,
    GLYPH_DOUBLE_SHARP_ACCIDENTAL,
    GLYPH_DOUBLE_FLAT_ACCIDENTAL,

    //clefs
    GLYPH_G_CLEF,
    GLYPH_F_CLEF,
    GLYPH_F_CLEF_OTTAVA_ALTA, 
    GLYPH_F_CLEF_OTTAVA_BASSA,
    GLYPH_C_CLEF,
    GLYPH_PERCUSSION_CLEF_BLOCK,

    //numbers for time signatures
    GLYPH_NUMBER_0,
    GLYPH_NUMBER_1,
    GLYPH_NUMBER_2,
    GLYPH_NUMBER_3,
    GLYPH_NUMBER_4,
    GLYPH_NUMBER_5,
    GLYPH_NUMBER_6,
    GLYPH_NUMBER_7,
    GLYPH_NUMBER_8,
    GLYPH_NUMBER_9,

    //signs
    GLYPH_DOT,                          //dot, for dotted notes
    GLYPH_SMALL_QUARTER_NOTE,           //small quarter note up, for metronome marks
    GLYPH_SMALL_QUARTER_NOTE_DOTTED,    //small dotted quarter note up
    GLYPH_SMALL_EIGHTH_NOTE,            //small eighth note up
    GLYPH_SMALL_EIGHTH_NOTE_DOTTED,     //small dotted eighth note up
    GLYPH_SMALL_EQUAL_SIGN,             //small equal sign, for metronome marks


};


extern const lmGlyph aGlyphsInfo[];     //the glyphs table



#endif  // __GLYPH_H__
