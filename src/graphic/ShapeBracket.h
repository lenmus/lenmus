//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2009 LenMus project
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation,
//    either version 3 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program. If not, see <http://www.gnu.org/licenses/>.
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

class lmShapeBracketBrace : public lmSimpleShape
{
public:
    lmShapeBracketBrace(lmInstrument* pInstr, lmEBracketSymbol nSymbol,
                        wxColour color, wxString sName);
	virtual ~lmShapeBracketBrace();

	//implementation of pure virtual methods in base class
    void Render(lmPaper* pPaper, wxColour color);
    wxString Dump(int nIndent);
    void Shift(lmLUnits xIncr, lmLUnits yIncr);

    //vertex source
    void RewindVertices(int nPathId = 0) { m_nCurVertex = 0; }
    virtual unsigned GetVertex(lmLUnits* pux, lmLUnits* puy) = 0;


protected:
    virtual void SetAffineTransform() = 0;

    //attributes
    lmEBracketSymbol    m_nSymbol;
    lmLUnits		    m_uxLeft, m_uyTop;
    lmLUnits            m_uxRight, m_uyBottom;

    //vertex control
    int                 m_nCurVertex;   //index to current vertex
    agg::trans_affine   m_trans;        //affine transformation to apply
    int                 m_nContour;     //contour number (0..3) for brace

};

class lmShapeBracket : public lmShapeBracketBrace
{
public:
    lmShapeBracket(lmInstrument* pInstr, lmLUnits xLeft, lmLUnits yTop,
                   lmLUnits xRight, lmLUnits yBottom, wxColour color = *wxBLACK);
	~lmShapeBracket();

    //vertex source
    unsigned GetVertex(lmLUnits* pux, lmLUnits* puy);


protected:
    void SetAffineTransform();

};


class lmShapeBrace : public lmShapeBracketBrace
{
public:
    lmShapeBrace(lmInstrument* pInstr, lmLUnits xLeft, lmLUnits yTop, lmLUnits xRight,
                 lmLUnits yBottom, lmLUnits dyHook, wxColour color = *wxBLACK);
	~lmShapeBrace();

    //vertex source
    unsigned GetVertex(lmLUnits* pux, lmLUnits* puy);


protected:
    void SetAffineTransform();

    //attributes
    double              m_rBraceBarHeight;
    lmLUnits            m_udyHook;
};

#endif    // __LM_SHAPEBRACKET_H__

