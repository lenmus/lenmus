//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "Glyph.h"
#endif

// For compilers that support precompilation, includes <wx/wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "Glyph.h"

// offset:  the y-axis offset from paper cursor position so that shape get correctly
//          positioned over a five-lines staff (units: tenths of inter-line space)
// shift:   the y-axis offset from bitmap rectangle to the selection rectangle origin
//          (units: tenths of inter-line space)
// height:  the height of the selection rectangle
//          units: tenths (tenths of inter-line space)

lmGlyph::lmGlyph(const wxChar glyph, int yOffset, int yShift, int selHeight, int top, int bottom, 
		         int xPos, int yPos, int width, int height, int xDrag, int yDrag)
    : GlyphChar(glyph)
    , GlyphOffset(yOffset)
    , Top(top)
    , Bottom(bottom)
{
    // Table data is in FUnits but as 512 FU are 1 line (10 tenths) it is simple
    // to convert these data into tenths: just divide FU by 51.2
	thxPos = xPos / 51.2;
	thyPos = yPos / 51.2;
	thWidth = width / 51.2;
	thHeight = height / 51.2;
	txDrag = xDrag / 51.2;
	tyDrag = yDrag / 51.2;

	if (yShift == 0 && selHeight == 0)
    {
        //not specified. Use full glyph rectangle
		SelRectShift = (int)(((3072.0 -(float)(height + yPos)) / 51.2) + 0.5);
		SelRectHeight = (int)(((float)(height) / 51.2) + 0.5);
	}
	else
    {
        //use specified rectangle
		SelRectHeight = selHeight;
		SelRectShift = yShift; 
	}
}

