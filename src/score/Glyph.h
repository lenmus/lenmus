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

#ifndef __LM_GLYPH_H__        //to avoid nested includes
#define __LM_GLYPH_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Glyph.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "defs.h"

//---------------------------------------------------------
//   Glyphs info
//---------------------------------------------------------

// Definition of an entry of the Glyphs table
struct lmGlyph
{
    // all measurements in tenths
    wxChar	    GlyphChar;
    int		    GlyphOffset;
    int		    SelRectShift;
    int		    SelRectHeight;
    int		    Top;
    int		    Bottom;
	lmTenths	thxPos;
	lmTenths	thyPos;
	lmTenths	thWidth;
	lmTenths	thHeight;
	lmTenths	txDrag;
	lmTenths	tyDrag;

    lmGlyph(const wxChar glyph, int yOffset, int yShift, int selHeight, int top, int bottom, 
		    int xPos, int yPos, int width, int height, int xDrag, int yDrag);

};

//indexes for the table
enum lmEGlyphIndex
{
    GLYPH_NONE = -1,            //special value meaning 'No glyph'

    //noteheads
    GLYPH_NOTEHEAD_HALF = 0,    //half, blanca
    GLYPH_NOTEHEAD_QUARTER,     //quarter, negra
    GLYPH_NOTEHEAD_CROSS,       //cross, aspa

    //notes with stem and flag, in single char
    GLYPH_LONGA_NOTE,           //longa
    GLYPH_BREVE_NOTE,           //breve, cuadrada
    GLYPH_WHOLE_NOTE,
    GLYPH_HALF_NOTE_DOWN,       //half, blanca
    GLYPH_HALF_NOTE_UP,
    GLYPH_QUARTER_NOTE_DOWN,    //quarter, negra
    GLYPH_QUARTER_NOTE_UP,
    GLYPH_EIGHTH_NOTE_DOWN,     //eighth, corchea
    GLYPH_EIGHTH_NOTE_UP,
    GLYPH_16TH_NOTE_DOWN,       //16th, semicorchea
    GLYPH_16TH_NOTE_UP,
    GLYPH_32ND_NOTE_DOWN,       //32nd, fusa
    GLYPH_32ND_NOTE_UP,
    GLYPH_64TH_NOTE_DOWN,       //64th, semifusa
    GLYPH_64TH_NOTE_UP,
    GLYPH_128TH_NOTE_DOWN,      //128th garrapatea
    GLYPH_128TH_NOTE_UP,
    GLYPH_256TH_NOTE_DOWN,      //256th semigarrapatea
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

    //other for time signatures
    GLYPH_COMMON_TIME,
    GLYPH_CUT_TIME,

    //signs
    GLYPH_DOT,                          //dot, for dotted notes
    GLYPH_SMALL_QUARTER_NOTE,           //small quarter note up, for metronome marks
    GLYPH_SMALL_QUARTER_NOTE_DOTTED,    //small dotted quarter note up
    GLYPH_SMALL_EIGHTH_NOTE,            //small eighth note up
    GLYPH_SMALL_EIGHTH_NOTE_DOTTED,     //small dotted eighth note up
    GLYPH_SMALL_EQUAL_SIGN,             //small equal sign, for metronome marks

	GLYPH_BREATH_MARK_V,				//breath-mark  V
    GLYPH_DACAPO,
    GLYPH_DALSEGNO,
    GLYPH_CODA,
    GLYPH_SEGNO,
    GLYPH_OCTAVA,
    GLYPH_FERMATA_OVER,
    GLYPH_FERMATA_UNDER,

    //figured bass. Numbers and other symbols
    GLYPH_FIGURED_BASS_0,                   //number 0
    GLYPH_FIGURED_BASS_1,                   //number 1
    GLYPH_FIGURED_BASS_2,                   //number 2
    GLYPH_FIGURED_BASS_3,                   //number 3
    GLYPH_FIGURED_BASS_4,                   //number 4
    GLYPH_FIGURED_BASS_5,                   //number 5
    GLYPH_FIGURED_BASS_6,                   //number 6
    GLYPH_FIGURED_BASS_7,                   //number 7
    GLYPH_FIGURED_BASS_8,                   //number 8
    GLYPH_FIGURED_BASS_9,                   //number 9
    GLYPH_FIGURED_BASS_SHARP,               //Sharp symbol
    GLYPH_FIGURED_BASS_FLAT,                //Flat symbol
    GLYPH_FIGURED_BASS_NATURAL,             //Natural symbol
    GLYPH_FIGURED_BASS_PLUS,                // +
    GLYPH_FIGURED_BASS_MINUS,               // -
    GLYPH_FIGURED_BASS_OPEN_PARENTHESIS,    // (
    GLYPH_FIGURED_BASS_CLOSE_PARENTHESIS,   // )
    GLYPH_FIGURED_BASS_7_STRIKED,           // 7 with overlayered /

};


extern const lmGlyph aGlyphsInfo[];     //the glyphs table



#endif  // __LM_GLYPH_H__
