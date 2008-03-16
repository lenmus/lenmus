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

#ifndef __LM_STAFFOBJ_H__        //to avoid nested includes
#define __LM_STAFFOBJ_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "StaffObj.cpp"
#endif

#include <vector>

#include "../app/TheApp.h"
#include "../graphic/Shapes.h"

#if wxUSE_GENERIC_DRAGIMAGE
#include "wx/generic/dragimgg.h"
#define wxDragImage wxGenericDragImage
#else
#include "wx/dragimag.h"
#endif

#include "ColStaffObjs.h"

class lmUndoData;


#define lmDRAGGABLE         true
#define lmNO_DRAGGABLE      false
#define lmVISIBLE           true
#define lmNO_VISIBLE        false




//-------------------------------------------------------------------------------------------
// class lmScoreObj
//  This is the most abstract object. An object has an associated  context options obj.
//  and a parent
//-------------------------------------------------------------------------------------------

//type of ScoreObj
enum lmEScoreObjType {
	lmSOT_Instrument = 0,
	lmSOT_Score,
	lmSOT_Staff,
	lmSOT_ComponentObj,
	lmSOT_VStaff,
};

class lmAuxObj;
typedef std::vector<lmAuxObj*> lmAuxObjsCol; 
class lmObjOptions;
class lmBox;
class lmController;

class lmScoreObj
{
public:
    virtual ~lmScoreObj();

    //--- Options --------------------------------------------
    
    // get and set the value of an option
    lmObjOptions* GetCurrentObjOptions();
    lmObjOptions* GetObjOptions() { return m_pObjOptions; }
    void SetOption(wxString sName, long nLongValue);
    void SetOption(wxString sName, wxString sStringValue);
    void SetOption(wxString sName, double nDoubleValue);
    void SetOption(wxString sName, bool fBoolValue);

    //Look for the value of an option. A method for each supported data type.
    //Recursive search throug the ObjOptions chain
    long GetOptionLong(wxString sOptName);
    double GetOptionDouble(wxString sOptName);
    bool GetOptionBool(wxString sOptName);
    wxString GetOptionString(wxString sOptName);

	//--- ScoreObj properties ------------------------------
	virtual lmEScoreObjType GetScoreObjType()=0;

    //--- a ScoreObj can own other ScoreObjs -----------------------
    inline lmScoreObj* GetParentScoreObj() { return m_pParent; }


    //provide units conversion
    virtual lmLUnits TenthsToLogical(lmTenths nTenths)=0;
    virtual lmTenths LogicalToTenths(lmLUnits uUnits)=0;

    //attach and detach AuxObjs
    int AttachAuxObj(lmAuxObj* pAO);
    void DetachAuxObj(lmAuxObj* pAO);


	//--- a ScoreObj can be renderizable

	//position and main shape
    virtual void StoreOriginAndShiftShapes(lmLUnits uLeft);
	virtual lmLocation SetUserLocation(lmLocation tPos);
	inline lmShape* GetShap2() { return m_pShape; }
    virtual lmUPoint& GetReferencePaperPos() { return m_uPaperPos; }
    int GetPageNumber();
    virtual void SetOrigin(lmUPoint uPos) { m_uOrg = uPos; }
    virtual lmUPoint GetOrigin() { return m_uOrg; }
	virtual lmUPoint SetReferencePos(lmPaper* pPaper);
	virtual void SetReferencePos(lmUPoint& uPos);
	void ResetObjectLocation();

    //contextual menu
	virtual void PopupMenu(lmController* pCanvas, lmGMObject* pGMO, const lmDPoint& vPos);
	virtual void CustomizeContextualMenu(wxMenu* pMenu, lmGMObject* pGMO);

	//handlers for contextual menu
	virtual void OnProperties(lmGMObject* pGMO);

	//font to use to render the ScoreObj
	virtual wxFont* GetSuitableFont(lmPaper* pPaper);

    //debug methods
    virtual wxString Dump()=0;

	//edit management
	inline bool IsModified() { return m_fModified; }
	virtual void RecordHistory(lmUndoData* pUndoData);
	virtual void AcceptChanges();


protected:
    lmScoreObj(lmScoreObj* pParent);

    lmScoreObj*		m_pParent;          //the parent for the ObjOptions chain
    lmObjOptions*   m_pObjOptions;      //the collection of options or NULL if none
    lmAuxObjsCol*   m_pAuxObjs;         //the collection of attached AuxObjs or NULL if none
	bool			m_fModified;		//the object has been modified, directly or indirectly, by a user edit operation

