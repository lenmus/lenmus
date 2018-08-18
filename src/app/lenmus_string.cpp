//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2018 LenMus project
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

#include "lenmus_string.h"


namespace lenmus
{

//=======================================================================================
// Conversion wxString <-> std::string
//=======================================================================================

wxString to_wx_string(const std::string& string)
{
    return wxString( string.c_str(), wxConvUTF8 );
}

std::string to_std_string(const wxString& wxstring)
{
    return std::string( wxstring.mb_str(wxConvUTF8) );
}




}   //namespace lenmus
