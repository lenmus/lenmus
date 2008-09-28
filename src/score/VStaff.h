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

#ifndef __LM_VSTAFF_H__        //to avoid nested includes
#define __LM_VSTAFF_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "VStaff.cpp"
#endif

#include "StaffObjIterator.h"

class lmScore;
class lmSpacer;
class lmInstrument;
class lmRest;
class lmNote;
class lmSOControl;
class lmTextItem;
class lmMetronomeMark;
class lmBoxSliceVStaff;
class lmScoreCommand;
class lmUndoItem;

#include "Score.h"

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
//ETraversingOrder


//----------------------------------------------------------------------------------------
// lmVStaff 
//----------------------------------------------------------------------------------------

class lmVStaff : public lmScoreObj
{
public:
    lmVStaff(lmScore* pScore, lmInstrument* pInstr);
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
    lmClef*     AddClef(lmEClefType nClefType, int nStaff = 1, bool fVisible = true);

    lmTimeSignature* AddTimeSignature(int nBeats, int nBeatType,
                        bool fVisible = true);    //for type eTS_Normal
    lmTimeSignature* AddTimeSignature(lmETimeSignature nTimeSign,
                        bool fVisible = true);    //for type eTS_Normal
    lmTimeSignature* AddTimeSignature(lmETimeSignatureType nType,
                        bool fVisible = true);    //for types eTS_Common, eTS_Cut and eTS_SenzaMisura
    lmTimeSignature* AddTimeSignature(int nSingleNumber,
                        bool fVisible = true);    //for type eTS_SingleNumber
    lmTimeSignature* AddTimeSignature(int nNumBeats, int nBeats[], int nBeatType,
                        bool fVisible = true);    //for type eTS_Composite
    lmTimeSignature* AddTimeSignature(int nNumFractions, int nBeats[], int nBeatType[],
                        bool fVisible = true);    //for type eTS_Multiple

    lmKeySignature* AddKeySignature(int nFifths, bool fMajor, bool fVisible = true);
    lmKeySignature* AddKeySignature(lmEKeySignatures nKeySignature, bool fVisible = true);

    lmRest*     AddRest(lmENoteType nNoteType, float rDuration, int nDots,
                      int nStaff, int nVoice = 1,
					  bool fVisible = true,
                      bool fBeamed = false, lmTBeamInfo BeamInfo[] = NULL);

    lmNote*     AddNote(lmEPitchType nPitchType,
                    int nStep, int nOctave, int nAlter,
                    lmEAccidentals nAccidentals,
                    lmENoteType nNoteType, float rDuration, int nDots,
                    int nStaff, int nVoice = 1,
					bool fVisible = true,
                    bool fBeamed = false, lmTBeamInfo BeamInfo[] = NULL,
                    lmNote* pBaseOfChord = (lmNote*)NULL,
                    bool fTie = false,
                    lmEStemType nStem = lmSTEM_DEFAULT);

    lmBarline*  AddBarline(lmEBarline nType = lm_eBarlineSimple, bool fVisible = true);

    lmMetronomeMark* AddMetronomeMark(int nTicksPerMinute,
                            bool fParentheses = false, bool fVisible = true);
    lmMetronomeMark* AddMetronomeMark(lmENoteType nLeftNoteType, int nLeftDots,
                            lmENoteType nRightNoteType, int nRightDots,
                            bool fParentheses = false, bool fVisible = true);
    lmMetronomeMark* AddMetronomeMark(lmENoteType nLeftNoteType, int nLeftDots,
                            int nTicksPerMinute,
                            bool fParentheses = false, bool fVisible = true);

    lmSOControl* AddNewSystem();

    lmSpacer* AddSpacer(lmTenths nWidth);

    lmStaffObj* AddAnchorObj();

    lmTextItem* AddText(wxString& sText, lmEHAlign nHAlign, lmFontInfo& oFontData,
                        bool fHasWidth);
    lmTextItem* AddText(wxString& sText, lmEHAlign nHAlign, lmTextStyle* pStyle,
                        bool fHasWidth);

	//Edition commands

	    //--- inserting StaffObs
    lmBarline* Cmd_InsertBarline(lmUndoItem* pUndoItem, lmEBarline nType = lm_eBarlineSimple,
                                 bool fVisible = true);

	lmClef* Cmd_InsertClef(lmUndoItem* pUndoItem, lmEClefType nClefType, int nStaff,
                           bool fVisible);

    lmKeySignature* Cmd_InsertKeySignature(lmUndoItem* pUndoItem, int nFifths,
                                    bool fMajor, bool fVisible);

