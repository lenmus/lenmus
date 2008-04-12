//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ScoreCommand.cpp"
#endif

#include <list>

#include "wx/cmdproc.h"

#include "ScoreView.h"
#include "../score/defs.h"
#include "../score/UndoRedo.h"
#include "../score/ColStaffObjs.h"      //lmVCursorState
class lmComponentObj;
class lmScoreDocument;
class lmGMObject;
class lmScoreObj;
class lmVStaffCursor;
class lmVStaff;
class lmNote;
class lmGMSelection;




// base abstract class
class lmScoreCommand: public wxCommand
{
public:
	// Commands
	enum lmEScoreCommand
	{
		lmCMD_SelectSingle = 1,
		lmCMD_SelectMultiple,
		lmCMD_MoveScoreObj,
		lmCMD_DeleteObject,
        lmCMD_InsertBarline,
        lmCMD_InsertClef,
        lmCMD_InsertNote,
		lmCMD_ChangeNotePitch,
		lmCMD_ChangeNoteAccidentals,
	};

    virtual ~lmScoreCommand();

    virtual bool Do()=0;
    virtual bool Undo()=0;
    virtual lmEScoreCommand GetCommandType()=0;

protected:
    lmScoreCommand(const wxString& name, lmScoreDocument *pDoc);

    //common methods
    void CommandDone(bool fScoreModified, int nOptions=0);


    lmScoreDocument*    m_pDoc;
	bool				m_fDocModified;
	lmUndoData		    m_UndoData;		    //collection of undo/redo items
};


// Select object commands
//------------------------------------------------------------------------------------
class lmCmdSelectSingle: public lmScoreCommand
{
public:
    lmCmdSelectSingle(const wxString& name, lmScoreDocument *pDoc, lmView* pView, lmGMObject* pGMO)
        : lmScoreCommand(name, pDoc)
        {
            m_pGMO = pGMO;
            m_pView = pView;
        }

    ~lmCmdSelectSingle() {}

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();
    lmEScoreCommand GetCommandType() { return lmCMD_SelectSingle; }


protected:
	bool DoSelectObject();


    lmGMObject*		m_pGMO;
    lmView*         m_pView;

};

class lmCmdSelectMultiple: public lmScoreCommand
{
public:
    lmCmdSelectMultiple(const wxString& name, lmScoreDocument *pDoc, lmView* pView,
                        lmGMSelection* pSelection, bool fSelect);
    ~lmCmdSelectMultiple() {}

    //implementation of pure virtual methods in base class
    inline bool Do() { return DoSelectUnselect(); }
    inline bool Undo() { return DoSelectUnselect(); }
    lmEScoreCommand GetCommandType() { return lmCMD_SelectMultiple; }

protected:
	bool DoSelectUnselect();

    lmGMSelection*  m_pSelection;
    lmView*         m_pView;
    bool            m_fSelect;

};


// Move object command
//------------------------------------------------------------------------------------
class lmCmdUserMoveScoreObj: public lmScoreCommand
{
public:
    lmCmdUserMoveScoreObj(const wxString& name, lmScoreDocument *pDoc, lmScoreObj* pSO,
					   const lmUPoint& uPos);
    ~lmCmdUserMoveScoreObj() {}

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();
    lmEScoreCommand GetCommandType() { return lmCMD_MoveScoreObj; }


protected:
    lmLocation      m_tPos;
    lmLocation		m_tOldPos;        // for Undo
	lmScoreObj*		m_pSO;
};


// Delete object command
//------------------------------------------------------------------------------------
class lmCmdDeleteObject: public lmScoreCommand
{
public:
    lmCmdDeleteObject(lmVStaffCursor* pVCursor, const wxString& name, lmScoreDocument *pDoc);
    ~lmCmdDeleteObject();

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();
    lmEScoreCommand GetCommandType() { return lmCMD_DeleteObject; }


protected:
    lmVCursorState      m_tCursorState; //cursor state before deletion
    lmVStaff*           m_pVStaff;      //affected VStaff
    lmStaffObj*         m_pSO;          //deleted note
    bool                m_fDeleteSO;    //to control if m_pSO must be deleted
};


// Insert barline command
//------------------------------------------------------------------------------------
class lmCmdInsertBarline: public lmScoreCommand
{
public:

    lmCmdInsertBarline(lmVStaffCursor* pVCursor, const wxString& name, lmScoreDocument *pDoc,
                       lmEBarline nType);
    ~lmCmdInsertBarline() {}

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();
    lmEScoreCommand GetCommandType() { return lmCMD_InsertBarline; }


protected:
    lmVStaffCursor*     m_pVCursor;
    lmEBarline	        m_nBarlineType;
};


// Insert clef command
//------------------------------------------------------------------------------------
class lmCmdInsertClef: public lmScoreCommand
{
public:

    lmCmdInsertClef(lmVStaffCursor* pVCursor, const wxString& name, lmScoreDocument *pDoc,
                    lmEClefType nClefType);
    ~lmCmdInsertClef() {}

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();
    lmEScoreCommand GetCommandType() { return lmCMD_InsertClef; }


protected:
    lmVStaffCursor*     m_pVCursor;
    lmEClefType         m_nClefType;

    lmVStaff*           m_pVStaff;      //affected VStaff
    lmClef*				m_pNewClef;     //inserted clef
};


// Insert note command
//------------------------------------------------------------------------------------
class lmCmdInsertNote: public lmScoreCommand
{
public:

    lmCmdInsertNote(lmVStaffCursor* pVCursor, const wxString& name, lmScoreDocument *pDoc,
					lmEPitchType nPitchType, wxString sStep, wxString sOctave, 
					lmENoteType nNoteType, float rDuration, lmENoteHeads nNotehead,
					lmEAccidentals nAcc);
    ~lmCmdInsertNote() {}

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();
    lmEScoreCommand GetCommandType() { return lmCMD_InsertNote; }


protected:
    lmVStaffCursor*     m_pVCursor;
	lmENoteType		    m_nNoteType;
	lmEPitchType	    m_nPitchType;
	wxString		    m_sStep;
	wxString		    m_sOctave;
	float			    m_rDuration;
	lmENoteHeads	    m_nNotehead;
	lmEAccidentals	    m_nAcc;

    lmVStaff*           m_pVStaff;      //affected VStaff
    lmNote*             m_pNewNote;     //inserted note
};


// Change note pitch command
//------------------------------------------------------------------------------------
class lmCmdChangeNotePitch: public lmScoreCommand
{
public:

    lmCmdChangeNotePitch(const wxString& name, lmScoreDocument *pDoc, lmNote* pNote,
					int nSteps);
    ~lmCmdChangeNotePitch() {}

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();
    lmEScoreCommand GetCommandType() { return lmCMD_ChangeNotePitch; }


protected:
	int				m_nSteps;
	lmNote*			m_pNote;
};


// Change note accidentals command
//------------------------------------------------------------------------------------
class lmCmdChangeNoteAccidentals: public lmScoreCommand
{
public:

    lmCmdChangeNoteAccidentals(const wxString& name, lmScoreDocument *pDoc, lmNote* pNote,
					int nSteps);
    ~lmCmdChangeNoteAccidentals() {}

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();
    lmEScoreCommand GetCommandType() { return lmCMD_ChangeNoteAccidentals; }


protected:
	int				m_nSteps;
	lmNote*			m_pNote;
};

	void ChangeNoteAccidentals(int nSteps);



#endif    // __LM_SCORECOMMAND_H__        //to avoid nested includes
