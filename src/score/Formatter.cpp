// RCS-ID: $Id: Formatter.cpp,v 1.3 2006/02/23 19:22:56 cecilios Exp $
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
/*! @file Formatter.cpp
    @brief Implementation file for class lmFormatter
    @ingroup score_kernel
*/
/*! @class lmFormatter 
    @ingroup score_kernel
    @brief An algorithm to render a score.

    lmFormatter is an abstract class to implement different algorithms to render a music score.
    As rendering algorithms tend to be complex it's desirable to keep them well-contained or
    competely independent of the music sheet representation. So to isolate a rendering algorithm
    I encapsulate rendering algoritms in objects, all of them a specialization of the generic
    class lmFormatter
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

#include "wx/debug.h"
#include "wx/list.h"
#include "Score.h"


lmFormatter::lmFormatter()
{
    //
}