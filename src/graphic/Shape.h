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
/*! @file Shape.h
    @brief Header file for class lmShape
    @ingroup graphic_management
*/
#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __SHAPE_H__        //to avoid nested includes
#define __SHAPE_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "../score/defs.h"      // lmLUnits
#include "../app/Paper.h"
class lmScoreObj;
class lmStaffObj;
class lmStaff;



//
//  An lmShape is an abstract object representing any renderizable object, such as a line,
//  a glyph, an arch, etc.
//  It has positioning information. 
//  lmShapes know (1) how to draw themselves, (2) what space they occupy, and (3) their 
//  structure (children and parent)
//
class lmShapeObj
{
public:
    virtual ~lmShapeObj() {}

    virtual void Render(lmPaper* pPaper, lmUPoint pos, wxColour color = *wxBLACK) {};

    // methods related to selection rectangle
    void SetSelRectangle(int x, int y, int nWidth, int nHeight);
    void DrawSelRectangle(lmPaper* pPaper, lmUPoint pos, wxColour colorC = *wxBLUE);
    wxRect GetSelRectangle() const { return m_SelRect; }

    // methods related to bounds
    wxRect GetBoundsRectangle() const { return m_BoundsRect; }
    bool Collision(lmShapeObj* pShape);
    virtual lmLUnits GetWidth() { return m_BoundsRect.width; }

    //methods related to position
    virtual void Shift(lmLUnits xIncr) = 0;

    //Debug related methods
    virtual wxString Dump() = 0;


protected:
    lmShapeObj(lmScoreObj* pOwner);

    lmScoreObj*     m_pOwner;       //musical object owning this shape

    wxRect          m_BoundsRect;   // boundling rectangle (logical units, relative to renderization point)
    wxRect          m_SelRect;      // selection rectangle (logical units, relative to renderization point)

};

// declare a list of ShapeObjs
#include "wx/list.h"
WX_DECLARE_LIST(lmShapeObj, ShapesList);



class lmShapeSimple : public lmShapeObj
{
public:
    virtual ~lmShapeSimple() {}

    //implementation of virtual methods from base class
    virtual wxString Dump() = 0;
    virtual void Shift(lmLUnits xIncr) = 0;


protected:
    lmShapeSimple(lmScoreObj* pOwner);


};


class lmShapeComposite : public lmShapeObj
{
public:
    lmShapeComposite(lmScoreObj* pOwner);
    virtual ~lmShapeComposite();

    //implementation of virtual methods from base class
    void Render(lmPaper* pPaper, lmUPoint pos, wxColour color = *wxBLACK);
    virtual wxString Dump() { return _T("ShapeComposite"); }
    virtual void Shift(lmLUnits xIncr);

    //dealing with components
    virtual void Add(lmShapeObj* pShape);

protected:

    bool            m_fGrouped;         //its component shapes must be rendered as a single object
    ShapesList      m_Components;       //list of its component shapes

};


class lmShapeLine : public lmShapeSimple
{
public:
    lmShapeLine(lmScoreObj* pOwner, lmLUnits nLength, lmLUnits nWidth);
    ~lmShapeLine() {}

    //implementation of virtual methods from base class
    void Render(lmPaper* pPaper, lmUPoint pos, wxColour color = *wxBLACK);
    wxString Dump();
    void Shift(lmLUnits xIncr);

private:
    lmLUnits    m_nLength;
    lmLUnits    m_nWidth;

};

//represents a glyph from LenMus font
class lmShapeGlyph : public lmShapeSimple
{
public:
    lmShapeGlyph(lmScoreObj* pOwner, int nGlyph, wxFont* pFont);
    ~lmShapeGlyph() {}

    //implementation of virtual methods from base class
    void Render(lmPaper* pPaper, lmUPoint pos, wxColour color = *wxBLACK);
    wxString Dump();
    void Shift(lmLUnits xIncr);

    //specific methods
    void Measure(lmPaper* pPaper, lmStaff* pStaff, lmUPoint shift);
    void SetFont(wxFont *pFont);


private:
    int         m_nGlyph;
    wxFont*     m_pFont;
    lmUPoint     m_shift;         // to correctly position the glyph (relative to shape offset point)

};


#endif    // __SHAPE_H__

