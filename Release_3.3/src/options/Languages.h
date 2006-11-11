// RCS-ID: $Id: Languages.h,v 1.3 2006/02/23 19:22:19 cecilios Exp $
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
/*! @file Languages.h
    @brief Header file for global functions related to language
    @ingroup options_management
*/
#ifndef __LANGUAGES_H__
#define __LANGUAGES_H__

#include <wx/string.h>
#include <wx/list.h>

void GetLanguages(wxArrayString &langCodes, wxArrayString &langNames);
wxString GetSystemLanguageCode();

#endif // __LANGUAGES_H__
