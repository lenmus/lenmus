//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
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
class lmDocument;
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

    virtual bool Do()=0;
    virtual bool Undo();

protected:
    lmScoreCommand(const wxString& name, lmDocument *pDoc, lmVStaffCursor* pVCursor,
                   bool fHistory = true, int nOptions=0, bool fUpdateViews = true);


    //common methods
    bool CommandDone(bool fScoreModified, int nOptions=0);
    bool CommandUndone(int nOptions=0);

    //access to UndoInfo object
    inline lmUndoLog* GetUndoInfo() { return &m_UndoLog; }

    //save data for direct re-draw
    //void SetDirectRedrawData(lmScoreObj* pSCO, int nShapeIdx, lmPaper* pPaper);

    lmDocument*    m_pDoc;
	bool				m_fDocModified;
    bool                m_fHistory;         //include command in undo/redo history
    bool                m_fUpdateViews;     //Update all views after doing/undoing the command
    int                 m_nOptions;         //repaint hint options
    lmUndoLog           m_UndoLog;          //collection of undo/redo items
    lmUndoItem*         m_pUndoItem;        //undo item for this command
    lmVCursorState      m_tCursorState;     //VCursor state when issuing the command

    //data used only for direct re-draw, when the graphical model is not re-built
    lmScoreObj*         m_pSCO;             //object affected by the command
    int                 m_nShapeIdx;        //command affected shape
    lmPaper*            m_pPaper;           //current DC;
};


// Move object command
//------------------------------------------------------------------------------------
class lmCmdMoveObject: public lmScoreCommand
{
public:
    lmCmdMoveObject(const wxString& name, lmDocument *pDoc, lmGMObject* pGMO,
					const lmUPoint& uPos, bool fUpdateViews = true);
    ~lmCmdMoveObject() {}

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();

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
    lmCmdDeleteStaffObj(lmVStaffCursor* pVCursor, const wxString& name, lmDocument *pDoc,
                        lmStaffObj* pSO);
    ~lmCmdDeleteStaffObj();

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();

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
                         lmDocument *pDoc, lmGMSelection* pSelection);
    ~lmCmdDeleteSelection();

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();

protected:

    typedef struct
    {
        int             nObjType;       //type of object to delete
        bool            fObjDeleted;    //to control if the object must be deleted
        void*           pObj;           //the object to delete
        void*           pParm1;         //parameter 1
        void*           pParm2;         //parameter 2
    }
    lmDeletedSO;

    std::list<lmDeletedSO*>     m_ScoreObjects;     //deleted ScoreObjs
};


// Delete tie command
//------------------------------------------------------------------------------------
class lmCmdDeleteTie: public lmScoreCommand
{
public:
    lmCmdDeleteTie(const wxString& name, lmDocument *pDoc, lmNote* pEndNote);
    ~lmCmdDeleteTie();

    //implementation of pure virtual methods in base class
    bool Do();

protected:
    lmNote*     m_pEndNote;     //end of tie note
};


// Add tie command
//------------------------------------------------------------------------------------
class lmCmdAddTie: public lmScoreCommand
{
public:
    lmCmdAddTie(const wxString& name, lmDocument *pDoc, lmNote* pStartNote, lmNote* pEndNote);
    ~lmCmdAddTie();

    //implementation of pure virtual methods in base class
    bool Do();

protected:
    lmNote*     m_pStartNote;   //start of tie
    lmNote*     m_pEndNote;     //end of tie
};


// Insert barline command
//------------------------------------------------------------------------------------
class lmCmdInsertBarline: public lmScoreCommand
{
public:

    lmCmdInsertBarline(lmVStaffCursor* pVCursor, const wxString& name, lmDocument *pDoc,
                       lmEBarline nType);
    ~lmCmdInsertBarline() {}

    //implementation of pure virtual methods in base class
    bool Do();

protected:
    lmEBarline	        m_nBarlineType;
};


// Insert clef command
//------------------------------------------------------------------------------------
class lmCmdInsertClef: public lmScoreCommand
{
public:

    lmCmdInsertClef(lmVStaffCursor* pVCursor, const wxString& name, lmDocument *pDoc,
                    lmEClefType nClefType, bool fHistory=true);
    ~lmCmdInsertClef() {}

    //implementation of pure virtual methods in base class
    bool Do();

protected:
    lmEClefType         m_nClefType;

};



// Insert time signature command
//------------------------------------------------------------------------------------
class lmCmdInsertTimeSignature: public lmScoreCommand
{
public:

    lmCmdInsertTimeSignature(lmVStaffCursor* pVCursor, const wxString& name, lmDocument *pDoc,
                             int nBeats, int nBeatType, bool fVisible, bool fHistory=true);
    ~lmCmdInsertTimeSignature() {}

