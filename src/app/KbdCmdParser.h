//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#ifndef __LM_KBDCMDPARSER_H__        //to avoid nested includes
#define __LM_KBDCMDPARSER_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "KbdCmdParser.cpp"
#endif

#include "../Score/defs.h"      //for lmEAccidentals


class lmKbdCmdParser
{
public:
    lmKbdCmdParser();
    ~lmKbdCmdParser();

    bool ParserCommand(wxString sCmd);

    //get results for notes/rests
    lmEAccidentals GetAccidentals() { return m_nAcc; }
    int GetDots() { return m_nDots; }
    bool GetTiedPrev() { return m_fTiedPrev; }



private:
    bool DoParse();
    wxChar      GetNextChar();

    //results
    lmEAccidentals  m_nAcc;
    int             m_nDots;
    bool            m_fTiedPrev;

    wxString    m_sBuff;        //string to parse
    int         m_iFirst;       //index to first char not yet processed
    int         m_iMax;         //index to last char + 1

};

#endif    // __LM_KBDCMDPARSER_H__
