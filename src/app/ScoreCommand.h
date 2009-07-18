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
#include "../score/ColStaffObjs.h"      //lmCursorState
class lmComponentObj;
class lmDocument;
class lmGMObject;
class lmScoreObj;
class lmVStaffCursor;
class lmVStaff;
class lmNote;

#define lmUNDOABLE      true        //log command for undo/redo
#define lmNO_UNDO       false       //the action is not undoable and should not
                                    //   be added to the command history


// base abstract class
class lmScoreCommand: public wxCommand
{
public:
    virtual ~lmScoreCommand();

    virtual bool Do()=0;
    virtual bool Undo();

protected:
    lmScoreCommand(const wxString& name, lmDocument *pDoc,
                   bool fUndoable = true, int nOptions=0, bool fUpdateViews = true);

    //common methods
    bool CommandDone(bool fScoreModified, int nOptions=0);
    bool CommandUndone(int nOptions=0);
    void LogCommand();
    lmVStaff* GetVStaff();
    lmStaffObj* GetStaffObj(lmIRef& nIRef);


    lmDocument*         m_pDoc;             //+
	bool				m_fDocModified;     //+
    bool                m_fUndoable;        //include command in undo/redo history
    bool                m_fUpdateViews;     //Update all views after doing/undoing the command
    int                 m_nOptions;         //+repaint hint options
    wxString            m_sOldSource;       //+source code to restore for undoing this command
};

// Move object command
//------------------------------------------------------------------------------------
class lmCmdMoveObject: public lmScoreCommand
{
public:
    lmCmdMoveObject(bool fUndoable, const wxString& sName,
                    lmDocument *pDoc, lmGMObject* pGMO,
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
    lmCmdDeleteStaffObj(bool fUndoable, const wxString& name,
                        lmDocument *pDoc, lmStaffObj* pSO);
    ~lmCmdDeleteStaffObj() {}

    //implementation of pure virtual methods in base class
    bool Do();

protected:
    lmIRef          m_nIRef;        //IRef for object to delete
};


// Delete the current selection
//------------------------------------------------------------------------------------
class lmCmdDeleteSelection: public lmScoreCommand
{
public:
    lmCmdDeleteSelection(bool fUndoable,
                         const wxString& sName, lmDocument *pDoc,
                         lmGMSelection* pSelection);
    ~lmCmdDeleteSelection();

    //implementation of pure virtual methods in base class
    bool Do();

protected:
    std::list<lmScoreCommand*>   m_Commands;     //commands to delete the selected objects
};


// Delete tie command
//------------------------------------------------------------------------------------
class lmCmdDeleteTie: public lmScoreCommand
{
public:
    lmCmdDeleteTie(bool fUndoable, const wxString& sName,
                   lmDocument *pDoc, lmNote* pEndNote);
    ~lmCmdDeleteTie() {}

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();

protected:
    lmIRef      m_nStartNoteIRef;       //start of tie note
    lmIRef      m_nEndNoteIRef;         //end of tie note
};


// Add tie command
//------------------------------------------------------------------------------------
class lmCmdAddTie: public lmScoreCommand
{
public:
    lmCmdAddTie(bool fUndoable, const wxString& sName, 
                lmDocument *pDoc, lmNote* pStartNote, lmNote* pEndNote);
    ~lmCmdAddTie() {}

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();

protected:
    lmIRef      m_nStartNoteIRef;       //start of tie note
    lmIRef      m_nEndNoteIRef;         //end of tie note
};


// Insert barline command
//------------------------------------------------------------------------------------
class lmCmdInsertBarline: public lmScoreCommand
{
public:

    lmCmdInsertBarline(bool fUndoable, const wxString& name,
                       lmDocument *pDoc, lmEBarline nType);
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

    lmCmdInsertClef(bool fUndoable, const wxString& name,
                    lmDocument *pDoc, lmEClefType nClefType);
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

    lmCmdInsertTimeSignature(bool fUndoable,
                             const wxString& name, lmDocument *pDoc,
                             int nBeats, int nBeatType, bool fVisible);
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

    lmCmdInsertKeySignature(bool fUndoable,
                            const wxString& name, lmDocument *pDoc,
                            int nFifths, bool fMajor, bool fVisible);
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

    lmCmdInsertNote(bool fUndoable,
                    const wxString& name, lmDocument *pDoc,
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
};



// Insert rest command
//------------------------------------------------------------------------------------
class lmCmdInsertRest: public lmScoreCommand
{
public:

    lmCmdInsertRest(bool fUndoable,
                    const wxString& name, lmDocument *pDoc,
					lmENoteType nNoteType, float rDuration, int nDots, int nVoice);
    ~lmCmdInsertRest();

    //implementation of pure virtual methods in base class
    bool Do();

protected:
	lmENoteType		    m_nNoteType;
    int                 m_nDots;
    int                 m_nVoice;
	float			    m_rDuration;
};


// Change note pitch command
//------------------------------------------------------------------------------------
class lmCmdChangeNotePitch: public lmScoreCommand
{
public:

