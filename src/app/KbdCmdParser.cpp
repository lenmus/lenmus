//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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
#pragma implementation "KbdCmdParser.h"
#endif

// for (compilers that support precompilation, includes <wx/wx.h>.
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "KbdCmdParser.h"



const wxChar nEOB = _T('\x00');        //End of buffer


//-------------------------------------------------------------------------------------------
// Implementation of class lmKbdCmdParser
//-------------------------------------------------------------------------------------------

lmKbdCmdParser::lmKbdCmdParser()
{
}

lmKbdCmdParser::~lmKbdCmdParser()
{
}

bool lmKbdCmdParser::ParserCommand(wxString sCmd)
{
    //Return true if OK (no error)

    //default values for answers
    m_nAcc = lm_eNoAccidentals;
    m_nDots = 0;
    m_fTiedPrev = false;

    m_sBuff = sCmd;
    m_iFirst = 0;
    m_iMax = m_sBuff.Length();
    return DoParse();
}

wxChar lmKbdCmdParser::GetNextChar()
{
    // Returns next char.
    // when entering, m_iFirst points to the char to read

    if (m_iFirst >= m_iMax)
        return nEOB;
    else
        return m_sBuff.GetChar(m_iFirst++);
}

bool lmKbdCmdParser::DoParse()
{
    //Return true if OK (no error)

    //automata states
    enum {
        FT_Start,
        FT_Repeat,
        FT_ACC01,
        FT_ACC02,
        FT_ACC03,
        FT_DOT01,
        FT_Error
    };

    int nState = FT_Start;
    wxChar curChar = _T('-');
    while (true)
    {
        switch (nState) {
            case FT_Start:
                curChar = GetNextChar();
            case FT_Repeat:
                switch(curChar)
                {
                    case _T('+'):
                        nState = FT_ACC01;
                        break;
                    case _T('-'):
                        nState = FT_ACC02;
                        break;
                    case _T('='):
                        nState = FT_ACC03;
                        break;
                    case _T('x'):
                        nState = FT_Start;
                        m_nAcc = lm_eDoubleSharp;
                        break;
                    case _T('.'):
                        nState = FT_DOT01;
                        m_nDots = 1;
                        break;
                    case _T('_'):
                        nState = FT_Start;
                        m_fTiedPrev = true;
                        break;
                    case nEOB:
                        return true;    //End of buffer. Parsed without errors
                    default:
                        nState = FT_Error;
                }
                break;

            case FT_ACC01:
                curChar = GetNextChar();
                if (curChar == _T('+')) {
                    m_nAcc = lm_eSharpSharp;
                    nState = FT_Start;
                } else {
                    m_nAcc = lm_eSharp;
                    nState = FT_Repeat;
                }
                break;

            case FT_ACC02:
                curChar = GetNextChar();
                if (curChar == _T('-')) {
                    m_nAcc = lm_eFlatFlat;
                    nState = FT_Start;
                } else {
                    m_nAcc = lm_eFlat;
                    nState = FT_Repeat;
                }
                break;

            case FT_ACC03:
                curChar = GetNextChar();
                if (curChar == _T('+')) {
                    m_nAcc = lm_eNaturalSharp;
                    nState = FT_Start;
                } else if (curChar == _T('-')) {
                    m_nAcc = lm_eNaturalFlat;
                    nState = FT_Start;
                } else {
                    m_nAcc = lm_eNatural;
                    nState = FT_Repeat;
                }
                break;

            case FT_DOT01:
                curChar = GetNextChar();
                if (curChar == _T('.')) {
                    ++m_nDots;
                } else {
                    nState = FT_Repeat;
                }
                break;

            case FT_Error:
                return false;   //parse error

            default:
                wxASSERT(false);    //not defined state
                return false;   //parse error

        }    // switch for states
    } // while loop
    return true;    //parsed without errors
}

