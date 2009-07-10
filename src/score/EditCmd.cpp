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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "EditCmd.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include <list>

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "EditCmd.h"

#include "VStaff.h"
#include "UndoRedo.h"
#include "../app/Preferences.h"


//----------------------------------------------------------------------------------------
// lmEditCmd implementation
//----------------------------------------------------------------------------------------

lmEditCmd::lmEditCmd(lmScoreObj* pSCO)
{
    m_pSCO = pSCO;
}

lmEditCmd::~lmEditCmd()
{
}



//----------------------------------------------------------------------------------------
// lmECmdDeleteTie implementation
//----------------------------------------------------------------------------------------

lmECmdDeleteTie::lmECmdDeleteTie(lmVStaff* pVStaff, lmUndoItem* pUndoItem,
                                 lmNote* pEndNote)
    : lmEditCmd(pVStaff)
{
    m_pEndNote = pEndNote;
    pVStaff->Cmd_DeleteTie(pUndoItem, pEndNote);
}

void lmECmdDeleteTie::RollBack(lmUndoItem* pUndoItem)
{
    ((lmVStaff*)m_pSCO)->UndoCmd_DeleteTie(pUndoItem, m_pEndNote);
}



//----------------------------------------------------------------------------------------
// lmECmdAddTie implementation
//----------------------------------------------------------------------------------------

lmECmdAddTie::lmECmdAddTie(lmVStaff* pVStaff, lmUndoItem* pUndoItem,
                           lmNote* pStartNote, lmNote* pEndNote)
    : lmEditCmd(pVStaff)
{
    m_pStartNote = pStartNote;
    m_pEndNote = pEndNote;
    pVStaff->Cmd_AddTie(pUndoItem, m_pStartNote, m_pEndNote);
}

void lmECmdAddTie::RollBack(lmUndoItem* pUndoItem)
{
    ((lmVStaff*)m_pSCO)->UndoCmd_AddTie(pUndoItem, m_pStartNote, m_pEndNote);
}



//----------------------------------------------------------------------------------------
// lmECmdChangeDots implementation
//----------------------------------------------------------------------------------------

lmECmdChangeDots::lmECmdChangeDots(lmVStaff* pVStaff, lmUndoItem* pUndoItem,
                                   lmNoteRest* pNR, int nDots)
    : lmEditCmd(pVStaff)
{
    m_pNR = pNR;
    pVStaff->Cmd_ChangeDots(pUndoItem, pNR, nDots);
}

void lmECmdChangeDots::RollBack(lmUndoItem* pUndoItem)
{
    ((lmVStaff*)m_pSCO)->UndoCmd_ChangeDots(pUndoItem, m_pNR);
}



//----------------------------------------------------------------------------------------
// lmECmdDeleteTuplet implementation
//----------------------------------------------------------------------------------------

lmECmdDeleteTuplet::lmECmdDeleteTuplet(lmVStaff* pVStaff, lmUndoItem* pUndoItem,
                                       lmNoteRest* pStartNR)
    : lmEditCmd(pVStaff)
{
    m_pStartNR = pStartNR;
    pVStaff->Cmd_DeleteTuplet(pUndoItem, m_pStartNR);
}

void lmECmdDeleteTuplet::RollBack(lmUndoItem* pUndoItem)
{
    ((lmVStaff*)m_pSCO)->UndoCmd_DeleteTuplet(pUndoItem, m_pStartNR);
}



//----------------------------------------------------------------------------------------
// lmECmdAddTuplet implementation
//----------------------------------------------------------------------------------------

lmECmdAddTuplet::lmECmdAddTuplet(lmVStaff* pVStaff, lmUndoItem* pUndoItem,
                                 std::vector<lmNoteRest*>& notes,
                                 bool fShowNumber, int nNumber, bool fBracket,
                                 lmEPlacement nAbove, int nActual, int nNormal)
    : lmEditCmd(pVStaff), m_NotesRests(notes)
{
    m_fShowNumber = fShowNumber;
    m_nNumber = nNumber;
    m_fBracket = fBracket;
    m_nAbove = nAbove;
    m_nActual = nActual;
    m_nNormal = nNormal;

    pVStaff->Cmd_AddTuplet(pUndoItem, m_NotesRests, m_fShowNumber, m_nNumber, m_fBracket,
                           m_nAbove, m_nActual, m_nNormal);
}

