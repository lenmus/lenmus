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
/*! @file Glyph.cpp
    @brief Implementation file for class lmGlyph
    @ingroup score_kernel
*/

#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Glyph.h"

// offset:  the y-axis offset from bitmap rectangle to the selection rectangle origin
//          (units: tenths of inter-line space)
// shift:   the y-axis offset from paper cursor position so that shape get correctly
//          positioned over a five-lines staff (units: tenths of inter-line space)
// height:  the height of the selection rectangle
//          units: tenths (tenths of inter-line space)

//the glyphs table
//IMPORTANT: The table inicialization values MUST be ordered following the
//           enum lmEGlyphIndex, defined in Glyph.h 
const lmGlyph aGlyphsInfo[] =
{
// notes and notheads (LeMus font)
    //                       sel rectangle
    //       Glyph   offset  shift   height 
    lmGlyph(_T('N'),  10,     55,     10,    _T("Whole note")) , 
    lmGlyph(_T('O'),  10,     55,     10,    _T("Half note")) , 
    lmGlyph(_T('P'),  10,     55,     10,    _T("Quarter note notehead")) , 
    lmGlyph(_T('L'),  10,     55,     10,    _T("Cross notehead")) ,
    lmGlyph(_T('V'),  30,     15,     40,    _T("Eight note, flag bottom")) , 
    lmGlyph(_T('Q'), -10,     25,     40,    _T("Eight note, flag top")) , 
    lmGlyph(_T('W'),  30,     15,     45,    _T("16th note, flag bottom")) , 
    lmGlyph(_T('R'), -10,     25,     40,    _T("16th note, flag top")) , 
    lmGlyph(_T('X'),  30,     15,     55,    _T("32nd note, flag bottom")) , 
    lmGlyph(_T('S'), -10,     20,     45,    _T("32nd note, flag top")) ,
    lmGlyph(_T('Y'),  30,     15,     60,    _T("64th note, flag bottom")) , 
    lmGlyph(_T('T'), -10,     20,     45,    _T("64th note, flag top")),

// rests (LeMus font)
    //                       sel rectangle
    //       Glyph   offset  shift   height 
//  lmGlyph(_T('?'), -20,     30,     5,     _T("Long rest")) ,     //larga
//  lmGlyph(_T('?'), -20,     30,     5,     _T("Breve rest")) ,    //breve, cuadrada
    lmGlyph(_T('{'), -20,     30,     5,     _T("Whole rest")) ,    //whole redonda
    lmGlyph(_T('z'), -20,     35,     5,     _T("Half rest")) ,     //half blanca
    lmGlyph(_T('y'), -15,     20,     30,    _T("Quarter rest")) ,  //quarter negra
    lmGlyph(_T('x'), -15,     28,     20,    _T("Eight rest")) ,    //eighth corchea
    lmGlyph(_T('w'),  -7,     18,     30,    _T("16th rest")) ,     //16th semicorchea
    lmGlyph(_T('v'),  -7,     10,     40,    _T("32nd rest")) ,     //32nd fusa
    lmGlyph(_T('u'), -25,     25,     40,    _T("64th rest")) ,     //64th semifusa
//  lmGlyph(_T('?'), -20,     30,     5,     _T("128th rest")) ,    //128th garrapatea
//  lmGlyph(_T('?'), -20,     30,     5,     _T("256th rest")) ,    //256th semigarrapatea

//flags for notes.
    //  shift and height info is no needed
    //
    //                       sel rectangle
    //       Glyph   offset  shift   height 
    lmGlyph(_T('o'), -58,      0,      0,    _T("Eight note flag down")) ,
    lmGlyph(_T('f'), -48,      0,      0,    _T("Eight note flag up")) , 
    lmGlyph(_T('n'), -55,      0,      0,    _T("16th note flag down")) ,
    lmGlyph(_T('e'), -48,      0,      0,    _T("16th note flag up")) , 
    lmGlyph(_T('m'), -50,      0,      0,    _T("32nd note flag down")) ,         
    lmGlyph(_T('d'), -48,      0,      0,    _T("32nd note flag up")) ,
    lmGlyph(_T('l'), -40,      0,      0,    _T("64th note flag down")) , 
    lmGlyph(_T('c'), -48,      0,      0,    _T("64th note flag up")),
    lmGlyph(_T('k'), -35,      0,      0,    _T("128th note flag down")) , 
    lmGlyph(_T('b'), -48,      0,      0,    _T("128th note flag up")) ,
    lmGlyph(_T('j'), -28,      0,      0,    _T("256th note flag down")) , 
    lmGlyph(_T('a'), -48,      0,      0,    _T("256th note flag up")),

//accidentals

    //                       sel rectangle
    //       Glyph   offset  shift   height 
    lmGlyph(_T('\''), 10,     45,     30,    _T("Natural accidental")) ,
    lmGlyph(_T('#'),  10,     45,     30,    _T("Sharp accidental")) ,
    lmGlyph(_T('%'),  10,     40,     27,    _T("Flat accidental")) ,
    lmGlyph(_T('$'),  10,     54,     11,    _T("Double sharp accidental")) ,
    lmGlyph(_T('&'),  10,     40,     27,    _T("Double flat accidental")) ,


};


