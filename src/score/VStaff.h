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
class lmScoreText;
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
//lmEAlignment
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
                    bool fInChord = false,
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

    lmScoreText* AddText(wxString sText, lmEAlignment nAlign,
                         lmLocation* pPos, lmFontInfo oFontData, bool fHasWidth);

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
                           bool fTiedPrev, bool fAutoBar);

	lmRest* Cmd_InsertRest(lmUndoItem* pUndoItem, lmENoteType nNoteType,
                           float rDuration, int nDots, bool fAutoBar);

    lmTimeSignature* Cmd_InsertTimeSignature(lmUndoItem* pUndoItem, int nBeats,
                                    int nBeatType, bool fVisible);


    //--- deleting StaffObjs
    void Cmd_DeleteObject(lmUndoItem* pUndoItem, lmStaffObj* pSO);
    void Cmd_DeleteTie(lmUndoItem* pUndoItem, lmNote* pEndNote);


    //--- Undoing edition commands
    void UndoCmd_InsertBarline(lmUndoItem* pUndoItem, lmBarline* pBarline);
    void UndoCmd_InsertClef(lmUndoItem* pUndoItem, lmClef* pClef);
    void UndoCmd_InsertKeySignature(lmUndoItem* pUndoItem, lmKeySignature* pKS);
    void UndoCmd_InsertNote(lmUndoItem* pUndoItem, lmNote* pNote);
    void UndoCmd_InsertRest(lmUndoItem* pUndoItem, lmRest* pRest);
    void UndoCmd_InsertTimeSignature(lmUndoItem* pUndoItem, lmTimeSignature* pTS);

    void UndoCmd_DeleteObject(lmUndoItem* pUndoItem, lmStaffObj* pSO);
    void UndoCmd_DeleteTie(lmUndoItem* pUndoItem, lmNote* pEndNote);


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

    lmNote* FindPossibleStartOfTie(lmNote* pEndNote, bool fNotAdded = false);

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
    inline lmScore* GetScore() { return m_pScore; }

    //cursor management
	inline lmVStaffCursor* GetVCursor() { return &m_VCursor; }
    inline void ResetCursor() { m_VCursor.ResetCursor(); }


private:
    friend class lmColStaffObjs;

    void SetFont(lmStaff* pStaff, lmPaper* pPaper);

    //error management
    inline void SetError(wxString sMsge) { m_sErrorMsg = sMsge; }

    //common code for all time signatures types
    lmTimeSignature* AddTimeSignature(lmTimeSignature* pTS);

    //common code for keys and time signatures
    bool InsertKeyTimeSignature(lmUndoItem* pUndoItem, lmStaffObj* pKTS);

	//source LDP and MusicXML generation
	void LDP_AddShitTimeTagIfNeeded(wxString& sSource, int nIndent, bool fFwd,
								    float rTime, lmStaffObj* pSO);
	float LDP_AdvanceTimeCounter(lmStaffObj* pSO);
	void XML_AddShitTimeTagIfNeeded(wxString& sSource, int nIndent, bool fFwd,
								    float rTime, lmStaffObj* pSO);
    void XML_AddShitTimeTag(wxString& sSource, int nIndent, bool fFwd, float rTime);

    //access
    bool CheckIfNotesAffectedByClef();

    //barlines
    void CheckAndDoAutoBar(lmUndoItem* pUndoItem, lmNoteRest* pNR);





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


//----------------------------------------------------------------------------------------
// helper class lmVStaffCmd: a command with roll-back capabilities
//----------------------------------------------------------------------------------------

class lmVStaffCmd
{
public:
    lmVStaffCmd(lmVStaff* pVStaff);
    virtual ~lmVStaffCmd();

    virtual void RollBack(lmUndoItem* pUndoItem)=0;
    virtual bool Success()=0;

protected:
    lmVStaff*       m_pVStaff;
};

//---------------------------------------------------------------------------------------
class lmVCmdInsertNote : public lmVStaffCmd
{
public:
    lmVCmdInsertNote(lmVStaff* pVStaff, lmUndoItem* pUndoItem, lmEPitchType nPitchType,
                     int nStep, int nOctave, lmENoteType nNoteType, float rDuration,
					 int nDots, lmENoteHeads nNotehead, lmEAccidentals nAcc,
                     bool fTiedPrev);
    ~lmVCmdInsertNote() {}

    void RollBack(lmUndoItem* pUndoItem);
    inline bool Success() { return (m_pNewNote != (lmNote*)NULL); }

protected:
    lmNote*             m_pNewNote;     //inserted note

};

