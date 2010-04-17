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

#include "../graphic/Shapes.h"

#if wxUSE_GENERIC_DRAGIMAGE
#include "wx/generic/dragimgg.h"
#define wxDragImage wxGenericDragImage
#else
#include "wx/dragimag.h"
#endif

class lmSegment;
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
enum lmEScoreObjType 
{
    //simple objects
	lm_eSO_Score = 0,
	lm_eSO_Instrument,
	lm_eSO_Staff,
	lm_eSO_VStaff,

    //Component objs (StaffObjs & AuxObjs)
	lm_eSO_ComponentObj,

    //staff objects (lmStaffObj). Objects on the staff. Consume time
    lm_eSO_StaffObj = lm_eSO_ComponentObj,
    lm_eSO_Barline = lm_eSO_StaffObj,  // barline (lmBarline)
    lm_eSO_Clef,                    // clef (lmClef)
    lm_eSO_KeySignature,            // key signature (lmKeySignature)
    lm_eSO_TimeSignature,           // time signature (lmTimeSignature)
    lm_eSO_Note,                    // notes (lmNote -> lmNoteRest)
    lm_eSO_Rest,                    // rests (lmRest -> lmNoteRest)
    lm_eSO_Control,                 // control element: new system (lmSOControl)
    lm_eSO_MetronomeMark,           // metronome mark (lmMetronomeMark)
    lm_eSO_Spacer,                  // spacer (lmSpacer)
    lm_eSO_ScoreAnchor,             // anchor to attach AuxObjs to the score
    lm_eSO_FiguredBass,             // figured bass symbol (lmFiguredBass)
    lm_eSO_LastStaffObj,

    // aux objects (lmAuxObj). Auxiliary. Owned by StaffObjs
    lm_eSO_AuxObj = lm_eSO_LastStaffObj, 
    lm_eSO_Fermata = lm_eSO_AuxObj,
    lm_eSO_Line,                    // graphic object: line (lmScoreLine)
    lm_eSO_Lyric,
    lm_eSO_TextItem,                // score text (lmTextItem)
	lm_eSO_ScoreTitle,              // score text (lmScoreTitle)
    lm_eSO_TextBox,                 // score block (lmScoreTextBox)

    //lmBinaryRelObj
    lm_eSO_BinaryRelObj,
    lm_eSO_Tie,

    //lmMultiRelObj
    lm_eSO_MultiRelObj,
    lm_eSO_Beam,
    //lm_eSO_TupletBracket,           // tuplet bracket (lmTupletBracket)

    //lmBinaryRelObX
    lm_eSO_BinaryRelObX,
    lm_eSO_FBLine,                  //'hold chord' figured bass line

    //lmMultiRelObX
    lm_eSO_MultiRelObX,

    //end of table
    lm_eSO_LastAuxObj,
};


class lmAuxObj;
typedef std::vector<lmAuxObj*> lmAuxObjsCol;
class lmObjOptions;
class lmBox;
class lmController;
class lmScore;
class lmDlgProperties;
#if lmUSE_LIBRARY
    #include "../ldp_parser/LDPNode.h"
#endif

class lmScoreObj
{
public:
    virtual ~lmScoreObj();

    //building
    void DefineAsMultiShaped();

    //--- Options --------------------------------------------

    // get and set the value of an option
    lmObjOptions* GetCurrentObjOptions();
    inline lmObjOptions* GetObjOptions() { return m_pObjOptions; }
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
    inline long GetID() { return m_nId; }

