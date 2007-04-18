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

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "GraphObj.cpp"
#endif

#ifndef __GRAPHOBJ_H__        //to avoid nested includes
#define __GRAPHOBJ_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "defs.h"      // lmLUnits
#include "../app/Paper.h"
class lmScoreObj;
class lmStaffObj;
class lmStaff;



// An lmGraphObj is an abstract object representing any renderizable object, 
// such as a line, a text, an arch, etc.
// It has positioning information and a shape
class lmGraphObj : public lmAuxObj
{
public:
    virtual ~lmGraphObj() {}

    virtual wxString Dump() { return _T("lmGraphObj"); }


protected:
    lmGraphObj(lmScoreObj* pOwner);

    lmScoreObj*     m_pOwner;       //musical object owning this shape

    wxRect          m_BoundsRect;   // boundling rectangle (logical units, relative to renderization point)
    wxRect          m_SelRect;      // selection rectangle (logical units, relative to renderization point)

};

// declare a list of ShapeObjs
#include "wx/list.h"
WX_DECLARE_LIST(lmGraphObj, ShapesList);



class lmGraphSimpleObj : public lmGraphObj
{
public:
    virtual ~lmGraphSimpleObj() {}

    //implementation of virtual methods from base class
    virtual wxString Dump() = 0;
    virtual void Shift(lmLUnits xIncr) = 0;


protected:
    lmGraphSimpleObj(lmScoreObj* pOwner);


};


class lmGraphCompositeObj : public lmGraphObj
{
public:
    lmGraphCompositeObj(lmScoreObj* pOwner);
    virtual ~lmGraphCompositeObj();

    //implementation of virtual methods from base class
    void Render(lmPaper* pPaper, lmUPoint pos, wxColour color = *wxBLACK);
    virtual wxString Dump() { return _T("GraphCompositeObj"); }
    virtual void Shift(lmLUnits xIncr);

    //dealing with components
    virtual void Add(lmGraphObj* pShape);

protected:

    bool            m_fGrouped;         //its component shapes must be rendered as a single object
    ShapesList      m_Components;       //list of its component shapes

};


class lmGraphLine : public lmGraphSimpleObj
{
public:
    lmGraphLine(lmScoreObj* pOwner, lmLUnits uLength, lmLUnits uWidth);
    ~lmGraphLine() {}

    //implementation of virtual methods from base class
    void Render(lmPaper* pPaper, lmUPoint pos, wxColour color = *wxBLACK);
    wxString Dump();
    void Shift(lmLUnits xIncr);

private:
    lmLUnits    m_uLength;
    lmLUnits    m_uWidth;

};


#endif    // __GRAPHOBJ_H__

