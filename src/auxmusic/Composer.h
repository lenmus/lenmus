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

#ifndef __COMPOSER_H__        //to avoid nested includes
#define __COMPOSER_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "Composer.cpp"
#endif

#include "../score/Score.h"
#include "../exercises/ScoreConstrains.h"


class lmComposer
{
public:
    lmComposer();
    virtual ~lmComposer() {};

    //command to generate a new score
    virtual lmScore* GenerateScore(lmScoreConstrains* pConstrains) = 0;

protected:
    lmScore*        m_pScore;        //the score generated

};


#endif    // __COMPOSER_H__



