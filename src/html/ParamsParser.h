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
/*! @file ParamsParser.h
    @brief Header file for auxiliary global functions to parse objects' params
    @ingroup ldp_parser
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __PARAMSPARSER_H__        //to avoid nested includes
#define __PARAMSPARSER_H__

#include "../exercises/Constrains.h"    //KeyConstrains

extern wxString ParseKeys(wxString sParamValue, wxString sFullParam,
                          lmKeyConstrains* pKeys);
extern wxString ParseChords(wxString sParamValue, wxString sFullParam,
                            bool* m_fValidChords);


#endif    // __PARAMSPARSER_H__
