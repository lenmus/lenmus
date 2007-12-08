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

#include "../score/Score.h"
#include "ScoreCommand.h"
#include "ScoreDoc.h"
#include "TheApp.h"

//
// Implementation of the score commands class
//

lmScoreCommand::lmScoreCommand(const wxString& sName, lmEScoreCommand nCommand, lmScoreDocument *pDoc)
    : wxCommand(true, sName)
{
    m_pDoc = pDoc;
    m_cmd = nCommand;
	m_fDocModified = false;
}

lmScoreCommand::~lmScoreCommand()
{
}

//----------------------------------------------------------------------------------------
// lmCmdSelectSingle implementation
//----------------------------------------------------------------------------------------

bool lmCmdSelectSingle::Do()
{
    return DoSelectObject();
}

bool lmCmdSelectSingle::Undo()
{
    return DoSelectObject();
}

bool lmCmdSelectSingle::DoSelectObject()
{
    wxASSERT( m_pGMO);

    //Toggle 'selected' and return 'true' to force redraw it
    bool fSelected = m_pGMO->IsSelected();
    m_pGMO->SetSelected(!fSelected);
    m_pDoc->UpdateAllViews();
    return true;

}



//----------------------------------------------------------------------------------------
// lmScoreCommandMove implementation
//----------------------------------------------------------------------------------------

lmScoreCommandMove::lmScoreCommandMove(const wxString& sName, lmScoreDocument *pDoc,
									   lmScoreObj* pSO, const lmUPoint& uPos)
	: lmScoreCommand(sName, lmCMD_MoveScoreObj, pDoc)
{
	lmUPoint uOrg = pSO->GetOrigin();
	m_tPos.x = uPos.x - uOrg.x;
	m_tPos.y = uPos.y - uOrg.y;
	m_tPos.xType = lmLOCATION_USER_RELATIVE;
	m_tPos.yType = lmLOCATION_USER_RELATIVE;
	m_tPos.xUnits = lmLUNITS;
	m_tPos.yUnits = lmLUNITS;

	m_pSO = pSO;
}

bool lmScoreCommandMove::Do()
{
    wxASSERT_MSG( m_pSO, _T("[lmScoreCommandMove::Do] No ScoreObj to move!"));

    m_tOldPos = m_pSO->SetUserLocation(m_tPos);
	m_fDocModified = m_pDoc->IsModified();
	m_pDoc->Modify(true);
    m_pDoc->UpdateAllViews();
    return true;

}

bool lmScoreCommandMove::Undo()
{
    wxASSERT_MSG( m_pSO, _T("[lmScoreCommandMove::Undo]: No ScoreObj to move!"));

    m_pSO->SetUserLocation(m_tOldPos);
	m_pDoc->Modify(m_fDocModified);
    m_pDoc->UpdateAllViews();
    return true;

}
