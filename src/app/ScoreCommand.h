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

#ifndef __SCORE_COMMAND_H__        //to avoid nested includes
#define __SCORE_COMMAND_H__

#ifdef __GNUG__
#pragma interface "ScoreCommand.cpp"
#endif

#include "wx/cmdproc.h"

#include "global.h"
class lmScoreObj;
class lmScoreDocument;

// base class
class lmScoreCommand: public wxCommand
{
public:
	// Commands
	enum lmEScoreCommand
	{
		lmCMD_SelectObject = 1,
		lmCMD_MoveStaffObj,
	};

    lmScoreCommand(const wxString& name, lmEScoreCommand cmd, lmScoreDocument *pDoc, lmScoreObj* pScO);
    ~lmScoreCommand();

    bool Do();
    bool Undo();

private:
    bool CmdSelectObject();

protected:
    lmScoreDocument*    m_pDoc;
    lmEScoreCommand		m_cmd;
    lmScoreObj*			m_pScO;

};

class lmScoreCommandMove: public lmScoreCommand
{
public:
    lmScoreCommandMove(const wxString& name, lmScoreDocument *pDoc, lmScoreObj* pScO,
            const lmUPoint& uPos) :
        lmScoreCommand(name, lmCMD_MoveStaffObj, pDoc, pScO)
        {
            m_pos = uPos;
        }
    ~lmScoreCommandMove() {}

    bool DoMoveStaffObj();
    bool UndoMoveStaffObj();


protected:
    lmUPoint        m_pos;
    lmUPoint        m_oldPos;        // for Undo
};

#endif    // __SCORE_COMMAND_H__        //to avoid nested includes