//---------------------------------------------------------------------------------------
class lmVCmdInsertRest : public lmVStaffCmd
{
public:
    lmVCmdInsertRest(lmVStaff* pVStaff, lmUndoItem* pUndoItem,
                     lmENoteType nNoteType, float rDuration, int nDots);
    ~lmVCmdInsertRest() {}

    void RollBack(lmUndoItem* pUndoItem);
    inline bool Success() { return (m_pNewRest != (lmRest*)NULL); }

protected:
    lmRest*         m_pNewRest;     //inserted rest

};

//---------------------------------------------------------------------------------------
class lmVCmdInsertClef : public lmVStaffCmd
{
public:
    lmVCmdInsertClef(lmVStaff* pVStaff, lmUndoItem* pUndoItem, lmEClefType nClefType, 
                     int nStaff = 1, bool fVisible = lmVISIBLE);
    ~lmVCmdInsertClef() {}

    void RollBack(lmUndoItem* pUndoItem);
    inline bool Success() { return (m_pNewClef != (lmClef*)NULL); }

protected:
    lmClef*				m_pNewClef;     //inserted clef

};

//---------------------------------------------------------------------------------------
class lmVCmdInsertBarline : public lmVStaffCmd
{
public:
    lmVCmdInsertBarline(lmVStaff* pVStaff, lmUndoItem* pUndoItem, 
                        lmEBarline nBarlineType, bool fVisible=true);
    ~lmVCmdInsertBarline() {}

    void RollBack(lmUndoItem* pUndoItem);
    inline bool Success() { return (m_pNewBar != (lmBarline*)NULL); }

protected:
    lmBarline*          m_pNewBar;     //inserted barline

};

//---------------------------------------------------------------------------------------
class lmVCmdInsertTimeSignature : public lmVStaffCmd
{
public:
    lmVCmdInsertTimeSignature(lmVStaff* pVStaff, lmUndoItem* pUndoItem, int nBeats,
                              int nBeatType, bool fVisible);
    ~lmVCmdInsertTimeSignature() {}

    void RollBack(lmUndoItem* pUndoItem);
    inline bool Success() { return (m_pNewTime != (lmTimeSignature*)NULL); }

protected:
    lmTimeSignature*    m_pNewTime;     //inserted time signature

};

//---------------------------------------------------------------------------------------
class lmVCmdInsertKeySignature : public lmVStaffCmd
{
public:
    lmVCmdInsertKeySignature(lmVStaff* pVStaff, lmUndoItem* pUndoItem, int nFifths,
                             bool fMajor, bool fVisible);
    ~lmVCmdInsertKeySignature() {}

    void RollBack(lmUndoItem* pUndoItem);
    inline bool Success() { return (m_pNewKey != (lmKeySignature*)NULL); }

protected:
    lmKeySignature*     m_pNewKey;      //inserted key signature

};

//---------------------------------------------------------------------------------------
class lmVCmdDeleteObject : public lmVStaffCmd
{
public:
    lmVCmdDeleteObject(lmVStaff* pVStaff, lmUndoItem* pUndoItem, lmStaffObj* pSO);
    ~lmVCmdDeleteObject() {}

    void RollBack(lmUndoItem* pUndoItem);
    inline bool Success() { return (m_pSO != (lmStaffObj*)NULL); }

protected:
    lmStaffObj*         m_pSO;          //deleted note

};

//---------------------------------------------------------------------------------------
class lmVCmdDeleteStaffObj : public lmVStaffCmd
{
public:
    lmVCmdDeleteStaffObj(lmVStaff* pVStaff, lmUndoItem* pUndoItem, lmStaffObj* pSO);
    ~lmVCmdDeleteStaffObj() {}

    void RollBack(lmUndoItem* pUndoItem);
    inline bool Success() { return (m_pSO != (lmStaffObj*)NULL); }

protected:
    lmStaffObj*         m_pSO;          //deleted note

};

//---------------------------------------------------------------------------------------
class lmVCmdDeleteTie : public lmVStaffCmd
{
public:
    lmVCmdDeleteTie(lmVStaff* pVStaff, lmUndoItem* pUndoItem, lmNote* pEndNote);
    ~lmVCmdDeleteTie() {}

    void RollBack(lmUndoItem* pUndoItem);
    inline bool Success() { return !m_pEndNote->IsTiedToPrev(); }

protected:
    lmNote*         m_pEndNote;     //owner note

};



#endif    // __LM_VSTAFF_H__
