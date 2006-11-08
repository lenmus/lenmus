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
/*! @file global.h
    @brief Some useful definitions and global includes
    @ingroup app_gui
*/

#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __GLOBAL_H__        //to avoid nested includes
#define __GLOBAL_H__

#include "wx/colour.h"

//colors
#include "../globals/Colors.h"
extern lmColors* g_pColors;

// defined in lmScoreView.cpp
extern bool gfDrawSelRec;        //draw selection rectangles around staff objects

//some definitions for canvas->draw methods
#define DO_MEASURE true
#define DO_DRAW false
#define HIGHLIGHT true
#define NO_HIGHLIGHT true

// useful macros and definitions
#define Min(x, y)  ((x < y) ? x : y)
#define Max(x, y)  ((x > y) ? x : y)


//------------------------------------------
//Conditional compilation options
//------------------------------------------

// MDI style: Windows or Notebook
// 0 for Windows MDI style, 1 for Notebook MDI style
#define lmUSE_NOTEBOOK_MDI          0

// eBooks format: MS Windows Help format (htb) or LenMus format (lmb)
#define lmUSE_LENMUS_EBOOK_FORMAT   0


#endif    // __GLOBAL_H__
