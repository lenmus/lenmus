//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus projec
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "LDPToken.h"
#endif

// for (compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "LDPToken.h"
#include "LDPParser.h"



//constants for comparations
const wxChar chCR = _T('\x0d');
const wxChar chLF = _T('\x0a');
const wxChar chApostrophe = _T('\'');
const wxChar chAsterisk = _T('*');
const wxChar chBar = _T('|');
const wxChar chCloseBracket = _T(']');
const wxChar chCloseParenthesis = _T(')');
const wxChar chColon = _T(':');
const wxChar chComma = _T(',');
const wxChar chDollar = _T('$');
const wxChar chDot = _T('.');
const wxChar chEqualSign = _T('=');
const wxChar chGreaterSign = _T('>');
const wxChar chLowerSign = _T('<');
const wxChar chMinusSign = _T('-');
const wxChar chOpenBracket = _T('[');
const wxChar chOpenParenthesis = _T('(');
const wxChar chPlusSign = _T('+');
const wxChar chQuotes = _T('"');
const wxChar chSharp = _T('#');
const wxChar chSlash = _T('/');
const wxChar chSpace = _T(' ');
const wxChar chTab = _T('\t');
const wxChar chUnderscore = _T('_');


const wxChar nEOF = _T('\x03');     //ETX - To signal End Of File
const wxChar nEOL = _T('\x04');     //EOT - To signal End Of Line (end of buffer)

