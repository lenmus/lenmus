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

#ifndef __LM_VSTAFF_H__        //to avoid nested includes
#define __LM_VSTAFF_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "VStaff.cpp"
#endif

#include "Score.h"
#include "StaffObjIterator.h"

class lmScore;
class lmSpacer;
class lmInstrument;
class lmRest;
class lmNote;
class lmSOControl;
class lmTextItem;
class lmMetronomeMark;


////enums
//lmENoteHead
//lmENoteType
//lmEScoreObjType
//lmEClefType
//lmETimeSignature
//lmETimeSignatureType
//lmEKeySignatures
//lmEAccidentals
//lmEPitchType
//lmEStemType
//lmEHAlign


//----------------------------------------------------------------------------------------
// lmVStaff 
//----------------------------------------------------------------------------------------

class lmVStaff : public lmScoreObj
{
public:
    lmVStaff(lmScore* pScore, lmInstrument* pInstr, long nID);
    ~lmVStaff();

	//---- virtual methods of base class -------------------------

    // units conversion
    lmLUnits TenthsToLogical(lmTenths nTenths);
    lmTenths LogicalToTenths(lmLUnits uUnits);

	inline lmEScoreObjType GetScoreObjType() { return lmSOT_VStaff; }
    inline lmScore* GetScore() { return m_pScore; }


	//---- specific methods of this class ------------------------

	//Adding StaffObs (at the end)
    lmStaff*    AddStaff(int nNumLines=5, lmLUnits nMicrons=0);
    lmClef*     AddClef(lmEClefType nClefType, int nStaff = 1, bool fVisible = true,
                        long nID = 0L);

    lmTimeSignature* AddTimeSignature(long nID, int nBeats, int nBeatType,
                        bool fVisible = true);    //for type eTS_Normal
    lmTimeSignature* AddTimeSignature(long nID, lmETimeSignature nTimeSign,
                        bool fVisible = true);    //for type eTS_Normal
    lmTimeSignature* AddTimeSignature(long nID, lmETimeSignatureType nType,
                        bool fVisible = true);    //for types eTS_Common, eTS_Cut and eTS_SenzaMisura
    lmTimeSignature* AddTimeSignature(long nID, int nSingleNumber,
                        bool fVisible = true);    //for type eTS_SingleNumber
    lmTimeSignature* AddTimeSignature(long nID, int nNumBeats, int nBeats[], int nBeatType,
                        bool fVisible = true);    //for type eTS_Composite
    lmTimeSignature* AddTimeSignature(long nID, int nNumFractions, int nBeats[], int nBeatType[],
                        bool fVisible = true);    //for type eTS_Multiple

