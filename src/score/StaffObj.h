//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
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
/*! @file StaffObj.h    
    @brief Header file for classes lmScoreObj, lmAuxObj, lmStaffObj, lmSimpleObj and lmCompositeObj
    @ingroup score_kernel
*/
#ifdef __GNUG__
// #pragma interface
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

#define lmDRAGGABLE        true
#define lmNO_DRAGGABLE    false
#define lmVISIBLE        true
#define lmNO_VISIBLE    false

//--------------------------------------------------------------------------------------
// lmStaffObj: Abstract class to model all objects that migth appear on an staff
//--------------------------------------------------------------------------------------

/*
The full graphical object model is as follows:


            |
            |
            |pt0: m_paperPos
    --------+------------------------------------------------------------------
            |
            |
            |
            |            pt2: m_glyphPos (relative to m_paperPos)
            |            +---------------------+ <-- Rectangle that surrounds the character
            |            |                     |     or drag bitmap  
            |            |                     |   
            |            |                     |   
            |            |                     |   
            |            |pt3: m_selRect       |   
  pt1: m_alterPos        |+----------+         |   
            |pt1         ||          |         |   
            +-------+    ||          |         | 
            |       |    ||          |         | 
            |       |    ||          |         | 
            |       |    ||pt4: m_noteheadRect |       | 
            |       |    ||+--------+|         |       | 
            |       |    |||        ||         |<----->|  afterSpace
            +-------+    ||+--------+|         |       |
            |            |+----------+         |       | 
            |            ||                    |       | 
            |            +---------------------+       | 
            |             |                            | 
            |             |                            |
            |             x: m_xAnchor                 |
            |             |                            |
            |<---------------------------------------->|          
            |               m_nWidth                   |


All positions are relative to m_paperPos

@attention
  -    The anchor line is only used (for now) to align noteheads when some are precedeed by an alter
    sign and others no. Then, as it doesn't matter for StaffObjs different from Notes, the base
    class lmStaffObj just does it coincide with m_paperPos.x (see GetAnchoPos() virtual method
    declaration).
  -    Also, in StaffObjs different from Notes it does not exit the alter glyph. So, to save space
  the relate variable m_alterPos could be removed from the base class to thye Notes class.



*/



//-------------------------------------------------------------------------------------------
/* @class lmScoreObj
        This is the most abstract object. Has an ID. It is a renderizable object and,
        therefore, has positioning information. 
        They know (1) how to draw themselves, (2) what space they occupy, and (3) their 
        structure (children and parent)
*/
//-------------------------------------------------------------------------------------------
class lmScoreObj
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
    wxRect GetSelRect() const { return wxRect(m_selRect.x + m_paperPos.x, 
                                              m_selRect.y + m_paperPos.y,
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



protected:
    lmScoreObj(EScoreObjType nType, bool fIsDraggable = false);
    virtual void DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC,
                            bool fHighlight)=0;

    // virtual methods related to draggable objects
    wxBitmap* PrepareBitMap(double rScale, const wxString sGlyph);


    // type and identification
    EScoreObjType   m_nType;        //Type of lmScoreObj
    int         m_nId;          //unique number, to identify each lmScoreObj

    //positioning. Coordinates relative to origin of page (in logical units); updated each
    // time this object is drawn
    lmUPoint     m_paperPos;         // paper xPos, yBase position to render this object
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
    lmUPoint     m_glyphPos;         // origing to position the glyph (relative to m_paperPos)

    //transitional variables: renderization based on shapes
    bool            m_fShapeRendered;
    lmShapeObj*     m_pShape;

};

//-------------------------------------------------------------------------------------------
//    lmAuxObj
//
//-------------------------------------------------------------------------------------------
class lmAuxObj : public lmScoreObj
{
public:
    virtual ~lmAuxObj() {}

    // implementation of virtual methods of base class lmScoreObj
    void Draw(bool fMeasuring, lmPaper* pPaper, wxColour colorC = *wxBLACK,
              bool fHighlight = false);
    virtual void SetFont(lmPaper* pPaper) {}

    // debug methods
    virtual wxString Dump() { return _T(""); }


protected:
    lmAuxObj(EScoreObjType nType, bool fIsDraggable = false); 

};

// declare a list of AuxObjs
#include "wx/list.h"
WX_DECLARE_LIST(lmAuxObj, AuxObjsList);



//-------------------------------------------------------------------------------------------
//    lmStaffObj
//        This is the most abstract object. Has an ID. It is a renderizable object and,
//        therefore, has positioning information. 
//        Can produce source code. 
//
//        To simplify the score layout process, all StaffObjs have timing information although
//        not all ScoreObjs are time positioned.
//
//-------------------------------------------------------------------------------------------
class lmStaffObj : public lmScoreObj
{
public:
    virtual ~lmStaffObj();

    // characteristics
    virtual inline bool IsComposite()=0;
    virtual inline bool IsSizeable() { return false; }
    virtual inline bool IsFontRederized() { return false; }
    inline bool IsVisible() { return m_fVisible; }

