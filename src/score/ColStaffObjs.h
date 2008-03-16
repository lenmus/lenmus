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

#ifndef __LM_COLSTAFFOBJS_H__        //to avoid nested includes
#define __LM_COLSTAFFOBJS_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ColStaffObjs.cpp"
#endif

#include <vector>
#include <list>

class lmStaffObj;
class lmNoteRest;
class lmSOIterator;
class lmContext;
class lmVStaff;
class lmInstrument;
class lmVStaffCursor;
class lmColStaffObjs;
class lmBarline;
class lmView;
class lmScoreCursor;
class lmUndoData;


// To simplify future modifications of this class (i.e changing the data structures to implement it)
// when an iterator is requested we will force to specify the intended usage so that 
// this class can optimize it. The defined codes are:
//
//	eTR_ByTime:
//		The StaffObjs must be recovered ordered by increasing time position.
//
//	eTR_AsStored:
//		Order is not important. Items are recovered as it is faster.

enum ETraversingOrder
{
    eTR_AsStored = 1,		//no order, as it is more fast
    eTR_ByTime,				//ordered by timepos
};


#define lmItMeasure		std::list<lmSegmentData*>::iterator 
#define lmItCSO			std::list<lmStaffObj*>::iterator

class lmSegment
{
public:
    lmSegment(lmColStaffObjs* pOwner, int nSegment);
    ~lmSegment();

	void Store(lmStaffObj* pNewSO, lmVStaffCursor* pCursor);
    void UpdateDuration();

	//context management
    inline void SetContext(int iStaff, lmContext* pContext) { m_pContext[iStaff] = pContext; }
	inline lmContext* GetContext(int iStaff) { return m_pContext[iStaff]; }

    //info
    int GetNumVoices();
    bool IsVoiceUsed(int nVoice);
    inline float GetDuration() { return m_rMaxTime; }

	//debug
	wxString Dump();


private:
	friend class lmVStaffCursor;
	friend class lmColStaffObjs;
	friend class lmSOIterator;

    void VoiceUsed(int nVoice);
    void ShiftRightTimepos(lmItCSO itStart, int nVoice);
    void ShiftLeftTimepos(lmNoteRest* pSO, lmItCSO itStart);

    std::list<lmStaffObj*>	m_StaffObjs;		//list of StaffObjs in this segment
    lmColStaffObjs* m_pOwner;                   
    int				m_nNumSegment;				//0..n-1
    lmBarline*		m_pBarline;					//segment barline, if exists
    lmContext*		m_pContext[lmMAX_STAFF];	//ptr to current context for each staff
    int             m_bVoices;                  //voices in this segment. One bit per used voice
    float	        m_rMaxTime;                 //occupied time from start of the measure

};


class lmColStaffObjs
{
public:
    lmColStaffObjs(lmVStaff* pOwner, int nNumStaves);
    ~lmColStaffObjs();

	//creation related
	inline void AttachCursor(lmVStaffCursor* pCursor) { m_pVCursor = pCursor; } 
    void AddStaff();

    //add/remove StaffObjs
    void Add(lmStaffObj* pNewSO);
	void Delete(lmStaffObj* pCursorSO, bool fDelete = true);
	void Insert(lmStaffObj* pNewSO, lmStaffObj* pBeforeSO);
    bool ShiftTime(float rTimeShift);

    //iterator related methods
    lmSOIterator* CreateIterator(ETraversingOrder nOrder, int nVoice=-1);
	lmSOIterator* CreateIteratorTo(ETraversingOrder nOrder, lmStaffObj* pSO);

	//info related to measures
    int GetNumMeasures();
    float GetMeasureDuration(int nMeasure);
	int GetNumVoicesInMeasure(int nMeasure);
    bool IsVoiceUsedInMeasure(int nVoice, int nMeasure);