    lmKeySignature* AddKeySignature(int nFifths, bool fMajor, bool fVisible = true, long nID = 0L);
    lmKeySignature* AddKeySignature(lmEKeySignatures nKeySignature, bool fVisible = true, long nID = 0L);

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
                           long nID = 0L);

    lmMetronomeMark* AddMetronomeMark(int nTicksPerMinute,
                            bool fParentheses = false, bool fVisible = true, long nID = 0L);
    lmMetronomeMark* AddMetronomeMark(lmENoteType nLeftNoteType, int nLeftDots,
                            lmENoteType nRightNoteType, int nRightDots,
                            bool fParentheses = false, bool fVisible = true, long nID = 0L);
    lmMetronomeMark* AddMetronomeMark(lmENoteType nLeftNoteType, int nLeftDots,
                            int nTicksPerMinute,
                            bool fParentheses = false, bool fVisible = true, long nID = 0L);

    lmSOControl* AddNewSystem();

    lmSpacer* AddSpacer(lmTenths nWidth);

    lmStaffObj* AddAnchorObj();

    lmTextItem* AddText(wxString& sText, lmEHAlign nHAlign, lmFontInfo& oFontData,
                        bool fHasWidth, long nID = 0L);
    lmTextItem* AddText(wxString& sText, lmEHAlign nHAlign, lmTextStyle* pStyle,
                        bool fHasWidth, long nID = 0L);

	//Edition commands

	    //--- inserting StaffObs
    lmBarline* CmdNew_InsertBarline(lmEBarline nType = lm_eBarlineSimple, bool fVisible = true);
	lmClef* CmdNew_InsertClef(lmEClefType nClefType, bool fVisible = true);
    lmKeySignature* CmdNew_InsertKeySignature(int nFifths, bool fMajor, bool fVisible = true);
	lmNote* CmdNew_InsertNote(lmEPitchType nPitchType, int nStep,
					       int nOctave, lmENoteType nNoteType, float rDuration, int nDots,
					       lmENoteHeads nNotehead, lmEAccidentals nAcc, 
                           int nVoice, lmNote* pBaseOfChord, bool fTiedPrev, bool fAutoBar);

	lmRest* CmdNew_InsertRest(lmENoteType nNoteType, float rDuration,
                           int nDots, int nVoice, bool fAutoBar);

    lmTimeSignature* CmdNew_InsertTimeSignature(int nBeats, int nBeatType, bool fVisible = true);


        //--- deleting StaffObjs
    bool CmdNew_DeleteStaffObj(lmStaffObj* pSO);
    bool CmdNew_DeleteClef(lmClef* pClef);
    bool CmdNew_DeleteKeySignature(lmKeySignature* pKS);
    bool CmdNew_DeleteTimeSignature(lmTimeSignature* pTS);

        //--- deleting AuxObjs
    bool CmdNew_DeleteTuplet(lmNoteRest* pStartNote);

        //--- Modifying staffobjs/AuxObjs
    void CmdNew_ChangeDots(lmNoteRest* pNR, int nDots);
    void CmdNew_BreakBeam(lmNoteRest* pBeforeNR);
    void CmdNew_JoinBeam(std::vector<lmNoteRest*>& notes);
    bool CmdNew_DeleteBeam(lmNoteRest* pNR);

        //--- Adding other markup
    void CmdNew_AddTie(lmNote* pStartNote, lmNote* pEndNote);
    bool CmdNew_AddTuplet(std::vector<lmNoteRest*>& notes,
                       bool fShowNumber, int nNumber, bool fBracket,
                       lmEPlacement nAbove, int nActual, int nNormal);



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
    lmBarline* GetBarlineOfLastNonEmptyMeasure(lmLUnits* pPos);
    void SetSpaceBeforeClef(lmLUnits nSpace) { m_nSpaceBeforeClef = nSpace; }
    lmLUnits GetSpaceBeforeClef() { return m_nSpaceBeforeClef; }

    //general StaffObjs management
    void AssignID(lmStaffObj* pSO);

    //miscellaneous
    inline bool IsGlobalStaff() const { return (m_pInstrument == (lmInstrument*)NULL); }
	inline lmInstrument* GetOwnerInstrument() const { return m_pInstrument; }
    int GetNumberOfStaff(lmStaff* pStaff);       //1..n
    inline int GetNumInstr() { return m_pInstrument->GetNumInstr(); }

    //cursor management and cursor related
    void AttachCursor(lmVStaffCursor* pVCursor);
    inline void ResetCursor() { GetVCursor()->ResetCursor(); }
    inline lmContext* GetContextAtCursorPoint() { return GetVCursor()->GetCurrentContext(); }
    inline int GetCursorStaffNum() { return GetVCursor()->GetNumStaff(); }
    inline lmStaffObj* GetCursorStaffObj() { return GetVCursor()->GetStaffObj(); }
    inline int GetCursorSegmentNum() { return GetVCursor()->GetSegment(); }
    inline float GetCursorTimepos() { return GetVCursor()->GetTimepos(); }

    //Debug methods
    wxString Dump();


private:
    friend class lmColStaffObjs;

    //cursor management
	lmVStaffCursor* GetVCursor();

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

    //operations
    int AskUserAboutClef();
    int AskUserAboutKey();
    bool CheckIfNotesAffectedByClef(bool fSkip);
    bool CheckIfNotesAffectedByKey(bool fSkip);

    //barlines
    void CheckAndDoAutoBar(lmNoteRest* pNR);


        // member variables

    lmScore*            m_pScore;           //lmScore to which this lmVStaff belongs
    lmInstrument*       m_pInstrument;      //lmInstrument to which this lmVStaff belongs
    lmColStaffObjs      m_cStaffObjs;       //collection of StaffObjs that form this lmVStaff
	//lmVStaffCursor	    m_VCursor;			//cursor for the staffobjs collection

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
