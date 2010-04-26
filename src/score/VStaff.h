//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2010 LenMus project
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

#ifndef __LM_VSTAFF_H__        //to avoid nested includes
#define __LM_VSTAFF_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "VStaff.cpp"
#endif

#include "Score.h"
#include "FiguredBass.h"
#include "StaffObjIterator.h"

class lmScore;
class lmSpacer;
class lmInstrument;
class lmRest;
class lmNote;
class lmSOControl;
class lmTextItem;
class lmMetronomeMark;
class lmBeamInfo;
class lmColStaffObjsTest;



//----------------------------------------------------------------------------------------
// lmVStaff 
//----------------------------------------------------------------------------------------

class lmVStaff : public lmScoreObj
{
public:
    lmVStaff(lmScore* pScore, lmInstrument* pInstr, long nID, long nStaffID);
    ~lmVStaff();

	//---- virtual methods of base class -------------------------

    // units conversion
    lmLUnits TenthsToLogical(lmTenths nTenths);
    lmTenths LogicalToTenths(lmLUnits uUnits);

    inline lmScore* GetScore() { return m_pScore; }


	//---- specific methods of this class ------------------------

	//Adding StaffObs (at the end)
    lmStaff*    AddStaff(int nNumLines=5, long nID=lmNEW_ID, lmLUnits uSpacing=0.0f,
                         lmLUnits uDistance=0.0f, lmLUnits uLineThickness=0.0f);
    lmClef*     AddClef(lmEClefType nClefType, int nStaff = 1, bool fVisible = true,
                        long nID = lmNEW_ID);

    lmTimeSignature* AddTimeSignature(int nBeats, int nBeatType,    //for type eTS_Normal
                                      bool fVisible = true, long nID=lmNEW_ID);    
    lmTimeSignature* AddTimeSignature(lmETimeSignature nTimeSign,   //for type eTS_Normal
                                      bool fVisible = true, long nID=lmNEW_ID);    
    lmTimeSignature* AddTimeSignature(lmETimeSignatureType nType,   //for types eTS_Common, eTS_Cut and eTS_SenzaMisura
                                      bool fVisible = true, long nID=lmNEW_ID);    
    lmTimeSignature* AddTimeSignature(int nSingleNumber,            //for type eTS_SingleNumber
                                      bool fVisible = true, long nID=lmNEW_ID);    
    lmTimeSignature* AddTimeSignature(int nNumBeats, int nBeats[], int nBeatType,   //for type eTS_Composite
                                      bool fVisible = true, long nID=lmNEW_ID);    
    lmTimeSignature* AddTimeSignature(int nNumFractions, int nBeats[], int nBeatType[], //for type eTS_Multiple
                                      bool fVisible = true, long nID=lmNEW_ID);    

    lmKeySignature* AddKeySignature(int nFifths, bool fMajor, bool fVisible = true,
                                    long nID = lmNEW_ID);
    lmKeySignature* AddKeySignature(lmEKeySignatures nKeySignature,
                                    bool fVisible = true, long nID = lmNEW_ID);

    lmRest*     AddRest(long nID, lmENoteType nNoteType, float rDuration, int nDots,
                      int nStaff, int nVoice = 1,
					  bool fVisible = true,
                      bool fBeamed = false, lmTBeamInfo BeamInfo[] = NULL);

    lmNote*     AddNote(long nID, lmEPitchType nPitchType,
                    int nStep, int nOctave, int nAlter,
                    lmEAccidentals nAccidentals,
                    lmENoteType nNoteType, float rDuration, int nDots,
                    int nStaff, int nVoice = 1,
					bool fVisible = true,
                    bool fBeamed = false, lmTBeamInfo BeamInfo[] = NULL,
                    lmNote* pBaseOfChord = (lmNote*)NULL,
                    bool fTie = false,
                    lmEStemType nStem = lmSTEM_DEFAULT);

