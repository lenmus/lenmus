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

class lmSegment;
class lmUndoData;
class lmUndoItem;
class lmTextItem;


#define lmDRAGGABLE         true
#define lmNO_DRAGGABLE      false
#define lmVISIBLE           true
#define lmNO_VISIBLE        false


//---------------------------------------------------------------------------------------
// Helper classes for to define different shapes management behaviours
//---------------------------------------------------------------------------------------

//an object that is renderized by using shapes. Default behaviour is to use one shape
class lmShapesMngr       
{
public:
    lmShapesMngr();
    virtual~lmShapesMngr();

    //graphic objects
    lmShape* GetShape(int nShapeIdx) { return (lmShape*)GetGraphicObject(nShapeIdx); }
    virtual lmGMObject* GetGraphicObject(int nShapeIdx);
	virtual void SaveUserLocation(lmLUnits xPos, lmLUnits yPos, int nShapeIdx);
	void SaveUserXLocation(lmLUnits xPos, int nShapeIdx);
	virtual lmUPoint GetUserShift(int nShapeIdx);
    virtual void StoreShape(lmGMObject* pGMObj);
    virtual int NewShapeIndex() { return 0; }
    virtual void SetShapesIndexCounter(int nNextIndex) {}    
    virtual void Init(bool fDeleteShapes) {}
    virtual bool IsMultishaped() { return false; }

protected:
    lmGMObject*		m_pGMObj;		//shape/box that renders this object
    lmUPoint        m_uUserShift;   //(0.0, 0.0) if no user requirements
};


//an object that is renderized using many independent shapes
class lmMultiShapesMngr : public lmShapesMngr
{
public:
    lmMultiShapesMngr();
    ~lmMultiShapesMngr();

    //implementation of virtual methods
    lmGMObject* GetGraphicObject(int nShapeIdx);
	void SaveUserLocation(lmLUnits xPos, lmLUnits yPos, int nShapeIdx);
	void SaveUserXLocation(lmLUnits xPos, int nShapeIdx);
	lmUPoint GetUserShift(int nShapeIdx);
    void StoreShape(lmGMObject* pGMObj);
    inline int NewShapeIndex() { return m_nNextIdx++; }
    inline void SetShapesIndexCounter(int nNextIndex) { m_nNextIdx = nNextIndex; }    
    void Init(bool fDeleteShapes);
    inline bool IsMultishaped() { return true; }

protected:

	typedef struct lmShapeInfo_Struct {
		lmGMObject*		pGMObj;			//ptr. to shape
		lmUPoint    	uUserShift;		//user shift
	} lmShapeInfo;

	//array of shapes
    int     m_nNextIdx;
    std::vector<lmShapeInfo*> m_ShapesInfo;
};



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
class lmScore;
class lmDlgProperties;

class lmScoreObj
{
public:
    virtual ~lmScoreObj();

    //building
    void DefineAsMultiShaped();

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
	bool IsInstrument() { return GetScoreObjType() == lmSOT_Instrument; }
	bool IsScore() { return GetScoreObjType() == lmSOT_Score; }
	bool IsStaff() { return GetScoreObjType() == lmSOT_Staff; }
	bool IsComponentObj() { return GetScoreObjType() == lmSOT_ComponentObj; }
	bool IsVStaff() { return GetScoreObjType() == lmSOT_VStaff; }

    //--- a ScoreObj can own other ScoreObjs -----------------------
    inline lmScoreObj* GetParentScoreObj() { return m_pParent; }
    virtual lmScore* GetScore()=0;
    virtual lmStaff* GetStaff() { return (lmStaff*)NULL; }


    //provide units conversion
    virtual lmLUnits TenthsToLogical(lmTenths nTenths)=0;
    virtual lmTenths LogicalToTenths(lmLUnits uUnits)=0;

    //attach and detach AuxObjs
    virtual int AttachAuxObj(lmAuxObj* pAO, int nIndex = -1);
    virtual int DetachAuxObj(lmAuxObj* pAO);


	//--- a ScoreObj can be renderizable

    //incremental renderization
    virtual bool IsDirty() const { return m_fDirty; }
    virtual void SetDirty(bool fValue, bool fPropagate=false);

    //interface with shapes manager
    inline void StoreShape(lmGMObject* pGMObj) { m_pShapesMngr->StoreShape(pGMObj); }
    inline lmShape* GetShapeFromIdx(int nShapeIdx=0) { return (lmShape*)GetGraphicObject(nShapeIdx); }
    virtual lmShape* GetShape(int nStaff=1);
    inline lmGMObject* GetGraphicObject(int nShapeIdx=0) { return m_pShapesMngr->GetGraphicObject(nShapeIdx); }
	inline void SaveUserLocation(lmLUnits xPos, lmLUnits yPos, int nShapeIdx = 0) {
                m_pShapesMngr->SaveUserLocation(xPos, yPos, nShapeIdx); 
            }
	inline void SaveUserXLocation(lmLUnits xPos, int nShapeIdx = 0) {
                m_pShapesMngr->SaveUserXLocation(xPos, nShapeIdx); 
            }