    lmCmdChangeNotePitch(bool fUndoable,
                         const wxString& name, lmDocument *pDoc, lmNote* pNote,
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

    lmCmdChangeNoteAccidentals(bool fUndoable,
                               const wxString& name, lmDocument *pDoc,
                               lmGMSelection* pSelection, int nAcc);
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

    lmCmdChangeNoteRestDots(bool fUndoable,
                            const wxString& name, lmDocument *pDoc,
                            lmGMSelection* pSelection, int nDots);
    ~lmCmdChangeNoteRestDots();

    //implementation of pure virtual methods in base class
    bool Do();

protected:
	int                 m_nDots;
    std::list<lmIRef>   m_NoteRests;    //modified note/rests
};


// Delete tuplet command
//------------------------------------------------------------------------------------
class lmCmdDeleteTuplet: public lmScoreCommand
{
public:
    lmCmdDeleteTuplet(bool fUndoable, 
                      const wxString& sName, lmDocument *pDoc, lmNoteRest* pStartNR);
    ~lmCmdDeleteTuplet() {}

    //implementation of pure virtual methods in base class
    bool Do();

protected:
    lmIRef      m_nStartIRef;       //IRef for start nore/rest
};


// Add tuplet command
//------------------------------------------------------------------------------------
class lmCmdAddTuplet: public lmScoreCommand
{
public:
    lmCmdAddTuplet(bool fUndoable, const wxString& sName,
                   lmDocument *pDoc, lmGMSelection* pSelection, bool fShowNumber, int nNumber,
                   bool fBracket, lmEPlacement nAbove, int nActual, int nNormal);

    ~lmCmdAddTuplet();

    //implementation of pure virtual methods in base class
    bool Do();

protected:
    bool                    m_fShowNumber;
    bool                    m_fBracket;
    int                     m_nNumber;
    lmEPlacement            m_nAbove;
    int                     m_nActual;
    int                     m_nNormal;
    std::list<lmIRef>       m_NotesRests;
};


// break a beam command
//------------------------------------------------------------------------------------
class lmCmdBreakBeam: public lmScoreCommand
{
public:
    lmCmdBreakBeam(bool fUndoable, const wxString& sName,
                   lmDocument *pDoc, lmNoteRest* pBeforeNR);
    ~lmCmdBreakBeam();

    //implementation of pure virtual methods in base class
    bool Do();

protected:
    lmIRef         m_nBeforeNR;
};


// break a beam command
//------------------------------------------------------------------------------------
class lmCmdJoinBeam: public lmScoreCommand
{
public:
    lmCmdJoinBeam(bool fUndoable, const wxString& sName,
                  lmDocument *pDoc, lmGMSelection* pSelection);
    ~lmCmdJoinBeam() {}

    //implementation of pure virtual methods in base class
    bool Do();

protected:
    std::vector<lmIRef>     m_NotesRests;
};


// Change ScoreText properties
//------------------------------------------------------------------------------------
class lmCmdChangeText: public lmScoreCommand
{
public:

    lmCmdChangeText(bool fUndoable, const wxString& name,
                    lmDocument *pDoc, lmScoreText* pST, wxString& sText,
                    lmEHAlign nAlign, lmLocation tPos, lmTextStyle* pStyle,
                    int nHintOptions);
    ~lmCmdChangeText() {}

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();

protected:
    lmScoreText*        m_pST;

    //new values
    wxString            m_sText;
    lmEHAlign           m_nHAlign;
    lmLocation          m_tPos;
    lmTextStyle*        m_pStyle;

    //old values
    wxString            m_sOldText;
    lmEHAlign           m_nOldHAlign;
    lmLocation          m_tOldPos;
    lmTextStyle*        m_pOldStyle;
};


// Change page margin command
//------------------------------------------------------------------------------------
class lmCmdChangePageMargin: public lmScoreCommand
{
public:
    lmCmdChangePageMargin(bool fUndoable,
                          const wxString& sName, lmDocument *pDoc, lmGMObject* pGMO,
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
    lmCmdAttachNewText(bool fUndoable,
                       const wxString& name, lmDocument *pDoc, lmComponentObj* pAnchor);
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
    lmCmdAddNewTitle(bool fUndoable, lmDocument *pDoc);
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

    lmCmdChangeBarline(bool fUndoable, lmDocument *pDoc, lmBarline* pBL, lmEBarline nType, bool fVisible);
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

    lmCmdChangeMidiSettings(bool fUndoable, lmDocument *pDoc, lmInstrument* pInstr,
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
    lmCmdMoveNote(bool fUndoable, lmDocument *pDoc, lmNote* pNote, const lmUPoint& uPos, int nSteps);
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
    lmCmdMoveObjectPoints(bool fUndoable, const wxString& name, lmDocument *pDoc,
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
