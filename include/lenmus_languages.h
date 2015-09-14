//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2015 LenMus project
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
//---------------------------------------------------------------------------------------

#ifndef __LENMUS_LANGUAGES_H__
#define __LENMUS_LANGUAGES_H__

//lenmus
#include "lenmus_standard_header.h"

//wxWidgets
#include <wx/string.h>
#include <wx/list.h>
#include <wx/arrstr.h>

namespace lenmus
{

void GetLanguages(wxArrayString &langCodes, wxArrayString &langNames);
wxString GetSystemLanguageCode();


}   // namespace lenmus

#endif // __LENMUS_LANGUAGES_H__