    lmBarline*  AddBarline(lmEBarline nType = lm_eBarlineSimple, bool fVisible = true,
                           long nID = lmNEW_ID);

    lmFiguredBass* AddFiguredBass(lmFiguredBassData* pFBData, long nID = lmNEW_ID);

    lmMetronomeMark* AddMetronomeMark(int nTicksPerMinute,
                            bool fParentheses = false, bool fVisible = true,
                            long nID = lmNEW_ID);
    lmMetronomeMark* AddMetronomeMark(lmENoteType nLeftNoteType, int nLeftDots,
                            lmENoteType nRightNoteType, int nRightDots,
                            bool fParentheses = false, bool fVisible = true,
                            long nID = lmNEW_ID);
    lmMetronomeMark* AddMetronomeMark(lmENoteType nLeftNoteType, int nLeftDots,
                            int nTicksPerMinute,
                            bool fParentheses = false, bool fVisible = true,
                            long nID = lmNEW_ID);

    lmSOControl* AddNewSystem(long nID = lmNEW_ID);

    lmSpacer* AddSpacer(lmTenths nWidth, long nID = lmNEW_ID, int nStaff = 1);

    lmStaffObj* AddAnchorObj(long nID = lmNEW_ID);

    lmTextItem* AddText(wxString& sText, lmEHAlign nHAlign, lmFontInfo& oFontData,
                        lmStaffObj* pAnchor, long nID = lmNEW_ID);
    lmTextItem* AddText(wxString& sText, lmEHAlign nHAlign, lmTextStyle* pStyle,
                        lmStaffObj* pAnchor, long nID = lmNEW_ID);

    lmBeam* CreateBeam(std::vector<lmBeamInfo*>& cBeamInfo);


	//Edition commands

	    //--- inserting StaffObs
    lmBarline* Cmd_InsertBarline(lmEBarline nType = lm_eBarlineSimple, bool fVisible = true);
	lmClef* Cmd_InsertClef(lmEClefType nClefType, bool fVisible = true);
    lmFiguredBass* Cmd_InsertFiguredBass(lmFiguredBassData* pFBData);
    lmFiguredBassLine* Cmd_InsertFBLine();
    lmKeySignature* Cmd_InsertKeySignature(int nFifths, bool fMajor, bool fVisible = true);
	lmNote* Cmd_InsertNote(lmEPitchType nPitchType, int nStep,
					       int nOctave, lmENoteType nNoteType, float rDuration, int nDots,
					       lmENoteHeads nNotehead, lmEAccidentals nAcc, 
                           int nVoice, lmNote* pBaseOfChord, bool fTiedPrev,
                           lmEStemType nStem, bool fAutoBar);

	lmRest* Cmd_InsertRest(lmENoteType nNoteType, float rDuration,
                           int nDots, int nVoice, bool fAutoBar);

    lmTimeSignature* Cmd_InsertTimeSignature(int nBeats, int nBeatType, bool fVisible = true);


        //--- deleting StaffObjs
    bool Cmd_DeleteStaffObj(lmStaffObj* pSO);
    bool Cmd_DeleteClef(lmClef* pClef, int nAction);
    bool Cmd_DeleteKeySignature(lmKeySignature* pKS, int nAction);
    bool Cmd_DeleteTimeSignature(lmTimeSignature* pTS);

        //--- deleting AuxObjs
    bool Cmd_DeleteTuplet(lmNoteRest* pStartNote);

        //--- Modifying staffobjs/AuxObjs
    void Cmd_ChangeDots(lmNoteRest* pNR, int nDots);
    void Cmd_BreakBeam(lmNoteRest* pBeforeNR);
    void Cmd_JoinBeam(std::vector<lmNoteRest*>& notes);
    bool Cmd_DeleteBeam(lmNoteRest* pNR);

        //--- Adding other markup
    void Cmd_AddTie(lmNote* pStartNote, lmNote* pEndNote);
    bool Cmd_AddTuplet(std::vector<lmNoteRest*>& notes,
                       bool fShowNumber, int nNumber, bool fBracket,
                       lmEPlacement nAbove, int nActual, int nNormal);