    //implementation of pure virtual methods in base class
    bool Do();

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

    lmCmdInsertKeySignature(lmVStaffCursor* pVCursor, const wxString& name, lmDocument *pDoc,
                            int nFifths, bool fMajor, bool fVisible, bool fHistory=true);
    ~lmCmdInsertKeySignature() {}

    //implementation of pure virtual methods in base class
    bool Do();

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

    lmCmdInsertNote(lmVStaffCursor* pVCursor, const wxString& name, lmDocument *pDoc,
					lmEPitchType nPitchType, int nStep, int nOctave,
					lmENoteType nNoteType, float rDuration, int nDots,
                    lmENoteHeads nNotehead, lmEAccidentals nAcc,
                    int nVoice, lmNote* pBaseOfChord, bool fTiedPrev);
    ~lmCmdInsertNote();

    //implementation of pure virtual methods in base class
    bool Do();

protected:
	lmENoteType		    m_nNoteType;
	lmEPitchType	    m_nPitchType;
	int		            m_nStep;
	int		            m_nOctave;
    int                 m_nDots;
	float			    m_rDuration;
	lmENoteHeads	    m_nNotehead;
	lmEAccidentals	    m_nAcc;
	int					m_nVoice;
	lmNote*				m_pBaseOfChord;
    bool                m_fTiedPrev;

    lmVStaff*           m_pVStaff;      //affected VStaff
};



// Insert rest command
//------------------------------------------------------------------------------------
class lmCmdInsertRest: public lmScoreCommand
{
public:

    lmCmdInsertRest(lmVStaffCursor* pVCursor, const wxString& name, lmDocument *pDoc,
					lmENoteType nNoteType, float rDuration, int nDots, int nVoice);
    ~lmCmdInsertRest();

    //implementation of pure virtual methods in base class
    bool Do();

protected:
	lmENoteType		    m_nNoteType;
    int                 m_nDots;
    int                 m_nVoice;
	float			    m_rDuration;

    lmVStaff*           m_pVStaff;      //affected VStaff
};


// Change note pitch command
//------------------------------------------------------------------------------------
class lmCmdChangeNotePitch: public lmScoreCommand
{
public:

    lmCmdChangeNotePitch(const wxString& name, lmDocument *pDoc, lmNote* pNote,
					int nSteps);
    ~lmCmdChangeNotePitch() {}

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();

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
                               lmDocument *pDoc, lmGMSelection* pSelection,
					           int nAcc);
    ~lmCmdChangeNoteAccidentals();

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();

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
                               lmDocument *pDoc, lmGMSelection* pSelection,
					           int nDots);
    ~lmCmdChangeNoteRestDots();

    //implementation of pure virtual methods in base class
    bool Do();

protected:
	int                     m_nDots;
    std::list<lmNoteRest*>  m_NoteRests;    //modified note/rests
};


// Delete tuplet command
//------------------------------------------------------------------------------------
class lmCmdDeleteTuplet: public lmScoreCommand
{
public:
    lmCmdDeleteTuplet(const wxString& name, lmDocument *pDoc, lmNoteRest* pStartNR);
    ~lmCmdDeleteTuplet();

    //implementation of pure virtual methods in base class
    bool Do();

protected:
    lmNoteRest*     m_pStartNR;
};


// Add tuplet command
//------------------------------------------------------------------------------------
class lmCmdAddTuplet: public lmScoreCommand
{
public:
    lmCmdAddTuplet(lmVStaffCursor* pVCursor, const wxString& name, lmDocument *pDoc,
                   lmGMSelection* pSelection, bool fShowNumber, int nNumber, bool fBracket,
                   lmEPlacement nAbove, int nActual, int nNormal);

    ~lmCmdAddTuplet();

    //implementation of pure virtual methods in base class
    bool Do();

protected:
    bool                        m_fShowNumber;
    bool                        m_fBracket;
    int                         m_nNumber;
    lmEPlacement                m_nAbove;
    int                         m_nActual;
    int                         m_nNormal;
    std::vector<lmNoteRest*>    m_NotesRests;
};


// break a beam command
//------------------------------------------------------------------------------------
class lmCmdBreakBeam: public lmScoreCommand
{
public:
    lmCmdBreakBeam(lmVStaffCursor* pVCursor, const wxString& name, lmDocument *pDoc,
                   lmNoteRest* pBeforeNR);
    ~lmCmdBreakBeam();

    //implementation of pure virtual methods in base class
    bool Do();

protected:
    lmNoteRest*         m_pBeforeNR;
};