    // source code related methods
    virtual wxString SourceLDP() = 0;
    virtual wxString SourceXML() = 0;

    // debug related methods
    virtual wxString Dump() = 0;

    // methods related to time and duration
    float GetTimePos() { return m_rTimePos; }
    void SetTimePos(float rTimePos) { m_rTimePos = rTimePos; }
    virtual float GetTimePosIncrement() { return 0; }
    //virtual int GetDurationOld();

    // methods related to positioning
    virtual lmLUnits GetAnchorPos() {return 0; }

    // implementation of pure virtual methods of base class
    virtual void Draw(bool fMeasuring, lmPaper* pPaper,
                      wxColour colorC = *wxBLACK, bool fHighlight = false);
    virtual void SetFont(lmPaper* pPaper);

    // methods related to staff ownership
    void    SetNumMeasure(int nNum) { m_numMeasure = nNum; }
    int    GetStaffNum() { return m_nStaffNum; }
    lmVStaff* GetVStaff() { return m_pVStaff; }



protected:
    lmStaffObj(EScoreObjType nType, 
             lmVStaff* pStaff = (lmVStaff*)NULL, int nStaff=1,    // only for staff owned objects    
             bool fVisible = true, 
             bool fIsDraggable = false);



        // member variables
        //----------------

    //properties
    bool        m_fVisible;         // this lmScoreObj is visible on the score

    // time related variables
    float       m_rTimePos;         // time from start of measure

    // Info about staff ownership
    lmVStaff*   m_pVStaff;          // lmVStaff to which this lmStaffObj belongs or NULL
    int     m_nStaffNum;        // lmStaff (1..n) on which this object is located
    int     m_numMeasure;       // measure number in which this lmStaffObj is included

};

// declare a list of StaffObjs class
#include "wx/list.h"
WX_DECLARE_LIST(lmStaffObj, StaffObjsList);



//-------------------------------------------------------------------------------------------
//    lmSimpleObj
//
//-------------------------------------------------------------------------------------------
class lmSimpleObj : public lmStaffObj
{
public:
    virtual ~lmSimpleObj() {}
    inline bool IsComposite() { return false; }

protected:
    lmSimpleObj(EScoreObjType nType, 
             lmVStaff* pStaff = (lmVStaff*)NULL, int nStaff=1,
             bool fVisible = true, 
             bool fIsDraggable = false);

};

//-------------------------------------------------------------------------------------------
//    lmCompositeObj
//
//-------------------------------------------------------------------------------------------
class lmCompositeObj : public lmStaffObj
{
public:
    virtual ~lmCompositeObj() {}
    inline bool IsComposite() { return true; }

    virtual lmScoreObj* FindSelectableObject(lmUPoint& pt)=0;

protected:
    lmCompositeObj(EScoreObjType nType, 
             lmVStaff* pStaff = (lmVStaff*)NULL, int nStaff=1,
             bool fVisible = true, 
             bool fIsDraggable = false);

};

///*
//-------------------------------------------------------------------------------------------
//    Font renderized object
//        Its renderization is done totally (i.e. texts) or partially (i.e. notes) by
//        using a font.
//        In opposition to objects totally renderized by direct drawing: tie, slur, barline
//-------------------------------------------------------------------------------------------
//*/
//class sbFontRenderizedObj : public lmStaffObj
//{
//public:
//    sbFontRenderizedObj();
//    ~sbFontRenderizedObj() {}
//
//    inline bool IsFontRederized() { return true; }
//
//    // methods related to font rendered objects
//    lmUPoint GetGlyphPosition() const {
//            return lmUPoint(m_paperPos.x + m_glyphPos.x, m_paperPos.y + m_glyphPos.y);
//        }
//
//protected:
//    // variables related to font rendered objects
//    wxFont*        m_pFont;        // font to use for drawing this object
//    lmUPoint        m_glyphPos;        // origing to position the glyphs (relative to m_paperPos)
//
//};
//



////-------------------------------------------------------------------------------------------
////    Draggable object:
////        is a SelectableObj that can be freely positioned on the score, only constrained
////        by music writting rules (i.e.: note, clef).
////-------------------------------------------------------------------------------------------
//class sbDraggableObj : public sbSelectableObj
//{
//public:
//    sbDraggableObj();
//    ~sbDraggableObj() {}
//    
//    inline bool IsDraggable() { return true; };
//
//    // methods related to draggable objects
//    virtual wxBitmap* GetBitmap(double rScale) = 0;
//    virtual void MoveDragImage(lmPaper* pPaper, wxDragImage* pDragImage, lmDPoint& offsetD, 
//                         const lmUPoint& pagePosL, const lmUPoint& dragStartPosL,
//                         const lmDPoint& canvasPosD);
//    virtual lmUPoint EndDrag(const lmUPoint& pos);
//    void MoveTo(lmUPoint& pt);
//
//protected:
//    wxBitmap* PrepareBitMap(double rScale, const wxString sGlyph);
//
//};
//


#endif    // __STAFFOBJ_H__