    //--- ScoreObj type ------------------------------------
    inline lmEScoreObjType GetScoreObjType() { return m_nObjType; }
	inline bool IsInstrument() { return m_nObjType == lm_eSO_Instrument; }
	inline bool IsScore() { return m_nObjType == lm_eSO_Score; }
	inline bool IsStaff() { return m_nObjType == lm_eSO_Staff; }
	inline bool IsVStaff() { return m_nObjType == lm_eSO_VStaff; }
	inline bool IsComponentObj() { return m_nObjType >= lm_eSO_ComponentObj; }
	inline bool IsStaffObj() { return m_nObjType >= lm_eSO_StaffObj
                                      && m_nObjType < lm_eSO_LastStaffObj; }
	inline bool IsAuxObj() { return m_nObjType >= lm_eSO_AuxObj
                                      && m_nObjType < lm_eSO_LastAuxObj; }
    //StaffObjs
    inline bool IsClef() { return m_nObjType == lm_eSO_Clef; }
    inline bool IsKeySignature() { return m_nObjType == lm_eSO_KeySignature; }
    inline bool IsTimeSignature() { return m_nObjType == lm_eSO_TimeSignature; }
    inline bool IsBarline() { return m_nObjType == lm_eSO_Barline; }
    inline bool IsNoteRest() { return m_nObjType == lm_eSO_Note
                                      || m_nObjType == lm_eSO_Rest; }
    inline bool IsNote() { return m_nObjType == lm_eSO_Note; }
    inline bool IsRest() { return m_nObjType == lm_eSO_Rest; }
    inline bool IsControl() { return m_nObjType == lm_eSO_Control; }
    inline bool IsMetronomeMark() { return m_nObjType == lm_eSO_MetronomeMark; }
    //inline bool IsTupletBracket() { return m_nObjType == lm_eSO_TupletBracket; }
    inline bool IsSpacer() { return m_nObjType == lm_eSO_Spacer; }
    inline bool IsScoreAnchor() { return m_nObjType == lm_eSO_ScoreAnchor; }
    inline bool IsFiguredBass() { return m_nObjType == lm_eSO_FiguredBass; }
    //AuxObjs
    inline bool IsFermata() { return m_nObjType == lm_eSO_Fermata; }
    inline bool IsFiguredBassLine() { return m_nObjType == lm_eSO_FBLine; }
    inline bool IsLine() { return m_nObjType == lm_eSO_Line; }
    inline bool IsLyric() { return m_nObjType == lm_eSO_Lyric; }
    inline bool IsTextItem() { return m_nObjType == lm_eSO_TextItem; }
    inline bool IsTextBlock() { return m_nObjType == lm_eSO_ScoreTitle; }
    inline bool IsTie() { return m_nObjType == lm_eSO_Tie; }
    inline bool IsBeam() { return m_nObjType == lm_eSO_Beam; }

    inline bool IsRelObj() { return m_nObjType >= lm_eSO_BinaryRelObj
                                    && m_nObjType < lm_eSO_BinaryRelObX; }
    inline bool IsRelObX() { return m_nObjType >= lm_eSO_BinaryRelObX; }
    inline bool IsBinaryRelObj() { return m_nObjType >= lm_eSO_BinaryRelObj
                                          && m_nObjType < lm_eSO_MultiRelObj; }
    inline bool IsBinaryRelObX() { return m_nObjType >= lm_eSO_BinaryRelObX
                                          && m_nObjType < lm_eSO_MultiRelObX; }
    inline bool IsMultiRelObj() { return m_nObjType >= lm_eSO_MultiRelObj; }

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

    //wrapper methods to encapsulate and simplify operations related to score creation by program
    lmAuxObj* AttachTextBox(lmTPoint& ntBoxPos, lmTPoint& ntLinePos, wxString& sText,
                            lmTextStyle* pTextStyle, wxSize size = wxSize(160, 80),
                            wxColour nBgColor = wxColour(_T("#fffeb0")),
                            long nID = lmNEW_ID);
    lmAuxObj* AttachLine(lmTenths xtStart, lmTenths ytStart, lmTenths xtEnd, lmTenths ytEnd,
                         lmTenths ntWidth, lmELineCap nStartCap, lmELineCap nEndCap,
                         lmELineStyle nStyle, wxColour nColor, long nID = lmNEW_ID);


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
    inline void SetLayer(long nLayer) { m_nLayer = nLayer; }
    inline long GetLayer() { return m_nLayer; }

