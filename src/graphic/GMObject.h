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

#ifndef __LM_GMOBJECT_H__        //to avoid nested includes
#define __LM_GMOBJECT_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "GMObject.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if wxUSE_GENERIC_DRAGIMAGE
#include "wx/generic/dragimgg.h"
#define wxDragImage wxGenericDragImage
#else
#include "wx/dragimag.h"
#endif

#include <vector>
#include <list>

#include "../score/defs.h"

extern bool g_fFreeMove;		// the shapes can be dragged without restrictions

class lmPaper;
class lmScoreObj;
class lmController;


//------------------------------------------------------------------------------
//the root object for the graphical model

enum lmEGMOType
{
    //boxes
    eGMO_Box = 0,
    eGMO_BoxScore = eGMO_Box,
    eGMO_BoxPage,
    eGMO_BoxSystem,
    eGMO_BoxSlice,
    eGMO_BoxSliceInstr,
    eGMO_BoxSliceVStaff,

	eGMO_LastBox,   //end of box objects

    // shapes
    eGMO_Shape = eGMO_LastBox,
	eGMO_ShapeStaff = eGMO_Shape,
    eGMO_ShapeArch,
    eGMO_ShapeBarline,
	eGMO_ShapeBeam,
    eGMO_ShapeBracket,
	eGMO_ShapeComposite,
	eGMO_ShapeGlyph,
	eGMO_ShapeLine,
	eGMO_ShapeMultiAttached,
	eGMO_ShapeNote,
	eGMO_ShapeRest,
	eGMO_ShapeText,
	eGMO_ShapeTuplet,
	eGMO_ShapeInvisible,
};


class lmGMObject : public wxObject
{
public:
    virtual ~lmGMObject();

    // methods related to type and identificaction
    inline int GetID() const { return m_nId; }
    inline lmEGMOType GetType() const { return m_nType; }
	inline wxString GetName() const { return m_sGMOName; }
    inline bool IsBox() const { return m_nType < eGMO_LastBox; }
    inline bool IsShape() const { return m_nType >= eGMO_Shape; }
		//boxes
	inline bool IsBoxScore() const { return m_nType == eGMO_BoxScore; }
	inline bool IsBoxPage() const { return m_nType == eGMO_BoxPage; }
    inline bool IsBoxSystem() const { return m_nType == eGMO_BoxSystem; }
    inline bool IsBoxSlice() const { return m_nType == eGMO_BoxSlice; }
    inline bool IsBoxSliceInstr() const { return m_nType == eGMO_BoxSliceInstr; }
    inline bool IsBoxSliceVStaff() const { return m_nType == eGMO_BoxSliceVStaff; }
		//shapes
	inline bool IsShapeStaff() const { return m_nType == eGMO_ShapeStaff; }
    inline bool IsShapeArch() const { return m_nType == eGMO_ShapeArch; }
    inline bool IsShapeBarline() const { return m_nType == eGMO_ShapeBarline; }
	inline bool IsShapeBeam() const { return m_nType == eGMO_ShapeBeam; }
	inline bool IsShapeComposite() const { return m_nType == eGMO_ShapeComposite; }
	inline bool IsShapeGlyph() const { return m_nType == eGMO_ShapeGlyph; }
	inline bool IsShapeLine() const { return m_nType == eGMO_ShapeLine; }
	inline bool IsShapeMultiAttached() const { return m_nType == eGMO_ShapeMultiAttached; }
	inline bool IsShapeNote() const { return m_nType == eGMO_ShapeNote; }
	inline bool IsShapeText() const { return m_nType == eGMO_ShapeText; }
	inline bool IsShapeTuplet() const { return m_nType == eGMO_ShapeTuplet; }
	inline bool IsShapeInvisible() const { return m_nType == eGMO_ShapeInvisible; }

    //bounding box
    inline void SetXLeft(lmLUnits xLeft) { m_uBoundsTop.x = xLeft; }
    inline void SetYTop(lmLUnits yTop) { m_uBoundsTop.y = yTop; }
    inline void SetXRight(lmLUnits xRight) { m_uBoundsBottom.x = xRight; }
    inline void SetYBottom(lmLUnits yBottom) { m_uBoundsBottom.y = yBottom; }
    inline lmLUnits GetXLeft() const { return m_uBoundsTop.x; }
    inline lmLUnits GetYTop() const { return m_uBoundsTop.y; }
    inline lmLUnits GetXRight() const { return m_uBoundsBottom.x; }
    inline lmLUnits GetYBottom() const { return m_uBoundsBottom.y; }
    inline lmURect GetBounds() const {
            return lmURect(m_uBoundsTop.x, m_uBoundsTop.y,
                           m_uBoundsBottom.x - m_uBoundsTop.x, m_uBoundsBottom.y - m_uBoundsTop.y);
        }
    virtual lmLUnits GetWidth() { return m_uBoundsBottom.x - m_uBoundsTop.x; }
    virtual lmLUnits GetHeight() { return m_uBoundsBottom.y - m_uBoundsTop.y; }

    virtual bool ContainsPoint(lmUPoint& pointL);

