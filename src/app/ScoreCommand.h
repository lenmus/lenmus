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


// Delete staffobj command
//------------------------------------------------------------------------------------
class lmCmdDeleteStaffObj: public lmScoreCommand
{
public:
    lmCmdDeleteStaffObj(lmVStaffCursor* pVCursor, const wxString& name, lmScoreDocument *pDoc,
                        lmStaffObj* pSO);
    ~lmCmdDeleteStaffObj();

    //implementation of pure virtual methods in base class
    bool Do();
    bool UndoCommand();

protected:
    lmVStaff*           m_pVStaff;      //affected VStaff
    lmStaffObj*         m_pSO;          //deleted note
    bool                m_fDeleteSO;    //to control if m_pSO must be deleted
};


// Delete the current selection
//------------------------------------------------------------------------------------
class lmCmdDeleteSelection: public lmScoreCommand
{
public:
    lmCmdDeleteSelection(lmVStaffCursor* pVCursor, const wxString& name,
                         lmScoreDocument *pDoc, lmGMSelection* pSelection);
    ~lmCmdDeleteSelection();

    //implementation of pure virtual methods in base class
    bool Do();
    bool UndoCommand();

protected:

    typedef struct
    {
        int             nObjType;       //type of object to delete
        bool            fSODeleted;     //to control if the SO must be deleted
        lmScoreObj*     pSO;            //the ScoreObj to delete
        void*           pParm1;         //parameter 1
        void*           pParm2;         //parameter 2
    } lmDeletedSO;

    std::list<lmDeletedSO*>     m_ScoreObjects;     //deleted ScoreObjs
};


// Delete tie command
//------------------------------------------------------------------------------------
class lmCmdDeleteTie: public lmScoreCommand
{
public:
    lmCmdDeleteTie(const wxString& name, lmScoreDocument *pDoc, lmNote* pEndNote);
    ~lmCmdDeleteTie();

    //implementation of pure virtual methods in base class
    bool Do();
    bool UndoCommand();

protected:
    lmNote*     m_pEndNote;     //end of tie note
};


// Add tie command
//------------------------------------------------------------------------------------
class lmCmdAddTie: public lmScoreCommand
{
public:
    lmCmdAddTie(const wxString& name, lmScoreDocument *pDoc, lmNote* pStartNote, lmNote* pEndNote);
    ~lmCmdAddTie();

    //implementation of pure virtual methods in base class
    bool Do();
    bool UndoCommand();

protected:
    lmNote*     m_pStartNote;   //start of tie
    lmNote*     m_pEndNote;     //end of tie
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
                    lmENoteHeads nNotehead, lmEAccidentals nAcc,
                    bool fTiedPrev);
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
    bool                m_fTiedPrev;

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

    lmCmdChangeNoteAccidentals(lmVStaffCursor* pVCursor, const wxString& name,
                               lmScoreDocument *pDoc, lmGMSelection* pSelection,
					           int nAcc);
    ~lmCmdChangeNoteAccidentals();

    //implementation of pure virtual methods in base class
    bool Do();
    bool UndoCommand();

protected:
	int                 m_nAcc;

    typedef struct
    {
        lmNote*         pNote;          //note to modify
        int             nAcc;           //current accidentals
    }
    lmCmdNoteData;

    std::list<lmCmdNoteData*>  m_Notes;    //modified notes
};


// Change note dots command
//------------------------------------------------------------------------------------
class lmCmdChangeNoteRestDots: public lmScoreCommand
{
public:

    lmCmdChangeNoteRestDots(lmVStaffCursor* pVCursor, const wxString& name,
                               lmScoreDocument *pDoc, lmGMSelection* pSelection,
					           int nDots);
    ~lmCmdChangeNoteRestDots();

    //implementation of pure virtual methods in base class
    bool Do();
    bool UndoCommand();

protected:
	int                     m_nDots;
    std::list<lmNoteRest*>  m_NoteRests;    //modified note/rests
};




#endif    // __LM_SCORECOMMAND_H__        //to avoid nested includes