	lmNote* Cmd_InsertNote(lmUndoItem* pUndoItem, lmEPitchType nPitchType, int nStep,
					       int nOctave, lmENoteType nNoteType, float rDuration, int nDots,
					       lmENoteHeads nNotehead, lmEAccidentals nAcc, 
                           int nVoice, lmNote* pBaseOfChord, bool fTiedPrev, bool fAutoBar);

	lmRest* Cmd_InsertRest(lmUndoItem* pUndoItem, lmENoteType nNoteType,
                           float rDuration, int nDots, bool fAutoBar);

    lmTimeSignature* Cmd_InsertTimeSignature(lmUndoItem* pUndoItem, int nBeats,
                                    int nBeatType, bool fVisible);


        //--- deleting StaffObjs
    void Cmd_DeleteStaffObj(lmUndoItem* pUndoItem, lmStaffObj* pSO);
    void Cmd_DeleteClef(lmUndoItem* pUndoItem, lmClef* pClef);
    void Cmd_DeleteKeySignature(lmUndoItem* pUndoItem, lmKeySignature* pKS);
    void Cmd_DeleteTimeSignature(lmUndoItem* pUndoItem, lmTimeSignature* pTS);
    void Cmd_DeleteTie(lmUndoItem* pUndoItem, lmNote* pEndNote);
    void Cmd_DeleteTuplet(lmUndoItem* pUndoItem, lmNoteRest* pStartNote);

        //--- Modifying staffobjs
    void Cmd_ChangeDots(lmUndoItem* pUndoItem, lmNoteRest* pNR, int nDots);
    void Cmd_BreakBeam(lmUndoItem* pUndoItem, lmNoteRest* pBeforeNR);
    void Cmd_JoinBeam(lmUndoItem* pUndoItem, std::vector<lmNoteRest*>& notes);
    void Cmd_DeleteBeam(lmUndoItem* pUndoItem, lmNoteRest* pNR);

        //--- Adding other markup
    void Cmd_AddTie(lmUndoItem* pUndoItem, lmNote* pStartNote, lmNote* pEndNote);
    void Cmd_AddTuplet(lmUndoItem* pUndoItem, std::vector<lmNoteRest*>& notes,
                       bool fShowNumber, int nNumber, bool fBracket,
                       lmEPlacement nAbove, int nActual, int nNormal);


    //--- Undoing edition commands

	    //--- inserting StaffObs
    void UndoCmd_InsertBarline(lmUndoItem* pUndoItem, lmBarline* pBarline);
    void UndoCmd_InsertClef(lmUndoItem* pUndoItem, lmClef* pClef);
    void UndoCmd_InsertKeySignature(lmUndoItem* pUndoItem, lmKeySignature* pKS);
    void UndoCmd_InsertNote(lmUndoItem* pUndoItem, lmNote* pNote);
    void UndoCmd_InsertRest(lmUndoItem* pUndoItem, lmRest* pRest);
    void UndoCmd_InsertTimeSignature(lmUndoItem* pUndoItem, lmTimeSignature* pTS);

        //--- deleting StaffObjs
    void UndoCmd_DeleteStaffObj(lmUndoItem* pUndoItem, lmStaffObj* pSO);
    void UndoCmd_DeleteClef(lmUndoItem* pUndoItem, lmClef* pClef);
    void UndoCmd_DeleteKeySignature(lmUndoItem* pUndoItem, lmKeySignature* pKS);
    void UndoCmd_DeleteTimeSignature(lmUndoItem* pUndoItem, lmTimeSignature* pTS);
    void UndoCmd_DeleteTie(lmUndoItem* pUndoItem, lmNote* pEndNote);
    void UndoCmd_DeleteTuplet(lmUndoItem* pUndoItem, lmNoteRest* pStartNote);

        //--- Modifying staffobjs
    void UndoCmd_ChangeDots(lmUndoItem* pUndoItem, lmNoteRest* pNR);
    void UndoCmd_BreakBeam(lmUndoItem* pUndoItem, lmNoteRest* pBeforeNR);
    void UndoCmd_JoinBeam(lmUndoItem* pUndoItem);
    void UndoCmd_DeleteBeam(lmUndoItem* pUndoItem);

        //--- Adding other markup
    void UndoCmd_AddTie(lmUndoItem* pUndoItem, lmNote* pStartNote, lmNote* pEndNote);
    void UndoCmd_AddTuplet(lmUndoItem* pUndoItem, lmNoteRest* pStartNR);



    //error management
    inline wxString GetErrorMessage() { return m_sErrorMsg; }


    bool ShiftTime(float rTimeShift);


    // rendering methods
    lmLUnits LayoutStaffLines(lmBox* pBox, lmLUnits xFrom, lmLUnits xTo, lmLUnits yPos);
    void NewLine(lmPaper* pPaper);
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


    //Debug methods
    wxString Dump();
    wxString SourceLDP(int nIndent);
    wxString SourceXML(int nIndent);