	//information only valid for rendering as score: position and shape
    //These variables are only valid for the Formatter algorithm and, therefore, are not
	//valid for other views using different formats.
    lmLocation      m_tPos;         //desired position for this object
	lmLocation		m_tSrcPos;		//position specified in source code
    lmUPoint		m_uPaperPos;	//relative origin to render this object: paper position
    lmShape*		m_pShape;		//new shape
    lmUPoint		m_uOrg;	        //real origin to render this object: paper position

};


//-------------------------------------------------------------------------------------------
// class lmComponentObj
//-------------------------------------------------------------------------------------------

enum EScoreObjType
{
    eSCOT_StaffObj = 1,         // staff objects (lmStaffObj). Main objects. Consume time
    eSCOT_AuxObj,               // aux objects (lmAuxObj). Auxiliary. Owned by StaffObjs
};


class lmComponentObj : public lmScoreObj
{
public:
    virtual ~lmComponentObj();

	//---- virtual methods of base class -------------------------

    // units conversion
    virtual lmLUnits TenthsToLogical(lmTenths nTenths)=0;
    virtual lmTenths LogicalToTenths(lmLUnits uUnits)=0;

	inline lmEScoreObjType GetScoreObjType() { return lmSOT_ComponentObj; }


	//---- specific methods of this class ------------------------

	// type and identificaction
    inline int GetID() const { return m_nId; }
    inline EScoreObjType GetType() const { return m_nType; }

    // graphic model
    virtual void Layout(lmBox* pBox, lmPaper* pPaper, 
						wxColour colorC = *wxBLACK, bool fHighlight = false)=0;
	virtual lmUPoint ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper)=0;


protected:
    lmComponentObj(lmScoreObj* pParent, EScoreObjType nType, lmLocation* pPos = &g_tDefaultPos,
                   bool fIsDraggable = false);

    wxString SourceLDP_Location(lmUPoint uPaperPos);
	lmUPoint ComputeObjectLocation(lmPaper* pPaper);


    EScoreObjType   m_nType;        //type of ComponentObj
    int             m_nId;          //unique number, to identify each lmComponentObj
    bool            m_fIsDraggable;


};


//-------------------------------------------------------------------------------------------
//    lmStaffObj
//-------------------------------------------------------------------------------------------

enum EStaffObjType
{
    eSFOT_Clef = 1,             // clef (lmClef)
    eSFOT_KeySignature,         // key signature (lmKeySignature)
    eSFOT_TimeSignature,        // time signature (lmTimeSignature)
    eSFOT_Notation,             // notations (lmNotation). Has subtype
    eSFOT_Barline,              // barlines (lmBarline)
    eSFOT_NoteRest,             // notes and rests (lmNoreRest)
    eSFOT_Text,                 // texts (lmScoreText)
    eSFOT_Control,              // control element (backup, forward) (lmSOControl)
    eSFOT_MetronomeMark,        // metronome mark (lmMetronomeMark)
    eSFOT_TupletBracket,        // tuplet bracket (lmTupletBracket)
};


class lmVStaff;
class lmAuxObj;
class lmContext;

class lmStaffObj : public lmComponentObj
{
public:
    virtual ~lmStaffObj();

	//---- virtual methods of base class -------------------------

    virtual void Layout(lmBox* pBox, lmPaper* pPaper, wxColour colorC = *wxBLACK,
                        bool fHighlight = false);
    //virtual void SetFont(lmPaper* pPaper);
	virtual wxFont* GetSuitableFont(lmPaper* pPaper);

    // units conversion
    lmLUnits TenthsToLogical(lmTenths nTenths);
    lmTenths LogicalToTenths(lmLUnits uUnits);

    // debug
    virtual wxString Dump();

	//---- specific methods of this class ------------------------

    // characteristics
    virtual inline bool IsSizeable() { return false; }
    inline bool IsVisible() { return m_fVisible; }
    inline EStaffObjType GetClass() { return m_nClass; }
	virtual wxString GetName() const=0;

    //classification
    inline bool IsClef() { return GetClass() == eSFOT_Clef; }
    inline bool IsKeySignature() { return GetClass() == eSFOT_KeySignature; }
    inline bool IsTimeSignature() { return GetClass() == eSFOT_TimeSignature; }
    inline bool IsNotation() { return GetClass() == eSFOT_Notation; }
    inline bool IsBarline() { return GetClass() == eSFOT_Barline; }
    inline bool IsNoteRest() { return GetClass() == eSFOT_NoteRest; }
    inline bool IsText() { return GetClass() == eSFOT_Text; }
    inline bool IsControl() { return GetClass() == eSFOT_Control; }
    inline bool IsMetronomeMark() { return GetClass() == eSFOT_MetronomeMark; }
    inline bool IsTupletBracket() { return GetClass() == eSFOT_TupletBracket; }

