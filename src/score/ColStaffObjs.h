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
class lmVStaffCursor;
class lmColStaffObjs;
class lmBarline;
class lmView;
class lmScoreCursor;
class lmTimeSignature;
class lmKeySignature;

#include "StaffObj.h"   //EStaffObjType


#define lmNO_TIME_LIMIT     100000000.0f        //a too big measure duration

#define lmItMeasure		std::list<lmSegmentData*>::iterator

class lmSegment
{
public:
    lmSegment(lmColStaffObjs* pOwner, int nSegment);
    ~lmSegment();

    //operations
	void Store(lmStaffObj* pNewSO, lmVStaffCursor* pCursor);
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
    lmStaffObj* GetStaffObj(int nPosition);

	//debug
	wxString Dump();



private:

    void VoiceUsed(int nVoice);
    void SortMeasure();
    void ShiftRightTimepos(lmStaffObj* pStartSO, float rTimeShift);
    void AddToCollection(lmStaffObj* pNewSO, lmStaffObj* pNextSO);
    void ShiftLeftTimepos(lmStaffObj* pStartSO, float rTimeShift, float rStartTimepos, int nVoice);
    void CountObjects();

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
typedef struct lmCursorState_Struct
{
    int         nInstr;         //instrument (1..n)
	int         nStaff;         //staff (1..n)
	float       rTimepos;       //timepos
	lmStaffObj* pSO;			//current pointed staffobj
}
lmCursorState;


//global variable used as default initializator
extern lmCursorState g_tNoCursorState;

//global function to compare with g_tNoCursorState
extern bool IsEmptyState(lmCursorState& t);
// Cursor pointing to current position



class lmVStaffCursor
{
public:
	lmVStaffCursor();
	~lmVStaffCursor();

	//creation related
	void AttachToCollection(lmColStaffObjs* pColStaffObjs, bool fReset=true);

    //attachment to a ScoreCursor
	lmVStaffCursor* AttachCursor(lmScoreCursor* pSCursor);

    //positioning

    //Move methods: intended to implement user commands. They call back ScoreObj to
    //inform it about a position change, for highlight or GUI update.
	void MoveRight(bool fAlsoChordNotes = true);
	void MoveLeft(bool fAlsoChordNotes = true);
    void MoveToTime(float rNewTime);
    void MoveToFirst(int nStaff=0);
	void MoveToSegment(int nSegment, int iStaff, lmUPoint uPos);
    void MoveCursorToObject(lmStaffObj* pSO);
    void MoveTo(int iStaff, int nSegment, float rTime);

    //Advance methods: Intended for internal usage. They do not inform ScoreObj about
    //position change.
    void ResetCursor();
    void AdvanceToTime(float rTime);
    void AdvanceToNextSegment();
    void AdvanceToStartOfSegment(int nSegment, int nStaff);
    void AdvanceToStartOfTimepos();
    void SetCursorState(lmScoreCursor* pSCursor, lmCursorState* pState, bool fUpdateTimepos=false);
    void SkipClefKey(bool fSkipKey);

    //current position
    bool IsAtEnd();
    bool IsAtBeginning();

	//access to cursor info
	int GetSegment();
    inline int GetInstrumentNumber() { return m_nInstr; }
    inline float GetTimepos() { return m_rTimepos; }
    inline int GetNumStaff() { return m_nStaff; }

    lmStaffObj* GetStaffObj();
    lmCursorState GetState();

    inline lmScoreCursor* GetScoreCursor() { return m_pScoreCursor; }
    lmStaffObj* GetPreviousStaffobj();
    lmContext* GetCurrentContext();

    lmUPoint GetCursorPoint(int* pNumPage = NULL);
    lmStaff* GetCursorStaff();
    lmVStaff* GetVStaff();

    //other
    lmStaffObj* GetReferredObject(lmIRef& nIRef);


private:
    void UpdateTimepos();
    float GetStaffPosY(lmStaffObj* pSO);

    //cursor
    void MoveRightToNextTime();
    void MoveLeftToPrevTime();

    //helper, for cursor common operations
    void PositionAt(float rTargetTimepos);


	lmColStaffObjs*		m_pColStaffObjs;	//collection pointed by this cursor
    lmScoreCursor*      m_pScoreCursor;     //ScoreCursor using this VCursor

    //state variables
    int             m_nInstr;       //instrument (1..n)
    int				m_nStaff;       //staff (1..n)
	float			m_rTimepos;     //timepos
    int             m_nSegment;     //segment number (0..n). -1 if no StaffObj
    int             m_nPosition;    //position in the list (0..n)