    inline lmUPoint GetUserShift(int nShapeIdx = 0) { return m_pShapesMngr->GetUserShift(nShapeIdx); }
    inline int NewShapeIndex() { return m_pShapesMngr->NewShapeIndex(); }
    inline void SetShapesIndexCounter(int nNextIdx) {
                m_pShapesMngr->SetShapesIndexCounter(nNextIdx);
            }

    //other shapes related methods
    virtual inline bool IsMainShape(int nShapeIdx) { return nShapeIdx == 0; }
    inline bool IsMultishaped() { return m_pShapesMngr->IsMultishaped(); }

	//positioning
    virtual void StoreOriginAndShiftShapes(lmLUnits uLeft, int nShapeIdx = 0);
	virtual lmLocation SetUserLocation(lmLocation tPos, int nShapeIdx = 0);
    virtual lmLocation SetUserLocation(lmTenths xPos, lmTenths yPos, int nShapeIdx = 0);
	virtual lmLUnits SetUserXLocation(lmLUnits uxPos, int nShapeIdx = 0);
    virtual lmUPoint& GetReferencePaperPos() { return m_uPaperPos; }
    int GetPageNumber();
    inline lmUPoint GetLayoutRefPos() { return m_uComputedPos; }
    inline void SetLayoutRefPos(lmUPoint uPos) { m_uComputedPos = uPos; }

	virtual lmUPoint SetReferencePos(lmPaper* pPaper);
	virtual void SetReferencePos(lmUPoint& uPos);

    //contextual menu
	virtual void PopupMenu(lmController* pCanvas, lmGMObject* pGMO, const lmDPoint& vPos);
	virtual void CustomizeContextualMenu(wxMenu* pMenu, lmGMObject* pGMO);

	//handlers for contextual menu and related methods
	virtual void OnProperties(lmController* pController, lmGMObject* pGMO);
	virtual void OnEditProperties(lmDlgProperties* pDlg, const wxString& sTabName = wxEmptyString);
    virtual void OnPropertiesChanged() {}

	//font to use to render the ScoreObj
	virtual wxFont* GetSuitableFont(lmPaper* pPaper);


    //--- other methods

    //debug methods
	virtual wxString Dump();
    virtual wxString SourceLDP(int nIndent);
    virtual wxString SourceXML(int nIndent);



protected:
    lmScoreObj(lmScoreObj* pParent);
    void PrepareToCreateShapes();

    lmScoreObj*		m_pParent;          //the parent for the ObjOptions chain
    lmObjOptions*   m_pObjOptions;      //the collection of options or NULL if none
    lmAuxObjsCol*   m_pAuxObjs;         //the collection of attached AuxObjs or NULL if none
    bool            m_fDirty;           //the object has been modified and needs layout recomputation

	//information only valid for rendering as score: position and shape
    //These variables are only valid for the Formatter algorithm and, therefore, are not
	//valid for other views using different formats.
    lmUPoint		m_uPaperPos;	//relative origin to render this object: paper position

    lmUPoint        m_uComputedPos; //absolute (referenced to top-left paper margin corner)

private:
    lmShapesMngr*   m_pShapesMngr;
};


//-------------------------------------------------------------------------------------------
// class lmComponentObj
//-------------------------------------------------------------------------------------------

enum lmEComponentObjType
{
    lm_eStaffObj = 1,         // staff objects (lmStaffObj). Main objects. Consume time
    lm_eAuxObj,               // aux objects (lmAuxObj). Auxiliary. Owned by StaffObjs
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
    virtual lmScore* GetScore()=0;


	//---- specific methods of this class ------------------------

	// type and identificaction
    inline int GetID() const { return m_nId; }
    inline lmEComponentObjType GetType() const { return m_nType; }
	inline bool IsStaffObj() { return m_nType == lm_eStaffObj; }
	inline bool IsAuxObj() { return m_nType == lm_eAuxObj; }

    // graphical model
    virtual void Layout(lmBox* pBox, lmPaper* pPaper, bool fHighlight = false)=0;
    virtual lmUPoint ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper);

    //properties
    inline void SetColour(wxColour color) { m_color = color; }



protected:
    lmComponentObj(lmScoreObj* pParent, lmEComponentObjType nType, bool fIsDraggable = false);


    lmEComponentObjType     m_nType;        //type of ComponentObj
    int                     m_nId;          //unique number, to identify each lmComponentObj
    bool                    m_fIsDraggable;
	wxColour		        m_color;        //object color

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
    eSFOT_Text,                 // texts (lmTextItem)
    eSFOT_Control,              // control element (backup, forward) (lmSOControl)
    eSFOT_MetronomeMark,        // metronome mark (lmMetronomeMark)
    eSFOT_TupletBracket,        // tuplet bracket (lmTupletBracket)
};


class lmVStaff;
class lmAuxObj;
class lmContext;
class lmClef;
class lmTimeSignature;
class lmKeySignature;


class lmStaffObj : public lmComponentObj
{
public:
    virtual ~lmStaffObj();

	//---- virtual methods of base class -------------------------

    virtual void Layout(lmBox* pBox, lmPaper* pPaper, bool fHighlight = false);
	virtual wxFont* GetSuitableFont(lmPaper* pPaper);
    lmScore* GetScore();
    lmStaff* GetStaff();