	//context management
	lmContext* GetCurrentContext(lmStaffObj* pSO);
	lmContext* NewUpdatedContext(lmStaffObj* pSO);
	lmContext* NewUpdatedLastContext(int nStaff);
	lmContext* GetLastContext(int nStaff);
    lmContext* GetStartOfSegmentContext(int nMeasure, int nStaff);

	//debug
	wxString Dump();

    //undo/redo related methods
    void LogPosition(lmUndoData* pUndoData, lmStaffObj* pSO);


private:
	friend class lmSOIterator;
	friend class lmVStaffCursor;
	friend class lmSegment;

	//general management
    void Store(lmStaffObj* pNewSO);

	//segments management
	void SplitSegment(int nSegment, lmStaffObj* pLastSO);
    void CreateNewSegment(int nSegment);
    void RemoveSegment(int nSegment);
	void UpdateContexts(lmSegment* pSegment);

	//voices management
	void AssignVoice(lmStaffObj* pSO, int nSegment);

	//timepos management
    void AssignTime(lmStaffObj* pSO);
    bool IsTimePosOccupied(lmSegment* pSegment, float rTime, float rDuration, int nVoice);


    lmVStaff*                   m_pOwner;           //owner VStaff
	std::vector<lmSegment*>		m_Segments;			//segments collection
    int                         m_nNumStaves;       //num staves in the collection
	lmVStaffCursor*          	m_pVCursor;			//cursor

};



// Cursor pointing to current position

//cursor state
typedef struct lmVCursorState_Struct {
	int         nStaff;         //staff (1..n)
	int         nSegment;       //current segment (0..n-1)
	float       rTimepos;       //timepos
	lmStaffObj* pSO;			//current pointed staffobj
} lmVCursorState;

class lmVStaffCursor
{
public:
	lmVStaffCursor();
	~lmVStaffCursor() {}

	//creation related
	void AttachToCollection(lmColStaffObjs* pColStaffObjs);

    //attachment to a ScoreCursor
	lmVStaffCursor* AttachCursor(lmScoreCursor* pSCursor);
	void DetachCursor();

    //positioning
	void MoveRight(bool fAlsoChordNotes = true, bool fIncrementIterator = true);
	void MoveLeft(bool fAlsoChordNotes = true);
    void MoveToTime(float rNewTime);
    void MoveToFirst(int nStaff=0);
    void ResetCursor();
    void AdvanceToTime(float rTime);
    void AdvanceToNextSegment();
	void MoveToSegment(int nSegment, int nStaff, lmUPoint uPos);
    void MoveCursorToObject(lmStaffObj* pSO);
    void SetNewCursorState(lmScoreCursor* pSCursor, lmVCursorState* pState);

    //call backs
    void OnCursorObjectDeleted(lmItCSO itNext);

    //current position
    bool IsAtEnd();
    bool IsAtBeginning();

	//access to cursor info
	inline int GetSegment() { return m_nSegment; }
	lmStaffObj* GetStaffObj();
    inline float GetTimepos() { return m_rTimepos; }
    inline lmItCSO GetCurIt() { return m_it; }
    lmVCursorState GetState();

    lmUPoint GetCursorPoint();
    lmStaff* GetCursorStaff();
    inline int GetCursorNumStaff() { return m_nStaff; }
    lmVStaff* GetVStaff();



private:
    void UpdateTimepos();
    lmStaffObj* GetPreviousStaffobj();


	lmColStaffObjs*		m_pColStaffObjs;	//collection pointed by this cursor
    lmScoreCursor*      m_pScoreCursor;     //ScoreCursor using this VCursor
	lmSegment*			m_pSegment;			//current segment

    //state variables
    int					m_nStaff;			//staff (1..n)
	int					m_nSegment;			//current segment (0..n-1)
	float				m_rTimepos;			//timepos
	lmItCSO				m_it;				//iterator, to speed up cursor moves

};



#endif    // __LM_COLSTAFFOBJS_H__
