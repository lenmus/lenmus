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

//---------------------------------------------------------------------------------------
class lmECmdDeleteTie : public lmEditCmd
{
public:
    lmECmdDeleteTie(lmVStaff* pVStaff, lmUndoItem* pUndoItem, lmNote* pEndNote);
    ~lmECmdDeleteTie() {}

    void RollBack(lmUndoItem* pUndoItem);
    inline bool Success() { return !m_pEndNote->IsTiedToPrev(); }

protected:
    lmNote*         m_pEndNote;     //owner note

};

//---------------------------------------------------------------------------------------
class lmECmdAddTie : public lmEditCmd
{
public:
    lmECmdAddTie(lmVStaff* pVStaff, lmUndoItem* pUndoItem, lmNote* pStartNote, lmNote* pEndNote);
    ~lmECmdAddTie() {}

    void RollBack(lmUndoItem* pUndoItem);
    inline bool Success() { return m_pEndNote->IsTiedToPrev(); }

protected:
    lmNote*         m_pStartNote;
    lmNote*         m_pEndNote;

};

//---------------------------------------------------------------------------------------
class lmECmdChangeDots : public lmEditCmd
{
public:
    lmECmdChangeDots(lmVStaff* pVStaff, lmUndoItem* pUndoItem, lmNoteRest* pNR, int nDots);
    ~lmECmdChangeDots() {}

    void RollBack(lmUndoItem* pUndoItem);
    inline bool Success() { return true; }

protected:
    lmNoteRest*     m_pNR;     //affected note/rest

};

//---------------------------------------------------------------------------------------
class lmECmdDeleteTuplet : public lmEditCmd
{
public:
    lmECmdDeleteTuplet(lmVStaff* pVStaff, lmUndoItem* pUndoItem, lmNoteRest* pStartNR);
    ~lmECmdDeleteTuplet() {}

    void RollBack(lmUndoItem* pUndoItem);
    inline bool Success() { return !m_pStartNR->IsInTuplet(); }

protected:
    lmNoteRest*     m_pStartNR;

};

//---------------------------------------------------------------------------------------
class lmECmdAddTuplet : public lmEditCmd
{
public:
    lmECmdAddTuplet(lmVStaff* pVStaff, lmUndoItem* pUndoItem,
                    std::vector<lmNoteRest*>& notes,
                    bool fShowNumber, int nNumber, bool fBracket,
                    lmEPlacement nAbove, int nActual, int nNormal);

    ~lmECmdAddTuplet() {}

    void RollBack(lmUndoItem* pUndoItem);
    inline bool Success() { return m_NotesRests.front()->IsInTuplet(); }

protected:
    bool                        m_fShowNumber;
    bool                        m_fBracket;
    int                         m_nNumber;
    lmEPlacement                m_nAbove;
    int                         m_nActual;
    int                         m_nNormal;
    std::vector<lmNoteRest*>&   m_NotesRests;

};

//---------------------------------------------------------------------------------------
class lmECmdBreakBeam : public lmEditCmd
{
public:
    lmECmdBreakBeam(lmVStaff* pVStaff, lmUndoItem* pUndoItem, lmNoteRest* pBeforeNR);
    ~lmECmdBreakBeam() {}

    void RollBack(lmUndoItem* pUndoItem);
    inline bool Success() { return true; }

protected:
    lmNoteRest*     m_pBeforeNR;

};

//---------------------------------------------------------------------------------------
class lmECmdJoinBeam : public lmEditCmd
{
public:
    lmECmdJoinBeam(lmVStaff* pVStaff, lmUndoItem* pUndoItem, std::vector<lmNoteRest*>& notes);
    ~lmECmdJoinBeam() {}

    void RollBack(lmUndoItem* pUndoItem);
    inline bool Success() { return true; }

protected:
    std::vector<lmNoteRest*>&   m_NotesRests;

};

//---------------------------------------------------------------------------------------
class lmECmdDeleteBeam : public lmEditCmd
{
public:
    lmECmdDeleteBeam(lmVStaff* pVStaff, lmUndoItem* pUndoItem, lmNoteRest* pNR);
    ~lmECmdDeleteBeam() {}

    void RollBack(lmUndoItem* pUndoItem);
    inline bool Success() { return true; }

protected:
    lmNoteRest*     m_pNR;

};

//---------------------------------------------------------------------------------------
class lmECmdChangeText : public lmEditCmd
{
public:
    lmECmdChangeText(lmScoreText* pST, lmUndoItem* pUndoItem, 
                     wxString& sText, lmEHAlign nHAlign, lmLocation tPos, 
                     lmTextStyle* pStyle);
    ~lmECmdChangeText() {}

    void RollBack(lmUndoItem* pUndoItem);
    inline bool Success() { return true; }

protected:
    wxString        m_sText;
    lmEHAlign       m_nHAlign;
    lmLocation      m_tPos;
    lmTextStyle*    m_pStyle;
};

//---------------------------------------------------------------------------------------
class lmEDeleteText : public lmEditCmd
{
public:
    lmEDeleteText(lmScoreText* pST, lmComponentObj* pAnchor, lmUndoItem* WXUNUSED(pUndoItem));
    ~lmEDeleteText() {}

    void RollBack(lmUndoItem* pUndoItem);
    inline bool Success() { return true; }

protected:
    lmScoreText*		m_pST;
    lmComponentObj*		m_pAnchor;
	int					m_nIdx;
};




#endif    // __LM_EDITCMD_H__