//the glyphs table
//IMPORTANT: The table inicialization values MUST be ordered following the
//           enum lmEGlyphIndex, defined in Glyph.h
const lmGlyph aGlyphsInfo[] =
{
// notheads
	//                                                      ----- FUnits ------------      -- FUnits--
    //                       sel rectangle                  Position    Size                drag point
    //       Glyph   offset  shift   height                 x       y   width   height      x       y
    lmGlyph(_T('O'),  10,     55,     10,   0,      0,      0,      0,      0,      0,      0,      0 ),    //_T("Half note")) ,
    lmGlyph(_T('P'),  10,     55,     10,   0,      0,      0,      0,      0,      0,      0,      0 ),    //_T("Quarter note notehead")) ,
    lmGlyph(_T('L'),  10,     55,     10,   0,      0,      0,      0,      0,      0,      0,      0 ),    //_T("Cross notehead")) ,

//notes with stem and flag, in single char
	//                                                           ----- FUnits ------------      -- FUnits--
    //                            sel rectangle                  Position    Size                drag point
    //            Glyph   offset  shift   height                 x       y   width   height      x       y
    lmGlyph(_T('\uF04B'),   0,      0,      0,   0,      0,      0,   -369,    960,    738,    480,    369 ),    //_T("Longa note")) ,
    lmGlyph(_T('\uF04D'),   0,      0,      0,   0,      0,      0,   -283,   1182,    567,    591,    283 ),    //_T("Breve note")) ,
    lmGlyph(_T('\uF04E'),  10,      0,      0,   0,      0,      0,   -282,   1010,    566,    505,    283 ),    //_T("Whole note")) ,
    lmGlyph(_T('\uF086'),   0,      0,      0,   0,      0,      0,  -1847,    709,   2127,    354,    279 ),    //_T("Half note, stem down")) ,
    lmGlyph(_T('\uF087'),   0,      0,      0,   0,      0,      0,   -279,    709,   2171,    354,   1923 ),    //_T("Half note, stem up")) ,
    lmGlyph(_T('\uF088'),   0,      0,      0,   0,      0,      0,  -1779,    640,   2027,    320,    248 ),    //_T("Quarter note, stem down")) ,
    lmGlyph(_T('\uF089'),   0,      0,      0,   0,      0,      0,   -248,    640,   2027,    320,   1779 ),    //_T("Quarter note, stem up")) ,
    lmGlyph(_T('\uF08A'),   0,      0,      0,   0,      0,      0,  -1782,    640,   2030,    320,    248 ),    //_T("Eight note, flag bottom")) ,
    lmGlyph(_T('\uF08B'),   0,      0,      0,   0,      0,      0,   -248,   1068,   2027,    320,   1779 ),    //_T("Eight note, flag top")) ,
    lmGlyph(_T('\uF08C'),   0,      0,      0,   0,      0,      0,  -1779,    640,   2027,    320,    248 ),    //_T("16th note, flag bottom")) ,
    lmGlyph(_T('\uF08D'),   0,      0,      0,   0,      0,      0,   -248,   1070,   2027,    320,   1779 ),    //_T("16th note, flag top")) ,
    lmGlyph(_T('\uF08E'),   0,      0,      0,   0,      0,      0,  -2241,    640,   2489,    320,    248 ),    //_T("32nd note, flag bottom")) ,
    lmGlyph(_T('\uF08F'),   0,      0,      0,   0,      0,      0,   -248,   1071,   2439,    320,   2191 ),    //_T("32nd note, flag top")) ,
    lmGlyph(_T('\uF090'),   0,      0,      0,   0,      0,      0,  -2508,    640,   2756,    320,    248 ),    //_T("64th note, flag bottom")) ,
    lmGlyph(_T('\uF091'),   0,      0,      0,   0,      0,      0,   -248,   1067,   2954,    320,   2706 ),    //_T("64th note, flag top")),
    lmGlyph(_T('\uF092'),   0,      0,      0,   0,      0,      0,  -2735,    640,   2983,    320,    248 ),    //_T("128th note, flag bottom")) ,
    lmGlyph(_T('\uF093'),   0,      0,      0,   0,      0,      0,   -248,   1067,   3189,    320,   2941 ),    //_T("128th note, flag top")),
    lmGlyph(_T('\uF094'),   0,      0,      0,   0,      0,      0,  -3132,    640,   3380,    320,    248 ),    //_T("256th note, flag bottom")) ,
    lmGlyph(_T('\uF095'),   0,      0,      0,   0,      0,      0,   -248,   1093,   3578,    320,   3330 ),    //_T("256th note, flag top")),

// rests
	//                                                      ----- FUnits ------------      -- FUnits--
    //                       sel rectangle                  Position    Size                drag point
    //       Glyph   offset  shift   height                 x       y   width   height      x       y
    lmGlyph(_T('}'), -20,      0,     0,    0,      0,      0,    512,    288,   1024,    144,      0 ),    //_T("Longa rest")) ,     //larga
    lmGlyph(_T('|'), -20,      0,     0,    0,      0,      0,   1024,    307,    512,    154,      0 ),    //_T("Breve rest")) ,    //breve, cuadrada
    lmGlyph(_T('{'), -20,      0,     0,    0,      0,      0,   1280,    720,    256,    360,      0 ),    //_T("Whole rest")) ,    //whole redonda
    lmGlyph(_T('z'), -20,      0,     0,    0,      0,      0,   1024,    720,    256,    360,      0 ),    //_T("Half rest")) ,     //half blanca
    lmGlyph(_T('y'), -20,      0,     0,    0,      0,      0,    343,    510,   1362,    205,    975 ),    //_T("Quarter rest")) ,  //quarter negra
    lmGlyph(_T('x'), -20,      0,     0,    0,      0,      0,    512,    480,    888,    240,    700 ),    //_T("Eight rest")) ,    //eighth corchea
    lmGlyph(_T('w'), -20,      0,     0,    0,      0,      0,      0,    618,   1368,    309,    684 ),    //_T("16th rest")) ,     //16th semicorchea
    lmGlyph(_T('v'), -20,      0,     0,    0,      0,      0,      0,    726,   1848,    363,    924 ),    //_T("32nd rest")) ,     //32nd fusa
    lmGlyph(_T('u'), -20,      0,     0,    0,      0,      0,   -450,    792,   2322,    396,    936 ),    //_T("64th rest")) ,     //64th semifusa
    lmGlyph(_T('t'), -20,      0,     0,    0,      0,      0,   -512,    828,   2802,    414,   1145 ),    //_T("128th rest")) ,    //128th garrapatea
    lmGlyph(_T('s'), -20,      0,     0,    0,      0,      0,   -924,    924,   3288,    462,   1182 ),    //_T("256th rest")) ,    //256th semigarrapatea

//flags for notes.
    //  shift and height info is no needed
    //
	//                                                      ----- FUnits ------------      -- FUnits--
    //                       sel rectangle                  Position    Size                drag point
    //       Glyph   offset  shift   height                 x       y   width   height      x       y
    lmGlyph(_T('o'), -20,      1,     29,  2956, 1570,      0,      0,      0,      0,      0,      0 ),    //_T("Eight note flag down")) ,
    lmGlyph(_T('n'), -20,     10,     30,  2606, 1310,      0,   1024,    621,   1539,      0,      0 ),    //_T("16th note flag down")) ,
    lmGlyph(_T('m'), -20,     11,     39,  2586,  912,      0,    512,    618,   1994,      0,      0 ),    //_T("32nd note flag down")) ,
    lmGlyph(_T('l'), -20,     16,     44,  2435,  545,      0,      0,    621,   2250,      0,      0 ),    //_T("64th note flag down")) ,
    lmGlyph(_T('k'), -20,     12,     48,  2437,  180,      0,     -3,    621,   2483,      0,      0 ),    //_T("128th note flag down")) ,
    lmGlyph(_T('j'), -20,     14,     56,  2434, -180,      0,   -512,    621,   2875,      0,      0 ),    //_T("256th note flag down")) ,
    lmGlyph(_T('f'), -60,     50,     30,   508, -1043,   -70,  -1027,    498,   1540,      0,      0 ),    //_T("Eight note flag up")) ,
    lmGlyph(_T('e'), -60,     40,     35,   890, -945,    -69,   -779,    499,   1795,      0,      0 ),    //_T("16th note flag up")) ,
    lmGlyph(_T('d'), -60,     30,     43,  1120, -727,    -69,   -661,    498,   2197,      0,      0 ),    //_T("32nd note flag up")) ,
    lmGlyph(_T('c'), -60,     20,     53,  1541, -770,    -69,   -663,    497,   2711,      0,      0 ),    //_T("64th note flag up")),
    lmGlyph(_T('b'), -60,     15,     58,  1901, -756,    -69,   -628,    496,   2945,      0,      0 ),    //_T("128th note flag up")) ,
    lmGlyph(_T('a'), -60,     10,     65,  2280, -766,    -69,   -766,    523,   3332,      0,      0 ),    //_T("256th note flag up")),

//accidentals
	//                                                      ----- FUnits ------------      -- FUnits--
    //                       sel rectangle                  Position    Size                drag point
    //       Glyph   offset  shift   height                 x       y   width   height      x       y
    lmGlyph(_T('\''), 10,     45,     30,   0,      0,      0,      0,      0,      0,      0,      0 ),    //_T("Natural accidental")) ,
    lmGlyph(_T('#'),  10,     45,     30,   0,      0,      0,      0,      0,      0,      0,      0 ),    //_T("Sharp accidental")) ,
    lmGlyph(_T('%'),  10,     40,     27,   0,      0,      0,      0,      0,      0,      0,      0 ),    //_T("Flat accidental")) ,
    lmGlyph(_T('$'),  10,     54,     11,   0,      0,      0,      0,      0,      0,      0,      0 ),    //_T("Double sharp accidental")) ,
    lmGlyph(_T('&'),  10,     40,     27,   0,      0,      0,      0,      0,      0,      0,      0 ),    //_T("Double flat accidental")) ,


//clefs
	//                                                      ----- FUnits ------------      -- FUnits--
    //                       sel rectangle                  Position    Size                drag point
    //       Glyph   offset  shift   height                 x       y   width   height      x       y
    lmGlyph(_T('A'), -20,      0,      0,   0,      0,      0,   -853,   1243,   3606,    621,   1377 ),    //_T("G clef")) ,
    lmGlyph(_T('B'), -20,      0,      0,   0,      0,      0,    324,   1451,   1728,    725,   1026 ),    //_T("F clef")) ,
    lmGlyph(_T('D'), -20,     20,     35,   0,      0,      0,      0,      0,      0,      0,      0 ),    //_T("F clef ottava alta")) ,
    lmGlyph(_T('E'), -20,     20,     35,   0,      0,      0,      0,      0,      0,      0,      0 ),    //_T("F clef ottava bassa")) ,
    lmGlyph(_T('C'), -20,      0,      0,   0,      0,      0,      0,   1393,   2050,    681,   1025 ),    //_T("C clef")) ,
    lmGlyph(_T('G'), -19,     20,     35,   0,      0,      0,      0,      0,      0,      0,      0 ),    //_T("Percussion clef, block")) ,

//numbers for time signatures
	//                                                      ----- FUnits ------------      -- FUnits--
    //                       sel rectangle                  Position    Size                drag point
    //       Glyph   offset  shift   height                 x       y   width   height      x       y
    lmGlyph(_T('0'), -20,      0,      0,   0,      0,      0,     37,    702,    950,      0,      0 ),    //_T("Number 0")) ,
    lmGlyph(_T('1'), -20,      0,      0,   0,      0,      0,     37,    612,    950,      0,      0 ),    //_T("Number 1")) ,
    lmGlyph(_T('2'), -20,      0,      0,   0,      0,      0,     37,    702,    950,      0,      0 ),    //_T("Number 2")) ,
    lmGlyph(_T('3'), -20,      0,      0,   0,      0,      0,     37,    636,    950,      0,      0 ),    //_T("Number 3")) ,
    lmGlyph(_T('4'), -20,      0,      0,   0,      0,      0,     37,    762,    950,      0,      0 ),    //_T("Number 4")) ,
    lmGlyph(_T('5'), -20,      0,      0,   0,      0,      0,     37,    666,    950,      0,      0 ),    //_T("Number 5")) ,
    lmGlyph(_T('6'), -20,      0,      0,   0,      0,      0,     37,    654,    950,      0,      0 ),    //_T("Number 6")) ,
    lmGlyph(_T('7'), -20,      0,      0,   0,      0,      0,     37,    696,    950,      0,      0 ),    //_T("Number 7")) ,
    lmGlyph(_T('8'), -20,      0,      0,   0,      0,      0,     37,    714,    950,      0,      0 ),    //_T("Number 8")) ,
    lmGlyph(_T('9'), -20,      0,      0,   0,      0,      0,     37,    648,    950,      0,      0 ),    //_T("Number 9")) ,

//other for time signatures
    lmGlyph(_T('>'),   0,      0,      0,   0,      0,      0,     37,    702,    950,      0,      0 ),    //GLYPH_COMMON_TIME,
    lmGlyph(_T('?'),   0,      0,      0,   0,      0,      0,     37,    702,    950,      0,      0 ),    //GLYPH_CUT_TIME,

//signs
	//                                                      ----- FUnits ------------      -- FUnits--
    //                       sel rectangle                  Position    Size                drag point
    //       Glyph   offset  shift   height                 x       y   width   height      x       y
    lmGlyph(_T('.'), -60,     58,     5,   121,  -112,      0,      0,      0,      0,      0,      0 ),    //_T("Dot (for dotted notes)")) ,
    lmGlyph(_T('_'),   0,     33,    31,  1403,  -201,      0,      0,      0,      0,      0,      0 ),    //_T("small quarter note up (for metronome marks)")) ,
    lmGlyph(_T('`'),   0,     33,    31,  1403,  -201,      0,      0,      0,      0,      0,      0 ),    //_T("small dotted quarter note up")) ,
    lmGlyph(_T(']'),   0,     32,    32,  1432,  -205,      0,      0,      0,      0,      0,      0 ),    //_T("small eighth note up")) ,
    lmGlyph(_T('^'),   0,     32,    32,  1432,  -205,      0,      0,      0,      0,      0,      0 ),    //_T("small dotted eighth note up")) ,
    lmGlyph(_T('['),   0,      5,    32,   143,  -113,      0,      0,      0,      0,      0,      0 ),    //_T("small equal sign")) ,

    lmGlyph(_T(':'),   0,      0,      0,   0,      0,      0,      0,    469,    854,      0,      0 ),    //_T("V breath-mark")) ,
    lmGlyph(_T('+'),   0,      0,      0,   0,      0,      0,      0,      0,      0,      0,      0 ),    //_T("Da Capo sign")) ,
    lmGlyph(_T('*'),   0,      0,      0,   0,      0,      0,      0,      0,      0,      0,      0 ),    //_T("Dal Segno sign")) ,
    lmGlyph(_T('-'),   0,      0,      0,   0,      0,      0,      0,      0,      0,      0,      0 ),    //_T("Coda sign")) ,
    lmGlyph(_T(','),   0,      0,      0,   0,      0,      0,      0,      0,      0,      0,      0 ),    //_T("Segno sign")) ,
    lmGlyph(_T('>'),   0,      0,      0,   0,      0,      0,      0,      0,      0,      0,      0 ),    //_T("Octava sign")) ,
    lmGlyph(_T(';'),   0,      0,      0,   0,      0,      5,      8,   1359,    778,      0,      0 ),    //_T("Fermata over (arch)")) ,
    lmGlyph(_T('<'),   0,      0,      0,   0,      0,      0,   -796,   1359,    781,      0,      0 ),    //_T("Fermata under (arch)")) ,

//figured bass. Numbers and other symbols
	//                                                           ----- FUnits ------------      -- FUnits--
    //                            sel rectangle                  Position    Size                drag point
    //            Glyph   offset  shift   height                 x       y   width   height      x       y
    lmGlyph(_T('\uF09F'),   0,      0,      0,   0,      0,      0,      0,      0,      0,      0,      0 ),    //_T("Figured bass. Number 0")) ,
    lmGlyph(_T('\uF096'),   0,      0,      0,   0,      0,      0,      0,      0,      0,      0,      0 ),    //_T("Figured bass. Number 1")) ,
    lmGlyph(_T('\uF097'),   0,      0,      0,   0,      0,      0,      0,      0,      0,      0,      0 ),    //_T("Figured bass. Number 2")) ,
    lmGlyph(_T('\uF098'),   0,      0,      0,   0,      0,      0,      0,      0,      0,      0,      0 ),    //_T("Figured bass. Number 3")) ,
    lmGlyph(_T('\uF099'),   0,      0,      0,   0,      0,      0,      0,      0,      0,      0,      0 ),    //_T("Figured bass. Number 4")) ,
    lmGlyph(_T('\uF09A'),   0,      0,      0,   0,      0,      0,      0,      0,      0,      0,      0 ),    //_T("Figured bass. Number 5")) ,
    lmGlyph(_T('\uF09B'),   0,      0,      0,   0,      0,      0,      0,      0,      0,      0,      0 ),    //_T("Figured bass. Number 6")) ,
    lmGlyph(_T('\uF09C'),   0,      0,      0,   0,      0,      0,      0,      0,    664,      0,      0 ),    //_T("Figured bass. Number 7")) ,
    lmGlyph(_T('\uF09D'),   0,      0,      0,   0,      0,      0,      0,      0,      0,      0,      0 ),    //_T("Figured bass. Number 8")) ,
    lmGlyph(_T('\uF09E'),   0,      0,      0,   0,      0,      0,      0,      0,      0,      0,      0 ),    //_T("Figured bass. Number 9")) ,
    lmGlyph(_T('\uF0A0'),   0,      0,      0,   0,      0,      0,      0,      0,      0,      0,      0 ),    //_T("Figured bass. Sharp symbol")) ,
    lmGlyph(_T('\uF0A1'),   0,      0,      0,   0,      0,      0,      0,      0,      0,      0,      0 ),    //GLYPH_FIGURED_BASS_FLAT,                //Flat symbol
    lmGlyph(_T('\uF0A2'),   0,      0,      0,   0,      0,      0,      0,      0,      0,      0,      0 ),    //GLYPH_FIGURED_BASS_NATURAL,             //Natural symbol
    lmGlyph(_T('\uF0AB'),   0,      0,      0,   0,      0,      0,      0,      0,      0,      0,      0 ),    //GLYPH_FIGURED_BASS_PLUS,                // +
    lmGlyph(_T('\uF0AC'),   0,      0,      0,   0,      0,      0,      0,      0,      0,      0,      0 ),    //GLYPH_FIGURED_BASS_MINUS,               // -
    lmGlyph(_T('\uF0A8'),   0,      0,      0,   0,      0,      0,      0,      0,      0,      0,      0 ),    //GLYPH_FIGURED_BASS_OPEN_PARENTHESIS,    // (
    lmGlyph(_T('\uF0A3'),   0,      0,      0,   0,      0,      0,      0,      0,      0,      0,      0 ),    //GLYPH_FIGURED_BASS_CLOSE_PARENTHESIS,   // )
    lmGlyph(_T('\uF0AA'),   0,      0,      0,   0,      0,      0,      0,      0,      0,      0,      0 ),    //GLYPH_FIGURED_BASS_7_STRIKED,           // 7 with overlayered /
};