	//positioning
    virtual void StoreOriginAndShiftShapes(lmLUnits uLeft, int nShapeIdx = 0);
	virtual lmLocation SetUserLocation(lmLocation tPos, int nShapeIdx = 0);
    virtual lmLocation SetUserLocation(lmTenths xPos, lmTenths yPos, int nShapeIdx = 0);
	virtual lmLUnits SetUserXLocation(lmLUnits uxPos, int nShapeIdx = 0);
    virtual lmUPoint SetUserLocation(lmUPoint uNewPos, int nShapeIdx = 0);
    virtual void MoveObjectPoints(int nNumPoints, int nShapeIdx, lmUPoint* pShifts,
                                  bool fAddShifts);
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
    virtual wxString SourceLDP(int nIndent, bool fUndoData);
    virtual wxString SourceXML(int nIndent);

#if lmUSE_LIBRARY
    inline void SetLdpElement(lmLDPNode* pNode) { m_pParentLdpElement = pNode; }
    inline lmLDPNode* GetLdpElement() { return m_pParentLdpElement; }
#endif


protected:
    lmScoreObj(lmScoreObj* pParent, long nID, lmEScoreObjType nType);
    void PrepareToCreateShapes();

    friend class lmScore;
    inline void SetID(long nValue) { m_nId = nValue; }   //To be used only by lmScore


    lmScoreObj*		m_pParent;          //the parent for the ObjOptions chain
    long            m_nId;              //the ID for this object in the score
    lmEScoreObjType m_nObjType;         //type of this score obj
    long            m_nLayer;           //layer in which it will be rendered
    lmObjOptions*   m_pObjOptions;      //the collection of options or NULL if none
    lmAuxObjsCol*   m_pAuxObjs;         //the collection of attached AuxObjs or NULL if none
    bool            m_fDirty;           //the object has been modified and needs layout recomputation
#if lmUSE_LIBRARY
    lmLDPNode*      m_pParentLdpElement;
#endif

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

class lmComponentObj : public lmScoreObj
{
public:
    virtual ~lmComponentObj();

	//---- specific methods of this class ------------------------

    // graphical model
    virtual void Layout(lmBox* pBox, lmPaper* pPaper, bool fHighlight = false)=0;
    virtual lmUPoint ComputeBestLocation(lmUPoint& uOrg, lmPaper* pPaper);

    //properties
    inline void SetColour(wxColour color) { m_color = color; }

protected:
    lmComponentObj(lmScoreObj* pParent, long nID, lmEScoreObjType nType,
                   bool fIsDraggable = false);

    bool                    m_fIsDraggable;
	wxColour		        m_color;        //object color
};


//-------------------------------------------------------------------------------------------
//    lmStaffObj
//-------------------------------------------------------------------------------------------

class lmVStaff;
class lmAuxObj;
class lmContext;
class lmClef;
class lmTimeSignature;
class lmKeySignature;
class lmRelObX;


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
	virtual wxString GetName() const=0;

    //layout properties
    virtual bool IsAligned() { return false; }


    // source code related methods
    virtual wxString SourceLDP(int nIndent, bool fUndoData);
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
    int GetNumSegment();
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
    virtual lmEClefType GetCtxApplicableClefType();
    lmClef* GetApplicableClef();
    lmTimeSignature* GetApplicableTimeSignature();
    lmKeySignature* GetApplicableKeySignature();

	//navigation in the VStaff collection and collection management
	inline lmStaffObj* GetPrevSO() const { return m_pPrevSO; }
	inline lmStaffObj* GetNextSO() const { return m_pNextSO; }
	inline void SetPrevSO(lmStaffObj* pPrevSO) { m_pPrevSO = pPrevSO; }
	inline void SetNextSO(lmStaffObj* pNextSO) { m_pNextSO = pNextSO; }

	//relationships
    virtual void OnAddedToRelation(lmRelObX* pRel) {}
    virtual void OnRemovedFromRelation(lmRelObX* pRel) {}



protected:
    lmStaffObj(lmScoreObj* pParent, long nID, lmEScoreObjType nType,
             lmVStaff* pStaff = (lmVStaff*)NULL, int nStaff=1,    // only for staff owned objects
             bool fVisible = true,
             bool fIsDraggable = false);

	//rendering
    virtual lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)=0;



    //properties
    bool            m_fVisible;     // this lmComponentObj is visible on the score

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

