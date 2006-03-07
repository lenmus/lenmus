// RCS-ID: $Id: LDPToken.cpp,v 1.4 2006/02/23 19:21:45 cecilios Exp $
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
/*! @file LDPToken.cpp
    @brief Implementation file for class lmLDPToken
    @ingroup ldp_parser
*/
/*! @class lmLDPToken
    @ingroup ldp_parser
    @brief Methods to read and form a token

*/
#ifdef __GNUG__
// #pragma implementation
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



//constants for comparation
const wxChar chTab = _T('\t');
const wxChar chSpace = _T(' ');
const wxChar chApostrophe = _T('\'');
const wxChar chOpenParenthesis = _T('(');
const wxChar chCloseParenthesis = _T(')');
const wxChar chSlash = _T('/');
const wxChar chUnderscore = _T('_');
const wxChar chDot = _T('.');
const wxChar chPlusSign = _T('+');
const wxChar chMinusSign = _T('-');
const wxChar chEqualSign = _T('=');
const wxChar chQuotes = _T('"');
const wxChar chLowerSign = _T('<');
const wxChar chGreaterSign = _T('>');
const wxChar chDollar = _T('$');

const wxChar nEOF = _T('\x03');        //ETX
const wxChar nEOL = _T('\x04');        //EOT

const wxString sLetters = _T("ABCDEFGHIJKLMNÑOPQRSTUVWXYZabcdefghijklmnñopqrstuvwxyz");
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
            return _T("Etiqueta");
        case tkEndOfElement:
            return _T("Fin de Elemento");
        case tkStartOfElement:
            return _T("Inicio de Elemento");
        case tkSpaces:
            return _T("Espacio");
        case tkIntegerNumber:
            return _T("Número entero");
        case tkRealNumber:
            return _T("Número punto fijo");
        case tkComment:
            return _T("Comentario");
        case tkString:
            return _T("String");
        case tkEndOfFile:
            return _T("EOF");
        default:
            return wxString::Format(_T("TipoErroneo<%d>"), m_Type);
            //wxASSERT(false);
    }

}


//-------------------------------------------------------------------------------------------
// Implementation of class lmLDPTokenBuilder
//-------------------------------------------------------------------------------------------

//Estas funciones realiza un análisis a nivel de caracteres. Cuando durante
//el análisis se encuentra la secuencia "//" se ignora hasta fin de línea, incluyendo
//ambos caracteres, y se prosigue el análisis como si el trozo ignorado no hubiera
//existido.

lmLDPTokenBuilder::lmLDPTokenBuilder(lmLDPParser* pParser)
{
    m_pParser = pParser;
    m_fRepeatToken = false;
    m_sInBuf = _T("");            // no buffer read
}


lmLDPToken* lmLDPTokenBuilder::ReadToken()
{
static long nTimes=0;

//switch(nTimes++) {
//    case 0:
//        m_token.Set(tkStartOfElement, _T("("));
//        return &m_token;
//    case 1:
//        m_token.Set(tkLabel, _T("N"));
//        return &m_token;
//    case 2:
//        m_token.Set(tkLabel, _T("S"));
//        return &m_token;
//    case 3:
//        m_token.Set(tkEndOfElement, _T(")"));
//        return &m_token;
//    case 4:
//        m_token.Set(tkEndOfFile, _T(""));
//        return &m_token;
//    default:
//        m_token.Set(tkEndOfFile, _T(""));
//        return &m_token;
//}

    if (m_fRepeatToken) {
        m_fRepeatToken = false;
        return &m_token;
    }
    
    // loop until a token is found
    while(true) {
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
    // when entering this function m_lastPos points to the last character read
    if (m_lastPos < m_maxPos) {
        m_curChar = m_sInBuf.GetChar(m_lastPos);
    } else {
        m_curChar = nEOL;
    }
    if (m_curChar == chTab) m_curChar = chSpace;    //change Tabs by Spaces
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
        return m_sInBuf.Mid(iFrom-1, iTo - iFrom + 1);
    } else {
        return m_sInBuf.Mid(iFrom-1);
    }
    
}

bool lmLDPTokenBuilder::IsLetter(wxChar ch)
{
    return (sLetters.Find(ch) != -1);
}

