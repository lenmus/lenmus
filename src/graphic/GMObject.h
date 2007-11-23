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

#include "wx/cmdproc.h"		//wxCommandProcessor
#include "vector"

#include "../score/defs.h"

extern bool g_fFreeMove;		// the shapes can be dragged without restrictions

class lmPaper;
class lmObject;


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

    eGMO_BoxInstrSlice,
    eGMO_BoxVStaffSlice,

    eGMO_LastBox,   //end of box objects

    // shapes
    eGMO_Shape = eGMO_LastBox,
	eGMO_ShapeStaff = eGMO_Shape,
    eGMO_ShapeArch,
    eGMO_ShapeBarline,
	eGMO_ShapeBeam,
	eGMO_ShapeComposite,
	eGMO_ShapeGlyph,
	eGMO_ShapeLine,
	eGMO_ShapeMultiAttached,
	eGMO_ShapeNote,
	eGMO_ShapeText,
	eGMO_ShapeTuplet,
};


class lmGMObject : public wxObject
{
public:
    virtual ~lmGMObject();

    // methods related to type and identificaction
    inline int GetID() const { return m_nId; }
    inline lmEGMOType GetType() const { return m_nType; }
    inline bool IsBox() const { return m_nType < eGMO_LastBox; }
    inline bool IsShape() const { return m_nType >= eGMO_Shape; }

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

    virtual bool ContainsPoint(lmUPoint& pointL);


    //rendering
    virtual void DrawBounds(lmPaper* pPaper, wxColour color);

	//debugging
    virtual wxString Dump(int nIndent)=0;

    //selection
    inline bool IsSelected() const { return m_fSelected; }
    inline void SetSelected(bool fValue) { m_fSelected = fValue; }

	//dragging
    inline bool IsDraggable() const { return m_fDraggable; }
	virtual wxBitmap* OnBeginDrag(double rScale) { return (wxBitmap*)NULL; }
    virtual lmUPoint OnDrag(lmPaper* pPaper, const lmUPoint& uPos) { return uPos; };
	virtual lmUPoint GetObjectOrigin();
    virtual void OnEndDrag(wxCommandProcessor* pCP, const lmUPoint& uPos) {};


protected:
    lmGMObject(lmEGMOType m_nType, bool fDraggable = false);
    wxString DumpBounds();
	void NormaliceBoundsRectangle();

	enum {
		lmINDENT_STEP = 3,		//for Dump() method
	};


    lmEGMOType          m_nType;        //type of GMO
    int                 m_nId;          //unique identification number

    //bounding box: rectangle delimiting the visual representation
	//the rectangle is referred to page origin
    lmUPoint        m_uBoundsBottom;	//bottom right corner point
    lmUPoint        m_uBoundsTop;		//top left corner point

    //selection
    bool            m_fSelected;        //this object is selected

    //dragging
    bool			m_fDraggable;		//this object is draggable

};

//------------------------------------------------------------------------------
//An abstract container for the score graphical model
//abstract class to derive all lmBoxXXXXX objects

class lmShape;

class lmBox : public lmGMObject
{
public:
    virtual ~lmBox();

    virtual void AddShape(lmShape* pShape);

    //implementation of virtual methods from base class
    virtual wxString Dump(int nIndent)=0;


protected:
    lmBox(lmEGMOType m_nType);
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



class lmShape : public lmGMObject
{
public:
    virtual ~lmShape();

	virtual void Render(lmPaper* pPaper, wxColour color=*wxBLACK)=0;

    // methods related to selection rectangle
    void SetSelRectangle(lmLUnits x, lmLUnits y, lmLUnits uWidth, lmLUnits uHeight);
    void DrawSelRectangle(lmPaper* pPaper, wxColour colorC = *wxBLUE);
    lmURect GetSelRectangle() const { return m_uSelRect; }