class lmNoteRest;


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

    virtual void OnParentComputedPositionShifted(lmLUnits uxShift, lmLUnits uyShift);
	virtual void OnParentMoved(lmLUnits uxShift, lmLUnits uyShift);

	//---- specific methods of this class ------------------------

    // ownership
    void SetOwner(lmScoreObj* pOwner);

    // source code related methods
    virtual wxString SourceLDP(int nIndent, bool fUndoData);
    virtual wxString SourceXML(int nIndent);


protected:
    lmAuxObj(lmScoreObj* pOwner, long nID, lmEScoreObjType nType, bool fIsDraggable = false);
    virtual lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, lmUPoint uPos, wxColour colorC)=0;

};

// declare a list of AuxObjs
#include "wx/list.h"
WX_DECLARE_LIST(lmAuxObj, AuxObjsList);


//An AuxObj relating at least two Notes/Rests
class lmRelObj : public lmAuxObj
{
public:
	virtual ~lmRelObj() {}

    //building/destroying the relationship
    virtual void Include(lmNoteRest* pNR, int nIndex = -1)=0;
    virtual void Remove(lmNoteRest* pNR)=0;
	virtual void OnRelationshipModified()=0;

    //information
    virtual lmNoteRest* GetStartNoteRest()=0;
    virtual lmNoteRest* GetEndNoteRest()=0;

    //source code generation
    virtual wxString SourceLDP_First(int nIndent, bool fUndoData, lmNoteRest* pNR)
                        { return wxEmptyString; }
    virtual wxString SourceLDP_Middle(int nIndent, bool fUndoData, lmNoteRest* pNR)
                        { return wxEmptyString; }
    virtual wxString SourceLDP_Last(int nIndent, bool fUndoData, lmNoteRest* pNR)
                        { return wxEmptyString; }
    virtual wxString SourceXML_First(int nIndent, lmNoteRest* pNR)
                        { return wxEmptyString; }
    virtual wxString SourceXML_Middle(int nIndent, lmNoteRest* pNR)
                        { return wxEmptyString; }
    virtual wxString SourceXML_Last(int nIndent, lmNoteRest* pNR)
                        { return wxEmptyString; }

    //overrides
    wxString SourceLDP(int nIndent, bool fUndoData);

protected:
	lmRelObj(lmScoreObj* pOwner, long nID, lmEScoreObjType nType, bool fIsDraggable = true)
        : lmAuxObj(pOwner, nID, nType, fIsDraggable) {}
};


//An AuxObj relating two and only two Notes/Rests
class lmBinaryRelObj : public lmRelObj
{
public:
    virtual ~lmBinaryRelObj();

    //implementation of lmRelObj pure virtual methods
    virtual void Include(lmNoteRest* pNR, int nIndex = -1) {};
    virtual void Remove(lmNoteRest* pNR);
    virtual inline lmNoteRest* GetStartNoteRest() { return m_pStartNR; }
    virtual inline lmNoteRest* GetEndNoteRest() { return m_pEndNR; }
    virtual void OnRelationshipModified() {};


protected:
    lmBinaryRelObj(lmScoreObj* pOwner, long nID, lmEScoreObjType nType,
                   lmNoteRest* pStartNR, lmNoteRest* pEndNR,
                   bool fIsDraggable = true);

    lmNoteRest*     m_pStartNR;     //notes/rests related by this lmRelObj
    lmNoteRest*		m_pEndNR;
};


//An AuxObj relating two or more Notes/Rests
class lmMultiRelObj : public lmRelObj
{
public:
    virtual ~lmMultiRelObj();

    //implementation of lmRelObj pure virtual methods
    virtual void Include(lmNoteRest* pNR, int nIndex = -1);
    virtual void Remove(lmNoteRest* pNR);
    lmNoteRest* GetStartNoteRest() { return m_NoteRests.front(); }
    lmNoteRest* GetEndNoteRest() { return m_NoteRests.back(); }
    virtual void OnRelationshipModified() {};

        //specific methods
    int NumNotes() { return (int)m_NoteRests.size(); }
    int GetNoteIndex(lmNoteRest* pNR);

    //two dirty methods to simplify traversing the collection
    lmNoteRest* GetFirstNoteRest();
    lmNoteRest* GetNextNoteRest();

	wxString Dump();

protected:
    lmMultiRelObj(lmScoreObj* pOwner, long nID, lmEScoreObjType nType, bool fIsDraggable = true);

