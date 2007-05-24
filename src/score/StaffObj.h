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

#ifdef __GNUG__
#pragma interface
#endif

#ifndef __STAFFOBJ_H__        //to avoid nested includes
#define __STAFFOBJ_H__

#include "../app/TheApp.h"
#include "../graphic/Shape.h"

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
// class lmObject
//  This is the most abstract object. An object has an associated  context options obj.
//  and a parent
//-------------------------------------------------------------------------------------------

class lmObjOptions;
class GraphicObjsList;

class lmObject
{
public:
    virtual ~lmObject();

    // Options: access and set value
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


protected:
    lmObject(lmObject* pParent);

    lmObject*       m_pParent;          //the parent for the ObjOptions chain

    // options
    lmObjOptions*   m_pObjOptions;      //the options object associated to this object or
                                        //   NULL if none

};


//-------------------------------------------------------------------------------------------
// class lmScoreObj
//-------------------------------------------------------------------------------------------

enum EScoreObjType
{
    eSCOT_StaffObj = 1,         // staff objects (lmStaffObj). Main objects. Consume time
    eSCOT_AuxObj,               // aux objects (lmAuxObj). Auxiliary. Owned by staffObjs
    eSCOT_GraphicObj,           // graphic objects (lmGraphicObj). Has subtype
};


class lmScoreObj : public lmObject
{
public:
    virtual ~lmScoreObj();

    // methods related to type and identificaction
    int GetID() { return m_nId; }
    EScoreObjType GetType() { return m_nType; }

    // capabilities
    inline bool IsDraggable() { return m_fIsDraggable; }

    // methods related to positioning
    inline lmUPoint& GetOrigin() { return m_paperPos; }
    bool IsAtPoint(lmUPoint& pt);
    virtual void SetLeft(lmLUnits nLeft) { m_paperPos.x = nLeft; }
    bool IsFixed() const { return m_fFixedPos; }
    void SetFixed(bool fFixed) { m_fFixedPos = fFixed; }
    void SetPageNumber(int nNum) { m_nNumPage = nNum; }
    int GetPageNumber() { return m_nNumPage; }

    // methods related to selection
    inline bool IsSelected() { return m_fSelected; }
    void SetSelected(bool fValue) { m_fSelected = fValue; }
    void SetSelRectangle(int x, int y, int nWidth, int nHeight) {
                m_selRect.width = nWidth;
                m_selRect.height = nHeight;
                m_selRect.x = x;
                m_selRect.y = y;
        }
    void DrawSelRectangle(lmPaper* pPaper, wxColour colorC = *wxRED);
    wxRect GetSelRect() const { return wxRect((int)(m_selRect.x + m_paperPos.x),
                                              (int)(m_selRect.y + m_paperPos.y),
                                              m_selRect.width,
                                              m_selRect.height); }

    // drawing related methods
    virtual void Draw(bool fMeasuring, lmPaper* pPaper,
                      wxColour colorC = *wxBLACK,
                      bool fHighlight = false)=0;

    // methods for draggable objects
    virtual wxBitmap* GetBitmap(double rScale) = 0;
    virtual void MoveDragImage(lmPaper* pPaper, wxDragImage* pDragImage, lmDPoint& offsetD,
                         const lmUPoint& pagePosL, const lmUPoint& dragStartPosL,
                         const lmDPoint& canvasPosD);
    virtual lmUPoint EndDrag(const lmUPoint& pos);
    virtual void MoveTo(lmUPoint& pt);

    // methods related to font rendered objects
    virtual void SetFont(lmPaper* pPaper) {}
    wxFont* GetFont() { return m_pFont; }
    lmUPoint GetGlyphPosition() const {
            return lmUPoint(m_paperPos.x + m_glyphPos.x, m_paperPos.y + m_glyphPos.y);
        }

    //transitional methods to shapes renderization
    void SetShapeRendered(bool fValue) { m_fShapeRendered = fValue; }
    bool IsShapeRendered() { return m_fShapeRendered; }
    void SetShape(lmShapeObj* pShape) { m_pShape = pShape; }
    lmShapeObj* GetShape() { return m_pShape; }

