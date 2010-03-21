//--------------------------------------------------------------------------------------
//  LenMus Library
//  Copyright (c) 2010 LenMus project
//
//  This program is free software; you can redistribute it and/or modify it under the 
//  terms of the GNU General Public License as published by the Free Software Foundation,
//  either version 3 of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//  PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License along
//  with this library; if not, see <http://www.gnu.org/licenses/> or write to the
//  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
//  MA  02111-1307,  USA.
//
//  For any comment, suggestion or feature request, please contact the manager of
//  the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

#ifndef __LM_STRINGTYPE_H__
#define __LM_STRINGTYPE_H__

#include <string>

//==================================================================
// 
// Char and String type dependent part
//
//  By default, the library is built using wide-character representation
//  (wchar_t' and sdt::wstring) as basic types. This simplifies parsing
//  of LDP files as they could contain texts in many different
//  languages. By using wide-character representation, the number of 
//  bytes used to represent a character is always the same, independently
//  of the character being represented. On the contrary, by using a multibyte
//  representation the number of bytes used for a character is variable
//  and this would complicate parsing.
//  
//  This typedefs allows you to build the library experimenting with
//  char/string types.
//
//  http://www.cplusplus.com/forum/windows/9797/
//  http://www.linux.com/archive/feed/51836
// 
//==================================================================

namespace lenmus 
{

#if 1    //#ifdef LM_USE_STRING
    typedef char char_type;
    typedef std::string string_type;
    #define tcout cout
    #define tostream ostream
    #define iftstream ifstream
    #define tstringstream stringstream
    #define _T(str) str
    #define _(str) gettext(str)

#else
    typedef wchar_t char_type;
    typedef std::wstring string_type;
    #define wcout tcout
    #define tostream wostream
    #define iftstream ifwstream
    #define tstringstream wstringstream
    #define _T(str) L##str
    #define _(str) gettext(Lstr)

#endif


#if 0
class wruntime_error : public std::runtime_error
{
public:
    wruntime_error( const std::wstring& msg ) ;
    virtual ~wruntime_error() ;

    // copy/assignment:
    wruntime_error( const wruntime_error& e ) ;
    wruntime_error& operator=( const wruntime_error& e ) ;

    // exception methods:
    const std::wstring& msg() const ;

private: 
    std::wstring    m_errorMsg ;
} ;
#endif


}

#endif	// __LM_STRINGTYPE_H__

