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

#ifndef __LM_COLSTAFFOBJS_H__        //to avoid nested includes
#define __LM_COLSTAFFOBJS_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ColStaffObjs.cpp"
#endif

#include <vector>
#include <list>

class lmStaffObj;
class lmNoteRest;
class lmClef;
class lmSOIterator;
class lmContext;
class lmVStaff;
class lmInstrument;
class lmColStaffObjs;
class lmBarline;
class lmView;
class lmScoreCursor;
class lmTimeSignature;
class lmKeySignature;

#include "StaffObj.h"   //lmEScoreObjType


#define lmNO_TIME_LIMIT     100000000.0f        //a too big measure duration

#define lmItMeasure		std::list<lmSegmentData*>::iterator

class lmSegment
{
public:
    lmSegment(lmColStaffObjs* pOwner, int nSegment);
    ~lmSegment();

    //operations
	void Store(lmStaffObj* pNewSO, lmScoreCursor* pCursor);
    void Remove(lmStaffObj* pSO, bool fDelete, bool fClefKeepPosition, bool fKeyKeepPitch);
    void UpdateDuration();
    void AutoBeam(int nVoice);
    void SetCollection(lmStaffObj* pFirstSO, lmStaffObj* pLastSO);
    void FinishSegmentAt(lmStaffObj* pLastSO);
    float JoinSegment(lmSegment* pSegment);
    bool IsTimePosOccupied(float rTime, float rDuration, int nVoice);
    lmSegment* GetNextSegment();
    inline void IncrementNumber() { m_nNumSegment++; }
    inline void DecrementNumber() { m_nNumSegment--; }
    void RecomputeSegmentDuration(lmNoteRest* pNR, float rTimeIncr);

	//context management
    inline void SetContext(int iStaff, lmContext* pContext) { m_pContext[iStaff] = pContext; }
	inline lmContext* GetContext(int iStaff) { return m_pContext[iStaff]; }
    void DoContextInsertion(lmKeySignature* pNewKey, lmStaffObj* pNextSO, bool fKeyKeepPitch);
    void DoContextInsertion(lmClef* pNewClef, lmStaffObj* pNextSO, bool fClefKeepPosition);
    void DoContextRemoval(lmKeySignature* pOldKey, lmStaffObj* pNextSO, bool fKeyKeepPitch);
    void DoContextRemoval(lmClef* pOldClef, lmStaffObj* pNextSO, bool fClefKeepPosition);

    //info
	inline int GetNumSegment() { return m_nNumSegment; }
    int GetNumInstr();

    bool IsSegmentFull();
    inline float GetDuration() { return m_rMaxTime; }
    float GetMaximumTime();
    inline bool IsEmpty() { return m_nNumSO == 0; }
    bool HasBarline();
    inline int GetNumObjects() { return m_nNumSO; }

    //voices
    int GetNumVoices();
    int StartNewVoice();
    bool IsVoiceUsed(int nVoice);

    //access to staffobjs
    lmBarline* GetBarline();
    inline lmStaffObj* GetFirstSO() { return m_pFirstSO; }
    inline lmStaffObj* GetLastSO() { return m_pLastSO; }
    int FindPosition(lmStaffObj* pSO);
    //lmStaffObj* GetStaffObj(int nPosition);
    lmStaffObj* GetStaffObj(long nID);

	//debug
	wxString Dump();



private:

    void VoiceUsed(int nVoice);
    void SortMeasure();
    void ShiftRightTimepos(lmStaffObj* pStartSO, float rTimeShift);
    void AddToCollection(lmStaffObj* pNewSO, lmStaffObj* pNextSO);
    void ShiftLeftTimepos(lmStaffObj* pStartSO, float rTimeShift, float rStartTimepos, int nVoice);
    void CountObjects();
    void AttachWarning();

    //segment management
    void UpdateMeasureDuration();

    //auto beam
    void AutoBeam_CreateBeam(std::list<lmNoteRest*>& cBeamedNotes);

