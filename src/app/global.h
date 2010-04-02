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

#ifndef __LM_GLOBAL_H__        //to avoid nested includes
#define __LM_GLOBAL_H__

#include "wx/colour.h"

//colors
#include "../globals/Colors.h"
extern lmColors* g_pColors;

//some definitions for canvas->draw methods
#define DO_MEASURE true
#define DO_DRAW false
#define HIGHLIGHT true
#define NO_HIGHLIGHT true

// useful macros and definitions
#define Min(x, y)  ((x < y) ? x : y)
#define Max(x, y)  ((x > y) ? x : y)




#endif    // __LM_GLOBAL_H__