//em=2048
//ascender=3072
//descender=-1024
//tenths = 512

//512 FUnits = 10 tenths   -->  t = FU*10/512

//offset = -20 for objects pre-positioned on staff
//shift = ascender - top

//A,  G_CLEF, 0, 1243, 2753, -864,    <offset_to_2nd_line=0>
//shift = 3072- 2753 FU = 319FU = 6,26t = 6         (5,73)
//height = (2743 - -864)FU = 3607FU = 70,45 = 71    (6,71)

//B,  F_CLEF, 0, 1451, 2053, 324      <offset_to_4th_line=0>
//shift = 3072- 2053 FU = 1019FU = 19,90t = 20      (20,35)
//height = (2053 - 324)FU = 1729FU = 33,76 = 34     (20,34)

//C,  C_CLEF, 0, 1394, 2050, 0        <offset_to_3rd_line=0>
//shift = 3072- 2050 FU = 1022FU = 19,96t = 20      (20,40)
//height = (2050 - 0)FU = 2050FU = 40,03 = 40       (20,40)

//DOT, '.',0, 227, 121, -112
//shift = 3072- 121 FU = 2951FU = 57,63 = 58
//height = (121 + 112)FU = 233FU = 4,55 = 5

//small quarter note up (for metronome marks)
//       xPos   yPos    width   height  Height+yPos
//  '_', 0      -201    481     1604    1403
//shift = 3072 - 1403 = 1669 FU = 1669*10/512 = 32,60 = 33
//height = 1604; FU = 1604/51,2 = 31,33 = 31

// see: lenmus/fonts/font_metrics.xml