    // methods related to selection rectangle
    void SetSelRectangle(lmLUnits x, lmLUnits y, lmLUnits uWidth, lmLUnits uHeight);
    void DrawSelRectangle(lmPaper* pPaper, wxColour colorC = *wxBLUE);
    lmURect GetSelRectangle() const { return m_uSelRect; }


    //rendering
    virtual void DrawBounds(lmPaper* pPaper, wxColour color);

	//debugging
    virtual wxString Dump(int nIndent)=0;

    //selection
    inline bool IsSelected() const { return m_fSelected; }
    inline void SetSelected(bool fValue) { m_fSelected = fValue; }

	//dragging and moving
    inline bool IsDraggable() const { return m_fDraggable; }
	virtual wxBitmap* OnBeginDrag(double rScale, wxDC* pDC) { return (wxBitmap*)NULL; }
    virtual lmUPoint OnDrag(lmPaper* pPaper, const lmUPoint& uPos) { return uPos; };
	virtual lmUPoint GetObjectOrigin();
    virtual void OnEndDrag(lmController* pCanvas, const lmUPoint& uPos);
    virtual void Shift(lmLUnits xIncr, lmLUnits yIncr);
	void Shift(lmUPoint uPos) { Shift(uPos.x, uPos.y); }
	void ShiftOrigin(lmUPoint uNewOrg);
    void ApplyUserShift(lmUPoint uUserShift);

    //info
    inline lmScoreObj* GetScoreOwner() { return m_pOwner; }
    inline int GetOwnerIDX() { return m_nOwnerIdx; }
	virtual int GetPageNumber() const { return 0; }

	//contextual menu
	virtual void OnRightClick(lmController* pCanvas, const lmDPoint& vPos, int nKeys);


protected:
    lmGMObject(lmScoreObj* pOwner, lmEGMOType m_nType, bool fDraggable = false,
		       wxString sName = _("Object"), int nOwnerIdx = -1);
    wxString DumpBounds();
	void ShiftBoundsAndSelRec(lmLUnits xIncr, lmLUnits yIncr);
	void NormaliceBoundsRectangle();

	enum {
		lmINDENT_STEP = 3,		//for Dump() method
	};


	lmScoreObj*	    m_pOwner;		//associated owner object (in lmScore representation)
    int             m_nOwnerIdx;    //index assigned by the owner to this GMObject (-1 if none)
    lmEGMOType      m_nType;        //type of GMO
    int             m_nId;          //unique identification number
	wxString		m_sGMOName;

    //bounding box: rectangle delimiting the visual representation
	//the rectangle is referred to page origin
    lmUPoint        m_uBoundsBottom;	//bottom right corner point
    lmUPoint        m_uBoundsTop;		//top left corner point

    lmUPoint        m_uUserShift;

	//selection rectangle
	lmURect		m_uSelRect;   

    //selection
    bool            m_fSelected;        //this object is selected

    //dragging
    bool			m_fDraggable;		//this object is draggable

};

//------------------------------------------------------------------------------
//An abstract container for the score graphical model
//abstract class to derive all lmBoxXXXXX objects

class lmShape;
class lmBoxSystem;
class lmGMSelection;

class lmBox : public lmGMObject
{
public:
    virtual ~lmBox();

    virtual void AddShape(lmShape* pShape);

    //implementation of virtual methods from base class
    virtual wxString Dump(int nIndent)=0;
	virtual int GetPageNumber() const { return 0; }

	//owners and related
	virtual lmBoxSystem* GetOwnerSystem()=0;

    //selection
    void AddShapesToSelection(lmGMSelection* pSelection, lmLUnits uXMin, lmLUnits uXMax,
                              lmLUnits uYMin, lmLUnits uYMax);

protected:
    lmBox(lmScoreObj* pOwner, lmEGMOType m_nType, wxString sName = _("Box"));
    lmShape* FindShapeAtPosition(lmUPoint& pointL);


	std::vector<lmShape*>	m_Shapes;		//list of contained shapes




};


//------------------------------------------------------------------------------
// An abstract class representing any renderizable object, such as a line,
// a glyph, a note head, an arch, etc.
// From this class to derive all lmShapeXXXXX objects

enum lmEAttachType
{
	eGMA_Unknown,
    eGMA_StartNote,
	eGMA_MiddleNote,
	eGMA_EndNote,
};

enum lmEParentEvent {
	lmSHIFT_EVENT = 0,
	lmMOVE_EVENT,
};

typedef struct lmVertexStruct {
    lmLUnits    ux_coord;
    lmLUnits    uy_coord;
    unsigned    cmd;
} lmVertex;
    


class lmShape : public lmGMObject
{
public:
    virtual ~lmShape();

	virtual void Render(lmPaper* pPaper, wxColour color)=0;
	virtual void Render(lmPaper* pPaper);

    virtual bool Collision(lmShape* pShape);

    //methods related to position
	virtual void OnAttachmentPointMoved(lmShape* pShape, lmEAttachType nTag,
										lmLUnits ux, lmLUnits uy, lmEParentEvent nEvent) {}

	//shapes can be attached to other shapes
	int Attach(lmShape* pShape, lmEAttachType nType = eGMA_Unknown);
	void Detach(lmShape* pShape);

