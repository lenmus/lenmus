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

#ifndef __LM_SHAPEBARLINE_H__        //to avoid nested includes
#define __LM_SHAPEBARLINE_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ShapeBarline.cpp"
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

class lmBarline;


class lmShapeBarline : public lmSimpleShape
{
public:
    lmShapeBarline(lmBarline* pBarline, lmEBarline nBarlineType, lmLUnits xPos, lmLUnits yTop,
				   lmLUnits yBottom, lmLUnits uThinLineWidth, lmLUnits uThickLineWidth,
                   lmLUnits uSpacing, lmLUnits uRadius, wxColour color);

	~lmShapeBarline();

	//implementation of pure virtual methods in base class
    void Render(lmPaper* pPaper, wxColour color);
    wxString Dump(int nIndent);
    void Shift(lmLUnits xIncr, lmLUnits yIncr);

	wxBitmap* OnBeginDrag(double rScale);


	//access to info
	inline lmLUnits GetXEnd() const { return m_uxPos + m_uWidth; }

protected:
    void DrawThinLine(lmPaper* pPaper, lmLUnits uxPos, lmLUnits uyTop, lmLUnits uyBottom,
                      wxColour color);
    void DrawThickLine(lmPaper* pPaper, lmLUnits uxLeft, lmLUnits uyTop, lmLUnits uWidth,
                       lmLUnits uHeight, wxColour color);
    void DrawTwoDots(lmPaper* pPaper, lmLUnits uxPos, lmLUnits uyPos);

    lmEBarline    m_nBarlineType;     //type of barline

    //geometry
    lmLUnits m_uThinLineWidth;
    lmLUnits m_uThickLineWidth;
    lmLUnits m_uSpacing;            // between lines and lines-dots
    lmLUnits m_uRadius;             // for dots

    //position and size
    lmLUnits    m_uxPos;
    lmLUnits    m_uyTop;
    lmLUnits    m_uyBottom;
	lmLUnits	m_uWidth;

};

#endif    // __LM_SHAPEBARLINE_H__

