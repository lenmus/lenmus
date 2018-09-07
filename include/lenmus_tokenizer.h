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

#ifndef __LENMUS_TOKENIZER_H__
#define __LENMUS_TOKENIZER_H__

//lenmus
#include "lenmus_standard_header.h"

//other
#include <string>
#include <list>
using namespace std;

namespace lenmus
{


/** A very fast an efficient tokenizer for strings, taken from:
    https://stackoverflow.com/questions/236129/the-most-elegant-way-to-iterate-the-words-of-a-string

    I usually choose to use std::vector<std::string> types as my second parameter
    (ContainerT)... but list<> is way faster than vector<> for when direct access is not
    needed.

    It's more than double as fast as the fastest tokenizer on this post and almost 5 times
    faster than some others. Also with the perfect parameter types you can eliminate
    all string and list copies for additional speed increases.

    Additionally it does not do the (extremely inefficient) return of result, but rather
    it passes the tokens as a reference, thus also allowing you to build up tokens using
    multiple calls if you so wished.

    Lastly it allows you to specify whether to trim empty tokens from the results via a
    last optional parameter.

    All it needs is std::string... the rest are optional. It does not use streams or the
    boost library, but is flexible enough to be able to accept some of these foreign
    types naturally.
*/
template < class ContainerT >
void tokenize(const std::string& str, ContainerT& tokens,
              const std::string& delimiters = " ", bool trimEmpty = false)
{
   std::string::size_type pos, lastPos = 0, length = str.length();

   using value_type = typename ContainerT::value_type;
   using size_type  = typename ContainerT::size_type;

   while(lastPos < length + 1)
   {
      pos = str.find_first_of(delimiters, lastPos);
      if(pos == std::string::npos)
      {
         pos = length;
      }

      if(pos != lastPos || !trimEmpty)
         tokens.push_back(value_type(str.data()+lastPos,
               (size_type)pos-lastPos ));

      lastPos = pos + 1;
   }
}

//=======================================================================================
// A simple replacement for old code using boost tokenizer
class Tokenizer
{
private:
    list<string> m_tokens;
    list<string>::const_iterator m_it;

public:
    Tokenizer() {}
    ~Tokenizer()
    {
        m_tokens.clear();
    }

    inline string initialize(const string& data, const string& seps)
    {
        tokenize< list<string> >(data, m_tokens, seps, true);
        m_it = m_tokens.begin();
        return get_next_token();
    }
    inline void operator++() { ++m_it; }
    inline string operator*() { return *m_it; }
    inline bool at_end() { return m_it == m_tokens.end(); }
    inline string get_next_token() { return (at_end() ? "" : *(m_it++)); }
};


}   // namespace lenmus

#endif    // __LENMUS_TOKENIZER_H__