    //context management
    lmContext* FindEndOfSegmentContext(int nStaff);
    void PropagateContextChange(lmContext* pStartContext, int nStaff,
                                lmKeySignature* pNewKey, lmKeySignature* pOldKey,
                                bool fKeyKeepPitch);
    void PropagateContextChange(lmContext* pStartContext, int nStaff,
                                lmEClefType nNewClefType, lmEClefType nOldClefType,
                                bool fClefKeepPosition);
    void PropagateContextChange(lmContext* pStartContext, int nStaff);

    //staffobjs management
    void Transpose(lmEClefType nNewClefType, lmEClefType nOldClefType,
                   lmStaffObj* pStartSO, int nStaff);
    void AddRemoveAccidentals(lmKeySignature* pNewKey, lmStaffObj* pStartSO);
    void ChangePitch(lmKeySignature* pOldKey, lmKeySignature* pNewKey, lmStaffObj* pStartSO);


    //member variables

    lmColStaffObjs*         m_pOwner;

    //info about this segment
    int			m_nNumSegment;				//0..n-1
    lmContext*	m_pContext[lmMAX_STAFF];	//ptr to current context for each staff
    int         m_bVoices;                  //voices in this segment. One bit per used voice
    float	    m_rMaxTime;                 //occupied time from start of the measure

    //info about StaffObjs in this segments:
    lmStaffObj*     m_pFirstSO;         //first SO in this measure (NULL, pSO)
    lmStaffObj*     m_pLastSO;          //last SO in this measure (barline, NULL, pSO)
    int             m_nNumSO;           //num of objects in this measure
};


//cursor state
class lmCursorState
{
public:
    lmCursorState();
    lmCursorState(int nInstr, int nStaff, float rTimepos, lmStaffObj* pSO);
    lmCursorState(int nInstr, int nStaff, float rTimepos, long nObjID);
    ~lmCursorState() {}

    //accesors
    inline int GetNumInstr() { return m_nInstr; }       //instrument (1..n)
    inline int GetNumStaff() { return m_nStaff; }       //staff (1..n)
    inline float GetTimepos() { return m_rTimepos; }
    inline long GetObjID() { return m_nObjID; }
    lmStaffObj* GetStaffObj(lmScore* pScore); 

    //other
    inline bool IsEmptyState() { return m_nInstr == 0; }
    inline void IncrementTimepos(float rTime) { m_rTimepos += rTime; }


protected:
    int         m_nInstr;       //instrument (1..n)
	int         m_nStaff;       //staff (1..n)
	float       m_rTimepos;     //timepos
    long        m_nObjID;       //current pointed staffobj ID or lmNO_ID if NULL
};



//=======================================================================================
// class lmScoreCursor
//=======================================================================================

class lmScoreCursor
{
public:
    lmScoreCursor(lmScore* pOwnerScore);
    ~lmScoreCursor();

    //positioning
    void MoveToStartOfInstrument(int nInstr);
    void MoveRight(bool fAlsoChordNotes = true);
    void MoveLeft(bool fAlsoChordNotes = true);
    void MoveUp();
    void MoveDown();
	void MoveNearTo(lmUPoint uPos, lmVStaff* pVStaff, int nStaff, int nMeasure);
    void MoveCursorToObject(lmStaffObj* pSO);
    void MoveTo(lmVStaff* pVStaff, int nStaff, int nMeasure, float rTime,
                bool fEndOfTime = false);
    void MoveToTime(float rNewTime, bool fEndOfTime = false);
    void AdvanceToTime(float rTime);
    void MoveToNextSegment();
    void MoveToStartOfSegment(int nSegment, int nStaff, bool fSkipClef=false,
                              bool fSkipKey=false);
    void MoveToStartOfTimepos();

    //internal state (setting it implies re-positioning)
    void SetState(lmCursorState* pState, bool fUpdateTimepos=false);
    lmCursorState GetState();

