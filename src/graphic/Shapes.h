//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
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

#ifndef __LM_SHAPES_H__        //to avoid nested includes
#define __LM_SHAPES_H__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
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
#include "../app/Paper.h"       // enum lmELineEdges
#include "GMObject.h"

class lmScoreObj;
class lmStaff;
class lmStaffObj;
class lmPaper;



//------------------------------------------------------------------------------------

class lmShapeLine : public lmSimpleShape
{
public:
    lmShapeLine(lmScoreObj* pOwner, lmLUnits xStart, lmLUnits yStart,
                lmLUnits xEnd, lmLUnits yEnd, lmLUnits uWidth,
				lmLUnits uBoundsExtraWidth, wxColour nColor, wxString sName = _T("Line"),
				lmELineEdges nEdge = eEdgeNormal);
    ~lmShapeLine() {}

    //implementation of virtual methods from base class
    void Render(lmPaper* pPaper, wxColour color = *wxBLACK);
    wxString Dump(int nIndent);
    void Shift(lmLUnits xIncr, lmLUnits yIncr);

protected:
	void Create(lmLUnits xStart, lmLUnits yStart, lmLUnits xEnd, lmLUnits yEnd,
				lmLUnits uWidth, lmLUnits uBoundsExtraWidth, wxColour nColor,
				lmELineEdges nEdge);


    lmLUnits		m_xStart, m_yStart;
    lmLUnits		m_xEnd, m_yEnd;
    lmLUnits		m_uWidth;
	lmLUnits		m_uBoundsExtraWidth;
	lmELineEdges	m_nEdge;

};

//------------------------------------------------------------------------------------

class lmShapeRectangle : public lmSimpleShape
{
public:
    lmShapeRectangle(lmScoreObj* pOwner, lmLUnits xLeft, lmLUnits yTop,
                     lmLUnits xRight, lmLUnits yBottom, lmLUnits uWidth,
                     wxString sName = _T("Rectangle"),
				     bool fDraggable = true, bool fSelectable = true, 
                     wxColour color = *wxBLACK, bool fVisible = true);
    ~lmShapeRectangle() {}

    //implementation of virtual methods from base class
    void Render(lmPaper* pPaper, wxColour color = *wxBLACK);
    wxString Dump(int nIndent);
    void Shift(lmLUnits xIncr, lmLUnits yIncr);

    //settings
    void SetCornerRadius(lmLUnits uRadius);


protected:
    lmLUnits        m_xLeft, m_yTop;
    lmLUnits        m_xRight, m_yBottom;
    lmLUnits        m_uWidth;
    lmLUnits        m_uCornerRadius;

};

//------------------------------------------------------------------------------------

//represents a glyph from LenMus font
class lmShapeGlyph : public lmSimpleShape
{
public:
    lmShapeGlyph(lmScoreObj* pOwner, int nShapeIdx, int nGlyph, 
                 lmPaper* pPaper, lmUPoint offset, wxString sName=_T("ShapeGlyph"),
				 bool fDraggable = false, wxColour color = *wxBLACK);
    virtual ~lmShapeGlyph() {}

    //implementation of virtual methods from base class
    virtual void Render(lmPaper* pPaper, wxColour color = *wxBLACK);
    void RenderHighlighted(wxDC* pDC, wxColour colorC);

    wxString Dump(int nIndent);
    void Shift(lmLUnits xIncr, lmLUnits yIncr);
	virtual wxBitmap* OnBeginDrag(double rScale, wxDC* pDC);
    virtual lmUPoint OnDrag(lmPaper* pPaper, const lmUPoint& uPos);
	virtual void OnEndDrag(lmController* pCanvas, const lmUPoint& uPos);
	lmUPoint GetObjectOrigin();


protected:
    wxBitmap* GetBitmapFromShape(double rScale, wxColour colorF, wxColour colorB = *wxWHITE);
    virtual double GetPointSize();

    int         m_nGlyph;
    lmUPoint    m_uGlyphPos;   //glyph position

};

//------------------------------------------------------------------------------------

class lmShapeStem : public lmShapeLine
{
public:
    lmShapeStem(lmScoreObj* pOwner, lmLUnits xPos, lmLUnits yStart, lmLUnits uExtraLength,
                lmLUnits yEnd, bool fStemDown, lmLUnits uWidth, wxColour nColor);
    ~lmShapeStem() {}

	//specific methods
	void SetLength(lmLUnits uLenght, bool fModifyTop);
	inline bool StemDown() const { return m_fStemDown; }
	void Adjust(lmLUnits xPos, lmLUnits yStart, lmLUnits yEnd, bool fStemDown);
	lmLUnits GetYStartStem();
	lmLUnits GetYEndStem();
	lmLUnits GetXCenterStem();
    inline lmLUnits GetExtraLenght() { return m_uExtraLength; }
private:
	bool	    m_fStemDown;
    lmLUnits    m_uExtraLength;

};

//------------------------------------------------------------------------------------

class lmShapeClef : public lmShapeGlyph
{
public:
    lmShapeClef(lmScoreObj* pOwner, int nShapeIdx, int nGlyph, lmPaper* pPaper,
                lmUPoint offset, bool fSmallClef, wxString sName=_T("Clef"),
				bool fDraggable = false, wxColour color = *wxBLACK); 
    ~lmShapeClef() {}

	//overrides
    lmUPoint OnDrag(lmPaper* pPaper, const lmUPoint& uPos);
    void OnEndDrag(lmController* pCanvas, const lmUPoint& uPos);
    double GetPointSize();

protected:
    bool        m_fSmallClef;
};

//------------------------------------------------------------------------------------

class lmShapeInvisible : public lmSimpleShape
{
public:
    lmShapeInvisible(lmScoreObj* pOwner, int nShapeIdx, lmUPoint uPos, lmUSize uSize,
                     wxString sName=_T("Invisible")); 
    ~lmShapeInvisible() {}

	//overrides
    wxString Dump(int nIndent);
	void Render(lmPaper* pPaper, wxColour color);


};



#endif    // __LM_SHAPES_H__