void lmECmdAddTuplet::RollBack(lmUndoItem* pUndoItem)
{
    ((lmVStaff*)m_pSCO)->UndoCmd_AddTuplet(pUndoItem, m_NotesRests.front());
}



//----------------------------------------------------------------------------------------
// lmECmdBreakBeam implementation
//----------------------------------------------------------------------------------------

lmECmdBreakBeam::lmECmdBreakBeam(lmVStaff* pVStaff, lmUndoItem* pUndoItem,
                                 lmNoteRest* pBeforeNR)
    : lmEditCmd(pVStaff)
{
    m_pBeforeNR = pBeforeNR;
    pVStaff->Cmd_BreakBeam(pUndoItem, m_pBeforeNR);
}

void lmECmdBreakBeam::RollBack(lmUndoItem* pUndoItem)
{
    ((lmVStaff*)m_pSCO)->UndoCmd_BreakBeam(pUndoItem, m_pBeforeNR);
}



//----------------------------------------------------------------------------------------
// lmECmdJoinBeam implementation
//----------------------------------------------------------------------------------------

lmECmdJoinBeam::lmECmdJoinBeam(lmVStaff* pVStaff, lmUndoItem* pUndoItem,
                               std::vector<lmNoteRest*>& notes)
    : lmEditCmd(pVStaff), m_NotesRests(notes)
{
    pVStaff->Cmd_JoinBeam(pUndoItem, m_NotesRests);
}

void lmECmdJoinBeam::RollBack(lmUndoItem* pUndoItem)
{
    ((lmVStaff*)m_pSCO)->UndoCmd_JoinBeam(pUndoItem);
}



//----------------------------------------------------------------------------------------
// lmECmdDeleteBeam implementation
//      pNR is any note/rest of the beam
//----------------------------------------------------------------------------------------

lmECmdDeleteBeam::lmECmdDeleteBeam(lmVStaff* pVStaff, lmUndoItem* pUndoItem,
                                 lmNoteRest* pNR)
    : lmEditCmd(pVStaff)
{
    m_pNR = pNR;
    pVStaff->Cmd_DeleteBeam(pUndoItem, m_pNR);
}

void lmECmdDeleteBeam::RollBack(lmUndoItem* pUndoItem)
{
    ((lmVStaff*)m_pSCO)->UndoCmd_DeleteBeam(pUndoItem);
}



//----------------------------------------------------------------------------------------
// lmECmdChangeText implementation
//----------------------------------------------------------------------------------------

lmECmdChangeText::lmECmdChangeText(lmScoreText* pST, lmUndoItem* pUndoItem,
                                   wxString& sText, lmEHAlign nHAlign,
                                   lmLocation tPos, lmTextStyle* pStyle)
    : lmEditCmd((lmScoreObj*)pST)
{
    //save all data to be modified
    m_sText = pST->GetText();
    m_nHAlign = pST->GetAlignment();
    m_tPos = pST->GetLocation();
    m_pStyle = pST->GetStyle();

    pST->Cmd_ChangeText(pUndoItem, sText, nHAlign, tPos, pStyle);
}

void lmECmdChangeText::RollBack(lmUndoItem* pUndoItem)
{
    ((lmScoreText*)m_pSCO)->UndoCmd_ChangeText(pUndoItem, m_sText, m_nHAlign, m_tPos, 
                                               m_pStyle);
}



//----------------------------------------------------------------------------------------
// lmEDeleteText implementation
//----------------------------------------------------------------------------------------

lmEDeleteText::lmEDeleteText(lmScoreText* pST, lmComponentObj* pAnchor,
							 lmUndoItem* WXUNUSED(pUndoItem))
    : lmEditCmd((lmScoreObj*)pST)
{
    //save all data to be modified
    m_pST = pST;
    m_pAnchor = pAnchor;
	m_nIdx = pAnchor->DetachAuxObj(pST);
}

void lmEDeleteText::RollBack(lmUndoItem* pUndoItem)
{
	m_pAnchor->AttachAuxObj(m_pST, m_nIdx);
}