    std::list<lmNoteRest*>   m_NoteRests;   //list of note/rests that form the relation
    std::list<lmNoteRest*>::iterator m_it;  //for methods GetFirstNoteRest() and GetNextNoteRest()
};

//NEW ====================================================================
//NEW ====================================================================
//NEW ====================================================================
//NEW ====================================================================

//An abstract AuxObj relating at least two StaffObjs
class lmRelObX : public lmAuxObj
{
public:
	virtual ~lmRelObX() {}

    //building/destroying the relationship
    virtual void Include(lmStaffObj* pSO, int nIndex = -1)=0;
    virtual void Remove(lmStaffObj* pSO)=0;
	virtual void OnRelationshipModified()=0;

    //information
    virtual lmStaffObj* GetStartSO()=0;
    virtual lmStaffObj* GetEndSO()=0;

    //source code generation
    virtual wxString SourceLDP_First(int nIndent, bool fUndoData, lmStaffObj* pSO)
                        { return wxEmptyString; }
    virtual wxString SourceLDP_Middle(int nIndent, bool fUndoData, lmStaffObj* pSO)
                        { return wxEmptyString; }
    virtual wxString SourceLDP_Last(int nIndent, bool fUndoData, lmStaffObj* pSO)
                        { return wxEmptyString; }
    virtual wxString SourceXML_First(int nIndent, lmStaffObj* pSO)
                        { return wxEmptyString; }
    virtual wxString SourceXML_Middle(int nIndent, lmStaffObj* pSO)
                        { return wxEmptyString; }
    virtual wxString SourceXML_Last(int nIndent, lmStaffObj* pSO)
                        { return wxEmptyString; }

    //overrides
    wxString SourceLDP(int nIndent, bool fUndoData);

protected:
	lmRelObX(lmScoreObj* pOwner, long nID, lmEScoreObjType nType, bool fIsDraggable = true)
        : lmAuxObj(pOwner, nID, nType, fIsDraggable) {}
};


//An abstract AuxObj relating two and only two StaffObjs
class lmBinaryRelObX : public lmRelObX
{
public:
    virtual ~lmBinaryRelObX();

    //implementation of lmRelObX pure virtual methods
    virtual void Include(lmStaffObj* pSO, int nIndex = -1) {};
    virtual void Remove(lmStaffObj* pSO);
    virtual inline lmStaffObj* GetStartSO() { return m_pStartSO; }
    virtual inline lmStaffObj* GetEndSO() { return m_pEndSO; }
    virtual void OnRelationshipModified() {};


protected:
    lmBinaryRelObX(lmScoreObj* pOwner, long nID, lmEScoreObjType nType,
                   lmStaffObj* pStartSO, lmStaffObj* pEndSO,
                   bool fIsDraggable = true);

    lmStaffObj*     m_pStartSO;     //StaffObjs related by this lmRelObX
    lmStaffObj*		m_pEndSO;
};


//An abstract AuxObj relating two or more StaffObjs
class lmMultiRelObX : public lmRelObX
{
public:
    virtual ~lmMultiRelObX();

    //implementation of lmRelObX pure virtual methods
    virtual void Include(lmStaffObj* pSO, int nIndex = -1);
    virtual void Remove(lmStaffObj* pSO);
    lmStaffObj* GetStartSO() { return m_relatedSO.front(); }
    lmStaffObj* GetEndSO() { return m_relatedSO.back(); }
    virtual void OnRelationshipModified() {};

        //specific methods
    int NumObjects() { return (int)m_relatedSO.size(); }
    int GetSOIndex(lmStaffObj* pSO);

    //two dirty methods to simplify traversing the collection
    lmStaffObj* GetFirstSO();
    lmStaffObj* GetNextSO();

	wxString Dump();

protected:
    lmMultiRelObX(lmScoreObj* pOwner, long nID, lmEScoreObjType nType,
                  bool fIsDraggable = true);

    std::list<lmStaffObj*>   m_relatedSO;   //list of StaffObjs that form the relation
    std::list<lmStaffObj*>::iterator m_it;  //for methods GetFirstSO() and GetNextSO()
};


#endif    // __LM_STAFFOBJ_H__