    //access to state info. Only meaninful if IsOK()
    inline bool IsOK() { return (GetCursorInstrumentNumber() > 0); }
    inline int GetCursorNumStaff() { return m_nStaff; }
    inline float GetCursorTime() { return m_rTimepos; }
    inline int GetCursorInstrumentNumber() { return m_nInstr; }
	int GetSegment();
    lmVStaff* GetVStaff();
    lmStaff* GetCursorStaff();
    lmUPoint GetCursorPoint(int* pNumPage = NULL);
    lmStaffObj* GetStaffObj();
    lmStaffObj* GetPreviousStaffobj();
    lmContext* GetCurrentContext();

    //other info
	inline lmScore* GetCursorScore() { return m_pScore; }


private:
    //movement
    void ResetCursor();
    void PointCursorToInstrument(int nInstr);
    void MoveAfterProlog();
    void MoveRightToNextTime();
    void MoveLeftToPrevTime();
    void PositionAt(float rTargetTimepos);
	void DoMoveRight(bool fAlsoChordNotes = true);
	void DoMoveLeft(bool fAlsoChordNotes = true);
    void DoMoveToFirst(int nStaff=0);
	void DoMoveToSegment(int nSegment, int nStaff, lmUPoint uPos);
    void SkipClefKey(bool fSkipKey);

    //current position
    bool IsAtEnd();
    bool IsAtBeginning();

    //helper, for cursor common operations
    void UpdateTimepos();
    float GetStaffPosY(lmStaffObj* pSO);


    lmScore*            m_pScore;           //owner score
	lmColStaffObjs*		m_pColStaffObjs;	//collection pointed by this cursor

    //state variables
    int             m_nInstr;       //instrument (1..n)
    int				m_nStaff;       //staff (1..n)
	float			m_rTimepos;     //timepos
    int             m_nSegment;     //segment number (0..n). -1 if no StaffObj

    lmSOIterator*   m_pIt;          //iterator pointing to current SO
};





// the collection
//---------------------------------------------------------------------------------------------

class lmColStaffObjs
{
public:
    lmColStaffObjs(lmVStaff* pOwner, int nNumStaves);
    ~lmColStaffObjs();

	//creation related
    void AddStaff();

    //add/remove StaffObjs
    void Add(lmStaffObj* pNewSO, bool fClefKeepPosition=true, bool fKeyKeepPitch=true);
	void Delete(lmStaffObj* pCursorSO, bool fClefKeepPosition = true,
                bool fKeyKeepPitch=true);
    void AddToCollection(lmStaffObj* pNewSO, lmStaffObj* pNextSO);


	void Insert(lmStaffObj* pNewSO, lmStaffObj* pBeforeSO);
    bool ShiftTime(float rTimeShift);

    //actions to deal with staffobjs modifications
    void RecomputeSegmentDuration(lmNoteRest* pNR, float rTimeIncr);

    //access StaffObjs
    lmBarline* GetBarlineOfMeasure(int nMeasure);      //1..n
    lmBarline* GetBarlineOfLastNonEmptyMeasure();
    lmTimeSignature* FindFwdTimeSignature(lmStaffObj* pCurSO);
    lmKeySignature* FindFwdKeySignature(lmStaffObj* pCurSO);
    lmClef* FindFwdClef(lmStaffObj* pCurSO);
    lmStaffObj* FindFwdStaffObj(lmStaffObj* pCurSO, lmEScoreObjType nType);
    lmStaffObj* FindNextStaffObj(lmStaffObj* pCurSO);
    lmStaffObj* FindPrevStaffObj(lmStaffObj* pCurSO);
    inline lmStaffObj* GetFirstSO() { return m_pFirstSO; }
    inline lmStaffObj* GetLastSO() { return m_pLastSO; }