    //Debug related methods
    virtual wxString Dump(int nIndent) = 0;
    wxString DumpSelRect();

	//visibility
	inline bool IsVisible() const { return m_fVisible; }
	void SetVisible(bool fVisible) { m_fVisible = fVisible; }

    //info
	virtual int GetPageNumber() const;

	//owners and related
	inline lmBox* GetOwnerBox() { return m_pOwnerBox; }
	inline void SetOwnerBox(lmBox* pOwnerBox) { m_pOwnerBox = pOwnerBox; }
	lmBoxSystem* GetOwnerSystem() { return m_pOwnerBox->GetOwnerSystem(); }

	//for composite shapes
	inline bool IsChildShape() const { return (bool)(m_pParentShape != (lmShape*)NULL ); }
	inline lmShape* GetParentShape() { return m_pParentShape; }
	inline void SetParentShape(lmShape* pShape) { m_pParentShape = pShape; }

    //selection
    bool IsInRectangle(lmURect& rect);

    //vertex source
    virtual void RewindVertices(int nPathId = 0) {}
    virtual unsigned GetVertex(lmLUnits* pux, lmLUnits* puy);

protected:
    lmShape(lmEGMOType m_nType, lmScoreObj* pOwner, int nOwnerIdx, wxString sName=_T("Shape"),
			bool fDraggable = false, wxColour color = *wxBLACK,
			bool fVisible = true);
    void RenderCommon(lmPaper* pPaper, wxColour colorC);
    void RenderCommon(lmPaper* pPaper);
	void InformAttachedShapes(lmLUnits ux, lmLUnits uy, lmEParentEvent nEvent);


	lmBox*		m_pOwnerBox;	//box in which this shape is included
	bool		m_fVisible;

	typedef struct lmAtachPoint_Struct {
		lmShape*		pShape;
		lmEAttachType	nType;
	} lmAtachPoint;

	//list of shapes attached to this one
	std::list<lmAtachPoint*>	m_cAttachments;
	
	wxColour	m_color;

	//for composite shapes
	lmShape*	m_pParentShape;


};


class lmSimpleShape : public lmShape
{
public:
    virtual ~lmSimpleShape();

    //implementation of virtual methods from base class
    virtual wxString Dump(int nIndent) = 0;
    virtual void Shift(lmLUnits xIncr, lmLUnits yIncr);
    virtual void Render(lmPaper* pPaper, wxColour color)=0;

    //dragging
	virtual wxBitmap* OnBeginDrag(double rScale, wxDC* pDC) { return (wxBitmap*)NULL; }
    virtual lmUPoint OnDrag(lmPaper* pPaper, const lmUPoint& uPos) { return uPos; };

protected:
    lmSimpleShape(lmEGMOType m_nType, lmScoreObj* pOwner, int nOwnerIdx,
                  wxString sName=_T("SimpleShape"),
				  bool fDraggable = false, wxColour color = *wxBLACK,
				  bool fVisible = true);

};


class lmCompositeShape : public lmShape
{
public:
    lmCompositeShape(lmScoreObj* pOwner, int nOwnerIdx, wxString sName = _T("CompositeShape"),
                     bool fDraggable = false, lmEGMOType nType = eGMO_ShapeComposite,
					 bool fVisible = true);
    virtual ~lmCompositeShape();

    //dealing with components
    virtual int Add(lmShape* pShape);
	inline int GetNumComponents() const { return (int)m_Components.size(); }
	virtual void RecomputeBounds();

    //virtual methods from base class
    virtual wxString Dump(int nIndent);
    virtual void Shift(lmLUnits xIncr, lmLUnits yIncr);
	virtual void Render(lmPaper* pPaper, wxColour color);

	//overrides
    bool ContainsPoint(lmUPoint& pointL);
    bool Collision(lmShape* pShape);

    //dragging
    virtual wxBitmap* OnBeginDrag(double rScale, wxDC* pDC);
    virtual lmUPoint OnDrag(lmPaper* pPaper, const lmUPoint& uPos);


protected:
	lmShape* GetShape(int nShape);

	bool					m_fDoingShift;	//semaphore to avoid recomputing constantly the bounds
    bool					m_fGrouped;		//its component shapes must be rendered as a single object
	std::vector<lmShape*>	m_Components;	//list of its constituent shapes

};


// lmGMSelection is data holder with information about a selection.

class lmGMSelection
{
public:
    lmGMSelection();
    ~lmGMSelection();
    
    void AddToSelection(lmGMObject* pGMO);
    void RemoveFromSelection(lmGMObject* pGMO);
    inline void Clear() { m_Selection.clear(); }

    //iteration
    lmGMObject* GetFirst();
    lmGMObject* GetNext();

    //info
    inline int NumObjects() { return (int)m_Selection.size(); }
    inline bool IsEmpty() const { return m_Selection.size()==0; }

    //debug
    wxString Dump();

private:
    std::list<lmGMObject*>              m_Selection;
    std::list<lmGMObject*>::iterator    m_it;           //for GetFirst(), GetNext() methods

};




#endif  // __LM_GMOBJECT_H__