const wxString sLetters = _T("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
const wxString sNumbers = _T("0123456789");

const wxString sEOF = _T("<<$$EOF$$>>");

//-------------------------------------------------------------------------------------------
// Implementation of class lmLDPToken
//-------------------------------------------------------------------------------------------

lmLDPToken::lmLDPToken()
{
}

wxString lmLDPToken::GetDescription()
{
    switch (m_Type) {
        case tkLabel:
            return _T("Label");
        case tkEndOfElement:
            return _T("End of Element");
        case tkStartOfElement:
            return _T("Start of Element");
        case tkSpaces:
            return _T("Space");
        case tkIntegerNumber:
            return _T("Integer Number");
        case tkRealNumber:
            return _T("Real Number");
        case tkComment:
            return _T("Comment");
        case tkString:
            return _T("String");
        case tkEndOfFile:
            return _T("EOF");
        default:
            return wxString::Format(_T("Bad type <%d>"), m_Type);
            //wxASSERT(false);
    }

}


//-------------------------------------------------------------------------------------------
// Implementation of class lmLDPTokenBuilder
//-------------------------------------------------------------------------------------------

//These methods perform an analysis at character level to form tokens.
//If during the analyisis the char sequence "//" is found the remaining chars until
//end of line are ignoerd, including both "//" chars. An then analyisis continues as
//if all those ignored chars never existed.

lmLDPTokenBuilder::lmLDPTokenBuilder(lmLDPParser* pParser)
{
    m_pParser = pParser;
    m_fRepeatToken = false;
    m_sInBuf = _T("");            // no buffer read

    //to deal with compact notation [ name:value --> (name value) ]
    m_fEndOfElementPending = false;
    m_fNamePartPending = false;
    m_fValuePartPending = false;

    //to deal with abbreviated notation [ (n c4 q t3) --> nc4q,t3 ]
    m_fAbbreviated = false;
}

lmLDPTokenBuilder::~lmLDPTokenBuilder()
{
    //delete tokens in pending array
    int i = m_cPending.GetCount();
    for(; i > 0; i--)
    {
        delete m_cPending.Item(i-1);
        m_cPending.RemoveAt(i-1);
    }
}

void lmLDPTokenBuilder::PushToken(ETokenType nType, wxString sValue)
{
    //create a token and add it to the pending array
    lmLDPToken* pT = new lmLDPToken;
    pT->Set(nType, sValue);
    m_cPending.Add(pT);
}

bool lmLDPTokenBuilder::PopToken()
{
    //returns true if a token is returned
    int i = m_cPending.GetCount();
    if (i > 0)
    {
        m_token = *(m_cPending.Item(i-1));
        delete m_cPending.Item(i-1);
        m_cPending.RemoveAt(i-1);
        return true;
    }
    else
        return false;
}

lmLDPToken* lmLDPTokenBuilder::ReadToken()
{

    if (m_fRepeatToken)
    {
        m_fRepeatToken = false;
        return &m_token;
    }

    //return any pending token
    if (PopToken()) return &m_token;

    // To deal with compact notation [ name:value --> (name value) ]
    if (m_fEndOfElementPending)
    {
        // when flag 'm_fEndOfElementPending' is set it implies that the 'value' part was
        // the last returned token. Therefore, the next token to return is an implicit ')'
        m_fEndOfElementPending = false;
        m_token.Set(tkEndOfElement, chCloseParenthesis);
        return &m_token;
    }
    if (m_fNamePartPending)
    {
        // when flag 'm_fNamePartPending' is set this implies that last returned token
        // was an implicit '(' and that the real token (the 'name' part of an element
        // written in compact notation) is pending and must be returned now
        m_fNamePartPending = false;
        m_fValuePartPending = true;
        return &m_tokenNamePart;
    }
    if (m_fValuePartPending)
    {
        //next token is the 'value' part. Set flag to indicate that after the value
        //part an implicit 'end of element' must be issued
        m_fValuePartPending = false;
        m_fEndOfElementPending = true;
    }

    // loop until a token is found
    while(true)
    {
        if (m_sInBuf == _T("")) GetNewBuffer();

        //if end of file return
        if (m_sInBuf == sEOF ) {    //|| m_token.GetType() == tkEndOfFile) {
            m_token.Set(tkEndOfFile, _T(""));
            return &m_token;
        }

        ParseNewToken();

        if (m_token.GetType() != tkSpaces && m_token.GetType() != tkComment) return &m_token;
            // spaces are not an external token but an internal token to signal
            // a separator between tokens
            // Comments could be an external token but as they are not going to be treated
            // we filter them out in here for optimization
    }

}

void lmLDPTokenBuilder::GNC()
{
    //Returns the next character to analyze.
    //When entering this function m_lastPos points to the last character read

    if (m_lastPos < m_maxPos)
        m_curChar = m_sInBuf.GetChar(m_lastPos);
    else
        m_curChar = nEOL;

    //wxLogMessage(_T("[lmLDPTokenBuilder::GNC] char 0x%x"), m_curChar);

    //change Tabs and CR chars by Spaces
    if (m_curChar == chTab || m_curChar == chCR)
        m_curChar = chSpace;

    m_lastPos++;
}

void lmLDPTokenBuilder::GetNewBuffer()
{
    m_sInBuf = m_pParser->GetNewBuffer();
    m_maxPos = (long) m_sInBuf.length();
    m_lastPos = 0;

}

wxString lmLDPTokenBuilder::Extract(long iFrom, long iTo)
{
    // This auxiliary function is a wrapper for extracting a substring.
    // As all buffer indexes are referred to 1 this function helps to avoid stupid errors
    // that takes a lot of time to debug
    if (iTo >= iFrom) {
        return m_sInBuf.substr(iFrom-1, iTo - iFrom + 1);
    } else {
        return m_sInBuf.substr(iFrom-1);
    }

}

void lmLDPTokenBuilder::ParseNewToken()
{
    //at this point m_lastPos points to last read char
    int iStart;

    enum tkStatus {
        FT_Start,
        FT_CMT01,
        FT_CMT02,
        FT_EOF01,
        FT_EOF02,
        FT_EOF03,
        FT_EOF04,
        FT_EOF05,
        FT_EOF06,
        FT_EOF07,
        FT_EOF08,
        FT_EOF09,
        FT_EOF10,
        FT_ETQ01,
        FT_ETQ02,
        FT_ETQ03,
        FT_NUM01,
        FT_NUM02,
        FT_SPC01,
        FT_STR01,
        FT_STR02,
        FT_STR03,
        FT_S01,
        FT_S02,
        FT_S03,
        FT_Error
    };

    tkStatus nState = FT_Start;

    iStart = m_lastPos + 1;     //token starts in the first char not yet read
    while (true) {
        switch (nState) {
            case FT_Start:
                GNC();
                if (lmIsLetter(m_curChar)
                    || m_curChar == chOpenBracket || m_curChar == chBar
                    || m_curChar == chColon || m_curChar == chAsterisk
                    || m_curChar == chSharp )
                {
                    nState = FT_ETQ01;
                }
                else if (m_curChar == chApostrophe) {
                    nState = FT_ETQ02;
                }
                else if (m_curChar == chUnderscore) {
                    nState = FT_ETQ03;
                    iStart++;
                }
                else if (lmIsNumber(m_curChar)) {
                    nState = FT_NUM01;
                }
                else {
                    switch (m_curChar) {
                        case chOpenParenthesis:
                            m_token.Set(tkStartOfElement, chOpenParenthesis);
                            return;
                        case chCloseParenthesis:
                            m_token.Set(tkEndOfElement, chCloseParenthesis);
                            return;
                        case chSpace:
                            if (m_fAbbreviated) {
                                m_fAbbreviated = false;
                                m_token.Set(tkEndOfElement, chCloseParenthesis);
                                return;
                            }
                            nState = FT_SPC01;
                            break;
                        case chSlash:
                            nState = FT_CMT01;
                            break;
                        case chPlusSign:
                        case chMinusSign:
                            nState = FT_S01;
                            break;
                        case chEqualSign:
                            nState = FT_S02;
                            break;
                        case chQuotes:
                            nState = FT_STR01;
                            break;
                        case chApostrophe:
                            nState = FT_STR02;
                            break;
                        case nEOF:
                            m_token.Set(tkEndOfFile, _T(""));
                            return;
                        case nEOL:
                            m_token.Set(tkSpaces, chSpace);
                            GetNewBuffer();
                            return;
                        case chLF:
                            m_token.Set(tkSpaces, chSpace);
                            return;
                        case chLowerSign:
                            nState = FT_EOF01;
                            break;
                        case chComma:
                            if (m_fAbbreviated) {
                                m_token.Set(tkSpaces, chSpace);
                                return;
                            }
                            nState = FT_Error;
                            break;
                        default:
                            nState = FT_Error;
                    }
                }
                break;

            case FT_ETQ02:
                GNC();
                if (m_curChar == chApostrophe) {
                    nState = FT_STR02;
                } else {
                    nState = FT_ETQ01;
                }
                break;

            case FT_ETQ03:
                GNC();
                if (m_curChar == chApostrophe)
                    nState = FT_ETQ02;
                else if (m_curChar == chQuotes)
                    nState = FT_STR01;                
                else
                    nState = FT_Error;
                break;

            case FT_STR02:
                GNC();
                if (m_curChar == chApostrophe) {
                    nState = FT_STR03;
                } else {
                    nState = FT_STR02;
                }
                break;

            case FT_STR03:
                GNC();
                if (m_curChar == chApostrophe) {
                    m_token.Set(tkString, Extract(iStart + 2, m_lastPos - 2) );
                    return;
                } else {
                    nState = FT_STR02;
                }
                break;

            case FT_CMT01:
                GNC();
                if (m_curChar == chSlash)
                    nState = FT_CMT02;
                else
                    nState = FT_Error;
                break;

            case FT_CMT02:
                GNC();
                if (m_curChar == chLF) {
                    m_token.Set(tkComment, Extract(iStart, m_lastPos - 1) );
                    return;
                } else if (m_curChar == nEOL) {
                    m_token.Set(tkComment, Extract(iStart, m_lastPos - 1) );
                    GetNewBuffer();
                    return;
                }
                //else continue in this state
                break;

            case FT_EOF01:
                GNC();
                if (m_curChar == chLowerSign) {
                    nState = FT_EOF02;
                } else {
                    nState = FT_Error;
                }
                break;

            case FT_EOF02:
                GNC();
                if (m_curChar == chDollar) {
                    nState = FT_EOF03;
                } else {
                    nState = FT_Error;
                }
                break;

            case FT_EOF03:
                GNC();
                if (m_curChar == chDollar) {
                    nState = FT_EOF04;
                } else {
                    nState = FT_Error;
                }
                break;

            case FT_EOF04:
                GNC();
                if (m_curChar == _T('E')) {
                    nState = FT_EOF05;
                } else {
                    nState = FT_Error;
                }
                break;

            case FT_EOF05:
                GNC();
                if (m_curChar == _T('O')) {
                    nState = FT_EOF06;
                } else {
                    nState = FT_Error;
                }
                break;

            case FT_EOF06:
                GNC();
                if (m_curChar == _T('F')) {
                    nState = FT_EOF07;
                } else {
                    nState = FT_Error;
                }
                break;

            case FT_EOF07:
                GNC();
                if (m_curChar == chDollar) {
                    nState = FT_EOF08;
                } else {
                    nState = FT_Error;
                }
                break;

            case FT_EOF08:
                GNC();
                if (m_curChar == chDollar) {
                    nState = FT_EOF09;
                } else {
                    nState = FT_Error;
                }
                break;

            case FT_EOF09:
                GNC();
                if (m_curChar == chGreaterSign) {
                    nState = FT_EOF10;
                } else {
                    nState = FT_Error;
                }
                break;

            case FT_EOF10:
                GNC();
                if (m_curChar == chGreaterSign) {
                    m_token.Set(tkEndOfFile, _T("<<$$EOF$$>>") );
                    m_sInBuf = _T("");        //Flush buffer
                    return;
                } else {
                    nState = FT_Error;
                }
                break;

            case FT_ETQ01:
                GNC();
                if (lmIsLetter(m_curChar) || lmIsNumber(m_curChar) ||
                    m_curChar == chUnderscore || m_curChar == chDot ||
                    m_curChar == chPlusSign || m_curChar == chMinusSign ||
                    m_curChar == chSharp || m_curChar == chSlash ||
                    m_curChar == chEqualSign || m_curChar == chApostrophe ||
                    m_curChar == chCloseBracket || m_curChar == chBar )
                {
                    nState = FT_ETQ01;
                }
                else if (m_curChar == chColon) {
                    // compact notation [ name:value --> (name value) ]
                    // 'name' part is parsed and we've found the ':' sign
                    m_fNamePartPending = true;
                    m_tokenNamePart.Set(tkLabel, Extract(iStart, m_lastPos-1));
                    m_token.Set(tkStartOfElement, chOpenParenthesis);
                    return;
                }
                else if (m_curChar == chComma) {
                    //abbreviated notation [ (n c4 q t3) --> nc4q,t3 ]
                    if (!m_fAbbreviated) {
                        //found the first comma. Token is the previous string
                        m_fAbbreviated = true;
                        m_token.Set(tkStartOfElement, chOpenParenthesis);
                        PushToken(tkLabel, Extract(iStart, m_lastPos-1));
                        return;
                    }
                    else {
                        //it is not the first parameter, just return it
                        m_token.Set(tkLabel, Extract(iStart, m_lastPos-1) );
                        return;
                    }
               }
                else {
                    m_lastPos = m_lastPos - 1;     //repeat last char
                    m_token.Set(tkLabel, Extract(iStart, m_lastPos) );
                    return;
                }
                break;

            case FT_NUM01:
                GNC();
                if (lmIsNumber(m_curChar)) {
                    nState = FT_NUM01;
                } else if (m_curChar == chDot) {
                    nState = FT_NUM02;
                } else if (lmIsLetter(m_curChar)) {
                    nState = FT_ETQ01;
                } else {
                    m_lastPos = m_lastPos - 1;     //repeat last char
                    m_token.Set(tkIntegerNumber, Extract(iStart, m_lastPos) );
                    return;
                }
                break;

            case FT_NUM02:
                GNC();
                if (lmIsNumber(m_curChar)) {
                    nState = FT_NUM02;
                } else {
                    m_lastPos = m_lastPos - 1;     //repeat last char
                    m_token.Set(tkRealNumber, Extract(iStart, m_lastPos) );
                    return;
                }
                break;

            case FT_SPC01:
                GNC();
                if (m_curChar == chSpace || m_curChar == chTab) {
                    nState = FT_SPC01;
                } else {
                    m_lastPos = m_lastPos - 1;     //repeat last char
                    m_token.Set(tkSpaces, chSpace );
                    return;
                }
                break;

            case FT_STR01:
                GNC();
                if (m_curChar == chQuotes) {
                    m_token.Set(tkString, Extract(iStart + 1, m_lastPos - 1) );
                    return;
                } else {
                    if (m_curChar == nEOF || m_curChar == nEOL) {
                        nState = FT_Error;
                    } else {
                        nState = FT_STR01;
                    }
                }
                break;

            case FT_S01:
                GNC();
                if (lmIsLetter(m_curChar)) {
                    nState = FT_ETQ01;
                } else if (lmIsNumber(m_curChar)) {
                    nState = FT_NUM01;
                } else if (m_curChar == chPlusSign || m_curChar == chMinusSign) {
                    nState = FT_S03;
                } else if (m_curChar == chSpace || m_curChar == chTab) {
                    m_token.Set(tkLabel, Extract(iStart, m_lastPos-1) );
                    return;
                } else {
                    nState = FT_Error;
                }
                break;

            case FT_S02:
                GNC();
                if (lmIsLetter(m_curChar)) {
                    nState = FT_ETQ01;
                } else if (lmIsNumber(m_curChar)) {
                    nState = FT_NUM01;
                } else if (m_curChar == chPlusSign || m_curChar == chMinusSign) {
                    nState = FT_S03;
                } else {
                    nState = FT_Error;
                }
                break;

            case FT_S03:
                GNC();
                if (lmIsLetter(m_curChar)) {
                    nState = FT_ETQ01;
                } else if (lmIsNumber(m_curChar)) {
                    nState = FT_NUM01;
                } else {
                    nState = FT_Error;
                }
                break;

            case FT_Error:
                if (m_curChar == nEOF) {
                    m_token.Set(tkEndOfFile, _T("") );
                    return;
                } else if (m_curChar == nEOL) {
                    m_pParser->ParseMsje(wxString::Format(_T("[lmLDPTokenBuilder::ParseNewToken.%d]: EOL found"),
                        nState));
                } else {
                    m_pParser->ParseMsje(wxString::Format(_T("[lmLDPTokenBuilder::ParseNewToken]: Bad character %d found. File %s"),
                        m_curChar, m_pParser->GetFilename().c_str() ));
                }
                nState = FT_Start;
                break;

            default:
                wxASSERT(false);
                return;

        } // switch
    } // while loop

}


//------------------------------------------------------------------------------------------
// Global functions
//------------------------------------------------------------------------------------------

bool lmIsLetter(wxChar ch)
{
    return (sLetters.Find(ch) != -1);
}

bool lmIsNumber(wxChar ch)
{
    return (sNumbers.Find(ch) != -1);
}

