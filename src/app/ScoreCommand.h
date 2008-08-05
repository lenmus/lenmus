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
    virtual ~lmScoreCommand();

    bool Undo();

    virtual bool Do()=0;
    virtual bool UndoCommand()=0;

protected:
    lmScoreCommand(const wxString& name, lmScoreDocument *pDoc, lmVStaffCursor* pVCursor,
                   bool fHistory = true );


    //common methods
    bool CommandDone(bool fScoreModified, int nOptions=0);

    //access to UndoInfo object
    inline lmUndoLog* GetUndoInfo() { return &m_UndoLog; }


    lmScoreDocument*    m_pDoc;
	bool				m_fDocModified;
    bool                m_fHistory;         //include command in undo/redo history
    lmUndoLog           m_UndoLog;          //collection of undo/redo items
    lmUndoItem*         m_pUndoItem;        //undo item for this command
    lmVCursorState      m_tCursorState;     //VCursor state when issuing the command

};


// Select object commands
//------------------------------------------------------------------------------------
class lmCmdSelectSingle: public lmScoreCommand
{
public:
    lmCmdSelectSingle(const wxString& name, lmScoreDocument *pDoc, lmView* pView, lmGMObject* pGMO)
        : lmScoreCommand(name, pDoc, (lmVStaffCursor*)NULL )
        {
            m_pGMO = pGMO;
            m_pView = pView;
        }

    ~lmCmdSelectSingle() {}

    //implementation of pure virtual methods in base class
    bool Do();
    bool UndoCommand();


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
    inline bool UndoCommand() { return DoSelectUnselect(); }

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
    lmCmdUserMoveScoreObj(const wxString& name, lmScoreDocument *pDoc, lmGMObject* pGMO,
					   const lmUPoint& uPos);
    ~lmCmdUserMoveScoreObj() {}

    //implementation of pure virtual methods in base class
    bool Do();
    bool UndoCommand();

protected:
    lmLocation      m_tPos;
    lmLocation		m_tOldPos;        // for Undo
	lmScoreObj*		m_pSO;
    int             m_nShapeIdx;
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
    bool UndoCommand();

protected:
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
    bool UndoCommand();

protected:
    lmEBarline	        m_nBarlineType;
};


// Insert clef command
//------------------------------------------------------------------------------------
class lmCmdInsertClef: public lmScoreCommand
{
public:

    lmCmdInsertClef(lmVStaffCursor* pVCursor, const wxString& name, lmScoreDocument *pDoc,
                    lmEClefType nClefType, bool fHistory=true);
    ~lmCmdInsertClef() {}

    //implementation of pure virtual methods in base class
    bool Do();
    bool UndoCommand();

protected:
    lmEClefType         m_nClefType;

};



// Insert time signature command
//------------------------------------------------------------------------------------
class lmCmdInsertTimeSignature: public lmScoreCommand
{
public:

    lmCmdInsertTimeSignature(lmVStaffCursor* pVCursor, const wxString& name, lmScoreDocument *pDoc,
                             int nBeats, int nBeatType, bool fVisible, bool fHistory=true);
    ~lmCmdInsertTimeSignature() {}

    //implementation of pure virtual methods in base class
    bool Do();
    bool UndoCommand();

protected:
    int                 m_nBeats;
    int                 m_nBeatType;
    bool                m_fVisible;
};



// Insert key signature command
//------------------------------------------------------------------------------------
class lmCmdInsertKeySignature: public lmScoreCommand
{
public:

    lmCmdInsertKeySignature(lmVStaffCursor* pVCursor, const wxString& name, lmScoreDocument *pDoc,
                            int nFifths, bool fMajor, bool fVisible, bool fHistory=true);
    ~lmCmdInsertKeySignature() {}

    //implementation of pure virtual methods in base class
    bool Do();
    bool UndoCommand();

protected:
    int                 m_nFifths;
    bool                m_fMajor;
    bool                m_fVisible;
};



// Insert note command
//------------------------------------------------------------------------------------
class lmCmdInsertNote: public lmScoreCommand
{
public:

    lmCmdInsertNote(lmVStaffCursor* pVCursor, const wxString& name, lmScoreDocument *pDoc,
					lmEPitchType nPitchType, int nStep, int nOctave, 
					lmENoteType nNoteType, float rDuration, int nDots, 
                    lmENoteHeads nNotehead, lmEAccidentals nAcc);
    ~lmCmdInsertNote();

    //implementation of pure virtual methods in base class
    bool Do();
    bool UndoCommand();

protected:
	lmENoteType		    m_nNoteType;
	lmEPitchType	    m_nPitchType;
	int		            m_nStep;
	int		            m_nOctave;
    int                 m_nDots;
	float			    m_rDuration;
	lmENoteHeads	    m_nNotehead;
	lmEAccidentals	    m_nAcc;

    lmVStaff*           m_pVStaff;      //affected VStaff
};



// Insert rest command
//------------------------------------------------------------------------------------
class lmCmdInsertRest: public lmScoreCommand
{
public:

    lmCmdInsertRest(lmVStaffCursor* pVCursor, const wxString& name, lmScoreDocument *pDoc,
					lmENoteType nNoteType, float rDuration, int nDots);
    ~lmCmdInsertRest();

    //implementation of pure virtual methods in base class
    bool Do();
    bool UndoCommand();

protected:
	lmENoteType		    m_nNoteType;
    int                 m_nDots;
	float			    m_rDuration;

    lmVStaff*           m_pVStaff;      //affected VStaff
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
    bool UndoCommand();

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
    bool UndoCommand();

protected:
	int				m_nSteps;
	lmNote*			m_pNote;
};

	void ChangeNoteAccidentals(int nSteps);



#endif    // __LM_SCORECOMMAND_H__        //to avoid nested includes