    //staves modification (nStaff = 1..n)
    void SetStaffLineSpacing(int nStaff, lmLUnits uSpacing);
    void SetStaffLineThickness(int nStaff, lmLUnits uTickness);
    void SetStaffNumLines(int nStaff, int nLines);
    void SetStaffDistance(int nStaff, lmLUnits uDistance);


    //error management
    inline wxString GetErrorMessage() { return m_sErrorMsg; }


    bool ShiftTime(float rTimeShift);


    // rendering methods
    lmLUnits LayoutStaffLines(lmBoxSystem* pBoxSystem, lmInstrument* pInstr,
                              lmLUnits xFrom, lmLUnits xTo, lmLUnits yPos);
    lmLUnits GetVStaffHeight();
    void SetUpFonts(lmPaper* pPaper);
    lmLUnits GetStaffLineThick(int nStaff);
    inline bool HideStaffLines() { return GetOptionBool(_T("StaffLines.Hide")); }

    //ties
    lmNote* FindPossibleStartOfTie(lmNote* pEndNote, bool fNotAdded = false);
    lmNote* FindPossibleEndOfTie(lmNote* pStartNote);

	//units conversion
    lmLUnits TenthsToLogical(lmTenths nTenths, int nStaff);
	lmTenths LogicalToTenths(lmLUnits uUnits, int nStaff);

    lmStaff* GetStaff(int nStaff);
    lmLUnits GetStaffOffset(int nStaff);
    lmLUnits GetYTop();
    lmLUnits GetYBottom();


    //Source code methods
    wxString SourceLDP(int nIndent, bool fUndoData);
    wxString SourceXML(int nIndent);

    // restricted methods
    lmSOIterator* CreateIterator();    //for lmFormatter objects
    inline lmSOIterator* CreateIteratorTo(lmStaffObj* pSO) 
                                        { return m_cStaffObjs.CreateIteratorTo(pSO); }

    //for navigation along staves
    inline int GetNumStaves() const { return m_nNumStaves; }
    lmStaff* GetFirstStaff();
    lmStaff* GetNextStaff();
    lmStaff* GetLastStaff();

    //context management
	inline lmContext* GetCurrentContext(lmStaffObj* pSO, int nStaff = -1)
                            { return m_cStaffObjs.GetCurrentContext(pSO, nStaff); }
	inline lmContext* NewUpdatedContext(int nStaff, lmStaffObj* pSO)
                            { return m_cStaffObjs.NewUpdatedContext(nStaff, pSO); }
	inline lmContext* NewUpdatedLastContext(int nStaff)
                            { return m_cStaffObjs.NewUpdatedLastContext(nStaff); }
	inline lmContext* GetLastContext(int nStaff)
                            { return m_cStaffObjs.GetLastContext(nStaff); }
    inline lmContext* GetStartOfSegmentContext(int nMeasure, int nStaff)
                            { return m_cStaffObjs.GetStartOfSegmentContext(nMeasure, nStaff); }

    //contexts related
    void OnAccidentalsUpdated(lmNote* pStartNote, int nStaff, int nStep, int nNewAccidentals);
	int GetUpdatedContextAccidentals(lmStaffObj* pThisSO, int nStep);
    lmTimeSignature* GetApplicableTimeSignature();

    //measures related
    int GetNumMeasures();
    float GetCurrentMesureDuration();

    //sound related methods
    lmSoundManager* ComputeMidiEvents(int nChannel);

    //renderization related methods
    lmBarline* GetBarlineOfMeasure(int nMeasure, lmLUnits* pPos = (lmLUnits*)NULL);
    lmBarline* GetBarlineOfLastNonEmptyMeasure(lmLUnits* pxPos, lmLUnits* pyPos);
    void SetSpaceBeforeClef(lmLUnits nSpace) { m_nSpaceBeforeClef = nSpace; }
    lmLUnits GetSpaceBeforeClef() { return m_nSpaceBeforeClef; }

