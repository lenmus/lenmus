// RCS-ID: $Id$
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
    wxString    sName;

    lmGlyph(const wxChar g, int yo, int ys, int h, const wxString n)
        : GlyphChar(g), GlyphOffset(yo), SelRectShift(ys), SelRectHeight(h), sName(n)
        {
        }

};

//indexes for the table
enum lmEGlyphIndex {
    // notes and noteheads
    ////! @todo    //larga
    ////! @todo    //breve, cuadrada
    GLYPH_NOTEHEAD_WHOLE = 0, 
    GLYPH_NOTEHEAD_HALF,    //half, blanca 
    GLYPH_NOTEHEAD_QUARTER, //quarter, negra 
    GLYPH_NOTEHEAD_CROSS,   //cross, aspa
    GLYPH_EIGHTH_NOTE_DOWN, //eighth, corchea 
    GLYPH_EIGHTH_NOTE_UP, 
    GLYPH_16TH_NOTE_DOWN,   //16th, semicorchea
    GLYPH_16TH_NOTE_UP, 
    GLYPH_32TH_NOTE_DOWN,   //32th, fusa 
    GLYPH_32TH_NOTE_UP,
    GLYPH_64TH_NOTE_DOWN,   //64th, semifusa 
    GLYPH_64TH_NOTE_UP,
    ////! @todo,    //128th garrapatea
    ////! @todo,    //256th semigarrapatea

    // rests
    ////! @todo    //larga
    ////! @todo    //breve, cuadrada
    GLYPH_WHOLE_REST,       //whole, redonda
    GLYPH_HALF_REST,        //half, blanca
    GLYPH_QUARTER_REST,     //quarter, negra
    GLYPH_EIGHTH_REST,      //eighth, corchea
    GLYPH_16TH_REST,        //16th, semicorchea
    GLYPH_32ND_REST,        //32th, fusa
    GLYPH_64TH_REST,        //64th, semifusa
    ////! @todo,    //128th garrapatea
    ////! @todo,    //256th semigarrapatea
};


extern const lmGlyph aGlyphsInfo[];     //the glyphs table



#endif  // __GLYPH_H__
