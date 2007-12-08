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

#ifndef __LM_SCORECOMMAND_H__        //to avoid nested includes
#define __LM_SCORECOMMAND_H__

#ifdef __GNUG__
#pragma interface "ScoreCommand.cpp"
#endif

#include "wx/cmdproc.h"

#include "../score/defs.h"
class lmComponentObj;
class lmScoreDocument;

// base abstract class
class lmScoreCommand: public wxCommand
{
public:
	// Commands
	enum lmEScoreCommand
	{
		lmCMD_SelectSingle = 1,
		lmCMD_MoveScoreObj,
	};

    virtual ~lmScoreCommand();

    virtual bool Do()=0;
    virtual bool Undo()=0;

protected:
    lmScoreCommand(const wxString& name, lmEScoreCommand cmd, lmScoreDocument *pDoc);

    lmScoreDocument*    m_pDoc;
	bool				m_fDocModified;
    lmEScoreCommand		m_cmd;

};

// Select object command
//------------------------------------------------------------------------------------
class lmCmdSelectSingle: public lmScoreCommand
{
public:
    lmCmdSelectSingle(const wxString& name, lmScoreDocument *pDoc, lmGMObject* pGMO)
        : lmScoreCommand(name, lmCMD_SelectSingle, pDoc)
        {
            m_pGMO = pGMO;
        }

    ~lmCmdSelectSingle() {}

    //overrides of pure virtual methods in base class
    bool Do();
    bool Undo();


protected:
	bool DoSelectObject();


    lmGMObject*		m_pGMO;

};

// Move object command
//------------------------------------------------------------------------------------
class lmScoreCommandMove: public lmScoreCommand
{
public:
    lmScoreCommandMove(const wxString& name, lmScoreDocument *pDoc, lmScoreObj* pSO,
					   const lmUPoint& uPos);
    ~lmScoreCommandMove() {}

    //overrides of pure virtual methods in base class
    bool Do();
    bool Undo();


protected:
    lmLocation      m_tPos;
    lmLocation		m_tOldPos;        // for Undo
	lmScoreObj*		m_pSO;
};

#endif    // __LM_SCORECOMMAND_H__        //to avoid nested includes