    //collection access and iterator related methods
	lmSOIterator* CreateIterator();
	lmSOIterator* CreateIteratorTo(lmStaffObj* pSO);
	lmSOIterator* CreateIteratorFrom(lmScoreCursor* pVCursor);
	lmSOIterator* CreateIteratorFrom(lmStaffObj* pSO);

	//info related to measures
    int GetNumMeasures();
    float GetMeasureDuration(int nMeasure);
	int GetNumVoicesInMeasure(int nMeasure);
    bool IsVoiceUsedInMeasure(int nVoice, int nMeasure);
    lmSegment* GetNextSegment(int nCurSegment);

    //info related to segments
    int GetNumSegments();
    inline lmSegment* GetSegment(int nSegment) { return m_Segments[nSegment]; }

    //other info
    inline lmVStaff* GetOwnerVStaff() { return m_pOwner; }
    inline bool IsEmpty() { return m_pFirstSO == (lmStaffObj*)NULL; }

	//context management
	lmContext* GetCurrentContext(lmStaffObj* pSO, int nStaff = -1);
	lmContext* NewUpdatedContext(int nStaff, lmStaffObj* pSO);
	lmContext* NewUpdatedLastContext(int nStaff);
	lmContext* GetLastContext(int nStaff);
    lmContext* GetStartOfSegmentContext(int nMeasure, int nStaff);

	//debug
	wxString Dump();

    //AutoReBar
    void AutoReBar(lmStaffObj* pFirstSO, lmStaffObj* pLastSO, lmTimeSignature* pNewTS);

	//timepos management
    bool IsTimePosOccupied(lmSegment* pSegment, float rTime, float rDuration, int nVoice);

    //These two methos are only to be used by lmSegment::SortMeasure
    inline void SetFirstSO(lmStaffObj* pSO) { m_pFirstSO = pSO; }
    inline void SetLastSO(lmStaffObj* pSO) { m_pLastSO = pSO; }


private:
	//general management
    void Store(lmStaffObj* pNewSO, bool fClefKeepPosition=true, bool fKeyKeepPitch=true);

	//segments management
	void SplitSegment(int nSegment, lmStaffObj* pLastSO);
    void CreateNewSegment(int nSegment);
    void RemoveSegment(int nSegment);
	void UpdateSegmentContexts(lmSegment* pSegment);

	//voices management
	int AssignVoice(lmStaffObj* pSO, int nSegment);

	//timepos management
    void AssignTime(lmStaffObj* pSO);

    //cursor management
    lmScoreCursor* GetCursor();
    inline int GetCursorSegment() { return GetCursor()->GetSegment(); }
    inline lmStaffObj* GetCursorStaffObj() { return GetCursor()->GetStaffObj(); }
    inline float GetCursorTimepos() { return GetCursor()->GetCursorTime(); }

    inline void AdvanceCursorToNextSegment() { GetCursor()->MoveToNextSegment(); }
    inline void AdvanceCursorToTime(float rTimepos) { GetCursor()->AdvanceToTime(rTimepos); }
    inline void MoveCursorToObject(lmStaffObj* pSO) { GetCursor()->MoveCursorToObject(pSO); }
    inline void MoveCursorToTime(float rTimepos, bool fToEndOfTime=false)
                    { GetCursor()->MoveToTime(rTimepos, fToEndOfTime); }
    inline void MoveCursorRight(bool fAlsoChordNotes) { GetCursor()->MoveRight(fAlsoChordNotes); }
    inline lmScoreCursor* GetScoreCursor() { return GetCursor(); }




    lmVStaff*                   m_pOwner;           //owner VStaff
	std::vector<lmSegment*>		m_Segments;			//segments collection
    int                         m_nNumStaves;       //num staves in the collection

    //The collection is implemented as a doubled linked list:
    lmStaffObj*     m_pFirstSO;         //first SO in the collection (NULL, pSO)
    lmStaffObj*     m_pLastSO;          //last SO in the collection (last barline, NULL, pSO)

};


#endif    // __LM_COLSTAFFOBJS_H__