    lmSOIterator*   m_pIt;          //iterator pointing to current SO

};

//=======================================================================================
// class lmScoreCursor
//=======================================================================================

class lmScoreCursor
{
public:
    lmScoreCursor(lmScore* pOwnerScore);
    ~lmScoreCursor() {}

    //positioning
    void MoveToStart();
    void MoveRight(bool fAlsoChordNotes = true);
    void MoveLeft(bool fAlsoChordNotes = true);
    void MoveUp();
    void MoveDown();
	void MoveNearTo(lmUPoint uPos, lmVStaff* pVStaff, int iStaff, int nMeasure);
    void MoveCursorToObject(lmStaffObj* pSO);
    void MoveTo(lmVStaff* pVStaff, int iStaff, int nMeasure, float rTime);
    void PointCursorToInstrument(int nInstr);

    //internal state (setting it implies re-positioning)
    void SetState(lmCursorState* pState);
    lmCursorState GetState();

    //access to state info. Only meaninful if IsOK()
    inline bool IsOK() { return (GetCursorInstrumentNumber() != 0); }
    inline int GetCursorNumStaff() { return m_VCursor.GetNumStaff(); }
    inline float GetCursorTime() { return m_VCursor.GetTimepos(); }
    inline lmStaffObj* GetCursorSO() { return m_VCursor.GetStaffObj(); }
    inline int GetCursorInstrumentNumber() { return m_VCursor.GetInstrumentNumber(); }
    inline int GetSegment() { return m_VCursor.GetSegment(); }

    //position related info
    lmVStaff* GetVStaff();
    lmStaff* GetCursorStaff();
    lmUPoint GetCursorPoint(int* pNumPage = NULL);

    //other info
	inline lmScore* GetCursorScore() { return m_pScore; }
    inline lmVStaffCursor* GetVCursor() { return &m_VCursor; }
    lmStaffObj* GetReferredObject(lmIRef& nIRef);


private:
    void MoveToInitialPosition();

    lmScore*            m_pScore;           //owner score
	lmVStaffCursor		m_VCursor;		    //internal instrument cursor
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
    lmStaffObj* FindFwdStaffObj(lmStaffObj* pCurSO, EStaffObjType nType);
    lmStaffObj* FindNextStaffObj(lmStaffObj* pCurSO);
    lmStaffObj* FindPrevStaffObj(lmStaffObj* pCurSO);
    inline lmStaffObj* GetFirstSO() { return m_pFirstSO; }
    inline lmStaffObj* GetLastSO() { return m_pLastSO; }

    //collection access and iterator related methods
	lmSOIterator* CreateIterator();
	lmSOIterator* CreateIteratorTo(lmStaffObj* pSO);
	lmSOIterator* CreateIteratorFrom(lmVStaffCursor* pVCursor);
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
    lmVStaffCursor* GetCursor();
    inline int GetCursorSegment() { return GetCursor()->GetSegment(); }
    inline lmStaffObj* GetCursorStaffObj() { return GetCursor()->GetStaffObj(); }
    inline float GetCursorTimepos() { return GetCursor()->GetTimepos(); }

    inline void AdvanceCursorToNextSegment() { GetCursor()->AdvanceToNextSegment(); }
    inline void AdvanceCursorToTime(float rTimepos) { GetCursor()->AdvanceToTime(rTimepos); }
    inline void MoveCursorToObject(lmStaffObj* pSO) { GetCursor()->MoveCursorToObject(pSO); }
    inline void MoveCursorToTime(float rTimepos) { GetCursor()->MoveToTime(rTimepos); }
    inline void MoveCursorRight(bool fAlsoChordNotes) { GetCursor()->MoveRight(fAlsoChordNotes); }
    inline lmScoreCursor* GetScoreCursor() { return GetCursor()->GetScoreCursor(); }




    lmVStaff*                   m_pOwner;           //owner VStaff
	std::vector<lmSegment*>		m_Segments;			//segments collection
    int                         m_nNumStaves;       //num staves in the collection

    //The collection is implemented as a doubled linked list:
    lmStaffObj*     m_pFirstSO;         //first SO in the collection (NULL, pSO)
    lmStaffObj*     m_pLastSO;          //last SO in the collection (last barline, NULL, pSO)

};


#endif    // __LM_COLSTAFFOBJS_H__