// break a beam command
//------------------------------------------------------------------------------------
class lmCmdJoinBeam: public lmScoreCommand
{
public:
    lmCmdJoinBeam(lmVStaffCursor* pVCursor, const wxString& name, lmDocument *pDoc,
                  lmGMSelection* pSelection);
    ~lmCmdJoinBeam();

    //implementation of pure virtual methods in base class
    bool Do();

protected:
    std::vector<lmNoteRest*>    m_NotesRests;
};


// Change ScoreText properties
//------------------------------------------------------------------------------------
class lmCmdChangeText: public lmScoreCommand
{
public:

    lmCmdChangeText(lmVStaffCursor* pVCursor, const wxString& name,
                    lmDocument *pDoc, lmScoreText* pST, wxString& sText,
                    lmEHAlign nAlign, lmLocation tPos, lmTextStyle* pStyle,
                    int nHintOptions);
    ~lmCmdChangeText();

    //implementation of pure virtual methods in base class
    bool Do();

protected:
    lmScoreText*        m_pST;
    wxString            m_sText;
    lmEHAlign           m_nHAlign;
    lmLocation          m_tPos;
    lmTextStyle*        m_pStyle;

};


// Change page margin command
//------------------------------------------------------------------------------------
class lmCmdChangePageMargin: public lmScoreCommand
{
public:
    lmCmdChangePageMargin(const wxString& name, lmDocument *pDoc, lmGMObject* pGMO,
					      int nIdx, int nPage, lmLUnits uPos);
    ~lmCmdChangePageMargin() {}

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();

protected:
    void ChangeMargin(lmLUnits uPos);

    lmScore*        m_pScore;
	lmLUnits        m_uNewPos;
	lmLUnits        m_uOldPos;
    int             m_nIdx;
	int				m_nPage;
};


// Attach new text to an AuxObj / StaffObj
//------------------------------------------------------------------------------------
class lmCmdAttachNewText: public lmScoreCommand
{
public:
    lmCmdAttachNewText(const wxString& name, lmDocument *pDoc, lmComponentObj* pAnchor);
    ~lmCmdAttachNewText();

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();

protected:
	lmComponentObj*     m_pAnchor;
    lmTextItem*         m_pNewText;
    bool                m_fDeleteText;
};


// Add a new title to the score
//------------------------------------------------------------------------------------
class lmCmdAddNewTitle: public lmScoreCommand
{
public:
    lmCmdAddNewTitle(lmDocument *pDoc);
    ~lmCmdAddNewTitle();

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();

protected:
    lmScoreTitle*		m_pNewTitle;
    bool                m_fDeleteTitle;
};


// Change barline properties
//------------------------------------------------------------------------------------
class lmCmdChangeBarline: public lmScoreCommand
{
public:

    lmCmdChangeBarline(lmDocument *pDoc, lmBarline* pBL, lmEBarline nType, bool fVisible);
    ~lmCmdChangeBarline();

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();

protected:
    lmBarline*			m_pBL;
    lmEBarline			m_nType;
    lmEBarline			m_nOldType;
	bool				m_fVisible;
	bool				m_fOldVisible;

};


// Change MIDI settings for a given instrument
//------------------------------------------------------------------------------------
class lmCmdChangeMidiSettings: public lmScoreCommand
{
public:

    lmCmdChangeMidiSettings(lmDocument *pDoc, lmInstrument* pInstr,
                            int nMidiChannel, int nMidiInstr);
    ~lmCmdChangeMidiSettings();

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();

protected:
    lmInstrument*       m_pInstr;
    int			        m_nMidiChannel;
    int			        m_nMidiInstr;
    int			        m_nOldMidiChannel;
    int			        m_nOldMidiInstr;

};


// Move note and change its pitch command
//------------------------------------------------------------------------------------
class lmCmdMoveNote: public lmScoreCommand
{
public:
    lmCmdMoveNote(lmDocument *pDoc, lmNote* pNote, const lmUPoint& uPos, int nSteps);
    ~lmCmdMoveNote() {}

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();

protected:
    lmLUnits        m_uxPos;
    lmLUnits        m_uxOldPos;        // for Undo
	lmNote*			m_pNote;
    int             m_nSteps;
};


// Move an object
//------------------------------------------------------------------------------------
class lmCmdMoveObjectPoints: public lmScoreCommand
{
public:
    lmCmdMoveObjectPoints(const wxString& name, lmDocument *pDoc,
                          lmGMObject* pGMO, lmUPoint uShifts[],
                          int nNumPoints, bool fUpdateViews);
    ~lmCmdMoveObjectPoints();

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();

protected:
    lmScoreObj*     m_pSCO;
    int             m_nShapeIdx;
    int             m_nNumPoints;
    lmUPoint*       m_pShifts;
};


#endif    // __LM_SCORECOMMAND_H__        //to avoid nested includes
