//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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

#define lmDRAGGABLE         true
#define lmNO_DRAGGABLE      false
#define lmVISIBLE           true
#define lmNO_VISIBLE        false




//-------------------------------------------------------------------------------------------
// class lmScoreObj
//  This is the most abstract object. An object has an associated  context options obj.
//  and a parent
//-------------------------------------------------------------------------------------------

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

    //--- Any ScoreObj can own AuxObjs -----------------------
    
	//provide origin position for owned AuxObjs
	virtual lmUPoint GetReferencePos(lmPaper* pPaper)=0;

    //provide units conversion
    virtual lmLUnits TenthsToLogical(lmTenths nTenths)=0;
    virtual lmTenths LogicalToTenths(lmLUnits uUnits)=0;

    //attach and detach AuxObjs
    int AttachAuxObj(lmAuxObj* pAO);
    void DetachAuxObj(lmAuxObj* pAO);

	//position and main shape
    virtual void ShiftObject(lmLUnits uLeft);
	virtual lmLocation SetUserLocation(lmLocation tPos);
	inline lmShape* GetShap2() { return m_pShape2; }

	//contextual menu
	virtual void PopupMenu(lmController* pCanvas, lmGMObject* pGMO, const lmDPoint& vPos);
	virtual void CustomizeContextualMenu(wxMenu* pMenu, lmGMObject* pGMO);

	virtual void OnProperties(lmGMObject* pGMO);



protected:
    lmScoreObj(lmScoreObj* pParent);

    lmScoreObj*		m_pParent;          //the parent for the ObjOptions chain
    lmObjOptions*   m_pObjOptions;      //the collection of options or NULL if none
    lmAuxObjsCol*   m_pAuxObjs;         //the collection of attached AuxObjs or NULL if none

	//position
    lmLocation      m_tPos;         //desired position for this object
    lmShape*		m_pShape2;		//new shape

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

	//owning AuxObjs
	virtual lmUPoint GetReferencePos(lmPaper* pPaper)=0;

    // units conversion
    virtual lmLUnits TenthsToLogical(lmTenths nTenths)=0;
    virtual lmTenths LogicalToTenths(lmLUnits uUnits)=0;



	//---- specific methods of this class ------------------------

	// type and identificaction
    inline int GetID() const { return m_nId; }
    inline EScoreObjType GetType() const { return m_nType; }

    // graphic model
    virtual void Layout(lmBox* pBox, lmPaper* pPaper, wxColour colorC = *wxBLACK,
                        bool fHighlight = false)=0;

    // debug
    virtual wxString Dump()=0;



#if lmCOMPATIBILITY_NO_SHAPES

    // positioning
    inline lmUPoint& GetOrigin() { return m_uPaperPos; }
    inline bool IsFixed() const { return m_fFixedPos; }
    void SetFixed(bool fFixed) { m_fFixedPos = fFixed; }
    void SetPageNumber(int nNum) { m_nNumPage = nNum; }
    inline int GetPageNumber() const { return m_nNumPage; }

    // methods related to font rendered objects
    virtual void SetFont(lmPaper* pPaper) {}
    wxFont* GetFont() { return m_pFont; }

#endif  //lmCOMPATIBILITY_NO_SHAPES

protected:
    lmComponentObj(lmScoreObj* pParent, EScoreObjType nType, lmLocation* pPos = &g_tDefaultPos,
                   bool fIsDraggable = false);
    wxString SourceLDP_Location(lmUPoint uPaperPos);

    EScoreObjType   m_nType;        //type of ComponentObj
    int             m_nId;          //unique number, to identify each lmComponentObj
    bool            m_fIsDraggable;


#if lmCOMPATIBILITY_NO_SHAPES

    //positioning. Coordinates relative to origin of page (in logical units); updated each
    // time this object is drawn
    lmUPoint    m_uPaperPos;        // paper xPos, yBase position to render this object
    bool        m_fFixedPos;        // its position is fixed. Do not recalculate it
    int         m_nNumPage;         // page on which this SO is rendered (1..n). Set Up in BoxSystem::RenderMeasure().

    // variables related to font rendered objects
    wxFont*     m_pFont;            // font to use for drawing this object


