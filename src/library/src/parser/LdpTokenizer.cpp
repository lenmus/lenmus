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

#include <sstream>
#include "LdpTokenizer.h"
#include "LdpReader.h"
#include "../global/StringType.h"

using namespace std;

namespace lenmus
{


//constants for comparations
const char_type chCR = _T('\x0d');
const char_type chLF = _T('\x0a');
const char_type chApostrophe = _T('\'');
const char_type chAsterisk = _T('*');
const char_type chBar = _T('|');
const char_type chCloseBracket = _T(']');
const char_type chCloseParenthesis = _T(')');
const char_type chColon = _T(':');
const char_type chComma = _T(',');
const char_type chDollar = _T('$');
const char_type chDot = _T('.');
const char_type chEqualSign = _T('=');
const char_type chGreaterSign = _T('>');
const char_type chLowerSign = _T('<');
const char_type chMinusSign = _T('-');
const char_type chOpenBracket = _T('[');
const char_type chOpenParenthesis = _T('(');
const char_type chPlusSign = _T('+');
const char_type chQuotes = _T('"');
const char_type chSharp = _T('#');
const char_type chSlash = _T('/');
const char_type chSpace = _T(' ');
const char_type chTab = _T('\t');
const char_type chUnderscore = _T('_');


const char_type nEOF = EOF;         //End Of File


//-------------------------------------------------------------------------------------------
// Implementation of class LdpTokenizer
//-------------------------------------------------------------------------------------------

//These methods perform an analysis at character level to form tokens.
//If during the analyisis the char sequence "//" is found the remaining chars until
//end of line are ignoerd, including both "//" chars. An then analyisis continues as
//if all those ignored chars never existed.

LdpTokenizer::LdpTokenizer(LdpReader& reader, tostream& reporter)
    : m_reader(reader)
    , m_reporter(reporter)
    , m_pToken(NULL)
    , m_repeatToken(false)
    //to deal with compact notation [ name:value --> (name value) ]
    , m_expectingEndOfElement(false)
    , m_expectingNamePart(false)
    , m_expectingValuePart(false)
{
}

LdpTokenizer::~LdpTokenizer()
{
    if (m_pToken)
        delete m_pToken;
}

LdpToken* LdpTokenizer::read_token()
{

    if (m_repeatToken)
    {
        m_repeatToken = false;
        return m_pToken;
    }

    if (m_pToken)
        delete m_pToken;

    // To deal with compact notation [ name:value --> (name value) ]
    if (m_expectingEndOfElement)
    {
        // when flag 'm_expectingEndOfElement' is set it implies that the 'value' part was
        // the last returned token. Therefore, the next token to return is an implicit ')'
        m_expectingEndOfElement = false;
        //m_token.Set(tkEndOfElement, chCloseParenthesis);
        m_pToken = new LdpToken(tkEndOfElement, chCloseParenthesis);
        return m_pToken;
    }
    if (m_expectingNamePart)
    {
        // when flag 'm_expectingNamePart' is set this implies that last returned token
        // was an implicit '(' and that the real token (the 'name' part of an element
        // written in compact notation) is pending and must be returned now
        m_expectingNamePart = false;
        m_expectingValuePart = true;
        m_pToken = m_pTokenNamePart;
        return m_pToken;
    }
    if (m_expectingValuePart)
    {
        //next token is the 'value' part. Set flag to indicate that after the value
        //part an implicit 'end of element' must be issued
        m_expectingValuePart = false;
        m_expectingEndOfElement = true;
    }

    // loop until a token is found
    while(true)
    {
        if (m_reader.end_of_data())
        {
            //m_token.Set(tkEndOfFile, _T(""));
            m_pToken = new LdpToken(tkEndOfFile, _T(""));
            return m_pToken;
        }

        m_pToken = parse_new_token();

        //filter out tokens of type 'spaces' and 'comment' to optimize. 
        if (m_pToken->get_type() != tkSpaces && m_pToken->get_type() != tkComment)
            return m_pToken;
    }

}

LdpToken* LdpTokenizer::parse_new_token()
{
    //Finite automata for parsing LDP tokens

    enum EAutomataState {
        k_Start,
        k_CMT01,
        k_CMT02,
        k_ETQ01,
        k_ETQ02,
        k_ETQ03,
        k_NUM01,
        k_NUM02,
        k_SPC01,
        k_STR00,
        k_STR01,
        k_STR02,
        k_STR03,
        k_STR04,
        k_S01,
        k_S02,
        k_S03,
        k_Error
    };

    EAutomataState state = k_Start;
    tstringstream tokendata;
    char_type curChar;

    while (true)
    {
        switch(state)
        {
            case k_Start:
                curChar = m_reader.get_next_char();
                if (is_letter(curChar)
                    || curChar == chOpenBracket
                    || curChar == chBar
                    || curChar == chColon
                    || curChar == chAsterisk
                    || curChar == chSharp )
                {
                    state = k_ETQ01;
                }
                else if (curChar == chApostrophe) {
                    state = k_ETQ02;
                }
                else if (curChar == chUnderscore) {
                    state = k_ETQ03;
                }
                else if (is_number(curChar)) {
                    state = k_NUM01;
                }
                else {
                    switch (curChar)
                    {
                        case chOpenParenthesis:
                            //m_token.Set(tkStartOfElement, chOpenParenthesis);
                            return new LdpToken(tkStartOfElement, chOpenParenthesis);
                        case chCloseParenthesis:
                            //m_token.Set(tkEndOfElement, chCloseParenthesis);
                            return new LdpToken(tkEndOfElement, chCloseParenthesis);
                        case chSpace:
                            state = k_SPC01;
                            break;
                        case chSlash:
                            state = k_CMT01;
                            break;
                        case chPlusSign:
                        case chMinusSign:
                            state = k_S01;
                            break;
                        case chEqualSign:
                            state = k_S02;
                            break;
                        case chQuotes:
                            state = k_STR00;
                            break;
                        case chApostrophe:
                            state = k_STR02;
                            break;
                        case nEOF:
                            //m_token.Set(tkEndOfFile, _T(""));
                            return new LdpToken(tkEndOfFile, _T(""));
                        case chLF:
                            //m_token.Set(tkSpaces, chSpace);
                            return new LdpToken(tkSpaces, chSpace);
                        case chComma:
                            state = k_Error;
                            break;
                        default:
                            state = k_Error;
                    }
                }
                break;

            case k_ETQ01:
                tokendata << curChar;
                curChar = m_reader.get_next_char();
                if (is_letter(curChar) || is_number(curChar) ||
                    curChar == chUnderscore || curChar == chDot ||
                    curChar == chPlusSign || curChar == chMinusSign ||
                    curChar == chSharp || curChar == chSlash ||
                    curChar == chEqualSign || curChar == chApostrophe ||
                    curChar == chCloseBracket || curChar == chBar )
                {
                    state = k_ETQ01;
                }
                else if (curChar == chColon) {
                    // compact notation [ name:value --> (name value) ]
                    // 'name' part is parsed and we've found the ':' sign
                    m_expectingNamePart = true;
                    //m_pTokenNamePart.Set(tkLabel, extract(iStart, m_lastPos-1));
                    m_pTokenNamePart = new LdpToken(tkLabel, tokendata.str());
                    //m_token.Set(tkStartOfElement, chOpenParenthesis);
                    return new LdpToken(tkStartOfElement, chOpenParenthesis);
                }
                else {
                    //m_lastPos = m_lastPos - 1;     //repeat last char
                    m_reader.repeat_last_char();
                    //m_token.Set(tkLabel, extract(iStart, m_lastPos) );
                    return new LdpToken(tkLabel, tokendata.str());
                }
                break;

            case k_ETQ02:
                curChar = m_reader.get_next_char();
                if (curChar == chApostrophe) {
                    state = k_STR04;
                } else {
                    state = k_ETQ01;
                }
                break;

            case k_ETQ03:
                curChar = m_reader.get_next_char();
                if (curChar == chApostrophe)
                    state = k_ETQ02;
                else if (curChar == chQuotes)
                    state = k_STR00;                
                else
                    state = k_Error;
                break;

            case k_STR00:
                curChar = m_reader.get_next_char();
                if (curChar == chQuotes) {
                    //m_token.Set(tkString, extract(iStart + 1, m_lastPos - 1) );
                    return new LdpToken(tkString, tokendata.str());
                } else {
                    if (curChar == nEOF) {
                        state = k_Error;
                    } else {
                        state = k_STR01;
                    }
                }
                break;

            case k_STR01:
                tokendata << curChar;
                curChar = m_reader.get_next_char();
                if (curChar == chQuotes) {
                    //m_token.Set(tkString, extract(iStart + 1, m_lastPos - 1) );
                    return new LdpToken(tkString, tokendata.str());
                } else {
                    if (curChar == nEOF) {
                        state = k_Error;
                    } else {
                        state = k_STR01;
                    }
                }
                break;

            case k_STR04:
                curChar = m_reader.get_next_char();
                if (curChar == nEOF) {
                    state = k_Error;
                } else {
                    state = k_STR02;
                }
                break;

            case k_STR02:
                tokendata << curChar;
                curChar = m_reader.get_next_char();
                if (curChar == chApostrophe) {
                    state = k_STR03;
                } else {
                    state = k_STR02;
                }
                break;

            case k_STR03:
                curChar = m_reader.get_next_char();
                if (curChar == chApostrophe) {
                    //m_token.Set(tkString, extract(iStart + 2, m_lastPos - 2) );
                    return new LdpToken(tkString, tokendata.str());
                } else {
                    state = k_STR02;
                }
                break;

            case k_CMT01:
                tokendata << curChar;
                curChar = m_reader.get_next_char();
                if (curChar == chSlash)
                    state = k_CMT02;
                else
                    state = k_Error;
                break;

            case k_CMT02:
                tokendata << curChar;
                curChar = m_reader.get_next_char();
                if (curChar == chLF) {
                    //m_token.Set(tkComment, extract(iStart, m_lastPos - 1) );
                    return new LdpToken(tkComment, tokendata.str());
                }                //else continue in this state
                break;

            case k_NUM01:
                tokendata << curChar;
                curChar = m_reader.get_next_char();
                if (is_number(curChar)) {
                    state = k_NUM01;
                } else if (curChar == chDot) {
                    state = k_NUM02;
                } else if (is_letter(curChar)) {
                    state = k_ETQ01;
                } else {
                    //m_lastPos = m_lastPos - 1;     //repeat last char
                   m_reader.repeat_last_char();
                    //m_token.Set(tkIntegerNumber, extract(iStart, m_lastPos) );
                    return new LdpToken(tkIntegerNumber, tokendata.str());
                }
                break;

            case k_NUM02:
                tokendata << curChar;
                curChar = m_reader.get_next_char();
                if (is_number(curChar)) {
                    state = k_NUM02;
                } else {
                    //m_lastPos = m_lastPos - 1;     //repeat last char
                    m_reader.repeat_last_char();
                    //m_token.Set(tkRealNumber, extract(iStart, m_lastPos) );
                    return new LdpToken(tkRealNumber, tokendata.str());
                }
                break;

            case k_SPC01:
                curChar = m_reader.get_next_char();
                if (curChar == chSpace || curChar == chTab) {
                    state = k_SPC01;
                } else {
                    //m_lastPos = m_lastPos - 1;     //repeat last char
                    m_reader.repeat_last_char();
                    //m_token.Set(tkSpaces, chSpace );
                    return new LdpToken(tkSpaces, chSpace );
                }
                break;

            case k_S01:
                tokendata << curChar;
                curChar = m_reader.get_next_char();
                if (is_letter(curChar)) {
                    state = k_ETQ01;
                } else if (is_number(curChar)) {
                    state = k_NUM01;
                } else if (curChar == chPlusSign || curChar == chMinusSign) {
                    state = k_S03;
                } else if (curChar == chSpace || curChar == chTab) {
                    //m_token.Set(tkLabel, extract(iStart, m_lastPos-1) );
                    return new LdpToken(tkLabel, tokendata.str());
                } else {
                    state = k_Error;
                }
                break;

            case k_S02:
                tokendata << curChar;
                curChar = m_reader.get_next_char();
                if (is_letter(curChar)) {
                    state = k_ETQ01;
                } else if (is_number(curChar)) {
                    state = k_NUM01;
                } else if (curChar == chPlusSign || curChar == chMinusSign) {
                    state = k_S03;
                } else {
                    state = k_Error;
                }
                break;

            case k_S03:
                tokendata << curChar;
                curChar = m_reader.get_next_char();
                if (is_letter(curChar)) {
                    state = k_ETQ01;
                } else if (is_number(curChar)) {
                    state = k_NUM01;
                } else {
                    state = k_Error;
                }
                break;

            case k_Error:
                if (curChar == nEOF) {
                    //m_token.Set(tkEndOfFile, _T("" );
                    return new LdpToken(tkEndOfFile, _T("") );
                } else {
                    m_reporter << _T("[LdpTokenizer::parse_new_token]: Bad character '") 
                               << curChar << _T("' found") << endl;
                throw _T("Invalid char");
                }
                state = k_Start;
                break;

            default:
                throw _T("Invalid state");

        } // switch
    } // while loop
}

bool LdpTokenizer::is_letter(char_type ch)
{
    static const string_type letters = _T("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    return (letters.find(ch) != string::npos);
}

bool LdpTokenizer::is_number(char_type ch)
{
    static const string_type numbers = _T("0123456789");
    return (numbers.find(ch) != string::npos);
}


}  //namespace lenmus

