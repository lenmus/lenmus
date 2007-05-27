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

#ifdef __GNUG__
#pragma implementation "ScoreCommand.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Score.h"
#include "../app/global.h"
#include "ScoreCommand.h"
#include "../app/TheApp.h"

//
// Implementation of the score commands class
//

lmScoreCommand::lmScoreCommand(const wxString& sName, EScoreCommand nCommand, lmScoreDocument *pDoc,
         lmScoreObj* pScO):
    wxCommand(true, sName)
{
    m_pDoc = pDoc;
    m_cmd = nCommand;
    m_pScO = pScO;
}

lmScoreCommand::~lmScoreCommand()
{
}

bool lmScoreCommand::Do()
{
    bool fResult = false;

    switch (m_cmd)
    {
        case CMD_SelectObject:
            fResult = CmdSelectObject();
            break;

        case CMD_MoveStaffObj:
        {
            lmScoreCommandMove* pSC = (lmScoreCommandMove*)this;
            fResult = pSC->DoMoveStaffObj();
            break;
        }

        default:
            wxASSERT(false);    // cmd not implemented or error in cmd code

    }
    //if (fUpdate) {
    //    m_pDoc->Modify(true);
    //    m_pDoc->UpdateAllViews();
    //}

    return fResult;
}

bool lmScoreCommand::Undo()
{
    bool fResult = false;

    switch (m_cmd)
    {
        case CMD_SelectObject:
            fResult = CmdSelectObject();
            break;

        case CMD_MoveStaffObj:
        {
            lmScoreCommandMove* pSC = (lmScoreCommandMove*)this;
            fResult = pSC->UndoMoveStaffObj();
            break;
        }

        default:
            wxASSERT(false);    // cmd not implemented or error in cmd code

    }
    return fResult;
}

bool lmScoreCommand::CmdSelectObject()
{
    //wxMessageDialog(GetMainFrame(), _T("Se ejecuta CmdSelectObject ") ).ShowModal();

    wxASSERT( m_pScO);

    //Mark as 'selected' and return 'true' to force redraw it in 'selected' color
    bool fSelected = m_pScO->IsSelected();
    m_pScO->SetSelected(!fSelected);
    m_pDoc->UpdateAllViews();
    return true;

}

//----------------------------------------------------------------------------------------
// lmScoreCommandMove implementation
//----------------------------------------------------------------------------------------

bool lmScoreCommandMove::DoMoveStaffObj()
{
    wxASSERT_MSG( m_pScO, _T("DoMoveStaffObj: No hay objeto!"));

    m_pScO->SetFixed(true);
    m_oldPos = m_pScO->EndDrag(m_pos);
    m_pDoc->UpdateAllViews();
    return true;

}

bool lmScoreCommandMove::UndoMoveStaffObj()
{
    wxASSERT_MSG( m_pScO, _T("UndoMoveStaffObj: No hay objeto!"));

    m_pScO->SetFixed(false);
    m_pScO->EndDrag(m_oldPos);
    m_pDoc->UpdateAllViews();
    return true;

}