    virtual bool Collision(lmShape* pShape);
    virtual lmLUnits GetWidth() { return m_uBoundsBottom.x - m_uBoundsTop.x; }
    virtual lmLUnits GetHeight() { return m_uBoundsBottom.y - m_uBoundsTop.y; }

    //methods related to position
    virtual void Shift(lmLUnits xIncr, lmLUnits yIncr) = 0;
	virtual void OnAttachmentPointMoved(lmShape* pShape, lmEAttachType nTag,
										lmLUnits ux, lmLUnits uy, lmEParentEvent nEvent) {}

	//shapes can be attached to other shapes
	int Attach(lmShape* pShape, lmEAttachType nType = eGMA_Unknown);

    //Debug related methods
    virtual wxString Dump(int nIndent) = 0;
    wxString DumpSelRect();

    //info
    inline lmObject* Owner() { return m_pOwner; }
	inline lmBox* GetOwnerBox() { return m_pOwnerBox; }
	inline void SetOwnerBox(lmBox* pOwnerBox) { m_pOwnerBox = pOwnerBox; }

	

protected:
    lmShape(lmEGMOType m_nType, lmObject* pOwner, wxString sName=_T("Shape"),
			bool fDraggable = false);
    void RenderCommon(lmPaper* pPaper, wxColour colorC);
    void RenderCommon(lmPaper* pPaper);
	void ShiftBoundsAndSelRec(lmLUnits xIncr, lmLUnits yIncr);
	void InformAttachedShapes(lmLUnits ux, lmLUnits uy, lmEParentEvent nEvent);


	lmObject*	m_pOwner;		//associated owner object (in lmScore representation)
	lmBox*		m_pOwnerBox;	//box in which this shape is included
    wxString    m_sShapeName;

	//selection rectangle (relative to paper origin)
	lmURect		m_uSelRect;   

	//Size: defines the height and width of the space occupied by the shape.
	lmUSize		m_uSize;

	typedef struct lmAtachPoint_Struct {
		lmShape*		pShape;
		lmEAttachType	nType;
	} lmAtachPoint;

	//list of shapes attached to this one
	std::vector<lmAtachPoint*>	m_cAttachments;


};


class lmSimpleShape : public lmShape
{
public:
    virtual ~lmSimpleShape();

    //implementation of virtual methods from base class
    virtual wxString Dump(int nIndent) = 0;
    virtual void Shift(lmLUnits xIncr, lmLUnits yIncr);
    virtual void Render(lmPaper* pPaper, wxColour color=*wxBLACK)=0;


protected:
    lmSimpleShape(lmEGMOType m_nType, lmObject* pOwner, wxString sName=_T("SimpleShape"),
				  bool fDraggable = false);


};


class lmCompositeShape : public lmShape
{
public:
    lmCompositeShape(lmObject* pOwner, wxString sName = _T("CompositeShape"),
                     bool fDraggable = false, lmEGMOType nType = eGMO_ShapeComposite);
    virtual ~lmCompositeShape();

    //dealing with components
    virtual int Add(lmShape* pShape);
	inline int GetNumComponents() const { return (int)m_Components.size(); }

    //virtual methods from base class
    virtual wxString Dump(int nIndent);
    virtual void Shift(lmLUnits xIncr, lmLUnits yIncr);
	virtual void Render(lmPaper* pPaper, wxColour color=*wxBLACK);

	//overrides
    bool ContainsPoint(lmUPoint& pointL);
    bool Collision(lmShape* pShape);

    //dragging
    virtual wxBitmap* OnBeginDrag(double rScale);
    virtual lmUPoint OnDrag(lmPaper* pPaper, const lmUPoint& uPos);
    virtual void OnEndDrag(wxCommandProcessor* pCP, const lmUPoint& uPos) {};



protected:
	virtual void RecomputeBounds();
	lmShape* GetShape(int nShape);

    bool					m_fGrouped;		//its component shapes must be rendered as a single object
	std::vector<lmShape*>	m_Components;	//list of its constituent shapes

};



#endif  // __LM_GMOBJECT_H__