    // units conversion
    lmLUnits TenthsToLogical(lmTenths nTenths);
    lmTenths LogicalToTenths(lmLUnits uUnits);

    // debug
    virtual wxString Dump();

	//---- specific methods of this class ------------------------

    // characteristics
    virtual inline bool IsSizeable() { return false; }
    inline bool IsVisible() { return m_fVisible; }
	inline void SetVisible(bool fVisible) { m_fVisible = fVisible; }
    inline EStaffObjType GetClass() { return m_nClass; }
	virtual wxString GetName() const=0;

    //classification
    inline bool IsClef() { return m_nClass == eSFOT_Clef; }
    inline bool IsKeySignature() { return m_nClass == eSFOT_KeySignature; }
    inline bool IsTimeSignature() { return m_nClass == eSFOT_TimeSignature; }
    inline bool IsNotation() { return m_nClass == eSFOT_Notation; }
    inline bool IsBarline() { return m_nClass == eSFOT_Barline; }
    inline bool IsNoteRest() { return m_nClass == eSFOT_NoteRest; }
    inline bool IsText() { return m_nClass == eSFOT_Text; }
    inline bool IsControl() { return m_nClass == eSFOT_Control; }
    inline bool IsMetronomeMark() { return m_nClass == eSFOT_MetronomeMark; }
    inline bool IsTupletBracket() { return m_nClass == eSFOT_TupletBracket; }

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
    virtual lmLUnits GetAnchorPos() {return GetShape()->GetXLeft(); }

	//layout and highligh
    virtual lmShape* CreateInvisibleShape(lmBox* pBox, lmUPoint uPos, int nShapeIdx);
	virtual void PlaybackHighlight(wxDC* pDC, wxColour colorC) {}

    // methods related to staff
    inline int GetStaffNum() { return m_nStaffNum; }
    inline lmVStaff* GetVStaff() { return m_pVStaff; }
	inline void SetSegment(lmSegment* pSegment) { m_pSegment = pSegment; }
	inline lmSegment* GetSegment() { return m_pSegment; }
    inline bool IsOnStaff(int nStaff) { return (m_nStaffNum == nStaff
                                                || IsKeySignature()
                                                || IsTimeSignature()
                                                || IsBarline() ); 
                                      }


    // methods related to AuxObj/GraphObj ownership
    virtual bool IsComposite() { return false; }

	//contexts
	lmContext* GetCurrentContext(int nStaff = -1);
	lmContext* NewUpdatedContext();
    virtual void RemoveCreatedContexts() {}
    lmEClefType GetApplicableClefType();
    lmClef* GetApplicableClef();
    lmTimeSignature* GetApplicableTimeSignature();
    lmKeySignature* GetApplicableKeySignature();


    //undo/redo
    virtual void Freeze(lmUndoData* pUndoData) {};
    virtual void UnFreeze(lmUndoData* pUndoData) {};

	//navigation in the VStaff collection and collection management
	inline lmStaffObj* GetPrevSO() const { return m_pPrevSO; }
	inline lmStaffObj* GetNextSO() const { return m_pNextSO; }
	inline void SetPrevSO(lmStaffObj* pPrevSO) { m_pPrevSO = pPrevSO; }
	inline void SetNextSO(lmStaffObj* pNextSO) { m_pNextSO = pNextSO; }



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

    //info for chaining StaffObjs in the VStaff collection
    lmStaffObj*     m_pNextSO;
    lmStaffObj*     m_pPrevSO;


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
    eAXOT_TextItem,
	eAOXT_TextBlock,

    //graphic objects
    eAXOT_Line,
};

class lmAuxObj : public lmComponentObj
{
public:
    virtual ~lmAuxObj() {}

	//---- virtual methods of base class -------------------------

    virtual void Layout(lmBox* pBox, lmPaper* pPaper, bool fHighlight = false);
	virtual wxFont* GetSuitableFont(lmPaper* pPaper);
    inline lmScore* GetScore() { return m_pParent->GetScore(); }
    inline lmStaff* GetStaff() { return m_pParent->GetStaff(); }

    // units conversion
    lmLUnits TenthsToLogical(lmTenths nTenths);
    lmTenths LogicalToTenths(lmLUnits uUnits);

    // debug methods
    virtual wxString Dump();

    void OnParentComputedPositionShifted(lmLUnits uxShift, lmLUnits uyShift);
	void OnParentMoved(lmLUnits uxShift, lmLUnits uyShift);

	//---- specific methods of this class ------------------------

    // ownership
    void SetOwner(lmScoreObj* pOwner);

    // class info
    virtual lmEAuxObjType GetAuxObjType()=0;

    // source code related methods
    virtual wxString SourceLDP(int nIndent);
    virtual wxString SourceXML(int nIndent);


protected:
    lmAuxObj(bool fIsDraggable = false);
    virtual lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)=0;

};

// declare a list of AuxObjs
#include "wx/list.h"
WX_DECLARE_LIST(lmAuxObj, AuxObjsList);


#endif    // __LM_STAFFOBJ_H__
