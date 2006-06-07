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
class lmScoreObj;
class lmStaffObj;



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
    ~lmShapeObj() {}

    virtual void Render(wxDC* pDC, wxPoint pos, wxColour color = *wxBLACK) {};


protected:
    lmShapeObj(lmScoreObj* pOwner);

    lmScoreObj*     m_pOwner;       //musical object owning this shape
    wxPoint         m_pos;          //position (referred to m_paperPos)

    wxRect          m_BoundsRect;   // selection rectangle (logical units, relative to paperPos)

};

// declare a list of ShapeObjs
#include "wx/list.h"
WX_DECLARE_LIST(lmShapeObj, ShapesList);



class lmShapeSimple : public lmShapeObj
{
public:
    virtual ~lmShapeSimple() {}

    //specific methods
    virtual wxRect GetBoundsRect() const;


protected:
    lmShapeSimple(lmScoreObj* pOwner);


};


class lmShapeComposite : public lmShapeObj
{
public:
    virtual ~lmShapeComposite() {};

    //dealing with components
    virtual void Add(lmShapeObj* pShape) {};
    virtual void Remove(lmShapeObj* pShape) {};

protected:
    lmShapeComposite(lmScoreObj* pOwner);

    bool            m_fGrouped;         //its component shapes must be rendered as a single object
    ShapesList      m_Components;       //list of its component shapes

};


class lmShapeLine : public lmShapeSimple
{
public:
    lmShapeLine(lmScoreObj* pOwner, lmLUnits nLength, lmLUnits nWidth);
    ~lmShapeLine() {}

    //implementation of virtual methods from base class
    void Render(wxDC* pDC, wxPoint pos, wxColour color = *wxBLACK);


private:
    lmLUnits    m_nLength;
    lmLUnits    m_nWidth;

};

//represents a glyph from LenMus font
class lmShapeGlyph : public lmShapeSimple
{
public:
    lmShapeGlyph(lmStaffObj* pOwner, int nGlyph, wxFont* pFont);
    ~lmShapeGlyph() {}

    //implementation of virtual methods from base class
    void Render(wxDC* pDC, wxPoint pos, wxColour color = *wxBLACK);

    //specific methods
    void Measure(wxDC* pDC, int nStaffNum = 1);
    void SetShift(lmLUnits x, lmLUnits y);


private:
    int         m_nGlyph;
    wxFont*     m_pFont;
    wxPoint     m_shift;         // to correctly position the glyph (relative to render point)

};


#endif    // __SHAPE_H__

