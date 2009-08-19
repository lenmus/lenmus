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
#include "../score/FiguredBass.h"       //lmFiguredBassInfo struct

class lmComponentObj;
class lmDocument;
class lmGMObject;
class lmScoreObj;
class lmScoreCursor;
class lmVStaff;
class lmNote;
class lmBezier;

//predefined values for flag 'fNormalCmd'
#define lmCMD_NORMAL    true
#define lmCMD_HIDDEN    false
//  true  - Normal command. The command will be logged in the undo/redo chain and,
//          if applicable, the score will be saved for undo.
//  false - Hidden command. The command is executed but the command will not be
//          added to the command history and, if applicable, the score will not
//          be saved for undo. After command execution the screen is not updated.
//          This option is usefull for building commands by chaining other
//          commnads. The main command will be undoable and all atomic commands
//          will not. See, for example, lmCmdDeleteSelection.


// base abstract class
class lmScoreCommand: public wxCommand
{
public:
    virtual ~lmScoreCommand();

    virtual bool Do()=0;
    virtual bool Undo();

protected:
    lmScoreCommand(const wxString& name, lmDocument *pDoc,
                   bool fNormalCmd = true, bool fDoLayout = true);

    //common methods
    bool CommandDone(bool fCmdSuccess, int nUpdateHints=0);
    bool CommandUndone(int nUpdateHints=0);
    void RestoreCursorAndPrepareForUndo();
    inline void LogScoreState() {}
    void RestoreCursor();
    lmVStaff* GetVStaff();
    lmScoreObj* GetScoreObj(long nID);


    lmDocument*         m_pDoc;             //
	bool				m_fDocModified;     //
    bool                m_fUndoable;        //include command in undo/redo history
    bool                m_fDoLayout;        //Update all views after doing/undoing the command
    wxString            m_sOldSource;       //source code to restore for undoing this command
    lmCursorState       m_CursorState;      //cursor state when issuing the command
};

// Move object command
//------------------------------------------------------------------------------------
class lmCmdMoveObject: public lmScoreCommand
{
public:
    lmCmdMoveObject(bool fNormalCmd, const wxString& sName,
                    lmDocument *pDoc, lmGMObject* pGMO,
					const lmUPoint& uPos);
    ~lmCmdMoveObject() {}

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();

protected:
    lmLocation      m_tPos;
    lmLocation		m_tOldPos;        // for Undo
	long		    m_nObjectID;
    int             m_nShapeIdx;
};


// Delete staffobj command
//------------------------------------------------------------------------------------
class lmCmdDeleteStaffObj: public lmScoreCommand
{
public:
    lmCmdDeleteStaffObj(bool fNormalCmd, const wxString& name,
                        lmDocument *pDoc, lmStaffObj* pSO);
    ~lmCmdDeleteStaffObj() {}

    //implementation of pure virtual methods in base class
    bool Do();

protected:
    long        m_nObjID;       //ID for object to delete
};


// Delete the current selection
//------------------------------------------------------------------------------------
class lmCmdDeleteSelection: public lmScoreCommand
{
public:
    lmCmdDeleteSelection(bool fNormalCmd,
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
    lmCmdDeleteTie(bool fNormalCmd, const wxString& sName,
                   lmDocument *pDoc, lmNote* pEndNote);
    ~lmCmdDeleteTie() {}

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();

protected:
    long        m_nStartNoteID;         //start of tie note
    long        m_nEndNoteID;           //end of tie note
    long        m_nTieID;               //id of tie to delete
    lmBezier    m_Bezier[2];            //bezier points data
};


// Add tie command
//------------------------------------------------------------------------------------
class lmCmdAddTie: public lmScoreCommand
{
public:
    lmCmdAddTie(bool fNormalCmd, const wxString& sName, 
                lmDocument *pDoc, lmNote* pStartNote, lmNote* pEndNote);
    ~lmCmdAddTie() {}

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();

protected:
    long        m_nStartNoteID;         //start of tie note
    long        m_nEndNoteID;           //end of tie note
};


// Insert barline command
//------------------------------------------------------------------------------------
class lmCmdInsertBarline: public lmScoreCommand
{
public:

    lmCmdInsertBarline(bool fNormalCmd, const wxString& name,
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

    lmCmdInsertClef(bool fNormalCmd, const wxString& name,
                    lmDocument *pDoc, lmEClefType nClefType);
    ~lmCmdInsertClef() {}

    //implementation of pure virtual methods in base class
    bool Do();

protected:
    lmEClefType         m_nClefType;
};


// Insert figured bass command
//------------------------------------------------------------------------------------
class lmCmdInsertFiguredBass: public lmScoreCommand
{
public:

    lmCmdInsertFiguredBass(bool fNormalCmd, lmDocument *pDoc, wxString& sFigBass);
    ~lmCmdInsertFiguredBass() {}

    //implementation of pure virtual methods in base class
    bool Do();

protected:
    lmFiguredBassInfo   m_tFBInfo[lmFB_MAX_INTV+1];
    bool                m_fFirstTime;
};


// Insert time signature command
//------------------------------------------------------------------------------------
class lmCmdInsertTimeSignature: public lmScoreCommand
{
public:

    lmCmdInsertTimeSignature(bool fNormalCmd,
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

    lmCmdInsertKeySignature(bool fNormalCmd,
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

    lmCmdInsertNote(bool fNormalCmd,
                    const wxString& name, lmDocument *pDoc,
					lmEPitchType nPitchType, int nStep, int nOctave,
					lmENoteType nNoteType, float rDuration, int nDots,
                    lmENoteHeads nNotehead, lmEAccidentals nAcc,
                    int nVoice, lmNote* pBaseOfChord, bool fTiedPrev,
                    lmEStemType nStem);
    ~lmCmdInsertNote();

    //implementation of pure virtual methods in base class
    bool Do();

protected:
	lmENoteType		    m_nNoteType;
	lmEPitchType	    m_nPitchType;
    lmEStemType         m_nStem;
	int		            m_nStep;
	int		            m_nOctave;
    int                 m_nDots;
	float			    m_rDuration;
	lmENoteHeads	    m_nNotehead;
	lmEAccidentals	    m_nAcc;
	int					m_nVoice;
	long				m_nBaseOfChordID;
    bool                m_fTiedPrev;
};



// Insert rest command
//------------------------------------------------------------------------------------
class lmCmdInsertRest: public lmScoreCommand
{
public:

    lmCmdInsertRest(bool fNormalCmd,
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

    lmCmdChangeNotePitch(bool fNormalCmd,
                         const wxString& name, lmDocument *pDoc, lmNote* pNote,
					     int nSteps);
    ~lmCmdChangeNotePitch() {}

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();

protected:
	int				m_nSteps;
	long			m_nNoteID;
};


// Change note accidentals command
//------------------------------------------------------------------------------------
class lmCmdChangeNoteAccidentals: public lmScoreCommand
{
public:

    lmCmdChangeNoteAccidentals(bool fNormalCmd,
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
        long			nNoteID;      //note to modify
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

    lmCmdChangeNoteRestDots(bool fNormalCmd,
                            const wxString& name, lmDocument *pDoc,
                            lmGMSelection* pSelection, int nDots);
    ~lmCmdChangeNoteRestDots();

    //implementation of pure virtual methods in base class
    bool Do();

protected:
	int                 m_nDots;
    std::list<long>     m_NoteRests;    //modified note/rests
};


// Delete tuplet command
//------------------------------------------------------------------------------------
class lmCmdDeleteTuplet: public lmScoreCommand
{
public:
    lmCmdDeleteTuplet(bool fNormalCmd, 
                      const wxString& sName, lmDocument *pDoc, lmNoteRest* pStartNR);
    ~lmCmdDeleteTuplet() {}

    //implementation of pure virtual methods in base class
    bool Do();

protected:
    long        m_nStartID;     //ID for start nore/rest
};


// Add tuplet command
//------------------------------------------------------------------------------------
class lmCmdAddTuplet: public lmScoreCommand
{
public:
    lmCmdAddTuplet(bool fNormalCmd, const wxString& sName,
                   lmDocument *pDoc, lmGMSelection* pSelection, bool fShowNumber, int nNumber,
                   bool fBracket, lmEPlacement nAbove, int nActual, int nNormal);

    ~lmCmdAddTuplet();

    //implementation of pure virtual methods in base class
    bool Do();

protected:
    bool                m_fShowNumber;
    bool                m_fBracket;
    int                 m_nNumber;
    lmEPlacement        m_nAbove;
    int                 m_nActual;
    int                 m_nNormal;
    std::list<long>     m_NotesRests;
};


// break a beam command
//------------------------------------------------------------------------------------
class lmCmdBreakBeam: public lmScoreCommand
{
public:
    lmCmdBreakBeam(bool fNormalCmd, const wxString& sName,
                   lmDocument *pDoc, lmNoteRest* pBeforeNR);
    ~lmCmdBreakBeam();

    //implementation of pure virtual methods in base class
    bool Do();

protected:
    long         m_nBeforeNR;
};


// break a beam command
//------------------------------------------------------------------------------------
class lmCmdJoinBeam: public lmScoreCommand
{
public:
    lmCmdJoinBeam(bool fNormalCmd, const wxString& sName,
                  lmDocument *pDoc, lmGMSelection* pSelection);
    ~lmCmdJoinBeam() {}

    //implementation of pure virtual methods in base class
    bool Do();

protected:
    std::vector<long>     m_NotesRests;
};


// Change ScoreText properties
//------------------------------------------------------------------------------------
class lmCmdChangeText: public lmScoreCommand
{
public:

    lmCmdChangeText(bool fNormalCmd, const wxString& name,
                    lmDocument *pDoc, lmScoreText* pST, wxString& sText,
                    lmEHAlign nAlign, lmLocation tPos, lmTextStyle* pStyle,
                    int nHintOptions);
    ~lmCmdChangeText() {}

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();

protected:
    long                m_nTextID;

    //new values
    wxString            m_sText;
    lmEHAlign           m_nHAlign;
    lmLocation          m_tPos;
    lmTextStyle         m_Style;

    //old values
    wxString            m_sOldText;
    lmEHAlign           m_nOldHAlign;
    lmLocation          m_tOldPos;
    lmTextStyle         m_OldStyle;
};


// Change page margin command
//------------------------------------------------------------------------------------
class lmCmdChangePageMargin: public lmScoreCommand
{
public:
    lmCmdChangePageMargin(bool fNormalCmd,
                          const wxString& sName, lmDocument *pDoc, lmGMObject* pGMO,
					      int nIdx, int nPage, lmLUnits uPos);
    ~lmCmdChangePageMargin() {}

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();

protected:
    void ChangeMargin(lmLUnits uPos);

	lmLUnits        m_uNewPos;
	lmLUnits        m_uOldPos;
    int             m_nIdx;
	int				m_nPage;
};


// Attach a text item to an AuxObj / StaffObj
//------------------------------------------------------------------------------------
class lmCmdAttachText: public lmScoreCommand
{
public:
    lmCmdAttachText(bool fNormalCmd, lmDocument *pDoc, wxString& sText,
                    lmTextStyle* pStyle, lmEHAlign nAlign,
                    lmComponentObj* pAnchor);
    ~lmCmdAttachText() {}

    //implementation of pure virtual methods in base class
    bool Do();

protected:
	long                m_nAnchorID;
    long                m_nTextID;
    wxString            m_sText;
    lmTextStyle         m_Style;
    lmEHAlign           m_nAlign;
};


// Add a new title to the score
//------------------------------------------------------------------------------------
class lmCmdAddTitle: public lmScoreCommand
{
public:
    lmCmdAddTitle(bool fNormalCmd, lmDocument *pDoc, wxString& sText,
                  lmTextStyle* pStyle, lmEHAlign nAlign);
    ~lmCmdAddTitle() {}

    //implementation of pure virtual methods in base class
    bool Do();

protected:
    long                m_nTitleID;
    wxString            m_sText;
    lmTextStyle         m_Style;
    lmEHAlign           m_nAlign;
};


// Change barline properties
//------------------------------------------------------------------------------------
class lmCmdChangeBarline : public lmScoreCommand
{
public:

    lmCmdChangeBarline(bool fNormalCmd, lmDocument *pDoc, lmBarline* pBL, lmEBarline nType, bool fVisible);
    ~lmCmdChangeBarline();

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();

protected:
    long			    m_nBarlineID;
    lmEBarline			m_nType;
    lmEBarline			m_nOldType;
	bool				m_fVisible;
	bool				m_fOldVisible;

};


// Change figured bass properties
//------------------------------------------------------------------------------------
class lmCmdChangeFiguredBass : public lmScoreCommand
{
public:
    lmCmdChangeFiguredBass(bool fNormalCmd, lmDocument *pDoc, lmFiguredBass* pFB, 
                           wxString& sFigBass);
    ~lmCmdChangeFiguredBass();

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();

protected:
    long			    m_nFigBasID;
	wxString            m_sFigBass;
    lmFiguredBassInfo   m_tOldInfo[lmFB_MAX_INTV+1];         //intervals 2..13, indexes 0 & 1 not used

};


// Change MIDI settings for a given instrument
//------------------------------------------------------------------------------------
class lmCmdChangeMidiSettings : public lmScoreCommand
{
public:

    lmCmdChangeMidiSettings(bool fNormalCmd, lmDocument *pDoc, lmInstrument* pInstr,
                            int nMidiChannel, int nMidiInstr);
    ~lmCmdChangeMidiSettings() {}

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();

protected:
    long        m_nInstrID;
    int         m_nMidiChannel;
    int         m_nMidiInstr;
    int         m_nOldMidiChannel;
    int         m_nOldMidiInstr;

};


// Move note and change its pitch command
//------------------------------------------------------------------------------------
class lmCmdMoveNote: public lmScoreCommand
{
public:
    lmCmdMoveNote(bool fNormalCmd, lmDocument *pDoc, lmNote* pNote, const lmUPoint& uPos, int nSteps);
    ~lmCmdMoveNote() {}

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();

protected:
    lmLUnits        m_uxPos;
    lmLUnits        m_uxOldPos;        // for Undo
	long			m_nNoteID;
    int             m_nSteps;
};


// Move an object
//------------------------------------------------------------------------------------
class lmCmdMoveObjectPoints: public lmScoreCommand
{
public:
    lmCmdMoveObjectPoints(bool fNormalCmd, const wxString& name, lmDocument *pDoc,
                          lmGMObject* pGMO, lmUPoint uShifts[],
                          int nNumPoints, bool fDoLayout);
    ~lmCmdMoveObjectPoints();

    //implementation of pure virtual methods in base class
    bool Do();
    bool Undo();

protected:
    long            m_nObjID;
    int             m_nShapeIdx;
    int             m_nNumPoints;
    lmUPoint*       m_pShifts;
};


#endif    // __LM_SCORECOMMAND_H__        //to avoid nested includes