#endif  //lmCOMPATIBILITY_NO_SHAPES

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

class lmStaffObj : public lmComponentObj
{
public:
    virtual ~lmStaffObj();

	//---- virtual methods of base class -------------------------

    virtual void Layout(lmBox* pBox, lmPaper* pPaper, wxColour colorC = *wxBLACK,
                        bool fHighlight = false);
    virtual void SetFont(lmPaper* pPaper);

	//owning AuxObjs
	virtual lmUPoint GetReferencePos(lmPaper* pPaper);

    // units conversion
    lmLUnits TenthsToLogical(lmTenths nTenths);
    lmTenths LogicalToTenths(lmLUnits uUnits);

    // debug
    virtual wxString Dump();

	//---- specific methods of this class ------------------------

    // characteristics
    virtual inline bool IsSizeable() { return false; }
    inline bool IsVisible() { return m_fVisible; }
    EStaffObjType GetClass() { return m_nClass; }

    // source code related methods
    virtual wxString SourceLDP(int nIndent);
    virtual wxString SourceXML(int nIndent);

    // methods related to time and duration
    float GetTimePos() { return m_rTimePos; }
    void SetTimePos(float rTimePos) { m_rTimePos = rTimePos; }
    virtual float GetTimePosIncrement() { return 0; }

    // methods related to positioning
    virtual lmLUnits GetAnchorPos() {return 0; }
    virtual void ShiftObject(lmLUnits uLeft);

	//highligh
	virtual void Highlight(lmPaper* pPaper, wxColour colorC) {}

    // methods related to staff ownership
    void SetNumMeasure(int nNum) { m_numMeasure = nNum; }
    int GetStaffNum() { return m_nStaffNum; }
    lmVStaff* GetVStaff() { return m_pVStaff; }

    // methods related to AuxObj/GraphObj ownership
    virtual bool IsComposite() { return false; }



protected:
    lmStaffObj(lmScoreObj* pParent, EStaffObjType nType,
             lmVStaff* pStaff = (lmVStaff*)NULL, int nStaff=1,    // only for staff owned objects
             bool fVisible = true,
             bool fIsDraggable = false);

	//rendering
    virtual lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, wxColour colorC)=0;


    //properties
    bool            m_fVisible;     // this lmComponentObj is visible on the score
    EStaffObjType   m_nClass;       // type of StaffObj

    // time related variables
    float       m_rTimePos;         // time from start of measure

    // Info about staff ownership
    lmVStaff*   m_pVStaff;          // lmVStaff owning this lmStaffObj
    int         m_nStaffNum;        // lmStaff (1..n) on which this object is located
    int         m_numMeasure;       // measure number in which this lmStaffObj is included


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

    virtual void Layout(lmBox* pBox, lmPaper* pPaper, wxColour colorC = *wxBLACK,
                bool fHighlight = false);
    virtual void SetFont(lmPaper* pPaper) {}

	//owning AuxObjs
	lmUPoint GetReferencePos(lmPaper* pPaper);

    // units conversion
    lmLUnits TenthsToLogical(lmTenths nTenths);
    lmTenths LogicalToTenths(lmLUnits uUnits);

    // debug methods
    virtual wxString Dump()=0;


	//---- specific methods of this class ------------------------

    // ownership
    void SetOwner(lmScoreObj* pOwner);

    // class info
    virtual lmEAuxObjType GetAuxObjType()=0;

    // source code related methods
    virtual wxString SourceLDP(int nIndent)=0;
    virtual wxString SourceXML(int nIndent)=0;


protected:
    lmAuxObj(bool fIsDraggable = false);
    virtual lmLUnits LayoutObject(lmBox* pBox, lmPaper* pPaper, wxColour colorC)=0;

};

// declare a list of AuxObjs
#include "wx/list.h"
WX_DECLARE_LIST(lmAuxObj, AuxObjsList);

#endif    // __LM_STAFFOBJ_H__