    // restricted methods
    lmSOIterator* CreateIterator(ETraversingOrder nOrder);    //for lmFormatter objects
    inline lmSOIterator* CreateIteratorTo(ETraversingOrder nOrder, lmStaffObj* pSO) 
                                        { return m_cStaffObjs.CreateIteratorTo(nOrder, pSO); }

    //for navigation along staves
    inline int GetNumStaves() const { return m_nNumStaves; }
    lmStaff* GetFirstStaff();
    lmStaff* GetNextStaff();
    lmStaff* GetLastStaff();

    //context management
	inline lmContext* GetCurrentContext(lmStaffObj* pSO)
                            { return m_cStaffObjs.GetCurrentContext(pSO); }
	inline lmContext* NewUpdatedContext(lmStaffObj* pSO)
                            { return m_cStaffObjs.NewUpdatedContext(pSO); }
	inline lmContext* NewUpdatedLastContext(int nStaff)
                            { return m_cStaffObjs.NewUpdatedLastContext(nStaff); }
	inline lmContext* GetLastContext(int nStaff)
                            { return m_cStaffObjs.GetLastContext(nStaff); }
    inline lmContext* GetStartOfSegmentContext(int nMeasure, int nStaff)
                            { return m_cStaffObjs.GetStartOfSegmentContext(nMeasure, nStaff); }

    //contexts related
    void OnContextUpdated(lmNote* pStartNote, int nStaff, int nStep,
                       int nNewAccidentals, lmContext* pCurrentContext);
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
	inline lmLUnits GetTopMargin() const { return m_topMargin; }

    //miscellaneous
    inline bool IsGlobalStaff() const { return (m_pInstrument == (lmInstrument*)NULL); }
	inline lmInstrument* GetOwnerInstrument() const { return m_pInstrument; }

    //cursor management
	inline lmVStaffCursor* GetVCursor() { return &m_VCursor; }
    inline void ResetCursor() { m_VCursor.ResetCursor(); }


private:
    friend class lmColStaffObjs;

	//seetings
    void SetFont(lmStaff* pStaff, lmPaper* pPaper);

    //error management
    inline void SetError(wxString sMsge) { m_sErrorMsg = sMsge; }

    //common code for all time signatures types
    lmTimeSignature* AddTimeSignature(lmTimeSignature* pTS);

    //common code for keys and time signatures
    bool InsertKeyTimeSignature(lmUndoItem* pUndoItem, lmStaffObj* pKTS,
                                bool fKeyKeepPitch = false);

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
    bool CheckIfNotesAffectedByClef();
    bool CheckIfNotesAffectedByKey();

    //barlines
    void CheckAndDoAutoBar(lmUndoItem* pUndoItem, lmNoteRest* pNR);

    //beams
        //structure with info about a note/rest beam status information
    typedef struct 
    {
        lmNoteRest*     pNR;
        lmTBeamInfo     tBeamInfo[6];
        int             nBeamRef;
    }
    lmBeamNoteInfo;

    void SaveBeamNoteInfo(lmNoteRest* pNR, std::list<lmBeamNoteInfo*>& oListNR, int nBeamIdx);
    void LogBeamData(lmUndoData* pUndoData, std::list<lmBeamNoteInfo*>& oListNR);
    void GetLoggedBeamData(lmUndoData* pUndoData, int* pNumNotes,
                           std::list<lmBeamNoteInfo>& oListNR);






        // member variables

    lmScore*            m_pScore;           //lmScore to which this lmVStaff belongs
    lmInstrument*       m_pInstrument;      //lmInstrument to which this lmVStaff belongs
    lmColStaffObjs      m_cStaffObjs;       //collection of StaffObjs that form this lmVStaff
	lmVStaffCursor	    m_VCursor;			//cursor for the staffobjs collection

    // staves
    lmStaff*            m_cStaves[lmMAX_STAFF];     //list of Staves (lmStaff objects) that form this lmVStaff
    int                 m_nNumStaves;
    int                 m_nCurStaff;                //for navigation

    //to draw barlines, from first staff to last staff
    lmLUnits            m_yLinTop;         //Y coord. of first line (line 5, first staff)
    lmLUnits            m_yLinBottom;      //Y coord. of last line (line 1, last staff)

    lmLUnits            m_leftMargin;      // lmVStaff margins (logical units)
    lmLUnits            m_topMargin;
    lmLUnits            m_rightMargin;
    lmLUnits            m_bottomMargin;
    lmLUnits            m_nHeight;          //TopMargin + Staves height + BottomMargin

    //for drawing prolog
    lmLUnits            m_nSpaceBeforeClef;

    //error management
    wxString            m_sErrorMsg;        //last error message

};


#endif    // __LM_VSTAFF_H__