bool lmLDPTokenBuilder::IsNumber(wxChar ch)
{
    return (sNumbers.Find(ch) != -1);
}


void lmLDPTokenBuilder::ParseNewToken()
{
    //cuando entra aquí m_lastPos señala al último caracter leido
    int iStart;

    enum tkStatus {
        FT_Start,
        FT_CMT01,
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
        FT_NUM01,
        FT_NUM02,
        FT_SPC01,
        FT_STR01,
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
                if (IsLetter(m_curChar)) {
                    nState = FT_ETQ01;
                } else if (IsNumber(m_curChar)) {
                    nState = FT_NUM01;
                } else {
                    switch (m_curChar) {
                        case chOpenParenthesis:
                            m_token.Set(tkStartOfElement, chOpenParenthesis);
                            return;
                        case chCloseParenthesis:
                            m_token.Set(tkEndOfElement, chCloseParenthesis);
                            return;
                        case chSpace:
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
                        case nEOF:
                            m_token.Set(tkEndOfFile, _T(""));
                            return;
                        case nEOL:
                            m_token.Set(tkSpaces, chSpace);
                            GetNewBuffer();
                            return;
                        case chLowerSign:
                            nState = FT_EOF01;
                            break;
                        default:
                            nState = FT_Error;
                    }
                }
                break;
    
            case FT_CMT01:
                GNC();
                if (m_curChar == chSlash) {
                    m_token.Set(tkComment, Extract(iStart) );
                    m_sInBuf = _T("");     //flush buffer
                    return;
                } else {
                    nState = FT_Error;
                }
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
                if (IsLetter(m_curChar) || IsNumber(m_curChar) ||
                    m_curChar == chUnderscore || m_curChar == chDot ||
                    m_curChar == chPlusSign || m_curChar == chMinusSign)
                {
                    nState = FT_ETQ01;
                } else {
                    m_lastPos = m_lastPos - 1;     //repeat last char
                    m_token.Set(tkLabel, Extract(iStart, m_lastPos) );
                    return;
                }
                break;
                
            case FT_NUM01:
                GNC();
                if (IsNumber(m_curChar)) {
                    nState = FT_NUM01;
                } else if (m_curChar == chDot) {
                    nState = FT_NUM02;
                } else {
                    m_lastPos = m_lastPos - 1;     //repeat last char
                    m_token.Set(tkIntegerNumber, Extract(iStart, m_lastPos) );
                    return;
                }
                break;

            case FT_NUM02:
                GNC();
                if (IsNumber(m_curChar)) {
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
                if (IsLetter(m_curChar)) {
                    nState = FT_ETQ01;
                } else if (IsNumber(m_curChar)) {
                    nState = FT_NUM01;
                } else if (m_curChar == chPlusSign || m_curChar == chMinusSign) {
                    nState = FT_S03;
                } else if (m_curChar == chSpace || m_curChar == chTab) {
                    m_token.Set(tkLabel, Extract(iStart, m_lastPos) );
                    return;
                } else {
                    nState = FT_Error;
                }
                break;

            case FT_S02:
                GNC();
                if (IsLetter(m_curChar)) {
                    nState = FT_ETQ01;
                } else if (IsNumber(m_curChar)) {
                    nState = FT_NUM01;
                } else if (m_curChar == chPlusSign || m_curChar == chMinusSign) {
                    nState = FT_S03;
                } else {
                    nState = FT_Error;
                }
                break;

            case FT_S03:
                GNC();
                if (IsLetter(m_curChar)) {
                    nState = FT_ETQ01;
                } else if (IsNumber(m_curChar)) {
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
                    m_pParser->ParseMsje(wxString::Format(_T("[Formador de tokens.%d]: Encontrado EOL"),
                        nState));
                } else {
                    m_pParser->ParseMsje(wxString::Format(_T("[Formador de tokens.%d]: Encontrado caracter extraño"),    // (Char:[%s], Dec:%s, Hex:%s). Token=<%s>"),
                        //nState, Chr$(m_curChar), m_curChar, Hex$(m_curChar),
                        //nState, m_curChar, m_curChar, m_curChar,
                        Extract(iStart, m_lastPos)) );
                }
                nState = FT_Start;
                break;

            default:
                wxASSERT(false);
                return;

        } // switch
    } // while loop

}

