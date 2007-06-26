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

#ifndef __GRAPHOBJ_H__        //to avoid nested includes
#define __GRAPHOBJ_H__

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "GraphicObj.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "defs.h"
#include "StaffObj.h"


// - An lmGraphicObj is an abstract class representing any graphical score object with
//   no meaning for the program (appart of beign a graphic), such as a line, a text,
//   an arch, etc. So, from the program point of view they are just 'decoration': texts 
//   (different from lyrics or other meaningful texts), lines, arrows. boxes, 
//   markup and visual emphasis, some <notations> (i.e. articulations), etc.
// - They can be owned only by StaffObjs and AuxObjs.
// - GraphicObjs are compound objs, that is, they can be formed by aggregation of 
//   simpler GraphicObjs.
// - No measuring phase needed as they are always parent anchored (relative dx,dy 
//   to parent m_uPaperPos) and do not 'consume' staff space/time.



enum lmEGraphicObjectType
{
    eGOT_Line = 1,            // line (lmGOLine)
};


class lmGraphicObj : public lmScoreObj
{
public:
    virtual ~lmGraphicObj() {}

    // implementation of virtual methods of base class lmScoreObj
    void Draw(bool fMeasuring, lmPaper* pPaper, wxColour colorC = *wxBLACK,
              bool fHighlight = false);
    virtual void SetFont(lmPaper* pPaper) {}
    virtual wxBitmap* GetBitmap(double rScale) = 0;
    virtual void DrawObject(bool fMeasuring, lmPaper* pPaper, wxColour colorC,
                            bool fHighlight);

    // properties
    virtual lmEGraphicObjectType GetGraphicObjectType() = 0;

    // debug methods
    virtual wxString Dump() { return _T("GraphicObj"); }


protected:
    lmGraphicObj(lmScoreObj* pOwner, bool fIsDraggable = true);

};

// declare a list of lmGraphicObjs
#include "wx/list.h"
WX_DECLARE_LIST(lmGraphicObj, GraphicObjsList);



class lmGOLine : public lmGraphicObj
{
public:
    lmGOLine(lmScoreObj* pOwner,
             lmTenths xStart, lmTenths yStart, 
             lmTenths xEnd, lmTenths yEnd, lmTenths nWidth, wxColour nColor);
    ~lmGOLine() {}

    //implementation of virtual methods from base class
    wxBitmap* GetBitmap(double rScale);
    lmEGraphicObjectType GetGraphicObjectType() { return eGOT_Line; }
    wxString Dump();

private:

};

#endif    // __GRAPHOBJ_H__

