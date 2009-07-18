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

#ifndef __LM_EDITCMD_H__        //to avoid nested includes
#define __LM_EDITCMD_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "EditCmd.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <vector>

#include "defs.h"

class lmRest;
class lmNote;
class lmNoreRest;
class lmClef;
class lmBarline;
class lmTimeSignature;
class lmKeySignature;
class lmStaffObj;
class lmTextItem;
class lmVStaff;
class lmUndoItem;

#include "Score.h"

//----------------------------------------------------------------------------------------
// helper class lmEditCmd: a command with roll-back capabilities
//----------------------------------------------------------------------------------------

class lmEditCmd
{
public:
    lmEditCmd(lmScoreObj* pSCO);
    virtual ~lmEditCmd();

    virtual void RollBack(lmUndoItem* pUndoItem)=0;
    virtual bool Success()=0;

protected:
    lmScoreObj*     m_pSCO;
};




#endif    // __LM_EDITCMD_H__