    //general StaffObjs management
    void AssignID(lmStaffObj* pSO);

    //miscellaneous
    inline bool IsGlobalStaff() const { return (m_pInstrument == (lmInstrument*)NULL); }
	inline lmInstrument* GetOwnerInstrument() const { return m_pInstrument; }
    int GetNumberOfStaff(lmStaff* pStaff);       //1..n
    int GetNumInstr();

    //cursor management and cursor related
    inline lmContext* GetContextAtCursorPoint() { return GetCursor()->GetCurrentContext(); }
    inline int GetCursorStaffNum() { return GetCursor()->GetCursorNumStaff(); }
    inline lmStaffObj* GetCursorStaffObj() { return GetCursor()->GetStaffObj(); }
    inline int GetCursorSegmentNum() { return GetCursor()->GetSegment(); }
    inline float GetCursorTimepos() { return GetCursor()->GetCursorTime(); }

    //Debug methods
    wxString Dump();

    //to clarify operations
    int AskUserAboutClef();
    int AskUserAboutKey();
    bool CheckIfNotesAffectedByAddingClef(lmEClefType nClefType);
    bool CheckIfNotesAffectedByDeletingClef();
    bool CheckIfNotesAffectedByKey(bool fSkip);


private:
    friend class lmColStaffObjs;
    friend class lmInstrument;      //access to GetCollection()
    friend class lmColStaffObjsTestFixture;     //tests
    friend class lmScoreCursorTestFixture;      //tests

    //cursor management
	lmScoreCursor* GetCursor();
    inline lmColStaffObjs* GetCollection() { return &m_cStaffObjs; }

	//settings
    void SetFontData(lmStaff* pStaff, lmPaper* pPaper);

    //error management
    inline void SetError(wxString sMsge) { m_sErrorMsg = sMsge; }

    //common code for all time signatures types
    lmTimeSignature* AddTimeSignature(lmTimeSignature* pTS);

    //common code for keys and time signatures
    bool InsertKeyTimeSignature(lmStaffObj* pKTS, bool fKeyKeepPitch = false);

	//source LDP and MusicXML generation
	void LDP_AddShitTimeTagIfNeeded(wxString& sSource, int nIndent, bool fFwd,
								    float rTime, lmStaffObj* pSO);
	float LDP_AdvanceTimeCounter(lmStaffObj* pSO);
	void XML_AddShitTimeTagIfNeeded(wxString& sSource, int nIndent, bool fFwd,
								    float rTime, lmStaffObj* pSO);
    void XML_AddShitTimeTag(wxString& sSource, int nIndent, bool fFwd, float rTime);

    //barlines
    void CheckAndDoAutoBar(lmNoteRest* pNR);

    //other
    lmFiguredBass* FindFiguredBass(bool fFwd, lmStaffObj* pStartSO);


        // member variables

    lmScore*            m_pScore;           //lmScore to which this lmVStaff belongs
    lmInstrument*       m_pInstrument;      //lmInstrument to which this lmVStaff belongs
    lmColStaffObjs      m_cStaffObjs;       //collection of StaffObjs that form this lmVStaff

    // staves
    lmStaff*            m_cStaves[lmMAX_STAFF];     //list of Staves (lmStaff objects) that form this lmVStaff
    int                 m_nNumStaves;
    int                 m_nCurStaff;                //for navigation

    //to draw barlines, from first staff to last staff
    lmLUnits            m_yLinTop;         //Y coord. of first line (line 5, first staff)
    lmLUnits            m_yLinBottom;      //Y coord. of last line (line 1, last staff)

    lmLUnits            m_uHeight;          //TopMargin + Staves height + BottomMargin

    //for drawing prolog
    lmLUnits            m_nSpaceBeforeClef;

    //error management
    wxString            m_sErrorMsg;        //last error message

};


#endif    // __LM_VSTAFF_H__