    // debug methods
    virtual wxString Dump()=0;

protected:
    lmScoreObj(lmObject* pParent, EScoreObjType nType, bool fIsDraggable = false);
    virtual void DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC,
                            bool fHighlight)=0;

    // virtual methods related to draggable objects
    wxBitmap* PrepareBitMap(double rScale, const wxString sGlyph);

    // Graphic objects can be attached to StaffObjs and AuxObj. The methods are
    // defined here, as they are common to both.
    void DoAddGraphicObj(lmScoreObj* pGO);
    void DoRemoveGraphicObj(lmScoreObj* pGO);


    // type and identification
    EScoreObjType   m_nType;        //Type of lmScoreObj
    int             m_nId;          //unique number, to identify each lmScoreObj

    //positioning. Coordinates relative to origin of page (in logical units); updated each
    // time this object is drawn
    lmUPoint    m_paperPos;         // paper xPos, yBase position to render this object
    bool        m_fFixedPos;        // its position is fixed. Do not recalculate it
    wxCoord     m_nWidth;           // total width of the image, including after space
    int         m_nNumPage;         // page on which this SO is rendered (1..n). Set Up in BoxSystem::RenderMeasure().

    // selection related variables
    bool        m_fSelected;        // this obj is selected
    wxRect      m_selRect;          // selection rectangle (logical units, relative to paperPos)

    // Info for draggable objects
    bool        m_fIsDraggable;

    // variables related to font rendered objects
    wxFont*     m_pFont;            // font to use for drawing this object
    lmUPoint    m_glyphPos;         // origing to position the glyph (relative to m_paperPos)

    //transitional variables: renderization based on shapes
    bool            m_fShapeRendered;
    lmShapeObj*     m_pShape;

    // grapich objects attached to this one
    GraphicObjsList*    m_pGraphObjs;   //the collection of GraphicObjs. NULL if none

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
    eSFOT_WordsDirection,       // texts (lmWordsDirection)

    eSFOT_TupletBracket,        // tuplet bracket (lmTupletBracket)
};


class lmVStaff;
class lmGraphicObj;

class lmStaffObj : public lmScoreObj
{
public:
    virtual ~lmStaffObj();

    // characteristics
    virtual inline bool IsSizeable() { return false; }
    virtual inline bool IsFontRederized() { return false; }
    inline bool IsVisible() { return m_fVisible; }
    EStaffObjType GetClass() { return m_nClass; }

    // source code related methods
    virtual wxString SourceLDP() = 0;
    virtual wxString SourceXML() = 0;

    // methods related to time and duration
    float GetTimePos() { return m_rTimePos; }
    void SetTimePos(float rTimePos) { m_rTimePos = rTimePos; }
    virtual float GetTimePosIncrement() { return 0; }

    // methods related to positioning
    virtual lmLUnits GetAnchorPos() {return 0; }

    // implementation of pure virtual methods of base class
    virtual void Draw(bool fMeasuring, lmPaper* pPaper,
                      wxColour colorC = *wxBLACK, bool fHighlight = false);
    virtual void SetFont(lmPaper* pPaper);

    // methods related to staff ownership
    void SetNumMeasure(int nNum) { m_numMeasure = nNum; }
    int GetStaffNum() { return m_nStaffNum; }
    lmVStaff* GetVStaff() { return m_pVStaff; }

    // methods related to AuxObj/GraphObj ownership
    virtual lmScoreObj* FindSelectableObject(lmUPoint& pt) { return (lmScoreObj*)NULL; }
    virtual bool IsComposite() { return false; }

    // Graphic objects can be attached to any StaffObj
    void AddGraphicObj(lmGraphicObj* pGO);
    void RemoveGraphicObj(lmGraphicObj* pGO);

    //helper methods
    lmLUnits TenthsToLogical(lmTenths nTenths);


protected:
    lmStaffObj(lmObject* pParent, EStaffObjType nType,
             lmVStaff* pStaff = (lmVStaff*)NULL, int nStaff=1,    // only for staff owned objects
             bool fVisible = true,
             bool fIsDraggable = false);

    //properties
    bool            m_fVisible;     // this lmScoreObj is visible on the score
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

enum EAuxObjType
{
    eAXOT_Symbol = 1,           // notations (lmNoteRestObj)
    eAXOT_Tie,                  // tie (lmTie)
};

class lmAuxObj : public lmScoreObj
{
public:
    virtual ~lmAuxObj() {}

    // implementation of virtual methods of base class lmScoreObj
    void Draw(bool fMeasuring, lmPaper* pPaper, wxColour colorC = *wxBLACK,
              bool fHighlight = false);
    virtual void SetFont(lmPaper* pPaper) {}

    // Graphic objects can be attached to any AuxObj
    void AddGraphicObj(lmGraphicObj* pGO);
    void RemoveGraphicObj(lmGraphicObj* pGO);


protected:
    lmAuxObj(lmObject* pParent, EAuxObjType nType, bool fIsDraggable = false);

    EAuxObjType     m_nClass;

};

// declare a list of AuxObjs
#include "wx/list.h"
WX_DECLARE_LIST(lmAuxObj, AuxObjsList);



////-------------------------------------------------------------------------------------------
//// lmContainer
////
////-------------------------------------------------------------------------------------------
//class lmContainer : public lmObject
//{
//public:
//    virtual ~lmContainer();
//
//    virtual lmScoreObj* FindSelectableObject(lmUPoint& pt)=0;
//
//
//protected:
//    lmContainer();
//
//
//
//};


#endif    // __STAFFOBJ_H__
