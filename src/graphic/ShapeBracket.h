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

#ifndef __LM_SHAPEBRACKET_H__        //to avoid nested includes
#define __LM_SHAPEBRACKET_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ShapeBracket.cpp"
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
#include "agg_trans_affine.h"

class lmInstrument;
class lmPaper;

class lmShapeBracket : public lmSimpleShape
{
public:
    lmShapeBracket(lmInstrument* pInstr, lmEBracketSymbol nSymbol,
                   lmLUnits xLeft, lmLUnits yTop,
                   lmLUnits xRight, lmLUnits yBottom,
				   wxColour color = *wxBLACK);
	~lmShapeBracket();

	//implementation of pure virtual methods in base class
    void Render(lmPaper* pPaper, wxColour color);
    wxString Dump(int nIndent);
    void Shift(lmLUnits xIncr, lmLUnits yIncr);

    //vertex source
    void RewindVertices(int nPathId = 0) { m_nCurVertex = 0; }
    unsigned GetVertex(lmLUnits* pux, lmLUnits* puy);


protected:
    void SetAffineTransform();

    //attributes
    lmLUnits		    m_uxLeft, m_uyTop;
    lmLUnits            m_uxRight, m_uyBottom;
    lmEBracketSymbol    m_nSymbol;
    double              m_rBraceBarHeight;
    lmLUnits            m_udyHook;

    //vertex control
    int                 m_nCurVertex;   //index to current vertex
    agg::trans_affine   m_trans;        //affine transformation to apply
    int                 m_nContour;     //contour number (0..3) for brace

};

#endif    // __LM_SHAPEBRACKET_H__

