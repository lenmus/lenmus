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

#ifndef __LM_SHAPES_H__        //to avoid nested includes
#define __LM_SHAPES_H__

#ifdef __GNUG__
#pragma interface "Shapes.cpp"
#endif

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
#include "GMObject.h"
class lmObject;
class lmStaff;



class lmShapeLine : public lmSimpleShape
{
public:
    lmShapeLine(lmObject* pOwner,
                lmLUnits xStart, lmLUnits yStart,
                lmLUnits xEnd, lmLUnits yEnd, lmLUnits uWidth, wxColour nColor);
    ~lmShapeLine() {}

    //implementation of virtual methods from base class
    void Render(lmPaper* pPaper, lmUPoint uPos, wxColour color = *wxBLACK);
    wxString Dump();
    void Shift(lmLUnits xIncr, lmLUnits yIncr);

private:
    lmLUnits    m_xStart, m_yStart;
    lmLUnits    m_xEnd, m_yEnd;
    lmLUnits    m_uWidth;
    wxColour    m_color;

};

//represents a glyph from LenMus font
class lmShapeGlyph : public lmSimpleShape
{
public:
    lmShapeGlyph(lmObject* pOwner, int nGlyph, wxFont* pFont);
    ~lmShapeGlyph() {}

    //implementation of virtual methods from base class
    void Render(lmPaper* pPaper, lmUPoint uPos, wxColour color = *wxBLACK);
    wxString Dump();
    void Shift(lmLUnits xIncr, lmLUnits yIncr);

    //specific methods
    void Measure(lmPaper* pPaper, lmStaff* pStaff, lmUPoint shift);
    void SetFont(wxFont *pFont);


private:
    int         m_nGlyph;
    wxFont*     m_pFont;
    lmUPoint    m_uShift;   // to correctly position the glyph (relative to shape offset point)

};


//represents a text with the same font
class lmShapeText : public lmSimpleShape
{
public:
    lmShapeText(lmObject* pOwner, wxString sText, wxFont* pFont);
    ~lmShapeText() {}

    //implementation of virtual methods from base class
    void Render(lmPaper* pPaper, lmUPoint uPos, wxColour color = *wxBLACK);
    wxString Dump();
    void Shift(lmLUnits xIncr, lmLUnits yIncr);

    //specific methods
    void Measure(lmPaper* pPaper, lmStaff* pStaff, lmUPoint shift);
    void SetFont(wxFont *pFont);
    wxString* GetText() { return &m_sText; }


private:
    wxString    m_sText;
    wxFont*     m_pFont;
    lmUPoint    m_uShift;        // to correctly position the text (relative to shape offset point)

};



#endif    // __LM_SHAPES_H__

