//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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
/*! @file LDPToken.h
    @brief Header file for class lmLDPToken
    @ingroup ldp_parser
*/
#ifndef __LMTOKEN_H        //to avoid nested includes
#define __LMTOKEN_H


class lmLDPParser;


enum ETokenType {
    tkStartOfElement = 0,
    tkEndOfElement,
    tkIntegerNumber,
    tkRealNumber,
    tkLabel,
    tkString,
    tkEndOfFile,
    //tokens for internal use
    tkSpaces,        //token separator
    tkComment        //to be filtered out in tokenizer routines
};


    
class lmLDPToken
{
public:
    lmLDPToken();
    ~lmLDPToken() {}

    ETokenType  GetType() { return m_Type; }
    wxString    GetValue() { return m_sValue; }
    wxString    GetDescription();
    void        Set(ETokenType nType, wxString sValue) {
                    m_Type = nType;
                    m_sValue = sValue;
                }

private:
    wxString    m_sValue;
    ETokenType  m_Type;

};

#include "wx/dynarray.h"
WX_DEFINE_ARRAY(lmLDPToken*, ArrayTokenPtrs);


class lmLDPTokenBuilder
{
public:
    lmLDPTokenBuilder(lmLDPParser* pParser);
    ~lmLDPTokenBuilder();

    void RepeatToken() { m_fRepeatToken = true; }
    lmLDPToken* ReadToken();


private:
    void        GNC();
    void        GetNewBuffer();
    wxString    Extract(long iFrom, long iTo=0);
    bool        IsLetter(wxChar ch);
    bool        IsNumber(wxChar ch);
    void        ParseNewToken();
    void        PushToken(ETokenType nType, wxString sValue);
    bool        PopToken();



    lmLDPParser*    m_pParser;        //parser using this token builder
    lmLDPToken      m_token;
    bool            m_fRepeatToken;
    wxString        m_sInBuf;       // buffer being analised. It is different from the file
                                    // reading buffer so that we can insert / alter its content: i.e.
                                    // to expand macros, #define susbtitutions, etc. 

    long        m_lastPos;      // index of last character read by GNC() First char = 1. 
                                // Zero means "no char read yet"
    long        m_maxPos;       // buffer size - 1 = maximum value for m_lastPos
    wxChar      m_curChar;      // character being processed. It is read by GNC()

    //stack of parsed tokens
    ArrayTokenPtrs      m_cPending;


    //to deal with compact notation [  name:value  -->  (name value)  ]
    bool        m_fEndOfElementPending;
    bool        m_fValuePartPending;
    bool        m_fNamePartPending;
    lmLDPToken  m_tokenNamePart;

    //to deal with abbreviated notation [ (n c4 q t3) --> nc4q,t3 ]
    bool        m_fAbbreviated;     

};
    
#endif    // __LMTOKEN_H