    //inline bool IsNote() { return m_nClass == eSFOT_NoteRest && !((lmNote*)this)->IsRest(); }
    //inline bool IsRest() { return m_nClass == eSFOT_NoteRest && ((lmNote*)this)->IsRest(); }

    // source code related methods
    virtual wxString SourceLDP(int nIndent);
    virtual wxString SourceXML(int nIndent);

    // methods related to time and duration
    float GetTimePos() { return m_rTimePos; }
    void SetTimePos(float rTimePos) { m_rTimePos = rTimePos; }
    virtual float GetTimePosIncrement() { return 0; }

    // methods related to positioning
    virtual lmLUnits GetAnchorPos() {return m_pShape->GetXLeft(); }

	//highligh
	virtual void PlaybackHighlight(lmPaper* pPaper, wxColour colorC) {}
	virtual void CursorHighlight(lmPaper* pPaper, int nStaff, bool fHighlight);

    // methods related to staff ownership
    inline int GetStaffNum() { return m_nStaffNum; }
    inline lmVStaff* GetVStaff() { return m_pVStaff; }
	inline void SetSegment(lmSegment* pSegment) { m_pSegment = pSegment; }
	inline lmSegment* GetSegment() { return m_pSegment; }

    // methods related to AuxObj/GraphObj ownership
    virtual bool IsComposite() { return false; }

	//contexts
	lmContext* GetCurrentContext();
	lmContext* NewUpdatedContext();


protected:
    lmStaffObj(lmScoreObj* pParent, EStaffObjType nType,
             lmVStaff* pStaff = (lmVStaff*)NULL, int nStaff=1,    // only for staff owned objects
             bool fVisible = true,
             bool fIsDraggable = false);

	//rendering
    virtual lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)=0;



    //properties
    bool            m_fVisible;     // this lmComponentObj is visible on the score
    EStaffObjType   m_nClass;       // type of StaffObj

    // time related variables
    float			m_rTimePos;		// time from start of measure

    // Info about staff ownership
    lmVStaff*		m_pVStaff;		// lmVStaff owning this lmStaffObj
    int				m_nStaffNum;	// lmStaff (1..n) on which this object is located
	lmSegment* 		m_pSegment;		// ptr to segment including this staffobj 


};

// declare a list of StaffObjs class
#include "wx/list.h"
WX_DECLARE_LIST(lmStaffObj, StaffObjsList);


//-------------------------------------------------------------------------------------------
//    lmAuxObj
//-------------------------------------------------------------------------------------------

enum lmEAuxObjType
{
    eAXOT_Fermata,
    eAXOT_Lyric,
    eAXOT_Text,

    //graphic objects
    eAXOT_Line,
};

class lmAuxObj : public lmComponentObj
{
public:
    virtual ~lmAuxObj() {}

	//---- virtual methods of base class -------------------------

    virtual void Layout(lmBox* pBox, lmPaper* pPaper, 
						wxColour colorC = *wxBLACK, bool fHighlight = false);
	virtual wxFont* GetSuitableFont(lmPaper* pPaper);

    // units conversion
    lmLUnits TenthsToLogical(lmTenths nTenths);
    lmTenths LogicalToTenths(lmLUnits uUnits);

    // debug methods
    virtual wxString Dump()=0;

    virtual void StoreOriginAndShiftShapes(lmLUnits uLeft);

	//---- specific methods of this class ------------------------

    // ownership
    void SetOwner(lmScoreObj* pOwner);
    virtual void SetOrigin(lmUPoint uPos);
    virtual lmUPoint GetOrigin();

    // class info
    virtual lmEAuxObjType GetAuxObjType()=0;

    // source code related methods
    virtual wxString SourceLDP(int nIndent)=0;
    virtual wxString SourceXML(int nIndent)=0;


protected:
    lmAuxObj(bool fIsDraggable = false);
    virtual lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)=0;
	virtual lmUPoint SetReferencePos(lmPaper* pPaper);

};

// declare a list of AuxObjs
#include "wx/list.h"
WX_DECLARE_LIST(lmAuxObj, AuxObjsList);

#endif    // __LM_STAFFOBJ_H__
